/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * Primitive state methods
 */
#include <stddef.h>
#include <string.h>

#include "drv.h"
#include "brassert.h"


/*
 * Default dispatch table for primitive state (defined at end of file)
 */
static struct br_primitive_state_dispatch primitiveStateDispatch;

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

static struct br_tv_template_entry primitiveStateTemplateEntries[] = {
	{BRT(IDENTIFIER_CSTR),		F(identifier),			Q | A,				BRTV_CONV_COPY,	},
	{BRT(PARTS_TL),				&PrimParts3DFXTokens,		Q | A | BRTV_ABS,	BRTV_CONV_LIST, },
};
#undef F

static struct br_tv_template primitiveStateTemplate = {
	BR_ASIZE(primitiveStateTemplateEntries),
	primitiveStateTemplateEntries
};

/*
 * Set up a static device object
 */
struct br_primitive_state * PrimitiveState3DfxAllocate(struct br_primitive_library *plib)
{
	struct br_primitive_state * self;

	self = BrResAllocate(DRIVER_RESOURCE, sizeof(*self), BR_MEMORY_OBJECT);
	
	if(self == NULL)
		return NULL;

	self->plib = plib;
	self->dispatch = &primitiveStateDispatch;

	self->prim.map_antialiasing = BRT_NONE;
	self->prim.map_interpolation = BRT_NONE;
	self->prim.mip_interpolation = BRT_NONE;
	self->prim.fog_type = BRT_NONE;
	self->prim.fog_colour = 0xff0000;
	self->prim.fog_min = 0.0f;
	self->prim.fog_max = 1.0f;

	self->prim.depth_test = BRT_LESS_OR_EQUAL;
	self->prim.blend_mode = BRT_BLEND_STANDARD;
	self->prim.map_width_limit = BRT_WRAP;
	self->prim.map_height_limit = BRT_WRAP;

	/*
	 * Setup initial state
	 */
	initConstantRanges(self);

	ObjectContainerAddFront(plib,(br_object *)self);

	return self;
}

static void BR_CMETHOD_DECL(br_primitive_state_3dfx, free)(br_primitive_state *self)
{
	ObjectContainerRemove(self->plib, (br_object *)self);

	BrResFreeNoCallback(self);
}

static br_token BR_CMETHOD_DECL(br_primitive_state_3dfx, type)(br_primitive_state *self)
{
	return BRT_PRIMITIVE_STATE;
}

static br_boolean BR_CMETHOD_DECL(br_primitive_state_3dfx, isType)(br_primitive_state *self, br_token t)
{
	return (t == BRT_PRIMITIVE_STATE) || (t == BRT_OBJECT_CONTAINER) || (t == BRT_OBJECT);
}

static br_int_32 BR_CMETHOD_DECL(br_primitive_state_3dfx, space)(br_primitive_state *self)
{
	return sizeof(br_primitive_state);
}

static struct br_tv_template * BR_CMETHOD_DECL(br_primitive_state_3dfx,templateQuery)
	(br_primitive_state *self)
{
	primitiveStateTemplate.res = DRIVER_RESOURCE;
	return &primitiveStateTemplate;
}

/*
 * Functions for setting output buffers - returns true if CACHE has been invalidated
 */
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

		return BR_TRUE;
	}
	
	if(old != NULL) {
		/*
		 * See if new pixelmap has same character as old...
		 */
		if(	ob->width == new->pm_width &&
			ob->height == new->pm_height &&
			ob->base_x == new->pm_base_x &&
			ob->base_y == new->pm_base_y) {
			/*
			 * It does, return false
			 */
			ob->pixelmap = new;
			return BR_FALSE;
		}
	}

	ob->pixelmap = new;
	ob->width = new->pm_width;
	ob->height = new->pm_height;
	ob->base_x = new->pm_base_x;
	ob->base_y = new->pm_base_y;

	return BR_TRUE;
}


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

static br_tv_template_entry primitiveTemplateEntries[] = {
	{BRT(SMOOTH_B),			F(prim.flags),		Q | S | A,	BRTV_CONV_BIT,	PRIMF_SMOOTH,		1},
	{BRT(SMOOTH_OPACITY_B),	F(prim.flags),		Q | S | A,	BRTV_CONV_BIT,	PRIMF_SMOOTH_ALPHA,	1},
	{BRT(PERSPECTIVE_B),	F(prim.flags),		Q | S | A,	BRTV_CONV_BIT,	PRIMF_PERSPECTIVE,	1},
	{BRT(DECAL_B),			F(prim.flags),		Q | S | A,	BRTV_CONV_BIT,	PRIMF_DECAL,		1},
	{BRT(DEPTH_WRITE_B),	F(prim.flags),		Q | S | A,	BRTV_CONV_BIT,	PRIMF_DEPTH_WRITE,	1},
	{BRT(COLOUR_WRITE_B),	F(prim.flags),		Q | S | A,	BRTV_CONV_BIT,	PRIMF_COLOUR_WRITE,	1},
	{BRT(BLEND_B),			F(prim.flags),		Q | S | A,	BRTV_CONV_BIT,	PRIMF_BLEND,		1},
	{BRT(MODULATE_B),		F(prim.flags),		Q | S | A,	BRTV_CONV_BIT,	PRIMF_MODULATE,		1},
	{BRT(COLOUR_KEY_B),		F(prim.flags),		Q | S | A,	BRTV_CONV_BIT,	PRIMF_COLOUR_KEY,	1},

	{BRT(COLOUR_T),			F(prim.colour_type),Q | S | A,	BRTV_CONV_COPY,0,					1},
	{BRT(COLOUR_B),			F(prim.colour_type),Q | S | A,	BRTV_CONV_BOOL_TOKEN,BRT_DEFAULT,	1},

	{BRT(COLOUR_MAP_O),		F(prim.colour_map),	Q | S | A,	BRTV_CONV_COPY},
	{BRT(TEXTURE_O),		F(prim.colour_map),	Q | S | A,	BRTV_CONV_COPY},

	{BRT(FOG_T),			F(prim.fog_type),	Q | A | S,	BRTV_CONV_COPY},
	{BRT(FOG_RGB),			F(prim.fog_colour),	Q | A | S,	BRTV_CONV_COPY},
	{BRT(FOG_MIN_X),		F(prim.fog_min),	Q | A | S,	BRTV_CONV_FIXED_FLOAT},
	{BRT(FOG_MAX_X),		F(prim.fog_max),	Q | A | S,	BRTV_CONV_FIXED_FLOAT},
	{BRT(FOG_MIN_F),		F(prim.fog_min),	Q | A | S,	BRTV_CONV_COPY},
	{BRT(FOG_MAX_F),		F(prim.fog_max),	Q | A | S,	BRTV_CONV_COPY},
	{BRT(OPACITY_X),		F(prim.alpha_val),	Q | A | S,	BRTV_CONV_FIXED_FLOAT},
	{BRT(OPACITY_F),		F(prim.alpha_val),	Q | A | S,	BRTV_CONV_COPY},

	{BRT(MAP_ANTIALIASING_T), 	F(prim.map_antialiasing),	Q | A | S,	BRTV_CONV_COPY},
	{BRT(MAP_INTERPOLATION_T),	F(prim.map_interpolation),	Q | A | S,	BRTV_CONV_COPY},
	{BRT(MIP_INTERPOLATION_T),	F(prim.mip_interpolation),	Q | A | S,	BRTV_CONV_COPY},

	{BRT(DEPTH_TEST_T),		F(prim.depth_test),	Q | A | S,	BRTV_CONV_COPY},
	{BRT(BLEND_MODE_T),		F(prim.blend_mode),	Q | A | S,	BRTV_CONV_COPY},
	{BRT(MAP_WIDTH_LIMIT_T),F(prim.map_width_limit),	Q | A | S,	BRTV_CONV_COPY},
	{BRT(MAP_HEIGHT_LIMIT_T),F(prim.map_height_limit),	Q | A | S,	BRTV_CONV_COPY},
};

br_tv_template primitiveTemplate = {
	BR_ASIZE(primitiveTemplateEntries),
	primitiveTemplateEntries
};

static br_tv_template_entry outputTemplateEntries[] = {
	{BRT(COLOUR_BUFFER_O),	F(out.colour.pixelmap),	Q | A,		BRTV_CONV_COPY},
	{BRT(COLOUR_BUFFER_O),	F(out.colour),			S,			BRTV_CONV_CUSTOM,	P(customOutputConv)		},
	{BRT(DEPTH_BUFFER_O),	F(out.depth.pixelmap), 	Q | A,		BRTV_CONV_COPY},
	{BRT(DEPTH_BUFFER_O),	F(out.depth),			S,			BRTV_CONV_CUSTOM,	P(customOutputConv)		},
};

br_tv_template outputTemplate = {
	BR_ASIZE(outputTemplateEntries),
	outputTemplateEntries
};

static br_tv_template *findTemplate(br_token part)
{
	switch(part) {
	case BRT_PRIMITIVE:
		return &primitiveTemplate;
	case BRT_OUTPUT:
		return &outputTemplate;
	}

	return NULL;
}

static br_error BR_CMETHOD_DECL(br_primitive_state_3dfx, partSet)(
		struct br_primitive_state *self,
		br_token part,
		br_int_32 index,
		br_token t,
		br_uint_32 value)
{
	br_error r;
	br_tv_template *tp = findTemplate(part);
	br_uint_32 m;

	/*
	 * If the part was unknown, punt
	 */
	if(tp == NULL)
		return BRE_DEV_FAIL;

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

static br_error BR_CMETHOD_DECL(br_primitive_state_3dfx, partSetMany)(
		struct br_primitive_state *self,
		br_token part,
		br_int_32 index,
		br_token_value * tv,
		br_int_32 *pcount)
{
	br_error r;
	br_tv_template *tp = findTemplate(part);
	br_uint_32 m;
	br_int_32 c;

	/*
	 * If the part was unknown, punt
	 */
	if(tp == NULL)
		return BRE_DEV_FAIL;

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

static br_error BR_CMETHOD_DECL(br_primitive_state_3dfx, partQuery)(
		struct br_primitive_state *self,
		br_token part,
		br_int_32 index,
		br_uint_32 *pvalue,
		br_token t)
{
	br_tv_template *tp = findTemplate(part);

	if(tp == NULL)
		return BRE_DEV_FAIL;

	return BrTokenValueQuery(pvalue, NULL, NULL, t,  self, tp);
}

static br_error BR_CMETHOD_DECL(br_primitive_state_3dfx, partQueryBuffer)(
		struct br_primitive_state *self,
		br_token part,
		br_int_32 index,
		br_uint_32 *pvalue,
		br_uint_32 *buffer,
		br_size_t buffer_size,
		br_token t)
{
	br_tv_template *tp = findTemplate(part);

	if(tp == NULL)
		return BRE_DEV_FAIL;

	return BrTokenValueQuery(pvalue, buffer, buffer_size, t,  self, tp);
}

static br_error BR_CMETHOD_DECL(br_primitive_state_3dfx, partQueryMany)(
		struct br_primitive_state *self,
		br_token part,
		br_int_32 index,
		br_token_value *tv,
		void *extra,
		br_size_t extra_size,
		br_int_32 *pcount)
{
	br_tv_template *tp = findTemplate(part);

	if(tp == NULL)
		return BRE_DEV_FAIL;

	return BrTokenValueQueryMany(tv, extra, extra_size, pcount,  self, tp);
}

static br_error BR_CMETHOD_DECL(br_primitive_state_3dfx, partQueryManySize)(
		struct br_primitive_state *self,
		br_token part,
		br_int_32 index,
		br_size_t *pextra_size,
		br_token_value *tv)
{
	br_tv_template *tp = findTemplate(part);

	if(tp == NULL)
		return BRE_DEV_FAIL;

	return BrTokenValueQueryManySize(pextra_size, tv, self, tp);
}

static br_error BR_CMETHOD_DECL(br_primitive_state_3dfx, partQueryAll)(
		struct br_primitive_state *self,
		br_token part,
		br_int_32 index,
		br_token_value *buffer,
		br_size_t buffer_size)
{
	br_tv_template *tp = findTemplate(part);

	if(tp == NULL)
		return BRE_DEV_FAIL;

	return BrTokenValueQueryAll(buffer, buffer_size, self, tp);
}

static br_error BR_CMETHOD_DECL(br_primitive_state_3dfx, partQueryAllSize)(
		struct br_primitive_state *self,
		br_token part,
		br_int_32 index,
		br_size_t *psize)
{
	br_tv_template *tp = findTemplate(part);

	if(tp == NULL)
		return BRE_DEV_FAIL;

	return BrTokenValueQueryAllSize(psize, self, tp);
}


static br_error BR_CMETHOD_DECL(br_primitive_state_3dfx, partQueryCapability)(
		struct br_primitive_state *self,
		br_token part,
		br_int_32 index,
		br_token_value *buffer,
        br_size_t buffer_size)
{
    return BRE_FAIL;
}



static br_error BR_CMETHOD_DECL(br_primitive_state_3dfx, partIndexQuery)(
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
		return BRE_DEV_FAIL;
	}

}

static br_error BR_CMETHOD_DECL(br_primitive_state_3dfx, stateDefault)(
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
		self->prim.colour_map = NULL;
		self->prim.colour_type = BRT_DEFAULT;

		self->prim.map_antialiasing = BRT_NONE;
		self->prim.map_interpolation = BRT_NONE;
		self->prim.mip_interpolation = BRT_NONE;
		self->prim.fog_type = BRT_NONE;
		self->prim.fog_colour = 0xff0000;
		self->prim.fog_min = 0.0f;
		self->prim.fog_max = 1.0f;

		self->prim.depth_test = BRT_LESS_OR_EQUAL;
		self->prim.blend_mode = BRT_BLEND_STANDARD;
		self->prim.map_width_limit = BRT_WRAP;
		self->prim.map_height_limit = BRT_WRAP;

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



static br_error BR_CMETHOD_DECL(br_primitive_state_3dfx, stateQueryPerformance)(
		struct br_primitive_state *self,
        br_fixed_lu *speed)
{
    return BRE_FAIL;
}


static br_error BR_CMETHOD_DECL(br_primitive_state_3dfx, stateCopy)(
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


void PrimitiveState3DfxClearTemplates(void)
{
	CLEAR_TEMPLATE(primitiveState);
	CLEAR_TEMPLATE(output);
	CLEAR_TEMPLATE(primitive);
}

/*
 * Default dispatch table for device
 */
static struct br_primitive_state_dispatch primitiveStateDispatch = {
	NULL,
	NULL,
	NULL,
	NULL,
	BR_CMETHOD_REF(br_primitive_state_3dfx,	free),
	BR_CMETHOD_REF(br_object_3dfx,			identifier),
	BR_CMETHOD_REF(br_primitive_state_3dfx, type),
	BR_CMETHOD_REF(br_primitive_state_3dfx, isType),
	BR_CMETHOD_REF(br_object_3dfx, 			device),
	BR_CMETHOD_REF(br_primitive_state_3dfx, space),

	BR_CMETHOD_REF(br_primitive_state_3dfx,	templateQuery),
	BR_CMETHOD_REF(br_object,				query),
	BR_CMETHOD_REF(br_object, 				queryBuffer),
	BR_CMETHOD_REF(br_object, 				queryMany),
	BR_CMETHOD_REF(br_object, 				queryManySize),
	BR_CMETHOD_REF(br_object, 				queryAll),
	BR_CMETHOD_REF(br_object,	 			queryAllSize),

	BR_CMETHOD_REF(br_primitive_state_3dfx,	partSet),
	BR_CMETHOD_REF(br_primitive_state_3dfx,	partSetMany),
	BR_CMETHOD_REF(br_primitive_state_3dfx,	partQuery),
	BR_CMETHOD_REF(br_primitive_state_3dfx,	partQueryBuffer),
	BR_CMETHOD_REF(br_primitive_state_3dfx,	partQueryMany),
	BR_CMETHOD_REF(br_primitive_state_3dfx,	partQueryManySize),
	BR_CMETHOD_REF(br_primitive_state_3dfx,	partQueryAll),
	BR_CMETHOD_REF(br_primitive_state_3dfx,	partQueryAllSize),
	BR_CMETHOD_REF(br_primitive_state_3dfx,	partIndexQuery),
	BR_CMETHOD_REF(br_primitive_state_3dfx,	stateDefault),
	BR_CMETHOD_REF(br_primitive_state_3dfx,	stateCopy),
	BR_CMETHOD_REF(br_primitive_state_3dfx,	renderBegin),
	BR_CMETHOD_REF(br_primitive_state_3dfx,	renderEnd),
	BR_CMETHOD_REF(br_primitive_state_3dfx,	rangesQueryF),
	BR_CMETHOD_REF(br_primitive_state_3dfx,	rangesQueryX),
    BR_CMETHOD_REF(br_primitive_state_3dfx, partQueryCapability),
    BR_CMETHOD_REF(br_primitive_state_3dfx, stateQueryPerformance),
};

