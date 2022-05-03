/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: devpixmp.c 1.1 1997/12/10 16:53:37 jon Exp $
 * $Locker: $
 *
 * Device pixelmap methods
 */
#include <stddef.h>

#include "drv.h"
#include "pm.h"
#include "host.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: devpixmp.c 1.1 1997/12/10 16:53:37 jon Exp $");

/*
 * Device pixelmap info. template
 */
#define F(f)	offsetof(struct br_device_pixelmap, f)

static struct br_tv_template_entry devicePixelmapTemplateEntries[] = {
	{BRT(WIDTH_I32),		F(pm_width),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_I32_U16, },
	{BRT(HEIGHT_I32),		F(pm_height),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_I32_U16, },
	{BRT(PIXEL_TYPE_U8),	F(pm_type),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_I32_U8, },
//	{BRT(PIXEL_CHANNELS_TL),0,					BRTV_QUERY | BRTV_ALL,	BRTV_CONV_CUSTOM, },
//	{BRT(INDEXED_B,)		F(indexed),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(OUTPUT_FACILITY_O),F(output_facility),	BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(FACILITY_O),		F(output_facility),	BRTV_QUERY,				BRTV_CONV_COPY, },
	{BRT(IDENTIFIER_CSTR),	F(pm_identifier),	BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(CLUT_O),			F(clut),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
};
#undef F

static struct br_tv_template devicePixelmapTemplate = {
	BR_ASIZE(devicePixelmapTemplateEntries),
	devicePixelmapTemplateEntries
};

static int pixelBytes(br_device_pixelmap *pm)
{
	switch(pm->pm_type) {
	case BR_PMT_RGB_555:
	case BR_PMT_RGB_565:
	case BR_PMT_DEPTH_16:
		return 2;

	case BR_PMT_RGB_888:
		return 3;

	case BR_PMT_RGBX_888:
	case BR_PMT_RGBA_8888:
		return 4;

	default:
		return 1;
	}
}

/*
 * Create a new device pixelmap and set a display mode
 */
br_device_pixelmap * DevicePixelmapVESAAllocateMode(br_device *dev, br_output_facility *facility, br_uint_16 w, br_uint_16 h)
{
	br_device_pixelmap *self;
	br_error r;
	br_uint_16 original_mode,sel_ds;
	struct vesa_work *vw;
	char tmp[80];


	if(dev->screen_active)
		return NULL;

	vw = DeviceVESAWork(dev);

	/*
	 * _DEV_ Setup mode
	 */
	VESAModeGet(&original_mode);

	/*
	 * See if linear access will work
	 */

	/*
	 * Put the device into the desired mode
	 */
	if(DeviceVESALinearAvailable(dev, OutputFacilityVESAModeInfo(facility))) {
		vw->access_linear = BR_TRUE;

		if(VESAModeSet(OutputFacilityVESAMode(facility) + VESA_MODE_LINEAR) != BRE_OK)
			return NULL;

		DeviceVESACurrentModeSet(dev, OutputFacilityVESAMode(facility) + VESA_MODE_LINEAR);

		if(DeviceVESALinearInitialise(dev, OutputFacilityVESAModeInfo(facility)) != BRE_OK)
			return NULL;

	} else {
		vw->access_linear = BR_FALSE;

		if(VESAModeSet(OutputFacilityVESAMode(facility)) != BRE_OK)
			return NULL;

		DeviceVESACurrentModeSet(dev, OutputFacilityVESAMode(facility));

		if(DeviceVESABankedInitialise(dev, OutputFacilityVESAModeInfo(facility)) != BRE_OK)
			return NULL;
	}

	dev->screen_active = BR_TRUE;


	/*
	 * Build self
	 */
	self = BrResAllocate(DeviceVESAResource(dev),
		sizeof(*self), BR_MEMORY_OBJECT_DATA);


	if(vw->access_linear) {
		self->dispatch = & devicePixelmapDispatch_l;

		if(vw->linear) {
			self->pm_pixels = vw->linear;
			HostSelectorDS(&sel_ds);
			self->pm_pixels_qualifier = sel_ds;

			self->pm_flags |= BR_PMF_PIXELS_NEAR;
		} else {
			self->pm_pixels = 0;
			self->pm_pixels_qualifier = vw->physical_selector;
		}
	} else {
		self->pm_pixels = (void *)vw->window_start;
		self->pm_pixels_qualifier = vw->selector;
		self->pm_flags = BR_PMF_NO_ACCESS;

		if(vw->scanline_breaks == 0)
			self->dispatch = & devicePixelmapDispatch_w;
		else
			self->dispatch = & devicePixelmapDispatch_wb;
	}

	self->restore_mode = BR_TRUE;
	self->original_mode = original_mode;

	self->pm_type = OutputFacilityVESAType(facility);
	self->pm_width = w;
	self->pm_height = h;
	self->pm_origin_x = 0;
	self->pm_origin_y = 0;
	self->pm_row_bytes = vw->stride; 


	self->pm_base_x = 0;
	self->pm_base_y = 0;

	self->output_facility = facility;

	if(self->pm_width * pixelBytes(self) == self->pm_row_bytes)
		self->pm_flags |= BR_PMF_LINEAR;
	else
		self->pm_flags &= ~BR_PMF_LINEAR;
	
	if(self->pm_type == BR_PMT_INDEX_8)
		self->clut = DeviceVESAClut(dev);

	/*
	 * Build a descriptive identifier string
	 */
	BrSprintfN(tmp,sizeof(tmp)-1,"%dx%dx%d %s %s",self->pm_width,self->pm_height,
			OutputFacilityVESABits(facility),
			vw->access_linear?"Linear":(vw->scanline_breaks?"Banked-Breaks":"Banked"),
			vw->bank_protected?"Protected":"Int10");

	self->pm_identifier = BrResStrDup(self,tmp);

	self->output_facility->num_instances++;
	ObjectContainerAddFront(facility, (br_object *)self);

	return self;
}

void BR_CMETHOD_DECL(br_device_pixelmap_vesa, free)(br_device_pixelmap *self)
{
	ObjectContainerRemove(self->output_facility, (br_object *)self);
	self->output_facility->num_instances--;

	/*
	 * Restore video mode
	 */
	if(self->restore_mode) {
		//VESAModeSet(self->original_mode);
		BIOSVideoSetMode(self->original_mode);
		DeviceVESACurrentModeSet(ObjectDevice(self), self->original_mode);
		ObjectDevice(self)->screen_active = BR_FALSE;
	}
	
	BrResFreeNoCallback(self);
}

br_token BR_CMETHOD_DECL(br_device_pixelmap_vesa, type)(br_device_pixelmap *self)
{
	return BRT_DEVICE_PIXELMAP;
}

br_boolean BR_CMETHOD_DECL(br_device_pixelmap_vesa, isType)(br_device_pixelmap *self, br_token t)
{
	return (t == BRT_DEVICE_PIXELMAP) || (t == BRT_OBJECT);
}

br_int_32 BR_CMETHOD_DECL(br_device_pixelmap_vesa, space)(br_device_pixelmap *self)
{
	return sizeof(br_device_pixelmap);
}

struct br_tv_template * BR_CMETHOD_DECL(br_device_pixelmap_vesa, templateQuery)
	(br_device_pixelmap *self)
{
	devicePixelmapTemplate.res = DeviceVESAResource(ObjectVESADevice(self));
	return &devicePixelmapTemplate;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_fix, flush)(
	struct br_device_pixelmap *self)
{
	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_fix, synchronise)(
	struct br_device_pixelmap *self,
	br_token sync_type,
	br_boolean block)
{
	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_fix, directLock)(
	struct br_device_pixelmap *self,
	br_boolean block)
{
	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_fix, directUnlock)(
	struct br_device_pixelmap *self)
{
	return BRE_OK;
}

