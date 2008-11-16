#include <stdarg.h>
#include <math.h>
#include "wspr_types.h"
#include "wspr_surface.h"
#include "wspr_geometry.h"
#include "wspr_transform.h"

void wspr_invert(Surface *surf) {
  for (S32 i=0;i<surf->wordsinrow*surf->bounds.h;i++) surf->pixels[i]=~surf->pixels[i];
}

void wspr_invertValue(Surface *surf) {
  S32 i,x,y;
  switch (surf->pixfmt) {
    case PIXFMT_MONO: for (i=0;i<surf->wordsinrow*surf->bounds.h;i++) surf->pixels[i]=~surf->pixels[i]; break;
    case PIXFMT_YCYC: for (i=0;i<surf->wordsinrow*surf->bounds.h;i++) surf->pixels[i]=(surf->pixels[i]^0xff00ff00); break;
    case PIXFMT_YCAC: for (i=0;i<surf->wordsinrow*surf->bounds.h;i++) surf->pixels[i]=(surf->pixels[i]^0xff000000); break;
    default: for (y=0;y<surf->bounds.h;y++) for (x=0;x<surf->bounds.w;x++) surf->setPixel(x,y,surf->getPixel(x,y)^0xff000000); break;
  }
}

void wspr_adjustSaturation(Surface *surf,S8 adjust) {
  if (surf->pixfmt==PIXFMT_MONO) return;
  if (adjust==0) return;
  if ((surf->pixfmt==PIXFMT_YCYC)||(surf->pixfmt==PIXFMT_YCAC)) {
    for (S32 i=0;i<surf->wordsinrow*surf->bounds.h;i++) {
      U32 px=surf->pixels[i];
      S32 cb=((px&0x00ff0000)>>16)-128;
      if (adjust>0) {
        if (cb>0) cb+=(adjust*(128-cb))/128;
        else if (cb<0) cb+=(adjust*(cb-128))/128;
      } else {
        if (cb>0) cb+=(adjust*cb)/128;
        else if (cb<0) cb-=(adjust*cb)/128;
      }
      if (cb<-128) cb=-128; else if (cb>127) cb=127;
      S32 cr=px&0x000000ff-128;
      if (adjust>0) {
        if (cr>0) cr+=(adjust*(128-cr))/128;
        else if (cr<0) cr+=(adjust*(cr-128))/128;
      } else {
        if (cr>0) cr+=(adjust*cr)/128;
        else if (cr<0) cr-=(adjust*cr)/128;
      }
      U8 cbb=cb,crb=cr;
      surf->pixels[i]=(px&0xff00ff00)|(cbb<<16)|(crb);
    }
  } else {
    for (S32 y=0;y<surf->bounds.h;y++) for (S32 x=0;y<surf->bounds.w;x++) {
      U32 px=surf->getPixel(x,y);
      S32 cb=((px&0x00ff0000)>>16)-128;
      if (adjust>0) {
        if (cb>0) cb+=(adjust*(128-cb))/128;
        else if (cb<0) cb+=(adjust*(cb-128))/128;
      } else {
        if (cb>0) cb+=(adjust*cb)/128;
        else if (cb<0) cb-=(adjust*cb)/128;
      }
      if (cb<-128) cb=-128; else if (cb>127) cb=127;
      S32 cr=px&0x000000ff-128;
      if (adjust>0) {
        if (cr>0) cr+=(adjust*(128-cr))/128;
        else if (cr<0) cr+=(adjust*(cr-128))/128;
      } else {
        if (cr>0) cr+=(adjust*cr)/128;
        else if (cr<0) cr-=(adjust*cr)/128;
      }
      U8 cbb=cb,crb=cr;
      surf->setPixel(x,y,(px&0xff00ff00)|(cbb<<16)|(crb));
    }
  }
}

void wspr_adjustValue(Surface *surf,S8 adjust) {
  switch (surf->pixfmt) {
    case PIXFMT_MONO: {  S32 len=surf->wordsinrow*surf->bounds.h;
                         if (adjust>0) for (S32 i=0;i<len;i++) surf->pixels[i]=1;
                         else if (adjust<0) for (S32 i=0;i<len;i++) surf->pixels[i]=0;
                      } break;
    case PIXFMT_HSVA: {  S32 len=surf->wordsinrow*surf->bounds.h;
                         for (S32 i=0;i<len;i++) {
                           U32 px=surf->pixels[i];
                           S32 v=(px&0x0000ff00)>>8;
                           if (adjust>0) { v+=((255-v)*127)/adjust; if (v>255) v=255; }
                           else { v+=(v*128)/adjust; if (v<0) v=0; }
                           surf->pixels[i]=(px&0xffff00ff)|(v<<8);
                         }
                      } break;
    case PIXFMT_YCYC: {  S32 len=surf->wordsinrow*surf->bounds.h;
                         for (S32 i=0;i<len;i++) {
                           U32 px=surf->pixels[i];
                           S32 v1=(px>>24);
                           if (adjust>0) { v1+=((255-v1)*127)/adjust; if (v1>255) v1=255; }
                           else { v1+=(v1*128)/adjust; if (v1<0) v1=0; }
                           S32 v2=(px&0x0000ff00)>>8;
                           if (adjust>0) { v2+=((255-v2)*127)/adjust; if (v2>255) v2=255; }
                           else { v2+=(v2*128)/adjust; if (v2<0) v2=0; }
                           surf->pixels[i]=(px&0x00ff00ff)|(v1<<24)|(v2<<8);
                         }
                      } break;
    case PIXFMT_YCAC: {  S32 len=surf->wordsinrow*surf->bounds.h;
                         for (S32 i=0;i<len;i++) {
                           U32 px=surf->pixels[i];
                           S32 v=(px>>24);
                           if (adjust>0) { v+=((255-v)*127)/adjust; if (v>255) v=255; }
                           else { v+=(v*128)/adjust; if (v<0) v=0; }
                           surf->pixels[i]=(px&0x00ffffff)|(v<<24);
                         }
                      } break;
    case PIXFMT_RGBA: {  S32 len=surf->wordsinrow*surf->bounds.h;
                         for (S32 i=0;i<len;i++) {
                           U32 px=surf->pixels[i];
                           S32 r=px>>24;
                           S32 g=(px&0x00ff0000)>>16;
                           S32 b=(px&0x0000ff00)>>8;
                           if (adjust>0) { 
                             S32 lo=MIN(r,g); lo=MIN(lo,b);
                             r+=((255-lo)*127)/adjust; if (r>255) r=255;
                             g+=((255-lo)*127)/adjust; if (g>255) g=255;
                             b+=((255-lo)*127)/adjust; if (b>255) b=255;
                           } else { 
                             S32 hi=MAX(r,g); hi=MAX(hi,b);
                             r+=(hi*128)/adjust; if (r<0) r=0;
                             g+=(hi*128)/adjust; if (g<0) g=0;
                             b+=(hi*128)/adjust; if (b<0) b=0;
                           }
                           surf->pixels[i]=(r<<24)|(g<<16)|(b<<8)|(px&0x000000ff);
                         }
                      } break;
    case PIXFMT_NONE: throw ArgumentError();
    default: {  for (S32 y=0;y<surf->bounds.h;y++) for (S32 x=0;x<surf->bounds.w;x++) {
                  U32 px=surf->getPixel(x,y);
                  S32 v=(px>>24);
                  if (adjust>0) { v+=((255-v)*127)/adjust; if (v>255) v=255; }
                  else { v+=(v*128)/adjust; if (v<0) v=0; }
                  surf->setPixel(x,y,(px&0x00ffffff)|(v<<24));
                }
             }
  }
}

void wspr_adjustContrast(Surface *surf,S8 adjust) {
  if (surf->pixfmt==PIXFMT_NONE) throw ArgumentError();
  if (surf->pixfmt==PIXFMT_MONO) return; // contrast in mono is always 100%
  if (!adjust) return;
  S32 len=surf->wordsinrow*surf->bounds.h;
  for (S32 i=0;i<len;i++) {
    U32 px=surf->pixels[i];
    switch (surf->pixfmt) {
      case PIXFMT_YCAC: {  S32 y=px>>24;
                           S32 cb=(px&0x00ff0000)>>16;
                           S32 cr=(px&0x000000ff);
                           if (adjust>0) {
                             if (y>=0x80) { y+=((255-y)*adjust)/127; if (y>255) y=255; }
                             else { y-=(y*adjust)/127; if (y<0) y=0; }
                             if (cb>=0x80) { cb+=((255-cb)*adjust)/127; if (cb>255) cb=255; }
                             else { cb-=(cb*adjust)/127; if (cb<0) cb=0; }
                             if (cr>=0x80) { cr+=((255-cr)*adjust)/127; if (cr>255) cr=255; }
                             else { cr-=(cr*adjust)/127; if (cr<0) cr=0; }
                           } else {
                             if (y>=0x80) { y+=((y-0x80)*adjust)/128; }
                             else { y-=((0x80-y)*adjust)/128; }
                             if (cb>=0x80) { cb+=((cb-0x80)*adjust)/128; }
                             else { cb-=((0x80-cb)*adjust)/128; }
                             if (cr>=0x80) { cr+=((cr-0x80)*adjust)/128; }
                             else { cr-=((0x80-cr)*adjust)/128; }
                           }
                        } break;
      case PIXFMT_YCYC: {  S32 y=px>>24;
                           S32 cb=(px&0x00ff0000)>>16;
                           S32 y2=(px&0x0000ff00)>>8;
                           S32 cr=(px&0x000000ff);
                           if (adjust>0) {
                             if (y>=0x80) { y+=((255-y)*adjust)/127; if (y>255) y=255; }
                             else { y-=(y*adjust)/127; if (y<0) y=0; }
                             if (y2>=0x80) { y2+=((255-y2)*adjust)/127; if (y2>255) y2=255; }
                             else { y2-=(y2*adjust)/127; if (y2<0) y2=0; }
                             if (cb>=0x80) { cb+=((255-cb)*adjust)/127; if (cb>255) cb=255; }
                             else { cb-=(cb*adjust)/127; if (cb<0) cb=0; }
                             if (cr>=0x80) { cr+=((255-cr)*adjust)/127; if (cr>255) cr=255; }
                             else { cr-=(cr*adjust)/127; if (cr<0) cr=0; }
                           } else {
                             if (y>=0x80) { y+=((y-0x80)*adjust)/128; }
                             else { y-=((0x80-y)*adjust)/128; }
                             if (y2>=0x80) { y2+=((y2-0x80)*adjust)/128; }
                             else { y2-=((0x80-y2)*adjust)/128; }
                             if (cb>=0x80) { cb+=((cb-0x80)*adjust)/128; }
                             else { cb-=((0x80-cb)*adjust)/128; }
                             if (cr>=0x80) { cr+=((cr-0x80)*adjust)/128; }
                             else { cr-=((0x80-cr)*adjust)/128; }
                           }
                        } break;
      case PIXFMT_HSVA: {  S32 h=px>>24;
                           S32 s=(px&0x00ff0000)>>16;
                           S32 v=(px&0x0000ff)>>8;
                           if (adjust>0) {
                             if (h>=0x80) { h+=((255-h)*adjust)/127; if (h>255) h=255; }
                             else { h-=(h*adjust)/127; if (h<0) h=0; }
                             if (s>=0x80) { s+=((255-s)*adjust)/127; if (s>255) s=255; }
                             else { s-=(s*adjust)/127; if (s<0) s=0; }
                             if (v>=0x80) { v+=((255-v)*adjust)/127; if (v>255) v=255; }
                             else { v-=(v*adjust)/127; if (v<0) v=0; }
                           } else {
                             if (h>=0x80) { h+=((h-0x80)*adjust)/128; }
                             else { h-=((0x80-h)*adjust)/128; }
                             if (s>=0x80) { s+=((s-0x80)*adjust)/128; }
                             else { s-=((0x80-s)*adjust)/128; }
                             if (v>=0x80) { v+=((v-0x80)*adjust)/128; }
                             else { v-=((0x80-v)*adjust)/128; }
                           }
                        } break;
      case PIXFMT_RGBA: {  S32 r=px>>24;
                           S32 g=(px&0x00ff0000)>>16;
                           S32 b=(px&0x0000ff00)>>8;
                           if (adjust>0) {
                             if (r>=0x80) { r+=((255-r)*adjust)/127; if (r>255) r=255; }
                             else { r-=(r*adjust)/127; if (r<0) r=0; }
                             if (g>=0x80) { g+=((255-g)*adjust)/127; if (g>255) g=255; }
                             else { g-=(g*adjust)/127; if (g<0) g=0; }
                             if (b>=0x80) { b+=((255-b)*adjust)/127; if (b>255) b=255; }
                             else { b-=(b*adjust)/127; if (b<0) b=0; }
                           } else {
                             if (r>=0x80) { r+=((r-0x80)*adjust)/128; }
                             else { r-=((0x80-r)*adjust)/128; }
                             if (g>=0x80) { g+=((g-0x80)*adjust)/128; }
                             else { g-=((0x80-g)*adjust)/128; }
                             if (b>=0x80) { b+=((b-0x80)*adjust)/128; }
                             else { b-=((0x80-b)*adjust)/128; }
                           }
                        } break;
    }
    surf->pixels[i]=px;
  }
}

void wspr_tint(Surface *surf,U32 c,U8 amt) {
  if (surf->pixfmt==PIXFMT_NONE) throw ArgumentError();
  if (surf->pixfmt==PIXFMT_MONO) return;
  switch (surf->pixfmt) {
    case PIXFMT_YCAC:
    case PIXFMT_YCYC:
    case PIXFMT_HSVA:
    case PIXFMT_RGBA: {  S32 len=surf->wordsinrow*surf->bounds.h;
                         S32 c0=(c>>24);
                         S32 c1=(c&0x00ff0000)>>16;
                         S32 c2=(c&0x0000ff00)>>8;
                         S32 c3=(c&0x000000ff);
                         for (S32 i=0;i<len;i++) {
                           U32 px=surf->pixels[i];
                           S32 b0=(px>>24);
                           S32 b1=(px&0x00ff0000)>>16;
                           S32 b2=(px&0x0000ff00)>>8;
                           S32 b3=(px&0x000000ff);
                           b0+=((c0-b0)*amt)/255; if (b0<0) b0=0; else if (b0>255) b0=255;
                           b1+=((c1-b1)*amt)/255; if (b1<0) b1=0; else if (b1>255) b1=255;
                           b2+=((c2-b2)*amt)/255; if (b2<0) b2=0; else if (b2>255) b2=255;
                           b3+=((c3-b3)*amt)/255; if (b3<0) b3=0; else if (b3>255) b3=255;
                           surf->pixels[i]=(b0<<24)|(b1<<16)|(b2<<8)|b3;
                         }
                      } break;
    default: { for (S32 y=0;y<surf->bounds.h;y++) for (S32 x=0;x<surf->bounds.w;x++) {
                 U32 px=surf->getPixel(x,y);
                 surf->setPixel(x,y,px+((c-px)*amt)/255);
               }
             }
  }
}

///////////////////////////////////////////////////////////////////////////////
// COLOR CONVERSIONS ------------------------------------------------------- //

#define RED_HUE 0
#define GREEN_HUE 86
#define BLUE_HUE 173

U32 wspr_YCAC_to_HSVA(U32 c) {
  S32 a=(c&0x0000ff00)>>8;
  S32 luma=(c&0xff000000)>>24;
  S32 cb=((c&0x00ff0000)>>16)-128;
  S32 cr=(c&0x000000ff)-128;
  
  S32 v=luma;
  S32 s=lround(sqrt(cb*cb+cr*cr)*2);
  S32 h=lround((atan2(cr,cb)*255)/twopi)-64;
  
  while (h<0) h+=256;
  while (h>255) h-=256;
  if (s<0) s=0; else if (s>255) s=255;
  if (v<0) v=0; else if (v>255) v=255;
  return (h<<24)|(s<<16)|(v<<8)|a;
}

U32 wspr_HSVA_to_YCAC(U32 c) {
  S32 a=(c&0x000000ff);
  S32 h=((c&0xff000000)>>24)+64;
  S32 s=(c&0x00ff0000)>>16;
  S32 v=(c&0x0000ff00)>>8;
  
  U32 luma=v;
  s/=2;
  FP64 theta=(h*twopi)/255;
  S32 cb=lround(cos(theta)*s);
  S32 cr=lround(sin(theta)*s);
  cb+=128;
  cr+=128;
  
  if (luma<0) luma=0; else if (luma>255) luma=255;
  if (cb<0) cb=0; else if (cb>255) cb=255;
  if (cr<0) cr=0; else if (cr>255) cr=255;
  return (luma<<24)|(cb<<16)|(a<<8)|cr;
}

U32 wspr_HSVA_to_RGBA(U32 c) {
  S32 h=(c&0xff000000)>>24;
  S32 s=(c&0x00ff0000)>>16;
  S32 v=(c&0x0000ff00)>>8;
  S32 a=(c&0x000000ff);
  
  S32 ch1=v;
  S32 ch2;
  S32 ch3=v-s;
  if ((h/43)%2) ch2=ch1-((h%43+4)*s)/43;
  else ch2=ch1-((43-h%43+4)*s)/43;
  
  S32 r=0,g=0,b=0;
  switch (h/43) {
    case 0: r=ch1; g=ch2; b=ch3; break; // 1 2 3
    case 1: r=ch2; g=ch1; b=ch3; break; // 2 1 3
    case 2: r=ch3; g=ch1; b=ch2; break; // 3 1 2 -- 3 2 3
    case 3: r=ch3; g=ch2; b=ch1; break; // 3 2 1 -- 3 2 3
    case 4: r=ch2; g=ch3; b=ch1; break; // 2 3 1
    case 5: r=ch1; g=ch3; b=ch2; break; // 1 3 2
  }
  
//  printf("%3d %3d %3d   -->   %3d %3d %3d\n",h,s,v,r,g,b);
  
  if (r<0) r=0; else if (r>255) r=255;
  if (g<0) g=0; else if (g>255) g=255;
  if (b<0) b=0; else if (b>255) b=255;
  return (r<<24)|(g<<16)|(b<<8)|a;
}

U32 wspr_RGBA_to_HSVA(U32 c) {
  S32 r=(c&0xff000000)>>24;
  S32 g=(c&0x00ff0000)>>16;
  S32 b=(c&0x0000ff00)>>8;
  S32 a=(c&0x000000ff);
  
  S32 h=0,s=0,v=0;
  if ((r==b)&&(b==g)) { v=r;
  } else {
  
    S32 ch1=0,ch2=0,ch3=0,sector=0;
    if (r>b) {
      if (r>g) { 
        ch1=r;
        if (b>g) { ch2=b; ch3=g; sector=5; }
        else { ch2=g; ch3=b; sector=0; }
      } else { ch1=g; ch2=r; ch3=b; sector=1; }
    } else if (b>g) {
      ch1=b;
      if (r>g) { ch2=r; ch3=g; sector=4; }
      else { ch2=g; ch3=r; sector=3; }
    } else { ch1=g; ch2=b; ch3=r; sector=2; }
    
    if (sector%2) h=sector*43+((ch1-ch2)*43)/(ch1);
    else h=(sector+1)*43-((ch1-ch2)*43)/(ch1);
    s=ch1-ch3;
    v=ch1;
    
  }
//  printf("%3d %3d %3d   <--   %3d %3d %3d\n",h,s,v,r,g,b);
  
  while (h<0) h+=256;
  while (h>255) h-=256;
  if (s<0) s=0; else if (s>255) s=255;
  if (v<0) v=0; else if (v>255) v=255;
  return (h<<24)|(s<<16)|(v<<8)|a;
}

U32 wspr_YCAC_to_RGBA(U32 c) { return wspr_HSVA_to_RGBA(wspr_YCAC_to_HSVA(c)); }
U32 wspr_RGBA_to_YCAC(U32 c) { return wspr_HSVA_to_YCAC(wspr_RGBA_to_HSVA(c)); }

U32 wspr_colorMix(U32 a,U32 b,U8 amt,S32 pixfmt) {
  if (amt==0) return a;
  if (amt==255) return b;
  switch (pixfmt) { case PIXFMT_YCYC:
                    case PIXFMT_RGBA:
                    case PIXFMT_HSVA:
                    case PIXFMT_YCAC: { S32 a0,a1,a2,a3,b0,b1,b2,b3;
                                        a0=(a>>24);
                                        a1=(a&0x00ff0000)>>16;
                                        a2=(a&0x0000ff00)>>8;
                                        a3=(a&0x000000ff);
                                        b0=(b>>24);
                                        b1=(b&0x00ff0000)>>16;
                                        b2=(b&0x0000ff00)>>8;
                                        b3=(b&0x000000ff);
                                        a0+=((b0-a0)*amt)/255; if (a0>255) a0=255; else if (a0<0) a0=0;
                                        a1+=((b1-a1)*amt)/255; if (a1>255) a1=255; else if (a1<0) a1=0;
                                        a2+=((b2-a2)*amt)/255; if (a2>255) a2=255; else if (a2<0) a2=0;
                                        a3+=((b3-a3)*amt)/255; if (a3>255) a3=255; else if (a3<0) a3=0;
                                        return (a0<<24)|(a1<<16)|(a2<<8)|a3;
                                      } 
                    case PIXFMT_MONO: if (a||b) return 1; else return 0;
                    case PIXFMT_NONE: throw ArgumentError();
                    default: return (a+b)/2;
  }
}

///////////////////////////////////////////////////////////////////////////////
// SURFACE CONVERSIONS ----------------------------------------------------- //

Surface *wspr_YCAC_to_RGBA_surf(Surface *surf) {
  Surface *nsurf=new Surface(surf->bounds.w,surf->bounds.h,PIXFMT_RGBA);
  if (!nsurf) throw MemoryError();
  S32 len=surf->wordsinrow*surf->bounds.h; // same for both
  for (S32 i=0;i<len;i++) nsurf->pixels[i]=wspr_YCAC_to_RGBA(surf->pixels[i]);
  return nsurf;
}

Surface *wspr_RGBA_to_YCAC_surf(Surface *surf) {
  Surface *nsurf=new Surface(surf->bounds.w,surf->bounds.h,PIXFMT_YCAC);
  if (!nsurf) throw MemoryError();
  S32 len=surf->wordsinrow*surf->bounds.h; // same for both
  for (S32 i=0;i<len;i++) nsurf->pixels[i]=wspr_RGBA_to_YCAC(surf->pixels[i]);
  return nsurf;
}

Surface *wspr_YCAC_to_HSVA_surf(Surface *surf) {
  Surface *nsurf=new Surface(surf->bounds.w,surf->bounds.h,PIXFMT_HSVA);
  if (!nsurf) throw MemoryError();
  S32 len=surf->wordsinrow*surf->bounds.h; // same for both
  for (S32 i=0;i<len;i++) nsurf->pixels[i]=wspr_YCAC_to_HSVA(surf->pixels[i]);
  return nsurf;
}

Surface *wspr_HSVA_to_YCAC_surf(Surface *surf) {
  Surface *nsurf=new Surface(surf->bounds.w,surf->bounds.h,PIXFMT_YCAC);
  if (!nsurf) throw MemoryError();
  S32 len=surf->wordsinrow*surf->bounds.h; // same for both
  for (S32 i=0;i<len;i++) nsurf->pixels[i]=wspr_HSVA_to_YCAC(surf->pixels[i]);
  return nsurf;
}

Surface *wspr_RGBA_to_HSVA_surf(Surface *surf) {
  Surface *nsurf=new Surface(surf->bounds.w,surf->bounds.h,PIXFMT_HSVA);
  if (!nsurf) throw MemoryError();
  S32 len=surf->wordsinrow*surf->bounds.h; // same for both
  for (S32 i=0;i<len;i++) nsurf->pixels[i]=wspr_RGBA_to_HSVA(surf->pixels[i]);
  return nsurf;
}

Surface *wspr_HSVA_to_RGBA_surf(Surface *surf) {
  Surface *nsurf=new Surface(surf->bounds.w,surf->bounds.h,PIXFMT_RGBA);
  if (!nsurf) throw MemoryError();
  S32 len=surf->wordsinrow*surf->bounds.h; // same for both
  for (S32 i=0;i<len;i++) nsurf->pixels[i]=wspr_HSVA_to_RGBA(surf->pixels[i]);
  return nsurf;
}

Surface *wspr_YCYC_to_YCAC_surf(Surface *surf) {
  Surface *nsurf=new Surface(surf->bounds.w,surf->bounds.h,PIXFMT_YCAC);
  if (!nsurf) throw MemoryError();
  S32 len=nsurf->wordsinrow*nsurf->bounds.h;
  U32 px=0;
  for (S32 i=0;i<len;i++) {
    if (!(i%2)) {
      px=surf->pixels[i/2];
      nsurf->pixels[i]=px|0x0000ff00;
    } else {
      nsurf->pixels[i]=(px&0x00ff00ff)|0x0000ff00|((px&0x0000ff00)<<16);
    }
  }
  return nsurf;
}

Surface *wspr_YCYC_to_MONO_surf(Surface *surf) {
  Surface *nsurf=new Surface(surf->bounds.w,surf->bounds.h,PIXFMT_MONO);
  if (!nsurf) throw MemoryError();
  for (S32 y=0;y<surf->bounds.h;y++) {
    U32 opx=0;
    U32 omask=0x80000000;
    S32 srcoffset=y*surf->wordsinrow;
    S32 destoffset=y*nsurf->wordsinrow;
    for (S32 x=0;x<surf->bounds.w;x++) {
      bool on;
      if (x%2) on=surf->pixels[srcoffset+x/2]&0x00008000;
      else on=surf->pixels[srcoffset+x/2]&0x80000000;
      if (on) opx|=omask;
      if (!(omask>>=1)) {
        nsurf->pixels[destoffset++]=opx;
        opx=0;
        omask=0x80000000;
      }
    }
    if (surf->bounds.w%32) nsurf->pixels[destoffset]=opx;
  }
  return nsurf;
}

Surface *wspr_YCAC_to_YCYC_surf(Surface *surf) {
  Surface *nsurf=new Surface(surf->bounds.w,surf->bounds.h,PIXFMT_YCYC);
  if (!nsurf) throw MemoryError();
  S32 len=surf->wordsinrow*surf->bounds.h;
  S32 destoffset=0;
  U32 px=0;
  for (S32 i=0;i<len;i++) {
    if (i%2) nsurf->pixels[destoffset++]=px|((surf->pixels[i]&0xff000000)>>16);
    else px=surf->pixels[i]&0xffff00ff;
  }
  return nsurf;
}

Surface *wspr_YCAC_to_MONO_surf(Surface *surf) {
  Surface *nsurf=new Surface(surf->bounds.w,surf->bounds.h,PIXFMT_MONO);
  if (!nsurf) throw MemoryError();
  for (S32 y=0;y<surf->bounds.h;y++) {
    U32 opx=0;
    U32 omask=0x80000000;
    S32 srcoffset=y*surf->wordsinrow;
    S32 destoffset=y*nsurf->wordsinrow;
    for (S32 x=0;x<surf->bounds.w;x++) {
      if (surf->pixels[srcoffset+x]&0x80000000) opx|=omask;
      if (!(omask>>=1)) {
        nsurf->pixels[destoffset++]=opx;
        opx=0;
        omask=0x80000000;
      }
    }
    if (surf->bounds.w%32) nsurf->pixels[destoffset]=opx;
  }
  return nsurf;
}

Surface *wspr_MONO_to_YCYC_surf(Surface *surf) {
  Surface *nsurf=new Surface(surf->bounds.w,surf->bounds.h,PIXFMT_YCYC);
  if (!nsurf) throw MemoryError();
  for (S32 y=0;y<surf->bounds.h;y++) {
    S32 srcoffset=y*surf->wordsinrow;
    S32 destoffset=y*nsurf->wordsinrow;
    U32 px=surf->pixels[srcoffset++];
    U32 pmask=0x80000000;
    for (S32 x=0;x<nsurf->bounds.w;x++) {
      if (!(x%2)) nsurf->pixels[destoffset]=0;
      if (px&pmask) {
        if (x%2) nsurf->pixels[destoffset++]|=0x00008000;
        else nsurf->pixels[destoffset]|=0x80000000;
      }
      if (!(pmask>>=1)) {
        pmask=0x80000000;
        px=surf->pixels[srcoffset++];
      }
    }
  }
  return nsurf;
}

Surface *wspr_MONO_to_YCAC_surf(Surface *surf) {
  Surface *nsurf=new Surface(surf->bounds.w,surf->bounds.h,PIXFMT_YCAC);
  if (!nsurf) throw MemoryError();
  for (S32 y=0;y<surf->bounds.h;y++) {
    S32 srcoffset=y*surf->wordsinrow;
    S32 destoffset=y*nsurf->wordsinrow;
    U32 px=surf->pixels[srcoffset++];
    U32 pmask=0x80000000;
    for (S32 x=0;x<surf->bounds.w;x++) {
      if (px&pmask) nsurf->pixels[destoffset++]=0xff80ff80;
      else nsurf->pixels[destoffset++]=0x0080ff80;
      if (!(pmask>>=1)) {
        pmask=0x80000000;
        px=surf->pixels[srcoffset++];
      }
    }
  }
  return nsurf;
}

///////////////////////////////////////////////////////////////////////////////
// TRANSFORMS -------------------------------------------------------------- //

Surface *wspr_flip(Surface *surf) {
  Surface *nsurf=new Surface(surf->bounds,surf->pixfmt);
  if (!nsurf) throw MemoryError();
  if (surf->pixinword==1) {
    for (S32 y=0;y<surf->bounds.h;y++) {
      S32 dx=0;
      S32 srcoffset=y*surf->wordsinrow;
      S32 destoffset=(surf->bounds.h-y)*surf->wordsinrow;
      while (dx<surf->bounds.w) nsurf->pixels[destoffset+(dx++)]=surf->pixels[srcoffset+dx];
    }
  } else {
    for (S32 y=0;y<surf->bounds.h;y++) {
      S32 sy=surf->bounds.h-y;
      for (S32 x=0;x<surf->bounds.w;x++) nsurf->setPixel(x,y,surf->getPixel(x,sy));
    }
  }
  return nsurf;
}

Surface *wspr_flop(Surface *surf) {
  Surface *nsurf=new Surface(surf->bounds,surf->pixfmt);
  if (!nsurf) throw MemoryError();
  if (surf->pixinword==1) {
    for (S32 y=0;y<surf->bounds.h;y++) {
      S32 dx=0;
      S32 sx=surf->bounds.w-1;
      S32 offset=y*surf->wordsinrow;
      while (dx<surf->bounds.w) nsurf->pixels[offset+(dx++)]=surf->pixels[offset+(sx--)];
    }
  } else {
    for (S32 y=0;y<surf->bounds.h;y++) {
      S32 dx=0;
      S32 sx=surf->bounds.w-1;
      while (dx<surf->bounds.w) nsurf->setPixel(dx,y,surf->getPixel(sx,y));
    }
  }
  return nsurf;
}

Surface *wspr_rotate90(Surface *surf,S32 degrees) {
  while (degrees<0) degrees+=360;
  while (degrees>=360) degrees-=360;
  if (!degrees) return new Surface(*surf);
  if (degrees%90) throw ArgumentError();
  S32 ow,oh;
  if (degrees==180) { ow=surf->bounds.w; oh=surf->bounds.h; }
  else { oh=surf->bounds.w; ow=surf->bounds.h; }
  Surface *nsurf=new Surface(ow,oh,surf->pixfmt);
  if (!nsurf) throw MemoryError();
  for (S32 y=0;y<oh;y++) for (S32 x=0;x<ow;x++) {
    switch (degrees) {
      case 90:  nsurf->setPixel(x,y,surf->getPixel(y,ow-x)); break;
      case 270: nsurf->setPixel(x,y,surf->getPixel(oh-y,x)); break;
      case 180: nsurf->setPixel(x,y,surf->getPixel(ow-x,oh-y)); break;
    }
  }
  return nsurf;
}

Surface *wspr_crop(Surface *surf,Rect &r) {
  Surface *nsurf=new Surface(r.w,r.h,surf->pixfmt);
  if (!nsurf) throw MemoryError();
  nsurf->blit(*surf,r,nsurf->bounds);
  return nsurf;
}

Surface *wspr_tileSurface(Surface *surf,S32 width,S32 height,S32 offsetx,S32 offsety) {
  Surface *nsurf=new Surface(width,height,surf->pixfmt);
  if (!nsurf) throw MemoryError();
  S32 colc=width/surf->bounds.w+1;
  S32 rowc=height/surf->bounds.h+1;
  Rect destr(-offsetx,-offsety,surf->bounds.w,surf->bounds.h);
  for (S32 col=0;col<colc;col++) {
    for (S32 row=0;row<rowc;row++) {
      nsurf->blit(*surf,surf->bounds,destr);
      destr.move(0,destr.h);
    }
    destr.x=destr.right();
    destr.y=-offsety;
  }
  return nsurf;
}

Surface *wspr_rotateFree(Surface *surf,S32 degrees,U32 bgcolor,S32 oversampling,bool crop) {
  while (degrees<0) degrees+=360;
  while (degrees>=360) degrees-=360;
  if (!degrees) return new Surface(*surf);
  if (oversampling<1) throw ArgumentError();
  FP64 rad=(degrees*pi)/180;
  S32 ow,oh;
  S32 imidx=surf->bounds.w/2;
  S32 imidy=surf->bounds.h/2;
  S32 omidx,omidy;
  if (crop) { ow=surf->bounds.w; oh=surf->bounds.h; omidx=imidx; omidy=imidy; }
  else { 
    FP64 radius=sqrt(surf->bounds.w*surf->bounds.w+surf->bounds.h*surf->bounds.h);
    FP64 cornerangle=atan2(imidy,imidx);
    S32 nwx=lround(cos(cornerangle+pi+rad)*radius);
    S32 nwy=lround(sin(cornerangle+pi+rad)*radius);
    S32 nex=lround(cos(twopi-cornerangle+rad)*radius);
    S32 ney=lround(sin(twopi-cornerangle+rad)*radius);
    S32 swx=lround(cos(pi-cornerangle+rad)*radius);
    S32 swy=lround(sin(pi-cornerangle+rad)*radius);
    S32 sex=lround(cos(cornerangle+pi+rad)*radius);
    S32 sey=lround(sin(cornerangle+pi+rad)*radius);
    ow=MAX4(nwx,nex,swx,sex)*2;
    oh=MAX4(nwy,ney,swy,sey)*2;
    omidx=ow/2;
    omidy=oh/2;
  }
  Surface *nsurf=new Surface(ow,oh,surf->pixfmt);
  if (!nsurf) throw MemoryError();
  S32 oversampling2=oversampling*oversampling;
  for (S32 y=0;y<oh;y++) for (S32 x=0;x<ow;x++) {
    S32 sum0=0,sum1=0,sum2=0,sum3=0;
    for (S32 suby=0;suby<oversampling;suby++) for (S32 subx=0;subx<oversampling;subx++) {
      FP64 rx=x+((FP64)subx)/oversampling;
      FP64 ry=y+((FP64)suby)/oversampling;
      FP64 theta=atan2(ry-omidy,rx-omidx);
      FP64 radius=sqrt((ry-omidy)*(ry-omidy)+(rx-omidx)*(rx-omidx));
      theta-=rad;
      S32 sx=imidx+lround(cos(theta)*radius);
      S32 sy=imidy+lround(sin(theta)*radius);
      U32 px;
      if ((sx<0)||(sy<0)||(sx>=surf->bounds.w)||(sy>=surf->bounds.h)) px=bgcolor;
      else px=surf->getPixel(sx,sy);
      sum0+=px>>24;
      sum1+=(px&0x00ff0000)>>16;
      sum2+=(px&0x0000ff00)>>8;
      sum3+=px&0x000000ff;
    }
    sum0/=oversampling2;
    sum1/=oversampling2;
    sum2/=oversampling2;
    sum3/=oversampling2;
    nsurf->setPixel(x,y,(sum0<<24)|(sum1<<16)|(sum2<<8)|(sum3));
  }
  return nsurf;
}

Surface *wspr_scale_up(Surface *surf,S32 factor) {
// FUTURE -- rewrite w/o getpixel and setpixel
  if (factor<1) throw ArgumentError();
  Surface *nsurf=new Surface(surf->bounds.w*factor,surf->bounds.h*factor,surf->pixfmt);
  if (!nsurf) throw MemoryError();
  
  for (S32 sx=0;sx<surf->bounds.w;sx++) {
    S32 dx=sx*factor;
    for (S32 sy=0;sy<surf->bounds.h;sy++) {
      S32 dy=sy*factor;
      U32 px=surf->getPixel(sx,sy);
      for (S32 subx=0;subx<factor;subx++) for (S32 suby=0;suby<factor;suby++) nsurf->setPixel(dx+subx,dy+suby,px);
  }}
  
  return nsurf;
}

Surface *wspr_scale(Surface *surf,S32 width,S32 height,S32 oversampling) {
// FUTURE -- rewrite without getpixel and setpixel
  if (oversampling<1) throw ArgumentError();
  if (oversampling>=100) throw ArgumentError(); // realistically, you don't want more than 4 or 5
  Surface *nsurf=new Surface(width,height,surf->pixfmt);
  
  S32 oversampling2=oversampling*oversampling;
  FP64 scalex=((FP64)width)/((FP64)surf->bounds.w);
  FP64 scaley=((FP64)height)/((FP64)surf->bounds.h);
  
  for (S32 x=0;x<nsurf->bounds.w;x++) { for (S32 y=0;y<nsurf->bounds.h;y++) {
    S32 totaly=0;
    S32 totalcb=0;
    S32 totala=0;
    S32 totalcr=0;
    for (S32 subx=0;subx<oversampling;subx++) for (S32 suby=0;suby<oversampling;suby++) {
      FP64 rx=x; rx+=((FP64)(subx+1))/((FP64)(oversampling+1));
      FP64 ry=y; ry+=((FP64)(suby+1))/((FP64)(oversampling+1));
      rx/=scalex;
      ry/=scaley;
      U32 px=surf->getPixel((S32)floor(rx),(S32)floor(ry));
      totaly+=px>>24;
      totalcb+=(px&0x00ff0000)>>16;
      totala+=(px&0x0000ff00)>>8;
      totalcr+=(px&0x000000ff);
    }
    totaly/=oversampling2;
    totalcb/=oversampling2;
    totala/=oversampling2;
    totalcr/=oversampling2;
    nsurf->setPixel(x,y,(totaly<<24)|(totalcb<<16)|(totala<<8)|(totalcr));
  }}
  
  return nsurf;
}

bool wspr_scale2_cartoon_defaultComparator(U32 a,U32 b) { return (a==b); }

Surface *wspr_scale2_cartoon(Surface *surf,Scale2CartoonComparator cmp) {
// FUTURE -- rewrite without getpixel and setpixel
  Surface *nsurf=new Surface(surf->bounds.w*2,surf->bounds.h*2,surf->pixfmt);
  if (!nsurf) throw MemoryError();
  if (!cmp) cmp=&wspr_scale2_cartoon_defaultComparator;
  
  for (S32 x=0;x<surf->bounds.w;x++) for (S32 y=0;y<surf->bounds.h;y++) {
    U32 cpx=surf->getPixel(x,y);
    U32 nw=((x>0)&&(y>0))?surf->getPixel(x-1,y-1):cpx;
    U32 n=(y>0)?surf->getPixel(x,y-1):cpx;
    U32 ne=((x<surf->bounds.w-1)&&(y>0))?surf->getPixel(x+1,y-1):cpx;
    U32 w=(x>0)?surf->getPixel(x-1,y):cpx;
    U32 e=(x<surf->bounds.w-1)?surf->getPixel(x+1,y):cpx;
    U32 sw=((x>0)&&(y<surf->bounds.h-1))?surf->getPixel(x-1,y+1):cpx;
    U32 s=(y<surf->bounds.h-1)?surf->getPixel(x,y+1):cpx;
    U32 se=((x<surf->bounds.w-1)&&(y<surf->bounds.h-1))?surf->getPixel(x+1,y+1):cpx;
    
    U32 tl=(cmp(w,nw)&&cmp(nw,n)&&!cmp(w,s)&&!cmp(n,e))?nw:cpx;
    U32 tr=(cmp(n,ne)&&cmp(ne,e)&&!cmp(w,n)&&!cmp(e,s))?ne:cpx;
    U32 bl=(cmp(e,sw)&&cmp(sw,s)&&!cmp(s,e)&&!cmp(n,w))?sw:cpx;
    U32 br=(cmp(s,se)&&cmp(se,e)&&!cmp(e,n)&&!cmp(w,s))?se:cpx;
    
    nsurf->setPixel(x*2,y*2,tl);
    nsurf->setPixel(x*2+1,y*2,tr);
    nsurf->setPixel(x*2,y*2+1,bl);
    nsurf->setPixel(x*2+1,y*2+1,br);
  }
  
  return nsurf;
}

///////////////////////////////////////////////////////////////////////////////
// GENERATION -------------------------------------------------------------- //

Surface *wspr_gradientH(U32 leftcolor,U32 rightcolor,S32 width,S32 height) {
  Surface *surf=new Surface(width,height);
  if (!surf) throw MemoryError();
  
  S32 lefty=leftcolor>>24;
  S32 leftcb=(leftcolor&0x00ff0000)>>16;
  S32 lefta=(leftcolor&0x0000ff00)>>8;
  S32 leftcr=(leftcolor&0x000000ff);
  S32 righty=rightcolor>>24;
  S32 rightcb=(rightcolor&0x00ff0000)>>16;
  S32 righta=(rightcolor&0x0000ff00)>>8;
  S32 rightcr=(rightcolor&0x000000ff);
  
  for (S32 x=0;x<width;x++) {
    S32 y=lefty+((righty-lefty)*x)/width;
    S32 cb=leftcb+((rightcb-leftcb)*x)/height;
    S32 a=lefta+((righta-lefta)*x)/width;
    S32 cr=leftcb+((rightcr-leftcr)*x)/width;
    
    U32 px=(y<<24)|(cb<<16)|(a<<8)|cr;
    for (S32 y=0;y<height;y++) surf->setPixel(x,y,px);
  }
  return surf;
}

Surface *wspr_gradientV(U32 topcolor,U32 bottomcolor,S32 width,S32 height) {
  Surface *surf=new Surface(width,height);
  if (!surf) throw MemoryError();
  
  S32 topy=topcolor>>24;
  S32 topcb=(topcolor&0x00ff0000)>>16;
  S32 topa=(topcolor&0x0000ff00)>>8;
  S32 topcr=(topcolor&0x000000ff);
  S32 bottomy=bottomcolor>>24;
  S32 bottomcb=(bottomcolor&0x00ff0000)>>16;
  S32 bottoma=(bottomcolor&0x0000ff00)>>8;
  S32 bottomcr=(bottomcolor&0x000000ff);
  
  for (S32 y=0;y<height;y++) {
    S32 luma=topy+((bottomy-topy)*y)/height;
    S32 cb=topcb+((bottomcb-topcb)*y)/height;
    S32 a=topa+((bottoma-topa)*y)/height;
    S32 cr=topcb+((bottomcr-topcr)*y)/height;
    
    U32 px=(luma<<24)|(cb<<16)|(a<<8)|cr;
    for (S32 x=0;x<width;x++) surf->setPixel(x,y,px);
  }
  return surf;
}

Surface *wspr_gradientLinear(U32 ac,U32 bc,S32 ax,S32 ay,S32 bx,S32 by,S32 width,S32 height) {
  Surface *surf=new Surface(width,height);
  if (!surf) throw MemoryError();
  
  // determine control points...
  S32 mx=(ax+bx)/2;
  S32 my=(ay+by)/2;
  S32 a2x=ax+(my-ay);
  S32 a2y=ay-(mx-ax);
  S32 b2x=bx-(by-my);
  S32 b2y=by+(bx-mx);
  FP64 distab=sqrt((ax-bx)*(ax-bx)+(ay-by)*(ay-by));
  FP64 distam=distab/2;
  FP64 distam2=distam*distam;
  
  for (S32 x=0;x<width;x++) for (S32 y=0;y<width;y++) {
    S32 dira=ptDirection(ax,ay,a2x,a2y,x,y);
    if (dira>=0) { surf->setPixel(x,y,ac);
    } else {
      S32 dirb=ptDirection(bx,by,b2x,b2y,x,y);
      if (dirb>=0) { surf->setPixel(x,y,bc);
      } else {
      
        FP64 dista=(x-ax)*(x-ax)+(y-ay)*(y-ay);
        FP64 distm=(x-mx)*(x-mx)+(y-my)*(y-my);
        FP64 xm=(distm+distam2-dista)/distab;
        FP64 ax=distam-xm;
        S32 mixamt=lround((ax*255)/distab);
        if (mixamt<0) mixamt=0; else if (mixamt>255) mixamt=255;
        surf->setPixel(x,y,wspr_colorMix(ac,bc,mixamt));
      
      }
    }
  }
  
  return surf;
}

Surface *wspr_gradientPointwise(S32 width,S32 height,S32 count,...) {
  Surface *surf=new Surface(width,height);
  if (!surf) throw MemoryError();
  
  // read arguments...
  if (count<2) throw ArgumentError();
  S32 *xs=(S32*)malloc(sizeof(S32)*count);
  if (!xs) throw MemoryError();
  S32 *ys=(S32*)malloc(sizeof(S32)*count);
  if (!ys) throw MemoryError();
  U32 *colors=(U32*)malloc(sizeof(U32)*(count+1));
  if (!colors) throw MemoryError();
  va_list vargs;
  va_start(vargs,count);
  for (S32 i=0;i<count;i++) {
    xs[i]=va_arg(vargs,S32);
    if (xs[i]<0) xs[i]=0; else if (xs[i]>width) xs[i]=width;
    ys[i]=va_arg(vargs,S32);
    if (ys[i]<0) ys[i]=0; else if (ys[i]>height) ys[i]=height;
    colors[i]=va_arg(vargs,U32);
  }
  
  S32 ytotal=0,cbtotal=0,atotal=0,crtotal=0;
  for (S32 i=0;i<count;i++) {
    ytotal+=(colors[i]>>24);
    cbtotal+=(colors[i]&0x00ff0000)>>16;
    atotal+=(colors[i]&0x0000ff00)>>8;
    crtotal+=(colors[i]&0x000000ff);
  }
  ytotal/=count;
  cbtotal/=count;
  atotal/=count;
  crtotal/=count;
  colors[count]=(ytotal<<24)|(cbtotal<<16)|(atotal<<8)|crtotal;
  
  S32 dists[count];
  S32 weights[count];
  S32 hypot=lround(sqrt(width*width+height*height));
  S32 defaultweight=1;
  
  // pixel-by-pixel...
  for (S32 col=0;col<width;col++) {
    for (S32 row=0;row<height;row++) {
      
      S32 disttotal=0;
      S32 disthi=0;
      for (S32 i=0;i<count;i++) {
        dists[i]=lround(hypot-sqrt((xs[i]-col)*(xs[i]-col)+(ys[i]-row)*(ys[i]-row)));
        disttotal+=dists[i];
        if (dists[i]>disthi) disthi=dists[i];
      }
      for (S32 i=0;i<count;i++) {
        weights[i]=hypot-dists[i];
      }
      
      S32 y,cb,a,cr,range;
      y=cb=a=cr=range=0;
      for (S32 i=0;i<count;i++) {
        range+=weights[i];
        y+=(colors[i]>>24)*weights[i];
        cb+=((colors[i]&0x00ff0000)>>16)*weights[i];
        a+=((colors[i]&0x0000ff00)>>8)*weights[i];
        cr+=(colors[i]&0x000000ff)*weights[i];
      }
      range+=defaultweight;
      y+=(colors[count]>>24)*defaultweight;
      cb+=((colors[count]&0x00ff0000)>>16)*defaultweight;
      a+=((colors[count]&0x0000ff00)>>8)*defaultweight;
      cr+=(colors[count]&0x000000ff)*defaultweight;
      
      y/=range;
      cb/=range;
      a/=range;
      cr/=range;
      
      surf->pixels[row*width+col]=(y<<24)|(cb<<16)|(a<<8)|cr;
    }
  }
  
  free(xs);
  free(ys);
  free(colors);
  va_end(vargs); 
  return surf;
}
/* --- older version ---
Surface *wspr_gradientPointwise(S32 width,S32 height,S32 count,...) {
  Surface *surf=new Surface(width,height);
  if (!surf) throw MemoryError();
  
  // read arguments...
  if (count<2) throw ArgumentError();
  S32 *xs=(S32*)malloc(sizeof(S32)*count);
  if (!xs) throw MemoryError();
  S32 *ys=(S32*)malloc(sizeof(S32)*count);
  if (!ys) throw MemoryError();
  U32 *colors=(U32*)malloc(sizeof(U32)*(count+1));
  if (!colors) throw MemoryError();
  va_list vargs;
  va_start(vargs,count);
  for (S32 i=0;i<count;i++) {
    xs[i]=va_arg(vargs,S32);
    ys[i]=va_arg(vargs,S32);
    colors[i]=va_arg(vargs,U32);
  }
  
  S32 ytotal=0,cbtotal=0,atotal=0,crtotal=0;
  for (S32 i=0;i<count;i++) {
    ytotal+=(colors[i]>>24);
    cbtotal+=(colors[i]&0x00ff0000)>>16;
    atotal+=(colors[i]&0x0000ff00)>>8;
    crtotal+=(colors[i]&0x000000ff);
  }
  ytotal/=count;
  cbtotal/=count;
  atotal/=count;
  crtotal/=count;
  colors[count]=(ytotal<<24)|(cbtotal<<16)|(atotal<<8)|crtotal;
  
  S32 dists[count];
  S32 weights[count];
  U32 c;
  S32 defaultweight=50;
  
  // pixel-by-pixel...
  for (S32 col=0;col<width;col++) {
    for (S32 row=0;row<height;row++) {
      
      S32 disttotal=0;
      S32 disthi=0;
      for (S32 i=0;i<count;i++) {
        dists[i]=lround(sqrt((xs[i]-col)*(xs[i]-col)+(ys[i]-row)*(ys[i]-row)));
        disttotal+=dists[i];
        if (dists[i]>disthi) disthi=dists[i];
      }
      for (S32 i=0;i<count;i++) {
        weights[i]=disthi-dists[i];
      }
      
      S32 y,cb,a,cr,range;
      y=cb=a=cr=range=0;
      for (S32 i=0;i<count;i++) {
        range+=weights[i];
        y+=(colors[i]>>24)*weights[i];
        cb+=((colors[i]&0x00ff0000)>>16)*weights[i];
        a+=((colors[i]&0x0000ff00)>>8)*weights[i];
        cr+=(colors[i]&0x000000ff)*weights[i];
      }
      range+=defaultweight;
      y+=(colors[count]>>24)*defaultweight;
      cb+=((colors[count]&0x00ff0000)>>16)*defaultweight;
      a+=((colors[count]&0x0000ff00)>>8)*defaultweight;
      cr+=(colors[count]&0x000000ff)*defaultweight;
      
      y/=range;
      cb/=range;
      a/=range;
      cr/=range;
      
      surf->pixels[row*width+col]=(y<<24)|(cb<<16)|(a<<8)|cr;
    }
  }
  
  free(xs);
  free(ys);
  free(colors);
  va_end(vargs); 
  return surf;
}*/
