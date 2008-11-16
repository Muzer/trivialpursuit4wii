#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <wiiuse/wiiuse.h>

#include "GRRLIB.h"

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

int main(int argc, char **argv) {
	
	GRRLIB_InitVideo();
	GRRLIB_Start();	

	VIDEO_Init();
	WPAD_Init();

	int accel = 9.8;
	int velo = 100;
	int x = 100;
	int y = 100;
	int direction = 0;
	
	WPADData *wd;

	while(1) {
		GRRLIB_FillScreen(0xFF000000);
		GRRLIB_DrawRectangle(x, y, 100, 100, 0xFFFFFFFF, true);
		GRRLIB_Render();
		if(direction == 0) x++;
		if(direction == 1) x--;
		y = y + velo;
		velo = velo - accel;
		if(x<=300) direction = 0;
		if(x>300) direction = 1;
		wd = WPAD_Data(0);
		if(wd->ir.valid) {
			GRRLIB_DrawRectangle(wd->ir.x, wd->ir.y, 2, 2, 0xFF000000, true);
			if((wd->ir.x)<x+10&&(wd->ir.x)>x-10&&(wd->ir.y)<y+10&&(wd->ir.y)>y-10) velo = 100;
		} else {
			
		}
		WPAD_ScanPads();
		u32 wpaddown = WPAD_ButtonsDown(0);
		if(wpaddown & WPAD_BUTTON_HOME) break;
	}

	return 0;
}
