#include "../debug.h"
#include "thread.h"
#include "iface.h"
#include "../watchers/watchers.h"

ssod_error ssod_thread_init(ssod_iface *iface)
{
	F_BEGIN();
	ssod_error error;
	if (!iface) E_RETURN(SSOD_ERR_IFACE_NOT_SUPPLIED);

	error = ssod_thread_free(iface);
	if (error) E_THROW(error);

	iface->thread = (ssod_thread*) malloc(sizeof (ssod_thread));
	iface->thread->status = SSOD_THREAD_STATUS_STOPPED;

	RETURN(SSOD_ERR_NONE);
}

ssod_error ssod_thread_free(ssod_iface *iface)
{
	F_BEGIN();
	if (!iface) E_RETURN(SSOD_ERR_IFACE_NOT_SUPPLIED);

	if (iface->thread)
	{
		ssod_thread_stop(iface);
		free(iface->thread);
		iface->thread = NULL;
	}

	RETURN(SSOD_ERR_NONE);
}

ssod_error ssod_thread_start(ssod_iface *iface)
{
	F_BEGIN();
	int retval;
	ssod_error error;
	if (!iface) E_RETURN(SSOD_ERR_IFACE_NOT_SUPPLIED);
	if (!iface->thread)
	{
		error = ssod_thread_init(iface);
		if (error) E_THROW(error);
	}
	
	if (iface->thread->status == SSOD_THREAD_STATUS_RUNNING) RETURN(SSOD_ERR_NONE);

	iface->thread->status = SSOD_THREAD_STATUS_RUNNING;
	retval = pthread_create(&iface->thread->id, NULL, ssod_network_watcher, iface);
	if (retval)
	{
		iface->thread->status = SSOD_THREAD_STATUS_STOPPED;
		E_RETURN(SSOD_ERR_EXTERNAL, "pthread_create", error_message(retval));
	}

	EVENT("[%s] thread started", iface->name);

	RETURN(SSOD_ERR_NONE);
}

ssod_error ssod_thread_stop(ssod_iface *iface)
{
	F_BEGIN();
	int retval;
	if (!iface) E_RETURN(SSOD_ERR_IFACE_NOT_SUPPLIED);
	if (!iface->thread) ssod_thread_init(iface);
	//if (iface->thread->status) RETURN(SSOD_ERR_NONE);
	iface->thread->status = SSOD_THREAD_STATUS_MUST_EXIT;

	RETURN(SSOD_ERR_NONE);
}
