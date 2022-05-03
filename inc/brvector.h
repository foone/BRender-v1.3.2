/*
 * Copyright (c) 1992,1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: brvector.h 1.7 1998/10/20 15:53:35 johng Exp $
 * $Locker: $
 *
 * Vector types
 */

#ifndef _VECTOR_H_
#define _VECTOR_H_

/*
 * Scalar vector types
 */

//br_vector2
#ifdef __cplusplus
typedef union{
	br_scalar v[2];
	struct{
        br_scalar x;
        union{
            br_scalar y;
            br_scalar z;
        };
	};
	struct{
        br_scalar s,t; // texture coords
	};
}br_vector2;
#else
typedef struct br_vector2 {
	br_scalar v[2];
} br_vector2;
#endif

//br_vector3
#ifdef __cplusplus
typedef union{
	br_scalar v[3];
	struct{
		br_scalar x,y,z;
	};
    struct{
        br_scalar r,g,b;
    };
}br_vector3;
#else
typedef struct br_vector3 {
	br_scalar v[3];
} br_vector3;
#endif

//br_vector4
#ifdef __cplusplus
typedef union{
	br_scalar v[4];
	struct{
        br_scalar x,y,z; // homogeneous coordinates
        union{
            br_scalar d;
            br_scalar w;
        };
	};
}br_vector4;
#else
typedef struct br_vector4 {
	br_scalar v[4];
} br_vector4;
#endif

/*
 * Fixed vectors
 */
typedef struct br_vector2_x {
	br_fixed_ls v[2];
} br_vector2_x;

typedef struct br_vector3_x {
	br_fixed_ls v[3];
} br_vector3_x;

typedef struct br_vector4_x {
	br_fixed_ls v[4];
} br_vector4_x;


/*
 * Float vectors
 */
typedef struct br_vector2_f {
	br_float v[2];
} br_vector2_f;

typedef struct br_vector3_f {
	br_float v[3];
} br_vector3_f;

typedef struct br_vector4_f {
	br_float v[4];
} br_vector4_f;


/*
 * Integer vectors
 */
typedef struct br_vector2_i {
	br_int_32 v[2];
} br_vector2_i;

typedef struct br_vector3_i {
	br_int_32 v[3];
} br_vector3_i;

typedef struct br_vector4_i {
	br_int_32 v[4];
} br_vector4_i;

typedef struct br_vector2_u16 {
    br_uint_16 v[2];
} br_vector2_u16;

typedef struct br_vector3_u16 {
    br_uint_16 v[3];
} br_vector3_u16;

typedef struct br_vector4_u16 {
    br_uint_16 v[4];
} br_vector4_u16;

/*
 * Fraction vector types
 */
typedef struct br_fvector2 {
	br_fraction v[2];
} br_fvector2;

typedef struct br_fvector3 {
	br_fraction v[3];
} br_fvector3;

typedef struct br_fvector4 {
	br_fraction v[4];
} br_fvector4;

typedef struct br_fvector2_f {
	br_fraction_f v[2];
} br_fvector2_f;

typedef struct br_fvector3_f {
	br_fraction_f v[3];
} br_fvector3_f;

typedef struct br_fvector4_f {
	br_fraction_f v[4];
} br_fvector4_f;

typedef struct br_fvector2_x {
	br_fraction_x v[2];
} br_fvector2_x;

typedef struct br_fvector3_x {
	br_fraction_x v[3];
} br_fvector3_x;

typedef struct br_fvector4_x {
	br_fraction_x v[4];
} br_fvector4_x;


/*
 * General structures for describing an axis aligned bounds
 */
typedef struct br_bounds2 {
	br_vector2 min;		/* Minimum corner */
	br_vector2 max;		/* Maximum corner */
} br_bounds2;

typedef struct br_bounds3 {
	br_vector3 min;		/* Minimum corner */
	br_vector3 max;		/* Maximum corner */
} br_bounds3;


typedef struct br_bounds4 {
	br_vector4 min;		/* Minimum corner */
	br_vector4 max;		/* Maximum corner */
} br_bounds4;

/*
 * Float bounding boxes
 */
typedef struct br_bounds2_f {
	br_vector2_f min;		/* Minimum corner */
	br_vector2_f max;		/* Maximum corner */
} br_bounds2_f;

typedef struct br_bounds3_f {
	br_vector3_f min;		/* Minimum corner */
	br_vector3_f max;		/* Maximum corner */
} br_bounds3_f;


typedef struct br_bounds4_f {
	br_vector4_f min;		/* Minimum corner */
	br_vector4_f max;		/* Maximum corner */
} br_bounds4_f;

/*
 * Fixed bounding boxes
 */
typedef struct br_bounds2_x {
	br_vector2_x min;		/* Minimum corner */
	br_vector2_x max;		/* Maximum corner */
} br_bounds2_x;

typedef struct br_bounds3_x {
	br_vector3_x min;		/* Minimum corner */
	br_vector3_x max;		/* Maximum corner */
} br_bounds3_x;


typedef struct br_bounds4_x {
	br_vector4_x min;		/* Minimum corner */
	br_vector4_x max;		/* Maximum corner */
} br_bounds4_x;

/*
 * Integer bounding boxes
 */
typedef struct br_bounds2_i {
	br_vector2_i min;		/* Minimum corner */
	br_vector2_i max;		/* Maximum corner */
} br_bounds2_i;

typedef struct br_bounds3_i {
	br_vector3_i min;		/* Minimum corner */
	br_vector3_i max;		/* Maximum corner */
} br_bounds3_i;


typedef struct br_bounds4_i {
	br_vector4_i min;		/* Minimum corner */
	br_vector4_i max;		/* Maximum corner */
} br_bounds4_i;

/*
 * Backwards compatibility
 */
typedef struct br_bounds3 br_bounds;
typedef struct br_bounds2_i br_bounds2i;
typedef struct br_bounds3_i br_bounds3i;
typedef struct br_bounds4_i br_bounds4i;

/*
 * Macros for static initialisation
 */
#define BR_VECTOR2(a,b)      {BR_SCALAR(a),BR_SCALAR(b)}
#define BR_VECTOR3(a,b,c)    {BR_SCALAR(a),BR_SCALAR(b),BR_SCALAR(c)}
#define BR_VECTOR4(a,b,c,d)  {BR_SCALAR(a),BR_SCALAR(b),BR_SCALAR(c),BR_SCALAR(d)}

#define BR_FVECTOR2(a,b)     {BR_FRACTION(a),BR_FRACTION(b)}
#define BR_FVECTOR3(a,b,c)   {BR_FRACTION(a),BR_FRACTION(b),BR_FRACTION(c)}
#define BR_FVECTOR4(a,b,c,d) {BR_FRACTION(a),BR_FRACTION(b),BR_FRACTION(c),BR_FRACTION(d)}

/*
 * A structure describing a convex region of 3-space as a list of bounding
 * planes with normals pointing inwards
 */
typedef struct br_convex_region {

	br_vector4 *planes;
	br_uint_32 nplanes;

} br_convex_region;

#endif

