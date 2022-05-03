/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: driver.c 1.1 1997/12/10 16:45:15 jon Exp $
 * $Locker: $
 *
 * Driver interface functions
 */
#include <stddef.h>
#include <string.h>

/*
 * Define GUIDs for driver-wide use
 */
#ifdef __DRIVER__
#define INITGUID
#endif

#include <windows.h>
#include <ddraw.h>

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: driver.c 1.1 1997/12/10 16:45:15 jon Exp $");

/*
 * Main entry point for device - this may get redefined by the makefile
 */
br_device * BR_EXPORT BrDrv1Begin(char *arguments)
{
	br_device *dev;

	/*
	 * Set up device
	 */
	dev = DeviceDirectDrawAllocate("DDRAW", arguments);

	if ( !dev )
      return NULL;

	/*
	 * Setup all the available types
	 */
	if(OutputFacilityDirectDrawInitialise(dev) != BRE_OK)
		return NULL;

	/*
	 * If nothing is available, then don't admit to being a device
	 */
	if(dev->output_facility_count == 0)
		return NULL;

	return dev;
}

