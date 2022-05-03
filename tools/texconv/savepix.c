/*
 * Copyright (c) 1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: savepix.c 1.1 1997/12/10 16:59:08 jon Exp $
 * $Locker: $
 *
 *
 */
#include <stddef.h>
#include <stdio.h>

#include "brender.h"
#include "dosio.h"

#include "texconv.h"

Output_Types OutputFileTypes[] = {
    
	{"palette",	T_PALETTE},	/* save any palette 		: pixelmap->map			*/
	{"pal",		T_PALETTE},
	
	{"image",	T_IMAGE},	/* save image without palette	: pixelmap, pixelmap->map=NULL	*/
	
	{"pixelmap",	T_PIXELMAP},	/* save pixelmap with palette	: pixelmap			*/
	{"pix",		T_PIXELMAP},	/* save pixelmap with palette	: pixelmap			*/
	
	{"targa",	T_TARGA},	/* save as (uncompressed) targa					*/
	{"tga",		T_TARGA},
};

int OutputFileTypesSize = BR_ASIZE(OutputFileTypes);

int OutputType = TYPE_GUESS;

void T_OutputPixelmap(char *filename, br_uint_32 type)
{
	t_pixelmap_cbfn_info *cbfn_command;
	int i, n;
	br_uint_32 count;
    
	cbfn_command = BrResAllocate(res_anchor, sizeof(*cbfn_command), BR_MEMORY_APPLICATION);

	/*
	 * save pixelmap or palette 
	 */
	cbfn_command->nmaps = BrMapCount(NULL);

	if(cbfn_command->nmaps == 0) {

		T_Log("No current data available to save");
		return;
	}

	switch(type) {

		case T_TARGA: {
		    		char *name;
				
				/*
				 * save pixelmaps as targa images
				 */
				T_GenerateMapTable(cbfn_command);

				if(cbfn_command->nmaps == 1) {
					name = T_SaveTarga(cbfn_command->map_table[0], filename);

					T_Log("Saved '%s' (%s) as '%s'\n",
						cbfn_command->map_table[0]->identifier,
						T_EncodePixelmapType(cbfn_command->map_table[0]->type),
						name);
				} else
					for(i=0; i<cbfn_command->nmaps; i++) {
				    
						name = T_SaveTarga(cbfn_command->map_table[i], NULL);

						T_Log("Saved '%s' (%s) as '%s'\n",
							cbfn_command->map_table[i]->identifier,
							T_EncodePixelmapType(cbfn_command->map_table[i]->type),
							name);
					}

				}
				break;

		case T_IMAGE:
				/*
				 * save image without palette
				 */
				cbfn_command->map_table = BrResAllocate(cbfn_command,
					sizeof(*cbfn_command->map_table) * cbfn_command->nmaps, BR_MEMORY_APPLICATION);
					
				cbfn_command->count = 0;
				cbfn_command->type = T_CBFN_GENERATE_IMAGE_TABLE;

				/*
				 * call generic cbfn
				 */
				BrMapEnum(NULL, T_PixelmapCbfn, cbfn_command);

				goto save_pix;
				
				break;
				
		case T_PALETTE:
				/*
				 * save palettes from indexed pixelmaps in registry
				 */
				cbfn_command->type = T_CBFN_COUNT_PALETTES;
				cbfn_command->npals= 0;

				/*
				 * count palettes
				 */
				BrMapEnum(NULL, T_PixelmapCbfn, cbfn_command);

				cbfn_command->map_table = BrResAllocate(cbfn_command,
					sizeof(*cbfn_command->map_table) * cbfn_command->npals, BR_MEMORY_APPLICATION);
					
				cbfn_command->type = T_CBFN_GENERATE_PALETTE_TABLE;
				cbfn_command->count = 0;

				/*
				 * get palette table
				 */
				BrMapEnum(NULL, T_PixelmapCbfn, cbfn_command);

				cbfn_command->nmaps = cbfn_command->count;

				goto save_pix;
		
				break;
				
		case T_PIXELMAP:
				/*
				 * save all pixelmaps in registry
				 */
				T_GenerateMapTable(cbfn_command);

				/*
				 * save map cbfn_command->map_table (table of pixelmap pointers)
				 */
save_pix:
				n = BrPixelmapSaveMany(filename, cbfn_command->map_table, cbfn_command->nmaps);

				if(n == 0)
					BR_ERROR0("Unable to save pixelmaps");
				else {
					T_Log("Saved ");

 					/*
					 * generate some lovely user information...
					 */
					for(i=0; i<cbfn_command->nmaps - 1; i++) {
					    
						T_Log("'%s' (%d,%d  %s)", cbfn_command->map_table[i]->identifier,
							cbfn_command->map_table[i]->width,
							cbfn_command->map_table[i]->height,
							T_EncodePixelmapType(cbfn_command->map_table[i]->type));

						if(cbfn_command->map_table[i]->map != NULL)
							T_Log(" with palette (%s)\n", T_EncodePixelmapType(cbfn_command->map_table[i]->map->type));
						else
							T_Log("\n");
					}

					T_Log("'%s' (%d,%d  %s)",
						cbfn_command->map_table[i]->identifier,
						cbfn_command->map_table[i]->width,
						cbfn_command->map_table[i]->height,
						T_EncodePixelmapType(cbfn_command->map_table[i]->type));

					if(cbfn_command->map_table[i]->map != NULL)
						T_Log(" with palette (%s)", T_EncodePixelmapType(cbfn_command->map_table[i]->map->type));

					T_Log(" to '%s' (%d pixelmap%s",
						filename,
						cbfn_command->nmaps, (cbfn_command->nmaps == 1)?")\n":"s)\n");
				}

				break;
	}

	/*
	 * clear up th' mess
	 */
	BrResFree(cbfn_command);
}
