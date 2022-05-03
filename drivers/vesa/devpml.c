/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: devpml.c 1.2 1998/10/21 15:41:22 jon Exp $
 * $Locker: $
 *
 * Device pixelmap methods, windowed, no breaks
 */
#include <stddef.h>
#include <string.h>

#include "drv.h"
#include "pm.h"
#include "host.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: devpml.c 1.2 1998/10/21 15:41:22 jon Exp $");

/*
 * Default dispatch table for device pixelmap - linear
 */
struct br_device_pixelmap_dispatch devicePixelmapDispatch_l = {
	NULL,
	NULL,
	NULL,
	NULL,
	BR_CMETHOD_REF(br_device_pixelmap_vesa, free),
	BR_CMETHOD_REF(br_object_vesa, identifier),
	BR_CMETHOD_REF(br_device_pixelmap_vesa, type),
	BR_CMETHOD_REF(br_device_pixelmap_vesa, isType),
	BR_CMETHOD_REF(br_object_vesa, device),
	BR_CMETHOD_REF(br_device_pixelmap_vesa, space),

	BR_CMETHOD_REF(br_device_pixelmap_vesa, templateQuery),
	BR_CMETHOD_REF(br_object, query),
	BR_CMETHOD_REF(br_object, queryBuffer),
	BR_CMETHOD_REF(br_object, queryMany),
	BR_CMETHOD_REF(br_object, queryManySize),
	BR_CMETHOD_REF(br_object, queryAll),
	BR_CMETHOD_REF(br_object, queryAllSize),

	BR_CMETHOD_REF(br_device_pixelmap_mem, validSource),
	BR_CMETHOD_REF(br_device_pixelmap_mem, resize),
	BR_CMETHOD_REF(br_device_pixelmap_mem, match),
	BR_CMETHOD_REF(br_device_pixelmap_mem, allocateSub),

	BR_CMETHOD_REF(br_device_pixelmap_mem, copyTo),
	BR_CMETHOD_REF(br_device_pixelmap_mem, copyTo),
	BR_CMETHOD_REF(br_device_pixelmap_mem, copyFrom),
	BR_CMETHOD_REF(br_device_pixelmap_mem, fill),
	BR_CMETHOD_REF(br_device_pixelmap_gen, doubleBuffer),

	BR_CMETHOD_REF(br_device_pixelmap_gen, copyDirty),
	BR_CMETHOD_REF(br_device_pixelmap_gen, copyToDirty),
	BR_CMETHOD_REF(br_device_pixelmap_gen, copyFromDirty),
	BR_CMETHOD_REF(br_device_pixelmap_gen, fillDirty),
	BR_CMETHOD_REF(br_device_pixelmap_gen, doubleBufferDirty),

	BR_CMETHOD_REF(br_device_pixelmap_gen, rectangle),
	BR_CMETHOD_REF(br_device_pixelmap_gen, rectangle2),
	BR_CMETHOD_REF(br_device_pixelmap_mem, rectangleCopyTo),
	BR_CMETHOD_REF(br_device_pixelmap_mem, rectangleCopyTo),
	BR_CMETHOD_REF(br_device_pixelmap_mem, rectangleCopyFrom),
	BR_CMETHOD_REF(br_device_pixelmap_mem,	rectangleStretchCopyTo),
	BR_CMETHOD_REF(br_device_pixelmap_mem,	rectangleStretchCopyTo),
	BR_CMETHOD_REF(br_device_pixelmap_mem,	rectangleStretchCopyFrom),
	BR_CMETHOD_REF(br_device_pixelmap_mem, rectangleFill),
	BR_CMETHOD_REF(br_device_pixelmap_mem, pixelSet),
	BR_CMETHOD_REF(br_device_pixelmap_mem, line),
	BR_CMETHOD_REF(br_device_pixelmap_mem, copyBits),

	BR_CMETHOD_REF(br_device_pixelmap_gen, text),
	BR_CMETHOD_REF(br_device_pixelmap_gen, textBounds),

	BR_CMETHOD_REF(br_device_pixelmap_mem, rowSize),
	BR_CMETHOD_REF(br_device_pixelmap_mem, rowQuery),
	BR_CMETHOD_REF(br_device_pixelmap_mem, rowSet),

	BR_CMETHOD_REF(br_device_pixelmap_mem, pixelQuery),
	BR_CMETHOD_REF(br_device_pixelmap_mem, pixelAddressQuery),

	BR_CMETHOD_REF(br_device_pixelmap_mem, pixelAddressSet),
	BR_CMETHOD_REF(br_device_pixelmap_mem, originSet),

	BR_CMETHOD_REF(br_device_pixelmap_fix, flush),
	BR_CMETHOD_REF(br_device_pixelmap_fix, synchronise),
	BR_CMETHOD_REF(br_device_pixelmap_fix, directLock),
	BR_CMETHOD_REF(br_device_pixelmap_fix, directUnlock),

	BR_CMETHOD_REF(br_device_pixelmap_gen,	getControls),
	BR_CMETHOD_REF(br_device_pixelmap_gen,	setControls),
};
