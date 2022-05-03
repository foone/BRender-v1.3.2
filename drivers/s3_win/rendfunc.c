/*
 * Triangle rendering functions
 * Actual renderer mode at any one time is set up in match.c
 * The job of the rendering functions is basically to perform as little
 * work as possible to render the primitives correctly. Depending on
 * the underlying hardware and library speed it can be _very_ important
 * to have these as optimal as possible. The ZCONVERT macro is necessary
 * because of a problem with BRender floating point Z parameters. Don't
 * be tempted to ask Brender for the lighting coefficients as integers -
 * it works out _much_ faster to convert them yourself at this level.
 */

#include <stdio.h>
#include <stdlib.h>
#include "drv.h"
#include "brassert.h"

extern S3DTK_LPFUNCTIONLIST   g_pS3DTK_Funct;



/* Storage for vertices
 * This value decides how many vertices are buffered by the rendering
 * functions before passing them to S3's rendering library.
 * NOTE - MAX_VERTEX_STORAGE should be a multiple of 3
 */

#define MAX_VERTEX_STORAGE	360
static  S3DTK_VERTEX_TEX    vertex[MAX_VERTEX_STORAGE];
static	S3DTK_VERTEX_TEX    *vertex_pointers[MAX_VERTEX_STORAGE];

static char	dbstr[256];

/* Current vertex counter */

static br_uint_32 num_vertices_ready = 0;

/* Macros to get at vertices */

#define NV0 num_vertices_ready
#define NV1 (num_vertices_ready + 1)
#define NV2 (num_vertices_ready + 2)


/* External flags to allow optimisations in the triangle renderers */
extern br_boolean  S3fog_enable;
extern br_boolean  S3tex_enable;
extern br_uint_8   S3alpha_value;
extern br_boolean  S3rgb_enable;

extern float  S3fog_min;
extern float  S3fog_max;
extern float  S3fog_const;

/* Optimal(?) macro to calculate fogging values at the vertices
 * S3fog_const is calculated inside match.c for optimal performance
 */


#define CalcFogCoefficients(a,b)\
   {                            \
   if((a) < S3fog_min)           \
      (b) = I_S3MINFOGVALUE;       \
   else if((a) > S3fog_max)      \
      (b) = I_S3MAXFOGVALUE;       \
   else                         \
      (b) = FastIntCast((S3MINFOGVALUE - (((a)-S3fog_min) * S3fog_const)));\
   }\


/* Fast ftoi function that casts to int MUCH faster than MSVC casts. This should
 * probably subtract 0.5 to correct the result, but the accuracy is good enough
 * anyway without it and it saves a lot of cycles. */

#pragma warning( disable : 4035 )

__inline int _cdecl FastIntCast(float f)
{
	int result;

	__asm
      {
      fld f
      fistp result
      mov eax,dword ptr result
      }
}

#pragma warning( default : 4035 )


/* Routine to create the pointers into the vertex array */
void InitialiseS3RenderFunctions(void)
{
   br_uint_32 i;

   for(i=0;i<MAX_VERTEX_STORAGE;i++)
      vertex_pointers[i] = &vertex[i];

   num_vertices_ready = 0;
}


/* Routine to flush the vertex data to the renderer */
void FlushPrimitivesS3(void)
{
   if(num_vertices_ready)
      {
      g_pS3DTK_Funct->S3DTK_TriangleSet(g_pS3DTK_Funct,(br_uint_32*)&vertex_pointers,
                                        num_vertices_ready,S3DTK_TRILIST);
      num_vertices_ready = 0;
      }
}


/* Macro to convert float Z values for S3 */
#define CONVERTZ(a,b) (a)->Z = (b) + 32767.0f

/* Unlit triangle renderer for 8 bit modes */ 
void BR_ASM_CALL  TriRender8Unlit(struct brp_block *block,
                             union brp_vertex *v0,
                             union brp_vertex *v1,
                             union brp_vertex *v2)
{
	S3DTK_VERTEX_TEX *vertex0,*vertex1,*vertex2;

	/* Flush the current primitives if we are out of storage */
	if(num_vertices_ready >= (MAX_VERTEX_STORAGE-3))
		FlushPrimitivesS3();

	/* Cache the pointers to the vertices locally for speed */
	vertex0 = vertex_pointers[NV0];
	vertex1 = vertex0 + 1;
	vertex2 = vertex1 + 1;


	/* vertex x,y and z interleaved - hopefully this enables the compiler to
	 * make intelligent register useage */
	vertex0->X = v0->comp_f[C_SX];
	vertex0->Y = v0->comp_f[C_SY];
	CONVERTZ(vertex0,v0->comp_f[C_SZ]);
	vertex1->X = v1->comp_f[C_SX];
	vertex1->Y = v1->comp_f[C_SY];
	CONVERTZ(vertex1,v1->comp_f[C_SZ]);
	vertex2->X = v2->comp_f[C_SX];
	vertex2->Y = v2->comp_f[C_SY];
	CONVERTZ(vertex2,v2->comp_f[C_SZ]);

	/* Texturing really isn't supported on top of shaded primitives...
	 * wierd colour effects will be seen if this is attempted */
	if(S3tex_enable)
		{
		vertex0->W = v0->comp_f[C_W];
		vertex0->U = v0->comp_f[C_U];
		vertex0->V = v0->comp_f[C_V];
		vertex1->W = v1->comp_f[C_W];
		vertex1->U = v1->comp_f[C_U];
		vertex1->V = v1->comp_f[C_V];
		vertex2->W = v2->comp_f[C_W];
		vertex2->U = v2->comp_f[C_U];
		vertex2->V = v2->comp_f[C_V];
		}

	/* S3 picks up colour indices from B component */
	vertex0->B = FastIntCast(v0->comp_f[C_I]);
	vertex1->B = FastIntCast(v1->comp_f[C_I]);
	vertex2->B = FastIntCast(v2->comp_f[C_I]);

	/* Increment vertex counter */
	num_vertices_ready +=3;
}




/* Gouraud triangle renderer for 8 bit modes */ 
void BR_ASM_CALL  TriRender8Smooth(struct brp_block *block,
                             union brp_vertex *v0,
                             union brp_vertex *v1,
                             union brp_vertex *v2)
{
	S3DTK_VERTEX_TEX *vertex0,*vertex1,*vertex2;

	/* Flush the current primitives if we are out of storage */
	if(num_vertices_ready >= (MAX_VERTEX_STORAGE-3))
		FlushPrimitivesS3();

	/* Cache the pointers to the vertices locally for speed */
	vertex0 = vertex_pointers[NV0];
	vertex1 = vertex0 + 1;
	vertex2 = vertex1 + 1;

	/* vertex x,y and z interleaved - hopefully this enables the compiler to
	 * make intelligent register useage */
	vertex0->X = v0->comp_f[C_SX];
	vertex0->Y = v0->comp_f[C_SY];
	CONVERTZ(vertex0,v0->comp_f[C_SZ]);
	vertex1->X = v1->comp_f[C_SX];
	vertex1->Y = v1->comp_f[C_SY];
	CONVERTZ(vertex1,v1->comp_f[C_SZ]);
	vertex2->X = v2->comp_f[C_SX];
	vertex2->Y = v2->comp_f[C_SY];
	CONVERTZ(vertex2,v2->comp_f[C_SZ]);

	/* Texturing isn't supported on top of index shaded primitives...
	 * all textured objects are internally locked to unlit - they
    * will be faster if they are _explicitly_ unlit */

	if(S3tex_enable)
		{
		vertex0->W = v0->comp_f[C_W];
		vertex0->U = v0->comp_f[C_U];
		vertex0->V = v0->comp_f[C_V];
		vertex1->W = v1->comp_f[C_W];
		vertex1->U = v1->comp_f[C_U];
		vertex1->V = v1->comp_f[C_V];
		vertex2->W = v2->comp_f[C_W];
		vertex2->U = v2->comp_f[C_U];
		vertex2->V = v2->comp_f[C_V];
		}

	/* S3 picks up colour indices from B component */
	vertex0->B = FastIntCast(v0->comp_f[C_I]);
	vertex1->B = FastIntCast(v1->comp_f[C_I]);
	vertex2->B = FastIntCast(v2->comp_f[C_I]);

	/* Increment vertex counter */
	num_vertices_ready +=3;
}



/* Flat triangle renderer for 8 bit modes */ 
void BR_ASM_CALL  TriRender8Flat(struct brp_block *block,
                             union brp_vertex *v0,
                             union brp_vertex *v1,
                             union brp_vertex *v2)
{
	S3DTK_VERTEX_TEX *vertex0,*vertex1,*vertex2;

	// Flush the current primitives if we are out of storage
	if(num_vertices_ready >= (MAX_VERTEX_STORAGE-3))
		FlushPrimitivesS3();

	/* Get the pointers to the appropriate locations in the vertex array */
	vertex0 = vertex_pointers[NV0];
	vertex1 = vertex0 + 1;
	vertex2 = vertex1 + 1;

	/* vertex x,y and z interleaved - hopefully this enables the compiler to
	 * make intelligent register useage */
	vertex0->X = v0->comp_f[C_SX];
	vertex0->Y = v0->comp_f[C_SY];
	CONVERTZ(vertex0,v0->comp_f[C_SZ]);
	vertex1->X = v1->comp_f[C_SX];
	vertex1->Y = v1->comp_f[C_SY];
	CONVERTZ(vertex1,v1->comp_f[C_SZ]);
	vertex2->X = v2->comp_f[C_SX];
	vertex2->Y = v2->comp_f[C_SY];
	CONVERTZ(vertex2,v2->comp_f[C_SZ]);

	if(S3tex_enable)
		{
		vertex0->W = v0->comp_f[C_W];
		vertex0->U = v0->comp_f[C_U];
		vertex0->V = v0->comp_f[C_V];
		vertex1->W = v1->comp_f[C_W];
		vertex1->U = v1->comp_f[C_U];
		vertex1->V = v1->comp_f[C_V];
		vertex2->W = v2->comp_f[C_W];
		vertex2->U = v2->comp_f[C_U];
		vertex2->V = v2->comp_f[C_V];
		}

	/* S3 picks up colour indices from B component */
	vertex0->B = vertex1->B = vertex2->B = FastIntCast(v0->comp_f[C_I]);

	/* Increment vertex counter */
	num_vertices_ready +=3;
}



/* Generic maximum speed triangle renderer for unlit textured modes */
void BR_ASM_CALL  TriRender1624Unlit(struct brp_block *block,
								 union brp_vertex *v0,
								 union brp_vertex *v1,
								 union brp_vertex *v2)
{
	S3DTK_VERTEX_TEX *vertex0,*vertex1,*vertex2;
#if 1
	/* Flush the current primitives if we are out of storage */
	if(num_vertices_ready >= (MAX_VERTEX_STORAGE-3))
		FlushPrimitivesS3();

	/* Get the pointers to the appropriate locations in the vertex array */
	vertex0 = vertex_pointers[NV0];
	vertex1 = vertex0 + 1;
	vertex2 = vertex0 + 2;

	vertex0->X = v0->comp_f[C_SX];
	vertex0->Y = v0->comp_f[C_SY];
	vertex1->X = v1->comp_f[C_SX];
	vertex1->Y = v1->comp_f[C_SY];
	vertex2->X = v2->comp_f[C_SX];
	vertex2->Y = v2->comp_f[C_SY];

	CONVERTZ(vertex0,v0->comp_f[C_SZ]);
	CONVERTZ(vertex1,v1->comp_f[C_SZ]);
	CONVERTZ(vertex2,v2->comp_f[C_SZ]);

#if 0
	sprintf(dbstr,"v0x = %f\n",vertex0->X);
	OUTDBS(dbstr);
	sprintf(dbstr,"v1x = %f\n",vertex1->X);
	OUTDBS(dbstr);
	sprintf(dbstr,"v2x = %f\n",vertex2->X);
	OUTDBS(dbstr);
	sprintf(dbstr,"v0y = %f\n",vertex0->Y);
	OUTDBS(dbstr);
	sprintf(dbstr,"v1y = %f\n",vertex1->Y);
	OUTDBS(dbstr);
	sprintf(dbstr,"v2y = %f\n",vertex2->Y);
	OUTDBS(dbstr);
#endif

	if(S3tex_enable)
		{
		vertex0->W = v0->comp_f[C_W];
		vertex0->U = v0->comp_f[C_U];
		vertex0->V = v0->comp_f[C_V];
		vertex1->W = v1->comp_f[C_W];
		vertex1->U = v1->comp_f[C_U];
		vertex1->V = v1->comp_f[C_V];
		vertex2->W = v2->comp_f[C_W];
		vertex2->U = v2->comp_f[C_U];
		vertex2->V = v2->comp_f[C_V];
		}
	// Only need to do rgb for non textured modes
	else if(S3rgb_enable)
      {
	   vertex0->R = FastIntCast(v0->comp_f[C_R]);
	   vertex0->G = FastIntCast(v0->comp_f[C_G]);
	   vertex0->B = FastIntCast(v0->comp_f[C_B]);
	   vertex1->R = vertex0->R;
	   vertex1->G = vertex0->G;
	   vertex1->B = vertex0->B;
	   vertex2->R = vertex0->R;
	   vertex2->G = vertex0->G;
	   vertex2->B = vertex0->B;
      }

	/* These next lines generate correct values for depth cue
	 * and fog effects in the Alpha component (required by S3) */
	if(S3fog_enable)
		{
		CalcFogCoefficients(v0->comp_f[C_W],vertex0->A);
		CalcFogCoefficients(v1->comp_f[C_W],vertex1->A);
		CalcFogCoefficients(v2->comp_f[C_W],vertex2->A);
		}
	else
		{
		vertex0->A = S3alpha_value;
		vertex1->A = S3alpha_value;
		vertex2->A = S3alpha_value;
		}

   /* Increment vertex counter */
   num_vertices_ready +=3;
#endif
}




/* Generic renderer for any smooth shaded triangles */

void BR_ASM_CALL  TriRender1624Smooth(struct brp_block *block,
                                    union brp_vertex *v0,
                                    union brp_vertex *v1,
                                    union brp_vertex *v2)
{
	S3DTK_VERTEX_TEX *vertex0,*vertex1,*vertex2;
#if 1

	// Flush the current primitives if we are out of storage
	if(num_vertices_ready >= (MAX_VERTEX_STORAGE-3))
		FlushPrimitivesS3();

	/* Get the pointers to the appropriate locations in the vertex array */
	vertex0 = vertex_pointers[NV0];
	vertex1 = vertex0 + 1;
	vertex2 = vertex1 + 1;

	/* vertex x,y and z interleaved - hopefully this enables the compiler to
	 * make intelligent register useage */
	vertex0->X = v0->comp_f[C_SX];
	vertex0->Y = v0->comp_f[C_SY];
	CONVERTZ(vertex0,v0->comp_f[C_SZ]);
	vertex1->X = v1->comp_f[C_SX];
	vertex1->Y = v1->comp_f[C_SY];
	CONVERTZ(vertex1,v1->comp_f[C_SZ]);
	vertex2->X = v2->comp_f[C_SX];
	vertex2->Y = v2->comp_f[C_SY];
	CONVERTZ(vertex2,v2->comp_f[C_SZ]);

#if 0
	sprintf(dbstr,"v0x = %f\n",vertex0->X);
	OUTDBS(dbstr);
	sprintf(dbstr,"v1x = %f\n",vertex1->X);
	OUTDBS(dbstr);
	sprintf(dbstr,"v2x = %f\n",vertex2->X);
	OUTDBS(dbstr);
	sprintf(dbstr,"v0y = %f\n",vertex0->Y);
	OUTDBS(dbstr);
	sprintf(dbstr,"v1y = %f\n",vertex1->Y);
	OUTDBS(dbstr);
	sprintf(dbstr,"v2y = %f\n",vertex2->Y);
	OUTDBS(dbstr);
#endif

	if(S3tex_enable)
		{
		vertex0->W = v0->comp_f[C_W];
		vertex0->U = v0->comp_f[C_U];
		vertex0->V = v0->comp_f[C_V];
		vertex1->W = v1->comp_f[C_W];
		vertex1->U = v1->comp_f[C_U];
		vertex1->V = v1->comp_f[C_V];
		vertex2->W = v2->comp_f[C_W];
		vertex2->U = v2->comp_f[C_U];
		vertex2->V = v2->comp_f[C_V];
		}

	if(S3rgb_enable)
		{
		vertex0->R = FastIntCast(v0->comp_f[C_R]);
		vertex0->G = FastIntCast(v0->comp_f[C_G]);
		vertex0->B = FastIntCast(v0->comp_f[C_B]);
		vertex1->R = FastIntCast(v1->comp_f[C_R]);
		vertex1->G = FastIntCast(v1->comp_f[C_G]);
		vertex1->B = FastIntCast(v1->comp_f[C_B]);
		vertex2->R = FastIntCast(v2->comp_f[C_R]);
		vertex2->G = FastIntCast(v2->comp_f[C_G]);
		vertex2->B = FastIntCast(v2->comp_f[C_B]);
		}

	/* These next lines generate correct values for depth cue
	 * and fog effects in the Alpha component (required by S3) */

	if(S3fog_enable)
		{
		CalcFogCoefficients(v0->comp_f[C_W],vertex0->A);
		CalcFogCoefficients(v1->comp_f[C_W],vertex1->A);
		CalcFogCoefficients(v2->comp_f[C_W],vertex2->A);
		}
	else
		{
		vertex0->A = S3alpha_value;
		vertex1->A = S3alpha_value;
		vertex2->A = S3alpha_value;
		}

   /* Increment vertex counter */
   num_vertices_ready +=3;
#endif
}



/* Generic renderer for any flat shaded triangles */

void BR_ASM_CALL  TriRender1624Flat(struct brp_block *block,
                                    union brp_vertex *v0,
                                    union brp_vertex *v1,
                                    union brp_vertex *v2)
{
	S3DTK_VERTEX_TEX *vertex0,*vertex1,*vertex2;
#if 1
	// Flush the current primitives if we are out of storage
	if(num_vertices_ready >= (MAX_VERTEX_STORAGE-3))
		FlushPrimitivesS3();

	/* Get the pointers to the appropriate locations in the vertex array */
	vertex0 = vertex_pointers[NV0];
	vertex1 = vertex0 + 1;
	vertex2 = vertex1 + 1;

	/* vertex x,v and z interleaved - hopefully this enables the compiler to
	 * make intelligent register useage */
	vertex0->X = v0->comp_f[C_SX];
	vertex0->Y = v0->comp_f[C_SY];
	CONVERTZ(vertex0,v0->comp_f[C_SZ]);
	vertex1->X = v1->comp_f[C_SX];
	vertex1->Y = v1->comp_f[C_SY];
	CONVERTZ(vertex1,v1->comp_f[C_SZ]);
	vertex2->X = v2->comp_f[C_SX];
	vertex2->Y = v2->comp_f[C_SY];
	CONVERTZ(vertex2,v2->comp_f[C_SZ]);

#if 0
	sprintf(dbstr,"v0x = %f\n",vertex0->X);
	OUTDBS(dbstr);
	sprintf(dbstr,"v1x = %f\n",vertex1->X);
	OUTDBS(dbstr);
	sprintf(dbstr,"v2x = %f\n",vertex2->X);
	OUTDBS(dbstr);
	sprintf(dbstr,"v0y = %f\n",vertex0->Y);
	OUTDBS(dbstr);
	sprintf(dbstr,"v1y = %f\n",vertex1->Y);
	OUTDBS(dbstr);
	sprintf(dbstr,"v2y = %f\n",vertex2->Y);
	OUTDBS(dbstr);
	
#endif


	if(S3tex_enable)
		{
		vertex0->W = v0->comp_f[C_W];
		vertex0->U = v0->comp_f[C_U];
		vertex0->V = v0->comp_f[C_V];
		vertex1->W = v1->comp_f[C_W];
		vertex1->U = v1->comp_f[C_U];
		vertex1->V = v1->comp_f[C_V];
		vertex2->W = v2->comp_f[C_W];
		vertex2->U = v2->comp_f[C_U];
		vertex2->V = v2->comp_f[C_V];
		}
 
	if(S3rgb_enable)
		{
		vertex0->R = FastIntCast(v0->comp_f[C_R]);
		vertex0->G = FastIntCast(v0->comp_f[C_G]);
		vertex0->B = FastIntCast(v0->comp_f[C_B]);
		vertex1->R = vertex0->R;
		vertex1->G = vertex0->G;
		vertex1->B = vertex0->B;
		vertex2->R = vertex0->R;
		vertex2->G = vertex0->G;
		vertex2->B = vertex0->B;
		}

	/* These next lines generate correct values for depth cue
	 * and fog effects in the Alpha component (required by S3) */

	if(S3fog_enable)
		{
		CalcFogCoefficients(v0->comp_f[C_W],vertex0->A);
		CalcFogCoefficients(v1->comp_f[C_W],vertex1->A);
		CalcFogCoefficients(v2->comp_f[C_W],vertex2->A);
		}
	else
		{
		vertex0->A = S3alpha_value;
		vertex1->A = S3alpha_value;
		vertex2->A = S3alpha_value;
		}

   /* Increment vertex counter */
   num_vertices_ready +=3;
#endif
}



/*---------------- END OF TRIANGLE RENDER FUNCTIONS ---------------*/
/*----------------- START OF EDGE (LINE) RENDER FUNCTIONS ----------------*/

void BR_ASM_CALL  EdgeRender8(struct brp_block *block,
                              union brp_vertex *v0,
                              union brp_vertex *v1)
{
	S3DTK_VERTEX_TEX *vertex0,*vertex1,*vertex2;

	// Flush the current primitives if we are out of storage
	if(num_vertices_ready >= (MAX_VERTEX_STORAGE-3))
		FlushPrimitivesS3();

	/* Get the pointers to the appropriate locations in the vertex array */
	vertex0 = vertex_pointers[NV0];
	vertex1 = vertex0 + 1;
	vertex2 = vertex1 + 1;

	/* vertex x,v and z interleaved - hopefully this enables the compiler to
	 * make intelligent register useage */
	vertex0->X = v0->comp_f[C_SX];
	vertex0->Y = v0->comp_f[C_SY];
	CONVERTZ(vertex0,v0->comp_f[C_SZ]);
	vertex1->X = v1->comp_f[C_SX];
	vertex1->Y = v1->comp_f[C_SY];
	CONVERTZ(vertex1,v1->comp_f[C_SZ]);
	vertex2->X = vertex1->X + 2;
	vertex2->Y = vertex1->Y + 2;
	vertex2->Z = vertex1->Z;

	if(S3tex_enable)
		{
		vertex0->W = v0->comp_f[C_W];
		vertex0->U = v0->comp_f[C_U];
		vertex0->V = v0->comp_f[C_V];
		vertex1->W = v1->comp_f[C_W];
		vertex1->U = v1->comp_f[C_U];
		vertex1->V = v1->comp_f[C_V];
		vertex2->W = vertex1->W;
		vertex2->U = vertex1->U;
		vertex2->V = vertex1->V;
		}

	/* S3 picks up colour indices from B component */
	vertex0->B = FastIntCast(v0->comp_f[C_I]);
	vertex1->B = FastIntCast(v1->comp_f[C_I]);
	vertex2->B = vertex1->B;

	/* Increment vertex counter */
	num_vertices_ready +=3;
}



void BR_ASM_CALL  EdgeRender1624Smooth(struct brp_block *block,
                                    union brp_vertex *v0,
                                    union brp_vertex *v1)
{
	S3DTK_VERTEX_TEX *vertex0,*vertex1,*vertex2;

	// Flush the current primitives if we are out of storage
	if(num_vertices_ready >= (MAX_VERTEX_STORAGE-3))
		FlushPrimitivesS3();

	/* Get the pointers to the appropriate locations in the vertex array */
	vertex0 = vertex_pointers[NV0];
	vertex1 = vertex0 + 1;
	vertex2 = vertex1 + 1;

	/* vertex x,v and z interleaved - hopefully this enables the compiler to
	 * make intelligent register useage */
	vertex0->X = v0->comp_f[C_SX];
	vertex0->Y = v0->comp_f[C_SY];
	CONVERTZ(vertex0,v0->comp_f[C_SZ]);
	vertex1->X = v1->comp_f[C_SX];
	vertex1->Y = v1->comp_f[C_SY];
	CONVERTZ(vertex1,v1->comp_f[C_SZ]);
	vertex2->X = vertex1->X + 2;
	vertex2->Y = vertex1->Y + 2;
	vertex2->Z = vertex1->Z;

	if(S3tex_enable)
		{
		vertex0->W = v0->comp_f[C_W];
		vertex0->U = v0->comp_f[C_U];
		vertex0->V = v0->comp_f[C_V];
		vertex1->W = v1->comp_f[C_W];
		vertex1->U = v1->comp_f[C_U];
		vertex1->V = v1->comp_f[C_V];
		vertex2->W = vertex1->W;
		vertex2->U = vertex1->U;
		vertex2->V = vertex1->V;
		}

	if(S3rgb_enable)
		{
		vertex0->R = FastIntCast(v0->comp_f[C_R]);
		vertex0->G = FastIntCast(v0->comp_f[C_G]);
		vertex0->B = FastIntCast(v0->comp_f[C_B]);
		vertex1->R = FastIntCast(v1->comp_f[C_R]);
		vertex1->G = FastIntCast(v1->comp_f[C_G]);
		vertex1->B = FastIntCast(v1->comp_f[C_B]);
		vertex2->R = vertex1->R;
		vertex2->G = vertex1->G;
		vertex2->B = vertex1->B;
		}

	/* These next lines generate correct values for depth cue
	 * and fog effects in the Alpha component (required by S3) */

	if(S3fog_enable)
		{
		CalcFogCoefficients(v0->comp_f[C_W],vertex0->A);
		CalcFogCoefficients(v1->comp_f[C_W],vertex1->A);
		vertex2->A = vertex1->A;
		}
	else
		{
		vertex0->A = S3alpha_value;
		vertex1->A = S3alpha_value;
		vertex2->A = S3alpha_value;
		}

   /* Increment vertex counter */
   num_vertices_ready +=3;
}





void BR_ASM_CALL  EdgeRender1624Flat(struct brp_block *block,
                                    union brp_vertex *v0,
                                    union brp_vertex *v1)
{
	S3DTK_VERTEX_TEX *vertex0,*vertex1,*vertex2;

	/* Flush the current primitives if we are out of storage */
	if(num_vertices_ready >= (MAX_VERTEX_STORAGE-3))
		FlushPrimitivesS3();

	/* Get the pointers to the appropriate locations in the vertex array */
	vertex0 = vertex_pointers[NV0];
	vertex1 = vertex0 + 1;
	vertex2 = vertex1 + 1;

	/* vertex x,v and z interleaved - hopefully this enables the compiler to
	 * make intelligent register useage */
	vertex0->X = v0->comp_f[C_SX];
	vertex0->Y = v0->comp_f[C_SY];
	CONVERTZ(vertex0,v0->comp_f[C_SZ]);
	vertex1->X = v1->comp_f[C_SX];
	vertex1->Y = v1->comp_f[C_SY];
	CONVERTZ(vertex1,v1->comp_f[C_SZ]);
	vertex2->X = vertex1->X + 2;
	vertex2->Y = vertex1->Y + 2;
	vertex2->Z = vertex1->Z;

	if(S3tex_enable)
		{
		vertex0->W = v0->comp_f[C_W];
		vertex0->U = v0->comp_f[C_U];
		vertex0->V = v0->comp_f[C_V];
		vertex1->W = v1->comp_f[C_W];
		vertex1->U = v1->comp_f[C_U];
		vertex1->V = v1->comp_f[C_V];
		vertex2->W = vertex1->W;
		vertex2->U = vertex1->U;
		vertex2->V = vertex1->V;
		}

	if(S3rgb_enable)
		{
		vertex0->R = FastIntCast(v0->comp_f[C_R]);
		vertex0->G = FastIntCast(v0->comp_f[C_G]);
		vertex0->B = FastIntCast(v0->comp_f[C_B]);
		vertex1->R = FastIntCast(v1->comp_f[C_R]);
		vertex1->G = FastIntCast(v1->comp_f[C_G]);
		vertex1->B = FastIntCast(v1->comp_f[C_B]);
		vertex2->R = vertex1->R;
		vertex2->G = vertex1->G;
		vertex2->B = vertex1->B;
		}

	/* These next lines generate correct values for depth cue
	 * and fog effects in the Alpha component (required by S3) */

	if(S3fog_enable)
		{
		CalcFogCoefficients(v0->comp_f[C_W],vertex0->A);
		CalcFogCoefficients(v1->comp_f[C_W],vertex1->A);
		vertex2->A = vertex1->A;
		}
	else
		{
		vertex0->A = S3alpha_value;
		vertex1->A = S3alpha_value;
		vertex2->A = S3alpha_value;
		}

   /* Increment vertex counter */
   num_vertices_ready +=3;
}




/*------------------ END OF EDGE RENDER FUNCTIONS ---------------*/
/*----------------- START OF POINT RENDER FUNCTIONS ----------------*/


void BR_ASM_CALL  PointRender8(struct brp_block *block,
                                   union brp_vertex *v0)
{
	S3DTK_VERTEX_TEX *vertex0,*vertex1,*vertex2;

	// Flush the current primitives if we are out of storage
	if(num_vertices_ready >= (MAX_VERTEX_STORAGE-3))
		FlushPrimitivesS3();

	/* Get the pointers to the appropriate locations in the vertex array */
	vertex0 = vertex_pointers[NV0];
	vertex1 = vertex0 + 1;
	vertex2 = vertex1 + 1;

	/* Use a very small triangle (0,0) (1,-1) (1,+1) to simulate points */
	vertex0->X = v0->comp_f[C_SX];
	vertex0->Y = v0->comp_f[C_SY];
	CONVERTZ(vertex0,v0->comp_f[C_SZ]);
	vertex1->X = vertex0->X + 1.0f;
	vertex1->Y = vertex0->Y - 1.0f;
	vertex1->Z = vertex0->Z;
	vertex2->X = vertex1->X;
	vertex2->Y = vertex1->Y + 2.0f;
	vertex2->Z = vertex1->Z;

	if(S3tex_enable)
		{
		vertex0->W = v0->comp_f[C_W];
		vertex0->U = v0->comp_f[C_U];
		vertex0->V = v0->comp_f[C_V];
		vertex1->W = vertex0->W;
		vertex1->U = vertex0->U;
		vertex1->V = vertex0->V;
		vertex2->W = vertex1->W;
		vertex2->U = vertex1->U;
		vertex2->V = vertex1->V;
		}

    /* S3 picks up colour indices from B component */
	vertex0->B = FastIntCast(v0->comp_f[C_I]);
	vertex1->B = vertex0->B;
	vertex2->B = vertex1->B;

	/* Increment vertex counter */
	num_vertices_ready +=3;
}


void BR_ASM_CALL  PointRender1624(struct brp_block *block,
                                    union brp_vertex *v0)
{
	S3DTK_VERTEX_TEX *vertex0,*vertex1,*vertex2;

	/* Flush the current primitives if we are out of storage */
	if(num_vertices_ready >= (MAX_VERTEX_STORAGE-3))
		FlushPrimitivesS3();

	/* Get the pointers to the appropriate locations in the vertex array */
	vertex0 = vertex_pointers[NV0];
	vertex1 = vertex0 + 1;
	vertex2 = vertex1 + 1;

	/* Use a very small triangle (0,0) (1,-1) (1,+1) to simulate points */
	vertex0->X = v0->comp_f[C_SX];
	vertex0->Y = v0->comp_f[C_SY];
	CONVERTZ(vertex0,v0->comp_f[C_SZ]);
	vertex1->X = vertex0->X + 1.0f;
	vertex1->Y = vertex0->Y - 1.0f;
	vertex1->Z = vertex0->Z;
	vertex2->X = vertex1->X;
	vertex2->Y = vertex1->Y + 2.0f;
	vertex2->Z = vertex1->Z;

	if(S3tex_enable)
		{
		vertex0->W = v0->comp_f[C_W];
		vertex0->U = v0->comp_f[C_U];
		vertex0->V = v0->comp_f[C_V];
		vertex1->W = vertex0->W;
		vertex1->U = vertex0->U;
		vertex1->V = vertex0->V;
		vertex2->W = vertex1->W;
		vertex2->U = vertex1->U;
		vertex2->V = vertex1->V;
		}

	if(S3rgb_enable)
		{
		vertex0->R = FastIntCast(v0->comp_f[C_R]);
		vertex0->G = FastIntCast(v0->comp_f[C_G]);
		vertex0->B = FastIntCast(v0->comp_f[C_B]);
		vertex1->R = vertex0->R;
		vertex1->G = vertex0->G;
		vertex1->B = vertex0->B;
		vertex2->R = vertex0->R;
		vertex2->G = vertex0->G;
		vertex2->B = vertex0->B;
		}

	/* These next lines generate correct values for depth cue
	 * and fog effects in the Alpha component (required by S3) */

	if(S3fog_enable)
		{
		CalcFogCoefficients(v0->comp_f[C_W],vertex0->A);
		vertex1->A = vertex0->A;
		vertex2->A = vertex0->A;
		}
	else
		{
		vertex0->A = S3alpha_value;
		vertex1->A = S3alpha_value;
		vertex2->A = S3alpha_value;
		}

   /* Increment vertex counter */
   num_vertices_ready +=3;
}


