#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include <ogcsys.h>
#include <gccore.h>
#include <wiiuse/wpad.h>

#include <time.h>

#include <unistd.h>

#include "impact.h"
#include "GRRLIB.h"
extern u16 *GRRLIB_buffer;

#define GREEN 0x00ff00
#define RED 0xff0000
#define BLUE 0x0000ff

#define RAND_MAX 100


Mtx GXmodelView2D;
extern u16 *GRRLIB_buffer;

void PrintFloat1(int x, int y, int xsize, int ysize, int value, const u16 tdata[]){
	char text[10];
	sprintf(text, "%d", value);
	GRRLIB_ImgPrint(x, y, xsize, ysize, text, tdata);
}

int main(int argc, char **argv) {
	GRRLIB_buffer=(u16 *)malloc(640*480*2);
	

	sleep(2);
	
	ir_t  ir;

	sleep(2);
	VIDEO_Init();

	
	GRRLIB_InitVideo();
	GRRLIB_Start();
	
	GRRLIB_FillScreen(0x0000);
	GRRLIB_Render();
	
	
	VIDEO_WaitVSync();

	
	
	WPAD_Init();
	WPAD_SetDataFormat(0,WPAD_FMT_BTNS_ACC_IR);
	WPAD_SetVRes(0,640,480);
	
	WPAD_ScanPads();
	


	int fallingObjectsXPOS[3];
	int fallingObjectsYVELOCITY[3];
	int fallingObjectsYPOS[3];
	
	int c2 = 0;
	
	while(c2<=3){
		fallingObjectsXPOS[c2] = (int)rand()*6.39;
		fallingObjectsYVELOCITY[c2] = (int)rand()/10;
		fallingObjectsYPOS[c2] = (int)rand()*4.79;
	}
	
	
	int score = 0;
	
	while(1){
		while(1) {
			
			WPAD_ScanPads();
			GRRLIB_FillScreen(0x0000);
			GRRLIB_ImgPrint(208,410,16,16,"CATCH'EM!",impact_img);
			GRRLIB_ImgPrint(200,430,16,16,"BY pineconecs",impact_img);
			
			WPAD_IR(0, &ir);
			
			
			GRRLIB_DrawRectangle(ir.x, 450, 100, 500, GREEN, 1);
			
			int c = 0;
			while(c<=3){
				if(fallingObjectsYPOS[c]>470) break;
				if(fallingObjectsYPOS[c]<=120&&(fallingObjectsXPOS[c]>ir.x-500&&fallingObjectsXPOS[c]<ir.x+500)){
					fallingObjectsXPOS[c] = rand() % 640 + 1;
					fallingObjectsYPOS[c] = 0;
					fallingObjectsYVELOCITY[c] = 0;
					score ++;
					GRRLIB_DrawRectangle(ir.x, 450, 100, 500, RED, 1);
				}
				fallingObjectsYVELOCITY[c] ++;
				fallingObjectsYPOS[c] += fallingObjectsYVELOCITY[c];
				
				GRRLIB_DrawRectangle(fallingObjectsXPOS[c], fallingObjectsYPOS[c], 20, 20, BLUE, 1);
				
				c++;
			}
			GRRLIB_Render();
			VIDEO_WaitVSync();
			
		}
		
		while(1){
			
			WPAD_ScanPads();
			u32 held = WPAD_ButtonsHeld(0);
			
			if ( held & WPAD_BUTTON_A ){
				break;
			}
			if(held & WPAD_BUTTON_HOME){
				return 0;
			}
			GRRLIB_ImgPrint(208,410,16,16,"GAMEOVER! YOUR SCORE:",impact_img);
			PrintFloat1(200,430,16,16, score,impact_img);
			GRRLIB_ImgPrint(208,450,16,16,"Press A to play again. HOME to quit.",impact_img);
		}
	}
	return 0;
}


