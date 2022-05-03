/*
 * Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: pmnull.c 1.1 1997/12/10 16:41:27 jon Exp $
 * $Locker: $
 *
 * Stub methods for pixelmap rendering that do nothing
 */
#include "pm.h"
#include "brassert.h"

BR_RCS_ID("$Id: pmnull.c 1.1 1997/12/10 16:41:27 jon Exp $")

br_error BR_CMETHOD_DECL(br_device_pixelmap_null, resize)(br_device_pixelmap *self, br_int_32 width, br_int_32 height)
{
	return BRE_OK;
}

/*
 * Operations on whole pixemap (with versions that include a hint dirty rectangle)
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_null, copyTo)(br_device_pixelmap *self, br_device_pixelmap *src)
{
	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_null, copyFrom)(br_device_pixelmap *self, br_device_pixelmap *src)
{
	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_null, fill)(br_device_pixelmap *self, br_uint_32 colour)
{
	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_null, doubleBuffer)(br_device_pixelmap *self, br_device_pixelmap *src)
{
	return BRE_OK;
}


br_error BR_CMETHOD_DECL(br_device_pixelmap_null, copyDirty)\
	(br_device_pixelmap *self, br_device_pixelmap *src, br_rectangle *dirty, br_int_32 num_rects)
{
	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_null, copyToDirty)\
	(br_device_pixelmap *self, br_device_pixelmap *src, br_rectangle *dirty, br_int_32 num_rects)
{
	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_null, copyFromDirty)\
	(br_device_pixelmap *self, br_device_pixelmap *src, br_rectangle *dirty, br_int_32 num_rects)
{
	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_null, fillDirty)\
	(br_device_pixelmap *self, br_uint_32 colour, br_rectangle *dirty, br_int_32 num_rects)
{
	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_null, doubleBufferDirty)\
	(br_device_pixelmap *self, br_device_pixelmap *src, br_rectangle *dirty, br_int_32 num_rects)
{
	return BRE_OK;
}


/*
 * Direct 2D rendering operations
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_null, rectangleCopyTo)\
	(br_device_pixelmap *self, br_point *p, br_device_pixelmap *src, br_rectangle *r)
{
	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_null, rectangleCopyFrom)\
	(br_device_pixelmap *self, br_point *p, br_device_pixelmap *mem, br_rectangle *r)
{
	return BRE_OK;
}


br_error BR_CMETHOD_DECL(br_device_pixelmap_null, rectangleStretchCopyTo)\
	(br_device_pixelmap *self, br_rectangle *d, br_device_pixelmap *src, br_rectangle *s)
{
	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_null, rectangleStretchCopyFrom)\
	(br_device_pixelmap *self, br_rectangle *d, br_device_pixelmap *src, br_rectangle *s)
{
	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_null, rectangleFill)\
	(br_device_pixelmap *self, br_rectangle *rect, br_uint_32 colour)
{
	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_null, pixelSet)\
	(br_device_pixelmap *self, br_point *p, br_uint_32 colour)
{
	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_null, copyBits)\
	(br_device_pixelmap *self, br_point *point,
	br_uint_8 *src,br_uint_16 s_stride,
	br_rectangle *bit_rect,
	br_uint_32 colour)
{
	return BRE_OK;
}

/*
 * Reading back
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_null, pixelQuery)\
	(br_device_pixelmap *self, br_uint_32 *pcolour, br_point *p)
{
	return BRE_OK;
}


br_error BR_CMETHOD_DECL(br_device_pixelmap_null, pixelAddressQuery)\
	(br_device_pixelmap *self, void **pptr, br_uint_32 *pqual, br_point *p)
{
	return BRE_OK;
}

/*
 * Updating the pixelmap
 */
br_error BR_CMETHOD_DECL(br_device_pixelmap_null, pixelAddressSet)(br_device_pixelmap *self, void *ptr, br_uint_32 *qual)
{
	return BRE_OK;
}
