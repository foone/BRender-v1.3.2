/*
 * Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: tokenmap.h 1.1 1997/12/10 16:41:11 jon Exp $
 * $Locker: $
 *
 */
#ifndef _TOKENMAP_H_
#define _TOKENMAP_H_

/*
 * Template structure used to map token/values to a structure
 */
typedef struct br_tv_template_entry {
	br_token token;
	char * name;
	br_int_32 offset;
	br_int_16 flags;
	br_int_16 conv;
	br_int_32 conv_arg;

	br_int_32 mask;
} br_tv_template_entry;

typedef struct br_tv_template {
	int n_entries;
	struct br_tv_template_entry *entries;

	/*
	 * Cached mapping information
	 */
	br_int_32 map_base;
	br_int_32 n_map_entries;

	br_tv_template_entry **map_query_entry;
	br_tv_template_entry **map_set_entry;

	br_boolean names_resolved;

	/*
	 * Resource to which cache info gets attached (if it was attached directly
	 * to template, then template could not be static)
	 */
	void *res;
} br_tv_template;

/*
 * Defined operations for Query/Set conversions
 *
 * In two part names: token value on left, struct value on right
 */
enum {
	BRTV_CONV_NOP,

	BRTV_CONV_DIRECT,

	BRTV_CONV_CUSTOM,

	BRTV_CONV_COPY,

	BRTV_CONV_V2_COPY,
	BRTV_CONV_V3_COPY,
	BRTV_CONV_V4_COPY,
	BRTV_CONV_M23_COPY,
	BRTV_CONV_M34_COPY,
	BRTV_CONV_M4_COPY,

	BRTV_CONV_I32_I8,
	BRTV_CONV_I32_I16,

	BRTV_CONV_I32_U8,
	BRTV_CONV_I32_U16,

	BRTV_CONV_FIXED_FLOAT,
	BRTV_CONV_FLOAT_FIXED,

	BRTV_CONV_V2_FIXED_FLOAT,
	BRTV_CONV_V2_FLOAT_FIXED,

	BRTV_CONV_V3_FIXED_FLOAT,
	BRTV_CONV_V3_FLOAT_FIXED,

	BRTV_CONV_V4_FIXED_FLOAT,
	BRTV_CONV_V4_FLOAT_FIXED,

	BRTV_CONV_M23_FIXED_FLOAT,
	BRTV_CONV_M23_FLOAT_FIXED,

	BRTV_CONV_M34_FIXED_FLOAT,
	BRTV_CONV_M34_FLOAT_FIXED,

	BRTV_CONV_M4_FIXED_FLOAT,
	BRTV_CONV_M4_FLOAT_FIXED,

	BRTV_CONV_STRING,

	BRTV_CONV_LIST,

	BRTV_CONV_BOOL_TOKEN,

	BRTV_CONV_BIT,
	BRTV_CONV_NOT_BIT
};

/*
 * Booleans are 32 bit at the moment
 */
#define	BRTV_CONV_I32_B  BRTV_CONV_COPY

/*
 * Typedef for custom conversion function
 */
typedef br_error BR_CALLBACK br_tv_custom_query_cbfn(br_uint_32 *pvalue, void **extra, br_size_t *pextra_size,
	void *block, struct br_tv_template_entry *tep);
typedef br_error BR_CALLBACK br_tv_custom_set_cbfn(void *block, br_uint_32 *pvalue, struct br_tv_template_entry *tep);
typedef br_size_t BR_CALLBACK br_tv_custom_extra_size_cbfn(void *block, struct br_tv_template_entry *tep);

typedef struct br_tv_custom {
	br_tv_custom_query_cbfn *query;
	br_tv_custom_set_cbfn *set;
	br_tv_custom_extra_size_cbfn *extra_size;
} br_tv_custom;

/*
 * Define some operations in terms of the current scalar type
 */
#if BASED_FLOAT
#define BRTV_CONV_FIXED_SCALAR		BRTV_CONV_FIXED_FLOAT
#define BRTV_CONV_FLOAT_SCALAR		BRTV_CONV_COPY

#define BRTV_CONV_V2_FIXED_SCALAR	BRTV_CONV_V2_FIXED_FLOAT
#define BRTV_CONV_V2_FLOAT_SCALAR	BRTV_CONV_V2_COPY

#define BRTV_CONV_V3_FIXED_SCALAR	BRTV_CONV_V3_FIXED_FLOAT
#define BRTV_CONV_V3_FLOAT_SCALAR	BRTV_CONV_V3_COPY

#define BRTV_CONV_V4_FIXED_SCALAR	BRTV_CONV_V4_FIXED_FLOAT
#define BRTV_CONV_V4_FLOAT_SCALAR	BRTV_CONV_V4_COPY

#define BRTV_CONV_M23_FIXED_SCALAR	BRTV_CONV_M23_FIXED_FLOAT
#define BRTV_CONV_M23_FLOAT_SCALAR	BRTV_CONV_M23_COPY

#define BRTV_CONV_M34_FIXED_SCALAR	BRTV_CONV_M34_FIXED_FLOAT
#define BRTV_CONV_M34_FLOAT_SCALAR	BRTV_CONV_M34_COPY

#define BRTV_CONV_M4_FIXED_SCALAR	BRTV_CONV_M4_FIXED_FLOAT
#define BRTV_CONV_M4_FLOAT_SCALAR	BRTV_CONV_M4_COPY
#endif

#if BASED_FIXED
#define BRTV_CONV_FIXED_SCALAR		BRTV_CONV_COPY      
#define BRTV_CONV_FLOAT_SCALAR		BRTV_CONV_FLOAT_FIXED

#define BRTV_CONV_V2_FIXED_SCALAR	BRTV_CONV_V2_COPY      
#define BRTV_CONV_V2_FLOAT_SCALAR	BRTV_CONV_V2_FLOAT_FIXED

#define BRTV_CONV_V3_FIXED_SCALAR	BRTV_CONV_V3_COPY      
#define BRTV_CONV_V3_FLOAT_SCALAR	BRTV_CONV_V3_FLOAT_FIXED

#define BRTV_CONV_V4_FIXED_SCALAR	BRTV_CONV_V4_COPY      
#define BRTV_CONV_V4_FLOAT_SCALAR	BRTV_CONV_V4_FLOAT_FIXED

#define BRTV_CONV_M23_FIXED_SCALAR	BRTV_CONV_M23_COPY      
#define BRTV_CONV_M23_FLOAT_SCALAR	BRTV_CONV_M23_FLOAT_FIXED

#define BRTV_CONV_M34_FIXED_SCALAR	BRTV_CONV_M34_COPY      
#define BRTV_CONV_M34_FLOAT_SCALAR	BRTV_CONV_M34_FLOAT_FIXED

#define BRTV_CONV_M4_FIXED_SCALAR	BRTV_CONV_M4_COPY      
#define BRTV_CONV_M4_FLOAT_SCALAR	BRTV_CONV_M4_FLOAT_FIXED
#endif

/*
 * Conversion flags
 */
enum br_tv_flag {
	BRTV_QUERY		= 0x0001,	/* This entry is valid for Query operations	*/
	BRTV_SET		= 0x0002,	/* This entry is valid for Set operations	*/
	BRTV_ALL		= 0x0004,	/* This entry is valid for QueryAll operations	*/

	BRTV_ABS		= 0x0008	/* 'offset' member of template contains value	*/
};

#endif


