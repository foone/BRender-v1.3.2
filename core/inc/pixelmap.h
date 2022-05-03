/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: pixelmap.h 1.7 1998/09/09 13:28:45 johng Exp $
 * $Locker: $
 *
 * A stopgap 2D pixelmap structure for brender. This should really be the
 * pixelmap data type from the underlying 2D system (whatever that will
 * be)
 *
 * Used for input (maps) and output (render buffer)
 */
#ifndef _PIXELMAP_H_
#define _PIXELMAP_H_

/*
 * Various types of pixel
 */
enum {
	BR_PMT_INDEX_1,
	BR_PMT_INDEX_2,
	BR_PMT_INDEX_4,

	/*
	 * Each pixel is an index into a colour map
	 */
	BR_PMT_INDEX_8,

	/*
	 * True colour RGB
	 */
	BR_PMT_RGB_555,		/* 16 bits per pixel */
	BR_PMT_RGB_565,		/* 16 bits per pixel */
	BR_PMT_RGB_888,		/* 24 bits per pixel */
	BR_PMT_RGBX_888,	/* 32 bits per pixel */
	BR_PMT_RGBA_8888,	/* 32 bits per pixel */

	/*
	 * YUV
	 */
	BR_PMT_YUYV_8888,	/* YU YV YU YV ... */
	BR_PMT_YUV_888,

	/*
	 * Depth
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_DEPTH_32,

	/*
	 * Opacity
	 */
	BR_PMT_ALPHA_8,

	/*
	 * Opacity + Index. 
	 */
	BR_PMT_INDEXA_88,

	/*
	 * Bump maps
	 */
	BR_PMT_NORMAL_INDEX_8,
	BR_PMT_NORMAL_XYZ,

	/*
	 * Wrong way around 15 bit true colour
	 */
	BR_PMT_BGR_555,

	/*
	 * 16 bit r,g,b & alpha
	 */
	BR_PMT_RGBA_4444,

	/*
	 * Handy types for converting to 15/16 bit
	 */
	BR_PMT_RBG_bab,
	BR_PMT_RBG_1aba,

	/*
	 * Pixelmap extensions.
	 */
	BR_PMT_RGB_332,
	BR_PMT_DEPTH_8,

	BR_PMT_ARGB_8888,
	BR_PMT_ALPHA_4,
	BR_PMT_INDEXA_44,
	BR_PMT_DEPTH_15,
	BR_PMT_DEPTH_31,
	BR_PMT_DEPTH_FP16,
	BR_PMT_DEPTH_FP15,

	BR_PMT_RGBA_5551,
	BR_PMT_ARGB_1555,
	BR_PMT_ARGB_4444,

	BR_PMT_MAX
};

/*
 * pixelmap flags
 */
enum {
	/*
	 * No direct access to pixels
	 */
	BR_PMF_NO_ACCESS                    = 0x01,

	BR_PMF_LINEAR						= 0x02,
	BR_PMF_ROW_WHOLEPIXELS				= 0x04,

	BR_PMF_PIXELS_NEAR					= 0x08,
	BR_PMF_PIXELS_NEAR_ALIAS			= 0x10,

	BR_PMF_KEYED_TRANSPARENCY			= 0x20,

	BR_PMF_KEEP_ORIGINAL				= 0x40,
	/*
	* Experimental - if set then pixelmap is tiled - low bits of Y are moved to least significant part of pixel address
	*/
        BR_PMF_ENABLE_TEXTURE_COMPRESSION       = 0x80,
//      BR_PMF_TILED = 0x80,
};

#define BR_PM_TILE_SIZE	2

/*
 * Public structure that is comaptible with a device_pixelmap
 */
typedef struct br_pixelmap {
	br_uint_32 _reserved;

	/*
	 * Optional identifier (when maps used as textures/tables etc.)
	 */
	char *identifier;

	/*																		
	 * pointer to raw pixel data											
	 */																		
	void *  pixels;															
																			
	/*																		
	 * Optional device speicific information								
	 */																		
	br_uint_32  pixels_qualifier;											
																			
	/*																		
	 * Optional pixel map when pixels are indexed.							
	 */																		
	struct br_pixelmap * map;

	/*
	 * Key colour ranges
	 */
	br_colour_range src_key;
	br_colour_range dst_key;

	/*
	 * Key colour
	 */
	br_uint_32 key;

	/*
	 * Byte difference between pixels at same column of adjacent rows
	 */																		
	br_int_16	row_bytes;													
																			
	/*																		
	 * if ! 0, offset (in top level rows) from top map start to top-1 map
	 */
	br_int_16	mip_offset;													
																			
	/*																		
	 * Type of pixels														
	 */																		
	br_uint_8	type;														
																			
	/*																		
	 * Flags																
	 */																		
        br_uint_8       flags;                                                                                                   

	/*
	 * Copy function
	 */
	br_uint_16	copy_function;
																			
	/*																		
	 * top left visible region in pixels from pixel at 'pixel' pointer		
	 */																		
	br_uint_16	base_x;														
	br_uint_16	base_y;														
																			
	/*																		
	 * Width and height of bitmap in pixels									
	 */																		
	br_uint_16	width;														
	br_uint_16	height;														
																			
	/*																		
	 * Local origin for any graphics system rendering into map, relative	
	 * to 'base'															
	 */																		
	br_int_16	origin_x;													
	br_int_16	origin_y;													
																			
	/*																		
	 * Workspace fields for user and database								
	 */																		
	void 		*user;														
	void		*stored;													

} br_pixelmap;

/*
 * A macro that declares the pixelmap member entries - this is so that
 * various compatible structures can be created
 */
#define BR_PIXELMAP_MEMBERS \
	/*																		\
	 * pointer to raw pixel data											\
	 */																		\
	void *  pm_pixels;														\
																			\
	/*																		\
	 * Optional device speicific information								\
	 */																		\
	br_uint_32  pm_pixels_qualifier;										\
																			\
	/*																		\
	 * Optional pixel map when pixels are indexed.							\
	 */																		\
	struct br_pixelmap * pm_map;											\
																			\
	/*																		\
	 * Key colour ranges													\
	 */																		\
	br_colour_range pm_src_key;												\
	br_colour_range pm_dst_key;												\
	                                                              \
	/*																		\
	 * Key colour															\
	 */																		\
	br_uint_32 pm_key;														\
																			\
	/*																		\
	 * Byte difference between pixels at same column of adjacent rows		\
	 */																		\
	br_int_16	pm_row_bytes;												\
																			\
	/*																		\
	 * if ! 0, offset (in top level rows) from top map start to top-1 map	\
	 */																		\
	br_int_16	pm_mip_offset;												\
																			\
	/*																		\
	 * Type of pixels														\
	 */																		\
	br_uint_8	pm_type;													\
																			\
	/*																		\
	 * Flags																\
	 */																		\
	br_uint_8	pm_flags;													\
                                                           \
	/*																		\
	 * Copy function														\
	 */																		\
	br_uint_16	pm_copy_function;											\
	                                                           \
																			\
	/*																		\
	 * top left visible region in pixels from pixel at 'pixel' pointer		\
	 */																		\
	br_uint_16	pm_base_x;													\
	br_uint_16	pm_base_y;													\
																			\
	/*																		\
	 * Width and height of bitmap in pixels									\
	 */																		\
	br_uint_16	pm_width;													\
	br_uint_16	pm_height;													\
																			\
	/*																		\
	 * Local origin for any graphics system rendering into map, relative	\
	 * to 'base'															\
	 */																		\
	br_int_16	pm_origin_x;												\
	br_int_16	pm_origin_y;												\
																			\
	/*																		\
	 * Workspace fields for user and database								\
	 */																		\
	void 		*pm_user;													\
	void		*pm_stored;													\

/*
 * Flags to BrPixelMapAllocate
 */
enum br_pixelmap_allocate_flags {
	BR_PMAF_NORMAL   	= 0x0000,		/* Setup pixelmap so that 0th scanline is at low memory		*/
	BR_PMAF_INVERTED 	= 0x0001,		/* Setup pixelmap so that 0th scanline is at high memory	*/
	BR_PMAF_NO_PIXELS   = 0x0002,		/* Don't allocate any pixel data							*/
};

/*
 * Channel flags
 */
enum br_pixelmap_channel_mask {
	BR_PMCHAN_INDEX	= 0x0001,
	BR_PMCHAN_RGB 	= 0x0002,
	BR_PMCHAN_DEPTH = 0x0004,
	BR_PMCHAN_ALPHA = 0x0008,
	BR_PMCHAN_YUV 	= 0x0010,
	BR_PMCHAN_VECTOR= 0x0020
};

/*
 * Matching pixelmap types
 */
enum br_pmmatch_type {
	BR_PMMATCH_OFFSCREEN,
	BR_PMMATCH_DEPTH_16,
	BR_PMMATCH_DEPTH,
	BR_PMMATCH_HIDDEN,
	BR_PMMATCH_HIDDEN_BUFFER,
	BR_PMMATCH_NO_RENDER,
	BR_PMMATCH_DEPTH_8,
	BR_PMMATCH_DEPTH_32,
	BR_PMMATCH_DEPTH_15,
	BR_PMMATCH_DEPTH_31,
	BR_PMMATCH_DEPTH_FP15,
	BR_PMMATCH_DEPTH_FP16,

	BR_PMMATCH_MAX
};


/*
 * Copy functions
 */
enum br_pixelmap_copy_function {
	BR_PMCOPY_NORMAL			= 0x0000,
	BR_PMCOPY_SRC_KEYED			= 0x0001,
	BR_PMCOPY_DST_KEYED			= 0x0002
};


/*
 * General 2D point and rectangle
 */
typedef  struct br_point {
	br_int_32	x;
	br_int_32	y;
} br_point;

typedef struct br_rectangle {
	br_int_32	x;
	br_int_32	y;
	br_int_32	w;
	br_int_32	h;
} br_rectangle ;

/*
 * General result for a clipping operation
 */
typedef enum br_clip_result {
	BR_CLIP_REJECT,
	BR_CLIP_PARTIAL,
	BR_CLIP_ACCEPT
} br_clip_result;


/*
 * Quantization methods used in mip-level generation for BR_PMT_INDEX_8
 */
enum{
	BR_QUANTIZE_RGB,
	BR_QUANTIZE_YIQ,
	BR_QUANTIZE_MAX
};

enum{
        BR_CONTROL_GAMMA_RGB    = 0x00000001,
        BR_CONTROL_GAMMA        = 0x00000002,
        BR_CONTROL_BRIGHTNESS   = 0x00000004,
        BR_CONTROL_CONTRAST     = 0x00000008,
        BR_CONTROL_HUE          = 0x00000010,
        BR_CONTROL_SATURATION   = 0x00000020,
        BR_CONTROL_SHARPNESS    = 0x00000040,
};

typedef struct{
        br_uint_32 flags;
        br_uint_16 gamma_red[256];
        br_uint_16 gamma_green[256];
        br_uint_16 gamma_blue[256];
        br_int_32 gamma;
        br_int_32 brightness;
        br_int_32 contrast;
        br_int_32 hue;
        br_int_32 saturation;
        br_int_32 sharpness;
}br_display_controls;

#endif
