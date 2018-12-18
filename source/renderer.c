#include "renderer.h"

#include <switch.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include "sdl_helpers.h"
#include "video.h"

void flushSwapBuffers(void);

static int clock_rates[] = {
    333000000, 
    710000000, 
    1020000000, 
    1224000000, 
    1581000000, 
    1785000000
    };

RenderContext *createRenderer()
{
    RenderContext* context = (RenderContext*)malloc(sizeof(RenderContext));
    context->texture = NULL;
    context->frame = NULL;
    context->frame_mut = SDL_CreateMutex();
    context->video_active_mut = SDL_CreateMutex();
    context->video_active = false;
    context->overclock_status = 0;
    return context;
}

u64 old_time, new_time;
void handleFrame(RenderContext *renderContext, VideoContext *videoContext)
{
    SDL_LockMutex(renderContext->frame_mut);
    AVFrame *old_frame = renderContext->frame;
    renderContext->frame = av_frame_clone(videoContext->frame);
    if (old_frame != NULL)
        av_frame_free(&old_frame);
    SDL_UnlockMutex(renderContext->frame_mut);

    if (++videoContext->video_frame_count % 60 == 0)
    {
        new_time = svcGetSystemTick();
        printf("Framerate: %f\n", 60.0 / ((new_time - old_time) / 19200000.0));
        old_time = new_time;
    }
}

void displayFrame(RenderContext *context) 
{
    SDL_LockMutex(context->frame_mut);
    AVFrame *frame = context->frame;
    if (frame != NULL)
    {
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

        av_frame_free(&context->frame);
        context->frame = NULL;
    }
    SDL_UnlockMutex(context->frame_mut);
}

void freeRenderer(RenderContext *context) 
{
    if (context->texture != NULL)
    {
        SDL_DestroyTexture(context->texture);
        context->texture = NULL;
    }

    SDL_DestroyMutex(context->frame_mut);
    SDL_DestroyMutex(context->video_active_mut);

    free(context);
}

void applyOC(int *overclockIndex, int diff)
{
    if (diff == 0)
        return;
    
    int newIndex =  *overclockIndex + diff;
    if (newIndex < 0)
    {
        newIndex = 0;
    }
    else if (newIndex > sizeof(clock_rates) / sizeof(int) - 1)
    {
        newIndex = sizeof(clock_rates) / sizeof(int) - 1;
    }

    if (newIndex != *overclockIndex)
    {
        *overclockIndex = newIndex;
        pcvSetClockRate(PcvModule_Cpu, clock_rates[newIndex]);
    }
}

bool isVideoActive(RenderContext *context)
{
    bool ret;
    SDL_LockMutex(context->video_active_mut);
    ret = context->video_active;
    SDL_UnlockMutex(context->video_active_mut);
    return ret;
}

void setVideoActive(RenderContext *context, bool active)
{
    SDL_LockMutex(context->video_active_mut);
    context->video_active = active;
    SDL_UnlockMutex(context->video_active_mut);
}