/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: light24.c 1.9 1998/08/13 13:56:38 jon Exp $
 * $Locker: $
 *
 * Bits of the lighting model for coloured pixels
 */

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

#include "math_ip.h"
#include "lightmac.h"

BR_RCS_ID("$Id: light24.c 1.9 1998/08/13 13:56:38 jon Exp $");


static br_scalar diffuse_r;
static br_scalar diffuse_g;
static br_scalar diffuse_b;

static br_scalar specular_r;
static br_scalar specular_g;
static br_scalar specular_b;


/*
 * Gross macros and declarations to allow inlining of common lighting routines
 */
#ifdef __VISUALC

#define INLINE_LIGHTING(type, fn, p, n) \
	if (alp->type == fn) \
		fn(self, (p), (n), alp); \
	else

#define CALL_LIGHTING_FN(fn, p, n) \
	INLINE_LIGHTING(fn, lightingColourNull, p, n) \
	INLINE_LIGHTING(fn, lightingColourDirect, p, n) \
	INLINE_LIGHTING(fn, lightingColourPointRadii, p, n) \
	INLINE_LIGHTING(fn, lightingColourSpotRadii, p, n) \
	alp->fn(self, (p), (n), alp)

#define INLINE __inline

INLINE static void lightingColourNull(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourAmbientVolume(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourAmbientVolumeFalloff(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourAmbientRadii(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourAmbientRadiiVolume(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourAmbientRadiiVolumeFalloff(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourDirect(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourDirectSpecular(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourDirectVolume(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourDirectVolumeSpecular(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourDirectVolumeFalloff(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourDirectVolumeFalloffSpecular(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourPoint(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourPointSpecular(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourPointVolume(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourPointVolumeSpecular(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourPointVolumeFalloff(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourPointVolumeFalloffSpecular(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourPointAttn(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourPointAttnSpecular(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourPointAttnVolume(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourPointAttnVolumeSpecular(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourPointAttnVolumeFalloff(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourPointAttnVolumeFalloffSpecular(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourPointAttnLinear(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourPointAttnLinearSpecular(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourPointAttnLinearVolume(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourPointAttnLinearVolumeSpecular(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourPointAttnLinearVolumeFalloff(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourPointAttnLinearVolumeFalloffSpecular(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourPointRadii(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourPointRadiiSpecular(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourPointRadiiVolume(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourPointRadiiVolumeSpecular(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourPointRadiiVolumeFalloff(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourPointRadiiVolumeFalloffSpecular(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourSpot(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourSpotSpecular(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourSpotVolume(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourSpotVolumeSpecular(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourSpotVolumeFalloff(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourSpotVolumeFalloffSpecular(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourSpotAttn(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourSpotAttnSpecular(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourSpotAttnVolume(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourSpotAttnVolumeSpecular(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourSpotAttnVolumeFalloff(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourSpotAttnVolumeFalloffSpecular(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourSpotAttnLinear(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourSpotAttnLinearSpecular(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourSpotAttnLinearVolume(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourSpotAttnLinearVolumeSpecular(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourSpotAttnLinearVolumeFalloff(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourSpotAttnLinearVolumeFalloffSpecular(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourSpotRadii(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourSpotRadiiSpecular(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourSpotRadiiVolume(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourSpotRadiiVolumeSpecular(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourSpotRadiiVolumeFalloff(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);
INLINE static void lightingColourSpotRadiiVolumeFalloffSpecular(br_renderer *self, br_vector3 *p, br_vector3 *n, struct active_light *alp);

#else

#define CALL_LIGHTING_FN(fn, p, n) alp->fn(self, (p), (n), alp)

#define INLINE

#endif

/*
 * Lighting function for unlit coloured 
 */
void SURFACE_CALL SurfaceColourZero(br_renderer *self, br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour colour, br_scalar *comp)
{
	comp[C_R] = self->state.cache.comp_offsets[C_R];
	comp[C_G] = self->state.cache.comp_offsets[C_G];
	comp[C_B] = self->state.cache.comp_offsets[C_B];
}

/*
 * Lighting function for prelit coloured 
 */
void SURFACE_CALL SurfaceColourUnlit(br_renderer *self, br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour colour, br_scalar *comp)
{
	comp[C_R] = BR_MUL(self->state.cache.comp_scales[C_R],
					BR_CONST_DIV(BrIntToScalar(BR_RED(colour)),256)) +
					self->state.cache.comp_offsets[C_R];

	comp[C_G] = BR_MUL(self->state.cache.comp_scales[C_G],
 					BR_CONST_DIV(BrIntToScalar(BR_GRN(colour)),256)) +
					self->state.cache.comp_offsets[C_G];

	comp[C_B] = BR_MUL(self->state.cache.comp_scales[C_B],
					BR_CONST_DIV(BrIntToScalar(BR_BLU(colour)),256)) +
					self->state.cache.comp_offsets[C_B];
}

br_boolean pointInLightVolumes(br_vector3 *p, br_light_volume *volume);
br_scalar lightVolumeAttenuation(br_vector3 *p, br_light_volume *volume);

/*
 * Accumulate lighting for multiple active lights by calling the
 * appropriate sub-function for each light
 *
 * Write the results into comp[C_R,G,B]
 */
void SURFACE_CALL SurfaceColourLit(br_renderer *self, br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour colour, br_scalar *comp)
{
	int i;
	struct active_light *alp;
	br_vector3 vp;
	br_vector3 vn;
	br_vector3 fvn;
	br_scalar red, green, blue;

	red = BrFixedToScalar(BR_RED(colour) << 8);
	green = BrFixedToScalar(BR_GRN(colour) << 8);
	blue = BrFixedToScalar(BR_BLU(colour) << 8);

	/*
	 * Ambient component
	 */
	if (scache.use_ambient_colour) {

		diffuse_r = BR_MUL(self->state.surface.ka, scache.ambient_red);
		diffuse_g = BR_MUL(self->state.surface.ka, scache.ambient_green);
		diffuse_b = BR_MUL(self->state.surface.ka, scache.ambient_blue);

	} else {

		diffuse_r = self->state.surface.ka;
		diffuse_g = self->state.surface.ka;
		diffuse_b = self->state.surface.ka;
	}

	specular_r = BR_SCALAR(0.0);
	specular_g = BR_SCALAR(0.0);
	specular_b = BR_SCALAR(0.0);

	/*
	 * Accumulate intensities for each active light in model space
	 */
	rend.eye_l = scache.eye_m_normalised;

	alp = scache.lights;

	if (self->state.surface.ks != BR_SCALAR(0.0)) {

		for(i=0; i < scache.nlights_model; i++, alp++)
			if (!alp->culled)
				alp->accumulate_colour_specular(self, p, n, alp);

	} else {

		for(i=0; i < scache.nlights_model; i++, alp++)
			if (!alp->culled)
				CALL_LIGHTING_FN(accumulate_colour, p, n);
	}

	/*
	 * See if any lights are to be calculated in view space
	 */
	if(scache.nlights_view) {
		/*
		 * Transform point and normal into view space ...
		 */
		BrMatrix34ApplyP(&vp, p, &self->state.matrix.model_to_view);
		BrMatrix34TApplyV(&vn, n, &scache.view_to_model);
		BrVector3Normalise(&fvn, &vn);

		rend.eye_l.v[0] = BR_SCALAR(0);
		rend.eye_l.v[1] = BR_SCALAR(0);
		rend.eye_l.v[2] = BR_SCALAR(1);

		/*
		 * ... and accumulate
		 */
		if (self->state.surface.ks != BR_SCALAR(0.0)) {

			for(i=0; i < scache.nlights_view; i++, alp++)
				if (!alp->culled)
					alp->accumulate_colour_specular(self, &vp, &fvn, alp);

		} else {

			for(i=0; i < scache.nlights_view; i++, alp++)
				if (!alp->culled)
					CALL_LIGHTING_FN(accumulate_colour, &vp, &fvn);
		}
	}

	/*
	 * Combine diffuse and specular parts and scale and clamp to final range
	 */
	CALCULATE_COMPONENT(C_R, red, BR_SCALAR(0.0), diffuse_r, specular_r);
	CALCULATE_COMPONENT(C_G, green, BR_SCALAR(0.0), diffuse_g, specular_g);
	CALCULATE_COMPONENT(C_B, blue, BR_SCALAR(0.0), diffuse_b, specular_b);
}

/*
 * Accumulate lighting for multiple active lights by calling the
 * appropriate sub-function for each light
 *
 * Write the results into comp[C_R,G,B]
 */
void SURFACE_CALL SurfaceColourLitPrelit(br_renderer *self, br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour colour, br_scalar *comp)
{
	int i;
	struct active_light *alp;
	br_vector3 vp;
	br_vector3 vn;
	br_vector3 fvn;
	br_scalar red, green, blue;

	red = BrFixedToScalar(BR_RED(colour) << 8);
	green = BrFixedToScalar(BR_GRN(colour) << 8);
	blue = BrFixedToScalar(BR_BLU(colour) << 8);

	/*
	 * Ambient component
	 */
	if (scache.use_ambient_colour) {

		diffuse_r = BR_MUL(self->state.surface.ka, scache.ambient_red);
		diffuse_g = BR_MUL(self->state.surface.ka, scache.ambient_green);
		diffuse_b = BR_MUL(self->state.surface.ka, scache.ambient_blue);

	} else {

		diffuse_r = self->state.surface.ka;
		diffuse_g = self->state.surface.ka;
		diffuse_b = self->state.surface.ka;
	}

	specular_r = BR_SCALAR(0.0);
	specular_g = BR_SCALAR(0.0);
	specular_b = BR_SCALAR(0.0);

	/*
	 * Accumulate intensities for each active light in model space
	 */
	rend.eye_l = scache.eye_m_normalised;

	alp = scache.lights;

	if (self->state.surface.ks != BR_SCALAR(0.0)) {

		for(i=0; i < scache.nlights_model; i++, alp++)
			if (!alp->culled)
				alp->accumulate_colour_specular(self, p, n, alp);

	} else {

		for(i=0; i < scache.nlights_model; i++, alp++)
			if (!alp->culled)
				CALL_LIGHTING_FN(accumulate_colour, p, n);
	}

	/*
	 * See if any lights are to be calculated in view space
	 */
	if(scache.nlights_view) {
		/*
		 * Transform point and normal into view space ...
		 */
		BrMatrix34ApplyP(&vp, p, &self->state.matrix.model_to_view);
		BrMatrix34TApplyV(&vn, n, &scache.view_to_model);
		BrVector3Normalise(&fvn, &vn);

		rend.eye_l.v[0] = BR_SCALAR(0);
		rend.eye_l.v[1] = BR_SCALAR(0);
		rend.eye_l.v[2] = BR_SCALAR(1);

		/*
		 * ... and accumulate
		 */
		if (self->state.surface.ks != BR_SCALAR(0.0)) {

			for(i=0; i < scache.nlights_view; i++, alp++)
				if (!alp->culled)
					alp->accumulate_colour_specular(self, &vp, &fvn, alp);

		} else {

			for(i=0; i < scache.nlights_view; i++, alp++)
				if (!alp->culled)
					CALL_LIGHTING_FN(accumulate_colour, &vp, &fvn);
		}
	}

	/*
	 * Combine prelit, diffuse and specular parts and scale and clamp to final range
	 */
	CALCULATE_COMPONENT(C_R, red, BR_CONST_DIV(BrIntToScalar(BR_RED(rend.prelit_colours[rend.current_index])), 256), diffuse_r, specular_r);
	CALCULATE_COMPONENT(C_G, green, BR_CONST_DIV(BrIntToScalar(BR_GRN(rend.prelit_colours[rend.current_index])), 256), diffuse_g, specular_g);
	CALCULATE_COMPONENT(C_B, blue, BR_CONST_DIV(BrIntToScalar(BR_BLU(rend.prelit_colours[rend.current_index])), 256), diffuse_b, specular_b);
}


br_boolean pointInLightVolumes(br_vector3 *p, br_light_volume *volume)
{
	br_uint_32 i, j;
	br_convex_region *region;
	br_vector4 *plane;

	for (region = volume->regions, i = 0; i < volume->nregions; i++, region++) {

		for (plane = region->planes, j = 0; j < region->nplanes; j++, plane++)
			if (BR_MAC3(p->v[0], plane->v[0], p->v[1], plane->v[1], p->v[2], plane->v[2]) < plane->v[3])
				break;

		if (j == region->nplanes)
			return BR_TRUE;
	}

	return BR_FALSE;
}

br_scalar lightVolumeAttenuation(br_vector3 *p, br_light_volume *volume)
{
	br_uint_32 i, j;
	br_convex_region *region;
	br_vector4 *plane;
	br_scalar dist, max_dist, atten_dist;
	br_boolean inside, outside;

	atten_dist = BR_SCALAR_MAX;

	for (region = volume->regions, i = 0; i < volume->nregions; i++, region++) {

		inside = BR_TRUE;
		outside = BR_FALSE;
		max_dist = BR_SCALAR(0.0);

		for (plane = region->planes, j = 0; j < region->nplanes; j++, plane++) {

			dist = -(BR_MAC3(p->v[0], plane->v[0], p->v[1], plane->v[1], p->v[2], plane->v[2]) - plane->v[3]);

			if (dist >= BR_SCALAR(0.0)) {

				inside = BR_FALSE;

				if (dist >= volume->falloff_distance)
					outside = BR_TRUE;

				else if (dist > max_dist)
					max_dist = dist;
			}
		}

		if (inside)
			return BR_SCALAR(1.0);

		if (!outside && max_dist < atten_dist)
			atten_dist = max_dist;
	}

	if (atten_dist >= volume->falloff_distance)
		return BR_SCALAR(0.0);

	return BR_SCALAR(1.0) - BR_DIV(atten_dist, volume->falloff_distance);	// ** CACHE 1/falloff **
}


/*
 * Lighting function that does nothing
 */
INLINE static void lightingColourNull(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
}

/*
 * Lighting for ambient light source in model space with lighting volume
 */
INLINE static void lightingColourAmbientVolume(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	CHECK_LIGHT_VOLUME();
	ACCUMULATE_AMBIENT_RGB(BR_MUL(self->state.surface.ka, alp->intensity));
}

INLINE static void lightingColourAmbientVolumeFalloff(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar attn;

	CALCULATE_LIGHT_VOLUME_ATTENUATION();
	ACCUMULATE_AMBIENT_RGB(BR_MUL(BR_MUL(self->state.surface.ka, attn), alp->intensity));
}

INLINE static void lightingColourAmbientRadii(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar attn,dist;
	br_vector3 dirn;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIST_CUTOFF(alp->cutoff);

	CALCULATE_ATTENUATION_RADII();

	ACCUMULATE_AMBIENT_RGB(BR_MUL(self->state.surface.ka, attn));
}

INLINE static void lightingColourAmbientRadiiVolume(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar attn,dist;
	br_vector3 dirn;

	CHECK_LIGHT_VOLUME();

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIST_CUTOFF(alp->cutoff);

	CALCULATE_ATTENUATION_RADII();

	ACCUMULATE_AMBIENT_RGB(BR_MUL(self->state.surface.ka, attn));
}

INLINE static void lightingColourAmbientRadiiVolumeFalloff(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar attn,dist;
	br_vector3 dirn;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIST_CUTOFF(alp->cutoff);

	CALCULATE_ATTENUATION_RADII();
	MULTIPLY_LIGHT_VOLUME_ATTENUATION(alp->intensity);

	ACCUMULATE_AMBIENT_RGB(BR_MUL(self->state.surface.ka, attn));
}

/*
 * Lighting for directional light source in model space
 */
INLINE static void lightingColourDirect(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot;

	DIFFUSE_DOT_DIRECT();
	ACCUMULATE_DIFFUSE_RGB(self->state.surface.kd);
}

INLINE static void lightingColourDirectSpecular(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot;

	DIFFUSE_DOT_DIRECT();
	ACCUMULATE_DIFFUSE_RGB(self->state.surface.kd);

	SPECULAR_DOT_DIRECT();
	ACCUMULATE_SPECULAR_RGB(BR_MUL(self->state.surface.ks, alp->intensity));
}

INLINE static void lightingColourDirectVolume(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot;

	DIFFUSE_DOT_DIRECT();
	CHECK_LIGHT_VOLUME();
	ACCUMULATE_DIFFUSE_RGB(self->state.surface.kd);
}

INLINE static void lightingColourDirectVolumeSpecular(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot;

	DIFFUSE_DOT_DIRECT();
	CHECK_LIGHT_VOLUME();
	ACCUMULATE_DIFFUSE_RGB(self->state.surface.kd);

	SPECULAR_DOT_DIRECT();
	ACCUMULATE_SPECULAR_RGB(BR_MUL(self->state.surface.ks, alp->intensity));
}

INLINE static void lightingColourDirectVolumeFalloff(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot,attn;

	DIFFUSE_DOT_DIRECT();

	CALCULATE_LIGHT_VOLUME_ATTENUATION();

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));
}

INLINE static void lightingColourDirectVolumeFalloffSpecular(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot,attn;

	DIFFUSE_DOT_DIRECT();

	CALCULATE_LIGHT_VOLUME_ATTENUATION();

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));

	SPECULAR_DOT_DIRECT();
	ACCUMULATE_SPECULAR_RGB(BR_MUL(BR_MUL(self->state.surface.ks, attn), alp->intensity));
}

/*
 * Lighting for point light source with attenuation
 * Colour
 */
INLINE static void lightingColourPoint(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM();

	DIFFUSE_DOT();
	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, alp->intensity));
}

INLINE static void lightingColourPointSpecular(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM();

	DIFFUSE_DOT();
	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, alp->intensity));

	SPECULAR_DOT();
	ACCUMULATE_SPECULAR_RGB(BR_MUL(self->state.surface.ks, alp->intensity));
}

INLINE static void lightingColourPointVolume(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot;
	br_vector3 dirn,dirn_norm;

	CHECK_LIGHT_VOLUME();

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM();

	DIFFUSE_DOT();
	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, alp->intensity));
}

INLINE static void lightingColourPointVolumeSpecular(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot;
	br_vector3 dirn,dirn_norm;

	CHECK_LIGHT_VOLUME();

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM();

	DIFFUSE_DOT();
	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, alp->intensity));

	SPECULAR_DOT();
	ACCUMULATE_SPECULAR_RGB(BR_MUL(self->state.surface.ks, alp->intensity));
}

INLINE static void lightingColourPointVolumeFalloff(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot,attn;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM();

	DIFFUSE_DOT();

	MULTIPLY_LIGHT_VOLUME_ATTENUATION(alp->intensity);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));
}

INLINE static void lightingColourPointVolumeFalloffSpecular(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot,attn;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM();

	DIFFUSE_DOT();

	MULTIPLY_LIGHT_VOLUME_ATTENUATION(alp->intensity);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));

	SPECULAR_DOT();
	ACCUMULATE_SPECULAR_RGB(BR_MUL(self->state.surface.ks, attn));
}

/*
 * Lighting for point light source with attenuation
 * Colour
 */
INLINE static void lightingColourPointAttn(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar attn,dot,dist;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST();

	DIFFUSE_DOT();

	CALCULATE_ATTENUATION();

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));
}

INLINE static void lightingColourPointAttnSpecular(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar attn,dot,dist;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST();

	DIFFUSE_DOT();

	CALCULATE_ATTENUATION();

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));

	SPECULAR_DOT();
	ACCUMULATE_SPECULAR_RGB(BR_MUL(self->state.surface.ks, attn));
}

INLINE static void lightingColourPointAttnVolume(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar attn,dot,dist;
	br_vector3 dirn,dirn_norm;

	CHECK_LIGHT_VOLUME();

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST();

	DIFFUSE_DOT();

	CALCULATE_ATTENUATION();

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));
}

INLINE static void lightingColourPointAttnVolumeSpecular(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar attn,dot,dist;
	br_vector3 dirn,dirn_norm;

	CHECK_LIGHT_VOLUME();

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST();

	DIFFUSE_DOT();

	CALCULATE_ATTENUATION();
	
	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));

	SPECULAR_DOT();
	ACCUMULATE_SPECULAR_RGB(BR_MUL(self->state.surface.ks, attn));
}

INLINE static void lightingColourPointAttnVolumeFalloff(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar attn,dot,dist;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST();

	DIFFUSE_DOT();

	CALCULATE_LIGHT_VOLUME_ATTENUATION();
	MULTIPLY_ATTENUATION(attn);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));
}

INLINE static void lightingColourPointAttnVolumeFalloffSpecular(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar attn,dot,dist;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST();
	
	DIFFUSE_DOT();

	CALCULATE_LIGHT_VOLUME_ATTENUATION();
	MULTIPLY_ATTENUATION(attn);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));

	SPECULAR_DOT();
	ACCUMULATE_SPECULAR_RGB(BR_MUL(self->state.surface.ks, attn));
}

/*
 * Lighting for point light source with attenuation
 * Colour
 */
INLINE static void lightingColourPointAttnLinear(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar attn,dot,dist;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST();

	DIFFUSE_DOT();

	CALCULATE_ATTENUATION_LINEAR();
	
	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));
}

INLINE static void lightingColourPointAttnLinearSpecular(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar attn,dot,dist;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST();
	
	DIFFUSE_DOT();

	CALCULATE_ATTENUATION_LINEAR();
	
	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));

	SPECULAR_DOT();
	ACCUMULATE_SPECULAR_RGB(BR_MUL(self->state.surface.ks, attn));
}

INLINE static void lightingColourPointAttnLinearVolume(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar attn,dot,dist;
	br_vector3 dirn,dirn_norm;

	CHECK_LIGHT_VOLUME();

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST();
	
	DIFFUSE_DOT();

	CALCULATE_ATTENUATION_LINEAR();
	
	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));
}

INLINE static void lightingColourPointAttnLinearVolumeSpecular(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar attn,dot,dist;
	br_vector3 dirn,dirn_norm;

	CHECK_LIGHT_VOLUME();

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST();
	
	DIFFUSE_DOT();

	CALCULATE_ATTENUATION_LINEAR();
	
	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));

	SPECULAR_DOT();
	ACCUMULATE_SPECULAR_RGB(BR_MUL(self->state.surface.ks, attn));
}

INLINE static void lightingColourPointAttnLinearVolumeFalloff(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar attn,dot,dist;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST();

	DIFFUSE_DOT();

	CALCULATE_LIGHT_VOLUME_ATTENUATION();
	MULTIPLY_ATTENUATION_LINEAR(attn);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));
}

INLINE static void lightingColourPointAttnLinearVolumeFalloffSpecular(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar attn,dot,dist;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST();

	DIFFUSE_DOT();

	CALCULATE_LIGHT_VOLUME_ATTENUATION();
	MULTIPLY_ATTENUATION_LINEAR(attn);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));

	SPECULAR_DOT();
	ACCUMULATE_SPECULAR_RGB(BR_MUL(self->state.surface.ks, attn));
}

/*
 * Lighting for point light source with linear falloff between two radii
 * Colour
 */
INLINE static void lightingColourPointRadii(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar attn,dot,dist;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST_CUTOFF(alp->cutoff);

	DIFFUSE_DOT();

	CALCULATE_ATTENUATION_RADII();

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));
}

INLINE static void lightingColourPointRadiiSpecular(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar attn,dot,dist;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST_CUTOFF(alp->cutoff);

	DIFFUSE_DOT();

	CALCULATE_ATTENUATION_RADII();

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));

	SPECULAR_DOT();
	ACCUMULATE_SPECULAR_RGB(BR_MUL(self->state.surface.ks, attn));
}

INLINE static void lightingColourPointRadiiVolume(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar attn,dot,dist;
	br_vector3 dirn,dirn_norm;

	CHECK_LIGHT_VOLUME();

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST_CUTOFF(alp->cutoff);

	DIFFUSE_DOT();

	CALCULATE_ATTENUATION_RADII();

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));
}

INLINE static void lightingColourPointRadiiVolumeSpecular(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar attn,dot,dist;
	br_vector3 dirn,dirn_norm;

	CHECK_LIGHT_VOLUME();

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST_CUTOFF(alp->cutoff);

	DIFFUSE_DOT();

	CALCULATE_ATTENUATION_RADII();

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));

	SPECULAR_DOT();
	ACCUMULATE_SPECULAR_RGB(BR_MUL(self->state.surface.ks, attn));
}

INLINE static void lightingColourPointRadiiVolumeFalloff(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar attn,dot,dist;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST_CUTOFF(alp->cutoff);

	DIFFUSE_DOT();

	CALCULATE_LIGHT_VOLUME_ATTENUATION();
	MULTIPLY_ATTENUATION_RADII(attn);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));
}

INLINE static void lightingColourPointRadiiVolumeFalloffSpecular(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar attn,dot,dist;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST_CUTOFF(alp->cutoff);

	DIFFUSE_DOT();

	CALCULATE_LIGHT_VOLUME_ATTENUATION();
	MULTIPLY_ATTENUATION_RADII(attn);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));

	SPECULAR_DOT();
	ACCUMULATE_SPECULAR_RGB(BR_MUL(self->state.surface.ks, attn));
}

/*
 * Lighting for spot light source
 * Colour
 */
INLINE static void lightingColourSpot(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot_spot, dot,  attn;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM();

	SPOT_DOT();

	DIFFUSE_DOT();

	MULTIPLY_SPOT_ATTENUATION(alp->intensity);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));
}

INLINE static void lightingColourSpotSpecular(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot_spot, dot,  attn;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM();

	SPOT_DOT();

	DIFFUSE_DOT();

	MULTIPLY_SPOT_ATTENUATION(alp->intensity);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));

	SPECULAR_DOT();
	ACCUMULATE_SPECULAR_RGB(BR_MUL(self->state.surface.ks, attn));
}

INLINE static void lightingColourSpotVolume(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot_spot, dot,  attn;
	br_vector3 dirn,dirn_norm;

	CHECK_LIGHT_VOLUME();

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM();

	SPOT_DOT();

	DIFFUSE_DOT();

	MULTIPLY_SPOT_ATTENUATION(alp->intensity);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));
}

INLINE static void lightingColourSpotVolumeSpecular(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot_spot, dot,  attn;
	br_vector3 dirn,dirn_norm;

	CHECK_LIGHT_VOLUME();

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM();

	SPOT_DOT();

	DIFFUSE_DOT();

	MULTIPLY_SPOT_ATTENUATION(alp->intensity);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));

	SPECULAR_DOT();
	ACCUMULATE_SPECULAR_RGB(BR_MUL(self->state.surface.ks, attn));
}

INLINE static void lightingColourSpotVolumeFalloff(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot_spot, dot,  attn;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM();

	SPOT_DOT();

	DIFFUSE_DOT();

	MULTIPLY_LIGHT_VOLUME_ATTENUATION(alp->intensity);
	MULTIPLY_SPOT_ATTENUATION(attn);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(attn, self->state.surface.kd));
}

INLINE static void lightingColourSpotVolumeFalloffSpecular(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot_spot, dot,  attn;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM();

	SPOT_DOT();

	DIFFUSE_DOT();

	MULTIPLY_LIGHT_VOLUME_ATTENUATION(alp->intensity);
	MULTIPLY_SPOT_ATTENUATION(attn);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));

	SPECULAR_DOT();
	ACCUMULATE_SPECULAR_RGB(BR_MUL(self->state.surface.ks, attn));
}

/*
 * Lighting for spot light source with attenuation
 * Colour
 */
INLINE static void lightingColourSpotAttn(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot,dot_spot,dist,attn;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST();

	SPOT_DOT();

	DIFFUSE_DOT();

	CALCULATE_ATTENUATION();
	MULTIPLY_SPOT_ATTENUATION(attn);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));
}

INLINE static void lightingColourSpotAttnSpecular(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot,dot_spot,dist,attn;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST();

	SPOT_DOT();

	DIFFUSE_DOT();

	CALCULATE_ATTENUATION();
	MULTIPLY_SPOT_ATTENUATION(attn);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));

	SPECULAR_DOT();
	ACCUMULATE_SPECULAR_RGB(BR_MUL(self->state.surface.ks, attn));
}

INLINE static void lightingColourSpotAttnVolume(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot,dot_spot,dist,attn;
	br_vector3 dirn,dirn_norm;

	CHECK_LIGHT_VOLUME();

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST();

	SPOT_DOT();

	DIFFUSE_DOT();

	CALCULATE_ATTENUATION();
	MULTIPLY_SPOT_ATTENUATION(attn);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));
}

INLINE static void lightingColourSpotAttnVolumeSpecular(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot,dot_spot,dist,attn;
	br_vector3 dirn,dirn_norm;

	CHECK_LIGHT_VOLUME();

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST();

	SPOT_DOT();

	DIFFUSE_DOT();

	CALCULATE_ATTENUATION();
	MULTIPLY_SPOT_ATTENUATION(attn);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));

	SPECULAR_DOT();
	ACCUMULATE_SPECULAR_RGB(BR_MUL(self->state.surface.ks, attn));
}

INLINE static void lightingColourSpotAttnVolumeFalloff(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot,dot_spot,dist,attn;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST();

	SPOT_DOT();

	DIFFUSE_DOT();

	CALCULATE_LIGHT_VOLUME_ATTENUATION();
	MULTIPLY_ATTENUATION(attn);
	MULTIPLY_SPOT_ATTENUATION(attn);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));
}

INLINE static void lightingColourSpotAttnVolumeFalloffSpecular(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot,dot_spot,dist,attn;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST();

	SPOT_DOT();

	DIFFUSE_DOT();

	CALCULATE_LIGHT_VOLUME_ATTENUATION();
	MULTIPLY_ATTENUATION(attn);
	MULTIPLY_SPOT_ATTENUATION(attn);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));

	SPECULAR_DOT();
	ACCUMULATE_SPECULAR_RGB(BR_MUL(self->state.surface.ks, attn));
}

INLINE static void lightingColourSpotAttnLinear(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot,dot_spot,dist,attn;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST();

	SPOT_DOT();

	DIFFUSE_DOT();

	CALCULATE_ATTENUATION_LINEAR();
	MULTIPLY_SPOT_ATTENUATION(attn);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));
}

INLINE static void lightingColourSpotAttnLinearSpecular(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot,dot_spot,dist,attn;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST();

	SPOT_DOT();

	DIFFUSE_DOT();

	CALCULATE_ATTENUATION_LINEAR();
	MULTIPLY_SPOT_ATTENUATION(attn);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));

	SPECULAR_DOT();
	ACCUMULATE_SPECULAR_RGB(BR_MUL(self->state.surface.ks, attn));
}

INLINE static void lightingColourSpotAttnLinearVolume(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot,dot_spot,dist,attn;
	br_vector3 dirn,dirn_norm;

	CHECK_LIGHT_VOLUME();

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST();

	SPOT_DOT();

	DIFFUSE_DOT();

	CALCULATE_ATTENUATION_LINEAR();
	MULTIPLY_SPOT_ATTENUATION(attn);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));
}

INLINE static void lightingColourSpotAttnLinearVolumeSpecular(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot,dot_spot,dist,attn;
	br_vector3 dirn,dirn_norm;

	CHECK_LIGHT_VOLUME();

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST();

	SPOT_DOT();

	DIFFUSE_DOT();

	CALCULATE_ATTENUATION_LINEAR();
	MULTIPLY_SPOT_ATTENUATION(attn);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));

	SPECULAR_DOT();
	ACCUMULATE_SPECULAR_RGB(BR_MUL(self->state.surface.ks, attn));
}

INLINE static void lightingColourSpotAttnLinearVolumeFalloff(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot,dot_spot,dist,attn;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST();

	SPOT_DOT();

	DIFFUSE_DOT();

	CALCULATE_LIGHT_VOLUME_ATTENUATION();
	MULTIPLY_ATTENUATION_LINEAR(attn);
	MULTIPLY_SPOT_ATTENUATION(attn);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));
}

INLINE static void lightingColourSpotAttnLinearVolumeFalloffSpecular(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot,dot_spot,dist,attn;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST();

	SPOT_DOT();

	DIFFUSE_DOT();

	CALCULATE_LIGHT_VOLUME_ATTENUATION();
	MULTIPLY_ATTENUATION_LINEAR(attn);
	MULTIPLY_SPOT_ATTENUATION(attn);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));

	SPECULAR_DOT();
	ACCUMULATE_SPECULAR_RGB(BR_MUL(self->state.surface.ks, attn));
}

INLINE static void lightingColourSpotRadii(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot,dot_spot,dist,attn;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST_CUTOFF(alp->cutoff);

	SPOT_DOT();

	DIFFUSE_DOT();

	CALCULATE_ATTENUATION_RADII();
	MULTIPLY_SPOT_ATTENUATION(attn);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));
}

INLINE static void lightingColourSpotRadiiSpecular(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot,dot_spot,dist,attn;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST_CUTOFF(alp->cutoff);

	SPOT_DOT();

	DIFFUSE_DOT();

	CALCULATE_ATTENUATION_RADII();
	MULTIPLY_SPOT_ATTENUATION(attn);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));

	SPECULAR_DOT();
	ACCUMULATE_SPECULAR_RGB(BR_MUL(self->state.surface.ks, attn));
}

INLINE static void lightingColourSpotRadiiVolume(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot,dot_spot,dist,attn;
	br_vector3 dirn,dirn_norm;

	CHECK_LIGHT_VOLUME();

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST_CUTOFF(alp->cutoff);

	SPOT_DOT();

	DIFFUSE_DOT();

	CALCULATE_ATTENUATION_RADII();
	MULTIPLY_SPOT_ATTENUATION(attn);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));
}

INLINE static void lightingColourSpotRadiiVolumeSpecular(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot,dot_spot,dist,attn;
	br_vector3 dirn,dirn_norm;

	CHECK_LIGHT_VOLUME();

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST_CUTOFF(alp->cutoff);

	SPOT_DOT();

	DIFFUSE_DOT();

	CALCULATE_ATTENUATION_RADII();
	MULTIPLY_SPOT_ATTENUATION(attn);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));

	SPECULAR_DOT();
	ACCUMULATE_SPECULAR_RGB(BR_MUL(self->state.surface.ks, attn));
}

INLINE static void lightingColourSpotRadiiVolumeFalloff(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot,dot_spot,dist,attn;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST_CUTOFF(alp->cutoff);

	SPOT_DOT();

	DIFFUSE_DOT();

	CALCULATE_LIGHT_VOLUME_ATTENUATION();
	MULTIPLY_ATTENUATION_RADII(attn);
	MULTIPLY_SPOT_ATTENUATION(attn);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));
}

INLINE static void lightingColourSpotRadiiVolumeFalloffSpecular(br_renderer *self,
	br_vector3 *p, br_vector3 *n,
	struct active_light *alp)
{
	br_scalar dot,dot_spot,dist,attn;
	br_vector3 dirn,dirn_norm;

	CALCULATE_LIGHT_DIRN();
	CALCULATE_DIRN_NORM_DIST_CUTOFF(alp->cutoff);

	SPOT_DOT();

	DIFFUSE_DOT();

	CALCULATE_LIGHT_VOLUME_ATTENUATION();
	MULTIPLY_ATTENUATION_RADII(attn);
	MULTIPLY_SPOT_ATTENUATION(attn);

	ACCUMULATE_DIFFUSE_RGB(BR_MUL(self->state.surface.kd, attn));

	SPECULAR_DOT();
	ACCUMULATE_SPECULAR_RGB(BR_MUL(self->state.surface.ks, attn));
}

/*
 * Select a per-light component accumulation function
 */
void ActiveLightAccumulateColourSet(struct active_light *alp)
{
	switch (alp->s->attenuation_type) {

	case BRT_QUADRATIC:

		switch(alp->type) {

		case BRT_AMBIENT:
				if (alp->volume.regions == NULL) {
					alp->accumulate_colour = lightingColourNull;
					alp->accumulate_colour_specular = lightingColourNull;
				} else if (alp->volume.falloff_distance == BR_SCALAR(0.0)) {
					alp->accumulate_colour = lightingColourAmbientVolume;
					alp->accumulate_colour_specular = lightingColourAmbientVolume;
				} else {
					alp->accumulate_colour = lightingColourAmbientVolumeFalloff;
					alp->accumulate_colour_specular = lightingColourAmbientVolumeFalloff;
				}
			break;

		case BRT_DIRECT:
				if (alp->volume.regions == NULL) {
					alp->accumulate_colour = lightingColourDirect;
					alp->accumulate_colour_specular = lightingColourDirectSpecular;
				} else if (alp->volume.falloff_distance == BR_SCALAR(0.0)) {
					alp->accumulate_colour = lightingColourDirectVolume;
					alp->accumulate_colour_specular = lightingColourDirectVolumeSpecular;
				} else {
					alp->accumulate_colour = lightingColourDirectVolumeFalloff;
					alp->accumulate_colour_specular = lightingColourDirectVolumeFalloffSpecular;
				}
			break;

		case BRT_POINT:

			switch (alp->s->attenuation_hint) {

			case BRT_CONSTANT:
				if (alp->volume.regions == NULL) {
					alp->accumulate_colour = lightingColourPoint;
					alp->accumulate_colour_specular = lightingColourPointSpecular;
				} else if (alp->volume.falloff_distance == BR_SCALAR(0.0)) {
					alp->accumulate_colour = lightingColourPointVolume;
					alp->accumulate_colour_specular = lightingColourPointVolumeSpecular;
				} else {
					alp->accumulate_colour = lightingColourPointVolumeFalloff;
					alp->accumulate_colour_specular = lightingColourPointVolumeFalloffSpecular;
				}
				break;

			case BRT_LINEAR:
				if (alp->volume.regions == NULL) {
					alp->accumulate_colour = lightingColourPointAttnLinear;
					alp->accumulate_colour_specular = lightingColourPointAttnLinearSpecular;
				} else if (alp->volume.falloff_distance == BR_SCALAR(0.0)) {
					alp->accumulate_colour = lightingColourPointAttnLinearVolume;
					alp->accumulate_colour_specular = lightingColourPointAttnLinearVolumeSpecular;
				} else {
					alp->accumulate_colour = lightingColourPointAttnLinearVolumeFalloff;
					alp->accumulate_colour_specular = lightingColourPointAttnLinearVolumeFalloffSpecular;
				}
				break;

			case BRT_QUADRATIC:
			default:
				if (alp->volume.regions == NULL) {
					alp->accumulate_colour = lightingColourPointAttn;
					alp->accumulate_colour_specular = lightingColourPointAttnSpecular;
				} else if (alp->volume.falloff_distance == BR_SCALAR(0.0)) {
					alp->accumulate_colour = lightingColourPointAttnVolume;
					alp->accumulate_colour_specular = lightingColourPointAttnVolumeSpecular;
				} else {
					alp->accumulate_colour = lightingColourPointAttnVolumeFalloff;
					alp->accumulate_colour_specular = lightingColourPointAttnVolumeFalloffSpecular;
				}
				break;
			}

			break;

		case BRT_SPOT:

			switch (alp->s->attenuation_hint) {

			case BRT_CONSTANT:
				if (alp->volume.regions == NULL) {
					alp->accumulate_colour = lightingColourSpot;
					alp->accumulate_colour_specular = lightingColourSpotSpecular;
				} else if (alp->volume.falloff_distance == BR_SCALAR(0.0)) {
					alp->accumulate_colour = lightingColourSpotVolume;
					alp->accumulate_colour_specular = lightingColourSpotVolumeSpecular;
				} else {
					alp->accumulate_colour = lightingColourSpotVolumeFalloff;
					alp->accumulate_colour_specular = lightingColourSpotVolumeFalloffSpecular;
				}
				break;

			case BRT_LINEAR:
				if (alp->volume.regions == NULL) {
					alp->accumulate_colour = lightingColourSpotAttnLinear;
					alp->accumulate_colour_specular = lightingColourSpotAttnLinearSpecular;
				} else if (alp->volume.falloff_distance == BR_SCALAR(0.0)) {
					alp->accumulate_colour = lightingColourSpotAttnLinearVolume;
					alp->accumulate_colour_specular = lightingColourSpotAttnLinearVolumeSpecular;
				} else {
					alp->accumulate_colour = lightingColourSpotAttnLinearVolumeFalloff;
					alp->accumulate_colour_specular = lightingColourSpotAttnLinearVolumeFalloffSpecular;
				}
				break;

			case BRT_QUADRATIC:
			default:
				if (alp->volume.regions == NULL) {
					alp->accumulate_colour = lightingColourSpotAttn;
					alp->accumulate_colour_specular = lightingColourSpotAttnSpecular;
				} else if (alp->volume.falloff_distance == BR_SCALAR(0.0)) {
					alp->accumulate_colour = lightingColourSpotAttnVolume;
					alp->accumulate_colour_specular = lightingColourSpotAttnVolumeSpecular;
				} else {
					alp->accumulate_colour = lightingColourSpotAttnVolumeFalloff;
					alp->accumulate_colour_specular = lightingColourSpotAttnVolumeFalloffSpecular;
				}
				break;
			}

			break;

		default:
			alp->accumulate_colour = lightingColourNull;
			alp->accumulate_colour_specular = lightingColourNull;
	 	}
	
		break;

	case BRT_RADII:

		switch(alp->type) {

		case BRT_AMBIENT:
				if (alp->volume.regions == NULL) {
					alp->accumulate_colour = lightingColourAmbientRadii;
					alp->accumulate_colour_specular = lightingColourAmbientRadii;
				} else if (alp->volume.falloff_distance == BR_SCALAR(0.0)) {
					alp->accumulate_colour = lightingColourAmbientRadiiVolume;
					alp->accumulate_colour_specular = lightingColourAmbientRadiiVolume;
				} else {
					alp->accumulate_colour = lightingColourAmbientRadiiVolumeFalloff;
					alp->accumulate_colour_specular = lightingColourAmbientRadiiVolumeFalloff;
				}
			break;

		case BRT_DIRECT:
			if (alp->volume.regions == NULL) {
				alp->accumulate_colour = lightingColourDirect;
				alp->accumulate_colour_specular = lightingColourDirectSpecular;
			} else if (alp->volume.falloff_distance == BR_SCALAR(0.0)) {
				alp->accumulate_colour = lightingColourDirectVolume;
				alp->accumulate_colour_specular = lightingColourDirectVolumeSpecular;
			} else {
				alp->accumulate_colour = lightingColourDirectVolumeFalloff;
				alp->accumulate_colour_specular = lightingColourDirectVolumeFalloffSpecular;
			}
			break;

		case BRT_POINT:
			if (alp->volume.regions == NULL) {
				alp->accumulate_colour = lightingColourPointRadii;
				alp->accumulate_colour_specular = lightingColourPointRadiiSpecular;
			} else if (alp->volume.falloff_distance == BR_SCALAR(0.0)) {
				alp->accumulate_colour = lightingColourPointRadiiVolume;
				alp->accumulate_colour_specular = lightingColourPointRadiiVolumeSpecular;
			} else {
				alp->accumulate_colour = lightingColourPointRadiiVolumeFalloff;
				alp->accumulate_colour_specular = lightingColourPointRadiiVolumeFalloffSpecular;
			}
			break;

		case BRT_SPOT:
			if (alp->volume.regions == NULL) {
				alp->accumulate_colour = lightingColourSpotRadii;
				alp->accumulate_colour_specular = lightingColourSpotRadiiSpecular;
			} else if (alp->volume.falloff_distance == BR_SCALAR(0.0)) {
				alp->accumulate_colour = lightingColourSpotRadiiVolume;
				alp->accumulate_colour_specular = lightingColourSpotRadiiVolumeSpecular;
			} else {
				alp->accumulate_colour = lightingColourSpotRadiiVolumeFalloff;
				alp->accumulate_colour_specular = lightingColourSpotRadiiVolumeFalloffSpecular;
			}
			break;

		default:
			alp->accumulate_colour = lightingColourNull;
			alp->accumulate_colour_specular = lightingColourNull;
	 	}

		break;

	default:

		alp->accumulate_colour = lightingColourNull;
		alp->accumulate_colour_specular = lightingColourNull;
		break;
	}
}

