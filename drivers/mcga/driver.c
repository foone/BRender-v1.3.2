/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: driver.c 1.1 1997/12/10 16:45:39 jon Exp $
 * $Locker: $
 *
 * Driver interface functions
 */
#include <stddef.h>
#include <string.h>

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: driver.c 1.1 1997/12/10 16:45:39 jon Exp $");

/*
 * Main entry point for device - this may get redefined by the makefile
 */
br_device * BR_EXPORT BrDrv1Begin(char *arguments)
{
	int i, type_count;
	br_device *device;

	/*
	 * Set up device
	 */
    device = DeviceVGAAllocate("MCGA");

	if(device == NULL)
        return NULL;

	/*
	 * Setup the output facility
	 */
	if(OutputFacilityVGAAllocate(device, "MCGA", 0x13, 320,200, 8, BR_PMT_INDEX_8, BR_TRUE) == NULL) {
		/*
		 * If nothing is available, then don't admit to being a device
		 */
        ObjectFree(device);
        return NULL;
    }

	return device;
}

