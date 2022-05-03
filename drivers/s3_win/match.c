/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: match.c 1.1 1997/12/10 16:51:03 jon Exp $
 * $Locker: $
 *
 * Functions that match a primitive against the renderers state
 */
#include <stdio.h>
#include <windows.h>
#include <ddraw.h>

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("Id: $");

/* Invalid value for unknown pixelmap types */
#define PMT_NONE 255

extern br_primitive_library PrimitiveLibraryHardS3;
extern S3DTK_LPFUNCTIONLIST   g_pS3DTK_Funct;

/* Descriptions of all the available renderers */
static struct local_block primInfo[] =
{
#include "matchinf.c"
};


/* static renderer state switches (only used internally) */
static   br_boolean  smooth_enable;
static   br_boolean  persp_enable;
static   br_boolean  light_enable;

/* Flag to tell the renderer whether it needs to calculate fog values */
br_boolean		S3fog_enable;

/* Current fog ranges must be visible to the renderer */
float S3fog_min;
float S3fog_max;
float S3fog_const;

/* Flag to tell the renderer whether it needs to parse u,v,w data */
br_boolean		S3tex_enable;

/* Alpha values must be visible to the renderer */
/* Brender doesn't appear to do per-vertex alpha yet. */
br_uint_8    	S3alpha_value;

/* Flag to tell the renderer whether it needs to parse lighting values */
br_boolean		S3rgb_enable;

/* Macro to check that a SetState is successful */
#define SafeSetState(a,b)  (S3SetState(a,b) == BRE_OK)

/* Macro to do only necessary Set States, since these may have high overhead */
#define QuickSetState(a,b,c) \
   if(b != c)\
      {\
      SafeSetState(a,b);\
      c = b;\
      }


/* Macro for converting from a 24 bit colour value to 15 bit colour */
#define CONV24TO15(a)   ((a & 0xFF)>>3) | ((a & 0xF800) >> 6) | ((a & 0xF80000) >> 9) 




static br_boolean updateRanges(struct br_primitive_state *self)
{
	int i;
   br_boolean changed = BR_FALSE;
   float sx_scale, sx_offset, sy_scale, sy_offset, u_scale, v_scale;

   OUTDBS("UpdateRanges\n");

   /*
    * SX,SY range are based on current colour_buffer. Can't get full Y range
    * (bug in S3 libs?)
    */
   sx_offset = (float)((self->out.colour.width-1)/2.0f);
   sx_scale  = (float)((((float)self->out.colour.width))/2.0f);
   sy_offset = (float)((self->out.colour.height)/2.0f);
   sy_scale  = -(float)((((float)self->out.colour.height)-1.5f)/2.0f);

//   sx_offset = (float)((self->out.colour.width)/2.0f);
//   sx_scale  = (float)((self->out.colour.width-2)/2.0f) + 1.0f;
//   sy_offset = (float)((self->out.colour.height)/2.0f);
//   sy_scale  = -(float)((self->out.colour.height-2)/2.0f) + 1.0f;

   changed |= (self->cache.comp_scales[C_SX] != sx_scale || self->cache.comp_offsets[C_SX] != sx_offset ||
               self->cache.comp_scales[C_SY] != sy_scale || self->cache.comp_offsets[C_SY] != sy_offset);
                                                                           
   self->cache.comp_offsets[C_SX] = sx_offset;                                
   self->cache.comp_scales[C_SX] = sx_scale;                                  
   self->cache.comp_offsets[C_SY] =  sy_offset;
   self->cache.comp_scales[C_SY] = sy_scale;                                  


	/* U,V range are based on current colour_map, if required */

	if(self->prim.colour_map)
	   {
      u_scale = self->prim.colour_map->buffer.width_p - 1;
      v_scale = self->prim.colour_map->buffer.height  - 1;

      changed |= self->cache.comp_scales[C_U] != u_scale || self->cache.comp_scales[C_V] != v_scale;

      self->cache.comp_offsets[C_U] = 0.0f;
      self->cache.comp_scales[C_U]  = u_scale;
      self->cache.comp_offsets[C_V] = 0.0f;
      self->cache.comp_scales[C_V]  = v_scale;
      }



   /* If 8 bit output, get index ranges for lighting */
	if(self->out.colour.pixelmap)
		{
		/* I base and range from primitive */
		if(self->out.colour.pixelmap->pm_type == BR_PMT_INDEX_8)
			{
			self->cache.comp_scales[C_I]  = (float)(self->prim.index_range);
			self->cache.comp_offsets[C_I] = (float)(self->prim.index_base + 0.5f);
         changed |= BR_TRUE;
			}
		}

//    changed = BR_TRUE;
   OUTDBS("UpdateRanges success\n");

	return changed;
}




void S3InitConstantRanges(struct br_primitive_state *self)
{
   int i;

   OUTDBS("InitConstantRanges\n");

   /*
    * Everything defaults to offset = 0, scale = 1
    */
   for(i=0; i < NUM_COMPONENTS; i++)
      {
      self->cache.comp_offsets[i] = 0.0f;
      self->cache.comp_scales[i] = 1.0f;
      }


	/* Z range -32767 to 32767  (+32767 in rendfunc.c) */
	self->cache.comp_offsets[C_SZ] = 0.0f;
	self->cache.comp_scales[C_SZ]  = -32767.0f;

   /*
    * R,G,B,A are 0-255
    */
   self->cache.comp_scales[C_R] = 254.5f;
   self->cache.comp_scales[C_G] = 254.5f;
   self->cache.comp_scales[C_B] = 254.5f;
   self->cache.comp_scales[C_A] = 254.5f;

   OUTDBS("InitConstantRangesSuccess\n");

}






// Buffer info - depth and colour should be cleared to NULL at the end
// of each render
struct br_buffer_stored *current_tb = NULL;
br_device_pixelmap      *last_depth = NULL;
br_device_pixelmap      *last_colour= NULL;

static void updateBuffers(struct br_primitive_state *self)
{

	/* Assume texture is unchanged */
	br_boolean  force_reload = BR_FALSE;

   OUTDBS("UpdateBuffers\n");
   // Output buffers

	if(!self->out.depth.pixelmap)
		/* Call the Z buffer setup function with BR_FALSE to disable Z buffer */
		S3SetZBufferState(BR_FALSE,NULL,0,0,NULL);
   else if(last_depth != self->out.depth.pixelmap)
   {
      S3SetZBufferState(BR_TRUE,
                        self->out.depth.pixelmap->surface,
                        self->out.depth.pixelmap->pm_width,
                        self->out.depth.pixelmap->pm_height,
                        self->out.depth.pixelmap);
	  last_depth = self->out.depth.pixelmap;
	  }


   if(self->out.colour.pixelmap)
      if(last_colour != self->out.colour.pixelmap)
	  {
         S3SetRenderWindow(self->out.colour.pixelmap->surface,0,0,
                           self->out.colour.pixelmap->pm_width,
                           self->out.colour.pixelmap->pm_height,
                           self->out.colour.type,
                           self->out.colour.pixelmap);
		 last_colour = self->out.colour.pixelmap;
	  }


	/* Input buffer */
	/* If we have a texture map */
	if(self->prim.colour_map)
		{
		S3tex_enable = BR_TRUE;

		/* If we have changed the texture map from the one we were using
		 * previously, OR a MapUpdate operation has occurred then we must
		 * assume that the currently loaded texture is now invalid */

		if(!(current_tb) ||
			(current_tb != self->prim.colour_map) ||
			(current_tb->buffer_updated == BR_TRUE) ||
			(current_tb->buffer.in_video_memory == BR_FALSE))
			{
			/* swap in the new texture map & setup S3 surface
			 * Set renderer texture input to this new surface */

			/* If buffer is updated then force a reload */
            if(self->prim.colour_map->buffer_updated)
               force_reload = BR_TRUE;

//            if(current_tb)
//                if(current_tb->buffer_updated == BR_TRUE)
//                    force_reload = BR_TRUE;

			if(!S3CacheTexture(&(self->prim.colour_map->buffer),
                              force_reload))
				{
				/* If we can't load the texture for any reason, override the
				 * normal system state and turn off texture mapping */
				S3tex_enable = BR_FALSE;
                current_tb = NULL;
				}
			else
				{
				self->prim.colour_map->buffer_updated = BR_FALSE;
				current_tb = self->prim.colour_map;
				/* Select the texture map to be the current one in use */
				g_pS3DTK_Funct->S3DTK_SetState(g_pS3DTK_Funct,
                                           S3DTK_TEXTUREACTIVE,
                                           &(self->prim.colour_map->buffer.s3_surface));
				}
			}
		}
	else
       {
       S3tex_enable = BR_FALSE;
       current_tb = NULL;
       }


   OUTDBS("UpdateBuffers success\n");

}

/* Enumerations used to store the old renderer state */
/* Set initially to what are (hopefully) invalid enumerations */

static br_uint_32      old_shade_mode        = 0xFFFFFFFF;
static br_uint_32      old_filter_mode       = 0xFFFFFFFF;
static br_uint_32      old_texel_blend_mode  = 0xFFFFFFFF;
static br_uint_32      old_alpha_mode        = 0xFFFFFFFF;
static br_uint_32      old_render_mode       = 0xFFFFFFFF;
static br_boolean      old_depth_write      = BR_TRUE;
static br_boolean      old_fog_enable        = BR_FALSE;


void ResetS3RendererHeldStates(void)
{
   old_shade_mode        = 0xFFFFFFFF;
   old_filter_mode       = 0xFFFFFFFF;
   old_texel_blend_mode  = 0xFFFFFFFF;
   old_alpha_mode        = 0xFFFFFFFF;
   old_render_mode       = 0xFFFFFFFF;
   old_fog_enable        = BR_FALSE;
	old_depth_write      = BR_TRUE;
}






/* This is the function called to update the state of S3's renderer functions */

static void UpdateS3Renderer(struct br_primitive_state *self,
							        br_token prim_type)
{

   /* Enumerations used to set the current S3 render state */
   br_uint_32 filter_mode;
   br_uint_32 texel_blend_mode;
   br_uint_32 alpha_mode;
   br_uint_32 render_mode;
   br_boolean depth_write;

   /* Now try to set the state of the S3 libraries appropriately
    * to match the primitive selected */

   OUTDBS("UpdateS3Renderer\n");

   /* If we are not lighting textures then set light off */
   if(self->prim.flags & PRIMF_MODULATE)
      light_enable = BR_TRUE;
   else
      light_enable = BR_FALSE;

   if(self->prim.flags & PRIMF_SMOOTH)
      smooth_enable = BR_TRUE;
   else
      smooth_enable = BR_FALSE;

   if(self->prim.flags & PRIMF_PERSPECTIVE)
      persp_enable = BR_TRUE;
   else
      persp_enable = BR_FALSE;


	updateBuffers(self);


   /* Set up texel calculation method */
   switch(self->prim.mip_interpolation)
      {
      case BRT_NONE:
      case BRT_NEAREST:
         switch(self->prim.map_interpolation)
            {
            case BRT_NONE:
            case BRT_NEAREST:
               filter_mode = S3DTK_TEX1TPP;
               break;
            case BRT_LINEAR:
               filter_mode = S3DTK_TEX4TPP;
               break;            
            default:
               filter_mode = S3DTK_TEX1TPP;
            }
         break;
      case BRT_LINEAR:
         switch(self->prim.map_interpolation)
            {
            case BRT_NONE:
               filter_mode = S3DTK_TEXM2TPP;
               break;
            case BRT_NEAREST:
               filter_mode = S3DTK_TEXM4TPP;
               break;
            case BRT_LINEAR:
               filter_mode = S3DTK_TEXM8TPP;
               break;            
            default:
               filter_mode = S3DTK_TEXM2TPP;
            }
         break;
      default:
         switch(self->prim.map_interpolation)
            {
            case BRT_NONE:
            case BRT_NEAREST:
               filter_mode = S3DTK_TEX1TPP;
               break;
            case BRT_LINEAR:
               filter_mode = S3DTK_TEX4TPP;
               break;            
            default:
               filter_mode = S3DTK_TEX1TPP;
            }
      }



   /* Check to see if this is really a mipmappable texture
    * i.e. the mip_offset is not zero. If the user is trying
    * to mipmap a non mipmap texture then give him the nearest
    * available non mipmap mode instead.
    * Of course, this can be screwed up by someone hardwiring the
    * mip_offset to something, but I never said this driver would
    * be proofed against intentional sabotage... */
	if(self->prim.colour_map)
		{
		if(!(self->prim.colour_map->buffer.mip_offset))
			{
			if(self->prim.mip_interpolation != BRT_NONE)
				{
				switch(self->prim.map_interpolation)
					{
					case BRT_NONE:
					case BRT_NEAREST:
						filter_mode = S3DTK_TEX1TPP;
						break;
					case BRT_LINEAR:
						filter_mode = S3DTK_TEX4TPP;
						break;            
					default:
						filter_mode = S3DTK_TEX1TPP;
					}
				}
			}
		}

   if(self->out.colour.pixelmap)
   {
      /* Make sure we don't try to do anything illegal in 8 bit output modes */
		if(self->out.colour.pixelmap->pm_type == BR_PMT_INDEX_8)
			{
         if(S3tex_enable == BR_TRUE)
            {
			   light_enable = BR_FALSE;
			   smooth_enable = BR_FALSE;
            }
         
         /* Disable alpha in indexed modes */
         alpha_mode = S3DTK_ALPHAOFF;
         /* Disable fog in indexed modes */
         S3fog_enable = BR_FALSE;
	      SafeSetState(S3DTK_FOGCOLOR,S3DTK_FOGOFF);
			}
      else
         {
         /* Alpha mode is valid if not indexed */
         if(self->prim.flags & PRIMF_BLEND)
		      {
		      alpha_mode = S3DTK_ALPHASOURCE;
		      S3alpha_value = (br_uint_8)(self->prim.alpha_val * 255.0f);
		      }
	      else
		      alpha_mode = S3DTK_ALPHAOFF;

         /* Fog modes are only valid if not indexed */
	      if(self->prim.fog_type != BRT_NONE)
		      {
		      S3fog_enable = BR_TRUE;
		      /* Set the fogging mode */
            if(self->prim.fog_colour == S3DTK_FOGOFF)
               /* S3 cannot fog to S3DTK_FOGOFF */
               SafeSetState(S3DTK_FOGCOLOR,S3DTK_FOGOFF-1);
            else
		         SafeSetState(S3DTK_FOGCOLOR, self->prim.fog_colour);
		      
            /* It is illegal for alpha mode to be alphasource if fog is enabled */
		      if(alpha_mode == S3DTK_ALPHASOURCE)
			      alpha_mode = S3DTK_ALPHAOFF;
		      }
	      else
		      {
		      S3fog_enable = BR_FALSE;
		      SafeSetState(S3DTK_FOGCOLOR,S3DTK_FOGOFF);
		      }

         /* If the current texture contains valid alpha channel data then this should override
	       * the alpha from BRender (assume alpha textures are _always_ alpha'd) */
		  if(self->prim.colour_map)
			if(self->prim.colour_map->buffer.alpha_pixels == BR_TRUE)
		       alpha_mode = S3DTK_ALPHATEXTURE;
         }
		}

   /* By default the renderer must parse rgb/index values */
   S3rgb_enable = BR_TRUE;

   /* Work out the rendering mode */
   switch(light_enable)
      {
      case  BR_TRUE:
         switch(S3tex_enable)
            {
            case  BR_TRUE:
               switch(persp_enable)
                  {
                  case  BR_TRUE:
                     render_mode = S3DTK_LITTEXTUREPERSPECT;
                     break;
                  case  BR_FALSE:
                  default:
                     render_mode = S3DTK_LITTEXTURE;
                     break;
                  }
				break;
            case  BR_FALSE:/* Deliberate fall through */
            default:
               render_mode = S3DTK_GOURAUD;
            }
         break;

      case  BR_FALSE: /* Deliberate fall through */
      default:
         switch(S3tex_enable)
            {
            case  BR_TRUE:
               switch(persp_enable)
                  {
                  case  BR_TRUE:
                     render_mode = S3DTK_UNLITTEXTUREPERSPECT;
					      /* No point in parsing RGB in unlit modes */
					      S3rgb_enable = BR_FALSE;
                     break;

                  case  BR_FALSE: /* Deliberate fall through */
                  default:
                     render_mode = S3DTK_UNLITTEXTURE;
					      /* No point in parsing RGB in unlit modes */
					      S3rgb_enable = BR_FALSE;
					      break;
                  }
               break;
            case  BR_FALSE: /* Deliberate fall through */
            default:
               render_mode = S3DTK_GOURAUD;
            }
         break;
      }

   if(S3fog_enable)
      {
      S3fog_min = self->prim.fog_min;
      S3fog_max = self->prim.fog_max;
      /* Calculate reciprocal so that we have this for the renderer later on */
      if(S3fog_max == S3fog_min)
	      S3fog_max += 0.01f;
      /* S3MINFOGVALUE can be found in drv.h */
      S3fog_const = (1.0f/(S3fog_max-S3fog_min) * S3MINFOGVALUE);
      }

	/* Lighting mode is always modulate */	   
	texel_blend_mode = S3DTK_TEXMODULATE;

	/*
	 * Disable depth write on all blended things
	 */
//	depth_write = !(self->prim.flags & PRIMF_BLEND);

   QuickSetState(S3DTK_RENDERINGTYPE,     render_mode,      old_render_mode);
   QuickSetState(S3DTK_TEXBLENDINGMODE,   texel_blend_mode, old_texel_blend_mode);
   QuickSetState(S3DTK_TEXFILTERINGMODE,  filter_mode,      old_filter_mode);
   QuickSetState(S3DTK_ALPHABLENDING,     alpha_mode,       old_alpha_mode);
   QuickSetState(S3DTK_ZBUFFERUPDATEENABLE, depth_write,    old_depth_write);

		OUTDBS("UpdateS3Renderer Success\n");

}





br_error BR_CMETHOD_DECL(br_primitive_state_hardS3, renderBegin)(
		struct br_primitive_state *self,
		struct brp_block **rpb,
		br_boolean *block_changed,
		br_boolean *ranges_changed,
		br_boolean no_render,
		br_token prim_type)
{
	int i;
	struct local_block *pb;
   br_uint_32 flags;

	ASSERT(rpb);

   OUTDBS("RenderBegin\n");
   /* If previous match is still valid, return that */
   if(self->cache.last_type == prim_type)
   {
      if (self->cache.timestamp_prim == self->prim.timestamp_major &&
          self->cache.timestamp_out == self->out.timestamp_major)
         {
         pb = self->cache.last_block;
         *rpb = &pb->p;
         *block_changed = BR_FALSE;
         *ranges_changed = BR_FALSE;

         /* Update the S3 renderer state */
         UpdateS3Renderer(self,prim_type);

         // Faff around with the block for alpha primitives
         if (self->prim.flags & PRIMF_BLEND || 
            (self->prim.colour_map && self->prim.colour_map->buffer.alpha_pixels))
            pb->p.flags |= BR_PRIMF_BLENDED;
         else
            pb->p.flags &= ~BR_PRIMF_BLENDED;

		   OUTDBS("RenderBegin success - block not changed\n");
		   return BRE_OK;
		   }
		}

	// If we get to here then the block has probably been changed, so
   // assume the worst
   *block_changed = BR_TRUE;
   *ranges_changed = updateRanges(self);

	/* Clear the flags to safe defaults */
	S3tex_enable = BR_FALSE;
	smooth_enable = BR_FALSE;
	persp_enable = BR_FALSE;
	light_enable = BR_TRUE;

	/* Update internal state flags */
   flags = self->prim.flags & ~PRIMF_INTERNAL;

	/* Match against primitives array */
	pb = primInfo;
	for(i=0; i < BR_ASIZE(primInfo); i++,pb++)
		{
		/* Is this the required type? */
		if(pb->p.type != prim_type)
			continue;

		/* Do the flags match */
		if((flags & pb->flags_mask) != pb->flags_cmp)
			continue;

		/* Check buffer types */
		if((pb->colour_type) != PMT_NONE && (self->out.colour.type != pb->colour_type))
			continue;

       // Only check 16/24 bit output type for speed. Renderers can deal with z & texture
       break;
	}

   /* Choke if we did not find a matching primitive */
   if(i >= BR_ASIZE(primInfo))
      return BRE_FAIL;

   /* return pointer to primitive, and set 'unchanged' if this
    * pointer is the same as previous match */
	 
   *rpb = &pb->p;

   if(pb == self->cache.last_block)
      *block_changed = BR_FALSE;
   else
      self->cache.last_block = pb;


   /* Update the caching (confidence is NOT high) */
   self->cache.last_type = prim_type;
   self->cache.timestamp_prim = self->prim.timestamp_major;
   self->cache.timestamp_out = self->out.timestamp_major;

   /* Update the S3 renderer state */
   UpdateS3Renderer(self,prim_type);

   if (self->prim.flags & PRIMF_BLEND || 
       (self->prim.colour_map && self->prim.colour_map->buffer.alpha_pixels))
      pb->p.flags |= BR_PRIMF_BLENDED;
   else
      pb->p.flags &= ~BR_PRIMF_BLENDED;

   OUTDBS("RenderBegin success - block changed!\n");

   
   return BRE_OK;
}


/* Called at the end of each rendering block, this is used to synchronise the renderer
 * and get rid of our remaining work so that we're ready for a change in state */
br_error BR_CMETHOD_DECL(br_primitive_state_hardS3, renderEnd)(
		struct br_primitive_state *self,
		struct brp_block *rpb)
{
   OUTDBS("RenderEnd\n");
	/* At the end of a block we must flush any unrendered primitives to the renderer */
	FlushPrimitivesS3();
	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_primitive_state_hardS3, rangesQueryF)(
		struct br_primitive_state *self,
		br_float *offset,
		br_float *scale,
		br_int_32 max_comp)
{
	int i;

   // Fail if the current info is not valid
   if (self->cache.timestamp_prim != self->prim.timestamp_major ||
       self->cache.timestamp_out != self->out.timestamp_major)
      return BRE_DEV_FAIL;

	for(i=0; i < max_comp; i++)
      {
		offset[i] = self->cache.comp_offsets[i];
		scale[i]  = self->cache.comp_scales[i];
	   }

	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_primitive_state_hardS3, rangesQueryX)(
		struct br_primitive_state *self,
		br_fixed_ls *offset,
		br_fixed_ls *scale,
		br_int_32 max_comp)
{
	int i;

   // Fail if the current info is not valid
   if (self->cache.timestamp_prim != self->prim.timestamp_major ||
       self->cache.timestamp_out != self->out.timestamp_major)
      return BRE_DEV_FAIL;

	for(i=0; i < max_comp; i++)
      {
		offset[i] = BrFloatToFixed(self->cache.comp_offsets[i]);
		scale[i]  = BrFloatToFixed(self->cache.comp_scales[i]);
	   }

	return BRE_OK;
}






