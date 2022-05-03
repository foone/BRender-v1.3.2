/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: devpixmp.h 1.1 1997/12/10 16:50:09 jon Exp $
 * $Locker: $
 *
 * Private device pixelmap structure
 */
#ifndef _DEVPIXMP_H_
#define _DEVPIXMP_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Private state of device pixelmap
 */
typedef struct br_device_pixelmap {
	/*
	 * Dispatch table
	 */
	struct br_device_pixelmap_dispatch *dispatch;

	/*
	 * Standard handle identifier
	 */
	char *pm_identifier;

	/** Standard pixelmap members (not including identifier) **/

	BR_PIXELMAP_MEMBERS

	/** End of br_pixelmap fields **/
	
	struct br_output_facility *output_facility;

	struct br_device_clut *clut;

	/* Type of device pixelmap */
	br_uint_16 buffer;

	/* Bits per pixel */
	br_uint_16 pixel_bits;

	/* Fullscreen flag */
	br_boolean fullscreen;

	/* Set if not a sub-pixelmap */
	br_boolean parent;

	/* Pointer to top of pixel memory */
	void *bits;

	/* Device context */
	HDC dc;

	/* Bitmap handle */
	HBITMAP bitmap;

	/* Pointer to DIB information */
	BITMAPINFO *info;

	/* Window handle */
	HWND hwnd;

	/* Palette handle */
	HPALETTE hpal;

	/* Various DirectDraw objects */
	LPDIRECTDRAWSURFACE surface;
	LPDIRECTDRAWCLIPPER clipper;
	LPDIRECTDRAWPALETTE palette;

  /* Is this surface in system or video memory */
  br_boolean videomemory;

  /* Offsets for sub pixelmaps from parent */
  br_uint_32 sub_x_offset;
  br_uint_32 sub_y_offset;

  /* Cache for address to avoid unnecessary locks */
  br_boolean	flipped;

} br_device_pixelmap;

/*
 * Device pixelmap type flags
 */
enum buffer_types {
	BUFFER_SCREEN			= 0x0001,
	BUFFER_OFFSCREEN		= 0x0002,

};

#ifdef __cplusplus
};
#endif
#endif


