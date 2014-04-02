#include "ssod.h"

char *ssod_error_string(ssod_error error)
{
	switch (error)
	{
		case SSOD_ERR_NONE: return "sucess";
		case SSOD_ERR_EXTERNAL: return "%s: %s";
		case SSOD_ERR_NO_SSOD_SUPPLIED: return "supplied ssod structure is NULL";
		case SSOD_ERR_NO_OUTPUT_POINTER: return "no pointer for an output value was supplied";
		case SSOD_ERR_IFACE_NOT_SUPPLIED: return "interface not supplied";
	

		case SSOD_ERR_NO_REALM_NAME_SUPPLIED: return "no realm supplied";
		case SSOD_ERR_NO_REALM_POINTER: return "no realm poiter supplied";
		
		case SSOD_ERR_IFACE_LISTENER_OFFLINE: return "listener is offline";
		
		case SSOD_ERR_NO_IFACE_NAME_SUPPLIED: return "no interface name supplied";
		case SSOD_ERR_LISTEN_TIMEOUT: return "timeout expired while listening for new events in the wpa_supplicant interface";
		
		
		case SSOD_ERR_CONFIG_FILE_PARSE: return "error while parsing config file (%s)";
		case SSOD_ERR_USER_DONT_EXISTS: return "username (%s) doesn't exists";
		case SSOD_ERR_CANT_CHANGE_USER: return "cannot switch to user %d (%s). setuid: %s";

		default: return "unknown error";
	}
}
