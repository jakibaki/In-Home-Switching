// The following ffmpeg code is inspired by an official ffmpeg example, so here is its Copyright notice:

/*
 * Copyright (c) 2012 Stefano Sabatini
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.


*
 * @file
 * Demuxing and decoding example.
 *
 * Show how to use the libavformat and libavcodec API to demux and
 * decode audio and video data.
 * @example demuxing_decoding.c

*/


#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#include <switch.h>

#include "input.h"
#include "video.h"

static const SocketInitConfig socketInitConf = {
    .bsdsockets_version = 1,

    .tcp_tx_buf_size = 0x8000,
    .tcp_rx_buf_size = 0x10000,
    .tcp_tx_buf_max_size = 0x40000,
    .tcp_rx_buf_max_size = 0x40000,

    .udp_tx_buf_size = 0xA2400,
    .udp_rx_buf_size = 0xAA500,

    .sb_efficiency = 4,

    .serialized_out_addrinfos_max_size = 0x1000,
    .serialized_out_hostent_max_size = 0x200,
    .bypass_nsd = false,
    .dns_timeout = 0,
};


void drawSplash()
{
    FILE* img = fopen("romfs:/splash.rgba", "rb");
    u8 *fbuf = gfxGetFramebuffer(NULL, NULL);
    fread(fbuf, 1280*720*4, 1, img);
    fclose(img);
    gfxFlushBuffers();
    gfxSwapBuffers();
}

int main(int argc, char **argv)
{
    pcvInitialize();
    pcvSetClockRate(PcvModule_Cpu, 1785000000);
    socketInitialize(&socketInitConf);
    romfsInit();
    nxlinkStdio();
    gfxInitDefault();

    static Thread inputHandlerThread;
    threadCreate(&inputHandlerThread, inputHandlerLoop, NULL, 0x1000, 0x2b, 0);
    threadStart(&inputHandlerThread);

    int ret = avformat_network_init();
    if (ret < 0)
    {
        char errbuf[100];
        av_strerror(ret, errbuf, 100);
        fprintf(stderr, "Initialize Error %s\n", errbuf);
    }
    while (appletMainLoop())
    {
        drawSplash();
        handleVid();
    }

    avformat_network_deinit();

    gfxExit();
    pcvExit();
    socketExit();
}
