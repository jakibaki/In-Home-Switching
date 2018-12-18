#ifndef _CONTEXT_H
#define _CONTEXT_H

#include <switch.h>
#include <SDL2/SDL.h>
#include <libavformat/avformat.h>

#define RESX 1280
#define RESY 720

typedef struct {
    SDL_Texture  *texture;
    AVFrame *frame;
    SDL_mutex *frame_mut;
    bool video_active;
    SDL_mutex *video_active_mut;
    int overclock_status;
} RenderContext;

typedef struct
{
    AVFormatContext *fmt_ctx;
    AVCodecContext *video_dec_ctx; //, *audio_dec_ctx;
    AVStream *video_stream;        //, *audio_stream = NULL;
    int video_stream_idx;          //, audio_stream_idx = -1;
    AVFrame *rgbframe;
    AVFrame *frame;
    uint8_t *video_dst_data[4];
    int video_dst_linesize[4];
    int video_frame_count;
    RenderContext *renderContext;
    //static int audio_frame_count = 0;
} VideoContext;

typedef struct
{
    int lissock;
    int sock;
} JoyConSocket;

#endif