/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: devpixmp.c 1.2 1998/10/21 15:41:12 jon Exp $
 * $Locker: $
 *
 * Device pixelmap methods
 */
#include <stddef.h>
#include <string.h>

#include "drv.h"
#include "pm.h"
#include "host.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: devpixmp.c 1.2 1998/10/21 15:41:12 jon Exp $");

/*
 * Display mode and stride are fixed for MCGA
 */
#define BIOS_MODE	0x13
#define BIOS_STRIDE	320

/*
 * Default dispatch table for device pixelmap (defined at end of file)
 */
static const struct br_device_pixelmap_dispatch devicePixelmapDispatch;

/*
 * Device pixelmap info. template
 */
#define F(f)	offsetof(struct br_device_pixelmap, f)

static const struct br_tv_template_entry devicePixelmapTemplateEntries[] = {
	{BRT_WIDTH_I32,			0,	F(pm_width),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_I32_U16, },
	{BRT_HEIGHT_I32,		0,	F(pm_height),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_I32_U16, },
	{BRT_PIXEL_TYPE_U8,		0,	F(pm_type),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_I32_U8, },
//	{BRT_PIXEL_CHANNELS_TL,	0,	0,					BRTV_QUERY | BRTV_ALL,	BRTV_CONV_CUSTOM, },
//	{BRT_INDEXED_B,			0,	F(indexed),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT_OUTPUT_FACILITY_O,	0,	F(output_facility),	BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT_FACILITY_O,		0,	F(output_facility),	BRTV_QUERY,				BRTV_CONV_COPY, },
	{BRT_IDENTIFIER_CSTR,	0,	F(pm_identifier),	BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT_CLUT_O,			0,	F(clut),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
};
#undef F

/*
 * Create a new device pixelmap and set a display mode
 */
br_device_pixelmap * DevicePixelmapMCGAAllocateMode(br_device *dev, br_output_facility *facility, br_uint_16 w, br_uint_16 h)
{
	br_device_pixelmap *self;
	br_error r;
	br_uint_16 original_mode;
	br_uint_16 sel;

	if(dev->active)
		return NULL;

	/*
	 * _DEV_ Setup mode
	 */
	original_mode = BIOSVideoGetMode();
	if(r = BIOSVideoSetMode(BIOS_MODE))
		return NULL;

	DeviceVGACurrentModeSet(dev, BIOS_MODE);
	dev->active = BR_TRUE;

	self = BrResAllocate(DeviceVGAResource(dev),
		sizeof(*self), BR_MEMORY_OBJECT_DATA);

	self->dispatch = & devicePixelmapDispatch;
 	self->pm_identifier = ObjectIdentifier(facility);
    self->device = dev;
	self->restore_mode = BR_TRUE;
	self->original_mode = original_mode;

	self->pm_pixels = (void *) 0xA0000;

	HostSelectorReal( &sel);
	self->pm_pixels_qualifier = sel;

	self->pm_type = OutputFacilityVGAType(facility); 
	self->pm_width = w;
	self->pm_height = h;
	self->pm_row_bytes = BIOS_STRIDE; 
	self->pm_origin_x = 0;
	self->pm_origin_y = 0;

	self->pm_flags = BR_PMF_ROW_WHOLEPIXELS | BR_PMF_LINEAR;
	self->pm_base_x = 0;
	self->pm_base_y = 0;

	self->output_facility = facility;
	self->clut = DeviceVGAClut(dev);

	ObjectContainerAddFront(facility, (br_object *)self);

	return self;
}

static void BR_CMETHOD_DECL(br_device_pixelmap_mcga, free)(br_device_pixelmap *self)
{

	ObjectContainerRemove(self->output_facility, (br_object *)self);
	self->output_facility->num_instances--;

	/*
	 * Restore video mode
	 */
	if(self->restore_mode) {
		BIOSVideoSetMode(self->original_mode);
		DeviceVGACurrentModeSet(ObjectDevice(self), self->original_mode);
		ObjectDevice(self)->active = BR_FALSE;
	}
	
	/*
	 * Free up pixelmap structure
	 */
	BrResFreeNoCallback(self);
}

static br_device *BR_CMETHOD_DECL(br_device_pixelmap_mcga, device)(br_device_pixelmap *self)
{
	return self->device;
}

static br_token BR_CMETHOD_DECL(br_device_pixelmap_mcga, type)(br_device_pixelmap *self)
{
	return BRT_DEVICE_PIXELMAP;
}

static br_boolean BR_CMETHOD_DECL(br_device_pixelmap_mcga, isType)(br_device_pixelmap *self, br_token t)
{
	return (t == BRT_DEVICE_PIXELMAP) || (t == BRT_OBJECT);
}

static br_int_32 BR_CMETHOD_DECL(br_device_pixelmap_mcga, space)(br_device_pixelmap *self)
{
	return sizeof(br_device_pixelmap);
}

static struct br_tv_template * BR_CMETHOD_DECL(br_device_pixelmap_vga,queryTemplate)
	(br_device_pixelmap *self)
{
    if(self->device->templates.devicePixelmapTemplate== NULL)
        self->device->templates.devicePixelmapTemplate= BrTVTemplateAllocate(self->device,
            devicePixelmapTemplateEntries,
            BR_ASIZE(devicePixelmapTemplateEntries));

    return self->device->templates.devicePixelmapTemplate;
}


/*
 * Default dispatch table for device pixelmap
 */
static const struct br_device_pixelmap_dispatch devicePixelmapDispatch = {
	NULL,
	NULL,
	NULL,
	NULL,
	BR_CMETHOD_REF(br_device_pixelmap_mcga, free),
	BR_CMETHOD_REF(br_object_vga,			identifier),
	BR_CMETHOD_REF(br_device_pixelmap_mcga,	type),
	BR_CMETHOD_REF(br_device_pixelmap_mcga,	isType),
	BR_CMETHOD_REF(br_device_pixelmap_mcga,	device),
	BR_CMETHOD_REF(br_device_pixelmap_mcga,	space),

	BR_CMETHOD_REF(br_device_pixelmap_vga,	queryTemplate),
	BR_CMETHOD_REF(br_object,				query),
	BR_CMETHOD_REF(br_object,				queryBuffer),
	BR_CMETHOD_REF(br_object,				queryMany),
	BR_CMETHOD_REF(br_object,				queryManySize),
	BR_CMETHOD_REF(br_object,				queryAll),
	BR_CMETHOD_REF(br_object,				queryAllSize),

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

	BR_CMETHOD_REF(br_device_pixelmap_gen, flush),
	BR_CMETHOD_REF(br_device_pixelmap_gen, synchronise),
	BR_CMETHOD_REF(br_device_pixelmap_gen, directLock),
	BR_CMETHOD_REF(br_device_pixelmap_gen, directUnlock),

	BR_CMETHOD_REF(br_device_pixelmap_gen,	getControls),
	BR_CMETHOD_REF(br_device_pixelmap_gen,	setControls),
};

