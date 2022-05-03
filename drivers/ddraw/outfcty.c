/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: outfcty.c 1.1 1997/12/10 16:45:26 jon Exp $
 * $Locker: $
 *
 * Output type methods
 */
#include <stddef.h>
#include <string.h>
#include <windows.h>
#include <ddraw.h>

#include "drv.h"
#include "host.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: outfcty.c 1.1 1997/12/10 16:45:26 jon Exp $");

/*
 * Default dispatch table for device (defined at end of file)
 */
static struct br_output_facility_dispatch outputFacilityDispatch;

/*
 * Output type information templates
 */
#define F(f)	offsetof(struct br_output_facility, f)

static struct br_tv_template_entry outputFacilityTemplateEntries[] = {
	{BRT(WIDTH_I32),				F(width),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(WIDTH_MIN_I32),			F(width),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(WIDTH_MAX_I32),			F(width),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(HEIGHT_I32),				F(height),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(HEIGHT_MIN_I32),			F(height),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(HEIGHT_MAX_I32),			F(height),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(PIXEL_BITS_I32),			F(colour_bits),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(PIXEL_TYPE_U8),			F(colour_type),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(INDEXED_B),  				F(indexed),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(WINDOW_FULLSCREEN_B),		F(fullscreen),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(IDENTIFIER_CSTR),			F(identifier),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{DEV(IS_STANDARD_VGA_B),		F(standard_vga),	BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{DEV(IS_MODEX_B),				F(modex),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(WINDOW_HANDLE_H),			0,					BRTV_QUERY | BRTV_ALL,	BRTV_CONV_DIRECT, },
};

#undef F


int BrDirectDraw_BPP( int type )
{
   switch ( type )  
   {
      case BR_PMT_INDEX_8 : return 8 ;
	   case BR_PMT_RGB_555 : return 15 ;
      case BR_PMT_RGB_565 : return 16 ;
      case BR_PMT_RGB_888 : return 24 ;
      case BR_PMT_RGBX_888 : return 32 ;
	   case BR_PMT_RGBA_8888 : return 32 ;
   }

   return 0 ;
}


br_error DirectDrawToBRenderSurfaceType( LPDDPIXELFORMAT format, br_uint_16 *pType )
{
   if(format->dwFlags & DDPF_PALETTEINDEXED8) 
   {
      *pType = BR_PMT_INDEX_8;
   } 
   else if((format->dwFlags & DDPF_RGB) && (format->dwRBitMask == 0x00007c00) &&
		(format->dwGBitMask == 0x000003e0) && (format->dwBBitMask == 0x0000001f)) {
		*pType = BR_PMT_RGB_555;
   } 
   else if((format->dwFlags & DDPF_RGB) && (format->dwRBitMask == 0x0000f800) &&
		(format->dwGBitMask == 0x000007e0) && (format->dwBBitMask == 0x0000001f)) {
		*pType = BR_PMT_RGB_565;
	} 
   else if((format->dwFlags & DDPF_RGB) && (format->dwRBitMask == 0x00ff0000) &&
		(format->dwGBitMask == 0x0000ff00) && (format->dwBBitMask == 0x000000ff)) {
		*pType = BR_PMT_RGB_888;
	} 
   else return BRE_FAIL ;

   return BRE_OK ;
}



/*
 * Display mode enumeration callback
 */
HRESULT FAR PASCAL EnumCallback(LPDDSURFACEDESC ddsd, LPVOID context)
{
	char tmp[64];
	br_uint_16 colour_type;
	br_device *dev;
	br_output_facility *self;

	dev = (br_device *)context;

	/*
	 * Determine the format
	 */
	if (DirectDrawToBRenderSurfaceType(&ddsd->ddpfPixelFormat, &colour_type) != BRE_OK)
		return DDENUMRET_OK;

	/*
	 * Allocate an object
	 */
	self = BrResAllocate(dev->res, sizeof(*self), BR_MEMORY_OBJECT );

	self->dispatch = &outputFacilityDispatch;

	self->device = dev;
	self->num_instances = 0;

	/*
	 * Add to list and increment instance counter
	 */
	self->object_list = BrObjectListAllocate(self);
	ObjectContainerAddFront(dev, (br_object *)self);	
	dev->output_facility_count++;

	/*
	 * Fill in display format
	 */
	self->colour_type = colour_type;
	self->colour_bits = BrDirectDraw_BPP(colour_type);
	self->indexed = (colour_type <= BR_PMT_INDEX_8);	// Gross!

	self->width = ddsd->dwWidth;
	self->height = ddsd->dwHeight;

	self->fullscreen = BR_TRUE;

#ifdef DDSCAPS_STANDARDVGAMODE
 	self->standard_vga = (ddsd->ddsCaps.dwCaps & DDSCAPS_STANDARDVGAMODE) != 0;
#else
 	self->standard_vga = BR_FALSE;
#endif
	self->modex = (ddsd->ddsCaps.dwCaps & DDSCAPS_MODEX) != 0;

	/*
	 * Attach a descriptive identifier
	 */
	BrSprintfN(tmp, sizeof(tmp) - 1, "F:%dx%dx%d", self->width, self->height, self->colour_bits);
	self->identifier = BrResStrDup(self, tmp);

	return DDENUMRET_OK;
}


/*
 * Build a list of available output facilities
 */
br_error OutputFacilityDirectDrawInitialise(br_device *dev)
{
	char tmp[64];
	br_output_facility *self;
	DDSURFACEDESC ddsd = { sizeof(DDSURFACEDESC) };
	br_uint_16 colour_type;
	br_output_facility **vga_facility = NULL, **modex_facility = NULL, **linear_facility = NULL;
	br_uint_32 num_vga_facilities = 0, num_modex_facilities = 0, num_linear_facilities = 0;
	br_uint_32 i, j;
	br_token_value tv[3];
	
    /*
	 * Determine the format of the Windows desktop
	 */	
	if (IDirectDraw2_GetDisplayMode(dev->DD, &ddsd) != DD_OK) {
		BR_ERROR0("GetDisplayMode");
		return BRE_FAIL;
	}

	if (DirectDrawToBRenderSurfaceType(&ddsd.ddpfPixelFormat, &colour_type) != BRE_OK) {
		BR_ERROR0("Invalid display mode");
		return BRE_FAIL;
	}

	/*
	 * Allocate an object
	 */
	self = BrResAllocate(dev->res, sizeof(*self), BR_MEMORY_OBJECT );

	self->dispatch = &outputFacilityDispatch;

	self->device = dev;
	self->num_instances = 0;

	/*
	 * Add to list and increment instance counter
	 */
	self->object_list = BrObjectListAllocate(self);
	ObjectContainerAddFront(dev, (br_object *)self);	
	dev->output_facility_count++;

	/*
	 * Fill in display format
	 */
	self->colour_type = colour_type;
	self->colour_bits = BrDirectDraw_BPP(colour_type);
	self->indexed = (colour_type <= BR_PMT_INDEX_8);	// Gross!

	self->width = ddsd.dwWidth;
	self->height = ddsd.dwHeight;

	self->fullscreen = BR_FALSE;

	/*
	 * Attach a descriptive identifier
	 */
	BrSprintfN(tmp, sizeof(tmp) - 1, "W:%dx%dx%d", self->width, self->height, self->colour_bits);
	self->identifier = BrResStrDup(self, tmp);

	/*
	 * Enumerate and record available fullscreen modes, if possible
	 */
	if (IDirectDraw2_SetCooperativeLevel(dev->DD, dev->window,
		dev->window != NULL? (DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX): DDSCL_NORMAL) == DD_OK) {

		/*
		 * Disabled standard vga mode support because it isn't quite working
		 */
#if 0//def DDEDM_STANDARDVGAMODES
		IDirectDraw2_EnumDisplayModes(dev->DD, DDEDM_STANDARDVGAMODES, NULL, dev, EnumCallback);
#else
		IDirectDraw2_EnumDisplayModes(dev->DD, 0, NULL, dev, EnumCallback);
#endif

		IDirectDraw2_SetCooperativeLevel(dev->DD, NULL, DDSCL_NORMAL);
	}

	/*
	 * Look for duplicate modes and use in order of preference (linear, standard VGA, ModeX)
	 */
	tv[0].t = BrTokenCreate("IS_STANDARD_VGA_B", BR_NULL_TOKEN);
	tv[1].t = BrTokenCreate("IS_MODEX_B", BR_NULL_TOKEN);
	tv[2].t = BR_NULL_TOKEN;

#ifdef DDEDM_STANDARDVGAMODES
	tv[0].v.b = BR_TRUE;
	tv[1].v.b = BR_FALSE;

	ObjectContainerCount(dev, &num_vga_facilities, BRT_OUTPUT_FACILITY, NULL, tv);

	if (num_vga_facilities != 0) {
		
		vga_facility = BrResAllocate(self, num_vga_facilities * sizeof(*vga_facility), BR_MEMORY_SCRATCH);
		
		if (vga_facility != NULL)

			ObjectContainerFindMany(dev, (br_object **)vga_facility, num_vga_facilities,
				&num_vga_facilities, BRT_OUTPUT_FACILITY, NULL, tv);

		else

			num_vga_facilities = 0;
	}
#endif

	tv[0].v.b = BR_FALSE;
	tv[1].v.b = BR_TRUE;

	ObjectContainerCount(dev, &num_modex_facilities, BRT_OUTPUT_FACILITY, NULL, tv);
	
	if (num_modex_facilities != 0) {
		
		modex_facility = BrResAllocate(self, num_modex_facilities * sizeof(*modex_facility), BR_MEMORY_SCRATCH);
		
		if (modex_facility != NULL)

			ObjectContainerFindMany(dev, (br_object **)modex_facility, num_modex_facilities,
				&num_modex_facilities, BRT_OUTPUT_FACILITY, NULL, tv);
		
		else

			num_modex_facilities = 0;
	}

	tv[0].v.b = BR_FALSE;
	tv[1].v.b = BR_FALSE;

	ObjectContainerCount(dev, &num_linear_facilities, BRT_OUTPUT_FACILITY, NULL, tv);
	
	if (num_linear_facilities != 0) {
		
		linear_facility = BrResAllocate(self, num_linear_facilities * sizeof(*linear_facility), BR_MEMORY_SCRATCH);
		
		if (linear_facility != NULL)

			ObjectContainerFindMany(dev, (br_object **)linear_facility, num_linear_facilities,
				&num_linear_facilities, BRT_OUTPUT_FACILITY, NULL, tv);

		else

			num_linear_facilities = 0;
	}

	/*
	 * Eliminate unwanted ModeX modes
	 */
	for (i = 0; i < num_modex_facilities; i++) {

#ifdef DDEDM_STANDARDVGAMODES
		for (j = 0; j < num_vga_facilities; j++)

			if (modex_facility[i]->width == vga_facility[j]->width &&
				modex_facility[i]->height == vga_facility[j]->height &&
				modex_facility[i]->colour_type == vga_facility[j]->colour_type) {

				ObjectContainerRemove(dev, (br_object *)modex_facility[i]);
				ObjectFree((br_object *)modex_facility[i]);
				break;
			}

		if (j == num_vga_facilities)
#endif

			for (j = 0; j < num_linear_facilities; j++)

				if (modex_facility[i]->width == linear_facility[j]->width &&
					modex_facility[i]->height == linear_facility[j]->height &&
					modex_facility[i]->colour_type == linear_facility[j]->colour_type) {

					ObjectContainerRemove(dev, (br_object *)modex_facility[i]);
					ObjectFree((br_object *)modex_facility[i]);
					break;
				}
	}

#ifdef DDEDM_STANDARDVGAMODES
	/*
	 * Eliminate unwanted VGA modes
	 */
	for (i = 0; i < num_vga_facilities; i++)

		for (j = 0; j < num_linear_facilities; j++)

			if (vga_facility[i]->width == linear_facility[j]->width &&
				vga_facility[i]->height == linear_facility[j]->height &&
				vga_facility[i]->colour_type == linear_facility[j]->colour_type) {

				ObjectContainerRemove(dev, (br_object *)vga_facility[i]);
				ObjectFree((br_object *)vga_facility[i]);
				break;
			}
#endif

#ifdef DDEDM_STANDARDVGAMODES
	if (vga_facility != NULL)
		BrResFree(vga_facility);
#endif

	if (modex_facility != NULL)
		BrResFree(modex_facility);

	if (linear_facility != NULL)
		BrResFree(linear_facility);

	return BRE_OK;
}

/*
 * Common object methods
 */
static void BR_CMETHOD_DECL(br_output_facility_dd, free)(br_output_facility *self)
{
   ObjectContainerRemove( self->device, (br_object *)self);

	/*
	 * Remove attached objects
	 */
	ObjectContainerFree((br_object_container *)self, BR_NULL_TOKEN, NULL, NULL);
}

static br_token BR_CMETHOD_DECL(br_output_facility_dd, type)(br_output_facility *self)
{
	return BRT_OUTPUT_FACILITY;
}

static br_boolean BR_CMETHOD_DECL(br_output_facility_dd, isType)(br_output_facility *self, br_token t)
{
	return (t == BRT_OUTPUT_FACILITY) || (t == BRT_OBJECT_CONTAINER) || (t == BRT_OBJECT);
}

static br_int_32 BR_CMETHOD_DECL(br_output_facility_dd, space)(br_output_facility *self)
{
	return sizeof(br_output_facility);
}

static struct br_tv_template * BR_CMETHOD_DECL(br_output_facility_dd, queryTemplate)(br_output_facility *self)
{
    if(self->device->templates.outputFacilityTemplate== NULL)
        self->device->templates.outputFacilityTemplate= BrTVTemplateAllocate(self->device->res,
            outputFacilityTemplateEntries,
            BR_ASIZE(outputFacilityTemplateEntries));

    return self->device->templates.outputFacilityTemplate;
}

static br_error BR_CMETHOD_DECL(br_output_facility_dd, validSource)(br_output_facility *self, br_boolean *bp, br_object *h)
{
	return BRE_OK;
}

/*
 * br_output_facility_dd::pixelmapNew
 *
 * Instantiate an output pixelmap from the output type
 */
static br_error BR_CMETHOD_DECL(br_output_facility_dd, pixelmapNew)\
	(br_output_facility *self, br_device_pixelmap **ppmap, br_token_value *tv)
{
	br_device *dev;
	br_device_pixelmap *pm;

	/*
	 * Create a device pixelmap structure representing display memory
	 */
	dev = self->device ;
	pm = DevicePixelmapDirectDrawAllocate(dev, self, tv);
	if(pm == NULL)
		return BRE_FAIL;

	*ppmap = pm;

	return BRE_OK;
}

/*
 * Cannot create new CLUTs, stuck with the single hardware one
 */
static br_error BR_CMETHOD_DECL(br_output_facility_dd, clutNew)(br_output_facility *self,
	br_device_clut **pclut, br_token_value *tv)
{
	return BRE_FAIL;
}

static void * BR_CMETHOD_DECL(br_output_facility_dd, listQuery)
	(br_device *self)
{
	return self->object_list;
}

static br_error BR_CMETHOD_DECL(br_output_facility_dd, queryCapability)(
    br_token_value *buffer_in, br_token_value *buffer_out, br_size_t size_buffer_out)
{
	return BRE_FAIL;
}



/*
 * Output facility dispatch table
 */
static struct br_output_facility_dispatch outputFacilityDispatch = {
	NULL,
	NULL,
	NULL,
	NULL,
	BR_CMETHOD_REF(br_output_facility_dd,	free),
	BR_CMETHOD_REF(br_object_dd,		  	identifier),
	BR_CMETHOD_REF(br_output_facility_dd,	type),
	BR_CMETHOD_REF(br_output_facility_dd,	isType),
	BR_CMETHOD_REF(br_object_dd,			device),
	BR_CMETHOD_REF(br_output_facility_dd,	space),

	BR_CMETHOD_REF(br_output_facility_dd,	queryTemplate),
	BR_CMETHOD_REF(br_object,				query),
	BR_CMETHOD_REF(br_object,				queryBuffer),
	BR_CMETHOD_REF(br_object,				queryMany),
	BR_CMETHOD_REF(br_object,				queryManySize),
	BR_CMETHOD_REF(br_object,				queryAll),
	BR_CMETHOD_REF(br_object,				queryAllSize),

	BR_CMETHOD_REF(br_output_facility_dd,	listQuery),
	BR_CMETHOD_REF(br_object_container,		tokensMatchBegin),
	BR_CMETHOD_REF(br_object_container,		tokensMatch),
	BR_CMETHOD_REF(br_object_container,		tokensMatchEnd),
	BR_CMETHOD_REF(br_object_container,		addFront),
	BR_CMETHOD_REF(br_object_container,		removeFront),
	BR_CMETHOD_REF(br_object_container,		remove),
	BR_CMETHOD_REF(br_object_container,		find),
	BR_CMETHOD_REF(br_object_container,		findMany),
	BR_CMETHOD_REF(br_object_container,		count),

	BR_CMETHOD_REF(br_output_facility_dd,	validSource),
	BR_CMETHOD_REF(br_output_facility_dd,	pixelmapNew),
	BR_CMETHOD_REF(br_output_facility_dd,	clutNew),
   BR_CMETHOD_REF(br_output_facility_dd,  queryCapability),

};


