#ifndef _RENDERER_H
#define _RENDERER_H

#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <switch.h>

typedef struct {
    struct SwsContext *ctx_sws;
    u8* gfxBuffer;
} RenderContext;

void drawSplash(const char* splashPath);
void drawFrame(AVFrame* frame, AVFrame* rgbframe, enum AVPixelFormat pix_fmt);

#endif