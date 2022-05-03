/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * Cheat method of copying into a window
 */
#ifndef _COPY2WIN_H_
#define _COPY2WIN_H_

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Structure containing necessary handles etc.
 */
typedef struct copy_to_win {

	/*
	 * Target window and destination rectangle
	 */
	HWND window;
	RECT region;

	/*
	 * Dimensions of intermediate buffer
	 */
	br_int_32 width;
	br_int_32 height;

	/*
	 * Dimensions of destination rectangle
	 */
	br_int_32 window_width;
	br_int_32 window_height;

	/*
	 * Intermediate compatible display context, bitmap and palette
	 */
	HDC dc;
	HBITMAP bitmap;
	HPALETTE palette;

	/*
	 * Pointer to bitmap data
	 */
	LPVOID bitmap_data;
	   
} copy_to_win;


copy_to_win *InitCopyToWin(HWND window, br_uint_32 max_width, br_uint_32 max_height);
br_error FreeCopyToWin(copy_to_win *copy);

br_error SetCopyToWinWindowSize(copy_to_win *copy, br_uint_32 width, br_uint_32 height);

br_error CopyToWin(copy_to_win *copy, GrBuffer_t buffer, br_uint_32 width, br_uint_32 height);

#ifdef __cplusplus
};
#endif
#endif

