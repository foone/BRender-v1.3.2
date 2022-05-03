/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: devpmddo.c 1.1 1997/12/10 16:45:13 jon Exp $
 * $Locker: $
 *
 * Device pixelmap methods - DirectDraw offscreen
 */
#define WIN32_LEAN_AND_MEAN
#include <stddef.h>
#include <string.h>
#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>

#include "drv.h"
#include "pm.h"
#include "host.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: devpmddo.c 1.1 1997/12/10 16:45:13 jon Exp $");

/*
 * Default dispatch table for device pixelmap (defined at end of file)
 */
static struct br_device_pixelmap_dispatch devicePixelmapDispatch_ddo;


/*
 * Lock surface
 */
static br_error LockSurface(br_device_pixelmap *self)
{
	DDSURFACEDESC ddsd;
	POINT pt;
	RECT rect ;
  
	UASSERT(self != NULL);

	memset(&ddsd, 0, sizeof(DDSURFACEDESC));
	ddsd.dwSize = sizeof(DDSURFACEDESC);

	if (self->bPrimary) {

		pt.x = pt.y = 0;
		ClientToScreen(self->hwnd, &pt);

		SetRect(&rect, pt.x, pt.y, pt.x + self->pm_width, pt.y + self->pm_height);

		if (IDirectDrawSurface_Lock(self->surface, &rect, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL) != DD_OK)
			return BRE_FAIL;

	} else

		if (IDirectDrawSurface_Lock(self->surface, NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL) != DD_OK)
			return BRE_FAIL;

	/*
	 * Set device pixelmap members
	 */
	self->bits = ddsd.lpSurface;
	self->pm_pixels = (char *)self->bits;
	self->pm_row_bytes = (br_uint_16) ddsd.lPitch;

	return BRE_OK;
}

/*
 * Unlock surface
 */
static br_error UnlockSurface(br_device_pixelmap *self)
{
	UASSERT(self != NULL);

	if (IDirectDrawSurface_Unlock(self->surface, self->bits) != DD_OK)
		return BRE_FAIL;

	/*
	 * Set device pixelmap members
	 */
	self->bits = NULL;
	self->pm_pixels = NULL;

	return BRE_OK;
}


int BrDD_PixelmapType_BPP( int type )
{
	switch (type) {

	case BR_PMT_INDEX_8 : return 8 ;
	case BR_PMT_RGB_555 : return 16 ;
	case BR_PMT_RGB_565 : return 16 ;
	case BR_PMT_RGB_888 : return 24 ;
	case BR_PMT_RGBX_888 : return 32 ;
	case BR_PMT_RGBA_8888 : return 32 ;
	}

	return 0 ;
}



void BrDD_SetPixelFormat( LPDDSURFACEDESC ddsd, int type )
{
   ddsd->ddpfPixelFormat.dwSize   = sizeof(DDPIXELFORMAT);
   ddsd->ddpfPixelFormat.dwRGBBitCount = BrDD_PixelmapType_BPP(type);
   
   switch( type )
   {
        case BR_PMT_INDEX_8 :

            ddsd->ddpfPixelFormat.dwFlags  = DDPF_PALETTEINDEXED8|DDPF_RGB ; 
           break ;

        case BR_PMT_RGB_555 :
             
            // Pixel format is RGB-555

            ddsd->ddpfPixelFormat.dwFlags  = DDPF_RGB ; 
            ddsd->ddpfPixelFormat.dwFourCC = BI_RGB ;
            ddsd->ddpfPixelFormat.dwRBitMask = 0x7c00 ; 
            ddsd->ddpfPixelFormat.dwGBitMask = 0x03e0; 
            ddsd->ddpfPixelFormat.dwBBitMask = 0x001f ; 
          break ;

        case BR_PMT_RGB_565 :
             
            // Pixel format is RGB-565

            ddsd->ddpfPixelFormat.dwFlags  = DDPF_RGB ; 
            ddsd->ddpfPixelFormat.dwFourCC = BI_RGB ;
            ddsd->ddpfPixelFormat.dwRBitMask = 0xf800 ; 
            ddsd->ddpfPixelFormat.dwGBitMask = 0x07e0; 
            ddsd->ddpfPixelFormat.dwBBitMask = 0x001f ; 
          break ;

       default : BR_ERROR0("Unsupported BRender pixel format. Sorry.");
    }            
}





/*
 * Allocate a DirectDraw offscreen surface
 */
br_error BufferDirectDrawAllocateOff(br_device_pixelmap *self, br_device_pixelmap **newpm, int w, int h, int type )
{
	int width;
	br_uint_16 qual;
	br_device *dev;
	br_device_pixelmap *devpm;
	LPDIRECTDRAWSURFACE surface;
	DDSURFACEDESC ddsd;

	UASSERT(self != NULL);
	UASSERT(newpm != NULL);
	UASSERT((w > 0) && (h > 0));

	/*
	 * Get pointer to device
	 */
	dev = self->device ;

	/*
	 * Ensure rows are long-aligned
	 */
	width = w ; //ALIGN4(w);

	/*
	 * Create an offscreen surface
	 */
	memset(&ddsd, 0, sizeof(DDSURFACEDESC));
	ddsd.dwSize = sizeof(DDSURFACEDESC);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	ddsd.dwWidth = width;
	ddsd.dwHeight = h;

	BrDD_SetPixelFormat( &ddsd, type );

	if(IDirectDraw2_CreateSurface(self->DD, &ddsd, &surface, NULL) != DD_OK)
		return BRE_FAIL;

	/*
	 * Allocate new pixelmap
	 */
	devpm = BrResAllocate( dev->res, sizeof(*devpm), BR_MEMORY_OBJECT );

	/*
	 * Fill members
	 */	
	*devpm = *self;
	devpm->dispatch = &devicePixelmapDispatch_ddo;
	devpm->pm_width = w;
	devpm->pm_height = h;
	devpm->surface = surface;
	devpm->device = dev ;
	HostSelectorDS(&qual);
	devpm->pm_pixels_qualifier = qual;
	*newpm = devpm;
	devpm->bPrimary = BR_FALSE ;


	/*
	 * Lock then immediately unlock to get row bytes
	 */
	LockSurface(devpm);
	UnlockSurface(devpm);

	return BRE_OK;
}

/*
 * br_device_pixelmap_ddo::directLock
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, directLock)(br_device_pixelmap *self, br_boolean block)
{
   if(block == BR_FALSE)
		return BRE_OK;

	return LockSurface(self);
}

br_device *	BR_CMETHOD_DECL(br_device_pixelmap_ddo, device)(br_device_pixelmap *self)
{
	return self->device;
}

/*
 * br_device_pixelmap_ddo::directUnlock
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, directUnlock)(br_device_pixelmap *self)
{
	return UnlockSurface(self);
}

/*
 * br_device_pixelmap_ddo::free
 */
static void BR_CMETHOD_DECL(br_device_pixelmap_ddo, free)(br_device_pixelmap *self)
{
   UASSERT(self != NULL);

	/*
	 * Release surface
	 */
	if (!self->sub_pixelmap && self->surface != NULL) {

		IDirectDrawSurface_Release(self->surface);
		self->surface = NULL ;
	}


	/*
	 * Free pixelmap structure
	 */
	BrResFreeNoCallback(self);
}

/*
 * br_device_pixelmap_ddo::resize
 */
static br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, resize)(br_device_pixelmap *self, br_int_32 width, br_int_32 height)
{
	HRESULT result;
	DDSURFACEDESC ddsd;

	/*
	 * Release surface
	 */
	IDirectDrawSurface_Release(self->surface);

	/*
	 * Create a new surface
	 */
	memset(&ddsd, 0, sizeof(DDSURFACEDESC));
	ddsd.dwSize = sizeof(DDSURFACEDESC);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    ddsd.dwWidth = width ; //ALIGN4(width);
	ddsd.dwHeight = height;
	result = IDirectDraw2_CreateSurface(self->DD, &ddsd, &self->surface, NULL);
	if(result != DD_OK)
		return BRE_FAIL;

	/*
	 * Adjust device pixelmap members
	 */
	self->pm_width  = (br_uint_16) width;
	self->pm_height = (br_uint_16) height;

	/*
	 * Lock then immediately unlock to get row bytes
	 */
	LockSurface(self);
	UnlockSurface(self);

	return BRE_OK;
}

/*
 * br_device_pixelmap_ddo::allocateSub
 */
static br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, allocateSub)(br_device_pixelmap *self, br_device_pixelmap **newpm, br_rectangle *rect)
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
	pm = BrResAllocate( self->device->res, sizeof(*pm), BR_MEMORY_OBJECT );

	*pm = *self;

	pm->pm_base_x += (br_uint_16)out.x;
	pm->pm_base_y += (br_uint_16)out.y;
	pm->pm_width = (br_uint_16)out.w;
	pm->pm_height = (br_uint_16)out.h;

	pm->pm_origin_x = 0;
	pm->pm_origin_y = 0;

	/*
	 * Pixel rows may not be contiguous
	 */
	if(self->pm_width != pm->pm_width)
		pm->pm_flags &= ~BR_PMF_LINEAR;

	pm->sub_pixelmap = BR_TRUE;

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
   HRESULT ddrval ;

	/*
	 * Set defaults
	 */
	bltfx_ptr = NULL;
	flags = DDBLT_WAIT;
	bltfx.dwSize = sizeof(DDBLTFX);

	/*
	 * Source colour key
	 */
	if(src->pm_copy_function == BR_PMCOPY_SRC_KEYED) 
   {
		bltfx.ddckSrcColorkey.dwColorSpaceLowValue = (DWORD)src->pm_src_key.low;
		bltfx.ddckSrcColorkey.dwColorSpaceHighValue = (DWORD)src->pm_src_key.high;
		flags |= DDBLT_KEYSRCOVERRIDE;
		bltfx_ptr = &bltfx;
	}

	/*
	 * Destination colour key
	 */
	if(dst->pm_copy_function == BR_PMCOPY_DST_KEYED) 
   {
		bltfx.ddckDestColorkey.dwColorSpaceLowValue = (DWORD)dst->pm_dst_key.low;
		bltfx.ddckDestColorkey.dwColorSpaceHighValue = (DWORD)dst->pm_dst_key.high;
		flags |= DDBLT_KEYDESTOVERRIDE;
		bltfx_ptr = &bltfx;
	}

	/*
	 * Perform the blit
	 */

   while( 1 )
   {
     	ddrval = IDirectDrawSurface_Blt(dst->surface, drect, src->surface, srect, flags, bltfx_ptr);
     
      if( ddrval == DD_OK )
           break;

      if ( ddrval == DDERR_SURFACELOST )
      {
           ddrval = dst->surface->lpVtbl->Restore(dst->surface);

           if ( ddrval != DD_OK )
              break ;

           ddrval = src->surface->lpVtbl->Restore(src->surface);

           if ( ddrval != DD_OK )
              break ;
		}

      if ( ddrval != DDERR_WASSTILLDRAWING )
      {
          break;
      }
   }

  	if ( ddrval == DD_OK )
     	return BRE_OK;
   else
      return BRE_FAIL ;

	return BRE_OK;
}

/*
 * br_device_pixelmap_ddo::copy
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, copy)\
	(br_device_pixelmap *self, br_device_pixelmap *src)
{
	RECT srect, drect;

	/*
	 * Set the source and destination RECTs
	 */
	if (!SetRect(&srect, src->pm_base_x, src->pm_base_y, src->pm_base_x + src->pm_width, src->pm_base_y + src->pm_height) ||
		!SetRect(&drect, self->pm_base_x, self->pm_base_y, self->pm_width, self->pm_height))
		return BRE_FAIL;

	/*
	 * Blit the pixels
	 */
	return FunctionBlit(self, &drect, src, &srect);	
}

/*
 * br_device_pixelmap_ddo::rectangleCopy
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, rectangleCopy)\
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
	if (!SetRect(&srect, src->pm_base_x + rr.x, src->pm_base_y + rr.y,
			src->pm_base_x + rr.x + rr.w, src->pm_base_y + rr.y + rr.h) ||
		!SetRect(&drect, self->pm_base_x + rr.x, self->pm_base_y + rr.y,
			self->pm_base_x + pp.x + rr.w, self->pm_base_y + pp.y + rr.h))
		return BRE_FAIL;

	/*
	 * Blit the pixels
	 */
	return FunctionBlit(self, &drect, src, &srect);	
}

/*
 * br_device_pixelmap_ddo::rectangleCopyTo
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, rectangleCopyTo)\
	(br_device_pixelmap *self, br_point *p, br_device_pixelmap *src, br_rectangle *r)
{
	br_error result;

	if(LockSurface(self) != BRE_OK)
		return BRE_FAIL;

	result = BR_CMETHOD(br_device_pixelmap_mem, rectangleCopyTo)(self, p, src, r);

	if(UnlockSurface(self) != BRE_OK)
		return BRE_FAIL;

	return result;
}

/*
 * br_device_pixelmap_ddo::rectangleCopyFrom
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, rectangleCopyFrom)\
	(br_device_pixelmap *self, br_point *p, br_device_pixelmap *src, br_rectangle *r)
{
	br_error result;

	if(LockSurface(self) != BRE_OK)
		return BRE_FAIL;

	result = BR_CMETHOD(br_device_pixelmap_mem, rectangleCopyFrom)(self, p, src, r);

	if(UnlockSurface(self) != BRE_OK)
		return BRE_FAIL;

	return result;
}

/*
 * br_device_pixelmap_ddo::rectangleStretchCopy
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, rectangleStretchCopy)\
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
	if (!SetRect(&srect, src->pm_base_x + ss.x, src->pm_base_y + ss.y,
			src->pm_base_x + ss.x + ss.w, src->pm_base_y + ss.y + ss.h) ||
		!SetRect(&drect, self->pm_base_x + dd.x, self->pm_base_y + dd.y,
			self->pm_base_x + dd.x + dd.w, self->pm_base_y + dd.y + dd.h))
		return BRE_FAIL;

	/*
	 * Blit the pixels
	 */
	return FunctionBlit(self, &drect, src, &srect);
}

/*
 * br_device_pixelmap_ddo::rectangleStretchCopyTo
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, rectangleStretchCopyTo)\
	(br_device_pixelmap *self, br_rectangle *d, br_device_pixelmap *src, br_rectangle *s)
{
	br_error result;

	if(LockSurface(self) != BRE_OK)
		return BRE_FAIL;

	result = BR_CMETHOD(br_device_pixelmap_mem, rectangleStretchCopyTo)(self, d, src, s);

	if(UnlockSurface(self) != BRE_OK)
		return BRE_FAIL;

	return result;
}

/*
 * br_device_pixelmap_ddo::rectangleStretchCopyFrom
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, rectangleStretchCopyFrom)\
	(br_device_pixelmap *self, br_rectangle *d, br_device_pixelmap *src, br_rectangle *s)
{
	br_error result;

	if(LockSurface(self) != BRE_OK)
		return BRE_FAIL;

	result = BR_CMETHOD(br_device_pixelmap_mem, rectangleStretchCopyFrom)(self, d, src, s);

	if(UnlockSurface(self) != BRE_OK)
		return BRE_FAIL;

	return result;
}


/*
 * br_device_pixelmap_ddo::pixelSet
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, pixelSet)\
	(br_device_pixelmap *self, br_point *p, br_uint_32 colour)
{
	br_error result;

	if(LockSurface(self) != BRE_OK)
		return BRE_FAIL;

	result = BR_CMETHOD(br_device_pixelmap_mem, pixelSet)(self, p, colour);

	if(UnlockSurface(self) != BRE_OK)
		return BRE_FAIL;

	return result;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, line)\
	(br_device_pixelmap *self, br_point *s, br_point *e, br_uint_32 colour)
{
	br_error result;

	if(LockSurface(self) != BRE_OK)
		return BRE_FAIL;

	result = BR_CMETHOD(br_device_pixelmap_mem, line)(self, s, e, colour);

	if(UnlockSurface(self) != BRE_OK)
		return BRE_FAIL;

	return result;
}

/*
 * br_device_pixelmap_ddo::copyBits
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, copyBits)\
	(br_device_pixelmap *self, br_point *point, br_uint_8 *src, br_uint_16 s_stride, br_rectangle *bit_rect, br_uint_32 colour)
{
	br_error result;

	if(LockSurface(self) != BRE_OK)
		return BRE_FAIL;

	result = BR_CMETHOD(br_device_pixelmap_mem, copyBits)(self, point, src, s_stride, bit_rect, colour);

	if(UnlockSurface(self) != BRE_OK)
		return BRE_FAIL;

	return result;
}

/*
 * br_device_pixelmap_ddo::fill
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, fill)\
	(br_device_pixelmap *self, br_uint_32 colour)
{
	HRESULT ddrval ;
	RECT rect;
	DDBLTFX bltfx;

	if (self->sub_pixelmap && !SetRect(&rect, self->pm_base_x, self->pm_base_y,
			self->pm_base_x + self->pm_width, self->pm_base_y + self->pm_height))
		return BRE_FAIL;

	/*
	 * Fill the surface using a blit
	 */
	bltfx.dwSize = sizeof(DDBLTFX);
	bltfx.dwFillColor = (DWORD)colour;

	while (1) {
 
		if (self->sub_pixelmap)
			ddrval = IDirectDrawSurface_Blt(self->surface, &rect, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &bltfx);
		else
			ddrval = IDirectDrawSurface_Blt(self->surface, NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &bltfx);

		if (ddrval == DD_OK)
           break;

		if (ddrval == DDERR_SURFACELOST) {

			ddrval = self->surface->lpVtbl->Restore(self->surface);

			if (ddrval != DD_OK)
				break;
		}

		if (ddrval != DDERR_WASSTILLDRAWING)
			break;
	}

	if (ddrval == DD_OK)
		return BRE_OK;
	else
		return BRE_FAIL ;
}

/*
 * br_device_pixelmap_ddo::rectangleFill
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, rectangleFill)\
	(br_device_pixelmap *self, br_rectangle *r, br_uint_32 colour)
{
	br_rectangle rr;
	RECT rect;
	DDBLTFX bltfx;
	HRESULT ddrval ;
	
	/*
 	 * Clip rectangle to pixelmap
	 */
	if(PixelmapRectangleClip(&rr, r, (br_pixelmap *)self) == BR_CLIP_REJECT)
		return BRE_OK;

	/*
	 * Set destination RECT
	 */
	if (!SetRect(&rect, self->pm_base_x + rr.x, self->pm_base_y + rr.y,
			self->pm_base_x + rr.x + rr.w, self->pm_base_y + rr.y + rr.h))
		return BRE_FAIL;

	/*
	 * Fill the rectangle using a blit
	 */
	bltfx.dwSize = sizeof(DDBLTFX);
	bltfx.dwFillColor = (DWORD)colour;

	while (1) {

		ddrval = IDirectDrawSurface_Blt(self->surface, &rect, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &bltfx);     

		if (ddrval == DD_OK)
			break;

		if (ddrval == DDERR_SURFACELOST) {

			ddrval = self->surface->lpVtbl->Restore(self->surface);

			if (ddrval != DD_OK)
				break ;
    	}

		if (ddrval != DDERR_WASSTILLDRAWING)
			break;
	}

  	if (ddrval == DD_OK)
     	return BRE_OK;
	else
		return BRE_FAIL ;
}

/*
 * br_device_pixelmap_ddo::pixelQuery
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, pixelQuery)\
	(br_device_pixelmap *self, br_uint_32 *pcolour, br_point *p)
{
	br_error result;

	if(LockSurface(self) != BRE_OK)
		return BRE_FAIL;

	result = BR_CMETHOD(br_device_pixelmap_mem, pixelQuery)(self, pcolour, p);

	if(UnlockSurface(self) != BRE_OK)
		return BRE_FAIL;

	return result;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, fillDirty)\
	(br_device_pixelmap *self, br_uint_32 colour, br_rectangle *dirty, br_int_32 num_rects)
{
	int i;
	RECT rect;
	DDBLTFX bltfx;
	HRESULT ddrval ;

	/*
	 * Initialise DDBLTFX structure
	 */
	bltfx.dwSize = sizeof(DDBLTFX);
	bltfx.dwFillColor = (DWORD)colour;

	/*
	 * Fill rectangles one by one
	 */
	for (i = 0; i < num_rects; i++, dirty++) {

		/*
		 * Set destination RECT
		 */
		SetRect(&rect, self->pm_base_x + dirty->x, self->pm_base_y + dirty->y, self->pm_base_x + dirty->x + dirty->w, self->pm_base_y + dirty->y + dirty->h);

		/*
		 * Fill dirty rectangle
		 */
		while (1) {

			ddrval = IDirectDrawSurface_Blt(self->surface, &rect, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &bltfx);

			if (ddrval == DD_OK)
				break;

			if (ddrval == DDERR_SURFACELOST)
				ddrval = self->surface->lpVtbl->Restore(self->surface);

			if (ddrval != DD_OK)
				break;
		}

		if (ddrval != DDERR_WASSTILLDRAWING)
			break;
	}

  	if (ddrval == DD_OK)
		return BRE_OK;
	else
		return BRE_FAIL ;
}


br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, flush)(
	struct br_device_pixelmap *self)
{
   return BRE_OK;
}



/*
 * Structures used to translate tokens and values
 */
struct match_off_tokens {
   br_token   use;
   br_int_32   pixel_bits;
   br_int_32   width;
   br_int_32   height;
   br_uint_8   type ;
};

#define F(f)	offsetof(struct match_off_tokens, f)

static struct br_tv_template_entry matchOffTemplateEntries[] = {
   {BRT_USE_T,           0,   F(use),               BRTV_SET,   BRTV_CONV_COPY, },
   {BRT_PIXEL_BITS_I32,  0,   F(pixel_bits),         BRTV_SET,   BRTV_CONV_COPY, },
   {BRT_WIDTH_I32,       0,   F(width),            BRTV_SET,   BRTV_CONV_COPY, },
   {BRT_HEIGHT_I32,      0,   F(height),            BRTV_SET,   BRTV_CONV_COPY, },
   {BRT_PIXEL_TYPE_U8,   0,   F(type),            BRTV_SET,   BRTV_CONV_COPY, },
};
#undef F


/*
 * br_device_pixelmap_ddo::match
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, match)\
	(br_device_pixelmap *self, br_device_pixelmap **newpm, br_token_value *tv)
{
	br_int_32 count;
	br_error result;
	struct match_off_tokens mt = {BRT_NONE};

   // Create offscreen match template, if required ...

   if ( self->device->templates.matchOffTemplate == NULL )
   {
        self->device->templates.matchOffTemplate = BrTVTemplateAllocate(self->device->res,
            matchOffTemplateEntries,
            BR_ASIZE(matchOffTemplateEntries));
   }

   // Default width and height

   mt.width = self->pm_width;
   mt.height = self->pm_height;
   mt.type = self->pm_type ;
 
	/*
	 * Get match type
	 */

	BrTokenValueSetMany(&mt, &count, NULL, tv,  self->device->templates.matchOffTemplate );

	switch(mt.use) {

	case BRT_CLONE:
	case BRT_HIDDEN:
	case BRT_HIDDEN_BUFFER:
	case BRT_NO_RENDER:
	case BRT_BUFFER:
	case BRT_OFFSCREEN:

		/*
		 * Clone the device pixelmap
		 */
		result = BufferDirectDrawAllocateOff(self, newpm, mt.width, mt.height, mt.type );
		break;

	case BRT_DEPTH:

		/*
		 * Revert to memory method
		 */
		result = BR_CMETHOD(br_device_pixelmap_mem, match)(self, newpm, tv);
		break;

	break;

	default:
		result = BRE_FAIL;
	}

	return result;
}

/*
 * Dispatch table for DirectDraw offscreen device pixelmaps
 */
static struct br_device_pixelmap_dispatch devicePixelmapDispatch_ddo = {
	NULL,
	NULL,
	NULL,
	NULL,
	BR_CMETHOD_REF(br_device_pixelmap_ddo,	free),
	BR_CMETHOD_REF(br_object_dd,			identifier),
	BR_CMETHOD_REF(br_device_pixelmap_dd,	type),
	BR_CMETHOD_REF(br_device_pixelmap_dd,	isType),
	BR_CMETHOD_REF(br_device_pixelmap_ddo,	device),
	BR_CMETHOD_REF(br_device_pixelmap_dd,	space),

	BR_CMETHOD_REF(br_device_pixelmap_dd,	queryTemplate),
	BR_CMETHOD_REF(br_object,				query),
	BR_CMETHOD_REF(br_object,				queryBuffer),
	BR_CMETHOD_REF(br_object,				queryMany),
	BR_CMETHOD_REF(br_object,				queryManySize),
	BR_CMETHOD_REF(br_object,				queryAll),
	BR_CMETHOD_REF(br_object,				queryAllSize),

	BR_CMETHOD_REF(br_device_pixelmap_mem,	validSource),
	BR_CMETHOD_REF(br_device_pixelmap_ddo,	resize),
	BR_CMETHOD_REF(br_device_pixelmap_ddo,	match),
	BR_CMETHOD_REF(br_device_pixelmap_ddo,	allocateSub),

	BR_CMETHOD_REF(br_device_pixelmap_ddo,	copy),
	BR_CMETHOD_REF(br_device_pixelmap_gen,	copyTo),
	BR_CMETHOD_REF(br_device_pixelmap_gen,	copyFrom),
	BR_CMETHOD_REF(br_device_pixelmap_ddo,	fill),
	BR_CMETHOD_REF(br_device_pixelmap_gen,	doubleBuffer),

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
};



