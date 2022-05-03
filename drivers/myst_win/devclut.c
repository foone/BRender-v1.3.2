/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * Mystique CLUT methods
 */
#include <stddef.h>
#include <string.h>

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

/*
 * Default dispatch table for device_clut (defined at end of file)
 */
static struct br_device_clut_dispatch deviceClutDispatch;

/*
 * Renderer info. template
 */
#define F(f)	offsetof(struct br_device_clut, f)

static struct br_tv_template_entry deviceClutTemplateEntries[] = {
	{BRT_IDENTIFIER_CSTR,	0,	F(identifier),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
};
#undef F

static struct br_tv_template deviceClutTemplate = {
	BR_ASIZE(deviceClutTemplateEntries),
	deviceClutTemplateEntries
};

/*
 * Create a new device CLUT
 */
br_device_clut * DeviceClutMystiqueAllocate(br_device *dev, char *identifier)
{
	br_device_clut *self;
	int i;

	self = BrResAllocate(DeviceMystResource(dev), sizeof(*self), BR_MEMORY_OBJECT);
	if (!self)
		return(NULL);

	self->dispatch = &deviceClutDispatch;
	if(identifier)
	 	self->identifier = identifier;

	for(i=0; i < CLUT_SIZE; i++)
		self->entries[i] = BR_COLOUR_RGB(i,i,i);

	ObjectContainerAddFront(dev, (br_object *)self);

	return self;
}

static void BR_CMETHOD_DECL(br_device_clut_mystique, free)(br_device_clut *self)
{
	ObjectContainerRemove(ObjectDevice(self), (br_object *)self);

	BrResFreeNoCallback(self);
}

static br_token BR_CMETHOD_DECL(br_device_clut_mystique, type)(br_device_clut *self)
{
	return BRT_DEVICE_CLUT;
}

static br_boolean BR_CMETHOD_DECL(br_device_clut_mystique, isType)(br_device_clut *self, br_token t)
{
	return (t == BRT_DEVICE_CLUT) || (t == BRT_OBJECT);
}

static br_int_32 BR_CMETHOD_DECL(br_device_clut_mystique, space)(br_device_clut *self)
{
	return sizeof(br_device_clut);
}

static struct br_tv_template * BR_CMETHOD_DECL(br_device_clut_mystique,queryTemplate)(br_device_clut *self)
{
	deviceClutTemplate.res = DeviceMystResource(ObjectMystDevice(self));
	return &deviceClutTemplate;
}

static br_error BR_CMETHOD_DECL(br_device_clut_mystique, entrySet)
		(br_device_clut *self, br_int_32 index, br_colour entry)
{
	if(index < 0 || index >= CLUT_SIZE)
		return BRE_OVERFLOW;

	self->entries[index] = entry;

	return BRE_OK;
}

static br_error BR_CMETHOD_DECL(br_device_clut_mystique, entryQuery)
		(br_device_clut *self, br_colour *entry, br_int_32 index)
{
	if(index < 0 || index >= CLUT_SIZE)
		return BRE_OVERFLOW;

	*entry = self->entries[index];

	return BRE_OK;
}

static br_error BR_CMETHOD_DECL(br_device_clut_mystique, entrySetMany)
		(br_device_clut *self, br_int_32 index, br_int_32 count, br_colour *entries)
{
	int i;

	if(index < 0 || index >= CLUT_SIZE)
		return BRE_OVERFLOW;

	if(index + count > CLUT_SIZE)
		return BRE_OVERFLOW;

	for(i=0; i < count; i++) {
		self->entries[index+i] = entries[i];
	}

	return BRE_OK;
}

static br_error BR_CMETHOD_DECL(br_device_clut_mystique, entryQueryMany)
		(br_device_clut *self, br_colour *entries, br_int_32 index, br_int_32 count)
{
	int i;

	if(index < 0 || index >= CLUT_SIZE)
		return BRE_OVERFLOW;

	if(index + count > CLUT_SIZE)
		return BRE_OVERFLOW;

	for(i=0; i < count; i++)
		entries[i] = self->entries[index+i];

	return BRE_OK;
}


/*
 * Default dispatch table for device CLUT
 */
static struct br_device_clut_dispatch deviceClutDispatch = {
	NULL,
	NULL,
	NULL,
	NULL,
	BR_CMETHOD_REF(br_device_clut_mystique,	free),
	BR_CMETHOD_REF(br_object_mystique,		identifier),
	BR_CMETHOD_REF(br_device_clut_mystique,	type),
	BR_CMETHOD_REF(br_device_clut_mystique,	isType),
	BR_CMETHOD_REF(br_object_mystique,		device),
	BR_CMETHOD_REF(br_device_clut_mystique,	space),

	BR_CMETHOD_REF(br_device_clut_mystique,	queryTemplate),
	BR_CMETHOD_REF(br_object,				query),
	BR_CMETHOD_REF(br_object,				queryBuffer),
	BR_CMETHOD_REF(br_object,				queryMany),
	BR_CMETHOD_REF(br_object,				queryManySize),
	BR_CMETHOD_REF(br_object,				queryAll),
	BR_CMETHOD_REF(br_object,				queryAllSize),

	BR_CMETHOD_REF(br_device_clut_mystique,	entrySet),
	BR_CMETHOD_REF(br_device_clut_mystique,	entryQuery),
	BR_CMETHOD_REF(br_device_clut_mystique,	entrySetMany),
	BR_CMETHOD_REF(br_device_clut_mystique,	entryQueryMany),
};

