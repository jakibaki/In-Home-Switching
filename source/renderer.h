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
void freeRenderer(RenderContext* context);

/* Sets the variable that indicates that there's a frame ready to be drawn */
void setFrameAvail(RenderContext* context);

/* Checks if a frame is ready to be drawn and sets that variable to false */

/* Returns true if there is a video playing right now */
bool isVideoActive(RenderContext *context);


#endif