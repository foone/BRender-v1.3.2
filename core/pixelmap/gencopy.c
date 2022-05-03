/*
 * Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: gencopy.c 1.1 1997/12/10 16:41:24 jon Exp $
 * $Locker: $
 *
 * Copy between device pixelmaps on different devices
 *
 * XXX: Needs writing!
 */
#include "pm.h"
#include "brassert.h"

BR_RCS_ID("$Id: gencopy.c 1.1 1997/12/10 16:41:24 jon Exp $")

br_error GeneralRectangleCopy(
	br_device_pixelmap *dest,
	br_point *p,
	br_device_pixelmap *src,
	br_rectangle *r)
{
	return BRE_UNSUPPORTED;
}

br_error GeneralCopy(
	br_device_pixelmap *dst,
	br_device_pixelmap *src)
{
	return BRE_UNSUPPORTED;
}

br_error GeneralRectangleStretchCopy(
	struct br_device_pixelmap *dst,
	br_rectangle *d,
	struct br_device_pixelmap *src,
	br_rectangle *s)
{
	return BRE_UNSUPPORTED;
}
