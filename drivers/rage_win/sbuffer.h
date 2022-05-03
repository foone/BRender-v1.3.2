/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: sbuffer.h 1.1 1997/12/10 16:50:40 jon Exp $
 * $Locker: $
 *
 * Stored buffer structure
 */
#ifndef _SBUFFER_H_
#define _SBUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Flags
 */
enum {
	SBUFF_SHARED	= 0x0001,		/* Data is shared with user */
};






/*
 * A description of a buffer to be used in rendering operations. Usually a
 * texture map, but could also be a destination pixelmap (i.e. colour buffer
 * or depth buffer)
 */
struct render_buffer
{
	void        *base;		   /* 0,0 pixel				*/
	br_uint_16  sel;			   /* Optional segment			*/
	br_uint_8   type;			   /* Original pixelmap type	*/
	br_uint_8   bpp;			   /* Bytes per pixel			*/
	br_uint_32  width_b;		   /* In bytes					*/
	br_uint_32  width_p;		   /* In pixels				*/
	br_uint_32  height;		   /* In scanlines				*/
	br_uint_32  stride;		   /* In bytes					*/
	br_uint_32  size;		      /* stride * width_bytes 	*/
	br_uint_16  mip_offset;    /* Offset to mip level 1 if mipmapped */
	br_boolean  alpha_pixels;  /* Flag to indicate if buffer contains alpha channel data */
	float		u_range;		// Cached ranges - we need to calculate a correction factor
	float		v_range;		// so that we don't overflow when using bilinear interpolation

   C3D_HTX     texture_handle; /* Handle if this is a texture */
   br_boolean  uncacheable;    /* Texture cannot be cached if BR_TRUE */
   C3D_HTXPAL  palette_handle; /* Handle to palette if paletted texture */

   LPDIRECTDRAWSURFACE surface;  /* DirectDraw surface corresponding to this texture */
   struct render_buffer *next_on_card; /* Pointer to next surface in texture cache */

   br_boolean always_in_video_memory;	/* True for hidden_buffer textures */
   br_colour colour_key;					/* Colour key for texture transparency */
   br_boolean chroma_enable;				/* Use chroma keying for this buffer */
};



typedef struct br_buffer_stored {
	/*
	 * Dispatch table
	 */
	struct br_buffer_stored_dispatch *dispatch;

	/*
	 * Standard object identifier
	 */
	char *identifier;

	/*
	 * Flags
	 */
	br_uint_32 flags;

	/*
	 * Reduced info about buffer
	 */
	struct render_buffer buffer;

   /* flag to indicate if this stored buffer has been updated */

   br_boolean  buffer_updated;

   br_primitive_library *plib;

} br_buffer_stored;

#ifdef __cplusplus
};
#endif
#endif


