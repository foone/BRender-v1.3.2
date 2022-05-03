/*
 * Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: vesaapi.h 1.1 1997/12/10 16:54:31 jon Exp $
 * $Locker: $
 *
 * VESA VBE 2.0 Structrures and API
 */
#ifndef _VESAAPI_H_
#define _VESAAPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)
/*
 * General VESA information
 */
struct vesa_info {
    br_uint_8 vbe_signature[4];
    br_uint_16 vbe_version;
	br_uint_8 * oem_string_ptr;
    br_uint_8 capabilities[4];
    br_uint_16 * videomode_ptr;
    br_uint_16 memory;
	br_uint_16 oem_software_rev;
	br_uint_8 * oem_vendor_name_ptr;
	br_uint_8 * oem_product_name_ptr;
	br_uint_8 * oem_product_rev_ptr;
    br_uint_8 reserved[222];
    br_uint_8 oem_data[256];
};

struct vesa_info_rm {
    br_uint_8 vbe_signature[4];
    br_uint_16 vbe_version;
    br_uint_16 oem_string_off;
    br_uint_16 oem_string_seg;
    br_uint_8 capabilities[4];
    br_uint_16 videomode_off;
    br_uint_16 videomode_seg;
    br_uint_16 memory;
	br_uint_16 oem_software_rev;
	br_uint_16 oem_vendor_name_off;
	br_uint_16 oem_vendor_name_seg;
	br_uint_16 oem_product_name_off;
	br_uint_16 oem_product_name_seg;
	br_uint_16 oem_product_rev_off;
	br_uint_16 oem_product_rev_seg;
    br_uint_8 reserved[222];
    br_uint_8 oem_data[256];
};

/*
 * Per mode VESA information
 */
struct vesa_modeinfo {
	/*
	 * All VBE versions
	 */
    br_uint_16 mode_attributes;
    br_uint_8 win_a_attributes;
    br_uint_8 win_b_attributes;
    br_uint_16 win_granularity;
    br_uint_16 win_size;
    br_uint_16 win_a_segment;
    br_uint_16 win_b_segment;
    br_uint_16 win_func_off;
    br_uint_16 win_func_seg;
    br_uint_16 bytes_per_scanline;
    br_uint_16 x_resolution;
    br_uint_16 y_resolution;
    br_uint_8 x_char_size;
    br_uint_8 y_char_size;
    br_uint_8 number_of_planes;
    br_uint_8 bits_per_pixel;
    br_uint_8 number_of_banks;
    br_uint_8 memory_model;
    br_uint_8 bank_size;
    br_uint_8 number_of_image_pages;
    br_uint_8 reserved1;

	/*
	 * VBE 1.2 and up
	 */
    br_uint_8 red_mask_size;
    br_uint_8 red_field_position;
    br_uint_8 green_mask_size;
    br_uint_8 green_field_position;
    br_uint_8 blue_mask_size;
    br_uint_8 blue_field_position;
    br_uint_8 rsvd_mask_size;
    br_uint_8 rsvd_field_position;
    br_uint_8 direct_color_mode_info;


	/*
	 * VBE 2.0 and up
	 */
	br_uint_32 phys_base_ptr;
	br_uint_32 offscreen_mem_offset;
	br_uint_16 offscreen_mem_size;

    br_uint_8 reserved2[206];
};

struct vesa_protected {
	br_uint_16	window_set;
	br_uint_16	display_start_set;
	br_uint_16	palette_set;
	br_uint_16	resources;
	br_uint_8	data[1];
};


#pragma pack()

enum vesa_mode_attribiutes {
	VESA_MA_SUPPORTED		= 0x0001,
	VESA_MA_TTY_SUPPORTED	= 0x0004,
	VESA_MA_COLOR			= 0x0008,
	VESA_MA_GRAPHICS		= 0x0010,
	VESA_MA_VGA_COMPATIBLE	= 0x0020,
	VESA_MA_WINDOWED		= 0x0040,
	VESA_MA_LINEAR			= 0x0080,
};

enum vesa_window_attribiutes {
	VESA_WA_RELOCATABLE		= 0x0001,
	VESA_WA_READABLE		= 0x0002,
	VESA_WA_WRITEABLE		= 0x0004,
};

enum vesa_memory_modes {
	VESA_MM_TEXT,
	VESA_MM_CGA,
	VESA_MM_HERCULES,
	VESA_MM_PLANAR,
	VESA_MM_PACKED_PIXEL,
	VESA_MM_NON_CHAIN4_256,
	VESA_MM_DIRECT,
	VESA_MM_YUV,
};

enum vesa_direct_color_modeinfo {
	VESA_DMCI_RAMP_PROGRAMMABLE = 0x0001,
	VESA_DMCI_RSVD_USEABLE		= 0x0002,
};

/*
 * Additional bits in mode work
 */
#define	VESA_MODE_LINEAR			0x4000
#define	VESA_MODE_NO_CLEAR			0x8000

br_error VESAInfo(struct vesa_info *vip);
br_error VESAModeInfo(struct vesa_modeinfo *vmip, br_uint_32 mode);

br_error VESAModeSet(br_uint_32 mode);
br_error VESAModeGet(br_uint_16 *mode);

br_error VESAScanlineBytesSet(br_uint_32 width, br_uint_16 * bytes, br_uint_16 * pixels, br_uint_16 * scanlines);
br_error VESAScanlinePixelsSet(br_uint_32 width, br_uint_16 * bytes, br_uint_16 * pixels, br_uint_16 * scanlines);
br_error VESAScanlineLengthGet(br_uint_16 * bytes, br_uint_16 * pixels, br_uint_16 * scanlines);

br_error VESADisplayStartSet(br_uint_32 x, br_uint_32 y);
br_error VESADisplayStartGet(br_uint_32 *px, br_uint_32 *py);

br_error VESAWindowSet(br_uint_32 window, br_uint_32 position);
br_error VESAWindowGet(br_uint_32 window, br_uint_32 *position);

br_error VESAPaletteFormatSet(br_uint_32 format);
br_error VESAPaletteFormatGet(br_uint_32 *formatp);

br_error VESAPaletteSet(br_int_32 *values, br_int_32 first, br_int_32 count, br_boolean blank);
br_error VESAPaletteGet(br_int_32 *values, br_int_32 first, br_int_32 count);

br_error VESAProtectedModeInterface(br_uint_16 *poffset, br_uint_16 *pseg, br_uint_16 *psize);

#ifdef __cplusplus
};
#endif
#endif



