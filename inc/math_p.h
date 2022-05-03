/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: math_p.h 1.7 1998/10/16 10:33:15 johng Exp $
 * $Locker: $
 *
 * Public function prototypes for BRender maths support
 */
#ifndef _MATH_P_H_
#define _MATH_P_H_

#ifndef _NO_PROTOTYPES

#ifdef __cplusplus
extern "C" {
#endif

/**
 ** Fixed Point
 **/

/* result = abs(a)
 */
br_fixed_ls BR_ASM_CALL BrFixedAbs(br_fixed_ls a);

/* result = a*b
 */
br_fixed_ls BR_ASM_CALL BrFixedMul(br_fixed_ls a, br_fixed_ls b);

/* result = a*b + c*d
 */
br_fixed_ls BR_ASM_CALL BrFixedMac2(br_fixed_ls a, br_fixed_ls b,
									br_fixed_ls c, br_fixed_ls d);

/* result = a*b + c*d + e*f
 */
br_fixed_ls BR_ASM_CALL BrFixedMac3(br_fixed_ls a, br_fixed_ls b,
									br_fixed_ls c, br_fixed_ls d,
									br_fixed_ls e, br_fixed_ls f);

/* result = a*b + c*d + e*f + g*h
 */
br_fixed_ls BR_ASM_CALL BrFixedMac4(br_fixed_ls a, br_fixed_ls b,
									br_fixed_ls c, br_fixed_ls d,
									br_fixed_ls e, br_fixed_ls f,
									br_fixed_ls g, br_fixed_ls h);

/* result = a*a
 */
br_fixed_ls BR_ASM_CALL BrFixedSqr(br_fixed_ls a);

/* result = a*a + b*b
 */
br_fixed_ls BR_ASM_CALL BrFixedSqr2(br_fixed_ls a,
									br_fixed_ls b);

/* result = a*a + b*b + c*c
 */
br_fixed_ls BR_ASM_CALL BrFixedSqr3(br_fixed_ls a,
									br_fixed_ls b,
									br_fixed_ls c);
/* result = a*a + b*b + c*c + d*d
 */
br_fixed_ls BR_ASM_CALL BrFixedSqr4(br_fixed_ls a,
									br_fixed_ls b,
									br_fixed_ls c,
									br_fixed_ls d);


/* result = sqrt(a*a + b*b)
 */
br_fixed_ls BR_ASM_CALL BrFixedLength2(br_fixed_ls a, br_fixed_ls b);

/* result = sqrt(a*a + b*b + c*c)
 */
br_fixed_ls BR_ASM_CALL BrFixedLength3(br_fixed_ls a,
									   br_fixed_ls b,
									   br_fixed_ls c);

/* result = sqrt(a*a + b*b + c*c + d*d)
 */
br_fixed_ls BR_ASM_CALL BrFixedLength4(br_fixed_ls a,
									   br_fixed_ls b,
									   br_fixed_ls c,
									   br_fixed_ls d);

/* result = 1/sqrt(a*a + b*b) (low precision)
 */
br_fixed_ls BR_ASM_CALL BrFixedRLength2(br_fixed_ls a, br_fixed_ls b);

/* result = 1/sqrt(a*a + b*b + c*c) (low precision)
 */
br_fixed_ls BR_ASM_CALL BrFixedRLength3(br_fixed_ls a,
										br_fixed_ls b,
										br_fixed_ls c);

/* result = 1/sqrt(a*a + b*b + c*c + d*d) (low precision)
 */
br_fixed_ls BR_ASM_CALL BrFixedRLength4(br_fixed_ls a,
										br_fixed_ls b,
										br_fixed_ls c,
										br_fixed_ls d);			

/* result = a/b
 */
br_fixed_ls BR_ASM_CALL BrFixedDiv(br_fixed_ls a, br_fixed_ls b);

/* result = a/b * 2^31
 */
br_fixed_ls BR_ASM_CALL BrFixedDivF(br_fixed_ls a, br_fixed_ls b);

/* result = a/b (rounded towards 0)
 */
br_fixed_ls BR_ASM_CALL BrFixedDivR(br_fixed_ls a, br_fixed_ls b);

/* 
 */
br_fixed_ls BR_ASM_CALL _FixedDivP(br_fixed_ls a, br_fixed_ls b);


/* result = a*b/c
 */
br_fixed_ls BR_ASM_CALL BrFixedMulDiv(br_fixed_ls a,
									  br_fixed_ls b,
									  br_fixed_ls c);

/* result = (a*b + c*d)/e
 */
br_fixed_ls BR_ASM_CALL BrFixedMac2Div(br_fixed_ls a, br_fixed_ls b,
									br_fixed_ls c, br_fixed_ls d,
									br_fixed_ls e);

/* result = (a*b + c*d + e*f)/g
 */
br_fixed_ls BR_ASM_CALL BrFixedMac3Div(br_fixed_ls a, br_fixed_ls b,
									br_fixed_ls c, br_fixed_ls d,
									br_fixed_ls e, br_fixed_ls f,
									br_fixed_ls g);

/* result = (a*b + c*d + e*f + g*h)/i
 */
br_fixed_ls BR_ASM_CALL BrFixedMac4Div(br_fixed_ls a, br_fixed_ls b,
									br_fixed_ls c, br_fixed_ls d,
									br_fixed_ls e, br_fixed_ls f,
									br_fixed_ls g, br_fixed_ls h,
									br_fixed_ls i);
/* result = 1.0/a
 */
br_fixed_ls BR_ASM_CALL BrFixedRcp(br_fixed_ls a);

/*
 * Various combinations with fractions
 */

/* result = a*b + c*d - a & c are fractions
 */
br_fixed_ls BR_ASM_CALL BrFixedFMac2(br_fixed_lsf a, br_fixed_ls b,
									 br_fixed_lsf c, br_fixed_ls d);

/* result = a*b + c*d + e*f - a,c & e are fractions
 */
br_fixed_ls BR_ASM_CALL BrFixedFMac3(br_fixed_lsf a, br_fixed_ls b,
									 br_fixed_lsf c, br_fixed_ls d,
									 br_fixed_lsf e, br_fixed_ls f);

/* result = a*b + c*d + e*f + g*h (a,c,e,g are fractions)
 */
br_fixed_ls BR_ASM_CALL BrFixedFMac4(br_fixed_ls a, br_fixed_ls b,
									br_fixed_ls c, br_fixed_ls d,
									br_fixed_ls e, br_fixed_ls f,
									br_fixed_ls g, br_fixed_ls h);

/*
 * Misc. support  functions
 */
br_fixed_ls BR_ASM_CALL BrFixedSin(br_angle a);

br_fixed_ls BR_ASM_CALL BrFixedCos(br_angle a);

br_angle BR_ASM_CALL BrFixedASin(br_fixed_ls s);

br_angle BR_ASM_CALL BrFixedACos(br_fixed_ls c);

br_angle BR_ASM_CALL BrFixedATan2(br_fixed_ls x, br_fixed_ls y);

br_angle BR_ASM_CALL BrFixedATan2Fast(br_fixed_ls x, br_fixed_ls y);

br_fixed_ls BR_PUBLIC_ENTRY BrFixedSqrt(br_fixed_ls a);
br_fixed_ls BR_PUBLIC_ENTRY BrFixedPow(br_fixed_ls a,br_fixed_ls b);

/*
 * Integer sqrt functions
 */
br_uint_32 BR_ASM_CALL BrSqrt(br_uint_32 a);
br_uint_32 BR_ASM_CALL BrFastSqrt(br_uint_32 a);
br_uint_32 BR_ASM_CALL BrFastRSqrt(br_uint_32 a);

/*
 * 3x4 Matrix ops.
 */
void BR_PUBLIC_ENTRY BrMatrix34Copy(br_matrix34 *A, br_matrix34 *b);
void BR_PUBLIC_ENTRY BrMatrix34Mul(br_matrix34 *A, br_matrix34 *B, br_matrix34 *C);
void BR_PUBLIC_ENTRY BrMatrix34Pre(br_matrix34 *mat , br_matrix34 *A);
void BR_PUBLIC_ENTRY BrMatrix34Post(br_matrix34 *mat , br_matrix34 *A);

void BR_PUBLIC_ENTRY BrMatrix34Identity(br_matrix34 *mat);

void BR_PUBLIC_ENTRY BrMatrix34RotateX(br_matrix34 *mat, br_angle rx);
void BR_PUBLIC_ENTRY BrMatrix34PreRotateX(br_matrix34 *mat, br_angle rx);
void BR_PUBLIC_ENTRY BrMatrix34PostRotateX(br_matrix34 *mat, br_angle rx);

void BR_PUBLIC_ENTRY BrMatrix34RotateY(br_matrix34 *mat, br_angle ry);
void BR_PUBLIC_ENTRY BrMatrix34PreRotateY(br_matrix34 *mat, br_angle ry);
void BR_PUBLIC_ENTRY BrMatrix34PostRotateY(br_matrix34 *mat, br_angle ry);

void BR_PUBLIC_ENTRY BrMatrix34RotateZ(br_matrix34 *mat, br_angle rz);
void BR_PUBLIC_ENTRY BrMatrix34PreRotateZ(br_matrix34 *mat, br_angle rz);
void BR_PUBLIC_ENTRY BrMatrix34PostRotateZ(br_matrix34 *mat, br_angle rz);

void BR_PUBLIC_ENTRY BrMatrix34Rotate(br_matrix34 *mat, br_angle r, br_vector3 *axis);
void BR_PUBLIC_ENTRY BrMatrix34PreRotate(br_matrix34 *mat, br_angle r, br_vector3 *axis);
void BR_PUBLIC_ENTRY BrMatrix34PostRotate(br_matrix34 *mat, br_angle r, br_vector3 *axis);

void BR_PUBLIC_ENTRY BrMatrix34Translate(br_matrix34 *mat, br_scalar x, br_scalar y, br_scalar z);
void BR_PUBLIC_ENTRY BrMatrix34PreTranslate(br_matrix34 *mat, br_scalar x, br_scalar y, br_scalar z);
void BR_PUBLIC_ENTRY BrMatrix34PostTranslate(br_matrix34 *mat, br_scalar x, br_scalar y, br_scalar z);

void BR_PUBLIC_ENTRY BrMatrix34Scale(br_matrix34 *mat, br_scalar sx, br_scalar sy, br_scalar sz);
void BR_PUBLIC_ENTRY BrMatrix34PreScale(br_matrix34 *mat, br_scalar sx, br_scalar sy, br_scalar sz);

void BR_PUBLIC_ENTRY BrMatrix34PostScale(br_matrix34 *mat, br_scalar sx, br_scalar sy, br_scalar sz);

void BR_PUBLIC_ENTRY BrMatrix34ShearX(br_matrix34 *mat, br_scalar sy, br_scalar sz);
void BR_PUBLIC_ENTRY BrMatrix34PreShearX(br_matrix34 *mat, br_scalar sy, br_scalar sz);
void BR_PUBLIC_ENTRY BrMatrix34PostShearX(br_matrix34 *mat, br_scalar sy, br_scalar sz);

void BR_PUBLIC_ENTRY BrMatrix34ShearY(br_matrix34 *mat, br_scalar sx, br_scalar sz);
void BR_PUBLIC_ENTRY BrMatrix34PreShearY(br_matrix34 *mat, br_scalar sx, br_scalar sz);
void BR_PUBLIC_ENTRY BrMatrix34PostShearY(br_matrix34 *mat, br_scalar sx, br_scalar sz);

void BR_PUBLIC_ENTRY BrMatrix34ShearZ(br_matrix34 *mat, br_scalar sx, br_scalar sy);
void BR_PUBLIC_ENTRY BrMatrix34PreShearZ(br_matrix34 *mat, br_scalar sx, br_scalar sy);
void BR_PUBLIC_ENTRY BrMatrix34PostShearZ(br_matrix34 *mat, br_scalar sx, br_scalar sy);

void BR_PUBLIC_ENTRY BrMatrix34ApplyV(br_vector3 *A, br_vector3 *B, br_matrix34 *C);
void BR_PUBLIC_ENTRY BrMatrix34ApplyP(br_vector3 *A, br_vector3 *B, br_matrix34 *C);
void BR_PUBLIC_ENTRY BrMatrix34Apply(br_vector3 *A,  br_vector4 *B, br_matrix34 *C);

void BR_PUBLIC_ENTRY BrMatrix34TApplyV(br_vector3 *A, br_vector3 *B, br_matrix34 *C);
void BR_PUBLIC_ENTRY BrMatrix34TApplyP(br_vector3 *A, br_vector3 *B, br_matrix34 *C);
void BR_PUBLIC_ENTRY BrMatrix34TApply(br_vector4 *A, br_vector4 *B, br_matrix34 *C);

br_scalar BR_PUBLIC_ENTRY BrMatrix34Inverse(br_matrix34 *out, br_matrix34 *in);
void BR_PUBLIC_ENTRY BrMatrix34LPInverse(br_matrix34 *A, br_matrix34 *B);
void BR_PUBLIC_ENTRY BrMatrix34LPNormalise(br_matrix34 *A, br_matrix34 *B);

void BR_PUBLIC_ENTRY BrMatrix34RollingBall(br_matrix34 *mat, int dx,int dy, int radius);

br_matrix34 * BR_PUBLIC_ENTRY BrBoundsToMatrix34( br_matrix34 *mat, br_bounds *bounds);

void BR_PUBLIC_ENTRY BrMatrix34ApplyBounds(br_bounds *A, br_bounds *B, br_matrix34 *C);

void BR_PUBLIC_ENTRY BrMatrix34ApplyPlaneEquation(br_vector4 *A, br_vector4 *B, br_matrix34 *C);

/*
 * 4x4 Matrix ops.
 */
void BR_PUBLIC_ENTRY BrMatrix4Copy(br_matrix4 *A, br_matrix4 *B);
void BR_PUBLIC_ENTRY BrMatrix4Mul(br_matrix4 *A, br_matrix4 *B, br_matrix4 *C);
void BR_PUBLIC_ENTRY BrMatrix4Identity(br_matrix4 *mat);
void BR_PUBLIC_ENTRY BrMatrix4Scale(br_matrix4 *mat, br_scalar sx, br_scalar sy, br_scalar sz);

br_scalar BR_PUBLIC_ENTRY BrMatrix4Inverse(br_matrix4 *A, br_matrix4 *B);
void BR_PUBLIC_ENTRY BrMatrix4Adjoint(br_matrix4 *A, br_matrix4 *B);
br_scalar BR_PUBLIC_ENTRY BrMatrix4Determinant(br_matrix4 *mat);

void BR_PUBLIC_ENTRY BrMatrix4Perspective(br_matrix4 *mat, 
		br_angle field_of_view,
		br_scalar aspect,
		br_scalar hither,
		br_scalar yon);

void BR_PUBLIC_ENTRY BrMatrix4ApplyV(br_vector4 *A, br_vector3 *B, br_matrix4 *C);
void BR_PUBLIC_ENTRY BrMatrix4ApplyP(br_vector4 *A, br_vector3 *B, br_matrix4 *C);
void BR_PUBLIC_ENTRY BrMatrix4Apply(br_vector4 *A,  br_vector4 *B, br_matrix4 *C);

void BR_PUBLIC_ENTRY BrMatrix4TApplyV(br_vector4 *A, br_vector3 *B, br_matrix4 *C);
void BR_PUBLIC_ENTRY BrMatrix4TApplyP(br_vector4 *A, br_vector3 *B, br_matrix4 *C);
void BR_PUBLIC_ENTRY BrMatrix4TApply(br_vector4 *A,  br_vector4 *B, br_matrix4 *C);

void BR_PUBLIC_ENTRY BrMatrix4Pre34(br_matrix4 *A, br_matrix34 *B);
void BR_PUBLIC_ENTRY BrMatrix4Mul34(br_matrix4 *A, br_matrix34 *B, br_matrix4 *C);
void BR_PUBLIC_ENTRY BrMatrix4Copy34(br_matrix4 *A, br_matrix34 *B);
void BR_PUBLIC_ENTRY BrMatrix34Copy4(br_matrix34 *A, br_matrix4 *B);


/**
 ** 2D Vectors
 **/
void BR_PUBLIC_ENTRY BrVector2Copy(br_vector2 *v1,br_vector2 * v2);
void BR_PUBLIC_ENTRY BrVector2Set(br_vector2 *v1, br_scalar s1, br_scalar s2);
void BR_PUBLIC_ENTRY BrVector2SetInt(br_vector2 *v1, int i1, int i2);
void BR_PUBLIC_ENTRY BrVector2SetFloat(br_vector2 *v1, float f1, float f2);
void BR_PUBLIC_ENTRY BrVector2Negate(br_vector2 *v1, br_vector2 *v2);
void BR_PUBLIC_ENTRY BrVector2Add(br_vector2 *v1, br_vector2 *v2, br_vector2 *v3);
void BR_PUBLIC_ENTRY BrVector2Accumulate(br_vector2 *v1, br_vector2 *v2);
void BR_PUBLIC_ENTRY BrVector2Sub(br_vector2 *v1, br_vector2 *v2, br_vector2 *v3);
void BR_PUBLIC_ENTRY BrVector2Scale(br_vector2 *v1, br_vector2 *v2, br_scalar s);
void BR_PUBLIC_ENTRY BrVector2InvScale(br_vector2 *v1, br_vector2 *v2, br_scalar s);
br_scalar BR_PUBLIC_ENTRY BrVector2Dot(br_vector2 *v1, br_vector2 *v2);
br_scalar BR_PUBLIC_ENTRY BrVector2Length(br_vector2 *v1);
br_scalar BR_PUBLIC_ENTRY BrVector2LengthSquared(br_vector2 *v1);

/**
 ** 3D VECTORS
 **/
void BR_PUBLIC_ENTRY BrVector3Copy(br_vector3 *v1, br_vector3 *v2);
void BR_PUBLIC_ENTRY BrVector3Set(br_vector3 *v1, br_scalar s1, br_scalar s2, br_scalar s3);
void BR_PUBLIC_ENTRY BrVector3SetInt(br_vector3 *v1, int i1, int i2, int i3);
void BR_PUBLIC_ENTRY BrVector3SetFloat(br_vector3 *v1, float f1, float f2, float f3);
void BR_PUBLIC_ENTRY BrVector3Negate(br_vector3 *v1,br_vector3 *v2);
void BR_PUBLIC_ENTRY BrVector3Add(br_vector3 *v1, br_vector3 *v2, br_vector3 *v3);
void BR_PUBLIC_ENTRY BrVector3Accumulate(br_vector3 *v1, br_vector3 *v2);
void BR_PUBLIC_ENTRY BrVector3Sub(br_vector3 *v1, br_vector3 *v2, br_vector3 *v3);
void BR_PUBLIC_ENTRY BrVector3Mul(br_vector3 *v1, br_vector3 *v2, br_vector3 *v3);
void BR_PUBLIC_ENTRY BrVector3Scale(br_vector3 *v1, br_vector3 *v2, br_scalar s);
void BR_PUBLIC_ENTRY BrVector3InvScale(br_vector3 *v1, br_vector3 *v2, br_scalar s);
br_scalar BR_PUBLIC_ENTRY BrVector3Dot(br_vector3 *v1, br_vector3 *v2);
void BR_PUBLIC_ENTRY BrVector3Cross(br_vector3 *v1,br_vector3 *v2,br_vector3 *v3);
br_scalar BR_PUBLIC_ENTRY BrVector3Length(br_vector3 *v1);
br_scalar BR_PUBLIC_ENTRY BrVector3LengthSquared(br_vector3 *v1);

void BR_PUBLIC_ENTRY BrVector3Normalise(br_vector3 *v1,br_vector3 *v2);
void BR_PUBLIC_ENTRY BrVector3NormaliseQuick(br_vector3 *v1,br_vector3 *v2);
void BR_PUBLIC_ENTRY BrVector3NormaliseLP(br_vector3 *v1,br_vector3 *v2);

br_int_32 BR_PUBLIC_ENTRY BrPlaneEquation(br_vector4 *eqn, br_vector3 *v0, br_vector3 *v1, br_vector3 *v2);

/*
 * 2D vectors
 */
void BR_PUBLIC_ENTRY BrVector2Normalise(br_vector2 *v1,br_vector2 *v2);

/**
 ** 4D Vectors
 **/
br_scalar BR_PUBLIC_ENTRY BrVector4Dot(br_vector4 *v1, br_vector4 *v2);
void BR_PUBLIC_ENTRY BrVector4Copy(br_vector4 *v1, br_vector4 *v2);

/*
 * Euler Angles
 */
br_matrix34 * BR_PUBLIC_ENTRY BrEulerToMatrix34(br_matrix34 *mat,br_euler *euler);
br_euler * BR_PUBLIC_ENTRY BrMatrix34ToEuler(br_euler *euler,br_matrix34 *mat);

br_matrix4 * BR_PUBLIC_ENTRY BrEulerToMatrix4(br_matrix4 *mat,br_euler *src);
br_euler * BR_PUBLIC_ENTRY BrMatrix4ToEuler(br_euler *dest,br_matrix4 *mat);

br_quat * BR_PUBLIC_ENTRY BrEulerToQuat(br_quat *quat,br_euler *euler);
br_euler * BR_PUBLIC_ENTRY BrQuatToEuler(br_euler *euler,br_quat *quat);

/*
 * Quaternions
 */
br_quat * BR_PUBLIC_ENTRY BrQuatMul(br_quat *q,br_quat *l,br_quat *r);
br_quat * BR_PUBLIC_ENTRY BrQuatNormalise(br_quat *q,br_quat *qq);
br_quat * BR_PUBLIC_ENTRY BrQuatInvert(br_quat *q,br_quat *qq);

br_quat * BR_PUBLIC_ENTRY BrQuatSlerp(br_quat *q,br_quat *l,br_quat *r,br_scalar t, br_int_16 spins);

br_matrix34 * BR_PUBLIC_ENTRY BrQuatToMatrix34(br_matrix34 *mat,br_quat *q);
br_quat * BR_PUBLIC_ENTRY BrMatrix34ToQuat(br_quat *q,br_matrix34 *mat);

br_matrix4 * BR_PUBLIC_ENTRY BrQuatToMatrix4(br_matrix4 *mat,br_quat *q);
br_quat * BR_PUBLIC_ENTRY BrMatrix4ToQuat(br_quat *q,br_matrix4 *mat);

/*
 * Transforms
 */
void BR_PUBLIC_ENTRY BrTransformToMatrix34(br_matrix34 *mat,br_transform *xform);
void BR_PUBLIC_ENTRY BrMatrix34ToTransform(br_transform *xform,br_matrix34 *mat);
void BR_PUBLIC_ENTRY BrTransformToTransform(br_transform *dest,br_transform *src);

void BR_PUBLIC_ENTRY BrMatrix34PreTransform(br_matrix34 *mat,br_transform *xform);
void BR_PUBLIC_ENTRY BrMatrix34PostTransform(br_matrix34 *mat,br_transform *xform);
void BR_PUBLIC_ENTRY BrMatrix4PreTransform(br_matrix4 *mat,br_transform *xform);

/*
 * 2x3 Matrix ops.
 */
void BR_PUBLIC_ENTRY BrMatrix23Copy(br_matrix23 *A, br_matrix23 *b);
void BR_PUBLIC_ENTRY BrMatrix23Mul(br_matrix23 *A, br_matrix23 *B, br_matrix23 *C);
void BR_PUBLIC_ENTRY BrMatrix23Pre(br_matrix23 *mat , br_matrix23 *A);
void BR_PUBLIC_ENTRY BrMatrix23Post(br_matrix23 *mat , br_matrix23 *A);

void BR_PUBLIC_ENTRY BrMatrix23Identity(br_matrix23 *mat);

void BR_PUBLIC_ENTRY BrMatrix23Rotate(br_matrix23 *mat, br_angle rz);
void BR_PUBLIC_ENTRY BrMatrix23PreRotate(br_matrix23 *mat, br_angle rz);
void BR_PUBLIC_ENTRY BrMatrix23PostRotate(br_matrix23 *mat, br_angle rz);

void BR_PUBLIC_ENTRY BrMatrix23Translate(br_matrix23 *mat, br_scalar x, br_scalar y);
void BR_PUBLIC_ENTRY BrMatrix23PreTranslate(br_matrix23 *mat, br_scalar x, br_scalar y);
void BR_PUBLIC_ENTRY BrMatrix23PostTranslate(br_matrix23 *mat, br_scalar x, br_scalar y);

void BR_PUBLIC_ENTRY BrMatrix23Scale(br_matrix23 *mat, br_scalar sx, br_scalar sy);
void BR_PUBLIC_ENTRY BrMatrix23PreScale(br_matrix23 *mat, br_scalar sx, br_scalar sy);
void BR_PUBLIC_ENTRY BrMatrix23PostScale(br_matrix23 *mat, br_scalar sx, br_scalar sy);

void BR_PUBLIC_ENTRY BrMatrix23ShearX(br_matrix23 *mat, br_scalar sy);
void BR_PUBLIC_ENTRY BrMatrix23PreShearX(br_matrix23 *mat, br_scalar sy);
void BR_PUBLIC_ENTRY BrMatrix23PostShearX(br_matrix23 *mat, br_scalar sy);

void BR_PUBLIC_ENTRY BrMatrix23ShearY(br_matrix23 *mat, br_scalar sx);
void BR_PUBLIC_ENTRY BrMatrix23PreShearY(br_matrix23 *mat, br_scalar sx);
void BR_PUBLIC_ENTRY BrMatrix23PostShearY(br_matrix23 *mat, br_scalar sx);

void BR_PUBLIC_ENTRY BrMatrix23ApplyV(br_vector2 *A, br_vector2 *B, br_matrix23 *C);
void BR_PUBLIC_ENTRY BrMatrix23ApplyP(br_vector2 *A, br_vector2 *B, br_matrix23 *C);

void BR_PUBLIC_ENTRY BrMatrix23TApplyV(br_vector2 *A, br_vector2 *B, br_matrix23 *C);
void BR_PUBLIC_ENTRY BrMatrix23TApplyP(br_vector2 *A, br_vector2 *B, br_matrix23 *C);

br_scalar BR_PUBLIC_ENTRY BrMatrix23Inverse(br_matrix23 *out, br_matrix23 *in);
void BR_PUBLIC_ENTRY BrMatrix23LPInverse(br_matrix23 *A, br_matrix23 *B);
void BR_PUBLIC_ENTRY BrMatrix23LPNormalise(br_matrix23 *A, br_matrix23 *B);

/*
 * Backwards compatibility
 */
#define BrMatrix34Transform BrTransformToMatrix34
#define BrTransformTransfer BrTransformToTransform

#ifdef __cplusplus
};
#endif
#endif /* _NO_PROTOTYPES */

/*
 * Optional inline fixed point maths
 */
#ifndef NO_PROTOTYPES
#if INLINE_FIXED

#ifdef __GNUC__
#include "g386ifix.h"
#endif

#ifdef __WATCOMC__
#include "wtcifix.h"
#endif

#if defined(_MSC_VER) && !defined(_USE_INTEL_COMPILER)
#include "mscifix.h"
#endif

#if defined(_USE_INTEL_COMPILER)
#include "proifix.h"
#endif

#endif
#endif

/*
 * Inline versions of some maths functions
 */
#ifndef NO_PROTOTYPES
#ifndef NO_INLINE_MATH

#if defined(_MSC_VER) && !defined(_USE_INTEL_COMPILER)
#include "mscimath.h"
#endif

#endif
#endif

/*
 * Inline versions of vector ops.
 */
#ifndef _NO_VECTOR_MACROS
#ifndef _VECFNS_H_
#include "vecfns.h"
#endif
#endif

/*
 * Extra compiler specific prototype info.
 */
#ifndef _MATH_X_H_
#include "math_x.h"
#endif

#endif
