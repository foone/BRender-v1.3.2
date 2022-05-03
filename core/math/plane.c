/*
 * Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: plane.c 1.3 1998/08/03 16:48:18 jon Exp $
 * $Locker: $
 */

#include "brender.h"
#include "shortcut.h"
#include "brmath.h"
#include "brassert.h"

BR_RCS_ID("$Id: plane.c 1.3 1998/08/03 16:48:18 jon Exp $")

/*
 * Work out the plane equation of the plane defined by three points
 *
 * Use float for intermdiates for accuracy
 *
 * Returns != 0 if points were co-linear 
 */
br_int_32 BR_PUBLIC_ENTRY BrPlaneEquation(br_vector4 *eqn, br_vector3 *v0, br_vector3 *v1, br_vector3 *v2)
{
	br_vector3 a,b;
	float ax,ay,az;
	float bx,by,bz;
	float nx,ny,nz,d;
	float l;

        UASSERT_MESSAGE("Plane Equation is NULL", eqn != NULL);
        UASSERT_MESSAGE("BrPlaneEquation: Vector 0 is NULL", v0 != NULL);
        UASSERT_MESSAGE("BrPlaneEquation: Vector 1 is NULL", v1 != NULL);
        UASSERT_MESSAGE("BrPlaneEquation: Vector 2 is NULL", v2 != NULL);

	BrVector3Sub(&a,v1,v0);
	BrVector3Sub(&b,v2,v0);

	ax = BrScalarToFloat(a.v[0]);	
	ay = BrScalarToFloat(a.v[1]);	
	az = BrScalarToFloat(a.v[2]);	

	bx = BrScalarToFloat(b.v[0]);	
	by = BrScalarToFloat(b.v[1]);	
	bz = BrScalarToFloat(b.v[2]);	

	nx = ay*bz-az*by;
	ny = az*bx-ax*bz;
	nz = ax*by-ay*bx;

	l = BrFloatSqrt(nx * nx + ny * ny + nz * nz);
	
	if(l != 0) {
		l = 1.0F/l;
		nx *= l;
		ny *= l;
		nz *= l;
	} else {
		nx = 0.0F;
		ny = 0.0F;
		nz = 1.0F;
	}

	d = nx * BrScalarToFloat(v0->v[0]) +
		ny * BrScalarToFloat(v0->v[1]) +
		nz * BrScalarToFloat(v0->v[2]);

	eqn->v[0] = BrFloatToScalar(nx);
	eqn->v[1] = BrFloatToScalar(ny);
	eqn->v[2] = BrFloatToScalar(nz);
	eqn->v[3] = BrFloatToScalar(d);

	return l == 0;
}

/*
 * Apply a matrix34 transformation to a plane equation
 */
void BR_PUBLIC_ENTRY BrMatrix34ApplyPlaneEquation(br_vector4 *A, br_vector4 *B, br_matrix34 *C)
{
	br_scalar length;

	BrMatrix34ApplyV((br_vector3 *)A, (br_vector3 *)B, C);

	length = BrVector3Length((br_vector3 *)A);
	BrVector3Scale((br_vector3 *)A, (br_vector3 *)A, BR_RCP(length));

	A->v[3] = BR_MUL(length, B->v[3]) + BrVector3Dot((br_vector3 *)A, (br_vector3 *)&C->m[3]);
}

