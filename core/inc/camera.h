/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: camera.h 1.1 1997/12/10 16:41:16 jon Exp $
 * $Locker: $
 *
 * Definitons for a camera
 */
#ifndef _CAMERA_H_
#define _CAMERA_H_

enum {
	BR_CAMERA_PARALLEL,
	BR_CAMERA_PERSPECTIVE_FOV,
	BR_CAMERA_PERSPECTIVE_WHD,
	BR_CAMERA_PARALLEL_OLD,
	BR_CAMERA_PERSPECTIVE_FOV_OLD,
};

/*
 * Backwards compatibility
 */
#define	BR_CAMERA_PERSPECTIVE		BR_CAMERA_PERSPECTIVE_FOV
#define	BR_CAMERA_PERSPECTIVE_OLD	BR_CAMERA_PERSPECTIVE_FOV_OLD

typedef struct br_camera {
	/*
	 * Optional identifier
	 */
	char *identifier;

	/*
	 * Type of camera
	 */
	br_uint_8 type;

	/*
	 * Field of view
	 * (BR_CAMERA_PERSPECTIVE_FOV only)
	 */
	br_angle field_of_view;

	/*
	 * Front and back of view volume in view coordinates
	 */
	br_scalar hither_z;
	br_scalar yon_z;

	/*
	 * Aspect ratio of viewport
	 * (BR_CAMERA_PERSPECTIVE_FOV only)
	 */
	br_scalar aspect;
	
	/*
	 * Width and height of projection surface
	 * (BR_CAMERA_PERSPECTIVE_WHD and BR_CAMERA_PARALLEL only)
	 */

	br_scalar width;
	br_scalar height;

	/*
	 * Distance of projection plane from center of projection
	 * (BR_CAMERA_PERSPECTIVE_WHD only)
	 */
	br_scalar distance;

        void * user;

} br_camera;


#endif

