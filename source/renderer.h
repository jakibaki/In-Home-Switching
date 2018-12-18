#ifndef _RENDERER_H
#define _RENDERER_H

#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include "context.h"

/* Allocates a render context */
RenderContext* createRenderer(void);

/* Handles a frame received from server */
void handleFrame(RenderContext* renderContext, VideoContext* videoContext);

/* Renders a frame recieved from server */
void displayFrame(RenderContext *context);

/* Deallocates the render context */
void freeRenderer(RenderContext* context);

/* Returns true if there is a video playing right now */
bool isVideoActive(RenderContext *context);

/* Sets the video-playing status */
void setVideoActive(RenderContext *context, bool active);

/* Sets the switch overclock status */
void applyOC(int *overclock_index, int diff);

#endif