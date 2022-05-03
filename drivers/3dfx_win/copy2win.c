/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * Cheat method of copying into a window
 */
#include <stddef.h>
#include <string.h>

#include "drv.h"
#include "host.h"

#include "copy2win.h"


copy_to_win *InitCopyToWin(HWND window, br_uint_32 max_width, br_uint_32 max_height)
{
	copy_to_win *copy;

	struct {

		BITMAPINFOHEADER header;
		DWORD colour_mask[3];

	} bitmap_info;

	struct {

		LOGPALETTE header;
		PALETTEENTRY colour_entry[255];

	} palette_info;

	br_uint_32 i, r, g, b;

	HDC dc;

	copy = BrMemAllocate(sizeof(*copy), BR_MEMORY_DRIVER);
	if (copy == NULL)
		return NULL;

	/*
	 * Deactivate the display
	 */
	grSstControl(GR_CONTROL_DEACTIVATE);

	/*
	 * Store the window handle and the dimensions, ensuring that each line
	 * is a whole number of QWORDS
	 */
	copy->window = window;

	copy->width = (max_width + 3) & ~3;
	copy->height = max_height;

	copy->window_width = max_width;
	copy->window_height = max_height;

	/*
	 * Create a compatible DC and bitmap
	 */
	copy->dc = CreateCompatibleDC(NULL);

	bitmap_info.header.biSize = sizeof(BITMAPINFOHEADER);
	bitmap_info.header.biWidth = copy->width;
	bitmap_info.header.biHeight = -copy->height;
	bitmap_info.header.biPlanes = 1;
	bitmap_info.header.biBitCount = 16;
	bitmap_info.header.biCompression = BI_BITFIELDS;
	bitmap_info.header.biSizeImage = 0;
	bitmap_info.header.biXPelsPerMeter = 0;
	bitmap_info.header.biYPelsPerMeter = 0;
	bitmap_info.header.biClrUsed = 0;
	bitmap_info.header.biClrImportant = 0;

	bitmap_info.colour_mask[0] = 0xf800;
	bitmap_info.colour_mask[1] = 0x07e0;
	bitmap_info.colour_mask[2] = 0x001f;

	copy->bitmap = CreateDIBSection(copy->dc, (BITMAPINFO *)&bitmap_info, DIB_RGB_COLORS, &copy->bitmap_data, NULL, 0);

	SelectObject(copy->dc, copy->bitmap);

	/*
	 * Create a palette containing equally spaced colours from the colour
	 * cube for 'acceptable' 8-bit behaviour
	 */
	palette_info.header.palVersion = 0x300;
	palette_info.header.palNumEntries = 256;

	i = 0;

	for (r = 0; r < 8; r++)
		for (g = 0; g < 8; g++)
			for (b = 0; b < 4; b++) {
				
				palette_info.header.palPalEntry[i].peRed = (r << 5) | (r << 2) | (r >> 1);
				palette_info.header.palPalEntry[i].peGreen = (g << 5) | (g << 2) | (g >> 1);
				palette_info.header.palPalEntry[i].peBlue = (b << 6) | (b << 4) | (b << 2) | b;
				palette_info.header.palPalEntry[i].peFlags = 0;

				i++;
			}

	copy->palette = CreatePalette(&palette_info.header);

	return copy;
}

br_error FreeCopyToWin(copy_to_win *copy)
{
	if (copy->palette != NULL) {
		DeleteObject(copy->palette);
		copy->palette = NULL;
	}

	if (copy->bitmap != NULL) {
		DeleteObject(copy->bitmap);
		copy->bitmap = NULL;
		copy->bitmap_data = NULL;
	}

	if (copy->dc != NULL) {
		DeleteObject(copy->dc);
		copy->dc = NULL;
	}

	copy->window = NULL;

	BrMemFree(copy);

	return BRE_OK;
}


br_error SetCopyToWinWindowSize(copy_to_win *copy, br_uint_32 width, br_uint_32 height)
{
	copy->window_width = width;
	copy->window_height = height;

	return BRE_OK;
}


br_error CopyToWin(copy_to_win *copy, GrBuffer_t buffer, br_uint_32 width, br_uint_32 height)
{
	GrLfbInfo_t lfb_info = { sizeof(GrLfbInfo_t) };
	br_int_32 i;
	char *src, *dest;
	HDC dc;
	br_uint_16 qual;

	if (width > copy->width)
		width = copy->width;

	if (height > copy->height)
		height = copy->height;

	/*
	 * Get a linear frame buffer pointer to the specified buffer, and copy it
	 * into the bitmap
	 */
	if (!grLfbLock(GR_LFB_READ_ONLY, buffer, 0, GR_ORIGIN_UPPER_LEFT, FXFALSE, &lfb_info))
		return BRE_FAIL;

	HostSelectorDS(&qual);

	if (width == copy->width && lfb_info.strideInBytes == copy->width * 2)
		_MemCopy3DFX_A(copy->bitmap_data, qual, lfb_info.lfbPtr, qual, width * height);
	else
		_MemRectCopy3DFX_A(copy->bitmap_data, qual, lfb_info.lfbPtr, qual, width, height, copy->width * 2, lfb_info.strideInBytes);

	grLfbUnlock(GR_LFB_READ_ONLY, buffer);

	/*
	 * Get a DC to the window, select our palette into it and blit the screen
	 * in
	 */
	dc = GetDC(copy->window);

	SelectPalette(dc, copy->palette, BR_FALSE);
	RealizePalette(dc);

	if (width == copy->window_width && height == copy->window_height)
		BitBlt(dc, 0, 0, width, height, copy->dc, 0, 0, SRCCOPY);
	else
		StretchBlt(dc, 0, 0, copy->window_width, copy->window_height,
			copy->dc, 0, 0, width, height, SRCCOPY);

	ReleaseDC(copy->window, dc);

	return BRE_OK;
}

