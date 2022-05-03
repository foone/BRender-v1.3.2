/*
 * Copyright (c) 1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: pmaps.c 1.2 1998/07/14 13:13:42 johng Exp jon $
 * $Locker: jon $
 *
 */
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <math.h>

#include "brender.h"
#include "dosio.h"

#include "texconv.h"

/*
 * Lookup table that tells everything needed about supported pixelmaps
 */
t_pixelmap_type_info PixelmapTypes[] = {

	NULL, NULL, NULL, 0, BR_PMT_INDEX_1, "BR_PMT_INDEX_1", "Not supported",
	NULL, NULL, NULL, 0, BR_PMT_INDEX_2, "BR_PMT_INDEX_2", "Not supported",
	T_Read_Index_4, T_Write_Index_4, T_Read_Flip_Index_4, 1, BR_PMT_INDEX_4, "BR_PMT_INDEX_4", "4 bit indexed",
	T_Read_Index_8, T_Write_Index_8, T_Read_Flip_Index_8, 1, BR_PMT_INDEX_8, "BR_PMT_INDEX_8", "8 bit indexed",

	T_Read_RGB_555, T_Write_RGB_555, T_Read_RGB_555, 0, BR_PMT_RGB_555, "BR_PMT_RGB_555", "RGB 16 bit 5 bits per colour component",
	T_Read_RGB_565, T_Write_RGB_565, T_Read_RGB_565, 0, BR_PMT_RGB_565, "BR_PMT_RGB_565", "RGB 16 bit 5,6,5 bit colour components",
	T_Read_RGB_888, T_Write_RGB_888, T_Read_RGB_888, 0, BR_PMT_RGB_888, "BR_PMT_RGB_888", "RGB 24 bit 8 bits per colour component",
	T_Read_RGBX_888, T_Write_RGBX_888, T_Read_RGBX_888, 0, BR_PMT_RGBX_888, "BR_PMT_RGBX_888", "RGB 32 bit 8 bits per colour component",
	T_Read_RGBX_888, T_Write_RGBX_888, T_Read_RGBX_888, 0, BR_PMT_RGBA_8888, "BR_PMT_RGBA_8888", "RGBA 32 bit 8 bits per colour component",

	NULL, NULL, NULL, 0, BR_PMT_YUYV_8888, "BR_PMT_YUYV_8888", "Not supported",
	NULL, NULL, NULL, 0, BR_PMT_YUV_888, "BR_PMT_YUV_888", "Not supported",
	NULL, NULL, NULL, 0, BR_PMT_DEPTH_16, "BR_PMT_DEPTH_16", "Not supported",
	NULL, NULL, NULL, 0, BR_PMT_DEPTH_32, "BR_PMT_DEPTH_32", "Not supported",
	T_Read_Alpha_8, T_Write_Alpha_8, T_Read_Alpha_8, 1, BR_PMT_ALPHA_8, "BR_PMT_ALPHA_8", "Opacity map",

	T_Read_IndexA_88, NULL, T_Read_Flip_IndexA_88, 0, BR_PMT_INDEXA_88, "BR_PMT_INDEXA_88", "Opacity + index",
	NULL, NULL, NULL, 0, BR_PMT_NORMAL_INDEX_8, "BR_PMT_NORMAL_INDEX_8","Not supported",
	NULL, NULL, NULL, 0, BR_PMT_NORMAL_XYZ, "BR_PMT_NORMAL_XYZ", "Not supported",
	T_Read_BGR_555, T_Write_BGR_555, T_Read_BGR_555, 0, BR_PMT_BGR_555, "BR_PMT_BGR_555", "Reversed 16 bit 5 bits per colour component",
    T_Read_ARGB_4444, T_Write_ARGB_4444, T_Read_ARGB_4444, 0, BR_PMT_ARGB_4444, "BR_PMT_ARGB_4444", "ARGB 16 bit 4 bits per colour component",

    NULL, NULL, NULL, 0, BR_PMT_RBG_bab     , "BR_PMT_RGB_bab", "Not supported",
    NULL, NULL, NULL, 0, BR_PMT_RBG_1aba    , "BR_PMT_RGB_1aba", "Not supported",
	NULL, NULL, NULL, 0, BR_PMT_RGB_332		, "BR_PMT_RGB_332", "Not supported",
	NULL, NULL, NULL, 0, BR_PMT_DEPTH_8		, "BR_PMT_DEPTH_8", "Not supported",
	NULL, NULL, NULL, 0, BR_PMT_ARGB_8888	, "BR_PMT_ARGB_8888", "Not supported",
	NULL, NULL, NULL, 0, BR_PMT_ALPHA_4		, "BR_PMT_ALPHA_4", "Not supported",
	NULL, NULL, NULL, 0, BR_PMT_INDEXA_44	, "BR_PMT_INDEXA_44", "Not supported",
	NULL, NULL, NULL, 0, BR_PMT_DEPTH_15	, "BR_PMT_DEPTH_15", "Not supported",
	NULL, NULL, NULL, 0, BR_PMT_DEPTH_31	, "BR_PMT_DEPTH_31", "Not supported",
	NULL, NULL, NULL, 0, BR_PMT_DEPTH_FP16	, "BR_PMT_DEPTH_FP16", "Not supported",
	NULL, NULL, NULL, 0, BR_PMT_DEPTH_FP15	, "BR_PMT_DEPTH_FP15", "Not supported",
											                      
	NULL, NULL, NULL, 0, BR_PMT_RGBA_5551	, "BR_PMT_RGBA_5551", "Not supported",
	NULL, NULL, NULL, 0, BR_PMT_ARGB_1555	, "BR_PMT_ARGB_1555", "Not supported",

    T_Read_RGBA_4444, T_Write_RGBA_4444, T_Read_RGBA_4444, 0, BR_PMT_RGBA_4444, "BR_PMT_RGBA_4444", "RGBA 16 bit 4 bits per colour component",

};

int PixelmapTypesSize = BR_ASIZE(PixelmapTypes);

/*
 * generate string of pixelmap type
 */
char *T_EncodePixelmapType(br_int_32 type)
{
	int i;
	char *type_str;

	for(i=0; i<PixelmapTypesSize; i++)
		if(type == PixelmapTypes[i].type) {
			type_str = PixelmapTypes[i].identifier;
			break;
		}

	if(i >= PixelmapTypesSize)
		BR_ERROR1("Unknown pixelmap type '%d'", type);

	return type_str;
}

/*
 * Generate a table of pixelmap pointers from the registry
 */
br_uint_32 T_GenerateMapTable(t_pixelmap_cbfn_info *cbfn_command)
{
	cbfn_command->nmaps = BrMapCount(NULL);
	cbfn_command->map_table = BrResAllocate(res_anchor,
		sizeof(*cbfn_command->map_table) * cbfn_command->nmaps, BR_MEMORY_APPLICATION);
	
	cbfn_command->count = 0;
	cbfn_command->type = T_CBFN_GENERATE_MAP_TABLE;

	/*
	 * Generate map table
	 */
	BrMapEnum(NULL, T_PixelmapCbfn, cbfn_command);

	return cbfn_command->nmaps;
}

br_uint_32 T_CountIndexedMaps(t_pixelmap_cbfn_info *cbfn_command)
{
	cbfn_command->type = T_CBFN_COUNT_PALETTES;
	cbfn_command->count = 0;
	cbfn_command->npals = 0;

	/*
	 * count number of pixelmaps with maps
	 */
	BrMapEnum(NULL, T_PixelmapCbfn, cbfn_command);
	cbfn_command->nmaps = cbfn_command->npals;

	return cbfn_command->nmaps;
}

/*
 * return pixelmap type from string
 */
br_int_32 T_DecodePixelmapType(char *type_string)
{
	br_int_32 type;
	int i;

	for(i=0; i<PixelmapTypesSize; i++)
		if(stricmp(type_string, PixelmapTypes[i].identifier) == 0) {
			type = PixelmapTypes[i].type;
			break;
		}

	if(i >= PixelmapTypesSize)
		BR_ERROR1("Unknown pixelmap type '%s'", type_string);
	    
	return type;
}

/*
 * wrapper for conversion (or remap) of a pixelmap
 */
br_pixelmap *T_Convert(br_pixelmap *item, t_pixelmap_cbfn_info *cbfn_command)
{

	br_pixelmap *pm;

	/*
	 * check if source and destination are both supported
	 */
	if(PixelmapTypes[item->type].read_pixel_fn == NULL)
		BR_ERROR4("Unable to convert '%s' of type %s to %s. %s source not supported.",
			item->identifier, T_EncodePixelmapType(item->type),
			T_EncodePixelmapType(cbfn_command->new_type), T_EncodePixelmapType(cbfn_command->new_type));

	if(PixelmapTypes[cbfn_command->new_type].write_pixel_fn == NULL)
		BR_ERROR2("Unable to convert to type %s. %s target not supported.",
			T_EncodePixelmapType(cbfn_command->new_type), T_EncodePixelmapType(cbfn_command->new_type));

	switch(cbfn_command->type) {

		case T_CBFN_FLIP_X_PIXELMAP:
		case T_CBFN_FLIP_Y_PIXELMAP:

			pm = T_ConvertDirectPixelmap(item, cbfn_command);
			
			if(item->map) {

				br_uint_32 type;

				/*
				 * make copy of palette
				 */
				cbfn_command->new_type = item->map->type;
				type = cbfn_command->type;
				cbfn_command->type = T_CBFN_CONVERT_PIXELMAP;
				
				pm->map = T_Convert(item->map, cbfn_command);

				cbfn_command->type = type;
			}

			break;

		default:
			if(PixelmapTypes[cbfn_command->new_type].needs_palette == 1)
				pm = T_ConvertIndexedPixelmap(item, cbfn_command);
			else
				pm = T_ConvertDirectPixelmap(item, cbfn_command);

			break;
	}
	
	return pm;
}

br_pixelmap *T_Scale(br_pixelmap *item, t_pixelmap_cbfn_info *cbfn_command) {

	br_pixelmap *temp_pm, *pm;
	br_int_32 temp_type;

	temp_type = item->type; /* save this for later */

	cbfn_command->new_type = BR_PMT_RGB_888;
	cbfn_command->threshold = alpha_threshold;

	temp_pm = T_Convert(item, cbfn_command);

	BrScaleBegin();
			
	pm = BrPixelmapScale(temp_pm, cbfn_command->scale_x, cbfn_command->scale_y, cbfn_command->fwidth);

	BrScaleEnd();

	BrPixelmapFree(temp_pm);

	cbfn_command->new_type = temp_type;
	cbfn_command->threshold = alpha_threshold;

	switch(cbfn_command->new_type) {
			
		case BR_PMT_INDEX_8:
		case BR_PMT_ALPHA_8:

			cbfn_command->range = cbfn_command->height = 256;
			cbfn_command->base = 0;

			break;
		case BR_PMT_INDEX_4:

			cbfn_command->range = cbfn_command->height = 16;
			cbfn_command->base = 0;

			break;
	}
	temp_pm = T_Convert(pm, cbfn_command);
	BrPixelmapFree(pm);

	temp_pm->identifier = BrMemStrDup(item->identifier);

	return temp_pm;
}

/*
 * generic map registry callback function
 */
br_uint_32 BR_CALLBACK T_PixelmapCbfn(br_pixelmap *item, void *arg)
{    
	t_pixelmap_cbfn_info *cbfn_command = (t_pixelmap_cbfn_info *)arg;
	br_pixelmap *pm;
	br_uint_32 ret = 0;
	int i;

	cbfn_command->current_pixelmap = item;

	switch(cbfn_command->type) {

		/*
		 * generate a table of pixelmap pointers
		 */
		case T_CBFN_GENERATE_MAP_TABLE:
		
			cbfn_command->map_table[cbfn_command->count++] = item;
		break;

		/*
		 * generate a table of pixelmaps
		 * indexed pixelmaps have no palettes
		 */
		case T_CBFN_GENERATE_IMAGE_TABLE:
		    
			if(item->map != NULL) {
				    
				pm = BrResAllocate(cbfn_command, sizeof(br_pixelmap), BR_MEMORY_APPLICATION);
				*pm = *item;
				pm->map = NULL;
			} else
				pm = item;
				
			cbfn_command->map_table[cbfn_command->count++] = pm;
		break;

		/*
		 * generate a table of palettes
		 */
		case T_CBFN_GENERATE_PALETTE_TABLE:

			if(item->map != NULL) {
			     i = 0;
			     while (i < cbfn_command->count) {
				if (cbfn_command->map_table[i] != item->map)
				  i++;
				else break;
			     }
			     if (i == cbfn_command->count)
			     cbfn_command->map_table[cbfn_command->count++] = item->map;
			}    
		break;

		/*
		 * generate mip levels
		 */
		case T_CBFN_GENERATE_MIP_LEVELS: {

			int level = 0;
			int width, height;
			t_pixelmap_list_type *tpm;
			char _scratch_string[256];
			char _base_string[256];

			strcpy(_base_string, item->identifier);
			cbfn_command->new_type = item->type;

			T_Log("Generating mip levels for '%s' (%s)\n", item->identifier, T_EncodePixelmapType(item->type));

			sprintf(_scratch_string, "%s_%d", _base_string, level);
			item->identifier = BrMemStrDup(_scratch_string);
			T_Log("   Level %d '%s' (%d,%d)\n", level++, item->identifier, item->width, item->height);

			for(width = item->width >> 1, height = item->height >> 1; (width > 0) && (height > 0); width >>= 1, height >>= 1, level++) {
				/*
				 * generate mip level
				 */
				cbfn_command->scale_x = width;
				cbfn_command->scale_y = height;
				cbfn_command->fwidth = (width > 3)?3:width;

				tpm = BrResAllocate(cbfn_command, sizeof(*tpm), BR_MEMORY_APPLICATION);

				tpm->pixelmap = T_Scale(item, cbfn_command);

				/*
				 * keep this!
				 */
				T_AddTail(cbfn_command->pixelmap_list, tpm);

				sprintf(_scratch_string, "%s_%d", _base_string, level);
				tpm->pixelmap->identifier = BrMemStrDup(_scratch_string);
				T_Log("   Level %d '%s' (%d,%d)\n", level, tpm->pixelmap->identifier, width, height);
				
				cbfn_command->count++;
			}
		}
		break;

		/*
		 * count number of pixelmaps that have palette (pm->map)
		 */
		case T_CBFN_COUNT_PALETTES:
		
			if(item->map != NULL)
				cbfn_command->npals++;

		break;

		/*
		 * convert pixelmap from one type to another
		 * store new pixelmap in convert table[count]
		 */
		case T_CBFN_CONVERT_PIXELMAP:

			pm = T_Convert(item, cbfn_command);

			cbfn_command->convert_table[cbfn_command->count++] = pm;

			T_Log("Converted '%s' %s (%d bit) to %s (%d bit)", item->identifier, T_EncodePixelmapType(item->type),
				BrPixelmapPixelSize(item), T_EncodePixelmapType(pm->type), BrPixelmapPixelSize(pm));

			if(PixelmapTypes[pm->type].needs_palette == 1)
				T_Log(" with palette type %s (%d bit)", T_EncodePixelmapType(pm->map->type),
					BrPixelmapPixelSize(pm->map));

			T_Log("\n");
		break;

		/*
		 * convert pixelmap->map to another type
		 */
		case T_CBFN_CONVERT_PALETTES:

			if(item->map) {

				cbfn_command->new_type = palette_type;
				pm = T_Convert(item->map, cbfn_command);

				cbfn_command->map_table[cbfn_command->count] = item;
				cbfn_command->convert_table[cbfn_command->count++] = pm;

				T_Log("Converted palette from '%s' (%s) from %s to %s\n",
					item->identifier, T_EncodePixelmapType(item->type),
					T_EncodePixelmapType(item->map->type), T_EncodePixelmapType(pm->type));
			}

		break;
		
		/*
		 * remap pixelmap
		 */
		case T_CBFN_REMAP_PIXELMAP:

			switch(item->type) {

				default:
				case BR_PMT_INDEX_8:

					cbfn_command->height = 256;
					cbfn_command->new_type = BR_PMT_INDEX_8;
				
					break;
					
				case BR_PMT_INDEX_4:

					cbfn_command->height = 16;
					cbfn_command->new_type = BR_PMT_INDEX_4;
				
					break;
			}

			pm = T_Convert(item, cbfn_command);
				
			cbfn_command->convert_table[cbfn_command->count++] = pm;

			T_Log("Remapped '%s' %s (%d bit) to %s (%d bit)", item->identifier, T_EncodePixelmapType(item->type),
				BrPixelmapPixelSize(item), T_EncodePixelmapType(pm->type), BrPixelmapPixelSize(pm));

			if(PixelmapTypes[pm->type].needs_palette == 1)
				T_Log(" with palette type %s (%d bit, %d,%d)", T_EncodePixelmapType(pm->map->type),
					BrPixelmapPixelSize(pm->map), cbfn_command->base, cbfn_command->range);

			T_Log("\n");

		break;

		/*
		 * quantize pixelmap to palette
		 */
		case T_CBFN_QUANTIZE_PIXELMAP:

			switch(cbfn_command->current_palette->height) {

				case 16:
					cbfn_command->new_type = BR_PMT_INDEX_4;
					cbfn_command->height = 16;
					
					break;
				default:
					cbfn_command->new_type = BR_PMT_INDEX_8;
					cbfn_command->height = 256;

					break;                      
			}

			pm = T_Convert(item, cbfn_command);

			cbfn_command->convert_table[cbfn_command->count++] = pm;

			T_Log("Quantized '%s' %s (%d bit) to %s (%d bit)", item->identifier, T_EncodePixelmapType(item->type),
				BrPixelmapPixelSize(item), T_EncodePixelmapType(pm->type), BrPixelmapPixelSize(pm));

			if(PixelmapTypes[pm->type].needs_palette == 1)
				T_Log(" with palette type %s (%d bit, %d,%d)", T_EncodePixelmapType(pm->map->type),
					BrPixelmapPixelSize(pm->map), cbfn_command->base, cbfn_command->range);

			T_Log("\n");
			
		break;

		/*
		 * assign palette to all indexed pixelmaps
		 */
		case T_CBFN_ASSIGN_PALETTE:

			switch(item->type) {

				case BR_PMT_INDEX_4:
				case BR_PMT_INDEX_8:
				case BR_PMT_ALPHA_8:
						/*
						 * this may lead to stry palettes in memory
						 */
						item->map = cbfn_command->current_palette;
						break;
			}

			cbfn_command->map_table[cbfn_command->count] = NULL;
			cbfn_command->convert_table[cbfn_command->count++] = item;
			
			T_Log("Assigned palette '%s' (%s) to '%s'\n", cbfn_command->current_palette->identifier,
				T_EncodePixelmapType(cbfn_command->current_palette->type),
				item->identifier,
				T_EncodePixelmapType(item->type));

		break;

		/*
		 * scale pixelmap
		 */
		case T_CBFN_SCALE_PIXELMAP:

			pm = T_Scale(item, cbfn_command);

			cbfn_command->convert_table[cbfn_command->count++] = pm;

			T_Log("Scaled '%s' (%s) to %d,%d\n", pm->identifier, T_EncodePixelmapType(pm->type),
				pm->width, pm->height);

		break;

		/*
		 * square pixelmap
		 */
		case T_CBFN_SQUARE_PIXELMAP:
			{
				// Find the largest side and ...
				br_int_32 size = (item->width > item->height ?
								 item->width : item->height);

				double poweroftwo = ceil(log ((double) size) / log(2));

#if defined _DEBUG || defined DEBUG
				printf ("[m1] p:%lf ", poweroftwo);
#endif


				// ... calculate the smallest power of 2 not smaller than this value ...
				size = (br_int_32) pow (2, poweroftwo);

				// ... which I now assign as the requested pixelmap size.
				cbfn_command->scale_x = cbfn_command->scale_y = size;

#if defined _DEBUG || defined DEBUG
				printf ("[m2] w:%ld h:%ld s:%ld ", item->width, item->height, size);
#endif

				pm = T_Scale(item, cbfn_command);

				cbfn_command->convert_table[cbfn_command->count++] = pm;

				T_Log("Scaled '%s' (%s) to %d,%d\n", pm->identifier, T_EncodePixelmapType(pm->type),
					pm->width, pm->height);
			}
		break;

		/*
		 * flip y
		 */
		case T_CBFN_FLIP_Y_PIXELMAP:

			cbfn_command->new_type = item->type;

			pm = T_Convert(item, cbfn_command);

			cbfn_command->convert_table[cbfn_command->count++] = pm;

			T_Log("Flipped '%s' (%s) top/bottom\n", pm->identifier, T_EncodePixelmapType(pm->type));
		break;

		/*
		 * flip x
		 */
		case T_CBFN_FLIP_X_PIXELMAP:

			cbfn_command->new_type = item->type;

			pm = T_Convert(item, cbfn_command);

			cbfn_command->convert_table[cbfn_command->count++] = pm;

			T_Log("Flipped '%s' (%s) left/right\n", pm->identifier, T_EncodePixelmapType(pm->type));
		break;

		/*
		 * reset origin
		 */
		case T_CBFN_SET_ORIGIN:

			item->origin_x = cbfn_command->origin_x;
			item->origin_y = cbfn_command->origin_y;

			T_Log("Origin of '%s' set to %d,%d\n", item->identifier, item->origin_x, item->origin_y);

		break;

		/*
		 * toggle high/wide palettes
		 */
		case T_CBFN_FLIP_HIGH_WIDE_PALETTES:

			if(item->map) {

				cbfn_command->new_type = item->map->type;
				
				pm = T_Convert(item->map, cbfn_command);

				cbfn_command->map_table[cbfn_command->count] = item;
				cbfn_command->convert_table[cbfn_command->count++] = pm;

				T_Log("Flipped palette (%s) from '%s' (%s) from %d,%d to %d,%d\n",
					T_EncodePixelmapType(item->map->type),
					item->identifier,
					T_EncodePixelmapType(item->type),
					pm->height, pm->width, pm->width, pm->height);
			}
		
		break;

		case T_CBFN_VIEW_PIXELMAPS:

			pm = DOSGfxBegin(NULL);

			if(pm->type == item->type) {

				int w,h,x,y;
				br_pixelmap *palette;
				br_int_16 orgx,orgy;

				orgx = item->origin_x;
				orgy = item->origin_y;

				item->origin_x = 0;
				item->origin_y = 0;

				w = (item->width>pm->width)?pm->width:item->width;
				h = (item->height>pm->height)?pm->height:item->height;

				x = (pm->width - w) >> 1;
				y = (pm->height - h) >> 1;

				if(item->map != NULL) {
					/*
					 * convert palette to BR_PMT_RGBX_888
					 */
					cbfn_command->new_type = BR_PMT_RGBX_888;
					palette = T_Convert(item->map, cbfn_command);

					/*
					 * convert high/wide
					 */
					if(palette->width > palette->height) {

						br_pixelmap *temp_pm;
						br_uint_32 old_type;                                

						cbfn_command->new_type = palette->type;
						old_type = cbfn_command->type;
						cbfn_command->type = T_CBFN_FLIP_HIGH_WIDE_PALETTES;
				
						temp_pm = T_Convert(palette, cbfn_command);

						BrPixelmapFree(palette);

						palette = temp_pm;
						cbfn_command->type = old_type;
					}
					
					DOSGfxPaletteSet(palette);

					BrPixelmapFree(palette);
				}
				
				BrPixelmapRectangleCopy(pm,x,y,item,0,0,w,h);
	
				item->origin_x = orgx;
				item->origin_y = orgy;

				if(getch() == 's') {

					br_pixelmap *temp_pm;

					temp_pm = BrPixelmapAllocate(pm->type, pm->width, pm->height, NULL, 0);
					BrPixelmapRectangleCopy(temp_pm, 0, 0, pm, 0, 0, pm->width, pm->height);
									    
					BrPixelmapSave("out.pix", temp_pm);

					BrPixelmapFree(temp_pm);
				}
			
				DOSGfxEnd();

			} else {
				DOSGfxEnd();

				T_Log("Cannot display '%s' (%s) to screen pixelmap of type %s. Press any key.\n",
					item->identifier, T_EncodePixelmapType(item->type),
					T_EncodePixelmapType(pm->type));

				getch();
			}

		break;
	}

	return ret;
}
