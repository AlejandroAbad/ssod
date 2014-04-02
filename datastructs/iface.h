/* 
 * File:   ssod_iface.h
 * Author: hanky
 *
 * Created on March 14, 2014, 7:49 PM
 */

#ifndef SSOD_IFACE_H
#define	SSOD_IFACE_H

#include "ssod.h"
#include <fnasso.h>

#ifdef	__cplusplus
extern "C" {
#endif
   
    ssod_error ssod_iface_free(ssod *ctx);
    
    ssod_error ssod_iface_add(ssod *ctx, const char *name);
    ssod_error ssod_iface_del(ssod *ctx, const char *name);
    ssod_error ssod_iface_search(ssod *ctx, const char *name, ssod_iface **ptr);
    
    ssod_error ssod_iface_head(ssod *ctx, ssod_iface **ptr);
    ssod_error ssod_iface_next(ssod *ctx, ssod_iface **ptr);
    
    ssod_error ssod_iface_get_tgt(ssod_iface *iface);
    

#ifdef	__cplusplus
}
#endif



#endif	/* SSOD_IFACE_H */

