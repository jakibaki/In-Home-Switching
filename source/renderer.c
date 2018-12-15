#include "renderer.h"

#include <switch.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include "video.h"

void flushSwapBuffers(void);

RenderContext* createRenderer()
{
    RenderContext* context = (RenderContext*)malloc(sizeof(RenderContext));
    context->ctx_sws = NULL;
    return context;
}

void drawSplash(RenderContext* context, const char* splashPath)
{
    FILE* img = fopen(splashPath, "rb");
    context->gfxBuffer = gfxGetFramebuffer(NULL, NULL);
    fread(context->gfxBuffer, 1280*720*4, 1, img);
    fclose(img);
    flushSwapBuffers();
}

void drawFrame(RenderContext* context, VideoContext* videoContext) 
{
    AVFrame* frame = videoContext->frame;
    AVFrame* rgbframe = videoContext->rgbframe;
    enum AVPixelFormat pix_fmt = videoContext->video_dec_ctx->pix_fmt;

    if (context->ctx_sws == NULL) 
        context->ctx_sws = sws_getContext(frame->width, frame->height, pix_fmt, frame->width, frame->height, AV_PIX_FMT_RGBA, SWS_BILINEAR, 0, 0, 0);
    
    // We're scaling "into" the framebuffer for performance reasons.
    context->gfxBuffer = gfxGetFramebuffer(NULL, NULL);
    sws_scale(context->ctx_sws, frame->data, frame->linesize, 0, frame->height, &(context->gfxBuffer), rgbframe->linesize);

    //memcpy(fbuf, rgbframe->data[0], 1280 * 720 * 4);

    flushSwapBuffers();
}

void flushSwapBuffers() 
{
    gfxFlushBuffers();
    gfxSwapBuffers();
}

void freeRenderer(RenderContext* context) {
    free(context);
}