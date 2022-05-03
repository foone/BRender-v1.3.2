/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: drv_ip.h 1.1 1997/12/10 16:45:41 jon Exp $
 * $Locker: $
 *
 * Prototypes for functions internal to driver
 */
#ifndef _DRV_IP_H_
#define _DRV_IP_H_

#ifndef NO_PROTOTYPES

#ifdef __cplusplus
extern "C" {
#endif

/*
 * driver.c
 */
extern br_device DriverDeviceVGA;

/*
 * object.c
 */
char * BR_CMETHOD_DECL(br_object_vga, identifier)( br_object *self);
br_device *	BR_CMETHOD_DECL(br_object_vga, device)( br_object *self);

/*
 * device.c
 */
br_device *DeviceVGAAllocate(char *identifier);

/*
 * outclass.c
 */
br_output_facility * OutputFacilityVGAAllocate(br_device *dev, char *identifier,
    br_int_32 mode, br_int_32 width, br_int_32 height, br_int_32 bits, br_int_32 type, br_boolean indexed);

/*
 * devpixmp.c
 */
br_device_pixelmap * DevicePixelmapMCGAAllocateMode(br_device *dev, br_output_facility *type, br_uint_16 w, br_uint_16 h);

/*
 * devclut.c
 */
br_device_clut * DeviceClutVGAAllocate(br_device *dev, char *identifier);

/*
 * bios.c
 */
br_error BIOSVideoSetMode(br_uint_16 mode);
br_uint_16 BIOSVideoGetMode(void);

/*
 * ocfree.c
 */
br_error ObjectContainerFree(struct br_object_container *self, br_token type, char *pattern, br_token_value *tv);

#ifdef __cplusplus
};
#endif

#endif
#endif

