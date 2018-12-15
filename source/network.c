#include "network.h"
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>

void networkInit(const SocketInitConfig* conf) 
{
    socketInitialize(conf);
    nxlinkStdio();
    avformat_network_init();
}

void networkDestroy() 
{
    avformat_network_deinit();
    socketExit();
}