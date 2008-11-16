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
	
	GRRLIB_buffer = (u16 *)malloc(640*480*2);
	
	VIDEO_Init();
	WPAD_Init();
	WPAD_SetIdleTimeout(60);
	
	GRRLIB_InitVideo();
	GRRLIB_Start();	
	VIDEO_WaitVSync();
	
	WPAD_SetDataFormat(0,WPAD_FMT_BTNS_ACC_IR);
	WPAD_SetVRes(0,640,480);
	WPAD_SetIdleTimeout(60);
	
	WPAD_ScanPads();
	
	ir_t ir;
	
	
	
	GRRLIB_FillScreen(BLACK);
	
	while(1){
		WPAD_IR(0, &ir);
		GRRLIB_FillScreen(BLACK);
		GRRLIB_Plot(ir.x, ir.y, WHITE);
		GRRLIB_Plot(ir.x+1, ir.y+1, WHITE);
		GRRLIB_Plot(ir.x+1, ir.y, WHITE);
		GRRLIB_Plot(ir.x, ir.y+1, WHITE);
		GRRLIB_Plot(ir.x, ir.y, WHITE);
		GRRLIB_Plot(ir.x+2, ir.y+2, WHITE);
		GRRLIB_Plot(ir.x+2, ir.y, WHITE);
		GRRLIB_Plot(ir.x, ir.y+2, WHITE);
		GRRLIB_Render();
		VIDEO_WaitVSync();
		WPAD_ScanPads();
		if(WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)
			break;
	}
	return 0;
	
}
