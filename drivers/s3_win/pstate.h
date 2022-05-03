/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: pstate.h 1.1 1997/12/10 16:51:19 jon Exp $
 * $Locker: $
 *
 * Private primitive state structure
 */
#ifndef _PSTATE_H_
#define _PSTATE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Mask bits for state
 */
enum {
	MASK_STATE_OUTPUT		= BR_STATE_OUTPUT,
	MASK_STATE_PRIMITIVE	= BR_STATE_PRIMITIVE,
   MASK_STATE_CACHE     = BR_STATE_CACHE,

	MASK_STATE_ALL			= BR_STATE_OUTPUT|BR_STATE_PRIMITIVE
};




/*
 * state.prim.flags
 */
enum {
	/*
	 * The public flags
	 */
	PRIMF_SMOOTH_BIT,
	PRIMF_PERSPECTIVE_BIT,
	PRIMF_DECAL_BIT,
	PRIMF_DEPTH_WRITE_BIT,
	PRIMF_COLOUR_WRITE_BIT,
	PRIMF_INDEXED_COLOUR_BIT,
	PRIMF_BLEND_BIT,
	PRIMF_MODULATE_BIT,

	/*
	 * Internal flags using for matching
	 */
	PRIMF_OPAQUE_MAP_BIT,
	PRIMF_NO_SKIP_BIT,
};




enum {
	PRIMF_SMOOTH			= (1<<PRIMF_SMOOTH_BIT),
	PRIMF_PERSPECTIVE		= (1<<PRIMF_PERSPECTIVE_BIT),
	PRIMF_DECAL				= (1<<PRIMF_DECAL_BIT),
	PRIMF_DEPTH_WRITE		= (1<<PRIMF_DEPTH_WRITE_BIT),
	PRIMF_COLOUR_WRITE   = (1<<PRIMF_COLOUR_WRITE_BIT),
	PRIMF_INDEXED_COLOUR = (1<<PRIMF_INDEXED_COLOUR_BIT),
	PRIMF_BLEND			   = (1<<PRIMF_BLEND_BIT),
	PRIMF_MODULATE		   = (1<<PRIMF_MODULATE_BIT),

	PRIMF_OPAQUE_MAP		= (1<<PRIMF_OPAQUE_MAP_BIT),
	PRIMF_NO_SKIP			= (1<<PRIMF_NO_SKIP_BIT),

	PRIMF_INTERNAL			= (PRIMF_OPAQUE_MAP|
								   PRIMF_NO_SKIP)
};



struct output_buffer
{
	/*
	 * object on pixelmap
	 */
	struct br_device_pixelmap *pixelmap;

	/*
	 * Cached info about current buffer - used to detect
	 * changes that will affect CACHE contents
	 */
	br_uint_32 width;
	br_uint_32 height;
	br_uint_32 type;
};

typedef struct br_primitive_state {
	/*
	 * Dispatch table
	 */
	struct br_primitive_state_dispatch *dispatch;

	/*
	 * Standard object identifier
	 */
	char *identifier;

	struct br_primitive_library *plib;
	
	/*
	 * flags
	 */
//    br_uint_32 flags;


   /*
    * PRIMITIVE part of state
    */
   struct
      {
      /* Timestamp for any change of this state */
      br_uint_32 timestamp;

      /* Timestamp for any changes other than input buffer addresses */
      br_uint_32 timestamp_major;

      /* flags */
      br_uint_32 flags;

      /* Type of colour */
      br_token colour_type;

      /* Current input buffers */
      struct br_buffer_stored *colour_map;

      /* SFX control */
      br_token    map_antialiasing;
      br_token    map_interpolation;
      br_token    mip_interpolation;
      br_token    fog_type;
      br_colour   fog_colour;
      float       fog_min;
      float       fog_max;
      float       alpha_val;
	   br_int_32 index_base;
	   br_int_32 index_range;
      } prim;


   /*
    * OUTPUT part of state
    */
   struct
      {
      /* Timestamp for any change of this state */
      br_uint_32 timestamp;

      /* Timestamp for any changes other than input buffer addresses */
      br_uint_32 timestamp_major;

      /* Current output buffers */
      struct output_buffer colour;
      struct output_buffer depth;
      } out;


   /*
    * Cached info derived from rest of state
    */
   struct
      {
      /* Last primitive block matched to this state */
      struct local_block *last_block;
      br_token last_type;

      /* Ranges for components */
      float comp_offsets[NUM_COMPONENTS];
      float comp_scales[NUM_COMPONENTS];

      /* Copies of timestamps for output and prim. states that
       * block & ranges are based on */
      br_uint_32 timestamp_prim;
      br_uint_32 timestamp_out;
      } cache;

} br_primitive_state;

#ifdef __cplusplus
};
#endif
#endif


