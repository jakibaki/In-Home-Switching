#ifndef _RENDERER_H
#define _RENDERER_H

#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#include "context.h"

RenderContext* createRenderer(void);
void drawSplash(RenderContext* context, const char* splashPath);
void drawFrame(RenderContext* context, VideoContext* videoContext);
void freeRenderer(RenderContext* context);

#endif