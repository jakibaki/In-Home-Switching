#ifndef _NETWORK_H
#define _NETWORK_H

#include <switch.h>

#define URL "tcp://0.0.0.0:2222"
//#define TCP_RECV_BUFFER "500000"

void networkInit(const SocketInitConfig* conf);
void networkDestroy(); 

#endif