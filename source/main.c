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

#include "resources/wiitriviaheadingimage.h"
#include "resources/cardimage.h"
#include "resources/stdfont.h"



#define POINTS2WIN 240;
#define BLACK 0xffff
#define WHITE 0x0000




Mtx GXmodelView2D;

u8 *tex_wth;
u8 *tex_ci;
u8 *tex_stdfont;

int videoInit = 0;

void initVideo(){
	if(videoInit!=1){

		VIDEO_Init();
		
		GRRLIB_InitVideo();
		GRRLIB_Start();
		
		tex_wth = GRRLIB_LoadTexture(wiitriviaheadingimage);
		tex_ci = GRRLIB_LoadTexture(cardimage);
		tex_stdfont = GRRLIB_LoadTexture(stdfont);
		
		videoInit = 1;
	}
}

void renderLogo(){
	initVideo();
	float rot=0;
	float alpha=255;
	GRRLIB_DrawImg(0, 0, 640, 200, tex_wth, rot, 1, 1, alpha);
}


int displayMenu(){
	int cursorPosition = 1;
	initVideo();
	while(true){
		WPAD_ScanPads();
		renderLogo();
		GRRLIB_Printf(100, 300, tex_stdfont, BLACK, 1, "Play Game:");
		GRRLIB_Printf(200, 350, tex_stdfont, BLACK, 1, "2 Teams");
		GRRLIB_Printf(200, 400, tex_stdfont, BLACK, 1, "3 Teams");
		GRRLIB_Printf(200, 450, tex_stdfont, BLACK, 1, "4 Teams");
		switch (cursorPosition) {
			case 1:
				GRRLIB_Printf(190, 350, tex_stdfont, BLACK, 1, ">>");
				break;
			case 2:
				GRRLIB_Printf(190, 400, tex_stdfont, BLACK, 1, ">>");
				break;
			case 3:
				GRRLIB_Printf(190, 450, tex_stdfont, BLACK, 1, ">>");
				break;
		}
		GRRLIB_Render();
		if((WPAD_ButtonsDown (0) & WPAD_BUTTON_UP)&&cursorPosition<3 )
			cursorPosition ++;
		if((WPAD_ButtonsDown (0) & WPAD_BUTTON_DOWN)&&cursorPosition>1)
			cursorPosition --;
		if(WPAD_ButtonsDown (0) & WPAD_BUTTON_A)
			break;
		VIDEO_WaitVSync();
		
	}
	return cursorPosition;
}

int displayQuestion(int * displayQuestion){
	initVideo();
	while(true){
		renderLogo();
		float rot=0;
		float alpha=255;
		GRRLIB_DrawImg(320, 250, 300, 400, tex_ci, rot, 1, 1, alpha);
	}
	
}

void playGame(int players, int question_pack){
	int playerpoints[players];
	int c=0;
	while(c<=players){
		playerpoints[c] = 0;
	}
	while(1){
		int points2earn;
		playerpoints[displayQuestion(&points2earn)] += points2earn;
	}
}

int main(int argc, char **argv) {
	int choice = displayMenu();
	playGame(choice, 0);
	return 0;
}



