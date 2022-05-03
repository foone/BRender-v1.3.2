/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: object.h 1.1 1997/12/10 16:54:08 jon Exp $
 * $Locker: $
 *
 * Private object structure
 */
#ifndef _OBJECT_H_
#define _OBJECT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Private state object - all device objects have an identifer pointer as their first element
 */
typedef struct br_object {
	/*
	 * Dispatch table
	 */
	struct br_object_dispatch *dispatch;

	/*
	 * Standard object identifier
	 */
	char *identifier;

} br_object;

/*
 * Override general case with special case for this driver
 */
#define ObjectVESADevice(d) ((br_device *)&DriverDeviceVESA)
#define ObjectVESAIdentifier(d) (((br_object *)d)->identifier)

#ifdef __cplusplus
};
#endif
#endif

