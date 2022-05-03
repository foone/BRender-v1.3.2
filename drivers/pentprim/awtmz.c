/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: awtmz.c 1.1 1997/12/10 16:46:38 jon Exp $
 * $Locker: $
 *
 * Arbitary width texture mapper
 */
#include <math.h>

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

#define swap(type,a,b) { type _; _ = a; a = b; b = _; }

#define high8(a) (*((unsigned char *)&a+3))
#define high16(a) (*((unsigned short *)&a+1))
#define cvsl(a) ((unsigned int)(unsigned short)(a))
#define mod2n(x,y) ((x)>=0 ? (x) & (y-1) : (x) | ~(y-1))
#define powerof2(x) (!((x) & (x)-1))

extern int BR_ASM_CALL SafeFixedMac2Div(int,int,int,int,int);

typedef void BR_ASM_CALL ScanLineCallFn(void);
typedef ScanLineCallFn *ScanLineCall;

#ifdef __WATCOMC__

extern int sar16(int);

#pragma aux sar16 = "sar eax,16" parm [eax]

#else

#define sar16 _sar16

#endif

typedef void BR_ASM_CALL TrapezoidRenderCall(void);

TrapezoidRenderCall TrapezoidRenderPIZ2TA;
TrapezoidRenderCall TrapezoidRenderPIZ2TIA;

TrapezoidRenderCall TrapezoidRenderPIZ2TA15;
TrapezoidRenderCall TrapezoidRenderPIZ2TA24;

TrapezoidRenderCall TrapezoidRenderPIZ2TAN;

TrapezoidRenderCall TrapezoidRenderPIZ2TIA_RGB_555;
TrapezoidRenderCall TrapezoidRenderPIZ2TIA_RGB_888;

#define ZB 1

#if PARTS & PART_8Z
#define DBPP 1
#define SBPP 1
#define FNAME TriangleRenderPIZ2TIA
#define TNAME TrapezoidRenderPIZ2TIA
#define BUMP 0
#define LIGHT 1
#include "awtmi.h"
#undef LIGHT
#undef BUMP
#undef FNAME
#undef TNAME
#undef SBPP
#undef DBPP
#endif

#if PARTS & PART_8Z
#define SBPP 1
#define DBPP 1
#define FNAME TriangleRenderPIZ2TA
#define TNAME TrapezoidRenderPIZ2TA
#define BUMP 0
#define LIGHT 0
#include "awtmi.h"
#undef LIGHT
#undef BUMP
#undef FNAME
#undef TNAME
#undef SBPP
#undef DBPP
#endif

#if PARTS & PART_BUMP
#define SBPP 1
#define DBPP 1
#define FNAME TriangleRenderPIZ2TAN
#define TNAME TrapezoidRenderPIZ2TAN
#define BUMP 1
#define LIGHT 0
#include "awtmi.h"
#undef LIGHT
#undef FNAME
#undef TNAME
#undef BUMP
#undef SBPP
#undef DBPP
#endif

#if PARTS & PART_24Z
#define SBPP 3
#define DBPP 3
#define FNAME TriangleRenderPIZ2TA24
#define TNAME TrapezoidRenderPIZ2TA24
#define BUMP 0
#define LIGHT 0
#include "awtmi.h"
#undef LIGHT
#undef BUMP
#undef FNAME
#undef TNAME
#undef SBPP
#undef DBPP
#endif

#if PARTS & PART_15Z
#define SBPP 2
#define DBPP 2
#define FNAME TriangleRenderPIZ2TA15
#define TNAME TrapezoidRenderPIZ2TA15
#define BUMP 0
#define LIGHT 0
#include "awtmi.h"
#undef LIGHT
#undef BUMP
#undef FNAME
#undef TNAME
#undef SBPP
#undef DBPP
#endif

#if PARTS & PART_15Z
#define SBPP 1
#define DBPP 2
#define FNAME TriangleRenderPIZ2TIA_RGB_555
#define TNAME TrapezoidRenderPIZ2TIA_RGB_555
#define BUMP 0
#define LIGHT 1
#include "awtmi.h"
#undef LIGHT
#undef BUMP
#undef FNAME
#undef TNAME
#undef SBPP
#undef DBPP
#endif

#if PARTS & PART_24Z
#define SBPP 1
#define DBPP 3
#define FNAME TriangleRenderPIZ2TIA_RGB_888
#define TNAME TrapezoidRenderPIZ2TIA_RGB_888
#define BUMP 0
#define LIGHT 1
#include "awtmi.h"
#undef LIGHT
#undef BUMP
#undef FNAME
#undef TNAME
#undef SBPP
#undef DBPP
#endif

