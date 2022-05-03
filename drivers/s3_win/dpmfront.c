/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: dpmfront.c 1.2 1998/10/21 15:18:22 jon Exp $
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

BR_RCS_ID("$Id: dpmfront.c 1.2 1998/10/21 15:18:22 jon Exp $");



static int allocated_one_offscreen;

/*
 * Default dispatch table for device pixelmap (defined at end of file)
 */
static struct br_device_pixelmap_dispatch devicePixelmapDispatch;



extern br_error S3AllocateBack(br_device_pixelmap *self, br_device_pixelmap **newpm, int w, int h);
extern br_error S3AllocateGenericOffscreen(br_device_pixelmap *self, br_device_pixelmap **newpm, int w, int h);

/*
 * Allocate DirectDraw front surfaces
 *
 * Two separate methods for front and back surface allocation, because the
 * two cases are vastly different. The fullscreen case sets up video memory
 * for both a front and back flipping surface. The non-fullscreen case
 * allocates the front buffer which in general will be the whole window.
 */

LPDIRECTDRAWSURFACE back_surface_s3 = NULL;
br_boolean back_surface_used_s3 = BR_FALSE;

br_error S3SetupFullscreenMemory(LPDIRECTDRAWSURFACE *surface)
{
	HRESULT result;
	DDSURFACEDESC ddsd;
   DDSCAPS ddsCaps;

   OUTDBS("Setup fullscreen memory\n");
  /* Create a primary surface with one back buffer */
	memset(&ddsd, 0, sizeof(DDSURFACEDESC));
	ddsd.dwSize = sizeof(DDSURFACEDESC);
	ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_COMPLEX | DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_VIDEOMEMORY;
  ddsd.dwBackBufferCount = 1;
	if ((result = IDirectDraw_CreateSurface(gpDD, &ddsd, surface, NULL)) != DD_OK)
		return BRE_FAIL;

  /* Pick up the back buffer and store to be collected later */
  memset(&ddsCaps, 0, sizeof(DDSCAPS));
  ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
  if (IDirectDrawSurface_GetAttachedSurface(*surface, &ddsCaps, &back_surface_s3) != DD_OK)
    return(BRE_FAIL);

  back_surface_used_s3 = BR_FALSE;

  OUTDBS("Setup fullscreen memory success\n");

  return(BRE_OK);
}


br_error S3SetupNonFullscreenFront(LPDIRECTDRAWSURFACE *surface)
{
	HRESULT result;
	DDSURFACEDESC ddsd;

  /* Pick up the Windows primary surface only */
	memset(&ddsd, 0, sizeof(DDSURFACEDESC));
	ddsd.dwSize = sizeof(DDSURFACEDESC);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	if ((result = IDirectDraw_CreateSurface(gpDD, &ddsd, surface, NULL)) != DD_OK)
		return BRE_FAIL;

  return(BRE_OK);
}




/*
 * Device pixelmap info. template
 */
#define F(f)	offsetof(struct br_device_pixelmap, f)

static struct br_tv_template_entry devicePixelmapTemplateEntries[] = {
	{BRT_WIDTH_I32,			  0,	F(pm_width), 		    BRTV_QUERY | BRTV_ALL,	BRTV_CONV_I32_U16, },
	{BRT_HEIGHT_I32,		  0,	F(pm_height),		    BRTV_QUERY | BRTV_ALL,	BRTV_CONV_I32_U16, },
	{BRT_PIXEL_TYPE_U8,		0,	F(pm_type),			    BRTV_QUERY | BRTV_ALL,	BRTV_CONV_I32_U8, },
	{BRT_OUTPUT_FACILITY_O,0,	F(output_facility),	BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT_IDENTIFIER_CSTR,	0,	F(pm_identifier), 	BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT_CLUT_O,          0,	F(clut),			      BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT_WINDOW_HANDLE_H,	0,	F(hwnd),		      	BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
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
	{BRT(WIDTH_I32),     		      F(width),         BRTV_SET,	BRTV_CONV_COPY, },
	{BRT(HEIGHT_I32),             F(height),        BRTV_SET,	BRTV_CONV_COPY, },
	{BRT(PIXEL_BITS_I32), 		   	F(pixel_bits),    BRTV_SET,	BRTV_CONV_COPY, },
	{BRT(WINDOW_FULLSCREEN_B),    F(fullscreen),    BRTV_SET,	BRTV_CONV_COPY, },
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
	br_int_32 pixel_bits;
	br_int_32 width;
	br_int_32 height;
  br_object *renderer;
};

#define F(f)	offsetof(struct match_tokens, f)

static struct br_tv_template_entry matchTemplateEntries[] = {
	{BRT_USE_T,           0,  F(use),         BRTV_SET,	BRTV_CONV_COPY, },
	{BRT_PIXEL_BITS_I32,  0,  F(pixel_bits),  BRTV_SET,	BRTV_CONV_COPY, },
	{BRT_WIDTH_I32,       0,  F(width),       BRTV_SET,	BRTV_CONV_COPY, },
	{BRT_HEIGHT_I32,      0,  F(height),      BRTV_SET,	BRTV_CONV_COPY, },
  {BRT_RENDERER_O,      0,  F(renderer),    BRTV_SET, BRTV_CONV_COPY, },
};

#undef F

static struct br_tv_template matchTemplate = {
	BR_ASIZE(matchTemplateEntries),
	matchTemplateEntries
};



void ClearDevpmFrontTemplates(void)
{
	CLEAR_TEMPLATE(devicePixelmap);
	CLEAR_TEMPLATE(match);
	CLEAR_TEMPLATE(hwnd);
	CLEAR_TEMPLATE(new);
}

/* Base address of video memory, set up when S3 libraries are instantiated */

extern br_uint_32 video_memory_base;

/* This is the flag used to decide what the driver behaviour is with 8 bit textures */
br_boolean S3_convert_8bit_to_15 = BR_FALSE;

/*
 * Create a new device pixelmap
 */
br_device_pixelmap * DevicePixelmapS3Allocate(br_device *dev, br_output_facility *facility, br_token_value *tv)
{
	br_error result;
	br_token method;
	br_uint_8 type;
	br_int_32 count;
	br_device_pixelmap *self;
	struct new_tokens nt;
	struct hwnd_tokens ht;
	br_uint_8 *pixel_address;
	br_uint_32 pixel_bits;
   PALETTEENTRY *entries;
	RECT rect;

	UASSERT(dev != NULL);
	UASSERT(facility != NULL);
	UASSERT(tv != NULL);

   OUTDBS("Device Pixelmap S3 Allocate\n");

	/*
	 * Get window handle
	 */
	ht.hwnd = NULL;
	hwndTemplate.res = DeviceS3Resource(dev);
	BrTokenValueSetMany(&ht, &count, NULL, tv, &hwndTemplate);

	if(ht.hwnd == NULL)
		return NULL;

	/*
	 * Set defaults
	 */
	GetClientRect(ht.hwnd, &rect);
	nt.width = rect.right;
	nt.height = rect.bottom;
	nt.fullscreen = BR_FALSE;
	nt.pixel_bits = GetDeviceCaps(GetDC(ht.hwnd), BITSPIXEL);

	/*
	 * Translate tokens and values
	 */
	newTemplate.res = DeviceS3Resource(dev);
	BrTokenValueSetMany(&nt, &count, NULL, tv, &newTemplate);

	/* Indicate that we have not yet allocated any offscreen buffers */
	allocated_one_offscreen = 0;

	/*
	 * Choose pixelmap type
	 * should really be taken from output facility
	 */
	switch(nt.pixel_bits)
		{
		case 8:
			type = BR_PMT_INDEX_8;
			break;
		case 15:
			type = BR_PMT_RGB_555;
			break;
		case 16:
			type = BR_PMT_RGB_565;
			break;
		case 24:
			type = BR_PMT_RGB_888;
			break;
		default:
			return NULL;
		}

	/*
	 * Allocate device pixelmap
	 */
	self = BrResAllocate(DeviceS3Resource(dev), sizeof(*self), BR_MEMORY_OBJECT);

	/*
	 * Fill members
	 */
	self->dispatch = &devicePixelmapDispatch;
 	self->pm_identifier = "Windows - S3:Screen";
	self->pm_type = type;
	self->pm_map = NULL;
	self->pm_flags = BR_PMF_NO_ACCESS;
	self->pm_copy_function = BR_PMCOPY_NORMAL;
	self->pm_base_x = 0;
	self->pm_base_y = 0;
	self->pm_width = nt.width;
	self->pm_height = nt.height;
	self->pm_origin_x = 0;
	self->pm_origin_y = 0;
	self->pm_row_bytes = 0;
	self->pm_pixels = NULL;
	self->buffer = BUFFER_SCREEN;
	self->fullscreen = nt.fullscreen;
	self->buffer_method = method;
	self->hwnd = ht.hwnd;
	self->pixel_bits = nt.pixel_bits;
	self->video_memory == BR_TRUE;
	self->flipped = BR_TRUE;

	/* Attach a CLUT object */
	self->clut = DeviceClutS3Allocate(dev, self, "Windows-CLUT");


	if (!self->fullscreen)
		{
		/* Non-fullscreen setup */
	   OUTDBS("NON FULLSCREEN\n");
		if(IDirectDraw_SetCooperativeLevel(gpDD, self->hwnd, DDSCL_NORMAL) != DD_OK)
			return NULL;

		if (S3SetupNonFullscreenFront(&self->surface) != BRE_OK)
			return(NULL);
    
		/* If not fullscreen, create a clipper object and attach to primary surface */
		if (IDirectDraw_CreateClipper(gpDD, 0, &self->clipper, NULL) != DD_OK)
			return NULL;
		if (IDirectDrawClipper_SetHWnd(self->clipper, 0, self->hwnd) != DD_OK)
			return NULL;
		if (IDirectDrawSurface_SetClipper(self->surface, self->clipper) != DD_OK)
			return NULL;
  		} 
	else
		{
		/* Fullscreen mode setup */
	   OUTDBS("FULLSCREEN\n");
		if(IDirectDraw_SetCooperativeLevel(gpDD, self->hwnd,
			                                 DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN) != DD_OK)
			return NULL;

		pixel_bits = self->pixel_bits;

		/* Direct draw does not understand requests for 15 bit modes */
		if(pixel_bits == 15)
			pixel_bits = 16;

		result = IDirectDraw_SetDisplayMode(gpDD, self->pm_width, self->pm_height, pixel_bits);
		if(result != DD_OK)
			return NULL;

		if (S3SetupFullscreenMemory(&self->surface) != BRE_OK)
			return(NULL);
		}


	/*
	 * Attach to output facility and increment instance counter
	 */
	self->output_facility = facility;
	self->output_facility->num_instances++;

	/*
	 * Add object to its container
	 */
	ObjectContainerAddFront(facility, (br_object *)self);


	/* Setup S3 Libraries (if fullscreen then we do not need to serialise access
	 * which is quicker) */
	if(self->fullscreen)
		{
		if (!S3InstantiateLibrary(BR_TRUE))
			return(NULL);
		}
	else
		{
		if (!S3InstantiateLibrary(BR_FALSE))
			return(NULL);
		}

   /* Sort out the format of the display surface in video memory */
   switch(nt.pixel_bits)
      {
      case  8:
         self->s3_surface.sfFormat = S3DTK_VIDEORGB8;
         break;
      case  15:
      case  16:
         self->s3_surface.sfFormat = S3DTK_VIDEORGB15;
         break;
      case  24:
         self->s3_surface.sfFormat = S3DTK_VIDEORGB24;
         break;
      default:
         break;
      }

   self->s3_surface.sfWidth = nt.width;
   self->s3_surface.sfHeight = nt.height;

   /* Get the surface offset within video memory */

   if (!S3Lock(self->surface, &pixel_address, NULL, NULL))
     {
     IDirectDrawSurface_Release(self->surface);
	 return(NULL);
	 }

   self->s3_surface.sfOffset = (br_uint_32)pixel_address-video_memory_base;
   S3Unlock(self->surface,&pixel_address);

	/* If we are in a true colour/hicolour video mode we must enable conversion
	 * of 8 bit textures to 15 bit ones for the S3 - no paletted texture support in
	 * true colour modes.  The conversion occurs in sbuffer.c
	 */
   if(self->pixel_bits != 8)
	   S3_convert_8bit_to_15 = BR_TRUE;
   else
      {
	   S3_convert_8bit_to_15 = BR_FALSE;
      /* Set up all the necessary clut data */
      entries = (PALETTEENTRY *)BrMemAllocate(self->clut->syspal_size * sizeof(PALETTEENTRY),
                                             BR_MEMORY_SCRATCH);
      GetPaletteEntries(self->clut->palette, 0,self->clut->syspal_size, entries);

      IDirectDraw_CreatePalette(gpDD, DDPCAPS_8BIT, entries, &self->palette, NULL);
      IDirectDrawSurface_SetPalette(self->surface, self->palette);

      BrMemFree(entries);
      }

   OUTDBS("DevicePixelmapS3Allocate success\n");

   
   return self;
}








/*
 * br_device_pixelmap_s3::resize
 */
static br_error BR_CMETHOD_DECL(br_device_pixelmap_s3, resize)\
	(br_device_pixelmap *self, br_int_32 width, br_int_32 height)
{
   br_uint_8 *pixel_address;
   br_uint_32 pixel_bits;
   HRESULT	result;

	UASSERT((width > 0) && (height >0));

   OUTDBS("Device Pixelmap S3 RESIZE\n");
	if (self->fullscreen)
		{
		IDirectDraw_RestoreDisplayMode(gpDD);
		if(IDirectDraw_SetCooperativeLevel(gpDD, self->hwnd, DDSCL_NORMAL) != DD_OK)
			return(BRE_FAIL);

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

		/* First of all free up the front/back surfaces */
		/* and clear the texture cache */
		S3ClearTextureCache();

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
				DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_NOWINDOWCHANGES) != DD_OK)
			return(BRE_FAIL);

		pixel_bits = self->pixel_bits;
		if(pixel_bits == 15)
			pixel_bits = 16;

		result = IDirectDraw_SetDisplayMode(gpDD, width, height, pixel_bits);
		if(result != DD_OK)
			return(BRE_FAIL);

		if (S3SetupFullscreenMemory(&self->surface) != BRE_OK)
			return(BRE_FAIL);

		allocated_one_offscreen = 0;
		self->pm_width = width;
		self->pm_height = height;

	   switch(pixel_bits)
			{
			case  8:
				self->s3_surface.sfFormat = S3DTK_VIDEORGB8;
				break;
			case  15:
			case  16:
				self->s3_surface.sfFormat = S3DTK_VIDEORGB15;
				break;
			case  24:
				self->s3_surface.sfFormat = S3DTK_VIDEORGB24;
				break;
			default:
				break;
			}

		self->s3_surface.sfWidth = width;
		self->s3_surface.sfHeight = height;

		/* Get the surface offset within video memory */

		if (!S3Lock(self->surface, &pixel_address, NULL, NULL))
			{
			IDirectDrawSurface_Release(self->surface);
			return(BRE_FAIL);
			}

		self->s3_surface.sfOffset = (br_uint_32)pixel_address-video_memory_base;
		S3Unlock(self->surface,&pixel_address);

		/* If we are in a true colour/hicolour video mode we must enable conversion
		 * of 8 bit textures to 15 bit ones for the S3 - no paletted texture support in
		 * true colour modes.  The conversion occurs in sbuffer.c
		 */
		if(self->pixel_bits != 8)
			S3_convert_8bit_to_15 = BR_TRUE;
		else
			S3_convert_8bit_to_15 = BR_FALSE;
		}
	else
		{
		/* Adjust pixelmap members */
		self->pm_width = width;
		self->pm_height = height;
		}

   OUTDBS("Device Pixelmap S3 RESIZE success\n");
   return(BRE_OK);
}


/*
 * br_device_pixelmap_s3::free
 */
static void BR_CMETHOD_DECL(br_device_pixelmap_s3, free)\
	(br_device_pixelmap *self)
{
	UASSERT(self->output_facility != NULL);

   OUTDBS("Device Pixelmap S3 FREE\n");
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

	/* Free pixelmap structure */
	BrResFreeNoCallback(self);

   /* Terminate the S3 libraries */
   S3TerminateLibrary();

   OUTDBS("Device Pixelmap S3 FREE success\n");

}


/* br_device_pixelmap_s3::match */
static br_error BR_CMETHOD_DECL(br_device_pixelmap_s3, match)\
	(br_device_pixelmap *self, br_device_pixelmap **newpm, br_token_value *tv)
{
	br_int_32 count;
	br_error result;
	br_device_pixelmap *pm;
	struct match_tokens mt = {BRT_NONE, 0};

	UASSERT(newpm != NULL);
	UASSERT(tv != NULL);

   OUTDBS("Device Pixelmap S3 MATCH\n");
	/*
	 * Set default width and height
	 */
	mt.width = self->pm_width;
	mt.height = self->pm_height;

	/*
	 * Convert match tokens
	 */
	matchTemplate.res = DeviceS3Resource(ObjectS3Device(self));
	BrTokenValueSetMany(&mt, &count, NULL, tv, &matchTemplate);

	/*
	 * Switch on match type
	 */
	switch(mt.use)
		{
		case BRT_OFFSCREEN:
			/*
			 * Create matching offscreen buffer: a back buffer if the first.
			 * Otherwise, the match will fail, because you can't guarantee more
			 * than one displayable buffer.
			 */
            OUTDBS("Offscreen\n");
			if (!allocated_one_offscreen)
				{
 				result = S3AllocateBack(self, newpm, mt.width, mt.height);
				allocated_one_offscreen = 1;
				return result;
				}
			else
				return(S3AllocateGenericOffscreen(self, newpm, mt.width, mt.height));
			break;

      case BRT_NO_RENDER:
            OUTDBS("No Render\n");
			return(S3AllocateGenericOffscreen(self, newpm, mt.width, mt.height));
         break;
		
      case BRT_HIDDEN_BUFFER:
            OUTDBS("Hidden buffer\n");
			/* Allocate a video memory buffer for texture rendering */
			return(S3AllocateVideoMemory(self, newpm, mt.width, mt.height));
			break;

		case BRT_HIDDEN:
		case BRT_CLONE:
		case BRT_DEPTH:
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
 * br_device_pixelmap_s3::doubleBuffer
 */
static br_error BR_CMETHOD_DECL(br_device_pixelmap_s3, doubleBuffer)\
	(br_device_pixelmap *self, br_device_pixelmap *src)
{
	br_error result;

	UASSERT(src != NULL);

	/*
	 * Switch on buffering method
	 */
	result = DoubleBufferDirectDraw(self, src);
	return result;
}



br_token BR_CMETHOD_DECL(br_device_pixelmap_s3, type)\
	(br_device_pixelmap *self)
{
	return BRT_DEVICE_PIXELMAP;
}

br_boolean BR_CMETHOD_DECL(br_device_pixelmap_s3, isType)\
	(br_device_pixelmap *self, br_token t)
{
	return (t == BRT_DEVICE_PIXELMAP) || (t == BRT_OBJECT);
}

br_int_32 BR_CMETHOD_DECL(br_device_pixelmap_s3, space)\
	(br_device_pixelmap *self)
{
	return sizeof(br_device_pixelmap);
}

struct br_tv_template * BR_CMETHOD_DECL(br_device_pixelmap_s3, queryTemplate)(br_device_pixelmap *self)
{
	devicePixelmapTemplate.res = DeviceS3Resource(ObjectS3Device(self));
	return &devicePixelmapTemplate;
}




/*
 * br_device_pixelmap_s3::allocateSub
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_s3, allocateSub)\
	(br_device_pixelmap *self, br_device_pixelmap **newpm, br_rectangle *rect)
{
	/*
	 * Method is not supported
	 */
 	return BRE_FAIL;
}



br_error BR_CMETHOD_DECL(br_device_pixelmap_s3, rectangleFill)\
	(br_device_pixelmap *self, br_rectangle *rect, br_uint_32 fill)
{

	return BRE_FAIL;
}



// Back pixelmap methods that are also valid for the front
#if 0
extern	BR_CMETHOD(br_device_pixelmap_s3_dd,	rectangleCopy)();
extern 	BR_CMETHOD(br_device_pixelmap_s3_dd,	rectangleCopyTo)();
extern 	BR_CMETHOD(br_device_pixelmap_s3_dd,	rectangleCopyFrom)();
extern	BR_CMETHOD(br_device_pixelmap_s3_dd,	rectangleStretchCopy)();
extern	BR_CMETHOD(br_device_pixelmap_s3_dd,	rectangleStretchCopyTo)();
extern	BR_CMETHOD(br_device_pixelmap_s3_dd,	rectangleStretchCopyFrom)();
extern	BR_CMETHOD(br_device_pixelmap_s3_dd,	rectangleFill)();
extern	BR_CMETHOD(br_device_pixelmap_s3_dd,	pixelSet)();
extern	BR_CMETHOD(br_device_pixelmap_s3_dd,	line)();
extern	BR_CMETHOD(br_device_pixelmap_s3_dd,	copyBits)();
#endif



/*
 * Default dispatch table for device pixelmap
 */
static struct br_device_pixelmap_dispatch devicePixelmapDispatch = {
	NULL,
	NULL,
	NULL,
	NULL,
	BR_CMETHOD_REF(br_device_pixelmap_s3,  free),
	BR_CMETHOD_REF(br_object_s3,           identifier),
	BR_CMETHOD_REF(br_device_pixelmap_s3,  type),
	BR_CMETHOD_REF(br_device_pixelmap_s3,  isType),
	BR_CMETHOD_REF(br_object_s3,           device),
	BR_CMETHOD_REF(br_device_pixelmap_s3,  space),

	BR_CMETHOD_REF(br_device_pixelmap_s3,  queryTemplate),
	BR_CMETHOD_REF(br_object,               query),
	BR_CMETHOD_REF(br_object,               queryBuffer),
	BR_CMETHOD_REF(br_object,               queryMany),
	BR_CMETHOD_REF(br_object,               queryManySize),
	BR_CMETHOD_REF(br_object,               queryAll),
	BR_CMETHOD_REF(br_object,               queryAllSize),

	BR_CMETHOD_REF(br_device_pixelmap_mem,  validSource),
	BR_CMETHOD_REF(br_device_pixelmap_s3,  resize),
	BR_CMETHOD_REF(br_device_pixelmap_s3,  match),
	BR_CMETHOD_REF(br_device_pixelmap_s3,  allocateSub),

	BR_CMETHOD_REF(br_device_pixelmap_gen,  copyTo),
	BR_CMETHOD_REF(br_device_pixelmap_gen,  copyTo),
	BR_CMETHOD_REF(br_device_pixelmap_gen,  copyFrom),
	BR_CMETHOD_REF(br_device_pixelmap_gen,  fill),
	BR_CMETHOD_REF(br_device_pixelmap_s3,  doubleBuffer),

	BR_CMETHOD_REF(br_device_pixelmap_gen,  copyDirty),
	BR_CMETHOD_REF(br_device_pixelmap_gen,  copyToDirty),
	BR_CMETHOD_REF(br_device_pixelmap_gen,  copyFromDirty),
	BR_CMETHOD_REF(br_device_pixelmap_gen,  fillDirty),
	BR_CMETHOD_REF(br_device_pixelmap_gen,  doubleBufferDirty),


	BR_CMETHOD_REF(br_device_pixelmap_gen,		rectangle),
	BR_CMETHOD_REF(br_device_pixelmap_gen,		rectangle2),
	BR_CMETHOD_REF(br_device_pixelmap_s3_dd,	rectangleCopy),
	BR_CMETHOD_REF(br_device_pixelmap_s3_dd,	rectangleCopyTo),
	BR_CMETHOD_REF(br_device_pixelmap_s3_dd,	rectangleCopyFrom),
	BR_CMETHOD_REF(br_device_pixelmap_s3_dd,	rectangleStretchCopy),
	BR_CMETHOD_REF(br_device_pixelmap_s3_dd,	rectangleStretchCopyTo),
	BR_CMETHOD_REF(br_device_pixelmap_s3_dd,	rectangleStretchCopyFrom),
	BR_CMETHOD_REF(br_device_pixelmap_s3_dd,	rectangleFill),
	BR_CMETHOD_REF(br_device_pixelmap_s3_dd,	pixelSet),
	BR_CMETHOD_REF(br_device_pixelmap_s3_dd,	line),
	BR_CMETHOD_REF(br_device_pixelmap_s3_dd,	copyBits),
	

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

	BR_CMETHOD_REF(br_device_pixelmap_s3_dd,	directLock),
	BR_CMETHOD_REF(br_device_pixelmap_s3_dd, 	directUnlock),	

	BR_CMETHOD_REF(br_device_pixelmap_gen,	getControls),
	BR_CMETHOD_REF(br_device_pixelmap_gen,	setControls),
};



br_boolean CheckFrontPixelmapDispatch(void *address)
{
	return(address == &devicePixelmapDispatch);
}


#if 0

	BR_CMETHOD_REF(br_device_pixelmap_gen,  rectangle),
	BR_CMETHOD_REF(br_device_pixelmap_gen,  rectangle2),
	BR_CMETHOD_REF(br_device_pixelmap_mem,  rectangleCopyTo),
	BR_CMETHOD_REF(br_device_pixelmap_mem,  rectangleCopyTo),
	BR_CMETHOD_REF(br_device_pixelmap_mem,  rectangleCopyFrom),
	BR_CMETHOD_REF(br_device_pixelmap_mem,  rectangleStretchCopyTo),
	BR_CMETHOD_REF(br_device_pixelmap_mem,  rectangleStretchCopyTo),
	BR_CMETHOD_REF(br_device_pixelmap_mem,  rectangleStretchCopyFrom),
	BR_CMETHOD_REF(br_device_pixelmap_s3,   rectangleFill),
	BR_CMETHOD_REF(br_device_pixelmap_mem,  pixelSet),
	BR_CMETHOD_REF(br_device_pixelmap_gen,  line),
	BR_CMETHOD_REF(br_device_pixelmap_gen,  copyBits),

#endif

