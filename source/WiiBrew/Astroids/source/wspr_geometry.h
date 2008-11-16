#ifndef WSPR_GEOMETRY_H
#define WSPR_GEOMETRY_H

#include "wspr_types.h"

/* Standing at point (ax,ay), looking at point (bx,by), is point (cx,cy)...
     ... to your left? == -1
     ... to your right?==  1
     ... dead-center?  ==  0 */
S32 ptDirection(S32 ax,S32 ay,S32 bx,S32 by,S32 cx,S32 cy);

#endif
