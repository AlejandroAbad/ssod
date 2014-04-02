/* 
 * File:   state_machine.h
 * Author: hanky
 *
 * Created on March 2, 2014, 5:12 PM
 */

#ifndef SSOD_H
#define	SSOD_H

#define TRUE 1
#define FALSE 0

// If no TGT received, wait for this time until next TGT request
#define SSOD_TIME_TGT_RENEW_MIN 30

// How many times we will try to attach to a wpa_supplicant ctrl iface
// before cosidering it offline, and the delay between tryes.
#define SSOD_TIME_LISTENER_ATTACH_TRYES 5
#define SSOD_TIME_LISTENER_ATTACH_DELAY 30

// Delays between checks for changes in the physical interfaces. Delay 1 will be used
// when there are monitorized interfaces (threr is at least one thread running), 
// delay 2 will be used when no interfaces are being monitorized.
#define SSOD_TIME_SEARCH_IFACE_CHANGES_1 60
#define SSOD_TIME_SEARCH_IFACE_CHANGES_2 60


#include "../debug.h"
#include "../wpa_ctrl/wpa_ctrl.h"
#include <pthread.h>
#include <fnasso.h>




#ifdef	__cplusplus
extern "C" {
#endif

    typedef enum {
        SSOD_ERR_NONE = 0,
        SSOD_ERR_EXTERNAL,
        SSOD_ERR_NO_SSOD_SUPPLIED,
        SSOD_ERR_IFACE_NOT_SUPPLIED,
        SSOD_ERR_IFACE_LISTENER_OFFLINE,
        SSOD_ERR_NO_REALM_NAME_SUPPLIED,
        SSOD_ERR_NO_REALM_POINTER,
        SSOD_ERR_NO_OUTPUT_POINTER,
        SSOD_ERR_NO_IFACE_NAME_SUPPLIED,
        SSOD_ERR_LISTEN_TIMEOUT,
        SSOD_ERR_CONFIG_FILE_PARSE,
        SSOD_ERR_USER_DONT_EXISTS,
        SSOD_ERR_CANT_CHANGE_USER
    } ssod_error;

    //typedef struct ssod_t ssod;

    typedef struct ssod_thread_t {
        pthread_t id;
        char status;
    } ssod_thread;

    typedef struct ssod_realm_t {
        struct ssod_realm_t *next;
        char *name;
    } ssod_realm;

    typedef struct ssod_listener_t {
        struct wpa_ctrl *monitor;
        char *path;
    } ssod_listener;

    typedef struct ssod_iface_t {
        char *name;
        unsigned char *msk;
        int msk_len;
        fnasso_ctx *sso;
        ssod_listener *listener;
        ssod_thread *thread;
        time_t tgt_renewal_time;
        struct ssod_t *ctx;
        struct ssod_iface_t *next;
    } ssod_iface;

    typedef struct ssod_t {
        char *wpa_ctrl_path;
        char *fnasso_ccache_path;
        ssod_iface *ifaces;
        ssod_realm *realms;
        pthread_cond_t thread_cond;
        pthread_mutex_t thread_mutex;
        ssod_iface *thread_dead_iface;
        uid_t uid;
    } ssod;

    char *ssod_error_string(ssod_error error);

    ssod_error ssod_init(ssod **context);
    ssod_error ssod_free(ssod **context);

    ssod_error ssod_set_user(ssod *context, const char *user);
    ssod_error ssod_set_wpa_ctrl_path(ssod *context, const char *wpa_ctrl_path);
    ssod_error ssod_set_ccache_path(ssod *context, const char *ccache_path);
    
    ssod_error ssod_update_networks(ssod *context);
    ssod_error ssod_start_threads(ssod *context);

    ssod_error ssod_join_thread(ssod *context);

#ifdef	__cplusplus
}
#endif

#endif	/* SSOD_H */

