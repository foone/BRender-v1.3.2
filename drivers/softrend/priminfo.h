/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: priminfo.h 1.1 1997/12/10 16:52:25 jon Exp $
 * $Locker: $
 *
 * Public interface to primitive renderers from sofware pipeline
 */
#ifndef _PRIMINFO_H_
#define _PRIMINFO_H_

#include "brender.h"

/**
 ** Primitive Vertex
 **/

/*
 * Per vertex components - some components share slots (eg, I and A)
 */
enum brp_components {
	C_FLAGS	= 0,
	C_X		= 1,
	C_Y		= 2,
	C_Z		= 3,
	C_W		= 4,

	C_SX	= 5,
	C_SY	= 6,
	C_SZ	= 7,

	C_U		= 8,
	C_V		= 9,

	C_I 	= 10,	
	C_A     = 10,

	C_R		= 11,
	C_G		= 12,
	C_B		= 13,

	C_UI 	= 10,	/* Unlit versions of RGB and I */
	C_UR	= 11,
	C_UG	= 12,
	C_UB	= 13,

	/*
	 * Following merged colour slots are only really meaningful as integers
	 *
	 * XXX Unimplemented
	 */
	C_ARGB	= 11,	/* Merged colour and alpha */

	C_URGB	= 11,	/* Merged unlit colour and alpha */
	C_DRGB	= 12,	/* Merged diffuse colour and alpha */
	C_SRGB  = 13,	/* Merged specular colour and alpha */

	C_S		= 14,
	C_SW	= 14,

	C_Q		= 15,	/* Reciprocal of C_W */

NUM_COMPONENTS = 16
};

/*
 * Mask bits for selecting which components take part in various operations
 *
 * NB: The bit number in the mask is not that same as the component entry
 * (some components share entries)
 */
enum brp_component_masks {
	CM_X    = 0x00000001,
	CM_Y    = 0x00000002,
	CM_Z    = 0x00000004,
	CM_W    = 0x00000008,

	CM_SX   = 0x00000010,
	CM_SY   = 0x00000020,
	CM_SZ   = 0x00000040,
	CM_U    = 0x00000080,
	CM_V    = 0x00000100,
	CM_I    = 0x00000200,
	CM_A    = 0x00000400,
	CM_R    = 0x00000800,
	CM_G    = 0x00001000,
	CM_B    = 0x00002000,

	CM_UI   = 0x00004000,
	CM_UR   = 0x00008000,
	CM_UG   = 0x00010000,
	CM_UB   = 0x00020000,

	CM_ARGB = 0x00040000,

	CM_URGB = 0x00080000,
	CM_DRGB = 0x00100000,
	CM_SRGB = 0x00200000,

	CM_SW   = 0x00400000,

	CM_Q    = 0x00800000,
};

/*
 * Given an component mask, maps (CM_SX,CM_SY,CM_SZ) to (CM_X,CM_Y,CM_Z)
 */
#define COMP_S2V(c) (((c) & ~(CM_SX | CM_SY | CM_SZ)) |	(((c) &  (CM_SX | CM_SY | CM_SZ)) >> 4))

/*
 * Flags that tell the renderer any special actions that need to be taken for this primitive
 */
enum brp_flags {
	BR_PRIMF_SCISSOR 		 = 0x00000001,	/* Primitive will be clipped to output buffer(s) by rasteriser				*/
	BR_PRIMF_CONST_DUPLICATE = 0x00000002,	/* Constant components will be duplicated to all vertices					*/
    BR_PRIMF_SUBDIVIDE       = 0x00000004,  /* Subdivide triangles to reduce perspective errors               			*/
	BR_PRIMF_BLENDED		 = 0x00000008,  /* This primitive is blended into existing output, may need to be defered	*/
};

typedef union brp_vertex {
	br_int_32 flags;
	br_scalar	comp  [NUM_COMPONENTS];		/* All the components of vertex	*/
	br_float	comp_f[NUM_COMPONENTS];	
	br_fixed_ls	comp_x[NUM_COMPONENTS];
	br_int_32	comp_i[NUM_COMPONENTS];
} brp_vertex;

/**
 ** Primitive renderer description block
 **/

/*
 * Render a primitive - takes 1 or more vertices
 */
#ifndef __H2INC__
typedef void BR_ASM_CALL brp_render_fn(struct brp_block *block, ...);
#else
typedef void BR_ASM_CALL brp_render_fn(struct brp_block *block);
#endif

typedef struct brp_block {
	/*
	 * Pointer to rendering function (single)
	 */
	brp_render_fn * render;

	/*
	 * Link for chaining blocks together
	 */
	struct brp_block *chain;

	/*
	 * String giving a readable identification of this primitive
	 */
	char *identifier;

	/*
	 * Empty space - was used as a pointer to primitive library
	 */
    void *_reserved0;

	/*
	 * Type of primtive
	 */
	br_token type;

	br_uint_32 flags;

	/*
	 * Bitmasks of what vertex info is needed for rendering
	 */
	br_uint_32	constant_components;		/* Only need to be set in v0 */
	br_uint_32	vertex_components;			/* Must be set per-vertex	*/

	/*
	 * Masks of slots that are needed as float
	 *
	 * NB: This is a bit per slot, not a bit per component,
	 *     initialise with (1<<C_X) rather than CM_X
	 */
	br_uint_32	convert_mask_f;

	/*
	 * Masks of slots that are needed as fixed
	 */
	br_uint_32	convert_mask_x;

	/*
	 * Masks of slots that are needed as integers
	 */
	br_uint_32	convert_mask_i;

	/*
	 * Mask of constant slots
	 */
	br_uint_32	constant_mask;

	/*
	 * A magic number for the subvidider
	 * The default 0
	 * +ve numbers make it more tolerant of errors
	 * -ve numbers make it less tolerant
	 *
	 * The useable range should be ~ +100 -> -100
	 */
	br_int_32	subdivide_tolerance;

	br_uint_32	_reserved_0;
	br_uint_32	_reserved_1;
	br_uint_32	_reserved_2;

} brp_block;

/*
 * A cut down version of the above block that is used to chain
 * operations together in the renderer
 */
typedef struct brp_block_min {
	/*
	 * Pointer to rendering function (single)
	 */
	brp_render_fn * render;

	/*
	 * Link for chaining blocks together
	 */
	struct brp_block *chain;

} brp_block_min;


#ifdef __cplusplus
};
#endif
#endif

