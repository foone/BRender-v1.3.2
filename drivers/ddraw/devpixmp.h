/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: devpixmp.h 1.1 1997/12/10 16:45:12 jon Exp $
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

   /*
    * Pointer to owning device - special case - has to be after public fields
    * because of backwards compatibility for the pixelmaps structure
    */

	struct br_device *device;
	
	struct br_output_facility *output_facility;

	struct br_device_clut *clut;

	/*
	 * Bits per pixel
	 */
	br_uint_16 pixel_bits;

	/*
	 * Fullscreen flag
	 */
	br_boolean fullscreen;

	/*
	 * Pointer to top of pixel memory
	 */
	void *bits;

	/*
	 * Device context
	 */
	HDC dc;

	/*
	 * Bitmap handle
	 */
	HBITMAP bitmap;

	/*
	 * Window handle
	 */
   HWND hwnd;


   /*
    *  Set if this is the primary surface
    */

   br_boolean bPrimary ;

	br_boolean sub_pixelmap;

	/*
	 * Palette handle
	 */
	HPALETTE hpal;

	/*
	 * Various DirectDraw objects
	 */
	LPDIRECTDRAW2		DD;
	LPDIRECTDRAWSURFACE surface;
	LPDIRECTDRAWCLIPPER clipper;
	LPDIRECTDRAWPALETTE palette;

	/*
	 * ModeX kludge
	 */
	LPDIRECTDRAWSURFACE offscreen;

} br_device_pixelmap;

#ifdef __cplusplus
};
#endif
#endif



