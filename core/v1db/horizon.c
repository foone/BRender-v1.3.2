/*
 * MIP mapped infinite plane renderer
 * (c) Argonaut 1995
 * $Locker: $
 */

#include <math.h>
#include <float.h>

#include "v1db.h"

#include "shortcut.h"
#include "brassert.h"
#include "priminfo.h"
#include "horizon.h"

typedef float real;

#define Am(i,j) A->m[i][j]
#define Bm(i,j) B->m[i][j]
#define Cm(i,j) C->m[i][j]

/*
 * It's too tricky to get the required accuracy using only fixed point
 * maths so reimplement those parts of BRender maths library required
 * using floating point maths.
 */

typedef struct {
  real m[4][4];
} d_matrix4;

typedef struct {
  real m[4][3];
} d_matrix34;

typedef struct {
  real m[3][2];
} d_matrix23;

typedef struct {
  real v[4];
} d_vector4;

typedef struct {
  real v[3];
} d_vector3;

typedef struct {
  real v[2];
} d_vector2;

real DVector4Dot(d_vector4 *A,d_vector4 *B) {
  return A->v[0]*B->v[0]+A->v[1]*B->v[1]+A->v[2]*B->v[2]+A->v[3]*B->v[3];
}

void DMatrix4TApply(d_vector4 *A, d_vector4 *B, d_matrix4 *C)
{
	A->v[0] = B->v[0]*Cm(0,0)+ B->v[1]*Cm(0,1)+ B->v[2]*Cm(0,2)+ B->v[3]*Cm(0,3);
	A->v[1] = B->v[0]*Cm(1,0)+ B->v[1]*Cm(1,1)+ B->v[2]*Cm(1,2)+ B->v[3]*Cm(1,3);
	A->v[2] = B->v[0]*Cm(2,0)+ B->v[1]*Cm(2,1)+ B->v[2]*Cm(2,2)+ B->v[3]*Cm(2,3);
	A->v[3] = B->v[0]*Cm(3,0)+ B->v[1]*Cm(3,1)+ B->v[2]*Cm(3,2)+ B->v[3]*Cm(3,3);
}

void DMatrix34TApply(d_vector4 *A, d_vector4 *B, d_matrix34 *C)
{
	A->v[0] = B->v[0]*Cm(0,0)+ B->v[1]*Cm(0,1)+ B->v[2]*Cm(0,2);
	A->v[1] = B->v[0]*Cm(1,0)+ B->v[1]*Cm(1,1)+ B->v[2]*Cm(1,2);
	A->v[2] = B->v[0]*Cm(2,0)+ B->v[1]*Cm(2,1)+ B->v[2]*Cm(2,2);
	A->v[3] = B->v[0]*Cm(3,0)+ B->v[1]*Cm(3,1)+ B->v[2]*Cm(3,2)+ B->v[3];
}

/*
 * [a b c d] = [ e f g 1 ] . M
 */
void DMatrix4ApplyP(d_vector4 *A, d_vector3 *B, d_matrix4 *C)
{
	A->v[0] = B->v[0]*Cm(0,0)+ B->v[1]*Cm(1,0)+ B->v[2]*Cm(2,0)+Cm(3,0);
	A->v[1] = B->v[0]*Cm(0,1)+ B->v[1]*Cm(1,1)+ B->v[2]*Cm(2,1)+Cm(3,1);
	A->v[2] = B->v[0]*Cm(0,2)+ B->v[1]*Cm(1,2)+ B->v[2]*Cm(2,2)+Cm(3,2);
	A->v[3] = B->v[0]*Cm(0,3)+ B->v[1]*Cm(1,3)+ B->v[2]*Cm(2,3)+Cm(3,3);
}

real DMatrix4Determinant(d_matrix4 *);

real DMatrix4Inverse(d_matrix4 *A, d_matrix4 *B)
{
    int i, j;
    real det,idet;

    /* calculate the adjoint matrix */

    DMatrix4Adjoint( A, B );

    /*
	 * Calculate the 4x4 determinant
     *  if the determinant is zero, 
     *  then the inverse matrix is not unique.
     */

    det = DMatrix4Determinant( B );

    if (fabs(det) < 1e-15)
		return 0;

    /*
	 * Scale the adjoint matrix to get the inverse
	 */
	idet = 1/det;

    for (i=0; i<4; i++)
        for(j=0; j<4; j++)
			Am(i,j) = Am(i,j)*idet;

	return det;
}

#define DETERMINANT2(a,b,c,d) ((a)*(d)-(b)*(c))

static real Determinant3(
	real a1, real a2, real a3,
	real b1, real b2,	real b3,
	real c1, real c2,	real c3)
{
    return a1*DETERMINANT2( b2, b3, c2, c3 )
                   -b1*DETERMINANT2( a2, a3, c2, c3 )+
        		    c1*DETERMINANT2( a2, a3, b2, b3 );
}

/*
 * return determinant(mat)
 * 
 */
real DMatrix4Determinant(d_matrix4 *mat)
{
    real a1,a2,a3,a4, b1,b2,b3,b4, c1,c2,c3,c4, d1,d2,d3,d4;

	a1 = M(0,0); b1 = M(0,1); 
	c1 = M(0,2); d1 = M(0,3);

	a2 = M(1,0); b2 = M(1,1); 
	c2 = M(1,2); d2 = M(1,3);

	a3 = M(2,0); b3 = M(2,1); 
	c3 = M(2,2); d3 = M(2,3);

	a4 = M(3,0); b4 = M(3,1); 
	c4 = M(3,2); d4 = M(3,3);

    return a1*Determinant3( b2, b3, b4, c2, c3, c4, d2, d3, d4)
		-b1*Determinant3( a2, a3, a4, c2, c3, c4, d2, d3, d4)+
		 c1*Determinant3( a2, a3, a4, b2, b3, b4, d2, d3, d4)
		-d1*Determinant3( a2, a3, a4, b2, b3, b4, c2, c3, c4);
}

/* 
 * A = adjoint(B)
 */

void DMatrix4Adjoint(d_matrix4 *A, d_matrix4 *B)
{
    real a1, a2, a3, a4, b1, b2, b3, b4;
    real c1, c2, c3, c4, d1, d2, d3, d4;

	a1 = Bm(0,0); b1 = Bm(0,1); 
	c1 = Bm(0,2); d1 = Bm(0,3);

	a2 = Bm(1,0); b2 = Bm(1,1); 
	c2 = Bm(1,2); d2 = Bm(1,3);

	a3 = Bm(2,0); b3 = Bm(2,1);
	c3 = Bm(2,2); d3 = Bm(2,3);

	a4 = Bm(3,0); b4 = Bm(3,1); 
	c4 = Bm(3,2); d4 = Bm(3,3);


    /* row column labelling reversed since we transpose rows & columns */

    Am(0,0) =  Determinant3( b2, b3, b4, c2, c3, c4, d2, d3, d4);
    Am(1,0) = -Determinant3( a2, a3, a4, c2, c3, c4, d2, d3, d4);
    Am(2,0) =  Determinant3( a2, a3, a4, b2, b3, b4, d2, d3, d4);
    Am(3,0) = -Determinant3( a2, a3, a4, b2, b3, b4, c2, c3, c4);
        
    Am(0,1) = -Determinant3( b1, b3, b4, c1, c3, c4, d1, d3, d4);
    Am(1,1) =  Determinant3( a1, a3, a4, c1, c3, c4, d1, d3, d4);
    Am(2,1) = -Determinant3( a1, a3, a4, b1, b3, b4, d1, d3, d4);
    Am(3,1) =  Determinant3( a1, a3, a4, b1, b3, b4, c1, c3, c4);
        
    Am(0,2) =  Determinant3( b1, b2, b4, c1, c2, c4, d1, d2, d4);
    Am(1,2) = -Determinant3( a1, a2, a4, c1, c2, c4, d1, d2, d4);
    Am(2,2) =  Determinant3( a1, a2, a4, b1, b2, b4, d1, d2, d4);
    Am(3,2) = -Determinant3( a1, a2, a4, b1, b2, b4, c1, c2, c4);
        
    Am(0,3) = -Determinant3( b1, b2, b3, c1, c2, c3, d1, d2, d3);
    Am(1,3) =  Determinant3( a1, a2, a3, c1, c2, c3, d1, d2, d3);
    Am(2,3) = -Determinant3( a1, a2, a3, b1, b2, b3, d1, d2, d3);
    Am(3,3) =  Determinant3( a1, a2, a3, b1, b2, b3, c1, c2, c3);
}

real DMatrix34Inverse(d_matrix34 *A,d_matrix34 *B) {
  d_matrix4 C,D;
  int i,j;
  real det;

  for (i = 0; i<4; i++)
    for (j = 0; j<3; j++)
      C.m[i][j] = Bm(i,j);

  C.m[0][3] = C.m[1][3] = C.m[2][3] = 0;
  C.m[3][3] = 1;

  det = DMatrix4Inverse(&D,&C);

  if (det==0)
    return det;

  for (i = 0; i<4; i++)
    for (j = 0; j<3; j++)
      Am(i,j) = D.m[i][j];

  return det;
}

/*
 * Generate a perspective tranform
 *
 */
void DMatrix4Perspective(d_matrix4 *A, 
		real field_of_view,
		real aspect,
		real hither,
		real yon)
{
    real scale = 1/tan(field_of_view/2);

    Am(0,0) = scale/aspect;
    Am(1,1) = scale;
    Am(2,2) = (yon+hither)/(yon-hither);
    Am(3,2) = -2*yon*hither/(yon-hither);

    Am(0,1) = 0; Am(0,2) = 0; Am(0,3) = 0;
    Am(1,0) = 0; Am(1,2) = 0; Am(1,3) = 0;
    Am(2,0) = 0; Am(2,1) = 0; Am(2,3) = -1;
    Am(3,0) = 0; Am(3,1) = 0; Am(3,3) = 0;
}

/*
 * [a b ] = [ e f ] . M
 */
void DMatrix23ApplyP(d_vector2 *A, d_vector2 *B, d_matrix23 *C)
{
    A->v[0] = BR_MAC2(B->v[0],C(0,0), B->v[1],C(1,0)) + C(2,0);
	A->v[1] = BR_MAC2(B->v[0],C(0,1), B->v[1],C(1,1)) + C(2,1);
}

/*
 * A screen_vertex is a vertex as rendered to the screenalong with any
 * values that may need interpolation when clipped such as index value
 * or z.
 */

typedef struct {
    real u;
    real v;
    real i;
    int z;
} screen_vertex;

extern br_scalar __cdecl SafeFixedMac3Div(br_scalar,br_scalar,br_scalar,br_scalar,br_scalar,br_scalar,br_scalar);

/*
 * Clip view port to region corresponding to a band of texture in the
 * infinite plane
 */

int
ViewClip(screen_vertex *out,screen_vertex *in,int nstrips,real nx,real ny,real nw,int z)
{
    int i,j = 0;
    real s0,s1;
    int state;
    screen_vertex *last;
    real lambda;

    /* Canonicalise normal */

    s1 = nx*in[nstrips-1].u+ny*in[nstrips-1].v+nw;
    state = s1<0;
    last = &in[nstrips-1];

    for (i = 0; i<nstrips; i++) {
	s0 = s1;
	s1 = nx*in[i].u+ny*in[i].v+nw;

	state <<= 1;
	state |= s1<0;
	state &= 3;

	switch (state) {
	case 0:
	    /* Whole side is outside - do nowt */

	    break;
	case 2:
	    /* Crossed out */

	    lambda = s0/(s0-s1);

	    if (in[i].u==-1 && last->u==-1) {
	        out[j].z = z;
		out[j].u = -1;
		out[j].v = (nx-nw)/ny;
		out[j++].i = last->i+lambda*(in[i].i-last->i);
		break;
	    }
	    if (in[i].u==1 && last->u==1) {
	        out[j].z = z;
		out[j].u = 1;
		out[j].v = (-nx-nw)/ny;
		out[j++].i = last->i+lambda*(in[i].i-last->i);
		break;
	    }
	    if (in[i].v==-1 && last->v==-1) {
	        out[j].z = z;
		out[j].u = (ny-nw)/nx;
		out[j].v = -1;
		out[j++].i = last->i+lambda*(in[i].i-last->i);
		break;
	    }
	    if (in[i].v==1 && last->v==1) {
	        out[j].z = z;
		out[j].u = (-ny-nw)/nx;
		out[j].v = 1;
		out[j++].i = last->i+lambda*(in[i].i-last->i);
		break;
	    }

	    out[j].z = z;
	    out[j].u = last->u+lambda*(in[i].u-last->u);
	    out[j].v = last->v+lambda*(in[i].v-last->v);
	    out[j++].i = last->i+lambda*(in[i].i-last->i);

	    break;
	case 1:
	    /* Was outside but just crossed in */

	    lambda = s0/(s0-s1);

	    if (in[i].u==-1 && last->u==-1) {
	        out[j].z = z;
		out[j].u = -1;
		out[j].v = (nx-nw)/ny;
		out[j++].i = last->i+lambda*(in[i].i-last->i);
		out[j++] = in[i];
		break;
	    }
	    if (in[i].u==1 && last->u==1) {
	        out[j].z = z;
		out[j].u = 1;
		out[j].v = (-nx-nw)/ny;
		out[j++].i = last->i+lambda*(in[i].i-last->i);
		out[j++] = in[i];
		break;
	    }
	    if (in[i].v==-1 && last->v==-1) {
	        out[j].z = z;
		out[j].u = (ny-nw)/nx;
		out[j].v = -1;
		out[j++].i = last->i+lambda*(in[i].i-last->i);
		out[j++] = in[i];
		break;
	    }
	    if (in[i].v==1 && last->v==1) {
	        out[j].z = z;
		out[j].u = (-ny-nw)/nx;
		out[j].v = 1;
		out[j++].i = last->i+lambda*(in[i].i-last->i);
		out[j++] = in[i];
		break;
	    }

	    out[j].z = z;
	    out[j].u = in[i].u+lambda*(last->u-in[i].u);
	    out[j].v = in[i].v+lambda*(last->v-in[i].v);
	    out[j++].i = in[i].i+lambda*(last->i-in[i].i);

	    out[j++] = in[i];
	    break;

	case 3:
	    /* Wholly inside - just copy side */

	    out[j++] = in[i];
	    break;
	}

	last = &in[i];
    }

    return j;
}

/*
 * Render a band of texture
 */

void
RenderBand(br_pixelmap *pm,screen_vertex *in,int nstrips,br_material *m,
real *u,real *v,real *w, real *z)
{
    brp_vertex a,b,c;
    int i, j;
    br_object *primstate;
    brp_block *primblock;
    br_token primstatetok;
    float offsets[NUM_COMPONENTS], scales[NUM_COMPONENTS];

    brp_block *block;
    br_boolean block_changed, ranges_changed;

    UASSERT(m->stored);

    RendererStateRestore(v1db.renderer, m->stored, (br_uint_32)BR_STATE_ALL);

    primstatetok = BrTokenCreate("PRIMITIVE_STATE_O", BRT_OBJECT);

// XXX change this error
    if(ObjectQuery((br_object *)v1db.renderer, (br_uint_32 *)&primstate,
        primstatetok) != BRE_OK)
        BR_ERROR0("No primitive state");

    PrimitiveStateRenderBegin(primstate, &primblock, &block_changed,
        &ranges_changed, 0, BRT_TRIANGLE);

// XXX change this error
//    if(ObjectQuery((br_object *)primstate, (br_uint_32 *)&primblock,
//        BRT_PRIMITIVE_BLOCK_P) != BRE_OK)
//        BR_ERROR0("No primitive block");

    if(PrimitiveStateRangesQueryF((br_object *)primstate, offsets, scales,
        NUM_COMPONENTS) != BRE_OK)
// XXX This should do something better
        return;

// XXX Vertex components should be checked for I/A

// XXX RGB values should be lit according to colour

    /* Assume that SX, SY and SZ are all wanted (pointless without them) */
    if(primblock->convert_mask_f & (1<<C_SX))
        a.comp_f[C_SX] = offsets[C_SX]+scales[C_SX]*in[0].u;
    else if(primblock->convert_mask_x & (1<<C_SX))
        a.comp_x[C_SX] = BrFloatToFixed(offsets[C_SX]+scales[C_SX]*in[0].u);
    else if(primblock->convert_mask_i & (1<<C_SX))
        a.comp_i[C_SX] = (int)(offsets[C_SX]+scales[C_SX]*in[0].u);

    if(primblock->convert_mask_f & (1<<C_SY))
        a.comp_f[C_SY] = offsets[C_SY]+scales[C_SY]*in[0].v;
    else if(primblock->convert_mask_x & (1<<C_SY))
        a.comp_x[C_SY] = BrFloatToFixed(offsets[C_SY]+scales[C_SY]*in[0].v);
    else if(primblock->convert_mask_i & (1<<C_SY))
        a.comp_i[C_SY] = (int)(offsets[C_SY]+scales[C_SY]*in[0].v);

    if(primblock->convert_mask_f & (1<<C_SZ))
        a.comp_f[C_SZ] = offsets[C_SZ]+scales[C_SZ]*z[0];
    else if(primblock->convert_mask_x & (1<<C_SZ))
        a.comp_x[C_SZ] = BrFloatToFixed(offsets[C_SZ]+scales[C_SZ]*z[0]);
    else if(primblock->convert_mask_i & (1<<C_SZ))
        a.comp_x[C_SZ] = (int)(offsets[C_SZ]+scales[C_SZ]*z[0]);

    if(primblock->convert_mask_f & (1<<C_W))
        a.comp_f[C_W] = offsets[C_W]+scales[C_W]*w[0];
    else if(primblock->convert_mask_x & (1<<C_W))
        a.comp_x[C_W] = BrFloatToFixed(offsets[C_W]+scales[C_W]*w[0]);
    else if(primblock->convert_mask_i & (1<<C_W))
        a.comp_i[C_W] = (int)(offsets[C_W]+scales[C_W]*w[0]);

    if(primblock->convert_mask_f & (1<<C_I))
        a.comp_f[C_I] = offsets[C_I]+scales[C_I]*in[0].i;
    else if(primblock->convert_mask_x & (1<<C_I))
        a.comp_x[C_I] = BrFloatToFixed(offsets[C_I]+scales[C_I]*in[0].i);
    else if(primblock->convert_mask_i & (1<<C_I))
        a.comp_i[C_I] = (int)(offsets[C_I]+scales[C_I]*in[0].i);

    /* Assumes RGB are all the same type */
    if(primblock->convert_mask_f & (1<<C_R)){
        a.comp_f[C_R] = offsets[C_R]+scales[C_R]*in[0].i*BR_RED(m->colour)/255;
        a.comp_f[C_G] = offsets[C_G]+scales[C_G]*in[0].i*BR_GRN(m->colour)/255;
        a.comp_f[C_B] = offsets[C_B]+scales[C_B]*in[0].i*BR_BLU(m->colour)/255;
    } else if(primblock->convert_mask_x & (1<<C_G)){
        a.comp_x[C_R] = BrFloatToFixed(offsets[C_R]+scales[C_R]*in[0].i*BR_RED(m->colour)/255);
        a.comp_x[C_G] = BrFloatToFixed(offsets[C_G]+scales[C_G]*in[0].i*BR_GRN(m->colour)/255);
        a.comp_x[C_B] = BrFloatToFixed(offsets[C_B]+scales[C_B]*in[0].i*BR_BLU(m->colour)/255);
    } else if(primblock->convert_mask_i & (1<<C_B)){
        a.comp_i[C_R] = (int)(offsets[C_R]+scales[C_R]*in[0].i*BR_RED(m->colour)/255);
        a.comp_i[C_G] = (int)(offsets[C_G]+scales[C_G]*in[0].i*BR_GRN(m->colour)/255);
        a.comp_i[C_B] = (int)(offsets[C_B]+scales[C_B]*in[0].i*BR_BLU(m->colour)/255);
    }

    if (m->colour_map) {
        if(primblock->convert_mask_f & (1<<C_U))
            a.comp_f[C_U] = offsets[C_U]+scales[C_U]*u[0];
        else if(primblock->convert_mask_x & (1<<C_U))
            a.comp_x[C_U] = BrFloatToFixed(offsets[C_U]+scales[C_U]*u[0]);
        else if(primblock->convert_mask_i & (1<<C_U))
            a.comp_i[C_U] = (int)(offsets[C_U]+scales[C_U]*u[0]);
        if(primblock->convert_mask_f & (1<<C_V))
            a.comp_f[C_V] = offsets[C_V]+scales[C_V]*v[0];
        else if(primblock->convert_mask_x & (1<<C_V))
            a.comp_x[C_V] = BrFloatToFixed(offsets[C_V]+scales[C_V]*v[0]);
        else if(primblock->convert_mask_i & (1<<C_V))
            a.comp_i[C_V] = (int)(offsets[C_V]+scales[C_V]*v[0]);
    }

    for (i = 2; i<nstrips; i++) {
        if(primblock->convert_mask_f & (1<<C_SX)){
            b.comp_f[C_SX] = offsets[C_SX]+scales[C_SX]*in[i-1].u;
            c.comp_f[C_SX] = offsets[C_SX]+scales[C_SX]*in[i].u;
        } else if(primblock->convert_mask_x & (1<<C_SX)){
            b.comp_x[C_SX] = BrFloatToFixed(offsets[C_SX]+scales[C_SX]*in[i-1].u);
            c.comp_x[C_SX] = BrFloatToFixed(offsets[C_SX]+scales[C_SX]*in[i].u);
        } else if(primblock->convert_mask_i & (1<<C_SX)){
            b.comp_i[C_SX] = (int)(offsets[C_SX]+scales[C_SX]*in[i-1].u);
            c.comp_i[C_SX] = (int)(offsets[C_SX]+scales[C_SX]*in[i].u);
        }

        if(primblock->convert_mask_f & (1<<C_SY)){
            b.comp_f[C_SY] = offsets[C_SY]+scales[C_SY]*in[i-1].v;
            c.comp_f[C_SY] = offsets[C_SY]+scales[C_SY]*in[i].v;
        } else if(primblock->convert_mask_x & (1<<C_SY)){
            b.comp_x[C_SY] = BrFloatToFixed(offsets[C_SY]+scales[C_SY]*in[i-1].v);
            c.comp_x[C_SY] = BrFloatToFixed(offsets[C_SY]+scales[C_SY]*in[i].v);
        } else if(primblock->convert_mask_i & (1<<C_SY)){
            b.comp_i[C_SY] = (int)(offsets[C_SY]+scales[C_SY]*in[i-1].v);
            c.comp_i[C_SY] = (int)(offsets[C_SY]+scales[C_SY]*in[i].v);
        }

        if(primblock->convert_mask_f & (1<<C_SZ)){
            b.comp_f[C_SZ] = offsets[C_SZ]+scales[C_SZ]*z[i-1];
            c.comp_f[C_SZ] = offsets[C_SZ]+scales[C_SZ]*z[i];
        } else if(primblock->convert_mask_x & (1<<C_SZ)){
            b.comp_x[C_SZ] = BrFloatToFixed(offsets[C_SZ]+scales[C_SZ]*z[i-1]);
            c.comp_x[C_SZ] = BrFloatToFixed(offsets[C_SZ]+scales[C_SZ]*z[i]);
        } else if(primblock->convert_mask_i & (1<<C_SZ)){
            b.comp_x[C_SZ] = (int)(offsets[C_SZ]+scales[C_SZ]*z[i-1]);
            c.comp_x[C_SZ] = (int)(offsets[C_SZ]+scales[C_SZ]*z[i]);
        }


        if(primblock->convert_mask_f & (1<<C_W)){
            b.comp_f[C_W] = offsets[C_W]+scales[C_W]*w[i-1];
            c.comp_f[C_W] = offsets[C_W]+scales[C_W]*w[i];
        } else if(primblock->convert_mask_x & (1<<C_W)){
            b.comp_x[C_W] = BrFloatToFixed(offsets[C_W]+scales[C_W]*w[i-1]);
            c.comp_x[C_W] = BrFloatToFixed(offsets[C_W]+scales[C_W]*w[i]);
        } else if(primblock->convert_mask_i & (1<<C_W)){
            b.comp_i[C_W] = (int)(offsets[C_W]+scales[C_W]*w[i-1]);
            c.comp_i[C_W] = (int)(offsets[C_W]+scales[C_W]*w[i]);
        }

        if(primblock->convert_mask_f & (1<<C_I)){
            b.comp_f[C_I] = offsets[C_I]+scales[C_I]*in[i-1].i;
            c.comp_f[C_I] = offsets[C_I]+scales[C_I]*in[i].i;
        } else if(primblock->convert_mask_x & (1<<C_I)){
            b.comp_x[C_I] = BrFloatToFixed(offsets[C_I]+scales[C_I]*in[i-1].i);
            c.comp_x[C_I] = BrFloatToFixed(offsets[C_I]+scales[C_I]*in[i].i);
        } else if(primblock->convert_mask_i & (1<<C_I)){
            b.comp_i[C_I] = (int)(offsets[C_I]+scales[C_I]*in[i-1].i);
            c.comp_i[C_I] = (int)(offsets[C_I]+scales[C_I]*in[i].i);
        }

        if(primblock->convert_mask_f & (1<<C_R)){
            b.comp_f[C_R] = offsets[C_R]+scales[C_R]*in[i-1].i*BR_RED(m->colour)/255;
            c.comp_f[C_R] = offsets[C_R]+scales[C_R]*in[i].i*BR_RED(m->colour)/255;
            b.comp_f[C_G] = offsets[C_G]+scales[C_G]*in[i-1].i*BR_GRN(m->colour)/255;
            c.comp_f[C_G] = offsets[C_G]+scales[C_G]*in[i].i*BR_GRN(m->colour)/255;
            b.comp_f[C_B] = offsets[C_B]+scales[C_B]*in[i-1].i*BR_BLU(m->colour)/255;
            c.comp_f[C_B] = offsets[C_B]+scales[C_B]*in[i].i*BR_BLU(m->colour)/255;
        } else if(primblock->convert_mask_x & (1<<C_G)){
            b.comp_x[C_R] = BrFloatToFixed(offsets[C_R]+scales[C_R]*in[i-1].i*BR_RED(m->colour)/255);
            c.comp_x[C_R] = BrFloatToFixed(offsets[C_R]+scales[C_R]*in[i].i*BR_RED(m->colour)/255);
            b.comp_x[C_G] = BrFloatToFixed(offsets[C_G]+scales[C_G]*in[i-1].i*BR_GRN(m->colour)/255);
            c.comp_x[C_G] = BrFloatToFixed(offsets[C_G]+scales[C_G]*in[i].i*BR_GRN(m->colour)/255);
            b.comp_x[C_B] = BrFloatToFixed(offsets[C_B]+scales[C_B]*in[i-1].i*BR_BLU(m->colour)/255);
            c.comp_x[C_B] = BrFloatToFixed(offsets[C_B]+scales[C_B]*in[i].i*BR_BLU(m->colour)/255);
        } else if(primblock->convert_mask_i & (1<<C_B)){
            b.comp_i[C_R] = (int)(offsets[C_R]+scales[C_R]*in[i-1].i*BR_RED(m->colour)/255);
            c.comp_i[C_R] = (int)(offsets[C_R]+scales[C_R]*in[i].i*BR_RED(m->colour)/255);
            b.comp_i[C_G] = (int)(offsets[C_G]+scales[C_G]*in[i-1].i*BR_GRN(m->colour)/255);
            c.comp_i[C_G] = (int)(offsets[C_G]+scales[C_G]*in[i].i*BR_GRN(m->colour)/255);
            b.comp_i[C_B] = (int)(offsets[C_B]+scales[C_B]*in[i-1].i*BR_BLU(m->colour)/255);
            c.comp_i[C_B] = (int)(offsets[C_B]+scales[C_B]*in[i].i*BR_BLU(m->colour)/255);
        }

        if (m->colour_map) {
            if(primblock->convert_mask_f & (1<<C_U)){
                b.comp_f[C_U] = offsets[C_U]+scales[C_U]*u[i-1];
                c.comp_f[C_U] = offsets[C_U]+scales[C_U]*u[i];
            } else if(primblock->convert_mask_x & (1<<C_U)){
                b.comp_x[C_U] = BrFloatToFixed(offsets[C_U]+scales[C_U]*u[i-1]);
                c.comp_x[C_U] = BrFloatToFixed(offsets[C_U]+scales[C_U]*u[i]);
            } else if(primblock->convert_mask_i & (1<<C_U)){
                b.comp_i[C_U] = (int)(offsets[C_U]+scales[C_U]*u[i-1]);
                c.comp_i[C_U] = (int)(offsets[C_U]+scales[C_U]*u[i]);
            }
            if(primblock->convert_mask_f & (1<<C_V)){
                b.comp_f[C_V] = offsets[C_V]+scales[C_V]*v[i-1];
                c.comp_f[C_V] = offsets[C_V]+scales[C_V]*v[i];
            } else if(primblock->convert_mask_x & (1<<C_V)){
                b.comp_x[C_V] = BrFloatToFixed(offsets[C_V]+scales[C_V]*v[i-1]);
                c.comp_x[C_V] = BrFloatToFixed(offsets[C_V]+scales[C_V]*v[i]);
            } else if(primblock->convert_mask_i & (1<<C_V)){
                b.comp_i[C_V] = (int)(offsets[C_V]+scales[C_V]*v[i-1]);
                c.comp_i[C_V] = (int)(offsets[C_V]+scales[C_V]*v[i]);
            }
        }

        primblock->render(primblock, &a, &b, &c);

    }

    PrimitiveStateRenderEnd(primstate, primblock);
}

/* Render infinite plane tiled with MIP mapped textures */

void BR_PUBLIC_ENTRY BrHorizonRender(br_pixelmap *pm,br_actor *camera,br_horizon *h) {
    d_matrix4 view_to_screen;
    d_matrix4 dual;
    br_camera *camera_data;

    d_vector4 x[4];
    screen_vertex in[9],out[8];
    d_vector4 n;	/* normal to plane in screen space */
    d_vector4 n_inf;	/* normal to plane in screen space at infinity */
    d_vector4 nn;
    d_vector3 tmp1;
    d_vector4 tmp2;
    real z[8];
    real _uc[8],_vc[8],_w[8];
    int sign = 1;
    d_matrix34 inverse;
    d_vector4 hn,hu,hv,tmp3;
    d_matrix34 hm;
    d_matrix23 maptrans;

    int i, j, k;
    int verts;

    hn.v[0] = 0;
    hn.v[1] = 0;
    hn.v[2] = 1;
    hn.v[3] = 0;

    hu.v[0] = 1;
    hu.v[1] = 0;
    hu.v[2] = 0;
    hu.v[3] = 0;

    hv.v[0] = 0;
    hv.v[1] = 1;
    hv.v[2] = 0;
    hv.v[3] = 0;

    for (i = 0; i<4; i++)
      for (j = 0; j<3; j++)
        hm.m[i][j] = BrScalarToFloat(h->m.m[i][j]);

    DMatrix34Inverse(&inverse,&hm);

    DMatrix34TApply(&tmp3,&hn,&inverse);
    hn = tmp3;
    DMatrix34TApply(&tmp3,&hu,&inverse);
    hu = tmp3;
    DMatrix34TApply(&tmp3,&hv,&inverse);
    hv = tmp3;

    if (hn.v[W]<0)
        return;

    camera_data = camera->type_data;

    DMatrix4Perspective(&view_to_screen,
	    BrScalarToFloat(BrAngleToRadian(camera_data->field_of_view)),
	    BrScalarToFloat(camera_data->aspect),
        -BrScalarToFloat(camera_data->hither_z),-BrScalarToFloat(camera_data->yon_z));

    DMatrix4Inverse(&dual,&view_to_screen);

    DMatrix4TApply(&n,&hn,&dual);

    nn = hn;
    if (nn.v[0]<0 || nn.v[0]==0 && nn.v[1]<0 || nn.v[0]==0 && nn.v[1]==0 && nn.v[2]<0) {
        sign = -1;
        nn.v[0] = -nn.v[0];
        nn.v[1] = -nn.v[1];
        nn.v[2] = -nn.v[2];
    }
    nn.v[3] = S0;
    DMatrix4TApply(&n_inf,&nn,&dual);
    if (sign<0) {
        n_inf.v[0] = -n_inf.v[0];
        n_inf.v[1] = -n_inf.v[1];
        n_inf.v[2] = -n_inf.v[2];
        n_inf.v[3] = -n_inf.v[3];
    }
    n.v[0] = n_inf.v[0];
    n.v[1] = n_inf.v[1];

    /* Now for each band clip the viewport to it */

    for (i = 0; i<h->nstrips; i++) {
        real k1,k2;
        real hither,yon;

        if (h->strips[i].far_z)
            yon = (
                -view_to_screen.m[2][2]*BrScalarToFloat(h->strips[i].far_z)+
                view_to_screen.m[3][2]
            )/BrScalarToFloat(h->strips[i].far_z);
        else
            yon = -view_to_screen.m[2][2];

        if (h->strips[i].near_z)
            hither = (
                -view_to_screen.m[2][2]*BrScalarToFloat(h->strips[i].near_z)+
                view_to_screen.m[3][2]
            )/BrScalarToFloat(h->strips[i].near_z);

        in[0].u = -1;
        in[0].v = -1;
        in[0].z = 0;
        in[1].u = 1;
        in[1].v = -1;
        in[1].z = 0;
        in[2].u = 1;
        in[2].v = 1;
        in[2].z = 0;
        in[3].u = -1;
        in[3].v = 1;
        in[3].z = 0;

        /*
         *  Find constant term in 2D equation of edge of band on screen
         */

        if (h->strips[i].far_z) {
            k2 = n.v[Z]*yon+n.v[W];
        } else
            k2 = n_inf.v[Z]*hither+n_inf.v[W];
        k1 = n.v[Z]*hither+n.v[W];


        if (fabs(k1-k2)<0.0001)
            continue;

        /*
         * Set up index values for 4 corners of screen
         */

        for (j = 0; j<4; j++)
            if (h->strips[i].near_z)
                in[j].i = BrScalarToFloat(h->strips[i].near_index)+
                    BrScalarToFloat(h->strips[i].far_index-h->strips[i].near_index)*
                    (n.v[X]*in[j].u+n.v[Y]*in[j].v+k1)/(-k2+k1);
            else
                in[j].i = BrScalarToFloat(h->strips[i].far_index);

        verts = 4;

        verts = ViewClip(out,in,verts,n.v[X],n.v[Y],k2,i);
        for (j = 0; j<verts; j++)
            in[j] = out[j];
	    
        if (h->strips[i].near_z) {
            verts = ViewClip(out,in,verts,-n.v[X],-n.v[Y],-k1,i);
            for (j = 0; j<verts; j++)
            in[j] = out[j];
        }

        /*
         * Lift 2D coords on screen to plane in 3-space getting texture coords
         */

        for (j = 0; j<3; j++)
          for (k = 0; k<2; k++)
            maptrans.m[j][k] = BrScalarToFloat(h->strips[i].mat->map_transform.m[j][k]);


        for (j = 0; j<verts; j++)  {
            real w;
            d_vector2 uvtmp, uv;

            z[j] = -(in[j].u*n.v[X]+in[j].v*n.v[Y]+n.v[W])/n.v[Z];

            tmp1.v[X] = in[j].u;
            tmp1.v[Y] = in[j].v;
            tmp1.v[Z] = z[j];
            /* Clamp Z */
            if(z[j] < -1)
                z[j] = -1;
            if(z[j] > 1)
                z[j] = 1;
            DMatrix4ApplyP(&tmp2,&tmp1,&dual);
            _w[j] = w = 1/tmp2.v[W];

            uvtmp.v[0] = DVector4Dot(&hu,&tmp2)*w;
            uvtmp.v[1] = DVector4Dot(&hv,&tmp2)*w;
            DMatrix23ApplyP(&uv, &uvtmp, &maptrans);

            _uc[j] = uv.v[0];
            _vc[j] = uv.v[1];
        }

        RenderBand(pm,in,verts,h->strips[i].mat,_uc,_vc,_w, z);
    }
}
