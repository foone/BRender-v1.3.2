/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: perspz.c 1.1 1997/12/10 16:47:33 jon Exp $
 * $Locker: $
 *
 * Perspective texture mapper
 */
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define FIX 1

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

#define swap(type,a,b) { type _; _ = a; a = b; b = _; }

#define high8(a) (*((unsigned char *)&a+3))

#define F_ABS(x) ((x)>0 ? (x) : -(x))
#define F_LENGTH(x,y) ((x)>(y) ? (x)+(y)*11/32 : (y)+(x)*11/32)

extern float frcp[100];

extern int dither[4][4];

extern void BR_ASM_CALL ScanLinePIZ2TPD64(void);
extern void BR_ASM_CALL ScanLinePIZ2TPD128(void);
extern void BR_ASM_CALL ScanLinePIZ2TPD256(void);
extern void BR_ASM_CALL ScanLinePIZ2TPD1024(void);

extern void BR_ASM_CALL ScanLinePIZ2TIP256_RGB_555(void);

extern void BR_ASM_CALL ScanLinePIZ2TIPB256_RGB_555(void);
extern void BR_ASM_CALL ScanLinePIZ2TIPB256_RGB_565(void);

extern int BR_ASM_CALL SafeFixedMac2Div(int,int,int,int,int);

#define cutoff 4.0

#ifdef __WATCOMC__

extern int sar16(int);

extern int inch(int);
extern int incl(int);
extern int dech(int);
extern int decl(int);

#pragma aux sar16 = "sar eax,16" parm [eax]

#pragma aux inch = "inc ah" parm [eax]
#pragma aux incl = "inc al" parm [eax]
#pragma aux dech = "dec ah" parm [eax]
#pragma aux decl = "dec al" parm [eax]

#else

#define sar16 _sar16

#endif

#define ZB 1

#define BPP 1

#if PARTS & PART_DITHER
#define CHEAT 0
#define FNAME TriangleRenderPIZ2TPD64
#define SNAME ScanLinePIZ2TPD64
#define SIZE 64
#define LIGHT 0
#include "perspzi.h"
#undef LINEAR
#undef LIGHT
#undef SIZE
#undef SNAME
#undef FNAME
#undef CHEAT
#endif

#if PARTS & PART_DITHER
#define CHEAT 0
#define FNAME TriangleRenderPIZ2TPD128
#define SNAME ScanLinePIZ2TPD128
#define SIZE 128
#define LIGHT 0
#include "perspzi.h"
#undef LINEAR
#undef LIGHT
#undef SIZE
#undef SNAME
#undef FNAME
#undef CHEAT
#endif

#if PARTS & PART_DITHER
#define CHEAT 0
#define FNAME TriangleRenderPIZ2TPD256
#define SNAME ScanLinePIZ2TPD256
#define SIZE 256
#define LIGHT 1
#include "perspzi.h"
#undef LINEAR
#undef LIGHT
#undef SIZE
#undef SNAME
#undef FNAME
#undef CHEAT
#endif

#if PARTS & PART_DITHER
#define CHEAT 0
#define FNAME TriangleRenderPIZ2TPD1024
#define SNAME ScanLinePIZ2TPD1024
#define SIZE 1024
#define LIGHT 0
#include "perspzi.h"
#undef LIGHT
#undef SIZE
#undef SNAME
#undef FNAME
#endif

#undef BPP
#define BPP 2

#if PARTS & PART_15Z
#define CHEAT 0
#define FNAME TriangleRenderPIZ2TIP256_RGB_555
#define SNAME ScanLinePIZ2TIP256_RGB_555
#define SIZE 256
#define LIGHT 1
#include "perspzi.h"
#undef LIGHT
#undef SIZE
#undef SNAME
#undef FNAME
#endif

#if PARTS & PART_15Z
#define CHEAT 0
#define FNAME TriangleRenderPIZ2TIPB256_RGB_555
#define SNAME ScanLinePIZ2TIPB256_RGB_555
#define SIZE 256
#define LIGHT 1
#include "perspzi.h"
#undef LIGHT
#undef SIZE
#undef SNAME
#undef FNAME
#endif

#if PARTS & PART_16Z
#define CHEAT 0
#define FNAME TriangleRenderPIZ2TIPB256_RGB_565
#define SNAME ScanLinePIZ2TIPB256_RGB_565
#define SIZE 256
#define LIGHT 1
#include "perspzi.h"
#undef LIGHT
#undef SIZE
#undef SNAME
#undef FNAME
#endif

#undef BPP
