/*
 * Copyright (c) 1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: loadpix.c 1.1 1997/12/10 16:58:53 jon Exp $
 * $Locker: $
 *
 */
#include <stddef.h>
#include <stdio.h>

#include "brender.h"
#include "dosio.h"
#include "fmt.h"

#include "texconv.h"

#define T_MAX_PIXELMAPS	8192		/* maximum number of pixelmaps in a pixelmap (.pix) file */


/*
 * Current import type
 */
int InputType = TYPE_GUESS;

Input_Types InputFileTypes[] = {
    	{"image",	T_LoadPixelmap},
	{"palette",	T_LoadPixelmap},
	{"pixelmap",	T_LoadPixelmap},
    	{"pix",		T_LoadPixelmap},
	{"pal",		T_LoadPixelmap},

	{"bmp",		BrFmtBMPLoad},
	{"rle",		BrFmtBMPLoad},
	{"dib",		BrFmtBMPLoad},
	{"gif",		BrFmtGIFLoad},
	{"tga",		BrFmtTGALoad},
	{"iff",		BrFmtIFFLoad},
	{"lbm",		BrFmtIFFLoad},

        {"tab",         T_LoadPixelmap},
};

int InputFileTypesSize = BR_ASIZE(InputFileTypes);
	
static br_pixelmap * BR_PUBLIC_ENTRY T_LoadPixelmap(char *filename,br_uint_32 type)
{
	br_pixelmap **ppm, *pm;
	int npix, i;

   	/*
	 * Load pixelmap(s)
	 */
	ppm = BrResAllocate(res_anchor, sizeof(*ppm) * T_MAX_PIXELMAPS, BR_MEMORY_APPLICATION);

	if((npix = BrPixelmapLoadMany(filename, ppm, T_MAX_PIXELMAPS)) == 0)
		BR_ERROR1("Could not load pixelmap: '%s'",filename);

	if(npix > 1) {
		BrMapAddMany(ppm + 1, npix - 1);

		for(i=0; i<(npix - 1); i++) {
		    
		    	/*
			 * print details of a pixelmap (and palette)
			 */
			T_Log("Loaded '%s' as %s (%d,%d)\n",
				ppm[i+1]->identifier,PixelmapTypes[ppm[i+1]->type].identifier,
				ppm[i+1]->width,
				ppm[i+1]->height);
				
			if(ppm[i+1]->map!=NULL)
				T_Log("    Palette '%s' %s (%d, %d)\n",
					ppm[i+1]->map->identifier,
					PixelmapTypes[ppm[i+1]->map->type].identifier,
					ppm[i+1]->map->width,
					ppm[i+1]->map->height);
		}
	}

	pm = ppm[0];

	BrResFree(ppm);

	return pm;
}

br_pixelmap *T_LoadRawPixelmap(char *filename, t_pixelmap_cbfn_info *cbfn_command)
{
	void *fh;
	int open_mode = BR_FS_MODE_BINARY;
	br_pixelmap *pm;
	int x,y,i;
	int pixel_size;
	char pixel[4];

	if((fh = BrFileOpenRead(filename, 0, NULL, &open_mode)) == NULL)
		BR_ERROR1("Unable to open '%s'", filename);

	BrFileAdvance(cbfn_command->offset, fh);

	pm = BrPixelmapAllocate(cbfn_command->new_type, cbfn_command->width, cbfn_command->height, NULL, 0);

	pm->identifier = BrMemStrDup(filename);

	pixel_size = BrPixelmapPixelSize(pm) >> 3;

	for(y=0; y<pm->height; y++)
		for(x=0; x<pm->width; x++) {

			if(BrFileRead(pixel, 1, pixel_size, fh) != pixel_size)
				BR_ERROR4("Unable to load raw data '%s' as %s (%d,%d)",
					filename,
					T_EncodePixelmapType(pm->type),
					pm->width,
					pm->height);

			for(i=0; i<=pixel_size; i++)
				((char *)pm->pixels)[y*pm->row_bytes+x*pixel_size+(pixel_size-i)] = pixel[i-1];
		}

	BrFileClose(fh);

	return pm;
}

