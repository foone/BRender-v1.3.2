/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: match.c 1.1 1997/12/10 16:50:18 jon Exp $
 * $Locker: $
 *
 * Functions that match a primitive against the renderers state
 *
 * Modified for ATI Expression by A. Pomianowski
 */


#include <stdio.h>
#include <windows.h>
#include <ddraw.h>

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("Id: $");

/*
 * Invalid value for unknown pixelmap types
 */
#define PMT_NONE 255

extern br_primitive_library PrimitiveLibraryHardATI;


/*
 * Descriptions of all the available renderers
 */
static struct local_block primInfo[] =
{
#include "matchinf.c"
};


/* Macro to check that a SetState is succesful - only really for logging
 * purposes - there's not a lot that can be done if the set state fails
 * anyway... */

#define SafeSetState(a,b,c)  ((ATI3DCIF_ContextSetState(a,b,c) == C3D_EC_OK))


/* Macro to do only necessary Set States, since these have high overhead */
#define QuickSetState(a,b,c,d) \
   if(*c != d)\
      {\
      SafeSetState(a,b,c);\
      d = *c;\
      }\

/* static renderer state switches */
static   br_boolean  smooth_enable;
static   br_boolean  persp_enable;
static   br_boolean  light_enable;

/* Fog/texture  enable must be visible to the renderer */

br_boolean     ATIfog_enable;
br_boolean     ATItex_enable;
br_boolean     ATIalpha_enable;

/* Alpha values must be visible to the renderer */

float     		ATIalpha_value;

/* Fog ranges must also be visible to renderer */

float ATIfog_min;
float ATIfog_max;
float ATIfog_const;


/* Global flag for the current state of the ATI libraries -
 * States whether we are inside an ATIRenderBegin -Render End pair */

br_boolean  ATI_renderer_initialised = BR_FALSE;


/* Macro for converting from a 24 bit colour value to a 15 bit colour */

#define CONV24TO15(a)   ((a & 0xFF)>>3) | ((a & 0xF800) >> 6) | ((a & 0xF80000) >> 9) 



HRESULT err;

// Buffer info - depth, colour and texture should be cleared to NULL
// at the start of each frame
struct br_buffer_stored *current_ati_tb = NULL;
br_device_pixelmap      *last_ati_depth = NULL;
br_device_pixelmap      *last_ati_colour = NULL;

static void UpdateOutputBuffers(struct br_primitive_state *self)
{
	br_boolean  force_reload = BR_FALSE;	/* Assume texture is unchanged */

	/* Output buffers */
	if(!self->out.depth.pixelmap)
		ATISetZBufferState(BR_FALSE,NULL,0,0);
	else if(last_ati_depth != self->out.depth.pixelmap)
		{
		ATISetZBufferState( BR_TRUE,
							self->out.depth.pixelmap->surface,
							self->out.depth.pixelmap->pm_width,
							self->out.depth.pixelmap->pm_height);
		last_ati_depth = self->out.depth.pixelmap;
		}


	if(self->out.colour.pixelmap)
		if(last_ati_colour != self->out.colour.pixelmap)
			{
			ATISetRenderWindow(	self->out.colour.pixelmap->surface,
								0,0,
								self->out.colour.pixelmap->pm_width-1,
								self->out.colour.pixelmap->pm_height-1,
								self->out.colour.pixelmap);
			last_ati_colour = self->out.colour.pixelmap;
			}
}






static void UpdateTexture(struct br_primitive_state *self)
{
	br_boolean  force_reload = BR_FALSE;	/* Assume texture is unchanged */

	/*
	 * Input buffers
	 */

	/* If we have a texture map */
	if(self->prim.colour_map)
		{
		ATItex_enable = BR_TRUE;

		/* If we have changed the texture map from the one we were using
		 * previously, OR a MapUpdate operation has occurred then we must
		 * assume that the currently loaded texture is now invalid */

		if(!(current_ati_tb) ||
			(current_ati_tb != self->prim.colour_map) ||
			(current_ati_tb->buffer_updated == BR_TRUE) ||
			!(self->prim.colour_map->buffer.texture_handle))
			{
			/* swap in the new texture map & setup ATI surface
			 * Set renderer texture input to this new surface */

			/* If buffer is updated then force a reload */
			if(self->prim.colour_map->buffer_updated == BR_TRUE)
				force_reload = BR_TRUE;

			if(!ATICacheTexture(&(self->prim.colour_map->buffer),
								force_reload))
				{
				/* If we can't load the texture for any reason, override the
				 * normal system state and turn off texture mapping */
				ATItex_enable = BR_FALSE;
//				current_ati_tb = NULL;
				}
			else
				{
				/* Setup the texture handle for the renderer */
				ATI3DCIF_ContextSetState(hRenderContext,
										 C3D_ERS_TMAP_SELECT,
										 &(self->prim.colour_map->buffer.texture_handle));

				self->prim.colour_map->buffer_updated = BR_FALSE;
				current_ati_tb = self->prim.colour_map; 
				}
			}
		}
	else
		{
		ATItex_enable = BR_FALSE;
		current_ati_tb = NULL;
		}
}







/*
 * Update the ranges that BRender will use when passing parameters to
 * the rendering functions
 */

static br_boolean updateRanges(struct br_primitive_state *self)
{
	br_boolean changed = BR_FALSE;
	float sx_scale, sx_offset, sy_scale, sy_offset;


	sx_offset = (float)((self->out.colour.width)/2);
	sx_scale  = (float)((self->out.colour.width)/2);
	sy_offset = (float)((self->out.colour.height)/2);
	sy_scale  = -(float)((self->out.colour.height)/2);


	changed |= (self->cache.comp_scales[C_SX] != sx_scale ||
				self->cache.comp_offsets[C_SX] != sx_offset ||
				self->cache.comp_scales[C_SY] != sy_scale ||
				self->cache.comp_offsets[C_SY] != sy_offset);

	self->cache.comp_offsets[C_SX] = sx_offset;
	self->cache.comp_scales[C_SX]  = sx_scale;
	self->cache.comp_offsets[C_SY] = sy_offset;
	self->cache.comp_scales[C_SY]  = sy_scale;

	if(self->prim.colour_map)
		{
		changed |= ((self->cache.comp_scales[C_U] != self->prim.colour_map->buffer.u_range) ||
					(self->cache.comp_scales[C_V] != self->prim.colour_map->buffer.v_range));

		/* For RAGE, u,v,w ranges are 0 -> 1.0 */
		self->cache.comp_scales[C_U] = self->prim.colour_map->buffer.u_range;
		self->cache.comp_scales[C_V] = self->prim.colour_map->buffer.v_range;
		}

	return changed;
}


// Initialise all ranges that will be constant for all blocks

void    ATIInitConstantRanges(struct br_primitive_state *self)
{
   int i;

   // Default to offset = 0, scale = 1
   for(i=0; i<NUM_COMPONENTS; i++)
      {
      self->cache.comp_offsets[i] = 0.0f;
      self->cache.comp_scales[i] = 1.0f;
      }

	/*
	 * SZ required range is 0 -> 65535 therefore offset is set at 32767
	 * with a scale of -32767, giving the full range
	 */
    self->cache.comp_offsets[C_SZ] = 0.0f;
    self->cache.comp_scales[C_SZ]  = -32767.0f;

	/* For RAGE, RGBA range 0->255 */
	self->cache.comp_scales[C_R] = 255.0f;
	self->cache.comp_scales[C_G] = 255.0f;
	self->cache.comp_scales[C_B] = 255.0f;
	self->cache.comp_scales[C_A] = 255.0f;
}





/* Enumerations used to store the old renderer state */
/* Set initially to what are (hopefully) invalid enumerations */

static C3D_ESHADE      old_shade_mode        = 255;
static C3D_ETEXFILTER  old_filter_mode       = 255;
static C3D_ETEXOP      old_texel_op          = 255;
static C3D_ETLIGHT     old_texel_light_mode  = 255;
static C3D_ETPERSPCOR  old_perspective_mode  = 255;
static C3D_EVERTEX     old_vertex_format     = 255;
static C3D_EASRC       old_src_alpha_mode    = 255;
static C3D_EADST       old_dst_alpha_mode    = 255;
static C3D_EPRIM       old_primitive_type    = 255;
static C3D_EZMODE      old_depth_mode        = 255;
static br_boolean      old_tex_enable        = BR_FALSE;
static br_boolean      old_fog_enable        = BR_FALSE;
static br_colour       old_fog_colour        = 0;


void ResetATIRendererHeldStates(void)
{
	old_shade_mode        = 255;
	old_filter_mode       = 255;
	old_texel_op          = 255;
	old_texel_light_mode  = 255;
	old_perspective_mode  = 255;
	old_vertex_format     = 255;
	old_src_alpha_mode    = 255;
	old_dst_alpha_mode    = 255;
	old_primitive_type    = 255;
	old_depth_mode        = 255;
	old_tex_enable        = BR_FALSE;
	old_fog_enable        = BR_FALSE;
	old_fog_colour        = 0;
}



/* This is the function called to update the state of ATI's
 * renderer functions. It will be called each time a new
 * primitive state comes through - essentially once for each
 * change in material within a given scene render
 */

static void UpdateATIRenderer(  struct br_primitive_state *self,
								br_token prim_type)
{
	/* Enumerations used to set the current ATI3DCIF render state */
	C3D_ESHADE      shade_mode;
	C3D_ETEXFILTER  filter_mode;
	C3D_ETEXOP      texel_op;
	C3D_ETLIGHT     texel_light_mode;
	C3D_ETPERSPCOR  perspective_mode;
	C3D_EVERTEX     vertex_format;
	C3D_EASRC       src_alpha_mode;
	C3D_EADST       dst_alpha_mode;
	C3D_EPRIM       primitive_type;
	C3D_EZMODE      depth_mode;
	C3D_COLOR       fog_colour;

	/* Now try to set the state of the ATI libraries appropriately
	 * to match the primitive selected */

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



	UpdateTexture(self);


	/* Set up texel calculation method */
    switch(self->prim.mip_interpolation)
       {
       case BRT_NONE:
          switch(self->prim.map_interpolation)
             {
             case BRT_NONE:
             case BRT_NEAREST:
                filter_mode = C3D_ETFILT_MINPNT_MAGPNT;
                break;
             case BRT_LINEAR:
                filter_mode = C3D_ETFILT_MIN2BY2_MAG2BY2;
                break;            
             default:
                filter_mode = C3D_ETFILT_MINPNT_MAGPNT;
                break;
             }
           break;
        case BRT_NEAREST:
           switch(self->prim.map_interpolation)
              {
              case BRT_NONE:
              case BRT_NEAREST:
                 filter_mode = C3D_ETFILT_MIPLIN_MAGPNT;
                 break;
              case BRT_LINEAR:
                 filter_mode = C3D_ETFILT_MIPLIN_MAG2BY2;
                 break;            
              default:
                 filter_mode = C3D_ETFILT_MIPLIN_MAGPNT;
                 break;
              }
           break;
        case BRT_LINEAR:
           switch(self->prim.map_interpolation)
              {
              case BRT_NONE:
              case BRT_NEAREST:
                 /* Not really supported, map to nearest available */
                 filter_mode = C3D_ETFILT_MIPLIN_MAG2BY2;
                 break;
              case BRT_LINEAR:
                 filter_mode = C3D_ETFILT_MIPTRI_MAG2BY2;
                 break;            
              default:
                 filter_mode = C3D_ETFILT_MIPLIN_MAG2BY2;
                 break;
              }
           break;
        default:
           switch(self->prim.map_interpolation)
              {
              case BRT_NONE:
              case BRT_NEAREST:
                 filter_mode = C3D_ETFILT_MINPNT_MAGPNT;
                 break;
              case BRT_LINEAR:
                 filter_mode = C3D_ETFILT_MIN2BY2_MAG2BY2;
                 break;            
              default:
                 filter_mode = C3D_ETFILT_MINPNT_MAGPNT;
                 break;
              }
         }


	/* Check to see if this is really a mipmappable texture
	 * i.e. the mip_offset is not zero. If the user is trying
	 * to mipmap a non mipmap texture then give him the nearest
	 * available non mipmap mode instead.
	 * Of course, this can be screwed up by someone hardwiring the
	 * mip_offset to something, but I never said this driver would
	 * be proofed against intentional sabotage...
	 */
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
                      filter_mode = C3D_ETFILT_MINPNT_MAGPNT;
                      break;
                   case BRT_LINEAR:
                      filter_mode = C3D_ETFILT_MIN2BY2_MAG2BY2;
                      break;            
                   default:
                      filter_mode = C3D_ETFILT_MINPNT_MAGPNT;
                   }
				}
			}
		}


	/* Work out the texel light mode */
	if(light_enable)
		texel_light_mode = C3D_ETL_MODULATE;
	else
		texel_light_mode = C3D_ETL_NONE;


	/* Work out the perspective correction mode */
	/* Three is supposed to give the best quality/speed tradeoff */
	if(persp_enable)
		perspective_mode = C3D_ETPC_THREE;
	else
		/* There was a bug in earlier versions of 3DCIF that meant that
		 * level 0 perspective correction did not work. This is now fixed */
		perspective_mode = C3D_ETPC_NONE;


	/* Work out the vertex format
	 * Set to the largest format for now - this is the only format
	 * supported by ATI3DCIF at the moment */
	vertex_format = C3D_EV_VTCF;
   

	/* Work out the primitive type */
    switch(prim_type)
       {
       case  BRT_TRIANGLE:
          primitive_type = C3D_EPRIM_TRI;
          break;
       case  BRT_LINE:
          primitive_type = C3D_EPRIM_LINE;
          break;
       case  BRT_POINT:
          primitive_type = C3D_EPRIM_LINE; /* We use a modified line renderer to render points */
          break;
       }

    if(self->prim.flags & PRIMF_BLEND)
		{
		ATIalpha_enable = BR_TRUE;
		ATIalpha_value = self->prim.alpha_val * 255.0f;
		src_alpha_mode = C3D_EASRC_SRCALPHA;
		dst_alpha_mode = C3D_EADST_INVSRCALPHA;
		}
	else
		{
		ATIalpha_enable = BR_FALSE;
		ATIalpha_value = 0.0f;
		src_alpha_mode = C3D_EASRC_ONE;
		dst_alpha_mode = C3D_EADST_ZERO;
		}

	/* Work out the correct texel operation mode from available info. */
    if(self->prim.colour_map)
		{
        if( (self->prim.colour_map->buffer.type == BR_PMT_RGBA_8888) ||
            (self->prim.colour_map->buffer.type == BR_PMT_RGBA_4444)) {
            /* The texture map has an alpha channel, so use it. */
			texel_op = C3D_ETEXOP_ALPHA;
			src_alpha_mode = C3D_EASRC_SRCALPHA;
			dst_alpha_mode = C3D_EADST_INVSRCALPHA;
        } else if(self->prim.colour_map->buffer.alpha_pixels == BR_TRUE)
			/* The texture map must be in 1555 format, use alpha as a mask
			 * (look in sbuffer.c for how this is set up) */
			texel_op = C3D_ETEXOP_ALPHA_MASK;
        else if(self->prim.colour_map->buffer.chroma_enable)
			/* The texture map has no valid alpha channel information
			 * but is chroma keyed */
			texel_op = C3D_ETEXOP_CHROMAKEY;
		else
			texel_op = C3D_ETEXOP_NONE;
		}
	else {
		texel_op = C3D_ETEXOP_NONE;
		texel_light_mode = C3D_ETL_NONE;
	}

    if(self->prim.fog_type != BRT_NONE)
		{
		/* Parse capabilities */
		if ((CIFInfo.u32CIFCaps1 & C3D_CAPS1_FOG) == C3D_CAPS1_FOG) 
			ATIfog_enable = BR_TRUE;
		else
			ATIfog_enable = BR_FALSE;
		}
	else
		ATIfog_enable = BR_FALSE;


	if(ATIfog_enable)
		{
		/*
		 * If fogging is enabled we MUST set the ATI into smooth shade
		 * mode, otherwise it does not interpolate fog
		 */
		smooth_enable = BR_TRUE;

        ATIfog_min = self->prim.fog_min;
        ATIfog_max = self->prim.fog_max;

		if(ATIfog_max == ATIfog_min)
			ATIfog_max += 0.01f;

		ATIfog_const = (1/(ATIfog_max-ATIfog_min) * ATIMINFOGVALUE);
		}


	/* Work out the shading mode */
	if(smooth_enable)
		shade_mode = C3D_ESH_SMOOTH;
	else
		shade_mode = C3D_ESH_FLAT;



	/* Set the fogging mode */
    if(self->prim.fog_colour != old_fog_colour)
		{
        old_fog_colour = self->prim.fog_colour;
        fog_colour.r = BR_RED(self->prim.fog_colour);
        fog_colour.g = BR_GRN(self->prim.fog_colour);
        fog_colour.b = BR_BLU(self->prim.fog_colour);
		fog_colour.a = 255;
		SafeSetState(hRenderContext,C3D_ERS_FG_CLR, &fog_colour);
		}

	/*
	 * Disable depth writes on blended primitives
	 */
	if (self->prim.flags & PRIMF_BLEND ||
		(self->prim.colour_map && self->prim.colour_map->buffer.alpha_pixels))

		depth_mode = C3D_EZMODE_TESTON;
	else
		depth_mode = C3D_EZMODE_TESTON_WRITEZ;

	if(ATItex_enable==BR_TRUE)
		QuickSetState(hRenderContext,C3D_ERS_TMAP_FILTER,&filter_mode,    old_filter_mode);

	QuickSetState(hRenderContext,C3D_ERS_TMAP_EN,       &ATItex_enable,  old_tex_enable);
	QuickSetState(hRenderContext,C3D_ERS_VERTEX_TYPE,   &vertex_format,  old_vertex_format);
	QuickSetState(hRenderContext,C3D_ERS_PRIM_TYPE,     &primitive_type, old_primitive_type);
	QuickSetState(hRenderContext,C3D_ERS_SHADE_MODE,    &shade_mode,     old_shade_mode);
	QuickSetState(hRenderContext,C3D_ERS_TMAP_LIGHT,    &texel_light_mode,old_texel_light_mode);
	QuickSetState(hRenderContext,C3D_ERS_TMAP_FILTER,   &filter_mode,    old_filter_mode);
	QuickSetState(hRenderContext,C3D_ERS_TMAP_PERSP_COR,&perspective_mode,old_perspective_mode);
	QuickSetState(hRenderContext,C3D_ERS_TMAP_TEXOP,    &texel_op,       old_texel_op);
	QuickSetState(hRenderContext,C3D_ERS_ALPHA_SRC,     &src_alpha_mode, old_src_alpha_mode);
	QuickSetState(hRenderContext,C3D_ERS_ALPHA_DST,     &dst_alpha_mode, old_dst_alpha_mode);
	QuickSetState(hRenderContext,C3D_ERS_FOG_EN,        &ATIfog_enable,  old_fog_enable);
	QuickSetState(hRenderContext,C3D_ERS_Z_MODE,        &depth_mode,     old_depth_mode);

}



/*
 * Here is the render begin method. The name is slightly confusing - it
 * doesn't actually equate to BRender's render begin call. It is actually
 * called once for each change in primitive state (so usually on
 * a per material basis.)
 */

br_error BR_CMETHOD_DECL(br_primitive_state_hardATI, renderBegin)(
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

	// Must update the buffers before locking the render buffer - otherwise
	// the locks needed to calculate the addresses of the buffers will fail.
    UpdateOutputBuffers(self);


#if 1
	/*
	 * Lock destination pixelmap for rendering a new model or scene if
	 * it is not already locked.
	 * N.B. Pixelmap is unlocked by br_primitive_library_soft::flush.
	 */
	if (!no_render && self->plib->colour_buffer != self->out.colour.pixelmap)
		{
		if(self->plib->colour_buffer != NULL)
			DevicePixelmapDirectUnlock( self->plib->colour_buffer);

		self->plib->colour_buffer = self->out.colour.pixelmap ;
		DevicePixelmapDirectLock( self->plib->colour_buffer, BR_TRUE);
		}
#endif


	/* Begin the ATI renderer if it hasn't already begun
	 * Calls to ATI3DCIF_ContextSetState outside a RenderBegin/RenderEnd
	 * pair are expensive, so this is done here to reduce this overhead
	 * As a result 2D operations are not allowed during 3D scene render
	 */
	if(ATI_renderer_initialised == BR_FALSE)
		{
		ATI3DCIF_RenderBegin(hRenderContext);
		ATI_renderer_initialised = BR_TRUE;
		}


	if(self->cache.last_type == prim_type)
		{
		if (self->cache.timestamp_prim == self->prim.timestamp_major &&
			self->cache.timestamp_out == self->out.timestamp_major)
			{
			pb = self->cache.last_block;
			*rpb = &pb->p;
			*block_changed = BR_FALSE;
			*ranges_changed = BR_FALSE;

			// Update the ATI renderer state
			UpdateATIRenderer(self,prim_type);

			// Faff around with the block for alpha primitives
			if(self->prim.flags & PRIMF_BLEND ||
				(self->prim.colour_map && self->prim.colour_map->buffer.alpha_pixels))
				pb->p.flags |= BR_PRIMF_BLENDED;
			else
				pb->p.flags &= ~BR_PRIMF_BLENDED;

			return BRE_OK;
			}
		}

	/*
	 * Otherwise, assume the worst - that changes will have happened
	 */
     *block_changed = BR_TRUE;
     *ranges_changed = updateRanges(self);

	/* Clear the flags to safe defaults */
	ATItex_enable = BR_FALSE;
	smooth_enable = BR_FALSE;
	persp_enable = BR_FALSE;
	light_enable = BR_TRUE;

	/* Set up internal matching flags */
    flags = self->prim.flags & (~PRIMF_INTERNAL);
	if (self->prim.colour_map)
		flags |= PRIMF_TEXTURE_BUFFER;
	else
		flags &= ~PRIMF_TEXTURE_BUFFER;

	/* Match against primitives array - simplified by texture cache rejection of
	 * unrenderable texture maps etc */
	pb = primInfo;
	for(i=0; i<BR_ASIZE(primInfo); i++,pb++)
		{
		/*
		 * Is this the required type?
		 */
		if(pb->p.type != prim_type)
			continue;

		/* Do the flags match */
		if((flags & pb->flags_mask) != pb->flags_cmp)
			continue;

		/* Got a renderer for this block */
		break;
		}


	/* Choke if we did not find a matching primitive */
	if(i >= BR_ASIZE(primInfo))
		return BRE_FAIL;

	/*
	 * return pointer to primitive, and set 'unchanged' if this
	 * pointer is the same as previous match
	 */
	*rpb = &pb->p;
    if(pb == self->cache.last_block)
        *block_changed = BR_FALSE;
	else
        self->cache.last_block = pb;

    // Update the caching
    self->cache.last_type = prim_type;
    self->cache.timestamp_prim = self->prim.timestamp_major;
    self->cache.timestamp_out = self->out.timestamp_major;

	/* Update the ATI render library state variables */
	UpdateATIRenderer(self,prim_type);

    // Faff around with the block for alpha primitives
    if(self->prim.flags & PRIMF_BLEND ||
      (self->prim.colour_map && self->prim.colour_map->buffer.alpha_pixels))
       pb->p.flags |= BR_PRIMF_BLENDED;
    else
       pb->p.flags &= ~BR_PRIMF_BLENDED;

	return BRE_OK;
}




/* This function is called at the end of rendering each block, so it may
 * well be called many times during the course of each scene render */

br_error BR_CMETHOD_DECL(br_primitive_state_hardATI, renderEnd)(
		struct br_primitive_state *self,
		struct brp_block *rpb)
{

	/* Flush any remaining primitives from the last block
	 * that have not yet been passed to the ATI renderers (rendfunc.c)
	 * so that they will be rendered with the correct mode set up */
	FlushPrimitivesATI();

#if 0
	/* End this render pass */
	if(ATI_renderer_initialised == BR_TRUE)
		{
		ATI_renderer_initialised = BR_FALSE;
		ATI3DCIF_RenderEnd();
		}
#endif

	return BRE_OK;
}





br_error BR_CMETHOD_DECL(br_primitive_state_hardATI, rangesQueryF)(
		struct br_primitive_state *self,
		br_float *offset,
		br_float *scale,
		br_int_32 max_comp)
{
	int i;

    // Fail if the current info is not valid
    if(self->cache.timestamp_prim != self->prim.timestamp_major ||
       self->cache.timestamp_out != self->out.timestamp_major)
       return BRE_DEV_FAIL;


    for(i=0; i < max_comp; i++)
       {
       offset[i] = self->cache.comp_offsets[i];
       scale[i]  = self->cache.comp_scales[i];
       }

	return BRE_OK;
}






br_error BR_CMETHOD_DECL(br_primitive_state_hardATI, rangesQueryX)(
		struct br_primitive_state *self,
		br_fixed_ls *offset,
		br_fixed_ls *scale,
		br_int_32 max_comp)
{
	int i;

    // Fail if the current info is not valid
    if(self->cache.timestamp_prim != self->prim.timestamp_major ||
       self->cache.timestamp_out != self->out.timestamp_major)
       return BRE_DEV_FAIL;

    for(i=0; i < max_comp; i++)
       {
       offset[i] = BrFloatToFixed(self->cache.comp_offsets[i]);
       scale[i]  = BrFloatToFixed(self->cache.comp_scales[i]);
       }

	return BRE_OK;
}






