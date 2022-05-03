/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: prepmatl.c 1.13 1998/11/13 16:22:29 jon Exp $
 * $Locker: $
 *
 * Precompute information for materials
	Last change:  TN   11 Apr 97   12:22 pm
 */
#include "v1db.h"
#include "brassert.h"
#include "shortcut.h"

BR_RCS_ID("$Id: prepmatl.c 1.13 1998/11/13 16:22:29 jon Exp $")

/*
 * Mask of renderer state parts that materials are mapped to
 */
#define PARTS_MASK (BR_STATE_SURFACE | BR_STATE_PRIMITIVE | BR_STATE_CULL)

void BR_PUBLIC_ENTRY BrMaterialUpdate(br_material *mat, br_uint_16 flags)
{
	br_token_value tva[64], *tvp;
	br_token t;
	br_int_32 c;

	/*
	 * Make sure there is is renderer
	 */
	if(v1db.renderer == NULL)
		return;

	/*
	 * Save current state and setup default
	 */
	RendererStatePush(v1db.renderer, (br_uint_32)PARTS_MASK);
	RendererStateDefault(v1db.renderer, (br_uint_32)PARTS_MASK);

	/*
	 * If there is an existing state, restore it into renderer
	 */
	if(mat->stored)
		RendererStateRestore(v1db.renderer, mat->stored, (br_uint_32)PARTS_MASK);

	/*
	 * Translate material into state
	 */

	if(flags & BR_MATU_MAP_TRANSFORM)
		RendererPartSet(v1db.renderer, BRT_SURFACE, 0, BRT_AS_MATRIX23_SCALAR(MAP_MATRIX), (br_uint_32)&mat->map_transform);

	if(flags & BR_MATU_RENDERING) {
		tvp = tva;

		tvp->t = BRT_COLOUR_SOURCE_T;
		tvp->v.t = mat->flags & (BR_MATF_LIGHT | BR_MATF_PRELIT)? BRT_VERTEX: BRT_PRIMITIVE;
		tvp++;

		tvp->t = BRT_COLOUR_RGB;
		tvp->v.rgb = mat->colour;
		tvp++;

		tvp->t = BRT_OPACITY_SOURCE_T;
		tvp->v.t = mat->flags & BR_MATF_PREALPHA? BRT_VERTEX: BRT_PRIMITIVE;
		tvp++;

		tvp->t = BRT_AS_SCALAR(OPACITY);
		tvp->v.s = BR_DIV(BrIntToScalar(mat->opacity),BR_SCALAR(255.0));
		tvp++;

        tvp->t = BRT_SEPARATE_SPECULAR_B;
        tvp->v.b =(mat->flags & BR_MATF_LIGHT)&&mat->ks&&(mat->flags & BR_MATF_SEPARATE_SPECULAR);
		tvp++;

		tvp->t = BRT_MODULATE_B;
		tvp->v.b = !!(mat->flags & (BR_MATF_LIGHT | BR_MATF_PRELIT)) ||
			mat->colour != BR_COLOUR_RGB(255, 255, 255);
		tvp++;

		tvp->t = BRT_BLEND_B;
		tvp->v.b = (mat->opacity < 255) || mat->flags & BR_MATF_PREALPHA;
		tvp++;

        tvp->t = BRT_MODULATE_ALPHA_B;
        tvp->v.b = (mat->flags&BR_MATF_MODULATE_ALPHA);
		tvp++;

		tvp->t = BRT_SMOOTH_B;
		tvp->v.b = !!(mat->flags & BR_MATF_SMOOTH);
		tvp++;

		tvp->t = BRT_SMOOTH_OPACITY_B;
		tvp->v.b = !!(mat->flags & BR_MATF_SMOOTH_ALPHA);
		tvp++;

		tvp->t = BRT_DEPTH_WRITE_B;
		tvp->v.b = !(mat->flags & BR_MATF_INHIBIT_DEPTH_WRITE);
		tvp++;

                tvp->t = BRT_COLOUR_KEY_B;
                tvp->v.b = !(mat->flags & BR_MATF_DISABLE_COLOUR_KEY);
		tvp++;

		if((mat->flags & BR_MATF_SUBDIVIDE)){
			tvp->t = BRT_PERSPECTIVE_T;
			tvp->v.t = BRT_SUBDIVIDE;
			tvp++;
			tvp->t = BRT_SUBDIVIDE_TOLERANCE_I32;
			tvp->v.i32 = mat->subdivide_tolerance;
			tvp++;
		} else {
			tvp->t = BRT_PERSPECTIVE_B;
			tvp->v.b = !!(mat->flags & BR_MATF_PERSPECTIVE);
			tvp++;
		}

		tvp->t = BRT_DECAL_B;
		tvp->v.b = !!(mat->flags & BR_MATF_DECAL);
		tvp++;

		tvp->t = BRT_DITHER_MAP_B;
		tvp->v.b = !!(mat->flags & BR_MATF_DITHER);
		tvp++;

		tvp->t = BRT_DITHER_COLOUR_B;
		tvp->v.b = !!(mat->flags & BR_MATF_DITHER);
		tvp++;

		tvp->t = BRT_INDEX_BASE_I32;
		tvp->v.i32 = mat->index_base;
		tvp++;

		tvp->t = BRT_INDEX_RANGE_I32;
		tvp->v.i32 = mat->index_range;
		tvp++;

		tvp->t = BRT_MAP_ANTIALIASING_T;
		tvp->v.t = (mat->flags & BR_MATF_MAP_ANTIALIASING)?BRT_LINEAR:BRT_NONE;
		tvp++;

		tvp->t = BRT_MAP_INTERPOLATION_T;
		tvp->v.t = (mat->flags & BR_MATF_MAP_INTERPOLATION)?BRT_LINEAR:BRT_NONE;
		tvp++;

		tvp->t = BRT_MIP_INTERPOLATION_T;
		tvp->v.t = (mat->flags & BR_MATF_MIP_INTERPOLATION)?BRT_LINEAR:BRT_NONE;
		tvp++;

		tvp->t = BRT_DEPTH_TEST_T;

		switch (mat->mode & BR_MATM_DEPTH_TEST_MASK) {

		case BR_MATM_DEPTH_TEST_GT:
			tvp->v.t = BRT_GREATER;
			break;

		case BR_MATM_DEPTH_TEST_GE:
			tvp->v.t = BRT_GREATER_OR_EQUAL;
			break;

		case BR_MATM_DEPTH_TEST_EQ:
			tvp->v.t = BRT_EQUAL;
			break;

		case BR_MATM_DEPTH_TEST_NE:
			tvp->v.t = BRT_NOT_EQUAL;
			break;

		case BR_MATM_DEPTH_TEST_LE:
			tvp->v.t = BRT_LESS_OR_EQUAL;
			break;

		case BR_MATM_DEPTH_TEST_LT:
			tvp->v.t = BRT_LESS;
			break;

		case BR_MATM_DEPTH_TEST_NV:
			tvp->v.t = BRT_NEVER;
			break;

		case BR_MATM_DEPTH_TEST_AL:
			tvp->v.t = BRT_ALWAYS;
			break;
		}

		tvp++;

		tvp->t = BRT_BLEND_MODE_T;

		switch (mat->mode & BR_MATM_BLEND_MODE_MASK) {

		case BR_MATM_BLEND_MODE_STANDARD:
			tvp->v.t = BRT_BLEND_STANDARD;
			break;

		case BR_MATM_BLEND_MODE_SUMMED:
			tvp->v.t = BRT_BLEND_SUMMED;
			break;

		case BR_MATM_BLEND_MODE_DIMMED:
			tvp->v.t = BRT_BLEND_DIMMED;
			break;

		case BR_MATM_BLEND_MODE_PREMULTIPLIED:
			tvp->v.t = BRT_BLEND_PREMULTIPLIED;
			break;
		}

		tvp++;

		tvp->t = BRT_MAP_WIDTH_LIMIT_T;

		switch (mat->mode & BR_MATM_MAP_WIDTH_LIMIT_MASK) {

		case BR_MATM_MAP_WIDTH_LIMIT_WRAP:
			tvp->v.t = BRT_WRAP;
			break;

		case BR_MATM_MAP_WIDTH_LIMIT_CLAMP:
			tvp->v.t = BRT_CLAMP;
			break;

		case BR_MATM_MAP_WIDTH_LIMIT_MIRROR:
			tvp->v.t = BRT_MIRROR;
			break;
		}

		tvp++;

		tvp->t = BRT_MAP_HEIGHT_LIMIT_T;

		switch (mat->mode & BR_MATM_MAP_HEIGHT_LIMIT_MASK) {

		case BR_MATM_MAP_HEIGHT_LIMIT_WRAP:
			tvp->v.t = BRT_WRAP;
			break;

		case BR_MATM_MAP_HEIGHT_LIMIT_CLAMP:
			tvp->v.t = BRT_CLAMP;
			break;

		case BR_MATM_MAP_HEIGHT_LIMIT_MIRROR:
			tvp->v.t = BRT_MIRROR;
			break;
		}

		tvp++;

		tvp->t = BRT_FOG_B;
		tvp->v.b = !!(mat->flags & BR_MATF_FOG_LOCAL);
		tvp++;
	
		tvp->t = BRT_FOG_T;
		tvp->v.t = (mat->flags & BR_MATF_FOG_LOCAL)?BRT_LINEAR:BRT_NONE;
		tvp++;
	
		if((mat->flags & BR_MATF_FOG_LOCAL)){
			tvp->t = BRT_AS_SCALAR(FOG_MIN);
			tvp->v.s = mat->fog_min;
			tvp++;
			tvp->t = BRT_AS_SCALAR(FOG_MAX);
			tvp->v.s = mat->fog_max;
			tvp++;
			tvp->t = BRT_FOG_RGB;
			tvp->v.rgb = mat->fog_colour;
			tvp++;
			tvp->t = BRT_FOG_SCALE_U8;
			tvp->v.u8 = mat->opacity;
			tvp++;
		}

		tvp->t = BR_NULL_TOKEN;

		RendererPartSetMany(v1db.renderer, BRT_PRIMITIVE, 0, tva, &c);

		tvp = tva;

		tvp->t = BRT_FORCE_FRONT_B;
		tvp->v.b = !!(mat->flags & BR_MATF_FORCE_FRONT);
		tvp++;

		tvp->t = BRT_FORCE_BACK_B;
		tvp->v.b = !!(mat->flags & BR_MATF_FORCE_BACK);
		tvp++;

		tvp->t = BRT_AS_SCALAR(DEPTH_BIAS);
		tvp->v.s = mat->depth_bias;
		tvp++;

		tvp->t = BR_NULL_TOKEN;

		RendererPartSetMany(v1db.renderer, BRT_SURFACE, 0, tva, &c);

		t = BRT_ONE_SIDED;

		if(mat->flags & BR_MATF_ALWAYS_VISIBLE)
			t = BRT_NONE;

		if(mat->flags & BR_MATF_TWO_SIDED)
			t = BRT_TWO_SIDED;


		RendererPartSet(v1db.renderer, BRT_CULL, 0, BRT_TYPE_T, t);
	}

	if(flags & BR_MATU_LIGHTING) {
		tvp = tva;

		tvp->t = BRT_COLOUR_RGB;
		tvp->v.rgb = mat->colour;
		tvp++;

		tvp->t = BRT_AS_SCALAR(OPACITY);
		tvp->v.s = BR_DIV(BrIntToScalar(mat->opacity),BR_SCALAR(255.0));
		tvp++;

		tvp->t = BRT_AS_SCALAR(AMBIENT);
		tvp->v.s = BrUFractionToScalar(mat->ka);
		tvp++;

		tvp->t = BRT_AS_SCALAR(DIFFUSE);
		tvp->v.s = BrUFractionToScalar(mat->kd);
		tvp++;

		tvp->t = BRT_AS_SCALAR(SPECULAR);
		tvp->v.s = BrUFractionToScalar(mat->ks);
		tvp++;

		tvp->t = BRT_AS_SCALAR(SPECULAR_POWER);
		tvp->v.s = mat->power;
		tvp++;

		tvp->t = BRT_LIGHTING_B;
		tvp->v.b = !!(mat->flags & BR_MATF_LIGHT);
		tvp++;

		tvp->t = BRT_PRELIGHTING_B;
		tvp->v.b = mat->flags & BR_MATF_PRELIT && mat->flags & BR_MATF_LIGHT;
		tvp++;

		tvp->t = BRT_COLOUR_SOURCE_T;
		if(mat->flags & BR_MATF_PRELIT)
			tvp->v.t = mat->flags & BR_MATF_LIGHT?BRT_SURFACE:BRT_GEOMETRY;
		else
			tvp->v.t = BRT_SURFACE;
		tvp++;
		
		tvp->t = BRT_OPACITY_SOURCE_T;
		tvp->v.t = (mat->flags & BR_MATF_PREALPHA)?BRT_GEOMETRY:BRT_SURFACE;
		tvp++;
		
		tvp->t = BRT_MAPPING_SOURCE_T;
		tvp->v.t = BRT_GEOMETRY_MAP;

		if(mat->flags & (BR_MATF_ENVIRONMENT_L))
			tvp->v.t = BRT_ENVIRONMENT_LOCAL;

		if(mat->flags & (BR_MATF_ENVIRONMENT_I))
			tvp->v.t = BRT_ENVIRONMENT_INFINITE;

		if(mat->flags & (BR_MATF_QUAD_MAPPING))
			tvp->v.t = BRT_QUAD;

		tvp++;

		tvp->t = BR_NULL_TOKEN;

		RendererPartSetMany(v1db.renderer, BRT_SURFACE, 0, tva, &c);
	}

	if(flags & BR_MATU_COLOURMAP) {
		tvp = tva;

		tvp->t = BRT_COLOUR_MAP_O;
		tvp->v.o = mat->colour_map?mat->colour_map->stored:NULL;
		tvp++;

		tvp->t = BRT_INDEX_SHADE_O;
		tvp->v.o = mat->index_shade?mat->index_shade->stored:NULL;
		tvp++;

		tvp->t = BRT_INDEX_BLEND_O;
		tvp->v.o = mat->index_blend?mat->index_blend->stored:NULL;
		tvp++;

		tvp->t = BRT_INDEX_FOG_O;
		tvp->v.o = mat->index_fog?mat->index_fog->stored:NULL;
		tvp++;

		tvp->t = BR_NULL_TOKEN;
		RendererPartSetMany(v1db.renderer, BRT_PRIMITIVE, 0, tva, &c);
	}

	if(flags & BR_MATU_SCREENDOOR) {
		tvp = tva;
		tvp->t = BRT_SCREEN_DOOR_O;
		tvp->v.o = mat->screendoor?mat->screendoor->stored:NULL;
		tvp++;

		tvp->t = BR_NULL_TOKEN;
		RendererPartSetMany(v1db.renderer, BRT_PRIMITIVE, 0, tva, &c);
	}

	if(flags & BR_MATU_EXTRA_SURF && mat->extra_surf)
		RendererPartSetMany(v1db.renderer, BRT_SURFACE, 0, mat->extra_surf, &c);

	if(flags & BR_MATU_EXTRA_PRIM && mat->extra_prim)
		RendererPartSetMany(v1db.renderer, BRT_PRIMITIVE, 0, mat->extra_prim, &c);

	/*
	 * If there was no previous state, create new one, otherwise, update existing state
	 */
	if(mat->stored)
		RendererStateSave(v1db.renderer, mat->stored, (br_uint_32)PARTS_MASK);
	else
		RendererStateStoredNew(v1db.renderer, (br_renderer_state_stored **)&mat->stored, (br_uint_32)PARTS_MASK, NULL);

	/*
	 * Restore previous state
	 */
	RendererStatePop(v1db.renderer, (br_uint_32)PARTS_MASK);
}

void BrMaterialClear(br_material *mat)
{
	/*
	 * Free up an allocated state
	 */
	if(mat->stored == NULL)
		return;

	ObjectFree(mat->stored);
	mat->stored = NULL;
}
