/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: sbuffer.h 1.1 1997/12/10 16:46:26 jon Exp $
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
 * The minimal description of a pixelmap required for rendering
 */
struct render_buffer {
	MSImemptr heap;			/* Pointer to start of texture heap, if in one */
	MSImemptr base;			/* 0,0 pixel				*/

	br_uint_32 width_b;		/* In bytes					*/
	br_uint_32 width_p;		/* In pixels				*/
	br_uint_32 height;		/* In scanlines				*/
	br_uint_32 size;
	br_uint_32 bits;		/* Bits per pixel			*/

	br_scalar u_range;		/* BRender's U and V ranges for texture mapping */
	br_scalar v_range;		/*  (these have to be tinkered with, see sbuffer.c) */

	MSImemptr palette;		/* Pointer to palette		*/
	MSImemptr palette_heap;	/* Pointer to palette heap  */

    br_boolean uncacheable; /* Impossible to feed to card */
    br_boolean on_card;     /* Downloaded to card       */

	br_uint_32 card_offset;	/* Offset to texture in card memory */
	br_boolean upload;		/* Needs uploading this frame */
};

/*
 * Flags
 */
enum {
	SBUF_SHARED			= 0x0001,	/* Data is shared with user */
	SBUF_PREFER_SHARE	= 0x0002,	/* User requests share explicitly */
	SBUF_ON_HEAP		= 0x0004,	/* Data is on a texture heap */
};


/*
 * Block structure
 */
typedef struct br_buffer_stored {
	/* Dispatch table */
	struct br_buffer_stored_dispatch *dispatch;

	/* Standard object identifier */
	char *identifier;

	/* Primitive library */
	struct br_primitive_library *plib;

	/* Flags */
	br_uint_32 flags;

	/* Reduced info about buffer */
	struct render_buffer buffer;

	/* Pixelmap which this belongs to */
	br_pixelmap *pixelmap;

	/* Heap block */
	void *heap_block;
	void *palette_heap_block;

} br_buffer_stored;



#ifdef __cplusplus
};
#endif
#endif


