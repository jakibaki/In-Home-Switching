#ifndef _VIDEO_H
#define _VIDEO_H

#include "context.h"

VideoContext* createVideoContext(void);
int handleVid(VideoContext* context);
void freeVideoContext(VideoContext* context);

#endif