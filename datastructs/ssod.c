#include "../debug.h"
#include "ssod.h"
#include "realm.h"
#include "iface.h"
#include "thread.h"
#include "listener.h"

#include <string.h>
#include <stdlib.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <com_err.h>
#include <errno.h>
#include <sys/time.h>
#include <pwd.h>

#define TRUE 1
#define FALSE 0

ssod_error ssod_init(ssod **context)
{
	F_BEGIN();
	ssod_error error;
	int fcp_len, retval;
	ssod *ctx;
	EVENT("initializing SSOD context");
	if (!context) E_RETURN(SSOD_ERR_NO_SSOD_SUPPLIED);



	ctx = (*context) = (ssod *) calloc(1, sizeof (ssod));

	retval = pthread_cond_init(&ctx->thread_cond, NULL);
	if (retval) E_RETURN(SSOD_ERR_EXTERNAL, "pthread_cond_init", error_message(errno));
	retval = pthread_mutex_init(&ctx->thread_mutex, NULL);
	if (retval) E_RETURN(SSOD_ERR_EXTERNAL, "pthread_mutex_init", error_message(errno));

	retval = pthread_mutex_lock(&ctx->thread_mutex);
	if (retval)
	{
		free(ctx);
		E_RETURN(SSOD_ERR_EXTERNAL, "pthread_mutex_lock", error_message(errno));
	}

	ssod_set_wpa_ctrl_path(ctx, NULL);
	ssod_set_ccache_path(ctx, NULL);

	RETURN(SSOD_ERR_NONE);
}

ssod_error ssod_free(ssod **context)
{
	F_BEGIN();
	ssod *ctx;

	if (context)
	{
		ctx = *context;

		ssod_realm_free(ctx);
		ssod_iface_free(ctx);

		if (ctx->fnasso_ccache_path) free(ctx->fnasso_ccache_path);
		if (ctx->wpa_ctrl_path) free(ctx->wpa_ctrl_path);

		pthread_mutex_unlock(&ctx->thread_mutex);
		pthread_cond_destroy(&ctx->thread_cond);
		pthread_mutex_destroy(&ctx->thread_mutex);

		free(ctx);
		context = NULL;
	}

	ctx->uid = -1;

	RETURN(SSOD_ERR_NONE);
}

ssod_error ssod_set_user(ssod *ctx, const char *user)
{
	F_BEGIN();
	struct passwd *pw;
	int retval;
	if (!ctx) E_RETURN(SSOD_ERR_NO_SSOD_SUPPLIED);

	if (!user || !(strlen(user)))
	{
		ctx->uid = 0;

		retval = setuid(ctx->uid);
		if (retval < 0)
		{
			E_RETURN(SSOD_ERR_CANT_CHANGE_USER, ctx->uid, "root", error_message(errno));
		}
		DEBUG("user has been set to 0 (root)");
	}
	else
	{
		pw = getpwnam(user);
		if (!pw)
		{
			E_RETURN(SSOD_ERR_USER_DONT_EXISTS, user);
		}
		ctx->uid = pw->pw_uid;

		retval = setuid(ctx->uid);
		if (retval < 0)
		{
			E_RETURN(SSOD_ERR_CANT_CHANGE_USER, ctx->uid, user, error_message(errno));
		}
		DEBUG("user has been set to %d (%s)", ctx->uid, user);
	}
	RETURN(SSOD_ERR_NONE);
}

ssod_error ssod_set_wpa_ctrl_path(ssod *ctx, const char *wpa_ctrl_path)
{
	F_BEGIN();
	int wcp_len;
	if (!ctx) E_RETURN(SSOD_ERR_NO_SSOD_SUPPLIED);

	// set a good wpa_ctrl base path
	if (!wpa_ctrl_path || !(wcp_len = strlen(wpa_ctrl_path)))
	{
		ctx->wpa_ctrl_path = (char *) malloc(sizeof ("/var/run/wpa_supplicant/"));
		strcpy(ctx->wpa_ctrl_path, "/var/run/wpa_supplicant/");
		DEBUG("No wpa_ctrl_path supplied, using (%s)", ctx->wpa_ctrl_path);
	}
	else
	{
		if (wpa_ctrl_path[wcp_len - 1] == '/')
		{
			ctx->wpa_ctrl_path = (char *) malloc(wcp_len + 1);
			strcpy(ctx->wpa_ctrl_path, wpa_ctrl_path);
		}
		else
		{
			ctx->wpa_ctrl_path = (char *) malloc(wcp_len + 2);
			memcpy(ctx->wpa_ctrl_path, wpa_ctrl_path, wcp_len);
			ctx->wpa_ctrl_path[wcp_len - 1] = '/';
			ctx->wpa_ctrl_path[wcp_len] = '\0';
		}
		DEBUG("wpa_ctrl_path supplied (%s), using (%s)", wpa_ctrl_path, ctx->wpa_ctrl_path);
	}

	RETURN(SSOD_ERR_NONE);
}

ssod_error ssod_set_ccache_path(ssod *ctx, const char *fnasso_ccache_path)
{
	F_BEGIN();
	int fcp_len;
	if (!ctx) E_RETURN(SSOD_ERR_NO_SSOD_SUPPLIED);

	// set a good cred cache base path
	if (!fnasso_ccache_path || !(fcp_len = strlen(fnasso_ccache_path)))
	{
		ctx->fnasso_ccache_path = NULL;
		DEBUG("no fnasso_cache_path supplied")
	}
	else
	{
		ctx->fnasso_ccache_path = (char *) malloc(fcp_len + 6);
		strcpy(ctx->fnasso_ccache_path, "FILE:");
		strcpy(ctx->fnasso_ccache_path + 5, fnasso_ccache_path);
		DEBUG("fnasso_ccache_path supplied (%s), using (%s)", fnasso_ccache_path, ctx->fnasso_ccache_path);
	}
	RETURN(SSOD_ERR_NONE);
}

ssod_error ssod_update_networks(ssod *ctx)
{
	F_BEGIN();
	ssod_error error;
	ssod_iface *iface;
	int retval, sockaddr_len;
	char host[NI_MAXHOST], *realm;
	struct sockaddr_in sa;
	if (!ctx) E_RETURN(SSOD_ERR_NO_SSOD_SUPPLIED);

	EVENT("searching available network interfaces");

	struct ifaddrs *interfaces, *ifaces_head;
	if (getifaddrs(&interfaces) != -1)
	{
		ifaces_head = interfaces;
		while (interfaces)
		{
			if (strcmp(interfaces->ifa_name, "lo"))
			{
				if (interfaces->ifa_addr->sa_family == AF_INET || interfaces->ifa_addr->sa_family == AF_INET6)
				{
					sockaddr_len = (interfaces->ifa_addr->sa_family == AF_INET) ? sizeof (struct sockaddr_in) : sizeof (struct sockaddr_in6);
					retval = getnameinfo(interfaces->ifa_addr, sockaddr_len, host, NI_MAXHOST, NULL, 0, 0);

					EVENT("[%s] found with address (%s)", interfaces->ifa_name, host);


					retval = inet_pton(AF_INET, host, &(sa.sin_addr));
					if (retval)
					{
						EVENT("[%s] (%s) is an IP, not a hostname, not splitting possible realms", interfaces->ifa_name, host);
					}
					else
					{
						EVENT("[%s] splitting possible realms from (%s)", interfaces->ifa_name, host);
						realm = host;
						while (realm = strchr(realm, '.'))
						{
							realm++;
							ssod_realm_add(ctx, realm);
						}
					}

					error = ssod_iface_add(ctx, interfaces->ifa_name);
					if (error)
					{
						WARN("[%s] unable to add interface", interfaces->ifa_name);
					}
					else
					{
						iface = NULL;
						error = ssod_iface_search(ctx, interfaces->ifa_name, &iface);
						if (!iface)
						{
							ERROR("[%s] Huh?!", interfaces->ifa_name);
						}
						else
						{
							error = ssod_listener_attach(iface);
							error &= ssod_thread_start(iface);
							if (error)
							{
								WARN("[%s] unable to start thread", interfaces->ifa_name);
								error = ssod_iface_del(ctx, interfaces->ifa_name);
								if (error)
								{
									WARN("[%s] unable to delete interface", interfaces->ifa_name);
								}
							}
						}
					}
				}
			}
			interfaces = interfaces->ifa_next;
		}

		if (ifaces_head)
			freeifaddrs(ifaces_head);
	}

	if (!ctx->ifaces)
		WARN("no interfaces are available");

	RETURN(SSOD_ERR_NONE);
}

ssod_error ssod_join_thread(ssod *ctx)
{
	F_BEGIN();
	struct timespec wake_up_time;
	ssod_error error;
	int timeout;
	if (!ctx) E_RETURN(SSOD_ERR_NO_SSOD_SUPPLIED);

	if (ctx->ifaces == NULL) timeout = SSOD_TIME_SEARCH_IFACE_CHANGES_2;
	else timeout = SSOD_TIME_SEARCH_IFACE_CHANGES_1;


	if (timeout > 0)
	{
		wake_up_time.tv_sec = time(NULL) + timeout;
		wake_up_time.tv_nsec = 0;

		pthread_cond_timedwait(&ctx->thread_cond, &ctx->thread_mutex, &wake_up_time);
		if (errno) WARN("pthread_cond_timedwait: %s", error_message(errno));
	}
	else
	{
		pthread_cond_wait(&ctx->thread_cond, &ctx->thread_mutex);
		if (errno) WARN("pthread_cond_wait: %s", error_message(errno));
	}

	if (ctx->thread_dead_iface)
	{
		error = ssod_iface_del(ctx, ctx->thread_dead_iface->name);
		if (error) WARN("[%s] cannot delete interface", ctx->thread_dead_iface->name)
			ctx->thread_dead_iface = NULL;
	}

	RETURN(SSOD_ERR_NONE);
}









