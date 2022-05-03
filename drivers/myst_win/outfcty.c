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
br_error OutputFacilityMystInitialise(br_output_facility *self, br_device *dev,
	br_primitive_library *prim_lib)
{
	CLEAR_TEMPLATE(outputFacility);

	self->dispatch = &outputFacilityDispatch;
	self->num_instances = 0;
    self->object_list = BrObjectListAllocate(DeviceMystResource(dev));
	self->prim_lib = prim_lib;
	ObjectContainerAddFront(dev, (br_object *)self);
	return BRE_OK;
}

/*
 * Common object methods
 */
static void BR_CMETHOD_DECL(br_output_facility_mystique, free)(br_output_facility *self)
{
	ObjectContainerRemove(ObjectDevice(self), (br_object *)self);

	/*
	 * Remove attached objects
	 */
	BrObjectContainerFree((br_object_container *)self, BR_NULL_TOKEN, NULL, NULL);

}

static br_token BR_CMETHOD_DECL(br_output_facility_mystique, type)(br_output_facility *self)
{
	return BRT_OUTPUT_FACILITY;
}

static br_boolean BR_CMETHOD_DECL(br_output_facility_mystique, isType)(br_output_facility *self, br_token t)
{
	return (t == BRT_OUTPUT_FACILITY) || (t == BRT_OBJECT_CONTAINER) || (t == BRT_OBJECT);
}

static br_int_32 BR_CMETHOD_DECL(br_output_facility_mystique, space)(br_output_facility *self)
{
	return sizeof(br_output_facility);
}

static struct br_tv_template * BR_CMETHOD_DECL(br_output_facility_mystique,queryTemplate)(br_output_facility *self)
{
    outputFacilityTemplate.res = DeviceMystResource(ObjectMystDevice(self));
	return &outputFacilityTemplate;
}

static br_error BR_CMETHOD_DECL(br_output_facility_mystique, validSource)(br_output_facility *self, br_boolean *bp, br_object *h)
{
	return BRE_OK;
}

/*
 * Instantiate an output pixelmap from the output type
 *
 */
static br_error BR_CMETHOD_DECL(br_output_facility_mystique, pixelmapNew)(br_output_facility *self,
	br_device_pixelmap **ppmap, br_token_value *tv)
{
	br_device_pixelmap *pm;
	br_error result;

	/*
	 * Create device pixelmap structure pointing at display memory
	 */
    result = DevicePixelmapMystAllocateMode(ObjectDevice(self), self,
											&pm, tv);

	if (result == BRE_OK) {
		*ppmap = pm;
		self->num_instances++;
	}

	return(result);
}

/*
 */
static br_error BR_CMETHOD_DECL(br_output_facility_mystique, clutNew)(br_output_facility *self,
	br_device_clut **pclut, br_token_value *tv)
{
	return BRE_DEV_FAIL;
}

static void * BR_CMETHOD_DECL(br_output_facility_mystique, listQuery)(br_output_facility *self)
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
    BR_CMETHOD_REF(br_output_facility_mystique, free),
    BR_CMETHOD_REF(br_object_mystique,          identifier),
    BR_CMETHOD_REF(br_output_facility_mystique, type),
    BR_CMETHOD_REF(br_output_facility_mystique, isType),
    BR_CMETHOD_REF(br_object_mystique,          device),
    BR_CMETHOD_REF(br_output_facility_mystique, space),

    BR_CMETHOD_REF(br_output_facility_mystique, queryTemplate),
    BR_CMETHOD_REF(br_object,                   query),
    BR_CMETHOD_REF(br_object,                   queryBuffer),
    BR_CMETHOD_REF(br_object,                   queryMany),
    BR_CMETHOD_REF(br_object,                   queryManySize),
    BR_CMETHOD_REF(br_object,                   queryAll),
    BR_CMETHOD_REF(br_object,                   queryAllSize),

    BR_CMETHOD_REF(br_output_facility_mystique, listQuery),
    BR_CMETHOD_REF(br_object_container,         tokensMatchBegin),
    BR_CMETHOD_REF(br_object_container,         tokensMatch),
    BR_CMETHOD_REF(br_object_container,         tokensMatchEnd),
    BR_CMETHOD_REF(br_object_container,         addFront),
    BR_CMETHOD_REF(br_object_container,         removeFront),
    BR_CMETHOD_REF(br_object_container,         remove),
    BR_CMETHOD_REF(br_object_container,         find),
    BR_CMETHOD_REF(br_object_container,         findMany),
    BR_CMETHOD_REF(br_object_container,         count),

    BR_CMETHOD_REF(br_output_facility_mystique, validSource),
    BR_CMETHOD_REF(br_output_facility_mystique, pixelmapNew),
    BR_CMETHOD_REF(br_output_facility_mystique, clutNew),
};

