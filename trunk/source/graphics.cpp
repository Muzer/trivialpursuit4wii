/*
 *  graphics.cpp
 *  TrivialPursuit4Wii
 *
 *  Created by pinecone on 16/11/2008.
 *  Copyright 2008 pinecone. All rights reserved.
 *
 */

#include "graphics.h"
extern "C" {
	#include "GRRLIB/GRRLIB.h"
}

void initVideo(){
	VIDEO_Init();
	
    GRRLIB_InitVideo();
    GRRLIB_Start();
}

void renderLogo(){

}


int displayMenu(){
	
}