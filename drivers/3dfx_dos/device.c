/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * Device methods
 */
#include <stddef.h>
#include <string.h>

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"




br_timestamp Prim3DfxDriverTimestamp = TIMESTAMP_START;

/*
 * Static device structure
 */
br_device DriverDevice3Dfx = {
#ifdef _WIN32
    NULL, "3dfx_win",
#else
    NULL, "3dfx_dos",
#endif
};


/*
 * Static primitive library
 */
br_primitive_library PrimitiveLibrary3Dfx = {
	NULL, "3Dfx-Primitives",
};


/*
 * Default dispatch table for device (defined at end of file)
 */
static struct br_device_dispatch deviceDispatch;

/*
 * Device info. template
 */
#define F(f)	offsetof(br_device, f)

static struct br_tv_template_entry deviceTemplateEntries[] = {
    {BRT_IDENTIFIER_CSTR,       0,  F(identifier),		BRTV_QUERY | BRTV_ALL,  BRTV_CONV_COPY, },
    {BRT_VERSION_U32,           0,  F(version),			BRTV_QUERY | BRTV_ALL,  BRTV_CONV_COPY, },
    {BRT_CREATOR_CSTR,          0,  F(creator),			BRTV_QUERY | BRTV_ALL,  BRTV_CONV_COPY, },
    {BRT_TITLE_CSTR,            0,  F(title),			BRTV_QUERY | BRTV_ALL,  BRTV_CONV_COPY, },
    {BRT_PRODUCT_CSTR,          0,  F(product),			BRTV_QUERY | BRTV_ALL,  BRTV_CONV_COPY, },
    {BRT_PRODUCT_VERSION_CSTR,  0,  F(product_version),	BRTV_QUERY | BRTV_ALL,  BRTV_CONV_COPY, },
    {BRT_CLUT_O,                0,  F(clut),			BRTV_QUERY | BRTV_ALL,  BRTV_CONV_COPY, },
};
#undef F

static struct br_tv_template deviceTemplate = {
	BR_ASIZE(deviceTemplateEntries),
	deviceTemplateEntries
};



/*
 * Set up a static device object
 */
br_error Device3DfxInitialise(br_device * self)
{
	/* Firstly clear out the template */
	CLEAR_TEMPLATE(device);

	/* Used to query device here but this has been moved in order to
	 * not respond if there wasn't a 3Dfx board out there */

	self->dispatch = &deviceDispatch;

	/*
	 * Set up resource anchor
	 */
	self->res = BrResAllocate(NULL, 0, BR_MEMORY_DRIVER);
    self->object_list = BrObjectListAllocate(self->res);

    /*
     * Set up information for queries
     */
    self->version           = BR_VERSION(1,1,2);
    self->creator           = "Volume 11 Software Development";
    self->title             = "3Dfx Voodoo Graphics Win95 Device Driver";
    self->product           = "3Dfx Voodoo Graphics";
    self->product_version   = "Voodoo Graphics Chipset/Glide 2.11";

	/*
	 * Build CLUT object
	 */
	self->clut = DeviceClut3DfxAllocate(self, "Pseudo-CLUT");

	return BRE_OK;
}

static void BR_CMETHOD_DECL(br_device_3dfx, free)(br_device *self)
{
	/*
	 * Remove attached objects
	 */
	BrObjectContainerFree((br_object_container *)self, BR_NULL_TOKEN, NULL, NULL);

	/*
	 * Throw away attached resources
	 */
	BrResFree(self->res);
	self->res = NULL;

	if (self->initialised) {
		grGlideShutdown();
		self->initialised = BR_FALSE;
	}
}

static br_token BR_CMETHOD_DECL(br_device_3dfx, type)(br_device *self)
{
	return BRT_DEVICE;
}

static br_boolean BR_CMETHOD_DECL(br_device_3dfx, isType)(br_device *self, br_token t)
{
	return (t == BRT_DEVICE) || (t == BRT_OBJECT_CONTAINER) || (t == BRT_OBJECT);
}

static br_int_32 BR_CMETHOD_DECL(br_device_3dfx, space)(br_device *self)
{
	return sizeof(br_device);
}

static struct br_tv_template * BR_CMETHOD_DECL(br_device_3dfx,templateQuery)
	(br_device *self)
{
	deviceTemplate.res = self->res;
	return &deviceTemplate;
}

static void * BR_CMETHOD_DECL(br_device_3dfx,listQuery)(br_device *self)
{
	return self->object_list;
}




/*
 * Main entry point for device - this may get redefined by the makefile
 */
#ifdef _WIN32
__declspec (dllexport)
#endif
br_device * BR_EXPORT BrDrv1Begin(char *arguments)
{
	int type_count;
	br_device *device = (br_device *)&DriverDevice3Dfx;
	br_error r;

	/*
	 * Set up timestamp
	 */
	Prim3DfxDriverTimestamp = TIMESTAMP_START;

	/*
	 * Set up device
	 */
	type_count  = 0;

	if(Device3DfxInitialise((br_device *)device) == BRE_OK) {

		/*
		 * Set up the primitive library
		 */
		r = PrimitiveLibrary3DfxInitialise(&PrimitiveLibrary3Dfx, device);
		if(r != BRE_OK)
			return NULL;

		/*
		 * Setup all the built in types
		 */
		type_count = OutputFacility3DfxInitialise(device, &PrimitiveLibrary3Dfx);

		/*
		 * If nothing is available, then don't admit to being a device
		 */
		if(type_count == 0)
			return NULL;
		else
			return device;
	} else
		return NULL;
}




/*
 * Local methods for default object object
 */


/*
 * Get identifier associated with a object
 */
char * BR_CMETHOD_DECL(br_object_3dfx, identifier)(br_object *self)
{
	return self->identifier;
}

/*
 * Find the device assocaited with a object
 */
br_device *	BR_CMETHOD_DECL(br_object_3dfx, device)(br_object *self)
{
	return (br_device *)&DriverDevice3Dfx;
}


/*
 * List of tokens which are not significant in matching (for output facilities)
 */
static br_token insignificantMatchTokens[] = {

	BRT(WINDOW_HANDLE_H),
	BR_NULL_TOKEN
};


/*
 * Default token matching does nothing other than make all tokens match
 *
 * makes a copy of token/value list
 */
struct token_match {
	br_token_value *original;
	br_token_value *query;	
	br_int_32 n;
	void *extra;
	br_size_t extra_size;
};


void * BR_CMETHOD_DECL(br_device_3dfx, tokensMatchBegin)
		(struct br_device *self, br_token t, br_token_value *tv)
{
	struct token_match *tm;
	br_int_32 i;

	if(tv == NULL)
		return NULL;

	tm = BrResAllocate(self->res, sizeof(*tm), BR_MEMORY_APPLICATION);
	tm->original = tv;

	for(i=0; tv[i].t != BR_NULL_TOKEN; i++)
		;

	tm->n = i+1;
	tm->query =	BrResAllocate(tm, tm->n * sizeof(br_token_value), BR_MEMORY_APPLICATION);
	BrMemCpy(tm->query,tv, i * sizeof(br_token_value));
	return (void *) tm;
}

br_boolean BR_CMETHOD_DECL(br_device_3dfx, tokensMatch)
		(struct br_object_container *self, br_object *h, void *arg)
{
	struct token_match *tm = arg;
	br_size_t s;
	br_int_32 n;

	if(arg == NULL)
		return BR_TRUE;

	/*
	 * Make a query on the object and then compare with the original tokens
	 */
	ObjectQueryManySize(h, &s,tm->query);

	if(s > tm->extra_size) {
		if(tm->extra)
			BrResFree(tm->extra);
		tm->extra =	BrResAllocate(tm, s, BR_MEMORY_APPLICATION);
		tm->extra_size = s;
	}
	
	ObjectQueryMany(h, tm->query, tm->extra, tm->extra_size, &n);

	/*
	 * Ensure that all tokens were found
	 */
	if (tm->query[n].t != BR_NULL_TOKEN)
		return BR_FALSE;

	/*
	 * Compare the two token lists
	 */
	return BrTokenValueComparePartial(tm->original, tm->query, insignificantMatchTokens);
}

void BR_CMETHOD_DECL(br_device_3dfx, tokensMatchEnd)
		(struct br_object_container *self, void *arg)
{
	if(arg)
		BrResFree(arg);
}


/*
 * Default dispatch table for device
 */
static struct br_device_dispatch deviceDispatch = {
	NULL,
	NULL,
	NULL,
	NULL,
	BR_CMETHOD_REF(br_device_3dfx,		free),
	BR_CMETHOD_REF(br_object_3dfx,		identifier),
	BR_CMETHOD_REF(br_device_3dfx,		type),
	BR_CMETHOD_REF(br_device_3dfx,		isType),
	BR_CMETHOD_REF(br_object_3dfx,		device),
	BR_CMETHOD_REF(br_device_3dfx,		space),

	BR_CMETHOD_REF(br_device_3dfx,		templateQuery),
	BR_CMETHOD_REF(br_object,			query),
	BR_CMETHOD_REF(br_object, 			queryBuffer),
	BR_CMETHOD_REF(br_object, 			queryMany),
	BR_CMETHOD_REF(br_object, 			queryManySize),
	BR_CMETHOD_REF(br_object, 			queryAll),
	BR_CMETHOD_REF(br_object, 			queryAllSize),

	BR_CMETHOD_REF(br_device_3dfx,		listQuery),
	BR_CMETHOD_REF(br_device_3dfx,		tokensMatchBegin),
	BR_CMETHOD_REF(br_device_3dfx,		tokensMatch),
	BR_CMETHOD_REF(br_device_3dfx,		tokensMatchEnd),
	BR_CMETHOD_REF(br_object_container,	addFront),
	BR_CMETHOD_REF(br_object_container,	removeFront),
	BR_CMETHOD_REF(br_object_container,	remove),
	BR_CMETHOD_REF(br_object_container,	find),
	BR_CMETHOD_REF(br_object_container, findMany),
	BR_CMETHOD_REF(br_object_container, count),
};

