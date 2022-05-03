/*
 * Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: transfrm.c 1.1 1997/12/10 16:41:22 jon Exp $
 * $Locker: $
 *
 */
#include "brender.h"
#include "shortcut.h"
#include "brassert.h"
#include "vecifns.h"

BR_RCS_ID("$Id: transfrm.c 1.1 1997/12/10 16:41:22 jon Exp $")

#define COMBINE_TRANSFORMS(a,b) (_CombineTransforms[(a)][(b)])
#define IS_LP(a) ((a) != BR_TRANSFORM_MATRIX34)

/*
 * A table of how transforms combine
 */
#define M34 BR_TRANSFORM_MATRIX34		
#define MLP BR_TRANSFORM_MATRIX34_LP	
#define QUT BR_TRANSFORM_QUAT			
#define EUL BR_TRANSFORM_EULER			
#define LUP BR_TRANSFORM_LOOK_UP		
#define TRA BR_TRANSFORM_TRANSLATION	
#define IDT BR_TRANSFORM_IDENTITY		

br_uint_8 _CombineTransforms[BR_TRANSFORM_MAX][BR_TRANSFORM_MAX] = {

			/*M34, MLP, QUT, EUL, LUP, TRA, IDT */

/* M34 */	{ M34, M34, M34, M34, M34, M34, M34, },
/* MLP */	{ M34, MLP, MLP, MLP, MLP, MLP, MLP, },
/* QUT */	{ M34, MLP, MLP, MLP, MLP, QUT, QUT, },
/* EUL */	{ M34, MLP, MLP, MLP, MLP, EUL, EUL, },
/* LUP */	{ M34, MLP, MLP, MLP, MLP, LUP, LUP, },
/* TRA */	{ M34, MLP, QUT, EUL, LUP, TRA, TRA, },
/* IDT */	{ M34, MLP, QUT, EUL, LUP, TRA, IDT, },
};

void BR_PUBLIC_ENTRY BrTransformToMatrix34(br_matrix34 *mat,br_transform *xform)
{
        UASSERT_MESSAGE("Destination matrix is NULL", mat != NULL);
        UASSERT_MESSAGE("Source transform is NULL", xform != NULL);

        switch(xform->type) {
	case BR_TRANSFORM_MATRIX34:
	case BR_TRANSFORM_MATRIX34_LP:
		*mat = xform->t.mat;
		break;

	case BR_TRANSFORM_QUAT:

		BrQuatToMatrix34(mat,&xform->t.quat.q);

		mat->m[W][X] = xform->t.quat.t.v[X];
		mat->m[W][Y] = xform->t.quat.t.v[Y];
		mat->m[W][Z] = xform->t.quat.t.v[Z];
		break;

	case BR_TRANSFORM_EULER:

		BrEulerToMatrix34(mat,&xform->t.euler.e);

		mat->m[W][X] = xform->t.euler.t.v[X];
		mat->m[W][Y] = xform->t.euler.t.v[Y];
		mat->m[W][Z] = xform->t.euler.t.v[Z];

		break;

	case BR_TRANSFORM_TRANSLATION:
		BrMatrix34Translate(mat,
			xform->t.look_up.t.v[X],
			xform->t.look_up.t.v[Y],
			xform->t.look_up.t.v[Z]);

		break;

	case BR_TRANSFORM_LOOK_UP:
		{
			/*
			 * 1) Normalise Lookat vector to get Z component of matrix
			 * 2) Cross with up vector and normalise to get X component
			 * 3) Cross X & Z to get Y
			 */
			br_vector3 vx,vy,vz;

			BrVector3Normalise(&vz,&xform->t.look_up.look);
			BrVector3Negate(&vz,&vz);
			BrVector3Cross(&vx,&xform->t.look_up.up,&vz);
			BrVector3Normalise(&vx,&vx);
			BrVector3Cross(&vy,&vz,&vx);
			
			mat->m[X][X] = vx.v[X];	mat->m[X][Y] = vx.v[Y];	mat->m[X][Z] = vx.v[Z];
			mat->m[Y][X] = vy.v[X];	mat->m[Y][Y] = vy.v[Y];	mat->m[Y][Z] = vy.v[Z];
			mat->m[Z][X] = vz.v[X];	mat->m[Z][Y] = vz.v[Y];	mat->m[Z][Z] = vz.v[Z];

			mat->m[W][X] = xform->t.look_up.t.v[X];
			mat->m[W][Y] = xform->t.look_up.t.v[Y];
			mat->m[W][Z] = xform->t.look_up.t.v[Z];
		}
		break;

	case BR_TRANSFORM_IDENTITY:
		BrMatrix34Identity(mat);
		break;
	}
}


void BR_PUBLIC_ENTRY BrMatrix34PreTransform(br_matrix34 *mat,br_transform *xform)
{
	br_matrix34 tmp;

        UASSERT_MESSAGE("Subject matrix is NULL", mat != NULL);
        UASSERT_MESSAGE("Pre-multiplying Transform is NULL", xform != NULL);

        if(xform->type == BR_TRANSFORM_IDENTITY)
		return;

	BrMatrix34Transform(&tmp,xform);
	BrMatrix34Pre(mat,&tmp);
}

void BR_PUBLIC_ENTRY BrMatrix34PostTransform(br_matrix34 *mat,br_transform *xform)
{
	br_matrix34 tmp;

        UASSERT_MESSAGE("Subject matrix is NULL", mat != NULL);
        UASSERT_MESSAGE("Post-multiplying Transform is NULL", xform != NULL);
    
	if(xform->type == BR_TRANSFORM_IDENTITY)
		return;

	BrTransformToMatrix34(&tmp,xform);

	BrMatrix34Post(mat,&tmp);
}

void BR_PUBLIC_ENTRY BrMatrix4PreTransform(br_matrix4 *mat, br_transform *xform)
{
	br_matrix34 tmp;

        UASSERT_MESSAGE("Subject matrix is NULL", mat != NULL);
        UASSERT_MESSAGE("Pre-multiplying Transform is NULL", xform != NULL);

        if(xform->type == BR_TRANSFORM_IDENTITY)
		return;

	BrTransformToMatrix34(&tmp,xform);

	BrMatrix4Pre34(mat,&tmp);
}

/*
 * Convert a 3x4 into a transform
 */
void BR_PUBLIC_ENTRY BrMatrix34ToTransform(br_transform *xform, br_matrix34 *mat)
{
        UASSERT_MESSAGE("Source matrix is NULL", mat != NULL);
        UASSERT_MESSAGE("Destination Transform is NULL", xform != NULL);

	switch(xform->type) {
             
        case BR_TRANSFORM_MATRIX34:
		xform->t.mat = *mat;
		break;

	case BR_TRANSFORM_MATRIX34_LP:
		xform->t.mat = *mat;
		BrMatrix34LPNormalise(&xform->t.mat,&xform->t.mat);
		break;

	case BR_TRANSFORM_QUAT:
		BrMatrix34ToQuat(&xform->t.quat.q,mat);

		xform->t.quat.t.v[X] = mat->m[W][X];
		xform->t.quat.t.v[Y] = mat->m[W][Y];
		xform->t.quat.t.v[Z] = mat->m[W][Z];
		break;

	case BR_TRANSFORM_EULER:

		BrMatrix34ToEuler(&xform->t.euler.e,mat);

		xform->t.euler.t.v[X] = mat->m[W][X];
		xform->t.euler.t.v[Y] = mat->m[W][Y];
		xform->t.euler.t.v[Z] = mat->m[W][Z];
		break;

	case BR_TRANSFORM_TRANSLATION:
		xform->t.translate.t.v[X] = mat->m[W][X];
		xform->t.translate.t.v[Y] = mat->m[W][Y];
		xform->t.translate.t.v[Z] = mat->m[W][Z];
		break;

	case BR_TRANSFORM_LOOK_UP:
		
		/*
		 * Copy the z vector into look
		 */
		BrVector3CopyMat34Row(&xform->t.look_up.look,mat,2);
                BrVector3Negate(&xform->t.look_up.look,&xform->t.look_up.look);

		/*
		 * If the UP vector is 0,0,0, set it to 0,1,0
		 */
		if( xform->t.look_up.up.v[0] == S0 &&
			xform->t.look_up.up.v[1] == S0 &&
			xform->t.look_up.up.v[2] == S0)
			BrVector3Set(&xform->t.look_up.up,S0,S1,S0);

		xform->t.look_up.t.v[X] = mat->m[W][X];
		xform->t.look_up.t.v[Y] = mat->m[W][Y];
		xform->t.look_up.t.v[Z] = mat->m[W][Z];

		break;

	case BR_TRANSFORM_IDENTITY:
		break;
	}
}

/*
 * Copy the source transform into the destination
 *
 * The destination type is retained. The operation tries
 * to keep as much information about the transform as possible
 */
void BR_PUBLIC_ENTRY BrTransformToTransform(br_transform *dest,br_transform *src)
{
	br_matrix34 temp;

        UASSERT_MESSAGE("Source transform is NULL", src != NULL);
        UASSERT_MESSAGE("Destination transform is NULL", dest != NULL);

	/*
	 * If the types are the same, simply copy the data
	 */
	if(dest->type == src->type) {
		*dest = *src;
		return;
	}

	/*
	 * If the destination type is a 3x4 matrix,
	 * simply generate the source into it
	 */
	if(dest->type <= BR_TRANSFORM_MATRIX34_LP) {
		BrMatrix34Transform(&dest->t.mat,src);

		/*
		 * If source was not length preserving,
		 * Orthonormalise the matrix
		 */
		if(!IS_LP(src->type))
			BrMatrix34LPNormalise(&dest->t.mat,&dest->t.mat);

		return;

	}

	/*
	 * Convert src to dest via temp. matrix
	 */
	BrTransformToMatrix34(&temp,src);
	BrMatrix34ToTransform(dest,&temp);
}

#if 0
br_uint_8 BR_PUBLIC_ENTRY BrTransformCombineTypes(br_uint_8 a, br_uint_8 b)
{
	return COMBINE_TRANSFORMS(a,b);
}
#endif


