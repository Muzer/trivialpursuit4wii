#include <math.h>
#include <sdcard.h>
#include "wspr.h"
#include <ogcsys.h>
#include <gccore.h>
#include <ogc/ipc.h>
#include <ogc/pad.h>
#include "asteroids.h"

int main(int argc,char **argv) {
  MainScreen screen;
  PAD_Init();
  srand(time(0));
  
  Graphics g(screen);
  g.foreColor(0xbb20ff80);
  g.backColor(0x0080ff80);
  Game game;
  
  Ship p1(&game,320,240);
  
  while (1) {
  
    PAD_ScanPads();
    p1.facet+=((FP64)PAD_StickX(0))/700;
    
    U32 btn=PAD_ButtonsDown(0);
    if (btn&PAD_BUTTON_A) p1.fire();
  
    game.update();
    game.draw(g);
    screen.flip();
  }
  return 0;
}
