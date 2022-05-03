/*
 * Copyright (c) 1992,1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: mscimath.h 1.1 1998/11/12 12:53:47 johng Exp $
 * $Locker: $
 *
 * Inline fixed point ops for Microsoft Visual C++
 *
 */
#if BASED_FLOAT

#pragma warning( disable : 4035 )

__inline void VCIMatrix34PreRotateX(br_matrix34 *mat, br_angle rx){
	float s,c;
	float t0,t1,t2;

	s=BR_SIN(rx);
	c=BR_COS(rx);

	t0=mat->m[1][0];
	t1=mat->m[1][1];
	t2=mat->m[1][2];

	mat->m[1][0]=c*mat->m[1][0]+s*mat->m[2][0];
	mat->m[1][1]=c*mat->m[1][1]+s*mat->m[2][1];
	mat->m[1][2]=c*mat->m[1][2]+s*mat->m[2][2];

	mat->m[2][0]=-s*t0+c*mat->m[2][0];
	mat->m[2][1]=-s*t1+c*mat->m[2][1];
	mat->m[2][2]=-s*t2+c*mat->m[2][2];
}

__inline void VCIMatrix34PostRotateX(br_matrix34 *mat, br_angle rx){
	float s,c;
	float t0,t1,t2,t3;

	s=BR_SIN(rx);
	c=BR_COS(rx);

	t0=mat->m[0][1];
	t1=mat->m[1][1];
	t2=mat->m[2][1];
	t3=mat->m[3][1];

	mat->m[0][1]=c*mat->m[0][1]-s*mat->m[0][2];
	mat->m[1][1]=c*mat->m[1][1]-s*mat->m[1][2];
	mat->m[2][1]=c*mat->m[2][1]-s*mat->m[2][2];
	mat->m[3][1]=c*mat->m[3][1]-s*mat->m[3][2];

	mat->m[0][2]=s*t0+c*mat->m[0][2];
	mat->m[1][2]=s*t1+c*mat->m[1][2];
	mat->m[2][2]=s*t2+c*mat->m[2][2];
	mat->m[3][2]=s*t3+c*mat->m[3][2];
}

__inline void VCIMatrix34PreRotateY(br_matrix34 *mat, br_angle ry){
	float s,c;
	float t0,t1,t2;

	s=BR_SIN(ry);
	c=BR_COS(ry);

	t0=mat->m[0][0];
	t1=mat->m[0][1];
	t2=mat->m[0][2];

	mat->m[0][0]=c*mat->m[0][0]-s*mat->m[2][0];
	mat->m[0][1]=c*mat->m[0][1]-s*mat->m[2][1];
	mat->m[0][2]=c*mat->m[0][2]-s*mat->m[2][2];

	mat->m[2][0]=s*t0+c*mat->m[2][0];
	mat->m[2][1]=s*t1+c*mat->m[2][1];
	mat->m[2][2]=s*t2+c*mat->m[2][2];
}

__inline void VCIMatrix34PostRotateY(br_matrix34 *mat, br_angle ry){
	float s,c;
	float t0,t1,t2,t3;

	s=BR_SIN(ry);
	c=BR_COS(ry);

	t0=mat->m[0][0];
	t1=mat->m[1][0];
	t2=mat->m[2][0];
	t3=mat->m[3][0];

	mat->m[0][0]=c*mat->m[0][0]+s*mat->m[0][2];
	mat->m[1][0]=c*mat->m[1][0]+s*mat->m[1][2];
	mat->m[2][0]=c*mat->m[2][0]+s*mat->m[2][2];
	mat->m[3][0]=c*mat->m[3][0]+s*mat->m[3][2];

	mat->m[0][2]=-s*t0+c*mat->m[0][2];
	mat->m[1][2]=-s*t1+c*mat->m[1][2];
	mat->m[2][2]=-s*t2+c*mat->m[2][2];
	mat->m[3][2]=-s*t3+c*mat->m[3][2];
}

__inline void VCIMatrix34PreRotateZ(br_matrix34 *mat, br_angle rz){
	float s,c;
	float t0,t1,t2;

	s=BR_SIN(rz);
	c=BR_COS(rz);

	t0=mat->m[0][0];
	t1=mat->m[0][1];
	t2=mat->m[0][2];

	mat->m[0][0]=c*mat->m[0][0]+s*mat->m[1][0];
	mat->m[0][1]=c*mat->m[0][1]+s*mat->m[1][1];
	mat->m[0][2]=c*mat->m[0][2]+s*mat->m[1][2];

	mat->m[1][0]=-s*t0+c*mat->m[1][0];
	mat->m[1][1]=-s*t1+c*mat->m[1][1];
	mat->m[1][2]=-s*t2+c*mat->m[1][2];
}

__inline void VCIMatrix34PostRotateZ(br_matrix34 *mat, br_angle rz){
	float s,c;
	float t0,t1,t2,t3;

	s=BR_SIN(rz);
	c=BR_COS(rz);

	t0=mat->m[0][0];
	t1=mat->m[1][0];
	t2=mat->m[2][0];
	t3=mat->m[3][0];

	mat->m[0][0]=c*mat->m[0][0]-s*mat->m[0][1];
	mat->m[1][0]=c*mat->m[1][0]-s*mat->m[1][1];
	mat->m[2][0]=c*mat->m[2][0]-s*mat->m[2][1];
	mat->m[3][0]=c*mat->m[3][0]-s*mat->m[3][1];

	mat->m[0][1]=s*t0+c*mat->m[0][1];
	mat->m[1][1]=s*t1+c*mat->m[1][1];
	mat->m[2][1]=s*t2+c*mat->m[2][1];
	mat->m[3][1]=s*t3+c*mat->m[3][1];
}

__inline void VCIMatrix34PostTranslate(br_matrix34 *mat, br_scalar x, br_scalar y, br_scalar z){
	mat->m[3][0]+=x;
	mat->m[3][1]+=y;
	mat->m[3][2]+=z;
}

__inline void VCIMatrix34PreScale(br_matrix34 *mat, br_scalar sx, br_scalar sy, br_scalar sz){
	mat->m[0][0]*=sx;
	mat->m[0][1]*=sx;
	mat->m[0][2]*=sx;
			    
	mat->m[1][0]*=sy;
	mat->m[1][1]*=sy;
	mat->m[1][2]*=sy;
			    
	mat->m[2][0]*=sz;
	mat->m[2][1]*=sz;
	mat->m[2][2]*=sz;
}

__inline void VCIMatrix34PostScale(br_matrix34 *mat, br_scalar sx, br_scalar sy, br_scalar sz){
	mat->m[0][0]*=sx;
	mat->m[1][0]*=sx;
	mat->m[2][0]*=sx;
	mat->m[3][0]*=sx;

	mat->m[0][1]*=sy;
	mat->m[1][1]*=sy;
	mat->m[2][1]*=sy;
	mat->m[3][1]*=sy;

	mat->m[0][2]*=sz;
	mat->m[1][2]*=sz;
	mat->m[2][2]*=sz;
	mat->m[3][2]*=sz;

}

void __inline VCIMatrix34ApplyV(br_vector3 *A, br_vector3 *B, br_matrix34 *C)
{
	A->v[0] = BR_MAC3(B->v[0],C->m[0][0], B->v[1],C->m[1][0], B->v[2],C->m[2][0]);
	A->v[1] = BR_MAC3(B->v[0],C->m[0][1], B->v[1],C->m[1][1], B->v[2],C->m[2][1]);
	A->v[2] = BR_MAC3(B->v[0],C->m[0][2], B->v[1],C->m[1][2], B->v[2],C->m[2][2]);
}

void __inline VCIMatrix34ApplyP(br_vector3 *A, br_vector3 *B, br_matrix34 *C)
{
	A->v[0] = BR_MAC3(B->v[0],C->m[0][0], B->v[1],C->m[1][0], B->v[2],C->m[2][0]) + C->m[3][0];
	A->v[1] = BR_MAC3(B->v[0],C->m[0][1], B->v[1],C->m[1][1], B->v[2],C->m[2][1]) + C->m[3][1];
	A->v[2] = BR_MAC3(B->v[0],C->m[0][2], B->v[1],C->m[1][2], B->v[2],C->m[2][2]) + C->m[3][2];
}

void __inline VCIMatrix23ApplyP(br_vector2 *A, br_vector2 *B, br_matrix23 *C)
{
    A->v[0] = BR_MAC2(B->v[0],C->m[0][0], B->v[1],C->m[1][0]) + C->m[2][0];
    A->v[1] = BR_MAC2(B->v[0],C->m[0][1], B->v[1],C->m[1][1]) + C->m[2][1];
}

#pragma warning( default : 4035 )

#define BrMatrix34PreRotateX(a,b)			VCIMatrix34PreRotateX(a,b)
#define BrMatrix34PostRotateX(a,b)			VCIMatrix34PostRotateX(a,b)
#define BrMatrix34PreRotateY(a,b)			VCIMatrix34PreRotateY(a,b)
#define BrMatrix34PostRotateY(a,b)			VCIMatrix34PostRotateY(a,b)
#define BrMatrix34PreRotateZ(a,b)			VCIMatrix34PreRotateZ(a,b)
#define BrMatrix34PostRotateZ(a,b)			VCIMatrix34PostRotateZ(a,b)
#define BrMatrix34PostTranslate(a,b,c,d)	VCIMatrix34PostTranslate(a,b,c,d)
#define BrMatrix34PreScale(a,b,c,d)			VCIMatrix34PreScale(a,b,c,d)
#define BrMatrix34PostScale(a,b,c,d)		VCIMatrix34PostScale(a,b,c,d)
#define BrMatrix34ApplyV(a,b,c)				VCIMatrix34ApplyV(a,b,c)
#define BrMatrix34ApplyP(a,b,c)				VCIMatrix34ApplyP(a,b,c)
#define BrMatrix23ApplyP(a,b,c)				VCIMatrix23ApplyP(a,b,c)

#endif

