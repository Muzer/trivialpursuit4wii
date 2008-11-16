//#define ANDY_USE_CONSOLE

#ifndef ANDY_BUILD_FOR_SDL
#include <ogcsys.h>
#endif
#include "wspr_surface.h"
#include "wspr_transform.h"

///////////////////////////////////////////////////////////////////////////////

Surface::Surface(Rect r,S32 pixfmt):usecolorkey(false) {
  if ((r.w==0)||(r.h==0)) throw ArgumentError();
  r.normalise();
  bounds=r;
  switch (this->pixfmt=pixfmt) {
    case PIXFMT_NONE: throw ArgumentError();
    case PIXFMT_YCYC: pixelsize=16; pixinword=2; break;
    case PIXFMT_YCAC: pixelsize=32; pixinword=1; break;
    case PIXFMT_MONO: pixelsize=1; pixinword=32; break;
    case PIXFMT_RGBA:
    case PIXFMT_HSVA: pixelsize=32; pixinword=1; break;
    default: throw ArgumentError();
  }
  wordsinrow=bounds.w/pixinword;
  if (bounds.w%pixinword) wordsinrow++;
  pixels=(U32*)malloc(wordsinrow*r.h*4);
  if (!pixels) throw MemoryError();
}

Surface::Surface(S32 w,S32 h,S32 pixfmt):usecolorkey(false),bounds(w,h) {
  if ((w<=0)||(h<=0)) throw ArgumentError();
  switch (this->pixfmt=pixfmt) {
    case PIXFMT_NONE: throw ArgumentError();
    case PIXFMT_YCYC: pixelsize=16; pixinword=2; break;
    case PIXFMT_YCAC: pixelsize=32; pixinword=1; break;
    case PIXFMT_MONO: pixelsize=1; pixinword=32; break;
    case PIXFMT_RGBA:
    case PIXFMT_HSVA: pixelsize=32; pixinword=1; break;
    default: throw ArgumentError();
  }
  wordsinrow=bounds.w/pixinword;
  if (bounds.w%pixinword) wordsinrow++;
  pixels=(U32*)malloc(wordsinrow*bounds.h*4);
  if (!pixels) throw MemoryError();
}

///////////////////////////////////////////////////////////////////////////////
#ifndef ANDY_BUILD_FOR_SDL
MainScreen::MainScreen() {
  GXRModeObj *vmode;
  VIDEO_Init();
  switch (VIDEO_GetCurrentTvMode()) {
    case VI_NTSC: vmode=&TVNtsc480IntDf;
                  break;
    case VI_PAL: vmode=&TVPal528IntDf;
                 break;
    case VI_MPAL: vmode=&TVMpal480IntDf;
                  break;
    default: vmode=&TVNtsc480IntDf;
  }
  VIDEO_Configure(vmode);
  bounds.set(0,0,vmode->fbWidth,vmode->efbHeight);
  buffers[0]=(U32*)MEM_K0_TO_K1(SYS_AllocateFramebuffer(vmode));
  buffers[1]=(U32*)MEM_K0_TO_K1(SYS_AllocateFramebuffer(vmode));
  if (!buffers[0]||!buffers[1]) throw MemoryError();
  
#ifdef ANDY_USE_CONSOLE  console_init(buffers[0],20,20,vmode->fbWidth,vmode->xfbHeight,vmode->fbWidth*VI_DISPLAY_PIX_SZ);
#endif
  
  VIDEO_SetNextFramebuffer(buffers[0]);
  pixels=buffers[whichbuffer=1];
  VIDEO_SetBlack(0);
  VIDEO_Flush();
  VIDEO_WaitVSync();
  if (vmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();
  wordsinrow=bounds.w/2;
  pixinword=2;
  pixfmt=PIXFMT_YCYC;
  pixelsize=16;
}

void MainScreen::flip() {
#ifndef ANDY_USE_CONSOLE
  VIDEO_SetNextFramebuffer(pixels);
  VIDEO_Flush();
  pixels=buffers[whichbuffer^=1];
  VIDEO_WaitVSync();
#endif
}
#endif
///////////////////////////////////////////////////////////////////////////////

U32 Surface::getPixel(S32 x,S32 y) const {
  if (!pixels||(x<0)||(y<0)||(x>=bounds.w)||(y>=bounds.h)) return 0;
  switch (pixfmt) {
    case PIXFMT_NONE: throw ArgumentError();
    case PIXFMT_YCYC: return _getPixel_YCYC(x,y);
    case PIXFMT_MONO: return _getPixel_MONO(x,y);
    case PIXFMT_YCAC: return pixels[y*wordsinrow+x];
    case PIXFMT_RGBA:
    case PIXFMT_HSVA: return pixels[y*wordsinrow+x];
    default: return _getPixel_unknown(x,y);
  }
}

U32 Surface::_getPixel_YCYC(S32 x,S32 y) const {
  U32 px=pixels[y*wordsinrow+x/2];
  if (x%2) return ((px&0x0000ff00)<<16)|(px&0x00ff00ff)|0x0000ff00;
  return (px|0x0000ff00);
}

U32 Surface::_getPixel_MONO(S32 x,S32 y) const {
  if (pixels[y*wordsinrow+x/32]&(0x80000000>>(x%32))) return 0xff80ff80;
  else return 0x0080ff80;
}

U32 Surface::_getPixel_unknown(S32 x,S32 y) const {
  U32 px=pixels[y*wordsinrow+x/pixinword];
  if (pixinword==1) return px;
  S32 mask=0x80000000;
  for (S32 i=0;i<pixelsize-1;i++) mask>>=1;
  U32 umask=(U32)mask;
  for (S32 i=0;i<x%pixinword;i++) umask>>=pixelsize;
  return (px&umask)>>((pixinword*pixelsize)-(x%pixinword+1)*pixelsize+(32-pixinword*pixelsize));
}

void Surface::setPixel(S32 x,S32 y,U32 px) {
  if (!pixels||(x<0)||(y<0)||(x>=bounds.w)||(y>=bounds.h)) return;
  switch (pixfmt) {
    case PIXFMT_NONE: throw ArgumentError();
    case PIXFMT_YCYC: _setPixel_YCYC(x,y,px); return;
    case PIXFMT_MONO: _setPixel_MONO(x,y,px); return;
    case PIXFMT_YCAC: pixels[y*wordsinrow+x]=px; return;
    case PIXFMT_RGBA:
    case PIXFMT_HSVA: pixels[y*wordsinrow+x]=px; return;
    default: _setPixel_unknown(x,y,px);
  }
}

void Surface::setAlphaPixel(S32 x,S32 y,U32 px,U8 alpha) {
  if (!pixels||(x<0)||(y<0)||(x>=bounds.w)||(y>=bounds.h)) return;
  switch (pixfmt) {
    case PIXFMT_NONE: throw ArgumentError();
    case PIXFMT_YCYC: _setAlphaPixel_YCYC(x,y,px,alpha); return;
    case PIXFMT_YCAC: _setAlphaPixel_YCAC(x,y,px,alpha); return;
    case PIXFMT_MONO: if (alpha>=0x80) _setPixel_MONO(x,y,px); return;
    default: if (alpha>=0x80) _setPixel_unknown(x,y,px);
  }
}

void Surface::_setPixel_YCYC(S32 x,S32 y,U32 px) {
  S32 index=y*wordsinrow+x/2;
  if (x%2) pixels[index]=((px&0xff000000)>>16)|(px&0x00ff00ff)|(pixels[index]&0xff000000);
  else pixels[index]=(px&0xffff00ff)|(pixels[index]&0x0000ff00);
}

void Surface::_setPixel_MONO(S32 x,S32 y,U32 px) {
  S32 index=y*wordsinrow+x/32;
  if ((px>>24)>=0x80) pixels[index]|=(0x80000000>>(x%32));
  else pixels[index]&=~(0x80000000>>(x%32));
}

void Surface::_setPixel_unknown(S32 x,S32 y,U32 px) {
  if (pixinword==1) {
    pixels[y*wordsinrow+x]=px;
    return;
  }
  S32 unusedbits=32-(pixinword*pixelsize);
  S32 offsetfromright=pixinword-(x%pixinword);
  px<<=(offsetfromright*pixelsize)+unusedbits;
  U32 masktmp=1,mask=0;
  masktmp<<=(offsetfromright*pixelsize)+unusedbits;
  for (S32 i=0;i<pixelsize;i++) {
    mask|=masktmp;
    masktmp<<=1;
  }
  S32 offset=y*wordsinrow+x;
  pixels[offset]&=~mask;
  pixels[offset]|=px;
}

void Surface::_setAlphaPixel_YCYC(S32 x,S32 y,U32 px,U8 alpha) {
  S32 index=y*wordsinrow+x/2;
  U32 opx=pixels[index];
  
  S32 ocb=((opx&0x00ff0000)>>16);
  S32 ncb=((px&0x00ff0000)>>16);
  ocb+=((ncb-ocb)*alpha)/255;
  if (ocb<0) ocb=0; else if (ocb>0xff) ocb=0xff;
  
  S32 ocr=opx&0x000000ff;
  S32 ncr=px&0x000000ff;
  ocr+=((ncr-ocr)*alpha)/255;
  if (ocr<0) ocr=0; else if (ocr>0xff) ocr=0xff;
  
  S32 luma=(x%2)?(opx&0x0000ff00)>>8:(opx&0xff000000)>>24;
  luma+=((((px&0xff000000)>>24)-luma)*alpha)/255;
  if (luma<0) luma=0; else if (luma>0xff) luma=0xff;
  
  if (x%2) pixels[index]=(pixels[index]&0xff000000)|(ocb<<16)|(luma<<8)|ocr;
  else pixels[index]=(luma<<24)|(ocb<<16)|(luma<<8)|ocr;
}

void Surface::_setAlphaPixel_YCAC(S32 x,S32 y,U32 px,U8 alpha) {
  S32 index=y*wordsinrow+x;
  
  S32 luma=(pixels[index]&0xff000000)>>24;
  S32 cb=(pixels[index]&0x00ff0000)>>16;
  S32 cr=(pixels[index]&0x000000ff);
  
  luma+=((((px&0xff000000)>>24)-luma)*alpha)/255;
  if (luma<0) luma=0; else if (luma>0xff) luma=0xff;
  cb+=((((px&0x00ff0000)>>16)-cb)*alpha)/255;
  if (cb<0) cb=0; else if (cb>0xff) cb=0xff;
  cr+=(((px&0x000000ff)-cr)*alpha)/255;
  if (cr<0) cr=0; else if (cr>0xff) cr=0xff;
  
  pixels[index]=(luma<<24)|(cr<<16)|(cb);
}

void Surface::setRow(S32 lox,S32 hix,S32 y,U32 px) {
  if ((y<0)||(y>=bounds.h)) return;
  if (lox<0) lox=0;
  if (hix>bounds.w) hix=bounds.w;
  if (hix<=lox) return;
  S32 offset;
  switch (pixfmt) {
    case PIXFMT_NONE: throw ArgumentError();
    case PIXFMT_YCYC: _setRow_YCYC(lox,hix,y,px); break;
    case PIXFMT_MONO: _setRow_MONO(lox,hix,y,px); break;
    case PIXFMT_YCAC: offset=y*wordsinrow; for (S32 x=lox;x<hix;x++) pixels[offset+x]=px; break;
    default: _setRow_unknown(lox,hix,y,px);
  }
}

void Surface::_setRow_YCYC(S32 lox,S32 hix,S32 y,U32 px) {
  if (lox%2) _setPixel_YCYC(lox++,y,px);
  if (hix%2) _setPixel_YCYC(--hix,y,px);
  px&=0xffff00ff;
  px|=(px&0xff000000)>>16;
  S32 offset=y*wordsinrow+lox/2;
  S32 count=(hix-lox)/2;
  for (S32 x=0;x<count;x++) pixels[offset+x]=px;
  
  pixels[1200]=0;
  pixels[1201]=0;
  pixels[1520]=0;
  pixels[1521]=0;
}

void Surface::_setRow_MONO(S32 lox,S32 hix,S32 y,U32 px) {
  while (lox%32) _setPixel_MONO(lox++,y,px); // maybe rewrite this with a single mask? is it worth the trouble?
  while (hix%32) _setPixel_MONO(--hix,y,px);
  S32 offset=y*wordsinrow+lox/32;
  S32 count=(hix-lox)/32;
  if ((px>>24)>=0x80) px=0xffffffff;
  else px=0;
  for (S32 x=0;x<count;x++) pixels[offset+count]=px;
}

///////////////////////////////////////////////////////////////////////////////

void Surface::blit(const Surface &src,const Rect &srcrs,const Rect &destrs) {
  Rect srcr(srcrs); 
  Rect destr(destrs);
  if (destr.w!=srcr.w) destr.w=srcr.w;
  if (srcr.x<0) { srcr.w+=srcr.x; destr.w+=srcr.x; destr.x-=srcr.x; srcr.x=0; }
  if (srcr.y<0) { srcr.h+=srcr.y; destr.h+=srcr.y; destr.y-=srcr.y; srcr.y=0; }
  if (srcr.right()>src.bounds.right()) { srcr.w-=(srcr.right()-src.bounds.right()); destr.w=srcr.w; }
  if (srcr.bottom()>src.bounds.bottom()) { srcr.h-=(srcr.bottom()-src.bounds.bottom()); destr.h=srcr.h; }
  if (destr.x<0) { destr.w+=destr.x; srcr.w+=destr.x; srcr.x-=destr.x; destr.x=0; }
  if (destr.y<0) { destr.h+=destr.y; srcr.h+=destr.y; srcr.y-=destr.y; destr.y=0; }
  if (destr.right()>bounds.right()) { destr.w-=(destr.right()-bounds.right()); srcr.w=destr.w; }
  if (destr.bottom()>bounds.bottom()) { destr.h-=(destr.bottom()-bounds.bottom()); srcr.h=destr.h; }
  switch (pixfmt) {
    case PIXFMT_NONE: throw ArgumentError();
    case PIXFMT_YCYC: switch (src.pixfmt) {
           case PIXFMT_NONE: throw ArgumentError();
           case PIXFMT_YCYC: _blit_YCYC_to_YCYC(src,srcr,destr); return;
           case PIXFMT_MONO: _blit_MONO_to_YCYC(src,srcr,destr); return;
           case PIXFMT_YCAC: _blit_YCAC_to_YCYC(src,srcr,destr); return;
           case PIXFMT_RGBA: _blit_RGBA_to_YCYC(src,srcr,destr); return;
           default: _blit_unknown(src,srcr,destr); return;
         }
    case PIXFMT_YCAC: switch (src.pixfmt) {
           case PIXFMT_NONE: throw ArgumentError();
           case PIXFMT_YCYC: _blit_YCYC_to_YCAC(src,srcr,destr); return;
           case PIXFMT_MONO: _blit_MONO_to_YCAC(src,srcr,destr); return;
           case PIXFMT_YCAC: _blit_straight(src,srcr,destr); return;
           case PIXFMT_RGBA: _blit_RGBA_to_YCAC(src,srcr,destr); return;
           default: if (src.pixinword==1) _blit_straight(src,srcr,destr);
                    else _blit_unknown(src,srcr,destr); return;
         }
    case PIXFMT_MONO: switch (src.pixfmt) {
           case PIXFMT_NONE: throw ArgumentError();
           case PIXFMT_YCYC: _blit_YCYC_to_MONO(src,srcr,destr); return;
           case PIXFMT_MONO: _blit_MONO_to_MONO(src,srcr,destr); return;
           case PIXFMT_YCAC: _blit_YCAC_to_MONO(src,srcr,destr); return;
           default: _blit_unknown_to_MONO(src,srcr,destr); return;
         }
    case PIXFMT_RGBA: switch (src.pixfmt) {
           case PIXFMT_NONE: throw ArgumentError();
           case PIXFMT_YCAC: _blit_YCAC_to_RGBA(src,srcr,destr); return;
           default: _blit_unknown(src,srcr,destr); return;
         }
    default: _blit_unknown(src,srcr,destr); return;
  }
}

void Surface::copyMask(const Surface &src,const Surface &mask,const Rect &srcrs,const Rect &maskrs,const Rect &destrs) {
// Only YCYC->YCYC YCAC->YCYC and YCAC->YCAC have specialised routines. everything else uses getpixel and setpixel
  Rect srcr(srcrs); 
  Rect maskr(maskrs);
  Rect destr(destrs);
  
  // unlike blit, we're strict about dimensions and format. exceptions will be thrown if the input doesn't compute
  if ((destr.w!=srcr.w)||(srcr.w!=maskr.w)||(destr.h!=srcr.h)||(srcr.h!=maskr.h)) throw ArgumentError();
  if (mask.pixfmt!=PIXFMT_MONO) throw ArgumentError();
  
  if (srcr.x<0) { srcr.w+=srcr.x; destr.w+=srcr.x; maskr.w+=srcr.x; destr.x-=srcr.x; maskr.x-=srcr.x; srcr.x=0; }
  if (srcr.y<0) { srcr.h+=srcr.y; destr.h+=srcr.y; maskr.h+=srcr.y; destr.y-=srcr.y; maskr.y-=srcr.y; srcr.y=0; }
  if (srcr.right()>src.bounds.right()) { srcr.w-=(srcr.right()-src.bounds.right()); destr.w=srcr.w; maskr.w=srcr.w; }
  if (srcr.bottom()>src.bounds.bottom()) { srcr.h-=(srcr.bottom()-src.bounds.bottom()); destr.h=srcr.h; maskr.h=srcr.h; }
  if (destr.x<0) { destr.w+=destr.x; srcr.w+=destr.x; maskr.w+=destr.x; srcr.x-=destr.x; maskr.x-=destr.x; destr.x=0; }
  if (destr.y<0) { destr.h+=destr.y; srcr.h+=destr.y; maskr.h+=destr.y; srcr.y-=destr.y; maskr.y-=destr.y; destr.y=0; }
  if (destr.right()>bounds.right()) { destr.w-=(destr.right()-bounds.right()); srcr.w=destr.w; maskr.w=destr.w; }
  if (destr.bottom()>bounds.bottom()) { destr.h-=(destr.bottom()-bounds.bottom()); srcr.h=destr.h; maskr.h=destr.h; }
  if (maskr.x<0) { srcr.w+=maskr.x; destr.w+=maskr.x; maskr.w+=maskr.x; srcr.x-=maskr.x; destr.x-=maskr.x; maskr.x=0; }
  if (maskr.y<0) { srcr.h+=maskr.y; destr.h+=maskr.y; maskr.h+=maskr.y; srcr.y-=maskr.y; destr.y-=maskr.y; maskr.y=0; }
  if (maskr.right()>bounds.right()) { maskr.w-=(maskr.right()-mask.bounds.right()); srcr.w=maskr.w; destr.w=maskr.w; }
  if (maskr.bottom()>bounds.bottom()) { maskr.h-=(maskr.bottom()-mask.bounds.bottom()); srcr.h=maskr.h; destr.h=maskr.h; }
  
  switch (pixfmt) {
    case PIXFMT_YCYC: switch (src.pixfmt) {
      case PIXFMT_YCYC: _copyMask_YCYC_to_YCYC(src,mask,srcr,maskr,destr); return;
      case PIXFMT_YCAC: _copyMask_YCAC_to_YCYC(src,mask,srcr,maskr,destr); return;
      default: _copyMask_unknown(src,mask,srcr,maskr,destr); return;
    }
    case PIXFMT_YCAC: switch (src.pixfmt) {
      case PIXFMT_YCAC: _copyMask_YCAC_to_YCAC(src,mask,srcr,maskr,destr); return;
      default: _copyMask_unknown(src,mask,srcr,maskr,destr); return;
    }
    default: _copyMask_unknown(src,mask,srcr,maskr,destr); return;
  }
}

///////////////////////////////////////////////////////////////////////////////

void Surface::_blit_unknown(const Surface &src,const Rect &srcr,const Rect &destr) {
  for (S32 x=0;x<srcr.w;x++) for (S32 y=0;y<srcr.h;y++) {
    _setPixel_unknown(x+destr.x,y+destr.y,src._getPixel_unknown(x+srcr.x,y+srcr.y));
  }
}

void Surface::_blit_straight(const Surface &src,const Rect &srcr,const Rect &destr) {
  for (S32 y=0;y<srcr.h;y++) {
    S32 srcoffset=(y+srcr.y)*src.wordsinrow+srcr.x;
    S32 destoffset=(y+destr.y)*wordsinrow+destr.x;
    for (S32 x=0;x<srcr.w;x++) pixels[destoffset+x]=src.pixels[srcoffset+x];
  }
}

void Surface::_blit_unknown_to_MONO(const Surface &src,const Rect &srcr,const Rect &destr) {
  Rect dsrcr(srcr);
  Rect ddestr(destr);
  while (dsrcr.x%32) {
    for (S32 y=0;y<dsrcr.h;y++) _setPixel_MONO(ddestr.x,y+ddestr.y,src._getPixel_unknown(dsrcr.x,y+dsrcr.y));
    dsrcr.x++;
    dsrcr.w--;
    ddestr.x++;
    ddestr.w--;
  }
  while (dsrcr.w%32) {
    S32 srcx=dsrcr.x+dsrcr.w-1;
    S32 destx=ddestr.x+ddestr.w-1;
    for (S32 y=0;y<dsrcr.h;y++) _setPixel_MONO(destx,y+ddestr.y,src._getPixel_unknown(srcx,y+dsrcr.y));
    dsrcr.w--;
    ddestr.w--;
  }
  if (dsrcr.w) for (S32 y=0;y<dsrcr.h;y++) {
//    S32 srcoffset=(y+dsrcr.y)*src.wordsinrow+dsrcr.x/src.pixinword;
    S32 destoffset=(y+ddestr.y)*wordsinrow+ddestr.x/32;
    U32 mask=0x80000000;
    U32 outpx=0;
    for (S32 x=0;x<dsrcr.left();x++) {
      U32 px=src._getPixel_unknown(x+dsrcr.x,y);
      if ((px>>24)>=0x80) outpx|=mask;
      mask>>=1;
      if (!mask) {
        mask=0x80000000;
        pixels[destoffset++]=outpx;
        outpx=0;
      }
    }
  }
}

void Surface::_blit_YCYC_to_YCYC(const Surface &src,const Rect &srcrr,const Rect &destrr) {
  if (srcrr.x%2!=destrr.x%2) { // they're not lined up right.... it's going to suck no matter what we do
    _blit_unknown(src,srcrr,destrr);
    return;
  }
  Rect srcr(srcrr),destr(destrr);
  if (destr.x%2) {
    for (S32 y=0;y<srcr.h;y++) _setPixel_YCYC(destr.x,y+destr.y,src._getPixel_YCYC(srcr.x,y+srcr.y));
    srcr.x++;
    srcr.w--;
    destr.x++;
    destr.w--;
  }
  if (destr.w%2) {
    S32 srcx=srcr.right()-1;
    S32 destx=destr.right()-1;
    for (S32 y=0;y<srcr.h;y++) _setPixel_YCYC(destx,y+destr.y,src._getPixel_YCYC(srcx,y+srcr.y));
    destr.w--;
    srcr.w--;
  }
  if (destr.w) {
    for (S32 y=0;y<srcr.h;y++) {
      S32 srcoffset=(y+srcr.y)*src.wordsinrow+srcr.x/2;
      S32 destoffset=(y+destr.y)*wordsinrow+destr.x/2;
      S32 halflen=destr.w/2;
      for (S32 x=0;x<halflen;x++) pixels[destoffset+x]=src.pixels[srcoffset+x];
    }
  }
}

void Surface::_blit_YCAC_to_YCYC(const Surface &src,const Rect &srcrr,const Rect &destrr) {
// does a quick'n dirty average of chromas... is there a better way?
// what's the right way to mix chroma?
  Rect srcr(srcrr),destr(destrr);
  U32 px;
  if (destr.x%2) {
    for (S32 y=0;y<srcr.h;y++) {
      px=src.pixels[(y+srcr.y)*src.wordsinrow+srcr.x];
      if ((src.usecolorkey)&&(px==src.colorkey)) ;
      else _setPixel_YCYC(destr.x,y+destr.y,px);
    }
    destr.x++;
    destr.w--;
    srcr.x++;
    srcr.w--;
  }
  if (destr.right()%2) {
    S32 srcx=srcr.right()-1;
    S32 destx=destr.right()-1;
    for (S32 y=0;y<srcr.h;y++) {
      px=src.pixels[(y+srcr.y)*src.wordsinrow+srcx];
      if ((src.usecolorkey)&&(px==src.colorkey)) ;
      else _setPixel_YCYC(destx,y+destr.y,px);
    }
    destr.w--;
    srcr.w--;
  }
  if (destr.w) {
    S32 halfwidth=srcr.w/2;
    for (S32 y=0;y<srcr.h;y++) {
      S32 srcoffset=(y+srcr.y)*src.wordsinrow;
      S32 destoffset=(y+destr.y)*wordsinrow;
      for (S32 x=0;x<halfwidth;x++) {
        U32 ycac1=src.pixels[srcoffset+srcr.x+x*2];
        U32 ycac2=src.pixels[srcoffset+srcr.x+x*2+1];
        if (src.usecolorkey) {
          if (ycac1==src.colorkey) {
            if (ycac2==src.colorkey) continue;
            ycac1=_getPixel_YCYC(destr.x+x*2,destr.y+y);
          } else if (ycac2==src.colorkey) ycac2=_getPixel_YCYC(destr.x+x*2+1,destr.y+y);
        }
        U32 c=(((ycac1&0x00ff00ff)+(ycac2&0x00ff00ff))>>1)|(ycac2&0x00010001); // the second OR keeps 0xff+0xff from rounding down
        U32 ycyc=(ycac1&0xff000000)|((ycac2&0xff000000)>>16)|c;
        pixels[destoffset+destr.x/2+x]=ycyc;
      }
    }
  }
}

void Surface::_blit_MONO_to_YCYC(const Surface &src,const Rect &srcrr,const Rect &destrr) {
  Rect srcr(srcrr),destr(destrr);
  U32 blackblack=0x00800080,blackwhite=0x0080ff80,whiteblack=0xff800080,whitewhite=0xff80ff80;
  if (destr.x%2) {
    for (S32 y=0;y<srcr.h;y++) _setPixel_YCYC(destr.x,destr.y+y,_getPixel_MONO(srcr.x,srcr.y+y));
    srcr.x++;
    srcr.w--;
    destr.x++;
    destr.w--;
  }
  if (destr.w%2) {
    S32 srcx=srcr.right()-1;
    S32 destx=destr.right()-1;
    for (S32 y=0;y<srcr.h;y++) _setPixel_YCYC(destx,destr.y+y,_getPixel_MONO(srcx,srcr.y+y));
    srcr.w--;
    destr.w--;
  }
  if (destr.w) {
    for (S32 y=0;y<srcr.h;y++) {
      S32 srcoffset=(srcr.y+y)*src.wordsinrow+srcr.x/32;
      S32 destoffset=(destr.y+y)*wordsinrow+destr.x/2;
      U32 mask=0x80000000>>(srcr.x%32);
      U32 srcpx=src.pixels[srcoffset];
      U32 leftcolor=0;
      for (S32 x=0;x<destr.w;x++) {
        if (srcpx&mask) {
          if (!leftcolor) leftcolor=1;
          else if (leftcolor==1) { pixels[destoffset++]=whitewhite; leftcolor=0; }
          else { pixels[destoffset++]=blackwhite; leftcolor=0; }
        } else if (!leftcolor) leftcolor=-1;
        else if (leftcolor==1) { pixels[destoffset++]=whiteblack; leftcolor=0; }
        else { pixels[destoffset++]=blackblack; leftcolor=0; }
        if (!(mask>>=1)) {
          srcpx=src.pixels[++srcoffset];
          mask=0x80000000;
        }
      }
    }
  }
}

void Surface::_blit_RGBA_to_YCYC(const Surface &src,const Rect &srcr,const Rect &destr) {
  for (S32 x=0;x<srcr.w;x++) for (S32 y=0;y<srcr.h;y++) {
    _setPixel_YCYC(x+destr.x,y+destr.y,wspr_RGBA_to_YCAC(src.pixels[(srcr.y+y)*src.wordsinrow+srcr.x+x]));
  }
}

void Surface::_blit_RGBA_to_YCAC(const Surface &src,const Rect &srcr,const Rect &destr) {
  for (S32 x=0;x<srcr.w;x++) for (S32 y=0;y<srcr.h;y++) {
    pixels[(y+destr.y)*wordsinrow+x]=wspr_RGBA_to_YCAC(src.pixels[(srcr.y+y)*src.wordsinrow+srcr.x+x]);
  }
}

void Surface::_blit_YCYC_to_YCAC(const Surface &src,const Rect &srcr,const Rect &destr) {
  for (S32 y=0;y<srcr.h;y++) {
    S32 srcoffset=(srcr.y+y)*src.wordsinrow+srcr.x/2;
    S32 destoffset=(destr.y+y)*wordsinrow+destr.x;
    for (S32 x=0;x<srcr.w;x++) {
      U32 px=src.pixels[srcoffset+x/2];
      if (x%2) pixels[destoffset+x]=((px&0x0000ff00)<<16)|(px&0x00ff00ff)|0x0000ff00;
      else pixels[destoffset+x]=(px|0x0000ff00);
    }
  }
}

void Surface::_blit_MONO_to_YCAC(const Surface &src,const Rect &srcr,const Rect &destr) {
  U32 black=0x0080ff80,white=0xff80ff80;
  for (S32 y=0;y<srcr.h;y++) {
    U32 mask=(0x80000000>>(srcr.x%32));
    S32 srcoffset=(srcr.y+y)*src.wordsinrow+srcr.x/32;
    U32 px=src.pixels[srcoffset];
    S32 destoffset=(destr.y+y)*wordsinrow+destr.x;
    for (S32 x=0;x<srcr.w;x++) {
      if (px&mask) pixels[destoffset++]=white;
      else pixels[destoffset++]=black;
      if (!(mask>>=1)) {
        mask=0x80000000;
        px=src.pixels[++srcoffset];
      }
    }
  }
}

void Surface::_blit_YCYC_to_MONO(const Surface &src,const Rect &srcr,const Rect &destr) {
  for (S32 y=0;y<srcr.h;y++) {
    S32 srcoffset=(srcr.y+y)*src.wordsinrow+srcr.x/2;
    S32 destoffset=(destr.y+y)*wordsinrow+destr.x/32;
    U32 mask=0;
    U32 px=0;
    U32 tmpmask=(0x80000000>>(srcr.x%32));
    for (S32 x=0;x<srcr.w;x++) {
      bool bit;
      if (x%2) bit=(src.pixels[srcoffset+x/2]&0x00008000);
      else bit=(src.pixels[srcoffset+x/2]&0x80000000);
      if (bit) px|=tmpmask;
      mask|=tmpmask;
      if ((x==srcr.w-1)||(!(tmpmask>>=1))) {
        if (mask==0xffffffff) pixels[++destoffset]=px;
        else {
          destoffset++;
          pixels[destoffset]=(pixels[destoffset]&~mask)|(px&mask);
        }
      }
    }
  }
}

void Surface::_blit_YCAC_to_MONO(const Surface &src,const Rect &srcr,const Rect &destr) {
  for (S32 y=0;y<srcr.h;y++) {
    S32 srcoffset=(srcr.y+y)*src.wordsinrow+srcr.x;
    S32 destoffset=(destr.y+y)*wordsinrow+destr.x/32;
    U32 mask=0;
    U32 px=0;
    U32 tmpmask=(0x80000000>>(srcr.x%32));
    for (S32 x=0;x<srcr.w;x++) {
      if (src.pixels[srcoffset+x]&0x80000000) px|=tmpmask;
      mask|=tmpmask;
      if ((x==srcr.w-1)||(!(tmpmask>>=1))) {
        if (mask==0xffffffff) pixels[++destoffset]=px;
        else {
          destoffset++;
          pixels[destoffset]=(pixels[destoffset]&~mask)|(px&mask);
        }
      }
    }
  }
}

void Surface::_blit_MONO_to_MONO(const Surface &src,const Rect &srcrr,const Rect &destrr) {
  Rect srcr(srcrr),destr(destrr);
  if (srcr.x%32) {
    S32 keepmask=0x80000000;
    for (S32 i=0;i<srcr.x%32-1;i++) keepmask>>=1;
    U32 newmask=~keepmask;
    for (S32 y=0;y<srcr.h;y++) {
      S32 destoffset=(destr.y+y)*wordsinrow+destr.x/32;
      S32 srcoffset=(srcr.y+y)*wordsinrow+srcr.x/32;
      pixels[destoffset]=(pixels[destoffset]&keepmask)|(pixels[srcoffset]&newmask);
    }
    destr.x+=srcr.x%32;
    destr.w-=srcr.x%32;
    srcr.w-=(srcr.x%32);
    srcr.x=(srcr.x/32+1)*32;
  }
  if (srcr.w%32) {
    S32 newmask=0x80000000;
    for (S32 i=0;i<srcr.x%32-1;i++) newmask>>=1;
    U32 keepmask=~newmask;
    for (S32 y=0;y<srcr.h;y++) {
      S32 destoffset=(destr.y+y)*wordsinrow+(destr.right()-1)/32;
      S32 srcoffset=(srcr.y+y)*wordsinrow+(srcr.right()-1)/32;
      pixels[destoffset]=(pixels[destoffset]&keepmask)|(pixels[srcoffset]&newmask);
    }
    destr.w=(destr.w/32)*32;
    srcr.w=destr.w;
  }
  if (srcr.w) {
    for (S32 y=0;y<srcr.h;y++) {
      S32 srcoffset=(y+srcr.y)*src.wordsinrow+srcr.x/32;
      S32 destoffset=(y+destr.y)*wordsinrow+destr.x/32;
      for (S32 x=0;x<srcr.w/32;x++) pixels[destoffset+x]=src.pixels[srcoffset+x];
    }
  }
}

void Surface::_blit_YCAC_to_RGBA(const Surface &src,const Rect &srcr,const Rect &destr) {
  for (S32 y=0;y<srcr.h;y++) {
    S32 srcoffset=(y+srcr.y)*src.wordsinrow+srcr.x;
    S32 destoffset=(y+destr.y)*wordsinrow+destr.x;
    for (S32 x=0;x<srcr.w;x++) {
      pixels[destoffset+x]=wspr_YCAC_to_RGBA(src.pixels[srcoffset+x]);
  }}
}

///////////////////////////////////////////////////////////////////////////////

void Surface::_copyMask_unknown(const Surface &src,const Surface &mask,Rect &srcr,Rect &maskr,Rect &destr) {
// this could be optimised by using the unchecked get/set pixel routines.....
  for (S32 y=0;y<srcr.h;y++) {
  
    S32 maskindex=(y+maskr.y)*mask.wordsinrow+maskr.x/32;
    U32 maskbits=mask.pixels[maskindex];
    U32 qmask=0x8000000>>(maskr.x%32);
    
    for (S32 x=0;x<srcr.w;x++) {
      if (maskbits&qmask) setPixel(x+destr.x,y+destr.y,src.getPixel(x+srcr.x,y+srcr.y));
      if (!(qmask>>=1)) {
        qmask=0x80000000;
        maskbits=mask.pixels[++maskindex];
      }
    }
  }
}

void Surface::_copyMask_YCYC_to_YCYC(const Surface &src,const Surface &mask,Rect &srcr,Rect &maskr,Rect &destr) {
  _copyMask_unknown(src,mask,srcr,maskr,destr);
}

void Surface::_copyMask_YCAC_to_YCYC(const Surface &src,const Surface &mask,Rect &srcr,Rect &maskr,Rect &destr) {
  _copyMask_unknown(src,mask,srcr,maskr,destr);
}

void Surface::_copyMask_YCAC_to_YCAC(const Surface &src,const Surface &mask,Rect &srcr,Rect &maskr,Rect &destr) {
  for (S32 y=0;y<srcr.h;y++) {
  
    S32 maskindex=(y+maskr.y)*mask.wordsinrow+maskr.x/32;
    U32 maskbits=mask.pixels[maskindex];
    U32 qmask=0x8000000>>(maskr.x%32);
    S32 srcoffset=(y+srcr.y)*src.wordsinrow+srcr.x;
    S32 destoffset=(y+destr.y)*wordsinrow+destr.x;
    
    for (S32 x=0;x<srcr.w;x++) {
      if (maskbits&qmask) pixels[destoffset+x]=src.pixels[srcoffset+x];
      if (!(qmask>>=1)) {
        qmask=0x80000000;
        maskbits=mask.pixels[++maskindex];
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////

void Surface::clear(U32 px) {
  // should set the pixel more intelligently -- write that later
  S32 limit=wordsinrow*bounds.h;
  for (S32 i=0;i<limit;i++) pixels[i]=px;
}
