/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: rend.h 1.7 1998/07/20 14:45:24 jon Exp $
 * $Locker: $
 *
 * Static scratch area for renderer
 */
#ifndef _REND_H_
#define _REND_H_

#ifdef __cplusplus
extern "C" {
#endif

#if 1
struct fmt_vertex {
	br_vector3 p;			/* Point in model space				*/
	br_vector2 map;			/* Mapping coordinates				*/
	br_vector3 n;			/* Surface normal at vertex			*/
};
#endif

/*
 * Flags for vertices
 */
#define TVDIR_FRONT	0x80000000
#define TVDIR_BACK	0x40000000

#define TV_CLIPPED	0x20000000

struct temp_face {

	/*
	 * Combined outcodes of face
	 */
	br_uint_16 codes;

	/*
	 * Flag describing visiblity of face
	 */
	br_uint_8 flag;
	br_uint_8 _pad[1];
};

/*
 * Values for temp_face.flag
 */
#define TFF_VISIBLE			4
#define TFF_CLIPPED			2
#define TFF_REVERSED		1

/*
 * Workspace for renderer functions
 */
typedef struct rend_block {
	/*
	 * Pointers into scratch buffer
	 */
	void *scratch;

	struct temp_face *temp_faces;
	br_int_8 *vertex_counts;
	union brp_vertex *temp_vertices;
	union brp_vertex **vertex_heap_pointers;

	br_int_8 *vertex_flags;
	br_int_8 *edge_flags;

//        br_int_32 nvisible_faces;
	br_boolean faces_clipped;

#if 1
	br_uint_16 (*face_vertices)[3];
	br_uint_16 (*face_edges)[3];
	br_vector4 *face_eqn;
#else
	struct v11face *faces;
#endif

#if 1
	br_vector3 *vertex_p;
	br_vector2 *vertex_map;
	br_vector3 *vertex_n;
#else
	struct fmt_vertex *vertices;
#endif

    br_colour *face_colours;
    br_colour *vertex_colours;

	br_uint_8 *face_flags;

	br_int_32 nfaces;
	br_int_32 nvertices;
	br_int_32 nedges;

	/*
	 * Index of the current vertex and face during surface and face op loops
	 */
	br_uint_32 current_index;

	/*
	 * Prelighting colour table currently in use
	 */
	br_colour *prelit_colours;

	br_vector3 eye_l;

	struct brp_block *block;

	br_boolean block_changed;
	br_boolean range_changed;

	/*
	 * Current renderer
	 */
	struct br_renderer *renderer;
	struct br_geometry *geometry;

	/*
	 * Current v11model
	 */
	struct v11model *v11model;

	/*
	 * Current subdivion limit
	 */
	br_scalar subdivide_threshold;
} _rend;

/*
 * Cached info that is based on current state
 */
struct active_light {
	br_boolean culled;
	br_token type;
	br_vector3	position;
	br_vector3	direction;
	br_vector3	half;
	br_scalar	intensity;
	br_scalar	falloff;
	br_scalar	cutoff;
	br_scalar	spot_falloff;
	br_scalar	red;
	br_scalar	green;
	br_scalar	blue;
	br_light_volume volume;
	void (*accumulate_index)(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp, br_scalar *comp);
	void (*accumulate_colour)(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
	void (*accumulate_colour_specular)(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
	struct state_light *s;
};

typedef struct static_cache {

	/*
	 * Resource anchor for any resources in static cache
	 */
	void *res;

	br_matrix4 model_to_screen;
	br_matrix34 view_to_model;
	br_matrix34 model_to_environment;

	/*
	 * model_to_screen with viewport transform multiplied in
	 */
	br_matrix4 model_to_viewport;

#if BASED_FIXED
	/*
	 * High words of bottom row of matrix
	 */
	br_scalar model_to_viewport_hi[4];
#endif

	/*
	 * Combined surface colour & opacity
	 */
	br_colour colour;

#if BASED_FIXED
	br_uint_32	loop_count;

	/*
	 * Local copies of X,Y,Z scale and offset for ASM transform & project
	 */
	br_scalar	scale_x,scale_y,scale_z;
	br_scalar	offset_x,offset_y,offset_z;
#endif
	/*
	 * Local copy of bounds
	 */
	br_vector2	min;
	br_vector2	max;

	br_vector4 eye_m;
	br_vector3 eye_m_normalised;

	br_boolean	user_clip_active;

	br_boolean valid_primitive_heap;

	/*
	 * Current lighting info.
	 */
	br_boolean light_1md;
	struct active_light lights[MAX_STATE_LIGHTS];
	br_int_32 nlights_model;
	br_int_32 nlights_view;

	br_boolean use_ambient_colour;
	br_scalar ambient_red;
	br_scalar ambient_green;
	br_scalar ambient_blue;

	br_boolean use_ambient_intensity;
	br_scalar ambient_intensity;

	/*
	 * Flags indicating what needs to be updated
	 */
	br_boolean	valid_v2m;
	br_boolean	valid_m2s;
	br_boolean	valid_per_model;
	br_boolean	valid_per_scene;

} _static_cache;

extern _rend BR_ASM_DATA rend;
extern _static_cache BR_ASM_DATA scache;

/*
 * Generates outcodes for a homogenous point
 */
#define OUTCODE_POINT(outcode,screen)\
{																				\
	/*																			\
	 * The 6 planes of the view volume...										\
	 */																			\
	if((screen)->v[X] >= (screen)->v[W])										\
		(outcode) ^= (OUTCODE_RIGHT | OUTCODE_N_RIGHT);							\
	if((screen)->v[X] < -(screen)->v[W])										\
		(outcode) ^= (OUTCODE_LEFT | OUTCODE_N_LEFT);							\
																				\
	if((screen)->v[Y] >=  (screen)->v[W])										\
		(outcode) ^= (OUTCODE_TOP | OUTCODE_N_TOP);								\
	if((screen)->v[Y] < -(screen)->v[W])										\
		(outcode) ^= (OUTCODE_BOTTOM | OUTCODE_N_BOTTOM);						\
																				\
	if((screen)->v[Z] >= (screen)->v[W])										\
		(outcode) ^= (OUTCODE_HITHER | OUTCODE_N_HITHER);						\
	if((screen)->v[Z] < -(screen)->v[W])										\
		(outcode) ^= (OUTCODE_YON | OUTCODE_N_YON);								\
																				\
	/*																			\
	 * Any user defined clip planes...											\
	 */																			\
	if(scache.user_clip_active) {												\
		int c;																	\
		for(c = 0; c < MAX_STATE_CLIP_PLANES; c++)								\
			if(renderer->state.clip[c].type == BRT_PLANE &&						\
				BrVector4Dot((screen),&renderer->state.clip[c].plane) < S0)		\
				(outcode) ^= (OUTCODE_USER | OUTCODE_N_USER) << c;				\
	}																			\
}


/*
 * Generic macro for projecting a vertex from homogenous coordinates
 */
#if BASED_FLOAT
#define PROJECT_VERTEX(tvp,sx,sy,sz,sw)										\
	{																		\
		br_scalar q = BR_DIV(S1,(sw));											\
		(tvp)->comp[C_SX] = renderer->state.cache.comp_offsets[C_SX] +		\
			BR_MUL(renderer->state.cache.comp_scales[C_SX],					\
				BR_MUL((sx),q));												\
		(tvp)->comp[C_SY] = renderer->state.cache.comp_offsets[C_SY] +		\
			BR_MUL(renderer->state.cache.comp_scales[C_SY],					\
				BR_MUL((sy),q));												\
		(tvp)->comp[C_SZ] = renderer->state.cache.comp_offsets[C_SZ] +		\
			BR_MUL(renderer->state.cache.comp_scales[C_SZ],					\
				BR_MUL((sz),q));												\
	}

#define PROJECT_VERTEX_WRITE_Q(tvp,sx,sy,sz,sw)								\
	{																		\
		(tvp)->comp[C_Q] = BR_DIV(S1,(sw));									\
		(tvp)->comp[C_SX] = renderer->state.cache.comp_offsets[C_SX] +		\
			BR_MUL(renderer->state.cache.comp_scales[C_SX],					\
				BR_MUL((sx),(tvp)->comp[C_Q]));								\
		(tvp)->comp[C_SY] = renderer->state.cache.comp_offsets[C_SY] +		\
			BR_MUL(renderer->state.cache.comp_scales[C_SY],					\
				BR_MUL((sy),(tvp)->comp[C_Q]));								\
		(tvp)->comp[C_SZ] = renderer->state.cache.comp_offsets[C_SZ] +		\
			BR_MUL(renderer->state.cache.comp_scales[C_SZ],					\
				BR_MUL((sz),(tvp)->comp[C_Q]));								\
	}
#endif

/*
 * Fixed point versions do not multiply by Q because of possible accuracy problems
 * N.B. Writing out a fixed point Q is not very accurate so I write out a float
 * and convert to a fixed if necessary (unlikely)
 */
#if BASED_FIXED
#define PROJECT_VERTEX(tvp,sx,sy,sz,sw)										\
	{																		\
		(tvp)->comp[C_SX] = renderer->state.cache.comp_offsets[C_SX] +		\
			BR_MULDIV(renderer->state.cache.comp_scales[C_SX],(sx),(sw));	\
		(tvp)->comp[C_SY] = renderer->state.cache.comp_offsets[C_SY] +		\
			BR_MULDIV(renderer->state.cache.comp_scales[C_SY],(sy),(sw));	\
		(tvp)->comp[C_SZ] = renderer->state.cache.comp_offsets[C_SZ] +		\
			BR_MULDIV(renderer->state.cache.comp_scales[C_SZ],(sz),(sw));	\
	}

#define PROJECT_VERTEX_WRITE_Q(tvp,sx,sy,sz,sw)								\
	{																		\
		(tvp)->comp_f[C_Q] = 1.0f / BrScalarToFloat(sw);						\
		(tvp)->comp[C_SX] = renderer->state.cache.comp_offsets[C_SX] +		\
			BR_MUL(renderer->state.cache.comp_scales[C_SX],					\
				BrFloatToScalar(BrScalarToFloat(sx) * (tvp)->comp_f[C_Q]));	\
		(tvp)->comp[C_SY] = renderer->state.cache.comp_offsets[C_SY] +		\
			BR_MUL(renderer->state.cache.comp_scales[C_SY],					\
				BrFloatToScalar(BrScalarToFloat(sy) * (tvp)->comp_f[C_Q]));	\
		(tvp)->comp[C_SZ] = renderer->state.cache.comp_offsets[C_SZ] +		\
			BR_MUL(renderer->state.cache.comp_scales[C_SZ],					\
				BrFloatToScalar(BrScalarToFloat(sz) * (tvp)->comp_f[C_Q]));	\
	}
#endif

/*
 * Transform into screen space - Inline expanded BrMatrix4ApplyP()
 */
#define TRANSFORM_VERTEX(dest,src,mat) {\
	(dest)->v[X] = BR_MAC3(						\
			(src)->v[X],(mat)->m[0][0],			\
			(src)->v[Y],(mat)->m[1][0],			\
			(src)->v[Z],(mat)->m[2][0]) +		\
					    (mat)->m[3][0];			\
												\
	(dest)->v[Y] = BR_MAC3(						\
			(src)->v[X],(mat)->m[0][1],			\
			(src)->v[Y],(mat)->m[1][1],			\
			(src)->v[Z],(mat)->m[2][1]) +		\
					    (mat)->m[3][1];			\
												\
	(dest)->v[Z] = BR_MAC3(						\
			(src)->v[X],(mat)->m[0][2],			\
			(src)->v[Y],(mat)->m[1][2],			\
			(src)->v[Z],(mat)->m[2][2]) +		\
					    (mat)->m[3][2];			\
												\
	(dest)->v[W] = BR_MAC3(						\
			(src)->v[X],(mat)->m[0][3],			\
			(src)->v[Y],(mat)->m[1][3],			\
			(src)->v[Z],(mat)->m[2][3]) +		\
					    (mat)->m[3][3];			\
}

/*
 * Alignment for block in scratch area
 */
#define SCRATCH_BOUNDARY 16
#define SCRATCH_ALIGN(x) (((x)+(SCRATCH_BOUNDARY-1)) & ~(SCRATCH_BOUNDARY-1))

/*
 * Update MIN and MAX vertices' x,y
 */
#define UPDATE_BOUNDS(tv) { \
		if((tv)->comp[C_SX] > scache.max.v[X]) scache.max.v[X] = (tv)->comp[C_SX]; \
		if((tv)->comp[C_SX] < scache.min.v[X]) scache.min.v[X] = (tv)->comp[C_SX]; \
		if((tv)->comp[C_SY] > scache.max.v[Y]) scache.max.v[Y] = (tv)->comp[C_SY]; \
		if((tv)->comp[C_SY] < scache.min.v[Y]) scache.min.v[Y] = (tv)->comp[C_SY]; \
	}

#if BASED_FIXED
#define PrimitiveStateRangesQuery PrimitiveStateRangesQueryX
#endif

#if BASED_FLOAT
#define PrimitiveStateRangesQuery PrimitiveStateRangesQueryF
#endif

/*
 * The level below which the fixed specular power function is zero
 */
#define SPECULARPOW_CUTOFF BR_SCALAR(0.6172)

#ifdef __cplusplus
};
#endif
#endif

