/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: mkfog.c 1.1 1997/12/10 16:58:22 jon Exp $
 * $Locker: $
 *
 * Calculate fog tables for BRender
 *
 * Takes - a source texture palette (pixelmap with 1xn palette size)
 *         a destination screen palette (pixelmap with 1xn palette size)
 *			 (above two will likely be the same)
 *                 the number of shades (default 256)
 *         hither and yon values for the camera to be used with
 *         camera-space values for minimum and maximum fog settings
 *         a fog colour
 *
 * Produces -
 *              A fog table that can be loaded into brender (as a pixmap)
 *
 * $BC<"make -f mkfog.mak %s.obj;">
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

void SetShades(int total, char *table);
int IsPalette(br_pixelmap *pm);

/*
 * Options
 */
int NumShades = 256;
int DestBase = 0;
int DestRange = 256;
float Hither = 0.1;
float Yon = 100.0;
float MinFog = -1.0;
float MaxFog = -1.0;
int FogR = 128, FogG = 128, FogB = 128;

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
	br_pixelmap *shades_map;
	int i;

    BR_BANNER("MKFOG","1996","$Revision: 1.1 $");

	BrBegin();
	
	while (argv++, --argc) {
		if (**argv == '-') {
			/*
			 * Got one or more flags
			 */
			for (cp = *argv + 1; *cp; cp++) {

                if (strchr("odnrcfs", *cp)) {
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
                    fprintf(stdout,
"usage: mkfog <palette> [options]\n"
"\n"
"    Generates a fog table to shade indices from a source palette into\n"
"    a destination palette.\n"
"\n"
"        <palette>             Source palette\n"
"        -o <fog-table>        Output fog table pixmap\n"
"        [-d <dest-palette>]   Destination palette if different from source\n"
"        [-n <num_shades>]     Number of shades to generate (default = 256)\n"
"        [-r <base>,<size>]    Range in output palette (default = 0,256)\n"
"        [-c <hither>,<yon>]   Camera settings (default = 0.1,100)\n"
"        [-f <min>,<max>]      Fog range (default = hither,yon)\n"
"        [-s <r>,<g>,<b>]      Fog shade (default = 128,128,128)\n");
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
				 * Range in destination palette
				 */
				case 'r':
					if(sscanf(*argv,"%d,%d",&DestBase,&DestRange) != 2)
						BR_ERROR0("Invalid range");
					break;
                                /*
                                 * Camera settings
                                 */
                                case 'c':
                                        if(sscanf(*argv,"%g,%g",&Hither,&Yon) != 2)
                                                BR_ERROR0("Invalid camera settings");
                                        if(Hither <= 0.0 || Yon <= Hither)
                                                BR_ERROR0("Invalid camera settings");
					break;
                                /*
                                 * Fog setings
                                 */
                                case 'f':
                                        if(sscanf(*argv,"%g,%g",&MinFog,&MaxFog) != 2){
                                                BR_ERROR0("Invalid fog settings");
                                        }
                                        if(MinFog <= 0.0 || MaxFog <= MinFog)
                                                BR_ERROR0("Invalid fog settings");
					break;
                                /*
                                 * Fog shade
                                 */
                                case 's':
                                        if(sscanf(*argv,"%d,%d,%d",&FogR,&FogG,&FogB) != 3){
                                                BR_ERROR0("Invalid fog settings");
                                        }
                                        if(FogR < 0   || FogG < 0   || FogB < 0  ||
                                           FogR > 255 || FogG > 255 || FogB > 255)
                                                BR_ERROR0("Invalid fog settings");
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

        if(MinFog == -1)
                MinFog = Hither;
        if(MaxFog == -1)
                MaxFog = Yon;

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
	 * Allocate pixelmap for shades
	 */
 	shades_map = BrPixelmapAllocate(BR_PMT_INDEX_8,256,NumShades,NULL,0);
	shades_map->identifier = "shade_table";

	/*
	 * Calculate shade table
	 */
        SetShades(NumShades, shades_map->pixels);

	/*
	 * Write out shade file
	 */
	BrPixelmapSave(OutputName,shades_map);

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
 * Fog an RGB value to a given intensity
 *
 */
void MakeColour(int r, int g, int b, int fr, int fg, int fb, int *nr, int *ng, int *nb, float level)
{
        if(level > 1) level = 1;
        if(level < 0) level = 0;

        *nr = level*fr + (1.0-level)*r;
        *ng = level*fg + (1.0-level)*g;
        *nb = level*fb + (1.0-level)*b;
}

/*
 * Build fogging table for a given number of shades
 */
void SetShades(int total, char *table)
{
	int c,n,c3,n3,i,near_c,min_d,d;
	int r,g,b,cr,cg,cb,rd,gd,bd,j;
	char *op;
        br_actor *camera;
        br_scalar viewz;

	br_colour *cp,*dp;

        camera = BrActorAllocate(BR_ACTOR_CAMERA, NULL);
        ((br_camera *)camera->type_data)->hither_z = BrFloatToScalar(Hither);
        ((br_camera *)camera->type_data)->yon_z = BrFloatToScalar(Yon);

        for(c=0, cp = SrcPalette->pixels; c< PAL_SIZE; c++, cp++) {

		/*
		 * Fetch source colour
		 */
		cr = BR_RED(*cp);
		cg = BR_GRN(*cp);
		cb = BR_BLU(*cp);

		op = table++;

		/*
		 * For each shade level...
		 */
 		for(i=0; i< total; i++) {

                        viewz = BrScreenZToCamera(camera,
                          BR_SCALAR(((float)i/(float)total)*65536.0-32768.0));

			/*
			 * Light source colour ->RGB
			 */
                        MakeColour(cr,cg,cb,FogR,FogG,FogB,&r,&g,&b,
                                   (BrScalarToFloat(viewz)+MinFog)/
                                        (MinFog-MaxFog));

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
			*op = near_c;
			op += PAL_SIZE;
		}
	}
}
