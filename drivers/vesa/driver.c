/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: driver.c 1.1 1997/12/10 16:53:52 jon Exp $
 * $Locker: $
 *
 * Driver interface functions
 */
#include <stddef.h>
#include <string.h>

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: driver.c 1.1 1997/12/10 16:53:52 jon Exp $");

/*
 * Static device structure
 */
br_device DriverDeviceVESA = {
	NULL, "VESA",
};

/*
 * Main entry point for device - this may get redefined by the makefile
 */
br_device * BR_EXPORT BrDrv1Begin(char *arguments)
{
	int i, type_count;
	br_device *device = (br_device *)&DriverDeviceVESA;

	/*
	 * Set up device
	 */
	type_count  = 0;

	if(DeviceVESAInitialise((br_device *)device, arguments) == BRE_OK) {

		/*
		 * Setup all the built in types
		 */
		if(OutputFacilityVESAInitialise( &type_count, device) != BRE_OK)
			return NULL;

		/*
		 * If nothing is available, then don't admit to being a device
		 */
		if(type_count == 0)
			return NULL;
		else
			return device;
	} else
		return NULL;
}

