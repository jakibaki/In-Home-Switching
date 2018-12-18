/*
    config: joel16/NX-Shell
    See config.h for license info.

    Modifications by CodeMooseUS
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "config.h"
#include "fs.h"

#define CONFIG_VERSION 0

Configuration config;
static int config_version_holder = 0;

const char *configFile =
	"config_ver = %d\n"
	"server_address = %[^\t\n]\n"
	"overclock_index = %d\n";

const char *configFileSave =
	"config_ver = %d\n"
	"server_address = %s\n"
	"overclock_index = %d\n";

Result Config_Save(Configuration config) 
{
	Result ret = 0;

	char *buf = (char *)malloc(512);
	snprintf(buf, 512, configFileSave, CONFIG_VERSION, config.server_address, config.overclock_index);
    printf("Saved config:\n%s\n", buf);
	// Todo: Fix this from causing a crash on reload
	// if (R_FAILED(ret = FS_Write("/switch/In-Home-Switching/config.cfg", buf))) 
    // {
	// 	free(buf);
	// 	return ret;
	// }

	free(buf);
	return 0;
}

Result Config_Load(void) {
	Result ret = 0;

	if (!FS_DirExists("/switch/"))
		FS_MakeDir("/switch");
	if (!FS_DirExists("/switch/In-Home-Switching/"))
		FS_MakeDir("/switch/In-Home-Switching");

	if (!FS_FileExists("/switch/In-Home-Switching/config.cfg")) {
    	printf("No config file found \n");
		strcpy(config.server_address, "192.168.0.117");
		config.overclock_index = 2;
		return Config_Save(config);
	}

	strcpy(config.server_address, "192.168.0.100");
	config.overclock_index = 2;

	u64 size = 0;
	FS_GetFileSize("/switch/In-Home-Switching/config.cfg", &size);
    printf("Loaded config:\n%d\n", size);
	char *buf = (char *)malloc(size + 1);

	// Todo: Fix this from causing a crash on reload
	// if (R_FAILED(ret = FS_Read("/switch/In-Home-Switching/config.cfg", size, buf))) {
	// 	free(buf);
	// 	return ret;
	// }

	buf[size] = '\0';
    printf("Loaded config:\n%s\n", buf);
	// Todo: Fix this from causing a crash on reload
	//sscanf(buf, configFile, &config_version_holder, &config.server_address[0], &config.overclock_index);
	free(buf);

	// Delete config file if config file is updated
	if (config_version_holder < CONFIG_VERSION) {
		FS_RemoveFile("/switch/In-Home-Switching/config.cfg");
		strcpy(config.server_address, "192.168.0.117");
		config.overclock_index = 2;
		return Config_Save(config);
	}

	return 0;
}
