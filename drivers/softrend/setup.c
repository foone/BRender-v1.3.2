/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: setup.c 1.9 1998/11/13 16:23:37 jon Exp $
 * $Locker: $
 *
 */
#include <stddef.h>
#include <string.h>

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

#include "vecifns.h"

BR_RCS_ID("$Id: setup.c 1.9 1998/11/13 16:23:37 jon Exp $");

#define COUNTERS 0

#if COUNTERS
int CounterCache;
int CounterPerScene;
int CounterPerModel;
int CounterViewToModel;
int CounterModelToScreen;
#endif
/*
 * Global scratch block for renderer
 */
_rend BR_ASM_DATA rend;

/*
 * Global static cache for renderer
 */
_static_cache BR_ASM_DATA scache;

/*
 * Find centre of projection in model space
 */
static void eyeInModel(br_renderer *self)
{
	br_matrix4 s2m;

	/*
	 * Spot special, easy, cases
	 */
	if(self->state.matrix.model_to_view_hint == BRT_LENGTH_PRESERVING) {
		if(self->state.matrix.view_to_screen_hint == BRT_PERSPECTIVE) {

			scache.eye_m.v[0] = -BR_MAC3(
				self->state.matrix.model_to_view.m[3][0],self->state.matrix.model_to_view.m[0][0],
				self->state.matrix.model_to_view.m[3][1],self->state.matrix.model_to_view.m[0][1],
				self->state.matrix.model_to_view.m[3][Z],self->state.matrix.model_to_view.m[0][2]);
			scache.eye_m.v[1] = -BR_MAC3(
				self->state.matrix.model_to_view.m[3][0],self->state.matrix.model_to_view.m[1][0],
				self->state.matrix.model_to_view.m[3][1],self->state.matrix.model_to_view.m[1][1],
				self->state.matrix.model_to_view.m[3][Z],self->state.matrix.model_to_view.m[1][2]);
			scache.eye_m.v[2] = -BR_MAC3(
				self->state.matrix.model_to_view.m[3][0],self->state.matrix.model_to_view.m[2][0],
				self->state.matrix.model_to_view.m[3][1],self->state.matrix.model_to_view.m[2][1],
				self->state.matrix.model_to_view.m[3][Z],self->state.matrix.model_to_view.m[2][2]);

			scache.eye_m.v[3] = BR_SCALAR(1.0);

			return;
		}

		if(self->state.matrix.view_to_screen_hint == BRT_PARALLEL) {
			BrVector3CopyMat34Col((br_vector3 *)&scache.eye_m, &self->state.matrix.model_to_view,2);
			scache.eye_m.v[3] = BR_SCALAR(0.0);
			return;
		}

	} else {
		if(self->state.matrix.view_to_screen_hint == BRT_PERSPECTIVE) {
			BrVector3CopyMat34Row((br_vector3 *)&scache.eye_m, &scache.view_to_model,3);
			scache.eye_m.v[3] = BR_SCALAR(1.0);
			return;
	 	}

		if(self->state.matrix.view_to_screen_hint == BRT_PARALLEL) {
			BrVector3CopyMat34Row((br_vector3 *)&scache.eye_m, &scache.view_to_model,2);
			scache.eye_m.v[3] = BR_SCALAR(0.0);
			return;
		}
	}

	/*
	 * If reached here, then we need to invert model_to_screen
	 */
	BrMatrix4Inverse(&s2m, &scache.model_to_screen);
	
	scache.eye_m.v[0] = s2m.m[Z][0];
	scache.eye_m.v[1] = s2m.m[Z][1];
	scache.eye_m.v[2] = s2m.m[Z][2];
	scache.eye_m.v[3] = s2m.m[Z][3];
}


void StaticCacheUpdate_PerScene(br_renderer *self)
{
	int i;

#if COUNTERS
	CounterPerScene++;
#endif

	/*
	 * Check that there is a valid heap for diverting primitives
	 */
	scache.valid_primitive_heap = self->state.hidden.heap != NULL &&
		self->state.hidden.heap->base != NULL &&
		self->state.hidden.heap->size != 0;

	/*
	 * See if any user-defined clip planes are active
	 */
	scache.user_clip_active = BR_FALSE;

	for(i = 0; i < MAX_STATE_CLIP_PLANES; i++)	
		if(self->state.clip[i].type == BRT_PLANE)
			scache.user_clip_active = BR_TRUE;

	/*
	 * Lighting setup
	 */
	ActiveLightsFind(self);
}

void StaticCacheUpdate_PerModel(br_renderer *self)
{
#if COUNTERS
	CounterPerModel++;
#endif

	if(!scache.valid_v2m) {
		ViewToModelUpdate(self);
		scache.valid_v2m = BR_TRUE;
	}

	if(!scache.valid_m2s) {
		ModelToScreenUpdate(self);
		scache.valid_m2s = BR_TRUE;
	}

	eyeInModel(self);
	BrVector3Normalise(&scache.eye_m_normalised, &scache.eye_m);

	if(self->state.matrix.view_to_environment_hint != BRT_DONT_CARE) {
		BrMatrix34Mul(&scache.model_to_environment,
			&self->state.matrix.model_to_view,
			&self->state.matrix.view_to_environment);
	}

	/*
	 * Lighting setup
	 */
	ActiveLightsUpdate(self);

}

br_error CacheUpdate(br_renderer *self)
{
#if COUNTERS
	CounterCache++;
#endif

	/*
	 * Fast exit if nothing has changed
	 */
	if(!rend.block_changed && !rend.range_changed && self->state.cache.valid)
		return BRE_OK;

	/*
	 * Work out what components should be clipped based on what
	 * the rasteriser needs per vertex - SX,SY,SZ are mapped to X,Y,Z
	 * and W is always set
	 */
	self->state.cache.clip_slots = ComponentMaskToSlots(CM_W | CM_Z | COMP_S2V(rend.block->vertex_components));

	PrimitiveStateRangesQuery(self->state.pstate,
		self->state.cache.comp_offsets, self->state.cache.comp_scales,
		NUM_COMPONENTS);
	
	/*
	 * Merge surface map_transform with ranges
	 */
	self->state.cache.map_transform.m[0][0] = BR_MUL(self->state.surface.map_transform.m[0][0],
													 self->state.cache.comp_scales[C_U]);

	self->state.cache.map_transform.m[0][1] = BR_MUL(self->state.surface.map_transform.m[0][1],
													 self->state.cache.comp_scales[C_V]);

	self->state.cache.map_transform.m[1][0] = BR_MUL(self->state.surface.map_transform.m[1][0],
													 self->state.cache.comp_scales[C_U]);

	self->state.cache.map_transform.m[1][1] = BR_MUL(self->state.surface.map_transform.m[1][1],
													 self->state.cache.comp_scales[C_V]);

	self->state.cache.map_transform.m[2][0] = BR_MUL(self->state.surface.map_transform.m[2][0],
													 self->state.cache.comp_scales[C_U]) +
													self->state.cache.comp_offsets[C_U];

	self->state.cache.map_transform.m[2][1] = BR_MUL(self->state.surface.map_transform.m[2][1],
													 self->state.cache.comp_scales[C_V]) +
													self->state.cache.comp_offsets[C_V];

	/*
	 * Cache transformed corners of unit quad, if quad mapping
	 *
	 * N.B. Actually use 0.0 .. 1.0-epsilon to avoid wrapping
	 */
	if (self->state.surface.mapping_source == BRT_QUAD) {

		self->state.cache.quad_transformed[0].v[0] = 
			self->state.cache.map_transform.m[2][0];

		self->state.cache.quad_transformed[0].v[1] = 
			self->state.cache.map_transform.m[2][1];

		self->state.cache.quad_transformed[1].v[0] =
			BR_ADD(BR_MUL(rend.renderer->state.cache.map_transform.m[1][0],
			              BR_SCALAR(1.0)-BR_SCALAR_EPSILON),
			       rend.renderer->state.cache.map_transform.m[2][0]);

		self->state.cache.quad_transformed[1].v[1] =
			BR_ADD(BR_MUL(rend.renderer->state.cache.map_transform.m[1][1],
			              BR_SCALAR(1.0)-BR_SCALAR_EPSILON),
		           rend.renderer->state.cache.map_transform.m[2][1]);

		self->state.cache.quad_transformed[2].v[0] =
			BR_ADD(BR_MUL(BR_ADD(rend.renderer->state.cache.map_transform.m[0][0],
			                     rend.renderer->state.cache.map_transform.m[1][0]),
			              BR_SCALAR(1.0)-BR_SCALAR_EPSILON),
		           rend.renderer->state.cache.map_transform.m[2][0]);

		self->state.cache.quad_transformed[2].v[1] =
			BR_ADD(BR_MUL(BR_ADD(rend.renderer->state.cache.map_transform.m[0][1],
			                     rend.renderer->state.cache.map_transform.m[1][1]),
			              BR_SCALAR(1.0)-BR_SCALAR_EPSILON),
		           rend.renderer->state.cache.map_transform.m[2][1]);

		self->state.cache.quad_transformed[3].v[0] =
			BR_ADD(BR_MUL(rend.renderer->state.cache.map_transform.m[0][0],
			              BR_SCALAR(1.0)-BR_SCALAR_EPSILON),
		           rend.renderer->state.cache.map_transform.m[2][0]);

		self->state.cache.quad_transformed[3].v[1] =
			BR_ADD(BR_MUL(rend.renderer->state.cache.map_transform.m[0][1],
			              BR_SCALAR(1.0)-BR_SCALAR_EPSILON),
		           rend.renderer->state.cache.map_transform.m[2][1]);
	}

	/*
	 * Surface component functions
	 */
	self->state.cache.nvertex_fns = GenerateSurfaceFunctions(self,
		self->state.cache.vertex_fns, rend.block->vertex_components);

	self->state.cache.nconstant_fns = GenerateSurfaceFunctions(self,
		self->state.cache.constant_fns, rend.block->constant_components);

	return BRE_OK;
}


br_error CacheClear(void)
{
	int l;
	struct active_light *alp;

	for (l = 0, alp = scache.lights; l < scache.nlights_model; l++, alp++) {

		if (alp->volume.regions != NULL) {
			BrResFree(alp->volume.regions);
			alp->volume.regions = NULL;
		}
	}

	return BRE_OK;
}


void ViewToModelUpdate(br_renderer *self)
{
#if COUNTERS
	CounterViewToModel++;
#endif
	BrMatrix34Inverse(&scache.view_to_model, &self->state.matrix.model_to_view);
}

void ModelToScreenUpdate(br_renderer *self)
{
#if COUNTERS
	CounterModelToScreen++;
#endif
	/*
	 * Model to screen matrix
	 */
	BrMatrix4Mul34(&scache.model_to_screen,
		&self->state.matrix.model_to_view,
		&self->state.matrix.view_to_screen);
}


void ActiveLightsFind(br_renderer *self)
{
	int l;
	struct state_light *lp;
	struct active_light *alp;
#if 0
	br_boolean found_ambient;
	br_scalar intensity;
#endif
	br_uint_32 i;

	/*
	 * Clear out previously cached data
	 */
	for (l = 0, alp = scache.lights; l < scache.nlights_model; l++, alp++) {

		if (alp->volume.regions != NULL) {
			BrResFree(alp->volume.regions);
			alp->volume.regions = NULL;
		}
	}

	scache.nlights_model = 0;
	scache.nlights_view = 0;

#if 0
	/*
	 * Find all ambient lights and sum them, applying constant attenuation
	 * only.  If none is found, use white.
	 */
	found_ambient = BR_FALSE;

	scache.ambient_red = BR_SCALAR(0.0);
	scache.ambient_green = BR_SCALAR(0.0);
	scache.ambient_blue = BR_SCALAR(0.0);
	scache.ambient_intensity = BR_SCALAR(0.0);

	for(l=0, lp = self->state.light; l < MAX_STATE_LIGHTS; l++, lp++) {
		if (lp->type != BRT_AMBIENT || lp->attenuation_type == BRT_RADII)
			continue;

		found_ambient = BR_TRUE;

		intensity = BR_RCP(lp->attenuation_c);

		scache.ambient_red += BR_MUL(BrFixedToScalar(BR_RED(lp->colour) << 8), intensity);
		scache.ambient_green += BR_MUL(BrFixedToScalar(BR_GRN(lp->colour) << 8), intensity);
		scache.ambient_blue += BR_MUL(BrFixedToScalar(BR_BLU(lp->colour) << 8), intensity);
		scache.ambient_intensity += intensity;
	}

	if (found_ambient) {

		if (scache.ambient_red > BR_SCALAR(1.0))
			scache.ambient_red = BR_SCALAR(1.0);

		if (scache.ambient_green > BR_SCALAR(1.0))
			scache.ambient_green = BR_SCALAR(1.0);

		if (scache.ambient_blue > BR_SCALAR(1.0))
			scache.ambient_blue = BR_SCALAR(1.0);

		scache.use_ambient_colour = 
			scache.ambient_red != BR_SCALAR(1.0) ||
			scache.ambient_green != BR_SCALAR(1.0) ||
			scache.ambient_blue != BR_SCALAR(1.0);

		if (scache.ambient_intensity > BR_SCALAR(1.0))
			scache.ambient_intensity = BR_SCALAR(1.0);

		scache.use_ambient_intensity = scache.ambient_intensity != BR_SCALAR(1.0);

	} else {

		scache.use_ambient_colour = BR_FALSE;
		scache.use_ambient_intensity = BR_FALSE;
	}
#endif

	alp = scache.lights;
	
	/*
	 * Find all model space lights
	 */
	for(l=0, lp = self->state.light; l < MAX_STATE_LIGHTS; l++, lp++) {

		if (lp->type == BRT_NONE || lp->lighting_space != BRT_MODEL ||
			lp->type == BRT_AMBIENT && lp->attenuation_type != BRT_RADII)
			continue;

		alp->s = lp;
		alp->intensity = BR_RCP(lp->attenuation_c);
		alp->type = lp->type;

		alp->red = BrFixedToScalar(BR_RED(lp->colour) << 8);
		alp->green = BrFixedToScalar(BR_GRN(lp->colour) << 8);
		alp->blue = BrFixedToScalar(BR_BLU(lp->colour) << 8);

		if (lp->attenuation_type == BRT_RADII) {

			if (lp->radius_inner != lp->radius_outer)
				alp->falloff = BR_DIV(alp->intensity, lp->radius_inner - lp->radius_outer);

			alp->cutoff = BR_SQR(lp->radius_outer);
		}

		if (lp->type == BRT_SPOT)
			alp->spot_falloff = BR_RCP(lp->spot_outer - lp->spot_inner);

		if (lp->volume.regions != NULL) {

			alp->volume.falloff_distance = lp->volume.falloff_distance;
			alp->volume.regions = BrResAllocate(scache.res, sizeof(*alp->volume.regions) * lp->volume.nregions, BR_MEMORY_OBJECT_DATA);
			alp->volume.nregions = lp->volume.nregions;

			if (alp->volume.regions != NULL)

				for (i = 0; i < lp->volume.nregions; i++) {

					alp->volume.regions[i].planes = BrResAllocate(alp->volume.regions, sizeof(*alp->volume.regions[i].planes) * lp->volume.regions[i].nplanes, BR_MEMORY_OBJECT_DATA);
					alp->volume.regions[i].nplanes = lp->volume.regions[i].nplanes;

					if (alp->volume.regions[i].planes == NULL) {
						BrResFree(alp->volume.regions);
						alp->volume.regions = NULL;
						break;
					}
				}
		}

		ActiveLightAccumulateIndexSet(alp);
		ActiveLightAccumulateColourSet(alp);

		alp++;
		scache.nlights_model++;

	}

	/*
	 * Find all view space lights
	 */
	for(l=0, lp = self->state.light; l < MAX_STATE_LIGHTS; l++, lp++) {

		if (lp->type == BRT_NONE || lp->lighting_space != BRT_VIEW ||
			lp->type == BRT_AMBIENT && lp->attenuation_type != BRT_RADII)
			continue;

		alp->s = lp;
		alp->intensity = BR_RCP(lp->attenuation_c);
		alp->type = lp->type;
		alp->position = lp->position;

		alp->red = BrFixedToScalar(BR_RED(lp->colour) << 8);
		alp->green = BrFixedToScalar(BR_GRN(lp->colour) << 8);
		alp->blue = BrFixedToScalar(BR_BLU(lp->colour) << 8);

		if(lp->type == BRT_DIRECT)
			BrVector3Scale(&alp->direction,&lp->direction,alp->intensity);
		else
			alp->direction = lp->direction;

		if (lp->attenuation_type == BRT_RADII) {

			if (lp->radius_inner != lp->radius_outer)
				alp->falloff = BR_DIV(alp->intensity, lp->radius_inner - lp->radius_outer);

			alp->cutoff = BR_SQR(lp->radius_outer);
		}

		if (lp->type == BRT_SPOT)
			alp->spot_falloff = BR_RCP(lp->spot_outer - lp->spot_inner);

		alp->volume.falloff_distance = lp->volume.falloff_distance;
		alp->volume.regions = lp->volume.regions;
		alp->volume.nregions = lp->volume.nregions;

		ActiveLightAccumulateIndexSet(alp);
		ActiveLightAccumulateColourSet(alp);

		alp++;
		scache.nlights_view++;
	}

	/*
	 * Set a flag to indicate special case of 1 directional model light
	 */
	scache.light_1md = (scache.nlights_view == 0) &&
			(scache.nlights_model == 1) &&
			(scache.lights[0].type == BRT_DIRECT);

}

static br_boolean sphereIntersectsCone(br_scalar sphere_radius, br_vector3 *cone_apex,
	br_vector3 *cone_axis, br_angle cone_angle);

void ActiveLightsUpdate(br_renderer *self)
{
	struct active_light *alp = scache.lights;
	int l;
	br_vector3 light_pos;
	br_scalar model_radius;
	br_convex_region *region, *new_region;
	br_vector4 *plane, *new_plane;
#if 1
	struct state_light *lp;
	br_boolean found_ambient;
	br_scalar intensity;
#endif
	br_uint_32 i, j;

#if 1
	/*
	 * Find all ambient lights and sum them, applying constant attenuation
	 * only.  If none is found, use white.
	 */
	found_ambient = BR_FALSE;

	scache.ambient_red = BR_SCALAR(0.0);
	scache.ambient_green = BR_SCALAR(0.0);
	scache.ambient_blue = BR_SCALAR(0.0);
	scache.ambient_intensity = BR_SCALAR(0.0);

	for(l=0, lp = self->state.light; l < MAX_STATE_LIGHTS; l++, lp++) {
		if (lp->type != BRT_AMBIENT || lp->attenuation_type == BRT_RADII)
			continue;

		found_ambient = BR_TRUE;

		if (lp->culled)
			continue;

		intensity = BR_RCP(lp->attenuation_c);

		scache.ambient_red += BR_MUL(BrFixedToScalar(BR_RED(lp->colour) << 8), intensity);
		scache.ambient_green += BR_MUL(BrFixedToScalar(BR_GRN(lp->colour) << 8), intensity);
		scache.ambient_blue += BR_MUL(BrFixedToScalar(BR_BLU(lp->colour) << 8), intensity);
		scache.ambient_intensity += intensity;
	}

	if (found_ambient) {

		if (scache.ambient_red > BR_SCALAR(1.0))
			scache.ambient_red = BR_SCALAR(1.0);

		if (scache.ambient_green > BR_SCALAR(1.0))
			scache.ambient_green = BR_SCALAR(1.0);

		if (scache.ambient_blue > BR_SCALAR(1.0))
			scache.ambient_blue = BR_SCALAR(1.0);

		scache.use_ambient_colour = 
			scache.ambient_red != BR_SCALAR(1.0) ||
			scache.ambient_green != BR_SCALAR(1.0) ||
			scache.ambient_blue != BR_SCALAR(1.0);

		if (scache.ambient_intensity > BR_SCALAR(1.0))
			scache.ambient_intensity = BR_SCALAR(1.0);

		scache.use_ambient_intensity = scache.ambient_intensity != BR_SCALAR(1.0);

	} else {

		scache.use_ambient_colour = BR_FALSE;
		scache.use_ambient_intensity = BR_FALSE;
	}
#endif

	for(l=0; l < scache.nlights_model; l++, alp++) {

		alp->culled = alp->s->culled;
		if (alp->culled)
			continue;

		switch(alp->type) {

		case BRT_AMBIENT:

			if (alp->s->radius_cull) {

		 		/*
			 	 * Transform light's position
			 	 */
				BrMatrix34ApplyP(&alp->position,&alp->s->position,&scache.view_to_model);

				if (BrVector3LengthSquared(&alp->position) > BR_SQR(alp->s->radius_outer + rend.v11model->radius)) {

					alp->culled = BR_TRUE;
					continue;
				}
			}
		
			break;

		case BRT_DIRECT:

			if (alp->s->radius_cull) {

				BrMatrix34ApplyP(&alp->position,&alp->s->position,&scache.view_to_model);

				if (BrVector3LengthSquared(&alp->position) > BR_SQR(alp->s->radius_outer + rend.v11model->radius)) {

					alp->culled = BR_TRUE;
					continue;
				}
			}

			/*
			 * Transform light's direction
			 */
			BrMatrix34TApplyV(&alp->direction,&alp->s->direction,&self->state.matrix.model_to_view);
			BrVector3Normalise(&alp->direction,&alp->direction);

			/*
			 * Work out a unit half vector
			 */
			BrVector3Add(&alp->half,&alp->direction,&scache.eye_m_normalised);
			BrVector3Normalise(&alp->half,&alp->half);

			/*
			 * Scale direction vector by light intensity
			 */
			BrVector3Scale(&alp->direction,&alp->direction,alp->intensity);

			break;

		case BRT_SPOT:
		 	/*
			 * Transform light's direction and position
			 */
			BrMatrix34ApplyP(&alp->position,&alp->s->position,&scache.view_to_model);

			if (alp->s->radius_cull)

				if (BrVector3LengthSquared(&alp->position) > BR_SQR(alp->s->radius_outer + rend.v11model->radius)) {

					alp->culled = BR_TRUE;
					continue;
				}
		
			BrMatrix34TApplyV(&alp->direction,&alp->s->direction,&self->state.matrix.model_to_view);
			BrVector3Normalise(&alp->direction,&alp->direction);

			if (alp->s->angle_cull)

				if (!sphereIntersectsCone(rend.v11model->radius, &alp->position, &alp->direction, alp->s->angle_outer)) {

					alp->culled = BR_TRUE;
					continue;
				}
		
			break;

		case BRT_POINT:
		 	/*
			 * Transform light's position
			 */
			BrMatrix34ApplyP(&alp->position,&alp->s->position,&scache.view_to_model);

			if (alp->s->radius_cull)

				if (BrVector3LengthSquared(&alp->position) > BR_SQR(alp->s->radius_outer + rend.v11model->radius)) {

					alp->culled = BR_TRUE;
					continue;
				}
		
			break;
		}

		if (alp->volume.regions != NULL)
			for (region = alp->s->volume.regions, new_region = alp->volume.regions, i = 0; i < alp->volume.nregions; i++, region++, new_region++)
				for (plane = region->planes, new_plane = new_region->planes, j = 0; j < region->nplanes; j++, plane++, new_plane++)
					BrMatrix34ApplyPlaneEquation(new_plane, plane, &scache.view_to_model);
	}
	
	if (scache.nlights_view > 0) {

		/*
		 * Calculate a bounding radius for the model in view space
		 *
		 * This is a horrendous overestimate!
		 */
		model_radius = BR_MUL(rend.v11model->radius, BR_SQRT(
			BR_SQR(BR_ABS(self->state.matrix.model_to_view.m[0][0]) +
				BR_ABS(self->state.matrix.model_to_view.m[1][0]) +
				BR_ABS(self->state.matrix.model_to_view.m[2][0])) +
			BR_SQR(BR_ABS(self->state.matrix.model_to_view.m[0][1]) +
				BR_ABS(self->state.matrix.model_to_view.m[1][1]) +
				BR_ABS(self->state.matrix.model_to_view.m[2][1])) +
			BR_SQR(BR_ABS(self->state.matrix.model_to_view.m[0][2]) +
				BR_ABS(self->state.matrix.model_to_view.m[1][2]) +
				BR_ABS(self->state.matrix.model_to_view.m[2][2]))));

		for(l=0; l < scache.nlights_view; l++, alp++) {

			alp->culled = alp->s->culled;
			if (alp->culled)
				continue;

			switch(alp->type) {

			case BRT_AMBIENT:

				if (alp->s->radius_cull) {

			 		/*
				 	 * Transform light's position
				 	 */
					BrVector3Sub(&light_pos, &alp->position, (br_vector3 *)&self->state.matrix.model_to_view.m[3]);

					if (BrVector3LengthSquared(&light_pos) > BR_SQR(alp->s->radius_outer + model_radius)) {

						alp->culled = BR_TRUE;
						continue;
					}
				}
			
				break;

			case BRT_DIRECT:

				if (alp->s->radius_cull) {

					BrVector3Sub(&light_pos, &alp->position, (br_vector3 *)&self->state.matrix.model_to_view.m[3]);

					if (BrVector3LengthSquared(&light_pos) > BR_SQR(alp->s->radius_outer + model_radius)) {

						alp->culled = BR_TRUE;
						continue;
					}
				}

				/*
				 * Work out a unit half vector:
				 *  eye = (0,0,1)
				 *  half = normalise(light_direection + eye)
				 */
				alp->half = alp->s->direction;
				alp->half.v[Z] += BR_SCALAR(1.0);
				BrVector3Normalise(&alp->half,&alp->half);
				break;

			case BRT_SPOT:

				if (!alp->s->radius_cull && !alp->s->angle_cull)
					break;

				BrVector3Sub(&light_pos, &alp->position, (br_vector3 *)&self->state.matrix.model_to_view.m[3]);

				if (alp->s->radius_cull)

					if (BrVector3LengthSquared(&light_pos) > BR_SQR(alp->s->radius_outer + model_radius)) {

						alp->culled = BR_TRUE;
						continue;
					}
			
				if (alp->s->angle_cull)

					if (!sphereIntersectsCone(rend.v11model->radius, &light_pos, &alp->direction, alp->s->angle_outer)) {

						alp->culled = BR_TRUE;
						continue;
					}
			
				break;

			case BRT_POINT:

				if (!alp->s->radius_cull)
					break;

				BrVector3Sub(&light_pos, &alp->position, (br_vector3 *)&self->state.matrix.model_to_view.m[3]);

				if (alp->s->radius_cull)

					if (BrVector3LengthSquared(&light_pos) > BR_SQR(alp->s->radius_outer + model_radius)) {

						alp->culled = BR_TRUE;
						continue;
					}
			
				break;
			}
		}
	}
}

static br_boolean sphereIntersectsCone(br_scalar sphere_radius, br_vector3 *cone_apex,
	br_vector3 *cone_axis, br_angle cone_angle)
{
	br_scalar dist;
	br_angle sphere_angle;
	br_scalar axis_dot;

	/*
	 * Find distance between sphere centre and cone apex
	 */
	dist = BrVector3Length(cone_apex);

	if (dist < sphere_radius)
		return BR_TRUE;

	/*
	 * Find angle of another cone surrounding the sphere with the same apex as the cone
	 */
	sphere_angle = BR_ASIN(BR_DIV(sphere_radius, dist));

	/*
	 * Find the cosine of the angle between the two cone axes
	 */
	axis_dot = BR_DIV(BrVector3Dot(cone_apex, cone_axis), dist);

	/*
	 * See if this angle exceeds the sum of the two cone angles
	 */
	return axis_dot > BR_COS(sphere_angle + cone_angle);
}


/*
 * Build the tables of functions that evaluate per-vertex and per-primitive components
 */
#if BASED_FIXED
static br_boolean isPowerof2(br_int_32 x)
{
	return !((x-1) & x);
}

static br_uint_8 findShift(br_int_32 x)
{
	br_uint_8 b;

	for(b=0; x; b++)
		x /= 2;

	return b;
}
#endif

br_int_32 GenerateSurfaceFunctions(br_renderer *self, surface_fn **fns, br_uint_32 mask)
{
	br_int_32 f = 0;
	br_matrix23 *m;

	/* U,V
	 */
	if(mask & (CM_U|CM_V)) {
		switch(self->state.surface.mapping_source) {
		case BRT_ENVIRONMENT_LOCAL:
			fns[f++] = SurfaceMapEnvironmentLocal;
			break;

		case BRT_ENVIRONMENT_INFINITE:
			fns[f++] = SurfaceMapEnvironmentInfinite;
			break;

		case BRT_GEOMETRY_MAP:
			fns[f++] = SurfaceMapGeometryMap;

			m = &self->state.cache.map_transform;

			if(m->m[0][1] == BR_SCALAR(0) &&
			   m->m[1][0] == BR_SCALAR(0)) {

				/*
				 * Can at least use this version
				 */
				fns[f-1] = SurfaceMapGeometryMapScaleTranslate;

				/*
				 * Spot zero translation
				 */
				if(m->m[2][0] == BR_SCALAR(0) &&
				   m->m[2][1] == BR_SCALAR(0))
					fns[f-1] = SurfaceMapGeometryMapScale;

#if BASED_FIXED
				/*
				 * Spot case where shifts can be used
				 */
				if( m->m[0][0] > BR_SCALAR(1) &&
                	m->m[1][1] > BR_SCALAR(1) &&
					isPowerof2(m->m[0][0]) &&
					isPowerof2(m->m[1][1])) {

					self->state.cache.u_shift = findShift(m->m[0][0]) - 17;
					self->state.cache.v_shift = findShift(m->m[1][1]) - 17;

					fns[f-1] = SurfaceMapGeometryMapShift;
				}
#endif

				/*
				 * Spot identity transform
				 */
			 	if(m->m[0][0] == BR_SCALAR(1) &&
				   m->m[1][1] == BR_SCALAR(1) &&
				   m->m[2][0] == BR_SCALAR(0) &&
				   m->m[2][1] == BR_SCALAR(0)) {
						fns[f-1] = SurfaceMapGeometryMapCopy;
				}

			}

			break;

		case BRT_GEOMETRY_X:
			fns[f++] = SurfaceMapGeometryX;
			break;

		case BRT_GEOMETRY_Y:
			fns[f++] = SurfaceMapGeometryY;
			break;

		case BRT_GEOMETRY_Z:
		default:
			fns[f++] = SurfaceMapGeometryZ;
			break;

		case BRT_QUAD:
			break;
		}
	}

	/* I
	 */
	if(mask & CM_I) {
		if(self->state.surface.lighting) {
			fns[f++] = SurfaceIndexLit;
		} else if(self->state.surface.colour_source == BRT_GEOMETRY) {
			fns[f++] = SurfaceIndexUnlit;
		} else {
			fns[f++] = SurfaceIndexZero;
		}
	} else if(mask & CM_UI) {
       	/* Unlit I
	     */
		if(self->state.surface.colour_source == BRT_GEOMETRY) {
			fns[f++] = SurfaceIndexUnlit;
		} else {
			fns[f++] = SurfaceIndexZero;
		}
	}


	/* RGB
	 */
	if(mask & (CM_R|CM_G|CM_B)) {
		if(self->state.surface.lighting)
			if (self->state.surface.prelighting)
				fns[f++] = SurfaceColourLitPrelit;
			else
				fns[f++] = SurfaceColourLit;
		else
			fns[f++] = SurfaceColourUnlit;
	} else if(mask & (CM_UR|CM_UG|CM_UB)) {
    	/* Unlit RGB
	     */
    	fns[f++] = SurfaceColourUnlit;
    }

	/* Alpha
	 */
	if(mask & CM_A)
		if(self->state.surface.opacity_source == BRT_GEOMETRY) {
			fns[f++] = SurfaceAlphaPrealpha;
		} else {
			fns[f++] = SurfaceAlpha;
		}

	/* Linear Z
	 */
	if(mask & CM_SW)
		fns[f++] = SurfaceLinearDepth;

	/* Terminate List with a NULL
	 */
	fns[f] = NULL;

	return f;
}

/*
 * Convert a component mask (CM_*) to a mask of bits per component entry
 */
br_uint_32 ComponentMaskToSlots(br_uint_32 cm)
{
	br_uint_32 m;
	int i;

	static br_uint_32 bits[] = {
		1<<C_X,		/* CM_X  */
		1<<C_Y,		/* CM_Y  */
		1<<C_Z,		/* CM_Z  */
		1<<C_W,		/* CM_W	 */
		1<<C_SX,	/* CM_SX */
		1<<C_SY,	/* CM_SY */
		1<<C_SZ,	/* CM_SZ */
		1<<C_U,		/* CM_U	 */
		1<<C_V,		/* CM_V	 */
		1<<C_I,		/* CM_I	 */
		1<<C_A,		/* CM_A	 */
		1<<C_R,		/* CM_R	 */
		1<<C_G,		/* CM_G	 */
		1<<C_B,		/* CM_B	 */
		1<<C_UI,	/* CM_UI */
		1<<C_UR,	/* CM_UR */
		1<<C_UG,	/* CM_UG */
		1<<C_UB,	/* CM_UB */
		1<<C_ARGB,	/* CM_ARGB */
		1<<C_URGB,	/* CM_URGB */
		1<<C_DRGB,	/* CM_DRGB */
		1<<C_SRGB,	/* CM_SRGB */
		1<<C_SW,	/* CM_SW */
		1<<C_Q,		/* CM_Q */
	};

	m = 0;

	for(i=0; cm; i++, cm /= 2)
		if(cm & 1)
			m |= bits[i];
		
	return m;
}

/*
 * Functions to accumulate lists of gemetry functions
 */
void GeometryFunctionReset(br_renderer *renderer)
{
	renderer->state.cache.ngeometry_fns = 0;
	renderer->state.cache.ngeometry_fns_onscreen = 0;
}

void GeometryFunctionAdd(br_renderer *renderer, geometry_fn *fn)
{
	ASSERT(renderer->state.cache.ngeometry_fns < BR_ASIZE(renderer->state.cache.geometry_fns));
	renderer->state.cache.geometry_fns[renderer->state.cache.ngeometry_fns++] = fn;
}

void GeometryFunctionOnScreenAdd(br_renderer *renderer, geometry_fn *fn)
{
	ASSERT(renderer->state.cache.ngeometry_fns_onscreen < BR_ASIZE(renderer->state.cache.geometry_fns_onscreen));
	renderer->state.cache.geometry_fns_onscreen[renderer->state.cache.ngeometry_fns_onscreen++] = fn;
}

void GeometryFunctionBothAdd(br_renderer *renderer, geometry_fn *fn)
{
	GeometryFunctionAdd(renderer, fn);
	GeometryFunctionOnScreenAdd(renderer, fn);
}

/*
 * Functions to chain primitive blocks together
 */
void PrimBlockReset(br_renderer *renderer)
{
	renderer->state.cache.nface_blocks = 0;
	renderer->state.cache.nface_blocks_onscreen = 0;
}

void PrimBlockAdd(br_renderer *renderer, brp_render_fn *fn)
{
	int f;

	ASSERT(renderer->state.cache.nface_blocks < (BR_ASIZE(renderer->state.cache.face_blocks)-1));

	f = renderer->state.cache.nface_blocks;
	renderer->state.cache.face_blocks[f].render = fn;
	renderer->state.cache.face_blocks[f+1].chain = (brp_block *)(renderer->state.cache.face_blocks+f);

	renderer->state.cache.nface_blocks = f+1;
}

void PrimBlockOnScreenAdd(br_renderer *renderer, brp_render_fn *fn)
{
	int f;

	ASSERT(renderer->state.cache.nface_blocks_onscreen < (BR_ASIZE(renderer->state.cache.face_blocks_onscreen)-1));

	f = renderer->state.cache.nface_blocks_onscreen;
	renderer->state.cache.face_blocks_onscreen[f].render = fn;
	renderer->state.cache.face_blocks_onscreen[f+1].chain = (brp_block *)(renderer->state.cache.face_blocks_onscreen+f);

	renderer->state.cache.nface_blocks_onscreen = f+1;
}

void PrimBlockAddBoth(br_renderer *renderer, brp_render_fn *fn)
{
	PrimBlockAdd(renderer, fn);
	PrimBlockOnScreenAdd(renderer, fn);
}

