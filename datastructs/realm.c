#include "realm.h"

#include <string.h>
#include <stdlib.h>

ssod_error ssod_realm_free(ssod *ctx)
{
	F_BEGIN();
	ssod_realm *ptr;

	if (ctx)
	{
		while (ptr = ctx->realms)
		{
			ctx->realms = ptr->next;
			free(ptr->name);
			free(ptr);
		}
	}

	RETURN(SSOD_ERR_NONE);
}

ssod_error ssod_realm_add(ssod *ctx, const char *realm_name)
{
	F_BEGIN();
	ssod_realm *ptr, *last, *new;

	if (!ctx) E_RETURN(SSOD_ERR_NO_SSOD_SUPPLIED);
	if (!realm_name || !strlen(realm_name)) E_RETURN(SSOD_ERR_NO_REALM_NAME_SUPPLIED);



	last = ptr = ctx->realms;

	while (ptr)
	{
		if (strcmp(ptr->name, realm_name) == 0)
		{
			EVENT("realm (%s) already exists", realm_name);
			RETURN(SSOD_ERR_NONE);
		}
		last = ptr;
		ptr = ptr->next;
	}

	new = (ssod_realm*) malloc(sizeof (ssod_realm));
	new->name = (char*) malloc(strlen(realm_name) + 1);
	strcpy(new->name, realm_name);
	new->next = NULL;

	if (last)
		last->next = new;
	else
		ctx->realms = new;


	EVENT("added realm (%s)", new->name);
	RETURN(SSOD_ERR_NONE);
}

ssod_error ssod_realm_del(ssod *ctx, const char *realm_name)
{
	F_BEGIN();
	ssod_realm *ptr, *last;

	if (!ctx) E_RETURN(SSOD_ERR_NO_SSOD_SUPPLIED);
	if (!realm_name || !strlen(realm_name)) E_RETURN(SSOD_ERR_NO_REALM_NAME_SUPPLIED);


	ptr = ctx->realms;
	last = NULL;

	while (ptr)
	{
		if (strcmp(ptr->name, realm_name) == 0)
		{
			if (last)
				last->next = ptr->next;
			else
				ctx->realms = ptr->next;

			EVENT("deleted realm (%s)", ptr->name);
			free(ptr->name);
			free(ptr);
			RETURN(SSOD_ERR_NONE);
		}
		last = ptr;
		ptr = ptr->next;
	}

	EVENT("cannot delete realm (%s), because it doesn't exists", realm_name);
	RETURN(SSOD_ERR_NONE);
}

ssod_error ssod_realm_search(ssod *ctx, const char *realm_name, ssod_realm **ptr)
{
	F_BEGIN();
	ssod_realm *pointer;
	if (!ctx) E_RETURN(SSOD_ERR_NO_SSOD_SUPPLIED);
	if (!realm_name || !strlen(realm_name)) E_RETURN(SSOD_ERR_NO_REALM_NAME_SUPPLIED);
	if (!ptr) E_RETURN(SSOD_ERR_NO_OUTPUT_POINTER);

	pointer = ctx->realms;

	while (pointer)
	{
		if (strcmp(pointer->name, realm_name))
		{
			(*ptr) = pointer;
			RETURN(SSOD_ERR_NONE);
		}
		pointer = pointer->next;
	}

	(*ptr) = NULL;
	RETURN(SSOD_ERR_NONE);
}

ssod_error ssod_realm_head(ssod *ctx, struct ssod_realm_t **ptr)
{
	F_BEGIN();
	ssod_error error;
	if (!ctx) E_RETURN(SSOD_ERR_NO_SSOD_SUPPLIED);
	if (!ptr) E_RETURN(SSOD_ERR_NO_OUTPUT_POINTER);

	(*ptr) = ctx->realms;

	RETURN(SSOD_ERR_NONE);
}

ssod_error ssod_realm_next(ssod *ctx, struct ssod_realm_t **ptr)
{
	F_BEGIN();
	ssod_error error;
	if (!ctx) E_RETURN(SSOD_ERR_NO_SSOD_SUPPLIED);

	if (ptr)
	{
		*ptr = (*ptr)->next;
	}

	RETURN(SSOD_ERR_NONE);
}