/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: gen_386.h 1.3 1998/07/20 19:52:18 jon Exp $
 * $Locker: $
 *
 * Prototypes for 386 downcoded geometry support
 */
#ifndef _GEN_386_H_
#define _GEN_386_H_


#ifdef __cplusplus
extern "C" {
#endif

br_boolean BR_ASM_CALL ModelToViewportUpdate_A(void);

void GEOMETRY_CALL Vertex_OS_TransformProjectBounds_A(struct br_geometry *self, struct br_renderer *renderer);
void GEOMETRY_CALL Vertex_OS_TransformProject_A(struct br_geometry *self, struct br_renderer *renderer);

void GEOMETRY_CALL Vertex_TransformProjectOutcode_A(struct br_geometry *self, struct br_renderer *renderer);

void GEOMETRY_CALL Vertex_OS_TransformProjectBoundsSurf_A(struct br_geometry *self, struct br_renderer *renderer);
void GEOMETRY_CALL Vertex_OS_TransformProjectSurf_A(struct br_geometry *self, struct br_renderer *renderer);

void GEOMETRY_CALL Vertex_OS_TransformProjectBoundsGeom_A(struct br_geometry *self, struct br_renderer *renderer);
void GEOMETRY_CALL Vertex_OS_TransformProjectSurfGeom_A(struct br_geometry *self, struct br_renderer *renderer);

#if BASED_FLOAT

#define Vertex_OS_TransformProject				Vertex_OS_TransformProject_A

/*
 * Removed as it does not support user clip planes.
 */
//#define Vertex_TransformProjectOutcode                              Vertex_TransformProjectOutcode_A

#endif

#if BASED_FIXED

/*
 * Need to be converted to use separate face equations and
 * vertex indices
 */
#if 1

#define ModelToViewportUpdate() (BR_FALSE)

#else

/*
 * N.B. Not sure why these are disabled (looks like Sam did it just before
 * 1.3 was released) but if they are ever reenabled, it will be necessary
 * to make them write out C_Q (in floating point, even though it's a fixed
 * point build) if necessary
 */
#define ModelToViewportUpdate					ModelToViewportUpdate_A
#define Vertex_OS_TransformProjectBounds	   	Vertex_OS_TransformProjectBounds_A
#define Vertex_OS_TransformProject				Vertex_OS_TransformProject_A
#define Vertex_OS_TransformProjectBoundsSurf	Vertex_OS_TransformProjectBoundsSurf_A
#define Vertex_OS_TransformProjectSurf			Vertex_OS_TransformProjectSurf_A
#define Vertex_OS_TransformProjectBoundsGeom	Vertex_OS_TransformProjectBoundsGeom_A
#define Vertex_OS_TransformProjectSurfGeom		Vertex_OS_TransformProjectSurfGeom_A

#endif

#endif

#ifdef __cplusplus
};
#endif
#endif

