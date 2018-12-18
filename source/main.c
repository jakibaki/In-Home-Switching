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

RenderContext* renderContext = NULL;

static const SocketInitConfig socketInitConf = {
    .bsdsockets_version = 1,

    .tcp_tx_buf_size = 0x8000,
    .tcp_rx_buf_size = 0x10000,
    .tcp_tx_buf_max_size = 0x40000,
    .tcp_rx_buf_max_size = 0x40000,

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

    if (R_FAILED(ret = pcvSetClockRate(PcvModule_Cpu, 1785000000)))
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

	return 0;
}

static void EnterServerAddress()
{
    OSK_Display("Enter Server Address", server_address);

	if (!strncmp(osk_buffer, "", 1))
		return;

    strcpy(server_address, osk_buffer);
}

static void ConnectToServer()
{
	if (!strncmp(server_address, "", 1))
		return;

    if (!isValidAddress(server_address))
        return;

    printf("Connecting to server...\n");

    static Thread inputHandlerThread;
    threadCreate(&inputHandlerThread, inputHandlerLoop, NULL, 0x1000, 0x2b, 0);
    threadStart(&inputHandlerThread);

    renderContext = createRenderer();
    VideoContext* videoContext = createVideoContext();
    videoContext->renderContext = renderContext;

    while (appletMainLoop())
    {
        handleVid(videoContext);
    }
    
    freeRenderer(renderContext);
    renderContext = NULL;
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
    strcpy(server_address, "192.168.0.117");

	TouchInfo touchInfo;
	Touch_Init(&touchInfo);

    int row = 0;

    while(appletMainLoop()) 
    {
		SDL_ClearScreen(BACKGROUND_COLOR);
		SDL_DrawRect(0, 60, 1280, 100, TITLE_BAR_COLOR);	// Title bar
		SDL_DrawImage(icon_app, 20, -18);  // Icon
		SDL_DrawText(400, 85, 50, TITLE_TEXT_COLOR, "In-Home-Switching"); // Title

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

        // Process input
		hidScanInput();
		Touch_Process(&touchInfo);
		u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
	
    	if ((kDown & KEY_DDOWN) || (kDown & KEY_LSTICK_DOWN) || (kDown & KEY_RSTICK_DOWN))
		    row++;
        else if ((kDown & KEY_DUP) || (kDown & KEY_LSTICK_UP) || (kDown & KEY_RSTICK_UP))
            row--;

        Utils_SetMax(&row, 0, 1);
		Utils_SetMin(&row, 1, 0);

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
        }
        else if (touchInfo.state == TouchEnded && touchInfo.tapType != TapNone) 
        {
            if (tapped_inside(touchInfo, serverAddressRect.x, serverAddressRect.y, serverAddressRect.x + serverAddressRect.width, serverAddressRect.y + serverAddressRect.height))
                EnterServerAddress();
            else if (tapped_inside(touchInfo, connectRect.x, connectRect.y, connectRect.x + connectRect.width, connectRect.y + connectRect.height))
                ConnectToServer();
        }

		SDL_RenderDisplay();

		if (kDown & KEY_PLUS)
			longjmp(exitJmp, 1);
	}

	Term_Services();
}
