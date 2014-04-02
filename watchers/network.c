#include "watchers.h"
#include "../datastructs/ssod.h"
#include "../datastructs/thread.h"

#include <string.h>

void *ssod_network_watcher(void *arg)
{
	ssod_iface *iface;
	ssod_error error;
	char *output;
	int output_len, changed, tryes;
	time_t wakeup_time, delay;


	iface = (ssod_iface*) arg;
	tryes = 0;
	output = NULL;
	DEBUG("[%s] started network watcher thread", iface->name);


bootstraping:
	changed = FALSE;
	ssod_listener_update_msk(iface, &changed);
	if (changed)
	{
		EVENT("[%s] New MSK obtained, requesting TGT", iface->name);
		ssod_iface_get_tgt(iface);
	}
	else
	{
		iface->tgt_renewal_time = time(NULL) + SSOD_TIME_TGT_RENEW_MIN;
	}


	while (iface->thread->status == SSOD_THREAD_STATUS_RUNNING)
	{
		output_len = WPA_CTRL_MAX_BUFFER_SIZE;
		error = ssod_listener_listen(iface, &output, &output_len);
		if (error)
		{
			error = ssod_listener_attach(iface);
			if (error)
			{
				tryes++;
				ERROR("[%s] unable to attach in (%d) tryes", iface->name, tryes);
				if (tryes > SSOD_TIME_LISTENER_ATTACH_TRYES)
				{
					EVENT("[%s] clossing thread because was unable to connect in (%d) tryes", iface->name, tryes - 1);
					iface->thread->status = SSOD_THREAD_STATUS_STOPPED;
				}
				else
				{
					DEBUG("[%s] thread going to sleep (%u) seconds", iface->name, SSOD_TIME_LISTENER_ATTACH_DELAY);
					
					delay = SSOD_TIME_LISTENER_ATTACH_DELAY;
					wakeup_time = time(NULL) + delay;
					
					while (delay > 0)
					{
						sleep(delay);
						delay = wakeup_time - time(NULL);
					}
					
					
					
				}
			}
			else
			{
				tryes = 0;
			}
		}

		if (output_len && strstr(output, WPA_EVENT_CONNECTED))
		{
			EVENT("[%s] NEW CONNECTION DETECTED", iface->name);
			goto bootstraping;
		}

		if (iface->tgt_renewal_time <= time(NULL))
		{
			EVENT("[%s] TGT renewal time reached, trying to get new MSK and TGT", iface->name);
			goto bootstraping;
		}




		if (output) free(output);
		output = NULL;
	}


	DEBUG("[%s] finished network watcher thread whit status (%d)", iface->name, iface->thread->status);

	if (iface->thread->status == SSOD_THREAD_STATUS_STOPPED)
	{
		pthread_mutex_lock(&iface->ctx->thread_mutex);
		iface->ctx->thread_dead_iface = iface;
		pthread_cond_signal(&iface->ctx->thread_cond);
		pthread_mutex_unlock(&iface->ctx->thread_mutex);
	}

	pthread_detach(iface->thread->id);
	pthread_exit(iface);
}
