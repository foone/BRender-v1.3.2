/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: mmx_ip.h 1.1 1997/12/10 16:47:23 jon Exp $
 * $Locker: $
 *
 * Prototypes for functions internal to driver related to MMX code
 */
#ifndef _MMX_IP_H_
#define _MMX_IP_H_

#ifndef NO_PROTOTYPES

#ifdef __cplusplus
extern "C" {
#endif

#if USE_MMX
/*
 * rastrise.asm
 */
void BR_ASM_CALL RasteriseBufferBegin(void);
void BR_ASM_CALL RasteriseBufferEnd(void);
void * BR_ASM_CALL RasteriseBufferAllocate(br_size_t param_size,br_size_t work_size);
void BR_ASM_CALL RasteriseBufferFlush(void);
void BR_ASM_CALL RasteriseBufferDone(void);
void BR_ASM_CALL RasteriseBufferDisable(void);

#else
#define RasteriseBufferBegin()
#define RasteriseBufferEnd()
#define RasteriseBufferFlush()
#endif

/*
 * gsetuptf.asm
 */
void BR_ASM_CALL GenericSetupTriangleFloat_A(
				brp_block *block,
				brp_vertex *v0,
				brp_vertex *v1,
				brp_vertex *v2);

void BR_ASM_CALL GenericSetupFloatZRGBUVS_A(void);
void BR_ASM_CALL GenericSetupFloatZRGBUV_A(void);
void BR_ASM_CALL GenericSetupFloatZRGB_A(void);
void BR_ASM_CALL GenericSetupFloatZ_A(void);

void BR_ASM_CALL GenericSetupFloatZRGBS_A(void);

void BR_ASM_CALL GenericSetupFloatZUVC_A(void);
void BR_ASM_CALL GenericSetupFloatZUV_A(void);

void BR_ASM_CALL GenericSetupFloatZC_A(void);

/*
 * gsetuptx.asm
 */
void BR_ASM_CALL GenericSetupTriangleFixed_A(
				brp_block *block,
				brp_vertex *v0,
				brp_vertex *v1,
				brp_vertex *v2);

void BR_ASM_CALL GenericSetupFixedZRGBUVS_A(void);
void BR_ASM_CALL GenericSetupFixedZRGBUV_A(void);
void BR_ASM_CALL GenericSetupFixedZRGB_A(void);
void BR_ASM_CALL GenericSetupFixedZ_A(void);

void BR_ASM_CALL GenericSetupFixedZRGBS_A(void);

void BR_ASM_CALL GenericSetupFixedZUVC_A(void);
void BR_ASM_CALL GenericSetupFixedZUV_A(void);

void BR_ASM_CALL GenericSetupFixedZC_A(void);

/*********************
 * Misc. rasterisers *
 *********************/

/*
 * 16 Bit
 */
void BR_ASM_CALL RasteriseMMX555_Z_RGB_LR(void);
void BR_ASM_CALL RasteriseMMX555_Z_RGB_RL(void);

void BR_ASM_CALL RasteriseMMX555_Z_RGB_S_LR(void);
void BR_ASM_CALL RasteriseMMX555_Z_RGB_S_RL(void);

void BR_ASM_CALL RasteriseMMX555_Z_RGB_D_LR(void);
void BR_ASM_CALL RasteriseMMX555_Z_RGB_D_RL(void);

void BR_ASM_CALL RasteriseMMX555_Z_RGB_SD_LR(void);
void BR_ASM_CALL RasteriseMMX555_Z_RGB_SD_RL(void);

void BR_ASM_CALL RasteriseMMX555_Z_UV_LR(void);
void BR_ASM_CALL RasteriseMMX555_Z_UV_RL(void);

void BR_ASM_CALL RasteriseMMX555_Z_UV_S_LR(void);
void BR_ASM_CALL RasteriseMMX555_Z_UV_S_RL(void);

void BR_ASM_CALL RasteriseMMX555_Z_UV_D_LR(void);
void BR_ASM_CALL RasteriseMMX555_Z_UV_D_RL(void);

void BR_ASM_CALL RasteriseMMX555_Z_UV_SD_LR(void);
void BR_ASM_CALL RasteriseMMX555_Z_UV_SD_RL(void);

void BR_ASM_CALL RasteriseMMX555_Z_UV_C_LR(void);
void BR_ASM_CALL RasteriseMMX555_Z_UV_C_RL(void);

void BR_ASM_CALL RasteriseMMX555_Z_UV_C_S_LR(void);
void BR_ASM_CALL RasteriseMMX555_Z_UV_C_S_RL(void);

void BR_ASM_CALL RasteriseMMX555_Z_UV_C_D_LR(void);
void BR_ASM_CALL RasteriseMMX555_Z_UV_C_D_RL(void);

void BR_ASM_CALL RasteriseMMX555_Z_UV_C_SD_LR(void);
void BR_ASM_CALL RasteriseMMX555_Z_UV_C_SD_RL(void);

void BR_ASM_CALL RasteriseMMX555_Z_UV_RGB_LR(void);
void BR_ASM_CALL RasteriseMMX555_Z_UV_RGB_RL(void);

void BR_ASM_CALL RasteriseMMX555_Z_UV_RGB_S_LR(void);
void BR_ASM_CALL RasteriseMMX555_Z_UV_RGB_S_RL(void);

void BR_ASM_CALL RasteriseMMX555_Z_UV_RGB_D_LR(void);
void BR_ASM_CALL RasteriseMMX555_Z_UV_RGB_D_RL(void);

void BR_ASM_CALL RasteriseMMX555_Z_UV_RGB_SD_LR(void);
void BR_ASM_CALL RasteriseMMX555_Z_UV_RGB_SD_RL(void);


/*
 * 16 Bit
 */

void BR_ASM_CALL RasteriseMMX565_Z_RGB_LR(void);
void BR_ASM_CALL RasteriseMMX565_Z_RGB_RL(void);

void BR_ASM_CALL RasteriseMMX565_Z_RGB_S_LR(void);
void BR_ASM_CALL RasteriseMMX565_Z_RGB_S_RL(void);

void BR_ASM_CALL RasteriseMMX565_Z_RGB_D_LR(void);
void BR_ASM_CALL RasteriseMMX565_Z_RGB_D_RL(void);

void BR_ASM_CALL RasteriseMMX565_Z_RGB_SD_LR(void);
void BR_ASM_CALL RasteriseMMX565_Z_RGB_SD_RL(void);

void BR_ASM_CALL RasteriseMMX565_Z_UV_LR(void);
void BR_ASM_CALL RasteriseMMX565_Z_UV_RL(void);

void BR_ASM_CALL RasteriseMMX565_Z_UV_S_LR(void);
void BR_ASM_CALL RasteriseMMX565_Z_UV_S_RL(void);

void BR_ASM_CALL RasteriseMMX565_Z_UV_D_LR(void);
void BR_ASM_CALL RasteriseMMX565_Z_UV_D_RL(void);

void BR_ASM_CALL RasteriseMMX565_Z_UV_SD_LR(void);
void BR_ASM_CALL RasteriseMMX565_Z_UV_SD_RL(void);

void BR_ASM_CALL RasteriseMMX565_Z_UV_C_LR(void);
void BR_ASM_CALL RasteriseMMX565_Z_UV_C_RL(void);

void BR_ASM_CALL RasteriseMMX565_Z_UV_C_S_LR(void);
void BR_ASM_CALL RasteriseMMX565_Z_UV_C_S_RL(void);

void BR_ASM_CALL RasteriseMMX565_Z_UV_C_D_LR(void);
void BR_ASM_CALL RasteriseMMX565_Z_UV_C_D_RL(void);

void BR_ASM_CALL RasteriseMMX565_Z_UV_C_SD_LR(void);
void BR_ASM_CALL RasteriseMMX565_Z_UV_C_SD_RL(void);

void BR_ASM_CALL RasteriseMMX565_Z_UV_RGB_LR(void);
void BR_ASM_CALL RasteriseMMX565_Z_UV_RGB_RL(void);

void BR_ASM_CALL RasteriseMMX565_Z_UV_RGB_S_LR(void);
void BR_ASM_CALL RasteriseMMX565_Z_UV_RGB_S_RL(void);

void BR_ASM_CALL RasteriseMMX565_Z_UV_RGB_D_LR(void);
void BR_ASM_CALL RasteriseMMX565_Z_UV_RGB_D_RL(void);

void BR_ASM_CALL RasteriseMMX565_Z_UV_RGB_SD_LR(void);
void BR_ASM_CALL RasteriseMMX565_Z_UV_RGB_SD_RL(void);

#ifdef __cplusplus
};
#endif

#endif
#endif

