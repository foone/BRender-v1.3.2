/*
 * Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: vesaloop.h 1.1 1997/12/10 16:54:41 jon Exp $
 * $Locker: $
 *
 * VESA specific pixelmap loops
 */
#ifndef _VESALOOPS_H_
#define _VESALOOPS_H_

#ifdef __cplusplus
extern "C" {
#endif


#ifndef _NO_PROTOTYPES
void BR_ASM_CALL VESAWindowCopyToLines(br_uint_8 *mem, br_uint_16 mem_qual, br_uint_32 mem_stride);
void BR_ASM_CALL VESAWindowCopyToLinear(br_uint_8 *mem, br_uint_16 mem_qual, br_uint_32 mem_stride);

void BR_ASM_CALL VESAWindowCopyFromLines(br_uint_8 *mem, br_uint_16 mem_qual, br_uint_32 mem_stride);
void BR_ASM_CALL VESAWindowCopyFromLinear(br_uint_8 *mem, br_uint_16 mem_qual, br_uint_32 mem_stride);

void BR_ASM_CALL VESAWindowRectangleCopyTo(
	br_uint_8 *mem, br_uint_16 mem_qual, br_uint_32 mem_stride,
	br_uint_32 x, br_uint_32 y, br_uint_32 width, br_uint_32 height);

void BR_ASM_CALL VESAWindowBreaksRectangleCopyTo(
	br_uint_8 *mem, br_uint_16 mem_qual, br_uint_32 mem_stride,
	br_uint_32 x, br_uint_32 y, br_uint_32 width,br_uint_32 height);

void BR_ASM_CALL VESAWindowRectangleCopyFrom(
	br_uint_8 *mem, br_uint_16 mem_qual, br_uint_32 mem_stride,
	br_uint_32 x, br_uint_32 y, br_uint_32 width,br_uint_32 height);

void BR_ASM_CALL VESAWindowBreaksRectangleCopyFrom(
	br_uint_8 *mem, br_uint_16 mem_qual, br_uint_32 mem_stride,
	br_uint_32 x, br_uint_32 y, br_uint_32 width,br_uint_32 height);

void BR_ASM_CALL VESAWindowRectangleFill(
	br_uint_32 x, br_uint_32 y, br_uint_32 width,br_uint_32 height,
	br_uint_32 colour);

void BR_ASM_CALL VESAWindowBreaksRectangleFill (
	br_uint_32 x, br_uint_32 y, br_uint_32 width,br_uint_32 height,
	br_uint_32 colour);

 void BR_ASM_CALL VESAWindowPixelSet(br_uint_32 px, br_uint_32 py, br_uint_32 colour);

 void BR_ASM_CALL VESAWindowBreaksPixelSet(br_uint_32 px, br_uint_32 py, br_uint_32 colour);

br_uint_32 BR_ASM_CALL VESAWindowPixelGet(br_uint_32 px, br_uint_32 py);
br_uint_32 BR_ASM_CALL VESAWindowBreaksPixelGet( br_uint_32 px, br_uint_32 py);

 void BR_ASM_CALL VESAWindowCopyBits(
		br_int_32 x,
		br_int_32 y,
		br_uint_8 *src,
		br_uint_32 s_stride,
		br_uint_32 start_bit,
		br_uint_32 end_bit,
               br_uint_32 n_rows,
		br_uint_32 colour);

 void BR_ASM_CALL VESAWindowBreaksCopyBits(
		br_int_32 x,
		br_int_32 y,
		br_uint_8 *src,
		br_uint_32 s_stride,
		br_uint_32 start_bit,
		br_uint_32 end_bit,
               br_uint_32 n_rows,
		br_uint_32 colour);
#endif

#ifdef __cplusplus
};
#endif
#endif

