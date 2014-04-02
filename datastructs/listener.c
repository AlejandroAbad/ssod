#include "../debug.h"
#include "listener.h"

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <com_err.h>
#include <et/com_err.h>
#include <stdio.h>

ssod_error ssod_listener_init(ssod_iface* iface)
{
	F_BEGIN();
	ssod_error error;
	char *wpa_ctrl_path;
	if (!iface) E_RETURN(SSOD_ERR_IFACE_NOT_SUPPLIED);

	error = ssod_listener_free(iface);
	if (error) E_THROW(error);

	EVENT("[%s] initializing listener", iface->name);

	wpa_ctrl_path = iface->ctx->wpa_ctrl_path;

	iface->listener = (ssod_listener*) malloc(sizeof (ssod_listener));

	iface->listener->path = (char *) malloc(strlen(iface->name) + strlen(wpa_ctrl_path) + 1);
	strcpy(iface->listener->path, wpa_ctrl_path);
	strcat(iface->listener->path, iface->name);
	EVENT("[%s] listener wpa interface socket path is (%s)", iface->name, iface->listener->path);

	errno = 0;
	iface->listener->monitor = wpa_ctrl_open(iface->listener->path);
	if (!iface->listener->monitor)
	{
		free(iface->listener->path);
		free(iface->listener);
		iface->listener = NULL;
		if (errno) E_RETURN(SSOD_ERR_EXTERNAL, "wpa_ctrl_open", error_message(errno));
		E_RETURN(SSOD_ERR_EXTERNAL, "wpa_ctrl_open", "unable to connect to the interface");
	}

	EVENT("[%s] listener ready to operate", iface->name);

	RETURN(SSOD_ERR_NONE);
}

ssod_error ssod_listener_free(ssod_iface* iface)
{
	F_BEGIN();

	if (iface && iface->listener)
	{
		if (iface->listener->path) free(iface->listener->path);
		wpa_ctrl_close(iface->listener->monitor);
		free(iface->listener);
		iface->listener = NULL;
	}

	RETURN(SSOD_ERR_NONE);
}

ssod_error ssod_listener_attach(ssod_iface* iface)
{
	F_BEGIN();
	ssod_error error;
	int retval;
	if (!iface) E_RETURN(SSOD_ERR_IFACE_NOT_SUPPLIED);
	if (!iface->listener)
	{
		error = ssod_listener_init(iface);
		if (error) E_THROW(error);
	}

	retval = wpa_ctrl_attach(iface->listener->monitor);
	if (retval)
	{
		if (errno) E_RETURN(SSOD_ERR_EXTERNAL, "wpa_ctrl_attach", error_message(errno));
		E_RETURN(SSOD_ERR_EXTERNAL, "wpa_ctrl_attach", "socket timeout");
	}

	EVENT("[%s] listener attached", iface->name);

	RETURN(SSOD_ERR_NONE);
}

ssod_error ssod_listener_detach(ssod_iface* iface)
{
	F_BEGIN();
	ssod_error error;
	int retval;
	if (!iface) E_RETURN(SSOD_ERR_IFACE_NOT_SUPPLIED);
	if (!iface->listener)
	{
		error = ssod_listener_init(iface);
		if (error) E_THROW(error);
	}

	retval = wpa_ctrl_detach(iface->listener->monitor);
	if (retval)
	{
		if (errno) E_RETURN(SSOD_ERR_EXTERNAL, "wpa_ctrl_detach", error_message(errno));
		E_RETURN(SSOD_ERR_EXTERNAL, "wpa_ctrl_detach", "socket timeout");
	}

	EVENT("[%s] listener detached", iface->name);

	RETURN(SSOD_ERR_NONE);
}

ssod_error ssod_listener_is_online(ssod_iface* iface, int *online)
{
	F_BEGIN();
	ssod_error error;
	char reply[WPA_CTRL_MAX_BUFFER_SIZE];
	int retval;
	size_t reply_len;
	if (!iface) E_RETURN(SSOD_ERR_IFACE_NOT_SUPPLIED);
	if (!online) E_RETURN(SSOD_ERR_NO_OUTPUT_POINTER);

	(*online) = FALSE;

	if (iface->listener && iface->listener->monitor)
	{
		reply_len = WPA_CTRL_MAX_BUFFER_SIZE;
		memset(reply, 0, reply_len);

		retval = wpa_ctrl_request(iface->listener->monitor, "PING", 4, reply, &reply_len, NULL);
		if (retval)
		{
			if (retval == -2 || errno == EAGAIN) WARN("wpa_ctrl_request: timeout");
			if (errno) WARN("wpa_ctrl_request: %s", error_message(errno));
		}
		else
		{
			DEBUG("[%s] received PONG", iface->name);
			if (strstr(reply, "PONG"))
				(*online) = TRUE;
		}
	}

	DEBUG("[%s] listener online=%d", iface->name, *online);
	RETURN(SSOD_ERR_NONE);
}

ssod_error ssod_listener_listen(ssod_iface* iface, char **output, int *output_len)
{
	F_BEGIN();
	ssod_error error;
	char reply[WPA_CTRL_MAX_BUFFER_SIZE];
	int retval;
	size_t reply_len;

	if (!iface) E_RETURN(SSOD_ERR_IFACE_NOT_SUPPLIED);
	if (!output) E_RETURN(SSOD_ERR_NO_OUTPUT_POINTER);
	if (!output_len) E_RETURN(SSOD_ERR_NO_OUTPUT_POINTER);

	reply_len = WPA_CTRL_MAX_BUFFER_SIZE;
	memset(reply, 0, reply_len);
	retval = wpa_ctrl_recv(iface->listener->monitor, reply, &reply_len);
	if (retval || reply_len == 0)
	{
		if (retval == -2 || errno == EAGAIN)
		{
			DEBUG("[%s] no event received, got timeout.", iface->name);
			*output_len = 0;
			*output = NULL;
			RETURN(SSOD_ERR_NONE);
		}
		if (errno)
		{
			WARN("[%s] wpa_ctrl_request: %s", iface->name, error_message(errno));
			RETURN(SSOD_ERR_LISTEN_TIMEOUT);
		}
	}

	DEBUG("[%s] received (%u) bytes -> %s", iface->name, reply_len, reply);

	*output_len = reply_len;
	(*output) = (char*) malloc(reply_len + 1);
	memcpy((*output), reply, reply_len);
	(*output)[reply_len] = '\0';

	RETURN(SSOD_ERR_NONE);
}

ssod_error ssod_listener_update_msk(ssod_iface* iface, int *changed)
{
	F_BEGIN();
	ssod_error error;
	char reply[WPA_CTRL_MAX_BUFFER_SIZE];
	unsigned char *msk;
	int retval, msk_len, i;
	size_t reply_len;

	if (!iface) E_RETURN(SSOD_ERR_IFACE_NOT_SUPPLIED);

	reply_len = WPA_CTRL_MAX_BUFFER_SIZE;
	memset(reply, 0, reply_len);
	retval = wpa_ctrl_request(iface->listener->monitor, "MSK", 3, reply, &reply_len, NULL);
	if (retval || reply_len < 16)
	{
		if (retval == -2)
			E_RETURN(SSOD_ERR_EXTERNAL, "wpa_ctrl_request", "timeout");
		if (reply_len)
			E_RETURN(SSOD_ERR_EXTERNAL, "wpa_ctrl_request", "no MSK available on this interface");
		E_RETURN(SSOD_ERR_EXTERNAL, "wpa_ctrl_request", error_message(errno));
	}

	if (changed)
		*changed = FALSE;

	DEBUG("[%s] MSK command -> received (%u) bytes -> %s", iface->name, reply_len, reply);

	msk_len = reply_len / 2;
	msk = (char *) malloc(msk_len);

	for (i = 0; i < msk_len; i++)
	{
		sscanf(reply + (i * 2), "%2hhx", &msk[i]);
	}

	free(msk);


	if (iface->msk_len != msk_len || memcmp(iface->msk, msk, msk_len))
	{
		EVENT("[%s] new MSK found", iface->name);
		if (changed)
			*changed = TRUE;

		iface->msk = (char*) realloc(iface->msk, msk_len);
		memcpy(iface->msk, msk, msk_len);
		iface->msk_len = msk_len;
		fnasso_user_set_with_msk(iface->sso, iface->msk, iface->msk_len);
	}

	if (changed) DEBUG("changed set to (%d)", *changed);
	RETURN(SSOD_ERR_NONE);
}
