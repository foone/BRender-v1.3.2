/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * Functions that match a primitive against the renderers state
 */
#include "drv.h"
#include "texture.h"
#include "shortcut.h"
#include "brassert.h"


/*
 * Invalid value for unknown pixelmap types
 */
#define PMT_NONE 255

extern br_primitive_library PrimitiveLibraryMyst;

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


static br_boolean updateRanges(struct br_primitive_state *self)
{
	br_boolean changed = BR_FALSE;
	br_scalar sx_scale, sx_offset, sy_scale, sy_offset, u_scale, v_scale;

	/*
 	 * XXX should make sure we have a valid block looked up
	 */
	if(self->cache.last_block == NULL)
		return(BR_TRUE);

	/* SX,SY range are based on current colour_buffer... but make sure we have one! */
	if (self->out.colour) {
		changed |= (self->cache.comp_scales[C_SX] != sx_scale ||
					self->cache.comp_offsets[C_SX] != sx_offset ||
					self->cache.comp_scales[C_SY] != sy_scale ||
					self->cache.comp_offsets[C_SY] != sy_offset);

		sx_offset = BrFloatToScalar(   ((float)self->out.colour->pm_width)  / 2.0f + self->out.colour->pm_base_x);
		sx_scale = BrFloatToScalar( ( ((float)self->out.colour->pm_width)  / 2.0f));
		sy_offset = BrFloatToScalar(   ((float)self->out.colour->pm_height) / 2.0f + self->out.colour->pm_base_y);
		sy_scale = -BrFloatToScalar( ( ((float)self->out.colour->pm_height) / 2.0f));

		self->cache.comp_offsets[C_SX] = sx_offset;
		self->cache.comp_scales[C_SX]  = sx_scale;
		self->cache.comp_offsets[C_SY] = sy_offset;
		self->cache.comp_scales[C_SY]  = sy_scale;
	}

	/*
     * U,V range are 0.0 - 1.0 but the Mystique and BRender don't use the same definition
	 * of 1.0: on the Mystique 1.0 is equivalent to 0.0, and on BRender it is the row
	 * before. See sbuffer.c for how these are allocated; these are 1.0 - one pixel,
	 * basically.
	 *
	 * Note these ranges are not guaranteed in sign, and therefore wrapping needs to be enabled.
	 */
#if 0
	/*
	 * No Dave, this is bollocks.  Also sign has nothing to do with it, it's just a question of
	 * staying in the range 0..1 and not allowing wrapping, or allowing arbitrary texture coordinates
	 * and allowing wrapping for repeating textures across meshes.  BRender just passes the UV
	 * coordinates from the model (via the map transform)
	 */
	if (self->prim.colour_map) {
		u_scale = self->prim.colour_map->buffer.u_range;
		v_scale = self->prim.colour_map->buffer.v_range;

		changed |= (self->cache.comp_scales[C_U] != u_scale || self->cache.comp_scales[C_V] != v_scale);

		self->cache.comp_scales[C_U] = u_scale;
		self->cache.comp_scales[C_V] = v_scale;
	}
#endif

	return(changed);
}


void MystiqueInitConstantRanges(struct br_primitive_state *self)
{
	int i;


	/*
	 * Everything defaults to offset = 0, scale = 1
	 */
	for(i=0; i < NUM_COMPONENTS; i++) {
		self->cache.comp_offsets[i] = BR_SCALAR(0.0);
		self->cache.comp_scales[i] = BR_SCALAR(1.0);
	}

	/*
	 * SZ: BRender 1.3.0 now supports scaling on the Z buffer; in a slightly strange way,
	 * where the range specifies the range either side of 0.
	 * the Mystique needs 0-1.0; therefore the range is -0.5 (so that result is -0.5 to
	 * +0.5) and an offset of 0.5 (to give the desired 0-1).
	 */
	self->cache.comp_offsets[C_SZ] = BR_SCALAR(0.5);
	self->cache.comp_scales[C_SZ] = -BR_SCALAR(0.5);


	/* R,G,B,A ranges 0 to 255.0 */
	self->cache.comp_scales[C_R] = BR_SCALAR(255.0f);
	self->cache.comp_scales[C_G] = BR_SCALAR(255.0f);
	self->cache.comp_scales[C_B] = BR_SCALAR(255.0f);
	self->cache.comp_scales[C_A] = BR_SCALAR(255.0f);
}



/* Mystique renderer state parameters */
static T_msiParameters msiParameters;

/* Local state parameters */
br_boolean r_textured;
br_boolean r_no_colour;     /* True only for non-modulate textures */
br_uint_8 opacity;			/* Opacity for screen-dooring */



br_boolean texture_cache_created_mystique = BR_FALSE;
void *current_palette;


br_boolean MystiqueTextureFreedCallback(struct render_buffer *texture)
{
	texture->on_card = BR_FALSE;

	/* Doesn't have to do anything to unregister Mystique libs */
	return(BR_TRUE);
}



/* Mystique requires all textures to be aligned on 32-byte boundaries */
#define TEXTURE_ALIGN	0x20

/*
 * Cache a texture in the card
 *
 * In;  the render buffer with all details of the texture
 */
br_error MystiqueCacheTexture(struct render_buffer *src)
{
	br_int_32      card_mem_pos;      // Position of texture in card memory (relative to start of texture cache)
	br_uint_32     texture_size;      // Size inc all mipmaps


	/* Initialise texture cache if required */
	if (!texture_cache_created_mystique) {
		br_uint_32 base, size;

		/* Initialise texture cache: reserve block for palette uploads */
		base = 0;
		size = msiInfo->msiTexture.MemSize;

		if (!TextureCacheInit(base, size, MystiqueTextureFreedCallback))
			return(BRE_DEV_NO_MEMORY);

		texture_cache_created_mystique = BR_TRUE;
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
	texture_size = src->size + TEXTURE_ALIGN;
	if (src->palette)
		texture_size += 512;	/* Add on size of palette */
	card_mem_pos = TextureCacheRequest(src, texture_size);
	if (card_mem_pos < 0) {
		src->uncacheable = BR_TRUE; // Any failure here also means never renderable
		return(BRE_DEV_NO_VIDEO_MEMORY);
	}
	src->card_offset = card_mem_pos & (~(TEXTURE_ALIGN-1));
	src->upload = BR_TRUE;

	src->on_card = BR_TRUE;

	return(BRE_OK);
}










/*
 * Set all important elements of the Mystique internal state
 *
 * Since the Mystique has a single state set function, there is
 * no performance penalty - indeed, there is probably a performance
 * advantage - in reprocessing and setting all state simultaneously.
 */
br_error SetMystiqueState(struct br_primitive_state *self,
                          struct local_block *pb, br_uint_32 flags)
{
	struct br_buffer_stored *colour_map;

    syslog("SetMystiqueState");

	colour_map = self->prim.colour_map;

    /*
     * Setup texture
     */
    if ((flags & PRIMF_TEXTURE_BUFFER) &&
        (MystiqueCacheTexture(&self->prim.colour_map->buffer) == BRE_OK)) {

        r_textured = BR_TRUE;

        /* Set texture details etc. */
        msiParameters.msiTexture.Enable = TRUE;
        msiParameters.msiTexture.Width  = self->prim.colour_map->buffer.width_p;
        msiParameters.msiTexture.Height = self->prim.colour_map->buffer.height;
        msiParameters.msiTexture.Planes = self->prim.colour_map->buffer.bits;
		msiParameters.msiTexture.Clamp_u = FALSE;
        msiParameters.msiTexture.Clamp_v = FALSE;

		/* Set texture transfer details */
		if (self->prim.colour_map->buffer.upload) {
			/* Set source for upload */
			msiParameters.msiTexture.pMem				= self->prim.colour_map->buffer.base;
			msiParameters.msiTexture.pHeap				= self->prim.colour_map->buffer.heap;
			if (self->prim.colour_map->buffer.palette) {
				/* Set palette upload */
				msiParameters.msiTexture.msiLUT.pMem	= self->prim.colour_map->buffer.palette;
				msiParameters.msiTexture.msiLUT.pHeap	= self->prim.colour_map->buffer.palette_heap;
			} else {
				/* No palette */
				msiParameters.msiTexture.msiLUT.pMem	= NULL;
				msiParameters.msiTexture.msiLUT.pHeap	= NULL;
			}
			self->prim.colour_map->buffer.upload = BR_FALSE;
		} else {
			/* Already on card */
			msiParameters.msiTexture.pMem			= NULL;
			msiParameters.msiTexture.pHeap			= NULL;
			msiParameters.msiTexture.msiLUT.pMem	= NULL;
			msiParameters.msiTexture.msiLUT.pHeap	= NULL;
		}

		/* Destination for upload and/or source for texture */
		msiParameters.msiTexture.CacheOffset = self->prim.colour_map->buffer.card_offset;
		if (self->prim.colour_map->buffer.palette)
			msiParameters.msiTexture.msiLUT.CacheOffset = self->prim.colour_map->buffer.card_offset +
														  self->prim.colour_map->buffer.size;
		else
			msiParameters.msiTexture.msiLUT.CacheOffset = 0;

		/* Set texturing mode */
        if (flags & PRIMF_MODULATE) {
            msiParameters.msiTexture.Modulate = TRUE;
            r_no_colour = BR_FALSE;
        } else {
            msiParameters.msiTexture.Modulate = FALSE;
            r_no_colour = BR_TRUE;
        }
        msiParameters.msiTexture.Decal = FALSE;
        msiParameters.msiTexture.Transparency = TRUE;
		msiParameters.msiTexture.KeyingColor = 0;
		switch(self->prim.colour_map->buffer.bits) {
		case 16:
			msiParameters.msiTexture.KeyingColorMask = 0xffff;
			break;
		case 15:
			msiParameters.msiTexture.KeyingColorMask = 0x7fff;
			break;
		case 8:
			msiParameters.msiTexture.KeyingColorMask = 0xff;
			break;
		}
        msiParameters.msiTexture.KeyingAlpha = 1;
        msiParameters.msiTexture.KeyingAlphaMask = 0;
    } else {
        /* Not textured or texture load failed: disable texturing */
        r_textured = BR_FALSE;
        r_no_colour = BR_FALSE;
        msiParameters.msiTexture.Enable = FALSE;
    }

    /* Enable/disable depth buffer */
    if (flags & PRIMF_DEPTH_BUFFER) {
        msiParameters.msiDepth.Enable  = TRUE;
        msiParameters.msiDepth.Compare = msiCMP_LEQUAL;
        msiParameters.msiDepth.Protect = FALSE;
    } else
		msiParameters.msiDepth.Enable = FALSE;

	if (flags & PRIMF_BLEND)
		opacity = self->prim.alpha_val*100.0f;
	else
		opacity = 100;

	msiParameters.msiColor.Dither = TRUE;
	msiParameters.msiColor.Protect = FALSE;

    /* Set renderer state and ready for primitives */
    msiSetParameters(&msiParameters);

    syslog(" -> complete");

    return BRE_OK;
}





/*
 * Start of a new block: parse renderer state
 */
br_error BR_CMETHOD_DECL(br_primitive_state_mystique, renderBegin)(
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
	br_token input_colour_type;

	/* Check and make sure that the renderer and buffers have been started up
	 * (in devpixmp.c) */
	MystiqueBuffersStartFrame();

	ASSERT(rpb);

    /* Pull rendering state flags, tinker them for hardware state setup and comparison -
	 * this simplifies SetMystiqueState and lets us use simpler block flags */
	flags = self->prim.flags;
    if (self->prim.colour_map)
        flags |= PRIMF_TEXTURE_BUFFER;
    if (self->out.depth)
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

			if (no_render)
				return BRE_OK;

			SetMystiqueState(self, pb, flags);

			/*
			 * Faff around with the block a bit
			 */
			if (self->prim.flags & PRIMF_BLEND)
				pb->p.flags |= BR_PRIMF_BLENDED;
			else
				pb->p.flags &= ~BR_PRIMF_BLENDED;
			return(BRE_OK);
		}
	}

	/*
     * Something has probably changed if we have reached here
     *
	 * Assume the worst, that changes will have happened
	 */
	*block_changed = BR_TRUE;

	if(self->prim.colour_type != BRT_DEFAULT)
		input_colour_type = self->prim.colour_type;
	else
		input_colour_type = BRT_RGB;


	switch(prim_type) {
	case BRT_POINT:			j = 0;	break;
	case BRT_LINE:			j = 1;	break;
	case BRT_TRIANGLE:		j = 2;	break;
	default:
		self->cache.last_block = NULL;
        return BRE_DEV_UNSUPPORTED;
	}

    /* Match against selected primitives */
	pb = primInfoTable[j].blocks;

	for(b=0; b < primInfoTable[j].nblocks; b++,pb++) {

        /* Do the flags match */
        if ((flags & pb->flags_mask) != pb->flags_cmp)
			continue;

        /* Got one!! */
		break;
	}

    /* Default to last primitive in block */
	if(b >= primInfoTable[j].nblocks) {
		pb--;
	}

    /* Return pointer to primitive, and set 'unchanged' if this
     * pointer is the same as previous match. */
	*rpb = &pb->p;

    if (pb == self->cache.last_block) {
		*block_changed = BR_FALSE;
	} else {
		self->cache.last_block = pb;
	}

	self->cache.last_type = prim_type;
	self->cache.timestamp_prim = self->prim.timestamp_major;
	self->cache.timestamp_out = self->out.timestamp_major;

	*ranges_changed = updateRanges(self);

	/*
	 * Faff around with the block a bit
	 */
	if (self->prim.flags & PRIMF_BLEND)
		pb->p.flags |= BR_PRIMF_BLENDED;
	else
		pb->p.flags &= ~BR_PRIMF_BLENDED;

    /* Stash copy of no_render flag in block */
	pb->no_render = no_render;

    /* Setup hardware state */
    if (no_render)
		return BRE_OK;

    return(SetMystiqueState(self, pb, flags));
}



br_error BR_CMETHOD_DECL(br_primitive_state_mystique, renderEnd)(
		struct br_primitive_state *self,
		struct brp_block *pb)
{
	struct local_block *lb = (struct local_block *)pb;

    if (lb->no_render)
		return BRE_OK;
		
	return BRE_OK;
}


br_error BR_CMETHOD_DECL(br_primitive_state_mystique, rangesQueryF)(
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


br_error BR_CMETHOD_DECL(br_primitive_state_mystique, rangesQueryX)(
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

