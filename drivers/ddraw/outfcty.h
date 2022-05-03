/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: outfcty.h 1.1 1997/12/10 16:45:27 jon Exp $
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
    * Pointer to owning device
    */

   struct br_device *device;

	/*
	 * List of instances associated with type
	 */
	void *object_list;

	/*
	 * Size or maximum size of mode in pixels
	 */
	br_int_32 width;
	br_int_32 height;	

	/*
	 * Bit depth
	 */
	br_int_32 colour_bits;
	br_int_32 depth_bits;

	/*
	 * Pixelmap types
	 */
	br_int_32 colour_type;
	br_int_32 depth_type;

	/*
	 * Is there a CLUT?
	 */
	br_boolean indexed;

	/*
	 * Fullscreen?
	 */
	br_boolean fullscreen;

	/*
	 * Flags to indicate VGA or ModeX modes, which need to be handled specially
	 */
	br_boolean standard_vga;
	br_boolean modex;

	/*
	 * Number of instances
	 */
	br_int_32 num_instances;

} br_output_facility;

#define OutputFacilityDirectDrawType(c) (((br_output_facility *)c)->colour_type)

#ifdef __cplusplus
};
#endif
#endif


