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

#include "util.h"
#include "gamepad.h"

#define _ERROR_PRINT
#include "h264/h264bsd_decoder.h"
#include "h264/h264bsd_util.h"

#include <switch.h>

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
    u8 *buf = malloc(BUF_SIZE);
    u8 *byteStrm = buf;


    printf("Decoder: Trying to listen!\n");    
    int listenfd = setupServerSocket(6543);
    printf("Decoder: Can listen!\n");

    int c = sizeof(struct sockaddr_in);
    struct sockaddr_in client;
    
    while (appletMainLoop())
    {
        u32 readBytes;
        int numPics = 0;
        u32 *pic;
        u32 picId, isIdrPic, numErrMbs;
        u32 top, left, width, height, croppingFlag;

        printf("Trying to accept!\n");
        int sock = accept(listenfd, (struct sockaddr *)&client, (socklen_t *)&c);
        if(sock < 0) {
            printf("Accepting failed!\n");
            close(listenfd);
            listenfd = setupServerSocket(6543);
            continue;
        }
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

            int recvlen = recv(sock, byteStrm + len, LEN_SIZE, 0);
            if (recvlen <= 0)
            {
                printf("Recv failed!\nConnection close?\n");
                break;
            }

            len += recvlen;

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

            char shouldCont = 1;
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
                    shouldCont = 0;
                    break;
                }
                printf("Decoded headers. Image size %dx%d\n", width, height);
                break;
            case H264BSD_RDY:
                break;
            case H264BSD_ERROR:
                fprintf(stderr, "Error\n");
                shouldCont = 0;
                break;
            case H264BSD_PARAM_SET_ERROR:
                fprintf(stderr, "Param set error\n");
                shouldCont = 0;
                break;
            }
            if (!shouldCont)
                break;
        }

        h264bsdShutdown(&dec);

        close(sock);
    }
    close(listenfd);
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
    threadCreate(&decodeThread, decodeLoop, NULL, 0x5000, 0x2C, 2);
    threadStart(&decodeThread);


    int c = sizeof(struct sockaddr_in);
    struct sockaddr_in client;

    int listenfd = setupServerSocket(6544);
    int sock = accept(listenfd, (struct sockaddr *)&client, (socklen_t *)&c);

    while (appletMainLoop())
    {
        u8 *fb = gfxGetFramebuffer(&fbwidth, &fbheight);
        mutexLock(&fbMut);
        h264bsdConvertToRGBA(RESX, RESY, fakebuf, fb);
        mutexUnlock(&fbMut);

        if(gamePadSend(sock) != 0) {
            sock = accept(listenfd, (struct sockaddr *)&client, (socklen_t *)&c);
        }


        gfxFlushBuffers();
        gfxSwapBuffers();


    }

    consoleExit(NULL);
    socketExit();
    pcvExit();
    gfxExit();
    return 0;
}
