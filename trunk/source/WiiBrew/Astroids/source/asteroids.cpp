#include <math.h>
#include "wspr_types.h"
#include "asteroids.h"

///////////////////////////////////////////////////////////////////////////////
// SHIP -------------------------------------------------------------------- //

Ship::Ship(Game *game,FP64 x,FP64 y):facet(pi+halfpi),movet(0),movev(0),game(game) { 
  if (game) game->addSprite(this); 
  survivereset=true; 
  this->x=x;
  this->y=y;
}

void Ship::update() {
  while (facet<0) facet+=twopi;
  while (facet>=twopi) facet-=twopi;
  
  if (movev<0.1) movev=0;
  else {
    x+=cos(movet)*movev;
    y+=sin(movet)*movev;
    movev*=SHIP_DECAY;
  }
  
  while (x<0) x+=640;
  while (x>=640) x-=640;
  while (y<0) y+=480;
  while (y>=480) y-=480;
  
  for (S32 i=0;i<game->sprc;i++) if (game->sprs[i]->hurtship) 
    if (game->sprs[i]->shipCollide(x,y)) {
      blowUp();
      break;
    }
}

void Ship::draw(Graphics &g) {
  g.penSize(3,3);
  g.foreColor(0xff00ff00);
  g.moveTo(lround(x+cos(facet)*20),lround(y+sin(facet)*20));
  g.lineToAA(lround(x+cos(facet+2.5)*20),lround(y+sin(facet+2.5)*20));
  g.lineToAA(lround(x+cos(facet-2.5)*20),lround(y+sin(facet-2.5)*20));
  g.lineToAA(lround(x+cos(facet)*20),lround(y+sin(facet)*20));
}

void Ship::addVelocity(FP64 t,FP64 v) {
  if (movev>0.1) {
    FP64 nx=x+cos(t)*v+cos(movet)*movev;
    FP64 ny=y+sin(t)*v+sin(movet)*movev;
    movev=sqrt((nx-x)*(nx-x)+(ny-y)*(ny-y));
    if (movev>MAX_VELOCITY) movev=MAX_VELOCITY;
    movet=atan2(ny-y,nx-x);
  } else {
    movet=t;
    movev=v;
    if (movev>MAX_VELOCITY) movev=MAX_VELOCITY;
  }
}

void Ship::fire() {
  addVelocity(facet+pi,2);
  Bullet *bullet=new Bullet(game);
  if (!bullet) return;
  bullet->x=x+cos(facet)*25;
  bullet->y=y+sin(facet)*25;
  bullet->t=facet;
  bullet->v=4;
  game->addSprite(bullet);
}

void Ship::blowUp() {
  game->reset();
}

void Ship::reset() {
  x=320;
  y=240;
  facet=pi+halfpi;
  movet=0;
  movev=0;
}

///////////////////////////////////////////////////////////////////////////////
// ROCK -------------------------------------------------------------------- //

void Rock::update() {
  x+=cos(t)*v;
  y+=sin(t)*v;
  
  for (S32 i=0;i<game->sprc;i++) if (game->sprs[i]->hurtrocks) {
    if (sqrt((game->sprs[i]->x-x)*(game->sprs[i]->x-x)+(game->sprs[i]->y-y)*(game->sprs[i]->y-y))<w/2+4) {
      FP64 incomingt=game->sprs[i]->t;
      game->sprs[i]->kill();
      blowUp(incomingt);
      return;
    }
  }
  
  if ((x<-w)||(y<-w)||(x>640+w)||(y>480+w)) kill();
}

void Rock::draw(Graphics &g) {
  g.foreColor(0x6680ffaa);
  S32 px=lround(x-w/2);
  S32 py=lround(y-w/2);
  g.paintOval(Rect(px,py,lround(w),lround(w)));
}

void Rock::kill() {
  game->removeSprite(this);
}

bool Rock::shipCollide(FP64 sx,FP64 sy) {
  return (sqrt((sx-x)*(sx-x)+(sy-y)*(sy-y))<w/2+8);
}

void Rock::blowUp(FP64 incomingt) {
  if (w<ROCK_MIN_WIDTH) kill(); else {
    Rock *orock=new Rock(game);
    if (!orock) { kill(); return; }
    w/=2;
    orock->w=w;
    FP64 nx=x+cos(incomingt)*v+cos(t)*3;
    FP64 ny=y+sin(incomingt)*v+sin(t)*3;
    v=sqrt((nx-x)*(nx-x)+(ny-y)*(ny-y));
    t=atan2(ny-y,nx-x);
    orock->v=v;
    orock->t=incomingt+(incomingt-t);
    orock->x=nx;
    orock->y=ny;
  }
}

///////////////////////////////////////////////////////////////////////////////
// BULLET ------------------------------------------------------------------ //

void Bullet::update() {
  x+=cos(t)*v;
  y+=sin(t)*v;
  if ((x<0)||(y<0)||(x>640)||(y>480)) kill();
}

void Bullet::draw(Graphics &g) {
  g.penSize(1,1);
  g.foreColor(0xbb20ff80);
  S32 px=lround(x-4);
  S32 py=lround(y-4);
  g.frameOval(px,py,8,8);
}

void Bullet::kill() {
  game->removeSprite(this);
}

///////////////////////////////////////////////////////////////////////////////
// GAME -------------------------------------------------------------------- //

void Game::update() {
  for (S32 i=0;i<sprc;i++) sprs[i]->update();
  if (--rocktimer<=0) {
    rocktimer=ROCK_TIME;
    Rock *rock=new Rock(this);
    if (!rock) return;
    
    Sprite *ship=findPlayer(1);
    
    rock->w=ROCK_MIN_WIDTH+rand()%(ROCK_MAX_WIDTH-ROCK_MIN_WIDTH);
    rock->v=ROCK_MIN_VEL+rand()%(ROCK_MAX_VEL-ROCK_MIN_VEL);
    while (1) {
      switch (rand()%4) {
        case 0: rock->x=rand()%640; rock->y=0; rock->t=((rand()%100)*pi)/100; break;
        case 1: rock->x=rand()%640; rock->y=480; rock->t=pi+((rand()%100)*pi)/100; break;
        case 2: rock->x=0; rock->y=rand()%480; rock->t=((rand()%100)*pi)/100-halfpi; break;
        case 3: rock->x=640; rock->y=rand()%480; rock->t=((rand()%100)*pi)/100+halfpi; break;
      }
      if (!ship||!rock->shipCollide(ship->x,ship->y)) break;
    }
  }
}

Sprite *Game::findPlayer(S32 who) {
  for (S32 i=0;i<sprc;i++) if (sprs[i]->playernum==who) return sprs[i];
  return NULL;
}

void Game::draw(Graphics &g) {
  g.eraseRect(g.bounds());
  for (S32 i=0;i<sprc;i++) sprs[i]->draw(g);
}

S32 Game::findSprite(Sprite *spr) {
  for (S32 i=0;i<sprc;i++) if (sprs[i]==spr) return i;
  return -1;
}

void Game::addSprite(Sprite *spr) {
  if (findSprite(spr)>=0) return;
  Sprite **nsprs=(Sprite**)malloc(sizeof(void*)*(sprc+1));
  if (!nsprs) throw MemoryError();
  if (sprs) {
    memcpy(nsprs,sprs,sizeof(void*)*sprc);
    free(sprs);
  }
  sprs=nsprs;
  sprs[sprc++]=spr;
}

void Game::_removeSprite(S32 index) {
  delete sprs[index];
  for (S32 i=index;i<sprc-1;i++) sprs[i]=sprs[i+1];
  Sprite **nsprs=(Sprite**)malloc(sizeof(void*)*(sprc-1));
  if (!nsprs) throw MemoryError();
  sprc--;
  for (S32 i=0;i<sprc;i++) nsprs[i]=sprs[i];
  free(sprs);
  sprs=nsprs;
}

void Game::removeSprite(Sprite *spr) {
  S32 index;
  if ((index=findSprite(spr))<0) return;
  _removeSprite(index);
}

void Game::reset() {
  for (S32 i=0;i<sprc;) {
    if (sprs[i]->survivereset) { sprs[i]->reset(); i++; }
    else _removeSprite(i);
  }
}
