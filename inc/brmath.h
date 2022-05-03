/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: brmath.h 1.2 1998/07/20 21:52:53 jon Exp $
 * $Locker: $
 *
 * Wrappers around standard C math functions
 */

#ifndef _BRMATH_H_
#define _BRMATH_H_

#include <stdarg.h>

#ifndef _NO_PROTOTYPES

#ifdef __cplusplus
extern "C" {
#endif

float BR_PUBLIC_ENTRY BrFloatFloor(float f);
float BR_PUBLIC_ENTRY BrFloatCeil(float f);
float BR_PUBLIC_ENTRY BrFloatSqrt(float f);
float BR_PUBLIC_ENTRY BrFloatPow(float a, float b);
float BR_PUBLIC_ENTRY BrFloatAtan2(float x, float y);

#ifdef __cplusplus
};
#endif

/*
 * Compiler specific overrides for functions that are inlined
 */

/*
 * Compiler specific overrides
 */
#if defined(__USE_CLIB__)

#include <math.h>

/*
 * Force C library operations to be used directly
 */
#define BrFloatFloor(f)		((float)floor(f))
#define BrFloatCeil(f)		((float)ceil(f))
#define BrFloatSqrt(f)		((float)sqrt(f))
#define BrFloatPow(a,b)		((float)pow(a,b))
#define BrFloatAtan2(x,y)	((float)atan2((x),(y)))

#elif defined(__WATCOMC__)

#elif defined(_MSC_VER)
#include <math.h>

/*
 * Force C library operations to be used directly
 */
#define BrFloatFloor(f)		((float)floor(f))
#define BrFloatCeil(f)		((float)ceil(f))
#define BrFloatSqrt(f)		((float)sqrt(f))
#define BrFloatPow(a,b)		((float)pow(a,b))
#define BrFloatAtan2(x,y)	((float)atan2((x),(y)))

#elif defined (__BORLANDC__)

#elif defined(__PROTON__)

#elif defined(__GNUC__) && defined(i386)

#endif

#endif

#endif

