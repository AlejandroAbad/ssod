/* 
 * File:   debug.h
 * Author: hanky
 *
 * Created on February 23, 2014, 4:18 PM
 */

#ifndef DEBUG_H
#define	DEBUG_H


#ifdef	__cplusplus
extern "C" {
#endif

#define SSOD_FUNC_TRACE_DEBUG 0
#define SSOD_DEBUG 1
#define SSOD_EVENT 2
#define SSOD_WARN 3
#define SSOD_ERROR 4

#ifdef FTRACE_DEBUG_VERSION
#ifndef DEBUG_VERSION
#define DEBUG_VERSION
#endif    
#endif

#ifdef DEBUG_MODE

#define F_BEGIN() {\
                ssod_func_start(__FUNCTION__);\
        }

#define RETURN(status) {\
                ssod_func_end(SSOD_FUNC_TRACE_DEBUG, __FUNCTION__, status);\
                return status;\
        }


#define E_RETURN(status, args...) {\
                ssod_printf(SSOD_ERROR, __FUNCTION__, ssod_error_string(status), ##args);\
                ssod_func_end(SSOD_FUNC_TRACE_DEBUG, __FUNCTION__, status);\
                return status;\
        }

#define E_THROW(status) {\
                ssod_func_end(SSOD_FUNC_TRACE_DEBUG, __FUNCTION__, status);\
                return status;\
        }

#define F_EXIT(status) {\
                ssod_func_end(SSOD_ERROR, __FUNCTION__, status);\
                exit(status);\
        }

#define DEBUG(fmt, args...) {\
                ssod_printf(SSOD_DEBUG, __FUNCTION__, fmt, ##args);\
        }

#define EVENT(fmt, args...) {\
                ssod_printf(SSOD_EVENT, __FUNCTION__, fmt, ##args);\
        }

#define WARN(fmt, args...) {\
                ssod_printf(SSOD_WARN, __FUNCTION__,fmt, ##args);\
        }

#define ERROR(fmt, args...) {\
                ssod_printf(SSOD_ERROR, __FUNCTION__,fmt, ##args);\
        }

#else /* DEBUG_VERSION */


#define F_BEGIN() {\
        }

#define RETURN(status) {\
                return status;\
        }

#define E_RETURN(status, args...) {\
                ssod_printf(SSOD_ERROR, __FUNCTION__, ssod_error_string(status), ##args);\
                return status;\
        }

#define E_THROW(status) {\
                return status;\
        }

#define F_EXIT(status, args...) {\
                exit(status);\
        }

#define DEBUG(fmt, args...) {\
        }

#define EVENT(fmt, args...) {\
                ssod_printf(SSOD_EVENT, __FUNCTION__, fmt, ##args);\
        }

#define WARN(fmt, args...) {\
                ssod_printf(SSOD_WARN, __FUNCTION__,fmt, ##args);\
        }

#define ERROR(fmt, args...) {\
                ssod_printf(SSOD_ERROR, __FUNCTION__,fmt, ##args);\
        }


#endif


#ifdef	__cplusplus
}
#endif

#endif	/* DEBUG_H */

