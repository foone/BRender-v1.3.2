/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: device.h 1.1 1997/12/10 16:45:56 jon Exp $
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
 * Possible types of buffer
 */
enum {
	BT_FRONTSCREEN,
	BT_BACKSCREEN,
	BT_DEPTH,
	BT_ALPHA,
	BT_SYSMEM,
	BT_MAX,
};

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

    /* Device information for queries */
    br_uint_32 version;
    char *creator;
    char *title;
    char *product;
    char *product_version;


	/*
	 * List of objects associated with this device
	 */
	void *object_list;

	/*
	 * Anchor for all device's resources
	 */
	void *res;

	struct br_device_clut *clut;

	/*
	 * Pointers to pixelmaps associated with hardware framebuffers
	 */
	struct br_device_pixelmap * active_buffers[BT_MAX];

    /*
     * Mystique hardware info
     */
    T_msiInfo *msiInfo;

} br_device;

/*
 * Some useful inline ops.
 */
#define DeviceMystResource(d) (((br_device *)d)->res)
extern T_msiInfo	*msiInfo;

#ifdef __cplusplus
};
#endif
#endif

