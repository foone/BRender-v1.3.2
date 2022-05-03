/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * Output type methods
 */
#include <stddef.h>
#include <string.h>

#include "drv.h"
#include "host.h"
#include "shortcut.h"
#include "brassert.h"


/*
 * Default dispatch table for device (defined at end of file)
 */
static struct br_output_facility_dispatch outputFacilityDispatch;

/*
 * Output Type info. template
 */
#define F(f)	offsetof(struct br_output_facility, f)

static struct br_tv_template_entry outputFacilityTemplateEntries[] = {
	{BRT(IDENTIFIER_CSTR),	F(identifier),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },

	{BRT(WIDTH_I32),	 	F(width),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(WIDTH_MIN_I32), 	F(width),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(WIDTH_MAX_I32), 	F(width),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(HEIGHT_I32),		F(height),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(HEIGHT_MIN_I32),	F(height),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(HEIGHT_MAX_I32),	F(height),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },

	{BRT(PIXEL_TYPE_U8),	F(colour_type),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(PIXEL_BITS_I32),	F(colour_bits),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },

	{BRT(INDEXED_B),	 	F(indexed),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },

	{BRT(WINDOW_FULLSCREEN_B), F(fullscreen),	BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },

	{BRT_PIXELMAP_MAX_I32, 	0,	0,				BRTV_QUERY | BRTV_ALL,	BRTV_CONV_DIRECT, 1},
	{BRT_CLUT_MAX_I32, 		0,	0,				BRTV_QUERY | BRTV_ALL,	BRTV_CONV_DIRECT, 0},

	{BRT(PRIMITIVE_LIBRARY_O),F(prim_lib),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },

	{BRT(WINDOW_HANDLE_H),	NULL,				BRTV_QUERY | BRTV_ALL,	BRTV_CONV_DIRECT, },
};
#undef F

static struct br_tv_template outputFacilityTemplate = {
	BR_ASIZE(outputFacilityTemplateEntries),
	outputFacilityTemplateEntries
};

/*
 * Setup a static output facility
 */
br_uint_32 OutputFacility3DfxInitialise(br_device *dev, br_primitive_library *prim_lib)
{
	int i;
	br_output_facility *self;
	void *res;
	char buf[256];

//#define GLIDE_MODES_SUPPORTED	11
#define GLIDE_MODES_SUPPORTED	4
	static br_uint_32 mode_list[3*GLIDE_MODES_SUPPORTED]={
//			GR_RESOLUTION_320x200,320,200,
//			GR_RESOLUTION_320x240,320,240,
//			GR_RESOLUTION_400x256,400,256,
			GR_RESOLUTION_512x384,512,384,
//			GR_RESOLUTION_640x200,640,200,
//			GR_RESOLUTION_640x350,640,350,
			GR_RESOLUTION_640x400,640,400,
			GR_RESOLUTION_640x480,640,480,
			GR_RESOLUTION_800x600,800,600,
//			GR_RESOLUTION_960x720,960,720,
//			GR_RESOLUTION_856x480,856,480
	};

	CLEAR_TEMPLATE(outputFacility);

	/* Build an output facility for each possible 3Dfx mode */
	res = Device3DfxResource(dev);

	for (i=0; i<GLIDE_MODES_SUPPORTED*2; i++) {
		self = BrResAllocate(res, sizeof(*self), BR_MEMORY_OBJECT);

		/* System stuff */
		self->num_instances		= 0;
		self->dispatch			= &outputFacilityDispatch;
		self->object_list		= BrObjectListAllocate(Device3DfxResource(dev));
		self->prim_lib			= prim_lib;

		/* Copy details into output facility */
		self->glide_res_code	= mode_list[3*(i/2)+0];
		self->width				= mode_list[3*(i/2)+1];
		self->height			= mode_list[3*(i/2)+2];

		/* Build 555 and 565 mode buffers alternately */
		if (i % 2) {
			self->colour_bits		= 16;
			self->colour_type		= BR_PMT_RGB_565;
			self->indexed			= BR_FALSE;
		} else {
			self->colour_bits		= 15;
			self->colour_type		= BR_PMT_RGB_555;
			self->indexed			= BR_FALSE;
		}

		self->depth_bits		= 16;
		self->depth_type		= BR_PMT_DEPTH_16;

		/* Always fullscreen here! */
		self->fullscreen		= BR_TRUE;

		/* Make identifier */
		BrSprintf(buf, "F:%dx%dx%d", self->width, self->height, self->colour_bits);
		self->identifier		= BrResStrDup(self, buf);

		/* Add to the device's list of output facilities */
		ObjectContainerAddFront(dev, (br_object *)self);
	}

	return(GLIDE_MODES_SUPPORTED);
}

/*
 * Common object methods
 */
static void BR_CMETHOD_DECL(br_output_facility_3dfx, free)(br_output_facility *self)
{
	ObjectContainerRemove(ObjectDevice(self), (br_object *)self);

	/*
	 * Remove attached objects
	 */
	BrObjectContainerFree((br_object_container *)self, BR_NULL_TOKEN, NULL, NULL);

	BrResFreeNoCallback(self);
}

static br_token BR_CMETHOD_DECL(br_output_facility_3dfx, type)(br_output_facility *self)
{
	return BRT_OUTPUT_FACILITY;
}

static br_boolean BR_CMETHOD_DECL(br_output_facility_3dfx, isType)(br_output_facility *self, br_token t)
{
	return (t == BRT_OUTPUT_FACILITY) || (t == BRT_OBJECT_CONTAINER) || (t == BRT_OBJECT);
}

static br_int_32 BR_CMETHOD_DECL(br_output_facility_3dfx, space)(br_output_facility *self)
{
	return sizeof(br_output_facility);
}

static struct br_tv_template * BR_CMETHOD_DECL(br_output_facility_3dfx,queryTemplate)(br_output_facility *self)
{
	outputFacilityTemplate.res = Device3DfxResource(Object3DfxDevice(self));
	return &outputFacilityTemplate;
}

static br_error BR_CMETHOD_DECL(br_output_facility_3dfx, validSource)(br_output_facility *self, br_boolean *bp, br_object *h)
{
	return BRE_OK;
}

/*
 * Instantiate an output pixelmap from the output type
 *
 */
static br_error BR_CMETHOD_DECL(br_output_facility_3dfx, pixelmapNew)(br_output_facility *self,
	br_device_pixelmap **ppmap, br_token_value *tv)
{
	br_device_pixelmap *pm;
	br_error result;

	/*
	 * Create device pixelmap structure pointing at display memory
	 */
	result = DevicePixelmap3DfxAllocateMode(ObjectDevice(self), self, &pm, tv);

	if (result == BRE_OK) {
		*ppmap = pm;
		self->num_instances++;
	}

	return(result);
}

/*
 */
static br_error BR_CMETHOD_DECL(br_output_facility_3dfx, clutNew)(br_output_facility *self,
	br_device_clut **pclut, br_token_value *tv)
{
	return BRE_DEV_FAIL;
}

static void * BR_CMETHOD_DECL(br_output_facility_3dfx, listQuery)(br_output_facility *self)
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
	BR_CMETHOD_REF(br_output_facility_3dfx,	free),
	BR_CMETHOD_REF(br_object_3dfx,		  	identifier),
	BR_CMETHOD_REF(br_output_facility_3dfx,	type),
	BR_CMETHOD_REF(br_output_facility_3dfx,	isType),
	BR_CMETHOD_REF(br_object_3dfx,			device),
	BR_CMETHOD_REF(br_output_facility_3dfx,	space),

	BR_CMETHOD_REF(br_output_facility_3dfx,	queryTemplate),
	BR_CMETHOD_REF(br_object,				query),
	BR_CMETHOD_REF(br_object,				queryBuffer),
	BR_CMETHOD_REF(br_object,				queryMany),
	BR_CMETHOD_REF(br_object,				queryManySize),
	BR_CMETHOD_REF(br_object,				queryAll),
	BR_CMETHOD_REF(br_object,				queryAllSize),

	BR_CMETHOD_REF(br_output_facility_3dfx,	listQuery),
	BR_CMETHOD_REF(br_object_container,		tokensMatchBegin),
	BR_CMETHOD_REF(br_object_container,		tokensMatch),
	BR_CMETHOD_REF(br_object_container,		tokensMatchEnd),
	BR_CMETHOD_REF(br_object_container,		addFront),
	BR_CMETHOD_REF(br_object_container,		removeFront),
	BR_CMETHOD_REF(br_object_container,		remove),
	BR_CMETHOD_REF(br_object_container,		find),
	BR_CMETHOD_REF(br_object_container,		findMany),
	BR_CMETHOD_REF(br_object_container,		count),

	BR_CMETHOD_REF(br_output_facility_3dfx,	validSource),
	BR_CMETHOD_REF(br_output_facility_3dfx,	pixelmapNew),
	BR_CMETHOD_REF(br_output_facility_3dfx,	clutNew),
};

