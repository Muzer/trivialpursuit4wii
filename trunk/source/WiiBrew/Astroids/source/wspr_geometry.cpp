#include "wspr_types.h"
#include "wspr_geometry.h"

S32 ptDirection(S32 ax,S32 ay,S32 bx,S32 by,S32 cx,S32 cy) {
  double cp=(bx-ax)*(cy-ay)-(cx-ax)*(by-ay);
  if (cp<0) return 1;
  if (cp>0) return -1;
  return 0;
}
