/*
 * Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: genclip.c 1.1 1997/12/10 16:41:24 jon Exp $
 * $Locker: $
 *
 * Useful line and rectangle clipping routines
 */
#include "pm.h"
#include "brassert.h"

BR_RCS_ID("$Id: genclip.c 1.1 1997/12/10 16:41:24 jon Exp $")

/*
 * Clip a point to a pixelmap, producing coordinates relative to base of pixelmap
 */
br_clip_result BR_RESIDENT_ENTRY PixelmapPointClip(br_point *out, br_point *in, br_pixelmap *pm)
{
	out->x = in->x + pm->origin_x;
	out->y = in->y + pm->origin_y;

	if(out->x < 0 || out->y < 0)
		return BR_CLIP_REJECT;

	if(out->x >= pm->width || out->y >= pm->height)
		return BR_CLIP_REJECT;

	return BR_CLIP_ACCEPT;
}

/*
 * Clip a line to a pixelmap, producing coordinates relative to base of pixelmap
 */
br_clip_result BR_RESIDENT_ENTRY PixelmapLineClip(br_point *s_out, br_point *e_out, br_point *s_in, br_point *e_in, br_pixelmap *pm)
{
	int temp;
	br_int_32 w,h;
	br_int_32 x1,x2,y1,y2;

	x1 = s_in->x + pm->origin_x;
	y1 = s_in->y + pm->origin_y;

	x2 = e_in->x + pm->origin_x;
	y2 = e_in->y + pm->origin_y;

#define USCALE(var,arg,num,den) ((var) = ((unsigned)(arg) * (unsigned)(num)) / (unsigned)(den))

#define EXCHG(a,b) do {							\
    int __temp__ = (a);							\
    (a) = (b);									\
    (b) = __temp__;								\
} while(0)

	w = pm->width-1;
	h = pm->height-1;

	if (x1 > x2) {
		EXCHG(x1, x2);
		EXCHG(y1, y2);
	}

	if ((x2 < 0) || (x1 > w)) {
		return BR_CLIP_REJECT;
	}

	if (y1 < y2) {
		if ((y2 < 0) || (y1 > h)) {
			return BR_CLIP_REJECT;
		}
		if (y1 < 0) {
			USCALE(temp, (x2 - x1), (0 - y1), (y2 - y1));
			if ((x1 += temp) > w) {
				return BR_CLIP_REJECT;
			}
			y1 = 0;
		}
		if (y2 > h) {
			USCALE(temp, (x2 - x1), (y2 - h), (y2 - y1));
			if ((x2 -= temp) < 0) {
				return BR_CLIP_REJECT;
			}
			y2 = h;
		}
		if (x1 < 0) {
			USCALE(temp, (y2 - y1), (0 - x1), (x2 - x1));
			y1 += temp;
			x1 = 0;
		}
		if (x2 > w) {
			USCALE(temp, (y2 - y1), (x2 - w), (x2 - x1));
			y2 -= temp;
			x2 = w;
		}
	} else {
		if ((y1 < 0) || (y2 > h)) {
			return BR_CLIP_REJECT;
		}
		if (y1 > h) {
			USCALE(temp, (x2 - x1), (y1 - h), (y1 - y2));
			if ((x1 += temp) > w) {
				return BR_CLIP_REJECT;
			}
			y1 = h;
		}
		if (y2 < 0) {
			USCALE(temp, (x2 - x1), (0 - y2), (y1 - y2));
			if ((x2 -= temp) < 0) {
				return BR_CLIP_REJECT;
			}
			y2 = 0;
		}
		if (x1 < 0) {
			USCALE(temp, (y1 - y2), (0 - x1), (x2 - x1));
			y1 -= temp;
			x1 = 0;
		}
		if (x2 > w) {
			USCALE(temp, (y1 - y2), (x2 - w), (x2 - x1));
			y2 += temp;
			x2 = w;
		}
	}

	s_out->x = x1;
	s_out->y = y1;

	e_out->x = x2;
	e_out->y = y2;

	return BR_CLIP_PARTIAL;
}

/*
 * Clip a rectangle to a pixelmap, producing coordinates relative to base of pixelmap
 */
br_clip_result BR_RESIDENT_ENTRY PixelmapRectangleClip(br_rectangle *out, br_rectangle *in, br_pixelmap *pm)
{
	out->x = in->x + pm->origin_x;
	out->y = in->y + pm->origin_y;
	out->w = in->w;
	out->h = in->h;

	/*
	 * Trivial reject
	 */
	if(out->x >= pm->width || out->y >= pm->height)
		return BR_CLIP_REJECT;

	if((out->x+out->w) <= 0 || (out->y+out->h) <= 0)
		return BR_CLIP_REJECT;

	/*
	 * Clip rectangle to destination
	 */

	if(out->x < 0) {
		out->w += out->x; out->x = 0;
	}

	if(out->y < 0) {
		out->h += out->y; out->y = 0;
	}

	if((out->x+out->w) > pm->width)
		out->w = pm->width - out->x;

	if((out->y+out->h) > pm->height)
		out->h = pm->height - out->y;

	/*
	 * Don't draw empty rectangles
	 */
	if(out->w == 0 || out->h == 0)
		return BR_CLIP_REJECT;

	return BR_CLIP_PARTIAL;
}

/*
 * Clip a rectangle to two pixelmaps, producing coordinates relative to base of pixelmap
 */
br_clip_result BR_RESIDENT_ENTRY PixelmapRectangleClipTwo(
	br_rectangle *r_out, br_point *p_out,
	br_rectangle *r_in, br_point *p_in,
	br_pixelmap *pm_dst, br_pixelmap *pm_src)
{
	r_out->x = r_in->x + pm_src->origin_x;
	r_out->y = r_in->y + pm_src->origin_y;
	r_out->w = r_in->w;
	r_out->h = r_in->h;

	p_out->x = p_in->x + pm_dst->origin_x;
	p_out->y = p_in->y + pm_dst->origin_y;


	/*
	 * Trivial reject
	 */
	if(p_out->x >= pm_dst->width || p_out->y >= pm_dst->height)
		return BR_CLIP_REJECT;

	if(r_out->x >= pm_src->width || r_out->y >= pm_src->height)
		return BR_CLIP_REJECT;

	if((p_out->x+r_out->w) <= 0 || (p_out->y+r_out->h) <= 0)
		return BR_CLIP_REJECT;

	if((r_out->x+r_out->w) <= 0 || (r_out->y+r_out->h) <= 0)
		return BR_CLIP_REJECT;


	/*
	 * Clip rectangle to destination
	 */
	if(p_out->x < 0) {
		r_out->w += p_out->x;
		r_out->x -= p_out->x;
		p_out->x = 0;
	}

	if(p_out->y < 0) {
		r_out->h += p_out->y;
		r_out->y -= p_out->y;
		p_out->y = 0;
	}

	if((p_out->x+r_out->w) > pm_dst->width)
		r_out->w = pm_dst->width - p_out->x;

	if((p_out->y+r_out->h) > pm_dst->height)
		r_out->h = pm_dst->height - p_out->y;

	/*
	 * Clip rectangle to source
	 */
	if(r_out->x < 0) {
		r_out->w += r_out->x;
		p_out->x -= r_out->x;
		r_out->x = 0;
	}

	if(r_out->y < 0) {
		r_out->h += r_out->y;
		p_out->y -= r_out->y;
		r_out->y = 0;
	}

	if((r_out->x+r_out->w) > pm_src->width)
		r_out->w = pm_src->width - r_out->x;

	if((r_out->y+r_out->h) > pm_src->height)
		r_out->h = pm_src->height - r_out->y;

	/*
	 * Don't draw empty rectangles
	 */
	if(r_out->w == 0 || r_out->h == 0)
		return BR_CLIP_REJECT;

	return BR_CLIP_PARTIAL;
}


/*
 * Clip two rectangles rectangle to two pixelmaps, producing coordinates relative to base of pixelmaps
 */
br_clip_result BR_RESIDENT_ENTRY PixelmapRectanglesClipTwo(
	br_rectangle *s_out, br_rectangle *d_out,
	br_rectangle *s_in, br_rectangle *d_in,
	br_pixelmap *pm_dst, br_pixelmap *pm_src)
{
	br_int_32 adjust;
	
	s_out->x = s_in->x + pm_src->origin_x;
	s_out->y = s_in->y + pm_src->origin_y;
	s_out->w = s_in->w;
	s_out->h = s_in->h;

	d_out->x = d_in->x + pm_dst->origin_x;
	d_out->y = d_in->y + pm_dst->origin_y;
	d_out->w = d_in->w;
	d_out->h = d_in->h;

	/*
	 * Trivial reject
	 */
	if (d_out->x >= pm_dst->width || d_out->y >= pm_dst->height)
		return BR_CLIP_REJECT;

	if (s_out->x >= pm_src->width || s_out->y >= pm_src->height)
		return BR_CLIP_REJECT;

	if ((d_out->x + d_out->w) <= 0 || (d_out->y + d_out->h) <= 0)
		return BR_CLIP_REJECT;

	if ((s_out->x + s_out->w) <= 0 || (s_out->y + s_out->h) <= 0)
		return BR_CLIP_REJECT;

	/*
	 * Clip rectangles to destination
	 */
	if (d_out->x < 0) {

		adjust = d_out->x * s_in->w / d_in->w;

		d_out->w += d_out->x;
		d_out->x = 0;
		
		s_out->x -= adjust;
		s_out->w += adjust;
	}

	if (d_out->y < 0) {

		adjust = d_out->y * s_in->h / d_in->h;

		d_out->h += d_out->y;
		d_out->y = 0;
		
		s_out->y -= adjust;
		s_out->h += adjust;
	}

	if ((d_out->x + d_out->w) > pm_dst->width) {

		d_out->w = pm_dst->width - d_out->x;
		s_out->w = d_out->w * s_in->w / d_in->w;
	}		

	if ((d_out->y + d_out->h) > pm_dst->height) {

		d_out->h = pm_dst->height - d_out->y;
		s_out->h = d_out->h * s_in->h / d_in->h;
	}		

	/*
	 * Clip rectangles to source
	 */
	if (s_out->x < 0) {

		adjust = s_out->x * d_in->w / s_in->w;

		s_out->w += s_out->x;
		s_out->x = 0;
		
		d_out->x -= adjust;
		d_out->w += adjust;
	}

	if (s_out->y < 0) {

		adjust = s_out->y * d_in->h / s_in->h;

		s_out->h += s_out->y;
		s_out->y = 0;
		
		d_out->y -= adjust;
		d_out->h += adjust;
	}

	if ((s_out->x + s_out->w) > pm_src->width) {

		s_out->w = pm_src->width - s_out->x;
		d_out->w = s_out->w * d_in->w / s_in->w;
	}		

	if ((s_out->y + s_out->h) > pm_src->height) {

		s_out->h = pm_src->height - s_out->y;
		d_out->h = s_out->h * d_in->h / s_in->h;
	}		

	/*
	 * Don't draw empty rectangles
	 */
	if(s_out->w <= 0 || s_out->h <= 0 || d_out->w <= 0 || d_out->h <= 0)
		return BR_CLIP_REJECT;

	return BR_CLIP_PARTIAL;
}


/*
 * Clip CopyBits arguments to a pixelmap
 */
br_clip_result BR_RESIDENT_ENTRY PixelmapCopyBitsClip(
	br_rectangle *r_out, br_point *p_out,
	br_rectangle *r_in, br_point *p_in,
	br_pixelmap *pm)
{
	r_out->x = r_in->x;
	r_out->y = r_in->y;
	r_out->w = r_in->w;
	r_out->h = r_in->h;

	p_out->x = p_in->x + pm->origin_x;
	p_out->y = p_in->y + pm->origin_y;

	/*
	 * Trivial reject
	 */
	if(p_out->x >= pm->width || p_out->y >= pm->height)
		return BR_CLIP_REJECT;

	if((p_out->x+r_out->w) <= 0 || (p_out->y+r_out->h) <= 0)
		return BR_CLIP_REJECT;

	/*
	 * Clip rectangle to destination
	 */
	if(p_out->x < 0) {
		r_out->w += p_out->x;
		r_out->x -= p_out->x;
		p_out->x = 0;
	}

	if(p_out->y < 0) {
		r_out->h += p_out->y;
		r_out->y -= p_out->y;
		p_out->y = 0;
	}

	if((p_out->x+r_out->w) > pm->width)
		r_out->w = pm->width - p_out->x;

	if((p_out->y+r_out->h) > pm->height)
		r_out->h = pm->height - p_out->y;

	/*
	 * Don't draw empty rectangles
	 */
	if(r_out->w == 0 || r_out->h == 0)
		return BR_CLIP_REJECT;

	return BR_CLIP_PARTIAL;
}
