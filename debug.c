#include "debug.h"
#include "fnasso.h"
#include <stdarg.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>


char *str_lvl(int lvl)
{
	switch (lvl)
	{
		case SSOD_FUNC_TRACE_DEBUG: return "FTD";
		case SSOD_DEBUG: return "DBG";
		case SSOD_EVENT: return "EVT";
		case SSOD_WARN: return "WRN";
		case SSOD_ERROR: return "ERR";
		default: "???";
	}
}

void ssod_printf(int lvl, const char* fooname, const char *fmt, ...)
{
#ifndef TRACE_MODE
	if (lvl == SSOD_FUNC_TRACE_DEBUG) return;
#endif
	
	va_list ap;
	va_start(ap, fmt);
	int i;
	char line[4092];
	
#ifdef DEBUG_MODE
	printf("@%lu [%s] %s: ", pthread_self(), str_lvl(lvl), fooname);
#else
	printf("[%s] %s: ", str_lvl(lvl), fooname);
#endif
	
	memset(line, 0, sizeof(line));
	vsprintf(line, fmt, ap);
	
	for (i = 0 ; i < strlen(line); i++)
	{
		if (line[i] == '\n')
			line[i] = ' ';
	}
	
	printf("%s\n", line);

	va_end(ap);
}

void ssod_func_start(const char *fooname)
{
	ssod_printf(SSOD_FUNC_TRACE_DEBUG, fooname, "");
}

void ssod_func_end(int level, const char *fooname, int status)
{
	ssod_printf(level, fooname, "exit status (%d)", status);
}


