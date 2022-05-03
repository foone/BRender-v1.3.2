/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: prim_ip.h 1.1 1997/12/10 16:50:32 jon Exp $
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
 * safediv.asm
 */
extern int BR_ASM_CALL SafeFixedMac2Div(int,int,int,int,int);

#ifndef __WATCOMC__
extern br_int_32 BR_ASM_CALL _sar16(br_int_32 a);
#endif


/*
 * rendfunc.c
 */

void BR_ASM_CALL ATI_LSPT_TriRender(struct brp_block *block,brp_vertex *v0,
                                          brp_vertex *v1,brp_vertex *v2);

void BR_ASM_CALL ATI_LST_TriRender(struct brp_block *block,brp_vertex *v0,
                                          brp_vertex *v1,brp_vertex *v2);

void BR_ASM_CALL ATI_LS_TriRender(struct brp_block *block,brp_vertex *v0,
                                          brp_vertex *v1,brp_vertex *v2);

void BR_ASM_CALL ATI_LFPT_TriRender(struct brp_block *block,brp_vertex *v0,
                                          brp_vertex *v1,brp_vertex *v2);

void BR_ASM_CALL ATI_LFT_TriRender(struct brp_block *block,brp_vertex *v0,
                                          brp_vertex *v1,brp_vertex *v2);

void BR_ASM_CALL ATI_LF_TriRender(struct brp_block *block,brp_vertex *v0,
                                          brp_vertex *v1,brp_vertex *v2);

void BR_ASM_CALL ATI_ULPT_TriRender(struct brp_block *block,brp_vertex *v0,
                                          brp_vertex *v1,brp_vertex *v2);

void BR_ASM_CALL ATI_ULT_TriRender(struct brp_block *block,brp_vertex *v0,
                                          brp_vertex *v1,brp_vertex *v2);

void BR_ASM_CALL ATI_UL_TriRender(struct brp_block *block,brp_vertex *v0,
                                          brp_vertex *v1,brp_vertex *v2);

void BR_ASM_CALL ATIEdgeRender(struct brp_block *block,brp_vertex *v0,
                                          brp_vertex *v1);

void BR_ASM_CALL ATIPointRender(struct brp_block *block,brp_vertex *v0);

void FlushPrimitivesATI(void);

void InitialiseATIRenderFunctions(void);




/*
 * match.c
 */
#if 0
brp_info * BR_CALLBACK PrimMatch(struct brp_base *pb, br_int_32 prim_type, struct br_renderer *renderer);
#endif
/*
 * info.c
 */
extern struct local_buffer_info BufferInfo[];
extern struct local_prim_info PrimInfo[];
extern struct brp_base PrimBase;

/*
 * setup.c
 */
br_error BR_ASM_CALL GenericBufferAllocate(
	struct brp_buffer_info *pp,
	void **ppv, br_pixelmap *buffer, br_boolean source_retained, br_token_value *tv);
br_error BR_ASM_CALL GenericBufferUpdate(
	struct brp_buffer_info *pp,
	void **ppv, br_pixelmap *buffer, br_boolean source_retained, br_token_value *tv);
br_error BR_ASM_CALL GenericBufferFree(
	struct brp_buffer_info *pp,
	void **ppv);
br_error BR_ASM_CALL GenericBufferSize(
	struct brp_buffer_info *pp,
	br_int_32 *psize, br_uint_32 *ptotal, br_uint_32 *pavail,
	void **ppv);

br_error BR_ASM_CALL GenericTableAllocate(
	struct brp_buffer_info *pp,
	void **ppv, br_pixelmap *buffer, br_boolean source_retained, br_token_value *tv);
br_error BR_ASM_CALL GenericTableUpdate(
	struct brp_buffer_info *pp,
	void **ppv, br_pixelmap *buffer, br_boolean source_retained, br_token_value *tv);
br_error BR_ASM_CALL GenericTableFree(
	struct brp_buffer_info *pp,
	void **ppv);
br_error BR_ASM_CALL GenericTableSize(
	struct brp_buffer_info *pp,
	br_int_32 *psize, br_uint_32 *ptotal, br_uint_32 *pavail,
	void **ppv);

void BR_ASM_CALL GenericSetBuffers(struct brp_info *pi, void **params);

/*
 * prim.c
 */
#if 0
brp_base * BR_EXPORT BrPrimBegin(char *args);
void BR_CALLBACK BrPrimEnd(brp_base *pb);
#endif

#ifdef __cplusplus
};
#endif

#endif
#endif
