/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: plib.c 1.1 1997/12/10 16:51:13 jon Exp $
 * $Locker: $
 *
 * Primitive library methods for S3
 * 
 */



#include <stddef.h>
#include <string.h>
#include <windows.h>
#include <ddraw.h>

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: plib.c 1.1 1997/12/10 16:51:13 jon Exp $");

/*
 * Default dispatch table for primitive library (defined at end of file)
 */
static struct br_primitive_library_dispatch primitiveLibraryDispatch;


// Renderer active/inactive flag

extern br_boolean S3RendererInitialised;


/*
 * 0 terminated list of tokens that are used to represent the state for
 * this library
 */
br_token PrimPartsTokensS3[] = {
	BRT_PRIMITIVE,
	BRT_OUTPUT,
	0
};

/*
 * Primitive library info. template
 */
#define F(f)	offsetof(br_primitive_library, f)

static struct br_tv_template_entry primitiveLibraryTemplateEntries[] = {
	{BRT(IDENTIFIER_CSTR),	F(identifier),		   BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(PARTS_TL),			(size_t)&PrimPartsTokensS3,		BRTV_QUERY | BRTV_ALL | BRTV_ABS,	BRTV_CONV_LIST },
	{BRT(PARTS_U32),		   MASK_STATE_CACHE | MASK_STATE_OUTPUT | MASK_STATE_PRIMITIVE, BRTV_QUERY | BRTV_ABS,	BRTV_CONV_COPY },

#if DEBUG
	{DEV(NO_CHANGES_B),		F(no_changes),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY},
#endif
};
#undef F

static struct br_tv_template primitiveLibraryTemplate = {
	BR_ASIZE(primitiveLibraryTemplateEntries),
	primitiveLibraryTemplateEntries
};

void ClearPlibTemplates(void)
{
	CLEAR_TEMPLATE(primitiveLibrary);
}
/*
 * Set up a static device object
 */
br_error PrimitiveLibraryHardS3Initialise(struct br_primitive_library * self, struct br_device *dev)
{
	self->dispatch = &primitiveLibraryDispatch;

	self->object_list = BrObjectListAllocate(DRIVER_RESOURCE);

   ObjectContainerAddFront(dev,(br_object*)self);

	return BRE_OK;
}

static void BR_CMETHOD_DECL(br_primitive_library_hardS3, free)(br_primitive_library *self)
{
   ObjectContainerRemove(ObjectDevice(self),(br_object*)self);
   BrObjectContainerFree((br_object_container*)self,BR_NULL_TOKEN,NULL,NULL);
}

static br_token BR_CMETHOD_DECL(br_primitive_library_hardS3, type)(br_primitive_library *self)
{
	return BRT_PRIMITIVE_LIBRARY;
}

static br_boolean BR_CMETHOD_DECL(br_primitive_library_hardS3, isType)(br_primitive_library *self, br_token t)
{
	return (t == BRT_PRIMITIVE_LIBRARY) || (t == BRT_OBJECT_CONTAINER) || (t == BRT_OBJECT);
}

static br_int_32 BR_CMETHOD_DECL(br_primitive_library_hardS3, space)(br_primitive_library *self)
{
	return sizeof(br_primitive_library);
}

static struct br_tv_template * BR_CMETHOD_DECL(br_primitive_library_hardS3,templateQuery)
	(br_primitive_library *self)
{
	primitiveLibraryTemplate.res = DRIVER_RESOURCE;
	return &primitiveLibraryTemplate;
}

static void * BR_CMETHOD_DECL(br_primitive_library_hardS3,listQuery)(br_primitive_library *self)
{
	return self->object_list;
}

static br_error BR_CMETHOD_DECL(br_primitive_library_hardS3, stateNew)(
		struct br_primitive_library *self,
		struct br_primitive_state **rps)
{
	struct br_primitive_state *ps;

	UASSERT(rps);

	ps = PrimitiveStateHardS3Allocate(self);

	if(ps == NULL)
		return BRE_FAIL;

	*rps = ps;

	return BRE_OK;
}

static br_error BR_CMETHOD_DECL(br_primitive_library_hardS3, bufferStoredNew)
	(struct br_primitive_library *self, struct br_buffer_stored **psm,
	br_token use, struct br_device_pixelmap *pm, br_token_value *tv)
{
	struct br_buffer_stored *sm;

	UASSERT(psm);

	sm = BufferStoredHardS3Allocate(self, use, pm ,tv);

	if(sm == NULL)
		return BRE_FAIL;

	*psm = sm;

	return BRE_OK;
}

static br_error BR_CMETHOD_DECL(br_primitive_library_hardS3, bufferStoredAvail)(
		struct br_primitive_library *self,
		br_int_32 *space,
		br_token use,
		br_token_value *tv)
{
	/*
	 * Should return free system memory
	 */
	return BRE_FAIL;
}


/* Called at the end of rendering */

static br_error BR_CMETHOD_DECL(br_primitive_library_hardS3, flush)(
		struct br_primitive_library *self,
		br_boolean wait)
{
   // Reset current buffer pointers so we will update the output buffers
   // when we begin a new render
   extern br_device_pixelmap      *last_depth;
   extern br_device_pixelmap      *last_colour;
	extern struct br_buffer_stored *current_tb;

   last_depth = NULL;
   last_colour = NULL;
   current_tb = NULL;

	return BRE_OK;
}

static br_error BR_CMETHOD_DECL(br_primitive_library_hardS3, synchronise)(
		struct br_primitive_library *self,
		br_token sync_type,
		br_boolean block)
{
	/*
	 * Never buffers any work so no need to synchronise
	 */
	return BRE_OK;
}

static br_error BR_CMETHOD_DECL(br_primitive_library_hardS3, mask)(
		struct br_primitive_library *self,
		br_uint_32 *mask,
		br_token *parts,
		int n_parts)
{
	int i;
	br_uint_32 m = 0;

	for(i=0; i < n_parts; i++) {
		switch(parts[i]) {
		case BRT_PRIMITIVE:
			m |= MASK_STATE_PRIMITIVE;
			break;

		case BRT_OUTPUT:
			m |= MASK_STATE_OUTPUT;
			break;
		}
	}

	*mask = m;

	return BRE_OK;
}

/*
 * Default dispatch table for device
 */
static struct br_primitive_library_dispatch primitiveLibraryDispatch = {
   NULL,NULL,NULL,NULL,
	BR_CMETHOD_REF(br_primitive_library_hardS3, free),
	BR_CMETHOD_REF(br_object_s3, identifier),
	BR_CMETHOD_REF(br_primitive_library_hardS3, type),
	BR_CMETHOD_REF(br_primitive_library_hardS3, isType),
	BR_CMETHOD_REF(br_object_s3, device),
	BR_CMETHOD_REF(br_primitive_library_hardS3, space),

	BR_CMETHOD_REF(br_primitive_library_hardS3,		templateQuery),
	BR_CMETHOD_REF(br_object,			query),
	BR_CMETHOD_REF(br_object, 			queryBuffer),
	BR_CMETHOD_REF(br_object, 			queryMany),
	BR_CMETHOD_REF(br_object, 			queryManySize),
	BR_CMETHOD_REF(br_object, 			queryAll),
	BR_CMETHOD_REF(br_object, 			queryAllSize),

	BR_CMETHOD_REF(br_primitive_library_hardS3,		listQuery),
	BR_CMETHOD_REF(br_object_container,	tokensMatchBegin),
	BR_CMETHOD_REF(br_object_container,	tokensMatch),
	BR_CMETHOD_REF(br_object_container,	tokensMatchEnd),
	BR_CMETHOD_REF(br_object_container,	addFront),
	BR_CMETHOD_REF(br_object_container,	removeFront),
	BR_CMETHOD_REF(br_object_container,	remove),
	BR_CMETHOD_REF(br_object_container,	find),
	BR_CMETHOD_REF(br_object_container, findMany),
	BR_CMETHOD_REF(br_object_container, count),

	BR_CMETHOD_REF(br_primitive_library_hardS3,		stateNew),
	BR_CMETHOD_REF(br_primitive_library_hardS3,		bufferStoredNew),
	BR_CMETHOD_REF(br_primitive_library_hardS3,		bufferStoredAvail),
	BR_CMETHOD_REF(br_primitive_library_hardS3,		flush),
	BR_CMETHOD_REF(br_primitive_library_hardS3,		synchronise),
	BR_CMETHOD_REF(br_primitive_library_hardS3,		mask),
};

