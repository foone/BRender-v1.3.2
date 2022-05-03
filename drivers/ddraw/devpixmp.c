/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: devpixmp.c 1.2 1998/10/21 15:41:46 jon Exp $
 * $Locker: $
 *
 * Device pixelmap methods
 */
#include <stddef.h>
#include <string.h>
#include <windows.h>
#include <ddraw.h>

#include "drv.h"
#include "pm.h"
#include "host.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: devpixmp.c 1.2 1998/10/21 15:41:46 jon Exp $");

/*
 * Default dispatch table for device pixelmap (defined at end of file)
 */
static struct br_device_pixelmap_dispatch devicePixelmapDispatch;

/*
 * Device pixelmap info. template
 */
#define F(f)	offsetof(struct br_device_pixelmap, f)

static struct br_tv_template_entry devicePixelmapTemplateEntries[] = {
	{BRT_WIDTH_I32,			0,	F(pm_width), 		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_I32_U16, },
	{BRT_HEIGHT_I32,		0,	F(pm_height),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_I32_U16, },
	{BRT_PIXEL_TYPE_U8,		0,	F(pm_type),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_I32_U8, },
//	{BRT_PIXEL_CHANNELS_TL,	0,	0,					BRTV_QUERY | BRTV_ALL,	BRTV_CONV_CUSTOM, },
//	{BRT_INDEXED_B,			0,	F(indexed),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT_OUTPUT_FACILITY_O,	0,	F(output_facility),	BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT_IDENTIFIER_CSTR,	0,	F(pm_identifier), 	BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT_CLUT_O,			0,	F(clut),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT_WINDOW_HANDLE_H,	0,	F(hwnd),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT_LPDIRECTDRAWSURFACE_P,	 0,	F(surface),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
};
#undef F

/*
 * Structures used to translate tokens and values
 */
struct hwnd_tokens {
	HWND hwnd;
};

#define F(f)	offsetof(struct hwnd_tokens, f)
static struct br_tv_template_entry hwndTemplateEntries[] = {
	{BRT(WINDOW_HANDLE_H),			F(hwnd),			BRTV_SET,	BRTV_CONV_COPY, },
};
#undef F


/*
 * Create a new device pixelmap
 */
br_device_pixelmap * DevicePixelmapDirectDrawAllocate(br_device *dev, br_output_facility *facility, br_token_value *tv)
{
	int size;
    br_int_32 count, bpp;
	br_uint_32 width, height;
	br_device_pixelmap *self;
	struct hwnd_tokens ht;
	RECT rect;
	DWORD flags;
	LPDIRECTDRAW ddraw;
	DDSURFACEDESC ddsd = { sizeof(DDSURFACEDESC) };
	PALETTEENTRY *entries;

	UASSERT(dev != NULL);
	UASSERT(facility != NULL);
	UASSERT(tv != NULL);

   // Create hwndTemplate, if necessary ...

   if( dev->templates.hwndTemplate == NULL)
   {
        dev->templates.hwndTemplate = BrTVTemplateAllocate( dev->res,
            hwndTemplateEntries,
            BR_ASIZE(hwndTemplateEntries));
   }

	/*
	 * Get window handle
	 */
	ht.hwnd = NULL;

	BrTokenValueSetMany(&ht, &count, NULL, tv, dev->templates.hwndTemplate );

	if( ht.hwnd == NULL )
	{
		BR_ERROR0("The DirectDraw driver requires a window handle\n");
		return NULL;
	}

	/*
	 * Determine pixelmap dimensions
	 */
	if(facility->fullscreen) 
	{
		width = facility->width;
		height = facility->height;
	} 
	else 
	{
		GetClientRect(ht.hwnd, &rect);
		width = rect.right;
		height = rect.bottom;
	}

	/*
	 * Increment output facility instance counter
	 */
	facility->num_instances++;

	/*
	 * Allocate device pixelmap
	 */

	self = BrResAllocate( dev->res, sizeof(*self), BR_MEMORY_OBJECT );

	/*
	 * Fill members
	 */
	self->dispatch = &devicePixelmapDispatch;
	self->device = dev ;
 	self->pm_identifier = "DirectDraw:Screen";
	self->pm_map = NULL;
	self->pm_flags = BR_PMF_NO_ACCESS;
	self->pm_copy_function = BR_PMCOPY_NORMAL;
	self->pm_base_x = 0;
	self->pm_base_y = 0;
	self->pm_width = (br_uint_16) width;
	self->pm_height = (br_uint_16) height;
	self->pm_origin_x = 0;
	self->pm_origin_y = 0;
	self->pm_row_bytes = 0;
	self->pm_pixels = NULL;
	self->pm_type = (br_uint_8) facility->colour_type;
	self->pixel_bits = (br_uint_16) facility->colour_bits;
	self->fullscreen = facility->fullscreen;
	self->hwnd = ht.hwnd;
	self->bPrimary = BR_TRUE ;
	self->output_facility = facility;

	/*
	 * Create a DirectDraw object
	 */
	if(dev->create(NULL, &ddraw, NULL) != DD_OK)
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
	 * Set cooperative level
	 */
	if (self->fullscreen)
		flags = DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT |
			(facility->modex || facility->standard_vga? DDSCL_ALLOWMODEX: 0);
	else
		flags = DDSCL_NORMAL;

	if (IDirectDraw2_SetCooperativeLevel(self->DD, self->hwnd, flags) != DD_OK) {
		BrResFree(self);
		return NULL;
	}

	/*
	 * Change display mode if fullscreen
	 */
	if(self->fullscreen) {

		bpp = self->pixel_bits;

		if (bpp == 15)
			bpp = 16;

#ifdef DDSDM_STANDARDVGAMODE
		if (IDirectDraw2_SetDisplayMode(self->DD, width, height, bpp, 0, (facility->standard_vga? DDSDM_STANDARDVGAMODE: 0)) != DD_OK) {
#else
		if (IDirectDraw2_SetDisplayMode(self->DD, width, height, bpp, 0, 0) != DD_OK) {
#endif
			BrResFree(self);
			return NULL;
		}
	}		

	/*
	 * Create a primary surface
	 */
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	/*
	 * For ModeX modes we also allocate a back buffer since we cannot access the front buffer
	 * at all
	 */
	if (facility->modex) {

		ddsd.ddsCaps.dwCaps |= DDSCAPS_COMPLEX | DDSCAPS_FLIP;
		ddsd.dwFlags |= DDSD_BACKBUFFERCOUNT;
		ddsd.dwBackBufferCount = 1;
	}

	if (IDirectDraw2_CreateSurface(self->DD, &ddsd, &self->surface, NULL) != DD_OK) {
		BrResFree(self);
		return NULL;
	}

	/*
	 * For ModeX, find the back buffer
	 */
	if (facility->modex) {

		DDSCAPS caps = { DDSCAPS_BACKBUFFER };

		if (IDirectDrawSurface_GetAttachedSurface(self->surface, &caps, &self->offscreen) != DD_OK) {
			BrResFree(self);
			return NULL;
		}
	}

	/*
	 * Just double check that the expected pixel format was returned
	 */
#if 1
	memset(&ddsd.ddpfPixelFormat, 0, sizeof(DDPIXELFORMAT));
	ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);

	if (IDirectDrawSurface_GetPixelFormat(self->surface, &ddsd.ddpfPixelFormat) != DD_OK) {
		BrResFree(self);
		return NULL;
	}

	/*
	 * Simple comparison code to match the simple enumeration code
	 */
	switch (facility->colour_type) {

	case BR_PMT_INDEX_8:

		if (!(ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8)) {
			BrResFree(self);
			return NULL;
		}

		break;

	case BR_PMT_RGB_555:

		if (!(ddsd.ddpfPixelFormat.dwFlags & DDPF_RGB) || (ddsd.ddpfPixelFormat.dwRGBBitCount != 16) ||
			(ddsd.ddpfPixelFormat.dwRBitMask != 0x00007c00) || (ddsd.ddpfPixelFormat.dwGBitMask != 0x000003e0) ||
			(ddsd.ddpfPixelFormat.dwBBitMask != 0x0000001f)) {

			BrResFree(self);
			return NULL;
		}

		break;

	case BR_PMT_RGB_565:

		if (!(ddsd.ddpfPixelFormat.dwFlags & DDPF_RGB) || (ddsd.ddpfPixelFormat.dwRGBBitCount != 16) ||
			(ddsd.ddpfPixelFormat.dwRBitMask != 0x0000f800) || (ddsd.ddpfPixelFormat.dwGBitMask != 0x000007e0) ||
			(ddsd.ddpfPixelFormat.dwBBitMask != 0x0000001f)) {

			BrResFree(self);
			return NULL;
		}

		break;

	case BR_PMT_RGB_888:

		if (!(ddsd.ddpfPixelFormat.dwFlags & DDPF_RGB) || (ddsd.ddpfPixelFormat.dwRGBBitCount != 24) ||
			(ddsd.ddpfPixelFormat.dwRBitMask != 0x00ff0000) || (ddsd.ddpfPixelFormat.dwGBitMask != 0x0000ff00) ||
			(ddsd.ddpfPixelFormat.dwBBitMask != 0x000000ff)) {

			BrResFree(self);
			return NULL;
		}

	default:
		BrResFree(self);
		return NULL;
	}
#endif

	/*
	 * Create and configure a clipper object if not fullscreen
	 */
	if(self->fullscreen == BR_FALSE) {
		IDirectDraw2_CreateClipper(self->DD, 0, &self->clipper, NULL);
		IDirectDrawClipper_SetHWnd(self->clipper, 0, self->hwnd);
		IDirectDrawSurface_SetClipper(self->surface, self->clipper);
	}

	/*
	 * Attach a CLUT to indexed surfaces
	 */
	switch(self->pm_type) {
	case BR_PMT_INDEX_8:

		self->clut = DeviceClutDirectDrawAllocate(dev, self, "DirectDraw-CLUT");

		size = self->clut->syspal_size;
		entries = (PALETTEENTRY *)BrMemAllocate(size * sizeof(PALETTEENTRY), BR_MEMORY_SCRATCH);
		GetPaletteEntries(self->clut->palette, 0, size, entries);

		IDirectDraw2_CreatePalette(self->DD, DDPCAPS_8BIT | (facility->fullscreen? DDPCAPS_ALLOW256: 0),
			entries, &self->palette, NULL);
		IDirectDrawSurface_SetPalette(self->surface, self->palette);

		BrMemFree(entries);
		break;
	}

	/*
	 * Add object to its container
	 */
	ObjectContainerAddFront(facility, (br_object *)self);

	return self;
}

br_device *	BR_CMETHOD_DECL(br_device_pixelmap_dd, device)(br_device_pixelmap *self)
{
	return self->device;
}

/*
 * br_device_pixelmap_dd::resize
 */
static void BR_CMETHOD_DECL(br_device_pixelmap_dd, resize)\
	(br_device_pixelmap *self, br_int_32 width, br_int_32 height)
{
   br_uint_16 bpp ;
   DDSURFACEDESC ddsd ;

   // Ignore requests to resize to zero width / height

	if ( ( width == 0 ) || ( height == 0 ) ) 
     return ;                         

	/*
	 * Adjust pixelmap members
	 */
	self->pm_width  = (br_uint_16) width;
	self->pm_height = (br_uint_16) height;

	// Change display mode if fullscreen

	if ( self->fullscreen == BR_TRUE ) 
   {
    	self->surface->lpVtbl->Release( self->surface );

		if((bpp = self->pixel_bits) == 15)
			bpp = 16;

		IDirectDraw2_SetDisplayMode(self->DD, width, height, bpp, 0, 0);

    	// Create a primary surface
	 
   	memset(&ddsd, 0, sizeof(DDSURFACEDESC));
   	ddsd.dwSize = sizeof(DDSURFACEDESC);
   	ddsd.dwFlags = DDSD_CAPS;
   	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
   	IDirectDraw2_CreateSurface(self->DD, &ddsd, &self->surface, NULL);

      if ( self->pm_type == BR_PMT_INDEX_8 )
         IDirectDrawSurface_SetPalette(self->surface, self->palette);
   }
}

/*
 * br_device_pixelmap_dd::free
 */
static void BR_CMETHOD_DECL(br_device_pixelmap_dd, free)\
	(br_device_pixelmap *self)
{
	UASSERT(self->output_facility != NULL);

	if (!self->sub_pixelmap) {

	  	if (self->surface != NULL)
			IDirectDrawSurface_Release(self->surface);

		/*
		 * Release DirectDraw object and items attached to it
		 */
		if (self->DD != NULL) {
			IDirectDraw2_SetCooperativeLevel(self->DD, NULL, DDSCL_NORMAL);
			IDirectDraw2_Release(self->DD);
		}
	}

	/*
	 * Remove object from its container
	 */
	ObjectContainerRemove(self->output_facility, (br_object *)self);

	/*
	 * Decrement instance counter
	 */
	self->output_facility->num_instances--;

	/*
	 * Free pixelmap structure
	 */
	BrResFreeNoCallback(self);
}

/*
 * Structures used to translate tokens and values
 */
struct match_tokens {
	br_token use;
	br_int_32 pixel_bits;
	br_int_32 width;
	br_int_32 height;
	br_uint_8 type ;
};

#define F(f)	offsetof(struct match_tokens, f)

static struct br_tv_template_entry matchTemplateEntries[] = {
	{BRT_USE_T, 			0,	F(use),			BRTV_SET,	BRTV_CONV_COPY, },
	{BRT_PIXEL_BITS_I32,	0,	F(pixel_bits),	BRTV_SET,	BRTV_CONV_COPY, },
	{BRT_WIDTH_I32,			0,	F(width),		BRTV_SET,	BRTV_CONV_COPY, },
	{BRT_HEIGHT_I32,		0,	F(height),		BRTV_SET,	BRTV_CONV_COPY, },
	{BRT_PIXEL_TYPE_U8,		0,	F(type),		BRTV_SET,	BRTV_CONV_COPY, },
};

#undef F

/*
 * br_device_pixelmap_dd::match
 */
static br_error BR_CMETHOD_DECL(br_device_pixelmap_dd, match)\
	(br_device_pixelmap *self, br_device_pixelmap **newpm, br_token_value *tv)
{
	br_int_32 count;
   struct match_tokens mt = {BRT_NONE, 0};

	UASSERT(newpm != NULL);
	UASSERT(tv != NULL);

   // Create match template, if required ...

   if ( self->device->templates.matchTemplate == NULL )
   {
        self->device->templates.matchTemplate = BrTVTemplateAllocate(self->device->res,
            matchTemplateEntries,
            BR_ASIZE(matchTemplateEntries));
   }

	/*
	 * Set default width and height
	 */
	mt.width = self->pm_width;
	mt.height = self->pm_height;
   mt.type = self->pm_type ;

	/*
	 * Convert match tokens
	 */
	BrTokenValueSetMany(&mt, &count, NULL, tv, self->device->templates.matchTemplate );

	/*
	 * Switch on match type
	 */
	switch(mt.use) {

	case BRT_OFFSCREEN:

		/*
		 * Create matching offscreen buffer
		 */
		return BufferDirectDrawAllocateOff(self, newpm, mt.width, mt.height, mt.type);

	case BRT_CLONE:
	case BRT_DEPTH:
	case BRT_HIDDEN:
	case BRT_HIDDEN_BUFFER:
		/*
		 * None of these match types is allowed
		 */
		return BRE_FAIL;

	default:
		return BRE_FAIL;
	}

	return BRE_OK;
}

/*
 * br_device_pixelmap_dd::doubleBuffer
 */
static br_error BR_CMETHOD_DECL(br_device_pixelmap_dd, doubleBuffer)\
	(br_device_pixelmap *self, br_device_pixelmap *src)
{
	POINT pt;
	RECT srect, drect;
	br_error error;

	UASSERT(self != NULL);
	UASSERT(src != NULL);

	/*
	 * Set the source and destination RECTs
	 */
	SetRect(&srect, 0, 0, src->pm_width, src->pm_height);
	SetRect(&drect, 0, 0, self->pm_width, self->pm_height);

	/*
	 * Offset if not fullscreen
	 */
	if (self->fullscreen == BR_FALSE) {
		pt.x = pt.y = 0;
		ClientToScreen(self->hwnd, &pt);
		OffsetRect(&drect, pt.x, pt.y);
	}

	/*
	 * Blit the pixels
	 */
	while (1) {

		switch (IDirectDrawSurface_Blt(self->output_facility->modex? self->offscreen: self->surface,
			&drect, src->surface, &srect, DDBLT_WAIT, NULL)) {
			
		case DD_OK:

			/*
			 * For ModeX modes we blit to the offscreen buffer and flip
			 */
			if (self->output_facility->modex)

				if (IDirectDrawSurface_Flip(self->surface, self->offscreen, DDFLIP_WAIT) == DD_OK)
					return BRE_OK;
				else
					return BRE_FAIL;

		   return BRE_OK;

		case DDERR_SURFACELOST:

			// Ed has pointed out a bug here :
#if 0
			if (IDirectDrawSurface_Restore(self->output_facility->modex? self->offscreen: self->surface) != DD_OK ||
				IDirectDrawSurface_Restore(src->surface) != DD_OK)
#else
			// Don't restore secondary attached surfaces; instead restore their parent
			if (IDirectDrawSurface_Restore(self->surface) != DD_OK ||
				IDirectDrawSurface_Restore(src->surface) != DD_OK)
#endif

				return BRE_FAIL;

			break;

		case DDERR_WASSTILLDRAWING:

			break;

		default:

			return BRE_FAIL;
		}
	}
}

br_token BR_CMETHOD_DECL(br_device_pixelmap_dd, type)\
	(br_device_pixelmap *self)
{
	return BRT_DEVICE_PIXELMAP;
}

br_boolean BR_CMETHOD_DECL(br_device_pixelmap_dd, isType)\
	(br_device_pixelmap *self, br_token t)
{
	return (t == BRT_DEVICE_PIXELMAP) || (t == BRT_OBJECT);
}

br_int_32 BR_CMETHOD_DECL(br_device_pixelmap_dd, space)\
	(br_device_pixelmap *self)
{
	return sizeof(br_device_pixelmap);
}

struct br_tv_template * BR_CMETHOD_DECL(br_device_pixelmap_dd, queryTemplate)(br_device_pixelmap *self)
{
    if ( self->device->templates.devicePixelmapTemplate == NULL )
        self->device->templates.devicePixelmapTemplate = BrTVTemplateAllocate( self->device->res,
            devicePixelmapTemplateEntries,
            BR_ASIZE(devicePixelmapTemplateEntries));

    return self->device->templates.devicePixelmapTemplate ;
}

/*
 * br_device_pixelmap_dd::allocateSub
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_dd, allocateSub)\
	(br_device_pixelmap *self, br_device_pixelmap **newpm, br_rectangle *rect)
{
	/*
	 * Method is not supported
	 */
 	return BRE_FAIL;
}




/*
 * Default dispatch table for device pixelmap
 */
static struct br_device_pixelmap_dispatch devicePixelmapDispatch = {
	NULL,
	NULL,
	NULL,
	NULL,
	BR_CMETHOD_REF(br_device_pixelmap_dd,	free),
	BR_CMETHOD_REF(br_object_dd,			identifier),
	BR_CMETHOD_REF(br_device_pixelmap_dd,	type),
	BR_CMETHOD_REF(br_device_pixelmap_dd,	isType),
	BR_CMETHOD_REF(br_device_pixelmap_dd,	device),
	BR_CMETHOD_REF(br_device_pixelmap_dd,	space),

	BR_CMETHOD_REF(br_device_pixelmap_dd,	queryTemplate),
	BR_CMETHOD_REF(br_object,				query),
	BR_CMETHOD_REF(br_object,				queryBuffer),
	BR_CMETHOD_REF(br_object,				queryMany),
	BR_CMETHOD_REF(br_object,				queryManySize),
	BR_CMETHOD_REF(br_object,				queryAll),
	BR_CMETHOD_REF(br_object,				queryAllSize),

	BR_CMETHOD_REF(br_device_pixelmap_mem,	validSource),
	BR_CMETHOD_REF(br_device_pixelmap_dd,	resize),
	BR_CMETHOD_REF(br_device_pixelmap_dd,	match),
	BR_CMETHOD_REF(br_device_pixelmap_dd,	allocateSub),

	BR_CMETHOD_REF(br_device_pixelmap_ddo,	copy),
	BR_CMETHOD_REF(br_device_pixelmap_gen,	copyTo),
	BR_CMETHOD_REF(br_device_pixelmap_gen,	copyFrom),
	BR_CMETHOD_REF(br_device_pixelmap_ddo,	fill),
	BR_CMETHOD_REF(br_device_pixelmap_dd,	doubleBuffer),

	BR_CMETHOD_REF(br_device_pixelmap_gen,	copyDirty),
	BR_CMETHOD_REF(br_device_pixelmap_gen,	copyToDirty),
	BR_CMETHOD_REF(br_device_pixelmap_gen,	copyFromDirty),
	BR_CMETHOD_REF(br_device_pixelmap_ddo,	fillDirty),
	BR_CMETHOD_REF(br_device_pixelmap_gen,	doubleBufferDirty),

	BR_CMETHOD_REF(br_device_pixelmap_gen,	rectangle),
	BR_CMETHOD_REF(br_device_pixelmap_gen,	rectangle2),
	BR_CMETHOD_REF(br_device_pixelmap_ddo,	rectangleCopy),
	BR_CMETHOD_REF(br_device_pixelmap_ddo,	rectangleCopyTo),
	BR_CMETHOD_REF(br_device_pixelmap_ddo,	rectangleCopyFrom),
	BR_CMETHOD_REF(br_device_pixelmap_ddo,	rectangleStretchCopy),
	BR_CMETHOD_REF(br_device_pixelmap_ddo,	rectangleStretchCopyTo),
	BR_CMETHOD_REF(br_device_pixelmap_ddo,	rectangleStretchCopyFrom),
	BR_CMETHOD_REF(br_device_pixelmap_ddo,	rectangleFill),
	BR_CMETHOD_REF(br_device_pixelmap_ddo,	pixelSet),
	BR_CMETHOD_REF(br_device_pixelmap_ddo,	line),
	BR_CMETHOD_REF(br_device_pixelmap_ddo,	copyBits),

	BR_CMETHOD_REF(br_device_pixelmap_gen,	text),
	BR_CMETHOD_REF(br_device_pixelmap_gen,	textBounds),

	BR_CMETHOD_REF(br_device_pixelmap_mem,	rowSize),
	BR_CMETHOD_REF(br_device_pixelmap_mem,	rowQuery),
	BR_CMETHOD_REF(br_device_pixelmap_mem,	rowSet),

	BR_CMETHOD_REF(br_device_pixelmap_ddo,	pixelQuery),
	BR_CMETHOD_REF(br_device_pixelmap_mem,	pixelAddressQuery),

	BR_CMETHOD_REF(br_device_pixelmap_mem,	pixelAddressSet),
	BR_CMETHOD_REF(br_device_pixelmap_mem,	originSet),

	BR_CMETHOD_REF(br_device_pixelmap_ddo,	flush),

	BR_CMETHOD_REF(br_device_pixelmap_mem,	synchronise),

	BR_CMETHOD_REF(br_device_pixelmap_ddo,	directLock),
	BR_CMETHOD_REF(br_device_pixelmap_ddo, directUnlock),	

	BR_CMETHOD_REF(br_device_pixelmap_gen,	getControls),
	BR_CMETHOD_REF(br_device_pixelmap_gen,	setControls),
};

