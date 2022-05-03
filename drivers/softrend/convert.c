/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: convert.c 1.2 1997/12/17 13:09:16 jon Exp $
 * $Locker: $
 *
 * Primitive renderer filters that will convert vertex components to desired destination format
 */
#include <stddef.h>
#include <string.h>

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: convert.c 1.2 1997/12/17 13:09:16 jon Exp $");

#if BASED_FIXED
#define FCONV	(float)BrFixedToFloat
#define XCONV	(br_int_32)
#define ICONV	(br_int_32)BrFixedToInt
#endif

#if BASED_FLOAT
#define FCONV	(float)
#define XCONV	BrFloatToFixed
#define ICONV	(br_int_32)
#endif

void ConvertVertex(brp_vertex *dest, brp_vertex *src)
{
	int c;
	br_uint_32 m;

#if BASED_FIXED
	br_boolean has_q = (rend.block->constant_components | rend.block->vertex_components) & CM_Q;

	if (has_q)
		m = rend.block->convert_mask_f & ~(1 << C_Q);
	else
#endif
	m = rend.block->convert_mask_f;
	if(m)
		for(c = 0; m; c++, m >>=1)
			if(m & 1)
				dest[0].comp_f[c] = FCONV(src->comp[c]);

#if BASED_FIXED
	if (has_q)
		m = rend.block->convert_mask_x & ~(1 << C_Q);
	else
#endif
	m = rend.block->convert_mask_x;
	if(m)
		for(c = 0; m; c++, m >>=1)
			if(m & 1)
				dest[0].comp_x[c] = XCONV(src->comp[c]);

#if BASED_FIXED
	if (has_q)
		m = rend.block->convert_mask_i & ~(1 << C_Q);
	else
#endif
	m = rend.block->convert_mask_i;
	if (m)
		for(c = 0; m; c++, m >>=1)
			if(m & 1)
				dest[0].comp_i[c] = ICONV(src->comp[c]);

#if BASED_FIXED
	/*
 	 * Special case - in fixed point, Q is stored as a float
	 */
	if (has_q)

		if (rend.block->convert_mask_x & (1 << C_Q))
			dest[0].comp_x[C_Q] = BrFloatToScalar(src[0].comp_f[C_Q]);

		else if (rend.block->convert_mask_i & (1 << C_Q))
			dest[0].comp_i[C_Q] = (br_int_32)(src[0].comp_f[C_Q]);

		else if (rend.block->convert_mask_f & (1 << C_Q))
			dest[0].comp_f[C_Q] = src[0].comp_f[C_Q];
#endif

	if(m = 0x0000ffff ^ (rend.block->convert_mask_i | rend.block->convert_mask_x | rend.block->convert_mask_f))
		for(c = 0; m; c++, m >>=1)
			if(m & 1)
				dest[0].comp[c] = src->comp[c];

}

void BR_ASM_CALL RenderConvert1(struct brp_block *block,
	brp_vertex *v0)
{
	int c;
	br_uint_32 m;
	brp_vertex outv[1];

#if BASED_FIXED
	br_boolean has_q = (rend.block->constant_components | rend.block->vertex_components) & CM_Q;

	if (has_q)
		m = rend.block->convert_mask_f & ~(1 << C_Q);
	else
#endif
	m = rend.block->convert_mask_f;
	if(m)
		for(c = 0; m; c++, m >>=1)
			if(m & 1)
				outv[0].comp_f[c] = FCONV(v0->comp[c]);

#if BASED_FIXED
	if (has_q)
		m = rend.block->convert_mask_x & ~(1 << C_Q);
	else
#endif
	m = rend.block->convert_mask_x;
	if(m)
		for(c = 0; m; c++, m >>=1)
			if(m & 1)
				outv[0].comp_x[c] = XCONV(v0->comp[c]);

#if BASED_FIXED
	if (has_q)
		m = rend.block->convert_mask_i & ~(1 << C_Q);
	else
#endif
	m = rend.block->convert_mask_i;
	if (m)
		for(c = 0; m; c++, m >>=1)
			if(m & 1)
				outv[0].comp_i[c] = ICONV(v0->comp[c]);

#if BASED_FIXED
	/*
 	 * Special case - in fixed point, Q is stored as a float
	 */
	if (has_q)

		if (rend.block->convert_mask_x & (1 << C_Q))
			outv[0].comp_x[C_Q] = BrFloatToScalar(v0->comp_f[C_Q]);

		else if (rend.block->convert_mask_i & (1 << C_Q))
			outv[0].comp_i[C_Q] = (br_int_32)(v0->comp_f[C_Q]);

		else if (rend.block->convert_mask_f & (1 << C_Q))
			outv[0].comp_f[C_Q] = v0->comp_f[C_Q];
#endif

	if(m=0x0000ffff^(rend.block->convert_mask_i|rend.block->convert_mask_x|rend.block->convert_mask_f))
		for(c = 0; m; c++, m >>=1)
			if(m & 1)
                                outv[0].comp_i[c] = v0->comp_i[c];

	block->chain->render(block->chain, outv);
}

void BR_ASM_CALL RenderConvert2(struct brp_block *block,
	brp_vertex *v0, brp_vertex *v1)
{
	int c;
	br_uint_32 m;
	brp_vertex outv[2];

#if BASED_FIXED
	br_boolean has_q = (rend.block->constant_components | rend.block->vertex_components) & CM_Q;

	if (has_q)
		m = rend.block->convert_mask_f & ~(1 << C_Q);
	else
#endif
	m = rend.block->convert_mask_f;
	if(m)
		for(c = 0; m; c++, m >>=1)
			if(m & 1) {
				outv[0].comp_f[c] = FCONV(v0->comp[c]);
				outv[1].comp_f[c] = FCONV(v1->comp[c]);
			}

#if BASED_FIXED
	if (has_q)
		m = rend.block->convert_mask_x & ~(1 << C_Q);
	else
#endif
	m = rend.block->convert_mask_x;
	if(m)
		for(c = 0; m; c++, m >>=1)
			if(m & 1) {
				outv[0].comp_x[c] = XCONV(v0->comp[c]);
				outv[1].comp_x[c] = XCONV(v1->comp[c]);
			}

#if BASED_FIXED
	if (has_q)
		m = rend.block->convert_mask_i & ~(1 << C_Q);
	else
#endif
	m = rend.block->convert_mask_i;
	if (m)
		for(c = 0; m; c++, m >>=1)
			if(m & 1) {
				outv[0].comp_i[c] = ICONV(v0->comp[c]);
				outv[1].comp_i[c] = ICONV(v1->comp[c]);
			}

#if BASED_FIXED
	/*
 	 * Special case - in fixed point, Q is stored as a float
	 */
	if (has_q)

		if (rend.block->convert_mask_x & (1 << C_Q)) {
			outv[0].comp_x[C_Q] = BrFloatToScalar(v0->comp_f[C_Q]);
			outv[1].comp_x[C_Q] = BrFloatToScalar(v1->comp_f[C_Q]);
		}

		else if (rend.block->convert_mask_i & (1 << C_Q)) {
			outv[0].comp_i[C_Q] = (br_int_32)(v0->comp_f[C_Q]);
			outv[1].comp_i[C_Q] = (br_int_32)(v1->comp_f[C_Q]);
		}

		else if (rend.block->convert_mask_f & (1 << C_Q)) {
			outv[0].comp_f[C_Q] = v0->comp_f[C_Q];
			outv[1].comp_f[C_Q] = v1->comp_f[C_Q];
		}
#endif

	if(m=0x0000ffff^(rend.block->convert_mask_i|rend.block->convert_mask_x|rend.block->convert_mask_f))
		for(c = 0; m; c++, m >>=1)
			if(m & 1) {
                                outv[0].comp_i[c] = v0->comp_i[c];
                                outv[1].comp_i[c] = v1->comp_i[c];
			}

	block->chain->render(block->chain, outv+0, outv+1);
}

void BR_ASM_CALL RenderConvert3(struct brp_block *block,
	brp_vertex *v0, brp_vertex *v1,brp_vertex *v2)
{
	int c;
	br_uint_32 m;
	brp_vertex outv[3];

#if BASED_FIXED
	br_boolean has_q = (rend.block->constant_components | rend.block->vertex_components) & CM_Q;

	if (has_q)
		m = rend.block->convert_mask_f & ~(1 << C_Q);
	else
#endif
	m = rend.block->convert_mask_f;
	if(m)
		for(c = 0; m; c++, m >>=1)
			if(m & 1) {
				outv[0].comp_f[c] = FCONV(v0->comp[c]);
				outv[1].comp_f[c] = FCONV(v1->comp[c]);
				outv[2].comp_f[c] = FCONV(v2->comp[c]);
			}

#if BASED_FIXED
	if (has_q)
		m = rend.block->convert_mask_x & ~(1 << C_Q);
	else
#endif
	m = rend.block->convert_mask_x;
	if(m)
		for(c = 0; m; c++, m >>=1)
			if(m & 1) {
				outv[0].comp_x[c] = XCONV(v0->comp[c]);
				outv[1].comp_x[c] = XCONV(v1->comp[c]);
				outv[2].comp_x[c] = XCONV(v2->comp[c]);
			}

#if BASED_FIXED
	if (has_q)
		m = rend.block->convert_mask_i & ~(1 << C_Q);
	else
#endif
	m = rend.block->convert_mask_i;
	if (m)
		for(c = 0; m; c++, m >>=1)
			if(m & 1) {
				outv[0].comp_i[c] = ICONV(v0->comp[c]);
				outv[1].comp_i[c] = ICONV(v1->comp[c]);
				outv[2].comp_i[c] = ICONV(v2->comp[c]);
			}

#if BASED_FIXED
	/*
 	 * Special case - in fixed point, Q is stored as a float
	 */
	if (has_q)

		if (rend.block->convert_mask_x & (1 << C_Q)) {
			outv[0].comp_x[C_Q] = BrFloatToScalar(v0->comp_f[C_Q]);
			outv[1].comp_x[C_Q] = BrFloatToScalar(v1->comp_f[C_Q]);
			outv[2].comp_x[C_Q] = BrFloatToScalar(v2->comp_f[C_Q]);
		}

		else if (rend.block->convert_mask_i & (1 << C_Q)) {
			outv[0].comp_i[C_Q] = (br_int_32)(v0->comp_f[C_Q]);
			outv[1].comp_i[C_Q] = (br_int_32)(v1->comp_f[C_Q]);
			outv[2].comp_i[C_Q] = (br_int_32)(v2->comp_f[C_Q]);
		}

		else if (rend.block->convert_mask_f & (1 << C_Q)) {
			outv[0].comp_f[C_Q] = v0->comp_f[C_Q];
			outv[1].comp_f[C_Q] = v1->comp_f[C_Q];
			outv[2].comp_f[C_Q] = v2->comp_f[C_Q];
		}
#endif

	if(m=0x0000ffff^(rend.block->convert_mask_i|rend.block->convert_mask_x|rend.block->convert_mask_f))
		for(c = 0; m; c++, m >>=1)
			if(m & 1) {
                                outv[0].comp_i[c] = v0->comp_i[c];
                                outv[1].comp_i[c] = v1->comp_i[c];
                                outv[2].comp_i[c] = v2->comp_i[c];
			}

	block->chain->render(block->chain, outv+0, outv+1, outv+2);
}

void BR_ASM_CALL RenderConvert4(struct brp_block *block,
	brp_vertex *v0, brp_vertex *v1,brp_vertex *v2,brp_vertex *v3)
{
	int c;
	br_uint_32 m;
	brp_vertex outv[4];

#if BASED_FIXED
	br_boolean has_q = (rend.block->constant_components | rend.block->vertex_components) & CM_Q;

	if (has_q)
		m = rend.block->convert_mask_f & ~(1 << C_Q);
	else
#endif
	m = rend.block->convert_mask_f;
	if(m)
		for(c = 0; m; c++, m >>=1)
			if(m & 1) {
				outv[0].comp_f[c] = FCONV(v0->comp[c]);
				outv[1].comp_f[c] = FCONV(v1->comp[c]);
				outv[2].comp_f[c] = FCONV(v2->comp[c]);
				outv[3].comp_f[c] = FCONV(v3->comp[c]);
			}

#if BASED_FIXED
	if (has_q)
		m = rend.block->convert_mask_x & ~(1 << C_Q);
	else
#endif
	m = rend.block->convert_mask_x;
	if(m)
		for(c = 0; m; c++, m >>=1)
			if(m & 1) {
				outv[0].comp_x[c] = XCONV(v0->comp[c]);
				outv[1].comp_x[c] = XCONV(v1->comp[c]);
				outv[2].comp_x[c] = XCONV(v2->comp[c]);
				outv[3].comp_x[c] = XCONV(v3->comp[c]);
			}

#if BASED_FIXED
	if (has_q)
		m = rend.block->convert_mask_i & ~(1 << C_Q);
	else
#endif
	m = rend.block->convert_mask_i;
	if (m)
		for(c = 0; m; c++, m >>=1)
			if(m & 1) {
				outv[0].comp_i[c] = ICONV(v0->comp[c]);
				outv[1].comp_i[c] = ICONV(v1->comp[c]);
				outv[2].comp_i[c] = ICONV(v2->comp[c]);
				outv[3].comp_i[c] = ICONV(v3->comp[c]);
			}

#if BASED_FIXED
	/*
 	 * Special case - in fixed point, Q is stored as a float
	 */
	if (has_q)

		if (rend.block->convert_mask_x & (1 << C_Q)) {
			outv[0].comp_x[C_Q] = BrFloatToScalar(v0->comp_f[C_Q]);
			outv[1].comp_x[C_Q] = BrFloatToScalar(v1->comp_f[C_Q]);
			outv[2].comp_x[C_Q] = BrFloatToScalar(v2->comp_f[C_Q]);
			outv[3].comp_x[C_Q] = BrFloatToScalar(v3->comp_f[C_Q]);
		}

		else if (rend.block->convert_mask_i & (1 << C_Q)) {
			outv[0].comp_i[C_Q] = (br_int_32)(v0->comp_f[C_Q]);
			outv[1].comp_i[C_Q] = (br_int_32)(v1->comp_f[C_Q]);
			outv[2].comp_i[C_Q] = (br_int_32)(v2->comp_f[C_Q]);
			outv[3].comp_i[C_Q] = (br_int_32)(v3->comp_f[C_Q]);
		}

		else if (rend.block->convert_mask_f & (1 << C_Q)) {
			outv[0].comp_f[C_Q] = v0->comp_f[C_Q];
			outv[1].comp_f[C_Q] = v1->comp_f[C_Q];
			outv[2].comp_f[C_Q] = v2->comp_f[C_Q];
			outv[3].comp_f[C_Q] = v3->comp_f[C_Q];
		}
#endif

	if(m=0x0000ffff^(rend.block->convert_mask_i|rend.block->convert_mask_x|rend.block->convert_mask_f))
		for(c = 0; m; c++, m >>=1)
			if(m & 1) {
                                outv[0].comp_i[c] = v0->comp_i[c];
                                outv[1].comp_i[c] = v1->comp_i[c];
                                outv[2].comp_i[c] = v2->comp_i[c];
                                outv[3].comp_i[c] = v3->comp_i[c];
			}

	block->chain->render(block->chain, outv+0, outv+1, outv+2, outv+3);
}

