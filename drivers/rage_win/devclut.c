/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: devclut.c 1.1 1997/12/10 16:50:04 jon Exp $
 * $Locker: $
 *
 * CLUT methods
 */
#include <stddef.h>
#include <string.h>
#include <windows.h>
#include <ddraw.h>
#include <conio.h>

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: devclut.c 1.1 1997/12/10 16:50:04 jon Exp $");

/*
 * Default dispatch table for device_clut (defined at end of file)
 */
static struct br_device_clut_dispatch deviceClutDispatch;

/*
 * Query template
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
br_device_clut * DeviceClutATIAllocate(br_device *dev, br_device_pixelmap *devpm, char *identifier)
{
	br_device_clut *self;

	UASSERT(dev != NULL);
	UASSERT(devpm != NULL);

  /* Clear static clut structure */
  CLEAR_TEMPLATE(deviceClut);

  /* Create a default palette for textures (sbuffer.c)*/
  if(CreateDefaultPalette() != BRE_OK)
     return NULL;

   /*
	 * Allocate device CLUT structure
	 */
	self = BrResAllocate(DeviceATIResource(dev), sizeof(*self), BR_MEMORY_OBJECT);

	/*
	 * Fill members
	 */
	self->dispatch = &deviceClutDispatch;
	if(identifier)
	 	self->identifier = identifier;
	self->devpm = devpm;

	/*
	 * Attach CLUT to device
	 */
	ObjectContainerAddFront(dev, (br_object *)self);

	return self;
}

/*
 * br_device_clut_ati::free
 */
static void BR_CMETHOD_DECL(br_device_clut_ati, free)\
	(br_device_clut *self)
{
	/*
	 * Detach CLUT from device
	 */
	ObjectContainerRemove(ObjectDevice(self), (br_object *)self);

	   /*
	 * Free resources
	 */
	BrResFreeNoCallback(self);

}

static br_token BR_CMETHOD_DECL(br_device_clut_ati, type)\
	(br_device_clut *self)
{
	return BRT_DEVICE_CLUT;
}

static br_boolean BR_CMETHOD_DECL(br_device_clut_ati, isType)\
	(br_device_clut *self, br_token t)
{
	return (t == BRT_DEVICE_CLUT) || (t == BRT_OBJECT);
}

static br_int_32 BR_CMETHOD_DECL(br_device_clut_ati, space)\
	(br_device_clut *self)
{
	return sizeof(br_device_clut);
}

static struct br_tv_template * BR_CMETHOD_DECL(br_device_clut_ati, queryTemplate)\
	(br_device_clut *self)
{
	deviceClutTemplate.res = DeviceATIResource(ObjectATIDevice(self));
	return &deviceClutTemplate;
}

/*
 * br_device_clut_ati::entrySet
 */
static br_error BR_CMETHOD_DECL(br_device_clut_ati, entrySet)\
	(br_device_clut *self, br_int_32 index, br_colour entry)
{
   br_error r;

   /* Set the entry in the default texture palette */
   r = SetDefaultPaletteEntry(index,entry);
   if(r != BRE_OK)
      return r;

	return BRE_OK;
}

/*
 * br_device_clut_ati::entrySetMany
 */
static br_error BR_CMETHOD_DECL(br_device_clut_ati, entrySetMany)\
	(br_device_clut *self, br_int_32 index, br_int_32 count, br_colour *entries)
{
   br_error r;

   /* Set these indices in the default texture palette */
   r = SetDefaultPaletteEntries(index,count,entries);
   if(r != BRE_OK)
      return r;
      
	return BRE_OK;
}

/*
 * br_device_clut_ati::entryQuery
 */
static br_error BR_CMETHOD_DECL(br_device_clut_ati, entryQuery)\
	(br_device_clut *self, br_colour *entry, br_int_32 index)
{
	return BRE_FAIL;
}

/**
 ** br_device_clut_ati::entryQueryMany
 **/
static br_error BR_CMETHOD_DECL(br_device_clut_ati, entryQueryMany)\
	(br_device_clut *self, br_colour *entries, br_int_32 index, br_int_32 count)
{
	return BRE_FAIL;
}

/*
 * Default dispatch table for device CLUT
 */
static struct br_device_clut_dispatch deviceClutDispatch = {
	NULL,
	NULL,
	NULL,
	NULL,
	BR_CMETHOD_REF(br_device_clut_ati,	free),
	BR_CMETHOD_REF(br_object_ati,		identifier),
	BR_CMETHOD_REF(br_device_clut_ati,	type),
	BR_CMETHOD_REF(br_device_clut_ati,	isType),
	BR_CMETHOD_REF(br_object_ati,		device),
	BR_CMETHOD_REF(br_device_clut_ati,	space),

	BR_CMETHOD_REF(br_device_clut_ati,	queryTemplate),
	BR_CMETHOD_REF(br_object,			query),
	BR_CMETHOD_REF(br_object,			queryBuffer),
	BR_CMETHOD_REF(br_object,			queryMany),
	BR_CMETHOD_REF(br_object,			queryManySize),
	BR_CMETHOD_REF(br_object,			queryAll),
	BR_CMETHOD_REF(br_object,			queryAllSize),

	BR_CMETHOD_REF(br_device_clut_ati,	entrySet),
	BR_CMETHOD_REF(br_device_clut_ati,	entryQuery),
	BR_CMETHOD_REF(br_device_clut_ati,	entrySetMany),
	BR_CMETHOD_REF(br_device_clut_ati,	entryQueryMany),
};

