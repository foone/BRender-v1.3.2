/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: device.h 1.1 1997/12/10 16:53:34 jon Exp $
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
 * Mode dependant information stored at mode setup
 */
struct vesa_scanline {
	br_uint_32	offset;
	br_uint_32	page;
	br_uint_32	split;
	br_uint_32	_pad;
};

struct vesa_work {
	br_uint_32	window_start;
	br_uint_16	selector;
	br_uint_32	window_end;
	br_uint_32	scanline_max;
	struct vesa_scanline *scanline_table;

	br_uint_32	pixel_size;
	br_uint_32	full_banks;
	br_uint_32	bytes_left;
	br_uint_32	bank_increment;

	br_uint_32	scanlines_per_page;
	br_uint_32	scanlines_remainder;

	br_uint_32	page_size;
	br_uint_32	stride;

	br_uint_32	current_page;

	br_boolean	access_linear;
	br_boolean	bank_protected;
	br_uint_16	scanline_breaks;

	void		*bank_function;

	br_uint_16	physical_selector;
	br_uint_32	physical_address;
	void 		*linear;
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

	/*
	 * List of objects associated with this device
	 */
	void *object_list;

	/*
	 * Anchor for all device's resources
	 */
	void *res;

	/*
	 * VESA mode at startup
	 */
	br_uint_16 original_mode;

	/*
	 * Information from arguments
	 */
	br_uint_16 version_limit;
	br_boolean	use_pmi;
	br_boolean	use_linear;
	br_boolean	set_stride;

	/*
	 * Current mode
	 */
	br_uint_16 current_mode;

	/*
	 * Bank switching info
	 */
	struct vesa_info info;

	struct vesa_work work;

	struct br_device_clut *clut;

	br_boolean screen_active;

} br_device;

/*
 * Some useful inline ops.
 */
#define DeviceVESAResource(d) (((br_device *)d)->res)
#define DeviceVESAOriginalMode(d) (((br_device *)d)->original_mode)
#define DeviceVESACurrentMode(d) (((br_device *)d)->current_mode)
#define DeviceVESACurrentModeSet(d,m) ((((br_device *)d)->current_mode) = m)
#define DeviceVESAInfo(d) (&(((br_device *)d)->info))
#define DeviceVESAWork(d) (&(((br_device *)d)->work))
#define DeviceVESAClut(d) (((br_device *)d)->clut)

/*
 * Global device structure - so that low level asm code can get at it
 */
extern br_device DriverDeviceVESA;

#ifdef __cplusplus
};
#endif
#endif

