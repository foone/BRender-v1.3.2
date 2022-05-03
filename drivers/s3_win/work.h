/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: work.h 1.1 1997/12/10 16:51:33 jon Exp $
 * $Locker: $
 *
 * Private work area and parameters for primitives
 */
#ifndef _WORK_H_
#define _WORK_H_

#define SCREEN_FIXED 1


/*
 * A minimal description of a pixelmap (with all origins and
 * bases factored out)
 */
struct render_buffer
{
	void        *base;		   /* 0,0 pixel				*/
	void		*base_mem;		/* actual base of allocated memory for data */
	br_uint_16  sel;			   /* Optional segment			*/
	br_uint_8   type;			   /* Original pixelmap type	*/
	br_uint_8   bpp;			   /* Bytes per pixel			*/
	br_uint_32  width_b;		   /* In bytes					*/
	br_uint_32  width_p;		   /* In pixels				*/
	br_uint_32  height;		   /* In scanlines				*/
	br_uint_32  stride;		   /* In bytes					*/
	br_uint_32  size;		      /* stride * width_bytes 	*/
	br_uint_16  mip_offset;    /* ajb for mip map textures ect */
	br_boolean  alpha_pixels;  /* ajb transparent texture flag */

    br_boolean  in_video_memory; /* Handle if this is a texture */
	br_boolean	always_in_video_memory; /* This buffer _always_ resides in video memory */
    br_boolean  uncacheable;
    S3DTK_SURFACE  s3_surface;

    LPDIRECTDRAWSURFACE surface;  /* DirectDraw surface corresponding to this texture */
    struct render_buffer *next_on_card; /* Pointer to next surface in texture cache */
};

/*
 * Static work area for renderers
 */
struct prim_work {
	/*
	 * Buffers
	 */
	struct render_buffer colour;
//	struct render_buffer bump;
//	struct render_buffer depth;
//	struct render_buffer texture;

	/*
	 * Base and range for decal
	 */
//	br_int_32	decal_index_base;
//	br_int_32	decal_index_range;
//	br_int_32	decal_shade_height;

   /*
    * S3 Surface in card memory for texture mapping
    */

   S3DTK_SURFACE  s3_surface;

   /* Current buffer pointer so we know if the material has changed */

   struct br_buffer_stored *current;
	
   /* Indicator of whether current texture uses alpha */
   br_boolean  alpha_pixels;

};

extern struct prim_work BR_ASM_DATA work;

/*
 * New line and point clamps
 */
#define CLAMP_LP(x,y) { \
  if (x<0) x = 0; \
  if (y<0) y = 0; \
  if (x>=work.colour.width_p) \
    x--; \
  if (y>=work.colour.height) \
    y--; \
}

typedef struct prim_work * BR_EXPORT br_work_fn(void);

#ifdef __cplusplus
};
#endif
#endif


