#include <stdio.h>
#include <string.h>
#include "common.h"
#include "config.h"

#define CONFIG_FILE "In-Home-Switching.cfg"
#define CONFIG_VERSION 0
#define CONFIG_SERVER "192.168.0.117"
#define CONFIG_OVERCLOCK 2

Configuration config;

const char *configFileLoad = "%d,%255[^,],%d";
const char *configFileSave = "%d,%s,%d\n";

int Config_Save(Configuration config) 
{
	FILE *fp = fopen(CONFIG_FILE, "w");
	if (fp)
	{
		// Todo: Fix this from causing a crash on reload
    	//fprintf(fp, configFileSave, CONFIG_VERSION, config.server_address, config.overclock_index);
    	fclose(fp);
	}
	else
	{ 
		fprintf(stderr, "Could not save config file");
		return -1;
	}

	return 0;
}

int Config_Load(void) 
{
	int config_version = CONFIG_VERSION;
	strcpy(config.server_address, CONFIG_SERVER);
	config.overclock_index = CONFIG_OVERCLOCK;

	int res = 0;
	FILE *fp = fopen(CONFIG_FILE, "r");
	if (fp)
	{
    	res = fscanf(fp, configFileLoad, &config_version, &config.server_address[0], &config.overclock_index);
    	fclose(fp);
	}

	// Resave defaults if config file is missing or updated
	if (!fp || res != 3 || config_version < CONFIG_VERSION) 
	{
		strcpy(config.server_address, CONFIG_SERVER);
		config.overclock_index = CONFIG_OVERCLOCK;
		return Config_Save(config);
	}

	return 0;
}
