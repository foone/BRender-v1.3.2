/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: device.h 1.1 1997/12/10 16:50:52 jon Exp $
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
	 * Available buffering methods
	 */
	br_boolean buffer_method_directdraw;

	/* Number of output facilities available */
	br_uint_32 output_facility_count;

	/* Actual bit depth of modes that Windows claims are 16-bit */
	br_uint_32 bits16;

   /* Brender query support */
   br_uint_32 version;
   char *creator;
   char *title;
   char *product;
   char *product_version;

	/* Critical section object */
	CRITICAL_SECTION csection;


	/*
	 * Handle for s3dtk.dll
	 */
	HMODULE s3dtk;

	/*
	 * Pointers to routines in s3dtk.dll
	 */
	ULONG (FAR *S3DTK_InitLib)(ULONG lParam);
	ULONG (FAR *S3DTK_ExitLib)(void);
	ULONG (FAR *S3DTK_CreateRenderer)(ULONG Param1, void **ppFunctionList);
	ULONG (FAR *S3DTK_DestroyRenderer)(void **ppFunctionList);

} br_device;


/*
 * Some useful inline ops.
 */
#define DeviceS3Resource(d) (((br_device *)d)->res)
#define DeviceS3OriginalMode(d) (((br_device *)d)->original_mode)
#define DeviceS3CurrentMode(d) (((br_device *)d)->current_mode)
#define DeviceS3CurrentModeSet(d,m) ((((br_device *)d)->current_mode) = m)
#define DeviceS3Clut(d) (((br_device *)d)->clut)

extern LPDIRECTDRAW gpDD;

#ifdef __cplusplus
};
#endif
#endif

