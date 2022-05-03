/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: match.c 1.1 1997/12/10 16:47:17 jon Exp $
 * $Locker: $
 *
 * Functions that match a primitive against the renderers state
 */
#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: match.c 1.1 1997/12/10 16:47:17 jon Exp $");

/*
 * Invalid value for unknown pixelmap types
 */
#define PMT_NONE 255

#if 0 /* DEBUG */
int	MatchCount = 0;
#endif

/*
 * Descriptions of all the available renderers
 *
 * These are not constant because the autoloader modifies the function pointer
 *
 */
static struct local_block primInfo_t8[] = {

#if BASED_FLOAT
#pragma message ("FLOAT")
#include "prim_t8f.c"
#endif

#if BASED_FIXED
#pragma message ("FIXED")
#include "prim_t8x.c"
#endif

};

static struct local_block primInfo_l8[] = {
#include "prim_l8.c"
};

static struct local_block primInfo_p8[] = {
#include "prim_p8.c"
};

static struct local_block primInfo_t15[] = {

#if BASED_FLOAT
#pragma message ("FLOAT")
#include "prm_t15f.c"
#endif

#if BASED_FIXED
#pragma message ("FIXED")
#include "prm_t15x.c"
#endif

};

static struct local_block primInfo_l15[] = {
#include "prim_l15.c"
};

static struct local_block primInfo_p15[] = {
#include "prim_p15.c"
};

static struct local_block primInfo_t16[] = {

#if BASED_FLOAT
#pragma message ("FLOAT")
#include "prm_t16f.c"
#endif

#if BASED_FIXED
#pragma message ("FIXED")
#include "prm_t16x.c"
#endif

};

static struct local_block primInfo_l16[] = {
#include "prim_l16.c"
};

static struct local_block primInfo_p16[] = {
#include "prim_p16.c"
};

static struct local_block primInfo_t24[] = {
#include "prim_t24.c"
};

static struct local_block primInfo_l24[] = {
#include "prim_l24.c"
};

static struct local_block primInfo_p24[] = {
#include "prim_p24.c"
};


struct prim_info_table {
	struct local_block *blocks;
	int nblocks;
};

static const struct prim_info_table  primInfoTables[4][3] = {
	{
		{ primInfo_p8, BR_ASIZE(primInfo_p8), },
		{ primInfo_l8, BR_ASIZE(primInfo_l8), },
		{ primInfo_t8, BR_ASIZE(primInfo_t8), },
	},{
		{ primInfo_p15, BR_ASIZE(primInfo_p15), },
		{ primInfo_l15, BR_ASIZE(primInfo_l15), },
		{ primInfo_t15, BR_ASIZE(primInfo_t15), },
	},{
		{ primInfo_p16, BR_ASIZE(primInfo_p16), },
		{ primInfo_l16, BR_ASIZE(primInfo_l16), },
		{ primInfo_t16, BR_ASIZE(primInfo_t16), },
	},{
		{ primInfo_p24, BR_ASIZE(primInfo_p24), },
		{ primInfo_l24, BR_ASIZE(primInfo_l24), },
		{ primInfo_t24, BR_ASIZE(primInfo_t24), },
	}
};

/*
 * Block of priminfo to use if MMX is detected
 */
#if USE_MMX
static struct local_block mmxInfo_t15[] = {
#if BASED_FLOAT
#include "mmx_t15f.c"
#endif

#if BASED_FIXED
#include "mmx_t15x.c"
#endif
};

static struct local_block mmxInfo_t16[] = {
#if BASED_FLOAT
#include "mmx_t16f.c"
#endif

#if BASED_FIXED
#include "mmx_t16x.c"
#endif
};


static const struct prim_info_table  mmxInfoTables[4][3] = {
	{
		{ primInfo_p8,	BR_ASIZE(primInfo_p8), },
		{ primInfo_l8,	BR_ASIZE(primInfo_l8), },
		{ primInfo_t8,	BR_ASIZE(primInfo_t8), },
	},{
		{ primInfo_p15, BR_ASIZE(primInfo_p15), },
		{ primInfo_l15, BR_ASIZE(primInfo_l15), },
		{ mmxInfo_t15, BR_ASIZE(mmxInfo_t15), },
	},{
		{ primInfo_p16, BR_ASIZE(primInfo_p16), },
		{ primInfo_l16, BR_ASIZE(primInfo_l16), },
		{ mmxInfo_t16,	BR_ASIZE(mmxInfo_t16), },
	},{
		{ primInfo_p24, BR_ASIZE(primInfo_p24), },
		{ primInfo_l24, BR_ASIZE(primInfo_l24), },
		{ primInfo_t24, BR_ASIZE(primInfo_t24), },
	}
};
#endif


/*
 * Transcribe buffer info into static paramater area for renderers
 */

/*
 * Output buffers
 */
static void updateWorkOut(struct prim_work *pw, struct br_primitive_state *self)
{
	if(self->out.colour.pixelmap)
		SetupRenderBuffer(&pw->colour, self->out.colour.pixelmap);

	if(self->out.depth.pixelmap)
		SetupRenderBuffer(&pw->depth, self->out.depth.pixelmap);

	pw->timestamp_out = self->out.timestamp;
}

/*
 * Input buffers
 */
static void updateWorkPrim(struct prim_work *pw, struct br_primitive_state *self)
{
	if(self->prim.colour_map.buffer) {
		pw->texture = self->prim.colour_map.buffer->buffer;

		/*
		 * If texture does not have a palette - inherit one from screen
		 */
		if(pw->texture.palette == NULL) {
			pw->texture.palette = pw->colour.palette;
			pw->texture.palette_size = pw->colour.palette_size;
		}
	}

	if(self->prim.bump.buffer)
		pw->bump = self->prim.bump.buffer->buffer;

	if(self->prim.index_shade.buffer) {
		pw->shade_type = self->prim.index_shade.buffer->buffer.type;
		pw->shade_table = self->prim.index_shade.buffer->buffer.base;
        pw->index_base = self->prim.index_base;
		pw->index_range = self->prim.index_range;

	}

	if(self->prim.index_blend.buffer) {
		pw->blend_type = self->prim.index_blend.buffer->buffer.type;
		pw->blend_table = self->prim.index_blend.buffer->buffer.base;
	}

	if(self->prim.screendoor.buffer) {
		pw->screendoor_type = self->prim.screendoor.buffer->buffer.type;
		pw->blend_table = self->prim.screendoor.buffer->buffer.base;
	}

	if(self->prim.lighting.buffer) {
		pw->lighting_type = self->prim.lighting.buffer->buffer.type;
		pw->blend_table = self->prim.lighting.buffer->buffer.base;
	}

    if(self->prim.index_fog.buffer) {
                pw->fog_type = self->prim.index_fog.buffer->buffer.type;
                pw->fog_table = self->prim.index_fog.buffer->buffer.base;
	}

	if(self->prim.flags & PRIMF_DECAL) {
		pw->decal_index_base = self->prim.index_base;
		pw->decal_index_range = self->prim.index_range;
		pw->decal_shade_height = self->prim.index_shade.height;
	}

	pw->timestamp_prim = self->prim.timestamp;
}

static void updateRanges(struct br_primitive_state *self)
{
	int i;
	br_uint_32 m;

	/*
 	 * XXX should make sure we have a valid block looked up
	 */
	if(self->cache.last_block == NULL)
		return;

	/*
	 * Mask of required components
	 */
	m = self->cache.last_block->p.constant_components | self->cache.last_block->p.vertex_components;

	/*
	 * Everything defaults to offset = 0, scale = 1
	 */
	for(i=0; i < NUM_COMPONENTS; i++) {
		self->cache.comp_offsets[i] = BR_SCALAR(0.0);
		self->cache.comp_scales[i] = BR_SCALAR(1.0);
	}

	/*
	 * SX,SY range are based on current colour_buffer
	 */
	if(self->cache.last_block->range_flags & RF_OFFSET_Y) {
		/*
		 * Offset Y by one scanline so that FPU setup code never gets y == 0 (as it chokes)
		 */
		self->cache.comp_offsets[C_SX] = BR_CONST_DIV(BrIntToScalar(self->out.colour.width),2) - BR_SCALAR(0.5);
		self->cache.comp_scales[C_SX] = BR_CONST_DIV(BrIntToScalar(self->out.colour.width),2);

		self->cache.comp_offsets[C_SY] = BR_CONST_DIV(BrIntToScalar(self->out.colour.height),2) + BR_SCALAR(0.5);
		self->cache.comp_scales[C_SY] = -BR_CONST_DIV(BrIntToScalar(self->out.colour.height),2);
	} else{
		self->cache.comp_offsets[C_SX] = BR_CONST_DIV(BrIntToScalar(self->out.colour.width),2) + BR_SCALAR(0.5);
		self->cache.comp_scales[C_SX] = BR_CONST_DIV(BrIntToScalar(self->out.colour.width),2);
		self->cache.comp_offsets[C_SY] = BR_CONST_DIV(BrIntToScalar(self->out.colour.height),2) + BR_SCALAR(0.5);
		self->cache.comp_scales[C_SY] = -BR_CONST_DIV(BrIntToScalar(self->out.colour.height),2);
	}

	/*
	 * SZ is fixed - -1 to +1 (Setup code remaps to unsigned 16 bit)
	 */
	self->cache.comp_offsets[C_SZ] = BR_SCALAR(0);
	self->cache.comp_scales[C_SZ] = -BR_SCALAR(32767);

	/*
	 * U,V range are based on current colour_map, if required
	 */
	if(m & (CM_U | CM_V)) {
		if(self->cache.last_block->texture_type != PMT_NONE) {
                        if(self->cache.last_block->range_flags & RF_UNSCALED_TEXTURE_COORDS) {
                                self->cache.comp_scales[C_U] = BR_SCALAR(1);
                                self->cache.comp_scales[C_V] = BR_SCALAR(1);
                        }else{
                                self->cache.comp_scales[C_U] = BrIntToScalar(self->prim.colour_map.width);
                                self->cache.comp_scales[C_V] = BrIntToScalar(self->prim.colour_map.height);
                        }
		}
	}

	/*
	 * R,G,B are pulled from current prim. block, unless 'rgb_shade' is true,
	 * when the scales are from the shade tbale height
	 */															   
	if(m & (CM_R | CM_G | CM_B)) {
		if(self->cache.last_block->range_flags & RF_RGB_SHADE) {
			self->cache.comp_scales[C_R] = BrIntToScalar(self->prim.index_shade.height-1);
			self->cache.comp_scales[C_G] = BrIntToScalar(self->prim.index_shade.height-1);
			self->cache.comp_scales[C_B] = BrIntToScalar(self->prim.index_shade.height-1);

			self->cache.comp_offsets[C_R] = BR_SCALAR(0.5);
			self->cache.comp_offsets[C_G] = BR_SCALAR(0.5);
			self->cache.comp_offsets[C_B] = BR_SCALAR(0.5);
		} else {
			self->cache.comp_scales[C_R] = self->cache.last_block->colour_scales[0];
			self->cache.comp_scales[C_G] = self->cache.last_block->colour_scales[1];
			self->cache.comp_scales[C_B] = self->cache.last_block->colour_scales[2];

			self->cache.comp_offsets[C_R] = self->cache.last_block->colour_offsets[0];
			self->cache.comp_offsets[C_G] = self->cache.last_block->colour_offsets[1];
			self->cache.comp_offsets[C_B] = self->cache.last_block->colour_offsets[2];
		}
	}

	/*
	 * Alpha scaling is specified in block, unless a blend or screendoor table is used
	 */
	if(m & CM_A) {
		if(self->cache.last_block->blend_type != PMT_NONE) {
			self->cache.comp_scales[C_A] = BrIntToScalar(self->prim.index_blend.height-1);
			self->cache.comp_offsets[C_A] = BR_SCALAR(0.5);
		} else if(self->cache.last_block->screendoor_type != PMT_NONE) {
			self->cache.comp_scales[C_A] = BrIntToScalar(self->prim.screendoor.height-1);
			self->cache.comp_offsets[C_A] = BR_SCALAR(0.5);
		} else {
			self->cache.comp_scales[C_A] = self->cache.last_block->colour_scales[3];
			self->cache.comp_offsets[C_A] = self->cache.last_block->colour_offsets[3];
		}
	}

	/*
	 * I - If a shade table is in use, comes from shade table, else use user base/range
	 */
	if(m & CM_I) {
		if(self->cache.last_block->range_flags & RF_DECAL) {
			self->cache.comp_offsets[C_I] = BR_SCALAR(0.5);
			self->cache.comp_scales[C_I] = BR_SCALAR(254.0);
		} else if(self->cache.last_block->shade_type != PMT_NONE) {
			self->cache.comp_scales[C_I] = BrIntToScalar(self->prim.index_shade.height-1);
			self->cache.comp_offsets[C_I] = BR_SCALAR(0.5);
		} else {
			self->cache.comp_scales[C_I] = BrIntToScalar(self->prim.index_range);
			self->cache.comp_offsets[C_I] = BrIntToScalar(self->prim.index_base) + BR_SCALAR(0.5);
		}
	}
}

/*
 * Reture true if argument is a power of 2
 */
static br_boolean isPowerof2(br_int_32 x)
{
	return !((x-1) & x);
}

br_error BR_CMETHOD_DECL(br_primitive_state_soft, renderBegin)(
		struct br_primitive_state *self,
		struct brp_block **rpb,
		br_boolean *block_changed,
		br_boolean *ranges_changed,
		br_boolean no_render,
		br_token prim_type)
{
	int i,j,b,nb;
	struct local_block *pb;
	br_uint_32 flags;
	br_token input_colour_type;

	ASSERT(rpb);
	ASSERT(self);
	ASSERT(self->plib);

	/*
	 * Lock destination pixelmap for rendering a new model or scene if
	 * it is not already locked.
	 * N.B. Pixelmap is unlocked by br_primitive_library_soft::flush.
	 */

	if (!no_render && self->plib->colour_buffer != self->out.colour.pixelmap)
	{
		/*
		 * Unlock previous buffer
		 */
		if(self->plib->colour_buffer != NULL)
			DevicePixelmapDirectUnlock( self->plib->colour_buffer);

		/*
		 * Lock new buffer
		 */
		self->plib->colour_buffer = self->out.colour.pixelmap ;
		DevicePixelmapDirectLock( self->plib->colour_buffer, BR_TRUE);
	}

#if 1
	/*
	 * If previous match is still valid, return that
	 */
	if(self->cache.last_type == prim_type) {

		if(self->cache.timestamp_prim == self->prim.timestamp_major &&
			self->cache.timestamp_out == self->out.timestamp_major) {

			*rpb = &self->cache.last_block->p;
			*block_changed = BR_FALSE;
			*ranges_changed = BR_FALSE;

			/*
			 * Generate 'work' data
			 */
			if(self->cache.last_block->work->timestamp_prim != self->prim.timestamp)
				updateWorkPrim(self->cache.last_block->work, self);

			if(self->cache.last_block->work->timestamp_out != self->out.timestamp)
				updateWorkOut(self->cache.last_block->work, self);

			/*
			 * Flush rasterise stack if necessary
			 */
			if(self->cache.last_block->range_flags & RF_FLUSH)
				RasteriseBufferFlush();

			return BRE_OK;
		}
	}
#endif

	/*
	 * Assume the worst, that changes will have happened
	 */
	*block_changed = BR_TRUE;
	*ranges_changed = BR_TRUE;

#if 0 /* DEBUG */
	MatchCount++;
#endif

	/*
	 * XXX Validate the various buffers we are about to use 
	 */
#if 0
	if(self->colour) {
		if(!ObjectIsA(self->colour,BRT_DEVICE_PIXELMAP))
			return BRE_FAIL;

		if(self->colour->flags & BR_PMF_NO_DIRECT_ACCESS)
			return BRE_FAIL;

		if(!(self->colour->flags & BR_PMF_PIXELS_NEAR))
			return BRE_FAIL;
	}
#endif

	/*
	 * Generate buffer info
	 */
	work.colour.type = PMT_NONE;
	work.depth.type = PMT_NONE;
	work.texture.type = PMT_NONE;
	work.bump.type = PMT_NONE;
	work.shade_type = PMT_NONE;
	work.blend_type = PMT_NONE;
	work.screendoor_type = PMT_NONE;
	work.lighting_type = PMT_NONE;
    work.fog_type = PMT_NONE;

	work.texture.width_p = 0;
	work.texture.height = 0;

	updateWorkOut(&work, self);
	updateWorkPrim(&work, self);

	if(self->prim.custom_block) {
		/*
		 * If a custom block has been give, use that
		 */
		pb = self->prim.custom_block;

	} else {
		/*
		 * Generate exrta match flag flags
		 */
		flags = self->prim.flags;

		if(work.index_range == 0)
			flags|=PRIMF_RANGE_ZERO;
		
		if(work.texture.type != PMT_NONE) {
			/*
			 * See if the stride is the same as the width
			 */
			if(work.texture.width_b == work.texture.stride_b)
				flags |= PRIMF_NO_SKIP;
        
            if(work.texture.stride_b>0)
                    flags |= PRIMF_STRIDE_POSITIVE;
			/*
			 * See if texture size is ^2 which is .lt. 1024
			 */
		    if(isPowerof2(work.texture.width_p)
		        && isPowerof2(work.texture.height)
			    && work.texture.width_p <= 1024
			    && work.texture.height <= 1024) {

			    flags |= PRIMF_POWER2;
		    }


		    /*
		     * Does texture have a large enough palette attached?
		     */
		    if(work.texture.palette) {
			    int s;

		        switch(work.texture.type) {
			    case BR_PMT_INDEX_1: s = 2; break;
			    case BR_PMT_INDEX_2: s = 4; break;
			    case BR_PMT_INDEX_4: s = 16; break;
			    case BR_PMT_INDEX_8: s = 256; break;
			    }

		        if(work.texture.palette_size >= s)
				    flags |= PRIMF_PALETTE;
		    }

	    }

	    /*
	     * Look for perpective correct primitives if type is set (but not to subdivide)
	     */
	    if(self->prim.perspective_type != BRT_NONE &&
		    self->prim.perspective_type != BRT_SUBDIVIDE) {
			    flags |= PRIMF_PERSPECTIVE;
	    }

	    /*
	     * Pick a primitives array based on colour buffer type and primitive
	     */
	    switch(work.colour.type) {
	    case BR_PMT_INDEX_8:	i = 0;	input_colour_type = BRT_INDEX; break;
	    case BR_PMT_RGB_555:	i = 1;	input_colour_type = BRT_RGB; break;
	    case BR_PMT_RGB_565:	i = 2;	input_colour_type = BRT_RGB; break;
	    case BR_PMT_RGB_888:	i = 3;	input_colour_type = BRT_RGB; break;
	    default:
		    self->cache.last_block = NULL;
		    return BRE_FAIL;
	    }

	    if(self->prim.colour_type != BRT_DEFAULT)
		    input_colour_type = self->prim.colour_type;

	    switch(prim_type) {
	    case BRT_POINT:			j = 0;	break;
	    case BRT_LINE:			j = 1;	break;
	    case BRT_TRIANGLE:		j = 2;	break;
	    default:
		    self->cache.last_block = NULL;
		    return BRE_FAIL;
	    }

	    /*
	     * Match against selected primitives
	     */
    #if USE_MMX
	    if(self->plib->use_mmx) {
		    pb = mmxInfoTables[i][j].blocks;
		    nb = mmxInfoTables[i][j].nblocks;
	    } else {
		    pb = primInfoTables[i][j].blocks;
		    nb = primInfoTables[i][j].nblocks;
	    }

    #else
	    pb = primInfoTables[i][j].blocks;
	    nb = primInfoTables[i][j].nblocks;
    #endif

	    for(b=0; b < nb; b++,pb++) {

		    /*
		     * Do the flags match
		     */
		    if((flags & pb->flags_mask) != pb->flags_cmp)
			    continue;

		    /*
		     * Check buffer types
		     */
		    if((pb->depth_type)  != PMT_NONE && (work.depth.type != pb->depth_type))
			    continue;

		    if((pb->texture_type) != PMT_NONE && (work.texture.type != pb->texture_type))
			    continue;

		    if((pb->shade_type) != PMT_NONE && (work.shade_type != pb->shade_type))
			    continue;

		    if((pb->bump_type) != PMT_NONE && (work.bump.type != pb->bump_type))
			    continue;

		    if((pb->lighting_type) != PMT_NONE && (work.lighting_type != pb->lighting_type))
			    continue;

		    if((pb->screendoor_type) != PMT_NONE && (work.screendoor_type != pb->screendoor_type))
			    continue;

                    if((pb->blend_type) != PMT_NONE && (work.blend_type != pb->blend_type))
			    continue;

                    if((pb->fog_type) != PMT_NONE && (work.fog_type != pb->fog_type))
			    continue;

		    /*
		     * See if input colour type matches
		     */
		    if(pb->input_colour_type && (input_colour_type != pb->input_colour_type))
			    continue;

		    /*
		     * See if there are any size restrictions
		     */
		    if(pb->map_width && (pb->map_width != work.texture.width_p))
			    continue;

		    if(pb->map_height && (pb->map_height != work.texture.height))
			    continue;

		    /*
		     * Got one!!
		     */
		    break;
	    }

	    /*
	     * Default to last primitive in block
	     */
	    if(b >= nb) {
		    pb--;
	    }
	}

	/*
	 * Copy buffer info if neccesary
	 */
	if(pb->work && (pb->work != &work))
		*pb->work = work;

	/*
	 * If subdivision was requested, mark up the block
	 * If rasteriser is perspective correct do not disturb subdivide flag
	 */
	if(self->prim.perspective_type == BRT_SUBDIVIDE || (pb->range_flags & RF_NEED_SUBDIVIDE)) {

		pb->p.flags |= BR_PRIMF_SUBDIVIDE;
		pb->p.subdivide_tolerance = self->prim.subdivide_tolerance;

	} else {
		pb->p.flags &= ~BR_PRIMF_SUBDIVIDE;
	}

	/*
	 * return pointer to primitive, and set 'unchanged' if this
	 * pointer is the same as previous match
	 */
	*rpb = &pb->p;

	if(pb == self->cache.last_block) {
		*block_changed = BR_FALSE;
	} else {
		self->cache.last_block = pb;
	}

	updateRanges(self);

	self->cache.last_type = prim_type;
	self->cache.timestamp_prim = self->prim.timestamp_major;
	self->cache.timestamp_out = self->out.timestamp_major;

	if(pb->range_flags & RF_FLUSH)
		RasteriseBufferFlush();

	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_primitive_state_soft, renderEnd)(
		struct br_primitive_state *self,
		struct brp_block *pb)
{

	return BRE_OK;
}

/*
 * Thunk that loads a renderer from a DLL
 *
 * pb->entry_info is a pointer to the name of the new render fn.
 */
void BR_ASM_CALL RenderAutoloadThunk(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2)
{
	struct local_block *pb = (struct local_block *)block;
	br_image *image;
	brp_render_fn *render_fn;
	br_work_fn *work_fn;
	struct prim_work *old_work;

	ASSERT(pb->image_name);
	ASSERT(pb->entry_info);

	/*
	 * Assume the worst - if no renderer is found - use dummy
	 */
	pb->p.render = (brp_render_fn *)TriangleRenderNull;

	/*
	 * Try any load up referenced image
	 */
	image = BrImageReference(pb->image_name);

	ASSERT(image);

	/*
	 * Don't do anything if it failed
	 */
	if(image == NULL)
		return;

	/*
	 * Look up function name
	 */
	render_fn = BrImageLookupName(image,(char *)pb->entry_info,0);

	if(render_fn == NULL) {
		BrImageDereference(image);
		return;
	}

	/*
	 * Look up work ptr function
	 */
	work_fn = BrImageLookupName(image,"_PrimLibWork",0);
	ASSERT(work_fn);

	if(work_fn == NULL) {
		BrImageDereference(image);
		return;
	}

	/*
	 * Get work pointer
	 */
	old_work = pb->work;
	pb->work = work_fn();

	/*
	 * Copy work data into final destination
	 */
	*pb->work = *old_work;

	/*
	 * Change block over to use loaded function
	 */
	pb->p.render = render_fn;

	/*
	 * Hand over to new rendering function
	 */
	render_fn(block,v0,v1,v2);
}

/*
 * Thunk that loads generic setup rasterisers from a DLL
 *
 * pb->entry_info is the new render entry point
 *
 * Each of the setup.rasterise... function pointers is converted from
 * a pointer to a string to a pointer to a function looked up in the image
 */
void BR_ASM_CALL GenericAutoloadThunk(brp_block *block, brp_vertex *v0, brp_vertex *v1,brp_vertex *v2)
{
	struct local_block *pb = (struct local_block *)block;
	br_image *image;

	ASSERT(pb->image_name);
	ASSERT(pb->entry_info);

	/*
	 * Assume the worst - if no renderer is found - use dummy
	 */
	pb->p.render = (brp_render_fn *)TriangleRenderNull;

	/*
	 * Try any load up referenced image
	 */
	image = BrImageReference(pb->image_name);

	ASSERT(image);

	/*
	 * Don't do anything if it failed
	 */
	if(image == NULL)
		return;

	/*
	 * Look up the rasteriser names
	 */
	pb->setup.rasterise_rl_l = BrImageLookupName(image,(char *)pb->setup.rasterise_rl_l,0);
	pb->setup.rasterise_lr_l = BrImageLookupName(image,(char *)pb->setup.rasterise_lr_l,0);
	pb->setup.rasterise_rl_s = BrImageLookupName(image,(char *)pb->setup.rasterise_rl_s,0);
	pb->setup.rasterise_lr_s = BrImageLookupName(image,(char *)pb->setup.rasterise_lr_s,0);

	if(pb->setup.rasterise_rl_l == NULL ||
    	pb->setup.rasterise_lr_l == NULL ||
	    pb->setup.rasterise_rl_s == NULL ||
		pb->setup.rasterise_lr_s == NULL) {
		BrImageDereference(image);
		return;
	}

	/*
	 * Change block over to use the generic setup function
	 */
	pb->p.render = pb->entry_info;

	/*
	 * Hand over to setp function
	 */
	pb->p.render(block,v0,v1,v2);
}

br_error BR_CMETHOD_DECL(br_primitive_state_soft, rangesQueryF)(
		struct br_primitive_state *self,
		br_float *offset,
		br_float *scale,
		br_int_32 max_comp)
{
	int i;

	/*
	 * Fail if the current info is not valid
	 */
	if(self->cache.timestamp_prim != self->prim.timestamp_major ||
		self->cache.timestamp_out != self->out.timestamp_major)
		return BRE_FAIL;

	for(i=0; i < max_comp; i++) {
		offset[i] = BrScalarToFloat(self->cache.comp_offsets[i]);
		scale[i] = BrScalarToFloat(self->cache.comp_scales[i]);
	}

	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_primitive_state_soft, rangesQueryX)(
		struct br_primitive_state *self,
		br_fixed_ls *offset,
		br_fixed_ls *scale,
		br_int_32 max_comp)
{
	int i;

	/*
	 * Fail if the current info is not valid
	 */
	if(self->cache.timestamp_prim != self->prim.timestamp_major ||
		self->cache.timestamp_out != self->out.timestamp_major)
		return BRE_FAIL;

	for(i=0; i < max_comp; i++) {
		offset[i] = BrScalarToFixed(self->cache.comp_offsets[i]);
		scale[i] = BrScalarToFixed(self->cache.comp_scales[i]);
	}

	return BRE_OK;
}

/*
 * Dummy triangle renderer
 */
void BR_ASM_CALL TriangleRenderNull(brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2)
{
}

