/* 
 * File:   ssod_realm.h
 * Author: hanky
 *
 * Created on March 16, 2014, 9:30 PM
 */

#ifndef SSOD_REALM_H
#define	SSOD_REALM_H


#include "ssod.h"

#ifdef	__cplusplus
extern "C" {
#endif

    ssod_error ssod_realm_free(ssod *ctx);
    
    ssod_error ssod_realm_add(ssod *ctx, const char *realm_name);
    ssod_error ssod_realm_del(ssod *ctx, const char *realm_name);
    ssod_error ssod_realm_search(ssod *ctx, const char *realm_name, ssod_realm **ptr);
    
    ssod_error ssod_realm_head(ssod *ctx, ssod_realm **ptr);
    ssod_error ssod_realm_next(ssod *ctx, ssod_realm **ptr);
    


#ifdef	__cplusplus
}
#endif

#endif	/* SSOD_REALM_H */

