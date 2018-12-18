#include "renderer.h"

#include <switch.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include "video.h"

void flushSwapBuffers(void);

RenderContext *createRenderer()
{
    RenderContext *context = (RenderContext *)malloc(sizeof(RenderContext));
    context->ctx_sws = NULL;

    context->window = SDL_CreateWindow("sdl2_gles2", 0, 0, 1920, 1080, SDL_WINDOW_FULLSCREEN);
    if (context->window == NULL)
    {
        SDL_Log("SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
        while (1)
            ;
    }

    int mode_count = SDL_GetNumDisplayModes(0);
    const SDL_DisplayMode mode;
    SDL_GetDisplayMode(0, 1, &mode);
    SDL_SetWindowDisplayMode(context->window, &mode);

    context->renderer = SDL_CreateRenderer(context->window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (context->renderer == NULL)
    {
        SDL_Log("SDL_CreateRenderer: %s\n", SDL_GetError());
        SDL_Quit();
        while (1)
            ;
    }

    context->yuv_text = SDL_CreateTexture(context->renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, RESX, RESY);

    context->rect.x = 0;
    context->rect.y = 0;
    context->rect.w = RESX;
    context->rect.h = RESY;

    mutexInit(&context->texture_mut);
    mutexInit(&context->frame_avail_mut);
    context->frame_avail = false;

    return context;
}

void setFrameAvail(RenderContext* context)
{
    mutexLock(&context->frame_avail_mut);
    context->frame_avail = true;
    mutexUnlock(&context->frame_avail_mut);
}

bool checkFrameAvail(RenderContext* context)
{
    bool ret;
    mutexLock(&context->frame_avail_mut);
    ret = context->frame_avail;
    context->frame_avail = false;
    mutexUnlock(&context->frame_avail_mut);
    return ret;
}

/*
void drawSplash(RenderContext *context, const char *splashPath)
{
    FILE *img = fopen(splashPath, "rb");
    context->gfxBuffer = gfxGetFramebuffer(NULL, NULL);
    fread(context->gfxBuffer, RESX * RESY * 4, 1, img);
    fclose(img);
    flushSwapBuffers();
}
*/

u64 old_time, new_time;
void drawFrame(RenderContext *renderContext, VideoContext *videoContext)
{
    AVFrame *frame = videoContext->frame;
    //AVFrame *rgbframe = videoContext->rgbframe;
    enum AVPixelFormat pix_fmt = videoContext->video_dec_ctx->pix_fmt;

    //if (context->ctx_sws == NULL)
    //    context->ctx_sws = sws_getContext(frame->width, frame->height, pix_fmt, frame->width, frame->height, AV_PIX_FMT_RGBA, SWS_BILINEAR, 0, 0, 0);

    // We're scaling "into" the framebuffer for performance reasons.
    //context->gfxBuffer = gfxGetFramebuffer(NULL, NULL);
    //sws_scale(context->ctx_sws, (const uint8_t *const *)frame->data, frame->linesize, 0, frame->height, &(context->gfxBuffer), rgbframe->linesize);

    //    SDL_UpdateTexture( renderContext->renderer, NULL, renderContext->buffer, RESX);

    mutexLock(&renderContext->texture_mut);
    memcpy(renderContext->YPlane, frame->data[0], sizeof(renderContext->YPlane));
    memcpy(renderContext->UPlane, frame->data[1], sizeof(renderContext->UPlane));
    memcpy(renderContext->VPlane, frame->data[2], sizeof(renderContext->VPlane));
    mutexUnlock(&renderContext->texture_mut);
    setFrameAvail(renderContext);

    if(++videoContext->video_frame_count % 60 == 0) {
        new_time = svcGetSystemTick();
        printf("Framerate: %f\n", 60.0 / ((new_time - old_time) / 19200000.0));
        old_time = new_time;
    }


    //memcpy(fbuf, rgbframe->data[0], RESX * RESY * 4);

    //flushSwapBuffers();
}

void flushSwapBuffers()
{
    gfxFlushBuffers();
    gfxSwapBuffers();
}

void freeRenderer(RenderContext *context)
{
    free(context);
}