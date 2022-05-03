/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * Private object structure
 */
#ifndef _OBJECT_H_
#define _OBJECT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Private state of device pixelmap
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
#define Object3DfxDevice(d) ((br_device *)&DriverDevice3Dfx)
#define Object3DfxIdentifier(d) (((br_object *)d)->identifier)

#ifdef __cplusplus
};
#endif
#endif

