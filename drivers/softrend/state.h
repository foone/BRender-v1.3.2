/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: state.h 1.5 1998/11/13 16:23:42 jon Exp $
 * $Locker: $
 *
 * Renderer state info
 */
#ifndef _STATE_H_
#define _STATE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Some pre-defined limits
 */
#define MAX_STATE_STACK			32
#define MAX_STATE_LIGHTS		48
#define MAX_STATE_CLIP_PLANES	6

/*
 * Bits used to create a mask for each part of the state
 *
 * There are some global defines for these, so use them
 */
enum {
	MASK_STATE_CULL		= BR_STATE_CULL,
	MASK_STATE_SURFACE	= BR_STATE_SURFACE,
	MASK_STATE_PRIMITIVE= BR_STATE_PRIMITIVE,
	MASK_STATE_MATRIX	= BR_STATE_MATRIX,
	MASK_STATE_ENABLE	= BR_STATE_ENABLE,
	MASK_STATE_LIGHT	= BR_STATE_LIGHT,
	MASK_STATE_CLIP		= BR_STATE_CLIP,
	MASK_STATE_BOUNDS	= BR_STATE_BOUNDS,
	MASK_STATE_CACHE	= BR_STATE_CACHE,

	MASK_STATE_LOCAL	= (	MASK_STATE_CULL|
							MASK_STATE_SURFACE|
							MASK_STATE_MATRIX|
							MASK_STATE_ENABLE|
							MASK_STATE_LIGHT|
							MASK_STATE_CLIP|
							MASK_STATE_BOUNDS)
};

/*
 * Mask of those state parts which are reflected in the cache
 */
#define MASK_CACHED_STATES (MASK_STATE_CULL | MASK_STATE_SURFACE)


/*
 * state.enable.flags
 */
enum {
	ENBL_CULL_BIT,
	ENBL_TRANSFORM_BIT,
	ENBL_SURFACE_BIT,
	ENBL_CLIPPING_BIT,
	ENBL_BOUNDS_BIT,
	ENBL_RENDER_BIT,
};

enum {
	ENBL_CULL		= (1<<ENBL_CULL_BIT),
	ENBL_TRANSFORM	= (1<<ENBL_TRANSFORM_BIT),
	ENBL_SURFACE	= (1<<ENBL_SURFACE_BIT),
	ENBL_CLIPPING	= (1<<ENBL_CLIPPING_BIT),
	ENBL_BOUNDS		= (1<<ENBL_BOUNDS_BIT),
	ENBL_RENDER		= (1<<ENBL_RENDER_BIT)
};

/*
 * Culling information
 */
struct state_cull {
	br_token	type;
	br_token	space;

	br_timestamp	timestamp;
};

/*
 * Surface information
 */
struct state_surface {
	br_colour	colour;

	br_scalar	opacity;
	br_scalar	ka;
	br_scalar	kd;
	br_scalar	ks;
	br_scalar	power;

	br_boolean	lighting;
	br_boolean	prelighting;
	br_boolean	force_front;
	br_boolean	force_back;
	br_scalar	depth_bias;

	br_token	colour_source;
	br_token	opacity_source;
	br_token	mapping_source;
	
	br_matrix23	map_transform;

	br_timestamp	timestamp;
	br_timestamp	timestamp_transform;
};

/*
 * Light information
 */
struct state_light {
	br_token	type;
	br_token	lighting_space;

	br_vector3	position;
	br_vector3	direction;

	br_colour	colour;

	br_scalar	spot_outer;
	br_scalar	spot_inner;

	br_scalar	radius_outer;
	br_scalar	radius_inner;

	br_angle	angle_outer;

	br_scalar	attenuation_l;
	br_scalar	attenuation_c;
	br_scalar	attenuation_q;

	br_token	attenuation_type;
	br_token	attenuation_hint;

	br_boolean	radius_cull;
	br_boolean	angle_cull;

	br_light_volume volume;

	br_boolean	culled;

	br_timestamp	timestamp;
};

/*
 * Clipping information
 */
struct state_clip {
	br_token	type;
	br_vector4	plane;
};

/*
 * Matrix information
 */
struct state_matrix {
	br_matrix34	model_to_view;
	br_matrix4	view_to_screen;
	br_matrix34	view_to_environment;

	br_token	model_to_view_hint;
	br_token	view_to_screen_hint;
	br_token	view_to_environment_hint;

	br_scalar	hither_z;
	br_scalar	yon_z;
	
	br_timestamp	timestamp;
	br_timestamp	timestamp_v2s_hint;
	br_timestamp	timestamp_m2v;
	br_timestamp	timestamp_v2s;
	br_timestamp	timestamp_v2e;
};

/*
 * Bounds information
 */
struct state_bounds {
	br_vector2	min;
	br_vector2	max;

	br_timestamp	timestamp;
};

/*
 * Enables
 */
struct state_enable {
	br_uint_32	flags;

	br_timestamp	timestamp;
};

/*
 * Hidden surface info.
 */
typedef void BR_CALLBACK insert_cbfn(
	br_primitive *primitive,
	void *arg1, void *arg2, void *arg3,
	br_order_table *order_table, br_scalar *z);

struct state_hidden {
	br_token			type;
	br_token			divert;
	br_order_table		*order_table;
	br_primitive_heap	*heap;
	insert_cbfn			*insert_fn;
	void				*insert_arg1;
	void				*insert_arg2;
	void				*insert_arg3;

	br_timestamp		timestamp;
};

/*
 * Cache of info that is copies with STATE_CACHE
 */
#define GEOMETRY_CALL BR_ASM_CALL
#define SURFACE_CALL BR_ASM_CALL

typedef void GEOMETRY_CALL geometry_fn(struct br_geometry *self, struct br_renderer *renderer);
typedef void SURFACE_CALL surface_fn(struct br_renderer *self,
	br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour colour, br_scalar *comp);

#define MAX_GEOMETRY_FNS 16
#define MAX_SURFACE_FNS 8
#define MAX_FACE_BLOCKS 6

struct state_cache {
	/*
	 * mask of components that need clipping
	 */
	br_uint_32 clip_slots;

	/*
	 * Component offsets and scales
	 */
	br_scalar comp_offsets[NUM_COMPONENTS];
	br_scalar comp_scales[NUM_COMPONENTS];

	/*
	 * Cached geometry & surface functions
	 */
	br_int_32 ngeometry_fns;
	geometry_fn *geometry_fns[MAX_GEOMETRY_FNS];

	br_int_32 ngeometry_fns_onscreen;
	geometry_fn *geometry_fns_onscreen[MAX_GEOMETRY_FNS];

	/*
	 * Minimal primitive blocks for extra per-face operations
	 */
	brp_block_min face_blocks[MAX_FACE_BLOCKS];
	brp_block_min face_blocks_onscreen[MAX_FACE_BLOCKS];
	br_int_8 nface_blocks;
	br_int_8 nface_blocks_onscreen;

	struct br_geometry *format;

	/*
	 * Cached surface functions
	 */
	br_int_32 nvertex_fns;
	surface_fn *vertex_fns[MAX_SURFACE_FNS];

	br_int_32 nconstant_fns;
	surface_fn *constant_fns[MAX_SURFACE_FNS];

	/*
	 * Full map_transform (including U,V ranges)
	 */
	br_matrix23	map_transform;

	/*
	 * Amounts to shift U & V, if map_transform is simple
	 */
	br_int_8 u_shift;
	br_int_8 v_shift;

	/*
	 * Transformed mapping coordinates for corners of unit quad
	 */
	br_vector2 quad_transformed[4];

	/*
	 * Flags indicating whether sharing of diverted vertices is valid for this pipeline
	 * 
	 */
	br_boolean share_vertex_0;
	br_boolean share_other_vertices;

	/*
	 * Flag indicating that primitives are to be diverted
	 */
	br_boolean divert;

	/*
	 * Flag for whether cache is valid
	 */
	br_boolean valid;
};

/*
 * Wrapper with all state
 */
typedef struct state_all {
	/*
	 * Pointer to renderer that this state is asociated with
	 */
	struct br_renderer *renderer;

	/*
	 * mask of valid parts
	 */
	br_uint_32 valid;

	struct state_cull cull;
	struct state_surface surface;

	struct state_light light[MAX_STATE_LIGHTS];
	br_timestamp timestamp_lights;

	struct state_clip clip[MAX_STATE_CLIP_PLANES];
	br_timestamp timestamp_clips;

	struct state_matrix matrix;
	struct state_bounds bounds;
	struct state_enable enable;
	struct state_hidden hidden;
	struct state_cache cache;

	br_timestamp timestamp_copy_cache;
	br_timestamp timestamp_cache;

	/*
	 * Pointer to primitive specific state
	 */
	struct br_primitive_state *pstate;

} state_all;

#ifdef __cplusplus
};
#endif
#endif

