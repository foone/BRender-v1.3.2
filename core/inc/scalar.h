/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: scalar.h 1.3 1998/07/01 19:31:26 jon Exp $
 * $Locker: $
 *
 * Scalar type
 *
 * External defines:
 *
 * BASED_FIXED
 *  True if system is based on signed 16.16 fixed point
 *
 * BASED_FLOAT
 *  True if system is based on 32 bit floating point
 *
 * One of these must be defined and the appropriate libraries must be used
 */
#ifndef _SCALAR_H_
#define _SCALAR_H_

#if !defined(BASED_FIXED) && !defined(BASED_FLOAT)
#define BASED_FIXED 1
#endif

#if !defined(BASED_FIXED)
#define BASED_FIXED 0
#endif

#if !defined(BASED_FLOAT)
#define BASED_FLOAT 0
#endif

#if !BASED_FIXED && !BASED_FLOAT
#error "Must define BASED_FIXED or BASED_FLOAT"
#endif

/**
 ** Basic types - either fixed or float
 **/

#if BASED_FIXED

	/*
	 * Fixed base types
	 */
typedef br_fixed_ls		br_scalar;
typedef br_fixed_lsf	br_fraction;
typedef br_fixed_luf	br_ufraction;

#define BR_SCALAR_EPSILON	0x0001
#define BR_SCALAR_MAX		0x7fffffff
#define BR_SCALAR_MIN		0x80000000

#define BR_SCALAR_TOKEN		BRT_FIXED

/*
 * Macros for static initialisation
 */
#define BR_SCALAR(x)		((br_scalar)(BR_ONE_LS * (x)))
#define BR_FRACTION(x)		((br_fraction)((BR_ONE_LSF * (x))>=BR_ONE_LSF?BR_ONE_LSF-1:BR_ONE_LSF * (x)))
#define BR_UFRACTION(x)		((br_ufraction)((BR_ONE_LUF * (x))>=BR_ONE_LUF?BR_ONE_LUF-1:BR_ONE_LUF * (x)))

/*
 * Macros for type conversion
 */
#define BrFloatToScalar(f)	((br_scalar)((f)*(float)BR_ONE_LS))
#define BrScalarToFloat(s)	((s)/(float)BR_ONE_LS)

#define BrIntToScalar(i)	((br_scalar)((i)*(int)BR_ONE_LS))
#define BrScalarToInt(s)	((s)/(int)BR_ONE_LS)

#define BrFixedToScalar(f)		(f)
#define BrScalarToFixed(s)		(s)

#define BrFractionToScalar(f)	((br_scalar)((f)*2))
#define BrScalarToFraction(s)	((br_fraction)((s)/2))

#define BrUFractionToScalar(f)	((br_scalar)(f))
#define BrScalarToUFraction(s)	((br_ufraction)(s))

/*
 * Various arithmetic operations
 */
#define BR_ADD(a,b)				((a)+(b))
#define BR_SUB(a,b)				((a)-(b))
#define BR_MUL(a,b)				BrFixedMul(a,b)
#define BR_SQR(a)				BrFixedSqr(a)

#define BR_ABS(a)				BrFixedAbs(a)
#define BR_NEG(a)				(-(a))

#define BR_DIV(a,b)				BrFixedDiv(a,b)
#define BR_DIVR(a,b)			BrFixedDivR(a,b)
#define BR_MULDIV(a,b,c)		BrFixedMulDiv(a,b,c)
#define BR_RCP(a)				BrFixedRcp(a)

#define BR_CONST_MUL(a,b)		((a)*(b))
#define BR_CONST_DIV(a,b)		((a)/(b))

#define BR_MAC2(a,b,c,d)		BrFixedMac2(a,b,c,d)
#define BR_MAC3(a,b,c,d,e,f)	BrFixedMac3(a,b,c,d,e,f)
#define BR_MAC4(a,b,c,d,e,f,g,h) BrFixedMac4(a,b,c,d,e,f,g,h)

#define BR_MAC2DIV(a,b,c,d,e)		BrFixedMac2Div(a,b,c,d,e)
#define BR_MAC3DIV(a,b,c,d,e,f,g)	BrFixedMac3Div(a,b,c,d,e,f,g)
#define BR_MAC4DIV(a,b,c,d,e,f,g,h,i) BrFixedMac4Div(a,b,c,d,e,f,g,h,i)

#define BR_SQR2(a,b)			BrFixedSqr2(a,b)
#define BR_SQR3(a,b,c)			BrFixedSqr3(a,b,c)
#define BR_SQR4(a,b,c,d)		BrFixedSqr4(a,b,c,d)

#define BR_FMAC2(a,b,c,d)		BrFixedFMac2(a,b,c,d)
#define BR_FMAC3(a,b,c,d,e,f)	  BrFixedFMac3(a,b,c,d,e,f)
#define BR_FMAC4(a,b,c,d,e,f,g,h) BrFixedFMac4(a,b,c,d,e,f,g,h)

#define BR_LENGTH2(a,b)			BrFixedLength2(a,b)
#define BR_LENGTH3(a,b,c)		BrFixedLength3(a,b,c)
#define BR_LENGTH4(a,b,c,d)		BrFixedLength4(a,b,c,d)

#define BR_RLENGTH2(a,b)		BrFixedRLength2(a,b)
#define BR_RLENGTH3(a,b,c)		BrFixedRLength3(a,b,c)
#define BR_RLENGTH4(a,b,c,d)	BrFixedRLength4(a,b,c,d)

#define BR_POW(a,b)				BrFixedPow((a),(b))
#define BR_SQRT(a)				BrFixedSqrt(a)

#define BR_FLOOR(a)				((a) & -BR_ONE_LS)
#define BR_CEIL(a)				(((a) + BR_ONE_LS-BR_SCALAR_EPSILON) & -BR_ONE_LS)

#endif

#if BASED_FLOAT

#ifndef _NO_PROTOTYPES
#if defined(_MSC_VER) && !defined(__H2INC__)
#include <math.h>
#elif defined (__IBMC__)
#include <math.h>
#else
double fabs(double);
double fabs(double);
double sqrt(double);
double cos(double);
double sin(double);
double acos(double);
double asin(double);
double atan2(double,double);
#endif
#endif

	/*
	 * Floating point base types
	 */
typedef float br_scalar;
typedef float br_fraction;
typedef float br_ufraction;

#define BR_SCALAR_EPSILON	1.192092896e-7f
#define BR_SCALAR_MAX		3.402823466e+38f
#define BR_SCALAR_MIN		(-3.402823466e+38f)

#define BR_SCALAR_TOKEN		BRT_FLOAT

/*
 * Macros for static initialisation
 */
#define BR_SCALAR(x)		((br_scalar)(x))
#define BR_FRACTION(x)		((br_fraction)(x))
#define BR_UFRACTION(x)		((br_ufraction)(x))

/*
 * Macros for type conversion
 */
#define BrFloatToScalar(f)	(f)
#define BrScalarToFloat(c)	(c)

#define BrIntToScalar(i)	((br_scalar)(i))
#define BrScalarToInt(s)	((int)(s))

#define BrFixedToScalar(f)	((br_scalar)((f)/(float)BR_ONE_LS))
#define BrScalarToFixed(s)	(br_fixed_ls)((s)*(br_scalar)BR_ONE_LS)

#define BrFractionToScalar(f)	(f)
#define BrScalarToFraction(s)	(s)

#define BrUFractionToScalar(f)	(f)
#define BrScalarToUFraction(s)	(s)

/*
 * Various arithmetic operations
 */
#define BR_ADD(a,b)				((a)+(b))
#define BR_SUB(a,b)				((a)-(b))
#define BR_MUL(a,b)				((a)*(b))
#define BR_SQR(a)				((a)*(a))

#define BR_ABS(a)				((br_scalar)fabs(a))
#define BR_NEG(a)				(-(a))

#define BR_DIV(a,b)				((a)/(b))
#define BR_DIVR(a,b)			((a)/(b))
#define BR_MULDIV(a,b,c)		((a)*(b)/(c))
#define BR_RCP(a)				((br_scalar)(1.0/(a)))

#define BR_CONST_MUL(a,b)		((a)*(b))
#define BR_CONST_DIV(a,b)		((a)/(b))


#define BR_MAC2(a,b,c,d)		((a)*(b)+(c)*(d))
#define BR_MAC3(a,b,c,d,e,f)	((a)*(b)+(c)*(d)+(e)*(f))
#define BR_MAC4(a,b,c,d,e,f,g,h) ((a)*(b)+(c)*(d)+(e)*(f)+(g)*(h))

#define BR_MAC2DIV(a,b,c,d,e)		(((a)*(b)+(c)*(d))/e)
#define BR_MAC3DIV(a,b,c,d,e,f,g)	(((a)*(b)+(c)*(d)+(e)*(f))/g)
#define BR_MAC4DIV(a,b,c,d,e,f,g,h,i) (((a)*(b)+(c)*(d)+(e)*(f)+(g)*(h))/i)

#define BR_SQR2(a,b)			((a)*(a)+(b)*(b))
#define BR_SQR3(a,b,c)			((a)*(a)+(b)*(b)+(c)*(c))
#define BR_SQR4(a,b,c,d)		((a)*(a)+(b)*(b)+(c)*(c)+(d)*(d))

#define BR_FMAC2(a,b,c,d)		((a)*(b)+(c)*(d))
#define BR_FMAC3(a,b,c,d,e,f)	((a)*(b)+(c)*(d)+(e)*(f))
#define BR_FMAC4(a,b,c,d,e,f,g,h) ((a)*(b)+(c)*(d)+(e)*(f)+(g)*(h))

#define BR_LENGTH2(a,b)			((br_scalar)sqrt((a)*(a)+(b)*(b)))
#define BR_LENGTH3(a,b,c)		((br_scalar)sqrt((a)*(a)+(b)*(b)+(c)*(c)))
#define BR_LENGTH4(a,b,c,d)		((br_scalar)sqrt((a)*(a)+(b)*(b)+(c)*(c)+(d)*(d)))

#define BR_RLENGTH2(a,b)		((br_scalar)(1.0/sqrt((a)*(a)+(b)*(b))))
#define BR_RLENGTH3(a,b,c)		((br_scalar)(1.0/sqrt((a)*(a)+(b)*(b)+(c)*(c))))
#define BR_RLENGTH4(a,b,c,d)	((br_scalar)(1.0/sqrt((a)*(a)+(b)*(b)+(c)*(c)+(d)*(d))))

#define BR_POW(a,b)				((br_scalar)pow((a),(b)))
#define BR_SQRT(a)				((br_scalar)sqrt(a))

#define BR_FLOOR(a)				((br_scalar)floor(a))
#define BR_CEIL(a)				((br_scalar)ceil(a))

#endif

/*
 * Make sure PI is defined
 */
#ifndef PI
#define PI		3.14159265358979323846
#endif

/*
 * Build independant types
 */
typedef br_fixed_ls		br_scalar_x;
typedef br_fixed_lsf	br_fraction_x;
typedef br_fixed_luf	br_ufraction_x;

typedef float br_scalar_f;
typedef float br_fraction_f;
typedef float br_ufraction_f;

/*
 * Some macros to speed up floating point comparisons
 *
 * N.B. when comparing two scalars in floating point
 * 		at least one must be non-negative
 * 		-0 < 0
 */
#if BASED_FIXED
#define BR_SCALAR_LE_0(a) ((a) <= BR_SCALAR(0.0))
#define BR_SCALAR_GE_1(a) ((a) >= BR_SCALAR(1.0))
#define BR_SCALAR_LE_2_EPSILON(a) ((a) <= 2 * BR_SCALAR_EPSILON)
#define BR_SCALAR_GT_SPECULARPOW_CUTOFF(a) ((a) > SPECULARPOW_CUTOFF)
#define BR_SCALAR_LT(a,b) ((a) < (b))
#define BR_SCALAR_GT(a,b) ((a) > (b))
#endif

#if BASED_FLOAT
#define BR_SCALAR_LE_0(a) (*(br_int_32 *)&(a) <= 0)
#define BR_SCALAR_GE_1(a) (*(br_int_32 *)&(a) >= 0x3f800000)
#define BR_SCALAR_LE_2_EPSILON(a) (*(br_int_32 *)&(a) <= 0x34800000)
#define BR_SCALAR_GT_SPECULARPOW_CUTOFF(a) (*(br_int_32 *)&(a) > 0x3f1e00d2)
#define BR_SCALAR_LT(a,b) (*(br_int_32 *)&(a) < *(br_int_32 *)&(b))
#define BR_SCALAR_GT(a,b) (*(br_int_32 *)&(a) > *(br_int_32 *)&(b))
#endif

#endif

