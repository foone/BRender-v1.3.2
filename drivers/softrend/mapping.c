/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: mapping.c 1.3 1998/07/20 14:45:20 jon Exp $
 * $Locker: $
 *
 * Generation of U,V components
 */
#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

#include "math_ip.h"

BR_RCS_ID("$Id: mapping.c 1.3 1998/07/20 14:45:20 jon Exp $");

#define APPLY_UV(du,dv,su,sv) do { \
	du = BR_ADD(													\
			BR_MAC2(self->state.cache.map_transform.m[0][0],su,		\
					self->state.cache.map_transform.m[1][0],sv),	\
			self->state.cache.map_transform.m[2][0]);				\
	dv = BR_ADD(													\
			BR_MAC2(self->state.cache.map_transform.m[0][1],su,		\
			self->state.cache.map_transform.m[1][1],sv),			\
			self->state.cache.map_transform.m[2][1]);				\
} while (0)

/*
 * Generate U,V for environment assuming infinite eye
 */
void SURFACE_CALL SurfaceMapEnvironmentInfinite(br_renderer *self,
	br_vector3 *p, br_vector2 *map, br_vector3 *normal, br_colour colour, br_scalar *comp)
{
	br_vector3 eye;
    br_vector3 r,wr;
	br_scalar d,cu,cv;

	/*
	 * Generate reflected vector
	 *
	 * -    - - -  -
	 * R = 2N(N.E)-E
	 */
    d = BR_CONST_MUL(BrVector3Dot(&scache.eye_m_normalised,normal),2);
	BrVector3Scale(&r,normal,d);
    BrVector3Sub(&r,&r,&scache.eye_m_normalised);

	/*
	 * If there is an environment frame, rotate vector into it
	 */
	if(self->state.matrix.view_to_environment_hint != BRT_DONT_CARE) {
		BrMatrix34ApplyV(&wr, &r, &scache.model_to_environment);
		BrVector3Normalise(&wr, &wr);
	} else
		wr = r;

	/*
	 * Convert vector to environment coordinates
	 */
	cu = BrAngleToScalar(BR_ATAN2(wr.v[0],-wr.v[2]));

#if 0
	cv = BrAngleToScalar(BR_ASIN(-wr.v[1]/2+BR_SCALAR(0.5)));
#else
	cv = -wr.v[1]/2+BR_SCALAR(0.5);
#endif

	APPLY_UV(comp[C_U],comp[C_V],cu,cv);
}

/*
 * Generate U,V for environment assuming local eye
 */
void SURFACE_CALL SurfaceMapEnvironmentLocal(br_renderer *self,
	br_vector3 *p, br_vector2 *map, br_vector3 *normal, br_colour colour, br_scalar *comp)
{
	br_vector3 eye;
	br_vector3 r,wr;
	br_scalar d,cu,cv;

	/*
	 * Generate eye vector - 
	 */
	BrVector3Sub(&eye,&scache.eye_m,p);
	BrVector3Normalise(&eye, &eye);
	
	/*
	 * Generate reflected vector
	 *
	 * -    - - -  -
	 * R = 2N(N.E)-E
	 */
	d = BR_CONST_MUL(BrVector3Dot(&eye,normal),2);
	BrVector3Scale(&r,normal,d);
	BrVector3Sub(&r,&r,&eye);

	/*
	 * If there is an environment frame, rotate vector into it
	 */
	if(self->state.matrix.view_to_environment_hint != BRT_DONT_CARE) {
		BrMatrix34ApplyV(&wr, &r, &scache.model_to_environment);
		BrVector3Normalise(&wr, &wr);
	} else
		wr = r;

	/*
	 * Convert vector to environment coordinates
	 */
	cu = BrAngleToScalar(BR_ATAN2(wr.v[0],-wr.v[2]));

#if 0
	cv = BrAngleToScalar(BR_ASIN(-wr.v[1]/2+BR_SCALAR(0.5)));
#else
	cv = -wr.v[1]/2+BR_SCALAR(0.5);
#endif

	APPLY_UV(comp[C_U],comp[C_V],cu,cv);
}

/*
 * Take U,V from vertex y,z
 */
void SURFACE_CALL SurfaceMapGeometryX(br_renderer *self,
	br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour colour, br_scalar *comp)
{
	APPLY_UV(comp[C_U],comp[C_V],p->v[1],p->v[2]);
}

void SURFACE_CALL SurfaceMapGeometryY(br_renderer *self,
	br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour colour, br_scalar *comp)
{
	APPLY_UV(comp[C_U],comp[C_V],p->v[2],p->v[0]);
}

void SURFACE_CALL SurfaceMapGeometryZ(br_renderer *self,
	br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour colour, br_scalar *comp)
{
	APPLY_UV(comp[C_U],comp[C_V],p->v[0],p->v[1]);
}

/*
 * Take U,V from vertex u,v
 */
void SURFACE_CALL SurfaceMapGeometryMap(br_renderer *self,
	br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour colour, br_scalar *comp)
{
	APPLY_UV(comp[C_U],comp[C_V],map->v[0],map->v[1]);
}

/*
 * Scale only
 */
void SURFACE_CALL SurfaceMapGeometryMapScale(br_renderer *self,
	br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour colour, br_scalar *comp)
{
    comp[C_U] = BR_MUL(self->state.cache.map_transform.m[0][0],map->v[0]);
    comp[C_V] = BR_MUL(self->state.cache.map_transform.m[1][1],map->v[1]);
}

/*
 * Scale and translate only
 */
void SURFACE_CALL SurfaceMapGeometryMapScaleTranslate(br_renderer *self,
	br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour colour, br_scalar *comp)
{
    comp[C_U] = BR_ADD(BR_MUL(self->state.cache.map_transform.m[0][0],map->v[0]),self->state.cache.map_transform.m[2][0]);
    comp[C_V] = BR_ADD(BR_MUL(self->state.cache.map_transform.m[1][1],map->v[1]),self->state.cache.map_transform.m[2][1]);
}

void SURFACE_CALL SurfaceMapGeometryMapCopy(br_renderer *self,
	br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour colour, br_scalar *comp)
{
	comp[C_U] = map->v[0];
	comp[C_V] = map->v[1];
}

#if BASED_FIXED
/*
 * Take U,V from vertex, assumeing offset == 0 and scales are a power of 2
 */
void SURFACE_CALL SurfaceMapGeometryMapShift(br_renderer *self,
	br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour colour, br_scalar *comp)
{
	comp[C_U] = (map->v[0] << self->state.cache.u_shift) + self->state.cache.map_transform.m[2][0];
	comp[C_V] = (map->v[1] << self->state.cache.v_shift) + self->state.cache.map_transform.m[2][1];
}
#endif

#ifndef OpTriangleMapQuad
void BR_ASM_CALL OpTriangleMapQuad(struct brp_block *block, union brp_vertex *v0, union brp_vertex *v1, union brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3], br_vector4 *fp_eqn, struct temp_face *tfp)
{
	br_vector2 *qv = (br_vector2 *)&rend.renderer->state.cache.quad_transformed;

	switch (rend.face_flags[rend.current_index] & BR_FACEF_QUAD_MASK) {

	case BR_FACEF_QUAD_012:

		v0->comp[C_U] = qv[0].v[0];
		v0->comp[C_V] = qv[0].v[1];
		v1->comp[C_U] = qv[1].v[0];
		v1->comp[C_V] = qv[1].v[1];
		v2->comp[C_U] = qv[2].v[0];
		v2->comp[C_V] = qv[2].v[1];
		break;

	case BR_FACEF_QUAD_123:

		v0->comp[C_U] = qv[1].v[0];
		v0->comp[C_V] = qv[1].v[1];
		v1->comp[C_U] = qv[2].v[0];
		v1->comp[C_V] = qv[2].v[1];
		v2->comp[C_U] = qv[3].v[0];
		v2->comp[C_V] = qv[3].v[1];
		break;

	case BR_FACEF_QUAD_230:

		v0->comp[C_U] = qv[2].v[0];
		v0->comp[C_V] = qv[2].v[1];
		v1->comp[C_U] = qv[3].v[0];
		v1->comp[C_V] = qv[3].v[1];
		v2->comp[C_U] = qv[0].v[0];
		v2->comp[C_V] = qv[0].v[1];
		break;

	case BR_FACEF_QUAD_301:

		v0->comp[C_U] = qv[3].v[0];
		v0->comp[C_V] = qv[3].v[1];
		v1->comp[C_U] = qv[0].v[0];
		v1->comp[C_V] = qv[0].v[1];
		v2->comp[C_U] = qv[1].v[0];
		v2->comp[C_V] = qv[1].v[1];
		break;

	case BR_FACEF_QUAD_032:

		v0->comp[C_U] = qv[0].v[0];
		v0->comp[C_V] = qv[0].v[1];
		v1->comp[C_U] = qv[3].v[0];
		v1->comp[C_V] = qv[3].v[1];
		v2->comp[C_U] = qv[2].v[0];
		v2->comp[C_V] = qv[2].v[1];
		break;

	case BR_FACEF_QUAD_103:

		v0->comp[C_U] = qv[1].v[0];
		v0->comp[C_V] = qv[1].v[1];
		v1->comp[C_U] = qv[0].v[0];
		v1->comp[C_V] = qv[0].v[1];
		v2->comp[C_U] = qv[3].v[0];
		v2->comp[C_V] = qv[3].v[1];
		break;

	case BR_FACEF_QUAD_210:

		v0->comp[C_U] = qv[2].v[0];
		v0->comp[C_V] = qv[2].v[1];
		v1->comp[C_U] = qv[1].v[0];
		v1->comp[C_V] = qv[1].v[1];
		v2->comp[C_U] = qv[0].v[0];
		v2->comp[C_V] = qv[0].v[1];
		break;

	case BR_FACEF_QUAD_321:

		v0->comp[C_U] = qv[3].v[0];
		v0->comp[C_V] = qv[3].v[1];
		v1->comp[C_U] = qv[2].v[0];
		v1->comp[C_V] = qv[2].v[1];
		v2->comp[C_U] = qv[1].v[0];
		v2->comp[C_V] = qv[1].v[1];
		break;
	}

	block->chain->render(block->chain, v0,v1,v2, fp_vertices, fp_edges, fp_eqn, tfp);
}
#endif



