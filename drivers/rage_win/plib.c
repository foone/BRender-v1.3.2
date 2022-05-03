/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: plib.c 1.1 1997/12/10 16:50:30 jon Exp $
 * $Locker: $
 *
 * Primitive library methods for ATI 3D Expression
 * 
 */



#include <stddef.h>
#include <string.h>
#include <windows.h>
#include <ddraw.h>

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: plib.c 1.1 1997/12/10 16:50:30 jon Exp $");

/*
 * Default dispatch table for primitive library (defined at end of file)
 */
static struct br_primitive_library_dispatch primitiveLibraryDispatch;


// Renderer active/inactive flags, set up in match.c

extern br_boolean ATI_renderer_initialised;

/*
 * 0 terminated list of tokens that are used to represent the state for
 * this library
 */
br_token PrimPartsTokensATI[] = {
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
	{BRT(PARTS_TL),			(size_t)&PrimPartsTokensATI,		BRTV_QUERY | BRTV_ALL | BRTV_ABS,	BRTV_CONV_LIST },
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

/*
 * Set up a static device object
 */
br_error PrimitiveLibraryHardATIInitialise(struct br_primitive_library * self, struct br_device *dev)
{
  CLEAR_TEMPLATE(primitiveLibrary);

	self->dispatch = &primitiveLibraryDispatch;

	self->object_list = BrObjectListAllocate(DRIVER_RESOURCE);

   ObjectContainerAddFront(dev,(br_object*)self);
   
   return BRE_OK;
}

static void BR_CMETHOD_DECL(br_primitive_library_hardATI, free)(br_primitive_library *self)
{
   ObjectContainerRemove(ObjectDevice(self),(br_object*)self);
   BrObjectContainerFree((br_object_container*)self,BR_NULL_TOKEN,NULL,NULL);
}

static br_token BR_CMETHOD_DECL(br_primitive_library_hardATI, type)(br_primitive_library *self)
{
	return BRT_PRIMITIVE_LIBRARY;
}

static br_boolean BR_CMETHOD_DECL(br_primitive_library_hardATI, isType)(br_primitive_library *self, br_token t)
{
	return (t == BRT_PRIMITIVE_LIBRARY) || (t == BRT_OBJECT_CONTAINER) || (t == BRT_OBJECT);
}

static br_int_32 BR_CMETHOD_DECL(br_primitive_library_hardATI, space)(br_primitive_library *self)
{
	return sizeof(br_primitive_library);
}

static struct br_tv_template * BR_CMETHOD_DECL(br_primitive_library_hardATI,templateQuery)
	(br_primitive_library *self)
{
	primitiveLibraryTemplate.res = DRIVER_RESOURCE;
	return &primitiveLibraryTemplate;
}

static void * BR_CMETHOD_DECL(br_primitive_library_hardATI,listQuery)(br_primitive_library *self)
{
	return self->object_list;
}

static br_error BR_CMETHOD_DECL(br_primitive_library_hardATI, stateNew)(
		struct br_primitive_library *self,
		struct br_primitive_state **rps)
{
	struct br_primitive_state *ps;

	UASSERT(rps);

	ps = PrimitiveStateHardATIAllocate(self);

	if(ps == NULL)
		return BRE_FAIL;

	*rps = ps;

	return BRE_OK;
}

static br_error BR_CMETHOD_DECL(br_primitive_library_hardATI, bufferStoredNew)
	(struct br_primitive_library *self, struct br_buffer_stored **psm,
	br_token use, struct br_device_pixelmap *pm, br_token_value *tv)
{
	struct br_buffer_stored *sm;

	UASSERT(psm);

	sm = BufferStoredHardATIAllocate(self, use, pm ,tv);

	if(sm == NULL)
		return BRE_FAIL;

	*psm = sm;

	return BRE_OK;
}

static br_error BR_CMETHOD_DECL(br_primitive_library_hardATI, bufferStoredAvail)(
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

static br_error BR_CMETHOD_DECL(br_primitive_library_hardATI, flush)(
		struct br_primitive_library *self,
		br_boolean wait)
{
   // Reset current buffer pointers so we will update the output buffers
   // when we begin a new render
   extern br_device_pixelmap      *last_ati_depth;
   extern br_device_pixelmap      *last_ati_colour;
   extern struct br_buffer_stored *current_ati_tb;

   last_ati_depth = NULL;
   last_ati_colour = NULL;
   current_ati_tb = NULL;


#if 1
	/*
	 * Never buffers any work so no need to flush
	 */
   if(ATI_renderer_initialised == BR_TRUE)
      {
      ATI_renderer_initialised = BR_FALSE;
      ATI3DCIF_RenderEnd();
      }
#endif

#if 1
 /*
  * Unlock destination pixelmap, now rendering is complete.
  */
 if(self->colour_buffer){
   DevicePixelmapDirectUnlock( self->colour_buffer );

   /*
    *  Clear the primitive library colour buffer pointer to
    *  signify to br_primitive_state_soft::renderBegin that
    *  it must relock the pixelmap next time it is called -
    *  next time it begins to render a model / scene.
    */

    self->colour_buffer = NULL ;
  }
#endif


  return BRE_OK;
}

static br_error BR_CMETHOD_DECL(br_primitive_library_hardATI, synchronise)(
		struct br_primitive_library *self,
		br_token sync_type,
		br_boolean block)
{
	/*
	 * Never buffers any work so no need to synchronise
	 */
	return BRE_OK;
}

static br_error BR_CMETHOD_DECL(br_primitive_library_hardATI, mask)(
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
	BR_CMETHOD_REF(br_primitive_library_hardATI, free),
	BR_CMETHOD_REF(br_object_ati,                identifier),
	BR_CMETHOD_REF(br_primitive_library_hardATI, type),
	BR_CMETHOD_REF(br_primitive_library_hardATI, isType),
	BR_CMETHOD_REF(br_object_ati,                device),
	BR_CMETHOD_REF(br_primitive_library_hardATI, space),

	BR_CMETHOD_REF(br_primitive_library_hardATI,	templateQuery),
	BR_CMETHOD_REF(br_object,			            query),
	BR_CMETHOD_REF(br_object, 			            queryBuffer),
	BR_CMETHOD_REF(br_object, 			            queryMany),
	BR_CMETHOD_REF(br_object, 			            queryManySize),
	BR_CMETHOD_REF(br_object, 			            queryAll),
	BR_CMETHOD_REF(br_object, 			            queryAllSize),

	BR_CMETHOD_REF(br_primitive_library_hardATI,	listQuery),
	BR_CMETHOD_REF(br_object_container,	         tokensMatchBegin),
	BR_CMETHOD_REF(br_object_container,	         tokensMatch),
	BR_CMETHOD_REF(br_object_container,	         tokensMatchEnd),
	BR_CMETHOD_REF(br_object_container,	         addFront),
	BR_CMETHOD_REF(br_object_container,	         removeFront),
	BR_CMETHOD_REF(br_object_container,	         remove),
	BR_CMETHOD_REF(br_object_container,	         find),
	BR_CMETHOD_REF(br_object_container,          findMany),
	BR_CMETHOD_REF(br_object_container,          count),

	BR_CMETHOD_REF(br_primitive_library_hardATI,	stateNew),
	BR_CMETHOD_REF(br_primitive_library_hardATI,	bufferStoredNew),
	BR_CMETHOD_REF(br_primitive_library_hardATI,	bufferStoredAvail),
	BR_CMETHOD_REF(br_primitive_library_hardATI,	flush),
	BR_CMETHOD_REF(br_primitive_library_hardATI,	synchronise),
	BR_CMETHOD_REF(br_primitive_library_hardATI,	mask),
};

