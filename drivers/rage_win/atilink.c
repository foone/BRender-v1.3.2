/*
 * atilink.c :  Link layer between Brender driver and ATI libraries
 *
 */
#include <string.h>

#include <windows.h>
#include <ddraw.h>

#include "atilink.h"
#include "brassert.h"
#include "device.h"

#define syslog(s)
#define syslog1(s, d)
//#define syslog1(s, d) syslog((s), (d))
//#define syslog4(s, d1,d2,d3,d4) syslog((s), (d1),(d2),(d3),(d4))

/* Steve Williams' two functions to set up pixelformat (from D3D Driver) */

static int BrDD_PixelmapType_BPP(int type)
{
	switch (type)  {
	case BR_PMT_INDEX_8:	return 8;
	case BR_PMT_RGB_555:	return 16;
	case BR_PMT_RGB_565:	return 16;
	case BR_PMT_RGBA_4444:	return 16;
	case BR_PMT_RGB_888:	return 24;
	case BR_PMT_RGBX_888:	return 32;
	case BR_PMT_RGBA_8888:	return 32;
	}

	return 0;
}


static void BrDD_SetPixelFormat(LPDDSURFACEDESC ddsd, int type)
{
	ddsd->ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	ddsd->ddpfPixelFormat.dwRGBBitCount = BrDD_PixelmapType_BPP(type);
   
	switch(type) {
	case BR_PMT_INDEX_8:
		// Pixel format is 8-bit indexed
		ddsd->ddpfPixelFormat.dwFlags  = DDPF_PALETTEINDEXED8 | DDPF_RGB ; 
		break;

	case BR_PMT_RGB_555:
		// Pixel format is RGB-555
		ddsd->ddpfPixelFormat.dwFlags  = DDPF_RGB ; 
		ddsd->ddpfPixelFormat.dwFourCC = BI_RGB ;
		ddsd->ddpfPixelFormat.dwRBitMask = 0x7c00 ; 
		ddsd->ddpfPixelFormat.dwGBitMask = 0x03e0; 
		ddsd->ddpfPixelFormat.dwBBitMask = 0x001f ; 
		break;

	case BR_PMT_RGB_565:
		// Pixel format is RGB-565
		ddsd->ddpfPixelFormat.dwFlags  = DDPF_RGB ; 
		ddsd->ddpfPixelFormat.dwFourCC = BI_RGB ;
		ddsd->ddpfPixelFormat.dwRBitMask = 0xf800 ; 
		ddsd->ddpfPixelFormat.dwGBitMask = 0x07e0; 
		ddsd->ddpfPixelFormat.dwBBitMask = 0x001f ; 
		break;

	case BR_PMT_RGBA_4444:
		// Pixel format is ARGB-4444
		ddsd->ddpfPixelFormat.dwFlags  = DDPF_RGB | DDPF_ALPHAPIXELS; 
//    ddsd->ddpfPixelFormat.dwFourCC = BI_RGB ;
		ddsd->ddpfPixelFormat.dwRBitMask = 0x0f00 ; 
		ddsd->ddpfPixelFormat.dwGBitMask = 0x00f0; 
		ddsd->ddpfPixelFormat.dwBBitMask = 0x000f ; 
		ddsd->ddpfPixelFormat.dwRGBAlphaBitMask = 0xf000;
		break;

	default:
		// Set a value that will cause the code to fail the createsurface
		ddsd->ddpfPixelFormat.dwFlags = 0;
		return;
	}
}


/* Head of the linked list of the texture cache, maintained through the next_on_card
 * method in render_buffer */
static struct render_buffer *texture_surface_head = NULL;


br_error ATICacheTextureToCard(struct render_buffer *src, br_uint_32 *texture_size,
                                 byte **texture_start)
{
	/* No mipmaps yet */
	HRESULT result;
	DDSURFACEDESC ddsd;
	LPDIRECTDRAWSURFACE surface;
	br_uint_32 height;

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
		return BRE_DEV_NO_VIDEO_MEMORY;

	/* OK surface has been allocated OK: get pixel pointer */
	if (!ATILock(surface, texture_start, NULL, NULL)) {
		IDirectDrawSurface_Release(surface);
		return(BRE_DEV_LOCK_FAIL);
	}

	/* Copy across */
	memcpy(*texture_start, src->base, *texture_size);

	/* Unlock again */
	if (!ATIUnlock(surface, texture_start)) {
		IDirectDrawSurface_Release(surface);
		return(BRE_DEV_LOCK_FAIL);
	}

	/* Allocated successfully; add to texture list */
	src->surface = surface;
	src->next_on_card = texture_surface_head;
	texture_surface_head = src;

	return(BRE_OK);
}





/*
 * The texture cache calls this function whenever it frees up a texture
 * to make room for others in the texture store
 */
br_boolean ATITextureFreedCallback(struct render_buffer *texture)
{

	if(texture->texture_handle) {
		syslog("TextureFreedCallback:\n   Calling unregister");
		if (ATI3DCIF_TextureUnreg(texture->texture_handle) != C3D_EC_OK) {
			syslog("   Error");
			return(BR_FALSE);
		}

		texture->texture_handle = NULL;
	}

	return(BR_TRUE);
}




/*
 * Try to find some memory to fit this texture in. Texture is supplied
 * for future enhancements (like finding a texture of the same size). Should
 * be tolerant of NULL to allow cache cleaning
 */
br_boolean ATIFindTextureMemory(struct render_buffer *texture)
{
	struct render_buffer *check, *last;

	if (!texture_surface_head)
		return(BR_FALSE); // Nothing I can do if no textures onboard

	/* Walk the linked list and free the last entry */
	last = NULL;
	for(check = texture_surface_head; check->next_on_card; check = check->next_on_card)
		last = check;

	/* Unlink it */
	if (last) last->next_on_card = check->next_on_card;
	else texture_surface_head = check->next_on_card;

	/* OK here we should have the last entry: free it */
	ATITextureFreedCallback(check);

	/* Now release the surface */
	if (check->surface) {
		IDirectDrawSurface_Release(check->surface);
		check->surface = NULL;
	}

	return(BR_TRUE);
}



/*
 * Clear cache by repeated search requests
 */
void ATIClearTextureCache(void)
{
	while(ATIFindTextureMemory(NULL))
		;
}



/*
 * A render buffer is about to be freed - better sort it out doublequick
 *
 * Also used to force reload of textures; clears any copies of this entry out
 */
void ATILoseRenderBuffer(struct render_buffer *buffer)
{
	struct render_buffer *check, *last;

	/* If this is a hidden buffer then we can't do anything to it except to
	 * clear its texture handle
	 */
	if(buffer->always_in_video_memory) {
		ATITextureFreedCallback(buffer);
		return;
	}


	/* Walk the linked list checking for this entry */
	last = NULL;
	for(check = texture_surface_head; check && (check != buffer); check = check->next_on_card)
		last = check;

	if (check) {
		if (last) last->next_on_card = check->next_on_card;
		else texture_surface_head = check->next_on_card;
	}


	ATITextureFreedCallback(buffer);

	if (buffer->surface) {
		IDirectDrawSurface_Release(buffer->surface);
		/* Null the surface */
		buffer->surface = NULL;
	}
}





br_boolean z_buffer_exists = BR_FALSE;


/* Master rendering handle */
C3D_HRC hRenderContext;

/* Card capabilities and information */
C3D_3DCIFINFO   CIFInfo;

br_boolean      ci8_texture_avail = BR_FALSE;

/*
 * Cache a texture in the card, registering it with the ATI library
 *
 * In;  the render buffer with all details of the texture
 */
br_boolean ATICacheTexture(struct render_buffer *src, br_boolean force_reload)
{
	br_int_32      log2x, log2y, maxlog2; // Texture x and y sizes
	br_uint_32     k, p;              // Counters

	C3D_EC         ecRetVal;          // Error code from ATIC3DCIF routines
	C3D_TMAP       TMap;              // Structure used to register texture
	C3D_HTX        texture_handle;    // Handle of texture

//  br_int_32      card_mem_pos;      // Position of texture in card memory (relative to start of texture cache)
	byte *         texture_start;     // Pointer to start of texture on card
	br_uint_32     base_texture_size; // Basic size of texture in bytes
	br_uint_32     texture_size;      // Size inc all mipmaps :: NOT USED
	br_error		 result;


	syslog("CacheTexture:");

	/* If we need to force a reload, order the texture cache to clear any
	 * entries corresponding to this texture, and make another attempt
	 * to cache it (in case the failure condition has now been corrected) */
	if (force_reload) {
		syslog("   Forced reload");
		src->uncacheable = BR_FALSE;
		ATILoseRenderBuffer(src);
	}

	/* If there's already a handle in the buffer, then the ATI library
	 * already knows about the texture so we don't need to do anything */
	if (src->texture_handle) {
		syslog("   Already loaded; not necessary");
		return(BR_TRUE);
	}

	/* If it's been marked as uncacheable, don't suffer the overhead of
	 * attempting to set it up again: failure conditions exist that
	 * will mean it's never loadable. */
	if (src->uncacheable) {
		syslog("   Marked uncacheable; can't load");
		return(BR_FALSE);
	}

	syslog("   Need to load texture");
	/* Set the x and y sizes */
	log2x = log2y = -1;
	for(p=1,k=0; k<11; p<<=1,k++) {
		if (src->width_p == p)
			log2x = k;
		if (src->height == p)
			log2y = k;
	}

	/* If not a power of 2 on both axes, can't render the texture */
	if ((log2x < 0) || (log2y < 0)) {
		syslog("   texture not renderable; sizes not power of 2");
		src->uncacheable = BR_TRUE; // Obviously if this failed, can never render
		return(BR_FALSE);
	}


	/* If the texture is a hidden_buffer, it is always in video memory, and
	 * therefore never needs to be copied for caching. Because it is not on the
	 * texture surface list, and has a null surface, there will not be any freeing
	 * problems either. */

	if (src->always_in_video_memory)
		{
		if (!src->surface)
			{
			src->uncacheable = BR_TRUE;
			return(BR_FALSE);
			}

		/* Set the address: size is not currently used */
		if (!ATILock(src->surface, &texture_start, NULL, NULL))
			return(BR_FALSE);
		if (!ATIUnlock(src->surface, &texture_start))
			return(BR_FALSE);

		}
	else
		{
		/* Attempt to allocate a DirectDraw surface for this texture */
		while((result = ATICacheTextureToCard(src, &texture_size, &texture_start)) != BRE_OK)
			{
			/* Make sure the problem is a lack of memory one */
			if (result != BRE_DEV_NO_VIDEO_MEMORY) {
				return(BR_FALSE);
			}

			/* OK the first attempt to allocate failed - call FindTextureMemory.
			 * If this returns false, there isn't any more to find! */
			if (!ATIFindTextureMemory(src))
				{
				/* Assume this means it's too big for the cache - otherwise, every
				 * time the routine attempts to register it it will flush the texture cache */
				src->uncacheable = BR_TRUE;
				return(BR_FALSE);
				}
			}
		syslog("   sent OK; starting register");
		}

	/*
	 * Now the texture is on the card, register it with the library
	 */

	/* Set up the TMap field with the information about the texture */
	memset(&TMap, 0, sizeof(TMap));
	TMap.u32Size = sizeof (TMap);

	if (src->mip_offset) {
		syslog("   it's mipmapped - doing mipmap process");
		/* Set up the structures for the mipmaps - this trashes texture_start */
		base_texture_size = src->width_b * src->height;
		maxlog2 = (int)(log2x >= log2y ? log2x : log2y);
		for (k=0; k < maxlog2; k++) {
			TMap.apvLevels[k] = texture_start;
			texture_start += base_texture_size;
			base_texture_size >>= 2;
		}
		TMap.bMipMap = TRUE;
	} else {
		syslog("   not mipmapped");
		/* Simple case: set the basic address only */
		TMap.apvLevels[0] = texture_start;
		TMap.bMipMap = FALSE;
	}

	TMap.u32MaxMapXSizeLg2 = log2x;
	TMap.u32MaxMapYSizeLg2 = log2y;
	SET_CIF_COLOR (TMap.clrTexChromaKey, BR_RED(src->colour_key), BR_GRN(src->colour_key),
										 BR_BLU(src->colour_key), BR_ALPHA(src->colour_key));

	/* Parse Brender pixelmap format to tell ATI libs what it is */
	switch(src->type) {
	case BR_PMT_RGB_565:
		TMap.eTexFormat = C3D_ETF_RGB565;
		break;
	case BR_PMT_RGB_555:
		TMap.eTexFormat = C3D_ETF_RGB1555;
		break;
	case BR_PMT_RGBA_4444:
		TMap.eTexFormat = C3D_ETF_RGB4444;
		break;
	case BR_PMT_RGBX_888:
	case BR_PMT_RGBA_8888:
		TMap.eTexFormat = C3D_ETF_RGB8888;  /* 32 bit only; no 24-bit */
		break;
#ifdef BR_PMT_RGB_332
	case BR_PMT_RGB_332:
		TMap.eTexFormat = C3D_ETF_RGB332;
		break;
#endif
	case BR_PMT_INDEX_8:
		syslog("   8-bit texture");
		TMap.eTexFormat = C3D_ETF_CI8;
		if (!ci8_texture_avail || !src->palette_handle) {
			syslog("   8-bit paletted texture can't be used: not supported or no palette set");
			src->uncacheable = BR_TRUE;
			return(BR_FALSE);
		}
		TMap.htxpalTexPalette = src->palette_handle;
		break;
	default: /* Can't support other texture types */
		syslog("   unsupported texture type");
		src->uncacheable = BR_TRUE; // No point in trying to render it
		return BR_FALSE;
	}

	/*
	 * HACK FOR CROC - Enable clamping on all textures
	 */
#if defined(C3D_CAPS2_TEXTURE_CLAMP)
	TMap.bClampS = TRUE;
	TMap.bClampT = TRUE;
#endif

	/* Register the texture with the ATI library */
	syslog("   calling TextureReg");
	ecRetVal = ATI3DCIF_TextureReg(&TMap, &texture_handle);
	if (ecRetVal != C3D_EC_OK) {
		syslog1("   failed, error code %d", ecRetVal);
		/* Assume this is a problem such as too many textures allocated; and
		 * so it might work if tried again later */
		return(BR_FALSE);
	}

	src->texture_handle = texture_handle;

	syslog("   all OK; finished");
	return(BR_TRUE);
}




/*
 * Setup hardware with correct render window
 *
 * Note that this ONLY applies to 3D functions; 2D functions will have
 * to be separately moved/clipped.
 */
br_boolean ATISetRenderWindow(LPDIRECTDRAWSURFACE surface,
                              br_int_32 x, br_int_32 y,
                              br_int_32 w, br_int_32 h,
							  br_device_pixelmap *orig_pm)
{
	void *    framebuffer;    // Start of video memory to be used
	RECT      lock_area;      // Area to be locked
	C3D_RECT  render_clip;    // C3D_RECT and RECT are the same but let's be nice
	long      pitch;
	br_uint_32 pixel_bits;

	syslog("SetRenderWindow:");

	/* Lock/unlock surface to get framebuffer address */
	lock_area.left = x;
	lock_area.right = x+w;
	lock_area.top = y;
	lock_area.bottom = y+h;
	syslog("   Calling lock");
	if (!ATILock(surface, &framebuffer, &lock_area, &pitch))
		return(BR_FALSE);
	syslog("   Calling unlock");
	if (!ATIUnlock(surface, framebuffer))
		return(BR_FALSE);

	/* If this is a sub pixelmap, shift the draw pointer to the
	 * correct place, and rely on Brender to know how to clip it */
	if(orig_pm->parent == BR_FALSE)
		{
		pixel_bits = orig_pm->pixel_bits;
		if(pixel_bits == 15) pixel_bits = 16;

		framebuffer = ((br_uint_32)framebuffer) + (br_uint_32)
						(orig_pm->sub_y_offset * orig_pm->pm_row_bytes) +
						(orig_pm->sub_x_offset * (pixel_bits/8));
		}


	syslog("   Set framebuffer ptr");
	/* Set details to rendering context: first address of framebuffer, then
	 * the desired clipping rectangle. */
	if (ATI3DCIF_ContextSetState(hRenderContext,
								 C3D_ERS_SURF_DRAW_PTR, &framebuffer) != C3D_EC_OK)
		return(BR_FALSE);

	pitch = (pitch/2 & 0xffff8);
	if (ATI3DCIF_ContextSetState(hRenderContext,
								 C3D_ERS_SURF_DRAW_PITCH, &pitch) != C3D_EC_OK)
		return(BR_FALSE);


	/* Let's let Brender clip for now */
#if 0
	render_clip.left = x;
	render_clip.right = x+w;
	render_clip.top = y;
	render_clip.bottom = y+h;
	syslog("   Set clip rect");
	if (ATI3DCIF_ContextSetState(hRenderContext,
								 C3D_ERS_SURF_VPORT, &render_clip) != C3D_EC_OK)
		return(BR_FALSE);

#endif

	return(BR_TRUE);
}



/*
 * Setup hardware with appropriate Z buffer information
 */
br_boolean ATISetZBufferState(br_boolean enable, LPDIRECTDRAWSURFACE surface,
                              br_int_32 w, br_int_32 h)
{
	void *     zbuffer_address;
	long zbuffer_pitch;
	C3D_EZMODE zmode;
	C3D_EZCMP  zcmp;

	syslog("SetZBufferState:");

	if (enable) {
		syslog("   Enabling Z-buffering");
	
		syslog("   Calling lock");
		if (!ATILock(surface, &zbuffer_address, NULL, &zbuffer_pitch))
			return(BR_FALSE);
		syslog("   Calling unlock");
		if (!ATIUnlock(surface, zbuffer_address))
			return(BR_FALSE);

		/* First set the Z surface address */
		syslog("   Set Z surface address");
		if (ATI3DCIF_ContextSetState(hRenderContext,
									 C3D_ERS_SURF_Z_PTR, &zbuffer_address) != C3D_EC_OK)
			return(BR_FALSE);

		/* Set the surface pitch: pitch is in pixels */
		syslog("   Set Z pitch");
		zbuffer_pitch /= 2;

		if (ATI3DCIF_ContextSetState(hRenderContext,
									 C3D_ERS_SURF_Z_PITCH, &zbuffer_pitch) != C3D_EC_OK)
			return(BR_FALSE);

		/* Set Z mode; always enabled if this function is called */
		syslog("   Set Z mode");
		zmode = C3D_EZMODE_TESTON_WRITEZ;
		if (ATI3DCIF_ContextSetState(hRenderContext,
									 C3D_ERS_Z_MODE, &zmode) != C3D_EC_OK)
			return(BR_FALSE);

		/* Set Z compare function; to be compatibile with BRender's default
		 * behaviour this is < mode */
		syslog("   Set Z compare function");
		zcmp = C3D_EZCMP_LESS;
		if (ATI3DCIF_ContextSetState(hRenderContext,
									 C3D_ERS_Z_CMP_FNC, &zcmp) != C3D_EC_OK)
			return(BR_FALSE);


	} else {
		syslog("   disabling Z-buffering");
	
		/* Set Z mode to disabled */
		syslog("   Set Z mode");
		zmode = C3D_EZMODE_OFF;
		if (ATI3DCIF_ContextSetState(hRenderContext,
									 C3D_ERS_Z_MODE, &zmode) != C3D_EC_OK)
			return(BR_FALSE);

	}
	syslog("   complete");

	return(BR_TRUE);
}



/*
 * Start up all ATI renderer components
 */
br_boolean ATIInstantiateLibrary(void)
{

  syslog("InstantiateLibrary:");

  syslog("   Init");
  if (ATI3DCIF_Init() != C3D_EC_OK)
    return(BR_FALSE);

  /* Create rendering context */
  syslog("   create render context");
  if (!(hRenderContext = ATI3DCIF_ContextCreate()))
    return(BR_FALSE);

  syslog("   get card capabilities");
  CIFInfo.u32Size = sizeof (CIFInfo);
  if (ATI3DCIF_GetInfo (&(CIFInfo)) != C3D_EC_OK)
    return(BR_FALSE);

  /* Parse capabilities */
  if ((CIFInfo.u32CIFCaps1 & C3D_CAPS1_Z_BUFFER) == C3D_CAPS1_Z_BUFFER) 
    z_buffer_exists = BR_TRUE;
  else
    z_buffer_exists = BR_FALSE;
  if ((CIFInfo.u32CIFCaps1 & C3D_CAPS1_CI8_TMAP) == C3D_CAPS1_CI8_TMAP) 
    ci8_texture_avail = BR_TRUE;
  else
    ci8_texture_avail = BR_FALSE;

  texture_surface_head = NULL;

  return(BR_TRUE);
}



/*
 * Finish off the renderer
 */
br_boolean ATITerminateLibrary(void)
{
  syslog("TerminateLibrary:");
  /* Close down texture cache, calling the callback to clear all the
   * texture handles from the ATI library: do this by repeated calls
   * to the clean-out function*/
  ATIClearTextureCache();

  /* Finish off rendering handle */
  ATI3DCIF_ContextDestroy(hRenderContext);

  /* Terminate 3DCIF */
  ATI3DCIF_Term();

  return(BR_TRUE);
}



