/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: mkshades.c 1.1 1997/12/10 16:58:31 jon Exp $
 * $Locker: $
 *
 * Calculate shade tables for BRender
 *
 * Takes - a source texture palette (pixelmap with 1xn palette size)
 *         a destination screen palette (pixelmap with 1xn palette size)
 *			 (above two will likely be the same)
 *		   the number of shades (default 64)
 *
 * Produces -
 *		A shade table that can be loaded into brender (as a pixmap)
 *
 */
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>

#include "brender.h"

/*
 * Number of entries in a palette
 */
#define PAL_SIZE 256

static void setShades(int total, br_pixelmap *table);
static br_boolean isPalette(br_pixelmap *pm);

static void setEntry8 (br_pixelmap *pm, int x, int y, int r, int g, int b);
static void setEntry15(br_pixelmap *pm, int x, int y, int r, int g, int b);
static void setEntry16(br_pixelmap *pm, int x, int y, int r, int g, int b);
static void setEntry24(br_pixelmap *pm, int x, int y, int r, int g, int b);
static void setEntry32(br_pixelmap *pm, int x, int y, int r, int g, int b);

static void makeColour(int r, int g, int b, int *nr, int *ng, int *nb, float level);

/*
 * Options
 */
int NumShades = 64;
int DestBase = 0;
int DestRange = 256;
int OutputBits = 8;

char *SrcPalName = NULL;
char *DestPalName = NULL;
char *OutputName = NULL;

/*
 * The two palettes
 */
br_pixelmap *SrcPalette,*DestPalette;

/*
 * Pointer to function to set an entry in destination shade table
 */
static void (*setEntry)(br_pixelmap *pm, int x, int y, int r, int g, int b);

int main(int argc, char ** argv)
{
	char	*cp;
	br_pixelmap *shades_map;
	int i;

	BR_BANNER("MKSHADES","1994-1995","$Revision: 1.1 $");

	BrBegin();
	
	while (argv++, --argc) {
		if (**argv == '-') {
			/*
			 * Got one or more flags
			 */
			for (cp = *argv + 1; *cp; cp++) {

				if (strchr("odnrt", *cp)) {
					argv++;
					if(--argc == 0)
						BR_ERROR1("The %c option requires an argument", *cp);
				}

				switch (*cp) {

				default:
					BR_ERROR1("unknown option '%c'\n", *cp);
					break;

				/*
				 * Usage
				 */
				case '?':
					fprintf(stderr,
"usage: mkshades <palette> [options]\n"
"\n"
"    Generates a shade table to shade indices from a source palette into\n"
"    a destination palette.\n"
"\n"
"        <palette>             Source palette\n"
"        -o <shade-table>      Output shade table pixmap\n"
"        -t <bits>             Generate true-colour shade table of 15,16 or 32 bits\n"
"        [-d <dest-palette>]   Destination palette if different from source\n"
"        [-n <num_shades>]     Number of shades to generate (default = 64)\n"
"        [-r <base>,<size>]    Range in output palette (default = 0,256)\n");
					exit(0);

				/*
				 * Output filename
				 */
				case 'o':
					if(OutputName != NULL)
						BR_ERROR0("Output name specified twice");

					OutputName = *argv;
					break;

				/*
				 * Destination palette
				 */
				case 'd':
					if(DestPalName != NULL)
						BR_ERROR0("Destination palette name specified twice");

					DestPalName = *argv;
					break;

				/*
				 * Number of shades
				 */
				case 'n':
					NumShades = atoi(*argv);
					break;

				/*
				 * Number of bits in output shade table
				 */
				case 't':
					OutputBits = atoi(*argv);
					break;

				/*
				 * Range in destination palette
				 */
				case 'r':
					if(sscanf(*argv,"%d,%d",&DestBase,&DestRange) != 2)
						BR_ERROR0("Invalid range");

					break;
				}
			}
		} else {
			/*
			 * Source palette name
			 */
			if(SrcPalName != NULL)
				BR_ERROR0("Source palette name specified twice");

			SrcPalName = *argv;
		}
	}
#if 0
	printf("SrcPalName  = %s\n",SrcPalName?SrcPalName:"<NULL>");
	printf("DestPalName = %s\n",DestPalName?DestPalName:"<NULL>");
	printf("OutputName  = %s\n",OutputName?OutputName:"<NULL>");
	printf("NumShades   = %d\n",NumShades);
	printf("DestBase    = %d\n",DestBase);
	printf("DestRange   = %d\n",DestRange);
#endif

	/*
	 * Read in palette files
	 */
	if(SrcPalName == NULL)
		BR_ERROR0("No source palette given");

	SrcPalette = BrPixelmapLoad(SrcPalName);
	if(SrcPalette == NULL)
		BR_ERROR1("Could not read '%s'",SrcPalName);
		
	if(!isPalette(SrcPalette))
		if(SrcPalette->map && isPalette(SrcPalette->map))
			SrcPalette = SrcPalette->map;
		else
			BR_ERROR1("'%s' does not contain a valid palette",SrcPalName);

	if(DestPalName == NULL)
	    	DestPalName = SrcPalName;

	DestPalette = BrPixelmapLoad(DestPalName);

	if(DestPalette == NULL)
		BR_ERROR1("Could not read '%s'",DestPalName);

	if(!isPalette(DestPalette))
		if(DestPalette->map && isPalette(DestPalette->map))
			DestPalette = DestPalette->map;
		else
			BR_ERROR1("'%s' does not contain a valid palette",DestPalName);
	    
	/*
	 * Allocate pixelmap for shades
	 */
	switch(OutputBits) {
	case 8:
	 	shades_map = BrPixelmapAllocate(BR_PMT_INDEX_8,PAL_SIZE,NumShades,NULL,0);
		setEntry = setEntry8;
		break;

	case 15:
	 	shades_map = BrPixelmapAllocate(BR_PMT_RGB_555,PAL_SIZE,NumShades,NULL,0);
		setEntry = setEntry15;
		break;

	case 16:
	 	shades_map = BrPixelmapAllocate(BR_PMT_RGB_565,PAL_SIZE,NumShades,NULL,0);
		setEntry = setEntry16;
		break;

	case 24:
	 	shades_map = BrPixelmapAllocate(BR_PMT_RGB_888,PAL_SIZE,NumShades,NULL,0);
		setEntry = setEntry24;
		break;

	case 32:
	 	shades_map = BrPixelmapAllocate(BR_PMT_RGBX_888,PAL_SIZE,NumShades,NULL,0);
		setEntry = setEntry32;
		break;

	default:
		BR_ERROR0("invalid number of bits for output shade table");
	}

	shades_map->identifier = "shade_table";

	/*
	 * Calculate shade table
	 */
	SetShades(NumShades, shades_map);

	/*
	 * Write out shade file
	 */
	BrPixelmapSave(OutputName,shades_map);

	BrEnd();

	return 0;
}

/*
 * Build shading table for a given number of shades
 */
void SetShades(int total, br_pixelmap *table)
{
	int c,n,c3,n3,i,near_c,min_d,d;
	int r,g,b,cr,cg,cb,rd,gd,bd,j;
	float f;

	br_colour *cp,*dp;

	for(c=0, cp = SrcPalette->pixels; c< PAL_SIZE; c++, cp++) {

		/*
		 * Fetch source colour
		 */
		cr = BR_RED(*cp);
		cg = BR_GRN(*cp);
		cb = BR_BLU(*cp);

		/*
		 * For each shade level...
		 */
 		for(i=0; i< total; i++) {

			/*
			 * Light source colour ->RGB
			 */
			makeColour(cr,cg,cb,&r,&g,&b,i?(float)i/(float)total:0.0);
			setEntry(table,c,i,r,g,b);
		}
	}
}

/*
 * See if a pixelmap is a valid palette file
 */
static br_boolean isPalette(br_pixelmap *pm)
{
	return (pm->type == BR_PMT_RGBX_888 &&
	        pm->width == 1 &&
			pm->height == PAL_SIZE);
}

/*
 * Light an RGB value to a given intensity
 *
 */
static void makeColour(int r, int g, int b, int *nr, int *ng, int *nb, float level)
{
	if(level < 0.75) {
		level /= 0.75;

		*nr = level * r;
		*ng = level * g;
		*nb = level * b;

	} else {
		level = (level-0.75)/0.25;

		*nr = (1.0 - level) * r + level * 255;
		*ng = (1.0 - level) * g + level * 255;
		*nb = (1.0 - level) * b + level * 255;
	}
}

static void setEntry8(br_pixelmap *pm, int x, int y, int r, int g, int b)
{
	int n,near_c,min_d,d;
	int rd,gd,bd;
	br_colour *dp;

	/*
	 * Look for nearest colour in destination palette
	 */
	near_c = 127;
	min_d = INT_MAX;

	for(n=DestBase, dp = DestPalette->pixels; n < (DestBase+DestRange); n++) {

		rd = BR_RED(dp[n]) - r;
		gd = BR_GRN(dp[n]) - g;
		bd = BR_BLU(dp[n]) - b;

		d = rd*rd + gd*gd + bd*bd;

		if (d < min_d) {
				min_d = d;
				near_c = n;
		}
	}

	BrPixelmapPixelSet(pm,x,y,near_c);
}

#define COLOUR_RGB555(r,g,b) \
    (((b) >> 3) & 0x1f | ((g) << 2) & 0x3e0 | ((r) << 7) & 0x7c00)

#define COLOUR_RGB565(r,g,b) \
    (((b) >> 3) & 0x1f | ((g) << 3) & 0x7c0 | ((r) << 8) & 0xf800)

static void setEntry15(br_pixelmap *pm, int x, int y, int r, int g, int b)
{
	BrPixelmapPixelSet(pm,x,y,COLOUR_RGB555(r,g,b));
}

static void setEntry16(br_pixelmap *pm, int x, int y, int r, int g, int b)
{
	BrPixelmapPixelSet(pm,x,y,COLOUR_RGB565(r,g,b));
}

static void setEntry32(br_pixelmap *pm, int x, int y, int r, int g, int b)
{
	BrPixelmapPixelSet(pm,x,y,BR_COLOUR_RGB(r,g,b));
}

static void setEntry24(br_pixelmap *pm, int x, int y, int r, int g, int b)
{
	BrPixelmapPixelSet(pm,x,y,BR_COLOUR_RGB(r,g,b));
}

