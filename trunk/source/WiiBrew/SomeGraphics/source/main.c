/*===========================================
 GRRLIB (GX version) 3.0.1 alpha
 Code     : NoNameNo
 GX hints : RedShade
 
 Template Code (Minimum Requirement)
 ============================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <ogcsys.h>
#include <gccore.h>
#include <wiiuse/wpad.h>

#include "GRRLIB.h"

Mtx GXmodelView2D;

int main(){
    VIDEO_Init();
    WPAD_Init();
	
    GRRLIB_InitVideo();
    GRRLIB_Start();
	
	int x = 0;
	int y = 0;
	
    while(1){
		WPAD_ScanPads();
		u32 wpaddown = WPAD_ButtonsDown(0);
		int random = rand();
		if(random<=0.25) x++;
		if(random<=0.5&&random>0.25) x--;
		if(random<=0.75&&random>0.5) y++;
		if(random<=1&&random>0.75) y--;	
		GRRLIB_PutPixel(x,y,0xFFFFFFFF);
		GRRLIB_Render();
		
		if(wpaddown & WPAD_BUTTON_A) exit(0);
    }
    return 0;
}