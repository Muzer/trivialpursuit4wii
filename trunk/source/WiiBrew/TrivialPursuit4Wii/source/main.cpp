#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>

#include "graphics.h"

#define POINTS2WIN 240;

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

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
	
	return 0;
}


