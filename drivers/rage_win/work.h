/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: work.h 1.1 1997/12/10 16:50:47 jon Exp $
 * $Locker: $
 *
 * Private work area and parameters for primitives
 */
#ifndef _WORK_H_
#define _WORK_H_

#define SCREEN_FIXED 1


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

   C3D_HTX     texture_handle; /* Handle if this is a texture */
   br_boolean  uncacheable;    /* Texture cannot be cached if BR_TRUE */
   C3D_HTXPAL  palette_handle; /* Handle to palette if paletted texture */

   LPDIRECTDRAWSURFACE surface;  /* DirectDraw surface corresponding to this texture */
   struct render_buffer *next_on_card; /* Pointer to next surface in texture cache */

   br_boolean always_in_video_memory;	/* True for hidden_buffer textures */
   br_colour colour_key;					/* Colour key for texture transparency */
   br_boolean chroma_enable;				/* Use chroma keying for this buffer */
};



/*
 * Static work area for renderers
 */
struct prim_work {
	/* Buffers */
// 	struct render_buffer colour;
	br_boolean	alpha_pixels;

	/* Current buffer pointer so we know if the material has changed */
    struct br_buffer_stored *current;

    C3D_HTX  texture_handle;   /* Handle to the current texture */
};

extern struct prim_work BR_ASM_DATA work;


typedef struct prim_work * BR_EXPORT br_work_fn(void);

#ifdef __cplusplus
};
#endif
#endif


