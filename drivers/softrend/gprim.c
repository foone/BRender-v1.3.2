/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: gprim.c 1.1 1997/12/10 16:51:59 jon Exp $
 * $Locker: $
 *
 * Geometry format for primitives
 */
#include <stddef.h>
#include <string.h>

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: gprim.c 1.1 1997/12/10 16:51:59 jon Exp $");

/*
 * Default dispatch table for renderer type (defined at and of file)
 */								 
static const struct br_geometry_primitives_dispatch geometryPrimitivesDispatch;

/*
 * Geometry format info. template
 */
#define F(f)	offsetof(struct br_geometry_primitives, f)

static const struct br_tv_template_entry geometryPrimitivesTemplateEntries[] = {
	{BRT_IDENTIFIER_CSTR,		0,	F(identifier),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT_RENDERER_FACILITY_O,	0,	F(renderer_facility),	BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT_FACILITY_O,			0,	F(renderer_facility),	BRTV_QUERY,				BRTV_CONV_COPY, },
};
#undef F

/*
 * Allocate a geometry format
 */
br_geometry_primitives * GeometryPrimitivesAllocate(br_renderer_facility *type, char *id)
{
	br_geometry_primitives * self;

	self = BrResAllocate(type->device, sizeof(*self), BR_MEMORY_OBJECT_DATA);

	if(self == NULL)
		return NULL;

	self->dispatch = (struct br_geometry_primitives_dispatch *)&geometryPrimitivesDispatch;
 	self->identifier = id;
 	self->renderer_facility = type;
    self->device = type->device;

	ObjectContainerAddFront(type, (br_object *)self);
	return self;
}

static void BR_CMETHOD_DECL(br_geometry_primitives_soft, free)(br_geometry_primitives *self)
{
	ObjectContainerRemove(self->renderer_facility, (br_object *)self);

	BrResFreeNoCallback(self);
}

static br_token BR_CMETHOD_DECL(br_geometry_primitives_soft, type)(br_geometry_primitives *self)
{
	return BRT_GEOMETRY_PRIMITIVES;
}

static br_boolean BR_CMETHOD_DECL(br_geometry_primitives_soft, isType)(br_geometry_primitives *self, br_token t)
{
	return (t == BRT_GEOMETRY_PRIMITIVES) || (t == BRT_GEOMETRY) || (t == BRT_OBJECT);
}

static br_int_32 BR_CMETHOD_DECL(br_geometry_primitives_soft, space)(br_geometry_primitives *self)
{
	return sizeof(br_geometry_primitives);
}

static struct br_tv_template * BR_CMETHOD_DECL(br_geometry_primitives_soft, templateQuery)
	(br_geometry_primitives *self)
{
    if(self->device->templates.geometryPrimitivesTemplate == NULL)
       self->device->templates.geometryPrimitivesTemplate = BrTVTemplateAllocate(self->device,
        (br_tv_template_entry *)geometryPrimitivesTemplateEntries,
		BR_ASIZE(geometryPrimitivesTemplateEntries));

    return self->device->templates.geometryPrimitivesTemplate;
}

static br_error BR_CMETHOD_DECL(br_geometry_primitives_soft, storedAvail)(
		struct br_geometry *self,
		br_int_32 *psize,
		br_token_value *tv)
{
	return BRE_FAIL;
}

br_error BR_CMETHOD_DECL(br_geometry_primitives_soft, render)
		(struct br_geometry *self, struct br_renderer *renderer, struct fmt_vertex *vertices, int nvertices, br_token type)
{
	return BRE_FAIL;
}

br_error BR_CMETHOD_DECL(br_geometry_primitives_soft, renderOnScreen)
		(struct br_geometry *self, struct br_renderer *renderer, struct fmt_vertex *vertices, int nvertices, br_token type)
{
	return BRE_FAIL;
}

br_error BR_CMETHOD_DECL(br_geometry_primitives_soft, storedNew)
		(struct br_geometry *self, struct br_renderer *renderer,
		struct br_geometry_stored **psg, struct fmt_vertex *vertices, int nvertices, br_token type, br_token_value *tv)
{
	return BRE_FAIL;
}

#if BASED_FIXED
br_error BR_CMETHOD_DECL(br_geometry_primitives_soft, renderFloatToFixed)
		(struct br_geometry *self, struct br_renderer *renderer, struct fmt_vertex *vertices, int nvertices, br_token type)
{
	return BRE_FAIL;
}

br_error BR_CMETHOD_DECL(br_geometry_primitives_soft, renderOnScreenFloatToFixed)
		(struct br_geometry *self, struct br_renderer *renderer, struct fmt_vertex *vertices, int nvertices, br_token type)
{
	return BRE_FAIL;
}

br_error BR_CMETHOD_DECL(br_geometry_primitives_soft, storedNewFloatToFixed)
		(struct br_geometry *self, struct br_renderer *renderer,
		struct br_geometry_stored **psg, struct fmt_vertex *vertices, int nvertices, br_token type, br_token_value *tv)
{
	return BRE_FAIL;
}
#endif

#if BASED_FLOAT
br_error BR_CMETHOD_DECL(br_geometry_primitives_soft, renderFixedToFloat)
		(struct br_geometry *self, struct br_renderer *renderer, struct fmt_vertex *vertices, int nvertices, br_token type)
{
	return BRE_FAIL;
}

br_error BR_CMETHOD_DECL(br_geometry_primitives_soft, renderOnScreenFixedToFloat)
		(struct br_geometry *self, struct br_renderer *renderer, struct fmt_vertex *vertices, int nvertices, br_token type)
{
	return BRE_FAIL;
}

br_error BR_CMETHOD_DECL(br_geometry_primitives_soft, storedNewFixedToFloat)
		(struct br_geometry *self, struct br_renderer *renderer,
		struct br_geometry_stored **psg, struct fmt_vertex *vertices, int nvertices, br_token type, br_token_value *tv)
{
	return BRE_FAIL;
}
#endif

/*
 * Default dispatch table for renderer type
 */
static const struct br_geometry_primitives_dispatch geometryPrimitivesDispatch = {
	NULL,
	NULL,
	NULL,
	NULL,
	BR_CMETHOD_REF(br_geometry_primitives_soft,	free),
	BR_CMETHOD_REF(br_object_soft,				identifier),
	BR_CMETHOD_REF(br_geometry_primitives_soft,	type),
	BR_CMETHOD_REF(br_geometry_primitives_soft,	isType),
	BR_CMETHOD_REF(br_object_soft,				device),
	BR_CMETHOD_REF(br_geometry_primitives_soft,	space),

	BR_CMETHOD_REF(br_geometry_primitives_soft,	templateQuery),
	BR_CMETHOD_REF(br_object,					query),
	BR_CMETHOD_REF(br_object,					queryBuffer),
	BR_CMETHOD_REF(br_object,					queryMany),
	BR_CMETHOD_REF(br_object,					queryManySize),
	BR_CMETHOD_REF(br_object,					queryAll),
	BR_CMETHOD_REF(br_object,					queryAllSize),

#if BASED_FIXED
	BR_CMETHOD_REF(br_geometry_primitives_soft,	renderFloatToFixed),
	BR_CMETHOD_REF(br_geometry_primitives_soft,	render),
	BR_CMETHOD_REF(br_geometry_primitives_soft,	renderOnScreenFloatToFixed),
	BR_CMETHOD_REF(br_geometry_primitives_soft,	renderOnScreen),
	BR_CMETHOD_REF(br_geometry_primitives_soft,	storedNewFloatToFixed),
	BR_CMETHOD_REF(br_geometry_primitives_soft,	storedNew),
#endif

#if BASED_FLOAT
	BR_CMETHOD_REF(br_geometry_primitives_soft,	render),
	BR_CMETHOD_REF(br_geometry_primitives_soft,	renderFixedToFloat),
	BR_CMETHOD_REF(br_geometry_primitives_soft,	renderOnScreen),
	BR_CMETHOD_REF(br_geometry_primitives_soft,	renderOnScreenFixedToFloat),
	BR_CMETHOD_REF(br_geometry_primitives_soft,	storedNew),
	BR_CMETHOD_REF(br_geometry_primitives_soft,	storedNewFixedToFloat),
#endif

	BR_CMETHOD_REF(br_geometry_primitives_soft,	storedAvail),
};

