#ifndef WSPR_TRANSFORM_H
#define WSPR_TRANSFORM_H

#include "wspr_types.h"

class Surface;

/******************************************************************************
  Color Conversion
  Note that converting RGBA to YCAC (or vice versa) uses HSVA as an intermediary.
  It's not fast, or accurate.
******************************************************************************/
U32 wspr_YCAC_to_HSVA(U32 c);
U32 wspr_HSVA_to_YCAC(U32 c);
U32 wspr_HSVA_to_RGBA(U32 c);
U32 wspr_RGBA_to_HSVA(U32 c);
U32 wspr_YCAC_to_RGBA(U32 c);
U32 wspr_RGBA_to_YCAC(U32 c);

U32 wspr_colorMix(U32 a,U32 b,U8 amt,S32 pixfmt=PIXFMT_YCAC); // 00=a ff=b

/******************************************************************************
  Filter Operations
  Modify a Surface's pixels in place
******************************************************************************/
void wspr_invert(Surface *surf);
void wspr_invertValue(Surface *surf);
void wspr_adjustSaturation(Surface *surf,S8 adjust);
void wspr_adjustValue(Surface *surf,S8 adjust);
void wspr_adjustContrast(Surface *surf,S8 adjust);
void wspr_tint(Surface *surf,U32 c,U8 amt);

/******************************************************************************
  Transform Operations
  Return a new Surface, leave the original intact.
******************************************************************************/
Surface *wspr_flip(Surface *surf);
Surface *wspr_flop(Surface *surf);
Surface *wspr_rotate90(Surface *surf,S32 degrees);
Surface *wspr_rotateFree(Surface *surf,S32 degrees,U32 bgcolor=0xff80ff80,S32 oversampling=1,bool crop=true);
Surface *wspr_crop(Surface *surf,Rect &r);
Surface *wspr_scale(Surface *surf,S32 width,S32 height,S32 oversampling=1); // uses floats, oversampling^2 queries per dest pixel (keep oversampling low or regret it!)
Surface *wspr_scale_up(Surface *surf,S32 factor); // faster than ordinary scale, for blocky results
Surface *wspr_tileSurface(Surface *surf,S32 width,S32 height,S32 offsetx=0,S32 offsety=0);

typedef bool(Scale2CartoonComparator(U32 a,U32 b));
Surface *wspr_scale2_cartoon(Surface *surf,Scale2CartoonComparator cmp=NULL); // scale to exactly 2x2 with good interpolation for line art

Surface *wspr_YCYC_to_YCAC_surf(Surface *surf);
Surface *wspr_YCYC_to_MONO_surf(Surface *surf);
Surface *wspr_YCAC_to_YCYC_surf(Surface *surf);
Surface *wspr_YCAC_to_MONO_surf(Surface *surf);
Surface *wspr_YCAC_to_RGBA_surf(Surface *surf);
Surface *wspr_YCAC_to_HSVA_surf(Surface *surf);
Surface *wspr_MONO_to_YCYC_surf(Surface *surf);
Surface *wspr_MONO_to_YCAC_surf(Surface *surf);
Surface *wspr_RGBA_to_YCAC_surf(Surface *surf);
Surface *wspr_HSVA_to_YCAC_surf(Surface *surf);
Surface *wspr_RGBA_to_HSVA_surf(Surface *surf);
Surface *wspr_HSVA_to_RGBA_surf(Surface *surf);

/******************************************************************************
  Generation Operations
  Return a new Surface, without a source Surface.
******************************************************************************/
Surface *wspr_gradientH(U32 leftcolor,U32 rightcolor,S32 width,S32 height);
Surface *wspr_gradientV(U32 topcolor,U32 bottomcolor,S32 width,S32 height);
Surface *wspr_gradientLinear(U32 ac,U32 bc,S32 ax,S32 ay,S32 bx,S32 by,S32 width,S32 height);
Surface *wspr_gradientPointwise(S32 width,S32 height,S32 count,...); // varargs: S32 x,S32 y,U32 c   * count

#endif
