/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: math_ip.h 1.1 1997/12/10 16:41:21 jon Exp $
 * $Locker: $
 *
 * Prototypes for private math functions
 */
#ifndef _MATH_IP_H_
#define _MATH_IP_H_

#ifndef NO_PROTOTYPES
#ifdef __cplusplus
extern "C" {
#endif

/*
 * matrix34.c
 */
void BrMatrix34TApplyFV(br_vector3 *A, br_fvector3 *B, br_matrix34 *C);

/*
 * matrix4.c
 */
void BR_PUBLIC_ENTRY BrMatrix4ShearZ(br_matrix4 *mat, br_scalar sx, br_scalar sy);

/*
 * transform.c
 */
extern br_uint_8 _CombineTransforms[BR_TRANSFORM_MAX][BR_TRANSFORM_MAX];

#define BrTransformCombineTypes(a,b) (_CombineTransforms[(a)][(b)])
#define BrTransformTypeIsLP(a) ((a) != BR_TRANSFORM_MATRIX34)
#define BrTransformTypeIsMatrix34(a) ((a) <= BR_TRANSFORM_MATRIX34_LP)

/* result = a/b * 2^31
 */
br_fixed_ls BR_ASM_CALL BrFixedDivP(br_fixed_ls a, br_fixed_ls b);

#ifdef __cplusplus
};
#endif

/*
 * Inline versions of vector ops.
 */
#ifndef _NO_VECTOR_MACROS
#ifndef _VECIFNS_H_
#include "vecifns.h"
#endif
#endif

#endif

#endif
