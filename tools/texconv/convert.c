/*
 * Copyright (c) 1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: convert.c 1.2 1998/07/14 13:13:30 johng Exp $
 * $Locker: $
 *
 */
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "brender.h"
#include "dosio.h"

#include "texconv.h"

extern br_boolean LEAVE_TRANSPARENT;

#define BR_COLOUR_ARGB(a,r,g,b) \
		((((unsigned int)(a))<<24) |\
		(((unsigned int)(r))<<16) |\
		(((unsigned int)(g))<<8) |\
		((unsigned int)(b)))

static br_pixelmap *current_src;
static br_pixelmap *current_dst;

/*
 * per pixel read/write operations
 */
br_colour T_Read_Alpha_8(char *pixels, br_float fract)
{
	br_uint_8 index;

	index = *((br_uint_8 *)pixels);

	return BR_COLOUR_ARGB(index, 0, 0, 0);
}

br_colour T_Read_Flip_Index_8(char *pixels, br_float fract)
{
	br_uint_8 index;

	index = *((br_uint_8 *)pixels);

	return BR_COLOUR_ARGB(0, 0, 0, index);
}

void T_Write_Alpha_8(char *pixels, br_float fract, br_colour new_pixel)
{
	/* colour format R G B R G B R G B */

	*((br_uint_8 *)pixels) = BR_ALPHA(new_pixel);
}

br_colour T_Read_IndexA_88(char *pixels, br_float fract)
{
    	/* palette colour format ??? */

	br_uint_8 index, alpha;
	char *s_pixels;
	br_colour colour;
	br_pixelmap *pm;
	br_float s_fract, s_step;

	index = ((br_uint_8 *)pixels)[1];
	alpha = ((br_uint_8 *)pixels)[0];

	if(current_src->map == NULL)
		BR_ERROR2("'%s' (%s) is an indexed pixelmap but has no palette information", current_src->identifier,
			T_EncodePixelmapType(current_src->type));

	pm = current_src;
	current_src = current_src->map;
	s_step = BrPixelmapPixelSize(current_src)/8;

	if(current_src->height > current_src->width) {
		s_pixels = ((char *)current_src->pixels) + ((index + current_src->base_y) * current_src->row_bytes);
		s_fract = (current_src->base_x) * s_step;
	} else {
		s_pixels = ((char *)current_src->pixels) + (current_src->base_y * current_src->row_bytes);
		s_fract = (current_src->base_x + index) * s_step;
	}

	colour = PixelmapTypes[current_src->type].read_pixel_fn(s_pixels + ((int)s_fract), ((int)s_fract));

	current_src = pm;

	colour = BR_COLOUR_ARGB(alpha, BR_RED(colour), BR_GRN(colour), BR_BLU(colour));

	return colour;
}

br_colour T_Read_Flip_IndexA_88(char *pixels, br_float fract)
{
	br_uint_8 index, alpha;

	index = ((br_uint_8 *)pixels)[1];
	alpha = ((br_uint_8 *)pixels)[0];

	return index;
}

br_colour T_Read_Index_8(char *pixels, br_float fract)
{
    	/* palette colour format ??? */

	br_uint_8 index;
	char *s_pixels;
	br_colour colour;
	br_pixelmap *pm;
	br_float s_fract, s_step;

	index = *((br_uint_8 *)pixels);
              
       	if(current_src->map == NULL)
		BR_ERROR2("'%s' (%s) is an indexed pixelmap but has no palette information", current_src->identifier,
			T_EncodePixelmapType(current_src->type));

        if (LEAVE_TRANSPARENT) {
            // If Colour 0 then Transparent
            if (index == 0) return BR_COLOUR_ARGB(255,0,0,0);
        }

	pm = current_src;
	current_src = current_src->map;
	s_step = BrPixelmapPixelSize(current_src)/8;

	if(current_src->height > current_src->width) {
		s_pixels = ((char *)current_src->pixels) + ((index + current_src->base_y) * current_src->row_bytes);
		s_fract = (current_src->base_x) * s_step;
	} else {
		s_pixels = ((char *)current_src->pixels) + (current_src->base_y * current_src->row_bytes);
		s_fract = (current_src->base_x + index) * s_step;
	}

        colour = PixelmapTypes[current_src->type].read_pixel_fn(s_pixels + ((int)s_fract), ((int)s_fract));

	current_src = pm;

	return colour;
}

void T_Write_Index_8(char *pixels, br_float fract, br_colour new_pixel)
{
	/*
	 * write an index
	 */
	 
	*((br_uint_8 *)pixels) = (br_uint_8)new_pixel;
}

br_colour T_Read_Index_4(char *pixels, br_float fract)
{
    	/* palette colour format ??? */

	br_uint_8 index;
	char *s_pixels;
	br_colour colour;
	br_pixelmap *pm;
	br_float s_step, s_fract;

	if(fract == 0)
		/*
		 * even number pixel
		 */
		index = (*((br_uint_8 *)pixels)) & 0x0f;
	else
		/*
		 * odd number pixel
		 */
		index = (*((br_uint_8 *)pixels) >> 4) & 0x0f;

	if(current_src->map == NULL)
		BR_ERROR2("'%s' (%s) is an indexed pixelmap but has no palette information", current_src->identifier,
			T_EncodePixelmapType(current_src->type));

	pm = current_src;
	current_src = current_src->map;
	s_step = ((br_float)BrPixelmapPixelSize(current_src))/8.0;

	if(current_src->height > current_src->width) {
		s_pixels = ((char *)current_src->pixels) + ((index + current_src->base_y) * current_src->row_bytes);
		s_fract = (current_src->base_x) * s_step;
	} else {
		s_pixels = ((char *)current_src->pixels) + (current_src->base_y * current_src->row_bytes);
		s_fract = (current_src->base_x + index) * s_step;
	}

	colour = PixelmapTypes[current_src->type].read_pixel_fn(s_pixels + ((int)s_fract), s_fract - ((int)s_fract));

	current_src = pm;

	return colour;
}

br_colour T_Read_Flip_Index_4(char *pixels, br_float fract)
{
	br_uint_8 index;

	if(fract == 0)
		/*
		 * even number pixel
		 */
		index = (*((br_uint_8 *)pixels)) & 0x0f;
	else
		/*
		 * odd number pixel
		 */
		index = (*((br_uint_8 *)pixels) >> 4) & 0x0f;

	return index;
}

void T_Write_Index_4(char *pixels, br_float fract, br_colour new_pixel)
{
	/*
	 * write an index
	 */
	if(fract == 0)
		/*
		 * even number pixel
		 */
		*((br_uint_8 *)pixels) = (*((br_uint_8 *)pixels) & 0xf0) | (new_pixel & 0x0f);
	else
		/*
		 * odd number pixel
		 */
		*((br_uint_8 *)pixels) = (*((br_uint_8 *)pixels) & 0x0f) | ((new_pixel << 4)& 0xf0);

}

br_colour T_Read_RGB_888(char *pixels, br_float fract)
{
	/* colour format R G B R G B R G B */
    
	br_colour colour;
	br_uint_8 r,g,b;

	r = ((br_uint_8 *)pixels)[2];
	g = ((br_uint_8 *)pixels)[1];
	b = ((br_uint_8 *)pixels)[0];

	colour = BR_COLOUR_ARGB(0,r,g,b);
	
	return colour;    
}

void T_Write_RGB_888(char *pixels, br_float fract, br_colour new_pixel)
{
	/* colour format R G B R G B R G B */

	((br_uint_8 *)pixels)[2] = BR_RED(new_pixel);
	((br_uint_8 *)pixels)[1] = BR_GRN(new_pixel);
	((br_uint_8 *)pixels)[0] = BR_BLU(new_pixel);
}

void T_Write_RGBX_888(char *pixels, br_float fract, br_colour new_pixel)
{
	/* colour format X R G B X R G B X R G B */

	*((br_colour *)pixels) = new_pixel;
}
br_colour T_Read_RGBX_888(char *pixels, br_float fract)
{
	/* colour format X R G B X R G B X R G B */

	return *((br_colour *)pixels);
}

br_colour T_Read_RGB_555(char *pixels, br_float fract)
{
	/* colour format R G B R G B R G B  - 2 bytes per pixel */
    
	br_uint_16 temp_colour;
	br_uint_16 r,g,b;

	temp_colour = *((br_uint_16 *)pixels);

	r = (temp_colour >> 7) & 0xf8;
	g = (temp_colour >> 2) & 0xf8;
	b = (temp_colour << 3) & 0xff;

	return BR_COLOUR_ARGB(0,r,g,b);
}

void T_Write_RGB_555(char *pixels, br_float fract, br_colour new_pixel)
{
	/* colour format R G B R G B R G B - 2 bytes per pixel */
	br_uint_16 temp_colour;

	temp_colour = ((BR_RED(new_pixel) >> 3) << 10) |
			((BR_GRN(new_pixel) >> 3) << 5) |
			((BR_BLU(new_pixel) >> 3));

	*((br_uint_16 *)pixels) = temp_colour;
}

br_colour T_Read_RGB_565(char *pixels, br_float fract)
{
	/* colour format R G B R G B R G B  - 2 bytes per pixel */
    
	br_colour colour;
	br_uint_16 temp_colour;
	br_uint_8 r,g,b;

	temp_colour = *((br_uint_16 *)pixels);

	r = (temp_colour >> 8) & 0xf8;
	g = (temp_colour >> 3) & 0xf8;
	b = (temp_colour << 3) & 0xf8;

	colour = BR_COLOUR_ARGB(0,r,g,b);
	
	return colour;    
}

void T_Write_RGB_565(char *pixels, br_float fract, br_colour new_pixel)
{
	/* colour format R G B R G B R G B - 2 bytes per pixel */
	br_uint_16 temp_colour;

	temp_colour = ((BR_RED(new_pixel) >> 3) << 11) |
			((BR_GRN(new_pixel) >> 2) << 5) |
			((BR_BLU(new_pixel) >> 3));

	*((br_uint_16 *)pixels) = temp_colour;
}

br_colour T_Read_BGR_555(char *pixels, br_float fract)
{
	/* colour format B G R B G R B G R  - 2 bytes per pixel */
    
	br_colour colour;
	br_uint_16 temp_colour;
	br_uint_8 r,g,b;

	temp_colour = *((br_uint_16 *)pixels);

	b = (temp_colour >> 7) & 0xf8;
	g = (temp_colour >> 2) & 0xf8;
	r = (temp_colour << 3) & 0xf8;

	colour = BR_COLOUR_ARGB(0,r,g,b);
	
	return colour;    
}

void T_Write_BGR_555(char *pixels, br_float fract, br_colour new_pixel)
{
	/* colour format B G R - 2 bytes per pixel */
	br_uint_16 temp_colour;

	temp_colour = ((BR_BLU(new_pixel) >> 3) << 10) |
			((BR_GRN(new_pixel) >> 3) << 5) |
			((BR_RED(new_pixel) >> 3));

	*((br_uint_16 *)pixels) = temp_colour;
}

br_colour T_Read_RGBA_4444(char *pixels, br_float fract)
{
	/* colour format R G B A R G B A R G B A - 2 bytes per pixel */
    
	br_colour colour;
	br_uint_16 temp_colour;
	br_uint_8 r,g,b,a;

	temp_colour = *((br_uint_16 *)pixels);

	r = (temp_colour >> 8) & 0xf0;
	g = (temp_colour >> 4) & 0xf0;
	b = (temp_colour) & 0xf0;
	a = (temp_colour << 4) & 0xf0;

	colour = BR_COLOUR_ARGB(a,r,g,b);
	
	return colour;    
}

br_colour T_Read_ARGB_4444(char *pixels, br_float fract)
{
	/* colour format R G B A R G B A R G B A - 2 bytes per pixel */
    
	br_colour colour;
	br_uint_16 temp_colour;
	br_uint_8 r,g,b,a;

	temp_colour = *((br_uint_16 *)pixels);

	r = (temp_colour >> 4) & 0xf0;
	g = (temp_colour) & 0xf0;
	b = (temp_colour << 4) & 0xf0;
	a = (temp_colour << 8) & 0xf0;

	colour = BR_COLOUR_ARGB(a,r,g,b);
	
	return colour;    
}


void T_Write_RGBA_4444(char *pixels, br_float fract, br_colour new_pixel)
{
	/* colour format R G B A R G B A R G B A- 2 bytes per pixel */
	br_uint_16 temp_colour;

	temp_colour = ((BR_RED(new_pixel) >> 4) << 12) |
			((BR_GRN(new_pixel) >> 4) << 8) |
			((BR_BLU(new_pixel) >> 4) << 4) |
			((BR_ALPHA(new_pixel) >> 4));

	*((br_uint_16 *)pixels) = temp_colour;
}

void T_Write_ARGB_4444(char *pixels, br_float fract, br_colour new_pixel)
{
	/* colour format R G B A R G B A R G B A- 2 bytes per pixel */
	br_uint_16 temp_colour;

	temp_colour = ((BR_ALPHA(new_pixel) >> 4) << 12) |
			((BR_RED(new_pixel) >> 4) << 8) |
			((BR_GRN(new_pixel) >> 4) << 4) |
			((BR_BLU(new_pixel) >> 4));

	*((br_uint_16 *)pixels) = temp_colour;
}





/*
 * convert all pixelmaps to new type
 */
br_uint_32 T_ConvertPixelmaps(t_pixelmap_cbfn_info *cbfn_command)
{
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

	cbfn_command->count = 0;

	switch(cbfn_command->type) {

		case T_CBFN_CONTROL_CONVERT:
			cbfn_command->type = T_CBFN_CONVERT_PIXELMAP;
			break;

		case T_CBFN_CONTROL_REGEN_PALETTES:
			cbfn_command->type = T_CBFN_CONVERT_PALETTES;
			break;
			
		case T_CBFN_CONTROL_REMAP:
			cbfn_command->type = T_CBFN_REMAP_PIXELMAP;
			break;

		case T_CBFN_CONTROL_QUANTIZE:
			cbfn_command->type = T_CBFN_QUANTIZE_PIXELMAP;
			break;
	}

	/*
	 * convert all pixelmaps in registry
	 */
	BrMapEnum(NULL, T_PixelmapCbfn, cbfn_command);

	/*
	 * clean up
	 */
	return cbfn_command->count;
}

/*
 * convert to a direct colour pixelmap
 */
br_pixelmap *T_ConvertDirectPixelmap(br_pixelmap *item, t_pixelmap_cbfn_info *cbfn_command)
{

	br_pixelmap *pm;
	br_uint_32 x, y;
	char *s_pixels, *d_pixels;
	br_float s_fract, d_fract;
	br_float s_x_step, d_x_step;
	br_float d_x_start, s_x_start;
	br_float d_y_step, s_y_step;
	br_float d_y_start;

	t_read_pixel_fn_type *read_pixel_fn;
	t_write_pixel_fn_type *write_pixel_fn;
	
	/*
	 * allocate new pixelmap
	 */
	if(cbfn_command->type == T_CBFN_FLIP_HIGH_WIDE_PALETTES)
		pm = BrPixelmapAllocate(cbfn_command->new_type, item->height, item->width, NULL, 0);
	else
		pm = BrPixelmapAllocate(cbfn_command->new_type, item->width, item->height, NULL, 0);

	pm->identifier = BrMemStrDup(item->identifier);
	
	/*
	 * convert to direct colour pixelmap
	 */
	d_y_start = pm->base_y * pm->row_bytes;
	 
	s_pixels = ((char *)item->pixels) + (item->base_y * item->row_bytes);
	d_pixels = ((char *)pm->pixels) + (int)d_y_start;

	s_x_step = ((br_float)BrPixelmapPixelSize(item))/8.0;
	d_x_step = ((br_float)BrPixelmapPixelSize(pm))/8.0;

	s_y_step = item->row_bytes;
	d_y_step = pm->row_bytes;

	s_x_start = item->base_x * s_x_step;
	d_x_start = pm->base_x * d_x_step;
	
	read_pixel_fn = PixelmapTypes[item->type].read_pixel_fn;
	write_pixel_fn = PixelmapTypes[pm->type].write_pixel_fn;

	if(cbfn_command->type == T_CBFN_FLIP_X_PIXELMAP) {
		d_x_start += ((pm->width - 1) * d_x_step);
		d_x_step = -d_x_step;

		read_pixel_fn = PixelmapTypes[item->type].read_flip_pixel_fn;
	}

	if(cbfn_command->type == T_CBFN_FLIP_Y_PIXELMAP) {

		d_pixels += ((pm->height - 1) * pm->row_bytes);
		d_y_step = -d_y_step;

		read_pixel_fn = PixelmapTypes[item->type].read_flip_pixel_fn;
	}

	current_src = item;
	current_dst = pm;

	if(cbfn_command->type == T_CBFN_FLIP_HIGH_WIDE_PALETTES) {
		for(y=0, s_fract = s_x_start, d_fract = d_x_start; y<item->height; y++, s_pixels += (int)s_y_step, d_pixels = ((char *)pm->pixels) + (int)d_y_start, d_fract += d_x_step, s_fract = s_x_start) {
			for(x=0; x<item->width; x++, s_fract += s_x_step, d_pixels += (int)d_y_step)  {
				write_pixel_fn(d_pixels + ((int)d_fract), d_fract - ((int)d_fract), 
					read_pixel_fn(s_pixels + ((int)s_fract), s_fract - ((int)s_fract)));
			}
		}
	} else {
		for(y=0, s_fract = s_x_start, d_fract = d_x_start; y<item->height; y++, s_pixels += (int)s_y_step, d_pixels += (int)d_y_step, s_fract = s_x_start, d_fract = d_x_start) {
			for(x=0; x<item->width; x++, s_fract += s_x_step, d_fract += d_x_step) {
				write_pixel_fn(d_pixels + ((int)d_fract), d_fract - ((int)d_fract),
					read_pixel_fn(s_pixels + ((int)s_fract), s_fract - ((int)s_fract)));
			}
		}
	}
	
	return pm;
}

/*
 * convert to an indexed pixelmap
 */
br_pixelmap *T_ConvertIndexedPixelmap(br_pixelmap *item, t_pixelmap_cbfn_info *cbfn_command)
{
	br_pixelmap *pm, *temp_pm, *temp_palette;
	char scratch_string[256];
	char *s_pixels, *d_pixels;
	br_uint_32 x, y;
	br_colour pixel;
	br_uint_8 index;
	char rgb[3];
	br_uint_8 threshold;
	br_int_32 temp_type;
	br_float s_fract, d_fract, s_step, d_step;

	pm = BrPixelmapAllocate(cbfn_command->new_type, item->width, item->height, NULL, 0);

	pm->identifier = BrMemStrDup(item->identifier);

	BrQuantBegin();

	if(cbfn_command->type == T_CBFN_REMAP_PIXELMAP) {

		/*
		 * perform some validation on control data
		 */
		if((cbfn_command->base < 0) || (cbfn_command->base > cbfn_command->height - 1))
			BR_ERROR3("Index base value out of range 0-%d (%d) for type %s", cbfn_command->height - 1, cbfn_command->base, T_EncodePixelmapType(pm->type));
		if((cbfn_command->range < 1) || (cbfn_command->range > cbfn_command->height))
			BR_ERROR2("Index range value too large (%d) for type %s", cbfn_command->height, T_EncodePixelmapType(pm->type));
		if((cbfn_command->base + cbfn_command->range - 1) > (cbfn_command->height - 1))
			BR_ERROR2("Too many colours specified for indexed pixelmap of type %s (%d)",
				T_EncodePixelmapType(pm->type), cbfn_command->base + cbfn_command->range - 1);
	}

	if(cbfn_command->type != T_CBFN_QUANTIZE_PIXELMAP)
		temp_palette = BrPixelmapAllocate(BR_PMT_RGBX_888, 1, cbfn_command->height, NULL, 0);
	else {
	    	/*
		 * get palette in correct format
		 */
		temp_type = cbfn_command->new_type;
		cbfn_command->new_type = BR_PMT_RGBX_888;
		temp_palette = T_ConvertDirectPixelmap(cbfn_command->current_palette, cbfn_command);
		cbfn_command->new_type = temp_type;	    
	}

	temp_type = cbfn_command->new_type;
	cbfn_command->new_type = BR_PMT_RGBA_8888;
	temp_pm = T_ConvertDirectPixelmap(item, cbfn_command);
	cbfn_command->new_type = temp_type;

	s_pixels = ((char *)temp_pm->pixels) + (temp_pm->base_y * temp_pm->row_bytes);
	s_step = BrPixelmapPixelSize(temp_pm)/8;

	/*
	 * add pixels to quantizer
	 */
	for(y=0, s_fract = (temp_pm->base_x * s_step); y<temp_pm->height; y++, s_pixels += temp_pm->row_bytes, s_fract = temp_pm->base_x * s_step)
		for(x=0; x<temp_pm->width; x++, s_fract += s_step) {
    
			pixel = PixelmapTypes[temp_pm->type].read_pixel_fn(s_pixels + ((int)s_fract), s_fract - ((int)s_fract));
	
			threshold = BR_ALPHA(pixel);

                        if(threshold <= alpha_threshold) {  

				rgb[0] = BR_RED(pixel);
				rgb[1] = BR_GRN(pixel);
				rgb[2] = BR_BLU(pixel);

				/*
				 * insert RGB pixel into quantizer
				 */
				BrQuantAddColours(rgb,1);
			}
    
		}

	/*
	 * generate new palette
	 */
	if(cbfn_command->type != T_CBFN_QUANTIZE_PIXELMAP)
		BrQuantMakePalette(cbfn_command->base, cbfn_command->range, temp_palette);

	/*
	 * map old image to palette
	 */
	BrQuantPrepareMapping(cbfn_command->base, cbfn_command->range, temp_palette);

	s_pixels = ((char *)temp_pm->pixels) + (temp_pm->base_y * temp_pm->row_bytes);
	d_pixels = ((char *)pm->pixels) + (pm->base_y * pm->row_bytes);
	s_step = BrPixelmapPixelSize(temp_pm)/8;
	d_step = ((br_float)BrPixelmapPixelSize(pm))/8.0;

	/*
	 * generate pixelmap of indicies
	 */
	for(y=0, s_fract = temp_pm->base_x * s_step, d_fract = pm->base_x * d_step; y<temp_pm->height; y++, s_pixels += temp_pm->row_bytes, d_pixels += pm->row_bytes, s_fract = temp_pm->base_x * s_step, d_fract = pm->base_x * d_step)
		for(x=0; x<temp_pm->width; x++, s_fract += s_step, d_fract += d_step) {

			pixel = PixelmapTypes[temp_pm->type].read_pixel_fn(s_pixels + ((int)s_fract), s_fract - ((int)s_fract));

			threshold = BR_ALPHA(pixel);

                        if(threshold <= alpha_threshold) { 

				rgb[0] = BR_RED(pixel);
				rgb[1] = BR_GRN(pixel);
				rgb[2] = BR_BLU(pixel);

				/*
				 * map RGB pixel to quantizer
				 */
				BrQuantMapColours(cbfn_command->base, (br_uint_8 *)rgb, &index, 1);
                                PixelmapTypes[pm->type].write_pixel_fn(d_pixels + ((int)d_fract), d_fract - ((int)d_fract), (br_colour)index);

                        } else  {
				pixel = 0;
                                PixelmapTypes[pm->type].write_pixel_fn(d_pixels + ((int)d_fract), d_fract - ((int)d_fract), (br_colour)0);
                        }
		}

	/*
	 * regenerate palette in correct format
	 */
	temp_type = cbfn_command->new_type;
	cbfn_command->new_type = palette_type;

	sprintf(scratch_string, "%spal", BrMemStrDup(item->identifier));

	temp_palette->identifier = BrMemStrDup(scratch_string);
	pm->map = T_ConvertDirectPixelmap(temp_palette, cbfn_command);
	cbfn_command->new_type = temp_type;

	/*
	 * mark identifier as palette
	 */
	strcpy(scratch_string, pm->identifier);
	strcat(scratch_string, " (palette)");

	pm->map->identifier = BrMemStrDup(scratch_string);

	/*
	 * clean up
	 */
	BrPixelmapFree(temp_pm);
	BrPixelmapFree(temp_palette);
	BrQuantEnd();

	return pm;	
}

