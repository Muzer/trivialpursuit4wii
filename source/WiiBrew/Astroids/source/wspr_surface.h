#ifndef WSPR_SURFACE_H
#define WSPR_SURFACE_H

#include <memory>
#ifndef ANDY_BUILD_FOR_SDL
#include <gccore.h>
#endif
#include "wspr_types.h"

#define PIXFMT_NONE 0
#define PIXFMT_YCYC 1 // the Wii's internal format
#define PIXFMT_YCAC 2 // our reference format
#define PIXFMT_MONO 3 // 1-bit (for masks, etc)
#define PIXFMT_RGBA 4 // only works with conversions (so far)
#define PIXFMT_HSVA 5

///////////////////////////////////////////////////////////////////////////////
class Surface {
public:
  U32 *pixels;
  S32 wordsinrow;
  S32 pixinword;
  S32 pixelsize;
  S32 pixfmt;
  U32 colorkey;
  bool usecolorkey;
  Rect bounds;
  
  Surface():pixels(NULL),wordsinrow(0),pixinword(1),pixfmt(PIXFMT_NONE),usecolorkey(false) {}
  Surface(Rect r,S32 pixfmt=PIXFMT_YCAC);
  Surface(S32 w,S32 h,S32 pixfmt=PIXFMT_YCAC);
  ~Surface() { if (pixels) free(pixels); }
  
  void setColorKey(U32 c) { this->colorkey=c; usecolorkey=true; }
  void setNoColorKey() { usecolorkey=false; }
  
  // safe routines...
  U32 getPixel(S32 x,S32 y) const;
  void setPixel(S32 x,S32 y,U32 px);
  void setAlphaPixel(S32 x,S32 y,U32 px,U8 alpha);
  void setRow(S32 lox,S32 hix,S32 y,U32 px); // hi is exclusive
  void blit(const Surface &src,const Rect &srcr,const Rect &destr);
  void copyMask(const Surface &src,const Surface &mask,const Rect &srcr,const Rect &maskr,const Rect &destr);
  void clear(U32 px=0);
  
  // unchecked routines...
  U32 _getPixel_YCYC(S32 x,S32 y) const; // will return YYCCffCC
  U32 _getPixel_MONO(S32 x,S32 y) const; // will return ff80ff80 or 0080ff80
  U32 _getPixel_unknown(S32 x,S32 y) const; // will return the appropriate number based on pixinword and wordsinrow (unmodified)
  
  void _setPixel_YCYC(S32 x,S32 y,U32 px); // will overwrite existing chroma
  void _setPixel_MONO(S32 x,S32 y,U32 px); // 80xxxxxx+ == white, 7fxxxxxx- == black
  void _setPixel_unknown(S32 x,S32 y,U32 px);
  
  void _setAlphaPixel_YCYC(S32 x,S32 y,U32 px,U8 alpha);
  void _setAlphaPixel_YCAC(S32 x,S32 y,U32 px,U8 alpha);
  
  void _setRow_YCYC(S32 lox,S32 hix,S32 y,U32 px);
  void _setRow_MONO(S32 lox,S32 hix,S32 y,U32 px);
  void _setRow_unknown(S32 lox,S32 hix,S32 y,U32 px) { for (S32 x=lox;x<hix;x++) _setPixel_unknown(x,y,px); }
  
  void _blit_unknown(const Surface &src,const Rect &srcr,const Rect &destr); // unknown formats, uses getPixel and setPixel
  void _blit_straight(const Surface &src,const Rect &srcr,const Rect &destr); // copies 32-bit pixels literally (for YCAC <-> YCAC)
  void _blit_unknown_to_MONO(const Surface &src,const Rect &srcr,const Rect &destr); // reads with getPixel, writing is a little more optimised
  void _blit_YCYC_to_YCYC(const Surface &src,const Rect &srcr,const Rect &destr);
  void _blit_YCAC_to_YCYC(const Surface &src,const Rect &srcr,const Rect &destr);
  void _blit_MONO_to_YCYC(const Surface &src,const Rect &srcr,const Rect &destr);
  void _blit_RGBA_to_YCYC(const Surface &src,const Rect &srcr,const Rect &destr);
  void _blit_YCYC_to_YCAC(const Surface &src,const Rect &srcr,const Rect &destr);
  void _blit_MONO_to_YCAC(const Surface &src,const Rect &srcr,const Rect &destr);
  void _blit_RGBA_to_YCAC(const Surface &src,const Rect &srcr,const Rect &destr);
  void _blit_YCYC_to_MONO(const Surface &src,const Rect &srcr,const Rect &destr);
  void _blit_YCAC_to_MONO(const Surface &src,const Rect &srcr,const Rect &destr);
  void _blit_MONO_to_MONO(const Surface &src,const Rect &srcr,const Rect &destr);
  void _blit_YCAC_to_RGBA(const Surface &src,const Rect &srcr,const Rect &destr);
  
  void _copyMask_unknown(const Surface &src,const Surface &mask,Rect &srcr,Rect &maskr,Rect &destr);
  void _copyMask_YCYC_to_YCYC(const Surface &src,const Surface &mask,Rect &srcr,Rect &maskr,Rect &destr);
  void _copyMask_YCAC_to_YCYC(const Surface &src,const Surface &mask,Rect &srcr,Rect &maskr,Rect &destr);
  void _copyMask_YCAC_to_YCAC(const Surface &src,const Surface &mask,Rect &srcr,Rect &maskr,Rect &destr);
};

#ifndef ANDY_BUILDING_FOR_SDL
///////////////////////////////////////////////////////////////////////////////
class MainScreen : public Surface {
  U32 *buffers[2];
  S32 whichbuffer;
public:
  MainScreen();
  ~MainScreen() {}
  
  void flip();
};
#endif
///////////////////////////////////////////////////////////////////////////////

#endif
