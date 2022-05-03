/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * Stored buffer methods
 */
#include <stddef.h>
#include <string.h>

#include "texture.h"

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"


/*
 * Check if a number of a power of 2
 */
static br_boolean isPowerOf2(br_int_32 x)
{
	return !((x-1) & x);
}

static pixelmapBPP(br_device_pixelmap *pm)
{
	switch(pm->pm_type) {
	default:
		return 1;
	case BR_PMT_RGB_555:
	case BR_PMT_RGB_565:
	case BR_PMT_DEPTH_16:
	case BR_PMT_RGBA_4444:
		return 2;

	case BR_PMT_RGB_888:
		return 3;

	case BR_PMT_RGBX_888:
	case BR_PMT_RGBA_8888:
		return 4;
	}
}

/*
 * Default dispatch table for primitive state (defined at end of file)
 */
static struct br_buffer_stored_dispatch bufferStoredDispatch;

/*
 * Primitive state info. template
 */
#define F(f)	offsetof(struct br_buffer_stored, f)

static struct br_tv_template_entry bufferStoredTemplateEntries[] = {
	{BRT_IDENTIFIER_CSTR,	0,	F(identifier),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
};
#undef F

static struct br_tv_template bufferStoredTemplate = {
	BR_ASIZE(bufferStoredTemplateEntries),
	bufferStoredTemplateEntries
};



void BufferStored3DfxClearTemplate(void)
{
	CLEAR_TEMPLATE(bufferStored);
}



/*
 */
br_error Setup3DfxBuffer(br_buffer_stored *buffer, br_device_pixelmap *pm)
{
	br_colour *map;
	br_uint_32 large_dimension;

	/* Any failure inside here will make the texture uncacheable and unrenderable */
	buffer->buffer.uncacheable = BR_TRUE;

	/*
	 * Inspect the texture dimensions
	 */
	if(pm->pm_width > 256 || pm->pm_height > 256)
        return BRE_DEV_UNSUPPORTED;

	if(!isPowerOf2(pm->pm_width) || !isPowerOf2(pm->pm_height))
        return BRE_DEV_UNSUPPORTED;
	
	if(pm->pm_width >= pm->pm_height) {
		large_dimension = pm->pm_width;
		switch(pm->pm_width / pm->pm_height) {
		case 8:
            buffer->buffer.info.aspectRatio = GR_ASPECT_8x1;
			buffer->buffer.u_range = BR_SCALAR(256.0);
			buffer->buffer.v_range = BR_SCALAR( 32.0);
			break;
		case 4:
            buffer->buffer.info.aspectRatio = GR_ASPECT_4x1;
			buffer->buffer.u_range = BR_SCALAR(256.0);
			buffer->buffer.v_range = BR_SCALAR( 64.0);
			break;
		case 2:
            buffer->buffer.info.aspectRatio = GR_ASPECT_2x1;
			buffer->buffer.u_range = BR_SCALAR(256.0);
			buffer->buffer.v_range = BR_SCALAR(128.0);
			break;

		case 1:
            buffer->buffer.info.aspectRatio = GR_ASPECT_1x1;
			buffer->buffer.u_range = BR_SCALAR(256.0);
			buffer->buffer.v_range = BR_SCALAR(256.0);
			break;
		default:
            return BRE_DEV_UNSUPPORTED;
		}
	} else {
		large_dimension = pm->pm_height;
		switch(pm->pm_height / pm->pm_width) {
		case 8:
            buffer->buffer.info.aspectRatio = GR_ASPECT_1x8;
			buffer->buffer.u_range = BR_SCALAR( 32.0);
			buffer->buffer.v_range = BR_SCALAR(256.0);
			break;
		case 4:
            buffer->buffer.info.aspectRatio = GR_ASPECT_1x4;
			buffer->buffer.u_range = BR_SCALAR( 64.0);
			buffer->buffer.v_range = BR_SCALAR(256.0);
			break;
		case 2:
            buffer->buffer.info.aspectRatio = GR_ASPECT_1x2;
			buffer->buffer.u_range = BR_SCALAR(128.0);
			buffer->buffer.v_range = BR_SCALAR(256.0);
			break;
		default:
            return BRE_DEV_UNSUPPORTED;
		}
	}


	switch(large_dimension) {
	case 256:
        buffer->buffer.info.largeLod = GR_LOD_256;
		break;
	case 128:
        buffer->buffer.info.largeLod = GR_LOD_128;
		break;
	case 64:
        buffer->buffer.info.largeLod = GR_LOD_64;
		break;
	case 32:
        buffer->buffer.info.largeLod = GR_LOD_32;
		break;
	case 16:
        buffer->buffer.info.largeLod = GR_LOD_16;
		break;
	case 8:
        buffer->buffer.info.largeLod = GR_LOD_8;
		break;
	case 4:
        buffer->buffer.info.largeLod = GR_LOD_4;
		break;
	case 2:
        buffer->buffer.info.largeLod = GR_LOD_2;
		break;
	case 1:
        buffer->buffer.info.largeLod = GR_LOD_1;
		break;
	}

    buffer->buffer.width_p = pm->pm_width;
    buffer->buffer.width_b = pm->pm_width * pixelmapBPP(pm);
    buffer->buffer.height = pm->pm_height;

	/* Check for mipmapping */
	if (pm->pm_mip_offset)
		buffer->buffer.info.smallLod = GR_LOD_1;
	else
		buffer->buffer.info.smallLod = buffer->buffer.info.largeLod;

	/* Resolve pointer to source */
    buffer->pixels_pointer = (char *) (pm->pm_pixels) +
                             pm->pm_base_y * pm->pm_row_bytes +
                             pm->pm_base_x * pixelmapBPP(pm);

    /* Only paletted textures carry a palette */
    buffer->buffer.palette_pointer = NULL;

	switch(pm->pm_type) {
	case BR_PMT_INDEX_8:
        buffer->buffer.info.format = GR_TEXFMT_P_8;

        if (pm->pm_map)
			map = pm->pm_map->pixels;
		else
			map = ObjectDevice(buffer)->clut->entries;

        /* Store palette for use when rendering */
        buffer->buffer.palette_pointer = map;
		buffer->buffer.blended = BR_FALSE;
		break;

	case BR_PMT_RGB_555:
		buffer->buffer.info.format = GR_TEXFMT_ARGB_1555;
		buffer->buffer.blended = BR_FALSE;
		break;

	case BR_PMT_RGB_565:
		buffer->buffer.info.format = GR_TEXFMT_RGB_565;
		buffer->buffer.blended = BR_FALSE;
		break;

	case BR_PMT_RGBA_4444:
		buffer->buffer.info.format = GR_TEXFMT_ARGB_4444;
		buffer->buffer.blended = BR_TRUE;
		break;

		/* The 3Dfx doesn't support 24-bit textures */
	case BR_PMT_RGB_888:
	case BR_PMT_RGBX_888:
	case BR_PMT_RGBA_8888:
	default:
		/*
         * Unknown source
		 */
        return(BRE_DEV_UNSUPPORTED);
	}

	/* Set start address */
	buffer->buffer.info.data = buffer->pixels_pointer;

    /* Everything on Glide TMU 0 for now. */
	buffer->buffer.tmu_id = GR_TMU0;

	/* It's all OK, so we can mark it as usable now! */
	buffer->buffer.uncacheable = BR_FALSE;

    return BRE_OK;
}


/*
 * Set up a static device object
 */
struct br_buffer_stored *BufferStored3DfxAllocate(struct br_primitive_library *plib,
	br_token use, struct br_device_pixelmap *pm, br_token_value *tv)
{
	struct br_buffer_stored * self;
	char *ident;

	switch(use) {
	case BRT_TEXTURE_O:
	case BRT_COLOUR_MAP_O:
		ident ="Texture";
		break;

	default:
		return NULL;
	}

	self = BrResAllocate(DRIVER_RESOURCE, sizeof(*self), BR_MEMORY_OBJECT);

	if(self == NULL)
		return NULL;

	self->dispatch = &bufferStoredDispatch;
	self->identifier = ident;
	self->plib = plib;

	/*
	 * If caller does not garuantee to keep source data around, or
	 * source data is not memory mapped, then clone an in-memory copy
	 */

/* <dave> Below is an entry for the programming Hall Of Fame.
 * Congratulations due to whoever came up with this one.... */
#if 0
	if() {
	} else {
	}
#endif

	self->flags |= SBUFF_SHARED;

    self->buffer.on_card = BR_FALSE;
	Setup3DfxBuffer(self, pm);

	ObjectContainerAddFront(plib,(br_object *)self);

	return self;
}


static br_error BR_CMETHOD_DECL(br_buffer_stored_3dfx, update)(
	struct br_buffer_stored *self,
	struct br_device_pixelmap *pm,
	br_token_value *tv)
{
	Setup3DfxBuffer(self, pm);

    self->buffer.force_reload = BR_TRUE;

	return BRE_OK;
}


static void BR_CMETHOD_DECL(br_buffer_stored_3dfx, free)(br_buffer_stored *self)
{
    TextureCacheClearEntry(&self->buffer);

	ObjectContainerRemove(self->plib, (br_object *)self);

	BrResFreeNoCallback(self);
}

static br_token BR_CMETHOD_DECL(br_buffer_stored_3dfx, type)(br_buffer_stored *self)
{
	return BRT_BUFFER_STORED;
}

static br_boolean BR_CMETHOD_DECL(br_buffer_stored_3dfx, isType)(br_buffer_stored *self, br_token t)
{
	return (t == BRT_BUFFER_STORED) || (t == BRT_OBJECT);
}

static br_int_32 BR_CMETHOD_DECL(br_buffer_stored_3dfx, space)(br_buffer_stored *self)
{
	return BrResSizeTotal(self);
}

static struct br_tv_template * BR_CMETHOD_DECL(br_buffer_stored_3dfx,templateQuery)
	(br_buffer_stored *self)
{
	bufferStoredTemplate.res = DRIVER_RESOURCE;
	return &bufferStoredTemplate;
}

/*
 * Default dispatch table for device
 */
static struct br_buffer_stored_dispatch bufferStoredDispatch = {
	NULL,
	NULL,
	NULL,
	NULL,
	BR_CMETHOD_REF(br_buffer_stored_3dfx,	free),
	BR_CMETHOD_REF(br_object_3dfx,			identifier),
	BR_CMETHOD_REF(br_buffer_stored_3dfx, 	type),
	BR_CMETHOD_REF(br_buffer_stored_3dfx, 	isType),
	BR_CMETHOD_REF(br_object_3dfx, 			device),
	BR_CMETHOD_REF(br_buffer_stored_3dfx, 	space),

	BR_CMETHOD_REF(br_buffer_stored_3dfx,	templateQuery),
	BR_CMETHOD_REF(br_object,				query),
	BR_CMETHOD_REF(br_object, 				queryBuffer),
	BR_CMETHOD_REF(br_object, 				queryMany),
	BR_CMETHOD_REF(br_object, 				queryManySize),
	BR_CMETHOD_REF(br_object, 				queryAll),
	BR_CMETHOD_REF(br_object,	 			queryAllSize),

	BR_CMETHOD_REF(br_buffer_stored_3dfx,	update),
};

