#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <inttypes.h>
#include <switch.h>

#include <netdb.h>
#include <netinet/in.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <unistd.h>

#define SAMPLERATE 48000
#define CHANNELCOUNT 2
#define FRAMERATE (1000 / 30)
#define SAMPLECOUNT (SAMPLERATE / FRAMERATE)
#define BYTESPERSAMPLE 2

int listenfd = -1;
int wait_for_conn()
{
    if (listenfd == -1)
    {
        struct sockaddr_in server;

        listenfd = socket(AF_INET, SOCK_STREAM, 0);
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port = htons(2224);

        if (bind(listenfd, (struct sockaddr *)&server, sizeof(server)) < 0)
        {
            close(listenfd);
            listenfd = -1;
            return 0;
        }
        listen(listenfd, 1);
    }

    struct sockaddr_in client;
    int c = sizeof(struct sockaddr_in);
    int retsock = accept(listenfd, (struct sockaddr *)&client, (socklen_t *)&c);
    if (retsock < 0)
    {
        close(listenfd);
        listenfd = -1;
        return wait_for_conn();
    }
    return retsock;
}


#define BUF_COUNT 2
AudioOutBuffer audiobuf[BUF_COUNT];
u8 *buf_data[BUF_COUNT];
int curBuf = 0;
#define swapbuf (curBuf = (curBuf + 1) % (BUF_COUNT))

AudioOutBuffer *audout_released_buf;
int audout_filled = 0;
void play_buf(int buffer_size, int data_size)
{

    audiobuf[curBuf].next = 0;
    audiobuf[curBuf].buffer = buf_data[curBuf];
    audiobuf[curBuf].buffer_size = buffer_size;
    audiobuf[curBuf].data_size = data_size;
    audiobuf[curBuf].data_offset = 0;
    audoutAppendAudioOutBuffer(&audiobuf[curBuf]);

    audout_filled++;

    if (audout_filled >= BUF_COUNT)
    {
        u32 released_count;
        audoutWaitPlayFinish(&audout_released_buf, &released_count, UINT64_MAX);
    }

    swapbuf;
}

#define MIN_LEFT_SOCK 15000
char bs_buf[MIN_LEFT_SOCK];

void audioHandlerLoop()
{
    u32 data_size = (SAMPLECOUNT * CHANNELCOUNT * BYTESPERSAMPLE);
    u32 buffer_size = (data_size + 0xfff) & ~0xfff;

    for (int curBuf = 0; curBuf < BUF_COUNT; curBuf++)
    {
        buf_data[curBuf] = memalign(0x1000, buffer_size);
    }

    int sock = wait_for_conn();

    int played =0;
    while (1)
    {

        //memset(buf_data[curBuf], 0, buffer_size); // TODO: probably not needed
        
        int ret = recv(sock, buf_data[curBuf], data_size, MSG_WAITALL);
        if(ret < 0) {
            perror("Oh no!\n");
            while(1);
        }

        // We "peek" the socket to find out how much data is left in the buffer and decide if we want to play or skip the samples
        int n_bytes = recv(sock, bs_buf, MIN_LEFT_SOCK, MSG_PEEK);
        if (n_bytes < MIN_LEFT_SOCK)
        {
            //armDCacheFlush(buf_data, buffer_size);
            play_buf(buffer_size, data_size);
            played++;
        } else {
            //recv(sock, bs_buf, MIN_LEFT_SOCK, 0);
        }
    }


    for (int curBuf = 0; curBuf < BUF_COUNT; curBuf++)
    {
        free(buf_data[curBuf]);
    }

    return 0;
}
