/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: outfcty.c 1.1 1997/12/10 16:51:11 jon Exp $
 * $Locker: $
 *
 * Output type methods
 */
#include <stddef.h>
#include <string.h>
#include <windows.h>
#include <ddraw.h>

#include "drv.h"
#include "host.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: outfcty.c 1.1 1997/12/10 16:51:11 jon Exp $");

/*
 * Default dispatch table for device (defined at end of file)
 */
static struct br_output_facility_dispatch outputFacilityDispatch;

// The primitive library we are going to use
extern br_primitive_library PrimitiveLibraryHardS3;

/*
 * Output Type info. template
 */
#define F(f)	offsetof(struct br_output_facility, f)

static struct br_tv_template_entry outputFacilityTemplateEntries[] = {
	{BRT(WIDTH_I32),      F(width),       BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY, },
	{BRT(WIDTH_MIN_I32),  F(width),       BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY, },
	{BRT(WIDTH_MAX_I32),  F(width),       BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY, },
	{BRT(HEIGHT_I32),     F(height),      BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY, },
	{BRT(HEIGHT_MIN_I32), F(height),      BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY, },
	{BRT(HEIGHT_MAX_I32), F(height),      BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY, },
	{BRT(PIXEL_BITS_I32), F(colour_bits), BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY, },
	{BRT(PIXEL_TYPE_U8),  F(colour_type), BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY, },
	{BRT(INDEXED_B),      F(indexed),     BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY, },
	{BRT(IDENTIFIER_CSTR),F(identifier),  BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY, },
   {BRT(PRIMITIVE_LIBRARY_O), F(prim_lib), BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY, },
	{BRT(WINDOW_FULLSCREEN_B),   F(fullscreen),     BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY, },

	{BRT_PIXELMAP_MAX_I32,	0,	0,					BRTV_QUERY | BRTV_ALL,	BRTV_CONV_DIRECT, 1},
	{BRT_CLUT_MAX_I32,		0,	0,					BRTV_QUERY | BRTV_ALL,	BRTV_CONV_DIRECT, 0},
	{BRT(WINDOW_HANDLE_H),	NULL,				BRTV_QUERY | BRTV_ALL,	BRTV_CONV_DIRECT, },
};
#undef F

static struct br_tv_template outputFacilityTemplate = {
	BR_ASIZE(outputFacilityTemplateEntries),
	outputFacilityTemplateEntries
};

void ClearOutfctyTemplates(void)
{
	CLEAR_TEMPLATE(outputFacility);
}




/*
 * Display mode enumeration callback
 */
HRESULT WINAPI EnumCallbackS3(LPDDSURFACEDESC ddsd, LPVOID context)
{
	void *res;
	char tmp[64];
	br_int_32 bits, type;
	br_boolean indexed;
	br_device *dev;
	br_output_facility *self;
	DDPIXELFORMAT *format;

	dev = (br_device *)context;

	/*
	 * Extract information from surface description
	 */
	format = &ddsd->ddpfPixelFormat;
#if 0	// Don't support this pixel format in Croc
	if(format->dwFlags & DDPF_PALETTEINDEXED8)
      {
		bits = 8;
		type = BR_PMT_INDEX_8;
		indexed = BR_TRUE;
	   }
   else
#endif
	if((format->dwFlags & DDPF_RGB) && (format->dwRBitMask == 0x00007c00) &&
		(format->dwGBitMask == 0x000003e0) && (format->dwBBitMask == 0x0000001f))
      {
		bits = dev->bits16 = 15;
		type = BR_PMT_RGB_555;
		indexed = BR_FALSE;
	   }
   else if((format->dwFlags & DDPF_RGB) && (format->dwRBitMask == 0x0000f800) &&
		(format->dwGBitMask == 0x000007e0) && (format->dwBBitMask == 0x0000001f))
      {
		bits = dev->bits16 = 16;
		type = BR_PMT_RGB_565;
		indexed = BR_FALSE;
	   }
#if 0	// Don't support this pixel format in Croc
   else if((format->dwFlags & DDPF_RGB) && (format->dwRBitMask == 0x00ff0000) &&
		(format->dwGBitMask == 0x0000ff00) && (format->dwBBitMask == 0x000000ff) &&
      (format->dwRGBBitCount == 32))
      {
		bits = 32;
		type = BR_PMT_RGBX_888;
		indexed = BR_FALSE;
	   }
#endif
   else
      return DDENUMRET_OK;

	/*
	 * Allocate an object
	 */
	res = DeviceS3Resource(dev);
	self = BrResAllocate(res, sizeof(*self), BR_MEMORY_OBJECT);

	/*
	 * Fill members
	 */
	self->num_instances = 0;
	self->dispatch = &outputFacilityDispatch;

	self->width = ddsd->dwWidth;
	self->height = ddsd->dwHeight;

	self->colour_type = type;
	self->colour_bits = bits;

	self->indexed = indexed;
	self->fullscreen = BR_TRUE;

  /* Set primitive library for hardware rendering */
  self->prim_lib = (br_object*)&PrimitiveLibraryHardS3;

	/*
	 * Attach a descriptive identifier
	 */
	BrSprintfN(tmp, sizeof(tmp) - 1, "F:%dx%dx%d", self->width, self->height, self->colour_bits);
	self->identifier = BrResStrDup(self, tmp);

	/*
	 * Add to list
	 */
	self->object_list = BrObjectListAllocate(self);
	ObjectContainerAddFront(dev, (br_object *)self);	

	/*
	 * Increment counter
	 */
	dev->output_facility_count++;

	return DDENUMRET_OK;
}



/*
 * Build a list of available output facilities
 *
 */
br_error OutputFacilityS3Initialise(br_device *dev)
{
	void *res;
	char tmp[64];
	br_uint_32 bpp, width, height;
   br_boolean windowed_allowed = BR_TRUE;
	br_output_facility *self;
	HDC dc;

	/*
	 * Determine current Windows mode
	 */
	dc = CreateDC("DISPLAY", NULL, NULL, NULL);
	bpp = GetDeviceCaps(dc, BITSPIXEL);
	width = GetDeviceCaps(dc, HORZRES);
	height = GetDeviceCaps(dc, VERTRES);
	DeleteDC(dc);

	if (windowed_allowed)
		{
		/*
	     * Allocate an object for the current (windowed) display mode
	     */
		res = DeviceS3Resource(dev);
		self = BrResAllocate(res, sizeof(*self), BR_MEMORY_OBJECT);

		/*
	     * Add to list as first entry and increment instance counter
	     */
		self->object_list = BrObjectListAllocate(self);
		ObjectContainerAddFront(dev, (br_object *)self);	
		dev->output_facility_count++;
		}

	/*
	 * Enumerate and record available fullscreen modes: do this now, to pick
	 * up whether 16-bit modes are 16 or 15 bits
	 */
	IDirectDraw_EnumDisplayModes(gpDD, 0, NULL, dev, EnumCallbackS3);

	if (windowed_allowed)
		{
		/*
	     * If Windows claims that current mode is 16-bit, use the
	     * actual bit depth returned during the fullscreen enumeration
	     */
		if(bpp == 16)
  			bpp = dev->bits16;

		/*
		 * Fill members of the windowed mode
	     */
		self->num_instances = 0;
		self->dispatch = &outputFacilityDispatch;

		self->width = width;
		self->height = height;

		self->colour_bits = bpp;
		self->fullscreen = BR_FALSE;

		switch(bpp)
			{
			case 8:
  				self->colour_type = BR_PMT_INDEX_8;
				self->indexed = BR_TRUE;
				break;

			case 15:
  				self->colour_type = BR_PMT_RGB_555;
				self->indexed = BR_FALSE;
				break;

			case 16:
				self->colour_type = BR_PMT_RGB_565;
				self->indexed = BR_FALSE;
				break;

			case 24:
				self->colour_type = BR_PMT_RGB_888;
				self->indexed = BR_FALSE;
				break;
			}

		/* Set primitive library for hardware rendering */
		self->prim_lib = (br_object*)&PrimitiveLibraryHardS3;

		/*
	     * Attach a descriptive identifier
	     */
		BrSprintfN(tmp, sizeof(tmp) - 1, "W:%dx%dx%d", self->width, self->height, self->colour_bits);
		self->identifier = BrResStrDup(self, tmp);
		}
	return BRE_OK;
}



/*
 * Common object methods
 */
static void BR_CMETHOD_DECL(br_output_facility_s3, free)(br_output_facility *self)
{
	ObjectContainerRemove(ObjectDevice(self), (br_object *)self);

	/* Remove attached objects */
	ObjectContainerFree((br_object_container *)self, BR_NULL_TOKEN, NULL, NULL);
   BrResFreeNoCallback(self);
}

static br_token BR_CMETHOD_DECL(br_output_facility_s3, type)(br_output_facility *self)
{
	return BRT_OUTPUT_FACILITY;
}

static br_boolean BR_CMETHOD_DECL(br_output_facility_s3, isType)(br_output_facility *self, br_token t)
{
	return (t == BRT_OUTPUT_FACILITY) || (t == BRT_OBJECT_CONTAINER) || (t == BRT_OBJECT);
}

static br_int_32 BR_CMETHOD_DECL(br_output_facility_s3, space)(br_output_facility *self)
{
	return sizeof(br_output_facility);
}

static struct br_tv_template * BR_CMETHOD_DECL(br_output_facility_s3,queryTemplate)(br_output_facility *self)
{
	outputFacilityTemplate.res = DeviceS3Resource(ObjectS3Device(self));
	return &outputFacilityTemplate;
}

static br_error BR_CMETHOD_DECL(br_output_facility_s3, validSource)(br_output_facility *self, br_boolean *bp, br_object *h)
{
	return BRE_OK;
}

/*
 * br_output_facility_s3::pixelmapNew
 *
 * Instantiate an output pixelmap from the output type
 */
static br_error BR_CMETHOD_DECL(br_output_facility_s3, pixelmapNew)(br_output_facility *self,
	br_device_pixelmap **ppmap, br_token_value *tv)
{
	br_device_pixelmap *pm;

	/*
	 * Create a device pixelmap structure representing display memory
	 */
	pm = DevicePixelmapS3Allocate(ObjectDevice(self), self, tv);
	if(pm == NULL)
		return BRE_FAIL;

	*ppmap = pm;

	return BRE_OK;
}

/*
 * Cannot create new CLUTs, stuck with the single hardware one
 */
static br_error BR_CMETHOD_DECL(br_output_facility_s3, clutNew)(br_output_facility *self,
	br_device_clut **pclut, br_token_value *tv)
{
	return BRE_FAIL;
}

static void * BR_CMETHOD_DECL(br_output_facility_s3, listQuery)
	(br_output_facility *self)
{
	return self->object_list;
}

/*
 * Default dispatch table for device
 */
static struct br_output_facility_dispatch outputFacilityDispatch = {
	NULL,
	NULL,
	NULL,
	NULL,
	BR_CMETHOD_REF(br_output_facility_s3,	free),
	BR_CMETHOD_REF(br_object_s3,		  	   identifier),
	BR_CMETHOD_REF(br_output_facility_s3,	type),
	BR_CMETHOD_REF(br_output_facility_s3,	isType),
	BR_CMETHOD_REF(br_object_s3,			   device),
	BR_CMETHOD_REF(br_output_facility_s3,	space),

	BR_CMETHOD_REF(br_output_facility_s3,	queryTemplate),
	BR_CMETHOD_REF(br_object,				   query),
	BR_CMETHOD_REF(br_object,				   queryBuffer),
	BR_CMETHOD_REF(br_object,				   queryMany),
	BR_CMETHOD_REF(br_object,				   queryManySize),
	BR_CMETHOD_REF(br_object,				   queryAll),
	BR_CMETHOD_REF(br_object,				   queryAllSize),

	BR_CMETHOD_REF(br_output_facility_s3,	listQuery),
	BR_CMETHOD_REF(br_object_container,    tokensMatchBegin),
	BR_CMETHOD_REF(br_object_container,		tokensMatch),
	BR_CMETHOD_REF(br_object_container,		tokensMatchEnd),
	BR_CMETHOD_REF(br_object_container,		addFront),
	BR_CMETHOD_REF(br_object_container,		removeFront),
	BR_CMETHOD_REF(br_object_container,		remove),
	BR_CMETHOD_REF(br_object_container,		find),
	BR_CMETHOD_REF(br_object_container,		findMany),
	BR_CMETHOD_REF(br_object_container,		count),

	BR_CMETHOD_REF(br_output_facility_s3,	validSource),
	BR_CMETHOD_REF(br_output_facility_s3,	pixelmapNew),
	BR_CMETHOD_REF(br_output_facility_s3,	clutNew),
};


