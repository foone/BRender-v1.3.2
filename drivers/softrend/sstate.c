/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: sstate.c 1.1 1997/12/10 16:52:39 jon Exp $
 * $Locker: $
 *
 * Stored renderer state 
 */
#include <stddef.h>
#include <string.h>

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: sstate.c 1.1 1997/12/10 16:52:39 jon Exp $");

/*
 * Default dispatch table for renderer type (defined at and of file)
 */
static const struct br_renderer_state_stored_dispatch rendererStateStoredDispatch;

/*
 * Geometry format info. template
 */
#define F(f)	offsetof(struct br_renderer_state_stored, f)

static struct br_tv_template_entry rendererStateStoredTemplateEntries[] = {
	{BRT_IDENTIFIER_CSTR,	0,	F(identifier),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT_RENDERER_O,		0,	F(renderer),	BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT_PARTS_U32,			0,	F(valid),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },

	{DEV(PRIMITIVE_STATE_O),	F(pstate),	BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
};
#undef F

/*
 * Utility functions to copy all or part of a state from one place to another
 * where either source or destination is a stored state
 *
 * The destination recalc. bitmasks are updated appropiately
 */
br_error StateCopyToStored(struct br_renderer_state_stored *dest, struct state_all *src, br_uint_32 copy_mask, void *res)
{
	if (copy_mask & MASK_CACHED_STATES)
		copy_mask |= MASK_STATE_CACHE;

	/*
	 * Restrict copy_mask to parts held in stored states (should it return
	 * an error if valid parts will be lost?)
	 */
	copy_mask &= MASK_STATE_STORED;

	/*
	 * Restrict copy_mask to valid source parts
	 */
	copy_mask &= src->valid;

	/*
	 * Merge valid mask into destination
	 */
	dest->valid |= copy_mask;

	/*
	 * See if any parts of primitive state need copying
	 */
	if(copy_mask & ~MASK_STATE_LOCAL) {
		if(dest->pstate) {
			if(src->pstate)
				PrimitiveStateStateCopy(dest->pstate, src->pstate, copy_mask);
			else
				PrimitiveStateStateDefault(dest->pstate, copy_mask);
			
		} else if(src->pstate) {
			/*
			 * Create new dest. state and copy into it
			 */
			if(PrimitiveLibraryStateNew(src->renderer->plib, &dest->pstate) == BRE_OK) {
				/*
				 * Attach new primitive state to given resource parent
				 */
				if(res)
					BrResAdd(res,dest->pstate);

				PrimitiveStateStateCopy(dest->pstate, src->pstate, copy_mask);
			}
		}
	}

	if(copy_mask & MASK_STATE_CULL && dest->cull.timestamp != src->cull.timestamp)
		dest->cull = src->cull;

	if(copy_mask & MASK_STATE_SURFACE && dest->surface.timestamp != src->surface.timestamp)
		dest->surface = src->surface;

	/*
	 * Copy cache, or mark destination's cache as invalid as necessary
	 */
	if (copy_mask & MASK_STATE_CACHE)
		
		if (src->cache.valid && dest->cull.timestamp == src->cull.timestamp &&
			dest->surface.timestamp == src->surface.timestamp) {
			
			if (dest->timestamp_cache != src->timestamp_cache) {

				dest->cache = src->cache;
				dest->timestamp_cache = src->timestamp_cache;
			}

		} else 

			dest->cache.valid = BR_FALSE;

	return BRE_OK;
}

br_error StateCopyFromStored(struct state_all *dest, struct br_renderer_state_stored *src, br_uint_32 copy_mask, void *res)
{
	if (copy_mask & MASK_CACHED_STATES)
		copy_mask |= MASK_STATE_CACHE;

	/*
	 * Restrict copy_mask to parts held in stored states (should it return
	 * an error if requested parts will not be copied?)
	 */
	copy_mask &= MASK_STATE_STORED;

	/*
	 * Restrict copy_mask to valid source parts
	 */
	copy_mask &= src->valid;

	/*
	 * Merge valid mask into destination
	 */
	dest->valid |= copy_mask;

	/*
	 * See if any parts of primitive state need copying
	 */
	if(copy_mask & ~MASK_STATE_LOCAL) {
		if(dest->pstate) {
			if(src->pstate)
				PrimitiveStateStateCopy(dest->pstate, src->pstate, copy_mask);
			else
				PrimitiveStateStateDefault(dest->pstate, copy_mask);
			
		} else if(src->pstate) {
			/*
			 * Create new dest. state and copy into it
			 */
			if(PrimitiveLibraryStateNew(src->renderer->plib, &dest->pstate) == BRE_OK) {
				/*
				 * Attach new primitive state to given resource parent
				 */
				if(res)
					BrResAdd(res,dest->pstate);

				PrimitiveStateStateCopy(dest->pstate, src->pstate, copy_mask);
			}
		}
	}

	if(copy_mask & MASK_STATE_CULL && dest->cull.timestamp != src->cull.timestamp)
		dest->cull = src->cull;

	if(copy_mask & MASK_STATE_SURFACE && dest->surface.timestamp != src->surface.timestamp)
		dest->surface = src->surface;

	/*
	 * Copy cache, or mark destination's cache as invalid as necessary
	 */
	if (copy_mask & MASK_STATE_CACHE)
		
		if (src->cache.valid && dest->cull.timestamp == src->cull.timestamp &&
			dest->surface.timestamp == src->surface.timestamp) {
			
			if (dest->timestamp_cache != src->timestamp_cache) {

				dest->cache = src->cache;
				dest->timestamp_cache = src->timestamp_cache;
			}

		} else 

			dest->cache.valid = BR_FALSE;

	return BRE_OK;
}

/*
 * Allocate a stored state
 */
br_renderer_state_stored * RendererStateStoredSoftAllocate(br_renderer *renderer,
	struct state_all *base_state, br_uint_32 m, br_token_value *tv)
{
	br_renderer_state_stored * self;

	self = BrResAllocate(renderer->device, sizeof(*self), BR_MEMORY_OBJECT_DATA);

	if(self == NULL)
		return NULL;

	self->dispatch = (struct br_renderer_state_stored_dispatch *)&rendererStateStoredDispatch;
 	self->identifier = "Renderer-State-Stored";
    self->device = renderer->device;
 	self->renderer = renderer;

	/*
	 * Copy initial state
	 */
	StateCopyToStored(self, base_state, m, self);

	ObjectContainerAddFront(renderer, (br_object *)self);

	return self;
}

static void BR_CMETHOD_DECL(br_renderer_state_stored_soft, free)(br_renderer_state_stored *self)
{
	ObjectContainerRemove(self->renderer, (br_object *)self);

	/*
	 * Any associated primitive state will have been attached as a resource
	 */
	BrResFreeNoCallback(self);
}

static br_token BR_CMETHOD_DECL(br_renderer_state_stored_soft, type)(br_renderer_state_stored *self)
{
	return BRT_RENDERER_STATE_STORED;
}

static br_boolean BR_CMETHOD_DECL(br_renderer_state_stored_soft, isType)(br_renderer_state_stored *self, br_token t)
{
	return (t == BRT_RENDERER_STATE_STORED) || (t == BRT_OBJECT);
}

static br_int_32 BR_CMETHOD_DECL(br_renderer_state_stored_soft, space)(br_renderer_state_stored *self)
{
	return sizeof(br_renderer_state_stored);
}

static struct br_tv_template * BR_CMETHOD_DECL(br_renderer_state_stored_soft, templateQuery)
	(br_renderer_state_stored *self)
{
    if(self->device->templates.rendererStateStoredTemplate == NULL)
        self->device->templates.rendererStateStoredTemplate = BrTVTemplateAllocate(self->device,
            (br_tv_template_entry *)rendererStateStoredTemplateEntries,
			BR_ASIZE(rendererStateStoredTemplateEntries));

    return self->device->templates.rendererStateStoredTemplate;
}

/*
 * Default dispatch table for renderer type (defined at and of file)
 */
static const struct br_renderer_state_stored_dispatch rendererStateStoredDispatch = {
	NULL,
	NULL,
	NULL,
	NULL,
	BR_CMETHOD_REF(br_renderer_state_stored_soft,		free),
	BR_CMETHOD_REF(br_object_soft,						identifier),
	BR_CMETHOD_REF(br_renderer_state_stored_soft,		type),
	BR_CMETHOD_REF(br_renderer_state_stored_soft,		isType),
	BR_CMETHOD_REF(br_object_soft,						device),
	BR_CMETHOD_REF(br_renderer_state_stored_soft,		space),

	BR_CMETHOD_REF(br_renderer_state_stored_soft,		templateQuery),
	BR_CMETHOD_REF(br_object,							query),
	BR_CMETHOD_REF(br_object,							queryBuffer),
	BR_CMETHOD_REF(br_object,							queryMany),
	BR_CMETHOD_REF(br_object,							queryManySize),
	BR_CMETHOD_REF(br_object,							queryAll),
	BR_CMETHOD_REF(br_object,							queryAllSize),
};

