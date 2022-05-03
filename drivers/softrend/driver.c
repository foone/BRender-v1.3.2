/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: driver.c 1.1 1997/12/10 16:51:45 jon Exp $
 * $Locker: $
 *
 * Driver interface functions
 */
#include <stddef.h>
#include <string.h>

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: driver.c 1.1 1997/12/10 16:51:45 jon Exp $");

#if BASED_FIXED
#define SHORT_SUFFIX "RNDX"
#define LONG_SUFFIX "-Fixed"
#endif
#if BASED_FLOAT
#define SHORT_SUFFIX "RNDF"
#define LONG_SUFFIX "-Float"
#endif

/*
 * Driver-wide timestamp - the only static variable
 */
br_uint_32 DriverTimestamp;

/*
 * Main entry point for device - this may get redefined by the makefile
 */
br_device * BR_EXPORT BrDrv1Begin(char *arguments)
{
	br_device *device;

	/*
	 * Setup static timestamp
	 */
    if(DriverTimestamp == 0)
    	DriverTimestamp = TIMESTAMP_START;

	/*
	 * Set up device
	 */
    device = DeviceSoftAllocate("SOFT" SHORT_SUFFIX);

    if(device == NULL)
        return NULL;

	if(RendererFacilitySoftAllocate(device, "Default-Renderer" LONG_SUFFIX) == NULL) {
        ObjectFree(device);
        return NULL;
    }

    return device;
}

