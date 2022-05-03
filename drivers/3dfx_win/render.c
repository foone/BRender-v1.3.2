/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: render.c 1.1 1997/12/10 16:44:56 jon Exp $
 * $Locker: $
 *
 * Rendering hooks
 */
#include <glide.h>
#include <math.h>

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: render.c 1.1 1997/12/10 16:44:56 jon Exp $");


/* 1/w values don't need to be calculated in non-perspective renderers if fogging
 * isn't on. Since this saves 3 divides, even with the test it should be a lot faster */
extern br_boolean fog_enabled;



/* All functions in here are used for depth-buffered and non-depth-buffered cases. */



/* Glide Vertex Structures: Notes
 *
 * All modes:					x, y,			0 to screen resolution
 * Gouraud or lit textures:		r, g, b			0 to 255
 * Alpha (blend or test)		a				0 to 255
 * Texture mapping				sow, tow		0-255.0 on primary axis, aspect ratio on secondary, * oow
 * Texture mapping				oow				1.0 for non-perspective, else see below
 * Fogging (by table)			oow (main)		Not Clearly Defined. Doesn't Really Matter I think
 * Z-buffer						ooz				1.0 to 65535.0 (I think)
 *
 * BRender ignores scales for W, which goes from camera hither to yon
 */


/* Fixed vertex snapping: uses knowledge that BRender fixed point is 16.16 */
//#define FAST_SNAP_VERT(x) (BrFixedToFloat((x) & 0xfffff000))
/* Floating point vertex snapping */
// #define SNAP_VERT(v) ((float)((long)((v)*16))/16.0f)

// Faster floating point vertex snapping: uses prescaling in match ranges
#define SNAP_RECIP   (0.0625f)
#define SNAP_VERT(v) ((float)((long)(v)) * SNAP_RECIP)
/* This could be better but is not faster because ceil is not inlinable. If someone
 * was to effectively write this in asm - any sort of integer truncation would do -
 * it could be a significant speed upgrade. */
//#define SNAP_VERT(v) ((ceil(v)) * SNAP_RECIP)

#define FAST_W_DEPTH_CONVERT(v) (1.0f / ((v)->comp_f[C_W]) )
#define DEPTH_CONVERT(v) ((v)->comp_f[C_SZ])


extern float alpha_value;



void BR_ASM_CALL PointRender(brp_block *block, brp_vertex *a)
{

	GrVertex vtx[1];
								 
	vtx[0].x = SNAP_VERT(a->comp_f[C_SX]);
	vtx[0].y = SNAP_VERT(a->comp_f[C_SY]);

	vtx[0].ooz = DEPTH_CONVERT(a);
	vtx[0].oow = FAST_W_DEPTH_CONVERT(a);

	vtx[0].tmuvtx[0].oow = 1.0f;
	vtx[0].tmuvtx[0].sow = a->comp_f[C_U];
	vtx[0].tmuvtx[0].tow = a->comp_f[C_V];

	vtx[0].r = a->comp_f[C_R];
	vtx[0].g = a->comp_f[C_G];
	vtx[0].b = a->comp_f[C_B];

	grDrawPoint(vtx+0);
}



void BR_ASM_CALL LineRender_Flat_Textured_Begin(brp_block *block)
{
	grTexCombineFunction(0,GR_TEXTURECOMBINE_DECAL);
	guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ITRGB_DELTA0);
}

void BR_ASM_CALL LineRender_Flat_NonTextured_Begin(brp_block *block)
{
	grTexCombineFunction(0,GR_TEXTURECOMBINE_ZERO);
	guColorCombineFunction(GR_COLORCOMBINE_ITRGB_DELTA0);
}


void BR_ASM_CALL LineRender_Flat(brp_block *block, brp_vertex *a,brp_vertex *b)
{
	GrVertex vtx[2];

	vtx[0].x = SNAP_VERT(a->comp_f[C_SX]);
	vtx[0].y = SNAP_VERT(a->comp_f[C_SY]);
	vtx[0].ooz = DEPTH_CONVERT(a);
	vtx[0].oow = FAST_W_DEPTH_CONVERT(a);
	vtx[0].tmuvtx[0].oow = 1.0f;
	vtx[0].tmuvtx[0].sow = a->comp_f[C_U];
	vtx[0].tmuvtx[0].tow = a->comp_f[C_V];

	vtx[1].x = SNAP_VERT(b->comp_f[C_SX]);
	vtx[1].y = SNAP_VERT(b->comp_f[C_SY]);
	vtx[1].ooz = DEPTH_CONVERT(b);
	vtx[1].oow = FAST_W_DEPTH_CONVERT(b);
	vtx[1].tmuvtx[0].oow = 1.0f;
	vtx[1].tmuvtx[0].sow = b->comp_f[C_U];
	vtx[1].tmuvtx[0].tow = b->comp_f[C_V];

	grConstantColorValue4(alpha_value, a->comp_f[C_R], a->comp_f[C_G], a->comp_f[C_B]);

	grDrawLine(vtx+0, vtx+1);
}



void BR_ASM_CALL LineRender_Smooth_Textured_Begin(brp_block *block)
{
	grTexCombineFunction(0,GR_TEXTURECOMBINE_DECAL);
	guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ITRGB);
}

void BR_ASM_CALL LineRender_Smooth_NonTextured_Begin(brp_block *block)
{
	grTexCombineFunction(0,GR_TEXTURECOMBINE_ZERO);
	guColorCombineFunction(GR_COLORCOMBINE_ITRGB);
}


void BR_ASM_CALL LineRender_Smooth(brp_block *block, brp_vertex *a,brp_vertex *b)
{
	GrVertex vtx[2];
								 
	vtx[0].x = SNAP_VERT(a->comp_f[C_SX]);
	vtx[0].y = SNAP_VERT(a->comp_f[C_SY]);
	vtx[0].ooz = DEPTH_CONVERT(a);
	vtx[0].r = a->comp_f[C_R];
	vtx[0].g = a->comp_f[C_G];
	vtx[0].b = a->comp_f[C_B];
	vtx[0].oow = FAST_W_DEPTH_CONVERT(a);
	vtx[0].tmuvtx[0].oow = 1.0f;
	vtx[0].tmuvtx[0].sow = a->comp_f[C_U];
	vtx[0].tmuvtx[0].tow = a->comp_f[C_V];

	vtx[1].x = SNAP_VERT(b->comp_f[C_SX]);
	vtx[1].y = SNAP_VERT(b->comp_f[C_SY]);
	vtx[1].ooz = DEPTH_CONVERT(b);
	vtx[1].r = b->comp_f[C_R];
	vtx[1].g = b->comp_f[C_G];
	vtx[1].b = b->comp_f[C_B];
	vtx[1].oow = FAST_W_DEPTH_CONVERT(b);
	vtx[1].tmuvtx[0].oow = 1.0f;
	vtx[1].tmuvtx[0].sow = b->comp_f[C_U];
	vtx[1].tmuvtx[0].tow = b->comp_f[C_V];

	grDrawLine(vtx+0, vtx+1);
}








/*
 * Depth buffered, Flat shaded
 */
void BR_ASM_CALL TriangleRender_DF_Begin(brp_block *block)
{
	grTexCombineFunction(0,GR_TEXTURECOMBINE_ZERO);
	guColorCombineFunction(GR_COLORCOMBINE_ITRGB_DELTA0);
}

void BR_ASM_CALL TriangleRender_DF(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c)
{
	GrVertex vtx[3];
								 
	vtx[0].x = SNAP_VERT(a->comp_f[C_SX]);
	vtx[0].y = SNAP_VERT(a->comp_f[C_SY]);
	vtx[0].ooz = DEPTH_CONVERT(a);

	vtx[1].x = SNAP_VERT(b->comp_f[C_SX]);
	vtx[1].y = SNAP_VERT(b->comp_f[C_SY]);
	vtx[1].ooz = DEPTH_CONVERT(b);

	vtx[2].x = SNAP_VERT(c->comp_f[C_SX]);
	vtx[2].y = SNAP_VERT(c->comp_f[C_SY]);
	vtx[2].ooz = DEPTH_CONVERT(c);

	if (fog_enabled) {
		vtx[0].oow = FAST_W_DEPTH_CONVERT(a);
		vtx[1].oow = FAST_W_DEPTH_CONVERT(b);
		vtx[2].oow = FAST_W_DEPTH_CONVERT(c);
	}

	grConstantColorValue4(alpha_value, a->comp_f[C_R], a->comp_f[C_G], a->comp_f[C_B]);

	grDrawTriangle(vtx+0, vtx+1, vtx+2);
}

/*
 * Depth buffered, gouraud shaded
 */
void BR_ASM_CALL TriangleRender_DG_Begin(brp_block *block)
{
	grTexCombineFunction(0,GR_TEXTURECOMBINE_ZERO);
	guColorCombineFunction(GR_COLORCOMBINE_ITRGB);
}

void BR_ASM_CALL TriangleRender_DG(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c)
{
	GrVertex vtx[3];

	vtx[0].x = SNAP_VERT(a->comp_f[C_SX]);
	vtx[0].y = SNAP_VERT(a->comp_f[C_SY]);
	vtx[0].ooz = DEPTH_CONVERT(a);

	vtx[0].r = a->comp_f[C_R];
	vtx[0].g = a->comp_f[C_G];
	vtx[0].b = a->comp_f[C_B];


	vtx[1].x = SNAP_VERT(b->comp_f[C_SX]);
	vtx[1].y = SNAP_VERT(b->comp_f[C_SY]);
	vtx[1].ooz = DEPTH_CONVERT(b);

	vtx[1].r = b->comp_f[C_R];
	vtx[1].g = b->comp_f[C_G];
	vtx[1].b = b->comp_f[C_B];

	vtx[2].x = SNAP_VERT(c->comp_f[C_SX]);
	vtx[2].y = SNAP_VERT(c->comp_f[C_SY]);
	vtx[2].ooz = DEPTH_CONVERT(c);

	vtx[2].r = c->comp_f[C_R];
	vtx[2].g = c->comp_f[C_G];
	vtx[2].b = c->comp_f[C_B];

	if (fog_enabled) {
		vtx[0].oow = FAST_W_DEPTH_CONVERT(a);
		vtx[1].oow = FAST_W_DEPTH_CONVERT(b);
		vtx[2].oow = FAST_W_DEPTH_CONVERT(c);
	}

	grDrawTriangle(vtx+0, vtx+1, vtx+2);
}

/*
 * Depth buffered, Textured (affine)
 */
void BR_ASM_CALL TriangleRender_DT_Begin(brp_block *block)
{
	grTexCombineFunction(0,GR_TEXTURECOMBINE_DECAL);
	guColorCombineFunction(GR_COLORCOMBINE_DECAL_TEXTURE);

	grHints(GR_HINT_STWHINT,  GR_STWHINT_W_DIFF_TMU0 );
}

void BR_ASM_CALL TriangleRender_DT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c)
{
	GrVertex vtx[3];
								 
	vtx[0].x = SNAP_VERT(a->comp_f[C_SX]);
	vtx[0].y = SNAP_VERT(a->comp_f[C_SY]);
	vtx[0].ooz = DEPTH_CONVERT(a);
	vtx[0].tmuvtx[0].oow = 1.0f;
	vtx[0].tmuvtx[0].sow = a->comp_f[C_U];
	vtx[0].tmuvtx[0].tow = a->comp_f[C_V];

	vtx[1].x = SNAP_VERT(b->comp_f[C_SX]);
	vtx[1].y = SNAP_VERT(b->comp_f[C_SY]);
	vtx[1].ooz = DEPTH_CONVERT(b);
	vtx[1].tmuvtx[0].oow = 1.0f;
	vtx[1].tmuvtx[0].sow = b->comp_f[C_U];
	vtx[1].tmuvtx[0].tow = b->comp_f[C_V];

	vtx[2].x = SNAP_VERT(c->comp_f[C_SX]);
	vtx[2].y = SNAP_VERT(c->comp_f[C_SY]);
	vtx[2].ooz = DEPTH_CONVERT(c);
	vtx[2].tmuvtx[0].oow = 1.0f;
	vtx[2].tmuvtx[0].sow = c->comp_f[C_U];
	vtx[2].tmuvtx[0].tow = c->comp_f[C_V];

	if (fog_enabled) {
		vtx[0].oow = FAST_W_DEPTH_CONVERT(a);
		vtx[1].oow = FAST_W_DEPTH_CONVERT(b);
		vtx[2].oow = FAST_W_DEPTH_CONVERT(c);
	}

	grDrawTriangle(vtx+0, vtx+1, vtx+2);
}

/*
 * Depth buffered, Textured
 */
void BR_ASM_CALL TriangleRender_DTP_Begin(brp_block *block)
{
	grTexCombineFunction(0,GR_TEXTURECOMBINE_DECAL);
	guColorCombineFunction(GR_COLORCOMBINE_DECAL_TEXTURE);

}

void BR_ASM_CALL TriangleRender_DTP(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c)
{
	GrVertex vtx[3];
	float oow;								 
								 
	vtx[0].x = SNAP_VERT(a->comp_f[C_SX]);
	vtx[0].y = SNAP_VERT(a->comp_f[C_SY]);
	vtx[0].ooz = DEPTH_CONVERT(a);

	oow = 1.0f / a->comp_f[C_W];
	vtx[0].oow = oow;
	vtx[0].tmuvtx[0].oow = oow;
	vtx[0].tmuvtx[0].sow = a->comp_f[C_U] * oow;
	vtx[0].tmuvtx[0].tow = a->comp_f[C_V] * oow;

	vtx[1].x = SNAP_VERT(b->comp_f[C_SX]);
	vtx[1].y = SNAP_VERT(b->comp_f[C_SY]);
	vtx[1].ooz = DEPTH_CONVERT(b);

	oow = 1.0f / b->comp_f[C_W];
	vtx[1].oow = oow;
	vtx[1].tmuvtx[0].oow = oow;
	vtx[1].tmuvtx[0].sow = b->comp_f[C_U] * oow;
	vtx[1].tmuvtx[0].tow = b->comp_f[C_V] * oow;

	vtx[2].x = SNAP_VERT(c->comp_f[C_SX]);
	vtx[2].y = SNAP_VERT(c->comp_f[C_SY]);
	vtx[2].ooz = DEPTH_CONVERT(c);

	oow = 1.0f / c->comp_f[C_W];
	vtx[2].oow = oow;
	vtx[2].tmuvtx[0].oow = oow;
	vtx[2].tmuvtx[0].sow = c->comp_f[C_U] * oow;
	vtx[2].tmuvtx[0].tow = c->comp_f[C_V] * oow;

	grDrawTriangle(vtx+0, vtx+1, vtx+2);
}

/*
 * Depth buffered, Textured
 */
void BR_ASM_CALL TriangleRender_DFTP_Begin(brp_block *block)
{
	grTexCombineFunction(0,GR_TEXTURECOMBINE_DECAL);
	guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ITRGB_DELTA0);
}

void BR_ASM_CALL TriangleRender_DFTP(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c)
{
	GrVertex vtx[3];
	float oow;								 
								 
	vtx[0].x = SNAP_VERT(a->comp_f[C_SX]);
	vtx[0].y = SNAP_VERT(a->comp_f[C_SY]);
	vtx[0].ooz = DEPTH_CONVERT(a);

	oow = 1.0f / a->comp_f[C_W];
	vtx[0].oow = oow;
	vtx[0].tmuvtx[0].oow = oow;
	vtx[0].tmuvtx[0].sow = a->comp_f[C_U] * oow;
	vtx[0].tmuvtx[0].tow = a->comp_f[C_V] * oow;

	vtx[1].x = SNAP_VERT(b->comp_f[C_SX]);
	vtx[1].y = SNAP_VERT(b->comp_f[C_SY]);
	vtx[1].ooz = DEPTH_CONVERT(b);

	oow = 1.0f / b->comp_f[C_W];
	vtx[1].oow = oow;
	vtx[1].tmuvtx[0].oow = oow;
	vtx[1].tmuvtx[0].sow = b->comp_f[C_U] * oow;
	vtx[1].tmuvtx[0].tow = b->comp_f[C_V] * oow;

	vtx[2].x = SNAP_VERT(c->comp_f[C_SX]);
	vtx[2].y = SNAP_VERT(c->comp_f[C_SY]);
	vtx[2].ooz = DEPTH_CONVERT(c);

	oow = 1.0f / c->comp_f[C_W];
	vtx[2].oow = oow;
	vtx[2].tmuvtx[0].oow = oow;
	vtx[2].tmuvtx[0].sow = c->comp_f[C_U] * oow;
	vtx[2].tmuvtx[0].tow = c->comp_f[C_V] * oow;

	grConstantColorValue4(alpha_value, a->comp_f[C_R], a->comp_f[C_G], a->comp_f[C_B]);

	grDrawTriangle(vtx+0, vtx+1, vtx+2);
}

/*
 * Depth buffered, Flat shaded, Textured (affine)
 */
void BR_ASM_CALL TriangleRender_DFT_Begin(brp_block *block)
{
	grTexCombineFunction(0,GR_TEXTURECOMBINE_DECAL);
	guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ITRGB_DELTA0);

	grHints(GR_HINT_STWHINT,  GR_STWHINT_W_DIFF_TMU0 );
}

void BR_ASM_CALL TriangleRender_DFT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c)
{
	GrVertex vtx[3];
								 
	vtx[0].x = SNAP_VERT(a->comp_f[C_SX]);
	vtx[0].y = SNAP_VERT(a->comp_f[C_SY]);
	vtx[0].ooz = DEPTH_CONVERT(a);
	vtx[0].tmuvtx[0].oow = 1.0f;
	vtx[0].tmuvtx[0].sow = a->comp_f[C_U];
	vtx[0].tmuvtx[0].tow = a->comp_f[C_V];

	vtx[1].x = SNAP_VERT(b->comp_f[C_SX]);
	vtx[1].y = SNAP_VERT(b->comp_f[C_SY]);
	vtx[1].ooz = DEPTH_CONVERT(b);
	vtx[1].tmuvtx[0].oow = 1.0f;
	vtx[1].tmuvtx[0].sow = b->comp_f[C_U];
	vtx[1].tmuvtx[0].tow = b->comp_f[C_V];

	vtx[2].x = SNAP_VERT(c->comp_f[C_SX]);
	vtx[2].y = SNAP_VERT(c->comp_f[C_SY]);
	vtx[2].ooz = DEPTH_CONVERT(c);
	vtx[2].tmuvtx[0].oow = 1.0f;
	vtx[2].tmuvtx[0].sow = c->comp_f[C_U];
	vtx[2].tmuvtx[0].tow = c->comp_f[C_V];

	if (fog_enabled) {
		vtx[0].oow = FAST_W_DEPTH_CONVERT(a);
		vtx[1].oow = FAST_W_DEPTH_CONVERT(b);
		vtx[2].oow = FAST_W_DEPTH_CONVERT(c);
	}

	grConstantColorValue4(alpha_value, a->comp_f[C_R], a->comp_f[C_G], a->comp_f[C_B]);

	grDrawTriangle(vtx+0, vtx+1, vtx+2);
}

/*
 * Depth buffered, gouraud shaded, Textured
 */
void BR_ASM_CALL TriangleRender_DGT_Begin(brp_block *block)
{
	grTexCombineFunction(0,GR_TEXTURECOMBINE_DECAL);
	guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ITRGB);

//	grHints(GR_HINT_STWHINT,  GR_STWHINT_W_DIFF_TMU0 | GR_STWHINT_ST_DIFF_TMU0 );
	grHints(GR_HINT_STWHINT,  GR_STWHINT_W_DIFF_TMU0 );
}

void BR_ASM_CALL TriangleRender_DGT(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c)
{
	GrVertex vtx[3];

	vtx[0].x = SNAP_VERT(a->comp_f[C_SX]);
	vtx[0].y = SNAP_VERT(a->comp_f[C_SY]);
	vtx[0].ooz = DEPTH_CONVERT(a);
	vtx[0].r = a->comp_f[C_R];
	vtx[0].g = a->comp_f[C_G];
	vtx[0].b = a->comp_f[C_B];
	vtx[0].tmuvtx[0].oow = 1.0f;
	vtx[0].tmuvtx[0].sow = a->comp_f[C_U];
	vtx[0].tmuvtx[0].tow = a->comp_f[C_V];

	vtx[1].x = SNAP_VERT(b->comp_f[C_SX]);
	vtx[1].y = SNAP_VERT(b->comp_f[C_SY]);
	vtx[1].ooz = DEPTH_CONVERT(b);
	vtx[1].r = b->comp_f[C_R];
	vtx[1].g = b->comp_f[C_G];
	vtx[1].b = b->comp_f[C_B];
	vtx[1].tmuvtx[0].oow = 1.0f;
	vtx[1].tmuvtx[0].sow = b->comp_f[C_U];
	vtx[1].tmuvtx[0].tow = b->comp_f[C_V];

	vtx[2].x = SNAP_VERT(c->comp_f[C_SX]);
	vtx[2].y = SNAP_VERT(c->comp_f[C_SY]);
	vtx[2].ooz = DEPTH_CONVERT(c);
	vtx[2].r = c->comp_f[C_R];
	vtx[2].g = c->comp_f[C_G];
	vtx[2].b = c->comp_f[C_B];
	vtx[2].tmuvtx[0].oow = 1.0f;
	vtx[2].tmuvtx[0].sow = c->comp_f[C_U];
	vtx[2].tmuvtx[0].tow = c->comp_f[C_V];

	if (fog_enabled) {
		vtx[0].oow = FAST_W_DEPTH_CONVERT(a);
		vtx[1].oow = FAST_W_DEPTH_CONVERT(b);
		vtx[2].oow = FAST_W_DEPTH_CONVERT(c);
	}

	grDrawTriangle(vtx+0, vtx+1, vtx+2);
}

/*
 * Depth buffered, gouraud shaded, Textured, Perspective correct
 */
void BR_ASM_CALL TriangleRender_DGTP_Begin(brp_block *block)
{
	grTexCombineFunction(0,GR_TEXTURECOMBINE_DECAL);
	guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ITRGB);

	grHints(GR_HINT_STWHINT,  GR_STWHINT_W_DIFF_FBI | GR_STWHINT_W_DIFF_TMU0 | GR_STWHINT_ST_DIFF_TMU0 );

}

void BR_ASM_CALL TriangleRender_DGTP(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c)
{
	GrVertex vtx[3];
	float oow;	/* 1/w */

	vtx[0].x = SNAP_VERT(a->comp_f[C_SX]);
	vtx[0].y = SNAP_VERT(a->comp_f[C_SY]);
	vtx[0].ooz = DEPTH_CONVERT(a);

	vtx[0].r = a->comp_f[C_R];
	vtx[0].g = a->comp_f[C_G];
	vtx[0].b = a->comp_f[C_B];

	oow = 1.0f / a->comp_f[C_W];
	vtx[0].oow = oow;
	vtx[0].tmuvtx[0].oow = oow;
	vtx[0].tmuvtx[0].sow = a->comp_f[C_U] * oow;
	vtx[0].tmuvtx[0].tow = a->comp_f[C_V] * oow;

	vtx[1].x = SNAP_VERT(b->comp_f[C_SX]);
	vtx[1].y = SNAP_VERT(b->comp_f[C_SY]);
	vtx[1].ooz = DEPTH_CONVERT(b);

	vtx[1].r = b->comp_f[C_R];
	vtx[1].g = b->comp_f[C_G];
	vtx[1].b = b->comp_f[C_B];

	oow = 1.0f / b->comp_f[C_W];
	vtx[1].oow = oow;
	vtx[1].tmuvtx[0].oow = oow;
	vtx[1].tmuvtx[0].sow = b->comp_f[C_U] * oow;
	vtx[1].tmuvtx[0].tow = b->comp_f[C_V] * oow;

	vtx[2].x = SNAP_VERT(c->comp_f[C_SX]);
	vtx[2].y = SNAP_VERT(c->comp_f[C_SY]);
	vtx[2].ooz = DEPTH_CONVERT(c);

	vtx[2].r = c->comp_f[C_R];
	vtx[2].g = c->comp_f[C_G];
	vtx[2].b = c->comp_f[C_B];

	oow = 1.0f / c->comp_f[C_W];
	vtx[2].oow = oow;
	vtx[2].tmuvtx[0].oow = oow;
	vtx[2].tmuvtx[0].sow = c->comp_f[C_U] * oow;
	vtx[2].tmuvtx[0].tow = c->comp_f[C_V] * oow;

	grDrawTriangle(vtx+0, vtx+1, vtx+2);

}

