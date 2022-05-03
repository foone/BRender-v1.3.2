/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * Rendering hooks
 */
#include <math.h>

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"


/* 1/w values don't need to be calculated in non-textured renderers if fogging
 * isn't on. Since this saves 3 divides, even with the test it should be faster */
extern int fog_enabled;





/* Glide Vertex Structures: Notes
 *
 * All modes:					x, y,			0 to screen resolution
 * Gouraud or lit textures:		r, g, b			0 to 255
 * Alpha (blend or test)		a				0 to 255
 * Texture mapping				sow, tow		0-255.0 on primary axis, aspect ratio on secondary, * oow
 * Texture mapping				oow				1.0 for non-perspective, else see below
 * Fogging (by table)			oow				Not Clearly Defined. Doesn't Really Matter I think
 * Z-buffer						ooz				1.0 to 65535.0 (I think)
 *
 * BRender ignores scales for W, which goes from camera hither to yon
 */




/* Fixed vertex snapping: uses knowledge that BRender fixed point is 16.16 */
//#define FAST_SNAP_VERT(x) (BrFixedToFloat((x) & 0xfffff000))

/* Floating point vertex snapping */
// #define SNAP_VERT(v) ((float)((long)((v)*16))/16.0f)

/* Faster floating point vertex snapping: uses prescaling in match ranges */
#define SNAP_RECIP   (0.0625f)

#ifndef WIN32

/* Have to use this for DOS */
#define SNAP_VERT(v) ((float)((long)(v)) * SNAP_RECIP)

#else

/* Even faster; a function that casts to int MUCH faster than MSVC's. This should
 * probably subtract 0.5 to correct the result, but the accuracy is good enough
 * anyway without it and it saves a couple of cycles. */

#pragma warning( disable : 4035 )
__inline int intCast(float f)
{
	int result;

	__asm{
			fld f
			fistp result
			mov eax,dword ptr result
	}
}
#pragma warning( default : 4035 )

#define SNAP_VERT(v) ((float)(intCast(v)) * SNAP_RECIP)

#endif



#define DEPTH_CONVERT(v) ((v)->comp_f[C_SZ])
#define FAST_W_DEPTH_CONVERT(v) ((v)->comp_f[C_Q])
//#define FAST_W_DEPTH_CONVERT(v) (1.0f / ((v)->comp_f[C_W]) )


extern float alpha_value;



void BR_ASM_CALL PointRender(brp_block *block, brp_vertex *a)
{
	GrVertex vtx[1];
	float oow;

	vtx[0].x = SNAP_VERT(a->comp_f[C_SX]);
	vtx[0].y = SNAP_VERT(a->comp_f[C_SY]);
	vtx[0].ooz = DEPTH_CONVERT(a);

	oow = FAST_W_DEPTH_CONVERT(a);
	vtx[0].oow = oow;
	vtx[0].tmuvtx[0].sow = a->comp_f[C_U]*oow;
	vtx[0].tmuvtx[0].tow = a->comp_f[C_V]*oow;

	/* Cover this both ways - inefficient, but simple */
	vtx[0].r = a->comp_f[C_R];
	vtx[0].g = a->comp_f[C_G];
	vtx[0].b = a->comp_f[C_B];
	grConstantColorValue4(alpha_value, a->comp_f[C_R], a->comp_f[C_G], a->comp_f[C_B]);

	grDrawPoint(vtx+0);
}



void BR_ASM_CALL LineRender_Flat(brp_block *block, brp_vertex *a,brp_vertex *b)
{
	GrVertex vtx[2];
	float oow;

	vtx[0].x = SNAP_VERT(a->comp_f[C_SX]);
	vtx[0].y = SNAP_VERT(a->comp_f[C_SY]);
	vtx[0].ooz = DEPTH_CONVERT(a);
	oow = FAST_W_DEPTH_CONVERT(a);
	vtx[0].oow = oow;
	vtx[0].tmuvtx[0].sow = a->comp_f[C_U]*oow;
	vtx[0].tmuvtx[0].tow = a->comp_f[C_V]*oow;


	vtx[1].x = SNAP_VERT(b->comp_f[C_SX]);
	vtx[1].y = SNAP_VERT(b->comp_f[C_SY]);
	vtx[1].ooz = DEPTH_CONVERT(b);
	oow = FAST_W_DEPTH_CONVERT(b);
	vtx[1].oow = oow;
	vtx[1].tmuvtx[0].sow = b->comp_f[C_U]*oow;
	vtx[1].tmuvtx[0].tow = b->comp_f[C_V]*oow;

	grConstantColorValue4(alpha_value, a->comp_f[C_R], a->comp_f[C_G], a->comp_f[C_B]);

	grDrawLine(vtx+0, vtx+1);
}



void BR_ASM_CALL LineRender_Smooth(brp_block *block, brp_vertex *a,brp_vertex *b)
{
	GrVertex vtx[2];
	float oow;
								 
	vtx[0].x = SNAP_VERT(a->comp_f[C_SX]);
	vtx[0].y = SNAP_VERT(a->comp_f[C_SY]);
	vtx[0].ooz = DEPTH_CONVERT(a);
	oow = FAST_W_DEPTH_CONVERT(a);
	vtx[0].oow = oow;
	vtx[0].tmuvtx[0].sow = a->comp_f[C_U]*oow;
	vtx[0].tmuvtx[0].tow = a->comp_f[C_V]*oow;
	vtx[0].r = a->comp_f[C_R];
	vtx[0].g = a->comp_f[C_G];
	vtx[0].b = a->comp_f[C_B];


	vtx[1].x = SNAP_VERT(b->comp_f[C_SX]);
	vtx[1].y = SNAP_VERT(b->comp_f[C_SY]);
	vtx[1].ooz = DEPTH_CONVERT(b);
	oow = FAST_W_DEPTH_CONVERT(b);
	vtx[1].oow = oow;
	vtx[1].tmuvtx[0].sow = b->comp_f[C_U]*oow;
	vtx[1].tmuvtx[0].tow = b->comp_f[C_V]*oow;
	vtx[1].r = b->comp_f[C_R];
	vtx[1].g = b->comp_f[C_G];
	vtx[1].b = b->comp_f[C_B];

	grDrawLine(vtx+0, vtx+1);
}








/*
 * Untextured, flat shaded
 */
void BR_ASM_CALL TriangleRender_Flat(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c)
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
 * Untextured, gouraud shaded
 */
void BR_ASM_CALL TriangleRender_Smooth(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c)
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
 * Textured, unlit
 */
void BR_ASM_CALL TriangleRender_UnlitTextured(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c)
{
	GrVertex vtx[3];
	float oow;								 
								 
	vtx[0].x = SNAP_VERT(a->comp_f[C_SX]);
	vtx[0].y = SNAP_VERT(a->comp_f[C_SY]);
	vtx[0].ooz = DEPTH_CONVERT(a);
	oow = FAST_W_DEPTH_CONVERT(a);
	vtx[0].oow = oow;
	vtx[0].tmuvtx[0].sow = a->comp_f[C_U] * oow;
	vtx[0].tmuvtx[0].tow = a->comp_f[C_V] * oow;

	vtx[1].x = SNAP_VERT(b->comp_f[C_SX]);
	vtx[1].y = SNAP_VERT(b->comp_f[C_SY]);
	vtx[1].ooz = DEPTH_CONVERT(b);
	oow = FAST_W_DEPTH_CONVERT(b);
	vtx[1].oow = oow;
	vtx[1].tmuvtx[0].sow = b->comp_f[C_U] * oow;
	vtx[1].tmuvtx[0].tow = b->comp_f[C_V] * oow;

	vtx[2].x = SNAP_VERT(c->comp_f[C_SX]);
	vtx[2].y = SNAP_VERT(c->comp_f[C_SY]);
	vtx[2].ooz = DEPTH_CONVERT(c);
	oow = FAST_W_DEPTH_CONVERT(c);
	vtx[2].oow = oow;
	vtx[2].tmuvtx[0].sow = c->comp_f[C_U] * oow;
	vtx[2].tmuvtx[0].tow = c->comp_f[C_V] * oow;

	grDrawTriangle(vtx+0, vtx+1, vtx+2);
}



/*
 * Textured, flat shaded
 */
void BR_ASM_CALL TriangleRender_FlatTextured(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c)
{
	GrVertex vtx[3];
	float oow;								 
								 
	vtx[0].x = SNAP_VERT(a->comp_f[C_SX]);
	vtx[0].y = SNAP_VERT(a->comp_f[C_SY]);
	vtx[0].ooz = DEPTH_CONVERT(a);
	oow = FAST_W_DEPTH_CONVERT(a);
	vtx[0].oow = oow;
	vtx[0].tmuvtx[0].sow = a->comp_f[C_U] * oow;
	vtx[0].tmuvtx[0].tow = a->comp_f[C_V] * oow;

	vtx[1].x = SNAP_VERT(b->comp_f[C_SX]);
	vtx[1].y = SNAP_VERT(b->comp_f[C_SY]);
	vtx[1].ooz = DEPTH_CONVERT(b);
	oow = FAST_W_DEPTH_CONVERT(b);
	vtx[1].oow = oow;
	vtx[1].tmuvtx[0].sow = b->comp_f[C_U] * oow;
	vtx[1].tmuvtx[0].tow = b->comp_f[C_V] * oow;

	vtx[2].x = SNAP_VERT(c->comp_f[C_SX]);
	vtx[2].y = SNAP_VERT(c->comp_f[C_SY]);
	vtx[2].ooz = DEPTH_CONVERT(c);
	oow = FAST_W_DEPTH_CONVERT(c);
	vtx[2].oow = oow;
	vtx[2].tmuvtx[0].sow = c->comp_f[C_U] * oow;
	vtx[2].tmuvtx[0].tow = c->comp_f[C_V] * oow;

	grConstantColorValue4(alpha_value, a->comp_f[C_R], a->comp_f[C_G], a->comp_f[C_B]);

	grDrawTriangle(vtx+0, vtx+1, vtx+2);
}












/*
 * Textured, gouraud shaded
 */
#if 0

void BR_ASM_CALL TriangleRender_SmoothTextured(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c)
{
	GrVertex vtx[3];
	float oow;	/* 1/w */
	float oowc;

	oowc = 1.0 / (a->comp_f[C_W] * b->comp_f[C_W] * c->comp_f[C_W]);

	vtx[0].x = SNAP_VERT(a->comp_f[C_SX]);
	vtx[0].y = SNAP_VERT(a->comp_f[C_SY]);
	vtx[0].ooz = DEPTH_CONVERT(a);
	oow = oowc * b->comp_f[C_W] * c->comp_f[C_W];
	vtx[0].oow = oow;
	vtx[0].tmuvtx[0].sow = a->comp_f[C_U] * oow;
	vtx[0].tmuvtx[0].tow = a->comp_f[C_V] * oow;
	vtx[0].r = a->comp_f[C_R];
	vtx[0].g = a->comp_f[C_G];
	vtx[0].b = a->comp_f[C_B];

	vtx[1].x = SNAP_VERT(b->comp_f[C_SX]);
	vtx[1].y = SNAP_VERT(b->comp_f[C_SY]);
	vtx[1].ooz = DEPTH_CONVERT(b);
	oow = oowc * a->comp_f[C_W] * c->comp_f[C_W];
	vtx[1].oow = oow;
	vtx[1].tmuvtx[0].sow = b->comp_f[C_U] * oow;
	vtx[1].tmuvtx[0].tow = b->comp_f[C_V] * oow;
	vtx[1].r = b->comp_f[C_R];
	vtx[1].g = b->comp_f[C_G];
	vtx[1].b = b->comp_f[C_B];

	vtx[2].x = SNAP_VERT(c->comp_f[C_SX]);
	vtx[2].y = SNAP_VERT(c->comp_f[C_SY]);
	vtx[2].ooz = DEPTH_CONVERT(c);
	oow = oowc * a->comp_f[C_W] * b->comp_f[C_W];
	vtx[2].oow = oow;
	vtx[2].tmuvtx[0].sow = c->comp_f[C_U] * oow;
	vtx[2].tmuvtx[0].tow = c->comp_f[C_V] * oow;
	vtx[2].r = c->comp_f[C_R];
	vtx[2].g = c->comp_f[C_G];
	vtx[2].b = c->comp_f[C_B];

	grDrawTriangle(vtx+0, vtx+1, vtx+2);
}

#else

void BR_ASM_CALL TriangleRender_SmoothTextured(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c)
{
	GrVertex vtx[3];
	float oow;	/* 1/w */

	vtx[0].x = SNAP_VERT(a->comp_f[C_SX]);
	vtx[0].y = SNAP_VERT(a->comp_f[C_SY]);
	vtx[0].ooz = DEPTH_CONVERT(a);
	oow = FAST_W_DEPTH_CONVERT(a);
	vtx[0].oow = oow;
	vtx[0].tmuvtx[0].sow = a->comp_f[C_U] * oow;
	vtx[0].tmuvtx[0].tow = a->comp_f[C_V] * oow;
	vtx[0].r = a->comp_f[C_R];
	vtx[0].g = a->comp_f[C_G];
	vtx[0].b = a->comp_f[C_B];

	vtx[1].x = SNAP_VERT(b->comp_f[C_SX]);
	vtx[1].y = SNAP_VERT(b->comp_f[C_SY]);
	vtx[1].ooz = DEPTH_CONVERT(b);
	oow = FAST_W_DEPTH_CONVERT(b);
	vtx[1].oow = oow;
	vtx[1].tmuvtx[0].sow = b->comp_f[C_U] * oow;
	vtx[1].tmuvtx[0].tow = b->comp_f[C_V] * oow;
	vtx[1].r = b->comp_f[C_R];
	vtx[1].g = b->comp_f[C_G];
	vtx[1].b = b->comp_f[C_B];

	vtx[2].x = SNAP_VERT(c->comp_f[C_SX]);
	vtx[2].y = SNAP_VERT(c->comp_f[C_SY]);
	vtx[2].ooz = DEPTH_CONVERT(c);
	oow = FAST_W_DEPTH_CONVERT(c);
	vtx[2].oow = oow;
	vtx[2].tmuvtx[0].sow = c->comp_f[C_U] * oow;
	vtx[2].tmuvtx[0].tow = c->comp_f[C_V] * oow;
	vtx[2].r = c->comp_f[C_R];
	vtx[2].g = c->comp_f[C_G];
	vtx[2].b = c->comp_f[C_B];

	grDrawTriangle(vtx+0, vtx+1, vtx+2);
}

#endif
