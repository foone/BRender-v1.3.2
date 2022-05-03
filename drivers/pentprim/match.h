/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: match.h 1.1 1997/12/10 16:47:19 jon Exp $
 * $Locker: $
 *
 * Bits used to match renderer state to primitives
 */
#ifndef _MATCH_H_
#define _MATCH_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void BR_ASM_CALL rasterise_fn(void);
typedef void BR_ASM_CALL setup_param_fn(void);

/*
 * Local version of brp_info extended with information used to match primitve with current state
 */
struct local_block {
	brp_block	p;

	/**
	 ** Extra information needed by primitive library for range information
	 **
	 ** Custom blocks only need to provide the brp_block and these fields
	 **/

	/*
	 * Default offset and scale for R,G,B,A
	 */
	br_scalar	colour_offsets[4];
	br_scalar	colour_scales[4];

	/*
	 * Flags telling primitive library and special requirements for this primitive
	 */
	br_uint_32 range_flags;

	/*
	 * pointer to static parameter work area
	 */
	struct prim_work *work;

	/**
	 ** Extra information needed by primitive library for matching internal
	 ** blocks against the current state
	 **/

	/*
	 * Mask of match flags
	 */
	br_uint_32	flags_mask;
	br_uint_32	flags_cmp;

	/*
	 * Type of maps supported, if any
	 */
	br_int_32	depth_type;
	br_int_32	texture_type;
	br_int_32	shade_type;
	br_int_32	blend_type;
	br_int_32	screendoor_type;
	br_int_32	lighting_type;
	br_int_32	bump_type;
	br_int_32   fog_type;

	/*
	 * Used to check if the colour.width_b's of the two pixelmaps match -
	 *
	 *	if colour colour.width_b * depth_row_size != depth colour.width_b * colour_row_size
	 *	then this primtiive will not match
	 */
	br_uint_8	colour_row_size;
	br_uint_8	depth_row_size;

	/*
	 * Texture dimensions required (0 if don't care)
	 */
	br_int_32	map_width;
	br_int_32	map_height;

	/*
	 * Type of input colour (indexed or RGB)
	 */
	br_token	input_colour_type;

	/*
	 * Information for autoloading
	 */
	char	*image_name;
	void	*entry_info;

	/*
	 * Information for generic setup code
	 */
	struct generic_setup_info {
		/*
		 * Value of 1/2area at which to switch from small to large rasterisers
		 */
		float iarea_limit;

		/*
		 * Size of workspace required
		 */
		br_size_t param_size;

		/*
		 * Rasterisers pixel stride (has to be a power of 2)
		 */
		br_int_32 stride;

		/*
		 * Code to do remainder of parameter setup
		 */
		setup_param_fn	* setup_param;

		/*
		 * Pointers to rasterisers for large triangles, left to right and right to left
		 */
		rasterise_fn * rasterise_rl_l;
		rasterise_fn * rasterise_lr_l;

		/*
		 * Pointers to rasterisers for small triangles, left to right and right to left
		 */
		rasterise_fn * rasterise_rl_s;
		rasterise_fn * rasterise_lr_s;


	} setup;
};

struct custom_block {
	brp_block	p;

	/**
	 ** Extra information needed by primitive library for range information
	 **
	 ** Custom blocks only need to provide the brp_block and these fields
	 **/

	/*
	 * Default offset and scale for R,G,B,A
	 */
	br_scalar	colour_offsets[4];
	br_scalar	colour_scales[4];

	/*
	 * Flags telling primitive library and special requirements for this primitive
	 */
	br_uint_32 range_flags;

	/*
	 * pointer to static parameter work area
	 */
	struct prim_work *work;
};

/*
 * Valid range flags
 */
enum {
	RF_DECAL						= 0x0001,
	RF_RGB_SHADE					= 0x0002,
	RF_UNSCALED_TEXTURE_COORDS		= 0x0004,
	RF_FLUSH						= 0x0008,
	RF_OFFSET_Y						= 0x0010,		/* Change offset for MMX setup 				*/
	RF_NEED_SUBDIVIDE				= 0x0020		/* Force the SUBDIVIDE flag in prim block	*/
};

#ifdef __cplusplus
};
#endif
#endif

