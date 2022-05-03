/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: fmt.h 1.2 1998/07/28 14:33:10 jon Exp $
 * $Locker: $
 *
 * Definitions for foreign file format support
 */
#ifndef _FMT_H_
#define _FMT_H_

/*
 * Structure for passing results back from foreign format loaders
 */
typedef struct br_fmt_results {

	/*
	 * Pointers to tables of pointers to converted items
	 */
	br_actor	**actors;
	br_model	**models;
	br_light	**lights;
	br_camera	**cameras;
	br_vector4	**clip_planes;
	br_bounds3	**bounds;
	br_bounds3	**bounds_correct;
	br_material	**materials;
	br_pixelmap	**pixelmaps;

	/*
	 * Size of each array
	 */
	br_uint_32 nactors;
	br_uint_32 nmodels;
	br_uint_32 nlights;
	br_uint_32 ncameras;
	br_uint_32 nclip_planes;
	br_uint_32 nbounds;
	br_uint_32 nbounds_correct;
	br_uint_32 nmaterials;
	br_uint_32 npixelmaps;

} br_fmt_results;

/*
 * General options structure passed to all foreign format loaders
 */
typedef struct br_fmt_options {

	/*
	 * Flags determining which items to convert
	 */
	br_uint_32 convert_flags;

} br_fmt_options;

/*
 * Type of items to convert
 */
enum {
	BR_FMT_ACTORS			= 0x0001,
	BR_FMT_MODELS			= 0x0002,
	BR_FMT_LIGHTS			= 0x0004,
	BR_FMT_CAMERAS			= 0x0008,
	BR_FMT_CLIP_PLANES		= 0x0010,
	BR_FMT_BOUNDS			= 0x0020,
	BR_FMT_BOUNDS_CORRECT	= 0x0040,
	BR_FMT_MATERIALS		= 0x0080,
	BR_FMT_PIXELMAPS		= 0x0100,

	BR_FMT_CONVERT_ALL		= 0x7fff
};

/*
 * Type definition for a message reporting callback
 */
typedef void BR_CALLBACK br_fmt_report_cbfn(char *string);

/*
 * Options structure passed to 3DS format loader
 */
typedef struct br_3ds_options {

	/*
	 * Conversion flags
	 */
	br_uint_32 flags;

	/*
	 * Camera hither and yon values
	 */
	br_scalar hither;
	br_scalar yon;

	/*
	 * Scaling applied to models and hierarchy
	 */
	br_scalar scale;

	/*
	 * Optional function pointer for reporting conversion messages
	 */
	br_fmt_report_cbfn *report;

} br_3ds_options;

/*
 * 3DS conversion flags
 */
enum {
	BR_3DS_FLAT_HIERARCHY		=	0x00000001,
	BR_3DS_SET_PERSPECTIVE		=	0x00000002,
	BR_3DS_CORRECT_AXES			=	0x00000004,
	BR_3DS_APPLY_MESH_MATRIX	=	0x00000008,
	BR_3DS_APPLY_PIVOT			=	0x00000010,
	BR_3DS_DUMMY_LIGHTS			=	0x00000020,
	BR_3DS_DUMMY_CAMERAS  		=	0x00000040,
	BR_3DS_SET_HITHER			=	0x00000080,
	BR_3DS_SET_YON				=	0x00000100,
	BR_3DS_APPLY_SCALE			=	0x00000200,
	BR_3DS_KEEP_REPEATED_MODELS	=	0x00000400,
	BR_3DS_USE_MODEL_PIVOT		=	0x00000800,

};


/*
 * VUE format structures
 */

// Pointer to actor plus desired transform matrix.
struct vue_transform {
	br_actor       *actor;	// slot for actor to be moved by vue
	br_matrix34     mat;	// by this matrix
};

// All desired actor transforms required for a single frame.

struct vue_frame {
	struct vue_transform *transforms;//actor/transforms
	int             ntransforms;	// number of transforms for that frame
};


typedef struct br_vue{
	int 	nframes;
	int	ntransforms;
	int 	frame_delta;	// default=1
	int 	current_frame;	// initialised to zero
struct 	vue_frame *frames;
} br_vue;


#ifndef _FMT_P_H_
#include "fmt_p.h"
#endif

#endif

