/*
 * Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: pixelmap.c 1.1 1997/12/10 16:41:25 jon Exp $
 * $Locker: $
 *
 * Manipulating pixelmaps
 */

#include "pm.h"
#include "brassert.h"

BR_RCS_ID("$Id: pixelmap.c 1.1 1997/12/10 16:41:25 jon Exp $")

/*
 * Allocate a new, pixelmap of the given type and size
 *
 * If the 'pixels' pointer is NULL, an appropriate area of memeory will be allocated
 * 
 */
br_pixelmap * BR_RESIDENT_ENTRY BrPixelmapAllocate(br_uint_8 type,br_int_32 w,br_int_32 h, void *pixels, int flags)
{
	return (br_pixelmap *)DevicePixelmapMemAllocate(type, (br_uint_16)w, (br_uint_16)h, pixels, flags);
}
