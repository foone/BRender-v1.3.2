/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: device.c 1.1 1997/12/10 16:53:30 jon Exp $
 * $Locker: $
 *
 * Device methods
 */

#include <stddef.h>
#include <string.h>

#include "drv.h"
#include "host.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: device.c 1.1 1997/12/10 16:53:30 jon Exp $");

/*
 * Default dispatch table for device (defined at end of file)
 */
static struct br_device_dispatch deviceDispatch;

/*
 * Device info. template, two versions - one for VESA 1.x and one for VESA 2.x or greater
 */
#define F(f)	offsetof(struct br_device, f)

static struct br_tv_template_entry device1TemplateEntries[] = {
	{BRT(IDENTIFIER_CSTR),		F(identifier),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{DEV(VERSION_U16),			F(info.vbe_version),	BRTV_QUERY | BRTV_ALL,	BRTV_CONV_I32_U16, },
	{DEV(OEM_STRING_CSTR),		F(info.oem_string_ptr),	BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(CLUT_O),				F(clut),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
};

static struct br_tv_template_entry device2TemplateEntries[] = {
	{BRT(IDENTIFIER_CSTR),				F(identifier),					BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{DEV(VERSION_U16),					F(info.vbe_version), 			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_I32_U16, },
	{DEV(OEM_STRING_CSTR),				F(info.oem_string_ptr),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{DEV(OEM_PRODUCT_NAME_CSTR),		F(info.oem_product_name_ptr),	BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{DEV(OEM_PRODUCT_REV_CSTR),			F(info.oem_product_rev_ptr),	BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{DEV(OEM_VENDOR_NAME_CSTR),			F(info.oem_vendor_name_ptr),	BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(CLUT_O),						F(clut),						BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
};

#undef F

static struct br_tv_template device1Template = {
	BR_ASIZE(device1TemplateEntries),
	device1TemplateEntries
};

static struct br_tv_template device2Template = {
	BR_ASIZE(device2TemplateEntries),
	device2TemplateEntries
};

/*
 * Structure used to unpack driver arguments
 */
#define F(f)	offsetof(struct br_device, f)

static struct br_tv_template_entry deviceArgsTemplateEntries[] = {
	{DEV(VERSION_U16),		F(version_limit),				BRTV_SET,	BRTV_CONV_I32_U16, },
	{DEV(USE_PMI_B),		F(use_pmi),						BRTV_SET,	BRTV_CONV_I32_B, },
	{DEV(USE_LINEAR_B),		F(use_linear),					BRTV_SET,	BRTV_CONV_I32_B, },
	{DEV(SET_STRIDE_B),		F(set_stride),					BRTV_SET,	BRTV_CONV_I32_B, },
};
#undef F

static struct br_tv_template deviceArgsTemplate = {
	BR_ASIZE(deviceArgsTemplateEntries),
	deviceArgsTemplateEntries
};

/*
 * Set up a static device object
 */
br_error DeviceVESAInitialise(br_device * self, char * arguments)
{
	br_int_32 count;
	br_error r;
	br_token_value args_tv[256],*tp;

	self->dispatch = &deviceDispatch;

	/*
	 * Find out if VESA driver is around and
	 * fetch the VBE information buffer
	 */
	r = VESAInfo(&self->info);
	if(r != BRE_OK)
		return r;

	/*
	 * Create Anchor resource
	 */
	self->res = BrResAllocate(NULL, 0, BR_MEMORY_DRIVER);

	/*
	 * Create all the driver specific tokens
	 */
	BrTokenCreate("VESA_MODE_U16",					BRT_UINT_16);
	BrTokenCreate("VERSION_U16",					BRT_UINT_16);
	BrTokenCreate("OEM_STRING_CSTR",				BRT_CONSTANT_STRING);
	BrTokenCreate("OEM_VENDOR_NAME_CSTR",			BRT_CONSTANT_STRING);
	BrTokenCreate("OEM_PRODUCT_NAME_CSTR",			BRT_CONSTANT_STRING);
	BrTokenCreate("OEM_PRODUCT_REV_CSTR",			BRT_CONSTANT_STRING);
	BrTokenCreate("USE_PMI_B",						BRT_BOOLEAN);
	BrTokenCreate("USE_LINEAR_B",					BRT_BOOLEAN);
	BrTokenCreate("SET_STRIDE_B",					BRT_BOOLEAN);

	/*
	 * Parse arguments
	 */
	self->version_limit = 0xffff;
	self->use_pmi = BR_TRUE;
	self->use_linear = BR_TRUE;
	self->set_stride = BR_TRUE;

	if(arguments) {
		BrStringToTokenValue(args_tv, sizeof(args_tv), arguments);
		deviceArgsTemplate.res = self->res;
		BrTokenValueSetMany(self, &count, NULL, args_tv, &deviceArgsTemplate);
	}

	/*
	 * Clamp version number
	 */
	if(self->version_limit < self->info.vbe_version)
		self->info.vbe_version = self->version_limit;

	/*
	 * Set up bank switching interface (int10 vs. protected mode)
	 */
	DeviceVESABankSwitchInitialise(self);

	/*
	 * _DEV_ Remember initial VESA mode
	 */
//	VESAModeGet(&self->original_mode);
	self->original_mode=BIOSVideoGetMode();
	
	self->current_mode = self->original_mode;

	/*
	 * Set up object list
	 */
	self->object_list = BrObjectListAllocate(self->res);

	/*
	 * Build CLUT object
	 */
	self->clut = DeviceClutVESAAllocate(self, "Hardware-CLUT");

	self->screen_active = BR_FALSE;
	return BRE_OK;
}

/*
 * Find out the best way of switching banks
 */
br_error DeviceVESABankSwitchInitialise(br_device * self)
{
	struct vesa_work *vw = &self->work;
	br_error r;
	br_uint_16 off,seg,size, *res;
	struct vesa_protected *buffer;
	br_uint_32 mem_addr,mem_size = 0;

	/*
	 * Assume INT10
	 */
	vw->bank_protected = BR_FALSE;
	vw->bank_function = VESABankFunctionInt10;

	/*
	 * If VESA driver is earlier than 2.0, or PMI is disabled
	 * then use Int10
	 */
	if(self->info.vbe_version < 0x200 || !self->use_pmi)
		return BRE_OK;

	/*
	 * Get address (in real mode) of protected mode code
	 */
	r = VESAProtectedModeInterface(&off,&seg,&size);

	/*
	 * If that fails, stick with Int10
	 */
	if(r != BRE_OK)
		return BRE_OK;

	/*
	 * Allocate a buffer
	 */
	buffer = BrResAllocate(DeviceVESAResource(self),
		size,BR_MEMORY_DRIVER);

	/*
	 * Copy PM table
	 */
	HostRealBlockRead(off,seg,buffer,size);

	/*
	 * Investigate resource requirements
	 */
	if(buffer->resources) {

		res = (br_uint_16 *)((br_uint_8 *)buffer + buffer->resources);

		while(*res != 0xffff) {
#if 0
			BrLogPrintf("Port:   %04x\n",*res);
#endif
			res++;
		}

		res++;

		if(*(res+3) != 0xffff)
			return BRE_OK;
			
		mem_addr = *((br_uint_32 *)res);
		mem_size = *(res+2);
#if 0
		BrLogPrintf("Memory: %08x + %04x\n", mem_addr, mem_size);
#endif
	}

	/*
	 * Don't use PMI code if it requires mapped memory (for the moment)
	 */
	if(mem_size != 0)
		return BRE_OK;

	/*
	 * Use protected mode interface
	 */
	vw->bank_protected = BR_TRUE;
	vw->bank_function = (br_uint_8 *)buffer + buffer->window_set;

	/* XXX Add PaletteSet PMI */

	return BRE_OK;
}

/*
 * Set up some static information (including a per-scanline table) based
 * on the current display line length
 */
static void strideInitialise(struct vesa_work *vw, struct vesa_modeinfo *mi)
{
	int i,a,s;
	br_uint_32 o;

	vw->full_banks = mi->y_resolution * vw->stride / vw->page_size;
	vw->bytes_left = mi->y_resolution * vw->stride - vw->page_size * vw->full_banks;
	vw->scanlines_per_page = vw->page_size / vw->stride;
	vw->scanlines_remainder = mi->y_resolution - vw->full_banks * vw->scanlines_per_page;

	/*
	 * Fill in per scanline table
	 */
	for(i=0, a=0, s=0; i < mi->y_resolution; i++, a+= vw->stride) {
		vw->scanline_table[i].offset = a % vw->page_size;
		vw->scanline_table[i].page = a / vw->page_size * vw->bank_increment;

		/*
		 * Find end of visible scanline
		 */
		o = vw->scanline_table[i].offset + mi->x_resolution * vw->pixel_size;

		if(o > vw->page_size) {
			vw->scanline_table[i].split = vw->page_size - vw->scanline_table[i].offset;
			s++;
		} else {
			vw->scanline_table[i].split = 0;
		}
	}

	vw->scanline_breaks = s;
}

/*
 * Calculate a bunch of per-mode constants for use by the low level
 * loops whilst in banked mode
 */
br_error DeviceVESABankedInitialise(br_device * self, struct vesa_modeinfo *mi)
{
	struct vesa_work *vw = &self->work;
	br_uint_16 new_bytes, new_pixels, new_scanlines;
	int i,l;

	ASSERT(!(vw->access_linear));

	/*
	 * Setup information that is independant of stride
	 */
	vw->page_size = mi->win_size * 1024;;
	HostSelectorReal(&vw->selector);
	vw->window_start = mi->win_a_segment << 4;
	vw->window_end = vw->window_start + vw->page_size;

	vw->pixel_size = (mi->bits_per_pixel+7) >> 3;

	vw->bank_increment = mi->win_size/mi->win_granularity;

	vw->current_page = 0;

	/*
	 * Allocate a per-scanline table of information
	 */
	if(mi->y_resolution > vw->scanline_max) {
		if(vw->scanline_table)
			BrResFree(vw->scanline_table);

			vw->scanline_max = mi->y_resolution;

			vw->scanline_table = BrResAllocate(DeviceVESAResource(self),
				vw->scanline_max * sizeof(struct vesa_scanline),
				BR_MEMORY_DRIVER);
	}

	/*
	 * Start out using the given stride
	 */
	vw->stride = mi->bytes_per_scanline;
	strideInitialise(vw, mi);

	if(vw->scanline_breaks == 0)
		return BRE_OK;

	/*
	 * If there are broken scanlines, see if it is possible to reset stride
	 * to a more amenable value
	 */

	if(self->set_stride == BR_FALSE)
		return BRE_OK;

	/*
	 * Try the next power of 2
	 */
	for(i = 5; i < 16; i++) {
		l = 1 << i;
		if(l >= vw->stride)
			break;
	}

	/*
	 * Don;t bother if display won't fit on card
	 */
	if(l * mi->y_resolution > self->info.memory * 64 * 1024)
		return BRE_OK;

	if(l >= vw->stride) {
		if(VESAScanlineBytesSet(l, &new_bytes, &new_pixels, & new_scanlines) != BRE_OK) {
			/*
			 * Set failed, reset just in case
			 */
			VESAScanlineBytesSet(vw->stride, &new_bytes, &new_pixels, & new_scanlines);
		} else {
			/*
			 * Rebuild according to new width
			 */
			vw->stride = new_bytes;
			strideInitialise(vw, mi);
		}
	}

	return BRE_OK;
}

/*
 * Calculate a bunch of per-mode constants for use by the low level
 * loops whilst in linear mode
 */
br_error DeviceVESALinearInitialise(br_device * self, struct vesa_modeinfo *mi)
{
	br_error r;
	br_uint_32 pbase,dsbase,dslimit,psize,poffset;
	br_uint_16 sel_ds;
	struct vesa_work *vw = &self->work;
	struct host_info hi;

	ASSERT(vw->access_linear);

	/*
	 * Remove any previous physical mapping, if it is in wrong place
	 */
	if(vw->physical_selector && vw->physical_address != mi->phys_base_ptr) {
		HostSelectorFree(vw->physical_selector);
		vw->physical_selector = 0;
	}

	/*
	 * Map buffer in if not already available
	 */
	if(vw->physical_selector == 0) {
		/*
		 * Map memory into a new selector
		 */
		psize = self->info.memory * 64 * 1024;
		r = HostSelectorAllocatePhysical(&vw->physical_selector,
			mi->phys_base_ptr, psize);
		
		if(r != BRE_OK)
			return r;

		vw->physical_address = mi->phys_base_ptr;

		HostInfo(&hi, sizeof(hi));

		/*
		 * If the host extender allows us to fiddle with DS,
		 * or the DS limit is already ok, then make the
		 * frame buffer pointer NEAR
		 */

		HostSelectorDS(&sel_ds);
		HostSelectorBaseQuery(&pbase, vw->physical_selector);
		HostSelectorLimitQuery(&dslimit, sel_ds);
		HostSelectorBaseQuery(&dsbase, sel_ds);

		poffset = pbase - dsbase;

		if((poffset + psize-1) <= dslimit) {
			vw->linear = (void *)(poffset);

		} else if(hi.capabilities & HOST_CAPS_BASE_SELECTORS_WRITE) {

			HostSelectorLimitSet(sel_ds, 0xffffffff);
			vw->linear = (void *)(poffset);
		} else {
			vw->linear = NULL;
		}
	}

	vw->stride = mi->bytes_per_scanline;

	return BRE_OK;
}

/*
 * Return true if device can support linear mapping in the given mode
 */
br_boolean DeviceVESALinearAvailable(br_device * self, struct vesa_modeinfo *mi)
{
	host_info hi;

	/*
	 * VBE earlier that 2.0 does not support linear buffers
	 */
	if(self->info.vbe_version < 0x200)
		return BR_FALSE;
	
	/*
	 * Check if linear buffers are disabled
	 */
	if(self->use_linear == BR_FALSE)
		return BR_FALSE;

	/*
	 * Check mode info for physical buffer
	 */
	if(!(mi->mode_attributes & VESA_MA_LINEAR))
		return BR_FALSE;

	if(mi->phys_base_ptr == 0)
		return BR_FALSE;

	/*
	 * Can host do linear mapping?
	 */
	HostInfo(&hi,sizeof(hi));
	if(!(hi.capabilities & HOST_CAPS_PHYSICAL_MAP))
		return BR_FALSE;

	/*
	 * Everything looks good for linear mapping...
	 */
	return BR_TRUE;
}

static void BR_CMETHOD_DECL(br_device_vesa, free)(br_device *self)
{
	/*
	 * Remove attached objects
	 */
	ObjectContainerFree((br_object_container *)self, BR_NULL_TOKEN, NULL, NULL);

	/*
	 * Throw away attached resources
	 */
	BrResFreeNoCallback(self->res);
	self->res = NULL;
}

static br_token BR_CMETHOD_DECL(br_device_vesa, type)(br_device *self)
{
	return BRT_DEVICE;
}

static br_boolean BR_CMETHOD_DECL(br_device_vesa, isType)(br_device *self, br_token t)
{
	return (t == BRT_DEVICE) || (t == BRT_OBJECT_CONTAINER) || (t == BRT_OBJECT);
}

static br_int_32 BR_CMETHOD_DECL(br_device_vesa, space)(br_device *self)
{
	return sizeof(br_device);
}

static struct br_tv_template * BR_CMETHOD_DECL(br_device_vesa,queryTemplate)
	(br_device *self)
{
	if(self->info.vbe_version >= 0x200) {
		device2Template.res = self->res;
		return &device2Template;
	} else {
		device1Template.res = self->res;
		return &device1Template;
	}
}

static void * BR_CMETHOD_DECL(br_device_vesa,listQuery)
	(br_device *self)
{
	return self->object_list;
}

/*
 * Default dispatch table for device
 */
static struct br_device_dispatch deviceDispatch = {
	NULL,
	NULL,
	NULL,
	NULL,
	BR_CMETHOD_REF(br_device_vesa, free),
	BR_CMETHOD_REF(br_object_vesa, identifier),
	BR_CMETHOD_REF(br_device_vesa, type),
	BR_CMETHOD_REF(br_device_vesa, isType),
	BR_CMETHOD_REF(br_object_vesa, device),
	BR_CMETHOD_REF(br_device_vesa, space),

	BR_CMETHOD_REF(br_device_vesa,	queryTemplate),
	BR_CMETHOD_REF(br_object, 		query),
	BR_CMETHOD_REF(br_object, 		queryBuffer),
	BR_CMETHOD_REF(br_object, 		queryMany),
	BR_CMETHOD_REF(br_object, 		queryManySize),
	BR_CMETHOD_REF(br_object, 		queryAll),
	BR_CMETHOD_REF(br_object, 		queryAllSize),

	BR_CMETHOD_REF(br_device_vesa,		listQuery),
	BR_CMETHOD_REF(br_object_container,	tokensMatchBegin),
	BR_CMETHOD_REF(br_object_container,	tokensMatch),
	BR_CMETHOD_REF(br_object_container,	tokensMatchEnd),
	BR_CMETHOD_REF(br_object_container,	addFront),
	BR_CMETHOD_REF(br_object_container,	removeFront),
	BR_CMETHOD_REF(br_object_container,	remove),
	BR_CMETHOD_REF(br_object_container,	find),
	BR_CMETHOD_REF(br_object_container, findMany),
	BR_CMETHOD_REF(br_object_container, count),
};

