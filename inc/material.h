/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: material.h 1.12 1998/11/13 16:22:27 jon Exp $
 * $Locker: $
 *
 * Describes the appearance of a material than can be applied to a surface
	Last change:  TN    9 Apr 97    4:41 pm
 */
#ifndef _MATERIAL_H_
#define _MATERIAL_H_

typedef struct br_material {
	br_uint_32 _reserved;

	char *identifier;

	br_colour colour;

	br_uint_8 opacity;

	br_ufraction ka;
	br_ufraction kd;
	br_ufraction ks;

	/*
	 * Specular power
	 */
	br_scalar power;

	/*
	 * Controls how texture is rendered
	 */
	br_uint_32 flags;

	/*
	 * 2x3 transform applied to model U,V
	 */
	br_matrix23 map_transform;

	/*
	 * Various mode bit fields
	 */
	br_uint_16 mode;

	/*
	 * The direct index ramp base and range
	 */
	br_uint_8 index_base;
	br_uint_8 index_range;

	/*
	 * Pointers to colour map
	 */
	br_pixelmap *colour_map;

	/*
	 * Pointers to tables
	 */
	br_pixelmap *screendoor;		/* Screen door opacity  */

	br_pixelmap *index_shade;		/* Index shading        */
	br_pixelmap *index_blend;		/* Index blending       */
	br_pixelmap *index_fog;			/* Index fogging        */

	br_token_value *extra_surf;
	br_token_value *extra_prim;

	br_scalar fog_min;
	br_scalar fog_max;
	br_colour fog_colour;

	br_int_32 subdivide_tolerance;

	br_scalar depth_bias;

	/*
	 * Workspace fields for user and database
	 */
	void	  *user;
	void	  *stored;

} br_material;

/*
 * Bits for br_material->flags
 */
enum {
	BR_MATF_LIGHT			= 0x00000001,
	BR_MATF_PRELIT			= 0x00000002,

	BR_MATF_SMOOTH			= 0x00000004,

	BR_MATF_ENVIRONMENT_I	= 0x00000008,
 	BR_MATF_ENVIRONMENT_L	= 0x00000010,
	BR_MATF_PERSPECTIVE		= 0x00000020,
	BR_MATF_DECAL			= 0x00000040,

	BR_MATF_I_FROM_U		= 0x00000080,
	BR_MATF_I_FROM_V		= 0x00000100,
	BR_MATF_U_FROM_I		= 0x00000200,
	BR_MATF_V_FROM_I		= 0x00000400,

	BR_MATF_ALWAYS_VISIBLE	= 0x00000800,
	BR_MATF_TWO_SIDED		= 0x00001000,

	BR_MATF_FORCE_FRONT		= 0x00002000,

	BR_MATF_DITHER			= 0x00004000,
#if 0
	BR_MATF_CUSTOM			= 0x00008000
#endif

	BR_MATF_MAP_ANTIALIASING	= 0x00010000,
	BR_MATF_MAP_INTERPOLATION 	= 0x00020000,
	BR_MATF_MIP_INTERPOLATION 	= 0x00040000,

	BR_MATF_FOG_LOCAL       = 0x00080000,
	BR_MATF_SUBDIVIDE       = 0x00100000,

	BR_MATF_QUAD_MAPPING	= 0x00400000,

	BR_MATF_FORCE_BACK		= 0x00800000,

	BR_MATF_INHIBIT_DEPTH_WRITE = 0x01000000,

	BR_MATF_MODULATE		= 0x02000000,	// No longer supported - MAY BE REUSED

	BR_MATF_PREALPHA		= 0x04000000,

    BR_MATF_SEPARATE_SPECULAR  = 0x08000000,

    BR_MATF_MODULATE_ALPHA  = 0x10000000,

    BR_MATF_DISABLE_COLOUR_KEY  = 0x20000000,

    BR_MATF_SMOOTH_ALPHA	= 0x40000000,
};

/*
 * Backwards compatibility
 */
#define BR_MATF_GOURAUD		BR_MATF_SMOOTH
#define BR_MATF_MAP_COLOUR	0
#define BR_MATF_FORCE_Z_0	BR_MATF_FORCE_FRONT
#define BR_MATF_BLEND		0


/*
 * Bits for br_material->mode
 */
enum {

	BR_MATM_DEPTH_TEST_MASK	= 0x0007,
	BR_MATM_DEPTH_TEST_GT	= 0x0000,
	BR_MATM_DEPTH_TEST_GE	= 0x0001,
	BR_MATM_DEPTH_TEST_EQ	= 0x0002,
	BR_MATM_DEPTH_TEST_NE	= 0x0003,
	BR_MATM_DEPTH_TEST_LE	= 0x0004,
	BR_MATM_DEPTH_TEST_LT	= 0x0005,
	BR_MATM_DEPTH_TEST_NV	= 0x0006,
	BR_MATM_DEPTH_TEST_AL	= 0x0007,

	BR_MATM_BLEND_MODE_MASK			= 0x0018,
	BR_MATM_BLEND_MODE_STANDARD		= 0x0000,
	BR_MATM_BLEND_MODE_SUMMED		= 0x0008,
	BR_MATM_BLEND_MODE_DIMMED		= 0x0010,
	BR_MATM_BLEND_MODE_PREMULTIPLIED= 0x0018,

	BR_MATM_MAP_WIDTH_LIMIT_MASK	= 0x0060,	
	BR_MATM_MAP_WIDTH_LIMIT_WRAP	= 0x0000,	
	BR_MATM_MAP_WIDTH_LIMIT_CLAMP	= 0x0020,	
	BR_MATM_MAP_WIDTH_LIMIT_MIRROR	= 0x0040,	

	BR_MATM_MAP_HEIGHT_LIMIT_MASK	= 0x0180,	
	BR_MATM_MAP_HEIGHT_LIMIT_WRAP	= 0x0000,	
	BR_MATM_MAP_HEIGHT_LIMIT_CLAMP	= 0x0080,	
	BR_MATM_MAP_HEIGHT_LIMIT_MIRROR	= 0x0100,	
};

/*
 * Flags to BrMaterialUpdate()
 */
enum {
	BR_MATU_MAP_TRANSFORM	= 0x0001,
	BR_MATU_RENDERING		= 0x0002,
	BR_MATU_LIGHTING		= 0x0004,
	BR_MATU_COLOURMAP		= 0x0008,
	BR_MATU_SCREENDOOR		= 0x0010,
	BR_MATU_EXTRA_SURF		= 0x0020,
	BR_MATU_EXTRA_PRIM		= 0x0040,
	BR_MATU_ALL				= 0x7fff
};

/*
 * Backwards compatibility
 */

/*
 * Flags to BrMapUpdate()
 */
enum {
	BR_MAPU_DATA			= 0x0001, // If this flag is ommitted the driver gets a chance to free the original (assuming KEEP_ORIGINAL not set) without doing any extra work.
	BR_MAPU_ALL				= 0x0fff,
	BR_MAPU_SHARED			= 0x8000,
};

/*
 * Flags to BrTableUpdate()
 */
enum {
	BR_TABU_ALL				= 0x7fff
};

#endif
