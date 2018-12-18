#include "renderer.h"

#include <switch.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <unistd.h>
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

    const SDL_DisplayMode mode;
    SDL_GetDisplayMode(0, 1, &mode);
    //SDL_SetWindowDisplayMode(context->window, &mode);

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
    mutexInit(&context->video_active_mut);
    context->frame_avail = false;
    context->video_active = false;

    PlFontData fontData, fontExtData;
    plGetSharedFontByType(&fontData, PlSharedFontType_Standard);
    plGetSharedFontByType(&fontExtData, PlSharedFontType_NintendoExt);
    context->font = FC_CreateFont();
    FC_LoadFont_RW(context->font, context->renderer, SDL_RWFromMem((void *)fontData.address, fontData.size), SDL_RWFromMem((void *)fontExtData.address, fontExtData.size), 1, 40, FC_MakeColor(0, 0, 0, 255), TTF_STYLE_NORMAL);

    return context;
}

void setFrameAvail(RenderContext *context)
{
    mutexLock(&context->frame_avail_mut);
    context->frame_avail = true;
    mutexUnlock(&context->frame_avail_mut);
}

bool checkFrameAvail(RenderContext *context)
{
    bool ret;
    mutexLock(&context->frame_avail_mut);
    ret = context->frame_avail;
    context->frame_avail = false;
    mutexUnlock(&context->frame_avail_mut);
    return ret;
}

bool isVideoActive(RenderContext *context)
{
    bool ret;
    mutexLock(&context->video_active_mut);
    ret = context->video_active;
    mutexUnlock(&context->video_active_mut);
    return ret;
}

void setVideoActive(RenderContext *context, bool active)
{
    mutexLock(&context->video_active_mut);
    context->video_active = active;
    mutexUnlock(&context->video_active_mut);
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

void SDL_ClearScreen(RenderContext *context, SDL_Color colour)
{
    SDL_SetRenderDrawColor(context->renderer, colour.r, colour.g, colour.b, colour.a);
    SDL_RenderClear(context->renderer);
}

void SDL_DrawText(RenderContext *context, int x, int y, SDL_Color colour, const char *text)
{
    FC_DrawColor(context->font, context->renderer, x, y, colour, text);
}

void drawSplash(RenderContext *context)
{
    u32 ip = gethostid();
    char str_buf[300];
    snprintf(str_buf, 300, "Your Switch is now ready for a PC to connect!\nIt has the IP-Address %u.%u.%u.%u\n"
                           "\nInstructions can be found here:"
                           "\nhttps://bit.ly/2QrR1Lb",
             ip & 0xFF, (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, (ip >> 24) & 0xFF);

    SDL_Color black = {0, 0, 0, 255};
    SDL_Color white = {230, 230, 230, 255};
    SDL_ClearScreen(context, white);

    SDL_DrawText(context, 170, 150, black, str_buf);

    SDL_RenderPresent(context->renderer);
}

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

    if (++videoContext->video_frame_count % 60 == 0)
    {
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