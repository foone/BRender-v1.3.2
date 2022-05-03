/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: drv_ip.h 1.2 1998/10/21 15:36:29 jon Exp $
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
extern br_device DriverDeviceATI;

/*
 * object.c
 */
char * BR_CMETHOD_DECL(br_object_ati, identifier)(br_object *self);
br_device *	BR_CMETHOD_DECL(br_object_ati, device)(br_object *self);

/*
 * device.c
 */
br_error DeviceATIInitialise(br_device *dev);

/*
 * outfcty.c
 */
br_error OutputFacilityATIInitialise(br_device *dev);

/*
 * devpixmp.c
 */
br_device_pixelmap * DevicePixelmapATIAllocate(br_device *dev, br_output_facility *type, br_token_value *tv);
static br_token BufferMethodInit(br_device *dev, br_token method_hint);

br_token BR_CMETHOD_DECL(br_device_pixelmap_ati, type)(br_device_pixelmap *self);
br_boolean BR_CMETHOD_DECL(br_device_pixelmap_ati, isType)(br_device_pixelmap *self, br_token t);
br_int_32 BR_CMETHOD_DECL(br_device_pixelmap_ati, space)(br_device_pixelmap *self);
struct br_tv_template * BR_CMETHOD_DECL(br_device_pixelmap_ati, queryTemplate)(br_device_pixelmap *self);
br_error BR_CMETHOD_DECL(br_device_pixelmap_ati_dd, directLock)(br_device_pixelmap *self, br_boolean block);
br_error BR_CMETHOD_DECL(br_device_pixelmap_ati_dd, directUnlock)(br_device_pixelmap *self);

/*
 * devpmdd.c
 */
br_error ATIAllocateBack(br_device_pixelmap *self, br_device_pixelmap **newpm, int w, int h);
br_error ATIAllocateGenericOffscreen(br_device_pixelmap *self, br_device_pixelmap **newpm, int w, int h);
br_error ATIAllocateVideoMemory(br_device_pixelmap *self, br_device_pixelmap **newpm,
							   int w, int h);
static br_error LockSurface(br_device_pixelmap *self);
static br_error UnlockSurface(br_device_pixelmap *self);

/*
 * devclut.c
 */
br_device_clut * DeviceClutATIAllocate(br_device *dev, br_device_pixelmap *dpm, char *identifier);


/*
 * plib.c
 */
extern br_token PrimPartsTokensATI[];
br_error PrimitiveLibraryHardATIInitialise(struct br_primitive_library * self, br_device *dev);

/*
 * pstate.c
 */
struct br_primitive_state * PrimitiveStateHardATIAllocate(struct br_primitive_library *plib);

/*
 * sbuffer.c
 */
void ATISetupRenderBuffer(struct render_buffer *rb, br_device_pixelmap *pm, br_boolean texture);

struct br_buffer_stored * BufferStoredHardATIAllocate(struct br_primitive_library *plib,
	br_token use, struct br_device_pixelmap *pm, br_token_value *tv);

br_error SetDefaultPaletteEntry(br_uint_32 index,br_colour entry);

br_error SetDefaultPaletteEntries(br_uint_32 start,br_uint_32 num_entries,br_colour *entries);

br_error CreateDefaultPalette();

br_error DestroyDefaultPalette();

/*
 * match.c
 */
br_error BR_CMETHOD_DECL(br_primitive_state_hardATI, renderBegin)(
		struct br_primitive_state *self,
		struct brp_block **rpb,
		br_boolean *pchanged,
      br_boolean *ranges_changed,
		br_boolean no_render,
		br_token prim_type);

br_error BR_CMETHOD_DECL(br_primitive_state_hardATI, renderEnd)(
		struct br_primitive_state *self,
		struct brp_block *rpb);

br_error BR_CMETHOD_DECL(br_primitive_state_hardATI, rangesQueryF)(
		struct br_primitive_state *self,
		br_float *offset,
		br_float *scale,
		br_int_32 max_comp);

br_error BR_CMETHOD_DECL(br_primitive_state_hardATI, rangesQueryX)(
		struct br_primitive_state *self,
		br_fixed_ls *offset,
		br_fixed_ls *scale,
		br_int_32 max_comp);



/*
 * ocfree.c
 */
br_error ObjectContainerFree(struct br_object_container *self, br_token type, char *pattern, br_token_value *tv);

#ifdef __cplusplus
};
#endif

#endif
#endif


