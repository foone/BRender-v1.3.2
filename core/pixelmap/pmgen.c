/*
 * Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: pmgen.c 1.2 1998/08/10 16:12:24 johng Exp $
 * $Locker: $
 *
 * Various useful implmementations of pixelmap methods in terms of others
 */
#include "pm.h"
#include "brassert.h"

BR_RCS_ID("$Id: pmgen.c 1.2 1998/08/10 16:12:24 johng Exp $")

br_error BR_CMETHOD_DECL(br_device_pixelmap_gen, match)(br_device_pixelmap *self, br_device_pixelmap **newpm, br_token_value *tv)
{
	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_gen, copy)(br_device_pixelmap *self, br_device_pixelmap *src)
{
	br_rectangle r;
	br_point p;

	p.x = -self->pm_origin_x;
	p.y = -self->pm_origin_y;

	r.x = -src->pm_origin_x;
	r.y = -src->pm_origin_y;
	r.w = src->pm_width;
	r.h = src->pm_height;

	return DevicePixelmapRectangleCopy(self, &p, src, &r);
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_gen, copyTo)(br_device_pixelmap *self, br_device_pixelmap *src)
{
	br_rectangle r;
	br_point p;

	p.x = -self->pm_origin_x;
	p.y = -self->pm_origin_y;

	r.x = -src->pm_origin_x;
	r.y = -src->pm_origin_y;
	r.w = src->pm_width;
	r.h = src->pm_height;

	return DevicePixelmapRectangleCopyTo(self, &p, src, &r);
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_gen, copyFrom)(br_device_pixelmap *self, br_device_pixelmap *src)
{
	br_rectangle r;
	br_point p;

	p.x = -self->pm_origin_x;
	p.y = -self->pm_origin_y;

	r.x = -src->pm_origin_x;
	r.y = -src->pm_origin_y;
	r.w = src->pm_width;
	r.h = src->pm_height;

	return DevicePixelmapRectangleCopyFrom(self, &p, src, &r);
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_gen, fill)(br_device_pixelmap *self, br_uint_32 colour)
{
	br_rectangle r;

	r.x = -self->pm_origin_x;
	r.y = -self->pm_origin_y;
	r.w = self->pm_width;
	r.h = self->pm_height;

	return DevicePixelmapRectangleFill(self, &r, colour);
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_gen, doubleBuffer)(br_device_pixelmap *self, br_device_pixelmap *src)
{
	br_rectangle s;
	br_rectangle d;

	/*
	 * Use dispatch functions to allow double buffering between memory
	 * and device pixelmaps (I think the Direct Draw driver relies on
	 * this behaviour)
	 */
	if ((self->pm_width != src->pm_width) || (self->pm_height != src->pm_height)) {

		s.x = -src->pm_origin_x;
		s.y = -src->pm_origin_y;
		s.w = src->pm_width;
		s.h = src->pm_height;

		d.x = -self->pm_origin_x;
		d.y = -self->pm_origin_y;
		d.w = self->pm_width;
		d.h = self->pm_height;

		return DispatchRectangleStretchCopy((br_device_pixelmap *)self, &d, (br_device_pixelmap *)src, &s);
	}

	return DispatchCopy(self, src);

//	return DevicePixelmapCopy(self, src);
}


br_error BR_CMETHOD_DECL(br_device_pixelmap_gen, copyDirty)\
	(br_device_pixelmap *self, br_device_pixelmap *src, br_rectangle *dirty, br_int_32 num_rects)
{
	int i;
	br_point p;
	br_error e;

	/*
	 * Catch attempts to perform dirty rectangle operations between pixelmaps
	 * of different sizes
	 */
	if ((self->pm_width != src->pm_width) || (self->pm_height != src->pm_height))
		return DevicePixelmapCopy(self, src);

	for(i = 0; i < num_rects; i++) {
		p.x = dirty[i].x;
		p.y = dirty[i].y;
		e = DevicePixelmapRectangleCopy(self, &p, src, dirty+i);
		if (e != BRE_OK)
			return e;
	}	

	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_gen, copyToDirty)\
	(br_device_pixelmap *self, br_device_pixelmap *src, br_rectangle *dirty, br_int_32 num_rects)
{
	int i;
	br_point p;
	br_error e;

	/*
	 * Catch attempts to perform dirty rectangle operations between pixelmaps
	 * of different sizes
	 */
	if ((self->pm_width != src->pm_width) || (self->pm_height != src->pm_height))
		return DevicePixelmapCopyTo(self, src);

	for(i = 0; i < num_rects; i++) {
		p.x = dirty[i].x;
		p.y = dirty[i].y;
		e = DevicePixelmapRectangleCopyTo(self, &p, src, dirty+i);
		if (e != BRE_OK)
			return e;
	}	

	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_gen, copyFromDirty)\
	(br_device_pixelmap *self, br_device_pixelmap *src, br_rectangle *dirty, br_int_32 num_rects)
{
	int i;
	br_point p;
	br_error e;

	/*
	 * Catch attempts to perform dirty rectangle operations between pixelmaps
	 * of different sizes
	 */
	if ((self->pm_width != src->pm_width) || (self->pm_height != src->pm_height))
		return DevicePixelmapCopyFrom(self, src);

	for(i = 0; i < num_rects; i++) {
		p.x = dirty[i].x;
		p.y = dirty[i].y;
		e = DevicePixelmapRectangleCopyFrom(self, &p, src, dirty+i);
		if (e != BRE_OK)
			return e;
	}	

	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_gen, fillDirty)\
	(br_device_pixelmap *self, br_uint_32 colour, br_rectangle *dirty, br_int_32 num_rects)
{
	int i;
	br_error e;

	for(i = 0; i < num_rects; i++) {
		e = DevicePixelmapRectangleFill(self, dirty+i, colour);
		if (e != BRE_OK)
			return e;
	}
	
	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_gen, doubleBufferDirty)\
	(br_device_pixelmap *self, br_device_pixelmap *src, br_rectangle *dirty, br_int_32 num_rects)
{
	int i;
	br_point p;
	br_error e;

	/*
	 * Catch attempts to perform dirty rectangle operations between pixelmaps
	 * of different sizes
	 */
	if ((self->pm_width != src->pm_width) || (self->pm_height != src->pm_height))
		return DevicePixelmapDoubleBuffer(self, src);

	for(i = 0; i < num_rects; i++) {
		p.x = dirty[i].x;
		p.y = dirty[i].y;
		e = DevicePixelmapRectangleCopy(self, &p, src, dirty+i);
		if (e != BRE_OK)
			return e;
	}

	return BRE_OK;
}


br_error BR_CMETHOD_DECL(br_device_pixelmap_gen, rectangle)\
	(br_device_pixelmap *self, br_rectangle *rect, br_uint_32 colour)
{
	br_point tl,tr,bl,br;
	br_error e;

	tl.x = bl.x = rect->x;
	tl.y = tr.y = rect->y;
	tr.x = br.x = rect->x + rect->w - 1;
	bl.y = br.y = rect->y + rect->h - 1;

	e = DevicePixelmapLine(self, &tl, &tr, colour);
	if (e != BRE_OK)
		return e;

	e = DevicePixelmapLine(self, &tl, &bl, colour);
	if (e != BRE_OK)
		return e;

	e = DevicePixelmapLine(self, &bl, &br, colour);
	if (e != BRE_OK)
		return e;

	e = DevicePixelmapLine(self, &tr, &br, colour);
	if (e != BRE_OK)
		return e;

	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_gen, rectangle2)\
	(br_device_pixelmap *self, br_rectangle *rect, br_uint_32 colour_tl, br_uint_32 colour_br)
{
	br_point tl,tr,bl,br;
	br_error e;

	tl.x = bl.x = rect->x;
	tl.y = tr.y = rect->y;
	tr.x = br.x = rect->x + rect->w - 1;
	bl.y = br.y = rect->y + rect->h - 1;

	e = DevicePixelmapLine(self, &tl, &tr, colour_tl);
	if (e != BRE_OK)
		return e;

	e = DevicePixelmapLine(self, &tl, &bl, colour_tl);
	if (e != BRE_OK)
		return e;

	e = DevicePixelmapLine(self, &bl, &br, colour_br);
	if (e != BRE_OK)
		return e;

	e = DevicePixelmapLine(self, &tr, &br, colour_br);
	if (e != BRE_OK)
		return e;

	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_gen, line)\
	(br_device_pixelmap *self, br_point *s, br_point *e, br_uint_32 colour)
{
	int             dx, dy, incr1, incr2, D, x, y, xend, c, pixels_left;
	int             x1, y1;
	int             sign_x, sign_y, step, reverse, i;
	br_point		as,ae,p;

	/*
	 * Clip line to pixelmap (produces abs. coordinates)
	 */
	if(PixelmapLineClip(&as, &ae, s, e, (br_pixelmap *)self) == BR_CLIP_REJECT)
		return BRE_OK;

	/*
	 * Make clipped points relative to origin
	 */
	as.x -= self->pm_origin_x;
	as.y -= self->pm_origin_y;
	ae.x -= self->pm_origin_x;
	ae.y -= self->pm_origin_y;

/*
 * Symmetric Double Step Line Algorithm by Brian Wyvill from "Graphics Gems",
 * Academic Press, 1990
 */

#define LINEPLOT(_x,_y,flag) do {				\
	if(flag) {									\
		p.x = _y;								\
		p.y = _x;								\
	} else {									\
		p.x = _x;								\
		p.y = _y;								\
	}											\
	DevicePixelmapPixelSet(self, &p, colour);	\
} while(0)

#define swap(a,b)           {a^=b; b^=a; a^=b;}
#define absolute(i,j,k)     ( (i-j)*(k = ( (i-j)<0 ? -1 : 1)))

	dx = absolute(ae.x, as.x, sign_x);
	dy = absolute(ae.y, as.y, sign_y);

	if(dx == 0 && dy == 0) {
		DevicePixelmapPixelSet(self, &as, colour);
		return BRE_OK;
	}

	/* decide increment sign by the slope sign */
	if (sign_x == sign_y)
		step = 1;
	else
		step = -1;

	if (dy > dx) {		/* chooses axis of greatest movement (make dx) */
		swap(as.x, as.y);
		swap(ae.x, ae.y);
		swap(dx, dy);
		reverse = 1;
	} else
		reverse = 0;

	/* note error check for dx==0 should be included here */
	if (as.x > ae.x) {		/* start from the smaller coordinate */
		x = ae.x;
		y = ae.y;
		x1 = as.x;
		y1 = as.y;
	} else {
		x = as.x;
		y = as.y;
		x1 = ae.x;
		y1 = ae.y;
	}


	/* Note dx=n implies 0 - n or (dx+1) pixels to be set */
	/* Go round loop dx/4 times then plot last 0,1,2 or 3 pixels */
	/* In fact (dx-1)/4 as 2 pixels are already plottted */

	xend = (dx - 1) / 4;
	pixels_left = (dx - 1) % 4;	/* number of pixels left over at the end */
	LINEPLOT(x, y, reverse);
	LINEPLOT(x1, y1, reverse);	/* plot first two points */
	incr2 = 4 * dy - 2 * dx;
	if (incr2 < 0) {	/* slope less than 1/2 */
		c = 2 * dy;
		incr1 = 2 * c;
		D = incr1 - dx;

		for (i = 0; i < xend; i++) {	/* plotting loop */
			++x;
			--x1;
			if (D < 0) {
				/* pattern 1 forwards */
				LINEPLOT(x, y, reverse);
				LINEPLOT(++x, y, reverse);
				/* pattern 1 backwards */
				LINEPLOT(x1, y1, reverse);
				LINEPLOT(--x1, y1, reverse);
				D += incr1;
			} else {
				if (D < c) {
					/* pattern 2 forwards */
					LINEPLOT(x, y, reverse);
					LINEPLOT(++x, y += step, reverse);
					/* pattern 2 backwards */
					LINEPLOT(x1, y1, reverse);
					LINEPLOT(--x1, y1 -= step, reverse);
				} else {
					/* pattern 3 forwards */
					LINEPLOT(x, y += step, reverse);
					LINEPLOT(++x, y, reverse);
					/* pattern 3 backwards */
					LINEPLOT(x1, y1 -= step, reverse);
					LINEPLOT(--x1, y1, reverse);
				}
				D += incr2;
			}
		}		/* end for */

		/* plot last pattern */
		if (pixels_left) {
			if (D < 0) {
				LINEPLOT(++x, y, reverse);	/* pattern 1 */
				if (pixels_left > 1)
					LINEPLOT(++x, y, reverse);
				if (pixels_left > 2)
					LINEPLOT(--x1, y1, reverse);
			} else {
				if (D < c) {
					LINEPLOT(++x, y, reverse);	/* pattern 2  */
					if (pixels_left > 1)
						LINEPLOT(++x, y += step, reverse);
					if (pixels_left > 2)
						LINEPLOT(--x1, y1, reverse);
				} else {
					/* pattern 3 */
					LINEPLOT(++x, y += step, reverse);
					if (pixels_left > 1)
						LINEPLOT(++x, y, reverse);
					if (pixels_left > 2)
						LINEPLOT(--x1, y1 -= step, reverse);
				}
			}
		}		/* end if pixels_left */
	}
	/* end slope < 1/2 */
	else {			/* slope greater than 1/2 */
		c = 2 * (dy - dx);
		incr1 = 2 * c;
		D = incr1 + dx;
		for (i = 0; i < xend; i++) {
			++x;
			--x1;
			if (D > 0) {
				/* pattern 4 forwards */
				LINEPLOT(x, y += step, reverse);
				LINEPLOT(++x, y += step, reverse);
				/* pattern 4 backwards */
				LINEPLOT(x1, y1 -= step, reverse);
				LINEPLOT(--x1, y1 -= step, reverse);
				D += incr1;
			} else {
				if (D < c) {
					/* pattern 2 forwards */
					LINEPLOT(x, y, reverse);
					LINEPLOT(++x, y += step, reverse);

					/* pattern 2 backwards */
					LINEPLOT(x1, y1, reverse);
					LINEPLOT(--x1, y1 -= step, reverse);
				} else {
					/* pattern 3 forwards */
					LINEPLOT(x, y += step, reverse);
					LINEPLOT(++x, y, reverse);
					/* pattern 3 backwards */
					LINEPLOT(x1, y1 -= step, reverse);
					LINEPLOT(--x1, y1, reverse);
				}
				D += incr2;
			}
		}		/* end for */
		/* plot last pattern */
		if (pixels_left) {
			if (D > 0) {
				LINEPLOT(++x, y += step, reverse);	/* pattern 4 */
				if (pixels_left > 1)
					LINEPLOT(++x, y += step, reverse);
				if (pixels_left > 2)
					LINEPLOT(--x1, y1 -= step, reverse);
			} else {
				if (D < c) {
					LINEPLOT(++x, y, reverse);	/* pattern 2  */
					if (pixels_left > 1)
						LINEPLOT(++x, y += step, reverse);
					if (pixels_left > 2)
						LINEPLOT(--x1, y1, reverse);
				} else {
					/* pattern 3 */
					LINEPLOT(++x, y += step, reverse);
					if (pixels_left > 1)
						LINEPLOT(++x, y, reverse);
					if (pixels_left > 2) {
						if (D > c)	/* step 3 */
							LINEPLOT(--x1, y1 -= step, reverse);
						else	/* step 2 */
							LINEPLOT(--x1, y1, reverse);
					}
				}
			}
		}
	}

	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_gen, text)(br_device_pixelmap *self, br_point *point,
	br_font *font,
	char *text,
	br_uint_32 colour)
{
	br_rectangle r;
	br_point p;
	br_int_32 x,y;
	br_int_32 s_stride;

	/*
	 * Quit if off top, bottom or right screen
	 */
	x = point->x + self->pm_origin_x;
	y = point->y + self->pm_origin_y;

	if(y <= -font->glyph_y || y >= self->pm_height || x >= self->pm_width)
		return BRE_OK;

	/*
	 * Initialise current point
	 */
	p = *point;

	/*
	 * Initialise bit rectangle
	 */
	r.x = 0;
	r.y = 0;
	r.h = font->glyph_y;

	if(font->flags & BR_FONTF_PROPORTIONAL) {
		/*
		 * PROPORTIONAL
		 */
		for(;*(unsigned char *)text; text++) {
			r.w = font->width[*(unsigned char *)text];
			s_stride = (r.w+7)/8;
	
			if(x+r.w > 0) 
				DevicePixelmapCopyBits(self, &p,
					font->glyphs+font->encoding[*(unsigned char *)text],
					(br_uint_16) s_stride,
					&r, colour);

			x += r.w+1;
			p.x += r.w+1;

			if(x > self->pm_width)
				break;
		}

	} else {
		/*
		 * FIXED
		 */
		r.w = font->glyph_x;
		s_stride = (r.w+7)/8;

		for(;*(unsigned char *)text; text++) {

			if(x+r.w > 0)
				DevicePixelmapCopyBits(self, &p,
					font->glyphs+font->encoding[*(unsigned char *)text],
					(br_uint_16) s_stride,
					&r, colour);

			x += r.w+1;
			p.x += r.w+1;

			if(x > self->pm_width)
				break;
		}
	}

	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_gen, textBounds)(br_device_pixelmap *self,
	br_rectangle *rect,
	br_font *font,
	char *text)
{
   	int i,j;

	rect->x = 0;
	rect->y = 0;
	rect->w = 0;
	rect->h = font->glyph_y;

	if(text) {
		if(font->flags & BR_FONTF_PROPORTIONAL) {
		   	/*
			 * return length of (proportional) string in pixels
			 */
			for(i = 0, j = BrStrLen(text) ; i<j; i++, text++)
				rect->w += font->width[*(unsigned char *)text]+1;
	
			rect->w -= 1;
		} else
			rect->w = (font->glyph_x+1) * BrStrLen(text) -1;
	}

	return BRE_OK;
}


br_error BR_CMETHOD_DECL(br_device_pixelmap_gen, copyBits)\
	(br_device_pixelmap *self, br_point *point,
	br_uint_8 *src,br_uint_16 s_stride,
	br_rectangle *bit_rect,
	br_uint_32 colour)
{
	br_int_32 x,y;
	br_point p;

	/*
	 * Slow and simple!
	 */
	for(y = bit_rect->y ; y < bit_rect->y+bit_rect->h; y++) {
		for(x = bit_rect->x ; x < bit_rect->x+bit_rect->w; x++) {
			if(src[(y * s_stride) + (x / 8)] & 0x80 >> (x % 8) ) {
				p.x = point->x + x;
				p.y = point->y + y;
				DevicePixelmapPixelSet(self, &p, colour);
			}
		}
	}

	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_gen, flush)(
	struct br_device_pixelmap *self)
{
	return BRE_OK;
}
br_error BR_CMETHOD_DECL(br_device_pixelmap_gen, synchronise)(
	struct br_device_pixelmap *self,
	br_token sync_type,
	br_boolean block)
{
	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_gen, directLock)(br_device_pixelmap *self, br_boolean block)
{
	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_gen, directUnlock)(br_device_pixelmap *self)
{
	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_gen, getControls)(br_device_pixelmap *self, br_display_controls * controls)
{
        return BRE_FAIL;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_gen, setControls)(br_device_pixelmap *self, br_display_controls * controls)
{
        return BRE_FAIL;
}


