/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: device.c 1.1 1997/12/10 16:46:41 jon Exp $
 * $Locker: $
 *
 * Device methods
 */
#include <stddef.h>
#include <string.h>

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: device.c 1.1 1997/12/10 16:46:41 jon Exp $");

/*
 * Default dispatch table for device (defined at end of file)
 */
static const struct br_device_dispatch deviceDispatch;

/*
 * Device info. template
 */
#define F(f)	offsetof(br_device, f)

static const struct br_tv_template_entry deviceTemplateEntries[] = {
	{BRT_IDENTIFIER_CSTR,	0,	F(identifier),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
};
#undef F

/*
 * Set up a static device object
 */
br_device * DeviceSoftPrimAllocate(char * identifier)
{
    br_device *self;

   	/*
	 * Set up device block and resource anchor
	 */
	self = BrResAllocate(NULL, sizeof(*self), BR_MEMORY_OBJECT);
	self->res = BrResAllocate(self, 0, BR_MEMORY_DRIVER);

    self->identifier = identifier;
	self->dispatch = &deviceDispatch;
	self->device = self;

	self->object_list = BrObjectListAllocate(self);

    /*
     * Some useful tokens for debug queries
     */
#if DEBUG	
	BrTokenCreate("OPAQUE_MAP_B",	BRT_BOOLEAN);
	BrTokenCreate("NO_SKIP_B",		BRT_BOOLEAN);
	BrTokenCreate("BLOCK_P",		BRT_POINTER);
#endif

	return self;
}

static void BR_CMETHOD_DECL(br_device_softprim, free)(br_device *self)
{
	/*
	 * Remove attached objects
	 */
	BrObjectContainerFree((br_object_container *)self, BR_NULL_TOKEN, NULL, NULL);

	/*
	 * Remove resources
	 */
	BrResFreeNoCallback(self);
}

static br_token BR_CMETHOD_DECL(br_device_softprim, type)(br_device *self)
{
	return BRT_DEVICE;
}

static br_int_32 BR_CMETHOD_DECL(br_device_softprim, space)(br_device *self)
{
	return sizeof(br_device);
}

static br_boolean BR_CMETHOD_DECL(br_device_softprim, isType)(br_device *self, br_token t)
{
	return (t == BRT_DEVICE) || (t == BRT_OBJECT_CONTAINER) || (t == BRT_OBJECT);
}

static struct br_tv_template * BR_CMETHOD_DECL(br_device_softprim,templateQuery)
	(br_device *self)
{
    if(self->templates.deviceTemplate == NULL)
        self->templates.deviceTemplate = BrTVTemplateAllocate(self,
            deviceTemplateEntries,
            BR_ASIZE(deviceTemplateEntries));

    return self->templates.deviceTemplate;
}

static void * BR_CMETHOD_DECL(br_device_softprim,listQuery)(br_device *self)
{
	return self->object_list;
}

/*
 * Default dispatch table for device
 */
static const struct br_device_dispatch deviceDispatch = {
	NULL,
	NULL,
	NULL,
	NULL,
	BR_CMETHOD_REF(br_device_softprim, free),
	BR_CMETHOD_REF(br_object_softprim, identifier),
	BR_CMETHOD_REF(br_device_softprim, type),
	BR_CMETHOD_REF(br_device_softprim, isType),
	BR_CMETHOD_REF(br_object_softprim, device),
	BR_CMETHOD_REF(br_device_softprim, space),

	BR_CMETHOD_REF(br_device_softprim,	templateQuery),
	BR_CMETHOD_REF(br_object,			query),
	BR_CMETHOD_REF(br_object, 			queryBuffer),
	BR_CMETHOD_REF(br_object, 			queryMany),
	BR_CMETHOD_REF(br_object, 			queryManySize),
	BR_CMETHOD_REF(br_object, 			queryAll),
	BR_CMETHOD_REF(br_object, 			queryAllSize),

	BR_CMETHOD_REF(br_device_softprim,	listQuery),
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

