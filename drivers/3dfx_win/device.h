/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
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
    /* Dispatch table */
	struct br_device_dispatch *dispatch;

    /* Standard object identifier */
	char *identifier;

    /* Device information for queries */
    br_uint_32 version;
    char *creator;
    char *title;
    char *product;
    char *product_version;

    /* List of objects associated with this device */
	void *object_list;

    /* Anchor for all device's resources */
	void *res;

    /* Device clut */
	struct br_device_clut *clut;

    /* Pointers to pixelmaps associated with hardware framebuffers */
	struct br_device_pixelmap * active_buffers[BT_MAX];

    /* Pending colour clear */
	br_uint_32 clear_colour;

    /* Pending depth clear */
	br_uint_32 clear_depth;

	br_boolean initialised;

} br_device;

/*
 * Some useful inline ops.
 */
#define Device3DfxResource(d) (((br_device *)d)->res)

#ifdef __cplusplus
};
#endif
#endif

