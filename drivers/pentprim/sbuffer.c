/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: sbuffer.c 1.2 1998/10/21 15:42:23 jon Exp $
 * $Locker: $
 *
 * Stored buffer methods
 */
#include <stddef.h>
#include <string.h>

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: sbuffer.c 1.2 1998/10/21 15:42:23 jon Exp $");

/*
 * Default dispatch table for primitive state (defined at end of file)
 */
static const struct br_buffer_stored_dispatch bufferStoredDispatch;

/*
 * Primitive state info. template
 */
#define F(f)	offsetof(struct br_buffer_stored, f)

static const struct br_tv_template_entry bufferStoredTemplateEntries[] = {
	{BRT_IDENTIFIER_CSTR,	0,	F(identifier),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
};
#undef F

/*
 * Simple log2 function
 */
static br_uint_8 findShift(br_int_32 x)
{
	br_uint_8 b;

	for(b= -1; x; b++)
		x /= 2;

	return b;
}

/*
 * Build a render_buffer structure from a pixelmap
 */
void SetupRenderBuffer(struct render_buffer *rb, br_device_pixelmap *pm)
{
	int bpp = 1;

	switch(pm->pm_type) {
	case BR_PMT_RGB_555:
	case BR_PMT_RGB_565:
	case BR_PMT_DEPTH_16:
		bpp = 2;
		break;

	case BR_PMT_RGB_888:
		bpp = 3;
		break;

	case BR_PMT_RGBX_888:
	case BR_PMT_RGBA_8888:
		bpp = 4;
		break;
	}

	rb->type = pm->pm_type;
	rb->bpp = bpp;
	rb->width_b = pm->pm_width * bpp;
	rb->width_p = pm->pm_width;
	rb->height = pm->pm_height;
	rb->stride_b = pm->pm_row_bytes;
	rb->stride_p = pm->pm_row_bytes / bpp;
	rb->size = pm->pm_height * pm->pm_row_bytes;

	rb->base = (char *)(pm->pm_pixels)+ 
		pm->pm_base_y * pm->pm_row_bytes +
		pm->pm_base_x * bpp;

	rb->sel = (br_uint_16)pm->pm_pixels_qualifier;

	/*
	 * See if there is a valid palette attached
	 */
	if(pm->pm_map &&
		(pm->pm_map->type == BR_PMT_RGBX_888) &&
		pm->pm_map->width == 1 &&
		pm->pm_map->row_bytes == 4) {
		rb->palette_size = pm->pm_map->height;
		rb->palette = pm->pm_map->pixels;
	} else {
		rb->palette_size = 0;
		rb->palette = NULL;
	}

	/*
	 * Generate tiling texture info
	 */
	rb->width_s = findShift(rb->width_p);
	rb->height_s = findShift(rb->height);

#if 0
	if(pm->pm_flags & BR_PMF_TILED)
		rb->tile_s = BR_PM_TILE_SIZE;
	else
#endif
		rb->tile_s = 0;

}

/*
 * Set up a static device object
 */
struct br_buffer_stored * BufferStoredSoftAllocate(struct br_primitive_library *plib,
	br_token use, struct br_device_pixelmap *pm, br_token_value *tv)
{
	struct br_buffer_stored * self;
	char *ident;

	switch(use) {
	case BRT_TEXTURE_O:
	case BRT_COLOUR_MAP_O:
		ident ="Colour-Map";
		break;
	case BRT_INDEX_SHADE_O:
		ident ="Shade-Table";
		break;
	case BRT_INDEX_BLEND_O:
		ident ="Blend-Table";
		break;
	case BRT_SCREEN_DOOR_O:
		ident ="Screendoor-Table";
		break;
	case BRT_INDEX_LIGHT_O:
		ident ="Lighting-Table";
		break;
	case BRT_BUMP_O:
		ident ="Bump-Map";
		break;

	case BRT_UNKNOWN:
		ident ="Unknown";
		break;

	default:
		return NULL;
	}

	self = BrResAllocate(plib->device, sizeof(*self), BR_MEMORY_OBJECT);

	if(self == NULL)
		return NULL;

	self->dispatch = &bufferStoredDispatch;
	self->identifier = ident;
    self->device = plib->device;
	self->plib = plib;

	/*
	 * If caller does not garuantee to keep source data around, or
	 * source data is not memory mapped, then clone an in-memory copy
	 */
#if 0
	if() {
	} else {
	}
#endif

	self->flags |= SBUFF_SHARED;
	SetupRenderBuffer(&self->buffer, pm);

	ObjectContainerAddFront(plib,(br_object *)self);

	return self;
}

static br_error BR_CMETHOD_DECL(br_buffer_stored_soft, update)(
	struct br_buffer_stored *self,
	struct br_device_pixelmap *pm,
	br_token_value *tv)
{
	
	SetupRenderBuffer(&self->buffer,pm);

	return BRE_OK;
}

static void BR_CMETHOD_DECL(br_buffer_stored_soft, free)(br_buffer_stored *self)
{
	ObjectContainerRemove(self->plib, (br_object *)self);

    BrResFreeNoCallback(self);
}

static br_token BR_CMETHOD_DECL(br_buffer_stored_soft, type)(br_buffer_stored *self)
{
	return BRT_BUFFER_STORED;
}

static br_boolean BR_CMETHOD_DECL(br_buffer_stored_soft, isType)(br_buffer_stored *self, br_token t)
{
	return (t == BRT_BUFFER_STORED) || (t == BRT_OBJECT);
}

static br_int_32 BR_CMETHOD_DECL(br_buffer_stored_soft, space)(br_buffer_stored *self)
{
	return BrResSizeTotal(self);
}

static struct br_tv_template * BR_CMETHOD_DECL(br_buffer_stored_soft,templateQuery)
	(br_buffer_stored *self)
{
    if(self->device->templates.bufferStoredTemplate == NULL)
        self->device->templates.bufferStoredTemplate = BrTVTemplateAllocate(self->device,
            bufferStoredTemplateEntries, BR_ASIZE(bufferStoredTemplateEntries));

    return self->device->templates.bufferStoredTemplate;
}

/*
 * Default dispatch table for device
 */
static const struct br_buffer_stored_dispatch bufferStoredDispatch = {
	NULL,
	NULL,
	NULL,
	NULL,
	BR_CMETHOD_REF(br_buffer_stored_soft,	free),
	BR_CMETHOD_REF(br_object_softprim,		identifier),
	BR_CMETHOD_REF(br_buffer_stored_soft,   type),
	BR_CMETHOD_REF(br_buffer_stored_soft,   isType),
	BR_CMETHOD_REF(br_object_softprim,		device),
	BR_CMETHOD_REF(br_buffer_stored_soft,   space),

	BR_CMETHOD_REF(br_buffer_stored_soft,	templateQuery),
	BR_CMETHOD_REF(br_object,				query),
	BR_CMETHOD_REF(br_object, 				queryBuffer),
	BR_CMETHOD_REF(br_object, 				queryMany),
	BR_CMETHOD_REF(br_object, 				queryManySize),
	BR_CMETHOD_REF(br_object, 				queryAll),
	BR_CMETHOD_REF(br_object,	 			queryAllSize),

	BR_CMETHOD_REF(br_buffer_stored_soft,	update),
};

