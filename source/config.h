#ifndef _CONFIG_H
#define _CONFIG_H

#include <switch.h>

typedef struct {
	char server_address[256];
	int overclock_index;
} Configuration;

extern Configuration config;

int Config_Save(Configuration config);
int Config_Load(void);

#endif
