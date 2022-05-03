/*
 * s3link.c :  Link layer between Brender driver and S3 libraries
 *
 */
#include <string.h>

#include <windows.h>
#include <ddraw.h>

//#include "s3link.h"
#include "drv.h"
#include "brassert.h"


#define syslog(s)
#define syslog1(s, d)

typedef char byte;

extern LPDIRECTDRAW		gpDD;
extern LPDIRECTDRAW2	gpDD2;

br_uint_32 video_memory_base = 0;

/* Master rendering function pointer */

S3DTK_LPFUNCTIONLIST g_pS3DTK_Funct;


// DDlock and unlock are defined in devpmdd.c, but I want to use them...
extern br_boolean S3Lock(LPDIRECTDRAWSURFACE surface, void **framebuffer, RECT *lock_area, long *pitch);
extern br_boolean S3Unlock(LPDIRECTDRAWSURFACE surface, void *lock_address);

/* Steve Williams' two functions to set up pixelformat (from D3D Driver) */

static int BrDD_PixelmapType_BPP( int type )
{
  switch ( type )  {
    case BR_PMT_INDEX_8 : return 8 ;
	 case BR_PMT_RGB_555 : return 16 ;
    case BR_PMT_RGB_565 : return 16 ;
    case BR_PMT_RGBA_4444 : return 16 ;
    case BR_PMT_RGB_888 : return 24 ;
    case BR_PMT_RGBX_888 : return 32 ;
	 case BR_PMT_RGBA_8888 : return 32 ;
  }

  return 0 ;
}

static void BrDD_SetPixelFormat( LPDDSURFACEDESC ddsd, int type )
{
  ddsd->ddpfPixelFormat.dwSize   = sizeof(DDPIXELFORMAT);
  ddsd->ddpfPixelFormat.dwRGBBitCount = BrDD_PixelmapType_BPP(type);
   
  switch(type) {
  case BR_PMT_INDEX_8:
    // Pixel format is 8-bit indexed
    ddsd->ddpfPixelFormat.dwFlags  = DDPF_PALETTEINDEXED8 | DDPF_RGB ; 
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

  case BR_PMT_RGBA_4444 :             
    // Pixel format is ARGB-4444, but just use 555 for now and cheat
    ddsd->ddpfPixelFormat.dwFlags  = DDPF_RGB; 
    ddsd->ddpfPixelFormat.dwFourCC = BI_RGB ;
    ddsd->ddpfPixelFormat.dwRBitMask = 0x7c00 ; 
    ddsd->ddpfPixelFormat.dwGBitMask = 0x03e0; 
    ddsd->ddpfPixelFormat.dwBBitMask = 0x001f ; 
    break ;

  default :
    // Set a value that will cause the code to fail the createsurface
    ddsd->ddpfPixelFormat.dwFlags = 0;
    return;
  }
}



/* Head of the linked list of the texture cache, maintained through the next_on_card
 * method in render_buffer */
static struct render_buffer *texture_surface_head = NULL;


br_boolean S3CacheTextureToCard(struct render_buffer *src, br_uint_32 *texture_size,
                                 byte **texture_start)
{
   /* No mipmaps yet */
   HRESULT result;
   DDSURFACEDESC ddsd;
   LPDIRECTDRAWSURFACE surface;
	br_uint_32 height;


   /* Set up the S3 surface structure in the render buffer
    * to refer to the card copy of the texture. Offset must be
    * the offset in card memory, not the linear memory address */
   switch(src->type)
      {
	  case BR_PMT_INDEX_8:
		src->s3_surface.sfFormat = S3DTK_TEXPALETTIZED8;
		break;
      case BR_PMT_RGB_555:
         src->s3_surface.sfFormat = S3DTK_TEXARGB1555;
         break;
      case BR_PMT_RGBA_4444:
         src->s3_surface.sfFormat = S3DTK_TEXARGB4444;
         break;
      case BR_PMT_RGBX_888:
      case BR_PMT_RGBA_8888:
         src->s3_surface.sfFormat = S3DTK_TEXARGB8888;
         break;
      default:
         return(BR_FALSE);
         break;
      }

  /* Mipmapped surfaces are slight hacks; allocate a surface larger in height
   * by the 22/16 factor needed for mipmapping space. This is rather than the
   * 'official' DirectDraw method of allocating one surface for each mipmap because
   * that's one of the most hideous things I've ever seen */
  height = src->height;
  if (src->mip_offset)
    height = height*22 / 16;

   *texture_size = src->width_b * height;

   /* Allocate a DirectDraw surface for the texture */
   memset(&ddsd, 0, sizeof(DDSURFACEDESC));
   ddsd.dwSize = sizeof(DDSURFACEDESC);
   ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
   ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_VIDEOMEMORY;
   ddsd.dwWidth = src->width_p;
   ddsd.dwHeight = height;
   BrDD_SetPixelFormat(&ddsd, src->type);

   if ((result = IDirectDraw_CreateSurface(gpDD, &ddsd, &surface, NULL)) != DD_OK)
     return BR_FALSE;

   /* OK surface has been allocated OK: get pixel pointer */
   if (!S3Lock(surface, texture_start, NULL, NULL))
     {
     IDirectDrawSurface_Release(surface);
     return(BR_FALSE);
     }

	/* Copy across */
   memcpy(*texture_start, src->base_mem, *texture_size);

   /* Unlock again */
   if (!S3Unlock(surface, texture_start))
     {
     IDirectDrawSurface_Release(surface);
     return(BR_FALSE);
     }


   /* Indicate that the surface is now in video memory and is a texture*/
   src->s3_surface.sfFormat |= S3DTK_VIDEO;
   src->s3_surface.sfFormat |= S3DTK_TEXTURE;

   src->s3_surface.sfOffset = (ULONG)*texture_start - video_memory_base;
   src->s3_surface.sfWidth = src->width_p;
   src->s3_surface.sfHeight = src->height;
  
   src->in_video_memory = BR_TRUE;

   /* Allocated successfully; add to texture list */
   src->surface = surface;
   src->next_on_card = texture_surface_head;
   texture_surface_head = src;

   return(BR_TRUE);
}






/*
 * The texture cache calls this function whenever it frees up a texture
 * to make room for others in the texture store
 */
br_boolean S3TextureFreedCallback(struct render_buffer *texture)
{
	/* Indicate that this texture is no longer available in the card */

	texture->in_video_memory = BR_FALSE;
	texture->s3_surface.sfFormat &= ~S3DTK_VIDEO;
   
	return(BR_TRUE);
}




/*
 * Try to find some memory to fit this texture in. Texture is supplied
 * for future enhancements (like finding a texture of the same size). Should
 * be tolerant of NULL to allow cache cleaning
 */
br_boolean S3FindTextureMemory(struct render_buffer *texture)
{
   struct render_buffer *check, *last;

   if (!texture_surface_head)
      return(BR_FALSE); // Nothing I can do if no textures onboard

   /* Walk the linked list and free the last entry */
   last = NULL;
   for(check = texture_surface_head; check->next_on_card; check = check->next_on_card)
      last = check;

   /* Unlink it */
   if (last)
      last->next_on_card = check->next_on_card;
   else texture_surface_head = check->next_on_card;

   /* OK here we should have the last entry: free it */
   S3TextureFreedCallback(check);

   /* Now release the surface */
   IDirectDrawSurface_Release(check->surface);
   
   /* and set the pointer to NULL */
   check->surface = NULL;

   return(BR_TRUE);
}



/*
 * Clear cache by repeated search requests
 */
void S3ClearTextureCache(void)
{
  while(S3FindTextureMemory(NULL))
    ;
}


/*
 * A render buffer is about to be freed - better sort it out doublequick
 */
void S3LoseRenderBuffer(struct render_buffer *buffer)
{
   struct render_buffer *check, *last;

   /* Walk the linked list checking for this entry */
   last = NULL;
   for(check = texture_surface_head; check && (check != buffer); check = check->next_on_card)
      last = check;

   if(check)
      {
      if (last)
         last->next_on_card = check->next_on_card;
      else
         texture_surface_head = check->next_on_card;
      }


   /* We only have jurisdiction over textures that we have allocated, we can't free
    * buffers created by the application in video memory */
   if(buffer->always_in_video_memory == BR_FALSE)
		{
		if(buffer->in_video_memory == BR_TRUE)
			S3TextureFreedCallback(buffer);

		if(buffer->surface)
			{
			IDirectDrawSurface_Release(buffer->surface);
			buffer->surface = NULL;
			}
		}
}







/*
 * Cache a texture in the card
 *
 * In;  the render buffer with all details of the texture
 * When this is called it sets the s3_surface information in the render
 * buffer ready for use by the S3 libraries
 *
 */

br_boolean S3CacheTexture(struct render_buffer *src, br_boolean force_reload)
{
   br_int_32      log2x, log2y; // Texture x and y sizes
   br_uint_32     k, p;              // Counters

   byte *         texture_start;     // Pointer to start of texture on card
   br_uint_32     texture_size;      // Size inc all mipmaps

   /* If we need to force a reload, order the texture cache to clear any
    * entries corresponding to this texture, and make another attempt
    * to cache it (in case the failure condition has now been corrected) */

   if (force_reload)
      {
      src->uncacheable = BR_FALSE;
      S3LoseRenderBuffer(src);
      }


   /* If the buffer is already in video memory, then the S3 surface data
    * is already set up so we don't need to do anything to load it */

   if ((src->in_video_memory == BR_TRUE) || (src->always_in_video_memory == BR_TRUE))
       return(BR_TRUE);


   /* If it's been marked as uncacheable, don't suffer the overhead of
    * attempting to set it up again: failure conditions exist that
    * will mean it's never loadable. */

   if (src->uncacheable)
      return(BR_FALSE);


   /* Set the x and y sizes */
   log2x = log2y = -1;
   for(p=1,k=0; k<11; p<<=1,k++)
      {
      if (src->width_p == p)
         log2x = k;
      if (src->height == p)
         log2y = k;
      }

   /* If not a power of 2 on both axes, can't render the texture */
   if ((log2x < 0) || (log2y < 0))
      {
      src->uncacheable = BR_TRUE; // Obviously if this failed, can never render
      return(BR_FALSE);
      }

   /* Attempt to allocate a DirectDraw surface for this texture */

   while(!S3CacheTextureToCard(src, &texture_size, &texture_start))
      {
      /* OK the first attempt to allocate failed - call FindTextureMemory.
       * If this returns false, there isn't any more to find! */
      if (!S3FindTextureMemory(src))
         {
         /* Assume this means it's too big for the cache - otherwise, every
         * time the routine attempts to register it it will flush the texture cache */
         src->uncacheable = BR_TRUE;
         return(BR_FALSE);
         }
      }

   return(BR_TRUE);
}



/* Internal surface structures to set up render windows etc */

static S3DTK_SURFACE  s3_draw_surface;
static S3DTK_SURFACE  s3_display_surface;
static S3DTK_SURFACE  s3_z_surface;

static	LPDIRECTDRAWSURFACE draw_pm_ptr = NULL;
static	LPDIRECTDRAWSURFACE z_pm_ptr = NULL;

/*
 * Setup hardware with correct render window
 *
 * Note that this ONLY applies to 3D functions; 2D functions will have
 * to be separately moved/clipped.
 */
br_boolean S3SetRenderWindow(LPDIRECTDRAWSURFACE surface,
                             br_int_32 x, br_int_32 y,
                             br_int_32 w, br_int_32 h, br_uint_32 format,
							 br_device_pixelmap *orig_pm)
{
   void *   framebuffer;    // Start of video memory to be used
   RECT     lock_area;      // Area to be locked
   long     pitch;
   br_uint_32 pixel_bits;

   /* Error checking? */

   /* Lock/unlock surface to get framebuffer address */
   lock_area.left = x;
   lock_area.right = x+w;
   lock_area.top = y;
   lock_area.bottom = y+h;
   if (!S3Lock(surface, &framebuffer, &lock_area, &pitch))
      return(BR_FALSE);
   if (!S3Unlock(surface, framebuffer))
      return(BR_FALSE);

   s3_draw_surface.sfOffset = (br_uint_32)framebuffer - video_memory_base;
   s3_draw_surface.sfWidth  = w;
   // If this is a sub pixelmap then we must cheat and set w to the stride
   if((orig_pm->sub_x_offset != 0) || (orig_pm->sub_y_offset != 0))
	   {
	   pixel_bits = orig_pm->pixel_bits;
		// Hack for RGB_555 compatibility
	   if(pixel_bits == 15) pixel_bits = 16;
	   s3_draw_surface.sfWidth = orig_pm->pm_row_bytes / (pixel_bits/8);
	   s3_draw_surface.sfOffset +=(orig_pm->sub_y_offset * orig_pm->pm_row_bytes) +
								   (orig_pm->sub_x_offset *(pixel_bits/8));
	   }
   s3_draw_surface.sfHeight = h;


   switch(format)
      {
      case BR_PMT_INDEX_8:
         s3_draw_surface.sfFormat = S3DTK_VIDEORGB8 | S3DTK_VIDEO;
         break;
      case BR_PMT_RGB_555:
      case BR_PMT_RGB_565:
         s3_draw_surface.sfFormat = S3DTK_VIDEORGB15 | S3DTK_VIDEO;
         break;
      case BR_PMT_RGB_888:
         s3_draw_surface.sfFormat = S3DTK_VIDEORGB24 | S3DTK_VIDEO;
         break;
      default:
         return BR_FALSE;
      }

   /* Set details for renderer - address of framebuffer surface */
   if (g_pS3DTK_Funct->S3DTK_SetState(g_pS3DTK_Funct,S3DTK_DRAWSURFACE, (br_uint_32)&s3_draw_surface) == S3DTK_ERR)
      return(BR_FALSE);

   draw_pm_ptr = surface;
   orig_pm->flipped = BR_FALSE;

  return(BR_TRUE);
}



/*
 * Setup hardware with appropriate Z buffer information
 */
br_boolean S3SetZBufferState(br_boolean enable, LPDIRECTDRAWSURFACE surface,
                              br_int_32 w, br_int_32 h,br_device_pixelmap *orig_pm)
{
   void *     zbuffer_address;

   if (enable)
      {
      if (!S3Lock(surface, &zbuffer_address, NULL, NULL))
         return(BR_FALSE);
      if (!S3Unlock(surface, zbuffer_address))
         return(BR_FALSE);

      /* Set up the matching S3 surface */
      s3_z_surface.sfOffset = (br_uint_32)zbuffer_address - video_memory_base;
      s3_z_surface.sfWidth = w;
      s3_z_surface.sfHeight = h;
      s3_z_surface.sfFormat = S3DTK_Z16 | S3DTK_VIDEO;

	  /* If this is a sub pixelmap then we must cheat and set w to the stride */
	  if(orig_pm->sub_x_offset != 0)
		 {
		 s3_draw_surface.sfWidth = orig_pm->pm_row_bytes;
		 s3_draw_surface.sfOffset +=(orig_pm->sub_y_offset * orig_pm->pm_row_bytes) +
								   (orig_pm->sub_x_offset *(orig_pm->pixel_bits/8));
		 }


      /* First set the Z surface address */
      if (g_pS3DTK_Funct->S3DTK_SetState(g_pS3DTK_Funct,S3DTK_ZBUFFERSURFACE,(br_uint_32)&s3_z_surface)
          == S3DTK_ERR)
         return(BR_FALSE);

      /* Set Z enable/disable */
      if (g_pS3DTK_Funct->S3DTK_SetState(g_pS3DTK_Funct,S3DTK_ZBUFFERENABLE,S3DTK_ON)
          == S3DTK_ERR)
         return(BR_FALSE);

      /* Set Z compare mode to Z <= */
      if (g_pS3DTK_Funct->S3DTK_SetState(g_pS3DTK_Funct,S3DTK_ZBUFFERCOMPAREMODE,S3DTK_ZSRCLEZFB)
          == S3DTK_ERR)
         return(BR_FALSE);
      } 
   else
      {
      /* Set Z off */
      if (g_pS3DTK_Funct->S3DTK_SetState(g_pS3DTK_Funct,S3DTK_ZBUFFERENABLE,S3DTK_OFF)
          == S3DTK_ERR)
         return(BR_FALSE);
      }

  return(BR_TRUE);
}



static br_uint_32 libvalid=0;
static S3DTK_RENDERER_INITSTRUCT renderer_init;
static S3DTK_LIB_INIT LibTable;
static s3_libs_instantiated = BR_FALSE;

//#define USE_S3_DMA

/*
 * Start up all S3 renderer components
 */
br_boolean S3InstantiateLibrary(br_boolean exclusive)
{
	if(s3_libs_instantiated == BR_TRUE)
		/* Pretend we've reinstantiated the lib */
		return(BR_TRUE);

#ifdef USE_S3_DMA
	if(exclusive == BR_TRUE)
		LibTable.libFlags = S3DTK_INITDMA;
	else
		/* I have 2d serialization enabled for windowed, but it seems to do nothing
		 * in all tests I've done, and apparently it is slower? */
		LibTable.libFlags = S3DTK_INITDMA | S3DTK_INIT2D_SERIALIZATION_ON;
#else
	if(exclusive == BR_TRUE)
		LibTable.libFlags = S3DTK_INITPIO;
	else
		/* I have 2d serialization enabled for windowed, but it seems to do nothing
		 * in all tests I've done, and apparently it is slower? */
		LibTable.libFlags = S3DTK_INITPIO | S3DTK_INIT2D_SERIALIZATION_ON;
#endif


	LibTable.libVideoBufferLinAddr = 0;
	LibTable.libMMIOSpaceLinAddr = 0;

	if(DriverDeviceS3.S3DTK_InitLib((ULONG)&LibTable) != S3DTK_OK)
		return(BR_FALSE);

	libvalid = 1;

   /* Create the list of functions attached to the device 
    * The flag VERIFY_UVRANGE which is supposed to fix the "u/v must 
    * not differ by >= 128" problem doesn't seem to work properly */
   renderer_init.initFlags = S3DTK_FORMAT_FLOAT;
   renderer_init.initUserID = 0;
   renderer_init.initAppID = 0;

  
   if(DriverDeviceS3.S3DTK_CreateRenderer((br_uint_32)&renderer_init,&(g_pS3DTK_Funct)) != S3DTK_OK)
   {
      DriverDeviceS3.S3DTK_ExitLib();
      return(BR_FALSE);
      }

#ifdef USE_S3_DMA
   S3SetState(S3DTK_DMA_ON,1);
#else
   S3SetState(S3DTK_DMA_OFF,0);
#endif

   /* Get video memory address */

   S3QueryState(S3DTK_VIDEOMEMORYADDRESS,&video_memory_base);

   /* Parse capabilities */

   texture_surface_head = NULL;

   /* Setup render functions */
   InitialiseS3RenderFunctions();

   s3_libs_instantiated = BR_TRUE;

   return(BR_TRUE);
}



/*
 * Finish off the renderer
 */
br_boolean S3TerminateLibrary(void)
{
	if(s3_libs_instantiated == BR_FALSE)
		/* - don't need to do anything */
		return (BR_TRUE);

   /* Close down texture cache by repeated calls to the clean-out function */
   S3ClearTextureCache();

   /* Destroy the renderer */
   DriverDeviceS3.S3DTK_DestroyRenderer(&g_pS3DTK_Funct);

   DriverDeviceS3.S3DTK_ExitLib();

   s3_libs_instantiated = BR_FALSE;

   return(BR_TRUE);
}



/*_____________________________________________________________________*/

/* Here are the old functions from S3api.c that are still supported */

/*
 * Query the value of an S3 library register
 */

br_error S3QueryState(br_uint_32 variable,
                      br_uint_32 *value)
{
   g_pS3DTK_Funct->S3DTK_GetState(g_pS3DTK_Funct,
                                 variable,
                                 (br_uint_32)(value));

	return BRE_OK;
}


#if 0
/*
 * Set the value of an S3 library register
 */

br_error _inline _cdecl S3SetState(br_uint_32 variable,
							br_uint_32 value)
{
   if(g_pS3DTK_Funct->S3DTK_SetState(g_pS3DTK_Funct,
                                 variable,
                                 value) != S3DTK_ERR)

		return BRE_OK;
   else
		return BRE_FAIL;
}
#endif

br_boolean  S3QueryBusy(void)
{
   ULONG  value;

   g_pS3DTK_Funct->S3DTK_GetState(g_pS3DTK_Funct,
                                   S3DTK_GRAPHICS_ENGINE_IDLE,
                                   (ULONG)(&value));

   if(value)
      return(BR_TRUE);
   else
      return(BR_FALSE);

}

