/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: device.c 1.1 1997/12/10 16:45:08 jon Exp $
 * $Locker: $
 *
 * Device methods
 */
#include <stddef.h>
#include <string.h>
#include <windows.h>
#include <ddraw.h>

#include "drv.h"
#include "brassert.h"


BR_RCS_ID("$Id: device.c 1.1 1997/12/10 16:45:08 jon Exp $");

#define DEVICE_VERSION  BR_VERSION(1,0,0)


/*
 * Default dispatch table for device (defined at end of file)
 */
static const struct br_device_dispatch deviceDispatch;

/*
 * Device info. template
 */
 static const char deviceTitle[] = "DirectDraw Software Only";

 static const char deviceCreator[] = "Argonaut Technologies Ltd.";
 static const char deviceProduct[] = "DirectDraw";



/*
 * Device info. template
 */
#define _F(f)	offsetof(br_device, f)
#define _A(f)    ((br_int_32)(f))

static struct br_tv_template_entry deviceTemplateEntries[] = {
	{BRT_IDENTIFIER_CSTR,	0,	_F(identifier),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
   {BRT_VERSION_U32,           0,	0,  		            BRTV_QUERY | BRTV_ALL,	BRTV_CONV_DIRECT, DEVICE_VERSION},
   {BRT_BRENDER_VERSION_U32,   0,	0,	    	            BRTV_QUERY | BRTV_ALL,	BRTV_CONV_DIRECT, __BRENDER__},
   {BRT_DDI_VERSION_U32,       0,	0,	                    BRTV_QUERY | BRTV_ALL,	BRTV_CONV_DIRECT, __BRENDER_DDI__},
   {BRT_CREATOR_CSTR,          0,	_A(deviceCreator),	    BRTV_QUERY | BRTV_ALL | BRTV_ABS,	BRTV_CONV_COPY, },
   {BRT_TITLE_CSTR,            0,	_A(deviceTitle),	    BRTV_QUERY | BRTV_ALL | BRTV_ABS,	BRTV_CONV_COPY, },
   {BRT_PRODUCT_CSTR,          0,	_A(deviceProduct),	    BRTV_QUERY | BRTV_ALL | BRTV_ABS,	BRTV_CONV_COPY, },
};
#undef _F
#undef _A


static struct br_tv_template deviceTemplate = {
	BR_ASIZE(deviceTemplateEntries),
	deviceTemplateEntries
};


/*
 * List of tokens which are not significant in matching (for output facilities)
 */
static br_token insignificantMatchTokens[] = {

	BRT(WINDOW_HANDLE_H),
	BR_NULL_TOKEN
};


/*
 * Structure used to unpack driver arguments
 */
#define F(f)	offsetof(struct br_device, f)

static struct br_tv_template_entry deviceArgsTemplateEntries[] = {
	{BRT(WINDOW_HANDLE_H),	F(window),						BRTV_SET,	BRTV_CONV_COPY, },
};
#undef F

static struct br_tv_template deviceArgsTemplate = {
	BR_ASIZE(deviceArgsTemplateEntries),
	deviceArgsTemplateEntries
};


/*
 * Set up a static device object
 */
br_device *DeviceDirectDrawAllocate(char *id, char *arguments)
{
	br_device *self;
	br_token_value args_tv[256];
	br_uint_32 count;
	LPDIRECTDRAW ddraw;

	/*
	 * Set up device block and resource anchor
	 */

	self = BrResAllocate(NULL, sizeof(*self), BR_MEMORY_OBJECT );
	self->res = BrResAllocate(self, 0, BR_MEMORY_DRIVER);

	self->identifier = BrResStrDup( self, id);

	/*
	 * Parse any initialisation arguments
	 */
	if (arguments != NULL) {

		BrStringToTokenValue(args_tv, sizeof(args_tv), arguments);
		deviceArgsTemplate.res = self->res;
		BrTokenValueSetMany(self, &count, NULL, args_tv, &deviceArgsTemplate);
	}

	/*
	 * Attach dispatch table
	 */
	self->dispatch = &deviceDispatch;
	
	/*
	 * Set up object list
	 */
	self->object_list = BrObjectListAllocate(self->res);

	/*
	 * Load DirectDraw module
	 */
	if((self->hddraw = LoadLibrary("DDRAW")) == NULL)
	{
		BR_ERROR0("LoadLibrary DDRAW.DLL failed.\n");
		return NULL ;	
	}

	/*
	 * Locate DirectDraw entry point
	 */
	if((self->create = GetProcAddress(self->hddraw, "DirectDrawCreate")) == NULL)
	{
	    BR_ERROR0("GetProcAddress DirectDrawCreate failed.\n");
		return NULL ;
	}

	/*
	 * Create a DirectDraw object
	 */
	if(self->create(NULL, &ddraw, NULL) != DD_OK)
	{
        BR_ERROR0("DirectDrawCreate failed.\n");
		return NULL ;
	}

	if (IDirectDraw_QueryInterface(ddraw, &IID_IDirectDraw2, &self->DD) != DD_OK) {
		IDirectDraw_Release(ddraw);
        BR_ERROR0("DirectDrawCreate failed.\n");
		return NULL ;
	}

	IDirectDraw_Release(ddraw);

	/*
	 * Create a device-wide critical section object
	 */
	InitializeCriticalSection(&self->csection);

	return self ;
}

static void BR_CMETHOD_DECL(br_device_dd, free)\
	(br_device *self)
{
	/*
	 * Release resources used by critical section object
	 */
	DeleteCriticalSection(&self->csection);

	/*
	 * Release DirectDraw object
	 */
	IDirectDraw2_Release(self->DD);

	/*
	 * Remove attached objects
	 */
	ObjectContainerFree((br_object_container *)self, BR_NULL_TOKEN, NULL, NULL);

	/*
	 * Throw away attached resources
	 */
	BrResFreeNoCallback(self);
}

static br_token BR_CMETHOD_DECL(br_device_dd, type)\
	(br_device *self)
{
	return BRT_DEVICE;
}

static br_boolean BR_CMETHOD_DECL(br_device_dd, isType)\
	(br_device *self, br_token t)
{
	return (t == BRT_DEVICE) || (t == BRT_OBJECT_CONTAINER) || (t == BRT_OBJECT);
}

static br_int_32 BR_CMETHOD_DECL(br_device_dd, space)\
	(br_device *self)
{
	return sizeof(br_device);
}

static struct br_tv_template * BR_CMETHOD_DECL(br_device_dd, templateQuery)\
	(br_device *self)
{
    if(self->templates.deviceTemplate == NULL)
        self->templates.deviceTemplate = BrTVTemplateAllocate(self->res,
            deviceTemplateEntries,
            BR_ASIZE(deviceTemplateEntries));

    return self->templates.deviceTemplate;
}

static void * BR_CMETHOD_DECL(br_device_dd, listQuery)\
	(br_device *self)
{
	return self->object_list;
}

/*
 * Default token matching does nothing other than make all tokens match
 *
 * makes a copy of token/value list
 */
static struct token_match {
	br_token_value *original;
	br_token_value *query;	
	br_int_32 n;
	void *extra;
	br_size_t extra_size;
};


static void * BR_CMETHOD_DECL(br_device_dd, tokensMatchBegin)
		(struct br_object_container *self, br_token t, br_token_value *tv)
{
	struct token_match *tm;
	br_int_32 i;

	if(tv == NULL)
		return NULL;

	tm = BrResAllocate(self, sizeof(*tm), BR_MEMORY_APPLICATION);
	tm->original = tv;

	for(i=0; tv[i].t != BR_NULL_TOKEN; i++)
		;

	tm->n = i+1;
	tm->query =	BrResAllocate(tm, tm->n * sizeof(br_token_value), BR_MEMORY_APPLICATION);
	BrMemCpy(tm->query,tv, i * sizeof(br_token_value));
	return (void *) tm;
}

static br_boolean BR_CMETHOD_DECL(br_device_dd, tokensMatch)
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

static void BR_CMETHOD_DECL(br_device_dd, tokensMatchEnd)
		(struct br_object_container *self, void *arg)
{
	if(arg)
		BrResFree(arg);
}


/*
 * Default dispatch table for device
 */
static const struct br_device_dispatch deviceDispatch = {
	NULL,
	NULL,
	NULL,
	NULL,
	BR_CMETHOD_REF(br_device_dd,		free),
	BR_CMETHOD_REF(br_object_dd,		identifier),
	BR_CMETHOD_REF(br_device_dd,		type),
	BR_CMETHOD_REF(br_device_dd,		isType),
	BR_CMETHOD_REF(br_object_dd,		device),
	BR_CMETHOD_REF(br_device_dd,		space),

	BR_CMETHOD_REF(br_device_dd,		templateQuery),
	BR_CMETHOD_REF(br_object,			query),
	BR_CMETHOD_REF(br_object, 			queryBuffer),
	BR_CMETHOD_REF(br_object, 			queryMany),
	BR_CMETHOD_REF(br_object, 			queryManySize),
	BR_CMETHOD_REF(br_object, 			queryAll),
	BR_CMETHOD_REF(br_object, 			queryAllSize),

	BR_CMETHOD_REF(br_device_dd,		listQuery),
	BR_CMETHOD_REF(br_device_dd,		tokensMatchBegin),
	BR_CMETHOD_REF(br_device_dd,		tokensMatch),
	BR_CMETHOD_REF(br_device_dd,		tokensMatchEnd),
	BR_CMETHOD_REF(br_object_container,	addFront),
	BR_CMETHOD_REF(br_object_container,	removeFront),
	BR_CMETHOD_REF(br_object_container,	remove),
	BR_CMETHOD_REF(br_object_container,	find),
	BR_CMETHOD_REF(br_object_container, findMany),
	BR_CMETHOD_REF(br_object_container, count),
};


