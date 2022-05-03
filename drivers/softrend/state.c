/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: state.c 1.6 1998/11/13 16:23:39 jon Exp $
 * $Locker: $
 *
 * Default state and access templates
 */
#include <stddef.h>
#include <string.h>

#include "drv.h"
#include "brassert.h"

BR_RCS_ID("$Id: state.c 1.6 1998/11/13 16:23:39 jon Exp $");

/*
 * Shortcuts for template flags
 */
#define S BRTV_SET
#define Q BRTV_QUERY
#define A BRTV_ALL

#if BASED_FIXED
#define AX BRTV_ALL
#else 
#define AX 0
#endif

#if BASED_FLOAT
#define AF BRTV_ALL
#else 
#define AF 0
#endif

/*
 * Bits used for template masks to indicate actions when a template entry is triggered
 */
#define TM_PART				0x00001		/* set timestamp for whole part								*/
#define TM_INDEX			0x00002		/* set timestamp for indexed part							*/
#define TM_V2S_HINT			0x00004		/* set timestamp for matrix.view_to_screen_hint				*/
#define TM_M2V				0x00008		/* set timestamp for matrix.model_to_view					*/
#define TM_V2S				0x00010		/* set timestamp for matrix.view_to_screen					*/
#define TM_V2E				0x00020		/* set timestamp for matrix.view_to_environment				*/
#define TM_COPY_CACHE		0x00040		/* set timestamp for anything that affects the copied cache	*/
#define TM_CACHE			0x00080		/* set timestamp for anything that affects the static cache	*/

#define TM_CLEAR_M2V_HINT	0x00100		/* set M2V_HINT to NONE										*/
#define TM_CLEAR_V2S_HINT	0x00200		/* set V2S_HINT to NONE										*/


#define TM_INVALID_PS		0x01000		/* set Update Per Scene										*/
#define TM_INVALID_PM		0x02000		/* set Update Per Model										*/
#define TM_INVALID_V2M		0x04000		/* set Update View To Model									*/
#define TM_INVALID_M2S		0x08000		/* set Update Model To Screen								*/
#define TM_INVALID_CC		0x10000		/* set Update Copied Cache									*/

/**************************************************************************
 ** Culling
 **/

#define F(f) offsetof(struct state_all,f)
#define P(f)	((br_int_32)(&(f)))

static const br_tv_template_entry partCullTemplateEntries[] = {
	{BRT(TYPE_T),			F(cull.type),			Q | S | A,	BRTV_CONV_COPY,				0,TM_PART|TM_INVALID_CC},
	{BRT(SPACE_T),			F(cull.space),			Q | S | A,	BRTV_CONV_COPY,				0,TM_PART|TM_INVALID_CC},
};

/**************************************************************************
 ** Surface
 **/

static const br_tv_template_entry partSurfaceTemplateEntries[] = {
	{BRT(COLOUR_RGB),		F(surface.colour),			Q|S|AX,	BRTV_CONV_COPY,				0,TM_PART},

	{BRT(OPACITY_X),		F(surface.opacity),			Q|S|AX,	BRTV_CONV_FIXED_SCALAR,		0,TM_PART},
	{BRT(OPACITY_F),		F(surface.opacity),			Q|S|AF,	BRTV_CONV_FLOAT_SCALAR,		0,TM_PART},
	{BRT(AMBIENT_X),		F(surface.ka),				Q|S|AX,	BRTV_CONV_FIXED_SCALAR,		0,TM_PART},
	{BRT(AMBIENT_F),		F(surface.ka),				Q|S|AF,	BRTV_CONV_FLOAT_SCALAR,		0,TM_PART},
	{BRT(DIFFUSE_X),		F(surface.kd),				Q|S|AX,	BRTV_CONV_FIXED_SCALAR,		0,TM_PART},
	{BRT(DIFFUSE_F),		F(surface.kd),				Q|S|AF,	BRTV_CONV_FLOAT_SCALAR,		0,TM_PART},
	{BRT(SPECULAR_X),		F(surface.ks),				Q|S|AX,	BRTV_CONV_FIXED_SCALAR,		0,TM_PART},
	{BRT(SPECULAR_F),		F(surface.ks),				Q|S|AF,	BRTV_CONV_FLOAT_SCALAR,		0,TM_PART},
	{BRT(SPECULAR_POWER_X),	F(surface.power),			Q|S|AX,	BRTV_CONV_FIXED_SCALAR,		0,TM_PART},
	{BRT(SPECULAR_POWER_F),	F(surface.power),			Q|S|AF,	BRTV_CONV_FLOAT_SCALAR,		0,TM_PART},

	{BRT(LIGHTING_B),		F(surface.lighting),		Q|S|A,	BRTV_CONV_COPY,				0,TM_PART|TM_INVALID_CC},
	{BRT(PRELIGHTING_B),	F(surface.prelighting),		Q|S|A,	BRTV_CONV_COPY,				0,TM_PART|TM_INVALID_CC},
	{BRT(FORCE_FRONT_B),	F(surface.force_front),		Q|S|A,	BRTV_CONV_COPY,				0,TM_PART|TM_INVALID_CC},
	{BRT(FORCE_BACK_B),		F(surface.force_back),		Q|S|A,  BRTV_CONV_COPY,				0,TM_PART|TM_INVALID_CC},
	{BRT(DEPTH_BIAS_X),		F(surface.depth_bias),		Q|S|A,  BRTV_CONV_FIXED_SCALAR,		0,TM_PART|TM_INVALID_CC},
	{BRT(DEPTH_BIAS_F),		F(surface.depth_bias),		Q|S|A,  BRTV_CONV_FLOAT_SCALAR,		0,TM_PART|TM_INVALID_CC},

	{BRT(COLOUR_SOURCE_T),	F(surface.colour_source),	Q|S|A,	BRTV_CONV_COPY,				0,TM_PART|TM_INVALID_CC},
	{BRT(OPACITY_SOURCE_T),	F(surface.opacity_source),	Q|S|A,	BRTV_CONV_COPY,				0,TM_PART|TM_INVALID_CC},
	{BRT(MAPPING_SOURCE_T),	F(surface.mapping_source),	Q|S|A,	BRTV_CONV_COPY,				0,TM_PART|TM_INVALID_CC},

	{BRT(MAP_MATRIX_M23_X),	F(surface.map_transform),	Q|S|AX,	BRTV_CONV_M23_FIXED_SCALAR,	0,TM_PART|TM_INVALID_CC},
	{BRT(MAP_MATRIX_M23_F),	F(surface.map_transform),	Q|S|AF,	BRTV_CONV_M23_FLOAT_SCALAR,	0,TM_PART|TM_INVALID_CC},
};

/**************************************************************************
 ** Light
 **/
static br_error BR_CALLBACK customLightingVolumeSet(void *block, br_uint_32 *pvalue, struct br_tv_template_entry *tep);

static const struct br_tv_custom customLightingVolumeConv = {
	NULL,
	customLightingVolumeSet,
	NULL,
};


static const br_tv_template_entry partLightTemplateEntries[] = {
	{BRT(TYPE_T),			F(light[0].type),			Q|S|A,	BRTV_CONV_COPY,				0,TM_PART | TM_INDEX|TM_INVALID_PS|TM_INVALID_PM},
	{BRT(SPACE_T),			F(light[0].lighting_space),	Q|S|A,	BRTV_CONV_COPY,				0,TM_PART | TM_INDEX|TM_INVALID_PS|TM_INVALID_PM},
	{BRT(POSITION_V3_X),	F(light[0].position),		Q|S|AX,	BRTV_CONV_V3_FIXED_SCALAR,	0,TM_PART | TM_INDEX|TM_INVALID_PS|TM_INVALID_PM},
	{BRT(POSITION_V3_F),	F(light[0].position),		Q|S|AF,	BRTV_CONV_V3_FLOAT_SCALAR,	0,TM_PART | TM_INDEX|TM_INVALID_PS|TM_INVALID_PM},
	{BRT(DIRECTION_V3_X),	F(light[0].direction),		Q|S|AX,	BRTV_CONV_V3_FIXED_SCALAR,	0,TM_PART | TM_INDEX|TM_INVALID_PS|TM_INVALID_PM},
	{BRT(DIRECTION_V3_F),	F(light[0].direction),		Q|S|AF,	BRTV_CONV_V3_FLOAT_SCALAR,	0,TM_PART | TM_INDEX|TM_INVALID_PS|TM_INVALID_PM},
	{BRT(SPOT_INNER_X),		F(light[0].spot_inner),		Q|S|AX,	BRTV_CONV_FIXED_SCALAR,		0,TM_PART | TM_INDEX|TM_INVALID_PS|TM_INVALID_PM},
	{BRT(SPOT_INNER_F),		F(light[0].spot_inner),		Q|S|AF,	BRTV_CONV_FLOAT_SCALAR,		0,TM_PART | TM_INDEX|TM_INVALID_PS|TM_INVALID_PM},
	{BRT(SPOT_OUTER_X),		F(light[0].spot_outer),		Q|S|AX,	BRTV_CONV_FIXED_SCALAR,		0,TM_PART | TM_INDEX|TM_INVALID_PS|TM_INVALID_PM},
	{BRT(SPOT_OUTER_F),		F(light[0].spot_outer),		Q|S|AF,	BRTV_CONV_FLOAT_SCALAR,		0,TM_PART | TM_INDEX|TM_INVALID_PS|TM_INVALID_PM},
	{BRT(ANGLE_OUTER_A),	F(light[0].angle_outer),	Q|S|AF,	BRTV_CONV_COPY,				0,TM_PART | TM_INDEX|TM_INVALID_PS|TM_INVALID_PM},
	{BRT(RADIUS_INNER_X),	F(light[0].radius_inner),	Q|S|AX,	BRTV_CONV_FIXED_SCALAR,		0,TM_PART | TM_INDEX|TM_INVALID_PS|TM_INVALID_PM},
	{BRT(RADIUS_INNER_F),	F(light[0].radius_inner),	Q|S|AF,	BRTV_CONV_FLOAT_SCALAR,		0,TM_PART | TM_INDEX|TM_INVALID_PS|TM_INVALID_PM},
	{BRT(RADIUS_OUTER_X),	F(light[0].radius_outer),	Q|S|AX,	BRTV_CONV_FIXED_SCALAR,		0,TM_PART | TM_INDEX|TM_INVALID_PS|TM_INVALID_PM},
	{BRT(RADIUS_OUTER_F),	F(light[0].radius_outer),	Q|S|AF,	BRTV_CONV_FLOAT_SCALAR,		0,TM_PART | TM_INDEX|TM_INVALID_PS|TM_INVALID_PM},

	{BRT(COLOUR_RGB),		F(light[0].colour),			Q|S|AF,	BRTV_CONV_COPY,				0,TM_PART | TM_INDEX|TM_INVALID_PS|TM_INVALID_PM},

	{BRT(ATTENUATION_C_F),	F(light[0].attenuation_c),	Q|S|AF,	BRTV_CONV_FLOAT_SCALAR,		0,TM_PART | TM_INDEX|TM_INVALID_PS|TM_INVALID_PM},
	{BRT(ATTENUATION_C_X),	F(light[0].attenuation_c),	Q|S|AF,	BRTV_CONV_FIXED_SCALAR,		0,TM_PART | TM_INDEX|TM_INVALID_PS|TM_INVALID_PM},
	{BRT(ATTENUATION_L_F),	F(light[0].attenuation_l),	Q|S|AF,	BRTV_CONV_FLOAT_SCALAR,		0,TM_PART | TM_INDEX|TM_INVALID_PS|TM_INVALID_PM},
	{BRT(ATTENUATION_L_X),	F(light[0].attenuation_l),	Q|S|AF,	BRTV_CONV_FIXED_SCALAR,		0,TM_PART | TM_INDEX|TM_INVALID_PS|TM_INVALID_PM},
	{BRT(ATTENUATION_Q_F),	F(light[0].attenuation_q),	Q|S|AF,	BRTV_CONV_FLOAT_SCALAR,		0,TM_PART | TM_INDEX|TM_INVALID_PS|TM_INVALID_PM},
	{BRT(ATTENUATION_Q_X),	F(light[0].attenuation_q),	Q|S|AF,	BRTV_CONV_FIXED_SCALAR,		0,TM_PART | TM_INDEX|TM_INVALID_PS|TM_INVALID_PM},

	{BRT(ATTENUATION_TYPE_T),F(light[0].attenuation_type),Q|S|AF,	BRTV_CONV_COPY,			0,TM_PART | TM_INDEX|TM_INVALID_PS|TM_INVALID_PM},
	{BRT(ATTENUATION_HINT_T),F(light[0].attenuation_hint),Q|S|AF,	BRTV_CONV_COPY,			0,TM_PART | TM_INDEX|TM_INVALID_PS|TM_INVALID_PM},

	{BRT(RADIUS_CULL_B),	F(light[0].radius_cull),	Q|S|AF,	BRTV_CONV_COPY,				0,TM_PART | TM_INDEX|TM_INVALID_PS|TM_INVALID_PM},
	{BRT(ANGLE_CULL_B),		F(light[0].angle_cull),		Q|S|AF,	BRTV_CONV_COPY,				0,TM_PART | TM_INDEX|TM_INVALID_PS|TM_INVALID_PM},

	{BRT(LIGHTING_VOLUME_P),F(light[0].volume),			S,		BRTV_CONV_CUSTOM,			P(customLightingVolumeConv),TM_PART | TM_INDEX|TM_INVALID_PS|TM_INVALID_PM},

	{BRT(CULLED_B),			F(light[0].culled),			Q|S|AF,	BRTV_CONV_COPY,				0,TM_INVALID_PM},
};



/**************************************************************************
 ** Clip
 **/

static const br_tv_template_entry partClipTemplateEntries[] = {
	{BRT(TYPE_T),		F(clip[0].type), 	Q|S|A, 	BRTV_CONV_COPY,				0,TM_PART|TM_INVALID_PS},
	{BRT(PLANE_V4_X),	F(clip[0].plane),	Q|S|AX,	BRTV_CONV_V4_FIXED_SCALAR,	0,TM_PART|TM_INVALID_PS},
	{BRT(PLANE_V4_F),	F(clip[0].plane),	Q|S|AF,	BRTV_CONV_V4_FLOAT_SCALAR,	0,TM_PART|TM_INVALID_PS},
};


/**************************************************************************
 ** Matrix
 **/

static const br_tv_template_entry partMatrixTemplateEntries[] = {
	{BRT(MODEL_TO_VIEW_M34_X),		F(matrix.model_to_view),		Q | S | AX,	BRTV_CONV_M34_FIXED_SCALAR,	0,TM_PART | TM_M2V | TM_CLEAR_M2V_HINT|TM_INVALID_PM|TM_INVALID_V2M|TM_INVALID_M2S},
	{BRT(MODEL_TO_VIEW_M34_F),		F(matrix.model_to_view),		Q | S | AF,	BRTV_CONV_M34_FLOAT_SCALAR,	0,TM_PART | TM_M2V | TM_CLEAR_M2V_HINT|TM_INVALID_PM|TM_INVALID_V2M|TM_INVALID_M2S},
	{BRT(VIEW_TO_ENVIRONMENT_M34_X),F(matrix.view_to_environment),	Q | S | AX,	BRTV_CONV_M34_FIXED_SCALAR,	0,TM_PART | TM_V2E|TM_INVALID_PS|TM_INVALID_PM},
	{BRT(VIEW_TO_ENVIRONMENT_M34_F),F(matrix.view_to_environment),	Q | S | AF,	BRTV_CONV_M34_FLOAT_SCALAR,	0,TM_PART | TM_V2E|TM_INVALID_PS|TM_INVALID_PM},
	{BRT(VIEW_TO_SCREEN_M4_X),		F(matrix.view_to_screen),		Q | S | AX,	BRTV_CONV_M4_FIXED_SCALAR,	0,TM_PART | TM_V2S | TM_CLEAR_V2S_HINT | TM_V2S_HINT|TM_INVALID_PS|TM_INVALID_PM|TM_INVALID_M2S},
	{BRT(VIEW_TO_SCREEN_M4_F),		F(matrix.view_to_screen),		Q | S | AF,	BRTV_CONV_M4_FLOAT_SCALAR,	0,TM_PART | TM_V2S | TM_CLEAR_V2S_HINT | TM_V2S_HINT|TM_INVALID_PS|TM_INVALID_PM|TM_INVALID_M2S},
	{BRT(MODEL_TO_VIEW_HINT_T),		F(matrix.model_to_view_hint),	Q | S | A,	BRTV_CONV_COPY,				0,TM_PART|TM_INVALID_PM|TM_INVALID_V2M},
	{BRT(VIEW_TO_SCREEN_HINT_T),	F(matrix.view_to_screen_hint),	Q | S | A,	BRTV_CONV_COPY,				0,TM_PART | TM_V2S_HINT|TM_INVALID_PS|TM_INVALID_PM|TM_INVALID_M2S},
	{BRT(VIEW_TO_ENVIRONMENT_HINT_T),F(matrix.view_to_environment_hint),Q | S | A,	BRTV_CONV_COPY,			0,TM_PART | TM_V2E|TM_INVALID_PS|TM_INVALID_PM},
	{BRT(HITHER_Z_X),				F(matrix.hither_z),				Q | S | A,	BRTV_CONV_FIXED_SCALAR,		0,TM_PART},
	{BRT(HITHER_Z_F),				F(matrix.hither_z),				Q | S | A,	BRTV_CONV_FLOAT_SCALAR,		0,TM_PART},
	{BRT(YON_Z_X),					F(matrix.yon_z),				Q | S | A,	BRTV_CONV_FIXED_SCALAR,		0,TM_PART},
	{BRT(YON_Z_F),					F(matrix.yon_z),				Q | S | A,	BRTV_CONV_FLOAT_SCALAR,		0,TM_PART},
};

/**************************************************************************
 ** Enable
 **/

static const br_tv_template_entry partEnableTemplateEntries[] = {
	{BRT(CULL_B),	  	F(enable.flags),	Q|S|A,	BRTV_CONV_BIT,	ENBL_CULL,		TM_PART|TM_INVALID_PS|TM_INVALID_PM},
	{BRT(TRANSFORM_B),	F(enable.flags),	Q|S|A,	BRTV_CONV_BIT,	ENBL_TRANSFORM,	TM_PART|TM_INVALID_PS|TM_INVALID_PM},
	{BRT(SURFACE_B),  	F(enable.flags),	Q|S|A,	BRTV_CONV_BIT,	ENBL_SURFACE,	TM_PART|TM_INVALID_PS|TM_INVALID_PM},
	{BRT(CLIPPING_B),	F(enable.flags),	Q|S|A,	BRTV_CONV_BIT,	ENBL_CLIPPING,	TM_PART|TM_INVALID_PS|TM_INVALID_PM},
	{BRT(BOUNDS_B),		F(enable.flags),	Q|S|A,	BRTV_CONV_BIT,	ENBL_BOUNDS,	TM_PART|TM_INVALID_PS|TM_INVALID_PM},
	{BRT(RENDER_B),		F(enable.flags),	Q|S|A,	BRTV_CONV_BIT,	ENBL_RENDER,	TM_PART|TM_INVALID_PS|TM_INVALID_PM},
};

/**************************************************************************
 ** Hidden Surface
 **/
static const br_tv_template_entry partHiddenSurfaceTemplateEntries[] = {
	{BRT(TYPE_T),				F(hidden.type), 	  	Q|S|A, 	BRTV_CONV_COPY,	0,TM_PART|TM_INVALID_PS|TM_INVALID_PM},
	{BRT(DIVERT_T),				F(hidden.divert),		Q|S|A, 	BRTV_CONV_COPY,	0,TM_PART|TM_INVALID_PS|TM_INVALID_PM},

	{BRT(V1ORDER_TABLE_P),		F(hidden.order_table),	Q|S|A, 	BRTV_CONV_COPY,	0,TM_PART},
	{BRT(V1PRIMITIVE_HEAP_P),	F(hidden.heap), 		Q|S|A, 	BRTV_CONV_COPY,	0,TM_PART|TM_INVALID_PS|TM_INVALID_PM},

	{BRT(V1INSERT_FUNCTION_P),	F(hidden.insert_fn),	Q|S|A, 	BRTV_CONV_COPY,	0,TM_PART},
	{BRT(V1INSERT_ARG1_P),		F(hidden.insert_arg1),	Q|S|A, 	BRTV_CONV_COPY,	0,TM_PART},
	{BRT(V1INSERT_ARG2_P),		F(hidden.insert_arg2),	Q|S|A, 	BRTV_CONV_COPY,	0,TM_PART},
	{BRT(V1INSERT_ARG3_P),		F(hidden.insert_arg3),	Q|S|A, 	BRTV_CONV_COPY,	0,TM_PART},
};

/**************************************************************************
 ** Bounds
 **/

static const br_tv_template_entry partBoundsTemplateEntries[] = {
	{BRT(MIN_V2_X),	F(bounds.min),		Q|S|AX,	BRTV_CONV_V2_FIXED_SCALAR,	0},
	{BRT(MIN_V2_F),	F(bounds.min),		Q|S|AF,	BRTV_CONV_V2_FLOAT_SCALAR,	0},
	{BRT(MAX_V2_X),	F(bounds.max),		Q|S|AX,	BRTV_CONV_V2_FIXED_SCALAR,	0},
	{BRT(MAX_V2_F),	F(bounds.max),		Q|S|AF,	BRTV_CONV_V2_FLOAT_SCALAR,	0},
};


#undef F
#undef P

/**************************************************************************
 ** Default values
 **/
static const struct state_cull partCullDefault = {
	BRT_NONE,
	BRT_MODEL,
};

static const struct state_surface partSurfaceDefault = {
	BR_COLOUR_RGB(255,255,255),				/* colour			*/
	BR_SCALAR(1.0),							/* opacity			*/
	BR_SCALAR(0.0),							/* ka				*/
	BR_SCALAR(0.75),						/* kd				*/
	BR_SCALAR(0.0),							/* ks				*/
	BR_SCALAR(20.0),						/* power			*/

	BR_FALSE,								/* lighting			*/
	BR_FALSE,								/* prelighting		*/
	BR_FALSE,								/* force_front		*/
	BR_FALSE,								/* force_back		*/
	BR_SCALAR(0.0),							/* depth_bias		*/
	BRT_SURFACE,							/* colour_source	*/
	BRT_SURFACE,							/* opacity_source	*/
	BRT_GEOMETRY_MAP,						/* mapping_source	*/

	{{
		BR_VECTOR2(1,0),					/* map_transform	*/
		BR_VECTOR2(0,1),
		BR_VECTOR2(0,0),
	}},
};


static const struct state_light partLightDefault = {
	BRT_NONE,					/* type				*/
	BRT_MODEL,					/* lighting_space	*/
	BR_VECTOR3(0,0,0),			/* position			*/
	BR_VECTOR3(0,0,0),			/* direction		*/
	BR_COLOUR_RGB(255,255,255),	/* colour			*/
	BR_SCALAR(0.866025403),		/* spot_outer		*/
	BR_SCALAR(0.93969262),		/* spot_inner		*/
	BR_SCALAR(0.0),				/* radius_outer		*/
	BR_SCALAR(0.0),				/* radius_inner		*/
	BR_ANGLE_DEG(30.0),			/* angle_outer		*/
	BR_SCALAR(1.0),				/* attenuation_l	*/
	BR_SCALAR(0.0),				/* attenuation_c	*/
	BR_SCALAR(0.0),				/* attenuation_q	*/
	BRT_QUADRATIC,				/* attenuation_type	*/
	BRT_CONSTANT,				/* attenuation_hint	*/
	BR_FALSE,					/* radius_cull		*/
	BR_FALSE,					/* angle_cull		*/
	{ BR_SCALAR(0.0), NULL, 0 },/* volume			*/
	BR_FALSE,					/* culled			*/
};

static const struct state_clip partClipDefault = {
	BRT_NONE,				/* type */
	BR_VECTOR4(0,0,-1,0),	/* plane */
};

static const struct state_matrix partMatrixDefault = {
	{{
		BR_VECTOR3(1,0,0),		/* model_to_view */
		BR_VECTOR3(0,1,0),
		BR_VECTOR3(0,0,1),
		BR_VECTOR3(0,0,0)
	}},
	
	{{
		BR_VECTOR4(1,0,0,0),	/* view_to_screen */
		BR_VECTOR4(0,1,0,0),
		BR_VECTOR4(0,0,1,0),
		BR_VECTOR4(0,0,0,1)
	}},

	{{
		BR_VECTOR3(1,0,0),		/* view_to_environment */
		BR_VECTOR3(0,1,0),
		BR_VECTOR3(0,0,1),
		BR_VECTOR3(0,0,0)
	}},

	BRT_LENGTH_PRESERVING,		/* model_to_view_hint */
	BRT_PARALLEL,				/* view_to_screen_hint */
};

static const struct state_enable partEnableDefault = {
	ENBL_TRANSFORM | ENBL_CLIPPING | ENBL_RENDER,		/* flags			*/
};

static const struct state_hidden partHiddenSurfaceDefault = {
	BRT_NONE,
	BRT_NONE,
	NULL,
	NULL,
	NULL,
	NULL,
};

static const struct state_bounds partBoundsDefault = {
	{BR_SCALAR_MAX, BR_SCALAR_MAX},	
	{BR_SCALAR_MIN, BR_SCALAR_MIN},	
};

/*
 * Sets up an initial state and asociated templates
 */
br_error StateInitialise(struct state_all *state)
{
	int i;

	/*
	 * Some of state can just be copied from static initialisers
	 */
	state->surface = partSurfaceDefault;

	for(i=0; i < MAX_STATE_LIGHTS; i++) {

		if (state->light[i].volume.regions != NULL)
			BrResFree(state->light[i].volume.regions);

		state->light[i] = partLightDefault;
	}

	for(i=0; i < MAX_STATE_CLIP_PLANES; i++)
		state->clip[i] = partClipDefault;

	state->matrix = partMatrixDefault;
	state->enable = partEnableDefault;
	state->hidden = partHiddenSurfaceDefault;
	state->valid = MASK_STATE_LOCAL | MASK_STATE_CACHE;

	state->bounds = partBoundsDefault;
	state->cull = partCullDefault;

	return BRE_OK;
}

/*
 * Common action for all state operations, finds an appropriate template, given part & index
 *
 * Will adjust the state pointer to line up with indexed parts
 */
struct br_tv_template * FindStateTemplate(struct br_renderer *self, struct state_all **state, br_token part, br_int_32 index)
{
    struct br_tv_template_entry *entries;
    struct br_tv_template **tpp;
    int n_entries;

	switch(part) {
	case BRT_CULL:
        if(self->device->templates.partCullTemplate)
            return self->device->templates.partCullTemplate;

        entries = (struct br_tv_template_entry *)partCullTemplateEntries;
        n_entries = BR_ASIZE(partCullTemplateEntries);
        tpp = &self->device->templates.partCullTemplate;
        break;

	case BRT_SURFACE:
        if(self->device->templates.partSurfaceTemplate)
            return self->device->templates.partSurfaceTemplate;

        entries = (struct br_tv_template_entry *)partSurfaceTemplateEntries;
        n_entries = BR_ASIZE(partSurfaceTemplateEntries);
        tpp = &self->device->templates.partSurfaceTemplate;
        break;

	case BRT_MATRIX:
        if(self->device->templates.partMatrixTemplate)
            return self->device->templates.partMatrixTemplate;

        entries = (struct br_tv_template_entry *)partMatrixTemplateEntries;
        n_entries = BR_ASIZE(partMatrixTemplateEntries);
        tpp = &self->device->templates.partMatrixTemplate;
        break;

	case BRT_ENABLE:
        if(self->device->templates.partEnableTemplate)
            return self->device->templates.partEnableTemplate;

        entries = (struct br_tv_template_entry *)partEnableTemplateEntries;
        n_entries = BR_ASIZE(partEnableTemplateEntries);
        tpp = &self->device->templates.partEnableTemplate;
        break;

	case BRT_HIDDEN_SURFACE:
        if(self->device->templates.partHiddenSurfaceTemplate)
            return self->device->templates.partHiddenSurfaceTemplate;

        entries = (struct br_tv_template_entry *)partHiddenSurfaceTemplateEntries;
        n_entries = BR_ASIZE(partHiddenSurfaceTemplateEntries);
        tpp = &self->device->templates.partHiddenSurfaceTemplate;
        break;

	case BRT_BOUNDS:
        if(self->device->templates.partBoundsTemplate)
            return self->device->templates.partBoundsTemplate;

        entries = (struct br_tv_template_entry *)partBoundsTemplateEntries;
        n_entries = BR_ASIZE(partBoundsTemplateEntries);
        tpp = &self->device->templates.partBoundsTemplate;
        break;

	case BRT_LIGHT:
		if(index >= BR_ASIZE((*state)->light))
			return NULL;

		*state = (struct state_all *)((char *)(*state) + index * sizeof((*state)->light[0]));

        if(self->device->templates.partLightTemplate)
            return self->device->templates.partLightTemplate;

        entries = (struct br_tv_template_entry *)partLightTemplateEntries;
        n_entries = BR_ASIZE(partLightTemplateEntries);
        tpp = &self->device->templates.partLightTemplate;
        break;

	case BRT_CLIP:
		if(index >= BR_ASIZE((*state)->clip))
			return NULL;

		*state = (struct state_all *)((char *)(*state) + index * sizeof((*state)->clip[0]));

        if(self->device->templates.partClipTemplate)
            return self->device->templates.partClipTemplate;

        entries = (struct br_tv_template_entry *)partClipTemplateEntries;
        n_entries = BR_ASIZE(partClipTemplateEntries);
        tpp = &self->device->templates.partClipTemplate;
        break;

	default:
		return NULL;
	}

    return *tpp = BrTVTemplateAllocate(self->device,  entries, n_entries);
}

/*
 * Given the mask returned by a template set, perform the indicated actions
 */
void TemplateActions(struct state_all *state, br_token part, br_int_32 index, br_uint_32 mask)
{
#if 1
	if(mask & TM_PART) {
		switch(part) {
		case BRT_CULL:
			state->cull.timestamp = Timestamp();
			break;

		case BRT_SURFACE:
			state->surface.timestamp = Timestamp();
			break;

		case BRT_MATRIX:
			state->matrix.timestamp = Timestamp();
			break;

		case BRT_ENABLE:
			state->enable.timestamp = Timestamp();
			break;

		case BRT_HIDDEN_SURFACE:
			state->hidden.timestamp = Timestamp();
			break;

		case BRT_LIGHT:
			state->timestamp_lights = Timestamp();
			break;

		case BRT_CLIP:
			state->timestamp_clips = Timestamp();
			break;
		}
	}

	if(mask & TM_INDEX) {
		if(part == BRT_LIGHT)
			state->light[index].timestamp = Timestamp();
	}

	if(mask & TM_V2S_HINT)
		state->matrix.timestamp_v2s_hint = Timestamp();

	if(mask & TM_M2V)
		state->matrix.timestamp_m2v = Timestamp();

	if(mask & TM_V2S)
		state->matrix.timestamp_v2s = Timestamp();

	if(mask & TM_V2E)
		state->matrix.timestamp_v2e = Timestamp();

	if(mask & TM_COPY_CACHE)
		state->timestamp_copy_cache = Timestamp();

	if(mask & TM_CACHE)
		state->timestamp_cache = Timestamp();
#endif

	if(mask & TM_CLEAR_M2V_HINT)
		state->matrix.model_to_view_hint = BRT_NONE;

	if(mask & TM_CLEAR_V2S_HINT)
		state->matrix.view_to_screen_hint = BRT_NONE;

	if(mask & TM_INVALID_PS)
		scache.valid_per_scene = BR_FALSE;

	if(mask & TM_INVALID_PM)
		scache.valid_per_model = BR_FALSE;

	if(mask & TM_INVALID_V2M)
		scache.valid_v2m = BR_FALSE;

	if(mask & TM_INVALID_M2S)
		scache.valid_m2s = BR_FALSE;

	if(mask & TM_INVALID_CC)
		state->cache.valid = BR_FALSE;

}

/*
 * Called when something has touch the model_to_view matrix
 */
void TouchModelToView(br_renderer *self)
{
	self->state.matrix.timestamp = Timestamp();
	self->state.matrix.timestamp_m2v = Timestamp();

	scache.valid_v2m = BR_FALSE;
	scache.valid_m2s = BR_FALSE;
	scache.valid_per_model = BR_FALSE;
}


static br_error BR_CALLBACK customLightingVolumeSet(void *block, br_uint_32 *pvalue, struct br_tv_template_entry *tep)
{
	br_light_volume *volume, *new_volume;
	br_uint_32 i;
	
	volume = (br_light_volume *)*pvalue;
	new_volume = (br_light_volume *)((char *)block + tep->offset);

	if (volume == NULL) {

		new_volume->falloff_distance = BR_SCALAR(0.0);
		new_volume->regions = NULL;
		new_volume->nregions = 0;

		return BRE_OK;
	}

	new_volume->falloff_distance = volume->falloff_distance;
	
	if (new_volume->regions != NULL)
		BrResFree(new_volume->regions);

	if (volume->regions != NULL) {

		new_volume->regions = BrResAllocate(scache.res, sizeof(*new_volume->regions) * volume->nregions, BR_MEMORY_OBJECT_DATA);

		if (new_volume->regions != NULL)

			for (i = 0; i < volume->nregions; i++) {

				new_volume->regions[i].planes = BrResAllocate(new_volume->regions, sizeof(*new_volume->regions[i].planes) * volume->regions[i].nplanes, BR_MEMORY_OBJECT_DATA);
				new_volume->regions[i].nplanes = volume->regions[i].nplanes;

				if (new_volume->regions[i].planes == NULL) {
					BrResFree(new_volume->regions);
					new_volume->regions = NULL;
					break;
				}

				BrMemCpy(new_volume->regions[i].planes, volume->regions[i].planes, sizeof(*new_volume->regions[i].planes) * volume->regions[i].nplanes);
			}

	} else

		new_volume->regions = NULL;

	new_volume->nregions = volume->nregions;

	return BRE_OK;
}

