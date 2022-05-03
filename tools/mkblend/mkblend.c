/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: mkblend.c 1.1 1997/12/10 16:58:19 jon Exp $
 * $Locker: $
 *
 * $BC<"make -f mkblend.mak %s.obj;">
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

#define R 0
#define G 1
#define B 2

void SetBlend(char *table);
int IsPalette(br_pixelmap *pm);

/*
 * Options
 */
int DestBase = 0;
int DestRange = 256;


char *SrcPalName = NULL;
char *DestPalName = NULL;
char *OutputName = NULL;

/*
 * The two palettes
 */
br_pixelmap *SrcPalette,*DestPalette;

int main(int argc, char ** argv)
{
	char	*cp;
	br_pixelmap *blend_map;
	int i;

	BR_BANNER("MKBLEND","1994-1995","$Revision: 1.1 $");

	BrBegin();
	
	while (argv++, --argc) {
		if (**argv == '-') {
			/*
			 * Got one or more flags
			 */
			for (cp = *argv + 1; *cp; cp++) {

				if (strchr("odr", *cp)) {
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
"usage: mkblend <palette> [options]\n"
"\n"
"    Generates a shade table to shade indices from a source palette into\n"
"    a destination palette.\n"
"\n"
"        <palette>             Source palette\n"
"        -o <shade-table>      Output blend table pixmap\n"
"        [-d <dest-palette>]   Destination palette if different from source\n"
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
		
	if(!IsPalette(SrcPalette))
		if(SrcPalette->map && IsPalette(SrcPalette->map))
			SrcPalette = SrcPalette->map;
		else
			BR_ERROR1("'%s' does not contain a valid palette",SrcPalName);

	if(DestPalName == NULL)
	    	DestPalName = SrcPalName;

	DestPalette = BrPixelmapLoad(DestPalName);

	if(DestPalette == NULL)
		BR_ERROR1("Could not read '%s'",DestPalName);

	if(!IsPalette(DestPalette))
		if(DestPalette->map && IsPalette(DestPalette->map))
			DestPalette = DestPalette->map;
		else
			BR_ERROR1("'%s' does not contain a valid palette",DestPalName);
	    
	/*
	 * Allocate pixelmap for blend
	 */
 	blend_map = BrPixelmapAllocate(BR_PMT_INDEX_8,256,256,NULL,0);
	blend_map->identifier = "blend_table";

	/*
	 * Calculate shade table
	 */
	SetBlend(blend_map->pixels);

	/*
	 * Write out shade file
	 */
	BrPixelmapSave(OutputName,blend_map);

	BrEnd();

	return 0;
}

/*
 * See if a pixelmap is a valid palette file
 */
int IsPalette(br_pixelmap *pm)
{
	return (pm->type == BR_PMT_RGBX_888 &&
	        pm->width == 1 &&
			pm->height == PAL_SIZE);
}

/*
 */
static void colourOp(
	float *dest,
	float *a,
	float *b)
{
    dest[R] = a[R] * 0.5 + b[R] * 0.5;
    dest[G] = a[G] * 0.5 + b[G] * 0.5;
    dest[B] = a[B] * 0.5 + b[B] * 0.5;
}

/*
 * Build blend table
 */
void SetBlend(char *table)
{
	float a_c[3],b_c[3],dest_c[3];
	int a_i, b_i,d_i;	
	int near_c;
	float min_d,d,rd,bd,gd;
	br_colour *a_cp,*b_cp,*dest_cp;
	char *op = table;

	for(a_i=0, a_cp = SrcPalette->pixels; a_i < PAL_SIZE; a_i++, a_cp++) {

		a_c[R] = BR_RED(*a_cp)/255.0;
		a_c[G] = BR_GRN(*a_cp)/255.0;
		a_c[B] = BR_BLU(*a_cp)/255.0;

		for(b_i=0, b_cp = SrcPalette->pixels; b_i < PAL_SIZE; b_i++, b_cp++) {

			b_c[R] = BR_RED(*b_cp)/255.0;
			b_c[G] = BR_GRN(*b_cp)/255.0;
			b_c[B] = BR_BLU(*b_cp)/255.0;

			colourOp(dest_c,a_c,b_c);

			near_c = 127;
			min_d = INT_MAX;

			for(d_i=DestBase, dest_cp = DestPalette->pixels;
					d_i < (DestBase+DestRange);
					d_i++, dest_cp++) {

				rd = BR_RED(*dest_cp)/255.0 - dest_c[R];
				gd = BR_GRN(*dest_cp)/255.0 - dest_c[G];
				bd = BR_BLU(*dest_cp)/255.0 - dest_c[B];

				d = rd*rd + gd*gd + bd*bd;

				if (d < min_d) {
	 					min_d = d;
						near_c = d_i;
				}
			}

			*op = near_c;
			op++;
		}
	}
}

