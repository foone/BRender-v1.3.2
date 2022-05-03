/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: pstate.c 1.1 1997/12/10 16:47:56 jon Exp $
 * $Locker: $
 *
 * Primitive state methods
 */
#include <stddef.h>
#include <string.h>

#include "drv.h"
#include "brassert.h"

BR_RCS_ID("$Id: pstate.c 1.1 1997/12/10 16:47:56 jon Exp $");

/*
 * Default dispatch table for primitive state (defined at end of file)
 */
static const struct br_primitive_state_dispatch primitiveStateDispatch;

/*
 * Primitive state info. template
 */
/*
 * Shortcuts for template flags
 */
#define S BRTV_SET
#define Q BRTV_QUERY
#define A BRTV_ALL

#if BASED_FIXED
#define AX BRTV_ALL
#else 
#define AX 0
#endif

#if BASED_FLOAT
#define AF BRTV_ALL
#else 
#define AF 0
#endif

#define F(f)	offsetof(struct br_primitive_state, f)
#define P(f)	((br_int_32)(&(f)))

static const struct br_tv_template_entry primitiveStateTemplateEntries[] = {
	{BRT(IDENTIFIER_CSTR),		F(identifier),				Q | A,				BRTV_CONV_COPY,},
	{BRT(PARTS_TL),				P(PrimPartsTokens),			Q | A | BRTV_ABS,	BRTV_CONV_LIST, },
	/*
	 * Under the hood hack
	 */
	{BRT(PRIMITIVE_BLOCK_P),	F(cache.last_block),		Q | S,				BRTV_CONV_COPY,0,		1},
};
#undef F
#undef P

/*
 * Set up a static device object
 */
struct br_primitive_state * PrimitiveStateSoftAllocate(struct br_primitive_library *plib)
{
	struct br_primitive_state * self;

	self = BrResAllocate(plib->device, sizeof(*self), BR_MEMORY_OBJECT);
	
	if(self == NULL)
		return NULL;

	self->plib = plib;
	self->dispatch = &primitiveStateDispatch;
    self->device = plib->device;

	/*
	 * Setup initial state
	 */
	self->out.colour.viewport_changed = BR_TRUE;

	ObjectContainerAddFront(plib,(br_object *)self);

	return self;
}

static void BR_CMETHOD_DECL(br_primitive_state_soft, free)(br_primitive_state *self)
{
	ObjectContainerRemove(self->plib, (br_object *)self);

	BrResFreeNoCallback(self);
}

static br_token BR_CMETHOD_DECL(br_primitive_state_soft, type)(br_primitive_state *self)
{
	return BRT_PRIMITIVE_STATE;
}

static br_boolean BR_CMETHOD_DECL(br_primitive_state_soft, isType)(br_primitive_state *self, br_token t)
{
	return (t == BRT_PRIMITIVE_STATE) || (t == BRT_OBJECT_CONTAINER) || (t == BRT_OBJECT);
}

static br_int_32 BR_CMETHOD_DECL(br_primitive_state_soft, space)(br_primitive_state *self)
{
	return sizeof(br_primitive_state);
}

static struct br_tv_template * BR_CMETHOD_DECL(br_primitive_state_soft,templateQuery)
	(br_primitive_state *self)
{
    if(self->device->templates.primitiveStateTemplate == NULL)
        self->device->templates.primitiveStateTemplate = BrTVTemplateAllocate(self->device,
            primitiveStateTemplateEntries,
            BR_ASIZE(primitiveStateTemplateEntries));

    return self->device->templates.primitiveStateTemplate;
}

/*
 * Functions for setting input and output buffers - returns true if lookup has been invalidated
 */
static br_boolean inputSet(struct input_buffer *ib, br_buffer_stored *new)
{
	br_buffer_stored *old = ib->buffer;

	/*
	 * Special case of same, NULL value
	 */
	if(old == NULL && new == NULL)
		return BR_FALSE;
	
	/*
	 * If buffer is no longer being used
	 */
	if(new == NULL) {
		ib->buffer = NULL;
		return BR_TRUE;
	}
	
	if(old != NULL) {
		/*
		 * See if new buffer has same character as old...
		 */
		if(	ib->type == new->buffer.type &&
			ib->width == new->buffer.width_p &&
			ib->height == new->buffer.height &&
			ib->stride == new->buffer.stride_b) {
			/*
			 * It does, clear invalid flag
			 */
			ib->buffer = new;
			return BR_FALSE;
		}
	}

	ib->buffer = new;
	ib->type = new->buffer.type;
	ib->width = new->buffer.width_p;
	ib->height = new->buffer.height;
	ib->stride = new->buffer.stride_b;

	return BR_TRUE;
}

static br_boolean outputSet(struct output_buffer *ob, br_device_pixelmap *new)
{
	br_device_pixelmap *old = ob->pixelmap;

	/*
	 * Special case of same, NULL value
	 */
	if(old == NULL && new == NULL)
		return BR_FALSE;
	
	/*
	 * If pixelmap is no longer being used
	 */
	if(new == NULL) {
		ob->pixelmap = NULL;
		ob->width = 0;
		ob->height = 0;
		ob->viewport_changed = BR_TRUE;

		return BR_TRUE;
	}
	
	if(old != NULL) {
		/*
		 * See if new pixelmap has same character as old...
		 */
		if(	ob->type == new->pm_type &&
			ob->width == new->pm_width &&
			ob->height == new->pm_height &&
			ob->stride == new->pm_row_bytes) {
			/*
			 * It does, return false
			 */
			ob->pixelmap = new;
			return BR_FALSE;
		}
	}

	ob->pixelmap = new;
	ob->type = new->pm_type;
	ob->width = new->pm_width;
	ob->height = new->pm_height;
	ob->stride = new->pm_row_bytes;

 	ob->viewport_changed = BR_TRUE;

	return BR_TRUE;
}


static br_error BR_CALLBACK customInputSet(void *block, br_uint_32 *pvalue, struct br_tv_template_entry *tep)
{
	/*
	 * XXX validate incoming object
	 */

	if(inputSet(
		(struct input_buffer *)((char *)block + tep->offset), 
		(br_buffer_stored *)*pvalue))
		tep->mask = 1;
	else
		tep->mask = 0;

	return BRE_OK;
}

static struct br_tv_custom customInputConv = {
	NULL,
	customInputSet,
	NULL,
};

static br_error BR_CALLBACK customOutputSet(void *block, br_uint_32 *pvalue, struct br_tv_template_entry *tep)
{
	/*
	 * XXX validate incoming object
	 */

	if(outputSet(
		(struct output_buffer *)((char *)block + tep->offset), 
		(br_device_pixelmap *)*pvalue))
		tep->mask = 1;
	else
		tep->mask = 0;

	return BRE_OK;
}

static const struct br_tv_custom customOutputConv = {
	NULL,
	customOutputSet,
	NULL,
};

/*
 * Templates for state set/query
 */
#define F(f) offsetof(struct br_primitive_state, f)
#define P(f)	((br_int_32)(&(f)))

static br_tv_template_entry partPrimitiveTemplateEntries[] = {
	{BRT(FORCE_FRONT_B),	F(prim.flags),			Q | S | A,	BRTV_CONV_BIT,	PRIMF_FORCE_FRONT,			1},
	{BRT(SMOOTH_B),			F(prim.flags),			Q | S | A,	BRTV_CONV_BIT,	PRIMF_SMOOTH,				1},
	{BRT(DECAL_B),			F(prim.flags),			Q | S | A,	BRTV_CONV_BIT,	PRIMF_DECAL,				1},
	{BRT(DITHER_COLOUR_B),	F(prim.flags),			Q | S | A,	BRTV_CONV_BIT,	PRIMF_DITHER_COLOUR,		1},
	{BRT(DITHER_MAP_B),		F(prim.flags),			Q | S | A,	BRTV_CONV_BIT,	PRIMF_DITHER_MAP,			1},
	{BRT(DEPTH_WRITE_B),	F(prim.flags),			Q | S | A,	BRTV_CONV_BIT,	PRIMF_DEPTH_WRITE,			1},
	{BRT(COLOUR_WRITE_B),	F(prim.flags),			Q | S | A,	BRTV_CONV_BIT,	PRIMF_COLOUR_WRITE,			1},
	{BRT(BLEND_B),			F(prim.flags),			Q | S | A,	BRTV_CONV_BIT,	PRIMF_BLEND,				1},
	{BRT(MODULATE_B),		F(prim.flags),			Q | S | A,	BRTV_CONV_BIT,	PRIMF_MODULATE,				1},
	{BRT(FOG_T),			F(prim.fog_type),		Q | S | A,	BRTV_CONV_COPY},

	{BRT(COLOUR_T),			F(prim.colour_type),	Q | S | A,	BRTV_CONV_COPY,0,							1},
	{BRT(COLOUR_B),			F(prim.colour_type),	S,			BRTV_CONV_BOOL_TOKEN,BRT_DEFAULT,			1},

	{BRT(INDEX_BASE_I32),	F(prim.index_base),		Q | S | A,	BRTV_CONV_COPY,0,							1},
	{BRT(INDEX_RANGE_I32),	F(prim.index_range),	Q | S | A,	BRTV_CONV_COPY,0,							1},

  	{BRT(PERSPECTIVE_T),	F(prim.perspective_type),Q | S | A,	BRTV_CONV_COPY,0,							1},
  	{BRT(PERSPECTIVE_B),	F(prim.perspective_type),S,			BRTV_CONV_BOOL_TOKEN,BRT_DEFAULT,			1},

	{BRT(SUBDIVIDE_TOLERANCE_I32),	F(prim.subdivide_tolerance),Q | S | A,	BRTV_CONV_COPY,0,				1},

	{BRT(PRIMITIVE_BLOCK_P),F(prim.custom_block),	Q | S,		BRTV_CONV_COPY,0,							1},

	{BRT(COLOUR_MAP_O),		F(prim.colour_map.buffer),Q | A,	BRTV_CONV_COPY},
	{BRT(COLOUR_MAP_O),		F(prim.colour_map),		S,			BRTV_CONV_CUSTOM,	P(customInputConv)		},

	{BRT(TEXTURE_O),		F(prim.colour_map.buffer),Q,		BRTV_CONV_COPY},
	{BRT(TEXTURE_O),		F(prim.colour_map),		S,			BRTV_CONV_CUSTOM,	P(customInputConv)		},

	{BRT(INDEX_SHADE_O),	F(prim.index_shade.buffer),Q | A,	BRTV_CONV_COPY},
	{BRT(INDEX_SHADE_O),	F(prim.index_shade),	S,			BRTV_CONV_CUSTOM,	P(customInputConv)		},

	{BRT(INDEX_BLEND_O),	F(prim.index_blend.buffer),Q | A,	BRTV_CONV_COPY},
	{BRT(INDEX_BLEND_O),	F(prim.index_blend),	S,			BRTV_CONV_CUSTOM,	P(customInputConv)		},

	{BRT(SCREEN_DOOR_O),	F(prim.screendoor.buffer),Q | A,		BRTV_CONV_COPY},
	{BRT(SCREEN_DOOR_O),	F(prim.screendoor),		S,			BRTV_CONV_CUSTOM,	P(customInputConv)		},

	{BRT(INDEX_LIGHT_O),	F(prim.lighting.buffer),Q | A,		BRTV_CONV_COPY},
	{BRT(INDEX_LIGHT_O),	F(prim.lighting),		S,			BRTV_CONV_CUSTOM,	P(customInputConv)		},

	{BRT(BUMP_O),			F(prim.bump.buffer),	Q | A,		BRTV_CONV_COPY},
	{BRT(BUMP_O),			F(prim.bump),			S,			BRTV_CONV_CUSTOM,	P(customInputConv)		},

	{BRT(INDEX_FOG_O),    F(prim.index_fog.buffer),	Q | A,		BRTV_CONV_COPY},
	{BRT(INDEX_FOG_O),    F(prim.index_fog),		S,			BRTV_CONV_CUSTOM,	P(customInputConv)		},
};

static br_tv_template_entry partOutputTemplateEntries[] = {
	{BRT(COLOUR_BUFFER_O),	F(out.colour.pixelmap),	Q | A,		BRTV_CONV_COPY},
	{BRT(COLOUR_BUFFER_O),	F(out.colour),			S,			BRTV_CONV_CUSTOM,	P(customOutputConv)		},
	{BRT(DEPTH_BUFFER_O),	F(out.depth.pixelmap), 	Q | A,		BRTV_CONV_COPY},
	{BRT(DEPTH_BUFFER_O),	F(out.depth),			S,			BRTV_CONV_CUSTOM,	P(customOutputConv)		},
};

static br_tv_template *findTemplate(struct br_primitive_state *self, br_token part)
{
	switch(part) {
	case BRT_PRIMITIVE:
        if(self->device->templates.partPrimitiveTemplate == NULL)
            self->device->templates.partPrimitiveTemplate = BrTVTemplateAllocate(self->device,
            partPrimitiveTemplateEntries, BR_ASIZE(partPrimitiveTemplateEntries));

	    return self->device->templates.partPrimitiveTemplate;

	case BRT_OUTPUT:
        if(self->device->templates.partOutputTemplate == NULL)
            self->device->templates.partOutputTemplate = BrTVTemplateAllocate(self->device,
            partOutputTemplateEntries, BR_ASIZE(partOutputTemplateEntries));

	    return self->device->templates.partOutputTemplate;
	}

	return NULL;
}

static br_error BR_CMETHOD_DECL(br_primitive_state_soft, partSet)(
		struct br_primitive_state *self,
		br_token part,
		br_int_32 index,
		br_token t,
		br_uint_32 value)
{
	br_error r;
	br_tv_template *tp = findTemplate(self, part);
	br_uint_32 m;

	/*
	 * If the part was unknown, punt
	 */
	if(tp == NULL)
		return BRE_FAIL;

	/*
	 * Do the set...
	 */
	m = 0;
	r = BrTokenValueSet(self, &m, t, value, tp);

	/*
	 * If unrecognised, no changes
	 */
	if(r != BRE_OK)
		return r;

	/*
	 * Update timestamps
	 */
	switch(part) {
	case BRT_PRIMITIVE:
		self->prim.timestamp = Timestamp();
		if(m)
			self->prim.timestamp_major = Timestamp();
		break;

	case BRT_OUTPUT:
		self->out.timestamp = Timestamp();
		if(m)
			self->out.timestamp_major = Timestamp();
		break;
	}

	return BRE_OK;
}

static br_error BR_CMETHOD_DECL(br_primitive_state_soft, partSetMany)(
		struct br_primitive_state *self,
		br_token part,
		br_int_32 index,
		br_token_value * tv,
		br_int_32 *pcount)
{
	br_error r;
	br_tv_template *tp = findTemplate(self, part);
	br_uint_32 m;
	br_int_32 c;

	/*
	 * If the part was unknown, punt
	 */
	if(tp == NULL)
		return BRE_FAIL;

	/*
	 * Do the set...
	 */
	m = 0;
	r = BrTokenValueSetMany(self, &c, &m, tv, tp);

	/*
	 * If unrecognised, no changes
	 */
	if(r != BRE_OK || c == 0)
		return r;

	/*
	 * Pass count back on to caller
	 */
	if(pcount)
		*pcount = c;

	/*
	 * Update timestamps
	 */
	switch(part) {
	case BRT_PRIMITIVE:
		self->prim.timestamp = Timestamp();
		if(m)
			self->prim.timestamp_major = Timestamp();
		break;

	case BRT_OUTPUT:
		self->out.timestamp = Timestamp();
		if(m)
			self->out.timestamp_major = Timestamp();
		break;
	}

	return BRE_OK;
}

static br_error BR_CMETHOD_DECL(br_primitive_state_soft, partQuery)(
		struct br_primitive_state *self,
		br_token part,
		br_int_32 index,
		br_uint_32 *pvalue,
		br_token t)
{
	br_tv_template *tp = findTemplate(self, part);

	if(tp == NULL)
		return BRE_FAIL;

	return BrTokenValueQuery(pvalue, NULL, NULL, t,  self, tp);
}

static br_error BR_CMETHOD_DECL(br_primitive_state_soft, partQueryBuffer)(
		struct br_primitive_state *self,
		br_token part,
		br_int_32 index,
		br_uint_32 *pvalue,
		br_uint_32 *buffer,
		br_size_t buffer_size,
		br_token t)
{
	br_tv_template *tp = findTemplate(self, part);

	if(tp == NULL)
		return BRE_FAIL;

	return BrTokenValueQuery(pvalue, buffer, buffer_size, t,  self, tp);
}

static br_error BR_CMETHOD_DECL(br_primitive_state_soft, partQueryMany)(
		struct br_primitive_state *self,
		br_token part,
		br_int_32 index,
		br_token_value *tv,
		void *extra,
		br_size_t extra_size,
		br_int_32 *pcount)
{
	br_tv_template *tp = findTemplate(self, part);

	if(tp == NULL)
		return BRE_FAIL;

	return BrTokenValueQueryMany(tv, extra, extra_size, pcount,  self, tp);
}

static br_error BR_CMETHOD_DECL(br_primitive_state_soft, partQueryManySize)(
		struct br_primitive_state *self,
		br_token part,
		br_int_32 index,
		br_size_t *pextra_size,
		br_token_value *tv)
{
	br_tv_template *tp = findTemplate(self, part);

	if(tp == NULL)
		return BRE_FAIL;

	return BrTokenValueQueryManySize(pextra_size, tv, self, tp);
}

static br_error BR_CMETHOD_DECL(br_primitive_state_soft, partQueryAll)(
		struct br_primitive_state *self,
		br_token part,
		br_int_32 index,
		br_token_value *buffer,
		br_size_t buffer_size)
{
	br_tv_template *tp = findTemplate(self, part);

	if(tp == NULL)
		return BRE_FAIL;

	return BrTokenValueQueryAll(buffer, buffer_size, self, tp);
}

static br_error BR_CMETHOD_DECL(br_primitive_state_soft, partQueryAllSize)(
		struct br_primitive_state *self,
		br_token part,
		br_int_32 index,
		br_size_t *psize)
{
	br_tv_template *tp = findTemplate(self, part);

	if(tp == NULL)
		return BRE_FAIL;

	return BrTokenValueQueryAllSize(psize, self, tp);
}

static br_error BR_CMETHOD_DECL(br_primitive_state_soft, partQueryCapability)(
		struct br_primitive_state *self,
		br_token part,
		br_int_32 index,
		br_token_value *buffer,
        br_size_t buffer_size)
{
    return BRE_FAIL;
}


static br_error BR_CMETHOD_DECL(br_primitive_state_soft, partIndexQuery)(
		struct br_primitive_state *self,
		br_token part,
		br_int_32 *pnindex)
{
	int n;

	switch(part) {
	case BRT_OUTPUT:
	case BRT_PRIMITIVE:
		n = 1;
		break;

	default:
		n = 0;
	}

	if(pnindex) {
		*pnindex = n;
		return BRE_OK;
	} else {
		return BRE_FAIL;
	}

}

static br_error BR_CMETHOD_DECL(br_primitive_state_soft, stateQueryPerformance)(
		struct br_primitive_state *self,
        br_fixed_lu *speed)
{
    return BRE_FAIL;
}

static br_error BR_CMETHOD_DECL(br_primitive_state_soft, stateDefault)(
		struct br_primitive_state *self,
		br_uint_32 mask)
{
	/*
	 * Clear everything down
	 *
 	 * Could use inputSet and OutputSet?
	 */
	if(mask & MASK_STATE_PRIMITIVE) {
		self->prim.flags = PRIMF_COLOUR_WRITE | PRIMF_DEPTH_WRITE;
		self->prim.colour_map.buffer = NULL;
		self->prim.index_shade.buffer = NULL;
		self->prim.index_blend.buffer = NULL;
		self->prim.screendoor.buffer = NULL;
		self->prim.lighting.buffer = NULL;
		self->prim.bump.buffer = NULL;
        self->prim.index_fog.buffer = NULL;
		self->prim.colour_type = BRT_DEFAULT;

		self->prim.timestamp = Timestamp();
		self->prim.timestamp_major = Timestamp();
	}

	if(mask & MASK_STATE_OUTPUT) {
		self->out.colour.pixelmap = NULL;
		self->out.depth.pixelmap = NULL;

		self->out.timestamp = Timestamp();
		self->out.timestamp_major = Timestamp();
	}

	return BRE_OK;
}


static br_error BR_CMETHOD_DECL(br_primitive_state_soft, stateCopy)(
		struct br_primitive_state *self,
		struct br_primitive_state *source,
		br_uint_32 mask)
{
	if (mask & (MASK_STATE_PRIMITIVE | MASK_STATE_OUTPUT))
		mask |= MASK_STATE_CACHE;

	if((mask & MASK_STATE_PRIMITIVE) && (self->prim.timestamp != source->prim.timestamp))
		self->prim = source->prim;

	if((mask & MASK_STATE_OUTPUT) && (self->out.timestamp != source->out.timestamp))
		self->out = source->out;

	if((mask & MASK_STATE_CACHE) &&
		((self->cache.timestamp_prim != source->cache.timestamp_prim) ||
		(self->cache.timestamp_out != source->cache.timestamp_out) ||
		(self->cache.last_type != source->cache.last_type)))
		self->cache = source->cache;

	return BRE_OK;
}

/*
 * Default dispatch table for device
 */
static const struct br_primitive_state_dispatch primitiveStateDispatch = {
	NULL,
	NULL,
	NULL,
	NULL,
	BR_CMETHOD_REF(br_primitive_state_soft,	free),
	BR_CMETHOD_REF(br_object_softprim,  	identifier),
	BR_CMETHOD_REF(br_primitive_state_soft, type),
	BR_CMETHOD_REF(br_primitive_state_soft, isType),
	BR_CMETHOD_REF(br_object_softprim,		device),
	BR_CMETHOD_REF(br_primitive_state_soft, space),

	BR_CMETHOD_REF(br_primitive_state_soft,	templateQuery),
	BR_CMETHOD_REF(br_object,				query),
	BR_CMETHOD_REF(br_object, 				queryBuffer),
	BR_CMETHOD_REF(br_object, 				queryMany),
	BR_CMETHOD_REF(br_object, 				queryManySize),
	BR_CMETHOD_REF(br_object, 				queryAll),
	BR_CMETHOD_REF(br_object,	 			queryAllSize),

	BR_CMETHOD_REF(br_primitive_state_soft,	partSet),
	BR_CMETHOD_REF(br_primitive_state_soft,	partSetMany),
	BR_CMETHOD_REF(br_primitive_state_soft,	partQuery),
	BR_CMETHOD_REF(br_primitive_state_soft,	partQueryBuffer),
	BR_CMETHOD_REF(br_primitive_state_soft,	partQueryMany),
	BR_CMETHOD_REF(br_primitive_state_soft,	partQueryManySize),
	BR_CMETHOD_REF(br_primitive_state_soft,	partQueryAll),
	BR_CMETHOD_REF(br_primitive_state_soft,	partQueryAllSize),
	BR_CMETHOD_REF(br_primitive_state_soft,	partIndexQuery),
	BR_CMETHOD_REF(br_primitive_state_soft,	stateDefault),
	BR_CMETHOD_REF(br_primitive_state_soft,	stateCopy),
	BR_CMETHOD_REF(br_primitive_state_soft,	renderBegin),
	BR_CMETHOD_REF(br_primitive_state_soft,	renderEnd),
	BR_CMETHOD_REF(br_primitive_state_soft,	rangesQueryF),
	BR_CMETHOD_REF(br_primitive_state_soft,	rangesQueryX),
    BR_CMETHOD_REF(br_primitive_state_soft, partQueryCapability),
    BR_CMETHOD_REF(br_primitive_state_soft, stateQueryPerformance),
};

