#ifndef WSPR_TYPES_H
#define WSPR_TYPES_H

#define U8 unsigned char
#define S8 signed char
#define U16 unsigned short
#define S16 signed short
#define U32 unsigned int
#define S32 signed int
#define U64 unsigned long long
#define S64 signed long long
#define FP32 float
#define FP64 double
#define FP96 long double

#ifndef MAX
#define MAX(a,b) ((a)<(b)?(b):(a))
#endif
#define MAX4(a,b,c,d) ((a)>(b)? ((a)>(c)? ((a)>(d)?(a):(d)) : ((c)>(d)?(c):(d))) : ((b)>(c)? ((b)>(d)?(b):(d)) : ((c)>(d)?(c):(d))))
#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef ABS
#define ABS(a) ((a)<0?-(a):(a))
#endif

#define pi 3.1415926
#define twopi 6.2831852
#define halfpi 1.5707963
#define threehalfpi 4.7123889
#define roottwo 1.414213562

class MemoryError {};
class ArgumentError {};

class Graphics;

///////////////////////////////////////////////////////////////////////////////
class Rect {
public:
  S32 x,y,w,h;
  Rect():x(0),y(0),w(0),h(0) {}
  Rect(S32 w,S32 h):x(0),y(0),w(w),h(h) {}
  Rect(S32 x,S32 y,S32 w,S32 h):x(x),y(y),w(w),h(h) {}
  Rect(const Rect &r):x(r.x),y(r.y),w(r.w),h(r.h) {}
  ~Rect() {}
  
  void set(S32 x,S32 y,S32 w,S32 h) { this->x=x; this->y=y; this->w=w; this->h=h; }
  void normalise() { if (w<0) { x+=w; w=-w; } if (h<0) { y+=h; h=-h; } }
  void move(S32 dx,S32 dy) { x+=dx; y+=dy; }
  
  S32 left() const { return x; }
  S32 right() const { return x+w; }
  S32 top() const { return y; }
  S32 bottom() const { return y+h; }
  S32 width() const { return w; }
  S32 height() const { return h; }
  S32 centerx() const { return x+(w>>1); }
  S32 centery() const { return y+(h>>1); }
  
  void left(S32 n) { x=n; }
  void right(S32 n) { x=n-w; }
  void top(S32 n) { y=n; }
  void bottom(S32 n) { y=n-h; }
  void width(S32 n) { w=n; }
  void height(S32 n) { h=n; }
  void centerx(S32 n) { x=n-(w>>1); }
  void centery(S32 n) { y=n-(h>>1); }
};

#endif
