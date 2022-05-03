/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: plib.c 1.1 1997/12/10 16:47:37 jon Exp $
 * $Locker: $
 *
 * Primtive library methods
 */
#include <stddef.h>
#include <string.h>

#include "drv.h"
#include "host.h"

#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: plib.c 1.1 1997/12/10 16:47:37 jon Exp $");

/*
 * Default dispatch table for primitive library (defined at end of file)
 */
static const struct br_primitive_library_dispatch primitiveLibraryDispatch;

/*
 * 0 terminated list of tokens that are used to represent the state for
 * this library
 */
const br_token PrimPartsTokens[] = {
	BRT_PRIMITIVE,
	BRT_OUTPUT,
	0
};

/*
 * Primitive library info. template
 */
#define F(f)	offsetof(br_primitive_library, f)
#define P(f)	((br_int_32)(&(f)))

static struct br_tv_template_entry primitiveLibraryTemplateEntries[] = {
	{BRT(IDENTIFIER_CSTR),	F(identifier),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(PARTS_TL),			P(PrimPartsTokens),	BRTV_QUERY | BRTV_ALL | BRTV_ABS,	BRTV_CONV_LIST },
	{BRT(PARTS_U32),		MASK_STATE_CACHE | MASK_STATE_OUTPUT | MASK_STATE_PRIMITIVE, BRTV_QUERY | BRTV_ABS,	BRTV_CONV_COPY },

	/*
	 * Useful 'Under the hood' hook
	 */
	{DEV(WORK_P),			P(work),           	BRTV_QUERY | BRTV_ABS,	BRTV_CONV_COPY, },
};
#undef F
#undef P

/*
 * Structure used to unpack  arguments
 */
#define F(f)	offsetof(struct br_primitive_library, f)

static struct br_tv_template_entry argsTemplateEntries[] = {
	{DEV(PROCESSOR_T),			F(processor_type),				BRTV_SET,	BRTV_CONV_COPY, },
	{DEV(USE_MMX_B),			F(use_mmx),						BRTV_SET,	BRTV_CONV_I32_B, },
};
#undef F

static struct br_tv_template argsTemplate = {
	BR_ASIZE(argsTemplateEntries),
	argsTemplateEntries
};

/*
 * Set up a static device object
 */
struct br_primitive_library * PrimitiveLibrarySoftAllocate(struct br_device *dev, char * identifier, char *arguments)
{
	struct br_primitive_library * self;
	struct host_info hostinfo;
	br_token_value args_tv[256];
	br_int_32 count;

	/*
	 * Generate some local tokens
	 */
	BrTokenCreate("PROCESSOR_T",		BRT_TOKEN);
	BrTokenCreate("USE_MMX_B",			BRT_BOOLEAN);
	BrTokenCreate("WORK_P",				BRT_POINTER);

	self = BrResAllocate(NULL, sizeof(*self), BR_MEMORY_OBJECT);

	self->dispatch = (struct br_primitive_library_dispatch *)&primitiveLibraryDispatch;
	self->identifier = identifier;
	self->device = dev;
	self->colour_buffer = NULL ;

	self->object_list = BrObjectListAllocate(dev);

	ObjectContainerAddFront(dev, (br_object *)self);

	if(HostInfo(&hostinfo,sizeof(struct host_info)) == BRE_OK) {
		self->processor_type = hostinfo.processor_type;
		self->use_mmx = (hostinfo.capabilities & HOST_CAPS_MMX) != 0;
	}

	/*
	 * Override settings from arguments
	 */

	if(arguments) {
		BrStringToTokenValue(args_tv, sizeof(args_tv), arguments);
		argsTemplate.res = self;
		BrTokenValueSetMany(self, &count, NULL, args_tv,&argsTemplate);
	}

	if (self->use_mmx)
		RasteriseBufferBegin();
	else
		RasteriseBufferDisable();

	return self;
}

static void BR_CMETHOD_DECL(br_primitive_library_soft, free)(br_primitive_library *self)
{
	/*
	 * Tidy up rasteriser buffer
	 */
	RasteriseBufferEnd();

	/*
	 * Remove attached objects
	 */
	BrObjectContainerFree((br_object_container *)self, BR_NULL_TOKEN, NULL, NULL);

    BrResFreeNoCallback(self);
}

static br_token BR_CMETHOD_DECL(br_primitive_library_soft, type)(br_primitive_library *self)
{
	return BRT_PRIMITIVE_LIBRARY;
}

static br_boolean BR_CMETHOD_DECL(br_primitive_library_soft, isType)(br_primitive_library *self, br_token t)
{
	return (t == BRT_PRIMITIVE_LIBRARY) || (t == BRT_OBJECT_CONTAINER) || (t == BRT_OBJECT);
}

static br_int_32 BR_CMETHOD_DECL(br_primitive_library_soft, space)(br_primitive_library *self)
{
	return sizeof(br_primitive_library);
}

static struct br_tv_template * BR_CMETHOD_DECL(br_primitive_library_soft,templateQuery)
	(br_primitive_library *self)
{
    if(self->device->templates.primitiveLibraryTemplate == NULL)
        self->device->templates.primitiveLibraryTemplate = BrTVTemplateAllocate(self->device,
            (br_tv_template_entry *)primitiveLibraryTemplateEntries,
			BR_ASIZE(primitiveLibraryTemplateEntries));

    return self->device->templates.primitiveLibraryTemplate;
}

static void * BR_CMETHOD_DECL(br_primitive_library_soft,listQuery)(br_primitive_library *self)
{
	return self->object_list;
}

static br_error BR_CMETHOD_DECL(br_primitive_library_soft, stateNew)(
		struct br_primitive_library *self,
		struct br_primitive_state **rps)
{
	struct br_primitive_state *ps;

	UASSERT(rps);

	ps = PrimitiveStateSoftAllocate(self);

	if(ps == NULL)
		return BRE_FAIL;

	*rps = ps;

	return BRE_OK;
}

static br_error BR_CMETHOD_DECL(br_primitive_library_soft, bufferStoredNew)
	(struct br_primitive_library *self, struct br_buffer_stored **psm,
	br_token use, struct br_device_pixelmap *pm, br_token_value *tv)
{
	struct br_buffer_stored *sm;

	UASSERT(psm);

	sm = BufferStoredSoftAllocate(self, use, pm ,tv);

	if(sm == NULL)
		return BRE_FAIL;

	*psm = sm;

	return BRE_OK;
}

static br_error BR_CMETHOD_DECL(br_primitive_library_soft, bufferStoredAvail)(
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

static br_error BR_CMETHOD_DECL(br_primitive_library_soft, flush)(
		struct br_primitive_library *self,
		br_boolean wait)
{

   ASSERT(self);

	RasteriseBufferFlush();

	/*
	 * Unlock destination pixelmap, now rendering is complete.
	 */
    if(self->colour_buffer){
	DevicePixelmapDirectUnlock( self->colour_buffer );

   /*
    *	 Clear the primitive library colour buffer pointer to
    *  signify to br_primitive_state_soft::renderBegin that
    *  it must relock the pixelmap next time it is called -
    *  next time it begins to render a model / scene. 
    */

       self->colour_buffer = NULL ;
   }
 
   return BRE_OK;
}

static br_error BR_CMETHOD_DECL(br_primitive_library_soft, synchronise)(
		struct br_primitive_library *self,
		br_token sync_type,
		br_boolean block)
{
	/*
	 * Never buffers up any work so no need to synchronise
	 */
	return BRE_OK;
}

static br_error BR_CMETHOD_DECL(br_primitive_library_soft, mask)(
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
static const struct br_primitive_library_dispatch primitiveLibraryDispatch = {
	NULL,
	NULL,
	NULL,
	NULL,
	BR_CMETHOD_REF(br_primitive_library_soft,   free),
	BR_CMETHOD_REF(br_object_softprim,          identifier),
	BR_CMETHOD_REF(br_primitive_library_soft,   type),
	BR_CMETHOD_REF(br_primitive_library_soft,   isType),
	BR_CMETHOD_REF(br_object_softprim,          device),
	BR_CMETHOD_REF(br_primitive_library_soft,   space),

	BR_CMETHOD_REF(br_primitive_library_soft,   templateQuery),
	BR_CMETHOD_REF(br_object,		        	query),
	BR_CMETHOD_REF(br_object, 	    		    queryBuffer),
	BR_CMETHOD_REF(br_object, 		    	    queryMany),
	BR_CMETHOD_REF(br_object,       			queryManySize),
	BR_CMETHOD_REF(br_object, 		    	    queryAll),
	BR_CMETHOD_REF(br_object, 			        queryAllSize),

	BR_CMETHOD_REF(br_primitive_library_soft,	listQuery),
	BR_CMETHOD_REF(br_object_container,	        tokensMatchBegin),
	BR_CMETHOD_REF(br_object_container,	        tokensMatch),
	BR_CMETHOD_REF(br_object_container,	        tokensMatchEnd),
	BR_CMETHOD_REF(br_object_container,	        addFront),
	BR_CMETHOD_REF(br_object_container,	        removeFront),
	BR_CMETHOD_REF(br_object_container,	        remove),
	BR_CMETHOD_REF(br_object_container,	        find),
	BR_CMETHOD_REF(br_object_container,         findMany),
	BR_CMETHOD_REF(br_object_container,         count),

	BR_CMETHOD_REF(br_primitive_library_soft,	stateNew),
	BR_CMETHOD_REF(br_primitive_library_soft,	bufferStoredNew),
	BR_CMETHOD_REF(br_primitive_library_soft,	bufferStoredAvail),
	BR_CMETHOD_REF(br_primitive_library_soft,	flush),
	BR_CMETHOD_REF(br_primitive_library_soft,	synchronise),
	BR_CMETHOD_REF(br_primitive_library_soft,	mask),
};

