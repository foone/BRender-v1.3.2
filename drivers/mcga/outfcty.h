/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: outfcty.h 1.1 1997/12/10 16:45:47 jon Exp $
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
         * BIOS mode for INT 10
         */
       	br_int_32	bios_mode;

	/* Size of mode in pixels
	 */
	br_int_32	width;
	br_int_32	height;	

	/* Bit depth
	 */
	br_int_32	colour_bits;
//	br_int_32	depth_bits;

	/* Pixelmap types
	 */
	br_int_32	colour_type;
//	br_int_32	depth_type;

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

	/* Default CLUT
	 */
	struct br_device_clut *default_clut;

} br_output_facility;

#define OutputFacilityVGAType(c) (((br_output_facility *)c)->colour_type)

#define STATIC_OUTPUT_FACILITY_VGA(id,w,h,cb,db,ct,dt,idx, vm, hm)\
	{NULL,  id, NULL, w, h, cb, db, ct, dt, idx, vm, hm, }

#ifdef __cplusplus
};
#endif
#endif

