/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: dpmfront.c 1.2 1998/10/21 15:36:39 jon Exp $
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
#include "device.h"

BR_RCS_ID("$Id: dpmfront.c 1.2 1998/10/21 15:36:39 jon Exp $");



static int allocated_one_offscreen;

/*
 * Default dispatch table for device pixelmap (defined at end of file)
 */
static struct br_device_pixelmap_dispatch devicePixelmapDispatch;






/*
 * Allocate DirectDraw front surfaces
 *
 * Two separate methods for front and back surface allocation, because the
 * two cases are vastly different. The fullscreen case sets up video memory
 * for both a front and back flipping surface. The non-fullscreen case
 * allocates the front buffer which in general will be the whole window.
 */



LPDIRECTDRAWSURFACE back_surface_ati = NULL;
br_boolean back_surface_used_ati = BR_FALSE;


br_error ATISetupFullscreenMemory(LPDIRECTDRAWSURFACE *surface)
{
	HRESULT result;
	DDSURFACEDESC ddsd;
	DDSCAPS ddsCaps;

  /* Create a primary surface with one back buffer */
	memset(&ddsd, 0, sizeof(DDSURFACEDESC));
	ddsd.dwSize = sizeof(DDSURFACEDESC);
	ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_COMPLEX | DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_VIDEOMEMORY;
	ddsd.dwBackBufferCount = 1;
	if ((result = IDirectDraw_CreateSurface(gpDD, &ddsd, surface, NULL)) != DD_OK) {
		if (result == DDERR_OUTOFVIDEOMEMORY)
			return(BRE_DEV_NO_VIDEO_MEMORY);
		return BRE_DEV_FAIL;
	}

  /* Pick up the back buffer and store to be collected later */
  memset(&ddsCaps, 0, sizeof(DDSCAPS));
  ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
  if (IDirectDrawSurface_GetAttachedSurface(*surface, &ddsCaps, &back_surface_ati) != DD_OK)
    return(BRE_DEV_FAIL);

  back_surface_used_ati = BR_FALSE;
  return(BRE_OK);
}


br_error ATISetupNonFullscreenFront(LPDIRECTDRAWSURFACE *surface)
{
	HRESULT result;
	DDSURFACEDESC ddsd;

  /* Pick up the Windows primary surface only */
	memset(&ddsd, 0, sizeof(DDSURFACEDESC));
	ddsd.dwSize = sizeof(DDSURFACEDESC);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	if ((result = IDirectDraw_CreateSurface(gpDD, &ddsd, surface, NULL)) != DD_OK) {
		if (result == DDERR_OUTOFVIDEOMEMORY)
			return(BRE_DEV_NO_VIDEO_MEMORY);
		return BRE_DEV_FAIL;
	}

  return(BRE_OK);
}




/*
 * Device pixelmap info. template
 */
#define F(f)	offsetof(struct br_device_pixelmap, f)

static struct br_tv_template_entry devicePixelmapTemplateEntries[] = {
	{BRT_WIDTH_I32,			0,	F(pm_width),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_I32_U16, },
	{BRT_HEIGHT_I32,		0,	F(pm_height),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_I32_U16, },
	{BRT_PIXEL_TYPE_U8,		0,	F(pm_type),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_I32_U8, },
//	{BRT_PIXEL_CHANNELS_TL,	0,	0,					BRTV_QUERY | BRTV_ALL,	BRTV_CONV_CUSTOM, },
//	{BRT_INDEXED_B,			0,	F(indexed),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT_OUTPUT_FACILITY_O,	0,	F(output_facility),	BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT_IDENTIFIER_CSTR,	0,	F(pm_identifier),	BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT_CLUT_O,			0,	F(clut),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT_WINDOW_HANDLE_H,	0,	F(hwnd),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
};
#undef F

static struct br_tv_template devicePixelmapTemplate = {
	BR_ASIZE(devicePixelmapTemplateEntries),
	devicePixelmapTemplateEntries
};

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

static struct br_tv_template hwndTemplate = {
	BR_ASIZE(hwndTemplateEntries),
	hwndTemplateEntries
};

struct new_tokens {
	br_int_32 width;
	br_int_32 height;
	br_int_32 pixel_bits;
	br_boolean fullscreen;
};

#define F(f)	offsetof(struct new_tokens, f)
static struct br_tv_template_entry newTemplateEntries[] = {
	{BRT(WIDTH_I32),            F(width),       BRTV_SET,	BRTV_CONV_COPY, },
	{BRT(HEIGHT_I32),           F(height),      BRTV_SET,	BRTV_CONV_COPY, },
	{BRT(PIXEL_BITS_I32),       F(pixel_bits),  BRTV_SET,	BRTV_CONV_COPY, },
	{BRT(WINDOW_FULLSCREEN_B),  F(fullscreen),  BRTV_SET,	BRTV_CONV_COPY, },
};
#undef F

static struct br_tv_template newTemplate = {
	BR_ASIZE(newTemplateEntries),
	newTemplateEntries
};


/*
 * Structures used to translate tokens and values
 */
struct match_tokens {
	br_token use;
	br_int_32 width;
	br_int_32 height;
};

#define F(f)	offsetof(struct match_tokens, f)

static struct br_tv_template_entry matchTemplateEntries[] = {
	{BRT_USE_T,           0,  F(use),         BRTV_SET,	BRTV_CONV_COPY, },
	{BRT_WIDTH_I32,       0,  F(width),       BRTV_SET,	BRTV_CONV_COPY, },
	{BRT_HEIGHT_I32,      0,  F(height),      BRTV_SET,	BRTV_CONV_COPY, },
};

#undef F

static struct br_tv_template matchTemplate = {
	BR_ASIZE(matchTemplateEntries),
	matchTemplateEntries
};




/*
 * Create a new device pixelmap
 */
br_device_pixelmap * DevicePixelmapATIAllocate(br_device *dev, br_output_facility *facility, br_token_value *tv)
{
	br_int_32 count, width, height;
	br_device_pixelmap *self;
	struct new_tokens nt;
	struct hwnd_tokens ht;
	RECT rect;
	br_uint_16 pixel_bits;
	HRESULT result;

	UASSERT(dev != NULL);
	UASSERT(facility != NULL);
	UASSERT(tv != NULL);

	/* Clear pixelmap template */
	CLEAR_TEMPLATE(devicePixelmap);

	/* Clear template for later matching */
	CLEAR_TEMPLATE(match);

	/*
	 * Get window handle
	 */
	CLEAR_TEMPLATE(hwnd);
	ht.hwnd = NULL;
	hwndTemplate.res = DeviceATIResource(dev);
	BrTokenValueSetMany(&ht, &count, NULL, tv, &hwndTemplate);

	if(ht.hwnd == NULL)
		return NULL;

	if (facility->fullscreen) {
		width = facility->width;
		height = facility->height;

	} else {
		/*
		 * Set defaults
		 */
		CLEAR_TEMPLATE(new);
		GetClientRect(ht.hwnd, &rect);
		nt.width = rect.right;
		nt.height = rect.bottom;
		width = nt.width;
		height = nt.height;  
	}

	/*
	 * Allocate device pixelmap
	 */
	self = BrResAllocate(DeviceATIResource(dev), sizeof(*self), BR_MEMORY_OBJECT);


   /*
	 * Fill members
	 */
	self->dispatch = &devicePixelmapDispatch;
 	self->pm_identifier = "Windows-ATI Rage 3D:Screen";
	self->pm_map = NULL;
	self->pm_flags = BR_PMF_NO_ACCESS;
	self->pm_copy_function = BR_PMCOPY_NORMAL;
	self->pm_base_x = 0;
	self->pm_base_y = 0;
	self->pm_width = width;
	self->pm_height = height;
	self->pm_origin_x = 0;
	self->pm_origin_y = 0;
	self->pm_row_bytes = 0;
	self->pm_pixels = NULL;
	self->buffer = BUFFER_SCREEN;
	self->pm_type = facility->colour_type;
	self->fullscreen = facility->fullscreen;
	self->pixel_bits = facility->colour_bits;
	self->flipped = BR_TRUE;
	self->hwnd = ht.hwnd;

	/*
	 * Attach a new CLUT object
	 */
	self->clut = DeviceClutATIAllocate(dev, self, "Windows-CLUT");

   if (!self->fullscreen) {
		/* Non-fullscreen setup */
		if(IDirectDraw_SetCooperativeLevel(gpDD, self->hwnd, DDSCL_NORMAL) != DD_OK)
			return NULL;

		if (ATISetupNonFullscreenFront(&self->surface) != BRE_OK)
			return(NULL);
    
		/* If not fullscreen, create a clipper object and attach to primary surface */
		if (IDirectDraw_CreateClipper(gpDD, 0, &self->clipper, NULL) != DD_OK)
			return NULL;
		if (IDirectDrawClipper_SetHWnd(self->clipper, 0, self->hwnd) != DD_OK)
			return NULL;
		if (IDirectDrawSurface_SetClipper(self->surface, self->clipper) != DD_OK)
			return NULL;
  
    
	} else {
		/* Fullscreen mode setup */
		if(IDirectDraw_SetCooperativeLevel(gpDD, self->hwnd,
			                                 DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN) != DD_OK)
			return NULL;

		pixel_bits = self->pixel_bits;
		if(pixel_bits == 15)
			pixel_bits = 16;

		result = IDirectDraw_SetDisplayMode(gpDD, self->pm_width, self->pm_height, pixel_bits);
		if(result != DD_OK)
			return NULL;

		if (ATISetupFullscreenMemory(&self->surface) != BRE_OK)
			return(NULL);
	}

	/* Offscreen buffers matched off the front screen are a problem; to solve
	 * this, this flag indicates whether or not the first match has been done
	 * (the first match will be the back buffer, while others are miscellaneous
	 * pixelmaps for generic use) */
	allocated_one_offscreen = 0;

	// Setup ATI CIF
	if (!ATIInstantiateLibrary())
		return(NULL);

	/* Attach to output facility and increment instance counter */
	self->output_facility = facility;
	self->output_facility->num_instances++;

	/* Add object to its container */
	ObjectContainerAddFront(facility, (br_object *)self);

	return self;
}





/*
 * br_device_pixelmap_ati::resize
 */
static br_error BR_CMETHOD_DECL(br_device_pixelmap_ati, resize)\
	(br_device_pixelmap *self, br_int_32 width, br_int_32 height)
{
	HRESULT result;
	br_uint_32 pixel_bits;
	br_error bre;

	UASSERT((width > 0) && (height >0));

	if (self->fullscreen) {
		IDirectDraw_RestoreDisplayMode(gpDD);

		/* Andy's suggestion for driver spec change: in fullscreen mode the
		 * driver responds to resize commands by changing the display mode
		 * to the requested size.
		 *
		 * PROCEDURE:
		 * 1. Free Z buffer THEN back buffer. DO NOT FREE THE FRONT SURFACE!!!
		 * 2. Call Resize with front pixelmap
		 * 3. Call Match for back and z as before
		 *
		 * If this is not followed it won't work!
		 */

		/* First of all free up the front/back surfaces and the texture cache */
		ATIClearTextureCache();

		/* Release DirectDraw palette if attached */
		if(self->palette)
			IDirectDrawPalette_Release(self->palette);

		/* Release DirectDraw clipper if attached */
		if(self->clipper)
			IDirectDrawClipper_Release(self->clipper);

		/* Release primary surface (and any attached back surfaces) */
		IDirectDrawSurface_Release(self->surface);



		/* Repeat allocation */
		if(IDirectDraw_SetCooperativeLevel(gpDD, self->hwnd,
			                                 DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN) != DD_OK)
			return(BRE_DEV_REFUSED);

		pixel_bits = self->pixel_bits;
		if (pixel_bits == 15)
			pixel_bits = 16;

		result = IDirectDraw_SetDisplayMode(gpDD, width, height, pixel_bits);
		if (result != DD_OK)
			return(BRE_DEV_REFUSED);

		if ((bre = ATISetupFullscreenMemory(&self->surface)) != BRE_OK)
			return(bre);

		allocated_one_offscreen = 0;

		self->pm_width = width;
		self->pm_height = height;

	} else {
	  /*
	   * Adjust pixelmap members
	   */
	  self->pm_width = width;
	  self->pm_height = height;
	}

  return(BRE_OK);
}

/*
 * br_device_pixelmap_ati::free
 */
static void BR_CMETHOD_DECL(br_device_pixelmap_ati, free)\
	(br_device_pixelmap *self)
{
	UASSERT(self->output_facility != NULL);

	/* Release DirectDraw palette if attached */
	if(self->palette)
		IDirectDrawPalette_Release(self->palette);

	/* Release DirectDraw clipper if attached */
	if(self->clipper)
		IDirectDrawClipper_Release(self->clipper);

	/* Release primary surface (and any attached back surfaces) */
	IDirectDrawSurface_Release(self->surface);

	/* Restore display mode if necessary: if this fails, don't fail the
	 * rest of the free call - probably */
	if(self->fullscreen)
		IDirectDraw_RestoreDisplayMode(gpDD);

	/* Remove object from its container */
	ObjectContainerRemove(self->output_facility, (br_object *)self);

	/* Decrement instance counter */
	self->output_facility->num_instances--;

   /* Destroy the default texture palette */
   DestroyDefaultPalette();

	/* Free pixelmap structure */
	BrResFreeNoCallback(self);

   /* Shut down ATI libraries */
   ATITerminateLibrary();


}


/*
 * br_device_pixelmap_ati::match
 */
static br_error BR_CMETHOD_DECL(br_device_pixelmap_ati, match)\
	(br_device_pixelmap *self, br_device_pixelmap **newpm, br_token_value *tv)
{
	br_int_32 count;
	br_error result;
	struct match_tokens mt = {BRT_NONE, 0};

	UASSERT(newpm != NULL);
	UASSERT(tv != NULL);

	/*
	 * Set default width and height
	 */
	mt.width = self->pm_width;
	mt.height = self->pm_height;

	/*
	 * Convert match tokens
	 */
	matchTemplate.res = DeviceATIResource(ObjectATIDevice(self));
	BrTokenValueSetMany(&mt, &count, NULL, tv, &matchTemplate);

	/*
	 * Switch on match type
	 */
	switch(mt.use) {

	case BRT_OFFSCREEN:
		/*
		 * Create matching offscreen buffer: a back buffer if the first.
		 * Otherwise, the match will fail, because you can't guarantee more
		 * than one displayable buffer.
	     */
		if (!allocated_one_offscreen) {
 			result = ATIAllocateBack(self, newpm, mt.width, mt.height);
			allocated_one_offscreen = 1;
			return result;
		} else
			return(ATIAllocateGenericOffscreen(self, newpm, mt.width, mt.height));
		break;

   /* System memory device pixelmap */
   case BRT_NO_RENDER:
		return(ATIAllocateGenericOffscreen(self, newpm, mt.width, mt.height));
		break;

	case BRT_HIDDEN_BUFFER:
		/* Allocate a video memory buffer for texture rendering */
		return(ATIAllocateVideoMemory(self, newpm, mt.width, mt.height));
		break;

	case BRT_CLONE:
	case BRT_DEPTH:
	case BRT_HIDDEN:
		/*
		 * None of these match types is allowed from the front buffer
		 */
		return BRE_DEV_UNSUPPORTED;

	default:
		return BRE_DEV_UNSUPPORTED;
	}

	return BRE_OK;
}




/*
 * br_device_pixelmap_ati::doubleBuffer
 */
static br_error BR_CMETHOD_DECL(br_device_pixelmap_ati, doubleBuffer)\
	(br_device_pixelmap *self, br_device_pixelmap *src)
{
	POINT pt;
	RECT srect, drect;
	void *tmp;

	UASSERT(self != NULL);
	UASSERT(src != NULL);

	if (self->fullscreen) {
		/* Fullscreen double buffer by page flip */
		if (IDirectDrawSurface_Flip(self->surface, NULL, DDFLIP_WAIT) != DD_OK)
			return(BRE_DEV_FAIL);

		/* Swap pixel pointers (the flip command does the same function internally for surfaces) */
		tmp = self->pm_pixels;
		self->pm_pixels = src->pm_pixels;
		src->pm_pixels = tmp;

		self->flipped = BR_TRUE;
		src->flipped = BR_TRUE;
  
	} else {
		/* Non-fullscreen double buffer by blit */

		/* Set the source and destination RECTs */
		SetRect(&srect, 0, 0, src->pm_width, src->pm_height);
		SetRect(&drect, 0, 0, self->pm_width, self->pm_height);

		/* Offset if not fullscreen */
		pt.x = pt.y = 0;
		ClientToScreen(self->hwnd, &pt);
		OffsetRect(&drect, pt.x, pt.y);

		/* Blit the pixels */
		if(IDirectDrawSurface_Blt(self->surface, &drect, src->surface, &srect, DDBLT_WAIT, NULL) != DD_OK)
			return BRE_DEV_FAIL;
	}

	return BRE_OK;
}



br_token BR_CMETHOD_DECL(br_device_pixelmap_ati, type)\
	(br_device_pixelmap *self)
{
	return BRT_DEVICE_PIXELMAP;
}

br_boolean BR_CMETHOD_DECL(br_device_pixelmap_ati, isType)\
	(br_device_pixelmap *self, br_token t)
{
	return (t == BRT_DEVICE_PIXELMAP) || (t == BRT_OBJECT);
}

br_int_32 BR_CMETHOD_DECL(br_device_pixelmap_ati, space)\
	(br_device_pixelmap *self)
{
	return sizeof(br_device_pixelmap);
}

struct br_tv_template * BR_CMETHOD_DECL(br_device_pixelmap_ati, queryTemplate)(br_device_pixelmap *self)
{
	devicePixelmapTemplate.res = DeviceATIResource(ObjectATIDevice(self));
	return &devicePixelmapTemplate;
}

/*
 * br_device_pixelmap_ati::allocateSub
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_ati, allocateSub)\
	(br_device_pixelmap *self, br_device_pixelmap **newpm, br_rectangle *rect)
{
	/*
	 * Method is not supported
	 */
 	return BRE_FAIL;
}


br_error BR_CMETHOD_DECL(br_device_pixelmap_ati, rectangleFill)\
	(br_device_pixelmap *self, br_rectangle *rect, br_uint_32 fill)
{

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
	BR_CMETHOD_REF(br_device_pixelmap_ati,  free),
	BR_CMETHOD_REF(br_object_ati,           identifier),
	BR_CMETHOD_REF(br_device_pixelmap_ati,  type),
	BR_CMETHOD_REF(br_device_pixelmap_ati,  isType),
	BR_CMETHOD_REF(br_object_ati,           device),
	BR_CMETHOD_REF(br_device_pixelmap_ati,  space),

	BR_CMETHOD_REF(br_device_pixelmap_ati,  queryTemplate),
	BR_CMETHOD_REF(br_object,               query),
	BR_CMETHOD_REF(br_object,               queryBuffer),
	BR_CMETHOD_REF(br_object,               queryMany),
	BR_CMETHOD_REF(br_object,               queryManySize),
	BR_CMETHOD_REF(br_object,               queryAll),
	BR_CMETHOD_REF(br_object,               queryAllSize),

	BR_CMETHOD_REF(br_device_pixelmap_mem,  validSource),
	BR_CMETHOD_REF(br_device_pixelmap_ati,  resize),
	BR_CMETHOD_REF(br_device_pixelmap_ati,  match),
	BR_CMETHOD_REF(br_device_pixelmap_ati,  allocateSub),

	BR_CMETHOD_REF(br_device_pixelmap_gen,  copyTo),
	BR_CMETHOD_REF(br_device_pixelmap_gen,  copyTo),
	BR_CMETHOD_REF(br_device_pixelmap_gen,  copyFrom),
	BR_CMETHOD_REF(br_device_pixelmap_gen,  fill),
	BR_CMETHOD_REF(br_device_pixelmap_ati,  doubleBuffer),

	BR_CMETHOD_REF(br_device_pixelmap_gen,  copyDirty),
	BR_CMETHOD_REF(br_device_pixelmap_gen,  copyToDirty),
	BR_CMETHOD_REF(br_device_pixelmap_gen,  copyFromDirty),
	BR_CMETHOD_REF(br_device_pixelmap_gen,  fillDirty),
	BR_CMETHOD_REF(br_device_pixelmap_gen,  doubleBufferDirty),

	BR_CMETHOD_REF(br_device_pixelmap_gen,  rectangle),
	BR_CMETHOD_REF(br_device_pixelmap_gen,  rectangle2),
	BR_CMETHOD_REF(br_device_pixelmap_mem,  rectangleCopyTo),
	BR_CMETHOD_REF(br_device_pixelmap_mem,  rectangleCopyTo),
	BR_CMETHOD_REF(br_device_pixelmap_mem,  rectangleCopyFrom),
	BR_CMETHOD_REF(br_device_pixelmap_mem,  rectangleStretchCopyTo),
	BR_CMETHOD_REF(br_device_pixelmap_mem,  rectangleStretchCopyTo),
	BR_CMETHOD_REF(br_device_pixelmap_mem,  rectangleStretchCopyFrom),
	BR_CMETHOD_REF(br_device_pixelmap_ati,  rectangleFill),
	BR_CMETHOD_REF(br_device_pixelmap_mem,  pixelSet),
	BR_CMETHOD_REF(br_device_pixelmap_mem,  line),
	BR_CMETHOD_REF(br_device_pixelmap_mem,  copyBits),

	BR_CMETHOD_REF(br_device_pixelmap_gen,  text),
	BR_CMETHOD_REF(br_device_pixelmap_gen,  textBounds),

	BR_CMETHOD_REF(br_device_pixelmap_mem,  rowSize),
	BR_CMETHOD_REF(br_device_pixelmap_mem,  rowQuery),
	BR_CMETHOD_REF(br_device_pixelmap_mem,  rowSet),

	BR_CMETHOD_REF(br_device_pixelmap_mem,  pixelQuery),
	BR_CMETHOD_REF(br_device_pixelmap_mem,  pixelAddressQuery),

	BR_CMETHOD_REF(br_device_pixelmap_mem,  pixelAddressSet),
	BR_CMETHOD_REF(br_device_pixelmap_mem,  originSet),

	BR_CMETHOD_REF(br_device_pixelmap_mem,		flush),

	BR_CMETHOD_REF(br_device_pixelmap_mem,		synchronise),

	BR_CMETHOD_REF(br_device_pixelmap_ati_dd,	directLock),
	BR_CMETHOD_REF(br_device_pixelmap_ati_dd, directUnlock),	

	BR_CMETHOD_REF(br_device_pixelmap_gen,	getControls),
	BR_CMETHOD_REF(br_device_pixelmap_gen,	setControls),
};

br_boolean CheckFrontDispatch(void *dispatch)
{
	return(dispatch == &devicePixelmapDispatch);
}



