/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: custsupt.c 1.1 1997/12/10 16:41:29 jon Exp $
 * $Locker: $
 *
 * Support routines for application
 */
#include "v1db.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: custsupt.c 1.1 1997/12/10 16:41:29 jon Exp $")

#define BR_FIXED_SIGN 0x80000000UL

/*
 * Perspective divide for Z component of vertices
 */
#if BASED_FLOAT
#define PERSP_DIV_Z(a,b)				(-32767.0 * (a)/(b))
#endif

#if BASED_FIXED
#define PERSP_DIV_Z(a,b)				((long)((unsigned long)_FixedDivP(a,b)^BR_FIXED_SIGN))
#endif

#define OUTCODE_POINT(outcode,screen)\
{																				\
	/*																			\
	 * The 6 planes of the view volume...										\
	 */																			\
	if((screen)->v[X] >= (screen)->v[W])										\
		(outcode) ^= (OUTCODE_RIGHT | OUTCODE_N_RIGHT);							\
	if((screen)->v[X] < -(screen)->v[W])										\
		(outcode) ^= (OUTCODE_LEFT | OUTCODE_N_LEFT);							\
																				\
	if((screen)->v[Y] >=  (screen)->v[W])										\
		(outcode) ^= (OUTCODE_TOP | OUTCODE_N_TOP);								\
	if((screen)->v[Y] < -(screen)->v[W])										\
		(outcode) ^= (OUTCODE_BOTTOM | OUTCODE_N_BOTTOM);						\
																				\
	if((screen)->v[Z] >= (screen)->v[W])										\
		(outcode) ^= (OUTCODE_HITHER | OUTCODE_N_HITHER);						\
	if((screen)->v[Z] < -(screen)->v[W])										\
		(outcode) ^= (OUTCODE_YON | OUTCODE_N_YON);								\
}

/*
 * Fetches the current model_to_screen transform
 */
void BR_PUBLIC_ENTRY BrModelToScreenQuery(br_matrix4 *dest)
{
	br_uint_32 dummy;
	br_matrix4 v2s;
	br_matrix34 m2v;

	UASSERT(v1db.rendering);
	ASSERT(v1db.renderer);
    UASSERT_MESSAGE("BrModelToScreenQuery NULL pointer to the destination matrix that holds the current model_to_screen transform", dest != NULL);

	RendererPartQueryBuffer(v1db.renderer, BRT_MATRIX, 0, &dummy,
		(void *)&v2s, sizeof(v2s), BRT_AS_MATRIX4_SCALAR(VIEW_TO_SCREEN));
	RendererPartQueryBuffer(v1db.renderer, BRT_MATRIX, 0, &dummy,
		(void *)&m2v, sizeof(m2v), BRT_AS_MATRIX34_SCALAR(MODEL_TO_VIEW));

	BrMatrix4Mul34(dest, &m2v, &v2s);
}

/*
 * Fetches the current model_to_view transform
 */
void BR_PUBLIC_ENTRY BrModelToViewQuery(br_matrix34 *dest)
{
	br_uint_32 dummy;

	UASSERT(v1db.rendering);
	ASSERT(v1db.renderer);
    UASSERT_MESSAGE("BrModelToViewQuery NULL pointer to the destination matrix that holds the current model_to_view transform", dest != NULL);


	RendererPartQueryBuffer(v1db.renderer, BRT_MATRIX, 0, &dummy,
		(void *)dest, sizeof(*dest), BRT_AS_MATRIX34_SCALAR(MODEL_TO_VIEW));
}

/*
 * Transform and project 0,0,0 into screen space
 *
 * Return 1 if point is behind eye
 */
br_uint_8 BR_PUBLIC_ENTRY BrOriginToScreenXY(br_vector2 *screen)
{
	UASSERT(v1db.rendering);
    UASSERT_MESSAGE("BrOriginToScreenXY NULL pointer to the destination vector to receive the coords.in projected screen space", screen != NULL);


	if(!v1db.model_to_screen_valid) {
		BrModelToScreenQuery(&v1db.model_to_screen);
		v1db.model_to_screen_valid = BR_TRUE;
	}

    UASSERT_MESSAGE("BrOriginToScreenXY - Divide by zero error", v1db.model_to_screen.m[3][3] != 0 );

	screen->v[X] = v1db.vp_ox + BR_MULDIV(v1db.vp_width,v1db.model_to_screen.m[3][0],v1db.model_to_screen.m[3][3]);
	screen->v[Y] = v1db.vp_oy + BR_MULDIV(v1db.vp_height,v1db.model_to_screen.m[3][1],v1db.model_to_screen.m[3][3]);

	return (v1db.model_to_screen.m[3][2] > BR_SCALAR(0.0));
}

/*
 * Transform and project 0,0,0 into screen space, generate X,Y,Z and return outcode
 *
 * If the point is offscreen, it will not be projected
 */
br_uint_32 BR_PUBLIC_ENTRY BrOriginToScreenXYZO(br_vector3 *screen)
{
	br_uint_32 outcode = OUTCODES_NOT;

	UASSERT(v1db.rendering);
    UASSERT_MESSAGE("BrOriginToScreenXYZO NULL pointer to the destination vector to receive the coords.in projected screen space", screen != NULL);

	if(!v1db.model_to_screen_valid) {
		BrModelToScreenQuery(&v1db.model_to_screen);
		v1db.model_to_screen_valid = BR_TRUE;
	}

	OUTCODE_POINT(outcode,((br_vector4 *)(v1db.model_to_screen.m[3])));
    UASSERT_MESSAGE("BrOriginToScreenXYZO - Divide by zero error", v1db.model_to_screen.m[3][3] != 0 );


	if(!(outcode & OUTCODES_ALL)) {
		screen->v[X] = v1db.vp_ox + BR_MULDIV(v1db.vp_width,v1db.model_to_screen.m[3][0],v1db.model_to_screen.m[3][3]);
		screen->v[Y] = v1db.vp_oy + BR_MULDIV(v1db.vp_height,v1db.model_to_screen.m[3][1],v1db.model_to_screen.m[3][3]);
		screen->v[Z] = PERSP_DIV_Z(v1db.model_to_screen.m[3][2],v1db.model_to_screen.m[3][3]);
	}
	return outcode;
}

/*
 * Transform and project a single point into screen space
 *
 * Return 1 if point is behind eye
 */
br_uint_8 BR_PUBLIC_ENTRY BrPointToScreenXY(br_vector2 *screen, br_vector3 *point)
{
	br_vector4 sp;

	UASSERT(v1db.rendering);
    UASSERT_MESSAGE("BrPointToScreenXY NULL pointer to the destination vector to receive the coords.in projected screen space", screen != NULL);
    UASSERT_MESSAGE("BrPointToScreenXY NULL pointer to the source vector containing the coords of the point to project", point != NULL);


	if(!v1db.model_to_screen_valid) {
		BrModelToScreenQuery(&v1db.model_to_screen);
		v1db.model_to_screen_valid = BR_TRUE;
	}

	BrMatrix4ApplyP(&sp,point,&v1db.model_to_screen);


    UASSERT_MESSAGE("BrPointToScreenXY - Divide by zero error", sp.v[3] != 0 );

	screen->v[X] = v1db.vp_ox + BR_MULDIV(v1db.vp_width,sp.v[0],sp.v[3]);
	screen->v[Y] = v1db.vp_oy + BR_MULDIV(v1db.vp_height,sp.v[1],sp.v[3]);

	return (sp.v[2] > BR_SCALAR(0.0));
}

/*
 * Transform and project a single point into screen space
 *
 * Return 1 if point is behind eye
 */
br_uint_32 BR_PUBLIC_ENTRY BrPointToScreenXYZO(br_vector3 *screen, br_vector3 *point)
{
	br_vector4 sp;
	br_uint_32 outcode = OUTCODES_NOT;

	UASSERT(v1db.rendering);
    UASSERT_MESSAGE("BrPointToScreenXYZO NULL pointer to the destination vector to receive the coords.in projected screen space", screen != NULL);
    UASSERT_MESSAGE("BrPointToScreenXYZO NULL pointer to the source vector containing the coords of the point to project", point != NULL);

	if(!v1db.model_to_screen_valid) {
		BrModelToScreenQuery(&v1db.model_to_screen);
		v1db.model_to_screen_valid = BR_TRUE;
	}

	BrMatrix4ApplyP(&sp,point,&v1db.model_to_screen);

	OUTCODE_POINT(outcode,&sp);

    UASSERT_MESSAGE("BrPointToScreenXYZO - Divide by zero error", sp.v[3] != 0 );

	if(!(outcode & OUTCODES_ALL)) {
		screen->v[X] = v1db.vp_ox + BR_MULDIV(v1db.vp_width,sp.v[0],sp.v[3]);
		screen->v[Y] = v1db.vp_oy + BR_MULDIV(v1db.vp_height,sp.v[1],sp.v[3]);
		screen->v[Z] = PERSP_DIV_Z(sp.v[2],sp.v[3]);
	}
	return outcode;
}


/*
 * Transform and project many points into screen space
 */
void BR_PUBLIC_ENTRY BrPointToScreenXYMany(br_vector2 *screens, br_vector3 *points, br_uint_32 npoints)
{
	br_vector4 sp;
	int i;
	
	UASSERT(v1db.rendering);
    UASSERT_MESSAGE("BrPointToScreenXYMany NULL pointer to an array of destination vectors", screens != NULL);
    UASSERT_MESSAGE("BrPointToScreenXYMany NULL pointer to an array of source vectors containing the coords of the points in model space", points != NULL);

	if(!v1db.model_to_screen_valid) {
		BrModelToScreenQuery(&v1db.model_to_screen);
		v1db.model_to_screen_valid = BR_TRUE;
	}

	for(i=0; i< npoints; i++, screens++, points++) {
		BrMatrix4ApplyP(&sp,points,&v1db.model_to_screen);

        UASSERT_MESSAGE("BrPointToScreenXYMany - Divide by zero error", sp.v[3] != 0 );

		screens->v[X] = v1db.vp_ox + BR_MULDIV(v1db.vp_width,sp.v[0],sp.v[3]);
		screens->v[Y] = v1db.vp_oy + BR_MULDIV(v1db.vp_height,sp.v[1],sp.v[3]);
	}
}

/*
 * Transform and project many points into screen space, generate X,Y,Z
 *
 * Outcode each point, and if it is not on screen, don't project it
 */
void BR_PUBLIC_ENTRY BrPointToScreenXYZOMany(br_vector3 *screens, br_uint_32 *outcodes, br_vector3 *points, br_uint_32 npoints)
{
	br_vector4 sp;
	int i;
	br_uint_32 outcode = OUTCODES_NOT;

	UASSERT(v1db.rendering);
    UASSERT_MESSAGE("BrPointToScreenXYZOMany NULL pointer to an array of destination vectors", screens != NULL);
    UASSERT_MESSAGE("BrPointToScreenXYZOMany NULL pointer to an array of source vectors" , points != NULL);
    UASSERT_MESSAGE("BrPointToScreenXYZOMany NULL pointer to an array of outcodes for each point" , points != NULL);

	if(!v1db.model_to_screen_valid) {
		BrModelToScreenQuery(&v1db.model_to_screen);
		v1db.model_to_screen_valid = BR_TRUE;
	}

	for(i=0; i< npoints; i++, screens++, points++,outcodes++) {
		BrMatrix4ApplyP(&sp,points,&v1db.model_to_screen);

		OUTCODE_POINT(outcode,&sp);
		*outcodes = outcode;

		if(outcode & OUTCODES_ALL)
			continue;

		UASSERT_MESSAGE("BrPointToScreenXYZOMany - Divide by zero error", sp.v[3] != 0 );

		screens->v[X] = v1db.vp_ox + BR_MULDIV(v1db.vp_width,sp.v[0],sp.v[3]);
		screens->v[Y] = v1db.vp_oy + BR_MULDIV(v1db.vp_height,sp.v[1],sp.v[3]);
		screens->v[Z] = PERSP_DIV_Z(sp.v[2],sp.v[3]);
	}
}

#if 0
/*
 * Transform and project many points into screen space, generate X,Y,Z
 *
 */
void BR_PUBLIC_ENTRY BrPointToScreenXYZMany(br_vector3 *screens, br_vector3 *points, br_uint_32 npoints)
{
	br_vector4 sp;
	int i;
	br_uint_32 outcode = OUTCODES_NOT;

	UASSERT(v1db.rendering);
    UASSERT_MESSAGE("BrPointToScreenXYZMany NULL pointer to an array of destination vectors", screens != NULL);
    UASSERT_MESSAGE("BrPointToScreenXYZMany NULL pointer to an array of source vectors" , points != NULL);
    
	for(i=0; i< npoints; i++, screens++, points++) {
		BrMatrix4ApplyP(&sp,points,&v1db.model_to_screen);

        UASSERT_MESSAGE("BrPointToScreenXYZMany - Divide by zero error", sp.v[3] != 0 );

		screens->v[X] = v1db.vp_ox + BR_MULDIV(v1db.vp_width,sp.v[0],sp.v[3]);
		screens->v[Y] = v1db.vp_oy + BR_MULDIV(v1db.vp_height,sp.v[1],sp.v[3]);
		screens->v[Z] = PERSP_DIV_Z(sp.v[2],sp.v[3]);
	}
}
#endif

/*
 * Convert Z-buffer depth (0,0xffffffff) to screen Z (-32,768.0 to +32,767.9)
 */
br_scalar BR_PUBLIC_ENTRY BrZbDepthToScreenZ(br_uint_32 depth_z,const br_camera* camera)
{	UASSERT_MESSAGE("BrZbDepthToScreenZ NULL pointer to camera being used for rendering", camera != NULL);	// Not accessed, but should still be valid
	return BrFixedToScalar((br_fixed_ls)(depth_z^BR_FIXED_SIGN));
}

/*
 * Convert Screen Z (-32,768.0 to +32,767.9) to Z-buffer depth (0,0xffffffff)
 *
 * Clamp to view
 */

#define NEAREST (-32767.9f)
#define FARTHEST (32767.9f)
#define TOONEAR (-32768.f)
#define TOOFAR (32768.f)

br_uint_32 BR_PUBLIC_ENTRY BrZbScreenZToDepth(br_scalar sz,const br_camera* camera)
{	br_uint_32 depth=(br_uint_32)BrScalarToFixed(sz)^BR_FIXED_SIGN;
	UASSERT_MESSAGE("BrZbScreenZToDepth NULL pointer to camera being used for rendering", camera != NULL);	// Not accessed, but should still be valid
	/* Not accessed, but should still be valid */
#if BASED_FLOAT
	if (sz<=TOONEAR)
		return 0UL;
	else
		if (sz>=TOOFAR)
			return 0xffffffffUL;
#elif 0	/* Alternative that caters for border line cases - if they can occur */
	if (sz<NEAREST || sz>FARTHEST)	/* Only do further processing if screen z outside normal range */
	{	if (sz<NEAREST)
		{	if (sz<TOONEAR || depth>BR_FIXED_SIGN)	/* Either a long way off or just beyond the border line */
				return 0UL;
		}
		else
		{	if (sz>TOOFAR || depth<BR_FIXED_SIGN)	/* Either a long way off or just beyond the border line */
				return 0xffffffffUL;
		}
	}
#endif
	return depth;
}

#if 0
#if BASED_FIXED
	#define MUL15DIV(a,b) BrFixedDivF(a,b)
#endif

#if BASED_FLOAT
	#define MUL15DIV(a,b) ((a)/(b)*32768.f)
#endif
#endif

/*
 * Convert Z-sort depth (-hither,+yon) to screen Z (-32,768.0 to +32,767.9)
 *
 * Clamp to view
 * 
 */
br_scalar BR_PUBLIC_ENTRY BrZsDepthToScreenZ(br_scalar depth_z,const br_camera* camera)
{	br_scalar hither,yon;
	hither=camera->hither_z;
	yon=camera->yon_z;
    UASSERT_MESSAGE("BrZsDepthToScreenZ NULL pointer to camera being used for rendering", camera != NULL);	// Not accessed, but should still be valid
	
	if (depth_z<=hither)
		return BrFixedToScalar((br_fixed_ls)0x80000000);
	else
		if (depth_z>=yon)
			return BrFixedToScalar((br_fixed_ls)0x7fffffff);

    UASSERT_MESSAGE("BrZsDepthToScreenZ - Divide by zero error", yon != hither );

	return BR_CONST_MUL(BR_MULDIV(BR_SUB(BR_SUB(BR_CONST_MUL(depth_z,2),yon),hither),BR_SCALAR(16384),BR_SUB(yon,hither)),2);
}

/*
 * Convert Screen Z (-32,768.0 to +32,767.9) to Z-sort depth (-hither,+yon)
 *
 * Clamp to view
 */

br_scalar BR_PUBLIC_ENTRY BrZsScreenZToDepth(br_scalar sz,const br_camera* camera)
{	br_scalar hither,yon,depth;
	hither=camera->hither_z;
	yon=camera->yon_z;
    UASSERT_MESSAGE("BrZsScreenZToDepth NULL pointer to camera being used for rendering", camera != NULL);	// Not accessed, but should still be valid
	
/*	depth=BR_CONST_DIV(BR_MULDIV(sz,yon+hither,BR_SCALAR(-32768.f))-yon+hither,2); Not so good */
	depth=BR_CONST_DIV(BR_ADD(BR_MULDIV(BR_CONST_DIV(sz,2),BR_SUB(yon,hither),BR_SCALAR(16384)),BR_ADD(yon,hither)),2);
#if BASED_FLOAT
	if (sz<=TOONEAR)
		return hither;
	else
		if (sz>=TOOFAR)
			return yon;
#endif
	return depth;
}

/*
 * Convert z ordinate in screen space to view space
 */

br_scalar BR_PUBLIC_ENTRY BrScreenZToCamera(const br_actor* camera, br_scalar sz)
{	const br_camera* data;
	br_scalar hither,yon;

	UASSERT_MESSAGE("BrScreenZToCamera NULL pointer to camera actor", camera != NULL);
    UASSERT_MESSAGE("BrScreenZToCamera - invalid camera data", camera->type_data != NULL);

    data=camera->type_data;
	
	hither=data->hither_z;
	yon=data->yon_z;

	UASSERT(data->type==BR_CAMERA_PARALLEL || data->type==BR_CAMERA_PERSPECTIVE);

	#if BASED_FLOAT
	if (sz<=TOONEAR)
		return BR_NEG(hither);
	else
		if (sz>=TOOFAR)
			return BR_NEG(yon);
	#endif

	switch (data->type)
	{
	case BR_CAMERA_PARALLEL:
		return BR_SUB(
					BR_MULDIV(
						BR_CONST_DIV(sz,2),
						BR_CONST_DIV(BR_SUB(hither,yon),2),
						BR_SCALAR(16384))
				,	BR_CONST_DIV(BR_ADD(hither,yon),2)
				);

    case BR_CAMERA_PERSPECTIVE:

       	UASSERT_MESSAGE("BrScreenZToCamera - Divide by zero error",BR_SUB(
							BR_MULDIV(
								BR_CONST_DIV(sz,2)
							,	BR_SUB(yon,hither)
							,	BR_SCALAR(16384)
							)
						,	BR_ADD(hither,yon)
						) != 0);


        return BR_CONST_MUL(
					BR_MULDIV(
						hither
					,	yon
					,	BR_SUB(
							BR_MULDIV(
								BR_CONST_DIV(sz,2)
							,	BR_SUB(yon,hither)
							,	BR_SCALAR(16384)
							)
						,	BR_ADD(hither,yon)
						)
					)
				,	2
				);
	default:
		UASSERT(0);
		return BR_SCALAR(0);
	}
}

void BR_PUBLIC_ENTRY BrScreenXYZToCamera(br_vector3* point,const br_actor* camera,
	const br_pixelmap* screen_buffer, br_int_16 x, br_int_16 y,br_scalar sz)
{	br_scalar hx,hy;
	br_scalar vz;
	const br_camera* data;
	br_angle fov;
	br_scalar scale;

    UASSERT_MESSAGE("BrScreenXYZToCamera NULL pointer to vector to hold the converted point in camera space", point != NULL);
    UASSERT_MESSAGE("BrScreenXYZToCamera NULL pointer to camera actor", camera != NULL);
    UASSERT_MESSAGE("BrScreenXYZToCamera - invalid camera data", camera->type_data != NULL);
    UASSERT_MESSAGE("BrScreenXYZToCamera NULL pointer to the screen buffer", screen_buffer != NULL);

	data=camera->type_data;

	vz=BrScreenZToCamera(camera,sz); // Get the original view Z

    UASSERT_MESSAGE("BrScreenXYZToCamera - Divide by zero error", BrIntToScalar(screen_buffer->width) != 0);
    UASSERT_MESSAGE("BrScreenXYZToCamera - Divide by zero error", BrIntToScalar(screen_buffer->height) != 0);

	hx=BR_DIV(BrIntToScalar(screen_buffer->width-2*(x+screen_buffer->origin_x)),BrIntToScalar(screen_buffer->width));	// homogenous -x	[ hx=2(sx+ox)/sw-1, -hx=(sw-2(sx+ox))/sw ]
	hy=BR_DIV(BrIntToScalar(2*(y+screen_buffer->origin_y)-screen_buffer->height),BrIntToScalar(screen_buffer->height));	// homogenous -y	[ hy=1-2(sy+oy)/sh, -hy=(2(sy+oy)-sh)/sh ]

	switch (data->type)
	{
	case BR_CAMERA_PARALLEL:
		point->v[0]=BR_CONST_DIV(BR_MUL(BR_NEG(hx),BR_MUL(data->width,data->aspect)),2);	// Vx=-hx*cw*ca/2
		point->v[1]=BR_CONST_DIV(BR_MUL(BR_NEG(hy),data->height),2);						// Vy=-hy*ch/2
		point->v[2]=vz;																		// Vz
		break;
	case BR_CAMERA_PERSPECTIVE:
       	fov=(br_angle)(data->field_of_view/2);

        UASSERT_MESSAGE("BrScreenXYZToCamera - Divide by zero error", BR_SIN(fov) != 0);
        scale=BR_DIV(BR_COS(fov),BR_SIN(fov));	// Refer to BrMatrix4Perspective() - we are using its terms in reverse

        UASSERT_MESSAGE("BrScreenXYZToCamera - Divide by zero error", scale != 0);
        point->v[0]=BR_MULDIV(BR_MUL(hx,data->aspect),vz,scale);	// View x
		point->v[1]=BR_MULDIV(hy,vz,scale);						// View y
		point->v[2]=vz;											// View z
		break;
	default:
		UASSERT(0);
		break;
	}
}


