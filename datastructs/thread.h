/* 
 * File:   ssod_thread.h
 * Author: hanky
 *
 * Created on March 14, 2014, 7:33 PM
 */

#ifndef SSOD_THREAD_H
#define	SSOD_THREAD_H

#define SSOD_THREAD_STATUS_STOPPED 0
#define SSOD_THREAD_STATUS_RUNNING 1
#define SSOD_THREAD_STATUS_MUST_EXIT 2


#include "ssod.h"
#include <pthread.h>

#ifdef	__cplusplus
extern "C" {
#endif

    ssod_error ssod_thread_init(ssod_iface *iface);
    ssod_error ssod_thread_free(ssod_iface *iface);

    ssod_error ssod_thread_start(ssod_iface *iface);
    ssod_error ssod_thread_stop(ssod_iface *iface);

#ifdef	__cplusplus
}
#endif

#endif	/* SSOD_THREAD_H */

