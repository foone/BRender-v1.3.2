/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: device.h 1.1 1997/12/10 16:45:09 jon Exp $
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
    * Pointer to owning device
    */

   struct br_device *device;

	/*
	 * List of objects associated with this device
	 */
	void *object_list;

	/*
	 * Anchor for all device's resources
	 */
	void *res;

	/*
	 * Critical section object
	 */
	CRITICAL_SECTION csection;

	/*
	 * DirectDraw module handle
	 */
	HMODULE hddraw;

	/*
	 * DirectDraw object
	 */
	LPDIRECTDRAW2 DD;

	/*
	 * HWND to be used for enumeration (to allow use of ModeX modes)
	 */
	HWND window;

	/*
	 * Address of DirectDrawCreate function
	 */
	FARPROC create;

	/*
	 * Number of output facilities available
	 */
	br_uint_32 output_facility_count;

   struct device_templates templates;

} br_device;

/*
 * Some useful inline operations
 */
#define DeviceDirectDrawResource(d) (((br_device *)d)->res)
#define DeviceDirectDrawOriginalMode(d) (((br_device *)d)->original_mode)
#define DeviceDirectDrawCurrentMode(d) (((br_device *)d)->current_mode)
#define DeviceDirectDrawCurrentModeSet(d,m) ((((br_device *)d)->current_mode) = m)
#define DeviceDirectDrawClut(d) (((br_device *)d)->clut)

#ifdef __cplusplus
};
#endif
#endif


