/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: dpmback.c 1.2 1998/10/21 15:36:38 jon Exp $
 * $Locker: $
 *
 * Device pixelmap methods - DirectDraw
 */
#define WIN32_LEAN_AND_MEAN
#include <stddef.h>
#include <string.h>
#include <windows.h>
#include <windowsx.h>

#ifdef __DRIVER__
#define INITGUID
#endif

#include <ddraw.h>

#include "drv.h"
#include "atilink.h"
#include "pm.h"
#include "host.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: dpmback.c 1.2 1998/10/21 15:36:38 jon Exp $");

/*
 * Default dispatch table for device pixelmap (defined at end of file)
 */
static struct br_device_pixelmap_dispatch devicePixelmapDispatch_dd;


br_error BR_CMETHOD_DECL(br_device_pixelmap_ati_dd, rectangleFill)\
	(br_device_pixelmap *self, br_rectangle *r, br_uint_32 colour);



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
	{BRT_USE_T,           0,  F(use),         BRTV_SET, BRTV_CONV_COPY, },
	{BRT_WIDTH_I32,       0,  F(width),       BRTV_SET,	BRTV_CONV_COPY, },
	{BRT_HEIGHT_I32,      0,  F(height),      BRTV_SET,	BRTV_CONV_COPY, },
};
#undef F

static struct br_tv_template matchTemplate = {
	BR_ASIZE(matchTemplateEntries),
	matchTemplateEntries
};





extern LPDIRECTDRAWSURFACE back_surface_ati;
extern br_boolean back_surface_used_ati;




br_error ATISetupNonFullscreenBack(LPDIRECTDRAWSURFACE *surface,
                                                br_int_32 w, br_int_32 h)
{
	HRESULT result;
	DDSURFACEDESC ddsd;

	/* Allocate a new back surface in video memory so the card can draw on it */
	memset(&ddsd, 0, sizeof(DDSURFACEDESC));
	ddsd.dwSize = sizeof(DDSURFACEDESC);
 	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
 	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
	ddsd.dwWidth = w;
	ddsd.dwHeight = h;
 	if ((result = IDirectDraw_CreateSurface(gpDD, &ddsd, surface, NULL)) != DD_OK) {
		if (result == DDERR_OUTOFVIDEOMEMORY)
			return(BRE_DEV_NO_VIDEO_MEMORY);
		return BRE_DEV_FAIL;
	}

	return(BRE_OK);
}




br_error ATISetupZ(LPDIRECTDRAWSURFACE *surface,
                                br_int_32 w, br_int_32 h)
{
	HRESULT result;
	DDSURFACEDESC ddsd;

	/* Allocate a Z surface in video memory so the card can access it */
	memset(&ddsd, 0, sizeof(DDSURFACEDESC));
 	ddsd.dwSize = sizeof(DDSURFACEDESC);
 	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_ZBUFFERBITDEPTH;
 	ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_VIDEOMEMORY;
	ddsd.dwWidth = w;
	ddsd.dwHeight = h;
	ddsd.dwZBufferBitDepth = 16;
 	if ((result = IDirectDraw_CreateSurface(gpDD, &ddsd, surface, NULL)) != DD_OK) {
		if (result == DDERR_OUTOFVIDEOMEMORY)
			return(BRE_DEV_NO_VIDEO_MEMORY);
		return BRE_DEV_FAIL;
	}

	return(BRE_OK);
}



/*
 * Allocate a DirectDraw back surface
 */
br_error ATIAllocateBack(br_device_pixelmap *self, br_device_pixelmap **newpm, int w, int h)
{
	int width;
	br_uint_16 qual;
	br_device *dev;
	br_device_pixelmap *devpm;
	LPDIRECTDRAWSURFACE surface;
	br_error result;

	UASSERT(self != NULL);
	UASSERT(newpm != NULL);
	UASSERT((w > 0) && (h > 0));

	/* Clear template for later matching */
	CLEAR_TEMPLATE(match);

	/* Get pointer to device */
	dev = ObjectATIDevice(self);

	/* Ensure rows are long-aligned */
	width = ALIGN4(w);

	if (self->fullscreen) {
		/* Hand over the already-allocated fullscreen back surface. If it's
		 * already been handed over, fail terminally */
		if (back_surface_used_ati)
			return(BRE_DEV_FAIL);
		surface = back_surface_ati;
		back_surface_used_ati = BR_TRUE;

	} else {
		/* Allocate back screen for non-fullscreen case */

		/* Must clear the texture cache to give maximum possible memory */
		ATIClearTextureCache();

		if ((result = ATISetupNonFullscreenBack(&surface, w, h)) != BRE_OK)
			return(result);
	}

	/* Allocate new pixelmap */
	devpm = BrResAllocate(DeviceATIResource(dev), sizeof(*devpm), BR_MEMORY_OBJECT);

	/* Fill members */	
	*devpm = *self;
	devpm->pm_identifier = "Windows-ATI Rage 3D: Vid mem Offscreen Buffer";
    devpm->dispatch = &devicePixelmapDispatch_dd;
	devpm->pm_width = w;
	devpm->pm_height = h;
	devpm->surface = surface;
	HostSelectorDS(&qual);
	devpm->pm_pixels_qualifier = qual;
	devpm->buffer = BUFFER_OFFSCREEN;
	devpm->parent = BR_TRUE;
	devpm->videomemory = BR_TRUE;
	devpm->flipped = BR_TRUE;
	*newpm = devpm;

	/* Lock then immediately unlock to get row bytes */
	LockSurface(devpm);
	UnlockSurface(devpm);

	return BRE_OK;
}



/*
 * Allocate a DirectDraw generic offscreen surface; this will
 * be a surface in SYSTEM memory:
 */
br_error ATIAllocateGenericOffscreen(br_device_pixelmap *self, br_device_pixelmap **newpm, int w, int h)
{
	int width;
	br_uint_16 qual;
	br_device *dev;
	br_device_pixelmap *devpm;
	LPDIRECTDRAWSURFACE surface;
	HRESULT result;
	DDSURFACEDESC ddsd;

	UASSERT(self != NULL);
	UASSERT(newpm != NULL);
	UASSERT((w > 0) && (h > 0));

	/* Get pointer to device */
	dev = ObjectATIDevice(self);

	/* Ensure rows are long-aligned */
	width = ALIGN4(w);

	/* Allocate a new surface in system memory */
	memset(&ddsd, 0, sizeof(DDSURFACEDESC));
	ddsd.dwSize = sizeof(DDSURFACEDESC);
 	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
 	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	ddsd.dwWidth = w;
	ddsd.dwHeight = h;
 	if ((result = IDirectDraw_CreateSurface(gpDD, &ddsd, &surface, NULL)) != DD_OK)
		return BRE_DEV_FAIL;

	/* Allocate new pixelmap */
	devpm = BrResAllocate(DeviceATIResource(dev), sizeof(*devpm), BR_MEMORY_OBJECT);

	/* Fill members */	
	*devpm = *self;
	devpm->pm_identifier = "Windows-ATI Rage 3D: Generic Offscreen Buffer";
	devpm->dispatch = &devicePixelmapDispatch_dd;
	devpm->pm_width = w;
	devpm->pm_height = h;
	devpm->surface = surface;
	HostSelectorDS(&qual);
	devpm->pm_pixels_qualifier = qual;
	devpm->buffer = BUFFER_OFFSCREEN;
	devpm->parent = BR_TRUE;
	devpm->videomemory = BR_FALSE;
	devpm->flipped = BR_TRUE;
	*newpm = devpm;

	/* Lock then immediately unlock to get row bytes */
	LockSurface(devpm);
	UnlockSurface(devpm);

	return BRE_OK;
}




/*
 * Attempt to allocate a surface in video memory using DirectDraw.
 */
br_error ATIAllocateVideoMemory(br_device_pixelmap *self, br_device_pixelmap **newpm,
							   int w, int h)
{
	int width;
	br_uint_16 qual;
	br_device *dev;
	br_device_pixelmap *devpm;
	LPDIRECTDRAWSURFACE surface;
	HRESULT result;
	DDSURFACEDESC ddsd;

	UASSERT(self != NULL);
	UASSERT(newpm != NULL);
	UASSERT((w > 0) && (h > 0));

	/* Get pointer to device */
	dev = ObjectATIDevice(self);

	/* Ensure rows are long-aligned */
	width = ALIGN4(w);

	/* Allocate a new surface in video memory so the card can draw on it */
	memset(&ddsd, 0, sizeof(DDSURFACEDESC));
	ddsd.dwSize = sizeof(DDSURFACEDESC);
 	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
 	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
	ddsd.dwWidth = w;
	ddsd.dwHeight = h;
 	if ((result = IDirectDraw_CreateSurface(gpDD, &ddsd, &surface, NULL)) != DD_OK) {
		if (result == DDERR_OUTOFVIDEOMEMORY)
			return(BRE_DEV_NO_VIDEO_MEMORY);
		return BRE_DEV_FAIL;
	}

	/* Allocate new pixelmap */
	devpm = BrResAllocate(DeviceATIResource(dev), sizeof(*devpm), BR_MEMORY_OBJECT);

	/* Fill members */	
	*devpm = *self;
	devpm->pm_identifier = "Windows-ATI Rage 3D: Hidden Buffer";
	devpm->dispatch = &devicePixelmapDispatch_dd;
	devpm->pm_width = w;
	devpm->pm_height = h;
	devpm->surface = surface;
	HostSelectorDS(&qual);
	devpm->pm_pixels_qualifier = qual;
	devpm->buffer = BUFFER_OFFSCREEN;
	devpm->parent = BR_TRUE;
	devpm->videomemory = BR_TRUE;
	devpm->flipped = BR_TRUE;
	*newpm = devpm;

	/* Lock then immediately unlock to get row bytes */
	LockSurface(devpm);
	UnlockSurface(devpm);

	return BRE_OK;
}






/*
 * Match a DirectDraw Z-buffer surface
 */
br_error ATIAllocateZ(br_device_pixelmap *self, br_device_pixelmap **newpm)
{
	br_uint_16 qual;
	br_device *dev;
	br_device_pixelmap *devpm;
	LPDIRECTDRAWSURFACE surface;
	br_error result;

	UASSERT(self != NULL);
	UASSERT(newpm != NULL);

	/* Get pointer to device */
	dev = ObjectATIDevice(self);

	/* Must clear the texture cache to give maximum possible memory */
	ATIClearTextureCache();

	/* Set up a Z surface */
	if ((result = ATISetupZ(&surface, self->pm_width, self->pm_height)) != BRE_OK)
		return(result);

	/* Allocate new pixelmap */
	devpm = BrResAllocate(DeviceATIResource(dev), sizeof(*devpm), BR_MEMORY_OBJECT);

	/* Fill members */	
	*devpm = *self;
	devpm->pm_identifier = "Windows-ATI Rage 3D: Z Buffer";
	devpm->dispatch = &devicePixelmapDispatch_dd;
 	devpm->surface = surface;
	devpm->pm_type = BR_PMT_DEPTH_16;
	HostSelectorDS(&qual);
	devpm->pm_pixels_qualifier = qual;
	devpm->buffer = BUFFER_OFFSCREEN;
	devpm->parent = BR_TRUE;
	devpm->videomemory = BR_TRUE;
	devpm->flipped = BR_TRUE;
	*newpm = devpm;

	/* Lock then immediately unlock to get row bytes */
	LockSurface(devpm);
	UnlockSurface(devpm);

	return BRE_OK;
}





/*
 * Lock a surface; returns address of framebuffer if requested. lock_area
 * is optional; if set to NULL, locks whole surface.
 */
br_boolean ATILock(LPDIRECTDRAWSURFACE surface, void **framebuffer, RECT *lock_area, long *pitch)
{
	HRESULT        ddretval;
	DDSURFACEDESC  surface_descriptor;

	/* Lock the surface, pick up the info */
	memset(&surface_descriptor, 0, sizeof(surface_descriptor));
	surface_descriptor.dwSize = sizeof(surface_descriptor);
	ddretval = IDirectDrawSurface_Lock(surface, lock_area, &surface_descriptor,
									   DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
	if (ddretval != DD_OK)
		return(BR_FALSE);

	if (framebuffer)
		*framebuffer = surface_descriptor.lpSurface;
	if (pitch)
		*pitch = surface_descriptor.lPitch;

	return(BR_TRUE);
}

/*
 * Unlocks a surface previously locked by the above function
 */
br_boolean ATIUnlock(LPDIRECTDRAWSURFACE surface, void *lock_address)
{
	HRESULT        ddretval;


	ddretval = IDirectDrawSurface_Unlock(surface, lock_address);
	if (ddretval != DD_OK)
		return(BR_FALSE);

	return(BR_TRUE);
}







//#define DISABLE_LOCK


/*
 * Lock surface
 */
static br_error LockSurface(br_device_pixelmap *self)
{
	long pitch;

	UASSERT(self != NULL);

	if (!ATILock(self->surface, &self->bits, NULL, &pitch))
		return(BRE_DEV_LOCK_FAIL);
#ifdef DISABLE_LOCK
	if (!ATIUnlock(self->surface, self->bits))
		return(BRE_DEV_LOCK_FAIL);
#endif

	self->pm_row_bytes = pitch;
	self->pm_pixels = (char *)self->bits;

	return BRE_OK;
}

/*
 * Unlock surface
 */
static br_error UnlockSurface(br_device_pixelmap *self)
{
	UASSERT(self != NULL);

#ifndef DISABLE_LOCK
	if(!ATIUnlock(self->surface, self->bits))
		return BRE_DEV_LOCK_FAIL;
#endif

	/* Set device pixelmap members */
	self->bits = NULL;
	self->pm_pixels = NULL;

	return BRE_OK;
}

/*
 * br_device_pixelmap_ati_dd::directLock
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_ati_dd, directLock)(br_device_pixelmap *self, br_boolean block)
{
	if(block == BR_FALSE)
		return BRE_OK;

	return LockSurface(self);
}

/*
 * br_device_pixelmap_ati_dd::directUnlock
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_ati_dd, directUnlock)(br_device_pixelmap *self)
{
	return UnlockSurface(self);
}

/*
 * br_device_pixelmap_ati_dd::free
 */
static void BR_CMETHOD_DECL(br_device_pixelmap_ati_dd, free)(br_device_pixelmap *self)
{
	UASSERT(self != NULL);

	/*
	 * Only do the following if not a sub-pixelmap
	 */
	if(self->parent == BR_TRUE)
      {
      if(self->surface)
         {
		   /* Release offscreen surface */
		   if(gpDD != NULL)
            {
			   IDirectDrawSurface_Release(self->surface);
            self->surface = NULL;
            }
         }
	   }

	/*
	 * Free pixelmap structure
	 */
	BrResFreeNoCallback(self);
}

/*
 * br_device_pixelmap_ati_dd::resize
 */
static br_error BR_CMETHOD_DECL(br_device_pixelmap_ati_dd, resize)(br_device_pixelmap *self, br_int_32 width, br_int_32 height)
{
	UASSERT(self->surface);

	if (!self->videomemory) {
		/* This won't be called any more, but still supported */
		LPDIRECTDRAWSURFACE surface;
		HRESULT result;
		DDSURFACEDESC ddsd;

		/* Release old surface */
		IDirectDrawSurface_Release(self->surface);

		/* Reallocate offscreen surface in system memory */
		memset(&ddsd, 0, sizeof(DDSURFACEDESC));
		ddsd.dwSize = sizeof(DDSURFACEDESC);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	 	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
		ddsd.dwWidth = width;
		ddsd.dwHeight = height;
		if ((result = IDirectDraw_CreateSurface(gpDD, &ddsd, &surface, NULL)) != DD_OK) {
			if (result == DDERR_OUTOFVIDEOMEMORY)
				return(BRE_DEV_NO_VIDEO_MEMORY);
			return(BRE_DEV_FAIL);
		}

	} else {
		br_error result;

		if (self->fullscreen)
			return(BRE_DEV_UNSUPPORTED);

		if (self->buffer == BUFFER_SCREEN)
			return(BRE_OK);

		/* Must clear the texture cache to give maximum resize memory */
		ATIClearTextureCache();

		/* Release surface */
		IDirectDrawSurface_Release(self->surface);

		/* Allocate a new surface */
		if (self->pm_type == BR_PMT_DEPTH_16) {
			if ((result = ATISetupZ(&self->surface, width, height)) != BRE_OK)
				return(result);
		} else {
			if ((result = ATISetupNonFullscreenBack(&self->surface, width, height)) != BRE_OK)
				return(result);
		}
	}

	/* Adjust device pixelmap members */
	self->pm_width = width;
	self->pm_height = height;

	/* Lock then immediately unlock to get row bytes */
	LockSurface(self);
	UnlockSurface(self);

	return BRE_OK;
}

/*
 * br_device_pixelmap_ati_dd::allocateSub
 */
static br_error BR_CMETHOD_DECL(br_device_pixelmap_ati_dd, allocateSub)(br_device_pixelmap *self, br_device_pixelmap **newpm, br_rectangle *rect)
{
	br_rectangle out;
	br_device_pixelmap *pm;

	/*
	 * Create sub-window (clipped against original)
	 */
	if(PixelmapRectangleClip(&out, rect, (br_pixelmap *)self) == BR_CLIP_REJECT)
		return BRE_FAIL;

	/*
	 * Create the new structure and copy
	 */
	pm = BrResAllocate(DeviceATIResource(ObjectATIDevice(self)), sizeof(*pm), BR_MEMORY_OBJECT);

	*pm = *self;

	pm->pm_base_x += (br_uint_16)out.x;
	pm->pm_base_y += (br_uint_16)out.y;
	pm->pm_width = (br_uint_16)out.w;
	pm->pm_height = (br_uint_16)out.h;

	pm->pm_origin_x = 0;
	pm->pm_origin_y = 0;

	pm->parent = BR_FALSE;

	/* Set the offsets to the sub pixelmap */
	pm->sub_x_offset = out.x;
	pm->sub_y_offset = out.y;

	/* Pixel rows may not be contiguous */
	if(self->pm_width != pm->pm_width)
		pm->pm_flags &= ~BR_PMF_LINEAR;

	*newpm = pm;

	return BRE_OK;
}

/*
 * Perform a blit with the source and destination copy functions
 */
static br_error FunctionBlit(br_device_pixelmap *dst, RECT *drect, br_device_pixelmap *src, RECT *srect)
{
	DWORD flags;
	DDBLTFX bltfx, *bltfx_ptr;

	/*
	 * Set defaults
	 */
	bltfx_ptr = NULL;
	flags = DDBLT_WAIT;
	bltfx.dwSize = sizeof(DDBLTFX);

	/*
	 * Source colour key
	 */
	if(src->pm_copy_function == BR_PMCOPY_SRC_KEYED) {
		bltfx.ddckSrcColorkey.dwColorSpaceLowValue = (DWORD)src->pm_src_key.low;
		bltfx.ddckSrcColorkey.dwColorSpaceHighValue = (DWORD)src->pm_src_key.high;
		flags |= DDBLT_KEYSRCOVERRIDE;
		bltfx_ptr = &bltfx;
	}

	/*
	 * Destination colour key
	 */
	if(dst->pm_copy_function == BR_PMCOPY_DST_KEYED) {
		bltfx.ddckDestColorkey.dwColorSpaceLowValue = (DWORD)dst->pm_dst_key.low;
		bltfx.ddckDestColorkey.dwColorSpaceHighValue = (DWORD)dst->pm_dst_key.high;
		flags |= DDBLT_KEYDESTOVERRIDE;
		bltfx_ptr = &bltfx;
	}

	/*
	 * Perform the blit
	 */
	if(IDirectDrawSurface_Blt(dst->surface, drect, src->surface, srect, flags, bltfx_ptr) != DD_OK)
		return BRE_DEV_FAIL;

	return BRE_OK;
}

/*
 * br_device_pixelmap_ati_dd::copy
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_ati_dd, copy)\
	(br_device_pixelmap *self, br_device_pixelmap *src)
{
	RECT rect;

	/*
	 * Set the source and destination RECT
	 */
	if(SetRect(&rect, 0, 0, self->pm_width, self->pm_height) == FALSE)
		return BRE_DEV_FAIL;

	/*
	 * Blit the pixels
	 */
	return FunctionBlit(self, &rect, src, &rect);	
}

/*
 * br_device_pixelmap_ati_dd::rectangleCopy
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_ati_dd, rectangleCopy)\
	(br_device_pixelmap *self, br_point *p, br_device_pixelmap *src, br_rectangle *r)
{
	br_point pp;
	br_rectangle rr;
	RECT srect, drect;

	/*
	 * Clip rectangle to pixelmaps
	 */
	if(PixelmapRectangleClipTwo(&rr, &pp, r, p, (br_pixelmap *)self, (br_pixelmap *)src) == BR_CLIP_REJECT)
		return BRE_OK;

	/*
	 * Set the source and destination RECTs
	 */
	if(SetRect(&srect, rr.x, rr.y, rr.x + rr.w, rr.y + rr.h) == FALSE)
		return BRE_DEV_FAIL;

	if(SetRect(&drect, pp.x, pp.y, pp.x + rr.w, pp.y + rr.h) == FALSE)
		return BRE_DEV_FAIL;

	/*
	 * Blit the pixels
	 */
	return FunctionBlit(self, &drect, src, &srect);	
}

/*
 * br_device_pixelmap_ati_dd::rectangleCopyTo
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_ati_dd, rectangleCopyTo)\
	(br_device_pixelmap *self, br_point *p, br_device_pixelmap *src, br_rectangle *r)
{
	br_error result;

	if(LockSurface(self) != BRE_OK)
		return BRE_DEV_LOCK_FAIL;

	result = BR_CMETHOD(br_device_pixelmap_mem, rectangleCopyTo)(self, p, src, r);

	if(UnlockSurface(self) != BRE_OK)
		return BRE_DEV_LOCK_FAIL;

	return result;
}

/*
 * br_device_pixelmap_ati_dd::rectangleCopyFrom
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_ati_dd, rectangleCopyFrom)\
	(br_device_pixelmap *self, br_point *p, br_device_pixelmap *src, br_rectangle *r)
{
	br_error result;

	if(LockSurface(self) != BRE_OK)
		return BRE_DEV_LOCK_FAIL;

	result = BR_CMETHOD(br_device_pixelmap_mem, rectangleCopyFrom)(self, p, src, r);

	if(UnlockSurface(self) != BRE_OK)
		return BRE_DEV_LOCK_FAIL;

	return result;
}

/*
 * br_device_pixelmap_ati_dd::rectangleStretchCopy
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_ati_dd, rectangleStretchCopy)\
	(br_device_pixelmap *self, br_rectangle *d, br_device_pixelmap *src, br_rectangle *s)
{
	br_rectangle ss, dd;
	RECT srect, drect;

	/*
	 * Clip rectangles to pixelmaps
	 */
	if(PixelmapRectangleClip(&ss, s, (br_pixelmap *)src) == BR_CLIP_REJECT)
		return BRE_OK;

	if(PixelmapRectangleClip(&dd, d, (br_pixelmap *)self) == BR_CLIP_REJECT)
		return BRE_OK;

	/*
	 * Set the source and destination RECTs
	 */
	if(SetRect(&srect, ss.x, ss.y, ss.x + ss.w, ss.y + ss.h) == FALSE)
		return BRE_DEV_FAIL;

	if(SetRect(&drect, dd.x, dd.y, dd.x + dd.w, dd.y + dd.h) == FALSE)
		return BRE_DEV_FAIL;

	/*
	 * Blit the pixels
	 */
	return FunctionBlit(self, &drect, src, &srect);
}

/*
 * br_device_pixelmap_ati_dd::rectangleStretchCopyTo
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_ati_dd, rectangleStretchCopyTo)\
	(br_device_pixelmap *self, br_rectangle *d, br_device_pixelmap *src, br_rectangle *s)
{
	br_error result;

	if(LockSurface(self) != BRE_OK)
		return BRE_DEV_LOCK_FAIL;

	result = BR_CMETHOD(br_device_pixelmap_mem, rectangleStretchCopyTo)(self, d, src, s);

	if(UnlockSurface(self) != BRE_OK)
		return BRE_DEV_LOCK_FAIL;

	return result;
}

/*
 * br_device_pixelmap_ati_dd::rectangleStretchCopyFrom
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_ati_dd, rectangleStretchCopyFrom)\
	(br_device_pixelmap *self, br_rectangle *d, br_device_pixelmap *src, br_rectangle *s)
{
	br_error result;

	if(LockSurface(self) != BRE_OK)
		return BRE_DEV_LOCK_FAIL;

	result = BR_CMETHOD(br_device_pixelmap_mem, rectangleStretchCopyFrom)(self, d, src, s);

	if(UnlockSurface(self) != BRE_OK)
		return BRE_DEV_LOCK_FAIL;

	return result;
}

/*
 * br_device_pixelmap_ati_dd::pixelSet
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_ati_dd, pixelSet)\
	(br_device_pixelmap *self, br_point *p, br_uint_32 colour)
{
	br_error result;

	if(LockSurface(self) != BRE_OK)
		return BRE_DEV_LOCK_FAIL;

	result = BR_CMETHOD(br_device_pixelmap_mem, pixelSet)(self, p, colour);

	if(UnlockSurface(self) != BRE_OK)
		return BRE_DEV_LOCK_FAIL;

	return result;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_ati_dd, line)\
	(br_device_pixelmap *self, br_point *s, br_point *e, br_uint_32 colour)
{
	br_error result;

	if(LockSurface(self) != BRE_OK)
		return BRE_DEV_LOCK_FAIL;

	result = BR_CMETHOD(br_device_pixelmap_mem, line)(self, s, e, colour);

	if(UnlockSurface(self) != BRE_OK)
		return BRE_DEV_LOCK_FAIL;

	return result;
}

/*
 * br_device_pixelmap_ati_dd::copyBits
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_ati_dd, copyBits)\
	(br_device_pixelmap *self, br_point *point, br_uint_8 *src, br_uint_16 s_stride, br_rectangle *bit_rect, br_uint_32 colour)
{
	br_error result;

	if(LockSurface(self) != BRE_OK)
		return BRE_DEV_LOCK_FAIL;

	result = BR_CMETHOD(br_device_pixelmap_mem, copyBits)(self, point, src, s_stride, bit_rect, colour);

	if(UnlockSurface(self) != BRE_OK)
		return BRE_DEV_LOCK_FAIL;

	return result;
}

/*
 * br_device_pixelmap_ati_dd::fill
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_ati_dd, fill)\
	(br_device_pixelmap *self, br_uint_32 colour)
{
	DDBLTFX bltfx;

	/*
	 * Sub-pixelmaps can be filled too...
	 */
	if (self->pm_type != BR_PMT_DEPTH_16 && !self->parent) {

		br_rectangle rect;

		rect.x = - self->pm_origin_x;
		rect.y = - self->pm_origin_y;
		rect.w = self->pm_width;
		rect.h = self->pm_height;

		return(BR_CMETHOD(br_device_pixelmap_ati_dd, rectangleFill)(self, &rect, colour));
	}

	if (self->pm_type == BR_PMT_DEPTH_16) {
		/* Fill the surface using a blit: since this is a ZBUFFER surface this must
		* be a depthfill blit not a colourfill blit */
		bltfx.dwSize = sizeof(DDBLTFX);
		bltfx.dwFillDepth = (DWORD)colour;
		if(IDirectDrawSurface_Blt(self->surface, NULL, NULL, NULL, DDBLT_DEPTHFILL | DDBLT_WAIT, &bltfx) != DD_OK)
  			return BRE_DEV_FAIL;
	} else {
		/* Fill the surface using a blit */
		bltfx.dwSize = sizeof(DDBLTFX);
		bltfx.dwFillColor = (DWORD)colour;
		if(IDirectDrawSurface_Blt(self->surface, NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &bltfx) != DD_OK)
  			return BRE_DEV_FAIL;
	}
	return BRE_OK;
}

/*
 * br_device_pixelmap_ati_dd::rectangleFill
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_ati_dd, rectangleFill)\
	(br_device_pixelmap *self, br_rectangle *r, br_uint_32 colour)
{
	br_rectangle rr;
	RECT rect;
	DDBLTFX bltfx;
	
	/*
 	 * Clip rectangle to pixelmap
	 */
	if(PixelmapRectangleClip(&rr, r, (br_pixelmap *)self) == BR_CLIP_REJECT)
		return BRE_OK;

	/*
	 * Set destination RECT (CORRECTLY for sub-pixelmaps now)
	 */
	if(SetRect(&rect, self->pm_base_x + rr.x, self->pm_base_y + rr.y, self->pm_base_x + rr.x + rr.w, self->pm_base_y + rr.y + rr.h) == FALSE)
		return BRE_DEV_FAIL;

  if (self->pm_type == BR_PMT_DEPTH_16) {
    /* Fill the rectangle using a blit: since this is a ZBUFFER surface this must
     * be a depthfill blit not a colourfill blit */
	  bltfx.dwSize = sizeof(DDBLTFX);
	  bltfx.dwFillDepth = (DWORD)colour;
	  if(IDirectDrawSurface_Blt(self->surface, &rect, NULL, NULL, DDBLT_DEPTHFILL | DDBLT_WAIT, &bltfx) != DD_OK)
  		return BRE_DEV_FAIL;
  } else {
	  /* Fill the rectangle using a blit */
	  bltfx.dwSize = sizeof(DDBLTFX);
	  bltfx.dwFillColor = (DWORD)colour;
	  if(IDirectDrawSurface_Blt(self->surface, &rect, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &bltfx) != DD_OK)
  		return BRE_DEV_FAIL;
  }
 
	return BRE_OK;
}

/*
 * br_device_pixelmap_ati_dd::pixelQuery
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_ati_dd, pixelQuery)\
	(br_device_pixelmap *self, br_uint_32 *pcolour, br_point *p)
{
	br_error result;

	if(LockSurface(self) != BRE_OK)
		return BRE_DEV_LOCK_FAIL;

	result = BR_CMETHOD(br_device_pixelmap_mem, pixelQuery)(self, pcolour, p);

	if(UnlockSurface(self) != BRE_OK)
		return BRE_DEV_LOCK_FAIL;

	return result;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_ati_dd, fillDirty)\
	(br_device_pixelmap *self, br_uint_32 colour, br_rectangle *dirty, br_int_32 num_rects)
{
	int i;
	RECT rect;
	DDBLTFX bltfx;

	/*
	 * Initialise DDBLTFX structure
	 */
	bltfx.dwSize = sizeof(DDBLTFX);
	bltfx.dwFillColor = (DWORD)colour;

	/*
	 * Fill rectangles one by one
	 */
	for(i = 0; i < num_rects; i++, dirty++) {
		/*
		 * Set destination RECT
		 */
		SetRect(&rect, dirty->x, dirty->y, dirty->x + dirty->w, dirty->y + dirty->h);

		/*
		 * Fill dirty rectangle
		 */
		if(IDirectDrawSurface_Blt(self->surface, &rect, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &bltfx) != DD_OK)
			return BRE_DEV_FAIL;
	}

	return BRE_OK;
}

/*
 * br_device_pixelmap_ati_dd::match
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_ati_dd, match)\
	(br_device_pixelmap *self, br_device_pixelmap **newpm, br_token_value *tv)
{
	br_int_32 count;
	br_error result;
	struct match_tokens mt = {BRT_NONE};

	/*
	 * Get match type and details
	 */
	mt.width = self->pm_width;
	mt.height = self->pm_height;
	matchTemplate.res = DeviceATIResource(ObjectATIDevice(self));
	BrTokenValueSetMany(&mt, &count, NULL, tv, &matchTemplate);

	switch(mt.use) {
	case BRT_DEPTH:
	    result = ATIAllocateZ(self, newpm);
	    break;

   /* System memory device pixelmap */
   case BRT_NO_RENDER:
		return(ATIAllocateGenericOffscreen(self, newpm, mt.width, mt.height));
		break;

	case BRT_HIDDEN_BUFFER:
		/* Allocate a video memory buffer for texture rendering */
		return(ATIAllocateVideoMemory(self, newpm, mt.width, mt.height));
		break;

	case BRT_OFFSCREEN:
	case BRT_HIDDEN:
	case BRT_CLONE:

		return(BRE_DEV_UNSUPPORTED);
		break;

	default:
		result = BRE_DEV_UNSUPPORTED;
	}

	return result;
}

/*
 * Dispatch table for DirectDraw offscreen device pixelmaps
 */
static struct br_device_pixelmap_dispatch devicePixelmapDispatch_dd = {
	NULL,
	NULL,
	NULL,
	NULL,
	BR_CMETHOD_REF(br_device_pixelmap_ati_dd,	free),
	BR_CMETHOD_REF(br_object_ati,		  		   identifier),
	BR_CMETHOD_REF(br_device_pixelmap_ati,		type),
	BR_CMETHOD_REF(br_device_pixelmap_ati,		isType),
	BR_CMETHOD_REF(br_object_ati,		  		   device),
	BR_CMETHOD_REF(br_device_pixelmap_ati,		space),

	BR_CMETHOD_REF(br_device_pixelmap_ati,	   queryTemplate),
	BR_CMETHOD_REF(br_object,					   query),
	BR_CMETHOD_REF(br_object,					   queryBuffer),
	BR_CMETHOD_REF(br_object,					   queryMany),
	BR_CMETHOD_REF(br_object,					   queryManySize),
	BR_CMETHOD_REF(br_object,					   queryAll),
	BR_CMETHOD_REF(br_object,					   queryAllSize),

	BR_CMETHOD_REF(br_device_pixelmap_mem,		validSource),
	BR_CMETHOD_REF(br_device_pixelmap_ati_dd,	resize),
	BR_CMETHOD_REF(br_device_pixelmap_ati_dd,	match),
	BR_CMETHOD_REF(br_device_pixelmap_ati_dd,	allocateSub),

	BR_CMETHOD_REF(br_device_pixelmap_ati_dd,	copy),
	BR_CMETHOD_REF(br_device_pixelmap_gen,		copyTo),
	BR_CMETHOD_REF(br_device_pixelmap_gen,		copyFrom),
	BR_CMETHOD_REF(br_device_pixelmap_ati_dd,	fill),
	BR_CMETHOD_REF(br_device_pixelmap_gen,		doubleBuffer),

	BR_CMETHOD_REF(br_device_pixelmap_gen,		copyDirty),
	BR_CMETHOD_REF(br_device_pixelmap_gen,		copyToDirty),
	BR_CMETHOD_REF(br_device_pixelmap_gen,		copyFromDirty),
	BR_CMETHOD_REF(br_device_pixelmap_ati_dd,	fillDirty),
	BR_CMETHOD_REF(br_device_pixelmap_gen,		doubleBufferDirty),

	BR_CMETHOD_REF(br_device_pixelmap_gen,		rectangle),
	BR_CMETHOD_REF(br_device_pixelmap_gen,		rectangle2),
	BR_CMETHOD_REF(br_device_pixelmap_ati_dd,	rectangleCopy),
	BR_CMETHOD_REF(br_device_pixelmap_ati_dd,	rectangleCopyTo),
	BR_CMETHOD_REF(br_device_pixelmap_ati_dd,	rectangleCopyFrom),
	BR_CMETHOD_REF(br_device_pixelmap_ati_dd,	rectangleStretchCopy),
	BR_CMETHOD_REF(br_device_pixelmap_ati_dd,	rectangleStretchCopyTo),
	BR_CMETHOD_REF(br_device_pixelmap_ati_dd,	rectangleStretchCopyFrom),
	BR_CMETHOD_REF(br_device_pixelmap_ati_dd,	rectangleFill),
	BR_CMETHOD_REF(br_device_pixelmap_ati_dd,	pixelSet),
	BR_CMETHOD_REF(br_device_pixelmap_ati_dd,	line),
	BR_CMETHOD_REF(br_device_pixelmap_ati_dd,	copyBits),

	BR_CMETHOD_REF(br_device_pixelmap_gen,		text),
	BR_CMETHOD_REF(br_device_pixelmap_gen,		textBounds),

	BR_CMETHOD_REF(br_device_pixelmap_mem,		rowSize),
	BR_CMETHOD_REF(br_device_pixelmap_mem,		rowQuery),
	BR_CMETHOD_REF(br_device_pixelmap_mem,		rowSet),

	BR_CMETHOD_REF(br_device_pixelmap_ati_dd,	pixelQuery),
	BR_CMETHOD_REF(br_device_pixelmap_mem,		pixelAddressQuery),

	BR_CMETHOD_REF(br_device_pixelmap_mem,		pixelAddressSet),
	BR_CMETHOD_REF(br_device_pixelmap_mem,		originSet),

	BR_CMETHOD_REF(br_device_pixelmap_mem,		flush),

	BR_CMETHOD_REF(br_device_pixelmap_mem,		synchronise),

	BR_CMETHOD_REF(br_device_pixelmap_ati_dd,	directLock),
	BR_CMETHOD_REF(br_device_pixelmap_ati_dd, directUnlock),	

	BR_CMETHOD_REF(br_device_pixelmap_gen,	getControls),
	BR_CMETHOD_REF(br_device_pixelmap_gen,	setControls),
};


br_boolean CheckBackDispatch(void *dispatch)
{
	return(dispatch == &devicePixelmapDispatch_dd);
}


