/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: mkranges.c 1.1 1997/12/10 16:58:24 jon Exp $
 * $Locker: $
 *
 * Calculates a ramped palette
 *
 * Produces -
 *	A pixelmap containting the palette (1,256 RGBX_888)
 *
 * $BC<"make -f mkranges.mak %s.obj;">
 */
#include <stdio.h>
#include <ctype.h>
#include <math.h>

#include "brender.h"

/*
 * Number of entries in a palette
 */
#define PAL_SIZE 256

static br_colour MakeColour(br_colour a, br_colour d, br_colour s,float cut, float level);

int main(int argc, char ** argv)
{
	char line_buffer[256],*output_name,*input_name,*cp;
	br_pixelmap *palette;
	int base,range;
	int ar,ag,ab;
	int dr,dg,db;
	int sr,sg,sb;
	float cut;
	int n,line_number=0;
	FILE *infile;

	BR_BANNER("MKRANGES","1994-1995","$Revision: 1.1 $");

	if(argc != 3) {
		fprintf(stderr,"Usage: mkranges <input-script> <output-pixmap>\n");
		return 0;
	}
	input_name = argv[1];
	output_name = argv[2];

	BrBegin();

	/*
	 * Try to open the script file for input
	 */
	infile = fopen(input_name,"rt");

	if(infile == NULL)
		BR_ERROR1("Could not open \"%s\" for input\n",input_name);
	
	/*
	 * Allocate the output palette
	 */
	palette = BrPixelmapAllocate(BR_PMT_RGBX_888,1,PAL_SIZE,NULL,0);

	/*
	 * Process a line at a time from the input file
	 */
	line_number = 0;
	while(fgets(line_buffer, BR_ASIZE(line_buffer), infile)) {
		line_number++;

		/*
		 * Skip coments and blank lines
		 */
		cp = line_buffer;
		while(isspace(*cp))
			cp++;
			
		if(*cp == '#' || *cp == '\0')
			continue;

		n = sscanf(line_buffer,
		/*  Range Ambient  Diffuse  Specular Cut */
			"%d,%d %d,%d,%d %d,%d,%d %d,%d,%d %f",
			&base,&range,&ar,&ag,&ab,&dr,&dg,&db,&sr,&sg,&sb,&cut);

		if(n != 12)
			BR_ERROR1("Not enough parameters on line %d",line_number);

		if(base < 0 || base >= PAL_SIZE)
			BR_ERROR1("Base is invalid on line %d",line_number);

		if((base+range) < 0 || (base+range) > PAL_SIZE)
			BR_ERROR1("Range is invalid on line %d",line_number);

		/*
		 * Make the range
		 */
		cp = palette->pixels;
		cp += base * palette->row_bytes;
		for(n = 0 ; n < range; n++, cp+=palette->row_bytes)
				*(br_colour *)cp =
				MakeColour(BR_COLOUR_RGB(ar,ag,ab),
						   BR_COLOUR_RGB(dr,dg,db),
						   BR_COLOUR_RGB(sr,sg,sb),
						   cut,
						   n/(float)range);
	}

	/*
	 * Save the output palette
	 */
	palette->identifier = "Palette";
	BrPixelmapSave(output_name,palette);

	BrEnd();
	
	return 0;
}

/*
 * Construct a colour at ome point along a ramp
 *
 * level =      0                    cut        1.0
 * colour =     ambient -----------> diffuse -> specular 
 */
static br_colour MakeColour(br_colour a, br_colour d, br_colour s,float cut, float level)
{
	int r,g,b;

	if(level < cut) {
		level /= cut;

		return BR_COLOUR_RGB(
			(1.0-level) * BR_RED(a) + level * BR_RED(d),
			(1.0-level) * BR_GRN(a) + level * BR_GRN(d),
			(1.0-level) * BR_BLU(a) + level * BR_BLU(d));
	} else {
		level = (level-cut)/(1.0 - cut);

		return BR_COLOUR_RGB(
			(1.0-level) * BR_RED(d) + level * BR_RED(s),
			(1.0-level) * BR_GRN(d) + level * BR_GRN(s),
			(1.0-level) * BR_BLU(d) + level * BR_BLU(s));
	}
}
