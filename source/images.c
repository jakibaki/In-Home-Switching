#include "common.h"
#include "sdl_helpers.h"
#include "images.h"

void Images_Load(void) 
{
	SDL_LoadImage(&icon_app, "romfs:/app.png");
	SDL_LoadImage(&icon_remove, "romfs:/outline_backspace_white_18dp.png");
	SDL_LoadImage(&icon_accept, "romfs:/outline_check_white_18dp.png");
	SDL_LoadImage(&icon_back, "romfs:/ic_arrow_back_normal.png");
}

void Images_Free(void) 
{
	SDL_DestroyTexture(icon_back);
	SDL_DestroyTexture(icon_accept);
	SDL_DestroyTexture(icon_remove);
	SDL_DestroyTexture(icon_app);
}