/*
    sdl_helper: joel16/NX-Shell

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

#ifndef _SDL_HELPERS_H
#define _SDL_HELPERS_H

#include <switch.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h> 
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "sdl_fontcache.h"

#define WHITE                 FC_MakeColor(255, 255, 255, 255)
#define BLACK                 FC_MakeColor(0, 0, 0, 255)
#define BACKGROUND_COLOR      FC_MakeColor(48, 48, 48, 255)
#define TITLE_BAR_COLOR       FC_MakeColor(55, 71, 79, 255)
#define TITLE_TEXT_COLOR      FC_MakeColor(185, 185, 185, 255)
#define TEXT_COLOR            FC_MakeColor(236, 239, 241, 255)
#define SELECTION_COLOR       FC_MakeColor(77, 182, 172, 255)
#define TEXTBOX_COLOR         FC_MakeColor(200, 200, 200, 255)
#define TEXTBOX_TEXT_COLOR    FC_MakeColor(32, 32, 32, 255)
#define BUTTON_COLOR          FC_MakeColor(55, 71, 79, 255)
#define BUTTON_TEXT_COLOR     FC_MakeColor(200, 200, 200, 255)

typedef struct SDLRect
{
    u32 x;
    u32 y;
    u32 width;
    u32 height; 
} SDLRect;

SDL_Renderer *SDL_GetMainRenderer(void);
SDL_Window *SDL_GetMainWindow(void);
Result SDL_HelperInit(void);
void SDL_HelperTerm(void);
void SDL_ClearScreen(SDL_Color colour);
void SDL_DrawRect(int x, int y, int w, int h, SDL_Color colour);
void SDL_DrawRectR(SDLRect rect, int extent_x, int extent_y, SDL_Color colour);
void SDL_DrawCircle(int x, int y, int r, SDL_Color colour);
void SDL_DrawText(int x, int y, int size, SDL_Color colour, const char *text);
void SDL_DrawTextf(int x, int y, int size, SDL_Color colour, const char* text, ...);
void SDL_GetTextDimensions(int size, const char *text, u32 *width, u32 *height);
void SDL_LoadImage(SDL_Texture **texture, char *path);
void SDL_LoadImageBuf(SDL_Texture **texture, void *mem, int size);
void SDL_DrawImage(SDL_Texture *texture, int x, int y);
void SDL_DrawImageScale(SDL_Texture *texture, int x, int y, int w, int h);
void SDL_RenderDisplay(void);

#endif