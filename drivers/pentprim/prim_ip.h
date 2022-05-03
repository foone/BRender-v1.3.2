/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: prim_ip.h 1.1 1997/12/10 16:47:40 jon Exp $
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
 * rcp.c
 */
extern unsigned long BR_ASM_DATA _reciprocal[2048];

/*
 * ti8_piz.asm
 */
void BR_ASM_CALL TriangleRenderPIZ2(brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPIZ2I(brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPIZ2T(brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPIZ2TI(brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPIZ2TD(brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPIZ2TID(brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);

/*
 * tt24_piz.asm
 */
void BR_ASM_CALL TriangleRenderPIZ2_RGB_888(brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPIZ2I_RGB_888(brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);


/*
 * tt15_piz.asm
 */
void BR_ASM_CALL TriangleRenderPIZ2_RGB_555(brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPIZ2I_RGB_555(brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);

/*
 * awtmz.c
 */
void BR_ASM_CALL TriangleRenderPIZ2TA(brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TIA(brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TA15(brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TA24(brp_vertex *a,brp_vertex *b,brp_vertex *c);

/*
 * perspz.c
 */
void BR_ASM_CALL TriangleRenderPIZ2TIP1024(brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TIP256(brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TIP64(brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TP1024(brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TP256(brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TP64(brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TPD1024(brp_vertex *a,brp_vertex *b,brp_vertex *c);

/*
 * l_piz.c
 */

void BR_ASM_CALL LineRenderPIZ2I(brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPIZ2T(brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPIZ2TI(brp_vertex *v0,brp_vertex *v1);

void BR_ASM_CALL LineRenderPFZ2I(brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPFZ2I555(brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPFZ2I888(brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPFZ4I(brp_vertex *v0,brp_vertex *v1);

void BR_ASM_CALL LineRenderPIZ2T_RGB_888(brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPIZ2I_RGB_888(brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPIZ2T_RGB_555(brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPIZ2I_RGB_555(brp_vertex *v0,brp_vertex *v1);

/*
 * p_piz.c
 */
void BR_ASM_CALL PointRenderPIZ2(brp_vertex *v0);
void BR_ASM_CALL PointRenderPIZ2T(brp_vertex *v0);
void BR_ASM_CALL PointRenderPIZ2TI(brp_vertex *v0);

void BR_ASM_CALL PointRenderPIZ2_RGB_888(brp_vertex *v0);
void BR_ASM_CALL PointRenderPIZ2T_RGB_888(brp_vertex *v0);

void BR_ASM_CALL PointRenderPIZ2_RGB_555(brp_vertex *v0);
void BR_ASM_CALL PointRenderPIZ2T_RGB_555(brp_vertex *v0);

/*
 * ti8_pi.asm
 */
void BR_ASM_CALL TriangleRenderPI(brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPII(brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPIT(brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPITI(brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPITD(brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPITID(brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);

/*
 * tt24_pi.asm
 */
	/*
	 * 24 bit true colour, perfect, integer
	 */
void BR_ASM_CALL TriangleRenderPI_RGB_888(brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPII_RGB_888(brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);


/*
 * tt15_pi.asm
 */
	/*
	 * 15 bit true colour, perfect, integer
	 */
void BR_ASM_CALL TriangleRenderPI_RGB_555(brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPII_RGB_555(brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);
	
/*
 * persp.c
 */
void BR_ASM_CALL TriangleRenderPITIP1024(brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPITIP256(brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPITIP64(brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPITP1024(brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPITP256(brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPITP64(brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPITPD1024(brp_vertex *a,brp_vertex *b,brp_vertex *c);

/*
 * awtm.c
 */
void BR_ASM_CALL TriangleRenderPITA(brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPITIA(brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPITA15(brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPITA24(brp_vertex *a,brp_vertex *b,brp_vertex *c);

/*
 * l_pi.c
 */
void BR_ASM_CALL LineRenderPII(brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPIT(brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPITI(brp_vertex *v0,brp_vertex *v1);

void BR_ASM_CALL LineRenderPFI(brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPFI555(brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPFI888(brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPFI(brp_vertex *v0,brp_vertex *v1);

void BR_ASM_CALL LineRenderPIT_RGB_888(brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPII_RGB_888(brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPIT_RGB_555(brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPII_RGB_555(brp_vertex *v0,brp_vertex *v1);

/*
 * p_pi.c
 */
void BR_ASM_CALL PointRenderPI(brp_vertex *v0);
void BR_ASM_CALL PointRenderPIT(brp_vertex *v0);
void BR_ASM_CALL PointRenderPITI(brp_vertex *v0);

void BR_ASM_CALL PointRenderPI_RGB_888(brp_vertex *v0);
void BR_ASM_CALL PointRenderPIT_RGB_888(brp_vertex *v0);

void BR_ASM_CALL PointRenderPI_RGB_555(brp_vertex *v0);
void BR_ASM_CALL PointRenderPIT_RGB_555(brp_vertex *v0);

/*
 * safediv.asm
 */
extern int BR_ASM_CALL SafeFixedMac2Div(int,int,int,int,int);

#ifndef __WATCOMC__
extern br_int_32 BR_ASM_CALL _sar16(br_int_32 a);
#endif

/*
 * match.c
 */
brp_info * BR_CALLBACK PrimMatch(struct brp_base *pb, br_int_32 prim_type, struct br_renderer *renderer);

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
brp_base * BR_EXPORT BrPrimBegin(char *args);
void BR_CALLBACK BrPrimEnd(brp_base *pb);

#ifdef __cplusplus
};
#endif

#endif
#endif
