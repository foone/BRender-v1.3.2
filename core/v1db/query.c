/*
 * Copyright (c) 1993-1996 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: query.c 1.1 1997/12/10 16:41:33 jon Exp $
 * $Locker: $
 *
 * Device querying routines
 * Much of this comes from prepmatl.c
 */
#include "v1db.h"
#include "brassert.h"
#include "shortcut.h"

BR_RCS_ID("$Id: query.c 1.1 1997/12/10 16:41:33 jon Exp $")

/*
 * Mask of renderer state parts that materials are mapped to
 */
#define PARTS_MASK (BR_STATE_SURFACE | BR_STATE_PRIMITIVE | BR_STATE_CULL)

#if 0
/*
 * Searches for token in tokens.
 * If found, returns pointer to token, else returns NULL
 */
STATIC br_token_value *CheckTokenPresence(br_token_value *tokens, br_token token)
{
    br_token_value *tvp = tokens;

    while(tvp->t != BR_NULL_TOKEN)
        if(tvp->t == token)
            return tvp;

    return NULL;
}

static br_boolean materialValid(br_material *mat)
{
    br_boolean r = BR_TRUE;
    br_token_value *dtokens, *tvp;
    br_uint_32 t, d;

    // Clear state
    // dtokens = Get feature set tokens from DDI, for given state
    dtokens = NULL;

    /* Compare each feature group in token lists */

    /* Opacity, Blend, Index_blend */
    t = d = 0;
    if(tvp = CheckTokenPresence(tokens, BRT_AS_SCALAR(OPACITY)))
        t |= (tvp->v.s < BR_SCALAR(1)) ? 0x1 : 0;
    if(tvp = CheckTokenPresence(tokens, BRT_BLEND_B))
        t |= (tvp->v.b) ? 0x2 : 0;
    if(tvp = CheckTokenPresence(tokens, BRT_INDEX_BLEND_O))
        t |= (tvp->v.o) ? 0x4 : 0;
    d |= (CheckTokenPresence(dtokens, BRT_AS_SCALAR(OPACITY))) ? 0x1 : 0;
    d |= (CheckTokenPresence(dtokens, BRT_BLEND_B)) ? 0x2 : 0;
    d |= (CheckTokenPresence(dtokens, BRT_INDEX_BLEND_O)) ? 0x4 : 0;
    if(t && !(t & d))
        r = BR_FALSE;

    /* Modulate */
    if(tvp = CheckTokenPresence(tokens, BRT_MODULATE_B))
        if(tvp->v.b && !CheckTokenPresence(dtokens, BRT_MODULATE_B))
            r = BR_FALSE;

    /* Smooth */
    if(tvp = CheckTokenPresence(tokens, BRT_SMOOTH_B))
        if(tvp->v.b && !CheckTokenPresence(dtokens, BRT_SMOOTH_B))
            r = BR_FALSE;

    /* Perspective, Subdivide, Subdivide_tolerance */
    t = d = 0;
    if(tvp = CheckTokenPresence(tokens, BRT_PERSPECTIVE_B))
        t |= (tvp->v.b) ? 0x1 : 0;
    if(tvp = CheckTokenPresence(tokens, BRT_SUBDIVIDE))
        t |= (tvp->v.b) ? 0x2 : 0;
    if(tvp = CheckTokenPresence(tokens, BRT_SUBDIVIDE_TOLERANCE_I32))
        t |= (tvp->v.i32) ? 0x4 : 0;
    d |= (CheckTokenPresence(dtokens, BRT_PERSPECTIVE_B)) ? 0x1 : 0;
    d |= (CheckTokenPresence(dtokens, BRT_SUBDIVIDE)) ? 0x2 : 0;
    d |= (CheckTokenPresence(dtokens, BRT_SUBDIVIDE_TOLERANCE_I32)) ? 0x4 : 0;
    if(t && !(t & d))
        r = BR_FALSE;

    /* Decal */
    if(tvp = CheckTokenPresence(tokens, BRT_DECAL_B))
        if(tvp->v.b && !CheckTokenPresence(dtokens, BRT_DECAL_B))
            r = BR_FALSE;

    /* Dither map */
    if(tvp = CheckTokenPresence(tokens, BRT_DITHER_MAP_B))
        if(tvp->v.b && !CheckTokenPresence(dtokens, BRT_DITHER_MAP_B))
            r = BR_FALSE;

    /* Dither colour */
    if(tvp = CheckTokenPresence(tokens, BRT_DITHER_COLOUR_B))
        if(tvp->v.b && !CheckTokenPresence(dtokens, BRT_DITHER_COLOUR_B))
            r = BR_FALSE;

    /* Index_base, Index_range, Colour */
    t = d = 0;
    if(tvp = CheckTokenPresence(tokens, BRT_INDEX_BASE_I32))
        t |= (tvp->v.b) ? 0x1 : 0;
    if(tvp = CheckTokenPresence(tokens, BRT_INDEX_RANGE_I32))
        t |= (tvp->v.b) ? 0x2 : 0;
    if(tvp = CheckTokenPresence(tokens, BRT_COLOUR_RGB))
        t |= (tvp->v.rgb) ? 0x4 : 0;
    d |= (CheckTokenPresence(dtokens, BRT_INDEX_BASE_I32)) ? 0x1 : 0;
    d |= (CheckTokenPresence(dtokens, BRT_INDEX_RANGE_I32)) ? 0x2 : 0;
    d |= (CheckTokenPresence(dtokens, BRT_COLOUR_RGB)) ? 0x4 : 0;
    if(t && !(t & d))
        r = BR_FALSE;

    /* Map Antialiasing */
    if(tvp = CheckTokenPresence(tokens, BRT_MAP_ANTIALIASING_T))
        if(tvp->v.t != BRT_NONE && !CheckTokenPresence(dtokens, BRT_MAP_ANTIALIASING_T))
            r = BR_FALSE;

    /* Map Interpolation */
    if(tvp = CheckTokenPresence(tokens, BRT_MAP_INTERPOLATION_T))
        if(tvp->v.t != BRT_NONE && !CheckTokenPresence(dtokens, BRT_MAP_INTERPOLATION_T))
            r = BR_FALSE;

    /* Mip Interpolation */
    if(tvp = CheckTokenPresence(tokens, BRT_MIP_INTERPOLATION_T))
        if(tvp->v.t != BRT_NONE && !CheckTokenPresence(dtokens, BRT_MIP_INTERPOLATION_T))
            r = BR_FALSE;

    /* Fog, Fog_min, Fog_max, Fog_rgb, Index_fog */
    t = d = 0;
    if(tvp = CheckTokenPresence(tokens, BRT_FOG_T))
        t |= (tvp->v.t != BRT_NONE) ? 0x1 : 0;
    if(tvp = CheckTokenPresence(tokens, BRT_AS_SCALAR(FOG_MIN)))
        t |= (tvp->v.s) ? 0x2 : 0;
    if(tvp = CheckTokenPresence(tokens, BRT_AS_SCALAR(FOG_MAX)))
        t |= (tvp->v.s) ? 0x4 : 0;
    if(tvp = CheckTokenPresence(tokens, BRT_FOG_RGB))
        t |= (tvp->v.rgb) ? 0x8 : 0;
    if(tvp = CheckTokenPresence(tokens, BRT_INDEX_FOG_O))
        t |= (tvp->v.o) ? 0x10 : 0;
    d |= (CheckTokenPresence(dtokens, BRT_FOG_T)) ? 0x1 : 0;
    d |= (CheckTokenPresence(dtokens, BRT_AS_SCALAR(FOG_MIN))) ? 0x2 : 0;
    d |= (CheckTokenPresence(dtokens, BRT_AS_SCALAR(FOG_MAX))) ? 0x4 : 0;
    d |= (CheckTokenPresence(dtokens, BRT_FOG_RGB)) ? 0x8 : 0;
    d |= (CheckTokenPresence(dtokens, BRT_INDEX_FOG_O)) ? 0x10 : 0;
    if(t && !(t & d))
        r = BR_FALSE;

    /* Type */
    if(tvp = CheckTokenPresence(tokens, BRT_TYPE_T))
        if(tvp->v.t != BRT_ONE_SIDED && !CheckTokenPresence(dtokens, BRT_TYPE_T))
            r = BR_FALSE;

    /* Ambient */
    if(tvp = CheckTokenPresence(tokens, BRT_AS_SCALAR(AMBIENT)))
        if(tvp->v.s && !CheckTokenPresence(dtokens, BRT_AS_SCALAR(AMBIENT)))
            r = BR_FALSE;

    /* Diffuse */
    if(tvp = CheckTokenPresence(tokens, BRT_AS_SCALAR(DIFFUSE)))
        if(tvp->v.s && !CheckTokenPresence(dtokens, BRT_AS_SCALAR(DIFFUSE)))
            r = BR_FALSE;

    /* Specular */
    if(tvp = CheckTokenPresence(tokens, BRT_AS_SCALAR(SPECULAR)))
        if(tvp->v.s && !CheckTokenPresence(dtokens, BRT_AS_SCALAR(SPECULAR)))
            r = BR_FALSE;

    /* Specular_power */
    if(tvp = CheckTokenPresence(tokens, BRT_AS_SCALAR(SPECULAR_POWER)))
        if(tvp->v.s && !CheckTokenPresence(dtokens, BRT_AS_SCALAR(SPECULAR_POWER)))
            r = BR_FALSE;

    /* Lighting, Index_shade */
    t = d = 0;
    if(tvp = CheckTokenPresence(tokens, BRT_LIGHTING_B))
        t |= (tvp->v.b) ? 0x1 : 0;
    if(tvp = CheckTokenPresence(tokens, BRT_INDEX_SHADE_O))
        t |= (tvp->v.o) ? 0x2 : 0;
    d |= (CheckTokenPresence(dtokens, BRT_LIGHTING_B)) ? 0x1 : 0;
    d |= (CheckTokenPresence(dtokens, BRT_INDEX_SHADE_O)) ? 0x2 : 0;
    if(t && !(t & d))
        r = BR_FALSE;

    /* Force Front */
    if(tvp = CheckTokenPresence(tokens, BRT_FORCE_FRONT_B))
        if(tvp->v.b && !CheckTokenPresence(dtokens, BRT_FORCE_FRONT_B))
            r = BR_FALSE;

    /* Colour_source */
    if(tvp = CheckTokenPresence(tokens, BRT_COLOUR_SOURCE_T))
        if(tvp->v.t != BRT_SURFACE && !CheckTokenPresence(dtokens, BRT_COLOUR_SOURCE_T))
            r = BR_FALSE;

    /* Colour_map */
    if(tvp = CheckTokenPresence(tokens, BRT_COLOUR_MAP_O))
        if(tvp->v.o && !CheckTokenPresence(dtokens, BRT_COLOUR_MAP_O))
            r = BR_FALSE;

    /* Screen_door */
    if(tvp = CheckTokenPresence(tokens, BRT_SCREEN_DOOR_O))
        if(tvp->v.o && !CheckTokenPresence(dtokens, BRT_SCREEN_DOOR_O))
            r = BR_FALSE;

    return r;
}

#endif

br_error BR_PUBLIC_ENTRY BrQueryMaterialPerformance(br_material *mat,
                                                    br_scalar *s)
{
	br_token_value tva[32], *tvp;
	br_token t;
	br_int_32 c;
    br_uint_32 speed;

	/*
     * Make sure there is a renderer
	 */
	if(v1db.renderer == NULL)
		return BRE_FAIL;

	RendererStateDefault(v1db.renderer, (br_uint_32)PARTS_MASK);

	/*
	 * Translate material into state
	 */

    RendererPartSet(v1db.renderer, BRT_SURFACE, 0, BRT_AS_MATRIX23_SCALAR(MAP_MATRIX), (br_uint_32)&mat->map_transform);

	tvp = tva;

	tvp->t = BRT_COLOUR_RGB;
	tvp->v.rgb = mat->colour;
	tvp++;

	tvp->t = BRT_AS_SCALAR(OPACITY);
	tvp->v.s = BR_DIV(BrIntToScalar(mat->opacity),BR_SCALAR(255.0));
	tvp++;

	tvp->t = BRT_MODULATE_B;
	tvp->v.b = !!(mat->flags & (BR_MATF_LIGHT | BR_MATF_PRELIT));
	tvp++;

	tvp->t = BRT_BLEND_B;
	tvp->v.b = (mat->opacity < 255);
	tvp++;

	tvp->t = BRT_SMOOTH_B;
	tvp->v.b = !!(mat->flags & BR_MATF_SMOOTH);
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
    }

	tvp->t = BR_NULL_TOKEN;

	RendererPartSetMany(v1db.renderer, BRT_PRIMITIVE, 0, tva, &c);

	t = BRT_ONE_SIDED;

	if(mat->flags & BR_MATF_ALWAYS_VISIBLE)
		t = BRT_NONE;

	if(mat->flags & BR_MATF_TWO_SIDED)
		t = BRT_TWO_SIDED;

	RendererPartSet(v1db.renderer, BRT_CULL, 0, BRT_TYPE_T, t);

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
	if(mat->flags & BR_MATF_PRELIT)
		tvp->v.b = BR_FALSE;
	else
		tvp->v.b = !!(mat->flags & BR_MATF_LIGHT);
	tvp++;

	tvp->t = BRT_FORCE_FRONT_B;
	tvp->v.b = !!(mat->flags & BR_MATF_FORCE_FRONT);
	tvp++;

	tvp->t = BRT_COLOUR_SOURCE_T;
	tvp->v.t = (mat->flags & BR_MATF_PRELIT)?BRT_GEOMETRY:BRT_SURFACE;
	tvp++;

	tvp->t = BRT_MAPPING_SOURCE_T;
	tvp->v.t = BRT_GEOMETRY_MAP;

	if(mat->flags & (BR_MATF_ENVIRONMENT_L))
		tvp->v.t = BRT_ENVIRONMENT_LOCAL;

	if(mat->flags & (BR_MATF_ENVIRONMENT_I))
		tvp->v.t = BRT_ENVIRONMENT_INFINITE;
	tvp++;

	tvp->t = BR_NULL_TOKEN;

	RendererPartSetMany(v1db.renderer, BRT_SURFACE, 0, tva, &c);

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

	tvp = tva;
	tvp->t = BRT_SCREEN_DOOR_O;
	tvp->v.o = mat->screendoor?mat->screendoor->stored:NULL;
	tvp++;

	tvp->t = BR_NULL_TOKEN;
	RendererPartSetMany(v1db.renderer, BRT_PRIMITIVE, 0, tva, &c);

	if(mat->extra_surf)
		RendererPartSetMany(v1db.renderer, BRT_SURFACE, 0, mat->extra_surf, &c);

	if(mat->extra_prim)
		RendererPartSetMany(v1db.renderer, BRT_PRIMITIVE, 0, mat->extra_prim, &c);

	RendererStateQueryPerformance(v1db.renderer, &speed);

    *s = BR_DIV(BR_SCALAR(speed), BR_SCALAR(100));

    return BRE_OK;
}


br_error BR_PUBLIC_ENTRY BrQueryMaterialCapability(br_material *mat,
                                                   br_token_value *tokens)
{
	br_token_value tva[32], *tvp;
	br_token t;
	br_int_32 c;
    br_uint_32 speed;

	/*
     * Make sure there is a renderer
	 */
	if(v1db.renderer == NULL)
		return BRE_FAIL;

	RendererStateDefault(v1db.renderer, (br_uint_32)PARTS_MASK);

	/*
	 * Translate material into state
	 */

    RendererPartSet(v1db.renderer, BRT_SURFACE, 0, BRT_AS_MATRIX23_SCALAR(MAP_MATRIX), (br_uint_32)&mat->map_transform);

	tvp = tva;

	tvp->t = BRT_COLOUR_RGB;
	tvp->v.rgb = mat->colour;
	tvp++;

	tvp->t = BRT_AS_SCALAR(OPACITY);
	tvp->v.s = BR_DIV(BrIntToScalar(mat->opacity),BR_SCALAR(255.0));
	tvp++;

	tvp->t = BRT_MODULATE_B;
	tvp->v.b = !!(mat->flags & (BR_MATF_LIGHT | BR_MATF_PRELIT));
	tvp++;

	tvp->t = BRT_BLEND_B;
	tvp->v.b = (mat->opacity < 255);
	tvp++;

	tvp->t = BRT_SMOOTH_B;
	tvp->v.b = !!(mat->flags & BR_MATF_SMOOTH);
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
    }

	tvp->t = BR_NULL_TOKEN;

	RendererPartSetMany(v1db.renderer, BRT_PRIMITIVE, 0, tva, &c);

	t = BRT_ONE_SIDED;

	if(mat->flags & BR_MATF_ALWAYS_VISIBLE)
		t = BRT_NONE;

	if(mat->flags & BR_MATF_TWO_SIDED)
		t = BRT_TWO_SIDED;

	RendererPartSet(v1db.renderer, BRT_CULL, 0, BRT_TYPE_T, t);

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
	if(mat->flags & BR_MATF_PRELIT)
		tvp->v.b = BR_FALSE;
	else
		tvp->v.b = !!(mat->flags & BR_MATF_LIGHT);
	tvp++;

	tvp->t = BRT_FORCE_FRONT_B;
	tvp->v.b = !!(mat->flags & BR_MATF_FORCE_FRONT);
	tvp++;

	tvp->t = BRT_COLOUR_SOURCE_T;
	tvp->v.t = (mat->flags & BR_MATF_PRELIT)?BRT_GEOMETRY:BRT_SURFACE;
	tvp++;

	tvp->t = BRT_MAPPING_SOURCE_T;
	tvp->v.t = BRT_GEOMETRY_MAP;

	if(mat->flags & (BR_MATF_ENVIRONMENT_L))
		tvp->v.t = BRT_ENVIRONMENT_LOCAL;

	if(mat->flags & (BR_MATF_ENVIRONMENT_I))
		tvp->v.t = BRT_ENVIRONMENT_INFINITE;
	tvp++;

	tvp->t = BR_NULL_TOKEN;

	RendererPartSetMany(v1db.renderer, BRT_SURFACE, 0, tva, &c);

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

	tvp = tva;
	tvp->t = BRT_SCREEN_DOOR_O;
	tvp->v.o = mat->screendoor?mat->screendoor->stored:NULL;
	tvp++;

	tvp->t = BR_NULL_TOKEN;
	RendererPartSetMany(v1db.renderer, BRT_PRIMITIVE, 0, tva, &c);

	if(mat->extra_surf)
		RendererPartSetMany(v1db.renderer, BRT_SURFACE, 0, mat->extra_surf, &c);

	if(mat->extra_prim)
		RendererPartSetMany(v1db.renderer, BRT_PRIMITIVE, 0, mat->extra_prim, &c);

	RendererStateQueryPerformance(v1db.renderer, &speed);

    *s = BR_DIV(BR_SCALAR(speed), BR_SCALAR(100));

    return BRE_OK;
}



