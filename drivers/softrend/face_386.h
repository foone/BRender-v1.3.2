/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: face_386.h 1.2 1998/07/20 14:45:11 jon Exp $
 * $Locker: $
 *
 * Prototypes for 386 downcoded per-face operations
 */
#ifndef _FACE_386_H_
#define _FACE_386_H_


#ifdef __cplusplus
extern "C" {
#endif

void BR_ASM_CALL OpTriangleMapQuad_A(struct brp_block *block, union brp_vertex *v0, union brp_vertex *v1, union brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3], br_vector4 *fp_eqn, struct temp_face *tfp);

#define OpTriangleMapQuad					OpTriangleMapQuad_A

#ifdef __cplusplus
};
#endif
#endif

