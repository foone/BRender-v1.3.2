/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: lightmac.h 1.3 1998/08/13 13:57:15 jon Exp $
 * $Locker: $
 *
 * Macros for building the lighting functions
 */


 
/*
 * Clamp a component to (0,1], scale and offset
 */
#define CLAMP_SCALE(c)	do {														\
	if (BR_SCALAR_GE_1(comp[(c)]))													\
		comp[(c)] = self->state.cache.comp_offsets[(c)] +							\
			self->state.cache.comp_scales[(c)]-BR_SCALAR_EPSILON;					\
	else if (BR_SCALAR_LE_0(comp[(c)]))												\
		comp[(c)] = self->state.cache.comp_offsets[(c)];							\
	else																			\
		comp[(c)] = BR_MUL(self->state.cache.comp_scales[(c)], comp[(c)]) +			\
			self->state.cache.comp_offsets[(c)];									\
} while(0);

/*
 * Calculate a component from surface colour, diffuse and specular values and clamp
 * to (0,1], scale and offset
 */
#define CALCULATE_COMPONENT(c, colour, prelit, diffuse, specular) do {	\
	br_scalar total;													\
	total = prelit + BR_MUL(colour, diffuse) + specular;				\
	if (BR_SCALAR_GE_1(total))											\
		comp[(c)] = self->state.cache.comp_offsets[(c)] +				\
			self->state.cache.comp_scales[(c)]-BR_SCALAR_EPSILON;		\
	else if (BR_SCALAR_LE_0(total))										\
		comp[(c)] = self->state.cache.comp_offsets[(c)];				\
	else																\
		comp[(c)] = BR_MUL(self->state.cache.comp_scales[(c)], total) +	\
			self->state.cache.comp_offsets[(c)];						\
} while(0);


/*
 * Find dot product of reflected vector and eye
 *
 * uses dirn_norm, n, and rend.eye_l
 *
 * produces dot
 */
#define SPECULAR_DOT() do {								\
	br_vector3 r;   									\
	br_scalar rd;										\
														\
	rd = BR_CONST_MUL(BrVector3Dot(&dirn_norm,n),2);	\
	BrVector3Scale(&r,n,rd);							\
	BrVector3Sub(&r,&r,&dirn_norm);						\
														\
	dot = BrVector3Dot(&rend.eye_l,&r);					\
} while(0)

#define SPECULAR_DOT_DIRECT() do {		\
	dot = BrVector3Dot(n,&alp->half);	\
} while(0)

/*
 * Given the dot-product of eye & reflected vector, returns specular component of lighting eqn.
 */
#define SPECULAR_POWER(l) BR_MULDIV(dot,l,self->state.surface.power-BR_MUL(self->state.surface.power,dot)+dot)

/*
 * Calculates vector between point and light source
 */
#define CALCULATE_LIGHT_DIRN() do {			\
	BrVector3Sub(&dirn,&alp->position,p);	\
} while (0)

/*
 * Given dist, calculates dirn_norm & dist
 */
#define CALCULATE_DIRN_NORM() do {					\
	BrVector3Normalise(&dirn_norm,&dirn);			\
} while(0)

#define CALCULATE_DIRN_NORM_DIST() do {				\
	br_scalar s;									\
	dist = BrVector3Length(&dirn);					\
	if (BR_SCALAR_LE_2_EPSILON(dist))				\
		return;										\
	s = BR_RCP(dist);								\
	BrVector3Scale(&dirn_norm,&dirn,s);				\
} while(0)

#define CALCULATE_DIRN_NORM_DIST_CUTOFF(cutoff) do {	\
	br_scalar s;										\
	dist = BrVector3LengthSquared(&dirn);				\
	if (BR_SCALAR_GT(dist, cutoff))						\
		return;											\
	dist = BR_SQRT(dist);								\
	if (BR_SCALAR_LE_2_EPSILON(dist))					\
		return;											\
	s = BR_RCP(dist);									\
	BrVector3Scale(&dirn_norm,&dirn,s);					\
} while(0)

#define CALCULATE_DIST_CUTOFF(cutoff) do {	\
	dist = BrVector3LengthSquared(&dirn);	\
	if (BR_SCALAR_GT(dist, cutoff))			\
		return;								\
	dist = BR_SQRT(dist);					\
} while(0)

/*
 * Given dist, calculates attn
 */
#if BASED_FIXED
#define DIST_SQR(a) ((a) >= BR_SCALAR(180.0)? BR_SCALAR(32767.0): BR_SQR(a))
#endif

#if BASED_FLOAT
#define DIST_SQR(a) BR_SQR(a)
#endif

#define CALCULATE_ATTENUATION() do {				\
	br_scalar dist2 = DIST_SQR(dist);				\
	attn = BR_RCP(alp->s->attenuation_c				\
		+ BR_MUL(dist,alp->s->attenuation_l)		\
		+ BR_MUL(dist2,alp->s->attenuation_q));		\
} while(0)

#define MULTIPLY_ATTENUATION(l) do {				\
	br_scalar dist2 = DIST_SQR(dist);				\
	attn = BR_MUL((l), BR_RCP(alp->s->attenuation_c	\
		+ BR_MUL(dist,alp->s->attenuation_l)		\
		+ BR_MUL(dist2,alp->s->attenuation_q)));	\
} while(0)

#define CALCULATE_ATTENUATION_LINEAR() do {			\
	attn = BR_RCP(alp->s->attenuation_c				\
		+ BR_MUL(dist,alp->s->attenuation_l));		\
} while(0)

#define MULTIPLY_ATTENUATION_LINEAR(l) do {			\
	attn = BR_DIV((l), alp->s->attenuation_c		\
		+ BR_MUL(dist,alp->s->attenuation_l));		\
} while(0)

#define CALCULATE_ATTENUATION_RADII() do {			\
	if (BR_SCALAR_LT(dist, alp->s->radius_inner))	\
		attn = alp->intensity;						\
	else											\
		attn = BR_MUL(alp->falloff,					\
			dist - alp->s->radius_outer);			\
} while(0)

#define MULTIPLY_ATTENUATION_RADII(l) do {			\
	if (BR_SCALAR_LT(dist, alp->s->radius_inner))	\
		attn = BR_MUL((l), alp->intensity);			\
	else											\
		attn = BR_MUL((l), BR_MUL(alp->falloff,		\
			dist - alp->s->radius_outer));			\
} while(0)

/*
 * Finds dot product of surface normal and normalised light vector
 */
#define DIFFUSE_DOT() do {				\
	dot = BrVector3Dot(n,&dirn_norm);	\
	if (BR_SCALAR_LE_0(dot))			\
		return;							\
} while(0)

#define DIFFUSE_DOT_DIRECT() do {			\
	dot = BrVector3Dot(n,&alp->direction);	\
	if (BR_SCALAR_LE_0(dot))				\
		return;								\
} while(0)

/*
 * Finds dot product of light direction and normalised light vector
 */
#define SPOT_DOT() do {										\
	dot_spot = BrVector3Dot(&dirn_norm,&alp->direction);	\
	if (BR_SCALAR_LT(dot_spot, alp->s->spot_outer))			\
		return;												\
} while(0)

#define SPOT_FALLOFF(l) ((dot_spot < alp->s->spot_inner)?	\
	BR_MUL((l), BR_MUL(alp->s->spot_outer - dot_spot, alp->spot_falloff)):	(l))
 
#define MULTIPLY_SPOT_ATTENUATION(l) do {										\
	attn = (BR_SCALAR_LT(dot_spot, alp->s->spot_inner)?							\
		BR_MUL((l), BR_MUL(alp->s->spot_outer - dot_spot, alp->spot_falloff)):	\
		(l));																	\
} while (0)
 
/*
 * Checking light volume
 */
#define CHECK_LIGHT_VOLUME() do {				\
	if (!pointInLightVolumes(p, &alp->volume))	\
		return;									\
} while (0)

#define CALCULATE_LIGHT_VOLUME_ATTENUATION() do {  	\
	attn = lightVolumeAttenuation(p, &alp->volume);	\
	if (BR_SCALAR_LE_0(attn))					   	\
		return;									   	\
} while (0)

#define MULTIPLY_LIGHT_VOLUME_ATTENUATION(l) do {					\
	attn = BR_MUL((l), lightVolumeAttenuation(p, &alp->volume));	\
	if (BR_SCALAR_LE_0(attn))									   	\
		return;														\
} while (0)

/*
 * Accumulating light components
 */
#define ACCUMULATE_AMBIENT_RGB(l) do {			\
	diffuse_r += BR_MUL((l), alp->red);		\
	diffuse_g += BR_MUL((l), alp->green);	\
	diffuse_b += BR_MUL((l), alp->blue);	\
} while (0)

#define ACCUMULATE_DIFFUSE_RGB(l) do {			\
	br_scalar diffuse = BR_MUL(dot, (l));		\
	diffuse_r += BR_MUL(diffuse, alp->red);		\
	diffuse_g += BR_MUL(diffuse, alp->green);	\
	diffuse_b += BR_MUL(diffuse, alp->blue);	\
} while (0)

/*
 * Phong lighting approximation from Gems IV pg. 385
 */
#define ACCUMULATE_SPECULAR_RGB(l) do {				\
	if (BR_SCALAR_GT_SPECULARPOW_CUTOFF(dot)) {		\
		br_scalar specular = SPECULAR_POWER(l);		\
		specular_r += BR_MUL(specular, alp->red);	\
		specular_g += BR_MUL(specular, alp->green);	\
		specular_b += BR_MUL(specular, alp->blue);	\
	}												\
} while (0)
