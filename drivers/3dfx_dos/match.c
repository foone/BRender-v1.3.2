/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * Functions that match a primitive against the renderers state
 */
#include <math.h>

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

#include "texture.h"

/*
 * Invalid value for unknown pixelmap types
 */
#define PMT_NONE 255

extern br_primitive_library PrimitiveLibrary3Dfx;

/*
 * Fogging control table and cache
 */
static GrFog_t Local_Fog_Table[64];
static br_token Fog_Type_Old = BRT_NONE;
static br_uint_32 Fog_Colour_Old = 0;
static float Fog_Min_Old = 1.0f;
static float Fog_Max_Old = 0.0f;

float alpha_value;
float fog_constant;

int fog_enabled = -1;

/*
 * Descriptions of all the available renderers
 */
static struct local_block primInfo_t15[] = {
#include "prim_t15.c"
};

static struct local_block primInfo_l15[] = {
#include "prim_l15.c"
};

static struct local_block primInfo_p15[] = {
#include "prim_p15.c"
};

static struct {
	struct local_block *blocks;
	int nblocks;
} primInfoTable[3] = {
	{ primInfo_p15, BR_ASIZE(primInfo_p15), },
	{ primInfo_l15, BR_ASIZE(primInfo_l15), },
	{ primInfo_t15, BR_ASIZE(primInfo_t15), },
};

/*
 * Quickly modified to return a boolean indicating if any changes were detected.  Also changed to
 * not touch anything that will never change (this is done in initConstantRanges now)
 */
static br_boolean updateRanges(struct br_primitive_state *self)
{
	int i;
	br_boolean changed = BR_FALSE;
	br_scalar sx_scale, sx_offset, sy_scale, sy_offset, u_scale, v_scale;

	/* SX,SY range are based on current colour_buffer
	 *
	 * Multiplied by 16 so the vertex snapping can be accelerated */

	sx_scale = BrIntToScalar((self->out.colour.pixelmap->pm_width  / 2) * 16);
	sx_offset = BrIntToScalar(self->out.colour.pixelmap->pm_base_x * 16) + sx_scale;
	sy_scale = -BrIntToScalar((self->out.colour.pixelmap->pm_height  / 2) * 16);
	sy_offset = BrIntToScalar(self->out.colour.pixelmap->pm_base_y * 16) - sy_scale;

	changed |= (self->cache.comp_scales[C_SX] != sx_scale || self->cache.comp_offsets[C_SX] != sx_offset ||
		self->cache.comp_scales[C_SY] != sy_scale || self->cache.comp_offsets[C_SY] != sy_offset);

	self->cache.comp_offsets[C_SX] = sx_offset;
	self->cache.comp_scales[C_SX] = sx_scale;

	self->cache.comp_offsets[C_SY] =  sy_offset;
	self->cache.comp_scales[C_SY] = sy_scale;

	/* U,V range are set in SetupBuffer */
	if(self->prim.colour_map) {

		u_scale = self->prim.colour_map->buffer.u_range;
		v_scale = self->prim.colour_map->buffer.v_range;

		changed |= (self->cache.comp_scales[C_U] != u_scale || self->cache.comp_scales[C_V] != v_scale);

		self->cache.comp_scales[C_U] = u_scale;
		self->cache.comp_scales[C_V] = v_scale;
	}

	return changed;
}

void initConstantRanges(struct br_primitive_state *self)
{
	int i;

	/*
	 * Everything defaults to offset = 0, scale = 1
	 */
	for(i=0; i < NUM_COMPONENTS; i++) {
		self->cache.comp_offsets[i] = BR_SCALAR(0.0);
		self->cache.comp_scales[i] = BR_SCALAR(1.0);
	}

	/* SZ: BRender 1.3.0 now supports scaling on the Z buffer; in a slightly strange way,
	 * where the range specifies the range either side of 0.
	 * the 3Dfx seems to like 0-65535.0, although I don't think this is quite what the
	 * documentation says, it seems to work. */
	self->cache.comp_offsets[C_SZ] = BR_SCALAR(32767);
	self->cache.comp_scales[C_SZ] = -BR_SCALAR(32767);

	/*
	 * R,G,B,A are 0-255
	 */
	self->cache.comp_scales[C_R] = BR_SCALAR(255.0);
	self->cache.comp_scales[C_G] = BR_SCALAR(255.0);
	self->cache.comp_scales[C_B] = BR_SCALAR(255.0);
	self->cache.comp_scales[C_A] = BR_SCALAR(255.0);
}




/*
 * Texture cache interface
 */


br_boolean texture_cache_created_3dfx = BR_FALSE;
static void *current_palette = NULL;


br_boolean FXTextureFreedCallback(struct render_buffer *texture)
{
	texture->on_card = BR_FALSE;

	/* Doesn't have to do anything to unregister with GLIDE */
	return(BR_TRUE);
}




/*
 * Cache a texture in the card
 *
 * In;  the render buffer with all details of the texture
 */
br_error FXCacheTexture(struct render_buffer *src)
{
	br_int_32      card_mem_pos;      // Position of texture in card memory (relative to start of texture cache)
	br_uint_32     texture_size;      // Size inc all mipmaps
	GrMipMapId_t   id=0;


	/* Initialise texture cache if required */
	if (!texture_cache_created_3dfx) {
		br_uint_32 base, size;

		/* Initialise texture cache */
		base = grTexMinAddress( GR_TMU0 );
		size = grTexMaxAddress( GR_TMU0 ) - base;

		if (!TextureCacheInit(FXTextureFreedCallback))
			return(BRE_DEV_NO_MEMORY);

		/*
		 * Add separate blocks for each 2MB chunk, as textures cannot cross
		 * 2MB boundaries
		 */
		while (size > 1024*2048) {

			if (!TextureCacheAddBlock(base, 1024*2048))
				return(BRE_DEV_NO_MEMORY);

			size -= 1024*2048;
			base += 1024*2048;
		}

		if (size > 0)
			if (!TextureCacheAddBlock(base, size))
				return(BRE_DEV_NO_MEMORY);

		texture_cache_created_3dfx = BR_TRUE;
	}

	/* Send In The Palettes if required */
	if (src->palette_pointer && (current_palette != src->palette_pointer)) {
		grTexDownloadTable(GR_TMU0, GR_TEXTABLE_PALETTE, src->palette_pointer);
		current_palette = src->palette_pointer;
	}

	/* If we need to force a reload, order the texture cache to clear any
	 * entries corresponding to this texture, and make another attempt
	 * to cache it (in case the failure condition has now been corrected) */
	if (src->force_reload) {
		TextureCacheClearEntry(src);
		src->force_reload = BR_FALSE;
	}


	/* Already uploaded? */
	if (src->on_card)
		return(BRE_OK);


	/* If it's been marked as uncacheable, don't suffer the overhead of
	 * attempting to set it up again: failure conditions exist that
	 * will mean it's never loadable. */
	if (src->uncacheable)
		return(BRE_DEV_FAIL);


	/* Ask the texture cache to sort itself out for this texture */
	texture_size = grTexTextureMemRequired(GR_MIPMAPLEVELMASK_BOTH, &src->info);
	card_mem_pos = TextureCacheRequest(src, texture_size);
	if (card_mem_pos < 0) {
		src->uncacheable = BR_TRUE; // Any failure here also means never renderable
		return(BRE_DEV_NO_VIDEO_MEMORY);
	}
	src->startAddress = card_mem_pos;

	/* Send it to the card */
	grTexDownloadMipMap(src->tmu_id, card_mem_pos,
						GR_MIPMAPLEVELMASK_BOTH, &src->info);

	src->on_card = BR_TRUE;

	return(BRE_OK);
}




/*
 * Sort out everything to do with fog
 *
 * Completely rewritten by Dave
 */
void RecalculateFog(struct br_primitive_state *self)
{
	br_uint_32 fog_start_index, fog_end_index, index;

	if (self->prim.fog_type == BRT_NONE) {
		if (fog_enabled != 0) {
			grFogMode(GR_FOG_DISABLE);
			fog_enabled = 0;
		}

		return;
	}

	/* Fogging is enabled at this point */
	if (fog_enabled != 1) {
		grFogMode(GR_FOG_WITH_TABLE);
		fog_enabled = 1;
	}

	/* New fog colour? */
	if (self->prim.fog_colour != Fog_Colour_Old) {
		grFogColorValue(self->prim.fog_colour);
		Fog_Colour_Old = self->prim.fog_colour;
	}

	/* Check if the old uploadded fog table is still valid */
	if ((self->prim.fog_min == Fog_Min_Old) && (self->prim.fog_max == Fog_Max_Old))
		return;

	/* Recalculate fog table, store old values */
	Fog_Min_Old = self->prim.fog_min;
	Fog_Max_Old = self->prim.fog_max;

	/* Glide formula is W = 2 to the i/4, which gives i = 4 * log2(W).
	 * log is probably expensive, but scenes are unlikely to use lots of different
	 * fogging values. The fog_constant (4 / log(2)) is calculated in pstate.c to
	 * save time. */
	fog_start_index = (br_uint_32) (log(self->prim.fog_min)*fog_constant);
	fog_end_index   = (br_uint_32) (log(self->prim.fog_max)*fog_constant);

	/* Build fog table linearly between these two indices - this probably gives rise
	 * to a non-linear fogging table, but it's sufficiently close that I doubt there
	 * will be many complaints. */
	for(index=0;index<63;index++) {
		if (index < fog_start_index)
			Local_Fog_Table[index] = 0x00;	
		else if (index <= fog_end_index )
			Local_Fog_Table[index] = (GrFog_t)  (0xff * (index-fog_start_index) /
														(fog_end_index - fog_start_index));
		else
			Local_Fog_Table[index] = 0xff;	
	}

	grFogTable(Local_Fog_Table);
}




/* 3Dfx local state cache */
static int	blending_on = -1;
static int	depth_on = -1;
static int	depth_write_on = -1;
static int	texture_on = -1;
static int	filter_on = -1;
static int	colour_mode = -1;
static int	mipmap_mode = -1;
int zwrite = 1;


/*
 * Need to clear out the state when the driver is restarted
 * Pointed out by Matt G
 */
void Clear3DfxStateCache(void)
{
	blending_on = -1;
	depth_on = -1;
	depth_write_on = -1;
	texture_on = -1;
	filter_on = -1;
	colour_mode = -1;
	mipmap_mode = -1;
	zwrite = 1;
}

/*
 * Set the 3Dfx state that has to be reset on every pass
 */
void Set3DfxState(struct br_primitive_state *self, br_uint_32 flags)
{
	/* Check and reset fogging parameters */
	RecalculateFog(self);

	/* Setup glide texture */
	if ((flags & PRIMF_TEXTURE_BUFFER) &&
		(FXCacheTexture(&self->prim.colour_map->buffer) == BRE_OK)) {

		if (texture_on != 1) {
			/* Enable the static states required for texturing */
			grChromakeyMode(GR_CHROMAKEY_ENABLE);

			texture_on = 1;
		}

		grTexSource(self->prim.colour_map->buffer.tmu_id,
					self->prim.colour_map->buffer.startAddress,
					GR_MIPMAPLEVELMASK_BOTH,
					&self->prim.colour_map->buffer.info );

		/* Set up filtering and mipmapping
		 * BRender definition says map_antialiasing enables mipmapping, mip_interpolation
		 * controls interpolation between different levels of mipmapping */
		if ((self->prim.map_interpolation != BRT_NONE) &&
			(self->prim.map_interpolation != BRT_NEAREST)) {
			if (filter_on != 1) {
				grTexFilterMode(self->prim.colour_map->buffer.tmu_id, GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR);
				filter_on = 1;
			}
		} else {
			if (filter_on != 0) {
				grTexFilterMode(self->prim.colour_map->buffer.tmu_id, GR_TEXTUREFILTER_POINT_SAMPLED, GR_TEXTUREFILTER_POINT_SAMPLED);
				filter_on = 0;
			}
		}

		if (self->prim.map_antialiasing != BRT_NONE) {
			if (self->prim.mip_interpolation != BRT_NONE) {
				if (mipmap_mode != 2) {
					grTexMipMapMode(self->prim.colour_map->buffer.tmu_id, GR_MIPMAP_NEAREST_DITHER, FXFALSE);
					grHints(GR_HINT_ALLOW_MIPMAP_DITHER, 1);
					mipmap_mode = 2;
				}
			} else {
				if (mipmap_mode != 1) {
					grTexMipMapMode(self->prim.colour_map->buffer.tmu_id, GR_MIPMAP_NEAREST, FXFALSE);
					mipmap_mode = 1;
				}
			}
		} else {
			if (mipmap_mode != 0) {
				grTexMipMapMode(self->prim.colour_map->buffer.tmu_id, GR_MIPMAP_DISABLE, FXFALSE);
				mipmap_mode = 0;
			}
		}

		/* Setup colour combine unit for appropriate shade mode */
		if (flags & PRIMF_MODULATE) {
			if (flags & PRIMF_SMOOTH) {
				if (colour_mode != 0) {
					guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ITRGB);
					colour_mode = 0;
				}
			} else {
				if (colour_mode != 1) {
					guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ITRGB_DELTA0);
					colour_mode = 1;
				}
			}
		} else {
			if (colour_mode != 2) {
				guColorCombineFunction(GR_COLORCOMBINE_DECAL_TEXTURE);
				colour_mode = 2;
			}
		}
	} else {
		/* Not textured or texture load failed */
		if (texture_on != 0) {
			guTexSource(GR_NULL_MIPMAP_HANDLE);
			grChromakeyMode(GR_CHROMAKEY_DISABLE);

			texture_on = 0;
		}

		/* Setup colour combine unit for appropriate shade mode */
		if (flags & PRIMF_SMOOTH) {
			if (colour_mode != 3) {
				guColorCombineFunction(GR_COLORCOMBINE_ITRGB);
				colour_mode = 3;
			}
		} else {
			if (colour_mode != 4) {
				guColorCombineFunction(GR_COLORCOMBINE_ITRGB_DELTA0);
				colour_mode = 4;
			}
		}
	}

	/* Setup Glide depth buffer */
	if (flags & PRIMF_DEPTH_BUFFER) {
		if (depth_on != 1) {
			grDepthBufferMode(GR_DEPTHBUFFER_ZBUFFER);
			depth_on = 1;
		}
	} else if (depth_on != 0) {
		grDepthBufferMode(GR_DEPTHBUFFER_DISABLE);

		depth_on = 0;
	}

	if (flags & PRIMF_DEPTH_WRITE) {
		if (depth_write_on != 1) {
			grDepthMask(FXTRUE);
			depth_write_on = 1;
		}
	} else {
		if (depth_write_on != 0) {
			grDepthMask(FXFALSE);
			depth_write_on = 0;
		}
	}

	/* Setup Glide alpha blending */
	if (self->prim.flags & PRIMF_BLEND) {
		if (blending_on != 1) {
			grAlphaBlendFunction(GR_BLEND_SRC_ALPHA, GR_BLEND_ONE_MINUS_SRC_ALPHA,
								 GR_BLEND_ONE, GR_BLEND_ZERO);
			guAlphaSource(GR_ALPHASOURCE_CC_ALPHA);
			alpha_value = self->prim.alpha_val*255.0f;
			grConstantColorValue(((br_uint_32)alpha_value) << 24);

			blending_on = 1;
		}
	} else if (self->prim.colour_map != NULL && self->prim.colour_map->buffer.blended) {
		if (blending_on != 2) {
			grAlphaBlendFunction(GR_BLEND_SRC_ALPHA, GR_BLEND_ONE_MINUS_SRC_ALPHA,
								 GR_BLEND_ONE, GR_BLEND_ZERO);
			guAlphaSource(GR_ALPHASOURCE_TEXTURE_ALPHA);

			blending_on = 2;
		}
	} else {
		if (blending_on != 0) {
			grAlphaBlendFunction(GR_BLEND_ONE, GR_BLEND_ZERO,
								 GR_BLEND_ONE, GR_BLEND_ZERO);
			blending_on = 0;
		}
	}

	/*
	 * Disable depth writes for blended primitives.
	 *
	 * Not used at the moment - should be handled directly, as should depth
	 * comparision.
	 */
#if 0
	if (blending_on) {
		if (zwrite) {
			grDepthMask(FXFALSE);
			zwrite = 0;
		}
	} else {
		if (!zwrite) {
			grDepthMask(FXTRUE);
			zwrite = 1;
		}
	}
#endif
}






/*
 * Start of a new block: parse renderer state
 */
br_error BR_CMETHOD_DECL(br_primitive_state_3dfx, renderBegin)(
		struct br_primitive_state *self,
		struct brp_block **rpb,
		br_boolean *block_changed,
		br_boolean *ranges_changed,
		br_boolean no_render,
		br_token prim_type)
{
	int j, b;
	struct local_block *pb;
	br_uint_32 flags;


	ASSERT(rpb);


	/* Derive the mode flags; we could probably store this away somewhere
	 * Doing this eliminates a lot of extra prim_ states, and make the
	 * Set3DfxState function a little less confusing */
	flags = self->prim.flags;
    if (self->prim.colour_map)
        flags |= PRIMF_TEXTURE_BUFFER;
    if (self->out.depth.pixelmap)
        flags |= PRIMF_DEPTH_BUFFER;


	/*
	 * If previous match is still valid, return that
	 */
	if (self->cache.last_type == prim_type) {

        if (self->cache.timestamp_prim == self->prim.timestamp_major &&
			self->cache.timestamp_out == self->out.timestamp_major) {

			pb = self->cache.last_block;
			*rpb = &pb->p;
 
			*block_changed = BR_FALSE;
			*ranges_changed = BR_FALSE;

			/*
			 * Setup glide state
			 */
            if (no_render)
				return BRE_OK;


			/*
			 * Faff around with the block a bit
			 */
			if (self->prim.flags & PRIMF_BLEND || self->prim.colour_map != NULL && self->prim.colour_map->buffer.blended)
				pb->p.flags |= BR_PRIMF_BLENDED;
			else
				pb->p.flags &= ~BR_PRIMF_BLENDED;
	
#if 1
			/*
			 * KLUDGE - Disable depth write on all blended primitives
			 */
			if (pb->p.flags & BR_PRIMF_BLENDED)
				flags &= ~PRIMF_DEPTH_WRITE;
#endif

			Set3DfxState(self, flags);

            /* Call the begin function (if present) */
            if (pb->begin_fn)
				pb->begin_fn(&pb->p);

			return BRE_OK;
		}
	}


	/* The current block has probably changed if we make it here.
	 * Assume the worst, that changes will have happened */
	*block_changed = BR_TRUE;

	/*
	 * Try to be clever and work out if the ranges have changed
	 */
	*ranges_changed = updateRanges(self);

	/* Can render only to back buffers */
	if (self->out.colour.pixelmap->buffer_type != BT_BACKSCREEN) {
		self->cache.last_block = NULL;
        return BRE_DEV_UNSUPPORTED;
	}

	switch(prim_type) {
	case BRT_POINT:			j = 0;	break;
	case BRT_LINE:			j = 1;	break;
	case BRT_TRIANGLE:		j = 2;	break;
	default:
		self->cache.last_block = NULL;
        return BRE_DEV_UNSUPPORTED;
	}

	/*
	 * Match against selected primitives 
	 */
	pb = primInfoTable[j].blocks;

	for(b=0; b < primInfoTable[j].nblocks; b++,pb++) {
		/* Do the flags match */
        if ((flags & pb->flags_mask) == pb->flags_cmp)
			break;
	}

	/* Default to last primitive in block */
	if(b >= primInfoTable[j].nblocks)
		pb--;

	/* return pointer to primitive, and set 'unchanged' if this
	 * pointer is the same as previous match */
	*rpb = &pb->p;

    if (pb == self->cache.last_block)
		*block_changed = BR_FALSE;
	else
		self->cache.last_block = pb;

	/* Update the caching (confidence is NOT high) */
	self->cache.last_type = prim_type;
	self->cache.timestamp_prim = self->prim.timestamp_major;
	self->cache.timestamp_out = self->out.timestamp_major;

	/* Stash copy of no_render flag in block */
	pb->no_render = no_render;

	/* Setup glide state */
    if (no_render)
		return BRE_OK;

	/*
	 * Faff around with the block a bit
	 */
	if (self->prim.flags & PRIMF_BLEND || self->prim.colour_map != NULL && self->prim.colour_map->buffer.blended)
		pb->p.flags |= BR_PRIMF_BLENDED;
	else
		pb->p.flags &= ~BR_PRIMF_BLENDED;

#if 1
	/*
	 * KLUDGE - Disable depth write on all blended primitives
	 */
	if (pb->p.flags & BR_PRIMF_BLENDED)
		flags &= ~PRIMF_DEPTH_WRITE;
#endif

	Set3DfxState(self, flags);

	if (pb->begin_fn)
		pb->begin_fn(&pb->p);

	return BRE_OK;
}



br_error BR_CMETHOD_DECL(br_primitive_state_3dfx, renderEnd)(
		struct br_primitive_state *self,
		struct brp_block *pb)
{
	struct local_block *lb = (struct local_block *)pb;

    if (lb->no_render)
		return BRE_OK;
		
    if (lb->end_fn)
		lb->end_fn(pb);

	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_primitive_state_3dfx, rangesQueryF)(
		struct br_primitive_state *self,
		br_float *offset,
		br_float *scale,
		br_int_32 max_comp)
{
	int i;

	/*
	 * Fail if the current info is not valid
	 */
    if (self->cache.timestamp_prim != self->prim.timestamp_major ||
        self->cache.timestamp_out != self->out.timestamp_major)
        return BRE_DEV_FAIL;

	for(i=0; i < max_comp; i++) {
		offset[i] = BrScalarToFloat(self->cache.comp_offsets[i]);
		scale[i] = BrScalarToFloat(self->cache.comp_scales[i]);
	}

	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_primitive_state_3dfx, rangesQueryX)(
		struct br_primitive_state *self,
		br_fixed_ls *offset,
		br_fixed_ls *scale,
		br_int_32 max_comp)
{
	int i;

	/*
	 * Fail if the current info is not valid
	 */
    if (self->cache.timestamp_prim != self->prim.timestamp_major ||
		self->cache.timestamp_out != self->out.timestamp_major)
        return BRE_DEV_FAIL;

	for(i=0; i < max_comp; i++) {
		offset[i] = BrScalarToFixed(self->cache.comp_offsets[i]);
		scale[i] = BrScalarToFixed(self->cache.comp_scales[i]);
	}

	return BRE_OK;
}

