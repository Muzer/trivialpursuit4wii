#ifndef ASTEROIDS_H
#define ASTEROIDS_H

#include "wspr_types.h"
#include "wspr_graphics.h"

#define MAX_VELOCITY 8
#define SHIP_DECAY 0.98
#define SHIP_BULLET_DISTANCE 20
#define ROCK_TIME 60
#define ROCK_MIN_WIDTH 40
#define ROCK_MAX_WIDTH 100
#define ROCK_MIN_VEL 2
#define ROCK_MAX_VEL 5

class Game;

///////////////////////////////////////////////////////////////////////////////
class Sprite {
public:
  FP64 x,y,t;
  bool hurtship,survivereset,hurtrocks;
  S32 playernum;
  
  Sprite():x(0),y(0),t(0),hurtship(false),survivereset(false),hurtrocks(false),playernum(-1) {}
  ~Sprite() {}
  
  virtual void update() =0;
  virtual void draw(Graphics &g) =0;
  virtual bool shipCollide(FP64 x,FP64 y) =0;
  
  virtual void kill() {}
  virtual void reset() {}
};

///////////////////////////////////////////////////////////////////////////////
class Game {
public:
  S32 sprc;
  Sprite **sprs;
  S32 rocktimer;

  Game():sprc(0),sprs(NULL),rocktimer(ROCK_TIME) {}
  ~Game() {}
  
  void update();
  void draw(Graphics &g);
  
  void addSprite(Sprite *spr);
  void removeSprite(Sprite *spr);
  void _removeSprite(S32 index);
  S32 findSprite(Sprite *spr);
  Sprite *findPlayer(S32 who);
  
  void reset();
};

///////////////////////////////////////////////////////////////////////////////
class Ship : public Sprite {
public:
  FP64 facet;
  FP64 movet,movev;
  Game *game;
  
  Ship(Game *game,FP64 x,FP64 y);
  ~Ship() {}
  
  void update();
  void draw(Graphics &g);
  
  void addVelocity(FP64 t,FP64 v);
  void fire();
  void blowUp();

  bool shipCollide(FP64 x,FP64 y) { return false; }
  
  void reset();
};

///////////////////////////////////////////////////////////////////////////////
class Bullet : public Sprite {
public:
  FP64 v;
  Game *game;
  
  Bullet(Game *game):game(game) { if (game) game->addSprite(this); hurtship=true; hurtrocks=true; }
  ~Bullet() {}
  
  void update();
  void draw(Graphics &g);
  void kill();
  
  bool shipCollide(FP64 sx,FP64 sy) { return (sqrt((sx-x)*(sx-x)+(sy-y)*(sy-y))<SHIP_BULLET_DISTANCE); }
};

///////////////////////////////////////////////////////////////////////////////
class Rock : public Sprite {
public:
  Game *game;
  FP64 v,w;
  
  Rock(Game *game):game(game) { if (game) game->addSprite(this); hurtship=true; }
  ~Rock() {}
  
  void update();
  void draw(Graphics &g);
  
  void kill();
  void blowUp(FP64 incomingt);
  
  bool shipCollide(FP64 sx,FP64 sy);
};

#endif
