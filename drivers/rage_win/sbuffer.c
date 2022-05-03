/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: sbuffer.c 1.1 1997/12/10 16:50:39 jon Exp $
 * $Locker: $
 *
 * Stored buffer methods for ATI 3D Expression
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

BR_RCS_ID("$Id: sbuffer.c 1.1 1997/12/10 16:50:39 jon Exp $");

/*
 * Default dispatch table for primitive state (defined at end of file)
 */
static struct br_buffer_stored_dispatch bufferStoredDispatch;

/* External flag set by atilink.c to indicate paletted texture availability */
extern br_boolean      ci8_texture_avail;

/* External routines to allow us to check if incoming pixelmaps are device or not */
extern br_boolean CheckFrontDispatch(void *address);
extern br_boolean CheckBackDispatch(void *address);

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


/* Structure storing palette details for any textures with palettes. Although it
 * might make more sense to store all this stuff in devclut.c, since in the
 * RAGE driver palettes are only associated with textures I have kept the code
 * to handle them together */

typedef struct palette_list_element
{
   br_pixelmap           *palette;
   C3D_HTXPAL            palette_handle;
   C3D_PALETTENTRY      *c3d_palette;

   /* Instance counter allows us to delete the entry when the last
    * texture that uses it is freed */
   br_uint_32            num_instances;

   struct palette_list_element *next;
} palette_stored;

/* This is the head of the list of brender palettes for paletted textures */
static palette_stored  pal_head = {NULL,NULL,NULL,0,NULL};

/* This is the default palette set up by BrPixelmapPaletteSet */
static palette_stored default_palette = {NULL,NULL,NULL,0,NULL};


void	ResetATIPalettes(void)
{
	palette_stored *temp,*current;
	// Free any unfreed palette data
	while(pal_head.next != NULL)
	{
	for(temp = &pal_head,current=temp;temp->next != NULL;current=temp,temp = temp->next)
		;
	// Free this entry
	current->next = NULL;
	if(temp->palette_handle)
		{
		ATI3DCIF_TexturePaletteDestroy(temp->palette_handle);
		temp->palette_handle = NULL;
		}
	if(temp->c3d_palette)
		{
		BrResFree(temp->c3d_palette);
		temp->c3d_palette = NULL;
		}
	}

	if(default_palette.palette_handle)
		{
		ATI3DCIF_TexturePaletteDestroy(default_palette.palette_handle);
		default_palette.palette_handle = NULL;
		}
	if(default_palette.c3d_palette)
		{
		BrResFree(default_palette.c3d_palette);
		default_palette.c3d_palette = NULL;
		}

	memset(&default_palette,0,sizeof(palette_stored));
	memset(&pal_head,0,sizeof(palette_stored));
}



/* Call made to set an entry in the default palette */
br_error SetDefaultPaletteEntry(br_uint_32 index,br_colour entry)
{
   C3D_EC        ecRetVal;

   /* Storage must be allocated by this time */
   if(!default_palette.c3d_palette)
      return BRE_FAIL;

   if(index > 255)
      return BRE_FAIL;

   default_palette.c3d_palette[index].r = BR_RED(entry);
   default_palette.c3d_palette[index].g = BR_GRN(entry);
   default_palette.c3d_palette[index].b = BR_BLU(entry);
   /* Set flags to LOADPALETTEENTRY */
   default_palette.c3d_palette[index].flags = 0x40;

   /* If there is no palette handle for the default palette then make one */
   if(!default_palette.palette_handle)
      {
      /* Register this palette with 3DCIF */
      ecRetVal = ATI3DCIF_TexturePaletteCreate(C3D_ECI_TMAP_8BIT,
                                               default_palette.c3d_palette,
                                               &(default_palette.palette_handle));
      if(ecRetVal != C3D_EC_OK)
         return BRE_FAIL;
      }

   return BRE_OK;
}


/* Call made to set entries in the default palette */
br_error SetDefaultPaletteEntries(br_uint_32 start,br_uint_32 num_entries,br_colour *entries)
{
   br_uint_32 i;
   C3D_EC        ecRetVal;

   /* Storage must be allocated by this time */
   if(!default_palette.c3d_palette)
      return BRE_FAIL;

   if(start + num_entries > 256)
      num_entries = 256-start;

   /* Set the entries in the palette */
   for(i=start; i < start+num_entries;i++)
      {
      default_palette.c3d_palette[i].r = BR_RED(entries[i]);
      default_palette.c3d_palette[i].g = BR_GRN(entries[i]);
      default_palette.c3d_palette[i].b = BR_BLU(entries[i]);
      /* Set Flags field to LOADPALETTEENTRY */
      default_palette.c3d_palette[i].flags = 0x40;
      }

   /* If there is no palette handle for the default palette then make one */
   if(!default_palette.palette_handle)
      {
      /* Register this palette with 3DCIF */
      ecRetVal = ATI3DCIF_TexturePaletteCreate(C3D_ECI_TMAP_8BIT,
                                               default_palette.c3d_palette,
                                               &(default_palette.palette_handle));
      if(ecRetVal != C3D_EC_OK)
         return BRE_FAIL;
      }

   return BRE_OK;

}


/* Call made to create the default palette initially */
br_error CreateDefaultPalette()
{
   /* If we don't have storage for the colour palette then create it */
   if(!default_palette.c3d_palette)
      /* Create the C3D colour palette storage*/
      default_palette.c3d_palette = (C3D_PALETTENTRY*)BrResAllocate(NULL,256*sizeof(C3D_PALETTENTRY),BR_MEMORY_SCRATCH);

   /* If we still don't have storage, then return failure */
   if(!default_palette.c3d_palette)
      return BRE_FAIL;

   return BRE_OK;
}

br_error DestroyDefaultPalette()
{
   if(default_palette.c3d_palette)
      BrResFree(default_palette.c3d_palette);

   if(default_palette.palette_handle)
      ATI3DCIF_TexturePaletteDestroy(default_palette.palette_handle);

   default_palette.c3d_palette = NULL;
   default_palette.palette_handle = NULL;

   return BRE_OK;
}



void StoredBufferReset(void)
{
  CLEAR_TEMPLATE(bufferStored);
}


/*
 * Build a render_buffer structure from a pixelmap
 */
void ATISetupRenderBuffer(struct render_buffer *rb,
                       br_device_pixelmap *pm,
                       br_boolean texture)
{
   palette_stored *current,*old;   
   br_boolean found = BR_FALSE;
   C3D_EC        ecRetVal;          // Error code from ATIC3DCIF routines
   br_colour     *colptr;
   br_uint_32 i;
   br_boolean device_pm;
	int bpp = 1;

	switch(pm->pm_type)
      {
	case BR_PMT_INDEX_8:
		bpp = 1;
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

	   case BR_PMT_RGBA_8888:
         /* deliberate fall through */
	   case BR_PMT_RGBX_888:
		   bpp = 4;
		   break;
	   }

   /* Find out if this is a device pixelmap */
   device_pm = CheckFrontDispatch(pm->dispatch);
   if(!device_pm)
      device_pm = CheckBackDispatch(pm->dispatch);

	rb->type = pm->pm_type;
	rb->bpp = bpp;
	rb->width_b = pm->pm_width * bpp;
	rb->width_p = pm->pm_width;
	rb->height = pm->pm_height;
	rb->stride = pm->pm_row_bytes;
	rb->size = pm->pm_height * pm->pm_row_bytes;

	rb->base = (br_uint_8 *)(pm->pm_pixels)+ 
		        pm->pm_base_y * pm->pm_row_bytes +
		        pm->pm_base_x * bpp;

	rb->sel = (br_uint_16)pm->pm_pixels_qualifier;
	rb->mip_offset = (br_uint_16)pm->pm_mip_offset;

	
#if 0
   /* Colour keying method; this is not necessarily portable so off until BRender defines it */
   if(pm->pm_copy_function == BR_PMCOPY_SRC_KEYED)
      {
      rb->chroma_enable = BR_TRUE;
      rb->colour_key = pm->pm_src_key.low;
      }
   else
      {
      rb->chroma_enable = BR_FALSE;
      rb->colour_key = pm->pm_src_key.low;
      }
#else
   /* For now, make it behave as software renderers; key always on and always black */
   rb->chroma_enable = BR_TRUE;
   rb->colour_key = 0;
#endif

   /* Check if this buffer represents a hidden buffer pixelmap */
   if(device_pm)
      {
      if(pm->videomemory)
         {
         rb->always_in_video_memory = BR_TRUE;
         rb->surface = pm->surface;
         rb->alpha_pixels = BR_FALSE;
         /* Do not do texture map processing for hidden buffers */
         texture = BR_FALSE;
         /* hidden buffer textures do not chroma key */
         rb->chroma_enable = BR_FALSE;
         }
      else
         rb->always_in_video_memory = BR_FALSE;
      }
   else
      rb->always_in_video_memory = BR_FALSE;


   if(texture==BR_TRUE)
      {
  	   rb->surface = NULL;

      rb->u_range = 1.0f;
	  rb->v_range = 1.0f;

      /* Now we must find out if the buffer contains valid alpha channel
       * information, and if it does flag it as such */

      rb->alpha_pixels = BR_FALSE;  /* default to off */

      /* RGBA_8888 and 4444 texture maps should always have a valid alpha channel */
      if((rb->type == BR_PMT_RGBA_8888) || (rb->type == BR_PMT_RGBA_4444))
         rb->alpha_pixels = BR_TRUE;



      /* RGB_555 textures _may_ have alpha information in the high bit. This is the time
       * consuming part - we must check through the texture map. If the high bit of any
       * 16 bit word differs from the high bit of the first 16 bit word (pay attention now!),
       * then we assume that the texture map contains an alpha channel and flag it as such. */

       if((rb->type == BR_PMT_RGB_555) && (rb->base))
         {
         /* Must use 16 bit pointers for this method */
         br_uint_16 *current_texels;
         br_uint_16 *endptr;

         endptr = (br_uint_16*)(rb->base);
         endptr += (rb->width_p * rb->height);

         current_texels = (br_uint_16*)rb->base;
         if(*current_texels & 0x8000)
            {
            /* First texel has high bit set - scan for ones with high bit cleared */
            for(current_texels = (br_uint_16*)rb->base;
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
            for(current_texels = (br_uint_16*)rb->base;
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
      /* This next bit must be done last! (It returns in case of failure
       * so any code placed after it is not guaranteed to be executed! */

      /* If indexed textures are supported... */

      if (ci8_texture_avail)
         {
         /* If the pixelmap is indexed... */
         if(pm->pm_type == BR_PMT_INDEX_8)
            {
            /* If it has an attached palette... */
            if(pm->pm_map)
               {
               /* We must scan through the list of current palettes that
                * we have stored to see if we need to store a new one
                * for this buffer... */

               for(current=&pal_head;current != NULL;old=current,current = current->next)
                  {
                  if(current->palette == pm->pm_map)
                     {
                     /* Found one ... */

                     rb->palette_handle = current->palette_handle;
                     found = BR_TRUE;

                     /* Increment instance counter for this palette */
                     current->num_instances++;
                     break;
                     }
                  }

               if(found == BR_FALSE)
                  {
                  current = old;
                  /* This palette has not been registered, so we must register it */

                  current->next = (palette_stored*)BrResAllocate(NULL,sizeof(palette_stored),BR_MEMORY_SCRATCH);

                  /* If we can't allocate a new unit then return */
                  if(!(current->next))
                     return;

                  old = current;
                  current=current->next;

                  /* Create the C3D colour palette storage as a child resource of the palette */
                  current->c3d_palette = (C3D_PALETTENTRY*)BrResAllocate(current,256*sizeof(C3D_PALETTENTRY),BR_MEMORY_SCRATCH);

                  /* If we can't, then free resources and return */
                  if(!(current->c3d_palette))
                     {
                     BrResFree(current);
                     old->next = NULL;
                     return;
                     }


                  /* Fill in the palette details */
                  colptr = (br_colour*)pm->pm_map->pixels;
                  for(i=0; i<256; i++)
                     {
                     current->c3d_palette[i].r = BR_RED(colptr[i]);
                     current->c3d_palette[i].g = BR_GRN(colptr[i]);
                     current->c3d_palette[i].b = BR_BLU(colptr[i]);

                     /* Alpha must be 0x40 for ATIs palette system. Why? */
                     current->c3d_palette[i].flags = 0x40;
                     }

                  /* Register this palette with 3DCIF */
                  ecRetVal = ATI3DCIF_TexturePaletteCreate(C3D_ECI_TMAP_8BIT,
                                                        current->c3d_palette,
                                                        &current->palette_handle);

                  /* Again, if we fail then free up resources and return */
                  if (ecRetVal != C3D_EC_OK)
                     {
                     BrResFree(current);
                     old->next = NULL;
                     return;
                     }

                  /* Otherwise, we've succeeded, so set the palette handle
                   * for the render buffer, and set instance counter to 1 */

                  rb->palette_handle = current->palette_handle;
                  current->palette = pm->pm_map;
                  current->num_instances = 1;
                  }
               }
            /* Otherwise we must use the default palette if it is available */
            else
               rb->palette_handle = default_palette.palette_handle;
            }
         }
      }
}




/*
 * Set up a static device object
 */
struct br_buffer_stored * BufferStoredHardATIAllocate(struct br_primitive_library *plib,
	br_token use, struct br_device_pixelmap *pm, br_token_value *tv)
{
	struct br_buffer_stored * self;
	char *ident;

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
	 * If caller does not garuantee to keep source data around, or
	 * source data is not memory mapped, then clone an in-memory copy
	 */

	self->flags |= SBUFF_SHARED;

	/* Also used to set up palette */
	ATISetupRenderBuffer(&self->buffer, pm, BR_TRUE);

	self->plib = plib;

	ObjectContainerAddFront(plib,(br_object*)self);

	return self;
}


/* When freeing a stored buffer, this function is called to maintain the palette list */
static br_error ManagePaletteList(br_buffer_stored *self)
{
   palette_stored *current,*old;
   br_boolean found = BR_FALSE;
   
   /* Perform management on the palette list */
   if(self->buffer.palette_handle)
      {
      /* Check to see if it was the default palette */
      if(self->buffer.palette_handle != default_palette.palette_handle)
         {
         /* It had a palette handle, so search the palette list for the palette
          * entry that it used */
         for(current=&pal_head;
             current != NULL;
             old=current,current = current->next)
            {
            if(current->palette_handle == self->buffer.palette_handle)
               {
               /* Found it ... */
               found = BR_TRUE;
               current->num_instances--;
               if(current->num_instances == 0)
                  {
                  /* The last instance of a texture using this palette is
                   * being deleted, so delete the entry */
                  old->next = current->next; /* Relink the list */
                  /* Release the palette handle */
                  ATI3DCIF_TexturePaletteDestroy(current->palette_handle);
                  BrResFree(current);
                  self->buffer.palette_handle = NULL;
                  break;
                  }
               }
            }
         if(found == BR_FALSE)
            /* Couldn't find the palette entry! BAD! */
            return BRE_FAIL;
         }
      else
         {
         self->buffer.palette_handle = NULL;
         /* Don't have to do any management for the default palette */
         return BRE_OK;
         }
      }
   return BRE_OK;
}

/* If a buffer is updated that is the current input buffer we must
 * force a reload of the texture map. In actual fact, the easiest
 * thing to do is to recreate the render buffer structure from
 * scratch, so this is what we do */

static br_error BR_CMETHOD_DECL(br_buffer_stored_hardATI, update)(
	struct br_buffer_stored *self,
	struct br_device_pixelmap *pm,
	br_token_value *tv)
{
	br_error r;

	if(self->flags & SBUFF_SHARED)
		{
		/* Sort out the buffer's palette if necessary (should check the return value)*/
		r = ManagePaletteList(self);

		/* Lose the old render buffer, and assume it's no longer valid */
		ATILoseRenderBuffer(&self->buffer);

		/* Set up a new valid render buffer */
		ATISetupRenderBuffer(&self->buffer,pm,BR_TRUE);
		}

	self->buffer_updated = BR_TRUE;
	return BRE_OK;
}



/* If a buffer is freed then we must make sure that its ghost does not
 * come back to haunt us - ie if it is the current texture buffer then
 * we must force the current buffer to NULL */

static void BR_CMETHOD_DECL(br_buffer_stored_hardATI, free)(br_buffer_stored *self)
{
   br_error r;

   /* Sort out the palette handle if any */
   r = ManagePaletteList(self);
   ATILoseRenderBuffer(&self->buffer);

   ObjectContainerRemove(self->plib,(br_object*)self);   
   BrResFreeNoCallback(self);
}

static br_token BR_CMETHOD_DECL(br_buffer_stored_hardATI, type)(br_buffer_stored *self)
{
	return BRT_BUFFER_STORED;
}

static br_boolean BR_CMETHOD_DECL(br_buffer_stored_hardATI, isType)(br_buffer_stored *self, br_token t)
{
	return (t == BRT_BUFFER_STORED) || (t == BRT_OBJECT);
}

static br_int_32 BR_CMETHOD_DECL(br_buffer_stored_hardATI, space)(br_buffer_stored *self)
{
	return BrResSizeTotal(self);
}

static struct br_tv_template * BR_CMETHOD_DECL(br_buffer_stored_hardATI,templateQuery)
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
	BR_CMETHOD_REF(br_buffer_stored_hardATI,	free),
	BR_CMETHOD_REF(br_object_ati,			identifier),
	BR_CMETHOD_REF(br_buffer_stored_hardATI, type),
	BR_CMETHOD_REF(br_buffer_stored_hardATI, isType),
	BR_CMETHOD_REF(br_object_ati, 			device),
	BR_CMETHOD_REF(br_buffer_stored_hardATI, space),

	BR_CMETHOD_REF(br_buffer_stored_hardATI,	templateQuery),
	BR_CMETHOD_REF(br_object,				query),
	BR_CMETHOD_REF(br_object, 				queryBuffer),
	BR_CMETHOD_REF(br_object, 				queryMany),
	BR_CMETHOD_REF(br_object, 				queryManySize),
	BR_CMETHOD_REF(br_object, 				queryAll),
	BR_CMETHOD_REF(br_object,	 			queryAllSize),

	BR_CMETHOD_REF(br_buffer_stored_hardATI,	update),
};


