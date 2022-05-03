/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: prim_ip.h 1.1 1997/12/10 16:51:16 jon Exp $
 * $Locker: $
 *
 * Prototypes for functions internal to library
 */
#ifndef _PRIM_IP_H_
#define _PRIM_IP_H_

#ifndef _NO_PROTOTYPES

#ifdef __cplusplus
extern "C" {
#endif


/*
 * rendfunc.c
 */


void FlushPrimitivesS3(void);

void InitialiseS3RenderFunctions(void);


/* Smooth textured triangle renderer for 8 bit modes */ 
void BR_ASM_CALL  TriRender8Smooth(struct brp_block *block,
                                   union brp_vertex *v0,
                                   union brp_vertex *v1,
                                   union brp_vertex *v2);

/* Flat textured triangle renderer for 8 bit modes */ 
void BR_ASM_CALL  TriRender8Flat(struct brp_block *block,
                                   union brp_vertex *v0,
                                   union brp_vertex *v1,
                                   union brp_vertex *v2);

/* Unlit textured triangle renderer for 8 bit modes */ 
void BR_ASM_CALL  TriRender8Unlit(struct brp_block *block,
                                   union brp_vertex *v0,
                                   union brp_vertex *v1,
                                   union brp_vertex *v2);

/* Unlit true colour renderer */
void BR_ASM_CALL  TriRender1624Unlit(struct brp_block *block,
                                    union brp_vertex *v0,
                                    union brp_vertex *v1,
                                    union brp_vertex *v2);

void BR_ASM_CALL  TriRender1624Smooth(struct brp_block *block,
                                    union brp_vertex *v0,
                                    union brp_vertex *v1,
                                    union brp_vertex *v2);

void BR_ASM_CALL  TriRender1624Flat(struct brp_block *block,
                                    union brp_vertex *v0,
                                    union brp_vertex *v1,
                                    union brp_vertex *v2);

void BR_ASM_CALL  EdgeRender8(struct brp_block *block,
                                   union brp_vertex *v0,
                                   union brp_vertex *v1);

void BR_ASM_CALL  EdgeRender1624Smooth(struct brp_block *block,
                                    union brp_vertex *v0,
                                    union brp_vertex *v1);

void BR_ASM_CALL  EdgeRender1624Flat(struct brp_block *block,
                                    union brp_vertex *v0,
                                    union brp_vertex *v1);

void BR_ASM_CALL  PointRender8(struct brp_block *block,
                                   union brp_vertex *v0);


void BR_ASM_CALL  PointRender1624(struct brp_block *block,
                                    union brp_vertex *v0);



/*
 * prim_ip.h
 */


/*
 * Lock a surface; returns address of framebuffer if requested. lock_area
 * is optional; if set to NULL, locks whole surface.
 */
br_boolean S3Lock(LPDIRECTDRAWSURFACE surface, void **framebuffer, RECT *lock_area, long *pitch);

/*
 * Unlocks a surface previously locked by the above function
 */
br_boolean S3Unlock(LPDIRECTDRAWSURFACE surface, void *lock_address);



#ifdef __cplusplus
};
#endif

#endif
#endif
