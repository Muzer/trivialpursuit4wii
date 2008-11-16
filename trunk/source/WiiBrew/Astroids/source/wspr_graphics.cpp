#include <math.h>
#include "wspr_types.h"
#include "wspr_surface.h"
#include "wspr_graphics.h"

///////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR ------------------------------------------------------------- //

Graphics::Graphics():surf(NULL),pen(0,0,1,1),fgc(0x0080ff80),bgc(0xff80ff80) {
}

Graphics::Graphics(Surface &surf):surf(&surf),pen(0,0,1,1),fgc(0x0080ff80),bgc(0xff80ff80) {
}

Graphics::~Graphics() {
}

///////////////////////////////////////////////////////////////////////////////
// LINE -------------------------------------------------------------------- //

void Graphics::line(S32 ax,S32 ay,S32 bx,S32 by,S32 linewidth,S32 lineheight,U32 c) {
  if (!surf) return;
  // we want point a on top...
  if (ay>by) { int tmp=by; by=ay; ay=tmp; tmp=bx; bx=ax; ax=tmp; }
  
  // special cases...
  if ((ay==by)||(ax==bx)) {
    if (ax>bx) { int tmp=bx; bx=ax; ax=tmp; }
    for (int y=ay;y<by+lineheight;y++) surf->setRow(ax,bx+linewidth,y,c);
    return;
  }
  
  // set up...
  by++; // our algorithm draws inside the described rectangle -- that's not what the user wants, so fix it right here
  if (ax>bx) ax++; else bx++;
  int bottom=by+lineheight-1;
  int x=ax,y=ay;
  int direction=(bx>ax?1:-1);
  int xweight=(bx-ax)*direction;
  int yweight=by-ay;
  int xcount=yweight,ycount=xweight;
  int diagonaltolerance=(xweight<yweight?xweight:yweight)/2;
  int rowstarts[lineheight+1];
  for (int i=0;i<=lineheight;i++) rowstarts[i]=ax;
  int nextrowstart=0;
  int lastrowstart=1%lineheight;
  
  // trace from a to b...
  while (y<bottom) {
    int diff=xcount-ycount;
    
    if (diff<-diagonaltolerance) { // go horz
      x+=direction;
      xcount+=yweight;
      
    } else { // go vert or diagonal, draw the row
      int startcol,endcol;
      if (direction>0) {
        if (y<ay+lineheight) startcol=ax;
        else startcol=rowstarts[lastrowstart];
        if (y>=by) endcol=bx+linewidth;
        else endcol=x+linewidth;
      } else {
        if (y>=by) startcol=bx;
        else startcol=x;
        if (y<ay+lineheight) endcol=ax+linewidth;
        else endcol=rowstarts[lastrowstart]+linewidth;
      }
      surf->setRow(startcol,endcol,y,c);
      
      if (diff<diagonaltolerance) {
        x+=direction;
        xcount+=yweight;
      }
      
      if (++lastrowstart>=lineheight) lastrowstart=0;
      rowstarts[nextrowstart]=x;
      if (++nextrowstart>=lineheight) nextrowstart=0;
      y++;
      ycount+=xweight;
    
    }
  }
}

void Graphics::lineAA(S32 ax,S32 ay,S32 bx,S32 by,S32 linewidth,S32 lineheight,U32 c) {
  if ((ax==bx)||(ay==by)) {
    fillRect(MIN(ax,bx),MIN(ay,by),ABS(ax-bx)+linewidth,ABS(ay-by)+lineheight,c);
    return;
  }
  if (ay>by) { S32 tmp=ay; ay=by; by=tmp; tmp=ax; ax=bx; bx=tmp; }
  S32 bottom=by+lineheight;
  S32 width=ABS(ax-bx);
  S32 height=by-ay;
  FP64 xenter=0,xon=0,xexit=0,xoff=0;
  S32 left=MIN(ax,bx);
  S32 right=MAX(ax,bx)+linewidth;
  
  if (bx>ax) {
    xon=-lineheight*width; xon=ax+(xon/height);
    xoff=ax+linewidth;
  } else {
    xenter=ax;
    xexit=-lineheight*width; xexit=ax-(xexit/height);
  }
  
  for (S32 y=ay;y<bottom;y++) {
  
    if (bx>ax) {
      xenter=xon;
      xexit=xoff;
      xon=(((y+1)-(ay+lineheight))*width); xon=ax+(xon/height);
      xoff=(((y+1)-ay)*width); xoff=ax+linewidth+(xoff/height);
    } else {
      xon=xenter;
      xoff=xexit;
      xenter=((y+1)-ay)*width; xenter=ax-(xenter/height);
      xexit=((y+1)-(ay+lineheight))*width; xexit=ax+linewidth-(xexit/height);
    }
    
    S32 beginsolid,endsolid;
    FP64 fraco,fraci,wholeo,wholei;
    fraco=modf(xenter,&wholeo);
    fraci=modf(xon,&wholei);
    if (wholeo<left-1) { wholeo=left-1; fraco=0; } else if (wholeo>right+1) { wholeo=right+1; fraco=1; }
    if (wholei<left-1) { wholei=left-1; fraci=0; } else if (wholei>right+1) { wholei=right+1; fraci=1; }
    if (wholeo==wholei) {
      FP64 amt=0.5*(xon-xenter)+(1-fraci);
      surf->setAlphaPixel((S32)wholeo,y,c,lround(amt*255));
    } else {
      FP64 olega=(1-fraco);
      FP64 olegb=olega/(xon-xenter);
      FP64 oarea=(olega*olegb)/2;
      if (oarea<0) oarea=0; else if (oarea>1) oarea=1;
      surf->setAlphaPixel((S32)wholeo,y,c,lround(oarea*255));
      
      FP64 ilega=fraci/(xon-xenter);
      FP64 iarea=1-(fraci*ilega)/2;
      if (iarea<0) iarea=0; else if (iarea>1) iarea=1;
      surf->setAlphaPixel((S32)wholei,y,c,lround(iarea*255));
      
      for (S32 x=(S32)wholeo+1;x<(S32)wholei;x++) surf->setAlphaPixel(x,y,c,(U8)(((x-wholeo+1)*255)/(wholei-wholeo+1)));
    }
    beginsolid=(S32)wholei+1;
    
    fraco=modf(xoff,&wholeo);
    fraci=modf(xexit,&wholei);
    if (wholeo<left-1) { wholeo=left-1; fraco=0; } else if (wholeo>right+1) { wholeo=right+1; fraco=1; }
    if (wholei<left-1) { wholei=left-1; fraci=0; } else if (wholei>right+1) { wholei=right+1; fraci=1; }
    if (wholeo==wholei) {
      FP64 amt=0.5*(xoff-xexit)+(fraci);
      surf->setAlphaPixel((S32)wholeo,y,c,lround(amt*255));
    } else {
      FP64 olega=fraco;
      FP64 olegb=olega/(xoff-xexit);
      FP64 oarea=(olega*olegb)/2;
      if (oarea<0) oarea=0; else if (oarea>1) oarea=1;
      surf->setAlphaPixel((S32)wholeo,y,c,lround(oarea*255));  
      
      FP64 ilega=(1-fraci);
      FP64 ilegb=ilega/(xoff-xexit);
      FP64 iarea=1-(ilega*ilegb)/2;
      if (iarea<0) iarea=0; else if (iarea>1) iarea=1;
      surf->setAlphaPixel((S32)wholei,y,c,lround(iarea*255));
      
      for (S32 x=(S32)wholei+1;x<(S32)wholeo;x++) surf->setAlphaPixel(x,y,c,(U8)(((wholeo-x)*255)/(wholeo-wholei+1)));
    }
    endsolid=(S32)wholei;
    
    surf->setRow(beginsolid,endsolid,y,c);
    
  }
}

///////////////////////////////////////////////////////////////////////////////
// BEZIER CURVE ------------------------------------------------------------ //

void Graphics::locateCurvePoint(CurveControlPoint *qpt,int ax,int ay,int bx,int by,int cx,int cy,int tmax) {
  int acx=ax+((cx-ax)*qpt->t)/tmax;
  int acy=ay+((cy-ay)*qpt->t)/tmax;
  int cbx=cx+((bx-cx)*qpt->t)/tmax;
  int cby=cy+((by-cy)*qpt->t)/tmax;
  qpt->x=acx+((cbx-acx)*qpt->t)/tmax;
  qpt->y=acy+((cby-acy)*qpt->t)/tmax;
}

void Graphics::fillInCurve(CurveControlPoint *qpt,int ax,int ay,int bx,int by,int cx,int cy,int tmax,int tolerance) {
  int pdist=(qpt->prev?(ABS(qpt->x-qpt->prev->x)+ABS(qpt->y-qpt->prev->y)):0);
  int ndist=(qpt->next?(ABS(qpt->x-qpt->next->x)+ABS(qpt->y-qpt->next->y)):0);
  if (pdist>tolerance) {
  
    CurveControlPoint *npt=(CurveControlPoint*)malloc(sizeof(CurveControlPoint));
    if (!npt) throw MemoryError();
    npt->next=qpt;
    npt->prev=qpt->prev;
    npt->t=(qpt->t+qpt->prev->t)>>1;
    qpt->prev=npt;
    npt->prev->next=npt;
    locateCurvePoint(npt,ax,ay,bx,by,cx,cy,tmax);
    if ((npt->x!=qpt->x)||(npt->y!=qpt->y)) // if it returned the same point, we're too precise, so stop recursing
      fillInCurve(npt,ax,ay,bx,by,cx,cy,tmax,tolerance);
    
  }
  if (ndist>tolerance) {
  
    CurveControlPoint *npt=(CurveControlPoint*)malloc(sizeof(CurveControlPoint));
    if (!npt) throw MemoryError();
    npt->prev=qpt;
    npt->next=qpt->next;
    npt->t=(qpt->t+qpt->next->t)>>1;
    qpt->next=npt;
    npt->next->prev=npt;
    locateCurvePoint(npt,ax,ay,bx,by,cx,cy,tmax);
    if ((npt->x!=qpt->x)||(npt->y!=qpt->y))
      fillInCurve(npt,ax,ay,bx,by,cx,cy,tmax,tolerance);
    
  }
}

void Graphics::curve(S32 ax,S32 ay,S32 bx,S32 by,S32 cx,S32 cy,S32 linewidth,S32 lineheight,U32 c,S32 tolerance) {
  if (!surf) return;
  int tmax=1024; // arbitrary limit for scaling the one-dimensional movements
  CurveControlPoint *apt,*bpt,*midpt;
  apt=(CurveControlPoint*)malloc(sizeof(CurveControlPoint));
  bpt=(CurveControlPoint*)malloc(sizeof(CurveControlPoint));
  midpt=(CurveControlPoint*)malloc(sizeof(CurveControlPoint));
  if ((!apt)||(!bpt)||(!midpt)) throw MemoryError();
  
  apt->x=ax; apt->y=ay; apt->t=0; apt->prev=NULL; apt->next=midpt;
  bpt->x=bx; bpt->y=by; bpt->t=tmax; bpt->prev=midpt; bpt->next=NULL;
  midpt->prev=apt; midpt->next=bpt; midpt->t=tmax>>1;
  
  locateCurvePoint(midpt,ax,ay,bx,by,cx,cy,tmax);
  fillInCurve(midpt,ax,ay,bx,by,cx,cy,tmax,tolerance);
  
  while (apt) {
    bpt=apt->next;
    if (bpt) line(apt->x,apt->y,bpt->x,bpt->y,linewidth,lineheight,c);
    free(apt);
    apt=bpt;
  }
}

void Graphics::curveAA(S32 ax,S32 ay,S32 bx,S32 by,S32 cx,S32 cy,S32 linewidth,S32 lineheight,U32 c,S32 tolerance) {
  if (!surf) return;
  int tmax=1024; // arbitrary limit for scaling the one-dimensional movements
  CurveControlPoint *apt,*bpt,*midpt;
  apt=(CurveControlPoint*)malloc(sizeof(CurveControlPoint));
  bpt=(CurveControlPoint*)malloc(sizeof(CurveControlPoint));
  midpt=(CurveControlPoint*)malloc(sizeof(CurveControlPoint));
  if ((!apt)||(!bpt)||(!midpt)) throw MemoryError();
  
  apt->x=ax; apt->y=ay; apt->t=0; apt->prev=NULL; apt->next=midpt;
  bpt->x=bx; bpt->y=by; bpt->t=tmax; bpt->prev=midpt; bpt->next=NULL;
  midpt->prev=apt; midpt->next=bpt; midpt->t=tmax>>1;
  
  locateCurvePoint(midpt,ax,ay,bx,by,cx,cy,tmax);
  fillInCurve(midpt,ax,ay,bx,by,cx,cy,tmax,tolerance);
  
  while (apt) {
    bpt=apt->next;
    if (bpt) lineAA(apt->x,apt->y,bpt->x,bpt->y,linewidth,lineheight,c);
    free(apt);
    apt=bpt;
  }
}

///////////////////////////////////////////////////////////////////////////////
// RECT -------------------------------------------------------------------- //

void Graphics::fillRect(S32 x,S32 y,S32 w,S32 h,U32 c) {
  if (!surf) return;
  if (w<0) { x+=w; w=-w; }
  if (h<0) { y+=h; h=-h; }
  for (S32 yc=0;yc<h;yc++) surf->setRow(x,x+w,y+yc,c);
}

void Graphics::frameRect(S32 x,S32 y,S32 w,S32 h,S32 linewidth,S32 lineheight,U32 c) {
  if (!surf) return;
  if (w<0) { x+=w; w=-w; }
  if (h<0) { y+=h; h=-h; }
  for (int yc=0;yc<lineheight;yc++) {
    surf->setRow(x,x+w,y+yc,c);
    surf->setRow(x,x+w,y+h-yc-1,c);
  }
  int innerheight=h-lineheight;
  for (int yc=lineheight;yc<innerheight;yc++) {
    surf->setRow(x,x+linewidth,y+yc,c);
    surf->setRow(x+w-linewidth,x+w,y+yc,c);
  }
}

///////////////////////////////////////////////////////////////////////////////
// ROUND RECT (OVAL) ------------------------------------------------------- //

void Graphics::fillRoundRect(S32 x,S32 y,S32 w,S32 h,S32 xradius,S32 yradius,U16 portions,U32 c) {
  if (!surf) return;
  if (w<0) { x+=w; w=-w; }
  if (h<0) { y+=h; h=-h; }
  if (xradius<0) xradius=-xradius;
  if (yradius<0) yradius=-yradius;
  if ((w==1)||(h==1)||(xradius<1)||(yradius<1)) {
    fillRect(x,y,w,h,c);
    return;
  }
  if ((w<=0)||(h<=0)) return;
  int xr2=(xradius)*(xradius);
  int yr2=(yradius)*(yradius);
  int midx=x+xradius;
  int midy=y+yradius;
  int midright=x+w-xradius;
  int midbottom=y+h-yradius;
  
  int ycorrection=1;
  
  int cr2=(yradius+1)*(yradius+1);
  int k=(cr2<<15)/xr2;
  
  int dyr2,dxr2;
  dyr2=yr2;
  dxr2=0;
  int dx=0;
  int dy=yradius;
  
  bool nw=portions&0x0001,ne=portions&0x0004,sw=portions&0x0040,se=portions&0x0100;
  
  while (dy>=0) {
    if ((dyr2+((dxr2*k)>>15)>=cr2)) {
    
      if (nw) surf->setRow(midx-dx,midx+1,midy-dy,c);
      if (ne) surf->setRow(midright,midright+dx,midy-dy,c);
      if (sw) surf->setRow(midx-dx,midx+1,midbottom+dy-ycorrection,c);
      if (se) surf->setRow(midright,midright+dx,midbottom+dy-ycorrection,c);
      
      dy--;
      dyr2-=dy+dy-1; // previous square
      continue;
    }
    dx++;
    dxr2+=dx+dx-1; // next square
  }
  
  if (portions&0x0002) fillRect(x+xradius,y,w-xradius*2,yradius,c);
  if (portions&0x0008) fillRect(x,y+yradius,xradius,h-yradius*2,c);
  if (portions&0x0010) fillRect(x+xradius,y+yradius,w-xradius*2,h-yradius*2,c);
  if (portions&0x0020) fillRect(x+w-xradius,y+yradius,xradius,h-yradius*2,c);
  if (portions&0x0080) fillRect(x+xradius,y+h-yradius-ycorrection,w-xradius*2,yradius,c);
}

void Graphics::frameRoundRect(S32 x,S32 y,S32 w,S32 h,S32 xradius,S32 yradius,U16 portions,S32 linewidth,S32 lineheight,U32 c) {
  if (!surf) return;
  if (w<0) { x+=w; w=-w; }
  if (h<0) { y+=h; h=-h; }
  if (xradius<0) xradius=-xradius;
  if (yradius<0) yradius=-yradius;
  if ((w==1)||(h==1)||(xradius<1)||(yradius<1)) {
    fillRect(x,y,w,h,c);
    return;
  }
  //int xradius=width>>1;
  int xr2=(xradius)*(xradius);
  //int yradius=height>>1;
  int yr2=(yradius)*(yradius);
  int midx=x+xradius;
  int midy=y+yradius;
  int midright=x+w-xradius;
  int midbottom=y+h-yradius;
  
  int ycorrection=1;
  
  if ((linewidth>=xradius)||(lineheight>=yradius)) {
    fillRoundRect(x,y,w,h,xradius,yradius,portions,c);
    return;
  }
  
  int inneryradius=yradius-lineheight;
  int innerxradius=xradius-linewidth;
  int ixr2=innerxradius*innerxradius;
  
  int cr2=(yradius+1)*(yradius+1);
  int icr2=(inneryradius+1)*(inneryradius+1);
  int k=(cr2<<15)/xr2;
  int ik=(icr2<<15)/ixr2;
  
  int dyr2,dxr2;
  dyr2=yr2;
  dxr2=0;
  int dx=0;
  int dy=yradius;
  int idx=0;
  int idxr2=0;
  
  bool nw=portions&0x0001,ne=portions&0x0004,sw=portions&0x0040,se=portions&0x0100;
  
  while (dy>=0) {
    if (dyr2+((dxr2*k)>>15)>=cr2) {
    
      /*if (dy>inneryradius) {
        if (nw&&ne) surf->setRow(midx-dx,midx+dx,midy-dy,c);
        else if (nw) surf->setRow(midx-dx,midx,midy-dy,c);
        else if (ne) surf->setRow(midx,midx+dx,midy-dy,c);
        if (sw&&se) surf->setRow(midx-dx,midx+dx,midy+dy,c);
        else if (sw) surf->setRow(midx-dx,midx,midy+dy,c);
        else if (se) surf->setRow(midx,midx+dx,midy+dy,c);
      } else {*/
        if (nw) surf->setRow(midx-dx,midx-idx+1,midy-dy,c);
        if (ne) surf->setRow(midright+idx-1,midright+dx,midy-dy,c);
        if (sw) surf->setRow(midx-dx,midx-idx+1,midbottom+dy-ycorrection,c);
        if (se) surf->setRow(midright+idx-1,midright+dx,midbottom+dy-ycorrection,c);
      //}
      
      dy--;
      dyr2-=dy+dy-1; // previous square
      
      while (dyr2+((idxr2*ik)>>15)<icr2) {
        idx++;
        idxr2+=idx+idx-1;
      }
    
      continue;
    }
    dx++;
    dxr2+=dx+dx-1; // next square
    
  }
  
  if (portions&0x0002) fillRect(x+xradius,y,w-xradius*2,lineheight,c);
  if (portions&0x0008) fillRect(x,y+yradius,linewidth,h-yradius*2,c);
  if (portions&0x0020) fillRect(x+w-linewidth,y+yradius,linewidth,h-yradius*2,c);
  if (portions&0x0080) fillRect(x+xradius,y+h-lineheight,w-xradius*2,lineheight,c);
}
