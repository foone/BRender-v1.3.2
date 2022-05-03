/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: device.c 1.1 1997/12/10 16:50:50 jon Exp $
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


BR_RCS_ID("$Id: device.c 1.1 1997/12/10 16:50:50 jon Exp $");


br_primitive_library PrimitiveLibraryHardS3 = {
    NULL, "Default-Primitives",
};

/* Flag to cover reinitialisation of driver when it hasn't been shut down */
static br_boolean S3_device_active = BR_FALSE;

br_timestamp S3DriverTimestamp = TIMESTAMP_START;

LRESULT CALLBACK ThreadWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

/*
 * Default dispatch table for device (defined at end of file)
 */
static struct br_device_dispatch deviceDispatch;

/*
 * Device info. template
 */
#define F(f)	offsetof(br_device, f)

static struct br_tv_template_entry deviceTemplateEntries[] = {
	{BRT_IDENTIFIER_CSTR,	0,	F(identifier),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
   {BRT_VERSION_U32,           0,  F(version),     BRTV_QUERY | BRTV_ALL,  BRTV_CONV_COPY, },
   {BRT_CREATOR_CSTR,          0,  F(creator),     BRTV_QUERY | BRTV_ALL,  BRTV_CONV_COPY, },
   {BRT_TITLE_CSTR,            0,  F(title),       BRTV_QUERY | BRTV_ALL,  BRTV_CONV_COPY, },
   {BRT_PRODUCT_CSTR,          0,  F(product),     BRTV_QUERY | BRTV_ALL,  BRTV_CONV_COPY, },
   {BRT_PRODUCT_VERSION_CSTR,  0,  F(product_version),BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY, },
	{BRT_CLUT_O,			0,	F(clut),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
};
#undef F

static struct br_tv_template deviceTemplate = {
	BR_ASIZE(deviceTemplateEntries),
	deviceTemplateEntries
};


void ClearDeviceTemplates(void)
{
	CLEAR_TEMPLATE(device);
}

/*
 * Structure used to query operating system version
 */
static OSVERSIONINFO version_info;

/*
 * Global DirectDraw information
 */
static FARPROC DirectDrawCreate_P;
static HMODULE hddraw;
LPDIRECTDRAW gpDD;


/*
 * Set up a static device object
 */
br_error DeviceS3Initialise(br_device *self)
{
	static void FreeOldDevice(br_device *thedevice);

	UASSERT(self != NULL);

	/* If reinitialising the device without freeing it then free it explicitly */
	if(S3_device_active)
		FreeOldDevice(self);

	/*
	 * Open s3dtk.dll
	 */
	if (self->s3dtk == NULL)
		self->s3dtk = LoadLibrary("s3dtkw");

	if (self->s3dtk == NULL)
		return BRE_FAIL;

	/*
	 * Look up some DirectDraw global functions
	 */
	self->S3DTK_InitLib = (ULONG (FAR *)(ULONG lParam))GetProcAddress(self->s3dtk, "S3DTK_InitLib");
	self->S3DTK_ExitLib = (ULONG (FAR *)(void))GetProcAddress(self->s3dtk, "S3DTK_ExitLib");
	self->S3DTK_CreateRenderer = (ULONG (FAR *)(ULONG Param1, void **ppFunctionList))GetProcAddress(self->s3dtk, "S3DTK_CreateRenderer");
	self->S3DTK_DestroyRenderer = (ULONG (FAR *)(void **ppFunctionList))GetProcAddress(self->s3dtk, "S3DTK_DestroyRenderer");
	
	if (self->S3DTK_InitLib == NULL ||
		self->S3DTK_ExitLib == NULL ||
		self->S3DTK_CreateRenderer == NULL ||
		self->S3DTK_DestroyRenderer == NULL) {

		FreeLibrary(self->s3dtk);
		self->s3dtk = NULL;
		return BRE_FAIL;
	}

	S3DriverTimestamp = TIMESTAMP_START;

	/* Attach dispatch table */
	self->dispatch = &deviceDispatch;
	
	/* Check operating system and version information */
	version_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if(GetVersionEx(&version_info) == FALSE) {

		FreeLibrary(self->s3dtk);
		self->s3dtk = NULL;
		return BRE_FAIL;
	}

	if(version_info.dwPlatformId == VER_PLATFORM_WIN32s) {

		FreeLibrary(self->s3dtk);
		self->s3dtk = NULL;
		return BRE_UNSUPPORTED;
	}

	/* Create resource anchor */
	self->res = BrResAllocate(NULL, 0, BR_MEMORY_DRIVER);

	/* Set up object list */
	self->object_list = BrObjectListAllocate(self->res);

	/* Load the DirectDraw module */
	hddraw = LoadLibrary("DDRAW");
	if(hddraw == NULL) {

		FreeLibrary(self->s3dtk);
		self->s3dtk = NULL;
		return BRE_FAIL;
	}

	/* Get address of the DirectDrawCreate function */
	DirectDrawCreate_P = GetProcAddress(hddraw, "DirectDrawCreate");
	if(DirectDrawCreate_P == NULL) {

		FreeLibrary(hddraw);
		hddraw = NULL;
		FreeLibrary(self->s3dtk);
		self->s3dtk = NULL;
		return BRE_FAIL;
	}

	/* Create Direct Draw */
	if (DirectDrawCreate_P(NULL, &gpDD, NULL) != DD_OK) {

		FreeLibrary(hddraw);
		hddraw = NULL;
		FreeLibrary(self->s3dtk);
		self->s3dtk = NULL;
		return BRE_FAIL;
	}

	/* There is no device-wide CLUT as yet */
	self->clut = NULL;

	/* Clear buffer method availability flags */
	self->buffer_method_directdraw = BR_FALSE;

	/* Create a device-wide critical section object */
	InitializeCriticalSection(&self->csection);

	/* Setup the primitive library */
	if(PrimitiveLibraryHardS3Initialise(&PrimitiveLibraryHardS3,self) != BRE_OK) {

		IDirectDraw_Release(gpDD);
		gpDD = NULL;
		FreeLibrary(hddraw);
		hddraw = NULL;
		FreeLibrary(self->s3dtk);
		self->s3dtk = NULL;
		return BRE_FAIL;
	}

   /* Set up information for queries */
   self->version           = BR_VERSION(1,0,1);
   self->creator           = "Volume 11 Software Development";
   self->title             = "S3 ViRGE Win 95 Device Driver";
   self->product           = "S3 ViRGE";
   self->product_version   = "S3 ViRGE / ViRGE DX / ViRGE GX / ViRGE GX2";

   S3_device_active = BR_TRUE;

	return BRE_OK;
}


/* This routine reinitialises the template entries for all the segments of the
 * device back to their initial states when the device is freed */

static void ClearAllTemplates(void)
{
	void ClearDevclutTemplates(void);
	void ClearDeviceTemplates(void);
	void ClearDevpmFrontTemplates(void);
	void ClearDevpmBackTemplates(void);
	void ClearOutfctyTemplates(void);
	void ClearPlibTemplates(void);
	void ClearPstateTemplates(void);
	void ClearSbufferTemplates(void);

	ClearDevclutTemplates();
	ClearDeviceTemplates();
	ClearDevpmFrontTemplates();
	ClearDevpmBackTemplates();
	ClearOutfctyTemplates();
	ClearPlibTemplates();
	ClearPstateTemplates();
	ClearSbufferTemplates();
}



static void BR_CMETHOD_DECL(br_device_s3, free)\
	(br_device *self)
{
	/* Shut down S3 libraries (in case they haven't already been shut down) */
    S3TerminateLibrary();

	/* Release resources used by critical section object */
	DeleteCriticalSection(&self->csection);

	/* Remove attached objects */
	ObjectContainerFree((br_object_container *)self, BR_NULL_TOKEN, NULL, NULL);

	/* Throw away attached resources */
	BrResFree(self->res);
	self->res = NULL;

	/* Shut down of DirectDraw MUST be done _after_ ObjectContainerFree
	 * Direct draw must still be valid in case we need to free pixelmaps */
	IDirectDraw_Release(gpDD);
	gpDD = NULL;

	/* Reset the values of the templates in the device */
	ClearAllTemplates();

	if (hddraw != NULL) {
		FreeLibrary(self->s3dtk);
		hddraw = NULL;
	}

	/*
	 * Close s3dtk.dll
	 */
	if (self->s3dtk != NULL) {
		FreeLibrary(self->s3dtk);
		self->s3dtk = NULL;
	}

	S3_device_active = BR_FALSE;
}


static void FreeOldDevice(br_device *thedevice)
{
	BR_CMETHOD(br_device_s3,free)(thedevice);
}

static br_token BR_CMETHOD_DECL(br_device_s3, type)\
	(br_device *self)
{
	return BRT_DEVICE;
}

static br_boolean BR_CMETHOD_DECL(br_device_s3, isType)\
	(br_device *self, br_token t)
{
	return (t == BRT_DEVICE) || (t == BRT_OBJECT_CONTAINER) || (t == BRT_OBJECT);
}

static br_int_32 BR_CMETHOD_DECL(br_device_s3, space)\
	(br_device *self)
{
	return sizeof(br_device);
}

static struct br_tv_template * BR_CMETHOD_DECL(br_device_s3, templateQuery)\
	(br_device *self)
{
	deviceTemplate.res = self->res;
	return &deviceTemplate;
}

static void * BR_CMETHOD_DECL(br_device_s3, listQuery)\
	(br_device *self)
{
	return self->object_list;
}










/* List of tokens which are not significant in matching (for output facilities) */

static br_token insignificantMatchTokens[] =
{
	BRT(WINDOW_HANDLE_H),
	BR_NULL_TOKEN
};


/*
 * Default token matching does nothing other than make all tokens match
 *
 * makes a copy of token/value list
 */

struct token_match
{
	br_token_value *original;
	br_token_value *query;	
	br_int_32 n;
	void *extra;
	br_size_t extra_size;
};









void * BR_CMETHOD_DECL(br_device_s3, tokensMatchBegin)
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



br_boolean BR_CMETHOD_DECL(br_device_s3, tokensMatch)
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

	/* Ensure that all tokens were found */
	if (tm->query[n].t != BR_NULL_TOKEN)
		return BR_FALSE;

	/* Compare the two token lists */
	return BrTokenValueComparePartial(tm->original, tm->query, insignificantMatchTokens);
}

void BR_CMETHOD_DECL(br_device_s3, tokensMatchEnd)
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
	BR_CMETHOD_REF(br_device_s3,		free),
	BR_CMETHOD_REF(br_object_s3,		identifier),
	BR_CMETHOD_REF(br_device_s3,		type),
	BR_CMETHOD_REF(br_device_s3,		isType),
	BR_CMETHOD_REF(br_object_s3,		device),
	BR_CMETHOD_REF(br_device_s3,		space),

	BR_CMETHOD_REF(br_device_s3,		templateQuery),
	BR_CMETHOD_REF(br_object,			query),
	BR_CMETHOD_REF(br_object, 			queryBuffer),
	BR_CMETHOD_REF(br_object, 			queryMany),
	BR_CMETHOD_REF(br_object, 			queryManySize),
	BR_CMETHOD_REF(br_object, 			queryAll),
	BR_CMETHOD_REF(br_object, 			queryAllSize),

	BR_CMETHOD_REF(br_device_s3,		   listQuery),
	BR_CMETHOD_REF(br_device_s3,	      tokensMatchBegin),
	BR_CMETHOD_REF(br_device_s3,	      tokensMatch),
	BR_CMETHOD_REF(br_device_s3,	      tokensMatchEnd),
	BR_CMETHOD_REF(br_object_container,	addFront),
	BR_CMETHOD_REF(br_object_container,	removeFront),
	BR_CMETHOD_REF(br_object_container,	remove),
	BR_CMETHOD_REF(br_object_container,	find),
	BR_CMETHOD_REF(br_object_container, findMany),
	BR_CMETHOD_REF(br_object_container, count),
};


