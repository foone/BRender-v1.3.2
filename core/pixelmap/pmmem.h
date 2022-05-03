/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: pmmem.h 1.1 1997/12/10 16:41:27 jon Exp $
 * $Locker: $
 *
 * Private memory pixelmap structure
 */
#ifndef _PMMEM_H_
#define _PMMEM_H_

/*
 * Private state of memory pixelmap
 */
typedef struct br_device_pixelmap {
	/*
	 * Dispatch table
	 */
	struct br_device_pixelmap_dispatch *dispatch;

	/*
	 * Standard pixelmap members
	 */
	char *pm_identifier;

	BR_PIXELMAP_MEMBERS

} br_device_pixelmap;


/*
 * Useful info about each pixelmap type
 */
struct pm_type_info {
	/*
	 * Size, in bits, of each pixel (including padding)
	 */
	br_uint_16 bits;

	/*
	 * Size, in bytes, that should be used for saving/loading
	 */
	br_uint_16 file_size;

	/*
	 * Alignment of rows, in pixels
	 */
	br_uint_16 align;

	/*
	 * Mask of channels in pixelmap
	 */
	br_uint_16 channels;

};

#ifdef __cplusplus
extern "C" {
#endif

extern const struct pm_type_info pmTypeInfo[];

#ifdef __cplusplus
};
#endif


#define DevicePixelmapPixelBytes(pm) \
		(pmTypeInfo[((br_device_pixelmap *)(pm))->pm_type].bits >> 3)

#define DevicePixelmapPixelBits(pm) \
		(pmTypeInfo[((br_device_pixelmap *)(pm))->pm_type].bits)

#define DevicePixelmapFileBytes(pm) \
		(pmTypeInfo[((br_device_pixelmap *)(pm))->pm_type].file_size)

#define DevicePixelmapChannels(pm) \
		(pmTypeInfo[((br_device_pixelmap *)(pm))->pm_type].channels)

#define DevicePixelmapMemAddress(pm,x,y,bpp) \
		((char *)(((br_device_pixelmap *)(pm))->pm_pixels)+\
		(((br_device_pixelmap *)(pm))->pm_base_y+(y))*((br_device_pixelmap *)(pm))->pm_row_bytes+\
		(((br_device_pixelmap *)(pm))->pm_base_x+(x)) * (bpp))

#endif

