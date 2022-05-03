/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: drv_ip.h 1.1 1997/12/10 16:53:59 jon Exp $
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
 * object.c
 */
char * BR_CMETHOD_DECL(br_object_vesa, identifier)(struct br_object *self);
br_device *	BR_CMETHOD_DECL(br_object_vesa, device)(struct br_object *self);

/*
 * device.c
 */
br_error DeviceVESAInitialise(struct br_device * dev, char * arguments);
br_error DeviceVESABankSwitchInitialise(struct br_device * self);

br_boolean DeviceVESALinearAvailable(struct br_device * self, struct vesa_modeinfo *mi);

br_error DeviceVESALinearInitialise(struct br_device * self, struct vesa_modeinfo *mi);
br_error DeviceVESABankedInitialise(struct br_device * self, struct vesa_modeinfo *mi);

/*
 * outtype.c
 */
br_error OutputFacilityVESAInitialise(int *count, br_device *dev);

/*
 * devpixmp.c
 */
br_device_pixelmap * DevicePixelmapVESAAllocateMode(struct br_device *dev, struct br_output_facility *type, br_uint_16 w, br_uint_16 h);
void BR_CMETHOD_DECL(br_device_pixelmap_vesa, free)(struct br_device_pixelmap *self);
br_token BR_CMETHOD_DECL(br_device_pixelmap_vesa, type)(struct br_device_pixelmap *self);
br_boolean BR_CMETHOD_DECL(br_device_pixelmap_vesa, isType)(br_device_pixelmap *self, br_token t);
br_int_32 BR_CMETHOD_DECL(br_device_pixelmap_vesa, space)(struct br_device_pixelmap *self);
struct br_tv_template * BR_CMETHOD_DECL(br_device_pixelmap_vesa,templateQuery)(struct br_device_pixelmap *self);

br_error BR_CMETHOD_DECL(br_device_pixelmap_fix, flush)(
	struct br_device_pixelmap *self);

br_error BR_CMETHOD_DECL(br_device_pixelmap_fix, synchronise)(
	struct br_device_pixelmap *self,
	br_token sync_type,
	br_boolean block);

br_error BR_CMETHOD_DECL(br_device_pixelmap_fix, directLock)(
	struct br_device_pixelmap *self,
	br_boolean block);
br_error BR_CMETHOD_DECL(br_device_pixelmap_fix, directUnlock)(
	struct br_device_pixelmap *self);

/*
 * devpmw.c
 */
extern struct br_device_pixelmap_dispatch devicePixelmapDispatch_w;

/*
 * devpmwb.c
 */
extern struct br_device_pixelmap_dispatch devicePixelmapDispatch_wb;

/*
 * devpml.c
 */
extern struct br_device_pixelmap_dispatch devicePixelmapDispatch_l;

/*
 * devclut.c
 */
br_device_clut * DeviceClutVESAAllocate(struct br_device *dev, char *identifier);

/*
 * vesabank.asm
 */
void BR_ASM_CALL VESABankFunctionInt10(void);

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

