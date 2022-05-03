/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: sbuffer.c 1.1 1997/12/10 16:51:25 jon Exp $
 * $Locker: $
 *
 * Stored buffer methods for S3 VirGE 
 *
 */
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <windows.h>
#include <ddraw.h>

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: sbuffer.c 1.1 1997/12/10 16:51:25 jon Exp $");

/* Default dispatch table for primitive state (defined at end of file) */
static struct br_buffer_stored_dispatch bufferStoredDispatch;


/* External routines to allow us to find out if a pixelmap is a real device pixelmap or not  */
extern br_boolean CheckFrontPixelmapDispatch(void *address);
extern br_boolean CheckBackPixelmapDispatch(void *address);


/*
 * Primitive state info. template
 */
#define F(f)	offsetof(struct br_buffer_stored, f)

static struct br_tv_template_entry bufferStoredTemplateEntries[] = {
	{BRT_IDENTIFIER_CSTR,	0,	F(identifier),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
};
#undef F

static struct br_tv_template bufferStoredTemplate = {
	BR_ASIZE(bufferStoredTemplateEntries),
	bufferStoredTemplateEntries
};


/*
 * Hmmm
 */
void StoredBufferReset(void)
{
  CLEAR_TEMPLATE(bufferStored);
}

void ClearSbufferTemplates(void)
{
	CLEAR_TEMPLATE(bufferStored);
}



/*
 * This is the default palette used for paletted textures with no palette data attached
 * It is used to convert them to 15 bit textures for true colour video modes
 * Set in devclut.c
 */
br_colour	S3_default_palette[256];

/* This is the flag used to decide what the driver behaviour is with 8 bit textures
 * it is set in devpixmp.c when the front surface is created */

extern br_boolean S3_convert_8bit_to_15;


/* Macro for 24 bit to 15 bit colour conversion */
#define CONV24TO15(a)   ((a & 0xFF)>>3) | ((a & 0xF800) >> 6) | ((a & 0xF80000) >> 9) 

/* Spammy little routine to get the format of an S3 surface */

br_uint_32 GetS3Format(br_device_pixelmap *pm)
{
	switch(pm->pm_type)
		{
		case BR_PMT_INDEX_8:
			return(S3DTK_TEXPALETTIZED8);
		case BR_PMT_RGB_555:
			return(S3DTK_TEXARGB1555);
		case BR_PMT_RGBX_888:
		case BR_PMT_RGBA_8888:
			return(S3DTK_TEXARGB8888);
		case BR_PMT_RGBA_4444:
			return(S3DTK_TEXARGB4444);
		default:
			return(S3DTK_TEXPALETTIZED8);
		}
}


/* Routine used to convert an 8 bit pixelmap into a 15 bit render buffer
 * If create_alpha is TRUE then the top bit of the data will be used for a 1 bit alpha mask.
 * and any texels in colour 0 will be transparent. Otherwise the top bit of all texels will
 * be 0 */

br_boolean S3Convert8To15(br_device_pixelmap *pm,
						  struct render_buffer *rb,
						  br_buffer_stored *self,
						  br_boolean has_palette,
						  br_boolean create_alpha)
{
	br_uint_16	*new_pixel=NULL;
	br_uint_8	*old_pixel;
	br_colour	*palette;
	br_uint_32	num_locations;
	br_uint_32  i;

	num_locations = pm->pm_width * pm->pm_height;

	if(has_palette)
		palette = pm->pm_map->pixels;
   else
      palette = &(S3_default_palette[0]);

	/* Converted pixels are a child resource of the stored buffer */
	new_pixel = BrResAllocate(self,
							  num_locations*sizeof(br_uint_16),
							  BR_MEMORY_SCRATCH);
	if(!new_pixel)
		return(BR_FALSE);

	/* Now do the conversion */
	rb->base_mem = new_pixel;
	old_pixel = pm->pm_pixels;

	for(i=0;i<num_locations;i++,new_pixel++,old_pixel++)
		{
		*new_pixel = CONV24TO15(palette[*old_pixel]);
		if(create_alpha)
			{
			if(*old_pixel == 0)		  /* Palette (0) is transparent (Alpha Mask) */
				*new_pixel &= 0x7FFF; /* Clear Alpha bit */
			else
				*new_pixel |= 0x8000;
			}
		}

	rb->type = BR_PMT_RGB_555;
	if(CheckFrontPixelmapDispatch(pm->dispatch) || CheckBackPixelmapDispatch(pm->dispatch))
		rb->s3_surface = pm->s3_surface;
	else
		{
		rb->s3_surface.sfWidth = pm->pm_width;
		rb->s3_surface.sfHeight = pm->pm_height;
		}

	rb->s3_surface.sfFormat = S3DTK_TEXARGB1555;

	rb->s3_surface.sfOffset = rb->base_mem;
	rb->bpp = 2;
	rb->width_b = pm->pm_width * 2;
	rb->width_p = pm->pm_width;
	rb->height = pm->pm_height;
	rb->stride = pm->pm_row_bytes * 2;
	rb->size = rb->width_b * rb->height;
	rb->base = (br_uint_8 *)(rb->base_mem)+ 
		        pm->pm_base_y * rb->stride +
		        pm->pm_base_x * rb->bpp;

	rb->sel = NULL;

	rb->mip_offset = 0 ; /* I'm not allowing mipmapped 8 bit textures. */


	return(BR_TRUE);
}




/*
 * Build a render_buffer structure from a pixelmap  - used to create texture maps and
 * outputs (screen and Z). If it is being called to set up a texture map then the
 * texture flag will be BR_TRUE, and additional processing is carried out for alpha
 * channel and palette processing
 */
br_error S3SetupRenderBuffer(struct render_buffer *rb,
						br_device_pixelmap *pm,
						br_buffer_stored *self,
						br_boolean texture)
{
	br_boolean need_convert = BR_FALSE;
	int bpp = 1;
	br_boolean device_pm = BR_FALSE;

	switch(pm->pm_type)
		{
		case BR_PMT_INDEX_8:
			bpp = 1;
			need_convert = BR_TRUE;
			break;
		case BR_PMT_RGB_555:
		case BR_PMT_RGB_565:
		case BR_PMT_DEPTH_16:
		case BR_PMT_RGBA_4444:
			bpp = 2;
			break;

		case BR_PMT_RGB_888:
			bpp = 3;
			break;

		case BR_PMT_RGBX_888:
		case BR_PMT_RGBA_8888:
			bpp = 4;
			break;
		}

	/* Find out if this is a device pixelmap */
	device_pm = CheckFrontPixelmapDispatch(pm->dispatch) || CheckBackPixelmapDispatch(pm->dispatch);

	rb->type = pm->pm_type;
	rb->bpp = bpp;
	rb->width_b = pm->pm_width * bpp;
	rb->width_p = pm->pm_width;
	rb->height = pm->pm_height;
	rb->stride = pm->pm_row_bytes;
	rb->size = pm->pm_height * pm->pm_row_bytes;

	/* If this is a device pixelmap then we must lock and unlock it to get the correct pointer
	 * for the base of the pixel data */
	if(device_pm)
		{
		if(S3Lock(pm->surface, &pm->pm_pixels, NULL, NULL))
			{
			/* Memory base pointer */
			rb->base_mem = pm->pm_pixels;
			/* 0,0 pixel/texel */
			rb->base = (br_uint_8 *)(pm->pm_pixels)+ 
						pm->pm_base_y * pm->pm_row_bytes +
						pm->pm_base_x * bpp;
			S3Unlock(pm->surface,pm->pm_pixels);
			}
		else
			{
			rb->base_mem = pm->pm_pixels;
			/* 0,0 pixel/texel */
			rb->base = (br_uint_8 *)(pm->pm_pixels)+ 
						pm->pm_base_y * pm->pm_row_bytes +
						pm->pm_base_x * bpp;
         return(BRE_DEV_LOCK_FAIL);
			}
		}
	else
		{
		/* Memory base pointer */
		rb->base_mem = pm->pm_pixels;
		/* 0,0 pixel/texel */
		rb->base = (br_uint_8 *)(pm->pm_pixels)+ 
					pm->pm_base_y * pm->pm_row_bytes +
					pm->pm_base_x * bpp;
		}

	rb->sel = (br_uint_16)pm->pm_pixels_qualifier;


	rb->mip_offset = (br_uint_16)pm->pm_mip_offset;

	/* We must discover if this is a device pixelmap so that we know if the S3
	 * pixelmap fields are valid. The only way I know to do this is to see if its
	 * dispatch table pointer matches that for device pixelmaps */
	if(device_pm)
		{
		/* Copy the data from the parent pixelmap */
		rb->s3_surface = pm->s3_surface;
		if(pm->video_memory)
		   rb->s3_surface.sfFormat |= S3DTK_VIDEO;
		/* Set the render buffer field to reflect the location of the parent buffer */
		rb->always_in_video_memory = pm->video_memory;
		if(rb->always_in_video_memory == BR_TRUE)
			rb->in_video_memory = BR_TRUE;
		}
	else
		{
		/* Create the appropriate data */
		rb->s3_surface.sfWidth = pm->pm_width;
		rb->s3_surface.sfHeight = pm->pm_height;
		rb->s3_surface.sfOffset = pm->pm_pixels;
		rb->s3_surface.sfFormat = GetS3Format(pm);
		/* Non device pixelmaps are always in system memory */
		rb->always_in_video_memory = BR_FALSE;
		}


	/* Only do the following for texture buffers, not output buffers.
	 * NOTE! buffers allocated in video memory by a match operation _cannot_
	 * be colour converted */
	if((texture == BR_TRUE) && (rb->always_in_video_memory == BR_FALSE))
		{
		/* First convert the texture from 8 to 15 bit if required
		 * S3_convert_8bit_to_15 is set when the main device pixelmap is created in
		 * a true colour output mode */
		if(S3_convert_8bit_to_15 == BR_TRUE)
			{
			/* If we have a paletted texture and are in a true colour video mode 
			 * then we must convert it to a 15 bit equivalent for the purposes of
			 * the S3 (no paletted texture support in true colour modes).*/
			if(need_convert == BR_TRUE)
				{
				if(self)
					{
					if(pm->pm_map)
                  {
						if(!S3Convert8To15(pm,rb,self,BR_TRUE,BR_TRUE))
                     return(BRE_FAIL);
                  }
					else
                  {
						if(!S3Convert8To15(pm,rb,self,BR_FALSE,BR_TRUE))
                     return(BRE_FAIL);
                  }
					}
				}
			}

		/* Check the texture map out to see if it contains valid alpha channel information */
		rb->alpha_pixels = BR_FALSE;  /* default to off */

		/* RGBA_8888 and 4444 texture maps should always have a valid alpha channel */
		if((rb->type == BR_PMT_RGBA_8888) || (rb->type == BR_PMT_RGBA_4444))
			rb->alpha_pixels = BR_TRUE;

		/* RGB_555 textures _may_ have alpha information in the high bit. This is the time
		 * consuming part - we must check through the texture map. If the high bit of any
		 * 16 bit word differs from the high bit of the first 16 bit word (pay attention now!),
		 * then we assume that the texture map contains an alpha channel and flag it as such. */
		 if(rb->type == BR_PMT_RGB_555)
			{
			/* Must use 16 bit pointers for this method */
			br_uint_16 *current_texels;
			br_uint_16 *endptr;

			endptr = (br_uint_16*)(rb->base_mem);
			endptr += (rb->width_p * rb->height);

			current_texels = (br_uint_16*)rb->base_mem;
			if(*current_texels & 0x8000)
				{
				/* First texel has high bit set - scan for ones with high bit cleared */
				for(current_texels = (br_uint_16*)rb->base_mem;
					current_texels < endptr;
					current_texels++)
					{
					if(!(*current_texels & 0x8000))
						{
						rb->alpha_pixels = BR_TRUE;
						break;
						}
					}
				}
			else
				{
				/* First texel has high bit cleared - scan for ones with high bit set */
				for(current_texels = (br_uint_16*)rb->base_mem;
					current_texels < endptr;
					current_texels++)
					{
					if(*current_texels & 0x8000)
						{
						rb->alpha_pixels = BR_TRUE;
						break;
						}
					}
				}
			}
		}

	/* Make sure that the surface is flagged as a texture if that is its use */
	if(texture == BR_TRUE)
       rb->s3_surface.sfFormat |= S3DTK_TEXTURE;
   else
       rb->s3_surface.sfFormat &= ~S3DTK_TEXTURE;

#if 0
   /* If the texture dimensions differ by >= 128 then mark as uncacheable */
   if(texture == BR_TRUE)
      {
      if(abs(rb->width_p - rb->height) >= 128)
         rb->uncacheable = BR_TRUE;
      }
#endif

   return(BRE_OK);

}

/*
 * Set up a static device object
 */
struct br_buffer_stored * BufferStoredHardS3Allocate(struct br_primitive_library *plib,
	br_token use, struct br_device_pixelmap *pm, br_token_value *tv)
{
	struct br_buffer_stored * self;
	char *ident;
   br_error r;

	switch(use) {
	case BRT_TEXTURE_O:
	case BRT_COLOUR_MAP_O:
		ident ="Colour-Map";
		break;
	case BRT_INDEX_SHADE_O:
		ident ="Shade-Table";
		break;
	case BRT_INDEX_BLEND_O:
		ident ="Blend-Table";
		break;
	case BRT_SCREEN_DOOR_O:
		ident ="Screendoor-Table";
		break;
	case BRT_INDEX_LIGHT_O:
		ident ="Lighting-Table";
		break;
	case BRT_BUMP_O:
		ident ="Bump-Map";
		break;
	default:
		return NULL;
	}

	self = BrResAllocate(DRIVER_RESOURCE, sizeof(*self), BR_MEMORY_OBJECT);

	if(self == NULL)
		return NULL;

	self->dispatch = &bufferStoredDispatch;
	self->identifier = ident;

	/*
	 * If caller does not guarantee to keep source data around, or
	 * source data is not memory mapped, then clone an in-memory copy
	 */

	self->flags |= SBUFF_SHARED;

   r = S3SetupRenderBuffer(&self->buffer, pm, self,BR_TRUE);

   if(r != BRE_OK)
      {
      BrResFree(self);
      return(NULL);
      }

   self->plib = plib;

   ObjectContainerAddFront(plib,(br_object*)self);

	return self;
}


/* If a buffer is updated that is a current input buffer we must
 * force a reload (and possibly a re-conversion)of the texture map */

static br_error BR_CMETHOD_DECL(br_buffer_stored_hardS3, update)(
	struct br_buffer_stored *self,
	struct br_device_pixelmap *pm,
	br_token_value *tv)
{
   br_error r;

	/* Unpack any hints */
	if(self->flags & SBUFF_SHARED)
		{
		if(S3_convert_8bit_to_15 == BR_TRUE)
			{
			/* If we have a paletted texture then we must convert it to a 15 bit equivalent
			 * for the purposes of the S3 (no paletted texture support) */
			if(pm->pm_type == BR_PMT_INDEX_8)
				{
				/* Free the converted pixels pointer in the render buffer IF this is not a video memory pixelmap.
				 * If it is _only_ located in video memory then we do not have jurisdiction over the data
				 * so we cannot either convert or free data associated with this buffer */
				if((self->buffer.base_mem)&&(self->buffer.always_in_video_memory == BR_FALSE))
					{
					BrResFree(self->buffer.base_mem);
					self->buffer.base = NULL;
					}
				}
			}

		/* Set up the new render buffer */
      r = S3SetupRenderBuffer(&self->buffer,pm,self,BR_TRUE);
      if(r != BRE_OK)
         return(r);
		} 

	self->buffer_updated = BR_TRUE;

   return(BRE_OK);
 }



/* If a buffer is freed then we must make sure that its ghost does not */
/* come back to haunt us */
/* ie if it is the current texture buffer then we must force the current */
/* buffer to NULL */

static void BR_CMETHOD_DECL(br_buffer_stored_hardS3, free)(br_buffer_stored *self)
{
   S3LoseRenderBuffer(&self->buffer);
   ObjectContainerRemove(self->plib,(br_object*)self);
   BrResFreeNoCallback(self);
}

static br_token BR_CMETHOD_DECL(br_buffer_stored_hardS3, type)(br_buffer_stored *self)
{
	return BRT_BUFFER_STORED;
}

static br_boolean BR_CMETHOD_DECL(br_buffer_stored_hardS3, isType)(br_buffer_stored *self, br_token t)
{
	return (t == BRT_BUFFER_STORED) || (t == BRT_OBJECT);
}

static br_int_32 BR_CMETHOD_DECL(br_buffer_stored_hardS3, space)(br_buffer_stored *self)
{
	return BrResSizeTotal(self);
}

static struct br_tv_template * BR_CMETHOD_DECL(br_buffer_stored_hardS3,templateQuery)
	(br_buffer_stored *self)
{
	bufferStoredTemplate.res = DRIVER_RESOURCE;
	return &bufferStoredTemplate;
}

/*
 * Default dispatch table for device
 */
static struct br_buffer_stored_dispatch bufferStoredDispatch = {
   NULL,NULL,NULL,NULL,
	BR_CMETHOD_REF(br_buffer_stored_hardS3,	free),
	BR_CMETHOD_REF(br_object_s3,			identifier),
	BR_CMETHOD_REF(br_buffer_stored_hardS3, type),
	BR_CMETHOD_REF(br_buffer_stored_hardS3, isType),
	BR_CMETHOD_REF(br_object_s3, 			device),
	BR_CMETHOD_REF(br_buffer_stored_hardS3, space),

	BR_CMETHOD_REF(br_buffer_stored_hardS3,	templateQuery),
	BR_CMETHOD_REF(br_object,				query),
	BR_CMETHOD_REF(br_object, 				queryBuffer),
	BR_CMETHOD_REF(br_object, 				queryMany),
	BR_CMETHOD_REF(br_object, 				queryManySize),
	BR_CMETHOD_REF(br_object, 				queryAll),
	BR_CMETHOD_REF(br_object,	 			queryAllSize),

	BR_CMETHOD_REF(br_buffer_stored_hardS3,	update),
};


