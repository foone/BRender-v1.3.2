/*
 * Copyright (c) 1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: savetga.c 1.1 1997/12/10 16:59:11 jon Exp $
 * $Locker: $
 *
 */
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "brender.h"
#include "texconv.h"

/*
 * TGA header, this format allows for structure packing by compiler
 */
#pragma pack(1)

static struct {
    	char identsize;			/* number of bytes between header and whatever next 	*/
	char colourmaptype;		/* type of colourmap in file 0=RGB, 1=palette		*/
    	char imagetype;			/* type of storage of image				*/
    	unsigned short colourmapstart;	/* first colour index used in palette			*/
	unsigned short colourmaplength;	/* number of colours in palette from colourmapstart	*/
	char colourmapbits;		/* size of entry in palette table			*/
    	unsigned short xstart;		/* offset between uppr lft of image & uppr lft of screen*/
	unsigned short ystart;
	unsigned short width;		/* width in pixels					*/
	unsigned short depth;		/* depth in pixels					*/
	char bits;			/* number of bits of colour: 1,8,16,24,32		*/
	char descriptor;			/* bit 5 : set, image stored starting with last line	*/
					/* bit 4 : set, image stored pixels right to left	*/
					/* bits 0-3: number of bits available for overlays (n/a)*/
} TGAheader;

enum {
    	BR_TGA_REVERSED_WIDTH		=	0x10,	/* right to left 	*/
	BR_TGA_REVERSED_DEPTH		=	0x20,	/* bottom to top	*/
};

enum {
    	BR_TGA_UNCOMPRESSED_PALETTE	=	0x01,
	BR_TGA_UNCOMPRESSED_RGB		=	0x02,
	BR_TGA_UNCOMPRESSED_MONO	=	0x03,	/* not supported here	*/
	BR_TGA_COMPRESSED_PALETTE	=	0x09,
	BR_TGA_COMPRESSED_RGB		=	0x0a,
	BR_TGA_COMPRESSED_MONO		=	0x0b,	/* not supported here	*/
};

static unsigned short RGB_16;
static struct {
    	char blue,green,red;
} palette_entry;

#pragma pack()


char *T_SaveTarga(br_pixelmap *pm, char *target)
{
    	/*
	 * save pixelmap as uncompressed targa file
	 */
	char _scratch_string[32];
	char *filename;
	br_colour pixel;
	br_pixelmap *temp_pm;
	t_pixelmap_cbfn_info *cbfn_command;
	int set_new_palette = 0;
	
	int i,bytes;
	FILE *fh;

	cbfn_command = BrResAllocate(res_anchor, sizeof(*cbfn_command), BR_MEMORY_APPLICATION);

	switch(pm->type) {
	    	case BR_PMT_INDEX_8: TGAheader.colourmaptype = 1;
				     TGAheader.imagetype = BR_TGA_UNCOMPRESSED_PALETTE;
				     TGAheader.colourmapstart = 0;
				     TGAheader.colourmaplength = 256;
				     TGAheader.colourmapbits = 24;
				     TGAheader.bits = 8;
				     break;
				     
		case BR_PMT_RGB_555: TGAheader.colourmaptype = 0;
		 		     TGAheader.imagetype = BR_TGA_UNCOMPRESSED_RGB;
				     TGAheader.bits = 15;
				     TGAheader.identsize = 0;
				     break;
				     
		case BR_PMT_RGB_565: TGAheader.colourmaptype = 0;
		 		     TGAheader.imagetype = BR_TGA_UNCOMPRESSED_RGB;
				     TGAheader.bits = 16;
				     TGAheader.identsize = 0;
				     break;
				     
		case BR_PMT_RGB_888: TGAheader.colourmaptype = 0;
		 		     TGAheader.imagetype = BR_TGA_UNCOMPRESSED_RGB;
				     TGAheader.bits = 24;
				     TGAheader.identsize = 0;
				     break;
				     
		case BR_PMT_RGBX_888: TGAheader.colourmaptype = 0;
		 		     TGAheader.imagetype = BR_TGA_UNCOMPRESSED_RGB;
				     TGAheader.bits = 32;
				     TGAheader.identsize = 0;
				     break;
				     
		case BR_PMT_RGBA_8888: TGAheader.colourmaptype = 0;
		 		     TGAheader.imagetype = BR_TGA_UNCOMPRESSED_RGB;
				     TGAheader.bits = 32;
				     TGAheader.identsize = 0;
				     break;

		default:
			return NULL;
	}

	bytes = (TGAheader.bits + 1) >> 3;
	
	TGAheader.xstart = 0;
	TGAheader.ystart = 0;
	TGAheader.width = pm->width;
	TGAheader.depth = pm->height;

	TGAheader.descriptor = 0;
	TGAheader.descriptor = 0;

	if((target == NULL) && (pm->identifier == NULL))
		BR_ERROR1("Unable to save pixelmap (%s) as Targa with no identifier", T_EncodePixelmapType(pm->type));

	if(target == NULL) {

		filename = BrMemAllocate(20, BR_MEMORY_APPLICATION);
		strncpy(_scratch_string, pm->identifier, 8);
		_scratch_string[8] = 0;
		sprintf(filename, "%s.tga", _scratch_string);
	} else
		filename = target;

	fh = BrFileOpenWrite(filename,BR_FS_MODE_BINARY);
	/*
	 * save TGAheader
	 */
	BrFileWrite(&TGAheader,sizeof(TGAheader),1,fh);

	/*
	 * need to save palette?
	 */
	if(PixelmapTypes[pm->type].needs_palette) {

		br_pixelmap *palette;	    
	    
		if(pm->map == NULL)
			BR_ERROR2("Cannot save '%s' (%s) as targa file without palette",pm->identifier, T_EncodePixelmapType(pm->type));

		/*
		 * convert palette to n,1 BR_PMT_RGBX_888
		 */
		if(pm->map->type != BR_PMT_RGBX_888) {

			cbfn_command->new_type = BR_PMT_RGBX_888;
			cbfn_command->type = T_CBFN_CONVERT_PIXELMAP;

			palette = T_Convert(pm->map, cbfn_command);

			set_new_palette = 1;
		} else
			palette = pm->map;

		/*
		 * flip palette if necessary
		 */
		if(palette->width < palette->height) {

				cbfn_command->new_type = palette->type;
				cbfn_command->type = T_CBFN_FLIP_HIGH_WIDE_PALETTES;
				
				temp_pm = T_Convert(palette, cbfn_command);

				BrPixelmapFree(palette);

				palette = temp_pm;

				set_new_palette = 1;
		}
			
		for(i=0; i<256; i++) {
					    
			palette_entry.red = BR_RED(((br_colour *)(palette->pixels))[i]);
			palette_entry.green = BR_GRN(((br_colour *)(palette->pixels))[i]);
			palette_entry.blue = BR_BLU(((br_colour *)(palette->pixels))[i]);

			BrFileWrite(&palette_entry, sizeof(palette_entry), 1, fh);
		}

		if(set_new_palette)
			BrPixelmapFree(palette);
	}	

	/*
	 * save pixels
	 */
	for(i=pm->height-1; i>=0 ; i--)
		BrFileWrite(((char *)(pm->pixels))+(i*pm->row_bytes),pm->width*bytes,1,fh);

	BrFileClose(fh);

	BrResFree(cbfn_command);
	
	return filename;
}
