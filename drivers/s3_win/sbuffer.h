/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: sbuffer.h 1.1 1997/12/10 16:51:27 jon Exp $
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
 * A minimal description of a pixelmap (with all origins and
 * bases factored out)
 */


struct render_buffer
{
	void        *base;		   /* 0,0 pixel				*/
	void		   *base_mem;		/* actual base of allocated memory for data */
	br_uint_16  sel;			   /* Optional segment			*/
	br_uint_8   type;			   /* Original pixelmap type	*/
	br_uint_8   bpp;			   /* Bytes per pixel			*/
	br_uint_32  width_b;		   /* In bytes					*/
	br_uint_32  width_p;		   /* In pixels				*/
	br_uint_32  height;		   /* In scanlines				*/
	br_uint_32  stride;		   /* In bytes					*/
	br_uint_32  size;		      /* stride * width_bytes 	*/
	br_uint_16  mip_offset;    /* For mip map textures ect */
	br_boolean  alpha_pixels;  /* Transparent texture flag */

   br_boolean  in_video_memory; /* Handle if this is a texture */
	br_boolean	always_in_video_memory; /* This buffer _always_ resides in video memory */
   br_boolean  uncacheable;
   S3DTK_SURFACE  s3_surface;

   LPDIRECTDRAWSURFACE surface;  /* DirectDraw surface corresponding to this texture */
   struct render_buffer *next_on_card; /* Pointer to next surface in texture cache */
};





typedef struct br_buffer_stored
{
	/*
	 * Dispatch table
	 */
	struct br_buffer_stored_dispatch *dispatch;

	/*
	 * Standard object identifier
	 */
	char *identifier;

   /* Primitive library holding ownership */
   br_primitive_library *plib;

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


} br_buffer_stored;



#ifdef __cplusplus
};
#endif
#endif


