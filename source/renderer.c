#include "renderer.h"

#include <switch.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include "sdl_helpers.h"
#include "video.h"

void flushSwapBuffers(void);

RenderContext* createRenderer()
{
    RenderContext* context = (RenderContext*)malloc(sizeof(RenderContext));
    context->ctx_sws = NULL;
    context->texture = NULL;
    context->data = NULL;
    return context;
}

void drawFrame(RenderContext* context, VideoContext* videoContext) 
{
    AVFrame* frame = videoContext->frame;
    AVFrame* rgbframe = videoContext->rgbframe;

    if (context->ctx_sws == NULL)
    {
        // Allocate a place to put our YUV image on that screen
        context->texture = SDL_CreateTexture(
            SDL_GetMainRenderer(),
            SDL_PIXELFORMAT_RGBA32,
            SDL_TEXTUREACCESS_STREAMING,
            frame->width,
            frame->height
        );

        // Initialize SWS context for software scaling
        enum AVPixelFormat pix_fmt = videoContext->video_dec_ctx->pix_fmt;
        context->ctx_sws = sws_getContext(
            frame->width,
            frame->height,
            pix_fmt,
            frame->width,
            frame->height,
            AV_PIX_FMT_RGBA,
            SWS_BILINEAR,
            NULL,
            NULL,
            NULL);

        // Create the pixel buffer
        context->data = (uint8_t*)malloc(frame->width * frame->height * 4);
    }

    int pitch;
    SDL_LockTexture(context->texture, NULL, (void**)&context->data, &pitch);

	// Convert the image into format that SDL uses
	sws_scale( 
        context->ctx_sws, 
        (uint8_t const * const *)frame->data,
        frame->linesize, 
        0, 
        frame->height,
        (uint8_t* const*)&context->data, 
        rgbframe->linesize);

    SDL_UnlockTexture(context->texture);

    // Display the texture on the screen
    SDL_Renderer* renderer = SDL_GetMainRenderer();
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, context->texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void flushSwapBuffers() 
{
    gfxFlushBuffers();
    gfxSwapBuffers();
}

void freeRenderer(RenderContext* context) {
    free(context);
}