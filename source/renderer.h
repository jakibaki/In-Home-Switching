#ifndef _RENDERER_H
#define _RENDERER_H

#include <libavformat/avformat.h>
#include <libswscale/swscale.h>


#include "context.h"

/* Allocates a render context */
RenderContext* createRenderer(void);

/* Draws an image filling all screen */
void drawSplash(RenderContext* context, const char* splashPath);

/* Renders a frame recieved from server */
void drawFrame(RenderContext* context, VideoContext* videoContext);

/* Deallocates the render context */
void freeRenderer(RenderContext* context);

/* Sets the variable that indicates that there's a frame ready to be drawn */
void setFrameAvail(RenderContext* context);

/* Checks if a frame is ready to be drawn and sets that variable to false */
bool checkFrameAvail(RenderContext* context);

#endif