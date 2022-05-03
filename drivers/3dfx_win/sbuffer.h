/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
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
	void *base;				/* 0,0 pixel				*/
	br_uint_32 width_b;		/* In bytes					*/
	br_uint_32 width_p;		/* In pixels				*/
	br_uint_32 height;		/* In scanlines				*/
	br_uint_32 size;

    br_boolean uncacheable; /* Impossible to feed to card */
    br_boolean on_card;     /* Downloaded to card       */
	br_boolean force_reload; /* Force reload (i.e. update) */
	br_boolean blended;		/* Texture has alpha channel */

    void *palette_pointer;  /* Pointer to palette       */

	br_scalar u_range;		/* Ranges for map coordinates */
	br_scalar v_range;

    /* Glide parameters for rendering */
    GrChipID_t  tmu_id;     /* TMU on which texture is stored */
    br_uint_32  startAddress; /* Address in card mem    */
    GrTexInfo   info;       /* Glide's info for texture */
};

/*
 * Flags
 */
enum {
	SBUFF_SHARED	= 0x0001,		/* Data is shared with user */
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

	/* pointers to pixel data */
	void *pixels_pointer;
	void *pixels_converted;

} br_buffer_stored;



#ifdef __cplusplus
};
#endif
#endif


