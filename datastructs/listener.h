/* 
 * File:   ssod_iface.h
 * Author: hanky
 *
 * Created on March 14, 2014, 7:38 PM
 */

#ifndef SSOD_LISTENER_H
#define	SSOD_LISTENER_H

#include "ssod.h"

#include "../wpa_ctrl/wpa_ctrl.h"

#ifdef	__cplusplus
extern "C" {
#endif
    
    ssod_error ssod_listener_init(ssod_iface* iface);
    ssod_error ssod_listener_free(ssod_iface* iface);
    
    ssod_error ssod_listener_attach(ssod_iface* iface);
    ssod_error ssod_listener_detach(ssod_iface* iface);
    ssod_error ssod_listener_is_online(ssod_iface* iface, int *online);
    
    ssod_error ssod_listener_listen(ssod_iface* iface, char **output, int *output_len);
    ssod_error ssod_listener_update_msk(ssod_iface* iface, int *changed);
    
#ifdef	__cplusplus
}
#endif

#endif	/* SSOD_LISTENER_H */

