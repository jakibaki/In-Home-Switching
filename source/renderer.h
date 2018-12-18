#ifndef _RENDERER_H
#define _RENDERER_H

#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#include "context.h"

/* Allocates a render context */
RenderContext* createRenderer(void);

/* Renders a frame recieved from server */
void drawFrame(RenderContext* context, VideoContext* videoContext);

/* Deallocates the render context */
void freeRenderer(RenderContext* context);

#endif