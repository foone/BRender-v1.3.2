/*
 * Copyright (c) 1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: convert.h 1.2 1998/07/14 13:13:40 johng Exp $
 * $Locker: $
 *
 */
#ifndef _CONVERT_H_
#define _CONVERT_H_

br_colour T_Read_Alpha_8(char *pixels, br_float fract);
br_colour T_Read_IndexA_88(char *pixels, br_float fract);
br_colour T_Read_Index_8(char *pixels, br_float fract);
br_colour T_Read_Index_4(char *pixels, br_float fract);
br_colour T_Read_RGB_888(char *pixels, br_float fract);
br_colour T_Read_RGBX_888(char *pixels, br_float fract);
br_colour T_Read_RGB_555(char *pixels, br_float fract);
br_colour T_Read_RGB_565(char *pixels, br_float fract);
br_colour T_Read_BGR_555(char *pixels, br_float fract);
br_colour T_Read_RGBA_4444(char *pixels, br_float fract);
br_colour T_Read_ARGB_4444(char *pixels, br_float fract);

void T_Write_Alpha_8(char *pixels, br_float fract, br_colour new_pixel);
void T_Write_Index_8(char *pixels, br_float fract, br_colour new_pixel);
void T_Write_Index_4(char *pixels, br_float fract, br_colour new_pixel);
void T_Write_RGB_888(char *pixels, br_float fract, br_colour new_pixel);
void T_Write_RGBX_888(char *pixels, br_float fract, br_colour new_pixel);
void T_Write_RGB_555(char *pixels, br_float fract, br_colour new_pixel);
void T_Write_RGB_565(char *pixels, br_float fract, br_colour new_pixel);
void T_Write_BGR_555(char *pixels, br_float fract, br_colour new_pixel);
void T_Write_ARGB_4444(char *pixels, br_float fract, br_colour new_pixel);
void T_Write_RGBA_4444(char *pixels, br_float fract, br_colour new_pixel);

br_colour T_Read_Flip_IndexA_88(char *pixels, br_float fract);
br_colour T_Read_Flip_Index_4(char *pixels, br_float fract);
br_colour T_Read_Flip_Index_8(char *pixels, br_float fract);

br_uint_32 T_ConvertPixelmaps(t_pixelmap_cbfn_info *cbfn_command);
br_pixelmap *T_ConvertDirectPixelmap(br_pixelmap *item, t_pixelmap_cbfn_info *cbfn_command);
br_pixelmap *T_ConvertIndexedPixelmap(br_pixelmap *item, t_pixelmap_cbfn_info *cbfn_command);


#endif

