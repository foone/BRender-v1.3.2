/*
 * Brender triangle rendering functions for ATI 3D Expression
 *
 * Author A. Pomianowski
 *
 * Note that all renderer state setup is performed in match.c
 * Render functions compile vertex lists which are sent to the 
 * card when a FlushPrimitivesATI call occurs (either when the
 * list becomes full, or when a new rendering block comes through)
 *
 * Notes for future work -
 * There is some potential optimisation here - if the driver sits
 * underneath a fixed point Brender app then not much can be
 * done - so much time is spent converting fixed to float that
 * it's pointless. If sitting under float Brender, then these
 * renderer routines may well be a significant bottleneck on
 * performance.
 * Currently this isn't the case - ATI's 3DCIF is a more significant
 * bottleneck on the front end throughput, but if 3DCIF can be
 * further optimised significantly in future, or the RAGE hardware
 * gets faster in later revisions, some work on hand optimised
 * assembler may be in order! The C is about as fast as it can be
 * reasonably made to go. Doing trivial rejection of 0 height/width
 * triangles might give a speedup, but this is only easy in fixed point,
 * not float, so we would again lose our performance due to conversions.
 *
 * I have created special case renderers for most cases where speed
 * can be gained by doing so - this makes the code a bit less readable
 * as there are more triangle renderers, but allows the driver to perform
 * considerably better.
 *
 * With the release of the Rage PRO we might want to reevaluate the
 * optimisation status of these functions...
 */

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <ddraw.h>

#include "drv.h"
#include "brassert.h"


/* Storage for vertices
 * This value decides how many vertices are buffered by the rendering
 * functions before passing them to ATI's rendering library.
 * NOTE - MAX_VERTEX_STORAGE should be a multiple of 3 AND 2
 *
 * We have been informed by ATI that they do not yet have DMA transfer
 * of primitive lists working, so when the command FIFO becomes full
 * RenderPrimList will block. They suggest that having long command
 * lists is a good thing because when the DMA becomes functional we
 * will instantly see improved concurrency if we are using large
 * command lists. In the current version speed differences are marginal,
 * so I have set the lists to be fairly long.
 */

/* Number of triangles buffered is given by MAX_VERTEX_STORAGE /3 */

#define MAX_VERTEX_STORAGE 60


static  C3D_VTCF    vertex[MAX_VERTEX_STORAGE];
C3D_VTCF           *vertex_pointers[MAX_VERTEX_STORAGE];

/* Current vertex counter */

br_uint_32 num_vertices_ready = 0;

/* Macros to get at vertices */
#define NV0 num_vertices_ready
#define NV1 (num_vertices_ready + 1)
#define NV2 (num_vertices_ready + 2)


/* External flags to allow triangle renderers  to perform optimally */
extern br_boolean	ATIfog_enable;
extern br_boolean	ATItex_enable;
extern br_boolean	ATIalpha_enable;
extern float		ATIalpha_value;

extern float  ATIfog_min;
extern float  ATIfog_max;
extern float  ATIfog_const;

/* Optimal(?) macro to calculate fogging values at the vertices
 * ATIfog_const is calculated inside match.c
 */
#define CalcFogCoefficients(a,b)\
   {                            \
   if((a) < ATIfog_min)           \
      (b) = ATIMINFOGVALUE;       \
   else if((a) > ATIfog_max)      \
      (b) = ATIMAXFOGVALUE;       \
   else                         \
      (b) = ATIMINFOGVALUE - (((a)-ATIfog_min) * ATIfog_const);\
   }\

/* Z conversion function for z value munging */
#define ZCONVERT(a,b)  (a) = (b) + 32767


/* Routine to create the pointers into the vertex array */
void InitialiseATIRenderFunctions(void)
{
   br_uint_32 i;

   for(i=0;i<MAX_VERTEX_STORAGE;i++)
      vertex_pointers[i] = &vertex[i];

   num_vertices_ready = 0;
}


/* Routine to flush the vertex data to the 3DCIF renderer */
void FlushPrimitivesATI(void)
{
  if(num_vertices_ready)
      {
      ATI3DCIF_RenderPrimList((C3D_VLIST)vertex_pointers,num_vertices_ready);
      num_vertices_ready = 0;
      }
}




/* Catchall renderer for ATI - LitSmoothPerspective */

void BR_ASM_CALL ATI_LSPT_TriRender(struct brp_block *block,
                                          union brp_vertex *v0,
                                          union brp_vertex *v1,
                                          union brp_vertex *v2)
{
	/* Local cache for speed (should end up in registers) */
	C3D_VTCF *vertex0,*vertex1,*vertex2;

	/* Flush the current primitives if we are out of storage */
	if(num_vertices_ready >= (MAX_VERTEX_STORAGE-3))
		FlushPrimitivesATI();

	/* Get the pointers to the appropriate locations in the vertex array */
	vertex0 = vertex_pointers[NV0];
	vertex1 = vertex0 + 1;
	vertex2 = vertex1 + 1;

	/* vertex x,y and z interleaved - hopefully this enables the compiler to
	 * make intelligent register useage */
	vertex0->x = v0->comp_f[C_SX];
	vertex0->y = v0->comp_f[C_SY];
	ZCONVERT(vertex0->z,v0->comp_f[C_SZ]);
	vertex1->x = v1->comp_f[C_SX];
	vertex1->y = v1->comp_f[C_SY];
	ZCONVERT(vertex1->z,v1->comp_f[C_SZ]);
	vertex2->x = v2->comp_f[C_SX];
	vertex2->y = v2->comp_f[C_SY];
	ZCONVERT(vertex2->z,v2->comp_f[C_SZ]);

	/* Generate homogeneous texture coordinates */
	vertex0->w = 1 / v0->comp_f[C_W];
	vertex0->s = v0->comp_f[C_U] * vertex0->w;
	vertex0->t = v0->comp_f[C_V] * vertex0->w;
	vertex1->w = 1 / v1->comp_f[C_W];
	vertex1->s = v1->comp_f[C_U] * vertex1->w;
	vertex1->t = v1->comp_f[C_V] * vertex1->w;
	vertex2->w = 1 / v2->comp_f[C_W];
	vertex2->s = v2->comp_f[C_U] * vertex2->w;
	vertex2->t = v2->comp_f[C_V] * vertex2->w;

	vertex0->r = v0->comp_f[C_R];
	vertex0->g = v0->comp_f[C_G];
	vertex0->b = v0->comp_f[C_B];
	vertex1->r = v1->comp_f[C_R];
	vertex1->g = v1->comp_f[C_G];
	vertex1->b = v1->comp_f[C_B];
	vertex2->r = v2->comp_f[C_R];
	vertex2->g = v2->comp_f[C_G];
	vertex2->b = v2->comp_f[C_B];

	/* These next lines generate correct values for depth cue
	 * and fog effects in the Alpha component */

	if(ATIfog_enable)
		{
		CalcFogCoefficients(v0->comp_f[C_W],vertex0->a);
		CalcFogCoefficients(v1->comp_f[C_W],vertex1->a);
		CalcFogCoefficients(v2->comp_f[C_W],vertex2->a);
		}
	else if(ATIalpha_enable)
		{
		vertex0->a = ATIalpha_value;
		vertex1->a = ATIalpha_value;
		vertex2->a = ATIalpha_value;
		}

	/* Increment vertex counter */
	num_vertices_ready +=3;
}



/* Lit smooth Linear Textured */

void BR_ASM_CALL ATI_LST_TriRender(struct brp_block *block,
                                          union brp_vertex *v0,
                                          union brp_vertex *v1,
                                          union brp_vertex *v2)
{
	/* Local cache for speed (should end up in registers) */
	C3D_VTCF *vertex0,*vertex1,*vertex2;

	/* Flush the current primitives if we are out of storage */
	if(num_vertices_ready >= (MAX_VERTEX_STORAGE-3))
		FlushPrimitivesATI();

	/* Get the pointers to the appropriate locations in the vertex array */
	vertex0 = vertex_pointers[NV0];
	vertex1 = vertex0 + 1;
	vertex2 = vertex1 + 1;

	/* vertex x,y and z interleaved - hopefully this enables the compiler to
	 * make intelligent register useage */
	vertex0->x = v0->comp_f[C_SX];
	vertex0->y = v0->comp_f[C_SY];
	ZCONVERT(vertex0->z,v0->comp_f[C_SZ]);
	vertex1->x = v1->comp_f[C_SX];
	vertex1->y = v1->comp_f[C_SY];
	ZCONVERT(vertex1->z,v1->comp_f[C_SZ]);
	vertex2->x = v2->comp_f[C_SX];
	vertex2->y = v2->comp_f[C_SY];
	ZCONVERT(vertex2->z,v2->comp_f[C_SZ]);

	/* Generate homogeneous texture coordinates for ATI */
	vertex0->w = 1.0f;
	vertex0->s = v0->comp_f[C_U];
	vertex0->t = v0->comp_f[C_V];
	vertex1->w = 1.0f;
	vertex1->s = v1->comp_f[C_U];
	vertex1->t = v1->comp_f[C_V];
	vertex2->w = 1.0f;
	vertex2->s = v2->comp_f[C_U];
	vertex2->t = v2->comp_f[C_V];

	vertex0->r = v0->comp_f[C_R];
	vertex0->g = v0->comp_f[C_G];
	vertex0->b = v0->comp_f[C_B];
	vertex1->r = v1->comp_f[C_R];
	vertex1->g = v1->comp_f[C_G];
	vertex1->b = v1->comp_f[C_B];
	vertex2->r = v2->comp_f[C_R];
	vertex2->g = v2->comp_f[C_G];
	vertex2->b = v2->comp_f[C_B];

	/* These next lines generate correct values for depth cue
	 * and fog effects in the Alpha component */

	if(ATIfog_enable)
		{
		CalcFogCoefficients(v0->comp_f[C_W],vertex0->a);
		CalcFogCoefficients(v1->comp_f[C_W],vertex1->a);
		CalcFogCoefficients(v2->comp_f[C_W],vertex2->a);
		}
	else if(ATIalpha_enable)
		{
		vertex0->a = ATIalpha_value;
		vertex1->a = ATIalpha_value;
		vertex2->a = ATIalpha_value;
		}

	/* Increment vertex counter */
	num_vertices_ready +=3;
}



/* Lit Smooth untextured triangle renderer */

void BR_ASM_CALL ATI_LS_TriRender(struct brp_block *block,
                                          union brp_vertex *v0,
                                          union brp_vertex *v1,
                                          union brp_vertex *v2)
{
	/* Local cache for speed (should end up in registers) */
	C3D_VTCF *vertex0,*vertex1,*vertex2;

	/* Flush the current primitives if we are out of storage */
	if(num_vertices_ready >= (MAX_VERTEX_STORAGE-3))
		FlushPrimitivesATI();

	/* Get the pointers to the appropriate locations in the vertex array */
	vertex0 = vertex_pointers[NV0];
	vertex1 = vertex0 + 1;
	vertex2 = vertex1 + 1;

	/* vertex x,y and z interleaved - hopefully this enables the compiler to
	 * make intelligent register useage */
	vertex0->x = v0->comp_f[C_SX];
	vertex0->y = v0->comp_f[C_SY];
	ZCONVERT(vertex0->z,v0->comp_f[C_SZ]);
	vertex1->x = v1->comp_f[C_SX];
	vertex1->y = v1->comp_f[C_SY];
	ZCONVERT(vertex1->z,v1->comp_f[C_SZ]);
	vertex2->x = v2->comp_f[C_SX];
	vertex2->y = v2->comp_f[C_SY];
	ZCONVERT(vertex2->z,v2->comp_f[C_SZ]);

	vertex0->r = v0->comp_f[C_R];
	vertex0->g = v0->comp_f[C_G];
	vertex0->b = v0->comp_f[C_B];
	vertex1->r = v1->comp_f[C_R];
	vertex1->g = v1->comp_f[C_G];
	vertex1->b = v1->comp_f[C_B];
	vertex2->r = v2->comp_f[C_R];
	vertex2->g = v2->comp_f[C_G];
	vertex2->b = v2->comp_f[C_B];

	/* These next lines generate correct values for depth cue
	 * and fog effects in the Alpha component */

	if(ATIfog_enable)
		{
		CalcFogCoefficients(v0->comp_f[C_W],vertex0->a);
		CalcFogCoefficients(v1->comp_f[C_W],vertex1->a);
		CalcFogCoefficients(v2->comp_f[C_W],vertex2->a);
		}
	else if(ATIalpha_enable)
		{
		vertex0->a = ATIalpha_value;
		vertex1->a = ATIalpha_value;
		vertex2->a = ATIalpha_value;
		}

	/* Increment vertex counter */
	num_vertices_ready +=3;
}





/* Catchall Flat shaded Triangle Renderer for ATI 3DRage */

void BR_ASM_CALL ATI_LFPT_TriRender(struct brp_block *block,
                                          union brp_vertex *v0,
                                          union brp_vertex *v1,
                                          union brp_vertex *v2)
{
	/* Local cache for speed */
	C3D_VTCF *vertex0,*vertex1,*vertex2;

	/* Flush the current primitives if we are out of storage */
	if(num_vertices_ready >= (MAX_VERTEX_STORAGE-3))
		FlushPrimitivesATI();

	vertex0 = vertex_pointers[NV0];
	vertex1 = vertex0 + 1;
	vertex2 = vertex1 + 1;

	/* vertex x,y and z interleaved - hopefully this enables the compiler to
	 * make intelligent register useage */

  	vertex0->x = v0->comp_f[C_SX];
	vertex0->y = v0->comp_f[C_SY];
	ZCONVERT(vertex0->z,v0->comp_f[C_SZ]);
	vertex1->x = v1->comp_f[C_SX];
	vertex1->y = v1->comp_f[C_SY];
	ZCONVERT(vertex1->z,v1->comp_f[C_SZ]);
	vertex2->x = v2->comp_f[C_SX];
	vertex2->y = v2->comp_f[C_SY];
	ZCONVERT(vertex2->z,v2->comp_f[C_SZ]);

	/* Generate homogeneous texture coordinates for ATI */
	vertex0->w = 1 / v0->comp_f[C_W];
	vertex0->s = v0->comp_f[C_U] * vertex0->w;
	vertex0->t = v0->comp_f[C_V] * vertex0->w;
	vertex1->w = 1 / v1->comp_f[C_W];
	vertex1->s = v1->comp_f[C_U] * vertex1->w;
	vertex1->t = v1->comp_f[C_V] * vertex1->w;
	vertex2->w = 1 / v2->comp_f[C_W];
	vertex2->s = v2->comp_f[C_U] * vertex2->w;
	vertex2->t = v2->comp_f[C_V] * vertex2->w;

	/* Only need to set the colours of the last vertex in flat shade mode */
	vertex2->r = v0->comp_f[C_R];
	vertex2->g = v0->comp_f[C_G];
	vertex2->b = v0->comp_f[C_B];

	/* These next lines generate correct values for depth cue
	 * and fog effects in the Alpha component, only one of fog or
	 * alpha is allowed at any one time */

	if(ATIfog_enable)
		{
		/* Need to set colours for all vertices because when the ATI is
		 * fogging in flat shaded mode, the hardware is actually placed
		 * in a smooth shaded mode, otherwise it won't interpolate
		 * fog values properly.
		 */
		vertex0->r = v0->comp_f[C_R];
		vertex1->r = v0->comp_f[C_R];
		vertex0->g = v0->comp_f[C_G];
		vertex1->g = v0->comp_f[C_G];
 		vertex0->b = v0->comp_f[C_B];
		vertex1->b = v0->comp_f[C_B];
		CalcFogCoefficients(v0->comp_f[C_W],vertex0->a);
		CalcFogCoefficients(v1->comp_f[C_W],vertex1->a);
		CalcFogCoefficients(v2->comp_f[C_W],vertex2->a);
		}
	else if(ATIalpha_enable)
		{
		vertex0->r = v0->comp_f[C_R];
		vertex1->r = v0->comp_f[C_R];
		vertex0->g = v0->comp_f[C_G];
		vertex1->g = v0->comp_f[C_G];
 		vertex0->b = v0->comp_f[C_B];
		vertex1->b = v0->comp_f[C_B];

		vertex0->a = ATIalpha_value;
		vertex1->a = ATIalpha_value;
		vertex2->a = ATIalpha_value;
		}

	/* Increment vertex counter */
	num_vertices_ready +=3;

}




/* Lit Flat shaded linear textured */

void BR_ASM_CALL ATI_LFT_TriRender(struct brp_block *block,
                                          union brp_vertex *v0,
                                          union brp_vertex *v1,
                                          union brp_vertex *v2)
{
	/* Local cache for speed */
	C3D_VTCF *vertex0,*vertex1,*vertex2;

	/* Flush the current primitives if we are out of storage */
	if(num_vertices_ready >= (MAX_VERTEX_STORAGE-3))
		FlushPrimitivesATI();

	vertex0 = vertex_pointers[NV0];
	vertex1 = vertex0 + 1;
	vertex2 = vertex1 + 1;

	/* vertex x,y and z interleaved - hopefully this enables the compiler to
	 * make intelligent register useage */

  	vertex0->x = v0->comp_f[C_SX];
	vertex0->y = v0->comp_f[C_SY];
	ZCONVERT(vertex0->z,v0->comp_f[C_SZ]);
	vertex1->x = v1->comp_f[C_SX];
	vertex1->y = v1->comp_f[C_SY];
	ZCONVERT(vertex1->z,v1->comp_f[C_SZ]);
	vertex2->x = v2->comp_f[C_SX];
	vertex2->y = v2->comp_f[C_SY];
	ZCONVERT(vertex2->z,v2->comp_f[C_SZ]);

	/* Generate homogeneous texture coordinates for ATI */
	vertex0->w = 1.0f;
	vertex0->s = v0->comp_f[C_U];
	vertex0->t = v0->comp_f[C_V];
	vertex1->w = 1.0f;
	vertex1->s = v1->comp_f[C_U];
	vertex1->t = v1->comp_f[C_V];
	vertex2->w = 1.0f;
	vertex2->s = v2->comp_f[C_U];
	vertex2->t = v2->comp_f[C_V];

	vertex2->r = v0->comp_f[C_R];
	vertex2->g = v0->comp_f[C_G];
	vertex2->b = v0->comp_f[C_B];

	/* These next lines generate correct values for depth cue
	 * and fog effects in the Alpha component, only one of fog or
	 * alpha is allowed at any one time */

	if(ATIfog_enable)
		{
		vertex0->r = v0->comp_f[C_R];
		vertex1->r = v0->comp_f[C_R];
		vertex0->g = v0->comp_f[C_G];
		vertex1->g = v0->comp_f[C_G];
		vertex0->b = v0->comp_f[C_B];
		vertex1->b = v0->comp_f[C_B];

		CalcFogCoefficients(v0->comp_f[C_W],vertex0->a);
		CalcFogCoefficients(v1->comp_f[C_W],vertex1->a);
		CalcFogCoefficients(v2->comp_f[C_W],vertex2->a);
		}
	else if(ATIalpha_enable)
		{
		vertex0->r = v0->comp_f[C_R];
		vertex1->r = v0->comp_f[C_R];
		vertex0->g = v0->comp_f[C_G];
		vertex1->g = v0->comp_f[C_G];
 		vertex0->b = v0->comp_f[C_B];
		vertex1->b = v0->comp_f[C_B];

		vertex0->a = ATIalpha_value;
		vertex1->a = ATIalpha_value;
		vertex2->a = ATIalpha_value;
		}

	/* Increment vertex counter */
	num_vertices_ready +=3;

}




/* Lit Flat shaded untextured */

void BR_ASM_CALL ATI_LF_TriRender(struct brp_block *block,
                                          union brp_vertex *v0,
                                          union brp_vertex *v1,
                                          union brp_vertex *v2)
{
	/* Local cache for speed */
	C3D_VTCF *vertex0,*vertex1,*vertex2;

	/* Flush the current primitives if we are out of storage */
	if(num_vertices_ready >= (MAX_VERTEX_STORAGE-3))
		FlushPrimitivesATI();

	vertex0 = vertex_pointers[NV0];
	vertex1 = vertex0 + 1;
	vertex2 = vertex1 + 1;

	/* vertex x,y and z interleaved - hopefully this enables the compiler to
	 * make intelligent register useage */

  	vertex0->x = v0->comp_f[C_SX];
	vertex0->y = v0->comp_f[C_SY];
	ZCONVERT(vertex0->z,v0->comp_f[C_SZ]);
	vertex1->x = v1->comp_f[C_SX];
	vertex1->y = v1->comp_f[C_SY];
	ZCONVERT(vertex1->z,v1->comp_f[C_SZ]);
	vertex2->x = v2->comp_f[C_SX];
	vertex2->y = v2->comp_f[C_SY];
	ZCONVERT(vertex2->z,v2->comp_f[C_SZ]);

	vertex2->r = v0->comp_f[C_R];
	vertex2->g = v0->comp_f[C_G];
	vertex2->b = v0->comp_f[C_B];

	/* These next lines generate correct values for depth cue
	 * and fog effects in the Alpha component, only one of fog or
	 * alpha is allowed at any one time */

	if(ATIfog_enable)
		{
		vertex0->r = v0->comp_f[C_R];
		vertex1->r = v0->comp_f[C_R];
  		vertex0->g = v0->comp_f[C_G];
		vertex1->g = v0->comp_f[C_G];
 		vertex0->b = v0->comp_f[C_B];
		vertex1->b = v0->comp_f[C_B];

		CalcFogCoefficients(v0->comp_f[C_W],vertex0->a);
		CalcFogCoefficients(v1->comp_f[C_W],vertex1->a);
		CalcFogCoefficients(v2->comp_f[C_W],vertex2->a);
		}
	else if(ATIalpha_enable)
		{
		vertex0->r = v0->comp_f[C_R];
		vertex1->r = v0->comp_f[C_R];
		vertex0->g = v0->comp_f[C_G];
		vertex1->g = v0->comp_f[C_G];
 		vertex0->b = v0->comp_f[C_B];
		vertex1->b = v0->comp_f[C_B];

		vertex0->a = ATIalpha_value;
		vertex1->a = ATIalpha_value;
		vertex2->a = ATIalpha_value;
		}

	/* Increment vertex counter */
	num_vertices_ready += 3;
}





/* Unlit Perspective textured triangle renderer */

void BR_ASM_CALL ATI_ULPT_TriRender(struct brp_block *block,
                                    union brp_vertex *v0,
                                    union brp_vertex *v1,
                                    union brp_vertex *v2)
{
	/* Local cache for speed */
	C3D_VTCF *vertex0,*vertex1,*vertex2;

	/* Flush the current primitives if we are out of storage */
	if(num_vertices_ready >= (MAX_VERTEX_STORAGE-3))
		FlushPrimitivesATI();

	vertex0 = vertex_pointers[NV0];
	vertex1 = vertex0 + 1;
	vertex2 = vertex1 + 1;

	/* vertex x,y and z interleaved - hopefully this enables the compiler to
	 * make intelligent register useage */
	vertex0->x = v0->comp_f[C_SX];
	vertex0->y = v0->comp_f[C_SY];
	ZCONVERT(vertex0->z,v0->comp_f[C_SZ]);
	vertex1->x = v1->comp_f[C_SX];
	vertex1->y = v1->comp_f[C_SY];
	ZCONVERT(vertex1->z,v1->comp_f[C_SZ]);
	vertex2->x = v2->comp_f[C_SX];
	vertex2->y = v2->comp_f[C_SY];
	ZCONVERT(vertex2->z,v2->comp_f[C_SZ]);

	/* Generate homogeneous texture coordinates for ATI */
	vertex0->w = 1 / v0->comp_f[C_W];
	vertex0->s = v0->comp_f[C_U] * vertex0->w;
	vertex0->t = v0->comp_f[C_V] * vertex0->w;
	vertex1->w = 1 / v1->comp_f[C_W];
	vertex1->s = v1->comp_f[C_U] * vertex1->w;
	vertex1->t = v1->comp_f[C_V] * vertex1->w;
	vertex2->w = 1 / v2->comp_f[C_W];
	vertex2->s = v2->comp_f[C_U] * vertex2->w;
	vertex2->t = v2->comp_f[C_V] * vertex2->w;


	/* These next lines generate correct values for depth cue
	 * and fog effects in the Alpha component, only one of fog or
	 * alpha is allowed at any one time */

	if(ATIfog_enable)
		{
		CalcFogCoefficients(v0->comp_f[C_W],vertex0->a);
		CalcFogCoefficients(v1->comp_f[C_W],vertex1->a);
		CalcFogCoefficients(v2->comp_f[C_W],vertex2->a);
		}
	else if(ATIalpha_enable)
		{
		vertex0->a = ATIalpha_value;
		vertex1->a = ATIalpha_value;
		vertex2->a = ATIalpha_value;
		}

	/* Increment vertex counter */
	num_vertices_ready +=3;
}




/* Unlit Linear Textured triangle render */

void BR_ASM_CALL ATI_ULT_TriRender(struct brp_block *block,
                                          union brp_vertex *v0,
                                          union brp_vertex *v1,
                                          union brp_vertex *v2)
{
	/* Local cache for speed */
	C3D_VTCF *vertex0,*vertex1,*vertex2;

	/* Flush the current primitives if we are out of storage */
	if(num_vertices_ready >= (MAX_VERTEX_STORAGE-3))
		FlushPrimitivesATI();

	vertex0 = vertex_pointers[NV0];
	vertex1 = vertex0 + 1;
	vertex2 = vertex1 + 1;

	/* vertex x,y and z interleaved - hopefully this enables the compiler to
	 * make intelligent register useage */
	vertex0->x = v0->comp_f[C_SX];
	vertex0->y = v0->comp_f[C_SY];
	ZCONVERT(vertex0->z,v0->comp_f[C_SZ]);
	vertex1->x = v1->comp_f[C_SX];
	vertex1->y = v1->comp_f[C_SY];
	ZCONVERT(vertex1->z,v1->comp_f[C_SZ]);
	vertex2->x = v2->comp_f[C_SX];
	vertex2->y = v2->comp_f[C_SY];
	ZCONVERT(vertex2->z,v2->comp_f[C_SZ]);

	/* Generate homogeneous texture coordinates for ATI (linear) */
	vertex0->w = 1.0f;
	vertex0->s = v0->comp_f[C_U];
	vertex0->t = v0->comp_f[C_V];
	vertex1->w = 1.0f;
	vertex1->s = v1->comp_f[C_U];
	vertex1->t = v1->comp_f[C_V];
	vertex2->w = 1.0f;
	vertex2->s = v2->comp_f[C_U];
	vertex2->t = v2->comp_f[C_V];


	/* These next lines generate correct values for depth cue
	 * and fog effects in the Alpha component, only one of fog or
	 * alpha is allowed at any one time */

	if(ATIfog_enable)
		{
		CalcFogCoefficients(v0->comp_f[C_W],vertex0->a);
		CalcFogCoefficients(v1->comp_f[C_W],vertex1->a);
		CalcFogCoefficients(v2->comp_f[C_W],vertex2->a);
		}
	else if(ATIalpha_enable)
		{
		vertex0->a = ATIalpha_value;
		vertex1->a = ATIalpha_value;
		vertex2->a = ATIalpha_value;
		}

	/* Increment vertex counter */
	num_vertices_ready +=3;
}




/* Unlit Untextured triangle render */

void BR_ASM_CALL ATI_UL_TriRender(struct brp_block *block,
                                          union brp_vertex *v0,
                                          union brp_vertex *v1,
                                          union brp_vertex *v2)
{
	/* Local cache for speed */
	C3D_VTCF *vertex0,*vertex1,*vertex2;

	/* Flush the current primitives if we are out of storage */
	if(num_vertices_ready >= (MAX_VERTEX_STORAGE-3))
		FlushPrimitivesATI();

	vertex0 = vertex_pointers[NV0];
	vertex1 = vertex0 + 1;
	vertex2 = vertex1 + 1;

	/* vertex x,y and z interleaved - hopefully this enables the compiler to
	 * make intelligent register useage */
  	vertex0->x = v0->comp_f[C_SX];
	vertex0->y = v0->comp_f[C_SY];
	ZCONVERT(vertex0->z,v0->comp_f[C_SZ]);
	vertex1->x = v1->comp_f[C_SX];
	vertex1->y = v1->comp_f[C_SY];
	ZCONVERT(vertex1->z,v1->comp_f[C_SZ]);
	vertex2->x = v2->comp_f[C_SX];
	vertex2->y = v2->comp_f[C_SY];
	ZCONVERT(vertex2->z,v2->comp_f[C_SZ]);

	vertex0->r = v0->comp_f[C_R];
	vertex1->r = v0->comp_f[C_R];
	vertex2->r = v0->comp_f[C_R];
	vertex0->g = v0->comp_f[C_G];
	vertex1->g = v0->comp_f[C_G];
	vertex2->g = v0->comp_f[C_G];
	vertex0->b = v0->comp_f[C_B];
	vertex1->b = v0->comp_f[C_B];
	vertex2->b = v0->comp_f[C_B];

	/* These next lines generate correct values for depth cue
	 * and fog effects in the Alpha component, only one of fog or
	 * alpha is allowed at any one time */

	if(ATIfog_enable)
		{
		CalcFogCoefficients(v0->comp_f[C_W],vertex0->a);
		CalcFogCoefficients(v1->comp_f[C_W],vertex1->a);
		CalcFogCoefficients(v2->comp_f[C_W],vertex2->a);
		}
	else if(ATIalpha_enable)
		{
		vertex0->a = ATIalpha_value;
		vertex1->a = ATIalpha_value;
		vertex2->a = ATIalpha_value;
		}

	/* Increment vertex counter */
	num_vertices_ready +=3;

}












/* Catchall edge style renderer for ATI 3DRage */

void BR_ASM_CALL ATIEdgeRender(struct brp_block *block,
                               union brp_vertex *v0,
                               union brp_vertex *v1)
{
	/* Local cache for speed */
	C3D_VTCF *vertex0,*vertex1;

	/* Flush the current primitives if we are out of storage */
	if(num_vertices_ready >= (MAX_VERTEX_STORAGE-2))
		FlushPrimitivesATI();

	vertex0 = vertex_pointers[NV0];
	vertex1 = vertex0 + 1;

	vertex0->x = v0->comp_f[C_SX];
	vertex1->x = v1->comp_f[C_SX];

	vertex0->y = v0->comp_f[C_SY];
	vertex1->y = v1->comp_f[C_SY];

	ZCONVERT(vertex0->z,v0->comp_f[C_SZ]);
	ZCONVERT(vertex1->z,v1->comp_f[C_SZ]);


	if(ATItex_enable)
		{
		vertex0->w = 1 / v0->comp_f[C_W];
		vertex1->w = 1 / v1->comp_f[C_W];

		vertex0->s = v0->comp_f[C_U] * vertex0->w;
		vertex1->s = v1->comp_f[C_U] * vertex1->w;

		vertex0->t = v0->comp_f[C_V] * vertex0->w;
		vertex1->t = v1->comp_f[C_V] * vertex1->w;
		}

	vertex0->r = v0->comp_f[C_R];
	vertex0->g = v0->comp_f[C_G];
	vertex0->b = v0->comp_f[C_B];
	vertex1->r = v1->comp_f[C_R];
	vertex1->g = v1->comp_f[C_G];
	vertex1->b = v1->comp_f[C_B];

	if(ATIfog_enable)
		{
		CalcFogCoefficients(v0->comp_f[C_W],vertex0->a);
		CalcFogCoefficients(v1->comp_f[C_W],vertex1->a);
		}
	else if(ATIalpha_enable)
		{
		vertex0->a = ATIalpha_value;
		vertex1->a = ATIalpha_value;
		}
	num_vertices_ready += 2;
}






/* Catchall point style renderer for ATI 3DRage */

void BR_ASM_CALL ATIPointRender(struct brp_block *block,
                                union brp_vertex *v0)
{
	/* Local cache for speed */
	C3D_VTCF *vertex0,*vertex1;

	/* Flush the current primitives if we are out of storage */
	if(num_vertices_ready >= (MAX_VERTEX_STORAGE-2))
		FlushPrimitivesATI();

	vertex0 = vertex_pointers[NV0];
	vertex1 = vertex0 + 1;

	vertex0->x = v0->comp_f[C_SX];
	vertex0->y = v0->comp_f[C_SY];
	ZCONVERT(vertex0->z,v0->comp_f[C_SZ]);

	if(ATItex_enable)
		{
		vertex0->w = 1 / v0->comp_f[C_W];
		vertex0->s = v0->comp_f[C_U] * vertex[NV0].w;
		vertex0->t = v0->comp_f[C_V] * vertex[NV0].w;
		}

	vertex0->r = v0->comp_f[C_R];
	vertex0->g = v0->comp_f[C_G];
	vertex0->b = v0->comp_f[C_B];


	if(ATIfog_enable)
		{
		CalcFogCoefficients(v0->comp_f[C_W],vertex[NV0].a);
		}
	else if(ATIalpha_enable)
		vertex->a = ATIalpha_value;

	*vertex1 = *vertex0;
	vertex1->x += 1;
	vertex1->y += 1;

	num_vertices_ready += 2;
}








/* Benchmarking procedures */
#if 0

#define SafeSetState(a,b,c)  ((ATI3DCIF_ContextSetState(a,b,c) == C3D_EC_OK) ? syslog("SetstateOK") : syslog("3DCIF Error"))


void SetRAGE3DHardware(void)
{
   ATI3DCIF_RenderBegin(hRenderContext);
}


void Set3DCIFGouraud(void)
{
   C3D_ESHADE      shade_mode       = C3D_ESH_SMOOTH;
   C3D_HTX         tex_handle       = NULL;
   C3D_ETEXFILTER  filter_mode      = C3D_ETFILT_MINPNT_MAGPNT;
   C3D_ETEXOP      texel_op         = C3D_ETEXOP_NONE;
   C3D_ETLIGHT     texel_light_mode = C3D_ETL_MODULATE;
   C3D_ETPERSPCOR  perspective_mode = C3D_ETPC_ONE;
   C3D_EVERTEX     vertex_format    = C3D_EV_VTCF;
   C3D_EASRC       src_alpha_mode   = C3D_EASRC_ONE;
   C3D_EADST       dst_alpha_mode   = C3D_EADST_ZERO;
   C3D_EPRIM       primitive_type   = C3D_EPRIM_TRI;
   C3D_COLOR       fog_colour       = {0,0,0,0};
   br_boolean      fog_enable       = 0;
   br_boolean      tex_enable       = 0;


   ATIfog_enable   = BR_FALSE;
   ATItex_enable   = BR_FALSE;
   ATIalpha_enable = BR_FALSE;
   ATIalpha_value  = 0;


   SafeSetState(hRenderContext,C3D_ERS_FG_CLR,          &fog_colour);
   SafeSetState(hRenderContext,C3D_ERS_TMAP_FILTER,     &filter_mode);
   SafeSetState(hRenderContext,C3D_ERS_TMAP_EN,         &tex_enable);
   SafeSetState(hRenderContext,C3D_ERS_TMAP_SELECT,     &tex_handle);
   SafeSetState(hRenderContext,C3D_ERS_VERTEX_TYPE,     &vertex_format);
   SafeSetState(hRenderContext,C3D_ERS_PRIM_TYPE,       &primitive_type);
   SafeSetState(hRenderContext,C3D_ERS_SHADE_MODE,      &shade_mode);
   SafeSetState(hRenderContext,C3D_ERS_TMAP_LIGHT,      &texel_light_mode);
   SafeSetState(hRenderContext,C3D_ERS_TMAP_FILTER,     &filter_mode);
   SafeSetState(hRenderContext,C3D_ERS_TMAP_PERSP_COR,  &perspective_mode);
   SafeSetState(hRenderContext,C3D_ERS_TMAP_TEXOP,      &texel_op);
   SafeSetState(hRenderContext,C3D_ERS_ALPHA_SRC,       &src_alpha_mode);
   SafeSetState(hRenderContext,C3D_ERS_ALPHA_DST,       &dst_alpha_mode);
   SafeSetState(hRenderContext,C3D_ERS_FOG_EN,          &fog_enable);
}

void Set3DCIFTexturedFlat(void)
{
   C3D_ESHADE      shade_mode       = C3D_ESH_SMOOTH;
   C3D_HTX         tex_handle       = NULL;
   C3D_ETEXFILTER  filter_mode      = C3D_ETFILT_MINPNT_MAGPNT;
   C3D_ETEXOP      texel_op         = C3D_ETEXOP_NONE;
   C3D_ETLIGHT     texel_light_mode = C3D_ETL_MODULATE;
   C3D_ETPERSPCOR  perspective_mode = C3D_ETPC_ONE;
   C3D_EVERTEX     vertex_format    = C3D_EV_VTCF;
   C3D_EASRC       src_alpha_mode   = C3D_EASRC_ONE;
   C3D_EADST       dst_alpha_mode   = C3D_EADST_ZERO;
   C3D_EPRIM       primitive_type   = C3D_EPRIM_TRI;
   C3D_COLOR       fog_colour       = {0,0,0,0};
   br_boolean      fog_enable       = 0;
   br_boolean      tex_enable       = 0;

   ATIfog_enable   = BR_FALSE;
   ATItex_enable   = BR_FALSE;
   ATIalpha_enable = BR_FALSE;
   ATIalpha_value  = 0;

   SafeSetState(hRenderContext,C3D_ERS_FG_CLR,          &fog_colour);
   SafeSetState(hRenderContext,C3D_ERS_TMAP_FILTER,     &filter_mode);
   SafeSetState(hRenderContext,C3D_ERS_TMAP_EN,         &tex_enable);
   SafeSetState(hRenderContext,C3D_ERS_TMAP_SELECT,     &tex_handle);
   SafeSetState(hRenderContext,C3D_ERS_VERTEX_TYPE,     &vertex_format);
   SafeSetState(hRenderContext,C3D_ERS_PRIM_TYPE,       &primitive_type);
   SafeSetState(hRenderContext,C3D_ERS_SHADE_MODE,      &shade_mode);
   SafeSetState(hRenderContext,C3D_ERS_TMAP_LIGHT,      &texel_light_mode);
   SafeSetState(hRenderContext,C3D_ERS_TMAP_FILTER,     &filter_mode);
   SafeSetState(hRenderContext,C3D_ERS_TMAP_PERSP_COR,  &perspective_mode);
   SafeSetState(hRenderContext,C3D_ERS_TMAP_TEXOP,      &texel_op);
   SafeSetState(hRenderContext,C3D_ERS_ALPHA_SRC,       &src_alpha_mode);
   SafeSetState(hRenderContext,C3D_ERS_ALPHA_DST,       &dst_alpha_mode);
   SafeSetState(hRenderContext,C3D_ERS_FOG_EN,          &fog_enable);
}

void Set3DCIFTexturedGouraud(void)
{
   C3D_ESHADE      shade_mode       = C3D_ESH_SMOOTH;
   C3D_HTX         tex_handle       = NULL;
   C3D_ETEXFILTER  filter_mode      = C3D_ETFILT_MINPNT_MAGPNT;
   C3D_ETEXOP      texel_op         = C3D_ETEXOP_NONE;
   C3D_ETLIGHT     texel_light_mode = C3D_ETL_MODULATE;
   C3D_ETPERSPCOR  perspective_mode = C3D_ETPC_ONE;
   C3D_EVERTEX     vertex_format    = C3D_EV_VTCF;
   C3D_EASRC       src_alpha_mode   = C3D_EASRC_ONE;
   C3D_EADST       dst_alpha_mode   = C3D_EADST_ZERO;
   C3D_EPRIM       primitive_type   = C3D_EPRIM_TRI;
   C3D_COLOR       fog_colour       = {0,0,0,0};
   br_boolean      fog_enable       = 0;
   br_boolean      tex_enable       = 0;

   ATIfog_enable   = BR_FALSE;
   ATItex_enable   = BR_FALSE;
   ATIalpha_enable = BR_FALSE;
   ATIalpha_value  = 0;

   SafeSetState(hRenderContext,C3D_ERS_FG_CLR,          &fog_colour);
   SafeSetState(hRenderContext,C3D_ERS_TMAP_FILTER,     &filter_mode);
   SafeSetState(hRenderContext,C3D_ERS_TMAP_EN,         &tex_enable);
   SafeSetState(hRenderContext,C3D_ERS_TMAP_SELECT,     &tex_handle);
   SafeSetState(hRenderContext,C3D_ERS_VERTEX_TYPE,     &vertex_format);
   SafeSetState(hRenderContext,C3D_ERS_PRIM_TYPE,       &primitive_type);
   SafeSetState(hRenderContext,C3D_ERS_SHADE_MODE,      &shade_mode);
   SafeSetState(hRenderContext,C3D_ERS_TMAP_LIGHT,      &texel_light_mode);
   SafeSetState(hRenderContext,C3D_ERS_TMAP_FILTER,     &filter_mode);
   SafeSetState(hRenderContext,C3D_ERS_TMAP_PERSP_COR,  &perspective_mode);
   SafeSetState(hRenderContext,C3D_ERS_TMAP_TEXOP,      &texel_op);
   SafeSetState(hRenderContext,C3D_ERS_ALPHA_SRC,       &src_alpha_mode);
   SafeSetState(hRenderContext,C3D_ERS_ALPHA_DST,       &dst_alpha_mode);
   SafeSetState(hRenderContext,C3D_ERS_FOG_EN,          &fog_enable);
}


void SetRAGE2DHardware(void)
{
   ATI3DCIF_RenderEnd();
}




#endif
