/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: light.h 1.3 1998/06/17 16:11:46 jon Exp $
 * $Locker: $
 *
 * Definitons for a light
 */
#ifndef _LIGHT_H_
#define _LIGHT_H_

enum {
	/*
	 * Type of light
	 */
	BR_LIGHT_TYPE    = 0x0003,
	BR_LIGHT_POINT   = 0x0000,
 	BR_LIGHT_DIRECT  = 0x0001,
	BR_LIGHT_SPOT    = 0x0002,
	BR_LIGHT_AMBIENT = 0x0003,

	/*
     * Flag indicating that calculations are done in view space
     */ 
	BR_LIGHT_VIEW  = 0x0004,

	/*
	 * Flag indicating that linear falloff should be used
	 */
	BR_LIGHT_LINEAR_FALLOFF = 0x0008,
};

/*
 * Definition of cutoff volumes - vertex is lit if it falls within any of
 * a number of convex volumes, and partially lit if it falls within the
 * falloff distance from any
 */
typedef struct br_light_volume {

	br_scalar falloff_distance;
	br_convex_region *regions;
	br_uint_32 nregions;

} br_light_volume;

typedef struct br_light {
	/*
	 * Optional identifier
	 */
	char *identifier;

	/*
	 * Type of light
	 */
	br_uint_8 type;

	/*
	 * Colour of light (if renderer supports it)
	 */
	br_colour colour;

	/*
	 * Attenuation of light with distance - constant, linear, and quadratic
	 * l & q only apply to point and spot lights
	 */
	br_scalar attenuation_c;
	br_scalar attenuation_l;
	br_scalar attenuation_q;

	/*
	 * Cone angles for spot light
	 */
	br_angle cone_outer;
	br_angle cone_inner;

	/*
	 * Sphere radii for linear falloff and cutoff of normally attenuated lights
	 */
	br_scalar radius_outer;
	br_scalar radius_inner;

	/*
	 * Cutoff volumes
	 */
	br_light_volume volume;

	void * user;

} br_light;

#endif
