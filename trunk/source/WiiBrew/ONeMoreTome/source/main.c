#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>
#include <wiiuse/wpad.h>


static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

int main(int argc, char **argv) {

	VIDEO_Init();
	WPAD_Init();
	
	switch(VIDEO_GetCurrentTvMode()) {
		case VI_NTSC:
			rmode = &TVNtsc480IntDf;
			break;
		case VI_PAL:
			rmode = &TVPal528IntDf;
			break;
		case VI_MPAL:
			rmode = &TVMpal480IntDf;
			break;
		default:
			rmode = &TVNtsc480IntDf;
			break;
	}

	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(xfb);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();


	printf("Hello World!\n");

	while(1) {

		VIDEO_WaitVSync();
		WPAD_ScanPads();
		u32 wpaddown = WPAD_ButtonsDown(0);
		if(wpaddown & WPAD_BUTTON_A) break;
		printf("tomr");
	}

	return 0;
}
