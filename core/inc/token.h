/*
 * Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: token.h 1.2 1998/09/21 14:53:14 johng Exp $
 * $Locker: $
 *
 */
#ifndef _TOKEN_H_
#define _TOKEN_H_

#ifndef BR_TOKEN_ENUM
#define BR_TOKEN_ENUM	1
#endif

/*
 * Tokens are unique 32 bit numbers that are assoicated with a name
 * and a value type.
 *
 */
#if !BR_TOKEN_ENUM
typedef br_uint_32 br_token;
#endif

/*
 * 0 is reserved to indicate NULL tokens
 */
#ifdef __cplusplus
#define BR_NULL_TOKEN ((const br_token)0)
#else
#define BR_NULL_TOKEN 0
#endif

/*
 * Maximum length (including terminating '\0') of an string value
 */
#define BR_MAX_IDENTIFIER	64

/*
 * Predefined token values
 */
#include "pretok.h"

/*
 * Make some macros for setting token types in terms of scalar
 */
#if BASED_FIXED

#define BRT_AS_SCALAR(tok)			BRT_AS_FIXED(tok)

#define BRT_AS_MATRIX23_SCALAR(tok)	BRT_AS_MATRIX23_FIXED(tok)
#define BRT_AS_MATRIX34_SCALAR(tok)	BRT_AS_MATRIX34_FIXED(tok)
#define BRT_AS_MATRIX4_SCALAR(tok)	BRT_AS_MATRIX4_FIXED(tok)

#define BRT_AS_VECTOR2_SCALAR(tok)	BRT_AS_VECTOR2_FIXED(tok)
#define BRT_AS_VECTOR3_SCALAR(tok)	BRT_AS_VECTOR3_FIXED(tok)
#define BRT_AS_VECTOR4_SCALAR(tok)	BRT_AS_VECTOR4_FIXED(tok)

#endif

#if BASED_FLOAT

#define BRT_AS_SCALAR(tok)			BRT_AS_FLOAT(tok)

#define BRT_AS_MATRIX23_SCALAR(tok)	BRT_AS_MATRIX23_FLOAT(tok)
#define BRT_AS_MATRIX34_SCALAR(tok)	BRT_AS_MATRIX34_FLOAT(tok)
#define BRT_AS_MATRIX4_SCALAR(tok)	BRT_AS_MATRIX4_FLOAT(tok)	

#define BRT_AS_VECTOR2_SCALAR(tok)	BRT_AS_VECTOR2_FLOAT(tok)
#define BRT_AS_VECTOR3_SCALAR(tok)	BRT_AS_VECTOR3_FLOAT(tok)
#define BRT_AS_VECTOR4_SCALAR(tok)	BRT_AS_VECTOR4_FLOAT(tok)

#endif



typedef union br_value_tag {
		br_uint_32		u32;	/* unsigned long word at front for initialisation */

		br_boolean		b;
		void *			p;
		br_token		t;
		br_int_8		i8;
		br_uint_8		u8;
		br_int_16		i16;
		br_uint_16		u16;
		br_int_32		i32;
		br_fixed_ls		x;
		br_float		f;
		br_scalar		s;
		br_angle		a;
		br_colour		rgb;
		struct br_object *o;
		void			*h;
		br_vector2_i	*v2_i;
		br_vector3_i	*v3_i;
		br_vector4_i	*v4_i;
		br_vector2_x	*v2_x;
		br_vector3_x	*v3_x;
		br_vector4_x	*v4_x;
		br_vector2_f	*v2_f;
		br_vector3_f	*v3_f;
		br_vector4_f	*v4_f;
		br_matrix23_x	*m23_x;
		br_matrix34_x	*m34_x;
		br_matrix4_x	*m4_x;
		br_matrix23_f	*m23_f;
		br_matrix34_f	*m34_f;
		br_matrix4_f	*m4_f;
		char *			str;
		char *			cstr;
		struct br_object **ol;
		br_token		*tl;
		void *			*pl;
        struct br_token_value *tvl;
} br_value ;




/*
 * Token value pair
 */
typedef struct br_token_value {
	br_token t ;
   br_value v ;
} br_token_value;

#endif

