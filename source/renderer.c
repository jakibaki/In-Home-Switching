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
    context->texture = NULL;
    return context;
}

void drawFrame(RenderContext* context, VideoContext* videoContext) 
{
    AVFrame* frame = videoContext->frame;

    if (context->texture == NULL)
    {
        // Allocate a texture onto which we can render the video frame
        context->texture = SDL_CreateTexture(
            SDL_GetMainRenderer(),
            SDL_PIXELFORMAT_YV12,
            SDL_TEXTUREACCESS_STATIC,
            frame->width,
            frame->height
        );
    }

    // Copy the frame onto the texture
    SDL_UpdateYUVTexture(
        context->texture, 
        NULL,
        frame->data[0],
        frame->linesize[0],
        frame->data[1],
        frame->linesize[1],
        frame->data[2],
        frame->linesize[2]
    );

    // Display the texture on the screen
    SDL_RenderCopy(SDL_GetMainRenderer(), context->texture, NULL, NULL);
    SDL_RenderDisplay();
}

void freeRenderer(RenderContext* context) 
{
    if (context->texture != NULL)
    {
        SDL_DestroyTexture(context->texture);
        context->texture = NULL;
    }

    free(context);
}