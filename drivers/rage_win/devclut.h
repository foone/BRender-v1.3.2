/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: devclut.h 1.1 1997/12/10 16:50:05 jon Exp $
 * $Locker: $
 *
 * Private device CLUT state
 */
#ifndef _DEVCLUT_H_
#define _DEVCLUT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Private state of device CLUT
 */
typedef struct br_device_clut {
	/*
	 * Dispatch table
	 */
	struct br_device_clut_dispatch *dispatch;

	/*
	 * Standard handle identifier
	 */
	char *identifier;

	/*
	 * Parent device pixelmap
	 */
	br_device_pixelmap *devpm;
} br_device_clut;

#ifdef __cplusplus
};
#endif
#endif



