/*===========================================
        GRRLIB (GX version) 3.0 alpha
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
#include <fat.h>

#include "GRRLIB/GRRLIB.h"

Mtx GXmodelView2D;

int main(){
    VIDEO_Init();
    WPAD_Init();

    GRRLIB_InitVideo();
    GRRLIB_Start();

    while(1){
        WPAD_ScanPads();
        u32 wpaddown = WPAD_ButtonsDown(0);

        GRRLIB_FillScreen(0xFF000000);

        GRRLIB_Render();

        if (wpaddown & WPAD_BUTTON_A) exit(0);
    }
    return 0;
}
