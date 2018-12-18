#ifndef _CONTEXT_H
#define _CONTEXT_H

#include <libavformat/avformat.h>
#include <switch.h>
#include <SDL2/SDL.h>

typedef struct {
    SDL_Texture* texture;
} RenderContext;


typedef struct {
    AVFormatContext *fmt_ctx;
    AVCodecContext *video_dec_ctx; //, *audio_dec_ctx;
    AVStream *video_stream; //, *audio_stream = NULL;
    int video_stream_idx; //, audio_stream_idx = -1;
    AVFrame *rgbframe;
    AVFrame* frame;
    uint8_t *video_dst_data[4];
    int video_dst_linesize[4];
    int video_frame_count;
    RenderContext* renderContext;
    //static int audio_frame_count = 0;
} VideoContext;


typedef struct {
    int lissock;
    int sock;
} JoyConSocket;


#endif