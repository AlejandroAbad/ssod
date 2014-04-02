#include "datastructs/ssod.h"
#include "debug.h"
#include "inih/ini.h"
#include "datastructs/realm.h"


#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>
#include <getopt.h>



ssod *data;

void exit_handler(int signal)
{
	EVENT("shutting down process");
	ssod_free(&data);
	exit(127 + signal);
}

static int conf_reader(void* context, const char* section, const char* name, const char* value)
{
	ssod_error error;
	ssod* ctx = (ssod*) context;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

	DEBUG("[%s] %s -> %s", section, name, value);

	if (MATCH("", "user"))
	{
		error = ssod_set_user(ctx, value);
		if (error) E_THROW(error);
	}
	else if (MATCH("", "wpa_ctrl_path"))
	{
		error = ssod_set_wpa_ctrl_path(ctx, value);
		if (error) E_THROW(error);
	}
	else if (MATCH("", "credential_cache_file"))
	{
		error = ssod_set_ccache_path(ctx, value);
		if (error) E_THROW(error);
	}
	else if (MATCH("roaming_realms", "realm"))
	{
		error = ssod_realm_add(ctx, value);
		if (error) E_THROW(error);
	}
	else
	{
		return 0; /* unknown section/name, error */
	}
	return 1;
}

static void usage(char * programname)
{
	ERROR("usage: %s -c <config_file>", programname);
}

int main(int argc, char *argv[])
{
	ssod_error error;
	int retval;
	char *conf_file;

	opterr = 0;
	conf_file = "/etc/ssod/ssod.conf";
	while ((retval = getopt(argc, argv, "c:")) != -1)
	{
		switch (retval)
		{
			case 'c':
				conf_file = optarg;
				break;
			case '?':
			case 'h':
			default:
				usage(argv[0]);
				exit(1);
				break;
		}
	}

	signal(SIGINT, exit_handler);
	signal(SIGTERM, exit_handler);
	signal(SIGQUIT, exit_handler);
	signal(SIGHUP, exit_handler);


	error = ssod_init(&data);
	if (error) F_EXIT(error);

	DEBUG("reading configuration file (%s)", conf_file);
	if (ini_parse(conf_file, conf_reader, data) != 0)
	{
		E_RETURN(SSOD_ERR_CONFIG_FILE_PARSE, conf_file);
	}

	while (1)
	{
		ssod_update_networks(data);
		ssod_join_thread(data);
	}
}
