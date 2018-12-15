#include "renderer.h"

#include <switch.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>

RenderContext renderContext;

void flushSwapBuffers(void);

void drawSplash(const char* splashPath)
{
    FILE* img = fopen(splashPath, "rb");
    renderContext.gfxBuffer = gfxGetFramebuffer(NULL, NULL);
    fread(renderContext.gfxBuffer, 1280*720*4, 1, img);
    fclose(img);
    flushSwapBuffers();
}

void drawFrame(AVFrame* frame, AVFrame* rgbframe, enum AVPixelFormat pix_fmt) 
{
    if (renderContext.ctx_sws == NULL)
        renderContext.ctx_sws = sws_getContext(frame->width, frame->height, pix_fmt, frame->width, frame->height, AV_PIX_FMT_RGBA, SWS_BILINEAR, 0, 0, 0);

    // Reuse gfxBuffer
    if (renderContext.gfxBuffer == NULL)
        renderContext.gfxBuffer = gfxGetFramebuffer(NULL, NULL);

    // We're scaling "into" the framebuffer for performance reasons.
    sws_scale(renderContext.ctx_sws, frame->data, frame->linesize, 0, frame->height, &(renderContext.gfxBuffer), rgbframe->linesize);

    //memcpy(fbuf, rgbframe->data[0], 1280 * 720 * 4);

    flushSwapBuffers();
}

void flushSwapBuffers() 
{
    gfxFlushBuffers();
    gfxSwapBuffers();
}
