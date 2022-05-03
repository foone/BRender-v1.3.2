/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * Private output facility structure
 */
#ifndef _OUTFCTY_H_
#define _OUTFCTY_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Private state of output type
 */

typedef struct br_output_facility {
	/*
	 * Dispatch table
	 */
	struct br_output_facility_dispatch *dispatch;

	/*
	 * Standard object identifier
	 */
	char *identifier;

	/*
	 * List of instances associated with type
	 */
	void *object_list;

	/* Size of mode in pixels
	 */
	br_int_32	width;
	br_int_32	height;	

	/* Bit depth
	 */
	br_int_32	colour_bits;
	br_int_32	depth_bits;

	/* Pixelmap types
	 */
	br_int_32	colour_type;
	br_int_32	depth_type;

	/* Is there a CLUT?
	 */
	br_boolean	indexed;

	/* Video memory size
	 */
	br_int_32	video_memory;
	br_int_32	host_memory;

	/* Fullscreen flag - always TRUE for this driver */
	br_boolean	fullscreen;

	/*
	 * Number of instances
	 */
	br_int_32	num_instances;

	/* Primitive library to use
	 */
	struct br_primitive_library *prim_lib;

	/* Resolution code for Glide screen */
	GrScreenResolution_t glide_res_code;

} br_output_facility;


#ifdef __cplusplus
};
#endif
#endif

