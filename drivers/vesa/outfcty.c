/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: outfcty.c 1.1 1997/12/10 16:54:15 jon Exp $
 * $Locker: $
 *
 * Output facility methods
 */
#include <stddef.h>
#include <string.h>

#include "drv.h"
#include "host.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: outfcty.c 1.1 1997/12/10 16:54:15 jon Exp $");

/*
 * Default dispatch table for device (defined at end of file)
 */
static struct br_output_facility_dispatch outputFacilityDispatch;

/*
 * Output Facility info. template
 */
#define F(f)	offsetof(struct br_output_facility, f)
#define S(s)	(br_uint_32)(s)
static struct br_tv_template_entry outputFacilityTemplateEntries[] = {
	{BRT(IDENTIFIER_CSTR),	F(identifier),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },

	{BRT(WIDTH_I32),	 	F(width),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(WIDTH_MIN_I32), 	F(width),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(WIDTH_MAX_I32), 	F(width),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(HEIGHT_I32),		F(height),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(HEIGHT_MIN_I32),	F(height),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(HEIGHT_MAX_I32),	F(height),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(PIXEL_TYPE_U8),	F(colour_type),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT(PIXEL_BITS_I32),	F(colour_bits),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },

//	{BRT(PIXEL_CHANNELS_I32),F(channels_mask),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
//	{BRT(PIXEL_CHANNELS_TL), F(channels_list),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },

	{BRT(INDEXED_B),	 	F(indexed),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },

//	{BRT(MEMORY_MAPPED_B), 	F(),				BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },

	{BRT_PIXELMAP_MAX_I32, 	0,	0,				BRTV_QUERY | BRTV_ALL,	BRTV_CONV_DIRECT, 1},
	{BRT_CLUT_MAX_I32, 		0,	0,				BRTV_QUERY | BRTV_ALL,	BRTV_CONV_DIRECT, 0},

//	{BRT(VIDEO_MEMORY_U32), F(),				BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
//	{BRT(TEXTURE_MEMORY_U32), F(),				BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
//	{BRT(HOST_MEMORY_U32), F(),					BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },

	{BRT(MODE_U32),			F(vesa_mode),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_I32_U16, },
	{DEV(VESA_MODE_U16),	F(vesa_mode),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_I32_U16, },
};
#undef F

static struct br_tv_template outputFacilityTemplate = {
	BR_ASIZE(outputFacilityTemplateEntries),
	outputFacilityTemplateEntries
};

static struct masks_to_mode {
	br_uint_8	red_mask_size;
	br_uint_8	red_field_position;
	br_uint_8	green_mask_size;
	br_uint_8	green_field_position;
	br_uint_8	blue_mask_size;
	br_uint_8	blue_field_position;
	br_uint_8	rsvd_mask_size;
	br_uint_8	rsvd_field_position;

	br_uint_32	colour_type;
	br_uint_32	colour_bits;
} masksToMode[] = {
	{5,10, 	5,5,	5,0,	1,15,	BR_PMT_RGB_555,		15},
	{5,10, 	5,5,	5,0,	0,0,	BR_PMT_RGB_555,		15},
	{5,11, 	6,5,	5,0,	0,0,	BR_PMT_RGB_565,		16},
	{8,16, 	8,8,	8,0,	0,0,	BR_PMT_RGB_888,		24},
	{8,16, 	8,8,	8,0,	8,24,	BR_PMT_RGBX_888,	32},
};

/*
 * Setup a static output facility
 */
br_error OutputFacilityVESAInitialise(int *count, br_device *dev)
{
	br_output_facility *self;
	int m,i;
	struct vesa_info *vip = DeviceVESAInfo(dev);
	struct vesa_modeinfo modeinfo;
	br_int_32 type,bits;
	char tmp[80];

	/*
	 * Find the number of supported modes
	 */
	for(m=0; vip->videomode_ptr[m] != 0xFFFF;)
		m++;

	/*
	 * Go through and investigate the available vesa modes
	 */
	for(m--; m >= 0 ; m--) {

		if(VESAModeInfo(&modeinfo, vip->videomode_ptr[m]) != BRE_OK)
			continue;

		/*
		 * See if this mode is of interest
		 */
		if(!(modeinfo.mode_attributes & VESA_MA_SUPPORTED))
			continue;

		if(!(modeinfo.mode_attributes & VESA_MA_GRAPHICS))
			continue;

		type = -1;

		switch(modeinfo.memory_model) {
		case VESA_MM_PACKED_PIXEL:
			switch(modeinfo.bits_per_pixel) {
			case 8:
				type = BR_PMT_INDEX_8;
				bits = 8;
				break;
			case 15:
				type = BR_PMT_RGB_555;
				bits = 15;
				break;
			case 16:
				type = BR_PMT_RGB_565;
				bits = 16;
				break;
			case 24:
				type = BR_PMT_RGB_888;
				bits = 24;
				break;
			case 32:
				type = BR_PMT_RGBX_888;
				bits = 32;
				break;
			}
			break;

		case VESA_MM_DIRECT:

			for(i=0; i < BR_ASIZE(masksToMode); i++) {
				if((masksToMode[i].red_mask_size == modeinfo.red_mask_size) &&
				   (masksToMode[i].red_field_position == modeinfo.red_field_position) &&
				   (masksToMode[i].green_mask_size == modeinfo.green_mask_size) &&
				   (masksToMode[i].green_field_position == modeinfo.green_field_position) &&
				   (masksToMode[i].blue_mask_size == modeinfo.blue_mask_size) &&
				   (masksToMode[i].blue_field_position == modeinfo.blue_field_position) &&
				   (masksToMode[i].rsvd_mask_size == modeinfo.rsvd_mask_size) &&
				   (masksToMode[i].rsvd_field_position == modeinfo.rsvd_field_position)) {
					type = masksToMode[i].colour_type;
					bits = masksToMode[i].colour_bits;
					break;
				}
			}
			break;
		}

		if(type == -1)
			continue;

		/*
		 * Allocate an object and fill it in
		 */
		self = BrResAllocate(DeviceVESAResource(dev),
			sizeof(*self), BR_MEMORY_OBJECT_DATA);

		self->indexed = (type == BR_PMT_INDEX_8);
		self->colour_type = type;
		self->colour_bits = bits;
		self->dispatch = &outputFacilityDispatch;
		self->width = modeinfo.x_resolution;
		self->height = modeinfo.y_resolution;
		self->vesa_mode = vip->videomode_ptr[m];
		self->modeinfo = modeinfo;

		BrSprintfN(tmp,sizeof(tmp)-1,"%dx%dx%d",self->width,self->height,bits);
		self->identifier = BrResStrDup(self,tmp);

		self->object_list = BrObjectListAllocate(self);

		ObjectContainerAddFront(dev,(br_object *)self);

		(*count)++;
	}


	return BRE_OK;
}

/*
 * Common object methods
 */
static void BR_CMETHOD_DECL(br_output_facility_vesa, free)(br_output_facility *self)
{
	ObjectContainerRemove(ObjectDevice(self), (br_object *)self);

	/*
	 * Remove attached objects
	 */
	ObjectContainerFree((br_object_container *)self, BR_NULL_TOKEN, NULL, NULL);

	BrResFreeNoCallback(self);
}

static br_token BR_CMETHOD_DECL(br_output_facility_vesa, type)(br_output_facility *self)
{
	return BRT_OUTPUT_FACILITY;
}

static br_boolean BR_CMETHOD_DECL(br_output_facility_vesa, isType)(br_output_facility *self, br_token t)
{
	return (t == BRT_OUTPUT_FACILITY) || (t == BRT_OBJECT_CONTAINER) || (t == BRT_OBJECT);
}

static br_int_32 BR_CMETHOD_DECL(br_output_facility_vesa, space)(br_output_facility *self)
{
	return sizeof(br_output_facility);
}

static struct br_tv_template * BR_CMETHOD_DECL(br_output_facility_vesa, templateQuery)
	(br_output_facility *self)
{
	outputFacilityTemplate.res = DeviceVESAResource(ObjectVESADevice(self));
	return &outputFacilityTemplate;
}

static br_error BR_CMETHOD_DECL(br_output_facility_vesa, validSource)(br_output_facility *self, br_boolean *bp, br_object *h)
{
	return BRE_OK;
}

/*
 * Instantiate an output pixelmap from the output facility
 *
 */
static br_error BR_CMETHOD_DECL(br_output_facility_vesa, pixelmapNew)(br_output_facility *self, br_device_pixelmap **ppmap, br_token_value *tv)
{
	br_error r;
	br_device_pixelmap *pm;

	/*
	 * Create device pixelmap structure pointing at display memory
	 */
	pm = DevicePixelmapVESAAllocateMode(ObjectDevice(self), (br_output_facility *)self, self->width, self->height);

	if(pm == NULL)
		return BRE_FAIL;

	*ppmap = pm;
	self->num_instances++;

	return BRE_OK;
}

/*
 * Cannot create new CLUTs, stuck with the single hardware one
 */
static br_error BR_CMETHOD_DECL(br_output_facility_vesa, clutNew)(br_output_facility *self,
	br_device_clut **pclut, br_token_value *tv)
{
	return BRE_FAIL;
}

static void * BR_CMETHOD_DECL(br_output_facility_vesa,listQuery)
	(br_output_facility *self)
{
	return self->object_list;
}

/*
 * Default dispatch table for device
 */
static struct br_output_facility_dispatch outputFacilityDispatch = {
	NULL,
	NULL,
	NULL,
	NULL,
	BR_CMETHOD_REF(br_output_facility_vesa, free),
	BR_CMETHOD_REF(br_object_vesa, identifier),
	BR_CMETHOD_REF(br_output_facility_vesa, type),
	BR_CMETHOD_REF(br_output_facility_vesa,	isType),
	BR_CMETHOD_REF(br_object_vesa, device),
	BR_CMETHOD_REF(br_output_facility_vesa, space),

	BR_CMETHOD_REF(br_output_facility_vesa, templateQuery),
	BR_CMETHOD_REF(br_object, query),
	BR_CMETHOD_REF(br_object, queryBuffer),
	BR_CMETHOD_REF(br_object, queryMany),
	BR_CMETHOD_REF(br_object, queryManySize),
	BR_CMETHOD_REF(br_object, queryAll),
	BR_CMETHOD_REF(br_object, queryAllSize),

	BR_CMETHOD_REF(br_output_facility_vesa,		listQuery),
	BR_CMETHOD_REF(br_object_container,	tokensMatchBegin),
	BR_CMETHOD_REF(br_object_container,	tokensMatch),
	BR_CMETHOD_REF(br_object_container,	tokensMatchEnd),
	BR_CMETHOD_REF(br_object_container,	addFront),
	BR_CMETHOD_REF(br_object_container,	removeFront),
	BR_CMETHOD_REF(br_object_container,	remove),
	BR_CMETHOD_REF(br_object_container,	find),
	BR_CMETHOD_REF(br_object_container, findMany),
	BR_CMETHOD_REF(br_object_container, count),

	BR_CMETHOD_REF(br_output_facility_vesa, validSource),
	BR_CMETHOD_REF(br_output_facility_vesa, pixelmapNew),
	BR_CMETHOD_REF(br_output_facility_vesa, clutNew),
};

