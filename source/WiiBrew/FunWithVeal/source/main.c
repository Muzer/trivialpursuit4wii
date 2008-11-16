#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>

#include <wiiuse/wpad.h>

#include "GRRLIB.h"

#define BLACK 0xffff
#define WHITE 0x0000

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;
extern u16 *GRRLIB_buffer;

int main(int argc, char **argv) {
	
	GRRLIB_buffer=(u16 *)malloc(640*480*2);
	
	VIDEO_Init();
	WPAD_Init();
	WPAD_SetIdleTimeout(60);
	
	GRRLIB_InitVideo();
	GRRLIB_Start();	
	VIDEO_WaitVSync();
	
	int freq = 10;
	
	int col = WHITE;
	
	
	while(1){
		
		WPAD_ScanPads();
		
		GRRLIB_Render();
		if(WPAD_ButtonsDown (0) & WPAD_BUTTON_DOWN)
			freq += 1;
		if(WPAD_ButtonsDown (0) & WPAD_BUTTON_UP)
			freq -= 1;
		if(WPAD_ButtonsDown (0) & WPAD_BUTTON_HOME)
			return 0;
		if(WPAD_ButtonsDown (0) & WPAD_BUTTON_HOME)
			break;
		
		GRRLIB_FillScreen(col);
		
		if(col==WHITE)
			col = BLACK;
		else
			col = WHITE;
		
		int c = 0;
		
		while(c <= freq){
			GRRLIB_Render();
			VIDEO_WaitVSync();
			c++;
		}
		
	}
	
}
