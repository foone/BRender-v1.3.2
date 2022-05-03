/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: drv_ip.h 1.1 1997/12/10 16:46:47 jon Exp $
 * $Locker: $
 *
 * Prototypes for functions internal to driver
 */
#ifndef _DRV_IP_H_
#define _DRV_IP_H_

#ifndef NO_PROTOTYPES

#ifdef __cplusplus
extern "C" {
#endif

/*
 * object.c
 */
char * BR_CMETHOD_DECL(br_object_softprim, identifier)( br_object *self);
br_device *	BR_CMETHOD_DECL(br_object_softprim, device)( br_object *self);

/*
 * device.c
 */
br_device * DeviceSoftPrimAllocate(char *identifier);

/*
 * plib.c
 */
extern const br_token PrimPartsTokens[];
struct br_primitive_library * PrimitiveLibrarySoftAllocate(struct br_device *dev, char * identifier, char *arguments);

/*
 * pstate.c
 */
struct br_primitive_state * PrimitiveStateSoftAllocate(struct br_primitive_library *plib);

/*
 * sbuffer.c
 */
void SetupRenderBuffer(struct render_buffer *rb, br_device_pixelmap *pm);

struct br_buffer_stored * BufferStoredSoftAllocate(struct br_primitive_library *plib,
	br_token use, struct br_device_pixelmap *pm, br_token_value *tv);

/*
 * rcp.c
 */
extern const unsigned long BR_ASM_DATA _reciprocal[2048];

/*
 * ti8_piz.asm
 */
void BR_ASM_CALL TriangleRender_Z_I8_D16(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_Z_I8_D16_ShadeTable(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPIZ2I(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZI_I8_D16_ShadeTable(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZI_I8_D16(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZIF_I8_D16(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZIF_I8_D16_FLAT(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);

void BR_ASM_CALL TriangleRender_ZT_I8_D16_8(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZT_I8_D16_16(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZT_I8_D16_32(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZT_I8_D16_64(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZT_I8_D16_128(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZT_I8_D16_256(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZT_I8_D16_1024(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);

void BR_ASM_CALL TriangleRender_ZTB_I8_D16_8(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZTB_I8_D16_16(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZTB_I8_D16_32(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZTB_I8_D16_64(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZTB_I8_D16_128(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZTB_I8_D16_256(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZTB_I8_D16_1024(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);

void BR_ASM_CALL TriangleRender_T_I8_32(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_T_I8_64(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_T_I8_128(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_T_I8_1024(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);

void BR_ASM_CALL TriangleRender_ZTI_I8_D16(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZTI_I8_D16_8(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZTI_I8_D16_16(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZTI_I8_D16_32(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZTI_I8_D16_64(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZTI_I8_D16_128(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZTI_I8_D16_256(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZTI_I8_D16_1024(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);

void BR_ASM_CALL TriangleRender_ZTIB_I8_D16(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZTIB_I8_D16_8(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZTIB_I8_D16_16(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZTIB_I8_D16_32(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZTIB_I8_D16_64(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZTIB_I8_D16_128(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZTIB_I8_D16_256(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZTIB_I8_D16_1024(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);

void BR_ASM_CALL TriangleRender_ZT_I8_D16_8_FLAT(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZT_I8_D16_16_FLAT(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZT_I8_D16_32_FLAT(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZT_I8_D16_64_FLAT(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZT_I8_D16_128_FLAT(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZT_I8_D16_256_FLAT(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZT_I8_D16_1024_FLAT(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);

void BR_ASM_CALL TriangleRender_ZTB_I8_D16_8_FLAT(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZTB_I8_D16_16_FLAT(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZTB_I8_D16_32_FLAT(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZTB_I8_D16_64_FLAT(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZTB_I8_D16_128_FLAT(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZTB_I8_D16_256_FLAT(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZTB_I8_D16_1024_FLAT(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);

void BR_ASM_CALL TriangleRender_ZTF_I8_D16_256(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZTIF_I8_D16_256(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZTIF_I8_D16_256_FLAT(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPIZ2T(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPIZ2TI(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPIZ2TD(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPIZ2TID(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);

void BR_ASM_CALL TriangleRenderPIZ2TIC(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPIZ2TIB(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);

/*
 * tt24_piz.asm
 */
void BR_ASM_CALL TriangleRenderPIZ2_RGB_888(brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPIZ2I_RGB_888(brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPIZ2TIA_RGB_888(brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPITIA_RGB_888(brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);


/*
 * tt15_piz.asm
 */
void BR_ASM_CALL TriangleRenderPIZ2_RGB_555(brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPIZ2I_RGB_555(brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);

/*
 * tt16_piz.asm
 */
void BR_ASM_CALL TriangleRenderPIZ2_RGB_565(brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPIZ2I_RGB_565(brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);

/*
 * zb16p2ul.asm
 */
void BR_ASM_CALL TriangleRender_ZT_RGB565_D16_1024(brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZT_RGB565_D16_256(brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZT_RGB565_D16_128(brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZT_RGB565_D16_64(brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);
void BR_ASM_CALL TriangleRender_ZT_RGB565_D16_32(brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);

/*
 * awtmz.c
 */
void BR_ASM_CALL TriangleRenderPIZ2TA(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TIA(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TA15(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TA24(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TIA_RGB_555(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TAN(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);

void BR_ASM_CALL TriangleRender_ZTI_I8_D16(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZTI_I8_D16_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZTIF_I8_D16(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZTIF_I8_D16_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZTIB_I8_D16(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZTIB_I8_D16_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZTIFB_I8_D16(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZTIFB_I8_D16_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZT_I8_D16(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZTF_I8_D16(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZTB_I8_D16(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZTFB_I8_D16(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);

/*
 * perspz.c
 */
void BR_ASM_CALL TriangleRenderPIZ2TIP1024(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TIPF1024(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TPD1024(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TP1024(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TPF1024(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TIP256(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TIPF256(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TPD256(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TP256(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TPF256(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TIP128(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TIPF128(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TPD128(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TP128(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TPF128(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TIP64(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TIPF64(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TPD64(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TP64(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TPF64(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TIP256_RGB_555(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TIPB256_RGB_555(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPIZ2TIPB256_RGB_565(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);

void BR_ASM_CALL TriangleRender_ZPTI_I8_D16_32(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTI_I8_D16_32_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPT_I8_D16_32(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTI_I8_D16_64(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTI_I8_D16_64_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPT_I8_D16_64(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTI_I8_D16_128(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTI_I8_D16_128_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPT_I8_D16_128(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTI_I8_D16_256(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTI_I8_D16_256_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPT_I8_D16_256(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTI_I8_D16_1024(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTI_I8_D16_1024_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPT_I8_D16_1024(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTIF_I8_D16_32(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTIF_I8_D16_32_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTF_I8_D16_32(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTIF_I8_D16_64(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTIF_I8_D16_64_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTF_I8_D16_64(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTIF_I8_D16_128(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTIF_I8_D16_128_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTF_I8_D16_128(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTIF_I8_D16_256(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTIF_I8_D16_256_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTF_I8_D16_256(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTIF_I8_D16_1024(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTIF_I8_D16_1024_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTF_I8_D16_1024(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTIB_I8_D16_32(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTIB_I8_D16_32_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTB_I8_D16_32(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTIB_I8_D16_64(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTIB_I8_D16_64_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTB_I8_D16_64(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTIB_I8_D16_128(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTIB_I8_D16_128_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTB_I8_D16_128(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTIB_I8_D16_256(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTIB_I8_D16_256_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTB_I8_D16_256(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTIB_I8_D16_1024(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTIB_I8_D16_1024_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTB_I8_D16_1024(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTIFB_I8_D16_32(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTIFB_I8_D16_32_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTFB_I8_D16_32(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTIFB_I8_D16_64(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTIFB_I8_D16_64_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTFB_I8_D16_64(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTIFB_I8_D16_128(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTIFB_I8_D16_128_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTFB_I8_D16_128(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTIFB_I8_D16_256(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTIFB_I8_D16_256_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTFB_I8_D16_256(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTIFB_I8_D16_1024(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTIFB_I8_D16_1024_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZPTFB_I8_D16_1024(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);

/*
 * l_piz.c
 */

void BR_ASM_CALL LineRenderPIZ2I(brp_block *block, brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPIZ2T(brp_block *block, brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPIZ2TI(brp_block *block, brp_vertex *v0,brp_vertex *v1);

void BR_ASM_CALL LineRenderPFZ2I(brp_block *block, brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPFZ2I555(brp_block *block, brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPFZ2I888(brp_block *block, brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPFZ4I(brp_block *block, brp_vertex *v0,brp_vertex *v1);

void BR_ASM_CALL LineRenderPIZ2T_RGB_888(brp_block *block, brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPIZ2I_RGB_888(brp_block *block, brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPIZ2T_RGB_555(brp_block *block, brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPIZ2I_RGB_555(brp_block *block, brp_vertex *v0,brp_vertex *v1);

void BR_ASM_CALL LineRenderPIZ2I_RGB_565(brp_block *block, brp_vertex *v0,brp_vertex *v1);

/*
 * p_piz.c
 */
void BR_ASM_CALL PointRenderPIZ2(brp_block *block, brp_vertex *v0);
void BR_ASM_CALL PointRenderPIZ2T(brp_block *block, brp_vertex *v0);
void BR_ASM_CALL PointRenderPIZ2TI(brp_block *block, brp_vertex *v0);

void BR_ASM_CALL PointRenderPIZ2_RGB_888(brp_block *block, brp_vertex *v0);
void BR_ASM_CALL PointRenderPIZ2T_RGB_888(brp_block *block, brp_vertex *v0);

void BR_ASM_CALL PointRenderPIZ2_RGB_555(brp_block *block, brp_vertex *v0);
void BR_ASM_CALL PointRenderPIZ2T_RGB_555(brp_block *block, brp_vertex *v0);

void BR_ASM_CALL PointRenderPIZ2_RGB_565(brp_block *block, brp_vertex *v0);

/*
 * ti8_pi.asm
 */
void BR_ASM_CALL TriangleRender_I8(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPII(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_I_I8(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_T_I8(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_T_I8_256(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPIT(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPITI(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_TI_I8_256(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_TI_I8_256_FLAT(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_TID_I8(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_TID_I8_FLAT(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_TI_I8(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_TI_I8_FLAT(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
//void BR_ASM_CALL TriangleRenderPITD(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
//void BR_ASM_CALL TriangleRenderPITID(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_TID_I8_256(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL TriangleRender_TID_I8_256_FLAT(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);

void BR_ASM_CALL TriangleRenderPITIC(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);

/*
 * ti8_pi.asm
 */

void BR_ASM_CALL TriangleRender_I8(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);

/*
 * tt24_pi.asm
 */
	/*
	 * 24 bit true colour, perfect, integer
	 */
void BR_ASM_CALL TriangleRenderPI_RGB_888(brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPII_RGB_888(brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);


/*
 * tt15_pi.asm
 */
	/*
	 * 15 bit true colour, perfect, integer
	 */
void BR_ASM_CALL TriangleRenderPI_RGB_555(brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPII_RGB_555(brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);
	
/*
 * tt16_pi.asm
 */
	/*
	 * 16 bit true colour, perfect, integer
	 */
void BR_ASM_CALL TriangleRenderPI_RGB_565(brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPII_RGB_565(brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);
	
/*
 * persp.c
 */
void BR_ASM_CALL TriangleRenderPITIP1024(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPITPD1024(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPITP1024(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPITIP256(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPITPD256(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPITP256(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPITIP128(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPITPD128(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPITP128(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPITIP64(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPITPD64(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPITP64(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPITIP256_RGB_555(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPITIPB256_RGB_555(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPITIPB256_RGB_565(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);

void BR_ASM_CALL TriangleRender_PT_I8_32(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_PTI_I8_64(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_PTI_I8_64_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_PT_I8_64(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_PTI_I8_128(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_PTI_I8_128_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_PT_I8_128(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_PTI_I8_256(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_PTI_I8_256_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_PT_I8_256(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_PTI_I8_1024(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_PTI_I8_1024_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_PT_I8_1024(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);

/*
 * awtm.c
 */
void BR_ASM_CALL TriangleRenderPITA(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPITIA(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPITA15(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPITIA_RGB_555(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderPITA24(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);

void BR_ASM_CALL TriangleRenderPITAN(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);

void BR_ASM_CALL TriangleRender_TI_I8(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_TI_I8_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_T_I8(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);

/*
 * l_pi.c
 */
void BR_ASM_CALL LineRenderPII(brp_block *block, brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPIT(brp_block *block, brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPITI(brp_block *block, brp_vertex *v0,brp_vertex *v1);

void BR_ASM_CALL LineRenderPFI(brp_block *block, brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPFI555(brp_block *block, brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPFI888(brp_block *block, brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPFI(brp_block *block, brp_vertex *v0,brp_vertex *v1);

void BR_ASM_CALL LineRenderPIT_RGB_888(brp_block *block, brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPII_RGB_888(brp_block *block, brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPIT_RGB_555(brp_block *block, brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderPII_RGB_555(brp_block *block, brp_vertex *v0,brp_vertex *v1);

void BR_ASM_CALL LineRenderPII_RGB_565(brp_block *block, brp_vertex *v0,brp_vertex *v1);

/*
 * p_pi.c
 */
void BR_ASM_CALL PointRenderPI(brp_block *block, brp_vertex *v0);
void BR_ASM_CALL PointRenderPIT(brp_block *block, brp_vertex *v0);
void BR_ASM_CALL PointRenderPITI(brp_block *block, brp_vertex *v0);

void BR_ASM_CALL PointRenderPI_RGB_888(brp_block *block, brp_vertex *v0);
void BR_ASM_CALL PointRenderPIT_RGB_888(brp_block *block, brp_vertex *v0);

void BR_ASM_CALL PointRenderPI_RGB_555(brp_block *block, brp_vertex *v0);
void BR_ASM_CALL PointRenderPIT_RGB_555(brp_block *block, brp_vertex *v0);

void BR_ASM_CALL PointRenderPI_RGB_565(brp_block *block, brp_vertex *v0);

/*
 * decal.c
 */
void BR_ASM_CALL TriangleRenderPITAD(struct brp_block *block, union brp_vertex *v0, union brp_vertex *v1,union brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPITIAD(struct brp_block *block, union brp_vertex *v0, union brp_vertex *v1,union brp_vertex *v2);

void BR_ASM_CALL TriangleRender_TID_I8_256(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_TID_I8_256_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_TID_I8(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_TID_I8_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);

/*
 * decalz.c
 */
void BR_ASM_CALL TriangleRenderPIZ2TAD(struct brp_block *block, union brp_vertex *v0, union brp_vertex *v1,union brp_vertex *v2);
void BR_ASM_CALL TriangleRenderPIZ2TIAD(struct brp_block *block, union brp_vertex *v0, union brp_vertex *v1,union brp_vertex *v2);

void BR_ASM_CALL TriangleRender_ZTID_I8_D16_32(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZTID_I8_D16_64(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZTID_I8_D16_128(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZTID_I8_D16_256(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZTID_I8_D16_1024(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZTID_I8_D16_32_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZTID_I8_D16_64_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZTID_I8_D16_128_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZTID_I8_D16_256_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZTID_I8_D16_1024_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZTID_I8_D16(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_ZTID_I8_D16_FLAT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);

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
br_error BR_CMETHOD_DECL(br_primitive_state_soft, renderBegin)(
		struct br_primitive_state *self,
		struct brp_block **rpb,
		br_boolean *block_changed,
		br_boolean *ranges_changed,
		br_boolean no_render,
		br_token prim_type);

br_error BR_CMETHOD_DECL(br_primitive_state_soft, renderEnd)(
		struct br_primitive_state *self,
		struct brp_block *pb);

void BR_ASM_CALL RenderAutoloadThunk(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL GenericAutoloadThunk(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);

br_error BR_CMETHOD_DECL(br_primitive_state_soft, rangesQueryF)(
		struct br_primitive_state *self,
		br_float *offset,
		br_float *scale,
		br_int_32 max_comp);

br_error BR_CMETHOD_DECL(br_primitive_state_soft, rangesQueryX)(
		struct br_primitive_state *self,
		br_fixed_ls *offset,
		br_fixed_ls *scale,
		br_int_32 max_comp);

void BR_ASM_CALL TriangleRenderNull(brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);

#ifdef __cplusplus
};
#endif

#endif
#endif

