/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: outfcty.h 1.1 1997/12/10 16:54:18 jon Exp $
 * $Locker: $
 *
 * Private output facility structure
 */
#ifndef _OUTFCTY_H_
#define _OUTFCTY_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Private state of output facility
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
	 * List of instances associated with facility
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

	/*
	 * Number of instances
	 */
	br_int_32	num_instances;

	/*
	 * VESA mode for this 
	 */
	br_uint_16	vesa_mode;

	/*
	 * VESA mode info for this 
	 */
	struct vesa_modeinfo modeinfo;

} br_output_facility;

#define OutputFacilityVESAType(c) (((br_output_facility *)c)->colour_type)
#define OutputFacilityVESABits(c) (((br_output_facility *)c)->colour_bits)

#define OutputFacilityVESAMode(c) (((br_output_facility *)c)->vesa_mode)
#define OutputFacilityVESAModeInfo(c) (&(((br_output_facility *)c)->modeinfo))

#ifdef __cplusplus
};
#endif
#endif

