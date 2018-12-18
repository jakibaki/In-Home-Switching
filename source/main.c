// The following ffmpeg code is inspired by an official ffmpeg example, so here is its Copyright notice:

/*
 * Copyright (c) 2012 Stefano Sabatini
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.


*
 * @file
 * Demuxing and decoding example.
 *
 * Show how to use the libavformat and libavcodec API to demux and
 * decode audio and video data.
 * @example demuxing_decoding.c

*/

#include <libavutil/samplefmt.h>
#include <switch.h>
#include "common.h"
#include "sdl_helpers.h"
#include "touch_helpers.h"
#include "images.h"
#include "osk.h"
#include "utils.h"
#include "input.h"
#include "network.h"
#include "video.h"
#include "renderer.h"
#include "fs.h"
#include "config.h"

static char *clock_strings[] = {
    "333 MHz (underclocked, very slow)", 
    "710 MHz (underclocked, slow)", 
    "1020 MHz (standard, not overclocked)", 
    "1224 MHz (slightly overclocked)", 
    "1581 MHz (overclocked)", 
    "1785 MHz (strong overclock)"
    };

RenderContext* renderContext = NULL;

static const SocketInitConfig socketInitConf = {
    .bsdsockets_version = 1,

    .tcp_tx_buf_size = 0x80000,
    .tcp_rx_buf_size = 0x100000,
    .tcp_tx_buf_max_size = 0x400000,
    .tcp_rx_buf_max_size = 0x400000,

    .udp_tx_buf_size = 0xA2400,
    .udp_rx_buf_size = 0xAA500,

    .sb_efficiency = 4,

    .serialized_out_addrinfos_max_size = 0x1000,
    .serialized_out_hostent_max_size = 0x200,
    .bypass_nsd = false,
    .dns_timeout = 0,
};

static void Term_Services(void) 
{
    if (renderContext != NULL)
    {
        freeRenderer(renderContext);
        renderContext = NULL;
    }

    is_running = false;

	Images_Free();
	networkDestroy();

	nsExit();
	usbCommsExit();
    gfxExit();
	SDL_HelperTerm();
	romfsExit();
	psmExit();
	plExit();    
    pcvExit();
}

static Result Init_Services(void) 
{
	Result ret = 0;

    if (R_FAILED(ret = pcvInitialize()))
        return ret;

    if (R_FAILED(ret = plInitialize()))
        return ret;

    if (R_FAILED(ret = psmInitialize()))
        return ret;

    if (R_FAILED(ret = romfsInit()))
        return ret;

    gfxInitDefault();

    if (R_FAILED(ret = SDL_HelperInit()))
        return ret;

    networkInit(&socketInitConf);
	Images_Load();
    FS_Load();
    Config_Load();

	return 0;
}

void startInput()
{
    static Thread inputHandlerThread;
    threadCreate(&inputHandlerThread, inputHandlerLoop, NULL, 0x1000, 0x2b, 1);
    threadStart(&inputHandlerThread);
}

void startRender(VideoContext *videoContext)
{
    static Thread renderThread;
    threadCreate(&renderThread, videoLoop, videoContext, 0x1000000, 0x2b, 2);
    threadStart(&renderThread);
}

static void DrawTitle()
{
    SDL_ClearScreen(BACKGROUND_COLOR);
    SDL_DrawRect(0, 60, 1280, 100, TITLE_BAR_COLOR);	// Title bar
    SDL_DrawImage(icon_app, 20, -18);  // Icon
    SDL_DrawText(400, 85, 50, TITLE_TEXT_COLOR, "In-Home-Switching"); // Title
}

static void EnterServerAddress()
{
    OSK_Display("Enter Server Address", server_address);

	if (!strncmp(osk_buffer, "", 1))
		return;

    strcpy(server_address, osk_buffer);

    strcpy(config.server_address, server_address);
    Config_Save(config);
}

static void ConnectToServer()
{
	if (!strncmp(server_address, "", 1))
		return;

    if (!isValidAddress(server_address))
        return;

    renderContext = createRenderer();
    VideoContext* videoContext = createVideoContext();
    videoContext->renderContext = renderContext;
    
    startInput();
    startRender(videoContext);

    bool foundConnection = false;

    while (appletMainLoop())
    {
        if (isVideoActive(renderContext))
        {
            // Connected...
            if (!foundConnection)
            {
                printf("Connected to server\n");
                foundConnection = true;
            }
            displayFrame(renderContext);
        }
        else if (!foundConnection)
        {
            // Connecting...
            printf("Connecting to server...\n");
            DrawTitle();

            // Connecting text
            u32 con_width = 0, con_height = 0;
            SDL_GetTextDimensions(30, "Connecting...", &con_width, &con_height);
            SDL_DrawText(620 - con_width / 2, 
                        360 - con_height / 2, 
                        30, TEXT_COLOR, "Connecting...");

            SDL_RenderDisplay();
        }
        else
        {
            // Lost connection...
            printf("Lost connection to server...\n");

            // Reconnecting message
            SDLRect connectRect = { 0 };
            SDL_GetTextDimensions(30, "Reconnecting...", &connectRect.width, &connectRect.height); 
            connectRect.x = (640 - connectRect.width / 2); 
            connectRect.y = (360 - connectRect.height / 2);
            SDL_DrawRectR(connectRect, 30, 20, TITLE_BAR_COLOR);
            SDL_DrawText(connectRect.x, connectRect.y, 30, TEXT_COLOR, "Reconnecting...");

            SDL_RenderDisplay();

            // Todo: Quit all theads, clean up, and go back to main screen after some timeout
        }
    }

    is_running = false;
    
    freeVideoContext(videoContext);
    freeRenderer(renderContext);
    renderContext = NULL;
}

static void ChangeOverClock(int diff)
{
    applyOC(&config.overclock_index, diff);
    Config_Save(config);
}

int main(int argc, char **argv)
{
	Init_Services();

	if (setjmp(exitJmp)) 
    {
		Term_Services();
		return 0;
	}

    server_address[0] = '\0';
    strcpy(server_address, config.server_address);

	TouchInfo touchInfo;
	Touch_Init(&touchInfo);

    int row = 0;

    while(appletMainLoop()) 
    {
		DrawTitle();

        // Server address textbox
        SDLRect serverAddressRect = { 640 - 250, 350, 500, 80 };        
        if (row == 0) // Draw selection highlight
		    SDL_DrawRectR(serverAddressRect, 5, 5, SELECTION_COLOR);
        SDL_DrawRectR(serverAddressRect, 0, 0, TEXTBOX_COLOR);
        u32 ip_width = 0, ip_height = 0;
        SDL_GetTextDimensions(30, server_address, &ip_width, &ip_height);
        SDL_DrawText(serverAddressRect.x + serverAddressRect.width / 2 - ip_width / 2, 
                     serverAddressRect.y + serverAddressRect.height / 2 - ip_height / 2, 
                     30, TEXTBOX_TEXT_COLOR, server_address);

        // Connect button
        SDLRect connectRect = { 0 };
        SDL_GetTextDimensions(30, "Connect", &connectRect.width, &connectRect.height); 
        connectRect.x = (640 - connectRect.width / 2); 
        connectRect.y = (605 - connectRect.height / 2);
        if (row == 1) // Draw selection highlight
		    SDL_DrawRectR(connectRect, 35, 25, SELECTION_COLOR);
        SDL_DrawRectR(connectRect, 30, 20, BUTTON_COLOR);
	    SDL_DrawText(connectRect.x, connectRect.y, 30, BUTTON_TEXT_COLOR, "Connect");

        // Overclock
        SDL_DrawImage(icon_timer, 130, 200);
        SDLRect overclockDownRect = { 90, 200, 36, 36 }; 
        if (row == 2) // Draw selection highlight
		    SDL_DrawRectR(overclockDownRect, 3, 3, SELECTION_COLOR);
        SDL_DrawImage(icon_down, overclockDownRect.x, overclockDownRect.y);

        SDLRect overclockUpRect = { 170, 200, 36, 36 }; 
        if (row == 3) // Draw selection highlight
		    SDL_DrawRectR(overclockUpRect, 3, 3, SELECTION_COLOR);
        SDL_DrawImage(icon_up, overclockUpRect.x, overclockUpRect.y);

        SDLRect overclockTextRect = { 0 };
        SDL_GetTextDimensions(10, clock_strings[config.overclock_index], &overclockTextRect.width, &overclockTextRect.height); 
        overclockTextRect.x = (148 - overclockTextRect.width / 2); 
        overclockTextRect.y = (260 - overclockTextRect.height / 2);
	    SDL_DrawText(overclockTextRect.x, overclockTextRect.y, 10, TEXT_COLOR, clock_strings[config.overclock_index]);

        // Process input
		hidScanInput();
		Touch_Process(&touchInfo);
		u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
	
    	if ((kDown & KEY_DDOWN) || (kDown & KEY_LSTICK_DOWN) || (kDown & KEY_RSTICK_DOWN))
		    row++;
        else if ((kDown & KEY_DUP) || (kDown & KEY_LSTICK_UP) || (kDown & KEY_RSTICK_UP))
            row--;

        Utils_SetMax(&row, 0, 3);
		Utils_SetMin(&row, 3, 0);

        if (kDown & KEY_A) 
        {
            if (row == 0)
                EnterServerAddress();
            else if (row == 1)
                ConnectToServer();
        }

        if (touchInfo.state == TouchStart)
        {
            if (tapped_inside(touchInfo, serverAddressRect.x, serverAddressRect.y, serverAddressRect.x + serverAddressRect.width, serverAddressRect.y + serverAddressRect.height))
                row = 0;
            else if (tapped_inside(touchInfo, connectRect.x, connectRect.y, connectRect.x + connectRect.width, connectRect.y + connectRect.height))
                row = 1;
            else if (tapped_inside(touchInfo, overclockDownRect.x, overclockDownRect.y, overclockDownRect.x + overclockDownRect.width, overclockDownRect.y + overclockDownRect.height))
                row = 2;
            else if (tapped_inside(touchInfo, overclockUpRect.x, overclockUpRect.y, overclockUpRect.x + overclockUpRect.width, overclockUpRect.y + overclockUpRect.height))
                row = 3;
        }
        else if (touchInfo.state == TouchEnded && touchInfo.tapType != TapNone) 
        {
            if (tapped_inside(touchInfo, serverAddressRect.x, serverAddressRect.y, serverAddressRect.x + serverAddressRect.width, serverAddressRect.y + serverAddressRect.height))
                EnterServerAddress();
            else if (tapped_inside(touchInfo, connectRect.x, connectRect.y, connectRect.x + connectRect.width, connectRect.y + connectRect.height))
                ConnectToServer();
            else if (tapped_inside(touchInfo, overclockDownRect.x, overclockDownRect.y, overclockDownRect.x + overclockDownRect.width, overclockDownRect.y + overclockDownRect.height))
                ChangeOverClock(-1);
            else if (tapped_inside(touchInfo, overclockUpRect.x, overclockUpRect.y, overclockUpRect.x + overclockUpRect.width, overclockUpRect.y + overclockUpRect.height))
                ChangeOverClock(1);
        }

		SDL_RenderDisplay();

		if (kDown & KEY_PLUS)
			longjmp(exitJmp, 1);
	}

	Term_Services();
    return 0;
}
