/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * Device pixelmap methods
 */
#include <stddef.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "drv.h"
#include "pm.h"
#include "host.h"
#include "shortcut.h"
#include "brassert.h"



br_error Allocate3DfxSysMemPixelmap(br_device_pixelmap *self, br_device_pixelmap **newpm, int w, int h);

br_error BR_CMETHOD_DECL(br_device_pixelmap_3dfx, copyTo)(struct br_device_pixelmap *self, br_device_pixelmap *src);
br_error BR_CMETHOD_DECL(br_device_pixelmap_3dfx, copyFrom)(struct br_device_pixelmap *self, br_device_pixelmap *dest);
br_error BR_CMETHOD_DECL(br_device_pixelmap_3dfx, rectangleCopyTo)(struct br_device_pixelmap *self, br_point *p, br_device_pixelmap *src, br_rectangle *r);
br_error BR_CMETHOD_DECL(br_device_pixelmap_3dfx, rectangleCopyFrom)(struct br_device_pixelmap *self, br_point *p, br_device_pixelmap *dest, br_rectangle *r);



/*
 * Default dispatch table for device pixelmap (defined at end of file)
 */
static struct br_device_pixelmap_dispatch devicePixelmapDispatch;

/*
 * Device pixelmap info. template
 */
#define F(f)	offsetof(struct br_device_pixelmap, f)

static struct br_tv_template_entry devicePixelmapTemplateEntries[] = {
	{BRT_WIDTH_I32,			0,	F(pm_width),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_I32_U16, },
	{BRT_HEIGHT_I32,		0,	F(pm_height),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_I32_U16, },
	{BRT_PIXEL_TYPE_U8,		0,	F(pm_type),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_I32_U8, },
	{BRT_OUTPUT_FACILITY_O,	0,	F(output_facility),	BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT_FACILITY_O,		0,	F(output_facility),	BRTV_QUERY,				BRTV_CONV_COPY, },
	{BRT_IDENTIFIER_CSTR,	0,	F(pm_identifier),	BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT_CLUT_O,			0,	F(clut),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
};
#undef F

static struct br_tv_template devicePixelmapTemplate = {
	BR_ASIZE(devicePixelmapTemplateEntries),
	devicePixelmapTemplateEntries
};



/*
 * Structure used to unpack the 'match' tokens/values
 */
struct match_tokens {
	br_token	use;
	br_int_32	pixel_bits;
	br_int_32	width;
	br_int_32	height;
	br_object  *renderer;
};

#define F(f)	offsetof(struct match_tokens, f)

static struct br_tv_template_entry matchTemplateEntries[] = {
	{BRT_USE_T,				0,	F(use),					BRTV_SET,	BRTV_CONV_COPY, },
	{BRT_PIXEL_BITS_I32,	0,	F(pixel_bits),			BRTV_SET,	BRTV_CONV_COPY, },
	{BRT_RENDERER_O,		0,	F(renderer),			BRTV_SET,	BRTV_CONV_COPY, },
	{BRT_WIDTH_I32,			0,	F(width),				BRTV_SET,	BRTV_CONV_COPY, },
	{BRT_HEIGHT_I32,		0,	F(height),				BRTV_SET,	BRTV_CONV_COPY, },
};
#undef F

static struct br_tv_template matchTemplate = {
	BR_ASIZE(matchTemplateEntries),
	matchTemplateEntries
};



#ifdef _WIN32
/*
 * Structures used to translate tokens and values
 */
struct hwnd_tokens {
	HWND hwnd;
};

#define F(f)	offsetof(struct hwnd_tokens, f)
static struct br_tv_template_entry hwndTemplateEntries[] = {
	{BRT(WINDOW_HANDLE_H),			F(hwnd),			BRTV_SET,	BRTV_CONV_COPY, },
};
#undef F

static struct br_tv_template hwndTemplate = {
	BR_ASIZE(hwndTemplateEntries),
	hwndTemplateEntries
};

HWND hwnd;

#else

br_uint_32 hwnd;

#endif



br_boolean passthru_vga;
static br_uint_32 current_width;
static br_uint_32 current_height;

GrLfbWriteMode_t	lfb_write_mode;
GrHwConfiguration hwConfig;




/*
 * Glide error callback to replace its internal one
 */
void GlideErrorCallback( const char *string, FxBool fatal)
{
	if(fatal) {
		grGlideShutdown();
		DriverDevice3Dfx.initialised = BR_FALSE;
		BR_FATAL1("(Glide) %s", string);
	} else {
#if DEBUG
		BR_WARNING1("(Glide) %s", string);
#endif
	}
}







/*
 * Create a new device pixelmap and set a display mode
 */
br_error DevicePixelmap3DfxAllocateMode(br_device *dev, br_output_facility *facility,
										br_device_pixelmap **newpm, br_token_value *tv)
{
	br_device_pixelmap *self;
	int i, mem, fxmem;
	br_uint_16 qual;


	if(dev->active_buffers[BT_FRONTSCREEN])
		return(BRE_DEV_ALREADY_INIT);

	if (facility->colour_type == BR_PMT_RGB_565)
		lfb_write_mode = GR_LFBWRITEMODE_565;
	else if (facility->colour_type == BR_PMT_RGB_555)
		lfb_write_mode = GR_LFBWRITEMODE_555;
	else
		return(BRE_DEV_UNSUPPORTED);


	CLEAR_TEMPLATE(devicePixelmap);
	CLEAR_TEMPLATE(match);

	/*
	 * See if there is a board out there...
	 */
	if (!dev->initialised) {

		grGlideInit();
		if(!grSstQueryHardware(&hwConfig)) {
			grGlideShutdown();
			return BRE_DEV_HARDWARE_NOT_PRESENT;
		}

		dev->initialised = BR_TRUE;
	}

	/* Check we can support this resolution */
	if (facility->width > 640) mem = 4;
	else mem = 2;
	for(i=0; i<hwConfig.num_sst; i++) {
		if (hwConfig.SSTs[i].type == GR_SSTTYPE_VOODOO) {
			fxmem = hwConfig.SSTs[i].sstBoard.VoodooConfig.fbRam;
			if (fxmem < mem)
				return(BRE_DEV_NO_VIDEO_MEMORY);
		}
	}

	grErrorSetCallback(GlideErrorCallback);

	/* We have appropriate hardware */
	grSstSelect(0);

#ifdef _WIN32
	{
		int  ScreenWidth,ScreenHeight;
		struct hwnd_tokens ht;
		br_int_32 count;

		/* Parse for hwnd */
		CLEAR_TEMPLATE(hwnd);
		ht.hwnd = NULL;
		hwndTemplate.res = Device3DfxResource(dev);
		BrTokenValueSetMany(&ht, &count, NULL, tv, &hwndTemplate);
		hwnd = ht.hwnd;
		if (!hwnd)
			return(BRE_DEV_FAIL);

		ScreenWidth  =  GetSystemMetrics(SM_CXSCREEN);
		ScreenHeight =  GetSystemMetrics(SM_CYSCREEN);

		SetForegroundWindow(hwnd);
		MoveWindow(hwnd, 0, 0, ScreenWidth, ScreenHeight, TRUE);
	}
#endif



	/*
	 * Setup mode
	 */
	if(!grSstOpen(facility->glide_res_code,
	                 GR_REFRESH_60Hz,
	                 GR_COLORFORMAT_ARGB,
	                 GR_ORIGIN_UPPER_LEFT,
	                 GR_SMOOTHING_ENABLE,
	                 2)) {
		return(BRE_DEV_HARDWARE_SET);
	}

	/*
	 * Clear out the buffer after that yucky splash screen
	 */
	grDepthMask(FXFALSE);
	grRenderBuffer(GR_BUFFER_FRONTBUFFER);
	grBufferClear(0,0,0xFFFF);
	grRenderBuffer(GR_BUFFER_BACKBUFFER);
	
	/* Set up default state */
	grDepthBufferMode( GR_DEPTHBUFFER_ZBUFFER );
	grDepthBufferFunction(GR_CMP_LEQUAL);
	grDepthMask(FXTRUE);
	grChromakeyValue(0);
	grCullMode(GR_CULL_DISABLE);
	grTexDetailControl(GR_TMU0, 0, 8, 1.0f); /* LOD bias, detail select, detail max */
	grTexClampMode(GR_TMU0, GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP);
	guTexCombineFunction(GR_TMU0, GR_TEXTURECOMBINE_DECAL);


	/* Glide's default gamma is NOT 1.0 as suggested in the documentation; it is in
	 * fact 1.5 or thereabouts. This makes it a bit bright in comparison to all
	 * the other drivers. This brightness is not a major disadvantage; but to make
	 * the drivers more consistent I have (by experiment) decided 1.2 is a good
	 * compromise between a bright image */
	grGammaCorrectionValue(1.2f);

	self = BrResAllocate(Device3DfxResource(dev), sizeof(*self), BR_MEMORY_OBJECT);

	self->dispatch = &devicePixelmapDispatch;
 	self->pm_identifier = facility->identifier;
	self->pm_pixels = 0;

	self->pm_type	= (br_uint_8)  facility->colour_type;
	self->pm_width	= (br_uint_16) facility->width;
	self->pm_height = (br_uint_16) facility->height;
	self->pm_row_bytes = 0; 
	self->pm_origin_x = 0;
	self->pm_origin_y = 0;

	self->pm_flags = BR_PMF_NO_ACCESS;
	HostSelectorDS(&qual);
	self->pm_pixels_qualifier = qual;
	self->pm_base_x = 0;
	self->pm_base_y = 0;
	self->device = dev;
	self->output_facility = facility;
	self->clut = dev->clut;

	self->buffer_type = BT_FRONTSCREEN;
	dev->active_buffers[BT_FRONTSCREEN] = self;

	ObjectContainerAddFront(facility, (br_object *)self);

	*newpm = self;

	current_width = facility->width;
	current_height = facility->height;
	passthru_vga = BR_FALSE;

	Clear3DfxStateCache();
	return(BRE_OK);
}

static void BR_CMETHOD_DECL(br_device_pixelmap_3dfx, free)(br_device_pixelmap *self)
{
	if (!self->sub_pixelmap) {

		ObjectDevice(self)->active_buffers[self->buffer_type] = NULL;

		/*
		 * Frontscreen free shuts device down
		 */
		if (self->buffer_type == BT_FRONTSCREEN) {
			ObjectContainerRemove(self->output_facility, (br_object *)self);
			self->output_facility->num_instances--;

			/*
			 * Close down device
			 */
			grGlideShutdown();
			self->device->initialised = BR_FALSE;
		}
	}

	/*
	 * Free up pixelmap structure
	 */
	BrResFreeNoCallback(self);

}

br_token BR_CMETHOD_DECL(br_device_pixelmap_3dfx, type)(br_device_pixelmap *self)
{
	return BRT_DEVICE_PIXELMAP;
}

br_boolean BR_CMETHOD_DECL(br_device_pixelmap_3dfx, isType)(br_device_pixelmap *self, br_token t)
{
	return (t == BRT_DEVICE_PIXELMAP) || (t == BRT_OBJECT);
}

br_int_32 BR_CMETHOD_DECL(br_device_pixelmap_3dfx, space)(br_device_pixelmap *self)
{
	return sizeof(br_device_pixelmap);
}

struct br_tv_template * BR_CMETHOD_DECL(br_device_pixelmap_3dfx,queryTemplate)
	(br_device_pixelmap *self)
{
	devicePixelmapTemplate.res = Device3DfxResource(Object3DfxDevice(self));
	return &devicePixelmapTemplate;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_fix, flush)(
	struct br_device_pixelmap *self)
{
	return BRE_OK;
}
br_error BR_CMETHOD_DECL(br_device_pixelmap_fix, synchronise)(
	struct br_device_pixelmap *self,
	br_token sync_type,
	br_boolean block)
{
	return BRE_OK;
}

static br_error BR_CMETHOD_DECL(br_device_pixelmap_3dfx, resize)\
			(br_device_pixelmap *self, br_int_32 width, br_int_32 height)
{

	if (self->buffer_type != BT_FRONTSCREEN)
		return(BRE_DEV_UNSUPPORTED);

#if 0	// Disabled, no longer supported by Glide
	if ((width == 0) && (height == 0)) {
		/* This is a Dave 3Dfx special: resize to 0, 0 disables the passthrough connector */
		grSstPassthruMode(GR_PASSTHRU_SHOW_VGA);
		passthru_vga = BR_TRUE;
		return(BRE_OK);
	}

	if (passthru_vga && (width == current_width) && (height == current_height)) {
		grSstPassthruMode(GR_PASSTHRU_SHOW_SST1);
		return(BRE_OK);
	}
#endif

	return(BRE_DEV_UNSUPPORTED);
}



static br_error BR_CMETHOD_DECL(br_device_pixelmap_3dfx, match)(br_device_pixelmap *self,
	br_device_pixelmap **newpm, br_token_value *tv)
{
	struct match_tokens mt = {BRT_NONE, 0};
	br_int_32 count;
	br_device_pixelmap *pm;
	br_device *dev = ObjectDevice(self);
	br_uint_16 qual;

	matchTemplate.res = Device3DfxResource(dev);

	/*
	 * Default width and height
	 */
	mt.width = self->pm_width;
	mt.height = self->pm_height;

	BrTokenValueSetMany(&mt, &count, NULL, tv, &matchTemplate);

	/*
	 * Create the pixelmap structure
	 */

	if(mt.use == BRT_OFFSCREEN) {
		/*
		 * Check that the source buffer is of the correct type
		 */
		if(self->buffer_type != BT_FRONTSCREEN)
			return BRE_DEV_UNSUPPORTED;

		if(dev->active_buffers[BT_BACKSCREEN]) {
//			return BRE_DEV_ALREADY_INIT;
			/* OK we've already allocated the backscreen - now we have the hack */
			return(Allocate3DfxSysMemPixelmap(self, newpm, mt.width, mt.height));
		}

		/*
		 * Create an offscreen buffer pixelmap
		 */
		pm = BrResAllocate(Device3DfxResource(dev),sizeof(*pm), BR_MEMORY_OBJECT);

		pm->pm_width = self->pm_width;
		pm->pm_height = self->pm_height;
		pm->pm_pixels = 0;
		HostSelectorDS(&qual);
		pm->pm_pixels_qualifier = qual;

		pm->pm_origin_x = self->pm_origin_x;
		pm->pm_origin_y = self->pm_origin_y;

		pm->pm_flags = BR_PMF_NO_ACCESS;

		pm->output_facility = self->output_facility;

		pm->pm_type = self->pm_type;
		pm->dispatch = &devicePixelmapDispatch;
		pm->pm_identifier = "Offscreen";

		pm->buffer_type = BT_BACKSCREEN;
		dev->active_buffers[BT_BACKSCREEN] = pm;

	} else if(mt.use == BRT_DEPTH) {

		/*
		 * Check that the source buffer is of the correct type
		 */
		if(self->buffer_type != BT_FRONTSCREEN &&
			self->buffer_type != BT_BACKSCREEN)
			return BRE_DEV_UNSUPPORTED;

		if(dev->active_buffers[BT_DEPTH])
			return BRE_DEV_ALREADY_INIT;

		/*
		 * Create an depth buffer pixelmap
		 */
		pm = BrResAllocate(Device3DfxResource(dev),sizeof(*pm), BR_MEMORY_OBJECT);

		pm->pm_width = self->pm_width;
		pm->pm_height = self->pm_height;
		pm->pm_pixels = 0;
		HostSelectorDS(&qual);
		self->pm_pixels_qualifier = qual;

		pm->pm_origin_x = self->pm_origin_x;
		pm->pm_origin_y = self->pm_origin_y;

		pm->pm_flags = BR_PMF_NO_ACCESS;

		pm->output_facility = self->output_facility;

		pm->pm_type = BR_PMT_DEPTH_16;
		pm->dispatch = &devicePixelmapDispatch;
		pm->pm_identifier = "Depth";

		pm->buffer_type = BT_DEPTH;
		dev->active_buffers[BT_DEPTH] = pm;

	} else if (mt.use == BRT_NO_RENDER) {
		/* No render pixelmaps require a device system memory pixelmap */
		return(Allocate3DfxSysMemPixelmap(self, newpm, mt.width, mt.height));

	} else {
		return BRE_DEV_UNSUPPORTED;
	}

	*newpm = pm;

	return BRE_OK;
}



static br_error BR_CMETHOD_DECL(br_device_pixelmap_3dfx, rectangleFill)
	(br_device_pixelmap *self, br_rectangle *rect, br_uint_32 colour)
{
	br_rectangle arect;
	br_int_32 y;
	br_uint_16 *ptr;
	br_uint_32 *pd, c, col32;

	if(PixelmapRectangleClip(&arect, rect, (br_pixelmap *)self) == BR_CLIP_REJECT)
		return BRE_OK;

	grLfbBegin();
	grLfbBypassMode(GR_LFBBYPASS_ENABLE);

	if (self->buffer_type == BT_FRONTSCREEN)
		ptr = grLfbGetWritePtr( GR_BUFFER_FRONTBUFFER );
	else if (self->buffer_type == BT_BACKSCREEN)
		ptr = grLfbGetWritePtr( GR_BUFFER_BACKBUFFER );
	else if (self->buffer_type == BT_DEPTH) {
		ptr = grLfbGetWritePtr( GR_BUFFER_DEPTHBUFFER );
	} else {
		grLfbEnd();
		return(BRE_DEV_FAIL);
	}

	grLfbWriteMode( lfb_write_mode );

	ptr += (arect.x + self->pm_base_x) + ((arect.y + self->pm_base_y) * 1024);

	col32 = colour + (((br_uint_32) colour) << 16);

	for(y=0;y<arect.h;y++)
	{
		pd = (br_uint_32 *) ptr;
		c = arect.w >> 1;
		while (c--)
			*pd++ = col32;
		if (arect.w & 1)
			*((br_uint_16 *)pd) = (br_uint_16) colour;

		ptr += 1024;
	}

	grLfbEnd();

	return BRE_OK;
}


static br_error BR_CMETHOD_DECL(br_device_pixelmap_3dfx, fill)(br_device_pixelmap *self, br_uint_32 colour)
{
	int col;

	if (self->sub_pixelmap) {

		br_rectangle rect;

		rect.x = -self->pm_origin_x;
		rect.y = -self->pm_origin_y;
		rect.w = self->pm_width;
		rect.h = self->pm_height;

		return(BR_CMETHOD(br_device_pixelmap_3dfx, rectangleFill)(self, &rect, colour));
	}

	if (self->pm_type == BR_PMT_RGB_555) {
		col = ((colour & 0x7c00) << 9) |
			  ((colour & 0x03e0) << 6) |
			  ((colour & 0x001f) << 3);

	} else if (self->pm_type == BR_PMT_RGB_565) {
		col = ((colour & 0xf800) << 8) |
			  ((colour & 0x07e0) << 5) |
			  ((colour & 0x001f) << 3);

	} else
		col = 0;

	if(self == ObjectDevice(self)->active_buffers[BT_FRONTSCREEN]) {
		grDepthMask(FXFALSE);
		grRenderBuffer(GR_BUFFER_FRONTBUFFER);
		grBufferClear(col,0,0xFFFF);
		grRenderBuffer(GR_BUFFER_BACKBUFFER);
		grDepthMask(zwrite);
	}

	if(self == ObjectDevice(self)->active_buffers[BT_BACKSCREEN]) {
		grDepthMask(FXFALSE);
		grBufferClear(col,0,0xFFFF);
		grDepthMask(zwrite);
	}

	if(self == ObjectDevice(self)->active_buffers[BT_DEPTH]) {
		grColorMask(FXFALSE,FXFALSE);
		grDepthMask(FXTRUE);
		grBufferClear(0,0, (br_uint_16) (colour>>16));
		grColorMask(FXTRUE,FXFALSE);
		grDepthMask(zwrite);
	}

	return BRE_OK;
}



br_error BR_CMETHOD_DECL(br_device_pixelmap_3dfx, copy)(br_device_pixelmap *self, br_device_pixelmap *src)
{
	br_uint_16 *ptr, *dest_ptr;

	if (src->buffer_type == BT_SYSMEM)
		return BR_CMETHOD(br_device_pixelmap_3dfx, copyTo)(self, src);

	else if (self->buffer_type == BT_SYSMEM)
		return BR_CMETHOD(br_device_pixelmap_3dfx, copyFrom)(src, self);

	if ((self->pm_type != src->pm_type) || (self->pm_width != src->pm_width) || (self->pm_height != src->pm_height))
		return(BRE_DEV_FAIL);

	grLfbBegin();
	grLfbBypassMode(GR_LFBBYPASS_ENABLE);

	if (self->buffer_type == BT_FRONTSCREEN)
		dest_ptr = grLfbGetWritePtr( GR_BUFFER_FRONTBUFFER );
	else if (self->buffer_type == BT_BACKSCREEN)
		dest_ptr = grLfbGetWritePtr( GR_BUFFER_BACKBUFFER );
	else if (self->buffer_type == BT_DEPTH)
		dest_ptr = grLfbGetWritePtr( GR_BUFFER_DEPTHBUFFER );
	else {
		grLfbEnd();
		return(BRE_DEV_FAIL);
	}

	if (src->buffer_type == BT_FRONTSCREEN)
		ptr = grLfbGetReadPtr( GR_BUFFER_FRONTBUFFER );
	else if (src->buffer_type == BT_BACKSCREEN)
		ptr = grLfbGetReadPtr( GR_BUFFER_BACKBUFFER );
	else if (src->buffer_type == BT_DEPTH)
		ptr = grLfbGetReadPtr( GR_BUFFER_DEPTHBUFFER );
	else {
		grLfbEnd();
		return(BRE_DEV_FAIL);
	}

	grLfbWriteMode(GR_LFBWRITEMODE_565);

	ptr += src->pm_base_x + (src->pm_base_y * 1024);

	guFbWriteRegion(self->pm_base_x, self->pm_base_y, self->pm_width, self->pm_height, ptr, 1024 * 2);

	grLfbEnd();

	return(BRE_OK);
}


br_error BR_CMETHOD_DECL(br_device_pixelmap_3dfx, copyTo)(struct br_device_pixelmap *self, br_device_pixelmap *src)
{
	br_int_32 y;
	br_uint_16 *ptr, *dest_ptr;
	br_uint_32 *pd, *ps, c, colour;
	
	if ((self->pm_type != src->pm_type) || (self->pm_width != src->pm_width) || (self->pm_height != src->pm_height))
		return(BRE_DEV_FAIL);

	grLfbBegin();
	grLfbBypassMode(GR_LFBBYPASS_ENABLE);

	if (self->buffer_type == BT_FRONTSCREEN)
		dest_ptr = grLfbGetWritePtr( GR_BUFFER_FRONTBUFFER );
	else if (self->buffer_type == BT_BACKSCREEN)
		dest_ptr = grLfbGetWritePtr( GR_BUFFER_BACKBUFFER );
	else if (self->buffer_type == BT_DEPTH)
		dest_ptr = grLfbGetWritePtr( GR_BUFFER_DEPTHBUFFER );
	else {
		grLfbEnd();
		return(BRE_DEV_FAIL);
	}

	grLfbWriteMode( lfb_write_mode );

	ptr = src->pm_pixels;
	ptr += src->pm_base_x + (src->pm_base_y * src->pm_row_bytes / 2);

	guFbWriteRegion(self->pm_base_x, self->pm_base_y, self->pm_width, self->pm_height, ptr, src->pm_row_bytes);

	grLfbEnd();

	return(BRE_OK);
}


br_error BR_CMETHOD_DECL(br_device_pixelmap_3dfx, copyFrom)(struct br_device_pixelmap *self, br_device_pixelmap *dest)
{
	br_int_32 y;
	br_uint_16 *ptr, *dest_ptr;
	br_uint_32 *pd, *ps, c, colour;
	
	if ((self->pm_width != dest->pm_width) || (self->pm_height != dest->pm_height))
		return(BRE_DEV_FAIL);

	if (self->buffer_type == BT_DEPTH) {
		if (self->pm_type != dest->pm_type)
			return(BRE_DEV_FAIL);
	} else {
		if (self->pm_type != BR_PMT_RGB_565)
			return(BRE_DEV_FAIL);
	}

	grLfbBegin();
	grLfbBypassMode(GR_LFBBYPASS_ENABLE);

	if (self->buffer_type == BT_FRONTSCREEN)
		ptr = (br_uint_16 *)grLfbGetReadPtr( GR_BUFFER_FRONTBUFFER );
	else if (self->buffer_type == BT_BACKSCREEN)
		ptr = (br_uint_16 *)grLfbGetReadPtr( GR_BUFFER_BACKBUFFER );
	else if (self->buffer_type == BT_DEPTH)
		ptr = (br_uint_16 *)grLfbGetReadPtr( GR_BUFFER_DEPTHBUFFER );
	else {
		grLfbEnd();
		return(BRE_DEV_FAIL);
	}

	dest_ptr = dest->pm_pixels;
	dest_ptr += dest->pm_base_x + (dest->pm_base_y * dest->pm_row_bytes / 2);

	guFbReadRegion(self->pm_base_x, self->pm_base_y, self->pm_width, self->pm_height, dest_ptr, dest->pm_row_bytes);

	grLfbEnd();

	return(BRE_OK);
}


static br_error BR_CMETHOD_DECL(br_device_pixelmap_3dfx, doubleBuffer)(br_device_pixelmap *self, br_device_pixelmap *src)
{
	/*
	 * fail if doubleBuffer is not invoked on front/back pair
	 */
	if(self != ObjectDevice(self)->active_buffers[BT_FRONTSCREEN] ||
		src != ObjectDevice(self)->active_buffers[BT_BACKSCREEN])
		return BRE_DEV_UNSUPPORTED;

	/*
	 * Block if there is already a swap outstanding
	 */
	while(grBufferNumPending() != 0)
		;

	/*
	 * Do the flip...
	 */
	grBufferSwap(1);

	grDepthMask(zwrite);

	return BRE_OK;
}




static br_error BR_CMETHOD_DECL(br_device_pixelmap_3dfx, pixelSet)
	(br_device_pixelmap *self, br_point *p, br_uint_32 colour)
{
	br_point ap;
	br_uint_16 *ptr;
	
	if(PixelmapPointClip(&ap, p, (br_pixelmap *)self) == BR_CLIP_REJECT)
		return BRE_OK;

	grLfbBegin();
	grLfbBypassMode(GR_LFBBYPASS_ENABLE);

	if (self->buffer_type == BT_FRONTSCREEN)
		ptr = grLfbGetWritePtr( GR_BUFFER_FRONTBUFFER );
	else if (self->buffer_type == BT_BACKSCREEN)
		ptr = grLfbGetWritePtr( GR_BUFFER_BACKBUFFER );
	else if (self->buffer_type == BT_DEPTH)
		ptr = grLfbGetWritePtr( GR_BUFFER_DEPTHBUFFER );
	else {
		grLfbEnd();
		return(BRE_DEV_FAIL);
	}

	grLfbWriteMode( lfb_write_mode );
	ptr[(ap.x+self->pm_base_x) + ((ap.y+self->pm_base_y) * 1024) ] = (FxU16)colour; 

	grLfbEnd();

	return BRE_OK;
}



static br_error BR_CMETHOD_DECL(br_device_pixelmap_3dfx, pixelQuery)
	(br_device_pixelmap *self, br_uint_32 *pcolour, br_point *p)
{
	return BRE_DEV_UNSUPPORTED;
}



static br_error BR_CMETHOD_DECL(br_device_pixelmap_3dfx, copyBits)
	(br_device_pixelmap *self, br_point *point, br_uint_8 *src, br_uint_16 s_stride, br_rectangle *bit_rect, br_uint_32 colour)
{
	br_rectangle ar;
	br_point ap;
	br_error result;
	br_uint_16 *ptr;
	int bit;

	if(PixelmapCopyBitsClip(&ar, &ap, bit_rect, point, (br_pixelmap *)self) == BR_CLIP_REJECT)
		return BRE_OK;

	grLfbBegin();
	grLfbBypassMode(GR_LFBBYPASS_ENABLE);

	if (self->buffer_type == BT_FRONTSCREEN)
		ptr = grLfbGetWritePtr( GR_BUFFER_FRONTBUFFER );
	else if (self->buffer_type == BT_BACKSCREEN)
		ptr = grLfbGetWritePtr( GR_BUFFER_BACKBUFFER );
	else if (self->buffer_type == BT_DEPTH)
		ptr = grLfbGetWritePtr( GR_BUFFER_DEPTHBUFFER );
	else {
		grLfbEnd();
		return(BRE_DEV_FAIL);
	}

	bit = (ar.x & 7);

	ptr += (ap.x+(ar.x & ~7)-bit+self->pm_base_x) + ((ap.y+self->pm_base_y) * (1024));

	_MemCopyBits3DFX_A(
		ptr, self->pm_pixels_qualifier,
		1024 * 2,
		src + ar.y * s_stride,
		s_stride,
		bit, bit+ar.w,
		ar.h,
		colour);

	grLfbEnd();

	return BRE_OK;
}




static br_error BR_CMETHOD_DECL(br_device_pixelmap_3dfx, rectangleCopy)
	(br_device_pixelmap *self, br_point *p, br_device_pixelmap *src, br_rectangle *r)
{
	br_rectangle ar;
	br_point ap;
	br_uint_16 *ptr, *dest_ptr;

	if (src->buffer_type == BT_SYSMEM)
		return BR_CMETHOD(br_device_pixelmap_3dfx, rectangleCopyTo)(self, p, src, r);

	else if (self->buffer_type == BT_SYSMEM)
		return BR_CMETHOD(br_device_pixelmap_3dfx, rectangleCopyFrom)(src, p, self, r);

	if (self->pm_type != src->pm_type)
		return BRE_DEV_UNSUPPORTED;

	if(PixelmapRectangleClipTwo(&ar, &ap, r, p, (br_pixelmap *)self, (br_pixelmap *)src) == BR_CLIP_REJECT)
		return BRE_OK;

	grLfbBegin();
	grLfbBypassMode(GR_LFBBYPASS_ENABLE);

	if (self->buffer_type == BT_FRONTSCREEN)
		dest_ptr = grLfbGetWritePtr( GR_BUFFER_FRONTBUFFER );
	else if (self->buffer_type == BT_BACKSCREEN)
		dest_ptr = grLfbGetWritePtr( GR_BUFFER_BACKBUFFER );
	else if (self->buffer_type == BT_DEPTH)
		dest_ptr = grLfbGetWritePtr( GR_BUFFER_DEPTHBUFFER );
	else {
		grLfbEnd();
		return(BRE_DEV_FAIL);
	}

	if (src->buffer_type == BT_FRONTSCREEN)
		ptr = grLfbGetReadPtr( GR_BUFFER_FRONTBUFFER );
	else if (src->buffer_type == BT_BACKSCREEN)
		ptr = grLfbGetReadPtr( GR_BUFFER_BACKBUFFER );
	else if (src->buffer_type == BT_DEPTH)
		ptr = grLfbGetReadPtr( GR_BUFFER_DEPTHBUFFER );
	else {
		grLfbEnd();
		return(BRE_DEV_FAIL);
	}

	grLfbWriteMode(GR_LFBWRITEMODE_565);

	ptr += (src->pm_base_x + ar.x) + ((src->pm_base_y + ar.y) * 1024);

	guFbWriteRegion(ap.x+self->pm_base_x, ap.y+self->pm_base_y, ar.w, ar.h, ptr, 1024 * 2);

	grLfbEnd();

	return(BRE_OK);
}


static br_error BR_CMETHOD_DECL(br_device_pixelmap_3dfx, rectangleCopyTo)
	(br_device_pixelmap *self, br_point *p, br_device_pixelmap *src, br_rectangle *r)
{
	br_rectangle ar;
	br_point ap;
	br_int_32 y;
	br_uint_16 *ptr, *dest_ptr;
	br_uint_32 *pd, *ps, c, colour;

	if (self->pm_type != src->pm_type)
		return BRE_DEV_UNSUPPORTED;

	if(PixelmapRectangleClipTwo(&ar, &ap, r, p, (br_pixelmap *)self, (br_pixelmap *)src) == BR_CLIP_REJECT)
		return BRE_OK;

	grLfbBegin();
	grLfbBypassMode(GR_LFBBYPASS_ENABLE);

	if (self->buffer_type == BT_FRONTSCREEN)
		dest_ptr = grLfbGetWritePtr( GR_BUFFER_FRONTBUFFER );
	else if (self->buffer_type == BT_BACKSCREEN)
		dest_ptr = grLfbGetWritePtr( GR_BUFFER_BACKBUFFER );
	else if (self->buffer_type == BT_DEPTH)
		dest_ptr = grLfbGetWritePtr( GR_BUFFER_DEPTHBUFFER );
	else {
		grLfbEnd();
		return(BRE_DEV_FAIL);
	}

	grLfbWriteMode( lfb_write_mode );

	ptr = src->pm_pixels;
	ptr += (src->pm_base_x + ar.x) + ((src->pm_base_y + ar.y) * src->pm_row_bytes / 2);

	guFbWriteRegion(ap.x+self->pm_base_x, ap.y+self->pm_base_y, ar.w, ar.h, ptr, src->pm_row_bytes);

	grLfbEnd();

	return(BRE_OK);
}



static br_error BR_CMETHOD_DECL(br_device_pixelmap_3dfx, rectangleCopyFrom)
	(br_device_pixelmap *self, br_point *p, br_device_pixelmap *dest, br_rectangle *r)
{
	br_rectangle ar;
	br_point ap;
	br_int_32 y;
	br_uint_16 *ptr, *dest_ptr;
	br_uint_32 *pd, *ps, c, colour;

	if (self->buffer_type == BT_DEPTH) {
		if (self->pm_type != dest->pm_type)
			return(BRE_DEV_FAIL);
	} else {
		if (self->pm_type != BR_PMT_RGB_565)
			return(BRE_DEV_FAIL);
	}

	if(PixelmapRectangleClipTwo(&ar, &ap, r, p, (br_pixelmap *)dest, (br_pixelmap *)self) == BR_CLIP_REJECT)
		return BRE_OK;

	grLfbBegin();
	grLfbBypassMode(GR_LFBBYPASS_ENABLE);

	if (self->buffer_type == BT_FRONTSCREEN)
		ptr = (br_uint_16 *)grLfbGetReadPtr( GR_BUFFER_FRONTBUFFER );
	else if (self->buffer_type == BT_BACKSCREEN)
		ptr = (br_uint_16 *)grLfbGetReadPtr( GR_BUFFER_BACKBUFFER );
	else if (self->buffer_type == BT_DEPTH)
		ptr = (br_uint_16 *)grLfbGetReadPtr( GR_BUFFER_DEPTHBUFFER );
	else {
		grLfbEnd();
		return(BRE_DEV_FAIL);
	}

	dest_ptr = dest->pm_pixels;
	dest_ptr += (dest->pm_base_x + ap.x) + ((dest->pm_base_y + ap.y) * dest->pm_row_bytes / 2);

	guFbReadRegion(ar.x+self->pm_base_x, ar.y+self->pm_base_y, ar.w, ar.h, dest_ptr, dest->pm_row_bytes);

	grLfbEnd();

	return(BRE_OK);
}


/* Local buffers for scaling
 *
 * Using one buffer results in long waits while the hardware finishes
 * rendering to the first buffer before it can download the new texture.
 * (frame rate on FX Fighter jumped from 14 fps to 20 by only allowing one buffer
 * send per pass through the loop)
 */
#define SCALE_BUFFERS		2
static struct render_buffer scale_rb[SCALE_BUFFERS];
static br_uint_16 *scalebuffer[SCALE_BUFFERS] = { 0, 0 };
static GrVertex scalevtx[3];
static br_uint_32 current_scalebuffer = 0;

void init_scale_buffer(br_uint_32 i)
{
	if (lfb_write_mode == GR_LFBWRITEMODE_565)
		scale_rb[i].info.format = GR_TEXFMT_RGB_565;
	else
		scale_rb[i].info.format = GR_TEXFMT_ARGB_1555;
	scale_rb[i].info.aspectRatio = GR_ASPECT_1x1;
	scale_rb[i].u_range = BR_SCALAR(255.0);
	scale_rb[i].v_range = BR_SCALAR(255.0);
	scale_rb[i].info.largeLod = GR_LOD_256;
	scale_rb[i].width_p = 256;
	scale_rb[i].width_b = 512;
	scale_rb[i].height = 256;
	scale_rb[i].info.smallLod = GR_LOD_256;
	scale_rb[i].info.data = scalebuffer[i];
	scale_rb[i].tmu_id = GR_TMU0;
	scale_rb[i].on_card = BR_FALSE;
	scale_rb[i].uncacheable = BR_FALSE;
}


extern void StretchCopy16(
			br_uint_16 *out_pixels,
			br_uint_32 out_width, br_uint_32 out_height, br_uint_32 out_row_bytes,
			br_uint_16 *in_pixels,
			br_uint_32 in_width, br_uint_32 in_height, br_uint_32 in_row_bytes,
			br_uint_32 key);


void fill_scalebuffer(br_uint_16 *sptr, br_uint_16 *dptr,
					  br_uint_32 sw, br_uint_32 sh, br_uint_32 srb, br_uint_32 drb)
{
	br_uint_32 i, j;
	br_uint_32 *sp, *dp;

	if (sw & 1) {
		/* Odd width; need to add front or back trim byte;
		 * attempt to align dest for maximum speed */
		if (((br_uint_32) dptr) & 1) {
			/* Front trim */
			for(j=0;j<sh;j++) {
				sp = (br_uint_32 *) sptr;
				dp = (br_uint_32 *) dptr;
				*(br_uint_16 *)dp++ = *(br_uint_16 *)sp++;
				i = sw/2;
				while(i--)
					*dp++ = *sp++;
				sptr += srb;
				dptr += drb;
			}
		} else {
			/* Back trim */
			for(j=0;j<sh;j++) {
				sp = (br_uint_32 *) sptr;
				dp = (br_uint_32 *) dptr;
				i = sw/2;
				while(i--)
					*dp++ = *sp++;
				*(br_uint_16 *)dp++ = *(br_uint_16 *)sp++;
				sptr += srb;
				dptr += drb;
			}
		}

	} else {
		/* Even case: no trim bytes */
		for(j=0;j<sh;j++) {
			sp = (br_uint_32 *) sptr;
			dp = (br_uint_32 *) dptr;
			i = sw/2;
			while(i--)
				*dp++ = *sp++;
			sptr += srb;
			dptr += drb;
		}
	}
}



void copy_scalebuffer(GrTextureFilterMode_t filter)
{
    extern br_error FXCacheTexture(struct render_buffer *src);

	scale_rb[current_scalebuffer].force_reload = 1;
	FXCacheTexture(&scale_rb[current_scalebuffer]);
	grTexSource(scale_rb[current_scalebuffer].tmu_id,
				scale_rb[current_scalebuffer].startAddress,
				GR_MIPMAPLEVELMASK_BOTH,
				&scale_rb[current_scalebuffer].info );
	grTexFilterMode(scale_rb[current_scalebuffer].tmu_id, filter, filter);
	grTexClampMode( scale_rb[current_scalebuffer].tmu_id,
					GR_TEXTURECLAMP_WRAP,
					GR_TEXTURECLAMP_WRAP );
}



br_error BR_CMETHOD_DECL(br_device_pixelmap_3dfx, rectangleStretchCopy)\
	(br_device_pixelmap *self, br_rectangle *d, br_device_pixelmap *src, br_rectangle *s)
{
	br_rectangle srect, drect;
	br_uint_32 key, i;

	if ((s->w == d->w) &&
		(s->h == d->h) &&
		(self->buffer_type != BT_SYSMEM)) {
		br_point p;

		p.x = d->x;
		p.y = d->y;
		return(BR_CMETHOD(br_device_pixelmap_3dfx, rectangleCopyTo)(self, &p, src, s));
	}

	if(PixelmapRectangleClip(&drect, d, (br_pixelmap *)self) == BR_CLIP_REJECT)
		return BRE_OK;
	if(PixelmapRectangleClip(&srect, s, (br_pixelmap *)src) == BR_CLIP_REJECT)
		return BRE_OK;

	if (src->pm_type != self->pm_type)
		return(BRE_DEV_UNSUPPORTED);
	if ((src->pm_type != BR_PMT_RGB_555) &&
		(src->pm_type != BR_PMT_RGB_565))
		return(BRE_DEV_UNSUPPORTED);

	if (src->pm_copy_function == BR_PMCOPY_SRC_KEYED) key = src->pm_src_key.low;
	else key = 0xffff0000;

	if (self->buffer_type == BT_SYSMEM) {
		if (src->buffer_type != BT_SYSMEM)
			return(BRE_DEV_UNSUPPORTED);	/* Don't support stretches from card */

		/* System-system stretch */
		StretchCopy16(((br_uint_16 *)self->pm_pixels) + drect.x + (drect.y*self->pm_row_bytes/2),
					  drect.w, drect.h, self->pm_row_bytes,
					  ((br_uint_16 *)src->pm_pixels) + srect.x + (srect.y*src->pm_row_bytes/2),
					  srect.w, srect.h, src->pm_row_bytes, key);

	} else {
		/* System-card stretch */
#if 0
		br_uint_16 *ptr;

		/* Setup Glide for LFB writes */
		grLfbBegin();
		grLfbBypassMode(GR_LFBBYPASS_ENABLE);

		ptr = grLfbGetWritePtr( GR_BUFFER_BACKBUFFER );
		grLfbWriteMode( lfb_write_mode );

		StretchCopy16(ptr + drect.x + (drect.y*1024),
					  drect.w, drect.h, 2048,
					  ((br_uint_16 *)src->pm_pixels) + srect.x + (srect.y*src->pm_row_bytes/2),
					  srect.w, srect.h, src->pm_row_bytes, key);

		/* Deinit LFB */
		grLfbEnd();
#else
		/* Hardware stretch */
		if (!scalebuffer[0]) {
			/* First time; set up everything */
			for(i=0; i<SCALE_BUFFERS; i++) {
				scalebuffer[i] = BrResAllocate(Device3DfxResource(ObjectDevice(self)),
												256*256*2, BR_MEMORY_DRIVER);
				init_scale_buffer(i);
			}
			scalevtx[0].ooz = 65530.0f;
			scalevtx[1].ooz = 65530.0f;
			scalevtx[2].ooz = 65530.0f;
			scalevtx[0].oow = 1.0f;
			scalevtx[1].oow = 1.0f;
			scalevtx[2].oow = 1.0f;
			scalevtx[0].tmuvtx[0].oow = 1.0f;
			scalevtx[1].tmuvtx[0].oow = 1.0f;
			scalevtx[2].tmuvtx[0].oow = 1.0f;
		}

		/* WARNING: possible state corruption */
		grTexCombineFunction(0,GR_TEXTURECOMBINE_DECAL);
		guColorCombineFunction(GR_COLORCOMBINE_DECAL_TEXTURE);
		/*
		 * Removed by Matt G as its a pile of shite and lies,
		 * at least in the specific use in Croc!
		 * grHints(GR_HINT_STWHINT,  GR_STWHINT_W_DIFF_TMU0 );
		 */
		grAlphaBlendFunction(GR_BLEND_ONE, GR_BLEND_ZERO,
							 GR_BLEND_ONE, GR_BLEND_ZERO);

		{
		br_uint_32 sx, sy, sw, sh, xscale;
		br_uint_32 dx, dy, dw, dh, aspect;
		br_uint_16 *sptr, *dptr;
		GrTextureFilterMode_t filter;
		float fdx, fdy, fdw, fdh;


		sx = 0;	/* Offsets from start */
		sy = 0;
		dx = 0;
		dy = 0;
		xscale = (drect.w << 16) / srect.w;

		sh = srect.h;
		if (sh > 256) sh = 256;
		dh = drect.h;

		grDepthBufferMode(GR_DEPTHBUFFER_DISABLE);
		if (src->pm_copy_function == BR_PMCOPY_SRC_KEYED) {
			grChromakeyMode(GR_CHROMAKEY_ENABLE);
#if 0
			key =	((src->pm_src_key.low & 0xf800) << 8) |
					((src->pm_src_key.low & 0x07e0) << 5) |
					((src->pm_src_key.low & 0x001f) << 3) ;
			grChromakeyValue(key);
#endif
		} else
			grChromakeyMode(GR_CHROMAKEY_DISABLE);

		/* Disabled because it causes seaming and chromakey problems */
//		filter = GR_TEXTUREFILTER_BILINEAR,
		filter = GR_TEXTUREFILTER_POINT_SAMPLED;

		while (1) {
			sw = srect.w - sx;

			/* Stop if run out of data */
			if (sw <= 0)
				break;

			/* Max width in one go is 256x256 */
			if (sw > 256) {
				/* OK try to be devious; make the FIRST block sent the small - and
				 * therefore fast - one. */
				if (((srect.w - sx) - 256) <= 128) {
					sw = (srect.w - sx) - 256;
				} else
					sw = 256;
			}

			/* Fix up aspect ratio to reduce data sent to card */
			if (sw <= 32) {
				aspect = 8;
				scale_rb[current_scalebuffer].info.aspectRatio = GR_ASPECT_1x8;
			} else if (sw <= 32) {
				aspect = 4;
				scale_rb[current_scalebuffer].info.aspectRatio = GR_ASPECT_1x4;
			} else if (sw <= 128) {
				aspect = 2;
				scale_rb[current_scalebuffer].info.aspectRatio = GR_ASPECT_1x2;
			} else {
				aspect = 1;
				scale_rb[current_scalebuffer].info.aspectRatio = GR_ASPECT_1x1;
			}

			dw = (sw * xscale) >> 16;

			/* Copy from source pixelmap to texture */
		    sptr = ((br_uint_16 *)src->pm_pixels) +
								(srect.x+sx) +
								((srect.y+sy)*src->pm_row_bytes/2),
			dptr = scalebuffer[current_scalebuffer];
			fill_scalebuffer(sptr, dptr, sw, sh, src->pm_row_bytes/2, 256/aspect);

			/* Copy Texture across to card*/
			copy_scalebuffer(filter);

			/* Build vertices */
			fdx = (float) (drect.x + dx);
			fdy = (float) (drect.y + dy);
			fdw = (float) dw;
			fdh = (float) dh;
			scalevtx[0].x = fdx;
			scalevtx[0].y = fdy;
			scalevtx[0].tmuvtx[0].sow = 0.0f;
			scalevtx[0].tmuvtx[0].tow = 0.0f;
			scalevtx[1].x = fdx + fdw;
			scalevtx[1].y = fdy;
			scalevtx[1].tmuvtx[0].sow = (float) sw;
			scalevtx[1].tmuvtx[0].tow = 0.0f;
			scalevtx[2].x = fdx + fdw;
			scalevtx[2].y = fdy + fdh;
			scalevtx[2].tmuvtx[0].sow = (float) sw;
			scalevtx[2].tmuvtx[0].tow = (float) sh;

			grDrawTriangle(scalevtx+0, scalevtx+1, scalevtx+2);

			scalevtx[0].x = fdx;
			scalevtx[0].y = fdy;
			scalevtx[0].tmuvtx[0].sow = 0.0f;
			scalevtx[0].tmuvtx[0].tow = 0.0f;
			scalevtx[1].x = fdx + fdw;
			scalevtx[1].y = fdy + fdh;
			scalevtx[1].tmuvtx[0].sow = (float) sw;
			scalevtx[1].tmuvtx[0].tow = (float) sh;
			scalevtx[2].x = fdx;
			scalevtx[2].y = fdy + fdh;
			scalevtx[2].tmuvtx[0].sow = 0.0f;
			scalevtx[2].tmuvtx[0].tow = (float) sh;

			grDrawTriangle(scalevtx+0, scalevtx+1, scalevtx+2);

			sx += sw;
			dx += dw;
			current_scalebuffer = (current_scalebuffer+1) % SCALE_BUFFERS;
//break;
		}
		grChromakeyValue(0);
		grDepthBufferMode(GR_DEPTHBUFFER_ZBUFFER);
		}


#endif
	}

    return(BRE_OK);
}


br_error BR_CMETHOD_DECL(br_device_pixelmap_3dfx, allocateSub)\
	(br_device_pixelmap *self, br_device_pixelmap **newpm, br_rectangle *rect)
{
	br_device_pixelmap   *pm;
	br_rectangle         out;

	/*
	 * Create the new structure and copy
	 */
	pm = BrResAllocate(Device3DfxResource(Object3DfxDevice(self)),sizeof(*pm),BR_MEMORY_PIXELMAP);

	/*
	 * Set all the fields to be the same as the parent pixelmap for now
	 */
	*pm = *self;

	/*
	 * Create sub-window (clipped against original)
	 */
	if(PixelmapRectangleClip(&out, rect, (br_pixelmap *)self) == BR_CLIP_REJECT)
		return BRE_FAIL;

	pm->sub_pixelmap = BR_TRUE;

	/*
	 * Pixel rows are not contiguous
	 */
	if (out.w != self->pm_width)
		pm->pm_flags &= ~BR_PMF_LINEAR;

	pm->pm_base_x += out.x;
	pm->pm_base_y += out.y;

	pm->pm_width   = out.w;
	pm->pm_height  = out.h;

	pm->pm_origin_x = 0;
	pm->pm_origin_y = 0;

	*newpm = (br_device_pixelmap *)pm;

	return BRE_OK;
}


static br_error BR_CMETHOD_DECL(br_device_pixelmap_3dfx, rectangleStretchCopyTo)\
	(br_device_pixelmap *self, br_rectangle *d, br_device_pixelmap *src, br_rectangle *s)
{
	BR_CMETHOD(br_device_pixelmap_3dfx, rectangleFill)(self, d, 0);
    return(BRE_DEV_UNSUPPORTED);
}

static br_error BR_CMETHOD_DECL(br_device_pixelmap_3dfx, rectangleStretchCopyFrom)\
	(br_device_pixelmap *self, br_rectangle *d, br_device_pixelmap *src, br_rectangle *s)
{
    return(BRE_DEV_UNSUPPORTED);
}



static br_error BR_CMETHOD_DECL(br_device_pixelmap_3dfx, directLock)(br_device_pixelmap *self, br_boolean block)
{
	br_uint_16 *ptr;

	grLfbBegin();
	grLfbBypassMode(GR_LFBBYPASS_ENABLE);

	if (self->buffer_type == BT_FRONTSCREEN)
		ptr = grLfbGetWritePtr( GR_BUFFER_FRONTBUFFER );
	else if (self->buffer_type == BT_BACKSCREEN)
		ptr = grLfbGetWritePtr( GR_BUFFER_BACKBUFFER );
	else if (self->buffer_type == BT_DEPTH)
		ptr = grLfbGetWritePtr( GR_BUFFER_DEPTHBUFFER );
	else {
		grLfbEnd();
		return(BRE_DEV_FAIL);
	}

	self->pm_pixels = ptr + self->pm_base_x + (self->pm_base_y*1024);
	self->pm_row_bytes = 2048;

	grLfbWriteMode( lfb_write_mode );

    return(BRE_OK);
}

static br_error BR_CMETHOD_DECL(br_device_pixelmap_3dfx, directUnlock)(br_device_pixelmap *self)
{
	grLfbEnd();

	self->pm_pixels = NULL;
	self->pm_row_bytes = 0;

    return(BRE_OK);
}



/*
 * Default dispatch table for device pixelmap
 */
static struct br_device_pixelmap_dispatch devicePixelmapDispatch = {
	NULL,
	NULL,
	NULL,
	NULL,
	BR_CMETHOD_REF(br_device_pixelmap_3dfx, free),
	BR_CMETHOD_REF(br_object_3dfx,			identifier),
	BR_CMETHOD_REF(br_device_pixelmap_3dfx,	type),
	BR_CMETHOD_REF(br_device_pixelmap_3dfx,	isType),
	BR_CMETHOD_REF(br_object_3dfx,			device),
	BR_CMETHOD_REF(br_device_pixelmap_3dfx,	space),

	BR_CMETHOD_REF(br_device_pixelmap_3dfx,	queryTemplate),
	BR_CMETHOD_REF(br_object,				query),
	BR_CMETHOD_REF(br_object,				queryBuffer),
	BR_CMETHOD_REF(br_object,				queryMany),
	BR_CMETHOD_REF(br_object,				queryManySize),
	BR_CMETHOD_REF(br_object,				queryAll),
	BR_CMETHOD_REF(br_object,				queryAllSize),

	BR_CMETHOD_REF(br_device_pixelmap_mem,	validSource),
	BR_CMETHOD_REF(br_device_pixelmap_3dfx,	resize),
	BR_CMETHOD_REF(br_device_pixelmap_3dfx,	match),
	BR_CMETHOD_REF(br_device_pixelmap_3dfx,	allocateSub),

	BR_CMETHOD_REF(br_device_pixelmap_3dfx,	copy),
	BR_CMETHOD_REF(br_device_pixelmap_3dfx,	copyTo),
	BR_CMETHOD_REF(br_device_pixelmap_3dfx,	copyFrom),
	BR_CMETHOD_REF(br_device_pixelmap_3dfx,	fill),
	BR_CMETHOD_REF(br_device_pixelmap_3dfx,	doubleBuffer),

	BR_CMETHOD_REF(br_device_pixelmap_gen,	copyDirty),
	BR_CMETHOD_REF(br_device_pixelmap_gen,	copyToDirty),
	BR_CMETHOD_REF(br_device_pixelmap_gen,	copyFromDirty),
	BR_CMETHOD_REF(br_device_pixelmap_gen,	fillDirty),
	BR_CMETHOD_REF(br_device_pixelmap_gen,	doubleBufferDirty),

	BR_CMETHOD_REF(br_device_pixelmap_gen,	rectangle),
	BR_CMETHOD_REF(br_device_pixelmap_gen,	rectangle2),
	BR_CMETHOD_REF(br_device_pixelmap_3dfx,	rectangleCopy),
	BR_CMETHOD_REF(br_device_pixelmap_3dfx,	rectangleCopyTo),
	BR_CMETHOD_REF(br_device_pixelmap_3dfx,	rectangleCopyFrom),
    BR_CMETHOD_REF(br_device_pixelmap_3dfx,	rectangleStretchCopy),
    BR_CMETHOD_REF(br_device_pixelmap_3dfx,	rectangleStretchCopyTo),
    BR_CMETHOD_REF(br_device_pixelmap_3dfx,	rectangleStretchCopyFrom),
	BR_CMETHOD_REF(br_device_pixelmap_3dfx,	rectangleFill),
	BR_CMETHOD_REF(br_device_pixelmap_3dfx,	pixelSet),
	BR_CMETHOD_REF(br_device_pixelmap_gen,	line),
	BR_CMETHOD_REF(br_device_pixelmap_3dfx,	copyBits),


	BR_CMETHOD_REF(br_device_pixelmap_gen,	text),
	BR_CMETHOD_REF(br_device_pixelmap_gen,	textBounds),

	BR_CMETHOD_REF(br_device_pixelmap_mem,	rowSize),
	BR_CMETHOD_REF(br_device_pixelmap_mem,	rowQuery),
	BR_CMETHOD_REF(br_device_pixelmap_mem,	rowSet),

	BR_CMETHOD_REF(br_device_pixelmap_3dfx,	pixelQuery),
	BR_CMETHOD_REF(br_device_pixelmap_null, pixelAddressQuery),

	BR_CMETHOD_REF(br_device_pixelmap_null, pixelAddressSet),
	BR_CMETHOD_REF(br_device_pixelmap_mem,	originSet),

	BR_CMETHOD_REF(br_device_pixelmap_fix,	flush),
	BR_CMETHOD_REF(br_device_pixelmap_fix,	synchronise),
    BR_CMETHOD_REF(br_device_pixelmap_3dfx, directLock),
    BR_CMETHOD_REF(br_device_pixelmap_3dfx, directUnlock),

	BR_CMETHOD_REF(br_device_pixelmap_gen,	getControls),
	BR_CMETHOD_REF(br_device_pixelmap_gen,	setControls),
};

