/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: device.h 1.1 1997/12/10 16:50:08 jon Exp $
 * $Locker: $
 *
 * Private device driver structure
 */
#ifndef _DEVICE_H_
#define _DEVICE_H_

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Private state of device
 */
typedef struct br_device {
	/*
	 * Dispatch table
	 */
	struct br_device_dispatch *dispatch;

	/*
	 * Standard object identifier
	 */
	char *identifier;

	/*
	 * List of objects associated with this device
	 */
	void *object_list;

	/*
	 * Anchor for all device's resources
	 */
	void *res;

	/*
	 * Default (and only) CLUT
	 */
	struct br_device_clut *clut;

	/*
	 * Critical section object
	 */
	CRITICAL_SECTION csection;

  /* Number of output facilities available */
  br_uint_32 output_facility_count;

  /* Actual bit depth of modes that Windows claims are 16-bit */
  br_uint_32 bits16;

  /* Brender Query info */

  br_uint_32 version;
  char *creator;
  char *title;
  char *product;
  char *product_version;
} br_device;


/*
 * Some useful inline ops.
 */
#define DeviceATIResource(d) (((br_device *)d)->res)
#define DeviceATIOriginalMode(d) (((br_device *)d)->original_mode)
#define DeviceATICurrentMode(d) (((br_device *)d)->current_mode)
#define DeviceATICurrentModeSet(d,m) ((((br_device *)d)->current_mode) = m)
#define DeviceATIClut(d) (((br_device *)d)->clut)


/*
 * Direct Draw prototypes
 */
extern LPDIRECTDRAW gpDD;

#ifdef __cplusplus
};
#endif
#endif

