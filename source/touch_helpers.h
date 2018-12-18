/*
    touch_helpers: joel16/NX-Shell

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    Modifications by CodeMooseUS
*/

#ifndef _TOUCH_HELPERS_H
#define _TOUCH_HELPERS_H

#include <switch.h>
#include <time.h>

#define tapped_inside(touchInfo, x1, y1, x2, y2) (touchInfo.firstTouch.px >= x1 && touchInfo.firstTouch.px <= x2 && touchInfo.firstTouch.py >= y1 && touchInfo.firstTouch.py <= y2)
#define tapped_outside(touchInfo, x1, y1, x2, y2) (touchInfo.firstTouch.px < x1 || touchInfo.firstTouch.px > x2 || touchInfo.firstTouch.py < y1 || touchInfo.firstTouch.py > y2)

typedef enum TouchState 
{
    TouchNone,
    TouchStart,
    TouchMoving,
    TouchEnded
} TouchState;

typedef enum TapType 
{
    TapNone,
    TapShort,
    TapLong
} TapType;

typedef struct TouchInfo 
{
    TouchState state;
    touchPosition firstTouch;
    touchPosition prevTouch;
    TapType tapType;
    u64 touchStart;
} TouchInfo;

void Touch_Init(TouchInfo * touchInfo);
void Touch_Process(TouchInfo * touchInfo);

#endif