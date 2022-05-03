/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: outfcty.c 1.1 1997/12/10 16:45:46 jon Exp $
 * $Locker: $
 *
 * Output type methods
 */
#include <stddef.h>
#include <string.h>

#include "drv.h"
#include "host.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: outfcty.c 1.1 1997/12/10 16:45:46 jon Exp $");

/*
 * Default dispatch table for device (defined at end of file)
 */
static const struct br_output_facility_dispatch outputFacilityDispatch;

/*
 * Output Type info. template
 */
#define F(f)	offsetof(struct br_output_facility, f)

static const struct br_tv_template_entry outputFacilityTemplateEntries[] = {
	{BRT(IDENTIFIER_CSTR),	F(identifier),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },

	{BRT(WIDTH_I32),	 	F(width),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(WIDTH_MIN_I32), 	F(width),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(WIDTH_MAX_I32), 	F(width),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(HEIGHT_I32),		F(height),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(HEIGHT_MIN_I32),	F(height),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(HEIGHT_MAX_I32),	F(height),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },

	{BRT(PIXEL_TYPE_U8),	F(colour_type),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(PIXEL_BITS_I32),	F(colour_bits),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },

//	{BRT(PIXEL_CHANNELS_I32),F(channels_mask),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
//	{BRT(PIXEL_CHANNELS_TL), F(channels_list),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },

	{BRT(INDEXED_B),	 	F(indexed),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },

//	{BRT(MEMORY_MAPPED_B), 	F(),				BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },

	{BRT_PIXELMAP_MAX_I32, 	0,	0,				BRTV_QUERY | BRTV_ALL,	BRTV_CONV_DIRECT, 1},
	{BRT_CLUT_MAX_I32, 		0,	0,				BRTV_QUERY | BRTV_ALL,	BRTV_CONV_DIRECT, 0},

//	{BRT(VIDEO_MEMORY_U32), F(),				BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
//	{BRT(TEXTURE_MEMORY_U32), F(),				BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
//	{BRT(HOST_MEMORY_U32), F(),					BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },

	{BRT(MODE_U32),			0x13,				BRTV_QUERY | BRTV_ALL | BRTV_ABS,	BRTV_CONV_COPY, },
};
#undef F

/*
 * Setup a static output facility
 */
br_output_facility * OutputFacilityVGAAllocate(br_device *dev, char *identifier,
    br_int_32 mode, br_int_32 width, br_int_32 height, br_int_32 bits, br_int_32 type, br_boolean indexed)
{
    br_output_facility *self;

   	self = BrResAllocate(NULL, sizeof(*self), BR_MEMORY_OBJECT_DATA);

	self->dispatch = &outputFacilityDispatch;
    self->identifier = identifier;
    self->device = dev;

    self->bios_mode = mode;
    self->width = width;
    self->height = height;
    self->colour_type = type;
    self->colour_bits = bits;
    self->indexed = indexed;

	self->num_instances = 0;
	self->object_list = BrObjectListAllocate(DeviceVGAResource(dev));

	ObjectContainerAddFront(dev, (br_object *)self);
	return self;
}

/*
 * Common object methods
 */
static void BR_CMETHOD_DECL(br_output_facility_vga, free)(br_output_facility *self)
{
	br_object *h;

	ObjectContainerRemove(ObjectDevice(self), (br_object *)self);

	/*
	 * Remove attached objects
	 */
	BrObjectContainerFree((br_object_container *)self, BR_NULL_TOKEN, NULL, NULL);

    BrResFreeNoCallback(self);
}

static br_token BR_CMETHOD_DECL(br_output_facility_vga, type)(br_output_facility *self)
{
	return BRT_OUTPUT_FACILITY;
}

static br_boolean BR_CMETHOD_DECL(br_output_facility_vga, isType)(br_output_facility *self, br_token t)
{
	return (t == BRT_OUTPUT_FACILITY) || (t == BRT_OBJECT_CONTAINER) || (t == BRT_OBJECT);
}

static br_int_32 BR_CMETHOD_DECL(br_output_facility_vga, space)(br_output_facility *self)
{
	return sizeof(br_output_facility);
}

static struct br_tv_template * BR_CMETHOD_DECL(br_output_facility_vga,queryTemplate)(br_output_facility *self)
{
    if(self->device->templates.outputFacilityTemplate== NULL)
        self->device->templates.outputFacilityTemplate= BrTVTemplateAllocate(self->device,
            outputFacilityTemplateEntries,
            BR_ASIZE(outputFacilityTemplateEntries));

    return self->device->templates.outputFacilityTemplate;
}

static br_error BR_CMETHOD_DECL(br_output_facility_vga, validSource)(br_output_facility *self, br_boolean *bp, br_object *h)
{
	return BRE_OK;
}

/*
 * Instantiate an output pixelmap from the output type
 *
 */
static br_error BR_CMETHOD_DECL(br_output_facility_vga, pixelmapNew)(br_output_facility *self,
	br_device_pixelmap **ppmap, br_token_value *tv)
{
	br_error r;
	br_device_pixelmap *pm;

	/*
	 * Create device pixelmap structure pointing at display memory
	 */
	pm = DevicePixelmapMCGAAllocateMode(ObjectDevice(self), self, self->width, self->height);

	if(pm == NULL)
		return BRE_FAIL;

	*ppmap = pm;
	self->num_instances++;

	return BRE_OK;
}

/*
 * Cannot create new CLUTs, stuck with the single hardware one
 */
static br_error BR_CMETHOD_DECL(br_output_facility_vga, clutNew)(br_output_facility *self,
	br_device_clut **pclut, br_token_value *tv)
{
	return BRE_FAIL;
}

/*
 * No querying ability yet
 */
static br_error BR_CMETHOD_DECL(br_output_facility_vga, queryCapability)(
    br_token_value *buffer_in, br_token_value *buffer_out, br_size_t size_buffer_out)
{
	return BRE_FAIL;
}

static void * BR_CMETHOD_DECL(br_output_facility_vga, listQuery)
	(br_output_facility *self)
{
	return self->object_list;
}

/*
 * Default dispatch table for device
 */
static const struct br_output_facility_dispatch outputFacilityDispatch = {
	NULL,
	NULL,
	NULL,
	NULL,
	BR_CMETHOD_REF(br_output_facility_vga,	free),
	BR_CMETHOD_REF(br_object_vga,		  	identifier),
	BR_CMETHOD_REF(br_output_facility_vga,	type),
	BR_CMETHOD_REF(br_output_facility_vga,	isType),
	BR_CMETHOD_REF(br_object_vga,			device),
	BR_CMETHOD_REF(br_output_facility_vga,	space),

	BR_CMETHOD_REF(br_output_facility_vga,	queryTemplate),
	BR_CMETHOD_REF(br_object,				query),
	BR_CMETHOD_REF(br_object,				queryBuffer),
	BR_CMETHOD_REF(br_object,				queryMany),
	BR_CMETHOD_REF(br_object,				queryManySize),
	BR_CMETHOD_REF(br_object,				queryAll),
	BR_CMETHOD_REF(br_object,				queryAllSize),

	BR_CMETHOD_REF(br_output_facility_vga,	listQuery),
	BR_CMETHOD_REF(br_object_container,		tokensMatchBegin),
	BR_CMETHOD_REF(br_object_container,		tokensMatch),
	BR_CMETHOD_REF(br_object_container,		tokensMatchEnd),
	BR_CMETHOD_REF(br_object_container,		addFront),
	BR_CMETHOD_REF(br_object_container,		removeFront),
	BR_CMETHOD_REF(br_object_container,		remove),
	BR_CMETHOD_REF(br_object_container,		find),
	BR_CMETHOD_REF(br_object_container,		findMany),
	BR_CMETHOD_REF(br_object_container,		count),

	BR_CMETHOD_REF(br_output_facility_vga,	validSource),
	BR_CMETHOD_REF(br_output_facility_vga,	pixelmapNew),
	BR_CMETHOD_REF(br_output_facility_vga,	clutNew),
    BR_CMETHOD_REF(br_output_facility_vga,  queryCapability),
};

