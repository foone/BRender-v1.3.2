/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: rendfcty.h 1.1 1997/12/10 16:52:33 jon Exp $
 * $Locker: $
 *
 * Private renderer facility structure
 */
#ifndef _RENDFCTY_H_
#define _RENDFCTY_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Private state of renderer facility
 */
typedef struct br_renderer_facility {
	/*
	 * Dispatch table
	 */
	struct br_renderer_facility_dispatch *dispatch;

	/*
	 * Standard object identifier
	 */
	char *identifier;

    /*
     * Pointer to owning device
     */
    struct br_device *device;

	/*
	 * List of objects associated with this device
	 */
	void *object_list;

	/*
	 * Default state
	 */
	struct state_all default_state;

	/*
	 * Number of instances
	 */
	br_int_32	num_instances;

} br_renderer_facility;

#ifdef __cplusplus
};
#endif
#endif

