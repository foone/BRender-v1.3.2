/*
 * Copyright (c) 1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: pmaps.h 1.1 1997/12/10 16:59:06 jon Exp $
 * $Locker: $
 *
 */
#ifndef _PMAPS_H_
#define _PMAPS_H_


typedef br_colour t_read_pixel_fn_type(char *pixels, br_float fract);
typedef void t_write_pixel_fn_type(char *pixels, br_float fract, br_colour new_pixel);

typedef struct t_pixelmap_type_info {

	t_read_pixel_fn_type *read_pixel_fn;
	t_write_pixel_fn_type *write_pixel_fn;
	
	t_read_pixel_fn_type *read_flip_pixel_fn;

	br_uint_8 needs_palette;

	br_int_32 type;		/* pixelmap->type */
	
	char *identifier;
	char *description;
    
} t_pixelmap_type_info;

/*
 * command types for pixelmap cbfn
 */
enum {
	T_CBFN_GENERATE_MAP_TABLE,
	T_CBFN_GENERATE_INDEXED_MAP_TABLE,
	T_CBFN_GENERATE_PALETTE_TABLE,
	T_CBFN_GENERATE_IMAGE_TABLE,
	T_CBFN_GENERATE_MIP_LEVELS,

	T_CBFN_CONVERT_PIXELMAP,
	T_CBFN_CONVERT_PALETTES,
	T_CBFN_REMAP_PIXELMAP,
	T_CBFN_QUANTIZE_PIXELMAP,
	T_CBFN_SCALE_PIXELMAP,
	T_CBFN_SQUARE_PIXELMAP,
	T_CBFN_FLIP_Y_PIXELMAP,
	T_CBFN_FLIP_X_PIXELMAP,
	T_CBFN_FLIP_HIGH_WIDE_PALETTES,
	T_CBFN_SET_ORIGIN,

	T_CBFN_COUNT_PALETTES,
	T_CBFN_VIEW_PIXELMAPS,

	T_CBFN_ASSIGN_PALETTE,

	T_CBFN_CONTROL_CONVERT,
	T_CBFN_CONTROL_REGEN_PALETTES,
	T_CBFN_CONTROL_REMAP,
	T_CBFN_CONTROL_QUANTIZE,
};

typedef struct t_pixelmap_list_type {

	t_list_node node;

	br_pixelmap *pixelmap;
    
} t_pixelmap_list_type;


/*
 * command structure passed to pixelmap cbfn
 */
typedef struct t_pixelmap_cbfn_info {

	br_uint_32 type;	/* cbfn type */

	br_pixelmap *current_pixelmap;
	br_pixelmap *current_palette;

	int nmaps;
	int npals;
	
	br_pixelmap **map_table;

	br_pixelmap **convert_table;

	t_list *pixelmap_list;
	
	int count;

	br_int_32 new_type;
	br_int_32 threshold;

	br_int_32 width;
	br_int_32 height;
	br_int_32 base;
	br_int_32 range;

	br_int_32 origin_x;
	br_int_32 origin_y;

	br_int_32 scale_x;
	br_int_32 scale_y;

	br_float fwidth;

	br_uint_32 offset;
	
} t_pixelmap_cbfn_info;

/*
 * Generic pixelmap callback function
 */
br_uint_32 BR_CALLBACK T_PixelmapCbfn(br_pixelmap *item, void *arg);

/*
 * Generate an array of pixelmaps from registry
 */
br_uint_32 T_GenerateMapTable(t_pixelmap_cbfn_info *cbfn_command);
br_uint_32 T_CountIndexedMaps(t_pixelmap_cbfn_info *cbfn_command);

/*
 * return pixelmap type from char *
 */
br_int_32 T_DecodePixelmapType(char *type_string);


/*
 * return string of pixelmap type
 */
char *T_EncodePixelmapType(br_int_32 type);

br_pixelmap *T_Convert(br_pixelmap *item, t_pixelmap_cbfn_info *cbfn_command);


extern t_pixelmap_type_info PixelmapTypes[];
extern int PixelmapTypesSize;

#endif
