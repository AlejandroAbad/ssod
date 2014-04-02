#include "iface.h"

#include <string.h>
#include <stdlib.h>

ssod_error ssod_iface_free(ssod *ctx)
{
	F_BEGIN();
	ssod_iface *ptr;

	if (ctx)
	{
		while (ptr = ctx->ifaces)
		{
			ssod_iface_del(ctx, ctx->ifaces->name);
		}
		ctx->ifaces = NULL;
	}

	RETURN(SSOD_ERR_NONE);
}

ssod_error ssod_iface_add(ssod *ctx, const char *iface_name)
{
	F_BEGIN();
	ssod_error error;
	fnasso_error fnasso_error;
	ssod_iface *ptr, *last, *new;

	if (!ctx) E_RETURN(SSOD_ERR_NO_SSOD_SUPPLIED);
	if (!iface_name || !strlen(iface_name)) E_RETURN(SSOD_ERR_NO_IFACE_NAME_SUPPLIED);

	last = ptr = ctx->ifaces;

	DEBUG("adding interface (%s)", iface_name);

	// if interface already exists, exit
	while (ptr)
	{
		if (strcmp(ptr->name, iface_name) == 0)
		{
			EVENT("iface (%s) already exists, won't do anything", iface_name);
			RETURN(SSOD_ERR_NONE);
		}
		last = ptr;
		ptr = ptr->next;
	}

	// Allocate the new iface
	new = (ssod_iface*) calloc(1, sizeof (ssod_iface));
	if (last)
		last->next = new;
	else
		ctx->ifaces = new;

	// Initialize the new iface
	new->ctx = ctx;
	new->next = NULL;
	new->msk = NULL;
	new->msk_len = 0;
	new->tgt_renewal_time = time(NULL);
	new->name = (char*) malloc(strlen(iface_name) + 1);
	strcpy(new->name, iface_name);

	error = ssod_listener_init(new);
	if (error) goto error_exit;

	error = ssod_thread_init(new);
	if (error) goto error_exit;

	fnasso_error = fnasso_context_init(&new->sso);
	if (fnasso_error)
	{
		ssod_iface_del(ctx, iface_name);
		E_RETURN(FNASSO_ERR_EXTERNAL, "fnasso_context_init", "");
	}
	
	if (ctx->fnasso_ccache_path)
	{
		fnasso_error = fnasso_context_set_cache_name(new->sso, ctx->fnasso_ccache_path);
	}

	EVENT("added interface (%s)", new->name);
	RETURN(SSOD_ERR_NONE);

error_exit:
	ssod_iface_del(ctx, iface_name);
	E_THROW(error);
}

ssod_error ssod_iface_del(ssod *ctx, const char *iface_name)
{
	F_BEGIN();
	ssod_iface *ptr, *last;

	if (!ctx) E_RETURN(SSOD_ERR_NO_SSOD_SUPPLIED);
	if (!iface_name || !strlen(iface_name)) E_RETURN(SSOD_ERR_NO_IFACE_NAME_SUPPLIED);

	ptr = ctx->ifaces;
	last = NULL;

	// search the interface
	while (ptr)
	{
		if (strcmp(ptr->name, iface_name) == 0)
		{
			EVENT("deleting iface (%s)", iface_name);

			if (last) last->next = ptr->next;
			else ctx->ifaces = ptr->next;

			ssod_listener_free(ptr);
			ssod_thread_free(ptr);
			fnasso_context_free(&ptr->sso);
			if (ptr->msk) free(ptr->msk);
			free(ptr->name);
			free(ptr);
			//if (ptr == ctx->ifaces) ctx->ifaces = NULL;
			RETURN(SSOD_ERR_NONE);
		}
		last = ptr;
		ptr = ptr->next;
	}

	EVENT("iface (%s) not found, nothing to delete", iface_name);
	RETURN(SSOD_ERR_NONE);
}

ssod_error ssod_iface_search(ssod *ctx, const char *iface_name, ssod_iface **pointer)
{
	F_BEGIN();
	ssod_iface *ptr, *last;

	if (!ctx) E_RETURN(SSOD_ERR_NO_SSOD_SUPPLIED);
	if (!iface_name || !strlen(iface_name)) E_RETURN(SSOD_ERR_NO_IFACE_NAME_SUPPLIED);
	if (!pointer) E_RETURN(SSOD_ERR_NO_OUTPUT_POINTER);

	last = ptr = ctx->ifaces;

	// search the interface
	while (ptr)
	{
		if (strcmp(ptr->name, iface_name) == 0)
		{
			*pointer = ptr;
			RETURN(SSOD_ERR_NONE);
		}
		last = ptr;
		ptr = ptr->next;
	}

	*pointer = NULL;
	RETURN(SSOD_ERR_NONE);
}

ssod_error ssod_iface_head(ssod *ctx, ssod_iface **pointer)
{
	F_BEGIN();
	if (!ctx) E_RETURN(SSOD_ERR_NO_SSOD_SUPPLIED);
	if (!pointer) E_RETURN(SSOD_ERR_NO_SSOD_SUPPLIED);

	(*pointer) = ctx->ifaces;

	RETURN(SSOD_ERR_NONE);
}

ssod_error ssod_iface_next(ssod *ctx, ssod_iface **pointer)
{
	F_BEGIN();
	if (!ctx) E_RETURN(SSOD_ERR_NO_SSOD_SUPPLIED);

	if (pointer)
	{
		*pointer = (*pointer)->next;
	}

	RETURN(SSOD_ERR_NONE);
}

ssod_error ssod_iface_get_tgt(ssod_iface *iface)
{
	F_BEGIN();
	fnasso_error fnasso_err;
	ssod_realm *rlm;
	int tgt_time;

	if (!iface) E_RETURN(SSOD_ERR_NO_SSOD_SUPPLIED);
	rlm = NULL;

	ssod_realm_head(iface->ctx, &rlm);
	while (rlm)
	{
		fnasso_err = fnasso_context_set_roaming_realm(iface->sso, rlm->name);
		if (fnasso_err)
		{
			WARN("[%s] cannot set the realm (%s)", iface->name, rlm->name);
		}
		else
		{
			tgt_time = 0;
			DEBUG("[%s] trying to get TGT for realm (%s)", iface->name, rlm->name);
			fnasso_kdc_get_tgt(iface->sso, &tgt_time);
			if (tgt_time > 60)
			{
				EVENT("[%s] got TGT from realm (%s) for (%d) seconds", iface->name, rlm->name, tgt_time);
				iface->tgt_renewal_time = time(NULL) + tgt_time;
				RETURN(SSOD_ERR_NONE);
			}
		}
		ssod_realm_next(iface->ctx, &rlm);
	}

	// No TGT obtained
	iface->tgt_renewal_time = time(NULL) + SSOD_TIME_TGT_RENEW_MIN;
	RETURN(SSOD_ERR_NONE);
}