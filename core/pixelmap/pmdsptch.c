/*
 * Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: pmdsptch.c 1.2 1998/08/10 16:12:21 johng Exp $
 * $Locker: $
 *
 * Convert high level br_pixelmap ops. to br_device_pixelmap methods
 *
 * Main concerns are shuffling arguments and clipping
 */
#define BR_DEVICE_PIXELMAP_PRIVATE

#include "pm.h"
#include "brassert.h"

#include "pmmem.h"

BR_RCS_ID("$Id: pmdsptch.c 1.2 1998/08/10 16:12:21 johng Exp $")

/*
 * Comatibility hack to catch any pixelmaps with NULL dispatch pointers and
 * redirect them to the memory dispatch table
 */
#if 1
extern void _CheckDispatch(br_device_pixelmap *pm);
#define CheckDispatch(x) _CheckDispatch((br_device_pixelmap *)(x))
#else
#define 
CheckDispatch(x)
#endif

br_pixelmap * BR_PUBLIC_ENTRY BrPixelmapAllocateSub(br_pixelmap *src,
									br_int_32 x, br_int_32 y,
									br_int_32 w, br_int_32 h)
{
	br_pixelmap *new;
	br_rectangle r;

	r.x = x; r.y = y;
	r.w = w; r.h = h;

	CheckDispatch(src);

	if(DevicePixelmapAllocateSub(src, (br_device_pixelmap **)&new, &r))
		return NULL;

	return new;
}

void BR_PUBLIC_ENTRY BrPixelmapFree(br_pixelmap *src)
{
	UASSERT(src);

	CheckDispatch(src);

#if 1
	/*
	 * Gross V1.1 compatibity problem
	 *
 	 * If user free's the current DosGfxBegin() pixelmap - 
	 * mark it as unused
	 */
	if(BrDevLastBeginQuery() == src)
		BrDevLastBeginSet(NULL);
#endif

	ObjectFree(src);
}

br_pixelmap * BR_PUBLIC_ENTRY BrPixelmapResize(br_pixelmap *src,
	br_int_32 width, br_int_32 height)
{
	UASSERT(src);

	CheckDispatch(src);

	if(DevicePixelmapResize(src, width, height))
		return NULL;

	return src;
}

br_pixelmap * BR_PUBLIC_ENTRY BrPixelmapMatch(br_pixelmap *src,
	br_uint_8 match_type)
{
	br_pixelmap *new;

	br_token_value tv[] = {
		{BRT_USE_T,	0},
		{0,			0},
		{0,}};

	UASSERT_MESSAGE("Invalid match type", match_type < BR_PMMATCH_MAX);

	CheckDispatch(src);

	switch(match_type) {

	case BR_PMMATCH_OFFSCREEN:
		tv[0].v.t = BRT_OFFSCREEN;
		break;

	case BR_PMMATCH_HIDDEN:
		tv[0].v.t = BRT_HIDDEN;
		break;

	case BR_PMMATCH_HIDDEN_BUFFER:
		tv[0].v.t = BRT_HIDDEN_BUFFER;
		break;

	case BR_PMMATCH_NO_RENDER:
		tv[0].v.t = BRT_NO_RENDER;
		break;

 	case BR_PMMATCH_DEPTH_8:
		tv[0].v.t = BRT_DEPTH;
		tv[1].t = BRT_PIXEL_BITS_I32;
		tv[1].v.i32 = 8;
		break;

 	case BR_PMMATCH_DEPTH_16:
		tv[0].v.t = BRT_DEPTH;
		tv[1].t = BRT_PIXEL_BITS_I32;
		tv[1].v.i32 = 16;
		break;

 	case BR_PMMATCH_DEPTH_32:
		tv[0].v.t = BRT_DEPTH;
		tv[1].t = BRT_PIXEL_BITS_I32;
		tv[1].v.i32 = 32;
		break;

 	case BR_PMMATCH_DEPTH_15:
		tv[0].v.t = BRT_DEPTH_15;
		tv[1].t = BRT_PIXEL_BITS_I32;
		tv[1].v.i32 = 16;
		break;

 	case BR_PMMATCH_DEPTH_31:
		tv[0].v.t = BRT_DEPTH_31;
		tv[1].t = BRT_PIXEL_BITS_I32;
		tv[1].v.i32 = 32;
		break;

 	case BR_PMMATCH_DEPTH_FP16:
		tv[0].v.t = BRT_DEPTH_FP16;
		tv[1].t = BRT_PIXEL_BITS_I32;
		tv[1].v.i32 = 16;
		break;

 	case BR_PMMATCH_DEPTH_FP15:
		tv[0].v.t = BRT_DEPTH_FP15;
		tv[1].t = BRT_PIXEL_BITS_I32;
		tv[1].v.i32 = 16;
		break;

	case BR_PMMATCH_DEPTH:
		tv[0].v.t = BRT_DEPTH;
		break;
	}

	if(DevicePixelmapMatch(src, (br_device_pixelmap **)&new, tv))
		return NULL;

	return new;
}

br_pixelmap * BR_PUBLIC_ENTRY BrPixelmapMatchSized(br_pixelmap *src,
	br_uint_8 match_type, br_int_32 width, br_int_32 height)
{
	br_pixelmap *new;

	br_token_value tv[] = {
		{BRT_USE_T,			0},
		{BRT_WIDTH_I32,		0},
		{BRT_HEIGHT_I32,	0},
		{0,					0},
		{0 }};

	UASSERT_MESSAGE("Invalid match type", match_type < BR_PMMATCH_MAX);

	CheckDispatch(src);

	tv[1].v.i32 = width;
	tv[2].v.i32 = height;

	switch(match_type) {

	case BR_PMMATCH_OFFSCREEN:
		tv[0].v.t = BRT_OFFSCREEN;
		break;

	case BR_PMMATCH_HIDDEN:
		tv[0].v.t = BRT_HIDDEN;
		break;

	case BR_PMMATCH_HIDDEN_BUFFER:
		tv[0].v.t = BRT_HIDDEN_BUFFER;
		break;

	case BR_PMMATCH_NO_RENDER:
		tv[0].v.t = BRT_NO_RENDER;
		break;

	case BR_PMMATCH_DEPTH_16:
		tv[3].t = BRT_PIXEL_BITS_I32;
		tv[3].v.i32 = 16;
		/* FALL THROUGH */

	case BR_PMMATCH_DEPTH:
		tv[0].v.t = BRT_DEPTH;
		break;
	}

	if(DevicePixelmapMatch(src, (br_device_pixelmap **)&new, tv))
		return NULL;

	if (new->width != width || new->height != height) {

		BrPixelmapFree(new);
		return NULL;
	}

	return new;
}

br_pixelmap * BR_PUBLIC_ENTRY BrPixelmapMatchTyped(br_pixelmap *src,
	br_uint_8 match_type, br_uint_8 pixelmap_type)
{
	br_pixelmap *new;

	br_token_value tv[] = {
		{BRT_USE_T,        0},
		{BRT_PIXEL_TYPE_U8, 0},
		{0,                0},
		{0,}};

	UASSERT_MESSAGE("Invalid match type", match_type < BR_PMMATCH_MAX);
	UASSERT_MESSAGE("Invalid pixelmap type", pixelmap_type < BR_PMT_MAX);

	CheckDispatch(src);

	tv[1].v.u8 = pixelmap_type;

	switch(match_type) {

	case BR_PMMATCH_OFFSCREEN:
		tv[0].v.t = BRT_OFFSCREEN;
		break;

	case BR_PMMATCH_HIDDEN:
		tv[0].v.t = BRT_HIDDEN;
		break;

	case BR_PMMATCH_HIDDEN_BUFFER:
		tv[0].v.t = BRT_HIDDEN_BUFFER;
		break;

	case BR_PMMATCH_NO_RENDER:
		tv[0].v.t = BRT_NO_RENDER;
		break;

	case BR_PMMATCH_DEPTH_16:
		tv[2].t = BRT_PIXEL_BITS_I32;
		tv[2].v.i32 = 16;

		UASSERT_MESSAGE("Pixelmap type incompatible with match type", pixelmap_type == BR_PMT_DEPTH_16);

		/* FALL THROUGH */

	case BR_PMMATCH_DEPTH:
		tv[0].v.t = BRT_DEPTH;

		UASSERT_MESSAGE("Pixelmap type incompatible with match type", (pixelmap_type == BR_PMT_DEPTH_16) || (pixelmap_type == BR_PMT_DEPTH_32));

		break;
	}

	if(DevicePixelmapMatch(src, (br_device_pixelmap **)&new, tv))
		return NULL;

	if (new->type != pixelmap_type) {

		BrPixelmapFree(new);
		return NULL;
	}

	return new;
}

br_pixelmap * BR_PUBLIC_ENTRY BrPixelmapMatchTypedSized(br_pixelmap *src,
	br_uint_8 match_type, br_uint_8 pixelmap_type, br_int_32 width, br_int_32 height)
{
	br_pixelmap *new;

	br_token_value tv[] = {
		{BRT_USE_T,			0},
		{BRT_WIDTH_I32,		0},
		{BRT_HEIGHT_I32,	0},
		{BRT_PIXEL_TYPE_U8, 0},
		{0,					0},
		{0 }};

	UASSERT_MESSAGE("Invalid match type", match_type < BR_PMMATCH_MAX);
	UASSERT_MESSAGE("Invalid pixelmap type", pixelmap_type < BR_PMT_MAX);

	CheckDispatch(src);

	tv[3].v.u8 = pixelmap_type;
	tv[1].v.i32 = width;
	tv[2].v.i32 = height;

	switch(match_type) {

	case BR_PMMATCH_OFFSCREEN:
		tv[0].v.t = BRT_OFFSCREEN;
		break;

	case BR_PMMATCH_HIDDEN:
		tv[0].v.t = BRT_HIDDEN;
		break;

	case BR_PMMATCH_HIDDEN_BUFFER:
		tv[0].v.t = BRT_HIDDEN_BUFFER;
		break;

	case BR_PMMATCH_NO_RENDER:
		tv[0].v.t = BRT_NO_RENDER;
		break;

	case BR_PMMATCH_DEPTH_16:
		tv[4].t = BRT_PIXEL_BITS_I32;
		tv[4].v.i32 = 16;

		UASSERT_MESSAGE("Pixelmap type incompatible with match type", pixelmap_type == BR_PMT_DEPTH_16);

		/* FALL THROUGH */

	case BR_PMMATCH_DEPTH:
		tv[0].v.t = BRT_DEPTH;

		UASSERT_MESSAGE("Pixelmap type incompatible with match type", (pixelmap_type == BR_PMT_DEPTH_16) || (pixelmap_type == BR_PMT_DEPTH_32));

		break;
	}

	if(DevicePixelmapMatch(src, (br_device_pixelmap **)&new, tv))
		return NULL;

	if (new->type != pixelmap_type || new->width != width || new->height != height) {

		BrPixelmapFree(new);
		return NULL;
	}

	return new;
}

br_pixelmap * BR_PUBLIC_ENTRY BrPixelmapClone(br_pixelmap *src)
{
	br_pixelmap *new;

	br_token_value tv[] = {
		{BRT_USE_T,	BRT_CLONE},
		{0}};

	CheckDispatch(src);


	if(DevicePixelmapMatch(src, (br_device_pixelmap **)&new, tv))
		return NULL;

	return new;
}

void BR_PUBLIC_ENTRY BrPixelmapFill(br_pixelmap *dst, br_uint_32 colour)
{
	CheckDispatch(dst);

	DevicePixelmapFill(dst,colour);
}

void BR_PUBLIC_ENTRY BrPixelmapRectangle(br_pixelmap *dst,
	br_int_32 x,br_int_32 y,br_int_32 w,br_int_32 h,
	br_uint_32 colour)
{
	br_rectangle r;

	CheckDispatch(dst);

	r.x = x; r.y = y;
	r.w = w; r.h = h;

	DevicePixelmapRectangle(dst, &r, colour);
}

void BR_PUBLIC_ENTRY BrPixelmapRectangle2(br_pixelmap *dst,
	br_int_32 x,br_int_32 y,br_int_32 w,br_int_32 h,
	br_uint_32 colour_tl, br_uint_32 colour_br)
{
	br_rectangle r;

	CheckDispatch(dst);

	r.x = x; r.y = y;
	r.w = w; r.h = h;

	DevicePixelmapRectangle2(dst, &r, colour_tl, colour_br);
}

br_error DispatchCopy(br_device_pixelmap *self, br_device_pixelmap *src)
{
	CheckDispatch(self);
	CheckDispatch(src);

	/*
	 * Invoke one of three driver functions
	 *	device -> device
	 *	device -> memory
	 *	memory -> device
	 *	or Generic handler
	 */
	if(ObjectDevice(self) == ObjectDevice(src))
		/*
		 * If the same device, just let the device get on with it
		 */
		return DevicePixelmapCopy(self, (br_device_pixelmap *)src);
	else if(!(src->pm_flags & BR_PMF_NO_ACCESS))
		/*
		 * Spot case of copying to device (from addressable memory)
		 */
		return DevicePixelmapCopyTo(self, (br_device_pixelmap *)src);
	else if(!(self->pm_flags & BR_PMF_NO_ACCESS))
		/*
		 * Spot case of copying from device (to addressable memory)
		 */
		return DevicePixelmapCopyFrom(src, (br_device_pixelmap *)self);
	else
		/*
		 * Otherwise use a general copy with intermediate buffer
		 */
		return GeneralCopy((br_device_pixelmap *)self, (br_device_pixelmap *)src);
}


br_error DispatchRectangleCopy(br_device_pixelmap *self, br_point *p, br_device_pixelmap *src, br_rectangle *r)
{
	CheckDispatch(self);
	CheckDispatch(src);

	/*
	 * Invoke one of three driver functions
	 *	device -> device
	 *	device -> memory
	 *	memory -> device
	 *	or Generic handler
	 */
	if(ObjectDevice(self) == ObjectDevice(src))
		/*
		 * If the same device, just let the device get on with it
		 */
		return DevicePixelmapRectangleCopy(self, p, (br_device_pixelmap *)src, r);
	else if(!(src->pm_flags & BR_PMF_NO_ACCESS))
		/*
		 * Spot case of copying to device (from addressable memory)
		 */
		return DevicePixelmapRectangleCopyTo(self, p, (br_device_pixelmap *)src, r);
	else if(!(self->pm_flags & BR_PMF_NO_ACCESS))
		/*
		 * Spot case of copying from device (to addressable memory)
		 */
		return DevicePixelmapRectangleCopyFrom(src, p, (br_device_pixelmap *)self, r);
	else
		/*
		 * Otherwise use a general copy with intermediate buffer
		 */
		return GeneralRectangleCopy((br_device_pixelmap *)self, p, (br_device_pixelmap *)src, r);
}


br_error DispatchRectangleStretchCopy(br_device_pixelmap *self, br_rectangle *r, br_device_pixelmap *src, br_rectangle *s)
{
	CheckDispatch(self);
	CheckDispatch(src);

	/*
	 * Invoke one of three driver functions
	 *	device -> device
	 *	device -> memory
	 *	memory -> device
	 *	or Generic handler
	 */
	if(ObjectDevice(self) == ObjectDevice(src))
		/*
		 * If the same device, just let the device get on with it
		 */
		return DevicePixelmapRectangleStretchCopy(self, r, (br_device_pixelmap *)src, s);
	else if(!(src->pm_flags & BR_PMF_NO_ACCESS))
		/*
		 * Spot case of copying to device (from addressable memory)
		 */
		return DevicePixelmapRectangleStretchCopyTo(self, r, (br_device_pixelmap *)src, s);
	else if(!(self->pm_flags & BR_PMF_NO_ACCESS))
		/*
		 * Spot case of copying from device (to addressable memory)
		 */
		return DevicePixelmapRectangleStretchCopyFrom(src, r, (br_device_pixelmap *)self, s);
	else
		/*
		 * Otherwise use a general copy with intermediate buffer
		 */
		return GeneralRectangleStretchCopy((br_device_pixelmap *)self, r, (br_device_pixelmap *)src, s);
}



void BR_PUBLIC_ENTRY BrPixelmapRectangleCopy(br_pixelmap *dst,
	br_int_32 dx,br_int_32 dy,
	br_pixelmap *src,br_int_32 sx,br_int_32 sy,br_int_32 w,br_int_32 h)
{
	br_rectangle r;
	br_point p;

	r.x = sx;
	r.y = sy;
	r.w = w;
	r.h = h;

	p.x = dx;
	p.y = dy;

	DispatchRectangleCopy((br_device_pixelmap *)dst, &p, (br_device_pixelmap *)src, &r);
}

void BR_PUBLIC_ENTRY BrPixelmapRectangleStretchCopy(
	br_pixelmap *dst, br_int_32 dx, br_int_32 dy, br_int_32 dw, br_int_32 dh,
	br_pixelmap *src, br_int_32 sx, br_int_32 sy, br_int_32 sw, br_int_32 sh)
{
	br_rectangle s;
	br_rectangle d;

	s.x = sx;
	s.y = sy;
	s.w = sw;
	s.h = sh;

	d.x = dx;
	d.y = dy;
	d.w = dw;
	d.h = dh;

	DispatchRectangleStretchCopy((br_device_pixelmap *)dst, &d, (br_device_pixelmap *)src, &s);
}


void BR_PUBLIC_ENTRY BrPixelmapRectangleFill(br_pixelmap *dst,
	br_int_32 x,br_int_32 y,br_int_32 w,br_int_32 h,br_uint_32 colour)
{
	br_rectangle r;

	CheckDispatch(dst);

	r.x = x; r.y = y;
	r.w = w; r.h = h;

	DevicePixelmapRectangleFill(dst, &r, colour);
}

void BR_PUBLIC_ENTRY BrPixelmapDirtyRectangleCopy(br_pixelmap *dst,
	br_pixelmap *src,br_int_32 x,br_int_32 y,br_int_32 w,br_int_32 h)
{
	br_rectangle r;
	br_point p;

	UASSERT((src->width == dst->width) &&
	        (src->height == dst->height));

	CheckDispatch(dst);
	CheckDispatch(src);

	p.x = x; p.y = y;
	r.x = x; r.y = y;
	r.w = w; r.h = h;


	/*
	 * Invoke one of three driver functions
	 *	device -> device
	 *	device -> memory
	 *	memory -> device
	 *	or Generic handler
	 */
	if(ObjectDevice(dst) == ObjectDevice(src))
		/*
		 * If the same device, just let the device get on with it
		 */
		DevicePixelmapCopyDirty(dst, (br_device_pixelmap *)src, &r, 1);
	else if(!(src->flags & BR_PMF_NO_ACCESS))
		/*
		 * Spot case of copying to device (from addressable memory)
		 */
		DevicePixelmapCopyToDirty(dst, (br_device_pixelmap *)src, &r, 1);

	else if(!(dst->flags & BR_PMF_NO_ACCESS))
		/*
		 * Spot case of copying from device (to addressable memory)
		 */
		DevicePixelmapCopyFromDirty(src, (br_device_pixelmap *)dst, &r, 1);
	else 
		/*
		 * Otherwise use a general copy with intermediate buffer
		 */
		GeneralRectangleCopy((br_device_pixelmap *)dst, &p, (br_device_pixelmap *)src, &r);
}

void BR_PUBLIC_ENTRY BrPixelmapDirtyRectangleClear(br_pixelmap *dst,
	br_int_32 x,br_int_32 y,br_int_32 w,br_int_32 h,br_uint_32 colour)
{
	br_rectangle r;

	CheckDispatch(dst);

	r.x = x; r.y = y;
	r.w = w; r.h = h;

	DevicePixelmapFillDirty((br_device_pixelmap *)dst, colour, &r, 1);
}

void BR_PUBLIC_ENTRY BrPixelmapDirtyRectangleDoubleBuffer(br_pixelmap *dst,
	br_pixelmap *src,br_int_32 x,br_int_32 y,br_int_32 w,br_int_32 h)
{
	br_rectangle r;

	CheckDispatch(dst);

	r.x = x; r.y = y;
	r.w = w; r.h = h;

	DevicePixelmapDoubleBufferDirty((br_device_pixelmap *)dst, (br_device_pixelmap *)src, &r, 1);
}

void BR_PUBLIC_ENTRY BrPixelmapPixelSet(br_pixelmap *dst,
	br_int_32 x,br_int_32 y, br_uint_32 colour)
{
	br_point p;

	CheckDispatch(dst);

	p.x = x;
	p.y = y;

	DevicePixelmapPixelSet(dst, &p, colour);
}

br_uint_32 BR_PUBLIC_ENTRY BrPixelmapPixelGet(br_pixelmap *dst,
	br_int_32 x,br_int_32 y)
{
	br_point p;
	br_uint_32 colour;

	CheckDispatch(dst);

	p.x = x;
	p.y = y;

	DevicePixelmapPixelQuery(dst, &colour, &p);

	return colour;
}

void BR_PUBLIC_ENTRY BrPixelmapCopy(br_pixelmap *dst,br_pixelmap *src)
{
	br_rectangle s;
	br_rectangle d;

	CheckDispatch(dst);
	CheckDispatch(src);

	if ((dst->width != src->width) || (dst->height != src->height)) {

		s.x = -src->origin_x;
		s.y = -src->origin_y;
		s.w = src->width;
		s.h = src->height;

		d.x = -dst->origin_x;
		d.y = -dst->origin_y;
		d.w = dst->width;
		d.h = dst->height;

		DispatchRectangleStretchCopy((br_device_pixelmap *)dst, &d, (br_device_pixelmap *)src, &s);
		return;
	}

	DispatchCopy((br_device_pixelmap *)dst, (br_device_pixelmap *)src);
}

void BR_PUBLIC_ENTRY BrPixelmapLine(br_pixelmap *dst,
	br_int_32 x1, br_int_32 y1,
	br_int_32 x2, br_int_32 y2, br_uint_32 colour)
{
	br_point s,e;

	CheckDispatch(dst);

	s.x = x1;
	s.y = y1;
	e.x = x2;
	e.y = y2;

	DevicePixelmapLine(dst, &s, &e, colour);
}

void BR_PUBLIC_ENTRY BrPixelmapDoubleBuffer(br_pixelmap *dst,
	br_pixelmap *src)
{
	CheckDispatch(dst);
	CheckDispatch(src);

	DevicePixelmapDoubleBuffer(dst, (br_device_pixelmap *)src);
}

void BR_PUBLIC_ENTRY BrPixelmapText(br_pixelmap *dst,
	br_int_32 x, br_int_32 y,br_uint_32 colour, br_font *font, char *text)
{
	br_point p;

	CheckDispatch(dst);

	if(font == NULL)
		font = BrFontProp7x9;

	p.x = x;
	p.y = y;
	DevicePixelmapText(dst, &p, font, text, colour);
}

void BR_PUBLIC_ENTRY BrPixelmapTextF(br_pixelmap *dst,
	br_int_32 x, br_int_32 y,br_uint_32 colour,	br_font *font, char *fmt,...)
{
	char *ss = BrScratchString();
	br_point p;
 	va_list args;

	CheckDispatch(dst);

	if(font == NULL)
		font = BrFontProp7x9;

	/*
	 * Build output string
	 */
	va_start(args,fmt);
	BrVSprintfN(ss, BrScratchStringSize(), fmt, args);
	va_end(args);

	p.x = x;
	p.y = y;
	DevicePixelmapText(dst, &p, font, ss, colour);
}

br_uint_16 BR_PUBLIC_ENTRY BrPixelmapTextWidth(br_pixelmap *dst,
	br_font *font, char *text)
{
   	int i,j,w;

	CheckDispatch(dst);

        if (!text) return (font->glyph_x);

        if (BrStrLen(text) == 0) return 0;

        if(font->flags & BR_FONTF_PROPORTIONAL) {
	   	/*
		 * return length of (proportional) string in pixels
		 */
		for(i = 0, w = 0, j = BrStrLen(text) ; i<j; i++, text++)
			w += font->width[*text]+1;

		w -= 1;
		return w; 
	} else
		return (font->glyph_x+1) * BrStrLen(text) -1;
}

br_uint_16 BR_PUBLIC_ENTRY BrPixelmapTextHeight(br_pixelmap *dst,
	br_font *font)
{
	CheckDispatch(dst);

	return font->glyph_y;
}

void BR_PUBLIC_ENTRY BrPixelmapCopyBits(br_pixelmap *dst,
	br_int_32 x,br_int_32 y,
	br_uint_8 *src,br_int_32 s_stride,
	br_int_32 start_bit,br_int_32 end_bit,
	br_int_32 nrows,
	br_uint_32 colour)
{
	br_point p;
	br_rectangle r;

	CheckDispatch(dst);

	p.x = x; p.y = y;
	r.x = start_bit;
	r.y = 0;
	r.w = end_bit - start_bit;
	r.h = nrows;

	DevicePixelmapCopyBits(dst, &p, src,(br_uint_16) s_stride, &r, colour);
}

/*
 * Return the load/save size for a given pixelmap
 */
br_uint_16 BR_RESIDENT_ENTRY BrPixelmapFileSize(br_pixelmap *pm)
{
#if 0
	br_int_32 bytes = 0;

	CheckDispatch(pm);

	ObjectQuery(pm, (br_uint_32 *)&bytes, BRT_WORD_BYTES_I32);

	return (br_uint_16) bytes;
#endif

	return DevicePixelmapFileBytes(pm);
}

/*
 * Return the pixel size in bits
 */
br_uint_16 BR_RESIDENT_ENTRY BrPixelmapPixelSize(br_pixelmap *pm)
{
#if 0
	br_int_32 bits = 0;

	CheckDispatch(pm);

	ObjectQuery(pm, (br_uint_32 *)&bits, BRT_PIXEL_BITS_I32);

	return (br_uint_16) bits;
#endif
	return DevicePixelmapPixelBits(pm);
}

/*
 * Return a mask of the channels that a pixelmap has
 */
br_uint_16 BR_RESIDENT_ENTRY BrPixelmapChannels(br_pixelmap *pm)
{
#if 0
	br_uint_32 c;
	br_token *tl;
	br_token buffer[32];

	CheckDispatch(pm);

	/*
	 * Grab info from device (assumes that a pixelmap will never have more than 32 channels)
	 */
	if(ObjectQueryBuffer(pm, (br_uint_32 *)&tl, buffer, sizeof(buffer), BRT_PIXEL_CHANNELS_TL) != BRE_OK)
		return 0;

	/*
	 * Convert token list into a bitmask
	 */
	c = 0;

	for(; *tl != BR_NULL_TOKEN; tl++) {
		switch(*tl) {
		case BRT_INDEX:
			c |= BR_PMCHAN_INDEX;
			break;

		case BRT_RED:
		case BRT_GREEN:
		case BRT_BLUE:
			c |= BR_PMCHAN_RGB;
			break;

		case BRT_DEPTH:
			c |= BR_PMCHAN_DEPTH;
			break;

		case BRT_ALPHA:
			c |= BR_PMCHAN_ALPHA;
			break;
		}
	}

	return (br_uint_16)c;
#endif

	return DevicePixelmapChannels(pm);
}

void BR_PUBLIC_ENTRY BrPixelmapPaletteSet(br_pixelmap *pm, br_pixelmap *pal)
{
	UASSERT(pal);
	UASSERT(pm);
	
	CheckDispatch(pm);

	BrPixelmapPaletteEntrySetMany(pm, 0, pal->height, (br_colour *)pal->pixels);
}

void BR_PUBLIC_ENTRY BrPixelmapPaletteEntrySet(br_pixelmap *pm, br_int_32 index, br_colour colour)
{
	br_device_clut *clut;

	CheckDispatch(pm);

	/*
	 * See if pixelmap has CLUT attached...
	 */
	if(ObjectQuery(pm, (br_uint_32 *)&clut, BRT_CLUT_O) != BRE_OK)
		return;

	if(clut == NULL)
		return;

	DeviceClutEntrySet(clut, index, colour);
}

void BR_PUBLIC_ENTRY BrPixelmapPaletteEntrySetMany(br_pixelmap *pm, br_int_32 index , br_int_32 ncolours, br_colour *colours)
{
	br_device_clut *clut;

	CheckDispatch(pm);

	/*
	 * See if pixelmap has CLUT attached...
	 */
	if(ObjectQuery(pm, (br_uint_32 *)&clut, BRT_CLUT_O) != BRE_OK)
		return;

	if(clut == NULL)
		return;

	DeviceClutEntrySetMany(clut, index, ncolours, colours);
}

br_pixelmap * BR_PUBLIC_ENTRY BrPixelmapDirectLock(br_pixelmap *src, br_boolean block)
{
	UASSERT(src);

	CheckDispatch(src);

	if(DevicePixelmapDirectLock(src, block))
		return NULL;

	return src;
}

br_pixelmap * BR_PUBLIC_ENTRY BrPixelmapDirectUnlock(br_pixelmap *src)
{
	UASSERT(src);

	CheckDispatch(src);

	if(DevicePixelmapDirectUnlock(src))
		return NULL;

	return src;
}

br_error BR_PUBLIC_ENTRY BrPixelmapGetControls(br_pixelmap *src, br_display_controls *controls)
{
	UASSERT(src);

	CheckDispatch(src);

        return DevicePixelmapGetControls(src, controls);

}

br_error BR_PUBLIC_ENTRY BrPixelmapSetControls(br_pixelmap *src, br_display_controls *controls)
{
	UASSERT(src);

	CheckDispatch(src);

        return DevicePixelmapSetControls(src, controls);

}
