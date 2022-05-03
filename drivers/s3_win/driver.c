/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: driver.c 1.1 1997/12/10 16:50:57 jon Exp $
 * $Locker: $
 *
 * Driver interface functions
 */
#include <stddef.h>
#include <string.h>
#include <windows.h>
#include <ddraw.h>


#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: driver.c 1.1 1997/12/10 16:50:57 jon Exp $");

/*
 * Static device structure
 */
br_device DriverDeviceS3 =
{
	NULL, "S3_WIN",
};


/*
 * Static output types
 */
static br_output_facility outputFacilities[] =
{
	STATIC_OUTPUT_FACILITY_S3("Windows-S3", 320, 200,	8, 16, BR_PMT_INDEX_8, BR_PMT_DEPTH_16, BR_FALSE, 0, 0),
	STATIC_OUTPUT_FACILITY_S3("Windows-S3", 320, 200,	15, 16, BR_PMT_RGB_555, BR_PMT_DEPTH_16, BR_FALSE, 0, 0),
	STATIC_OUTPUT_FACILITY_S3("Windows-S3", 320, 200,	16, 16, BR_PMT_RGB_555, BR_PMT_DEPTH_16, BR_FALSE, 0, 0),
	STATIC_OUTPUT_FACILITY_S3("Windows-S3", 320, 200,	24, 16, BR_PMT_RGB_888, BR_PMT_DEPTH_16, BR_FALSE, 0, 0),
};

/*
 * Main entry point for device - this may get redefined by the makefile
 */
br_device * BR_EXPORT BrDrv1Begin(char *arguments)
{
	br_device *device = (br_device *)&DriverDeviceS3;

	/* Set up device */
	device->object_list = NULL;
	device->res = NULL;
	device->output_facility_count = 0;

	if(DeviceS3Initialise(device) != BRE_OK)
		return (NULL);

	/* Setup DirectDraw output facilities */
	if (OutputFacilityS3Initialise(device) != BRE_OK)
		return(NULL);

	/* If nothing is available, then don't admit to being a device */
	if (device->output_facility_count == 0)
		return NULL;
    else
		return device;
}


