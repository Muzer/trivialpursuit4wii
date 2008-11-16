#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <gcmodplay.h> 

#include "GRRLIB.h"

//Font used by GRRLIB_Print function
#include "fonts/font5.h"

//Logo from the Main menu
#include "img/logo.h"

//Number images for the score
#include "num/zero.h"
#include "num/one.h"
#include "num/two.h"
#include "num/three.h"
#include "num/four.h"
#include "num/five.h"
#include "num/six.h"
#include "num/seven.h"
#include "num/eight.h"
#include "num/nine.h"

//Sound files, converted to *.h(*.o) with the last part of the Makefile
#include "bar_bounce_mod.h"
#include "bounce_mod.h"

static MODPlay snd;

extern u16 *GRRLIB_buffer;

int num_players = 1; //Numbers of players. If set to 1, CPU will control the right bar. If set to 2, Wiimote 2 will control the right bar. By default, set to 1.

s32 CPU_dif = 2; //This is the speed(in pixels) the CPU will move the bar.

int P1_Score = 0; //Declare the Player 1 score
int P2_Score = 0; //Declare the Player 2 score

const int score_high=24; //Declare the score image height
const int score_width=17; //Declare the score image width

int game_mode = 0; //0 = Title Screen; 1 = Game; 2 = Info screen; 3 = Pause Screen

int title_selected = 0; //Declare the "Main menu initial selected option" variable
int title_position = 170; //Declare the "Main menu initial selected position" variable
int pause_selected = 0; //Declare the "Pause screen initial selected option" variable
int pause_position = 170; //Declare the "Pause screen initial selected position" variable
int players_selected = 0; //Declare the "Players screen initial selected option" variable
int players_position = 170; //Declare the "Players screen initial selected position" variable

int main(){

	s32 BallX = 311; //Ball X position.
	s32 BallY = 190; //Ball Y position. 
	s32 BallSpeedX =  rand() % 4 + 3; //Ball horitzontal speed. Changes when contacts borders or Bars.
	s32 BallSpeedY = rand() % -3 + -2; //Ball vertical speed. Changes when contacts borders or Bars.

	s32 LBarX = 40; //Left Bar X position(Player 1)
	s32 LBarY = 190; //Left Bar Y position(Player 1)

	s32 RBarX = 600; //Right Bar X position(Player 2)
	s32 RBarY = 190; //Right Bar Y position(Player 2)

	GRRLIB_buffer=(u16 *)malloc(640*480*2);

	VIDEO_Init();
	WPAD_Init();
	WPAD_SetIdleTimeout(60); //Wiimote is shutdown after 60 seconds of innactivity.
	
	MODPlay_Init(&snd);

	GRRLIB_InitVideo();
	GRRLIB_Start();	
	VIDEO_WaitVSync();
	
	void Restart() //This restarts all the game(Ball position, speed/direction, Bar position and Players score)
	{
		BallX = 311;
		BallY = 190;
		BallSpeedX =  rand() % 5 + 4;
		BallSpeedY =  rand() % 3 + 2;
		LBarX = 40; 
		LBarY = 190;
		RBarX = 600; 
		RBarY = 190;
		P1_Score = 0;
		P2_Score = 0;
	}
	
	while(1)
	{
		if(game_mode == 0) //Main menu screen
		{	
			while(game_mode == 0)
				{
					
					
					GRRLIB_Render();
					WPAD_ScanPads ();
					GRRLIB_FillScreen(0x0000);
					GRRLIB_DrawImg(212, 50, logo_width, logo_high, logo_img, 0, 1); //Draw the Logo image from "#include "img/logo.h""
					GRRLIB_Print(248,110,font5_char_width,font5_char_high,"Wii Version 1.1",font5,0xFFFF,0x0000);

					/* Draw 2 squares, so the lines show nice on a old TV. */
					GRRLIB_DrawRectangle(39,159,97,262,0x2104,1);
					GRRLIB_DrawRectangle(39,159,97,262,0xFFFF,0);
					GRRLIB_DrawRectangle(40,160,95,260,0xFFFF,0);
					
					GRRLIB_Print(70,170,font5_char_width,font5_char_high,"New game",font5,0xFFFF,0x2104);
					GRRLIB_Print(70,190,font5_char_width,font5_char_high,"Information",font5,0xFFFF,0x2104);
					GRRLIB_Print(70,210,font5_char_width,font5_char_high,"Online Mode",font5,0x4228,0x2104);
					GRRLIB_Print(70,230,font5_char_width,font5_char_high,"Exit",font5,0xFFFF,0x2104);
					
					GRRLIB_Print(243,440,font5_char_width,font5_char_high,"Coded by JPaRaDoX",font5,0x0544,0x0000);
					
					GRRLIB_Print(50,title_position,font5_char_width,font5_char_high,">",font5,0xFFFF,0x2104); //Print the pointer(">") at the position("title_position"). It changes with the next 2 lines.

					/* Define the selected option, and the pointer(">") position to print */
					if ((WPAD_ButtonsDown (0) & WPAD_BUTTON_DOWN) && (title_selected < 3)) 
						{
							title_selected += 1;
							title_position += 20;
							MODPlay_Stop(&snd);
						}
						
					if ((WPAD_ButtonsDown (0) & WPAD_BUTTON_UP) && (title_selected > 0))
						{
							title_selected += -1;
							title_position += -20;
							MODPlay_Stop(&snd);
						}
					
					if ((WPAD_ButtonsDown (0) & WPAD_BUTTON_A) && (title_selected == 0)) //Get the selected option, and reset the position of the pointer(">")
						{ 
							Restart();
							game_mode = 4;
						}
					
					if ((WPAD_ButtonsDown (0) & WPAD_BUTTON_A) && (title_selected == 1)) //Get the selected option, and reset the position of the pointer(">")
						{ 
							game_mode = 2;
						}
					
					if ((WPAD_ButtonsDown (0) & WPAD_BUTTON_A) && (title_selected == 2)) //Get the selected option, and reset the position of the pointer(">")
						{ 
							MODPlay_SetMOD(&snd, bounce_mod);
							MODPlay_Start(&snd);
						}
					
					if ((WPAD_ButtonsDown (0) & WPAD_BUTTON_A) && (title_selected == 3)) //Get the selected option, and reset the position of the pointer(">")
						{ 
							exit(0);
						}
					
					if (WPAD_ButtonsDown (0) & WPAD_BUTTON_B) //Get the selected option, and reset the position of the pointer(">")
						{ 
							title_selected = 3;
							title_position = 230;
						}
					
					if (WPAD_ButtonsDown (0) & WPAD_BUTTON_HOME) //Get the selected option, and reset the position of the pointer(">")
						{
							title_selected = 3;
							title_position = 230;
						}
					
					VIDEO_WaitVSync();
				}
		}
	
		if(game_mode == 1) //Main game
		{
			while(game_mode == 1)
			{
				
				WPAD_ScanPads ();
				GRRLIB_Render();

				//------------------------------------------------------------------------------------------------------------------------------------------//
				//------------------------------------------------------------------------------------------------------------------------------------------//

				GRRLIB_FillScreen(0x0000); //Fill the screen with 0x0000(black).

				GRRLIB_Line(320,40,320,400,0x1111); // Central line, made from 3 lines
				GRRLIB_Line(319,40,319,400,0x1111);
				GRRLIB_Line(321,40,321,400,0x1111);

				GRRLIB_Line(25,40,620,40,0xFFFF); // Top border, made from 3 lines
				GRRLIB_Line(26,41,619,41,0xFFFF);
				GRRLIB_Line(27,42,618,42,0xFFFF);

				GRRLIB_Line(27,400,618,400,0xFFFF); // Bottom border, made from 3 lines
				GRRLIB_Line(26,401,619,401,0xFFFF);
				GRRLIB_Line(25,402,620,402,0xFFFF);

				GRRLIB_Print(BallX,BallY,font5_char_width,font5_char_high,"  ",font5,0x0000,0xFFFF); //Print the "image" of the ball. It's just 2 spaces of text, and the X and Y position change to make the movement.

				GRRLIB_Print(LBarX,LBarY,font5_char_width,font5_char_high," ",font5,0x0000,0xFFFF); //Left Bar "image". This is just 1 space of text.
				GRRLIB_Print(LBarX,LBarY+font5_char_high,font5_char_width,font5_char_high," ",font5,0x0000,0xFFFF);
				GRRLIB_Print(LBarX,LBarY+(font5_char_high*2),font5_char_width,font5_char_high," ",font5,0x0000,0xFFFF);
				GRRLIB_Print(LBarX,LBarY+(font5_char_high*3),font5_char_width,font5_char_high," ",font5,0x0000,0xFFFF);

				GRRLIB_Print(RBarX,RBarY,font5_char_width,font5_char_high," ",font5,0x0000,0xFFFF); //Right Bar "image". This is just 1 space of text.
				GRRLIB_Print(RBarX,RBarY+font5_char_high,font5_char_width,font5_char_high," ",font5,0x0000,0xFFFF);
				GRRLIB_Print(RBarX,RBarY+(font5_char_high*2),font5_char_width,font5_char_high," ",font5,0x0000,0xFFFF);
				GRRLIB_Print(RBarX,RBarY+(font5_char_high*3),font5_char_width,font5_char_high," ",font5,0x0000,0xFFFF);

				//------------------------------------------------------------------------------------------------------------------------------------------//
				//------------------------------------------------------------------------------------------------------------------------------------------//

				if ((WPAD_ButtonsHeld(0) & WPAD_BUTTON_UP) && LBarY >= 50) LBarY += -5; //If Wiimote 0(Player 1) is holding UP button, take away 5 from the Left Bar Y position. This is the speed it moves at.
				if ((WPAD_ButtonsHeld(0) & WPAD_BUTTON_DOWN) && LBarY <=334) LBarY += 5; //If Wiimote 0(Player 1) is holding DOWN button, add 5 to the Left Bar Y position. This is the speed it moves at.
				
				if (num_players == 1) //Let the CPU control the right Bar
					{
						if (RBarY > BallY) RBarY += - CPU_dif; 
						if ((RBarY < BallY) && (RBarY < 335)) RBarY += CPU_dif; 
					}
				
				if (num_players == 2) //Let Wiimote(1) control the right Bar
					{
						if ((WPAD_ButtonsHeld(1) & WPAD_BUTTON_UP) && RBarY >= 50) RBarY += -5; //If Wiimote 1(Player 2) is holding UP button, take away 5 from the Right Bar Y position. This is the speed it moves at.
						if ((WPAD_ButtonsHeld(1) & WPAD_BUTTON_DOWN) && RBarY <=334) RBarY += 5; //If Wiimote 1(Player 2) is holding DOWN button, add 5 to the Right Bar Y position. This is the speed it moves at.
					}

				if(BallY >= 388) //If the Ball Y position is bigger or equal to 388(bottom border), invert the speed and direction. This simulates a bounce off the border.
					{
						BallSpeedY = rand() % -5 + -4;
						MODPlay_SetMOD(&snd, bounce_mod);
						MODPlay_Start(&snd);
					}
					
				if(BallY <= 40)//If the Ball Y position is smaller or equal to 40(top border), invert the speed and direction. This simulates a bounce off the border.
					{
						BallSpeedY = rand() % 4 + 3;
						MODPlay_SetMOD(&snd, bounce_mod);
						MODPlay_Start(&snd);
					}

				//Set the new speed/direction of the ball.
				BallX += BallSpeedX; 
				BallY += BallSpeedY;

				/* Correct collision and bouncing, thanks to zosh from GBATemp! */
				if (BallX <= LBarX + font5_char_width && BallX >= LBarX && BallY > LBarY - font5_char_high && BallY < LBarY + font5_char_high * 5) 
					{ 
						BallSpeedX = rand() % 5 + 4;
						MODPlay_SetMOD(&snd, bar_bounce_mod);
						MODPlay_Start(&snd);
					}
					
				if (BallX + font5_char_width * 2 >= RBarX && BallX + font5_char_width <= RBarX && BallY > RBarY - font5_char_high && BallY < RBarY + font5_char_high * 5)
					{
						BallSpeedX = rand() % -4 + -5;
						MODPlay_SetMOD(&snd, bar_bounce_mod);
						MODPlay_Start(&snd);
					}

				

				//------------------------------------------------------------------------------------------------------------------------------------------//
				//------------------------------------------------------------------------------------------------------------------------------------------//

				/* Print the text on the main screen */
				GRRLIB_Print(30,415,font5_char_width,font5_char_high,"Player 1",font5,0xFFFF,0x0000);
				
				if(num_players == 1) GRRLIB_Print(590,415,font5_char_width,font5_char_high,"CPU",font5,0xFFFF,0x0000);
				if(num_players == 2) GRRLIB_Print(545,415,font5_char_width,font5_char_high,"Player 2",font5,0xFFFF,0x0000);
				GRRLIB_Print(272,424,font5_char_width,font5_char_high," - SCORE - ",font5,0xFFFF,0x0000);

				/* Draw the score for Player 1 */
				if(P1_Score == 0) GRRLIB_DrawImg(250,420,score_width,score_high,zero_img,0,1);
				else if(P1_Score == 1) GRRLIB_DrawImg(250,420,score_width,score_high,one_img,0,1);
				else if(P1_Score == 2) GRRLIB_DrawImg(250,420,score_width,score_high,two_img,0,1);
				else if(P1_Score == 3) GRRLIB_DrawImg(250,420,score_width,score_high,three_img,0,1);
				else if(P1_Score == 4) GRRLIB_DrawImg(250,420,score_width,score_high,four_img,0,1);
				else if(P1_Score == 5) GRRLIB_DrawImg(250,420,score_width,score_high,five_img,0,1);
				else if(P1_Score == 6) GRRLIB_DrawImg(250,420,score_width,score_high,six_img,0,1);
				else if(P1_Score == 7) GRRLIB_DrawImg(250,420,score_width,score_high,seven_img,0,1);
				else if(P1_Score == 8) GRRLIB_DrawImg(250,420,score_width,score_high,eight_img,0,1);
				else GRRLIB_DrawImg(250,420,score_width,score_high,nine_img,0,1);

				/* Draw the score for Player 2 */
				if(P2_Score == 0) GRRLIB_DrawImg(375,420,score_width,score_high,zero_img,0,1);
				else if(P2_Score == 1) GRRLIB_DrawImg(375,420,score_width,score_high,one_img,0,1);
				else if(P2_Score == 2) GRRLIB_DrawImg(375,420,score_width,score_high,two_img,0,1);
				else if(P2_Score == 3) GRRLIB_DrawImg(375,420,score_width,score_high,three_img,0,1);
				else if(P2_Score == 4) GRRLIB_DrawImg(375,420,score_width,score_high,four_img,0,1);
				else if(P2_Score == 5) GRRLIB_DrawImg(375,420,score_width,score_high,five_img,0,1);
				else if(P2_Score == 6) GRRLIB_DrawImg(375,420,score_width,score_high,six_img,0,1);
				else if(P2_Score == 7) GRRLIB_DrawImg(375,420,score_width,score_high,seven_img,0,1);
				else if(P2_Score == 8) GRRLIB_DrawImg(375,420,score_width,score_high,eight_img,0,1);
				else GRRLIB_DrawImg(375,420,score_width,score_high,nine_img,0,1);

				//------------------------------------------------------------------------------------------------------------------------------------------//
				//------------------------------------------------------------------------------------------------------------------------------------------//		

				/* If the ball goes out of the right screen(over 640), add 1 point to P1 and reset the ball position */
				if(BallX > 740) {
					P1_Score += 1;
					BallX = 311;
					BallY = 190;
					BallSpeedX = -5;
					BallSpeedY = rand() % 3 + 2;
				}

				/* If the ball goes out of the left screen(under 0), add 1 point to P2 and reset the ball position */
				if(BallX < -100) {
					P2_Score += 1;
					BallX = 311;
					BallY = 190;
					BallSpeedX = 5;
					BallSpeedY = rand() % 3 + 2;
				}

				/* If the score is 10, return to main menu(end game). I'll add a "CONGRATULATIONS" screen */
				if(P1_Score == 10) {
					game_mode = 0;
				}
				
				if(P2_Score == 10) {
					game_mode = 0;
				}
				
				//------------------------------------------------------------------------------------------------------------------------------------------//
				//------------------------------------------------------------------------------------------------------------------------------------------//		

				VIDEO_WaitVSync();

				if (WPAD_ButtonsDown (0) & WPAD_BUTTON_PLUS)
					{
						MODPlay_Stop(&snd);
						game_mode = 3; //Press PLUS to open the pause screen(game_mode = 3)
					}
				}
			}
			
		if(game_mode == 2) //This is the "How to play" screen
		{	
			while(game_mode == 2)
				{
					GRRLIB_Render();
					WPAD_ScanPads ();
					GRRLIB_FillScreen(0x0000);
					
					GRRLIB_Print(270,80,font5_char_width,font5_char_high,"INFORMATION",font5,0x049F,0x0000);
					
					GRRLIB_Line(45,100,600,100,0xFFFF);
					GRRLIB_Line(46,101,599,101,0xFFFF);
					GRRLIB_Line(47,102,598,102,0xFFFF);
					
					GRRLIB_Print(50,120,font5_char_width,font5_char_high,"This is a VERY simple game of PONG for Wii.",font5,0xFFFF,0x0000);
					GRRLIB_Print(50,140,font5_char_width,font5_char_high,"Created from zero, it's not a port of any kind.",font5,0xFFFF,0x0000);
					GRRLIB_Print(50,180,font5_char_width,font5_char_high,"Move the left bar with Wiimote 1, and the right one with ",font5,0xFFFF,0x0000);
					GRRLIB_Print(50,200,font5_char_width,font5_char_high,"Wiimote 2.",font5,0xFFFF,0x0000);
					GRRLIB_Print(50,220,font5_char_width,font5_char_high,"Press PLUS to open pause the game.",font5,0xFFFF,0x0000);
					GRRLIB_Print(50,260,font5_char_width,font5_char_high,"TODO list and credits are in the README",font5,0xFFFF,0x0000);
					GRRLIB_Print(40,400,font5_char_width,font5_char_high,"Press A or B to return...",font5,0xFFFF,0x0000);
					
					if (WPAD_ButtonsDown (0) & WPAD_BUTTON_A) game_mode = 0;					
					if (WPAD_ButtonsDown (0) & WPAD_BUTTON_B) game_mode = 0;
					
					VIDEO_WaitVSync();
				}
		}
		
		if(game_mode == 3) //This is the PAUSE screen
		{	
			while(game_mode == 3)
				{
					GRRLIB_Render();
					WPAD_ScanPads ();
					GRRLIB_FillScreen(0x0000);
					GRRLIB_Print(298,80,font5_char_width,font5_char_high,"PAUSE",font5,0x049F,0x0000);
					
					GRRLIB_Line(45,100,600,100,0xFFFF);
					GRRLIB_Line(46,101,599,101,0xFFFF);
					GRRLIB_Line(47,102,598,102,0xFFFF);
 
					/* Define the selected option, and the pointer(">") position to print */
					if ((WPAD_ButtonsDown (0) & WPAD_BUTTON_DOWN) && (pause_selected < 2)) { pause_selected += 1; pause_position += 20; }
					if ((WPAD_ButtonsDown (0) & WPAD_BUTTON_UP) && (pause_selected > 0)) { pause_selected += -1; pause_position += -20; }

					/* Draw 2 squares, so the lines show nice on a old TV. */
					GRRLIB_DrawRectangle(39,159,77,242,0x2104,1);
					GRRLIB_DrawRectangle(39,159,77,242,0xFFFF,0);
					GRRLIB_DrawRectangle(40,160,75,240,0xFFFF,0);
					
					GRRLIB_Print(70,170,font5_char_width,font5_char_high,"Continue",font5,0xFFFF,0x2104);
					GRRLIB_Print(70,190,font5_char_width,font5_char_high,"Restart",font5,0xFFFF,0x2104);
					GRRLIB_Print(70,210,font5_char_width,font5_char_high,"Main menu",font5,0xFFFF,0x2104);
					
					GRRLIB_Print(50,pause_position,font5_char_width,font5_char_high,">",font5,0xFFFF,0x2104);
					
					if ((WPAD_ButtonsDown (0) & WPAD_BUTTON_A) && (pause_selected == 0)) { //Get the selected option, and reset the position of the pointer(">")
						pause_selected = 0; 
						pause_position = 170;
						game_mode = 1;
					}
					
					if ((WPAD_ButtonsDown (0) & WPAD_BUTTON_A) && (pause_selected == 1)) { //Get the selected option, and reset the position of the pointer(">")
						Restart();
						pause_selected = 0; 
						pause_position = 170;
						game_mode = 1;
					}
					
					if ((WPAD_ButtonsDown (0) & WPAD_BUTTON_A) && (pause_selected == 2)) { //Get the selected option, and reset the position of the pointer(">")
						Restart();
						pause_selected = 0; 
						pause_position = 170;
						game_mode = 0;
					}
					
					VIDEO_WaitVSync();
				}
		}
		
		if(game_mode == 4) //Players selection screen
		{	
			while(game_mode == 4)
				{
					u32 type;
					
					GRRLIB_Render();
					WPAD_ScanPads ();
					GRRLIB_FillScreen(0x0000);
					GRRLIB_DrawImg(212, 50, logo_width, logo_high, logo_img, 0, 1); //Draw the Logo image from "#include "img/logo.h""
					GRRLIB_Print(248,110,font5_char_width,font5_char_high,"Wii Version 1.1",font5,0xFFFF,0x0000);

					/* Draw 2 squares, so the lines show nice on a old TV. The first one is filled to make the background color. */
					GRRLIB_DrawRectangle(39,159,77,262,0x2104,1);
					GRRLIB_DrawRectangle(39,159,77,262,0xFFFF,0);
					GRRLIB_DrawRectangle(40,160,75,260,0xFFFF,0);
					
					GRRLIB_Print(70,170,font5_char_width,font5_char_high,"1P VS CPU",font5,0xFFFF,0x2104);
					
					if(!WPAD_Probe(1, &type) == WPAD_ERR_NONE)	GRRLIB_Print(70,190,font5_char_width,font5_char_high,"1P VS 2P",font5,0x528A,0x2104);
					else GRRLIB_Print(70,190,font5_char_width,font5_char_high,"1P VS 2P",font5,0xFFFF,0x2104);
					GRRLIB_Print(70,210,font5_char_width,font5_char_high,"Go back",font5,0xFFFF,0x2104);
					
					if((players_selected == 1) && (!WPAD_Probe(1, &type) == WPAD_ERR_NONE)) GRRLIB_Print(140,300,font5_char_width,font5_char_high,"Please connect Wiimote 2 to use this mode",font5,0xF984,0x0000);
					
					if(CPU_dif == 2)
						{
							GRRLIB_Print(181,170,font5_char_width,font5_char_high,"   Easy >>",font5,0xFFFF,0x2104);
							GRRLIB_Print(181,170,font5_char_width,font5_char_high,"<<",font5,0x528A,0x2104);
							
						}
						
					if(CPU_dif == 3)
						{
							GRRLIB_Print(181,170,font5_char_width,font5_char_high,"<< Hard ",font5,0xFFFF,0x2104);
							GRRLIB_Print(253,170,font5_char_width,font5_char_high,">>",font5,0x528A,0x2104);
						}
					
					GRRLIB_Print(243,440,font5_char_width,font5_char_high,"Coded by JPaRaDoX",font5,0x0544,0x0000);
					
					GRRLIB_Print(50,players_position,font5_char_width,font5_char_high,">",font5,0xFFFF,0x2104); //Print the pointer(">") at the position("title_position"). It changes with the next 2 lines.

					/* Define the selected option, and the pointer(">") position to print */
					if ((WPAD_ButtonsDown (0) & WPAD_BUTTON_DOWN) && (players_selected < 2)) { players_selected += 1; players_position += 20; }
					if ((WPAD_ButtonsDown (0) & WPAD_BUTTON_UP) && (players_selected > 0)) { players_selected += -1; players_position += -20; }
					
					if ((WPAD_ButtonsDown (0) & WPAD_BUTTON_LEFT) && CPU_dif == 3 && players_selected == 0) CPU_dif += -1; //Change the CPUs difficulty.
					if ((WPAD_ButtonsDown (0) & WPAD_BUTTON_RIGHT) && CPU_dif == 2 && players_selected == 0) CPU_dif += 1; //Change the CPUs difficulty.
					
					if ((WPAD_ButtonsDown (0) & WPAD_BUTTON_A) && (players_selected == 0)) { //Get the selected option, and reset the position of the pointer(">")
						num_players = 1;
						Restart();
						game_mode = 1;
					}
					
					if ((WPAD_ButtonsDown (0) & WPAD_BUTTON_A) && (players_selected == 1) && (WPAD_Probe(1, &type) == WPAD_ERR_NONE)) { //Get the selected option, and reset the position of the pointer(">")
						num_players = 2;
						Restart();
						game_mode = 1;
					}
					
					if ((WPAD_ButtonsDown (0) & WPAD_BUTTON_A) && (players_selected == 2)) { //Get the selected option, and reset the position of the pointer(">")
						players_selected = 0;
						players_position = 170;
						game_mode = 0;
					}
					
					VIDEO_WaitVSync();
				}
		}
		

	}

return(0);

}

