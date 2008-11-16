#ifndef WSPR_GRAPHICS_H
#define WSPR_GRAPHICS_H

#include "wspr_types.h"
#include "wspr_surface.h"

/******************************************************************************
Graphics goes hand-in-hand with Surface. Graphics is the drawing commands,
where Surface is the pixel storage.

90-degree arcs and circles are derived from the primitive roundRect.
RoundRect takes a "portions" argument:
 0x0001 nw corner
 0x0002 n  edge
 0x0004 ne corner
 0x0008 w  edge
 0x0010    middle
 0x0020 e  edge
 0x0040 sw corner
 0x0080 s  edge
 0x0100 se corner
 
Frame commands will draw an object's frame entirely within its bounds.
(this may seem illogical...)

A curve's tolerance is the minimum distance between two sample points.
(Not the true distance, just the sum of dx and dy).
0 for best results.
10 for speed (over appearance). You can go higher, too, but there's not much point.
4 is the default (a pretty good compromise).

I generally went for speed over prettiness with the curves. They get kind of sloppy, even at 0 tolerance.
They'll look better with thicker lines.
I figure when we're displaying on a TV set, we don't really need to be pixel-perfect. ;)
******************************************************************************/

class Graphics {

  typedef struct CurveControlPoint {
    int x,y,t;
    CurveControlPoint *prev,*next;
  };

  Surface *surf;
  Rect pen;
  U32 fgc,bgc;
  
  void locateCurvePoint(CurveControlPoint *qpt,int ax,int ay,int bx,int by,int cx,int cy,int tmax);
  void fillInCurve(CurveControlPoint *qpt,int ax,int ay,int bx,int by,int cx,int cy,int tmax,int tolerance);
  
public:
  Graphics();
  Graphics(Surface &surf);
  ~Graphics();
  
  void setSurface(Surface *surf) { this->surf=surf; }
  Rect bounds() const { if (surf) return surf->bounds; return Rect(); }
  
  // atomic primitives (do not depend on Graphics's state):
  void line(S32 ax,S32 ay,S32 bx,S32 by,S32 linewidth,S32 lineheight,U32 c);
  void lineAA(S32 ax,S32 ay,S32 bx,S32 by,S32 linewidth,S32 lineheight,U32 c);
  void curve(S32 ax,S32 ay,S32 bx,S32 by,S32 cx,S32 cy,S32 linewidth,S32 lineheight,U32 c,S32 tolerance);
  void curveAA(S32 ax,S32 ay,S32 bx,S32 by,S32 cx,S32 cy,S32 linewidth,S32 lineheight,U32 c,S32 tolerance);
  void fillRect(S32 x,S32 y,S32 w,S32 h,U32 c);
  void frameRect(S32 x,S32 y,S32 w,S32 h,S32 linewidth,S32 lineheight,U32 c);
  void fillRoundRect(S32 x,S32 y,S32 w,S32 h,S32 xradius,S32 yradius,U16 portions,U32 c);
  void frameRoundRect(S32 x,S32 y,S32 w,S32 h,S32 xradius,S32 yradius,U16 portions,S32 linewidth,S32 lineheight,U32 c);
  
  // derived shapes (using foreColor, backColor, penSize, etc):
  void paintRect(const Rect &r) { fillRect(r.x,r.y,r.w,r.h,fgc); }
  void paintRect(S32 x,S32 y,S32 w,S32 h) { fillRect(x,y,w,h,fgc); }
  void eraseRect(const Rect &r) { fillRect(r.x,r.y,r.w,r.h,bgc); }
  void eraseRect(S32 x,S32 y,S32 w,S32 h) { fillRect(x,y,w,h,bgc); }
  void frameRect(const Rect &r) { frameRect(r.x,r.y,r.w,r.h,pen.w,pen.h,fgc); }
  void frameRect(S32 x,S32 y,S32 w,S32 h) { frameRect(x,y,w,h,pen.w,pen.h,fgc); }
  
  void paintOval(const Rect &r) { fillRoundRect(r.x,r.y,r.w,r.h,r.w>>1,r.h>>1,0x0145,fgc); }
  void paintOval(S32 x,S32 y,S32 w,S32 h) { fillRoundRect(x,y,w,h,w>>1,h>>1,0x0145,fgc); }
  void eraseOval(const Rect &r) { fillRoundRect(r.w,r.y,r.w,r.h,r.w>>1,r.h>>1,0x0145,bgc); }
  void eraseOval(S32 x,S32 y,S32 w,S32 h) { fillRoundRect(x,y,w,h,w>>1,h>>1,0x0145,bgc); }
  void frameOval(const Rect &r) { frameRoundRect(r.x,r.y,r.w,r.h,r.w>>1,r.h>>1,0x0145,pen.w,pen.h,fgc); }
  void frameOval(S32 x,S32 y,S32 w,S32 h) { frameRoundRect(x,y,w,h,w>>1,h>>1,0x0145,pen.w,pen.h,fgc); }
  
  void paintRoundRect(const Rect &r,S32 xradius,S32 yradius) { fillRoundRect(r.x,r.y,r.w,r.h,xradius,yradius,0x01ff,fgc); }
  void paintRoundRect(S32 x,S32 y,S32 w,S32 h,S32 xradius,S32 yradius) { fillRoundRect(x,y,w,h,xradius,yradius,0x01ff,fgc); }
  void eraseRoundRect(const Rect &r,S32 xradius,S32 yradius) { fillRoundRect(r.x,r.y,r.w,r.h,xradius,yradius,0x01ff,bgc); }
  void eraseRoundRect(S32 x,S32 y,S32 w,S32 h,S32 xradius,S32 yradius) { fillRoundRect(x,y,w,h,xradius,yradius,0x01ff,bgc); }
  void frameRoundRect(const Rect &r,S32 xradius,S32 yradius) { frameRoundRect(r.x,r.y,r.w,r.h,xradius,yradius,0x01ff,pen.w,pen.h,fgc); }
  void frameRoundRect(S32 x,S32 y,S32 w,S32 h,S32 xradius,S32 yradius) { frameRoundRect(x,y,w,h,xradius,yradius,0x01ff,pen.w,pen.h,fgc); }
  
  // pen-based drawing:
  void move(S32 dx,S32 dy) { pen.x+=dx; pen.y+=dy; }
  void moveTo(S32 x,S32 y) { pen.x=x; pen.y=y; }
  void line(S32 dx,S32 dy) { line(pen.x,pen.y,pen.x+dx,pen.y+dy,pen.w,pen.h,fgc); move(dx,dy); }
  void lineTo(S32 x,S32 y) { line(pen.x,pen.y,x,y,pen.w,pen.h,fgc); moveTo(x,y); }
  void lineAA(S32 dx,S32 dy) { lineAA(pen.x,pen.y,pen.x+dx,pen.y+dy,pen.w,pen.h,fgc); move(dx,dy); }
  void lineToAA(S32 x,S32 y) { lineAA(pen.x,pen.y,x,y,pen.w,pen.h,fgc); moveTo(x,y); }
  void curve(S32 dx,S32 dy,S32 cdx,S32 cdy,S32 tolerance=3) { curve(pen.x,pen.y,pen.x+dx,pen.y+dy,pen.x+cdx,pen.y+cdy,pen.w,pen.h,fgc,tolerance); move(dx,dy); }
  void curveTo(S32 x,S32 y,S32 cx,S32 cy,S32 tolerance=3) { curve(pen.x,pen.y,x,y,cx,cy,pen.w,pen.h,fgc,tolerance); moveTo(x,y); }
  void curveAA(S32 dx,S32 dy,S32 cdx,S32 cdy,S32 tolerance=3) { curveAA(pen.x,pen.y,pen.x+dx,pen.y+dy,pen.x+cdx,pen.y+cdy,pen.w,pen.h,fgc,tolerance); move(dx,dy); }
  void curveToAA(S32 x,S32 y,S32 cx,S32 cy,S32 tolerance=3) { curveAA(pen.x,pen.y,x,y,cx,cy,pen.w,pen.h,fgc,tolerance); moveTo(x,y); }
  
  // pen state
  void penSize(S32 w,S32 h) { pen.w=w; pen.h=h; }
  void foreColor(U32 c) { fgc=c; }
  void backColor(U32 c) { bgc=c; }
};

#endif
