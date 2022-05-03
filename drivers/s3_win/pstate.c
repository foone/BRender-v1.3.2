/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: pstate.c 1.1 1997/12/10 16:51:17 jon Exp $
 * $Locker: $
 *
 * Primitive state methods for S3 Windows driver
 */

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <windows.h>
#include <ddraw.h>

#include "drv.h"
#include "brassert.h"

BR_RCS_ID("$Id: pstate.c 1.1 1997/12/10 16:51:17 jon Exp $");


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

#define F(f)	offsetof(struct br_primitive_state, f)

static struct br_tv_template_entry primitiveStateTemplateEntries[] = {
	{BRT(IDENTIFIER_CSTR),		F(identifier),		Q | A,	BRTV_CONV_COPY, },
	{BRT(PARTS_TL),				(size_t)&PrimPartsTokensS3,		Q | A | BRTV_ABS,	BRTV_CONV_LIST, },
};
#undef F

static struct br_tv_template primitiveStateTemplate = {
	BR_ASIZE(primitiveStateTemplateEntries),
	primitiveStateTemplateEntries
};


/*
 * Set up a static device object
 */
struct br_primitive_state * PrimitiveStateHardS3Allocate(struct br_primitive_library *plib)
{
	struct br_primitive_state * self;
	void PrimitiveResetTemplates(void);
	void StoredBufferReset(void);

    OUTDBS("PrimitiveStateHardS3Allocate\n");
    CLEAR_TEMPLATE(primitiveState);
    PrimitiveResetTemplates();
    StoredBufferReset();
    ResetS3RendererHeldStates();

	self = BrResAllocate(DRIVER_RESOURCE, sizeof(*self), BR_MEMORY_OBJECT);

	
	if(self == NULL)
		return NULL;

	self->plib = plib;
	self->dispatch = &primitiveStateDispatch;

   self->prim.map_antialiasing = BRT_NONE;
   self->prim.map_interpolation = BRT_NONE;
   self->prim.mip_interpolation = BRT_NONE;
   self->prim.fog_type = BRT_NONE;
   self->prim.fog_colour = 0;
   self->prim.fog_min = 0.0f;
   self->prim.fog_max = 255.0f;
   self->prim.alpha_val = 0.0f;

   /*
    * Setup initial state
    */
   S3InitConstantRanges(self);

   ObjectContainerAddFront(plib,(br_object *)self);

	return self;
}

static void BR_CMETHOD_DECL(br_primitive_state_hardS3, free)(br_primitive_state *self)
{
   OUTDBS("Primitive state free\n");
   ObjectContainerRemove(self->plib,(br_object*)self);
	BrResFreeNoCallback(self);
}

static br_token BR_CMETHOD_DECL(br_primitive_state_hardS3, type)(br_primitive_state *self)
{
	return BRT_PRIMITIVE_STATE;
}

static br_boolean BR_CMETHOD_DECL(br_primitive_state_hardS3, isType)(br_primitive_state *self, br_token t)
{
	return (t == BRT_PRIMITIVE_STATE) || (t == BRT_OBJECT_CONTAINER) || (t == BRT_OBJECT);
}

static br_int_32 BR_CMETHOD_DECL(br_primitive_state_hardS3, space)(br_primitive_state *self)
{
	return sizeof(br_primitive_state);
}

static struct br_tv_template * BR_CMETHOD_DECL(br_primitive_state_hardS3,templateQuery)
	(br_primitive_state *self)
{
	primitiveStateTemplate.res = DRIVER_RESOURCE;
	return &primitiveStateTemplate;
}


/* Function for setting output buffer - returns TRUE if CACHE has been invalidated */
static br_boolean outputSet(struct output_buffer *ob, br_device_pixelmap *new)
{
	br_device_pixelmap *old = ob->pixelmap;

   OUTDBS("OutputSet\n");

   /*
	 * Special case of same, NULL value
	 */
	if(old == NULL && new == NULL)
		return BR_FALSE;
	
	/*
	 * If pixelmap is no longer being used
	 */
	if(new == NULL)
      {
		ob->pixelmap = NULL;
      ob->width = 0;
      ob->height = 0;
		return BR_TRUE;
	   }
	
	if(old != NULL)
      {
		/*
		 * See if new pixelmap has same character as old...
		 */
		if(ob->width == new->pm_width && ob->height == new->pm_height)
         {
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
   ob->type = new->pm_type;

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

static struct br_tv_custom customOutputConv = {
	NULL,
	customOutputSet,
	NULL,
};

/*
 * Templates for state set/query
 */
#define F(f) offsetof(struct br_primitive_state, f)
#define P(f) ((br_uint_32)(&(f)))

static br_tv_template_entry primitiveTemplateEntries[] = {
	{BRT(SMOOTH_B),			   F(prim.flags),			Q | S | A,	BRTV_CONV_BIT,	PRIMF_SMOOTH,		 1},
	{BRT(PERSPECTIVE_B),	      F(prim.flags),			Q | S | A,	BRTV_CONV_BIT,	PRIMF_PERSPECTIVE, 1},
	{BRT(BLEND_B),				   F(prim.flags),			Q | S | A,	BRTV_CONV_BIT,	PRIMF_BLEND,		 1},
	{BRT(DECAL_B),			      F(prim.flags),			Q | S | A,	BRTV_CONV_BIT,	PRIMF_DECAL,		 1},
	{BRT(DEPTH_WRITE_B),	      F(prim.flags),			Q | S | A,	BRTV_CONV_BIT,	PRIMF_DEPTH_WRITE, 1},
	{BRT(COLOUR_WRITE_B),	   F(prim.flags),			Q | S | A,	BRTV_CONV_BIT,	PRIMF_COLOUR_WRITE,1},
	{BRT(MODULATE_B), 		   F(prim.flags),       Q | S | A,	BRTV_CONV_BIT, PRIMF_MODULATE,    1},

	{BRT(INDEX_BASE_I32),      F(prim.index_base),	Q | S | A,  BRTV_CONV_COPY},
	{BRT(INDEX_RANGE_I32),     F(prim.index_range),	Q | S | A,  BRTV_CONV_COPY},

	{BRT(COLOUR_MAP_O),	      F(prim.colour_map),  Q | S | A,	BRTV_CONV_COPY},
	{BRT(TEXTURE_O),		      F(prim.colour_map),  Q | S | A,  BRTV_CONV_COPY},

	{BRT(MAP_ANTIALIASING_T), 	F(prim.map_antialiasing), Q | A | S,	BRTV_CONV_COPY},
	{BRT(MAP_INTERPOLATION_T),	F(prim.map_interpolation),Q | A | S,	BRTV_CONV_COPY},
	{BRT(MIP_INTERPOLATION_T),	F(prim.mip_interpolation),Q | A | S,	BRTV_CONV_COPY},

	{BRT(FOG_T),			      F(prim.fog_type),		Q | A | S,	BRTV_CONV_COPY},
	{BRT(FOG_RGB),			      F(prim.fog_colour),	Q | A | S,	BRTV_CONV_COPY},
	{BRT(FOG_MIN_X),			   F(prim.fog_min),		Q | A | S,	BRTV_CONV_FIXED_FLOAT},
	{BRT(FOG_MAX_X),			   F(prim.fog_max),		Q | A | S,	BRTV_CONV_FIXED_FLOAT},
	{BRT(FOG_MIN_F),			   F(prim.fog_min),		Q | A | S,	BRTV_CONV_COPY},
	{BRT(FOG_MAX_F),			   F(prim.fog_max),		Q | A | S,	BRTV_CONV_COPY},
	{BRT(OPACITY_X),			   F(prim.alpha_val),   Q | A | S,	BRTV_CONV_FIXED_FLOAT},
	{BRT(OPACITY_F),			   F(prim.alpha_val),   Q | A | S,	BRTV_CONV_COPY},
};




static br_tv_template primitiveTemplate = {
	BR_ASIZE(primitiveTemplateEntries),
	primitiveTemplateEntries
};


static br_tv_template_entry outputTemplateEntries[] = {
	{BRT(COLOUR_BUFFER_O),	F(out.colour.pixelmap),	Q | A,		BRTV_CONV_COPY},
	{BRT(COLOUR_BUFFER_O),	F(out.colour),			   S,			   BRTV_CONV_CUSTOM,	P(customOutputConv)},
	{BRT(DEPTH_BUFFER_O),	F(out.depth.pixelmap), 	Q | A,		BRTV_CONV_COPY},
	{BRT(DEPTH_BUFFER_O),	F(out.depth),			   S,			   BRTV_CONV_CUSTOM,	P(customOutputConv)},
};


static br_tv_template outputTemplate = {
	BR_ASIZE(outputTemplateEntries),
	outputTemplateEntries
};



/*
 * Hmmmm
 */
void PrimitiveResetTemplates(void)
{
  CLEAR_TEMPLATE(output);
  CLEAR_TEMPLATE(primitive);
}



void ClearPstateTemplates(void)
{
	CLEAR_TEMPLATE(primitiveState);
	CLEAR_TEMPLATE(output);
	CLEAR_TEMPLATE(primitive);
}


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

static br_error BR_CMETHOD_DECL(br_primitive_state_hardS3, partSet)(
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
   switch(part)
      {
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

static br_error BR_CMETHOD_DECL(br_primitive_state_hardS3, partSetMany)(
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
   switch(part)
      {
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

static br_error BR_CMETHOD_DECL(br_primitive_state_hardS3, partQuery)(
		struct br_primitive_state *self,
		br_token part,
		br_int_32 index,
		br_uint_32 *pvalue,
		br_token t)
{
	br_tv_template *tp = findTemplate(part);

	if(tp == NULL)
		return BRE_FAIL;

	return BrTokenValueQuery(pvalue, NULL, NULL, t,  self, tp);
}

static br_error BR_CMETHOD_DECL(br_primitive_state_hardS3, partQueryBuffer)(
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
		return BRE_FAIL;

	return BrTokenValueQuery(pvalue, buffer, buffer_size, t,  self, tp);
}

static br_error BR_CMETHOD_DECL(br_primitive_state_hardS3, partQueryMany)(
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
		return BRE_FAIL;

	return BrTokenValueQueryMany(tv, extra, extra_size, pcount,  self, tp);
}

static br_error BR_CMETHOD_DECL(br_primitive_state_hardS3, partQueryManySize)(
		struct br_primitive_state *self,
		br_token part,
		br_int_32 index,
		br_size_t *pextra_size,
		br_token_value *tv)
{
	br_tv_template *tp = findTemplate(part);

	if(tp == NULL)
		return BRE_FAIL;

	return BrTokenValueQueryManySize(pextra_size, tv, self, tp);
}

static br_error BR_CMETHOD_DECL(br_primitive_state_hardS3, partQueryAll)(
		struct br_primitive_state *self,
		br_token part,
		br_int_32 index,
		br_token_value *buffer,
		br_size_t buffer_size)
{
	br_tv_template *tp = findTemplate(part);

	if(tp == NULL)
		return BRE_FAIL;

	return BrTokenValueQueryAll(buffer, buffer_size, self, tp);
}

static br_error BR_CMETHOD_DECL(br_primitive_state_hardS3, partQueryAllSize)(
		struct br_primitive_state *self,
		br_token part,
		br_int_32 index,
		br_size_t *psize)
{
	br_tv_template *tp = findTemplate(part);

	if(tp == NULL)
		return BRE_FAIL;

	return BrTokenValueQueryAllSize(psize, self, tp);
}



static br_error BR_CMETHOD_DECL(br_primitive_state_hardS3, partQueryCapability)(
      struct br_primitive_state *self,
      br_token part,
      br_int_32 index,
      br_token_value *buffer,
        br_size_t buffer_size)
{
    return BRE_FAIL;
}





static br_error BR_CMETHOD_DECL(br_primitive_state_hardS3, partIndexQuery)(
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





static br_error BR_CMETHOD_DECL(br_primitive_state_hardS3, partDefault)(
		struct br_primitive_state *self,
		br_uint_32 mask)
{
   OUTDBS("State - PartDefault\n");
   /*
    * Clear everything down
    *
    * Could use inputSet and OutputSet?
    */
   if(mask & MASK_STATE_PRIMITIVE)
      {
      self->prim.flags = PRIMF_COLOUR_WRITE | PRIMF_DEPTH_WRITE;
      self->prim.colour_map = NULL;
      self->prim.colour_type = BRT_DEFAULT;

      self->prim.timestamp = Timestamp();
      self->prim.timestamp_major = Timestamp();
      }

   if(mask & MASK_STATE_OUTPUT)
      {
      self->out.colour.pixelmap = NULL;
      self->out.depth.pixelmap = NULL;

      self->out.timestamp = Timestamp();
      self->out.timestamp_major = Timestamp();
      }

	return BRE_OK;
}





static br_error BR_CMETHOD_DECL(br_primitive_state_hardS3, stateQueryPerformance(
      struct br_primitive_state *self,
        br_fixed_lu *speed))
{
    return BRE_FAIL;
}



static br_error BR_CMETHOD_DECL(br_primitive_state_hardS3, stateCopy)(
		struct br_primitive_state *self,
		struct br_primitive_state *source,
		br_uint_32 mask)
{

   OUTDBS("State Copy\n");

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
static struct br_primitive_state_dispatch primitiveStateDispatch = {
   NULL,NULL,NULL,NULL,
	BR_CMETHOD_REF(br_primitive_state_hardS3,	free),
	BR_CMETHOD_REF(br_object_s3,			identifier),
	BR_CMETHOD_REF(br_primitive_state_hardS3, type),
	BR_CMETHOD_REF(br_primitive_state_hardS3, isType),
	BR_CMETHOD_REF(br_object_s3, 			device),
	BR_CMETHOD_REF(br_primitive_state_hardS3, space),

	BR_CMETHOD_REF(br_primitive_state_hardS3,	templateQuery),
	BR_CMETHOD_REF(br_object,				query),
	BR_CMETHOD_REF(br_object, 				queryBuffer),
	BR_CMETHOD_REF(br_object, 				queryMany),
	BR_CMETHOD_REF(br_object, 				queryManySize),
	BR_CMETHOD_REF(br_object, 				queryAll),
	BR_CMETHOD_REF(br_object,	 			queryAllSize),

	BR_CMETHOD_REF(br_primitive_state_hardS3,	partSet),
	BR_CMETHOD_REF(br_primitive_state_hardS3,	partSetMany),
	BR_CMETHOD_REF(br_primitive_state_hardS3,	partQuery),
	BR_CMETHOD_REF(br_primitive_state_hardS3,	partQueryBuffer),
	BR_CMETHOD_REF(br_primitive_state_hardS3,	partQueryMany),
	BR_CMETHOD_REF(br_primitive_state_hardS3,	partQueryManySize),
	BR_CMETHOD_REF(br_primitive_state_hardS3,	partQueryAll),
	BR_CMETHOD_REF(br_primitive_state_hardS3,	partQueryAllSize),
	BR_CMETHOD_REF(br_primitive_state_hardS3,	partIndexQuery),
	BR_CMETHOD_REF(br_primitive_state_hardS3,	partDefault),
	BR_CMETHOD_REF(br_primitive_state_hardS3,	stateCopy),
	BR_CMETHOD_REF(br_primitive_state_hardS3,	renderBegin),
	BR_CMETHOD_REF(br_primitive_state_hardS3,	renderEnd),
	BR_CMETHOD_REF(br_primitive_state_hardS3,	rangesQueryF),
	BR_CMETHOD_REF(br_primitive_state_hardS3,	rangesQueryX),
   BR_CMETHOD_REF(br_primitive_state_hardS3, partQueryCapability),
   BR_CMETHOD_REF(br_primitive_state_hardS3, stateQueryPerformance),
};


