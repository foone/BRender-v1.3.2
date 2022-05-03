/*
 * Copyright (c) 1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: texconv.c 1.1 1997/12/10 16:59:13 jon Exp $
 * $Locker: $
 *
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "brender.h"
#include "dosio.h"
#include "fmt.h"

#include "quantize.h"
#include "palquant.h"
#include "texconv.h"
#include "mip.h"

br_pixelmap *screen_buffer;
char *res_anchor;
br_boolean LEAVE_TRANSPARENT = BR_FALSE;

/*
 * current 32 bit pixelmap type
 */
br_uint_32 alpha_channel = BR_PMT_RGBX_888;
br_uint_32 palette_type = BR_PMT_RGBX_888;
br_uint_8 alpha_threshold = 127;

int main(int argc, char **argv)
{
	t_list *command_list;
	t_cla *current_command;
	t_cla *next_command;

	char *cp;
	int i, n_commands;

        BR_BANNER("TEXCONV","1997","$Revision: 1.1 $");

	/*
	 * Setup renderer and screen
	 */
	BrBegin();

	/*
	 * setup resource anchor
	 */
	res_anchor = BrResAllocate(NULL, sizeof(*res_anchor), BR_MEMORY_APPLICATION);

	command_list = BrResAllocate(res_anchor, sizeof(*command_list), BR_MEMORY_APPLICATION);
	T_NewList(command_list);

	/*
	 * Process command line to expand @files
	 */
	n_commands = T_DecodeCLA(argc - 1, argv + 1, command_list);

#if 0
	T_Log("Commands : %d\n\n", n_commands);
#endif  

	/*
	 * Process argument list in order
	 */
	while(command_list->head != NULL)
	{

		current_command = (t_cla *)T_RemoveHead(command_list);
		
		if(current_command->command[0] == '-')
		{

			/*
			 * got one or more flags
			 */
			for(cp = current_command->command + 1; *cp; cp++)
			{

				// Handle options which have an optional argument. We shall decide that
				// if the optional argument does not exist, we shall set current command 
				// to NULL before continuing.
                if(strchr("s", *cp)) // Alan's square pixelmap routine. Add other
									 // characters here if you want to use optional arguments.
				{
					// If the following fails it returns NULL in current command. Ideal as
					// we then assume no argument, as we reached the end of the command line,
					// or spotted another flag straight after our flag
					if (NULL != command_list->head  && '-' != ((t_cla *)command_list->head)->command[0])
					{
						current_command = (t_cla *)T_RemoveHead(command_list);
					}
					else
					{
						current_command = NULL;
					}
				}

				// Handle options which _require_ an argument
                if(strchr("CIMOPQRScmnoprw", *cp))
				{
	
					if(command_list->head == NULL)
						BR_ERROR1("The %c option requires an argument", *cp);

					current_command = (t_cla *)T_RemoveHead(command_list);
				}

				switch(*cp) {

					default:
						BR_ERROR1("Unknown option '%c'", *cp);

					/*
					 * usage
					 */
					case '?': {

						T_Log(  "\n"
							"Usage: texconv {options}\n"
							"\n"
							"Options are treated as commands executed in left-to-right order -\n"
							"\n"
							"   <input-file>             Load a file into current data\n"
							"   -I <input-type>          Set input file type\n"
							"   -O <output-type>         Set output file type\n"
							"\n"
							"   -a                       Toggle current 32 bit pixelmaps to exclude/include\n"
							"                            alpha data (default exclude)\n"
							"   -c <pixelmap-type>[,t]   Convert to pixelmap type, may involve quantizing.\n"
							"                            t is the alpha channel threshold (0-255) for\n"
							"                            conversions involving 32 bit pixelmaps.\n"
							"   -f                       'Forget' all current data\n"
							"   -m <b,r>                 Mix (combine) all loaded palettes and quantize\n"
							"                            to the (b)ase, and (r)ange.\n"
							"   -w <b,r,weight,smap1>[,smap2, ...]\n"
							"                            Mix (combine) all loaded palettes and quantize\n"
							"                            to the (b)ase and (r)ange. For the arbitrary"
							"                            number of super pixelmaps (smap1, smap2 etc.)\n"
							"                            load them if necessary and use the specified\n"
							"                            weight in the quantizer. A high weight (100)\n"
							"                            will ensure that all the colours in the super-\n"
							"                            maps appear in the resulting palette which is\n"
							"                            applied to all textures. If a supermap is already\n"
							"                            in Texconv it will also be converted to the new\n"
							"                            palette, otherwise it will only be used to \n"
							"                            reserve colours.\n"
							"   -n <name>                Assign identifier 'name' to data\n"
							"   -o <file>                Generate output file from current data\n"
							"   -p <pixelmap-type>       Convert all palettes from indexed pixelmaps to\n"
							"                            new type. Default palette type BR_PMT_RGBX_888\n"
							"   -r <file>,<pixelmap-type>,offset,x,y\n"
							"                            Load raw data file as pixelmap-type,\n"
							"                            with pixel dimensions x,y, from offset into file\n"
							"   -s [<float>]             Scales the data to a square. The smallest power of\n"
							"                            two {not smaller than the longest side of the\n"
							"                            original} is chosen for the size of the scaled\n"
							"                            pixelmap edges. The float value is an optional\n"
							"                            filter size like the -S command uses\n"
							"   -v [<pixelmap-type>]     View snapshot of current data as pixelmap-type\n"
							"                            default BR_PMT_RGB_888\n"
							"   -x                       Flip left/right\n"
							"   -y                       Flip top/bottom\n"
							"\n"
							"   -C <x,y>                 Reset pixelmap origin to x,y\n"
							"   -F                       Toggle high/wide palettes. Default is high. Only\n"
							"                            PlayStation currently requires wide palettes\n"
							"   -M <b,r>                 Generate all mip levels to 1x1 from current\n"
							"                            size (powers of 2).For indexed modes base=b range=r\n"
							"   -N                       Generate all mip levels to 1x1 from current\n"
							"                            size (powers of 2).\n"
							"   -P <name>[,RAW]          Apply palette from a file to all current indexed\n"
							"                            pixelmaps. If RAW is specified, the palette file\n"
							"                            is 768 byte rgb, otherwise pixelmap format\n"
							"   -Q <name>[,b,r]          Quantize to palette supplied (pixelmap format)\n"
							"                            using (b)ase and (r)ange palette entries\n"
							"   -R b,r                   Quantize and remap to (b)ase,(r)ange colours.\n"
							"                            (0 <= b,r <256)\n"
							);
	
						T_Log(
							"   -S x,y[,<float>]         Scale to new x,y dimensions using optional filter\n"
							"                            size float (default 3.0)\n"
							"   -X                       Exclude transparent pixels from remapping or\n"
							"                            quantizing operations.\n"
							"   @file                    perform all operation contained within <file>\n"
							"                            Comments marked with ; or #\n"
							"   <input-type> =\n"
							"       pix    - BRender Pixelmap format\n"
							"       bmp    - Windows BMP format   (4,8,24,RLE4,RLE8)\n"
							"       gif    - CompuServ GIF format (1< x <=8 bit)\n"
							"       tga    - Targa TGA format     (8,15,16,24,32 bit)\n"
							"       iff    - Amiga IFF/LBM format (1< x <=8 bit)\n"
							"\n"
							"   If a type is not specified, it will be guessed from the extension\n"
							"   of the filename.\n");
							
						T_Log(  "\n"
							"   <output-type> =\n"
							"       palette         Palette information stripped from bitmap (.pix)\n"
							"       image           Pixelmap without palette (.pix)\n"
							"       pixelmap        Image with any palette information (.pix)\n"
							"       targa           Output as .tga file\n"
							"\n"
							"   If a type is not specified, the default is pixelmap\n");
						T_Log(  "\n"
							"   <pixelmap-type> =\n");

						for(i=0; i<PixelmapTypesSize; i++) {
							T_Log("       %s   \t(%s)\n",
								PixelmapTypes[i].identifier,
								PixelmapTypes[i].description);
						}

						break;
					}

					case 'a': {
						br_pixelmap **mtable;
						int nmaps,i;
						
						if (alpha_channel == BR_PMT_RGBX_888)
						     alpha_channel = BR_PMT_RGBA_8888;
						else alpha_channel = BR_PMT_RGBX_888;

						nmaps = CurrentPixelmaps(&mtable);

						for(i=0; i<nmaps; i++) {
							if(mtable[i]->type ==
								       (alpha_channel == BR_PMT_RGBX_888)?
									BR_PMT_RGBA_8888:BR_PMT_RGBX_888)
							{
								mtable[i]->type = alpha_channel;                                                 
								T_Log("'%s' type changed to %s\n",
								mtable[i]->identifier,
								PixelmapTypes[mtable[i]->type].identifier);
							}
							else if (mtable[i]->type == alpha_channel)
								T_Log("'%s' type already %s\n",
								   mtable[i]->identifier,
								   PixelmapTypes[mtable[i]->type].identifier);
						}

						BrMemFree(mtable);
						
						break;
					}

					/*
					 * -I <input-type>, set input file type
					 */
					case 'I': {
						int i;

						for(i=0; i < InputFileTypesSize ; i++)
							if(!stricmp(current_command->command,InputFileTypes[i].name))
								break;

						if(i >= InputFileTypesSize)
							BR_ERROR1("Unknown input type \"%s\"",current_command->command);
	
						InputType = i;
					}
					break;

					/*
					 * -O <output-type>, set output file type
					 */
					case 'O': {
						int i;
	
						for(i=0; i < OutputFileTypesSize; i++)
							if(!stricmp(current_command->command,OutputFileTypes[i].name))
								break;
	
						if(i >= OutputFileTypesSize)
							BR_ERROR1("Unknown output type \"%s\"",current_command->command);
	
						OutputType = i;
					}
					break;

					/*
					 * -o <output_file>
					 */
					case 'o':{
						int i,ot = OutputType;
						char *cp;

						if(ot == TYPE_GUESS && (cp = (char *)strrchr(current_command->command,'.'))) {
							/*
							 * Guess import type based on extension
							 */
							for(i=0; i < OutputFileTypesSize; i++)
								if(!stricmp(OutputFileTypes[i].name,cp+1))
									ot = i;
						}
						/*
						 * Complain if, by this point, we don't know how to process file
						 */
						if(ot == TYPE_GUESS)
							BR_ERROR0("Unknown output file format");

						T_OutputPixelmap(current_command->command, OutputFileTypes[ot].type);
					}
					break;

					/*
					 * -f   forget all current data
					 */
					case 'f': {
						t_pixelmap_cbfn_info *cbfn_command;
					    
						alpha_channel = BR_PMT_RGBX_888;
						palette_type = BR_PMT_RGBX_888;
						alpha_threshold = 127;
						LEAVE_TRANSPARENT = BR_FALSE;

						InputType = TYPE_GUESS;
						OutputType = TYPE_GUESS;

						cbfn_command = BrResAllocate(res_anchor, sizeof(*cbfn_command), BR_MEMORY_APPLICATION);

						T_GenerateMapTable(cbfn_command);

						for(i=0; i<cbfn_command->nmaps; i++) {

							BrMapRemove(cbfn_command->map_table[i]);
							BrPixelmapFree(cbfn_command->map_table[i]);
						}

						T_Log("Current registry data removed (%d pixelmap%s",
							cbfn_command->nmaps, (cbfn_command->nmaps==1)?")\n":"s)\n");

						BrResFree(cbfn_command);
					}
					break;
					
					/*
					 * -N <name>   set pixelmap identifiers to 'name'
					 */
					case 'n':
					{
						t_pixelmap_cbfn_info *cbfn_command;

						cbfn_command = BrResAllocate(res_anchor, sizeof(*cbfn_command), BR_MEMORY_APPLICATION);

						T_GenerateMapTable(cbfn_command);

						/*
						 * this will lead to a memory leak on the old pixelmap identifier
						 */
						for(i=0; i<cbfn_command->nmaps; i++) {

							T_Log("'%s' assigned new identifier '%s'\n",
								cbfn_command->map_table[i]->identifier,
								current_command->command);

							cbfn_command->map_table[i]->identifier = BrMemStrDup(current_command->command);
						}

						BrResFree(cbfn_command);
					}
					break;

					/*
					 * -c <pixelmap-type>[,threshold]
					 * convert pixelmap from one type to th'other, may involve quantize
					 * with alpha threshold
					 */
					case 'c':{
						t_pixelmap_cbfn_info *cbfn_command;
						t_list *arg_list;
						t_cla *arg;
						br_int_32 new_type, threshold;

						arg_list = BrResAllocate(res_anchor, sizeof(*arg_list), BR_MEMORY_APPLICATION);
						T_NewList(arg_list);

						cbfn_command = BrResAllocate(res_anchor, sizeof(*cbfn_command), BR_MEMORY_APPLICATION);

						/* interpret commands */
						T_DecodeCommand(current_command->command, arg_list);

						arg = (t_cla *)T_RemoveHead(arg_list);

						new_type = T_DecodePixelmapType(arg->command);

						if(arg_list->head != NULL) {

							arg = (t_cla *)T_RemoveHead(arg_list);
							threshold = arg->ivalue;
						} else
							threshold = alpha_threshold;

						/*
						 * allocate pixelmap table & new convert table
						 */
						T_GenerateMapTable(cbfn_command);

						cbfn_command->convert_table = BrResAllocate(cbfn_command,
							sizeof(*cbfn_command->convert_table) * cbfn_command->nmaps,
							BR_MEMORY_APPLICATION);

						cbfn_command->new_type = new_type;
						cbfn_command->threshold = threshold;
						cbfn_command->type = T_CBFN_CONTROL_CONVERT;

						/*
						 * convert pixelmaps
						 */
						T_ConvertPixelmaps(cbfn_command);

						/*
						 * replace all pixelmaps in registry with new versions
						 */
						for(i=0; i<cbfn_command->nmaps; i++)
							if(cbfn_command->map_table != NULL) {
								BrMapRemove(cbfn_command->map_table[i]);
								BrPixelmapFree(cbfn_command->map_table[i]);
							} else
								BrMapRemove(cbfn_command->convert_table[i]);

						BrMapAddMany(cbfn_command->convert_table, cbfn_command->nmaps);
						
						BrResFree(cbfn_command);
						BrResFree(arg_list);
					}
					break;

					// -X exclude transparent pixels
					//    from remapping / quantising
					case 'X': {
					    LEAVE_TRANSPARENT = BR_TRUE;
					    T_Log ("Transparent Pixels (colour 0) will not be remapped.\n");
					}
					break;

					/*
					 * -R b,r , remap to base,range.
					 * may invlove quantizing.
					 */
					case 'R': {
						t_pixelmap_cbfn_info *cbfn_command;
						t_list *arg_list;
						t_cla *arg;

						arg_list = BrResAllocate(res_anchor, sizeof(*arg_list), BR_MEMORY_APPLICATION);
						T_NewList(arg_list);

						cbfn_command = BrResAllocate(res_anchor, sizeof(*cbfn_command), BR_MEMORY_APPLICATION);

						/* interpret commands */
						if(T_DecodeCommand(current_command->command, arg_list) != 2)
							BR_ERROR0("Incorrect number of parameters for -R");

						arg = (t_cla *)T_RemoveHead(arg_list);

						cbfn_command->base = arg->ivalue;

						arg = (t_cla *)T_RemoveHead(arg_list);
						
						cbfn_command->range = arg->ivalue;

						/*
						 * allocate pixelmap table & new convert table
						 */
						T_GenerateMapTable(cbfn_command);

						cbfn_command->convert_table = BrResAllocate(cbfn_command,
							sizeof(*cbfn_command->convert_table) * cbfn_command->nmaps,
							BR_MEMORY_APPLICATION);

						cbfn_command->type = T_CBFN_CONTROL_REMAP;
						cbfn_command->threshold = alpha_threshold;

						/*
						 * remap pixelmaps to same type
						 */
						T_ConvertPixelmaps(cbfn_command);

						/*
						 * replace all pixelmaps in registry with new versions
						 */
						for(i=0; i<cbfn_command->nmaps; i++)
							if(cbfn_command->map_table != NULL) {
							    
								BrMapRemove(cbfn_command->map_table[i]);
								BrPixelmapFree(cbfn_command->map_table[i]);
							} else
								BrMapRemove(cbfn_command->convert_table[i]);

						BrMapAddMany(cbfn_command->convert_table, cbfn_command->nmaps);
						
						BrResFree(cbfn_command);
						BrResFree(arg_list);
					}
					break;
					
					/*
					 * -Q <name>[,b,r], quantize to base,range in palette
					 */
					case 'Q': {
						t_pixelmap_cbfn_info *cbfn_command;
						t_list *arg_list;
						t_cla *arg;
						br_int_32 n;

						arg_list = BrResAllocate(res_anchor, sizeof(*arg_list), BR_MEMORY_APPLICATION);
						T_NewList(arg_list);

						cbfn_command = BrResAllocate(res_anchor, sizeof(*cbfn_command), BR_MEMORY_APPLICATION);

						/* interpret commands */
						n = T_DecodeCommand(current_command->command, arg_list);

						arg = (t_cla *)T_RemoveHead(arg_list);

						/*
						 * Load new palette
						 */
						if((cbfn_command->current_palette = BrPixelmapLoad(arg->command)) == NULL)
							BR_ERROR1("Unable to load palette '%s' for quantizing", arg->command);

						switch(--n) {
							case 2:
								arg = (t_cla *)T_RemoveHead(arg_list);
								cbfn_command->base = arg->ivalue;
								arg = (t_cla *)T_RemoveHead(arg_list);
								cbfn_command->range = arg->ivalue;
								break;
							case 1:
								arg = (t_cla *)T_RemoveHead(arg_list);
								cbfn_command->base = arg->ivalue;
								break;
							case 0:
								cbfn_command->base = 0;
								cbfn_command->range = 256;
								break;
						}

						/*
						 * allocate pixelmap table & new convert table
						 */
						T_GenerateMapTable(cbfn_command);

						cbfn_command->convert_table = BrResAllocate(cbfn_command,
							sizeof(*cbfn_command->convert_table) * cbfn_command->nmaps,
							BR_MEMORY_APPLICATION);

						cbfn_command->type = T_CBFN_CONTROL_QUANTIZE;
						cbfn_command->threshold = alpha_threshold;

						/*
						 * remap pixelmaps to same type
						 */
						T_ConvertPixelmaps(cbfn_command);

						/*
						 * replace all pixelmaps in registry with new versions
						 */
						for(i=0; i<cbfn_command->nmaps; i++)
							if(cbfn_command->map_table != NULL) {
								BrMapRemove(cbfn_command->map_table[i]);
								BrPixelmapFree(cbfn_command->map_table[i]);
							} else
								BrMapRemove(cbfn_command->convert_table[i]);

						BrMapAddMany(cbfn_command->convert_table, cbfn_command->nmaps);
						
						BrResFree(cbfn_command);
						BrResFree(arg_list);
						
					}
					break;

					/*
					 * -P <name>[,RAW], set indexed maps to have specified [raw] palette
					 */
					case 'P': {
						t_pixelmap_cbfn_info *cbfn_command;
						t_list *arg_list;
						t_cla *arg;
						char *filename;
						void *fh;
						int open_mode = BR_FS_MODE_BINARY;
						br_uint_32 y;
						br_colour *palette_entry;
						br_uint_8 pixel[3];

						arg_list = BrResAllocate(res_anchor, sizeof(*arg_list), BR_MEMORY_APPLICATION);
						T_NewList(arg_list);

						cbfn_command = BrResAllocate(res_anchor, sizeof(*cbfn_command), BR_MEMORY_APPLICATION);

						/* interpret commands */
						T_DecodeCommand(current_command->command, arg_list);

						arg = (t_cla *)T_RemoveHead(arg_list);

						filename = arg->command;

						if(arg_list->head != NULL) {

							arg = (t_cla *)T_RemoveHead(arg_list);
						}

						if(stricmp(arg->command, "RAW") == 0) {
							/*
							 * raw 768 byte palette
							 */
							cbfn_command->current_palette = BrPixelmapAllocate(BR_PMT_RGBX_888, 1, 256, NULL, 0);
							cbfn_command->current_palette->identifier = BrMemStrDup(filename);
								
							if((fh = BrFileOpenRead(filename, 0 ,NULL, &open_mode)) == NULL)
								BR_ERROR1("Unable to open raw palette file '%s'",filename);

							for(y=0, palette_entry = cbfn_command->current_palette->pixels;
								y<cbfn_command->current_palette->height; y++, palette_entry++) {

								if(BrFileRead(pixel, 1, 3, fh) != 3)
									BR_ERROR1("Unable to read raw palette from '%s'", filename);

								*palette_entry = BR_COLOUR_RGBA(0,pixel[0], pixel[1], pixel[2]);
							}
							BrFileClose(fh);
						} else
							/*
							 * try to load palette
							 */
							if((cbfn_command->current_palette = BrPixelmapLoad(filename)) == NULL)
									BR_ERROR1("Unable to load palette '%s'", filename);

						/*
						 * allocate pixelmap table & new convert table
						 */
						T_GenerateMapTable(cbfn_command);

						cbfn_command->convert_table = BrResAllocate(cbfn_command,
							sizeof(*cbfn_command->convert_table) * cbfn_command->nmaps,
							BR_MEMORY_APPLICATION);

						cbfn_command->type = T_CBFN_ASSIGN_PALETTE;
						cbfn_command->count = 0;

						/*
						 * assign palette to all pixelmaps
						 */
						BrMapEnum(NULL, T_PixelmapCbfn, cbfn_command);

						/*
						 * replace all pixelmaps in registry with new versions
						 */
						for(i=0; i<cbfn_command->nmaps; i++)
							BrMapRemove(cbfn_command->convert_table[i]);

						BrMapAddMany(cbfn_command->convert_table, cbfn_command->nmaps);
						
						BrResFree(cbfn_command);
						BrResFree(arg_list);
					}
					break;

					/*
					 * -S x,y[,f]  scale to new x,y with filter size (float) f
					 */
					case 'S': {
						t_pixelmap_cbfn_info *cbfn_command;
						t_list *arg_list;
						t_cla *arg;
						br_int_32 n;

						arg_list = BrResAllocate(res_anchor, sizeof(*arg_list), BR_MEMORY_APPLICATION);
						T_NewList(arg_list);

						cbfn_command = BrResAllocate(res_anchor, sizeof(*cbfn_command), BR_MEMORY_APPLICATION);

						/* interpret commands */
						if((n = T_DecodeCommand(current_command->command, arg_list)) < 2)
							BR_ERROR0("Incorrect number of parameters for -S");

						arg = (t_cla *)T_RemoveHead(arg_list);

						cbfn_command->scale_x = arg->ivalue;

						arg = (t_cla *)T_RemoveHead(arg_list);

						cbfn_command->scale_y = arg->ivalue;

						if(n == 3) {

							arg = (t_cla *)T_RemoveHead(arg_list);
													    
							cbfn_command->fwidth = arg->fvalue;
						} else
							cbfn_command->fwidth = 3.0;

						if((cbfn_command->scale_x < 0) || (cbfn_command->scale_y < 0))
							BR_ERROR0("Cannot scale to negative size");

						/*
						 * allocate pixelmap table & new convert table
						 */
						T_GenerateMapTable(cbfn_command);

						cbfn_command->convert_table = BrResAllocate(cbfn_command,
							sizeof(*cbfn_command->convert_table) * cbfn_command->nmaps,
							BR_MEMORY_APPLICATION);

						cbfn_command->type = T_CBFN_SCALE_PIXELMAP;
						cbfn_command->count = 0;

						BrMapEnum(NULL, T_PixelmapCbfn, cbfn_command);

						/*
						 * replace all pixelmaps in registry with new versions
						 */
						for(i=0; i<cbfn_command->nmaps; i++) {

							BrMapRemove(cbfn_command->map_table[i]);
							BrPixelmapFree(cbfn_command->map_table[i]);
						}

						BrMapAddMany(cbfn_command->convert_table, cbfn_command->nmaps);
						
						BrResFree(cbfn_command);
						BrResFree(arg_list);
					}
					break;


					/*
					 * -s [f]  scale to new size (s pixels by s) with filter size (float) f
					 */
					case 's': {
						t_pixelmap_cbfn_info *cbfn_command;
						t_list *arg_list;
						t_cla *arg;
						br_int_32 n;

						arg_list = BrResAllocate(res_anchor, sizeof(*arg_list), BR_MEMORY_APPLICATION);
						T_NewList(arg_list);

						cbfn_command = BrResAllocate(res_anchor, sizeof(*cbfn_command), BR_MEMORY_APPLICATION);

						// If there are no command parameters...
						if (NULL == current_command)
						{
							// set the default
							cbfn_command->fwidth = 3.0;
						}
						else
						{
							// suss out what the parameter contains
							if((n = T_DecodeCommand(current_command->command, arg_list)) > 1)
								BR_ERROR0("Too many parameters for -s");

							arg = (t_cla *)T_RemoveHead(arg_list);
							// set the filter size.
							cbfn_command->fwidth = arg->fvalue;
						}

						/*
						 * allocate pixelmap table & new convert table
						 */
						T_GenerateMapTable(cbfn_command);

						cbfn_command->convert_table = BrResAllocate(cbfn_command,
							sizeof(*cbfn_command->convert_table) * cbfn_command->nmaps,
							BR_MEMORY_APPLICATION);

						cbfn_command->type = T_CBFN_SQUARE_PIXELMAP;
						cbfn_command->count = 0;

						BrMapEnum(NULL, T_PixelmapCbfn, cbfn_command);

						/*
						 * replace all pixelmaps in registry with new versions
						 */
						for(i=0; i<cbfn_command->nmaps; i++) {

							BrMapRemove(cbfn_command->map_table[i]);
							BrPixelmapFree(cbfn_command->map_table[i]);
						}

						BrMapAddMany(cbfn_command->convert_table, cbfn_command->nmaps);
						
						BrResFree(cbfn_command);
						BrResFree(arg_list);
					}
/*
						t_pixelmap_cbfn_info *cbfn_command;
						t_list *arg_list;
						t_cla *arg;
						br_int_32 n;

						arg_list = BrResAllocate(res_anchor, sizeof(*arg_list), BR_MEMORY_APPLICATION);
						T_NewList(arg_list);
						
						cbfn_command = BrResAllocate(res_anchor, sizeof(*cbfn_command), BR_MEMORY_APPLICATION);
						n = T_DecodeCommand(current_command->command, arg_list);

						// interpret commands 
						arg = (t_cla *)T_RemoveHead(arg_list);
							cbfn_command->scale_x = cbfn_command->scale_y = arg->ivalue;


						if (2 <= n)
						{
							arg = (t_cla *)T_RemoveHead(arg_list);
							cbfn_command->fwidth = arg->fvalue;
						}
						else
						{
							cbfn_command->fwidth = 3.0;
						}

						if((cbfn_command->scale_x < 0) || (cbfn_command->scale_y < 0))
							BR_ERROR0("Cannot scale to negative size");

						// allocate pixelmap table & new convert table
printf ("[2] ");
						T_GenerateMapTable(cbfn_command);

						cbfn_command->convert_table = BrResAllocate(cbfn_command,
							sizeof(*cbfn_command->convert_table) * cbfn_command->nmaps,
							BR_MEMORY_APPLICATION);

						cbfn_command->type = T_CBFN_SQUARE_PIXELMAP;
						cbfn_command->count = 0;
printf ("[3] ");

						BrMapEnum(NULL, T_PixelmapCbfn, cbfn_command);

						// replace all pixelmaps in registry with new versions
printf ("[4] ");
						for(i=0; i<cbfn_command->nmaps; i++) {

							BrMapRemove(cbfn_command->map_table[i]);
							BrPixelmapFree(cbfn_command->map_table[i]);
						}

printf ("[5] ");
						BrMapAddMany(cbfn_command->convert_table, cbfn_command->nmaps);
						
printf ("[6] ");
						BrResFree(cbfn_command);
						BrResFree(arg_list);
					}
					*/
					break;

					/*
					 * -y flip top/bottom
					 */
					case 'y':{
						t_pixelmap_cbfn_info *cbfn_command;
					    
						cbfn_command = BrResAllocate(res_anchor, sizeof(*cbfn_command), BR_MEMORY_APPLICATION);

						T_GenerateMapTable(cbfn_command);

						cbfn_command->convert_table = BrResAllocate(cbfn_command,
							sizeof(*cbfn_command->convert_table) * cbfn_command->nmaps,
							BR_MEMORY_APPLICATION);


						cbfn_command->type = T_CBFN_FLIP_Y_PIXELMAP;
						cbfn_command->count = 0;

						BrMapEnum(NULL, T_PixelmapCbfn, cbfn_command);

						/*
						 * replace all pixelmaps in registry with new versions
						 */
						for(i=0; i<cbfn_command->nmaps; i++) {

							BrMapRemove(cbfn_command->map_table[i]);
							BrPixelmapFree(cbfn_command->map_table[i]);
						}
						BrMapAddMany(cbfn_command->convert_table, cbfn_command->nmaps);

						BrResFree(cbfn_command);
					}
					break;

					/*
					 * -x flip left/right
					 */
					case 'x':{
						t_pixelmap_cbfn_info *cbfn_command;
					    
						cbfn_command = BrResAllocate(res_anchor, sizeof(*cbfn_command), BR_MEMORY_APPLICATION);

						T_GenerateMapTable(cbfn_command);

						cbfn_command->convert_table = BrResAllocate(cbfn_command,
							sizeof(*cbfn_command->convert_table) * cbfn_command->nmaps,
							BR_MEMORY_APPLICATION);

						cbfn_command->type = T_CBFN_FLIP_X_PIXELMAP;
						cbfn_command->count = 0;

						BrMapEnum(NULL, T_PixelmapCbfn, cbfn_command);

						/*
						 * replace all pixelmaps in registry with new versions
						 */
						for(i=0; i<cbfn_command->nmaps; i++) {

							BrMapRemove(cbfn_command->map_table[i]);
							BrPixelmapFree(cbfn_command->map_table[i]);
						}
						BrMapAddMany(cbfn_command->convert_table, cbfn_command->nmaps);

						BrResFree(cbfn_command);
					}
					break;

					/*
					 * set origin
					 */
					case 'C': {
						t_pixelmap_cbfn_info *cbfn_command;
						t_list *arg_list;
						t_cla *arg;

						cbfn_command = BrResAllocate(res_anchor, sizeof(*cbfn_command), BR_MEMORY_APPLICATION);

						arg_list = BrResAllocate(res_anchor, sizeof(*arg_list), BR_MEMORY_APPLICATION);
						T_NewList(arg_list);

						cbfn_command = BrResAllocate(res_anchor, sizeof(*cbfn_command), BR_MEMORY_APPLICATION);

						/* interpret commands */
						if(T_DecodeCommand(current_command->command, arg_list) != 2)
							BR_ERROR0("Incorrect number of parameters for -C");

						arg = (t_cla *)T_RemoveHead(arg_list);

						cbfn_command->origin_x = arg->ivalue;

						arg = (t_cla *)T_RemoveHead(arg_list);

						cbfn_command->origin_y = arg->ivalue;

						T_GenerateMapTable(cbfn_command);

						cbfn_command->type = T_CBFN_SET_ORIGIN;
						cbfn_command->count = 0;

						BrMapEnum(NULL, T_PixelmapCbfn, cbfn_command);

						BrResFree(cbfn_command);
						BrResFree(arg_list);
					}
					break;

					/*
					 * flip high/wide palettes
					 */
					case 'F': {
						t_pixelmap_cbfn_info *cbfn_command;

						cbfn_command = BrResAllocate(res_anchor, sizeof(*cbfn_command), BR_MEMORY_APPLICATION);

						/*
						 * Generate a list of all indexed pixelmaps
						 */
						T_CountIndexedMaps(cbfn_command);

						cbfn_command->map_table = BrResAllocate(cbfn_command,
							sizeof(*cbfn_command->convert_table)*cbfn_command->nmaps, BR_MEMORY_APPLICATION);

						cbfn_command->convert_table = BrResAllocate(cbfn_command,
							sizeof(*cbfn_command->convert_table)*cbfn_command->nmaps, BR_MEMORY_APPLICATION);

						cbfn_command->type = T_CBFN_FLIP_HIGH_WIDE_PALETTES;
						cbfn_command->count = 0;

						BrMapEnum(NULL, T_PixelmapCbfn, cbfn_command);
						
						/*
						 * replace all pixelmaps in registry with new versions
						 */
						for(i=0; i<cbfn_command->count; i++) {

							BrPixelmapFree(cbfn_command->map_table[i]->map);

							/*
							 * assign new palette
							 */
							cbfn_command->map_table[i]->map = cbfn_command->convert_table[i];
						}

						BrResFree(cbfn_command);
					}
					break;


					/*
					 * mip mapping
					 */

					case 'N':
						generateMipLevels(0,256);
						break;
					
					case 'M':{

//Code that generates a combined Mip
						t_pixelmap_cbfn_info *cbfn_command;
						t_list *arg_list;
						t_cla *arg;
						br_int_32 n;

						arg_list = BrResAllocate(res_anchor, sizeof(*arg_list), BR_MEMORY_APPLICATION);
						T_NewList(arg_list);

						cbfn_command = BrResAllocate(res_anchor, sizeof(*cbfn_command), BR_MEMORY_APPLICATION);

						/* interpret commands */

						n = T_DecodeCommand(current_command->command, arg_list);
						
						/*
						 * Load new palette
						 */
						switch(n) {
							case 2:
								arg = (t_cla *)T_RemoveHead(arg_list);
								cbfn_command->base = arg->ivalue;
								arg = (t_cla *)T_RemoveHead(arg_list);
								cbfn_command->range = arg->ivalue;
								break;
							case 1:
								arg = (t_cla *)T_RemoveHead(arg_list);
								cbfn_command->base = arg->ivalue;
								cbfn_command->range = 256-cbfn_command->base;
								break;
							case 0:
								cbfn_command->base = 0;
								cbfn_command->range = 256;
								break;
						}
				
						generateMipLevels(cbfn_command->base,cbfn_command->range);
					}
					break;

#if 0 // Code that generates seperate maps.
						t_pixelmap_cbfn_info *cbfn_command;
						t_pixelmap_list_type *tpm;

						cbfn_command = BrResAllocate(res_anchor, sizeof(*cbfn_command), BR_MEMORY_APPLICATION);

						/*
						 * allocate pixelmap table & new pixelmap list
						 */
						T_GenerateMapTable(cbfn_command);

						cbfn_command->pixelmap_list = BrResAllocate(cbfn_command, sizeof(*cbfn_command->pixelmap_list), BR_MEMORY_APPLICATION);
						T_NewList(cbfn_command->pixelmap_list);

						cbfn_command->type = T_CBFN_GENERATE_MIP_LEVELS;
						cbfn_command->count = 0;

						BrMapEnum(NULL, T_PixelmapCbfn, cbfn_command);

						while(cbfn_command->pixelmap_list->head != NULL) {

							tpm = (t_pixelmap_list_type *)T_RemoveHead(cbfn_command->pixelmap_list);

							BrMapAdd(tpm->pixelmap);
						}

						BrResFree(cbfn_command);
					}
					break;
#endif

					/*
					 * -r <name>,<pixelmap-type>,offset,x,y  load raw file as pixelmap-type size x,y
					 */
					case 'r': {
						t_pixelmap_cbfn_info *cbfn_command;
						t_list *arg_list;
						t_cla *arg;
						char *filename;
						br_pixelmap *pm;

						cbfn_command = BrResAllocate(res_anchor, sizeof(*cbfn_command), BR_MEMORY_APPLICATION);

						arg_list = BrResAllocate(res_anchor, sizeof(*arg_list), BR_MEMORY_APPLICATION);
						T_NewList(arg_list);

						cbfn_command = BrResAllocate(res_anchor, sizeof(*cbfn_command), BR_MEMORY_APPLICATION);

						/* interpret commands */
						if(T_DecodeCommand(current_command->command, arg_list) != 5)
							BR_ERROR0("Incorrect number of parameters for -r");

						arg = (t_cla *)T_RemoveHead(arg_list);

						filename = arg->command;

						arg = (t_cla *)T_RemoveHead(arg_list);

						cbfn_command->new_type = T_DecodePixelmapType(arg->command);

						if(cbfn_command->new_type >= PixelmapTypesSize)
							BR_ERROR1("Unknown pixelmap type '%s'", arg->command);

						arg = (t_cla *)T_RemoveHead(arg_list);

						cbfn_command->offset = arg->ivalue;

						arg = (t_cla *)T_RemoveHead(arg_list);

						cbfn_command->width = arg->ivalue;

						arg = (t_cla *)T_RemoveHead(arg_list);

						cbfn_command->height = arg->ivalue;

						pm = T_LoadRawPixelmap(filename, cbfn_command);

						if(pm != NULL)
							BrMapAdd(pm);
						else
							BR_ERROR1("Unable to load '%s' as raw map", filename);
						
						BrResFree(cbfn_command);
						BrResFree(arg_list);
					}
					break;

					
					/*
					 * -v   view current entries in registry
					 */
					case 'v': {
						t_pixelmap_cbfn_info *cbfn_command;
					    
						cbfn_command = BrResAllocate(res_anchor, sizeof(*cbfn_command), BR_MEMORY_APPLICATION);

						T_GenerateMapTable(cbfn_command);

						cbfn_command->type = T_CBFN_VIEW_PIXELMAPS;

						BrMapEnum(NULL, T_PixelmapCbfn, cbfn_command);
	
						BrResFree(cbfn_command);
					}
					break;
					
					/*
					 * -p <pixelmap-type> convert all palettes to this type
					 */
					case 'p':{
						t_pixelmap_cbfn_info *cbfn_command;

						cbfn_command = BrResAllocate(res_anchor, sizeof(*cbfn_command), BR_MEMORY_APPLICATION);

						palette_type = T_DecodePixelmapType(current_command->command);

						/*
						 * Generate a list of all indexed pixelmaps
						 */
						T_CountIndexedMaps(cbfn_command);

						cbfn_command->map_table = BrResAllocate(cbfn_command,
							sizeof(*cbfn_command->convert_table)*cbfn_command->nmaps, BR_MEMORY_APPLICATION);

						cbfn_command->convert_table = BrResAllocate(cbfn_command,
							sizeof(*cbfn_command->convert_table)*cbfn_command->nmaps, BR_MEMORY_APPLICATION);

						cbfn_command->type = T_CBFN_CONTROL_REGEN_PALETTES;
						cbfn_command->threshold = alpha_threshold;
						cbfn_command->new_type = palette_type;

						/*
						 * convert palettes
						 */
						T_ConvertPixelmaps(cbfn_command);

						/*
						 * replace all pixelmaps in registry with new versions
						 */
						for(i=0; i<cbfn_command->nmaps; i++) {

							BrPixelmapFree(cbfn_command->map_table[i]->map);

							/*
							 * assign new palette
							 */
							cbfn_command->map_table[i]->map = cbfn_command->convert_table[i];
						}

						BrResFree(cbfn_command);
					}
					break;
					/*
					 * old -R b,r , remap to base,range.
					 * may invlove quantizing.
					 * from 1.6
					 */
					case 'm': {
						br_pixelmap *new_palette,**mtable,*new_pm;
						t_list *arg_list;
						t_cla *arg;
						int nmaps,i,base,range;

						arg_list = BrResAllocate(res_anchor, sizeof(*arg_list), BR_MEMORY_APPLICATION);
						T_NewList(arg_list);

						/* interpret commands */
						if (T_DecodeCommand(current_command->command, arg_list) != 2) 
                            BR_ERROR0("Incorrect number of parameters for -m");

						arg = (t_cla *)T_RemoveHead(arg_list);
						base = arg->ivalue;

						arg = (t_cla *)T_RemoveHead(arg_list);
						range = arg->ivalue;

						if ((base < 0 ) || (base >255))
							BR_ERROR0("Base value out of range 0-255");
						if ((range < 1) || (range >256))
							BR_ERROR0("Range value out of range 1-256");
						if (base + range - 1 > 255)
                            BR_ERROR0("Too many colours specified for -m");

						nmaps = CurrentPixelmaps(&mtable);

						DoRemap(mtable,nmaps,base,range);

						BrMemFree(mtable);

						BrResFree(arg_list);

						break;
					}


					// New remapper where you can weight some pixelmaps
					case 'w':
					{
						br_pixelmap *new_palette,**mtable, **smtable,*new_pm;
						t_list *arg_list;
						t_cla *arg;
						int nmaps, nsmaps = 0, i, base, range, weight, nparms;

						arg_list = BrResAllocate(res_anchor, sizeof(*arg_list), BR_MEMORY_APPLICATION);
						T_NewList(arg_list);

						/* interpret commands */
						nparms = T_DecodeCommand(current_command->command, arg_list);
#ifdef DEBUG
						printf ("[1] %s %d ", current_command->command, nparms);
#endif
						if (nparms < 4)
                            BR_ERROR0("Incorrect number of parameters for -w");

						arg = (t_cla *)T_RemoveHead(arg_list);
						base = arg->ivalue;

						arg = (t_cla *)T_RemoveHead(arg_list);
						range = arg->ivalue;

						arg = (t_cla *)T_RemoveHead(arg_list);
						weight = arg->ivalue;

						if ((base < 0 ) || (base >255))
							BR_ERROR0("Base value out of range 0-255");
						if ((range < 1) || (range >256))
							BR_ERROR0("Range value out of range 1-256");
						if (base + range - 1 > 255)
                            BR_ERROR0("Too many colours specified for -w");

						nmaps = CurrentPixelmaps(&mtable);

						// Setup a blank supermap table of maximum nparms supermaps
						smtable = BrMemCalloc(sizeof(*smtable),nparms,BR_MEMORY_APPLICATION);
						if (NULL == smtable)
						{
							BR_ERROR0("Calloc failed. (Probably out of memory)");
						}

						// Now grab the superpixelmaps from wherever they are.
						while (1)
						{
							// get the command line token
							arg = (t_cla *)T_RemoveHead(arg_list);
							if (NULL == arg)
							{
								// stop looking if there are no more arguments
								break;
							}

							// Check the registry for a file of the same name
							smtable[nsmaps] = BrMapFind (arg->command);
							if (NULL == smtable[nsmaps])
							{
								// Supermap not in registry. Must attempt to load it
								smtable[nsmaps] = BrPixelmapLoad (arg->command);

								// Test for success
								if (NULL == smtable[nsmaps])
								{
									// Give up
									BR_ERROR0("Unable to load supermap");
								}
							}
							// One more supermap in list
							nsmaps++;
						}

						DoRemapWithSupermaps (mtable, nmaps, smtable, nsmaps, weight, base, range);

						BrMemFree(mtable);

						BrResFree(arg_list);

						break;
					}

				}
			}
		}
		else
		{
			int i,it = InputType;
			char *cp, *input_name = current_command->command;
			br_pixelmap *pm;

			/**
			 ** process input file
			 **/

			if(it == TYPE_GUESS && (cp = (char *)strrchr(input_name, '.'))) {
				/*
				 * Guess import type based on extension
				 */
				for(i=0; i < InputFileTypesSize; i++)
				if(!stricmp(InputFileTypes[i].name,cp+1))
					it = i;
			}

			/*
			 * Complain if, by this point, we don't know how to process file
			 */
			if(it == TYPE_GUESS)
				BR_ERROR0("Unknown input file format");

			pm = InputFileTypes[it].function(input_name, alpha_channel);

			if(pm != NULL)
				BrMapAdd(pm);

			/*
			 * print details of a pixelmap (and palette)
			 */
			T_Log("Loaded '%s' as %s (%d,%d)\n",
				pm->identifier,PixelmapTypes[pm->type].identifier,pm->width,pm->height);
			if(pm->map!=NULL)
				T_Log("    Palette '%s' %s (%d, %d)\n",
					pm->map->identifier,PixelmapTypes[pm->map->type].identifier,pm->map->width, pm->map->height);
		}
	}

	/*
	 * free resources
	 */
	BrResFree(res_anchor);
	
	BrEnd();

	return 0;
}
/*
 * functions to read a pixel from a pixelmap type
 */

br_colour get_pmt_index_8(br_pixelmap *pm,int x,int y,int offset,int byte)
{
	char *src;
	br_colour *palette_entry;
	alpha_channel = 0;

	src = pm->pixels;
	palette_entry = pm->map->pixels;

	if (palette_entry == 0)
		alpha_channel = 255 << 24;

	return alpha_channel + palette_entry[*(src+offset+y*pm->row_bytes+x)];
}
br_colour get_pmt_rgb_555(br_pixelmap *pm,int x,int y,int offset,int byte)
{
	char *src;
	short pixel;

	src = pm->pixels;
	pixel = *(short *)(src+y*pm->row_bytes+x*byte+offset);

	return (BR_COLOUR_RGB((pixel >> 7) & 0xf8,(pixel >> 2) & 0xf8,(pixel << 3) & 0xf8)) & 0xffffff;
}
br_colour get_pmt_rgb_565(br_pixelmap *pm,int x,int y,int offset,int byte)
{
	char *src;
	short pixel;

	src = pm->pixels;
	pixel = *(short *)(src+y*pm->row_bytes+x*byte+offset);

	return (BR_COLOUR_RGB((pixel >> 8) & 0xf8,(pixel >> 3) & 0xf8,(pixel << 3) & 0xf8)) & 0xffffff;
}
br_colour get_pmt_rgb_888(br_pixelmap *pm,int x,int y,int offset,int byte)
{
	char *src;

	src = pm->pixels;
	src += offset+y*pm->row_bytes+x*byte;

	return (BR_COLOUR_RGB(src[2],src[1],src[0])& 0xffffff);
}
br_colour get_pmt_rgbx_888(br_pixelmap *pm,int x,int y,int offset,int byte)
{
	char *src;

	src = pm->pixels;
	src += offset+y*pm->row_bytes+x*byte;

	return  (src[3]<<24) + BR_COLOUR_RGB(src[2],src[1],src[0]);
}
br_colour get_pmt_rgba_8888(br_pixelmap *pm,int x,int y,int offset,int byte)
{
	char *src;

	src = pm->pixels;
	src += offset+y*pm->row_bytes+x*byte;

	return  (src[3]<<24) + BR_COLOUR_RGB(src[2],src[1],src[0]);
}

void put_pmt_rgb_555(br_pixelmap *pm,br_colour colour,int x,int y,int offset,int byte)
{
	char *src;
	short pixel;

	src = pm->pixels;

	pixel = ((BR_RED(colour) >> 3) << 10) +
		((BR_GRN(colour) >> 3) << 5) +
		((BR_BLU(colour) >> 3));

	*(short *)(src+y*pm->row_bytes+x*byte+offset) = pixel;
}
void put_pmt_rgb_565(br_pixelmap *pm,br_colour colour,int x,int y,int offset,int byte)
{
	char *src;
	short pixel;

	src = pm->pixels;

	pixel = ((BR_RED(colour) >> 3) << 11) +
		((BR_GRN(colour) >> 2) << 5) +
		((BR_BLU(colour) >> 3));

	*(short *)(src+y*pm->row_bytes+x*byte+offset) = pixel;
}
void put_pmt_rgb_888(br_pixelmap *pm,br_colour colour,int x,int y,int offset,int byte)
{
	char *src;

	src = pm->pixels;
	src += offset + y*pm->row_bytes+(x*byte);

	src[2] = BR_RED(colour);
	src[1] = BR_GRN(colour);
	src[0] = BR_BLU(colour);
}
void put_pmt_rgbx_888(br_pixelmap *pm,br_colour colour,int x,int y,int offset,int byte)
{
	char *src;

	src = pm->pixels;
	src += offset+y*pm->row_bytes+x*byte;

	src[3] = colour >> 24;
	src[2] = BR_RED(colour);
	src[1] = BR_GRN(colour);
	src[0] = BR_BLU(colour);
}
void put_pmt_rgba_8888(br_pixelmap *pm,br_colour colour,int x,int y,int offset,int byte)
{
	char *src;

	src = pm->pixels;
	src += offset+y*pm->row_bytes+x*byte;

	src[3] = colour >> 24;
	src[2] = BR_RED(colour);
	src[1] = BR_GRN(colour);
	src[0] = BR_BLU(colour);
}

static int CurrentPixelmaps(br_pixelmap ***mtable)
{
	int nmaps;
	/*
	 * return table of all current pixelmaps in registry
	 */
	nmaps = BrMapCount(NULL);
	if (nmaps != 0)
	{
		*mtable = BrMemCalloc(sizeof(**mtable),nmaps,BR_MEMORY_APPLICATION);

		nmaps = BrMapFindMany(NULL,*mtable,nmaps);
	}
	else BR_ERROR0("No current data available");

	return nmaps;
}
