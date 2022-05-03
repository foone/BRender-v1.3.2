/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: enables.c 1.7 1998/08/17 14:53:21 jon Exp $
 * $Locker: $
 *
 * Enabling/disabling of 'special' places in actor hierarchy
 */
#include "v1db.h"
#include "shortcut.h"
#include "brassert.h"

#include "math_ip.h"

BR_RCS_ID("$Id: enables.c 1.7 1998/08/17 14:53:21 jon Exp $")


static void actorEnable(br_v1db_enable *e, br_actor *a)
{
	int i;

	ASSERT_MESSAGE("actorEnable NULL pointer to an actor", a != NULL);
	ASSERT(a->type == e->type);

	if(e->enabled == NULL)
		e->enabled = BrResAllocate(v1db.res, e->max * sizeof(*e->enabled), BR_MEMORY_ENABLED_ACTORS);

	/*
	 * Look to see if actor is already enabled
	 */
	for(i=0; i < e->max; i++)
		if(e->enabled[i] == a)
			return;

	/*
	 * Find a blank slot
	 */
	for(i=0; i < e->max; i++) {
		if(e->enabled[i] == NULL) {
			e->enabled[i] = a;
			e->count++;
			return;
		}
	}

	BR_ERROR1("too many enabled %ss",e->name);
}

static void actorDisable(br_v1db_enable *e, br_actor *a)
{
	int i;

	ASSERT_MESSAGE("actorDisable NULL pointer to an actor", a != NULL);
	ASSERT(a->type == e->type);

	if(e->enabled == NULL)
		return;

	/*
	 * Find actor in table and remove it
	 */
	for(i=0; i < e->max; i++) {
		if(e->enabled[i] == a) {
			e->enabled[i] = NULL;
			e->count--;
			return;
		}
	}
}

/*
 * Add a light to the set that will illuminate the world
 */
void BR_PUBLIC_ENTRY BrLightEnable(br_actor *l)
{
    UASSERT_MESSAGE("BrLightEnable NULL pointer to a light actor", l != NULL);

	actorEnable(&v1db.enabled_lights,l);
}

/*
 * Remove a light from the set that will illuminate the world
 */
void BR_PUBLIC_ENTRY BrLightDisable(br_actor *l)
{

    UASSERT_MESSAGE("BrLightDisable NULL pointer to a light actor", l != NULL);
	actorDisable(&v1db.enabled_lights,l);
}

/*
 * Allow all lights to illuminate the current model
 */
br_error BrLightCullReset(void)
{
	br_token_value tv[2];
	br_uint_32 light_part;
	int i;

	/*
	 * Find the light in the list of enabled lights and set the appropriate
	 * part of the light state.  N.B. the culled state is reset before each
	 * model is rendered
	 */
	if (v1db.enabled_lights.enabled == NULL)
		return BRE_FAIL;

	tv[0].t = BRT_CULLED_B;
	tv[0].v.b = BR_FALSE;

	tv[1].t = BR_NULL_TOKEN;

	for (light_part = 0, i = 0; i < v1db.enabled_lights.max; i++) {

		if (v1db.enabled_lights.enabled[i] == NULL)
			continue;

		RendererPartSetMany(v1db.renderer, BRT_LIGHT, light_part, tv, NULL);

		light_part++;
	}

	return BRE_OK;
}

/*
 * Prevent a light from illuminating the current model
 */
br_error BR_PUBLIC_ENTRY BrLightModelCull(br_actor *light)
{
	br_token_value tv[2];
	br_uint_32 light_part;
	int i;

	/*
	 * Find the light in the list of enabled lights and set the appropriate
	 * part of the light state.  N.B. the culled state is reset before each
	 * model is rendered
	 */
	if (v1db.enabled_lights.enabled == NULL)
		return BRE_FAIL;

	tv[0].t = BRT_CULLED_B;
	tv[0].v.b = BR_TRUE;

	tv[1].t = BR_NULL_TOKEN;

	for (light_part = 0, i = 0; i < v1db.enabled_lights.max; i++) {

		if (v1db.enabled_lights.enabled[i] == NULL)
			continue;

		if (v1db.enabled_lights.enabled[i] == light) {

			RendererPartSetMany(v1db.renderer, BRT_LIGHT, light_part, tv, NULL);

			return BRE_OK;
		}

		light_part++;
	}

	return BRE_FAIL;
}

/*
 * Add a clip plane to world
 */
void BR_PUBLIC_ENTRY BrClipPlaneEnable(br_actor *c)
{

    UASSERT_MESSAGE("BrClipPlaneEnable NULL pointer to a clip plane actor", c != NULL);
	actorEnable(&v1db.enabled_clip_planes,c);
}

/*
 * Remove a clip plane 
 */
void BR_PUBLIC_ENTRY BrClipPlaneDisable(br_actor *c)
{

    UASSERT_MESSAGE("BrClipPlaneDisable NULL pointer to a clip plane actor", c != NULL);
	
	actorDisable(&v1db.enabled_clip_planes,c);
}

/*
 * Add a horizon plane to world
 */
void BR_PUBLIC_ENTRY BrHorizonPlaneEnable(br_actor *h)
{
    UASSERT_MESSAGE("BrHorizonPlaneEnable NULL pointer to a horizon plane actor", h != NULL);
	    
	actorEnable(&v1db.enabled_horizon_planes,h);
}

/*
 * Remove a horizon plane 
 */
void BR_PUBLIC_ENTRY BrHorizonPlaneDisable(br_actor *h)
{

    UASSERT_MESSAGE("BrHorizonPlaneDisable NULL pointer to a horizon plane actor", h != NULL);


	actorDisable(&v1db.enabled_horizon_planes,h);
}

/*
 * Sets the new environment anchor
 *
 * Returns the previous value
 */
br_actor * BR_PUBLIC_ENTRY BrEnvironmentSet(br_actor *a)
{

	br_actor *old_a = v1db.enabled_environment;

	v1db.enabled_environment = a;

	return old_a;
}

/*
 * Build transforms between the view and a given actor
 */
static br_boolean setupView(
	br_matrix34 *view_to_this,
	br_matrix34 *this_to_view,
	br_matrix34 *world_to_view, 
	br_int_32 w2vt,
	br_actor *world, br_actor *a)
{
	br_matrix34 this_to_world;
	br_int_32 root_t,t;

    ASSERT_MESSAGE("setupView NULL pointer", view_to_this != NULL);
    ASSERT_MESSAGE("setupView NULL pointer", this_to_view != NULL);
    ASSERT_MESSAGE("setupView NULL pointer", world_to_view != NULL);
    ASSERT_MESSAGE("setupView NULL pointer", world != NULL);

	/*
	 * Find this->world, fail if not in world
	 */
	if(!ActorToRootTyped(a, world, &this_to_world, &root_t))
		return BR_FALSE;

	/*
	 * Make this->view and invert it
	 */
	BrMatrix34Mul(this_to_view, &this_to_world, world_to_view);
	t = BrTransformCombineTypes(root_t, w2vt);

	/*
	 * Build view->light
	 */
	if(BrTransformTypeIsLP(t))
		BrMatrix34LPInverse(view_to_this,this_to_view);
	else
		BrMatrix34Inverse(view_to_this,this_to_view);

	return BR_TRUE;
}

/*
 * Configure renderer with current enabled lights
 */
void BrSetupLights(br_actor *world, br_matrix34 *world_to_view, br_int_32 w2vt)
{
	br_matrix34 this_to_view, view_to_this;
	int light_part = 0,l;
	br_token_value tv[32],*tvp;
	br_vector3 view_position, view_direction;
	br_light *light;
	br_light_volume new_volume;
	br_convex_region *temp_regions, *region, *new_region;
	br_vector4 *temp_planes, *plane, *new_plane;
	br_uint_32 i, j;

    ASSERT_MESSAGE("BrSetupLights NULL pointer to actor", world != NULL);
    ASSERT_MESSAGE("BrSetupLights NULL pointer", world_to_view != NULL);

	if(v1db.enabled_lights.enabled == NULL)
		return;

	for(l=0; l < v1db.enabled_lights.max; l++) {
		tvp = tv;

		if(v1db.enabled_lights.enabled[l] == NULL) {
			continue;
		}

		light = v1db.enabled_lights.enabled[l]->type_data;

		ASSERT_MESSAGE("Invalid light data", light != NULL);

		/*
		 * Work out view<->light transforms - ignore light if not part of current hierachy
		 */
		if(!setupView(&view_to_this, & this_to_view, world_to_view, w2vt,
			world, v1db.enabled_lights.enabled[l]))
			continue;

		/*
		 * Construct generic parts of light
		 */
		tvp->t = BRT_COLOUR_RGB;
		tvp->v.rgb = light->colour;
		tvp++;

		tvp->t = BRT_AS_SCALAR(ATTENUATION_C);
		tvp->v.s = light->attenuation_c;
		tvp++;

		tvp->t = BRT_AS_SCALAR(ATTENUATION_L);
		tvp->v.s = light->attenuation_l;
		tvp++;

		tvp->t = BRT_AS_SCALAR(ATTENUATION_Q);
		tvp->v.s = light->attenuation_q;
		tvp++;

		tvp->t = BRT_SPACE_T;
		tvp->v.t = (light->type & BR_LIGHT_VIEW)?BRT_VIEW:BRT_MODEL;
		tvp++;

		tvp->t = BRT_TYPE_T;

		switch(light->type & BR_LIGHT_TYPE) {

		case BR_LIGHT_POINT:
			tvp->v.t = BRT_POINT;
			break;

		case BR_LIGHT_DIRECT:
			tvp->v.t = BRT_DIRECT;
			break;

		case BR_LIGHT_SPOT:
			tvp->v.t = BRT_SPOT;
			break;

		case BR_LIGHT_AMBIENT:
			tvp->v.t = BRT_AMBIENT;
			break;
		}

		tvp++;

		/*
 		 * Set position and radius and enable radius culling, if
		 * appropriate
		 */
		if ((light->type & BR_LIGHT_TYPE) == BR_LIGHT_POINT ||
			(light->type & BR_LIGHT_TYPE) == BR_LIGHT_SPOT ||
			light->volume.regions != NULL ||
			light->type & BR_LIGHT_LINEAR_FALLOFF) {

			/*
			 * Transform position (0,0,0,1) into view space
			 */
			BrVector3CopyMat34Row(&view_position,&this_to_view,3);
			tvp->t = BRT_AS_VECTOR3_SCALAR(POSITION);
			tvp->v.p = &view_position;
			tvp++;

			tvp->t = BRT_AS_SCALAR(RADIUS_OUTER);
			tvp->v.s = light->radius_outer;
			tvp++;

			tvp->t = BRT_AS_SCALAR(RADIUS_INNER);
			tvp->v.s = light->radius_inner;
			tvp++;

			/*
		 	 * Enable radius culling if outer radius given
		 	 */
			tvp->t = BRT_RADIUS_CULL_B;
			tvp->v.b = light->radius_outer != BR_SCALAR(0.0);
			tvp++;

		} else {

			tvp->t = BRT_RADIUS_CULL_B;
			tvp->v.b = BR_FALSE;
			tvp++;
		}

		/*
 		 * Set direction and cone angles and enable angle culling, if
		 * appropriate
		 */
		if ((light->type & BR_LIGHT_TYPE) == BR_LIGHT_DIRECT ||
			(light->type & BR_LIGHT_TYPE) == BR_LIGHT_SPOT ||
			light->volume.regions != NULL) {

			/*
			 * Transform direction (0,0,1,0) into view space -
			 * use T(I(l_to_v)) - or column 2 of view_to_this
			 */
			BrVector3CopyMat34Col(&view_direction,&view_to_this,2);
			BrVector3Normalise(&view_direction, &view_direction);
			tvp->t = BRT_AS_VECTOR3_SCALAR(DIRECTION);
			tvp->v.p = &view_direction;
			tvp++;

			if ((light->type & BR_LIGHT_TYPE) == BR_LIGHT_SPOT) {

				tvp->t = BRT_AS_SCALAR(SPOT_OUTER);
				tvp->v.s = BR_COS(light->cone_outer);
				tvp++;

				tvp->t = BRT_AS_SCALAR(SPOT_INNER);
				tvp->v.s = BR_COS(light->cone_inner);
				tvp++;

				tvp->t = BRT_ANGLE_OUTER_A;
				tvp->v.a = light->cone_outer;
				tvp++;

				tvp->t = BRT_ANGLE_INNER_A;
				tvp->v.a = light->cone_inner;
				tvp++;

				tvp->t = BRT_ANGLE_CULL_B;
				tvp->v.b = BR_TRUE;
				tvp++;

			} else {

				tvp->t = BRT_ANGLE_CULL_B;
				tvp->v.b = BR_FALSE;
				tvp++;
			}

		} else {

			tvp->t = BRT_ANGLE_CULL_B;
			tvp->v.b = BR_FALSE;
			tvp++;
		}

		/*
		 * Pass down type of attenuation
		 */
		tvp->t = BRT_ATTENUATION_TYPE_T;
		tvp->v.t = light->type & BR_LIGHT_LINEAR_FALLOFF? BRT_RADII: BRT_QUADRATIC;
		tvp++;

		/*
		 * Pass down a hint describing the complexity of the attenuation
		 */
		tvp->t = BRT_ATTENUATION_HINT_T;
		tvp->v.t = light->attenuation_q != BR_SCALAR(0.0)? BRT_QUADRATIC:
		           light->attenuation_l != BR_SCALAR(0.0)? BRT_LINEAR:
		                                                   BRT_CONSTANT;
		tvp++;

		/*
		 * Transform lighting volume into view space and pass down
		 */
		tvp->t = BRT_LIGHTING_VOLUME_P;

		if (light->volume.regions != NULL) {

			temp_regions = BrResAllocate(v1db.res, sizeof(*temp_regions) * light->volume.nregions, BR_MEMORY_OBJECT_DATA);

			if (temp_regions != NULL)

				for (region = light->volume.regions, new_region = temp_regions, i = 0; i < light->volume.nregions; i++, region++, new_region++) {

					temp_planes = BrResAllocate(temp_regions, sizeof(*temp_planes) * region->nplanes, BR_MEMORY_OBJECT_DATA);

					if (temp_planes == NULL) {

						BrResFree(temp_regions);
						temp_regions = NULL;

						break;
					}

					for (plane = region->planes, new_plane = temp_planes, j = 0; j < region->nplanes; j++, plane++, new_plane++)
						BrMatrix34ApplyPlaneEquation(new_plane, plane, &this_to_view);

					new_region->planes = temp_planes;
					new_region->nplanes = region->nplanes;
				}

				new_volume.falloff_distance = light->volume.falloff_distance;
				new_volume.regions = temp_regions;
				new_volume.nregions = light->volume.nregions;

				tvp->v.p = &new_volume;

		} else {

			temp_regions = NULL;
			tvp->v.p = NULL;
		}
		
		tvp++;

		/*
		 * Send finished token list off to renderer
		 */
		tvp->t = BR_NULL_TOKEN;
		RendererPartSetMany(v1db.renderer, BRT_LIGHT, light_part, tv, NULL);
		light_part++;

		if (temp_regions != NULL)
			BrResFree(temp_regions);
	}

	/*
	 * Disable any previous remaining lights
	 */
	tv[0].t = BRT_TYPE_T;
	tv[0].v.t = BRT_NONE;
	tv[1].t = BR_NULL_TOKEN;

	for( ; light_part < v1db.max_light; light_part++)
		RendererPartSetMany(v1db.renderer, BRT_LIGHT, light_part, tv, NULL);

	v1db.max_light = light_part;
}

/*
 * Configure renderer with current enabled clip planes
 */
void BrSetupClipPlanes(br_actor *world, br_matrix34 *world_to_view, br_int_32 w2vt, br_matrix4 *view_to_screen)
{
	br_matrix34 this_to_view, view_to_this;
	br_matrix4 screen_to_view, tmp4, screen_to_this;
	int light_part = 0,i;
	br_int_32 clip_part;
	br_token_value tv[3];
	br_vector4 sp;

    ASSERT_MESSAGE("BrSetupClipPlanes NULL pointer to actor", world != NULL);
    ASSERT_MESSAGE("BrSetupClipPlanes NULL pointer", world_to_view != NULL);
    ASSERT_MESSAGE("BrSetupClipPlanes NULL pointer", view_to_screen != NULL);

	tv[0].t = BRT_TYPE_T;
	tv[0].v.t = BRT_PLANE;
	tv[1].t = BRT_AS_VECTOR4_SCALAR(PLANE);
	tv[1].v.p = &sp;
	tv[2].t = BR_NULL_TOKEN;

	clip_part = 0;

	if(v1db.enabled_clip_planes.enabled == NULL)
		return;

	if(v1db.enabled_clip_planes.count)
		BrMatrix4Inverse(&screen_to_view, view_to_screen);

	for(i=0; i < v1db.enabled_clip_planes.max; i++) {
		if(v1db.enabled_clip_planes.enabled[i] == NULL) {
			continue;
		}

		if(!setupView(&view_to_this, & this_to_view, world_to_view, w2vt,
			world, v1db.enabled_clip_planes.enabled[i]))
			continue;

		/*
		 * Make screen->plane
		 */
		BrMatrix4Copy34(&tmp4,&view_to_this);
		
		BrMatrix4Mul(&screen_to_this,&screen_to_view, &tmp4);

		/*
		 * Push plane through to screen space
		 */
		BrMatrix4TApply(&sp, v1db.enabled_clip_planes.enabled[i]->type_data, &screen_to_this);

		RendererPartSetMany(v1db.renderer, BRT_CLIP, clip_part, tv, NULL);
		clip_part++;
	}	

	tv[0].t = BRT_TYPE_T;
	tv[0].v.t = BRT_NONE;
	tv[1].t = BR_NULL_TOKEN;
	
	for( ; clip_part < v1db.max_clip; clip_part++)
		RendererPartSetMany(v1db.renderer, BRT_CLIP, clip_part, tv, NULL);

	v1db.max_clip = clip_part;
}

/*
 * Configure renderer with current environment
 */
void BrSetupEnvironment(br_actor *world, br_matrix34 *world_to_view, br_int_32 w2vt)
{
	br_matrix34 view_to_this, this_to_view;
	br_token h = BRT_DONT_CARE;

    ASSERT_MESSAGE("BrSetupEnvironment NULL pointer to actor", world != NULL);
    ASSERT_MESSAGE("BrSetupEnvironment NULL pointer", world_to_view != NULL);

	if(v1db.enabled_environment) {

		if(v1db.enabled_environment != world) {
			if(setupView(&view_to_this, & this_to_view, world_to_view, w2vt,
				world, v1db.enabled_environment))
				h = BRT_NONE;

		} else {
			if(BrTransformTypeIsLP(w2vt))
				BrMatrix34LPInverse(&view_to_this, world_to_view);
			else
				BrMatrix34Inverse(&view_to_this, world_to_view);

			h = BRT_NONE;
		}

	} 

	/*
	 * Send to renderer
	 */
	if(h != BRT_DONT_CARE)
		RendererPartSet(v1db.renderer, BRT_MATRIX, 0, BRT_AS_MATRIX34_SCALAR(VIEW_TO_ENVIRONMENT), (br_uint_32)&view_to_this);
	RendererPartSet(v1db.renderer, BRT_MATRIX, 0, BRT_VIEW_TO_ENVIRONMENT_HINT_T, h);
}

/*
 * Generate the current horizon planes
 */
void BrSetupHorizons(br_actor *world, br_matrix34 *world_to_view, br_int_32 w2vt)
{
	/* XXX */
}

/*
 * See if this is an 'enabled' actor
 */

/*
 * Check if an actor is enabled, prior to freeing
 *
 * This is not per the manual, which states that the app.
 * should disable actors before freeing them, but it does let
 * code that 'just' worked under 1.1 to function
 */
void BrActorEnableCheck(br_actor *a)
{
	/*
	 * Is the actor the environment anchor?
	 */
	if(a == v1db.enabled_environment)
		v1db.enabled_environment = NULL;

	/*
	 * See if actor of any specific type
	 */
	switch(a->type) {
	case BR_ACTOR_LIGHT:
		actorDisable(&v1db.enabled_lights,a);
		break;
	case BR_ACTOR_CLIP_PLANE:
		actorDisable(&v1db.enabled_clip_planes,a);
		break;
	case BR_ACTOR_HORIZON_PLANE:
		actorDisable(&v1db.enabled_horizon_planes,a);
		break;
	}
}

