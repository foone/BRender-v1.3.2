/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: colour.h 1.2 1998/11/13 16:22:33 jon Exp $
 * $Locker: $
 *
 * Colour type and macros
 *
 */
#ifndef _COLOUR_H_
#define _COLOUR_H_

typedef unsigned long int br_colour;

/*
 * N.B. Colour ranges are specified in the format of the pixelmap to which
 * they refer.  For indexed pixelmaps they specify a range of indices.
 * For RGB and YUV pixelmaps they specify a cube of colours in the
 * colourspace bounded by the planes obtained by separating out each
 * component from the low and high bounds.
 *
 * Note that the range is always specified inclusively.
 */
typedef struct br_colour_range {
	br_uint_32 low;
	br_uint_32 high;
} br_colour_range;

#define BR_COLOUR_RGB(r,g,b) \
		((((unsigned int)(r))<<16) |\
		(((unsigned int)(g))<<8) |\
		((unsigned int)(b)))

#define BR_COLOUR_RGBA(r,g,b,a) \
		((((unsigned int)(a))<<24) |\
		(((unsigned int)(r))<<16) |\
		(((unsigned int)(g))<<8) |\
		((unsigned int)(b)))

#define BR_COLOUR_ARGB(a,r,g,b) \
		((((unsigned int)(a))<<24) |\
		(((unsigned int)(r))<<16) |\
		(((unsigned int)(g))<<8) |\
		((unsigned int)(b)))

#define BR_COLOUR_565(r,g,b) \
		((((unsigned int)(r))<<11) |\
		(((unsigned int)(g))<<5) |\
		((unsigned int)(b)))

#define BR_ALPHA(c) (((c) >> 24) & 0xFF)
#define BR_RED(c) (((c) >> 16) & 0xFF)
#define BR_GRN(c) (((c) >> 8) & 0xFF)
#define BR_BLU(c) ((c) & 0xFF)

#define BR_RED_565(c) (((c) >> 11 & 31))
#define BR_GRN_565(c) (((c) >> 5) & 63)
#define BR_BLU_565(c) ((c) & 31)

#endif

