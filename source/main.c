#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define _ERROR_PRINT
#include "h264bsd_decoder.h"
#include "h264bsd_util.h"

#include <switch.h>

int setupServerSocket()
{
    int lissock;
    struct sockaddr_in server;
    lissock = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(6543);

    while (bind(lissock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        svcSleepThread(1e+9L);
    }
    listen(lissock, 3);
    return lissock;
}

#define MAGIC 0x010000 // "flipped" due to little endian
#define MAGIC_MASK 0x00FFFFFF

#define BUF_SIZE 10000000
#define LEN_SIZE 0x100

u32 fbwidth, fbheight;

#define RESX 1280
#define RESY 720
u8 *fakebuf[RESX * RESY * 4] = {0};
static Mutex fbMut;


void decodeLoop()
{
    int listenfd = setupServerSocket();
    int c = sizeof(struct sockaddr_in);
    struct sockaddr_in client;
    int sock = accept(listenfd, (struct sockaddr *)&client, (socklen_t *)&c);
    printf("Got connection!\n");

    u32 status;
    storage_t dec;
    status = h264bsdInit(&dec, 1);

    if (status != HANTRO_OK)
    {
        fprintf(stderr, "h264bsdInit failed\n");
        while (1)
            ;
    }

    u32 readBytes;
    int numPics = 0;
    u32 *pic;
    u32 picId, isIdrPic, numErrMbs;
    u32 top, left, width, height, croppingFlag;

    u8 *buf = malloc(BUF_SIZE);
    u8 *byteStrm = buf;
    int len = 0;

    while (appletMainLoop())
    {

        // If our buffer begins to run full we want to move stuff to the front
        if (BUF_SIZE - (byteStrm - buf) - len < LEN_SIZE)
        {
            // Copy the rest to the start of the buffer and set the byteStrm back to the beginning too
            memcpy(buf, byteStrm, BUF_SIZE - (byteStrm - buf) + len);
            byteStrm = buf;
        }

        len += recv(sock, byteStrm + len, LEN_SIZE, 0);

        // Waiting for the network to get enough data to decode stuff
        u8 done_data = 0;
        // TODO: No need to check the whole buffer every loop
        for (int i = 4; i < len - 4; i++)
        {
            if ((*((unsigned int *)(byteStrm + len - i)) & MAGIC_MASK) == MAGIC) // When it finds the magic the fun is over.
            {
                done_data = 1;
                break;
            }
        }
        if (!done_data)
            continue;

        u32 result = h264bsdDecode(&dec, byteStrm, len, 0, &readBytes);
        len -= readBytes;
        byteStrm += readBytes;

        switch (result)
        {
        case H264BSD_PIC_RDY:
            pic = h264bsdNextOutputPicture(&dec, &picId, &isIdrPic, &numErrMbs);

            mutexLock(&fbMut);
            memcpy(fakebuf, pic, fbwidth * fbheight * 4);
            mutexUnlock(&fbMut);

            if (++numPics % 60 == 0)
                printf("%d\n", numPics);

            break;
        case H264BSD_HDRS_RDY:
            h264bsdCroppingParams(&dec, &croppingFlag, &left, &width, &top, &height);
            if (!croppingFlag)
            {
                width = h264bsdPicWidth(&dec) * 16;
                height = h264bsdPicHeight(&dec) * 16;
            }
            else
            {
                fprintf(stderr, "Uhm, cropping flag set, panic!!!\n");
                while (1)
                    ;
            }
            printf("Decoded headers. Image size %dx%d\n", width, height);
            break;
        case H264BSD_RDY:
            break;
        case H264BSD_ERROR:
            fprintf(stderr, "Error\n");
            while (1)
                ;
        case H264BSD_PARAM_SET_ERROR:
            fprintf(stderr, "Param set error\n");
            while (1)
                ;
        }
    }
}

#define CLOCK_RATE 1785000000

int main(int argc, char **argv)
{
    pcvInitialize();
    pcvSetClockRate(PcvModule_Cpu, CLOCK_RATE); // Overclocking ;)

    // TODO: This is rather unoptimized
    static const SocketInitConfig socketInitConfig = {
        .bsdsockets_version = 1,

        .tcp_tx_buf_size = LEN_SIZE * 50,
        .tcp_rx_buf_size = LEN_SIZE * 50,
        .tcp_tx_buf_max_size = LEN_SIZE * 80,
        .tcp_rx_buf_max_size = LEN_SIZE * 80,

        .udp_tx_buf_size = 0x2400,
        .udp_rx_buf_size = 0xA500,

        .sb_efficiency = 2,
    };

    socketInitialize(&socketInitConfig);
    //socketInitializeDefault();

    nxlinkStdio();
    gfxInitDefault();
    mutexInit(&fbMut);

    Thread decodeThread;
    threadCreate(&decodeThread, decodeLoop, NULL, 0x5000, 0x3B, 2);
    threadStart(&decodeThread);

    while (appletMainLoop())
    {
        u8 *fb = gfxGetFramebuffer(&fbwidth, &fbheight);
        mutexLock(&fbMut);
        //memcpy(fb, fakebuf, RESX * RESY * 4);
        h264bsdConvertToRGBA(RESX, RESY, fakebuf, fb);
        mutexUnlock(&fbMut);
        gfxFlushBuffers();
        gfxSwapBuffers();
    }

    consoleExit(NULL);
    socketExit();
    pcvExit();
    gfxExit();
    return 0;
}
