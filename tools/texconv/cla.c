/*
 * Copyright (c) 1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: cla.c 1.2 1998/08/11 12:05:07 jon Exp $
 * $Locker: $
 *
 */
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>

#include "brender.h"

#include "texconv.h"

#define CLA_MAX_LINE	256

static br_uint_32 UnRoll(char *filename, t_list *list)
{
	void *fh = NULL;
	char scratch_string[CLA_MAX_LINE], ch, *c;
	int i,len,n;
	char *last_command, **command_array;

	t_list *temp_list;
	t_cla *temp_node;

	br_uint_32 count;
	int open_mode = BR_FS_MODE_TEXT;

	if(*filename) {
		if((fh = BrFileOpenRead(filename, 0, NULL, &open_mode)) == NULL)
			BR_ERROR1("Unable to open command file '%s'", filename);
	} else
		BR_ERROR0("No command file specified with '@' option");

	temp_list = BrResAllocate(res_anchor, sizeof(*temp_list), BR_MEMORY_APPLICATION);

	count = 0;

	T_NewList(temp_list);

	while(BrFileEof(fh) == 0) {

		/*
		 * read a command line
		 */
		if(BrFileGetLine(scratch_string, CLA_MAX_LINE, fh) == 0)
			continue;

		n = 0;

		c = scratch_string;

		while (*c != '\0' && *c != ';' && *c != '#') {

			/*
			 * Skip over command
			 */
			while (BrIsSpace(*c))
				c++;

			last_command = c;

			while (!BrIsSpace(*c) && *c != '\0' && *c != ';' && *c != '#')
				c++;

			/*
			 * Add the command, if any was found
			 */
			if (c > last_command) {

				n++;

				temp_node = BrResAllocate(temp_list, sizeof(*temp_node), BR_MEMORY_APPLICATION);

				temp_node->command = BrMemAllocate(c - last_command + 1, BR_MEMORY_APPLICATION);
				BrMemCpy(temp_node->command, last_command, c - last_command);
				temp_node->command[c - last_command] = '\0';

				T_AddTail(temp_list, temp_node);
				last_command = scratch_string + i + 1;
			}

			c++;
		}

		command_array = BrResAllocate(res_anchor, n * sizeof(*command_array), BR_MEMORY_APPLICATION);

		for(i=0; temp_list->head != NULL; i++) {

			temp_node = (t_cla *)T_RemoveHead(temp_list);
			
			command_array[i] = temp_node->command;
		}

		/*
		 * decode command array - recursive
		 */
		count += T_DecodeCLA(n, command_array, list);

	}

	BrResFree(temp_list);
	BrFileClose(fh);

	return count;
}

br_uint_32 T_DecodeCLA(int argc, char **argv, t_list *list)
{
	br_uint_32 count, index;

	t_cla *new_node;

	for(count = 0, index = 0; argc > 0; argc--, index++) {

		/*
		 * check for 'rolled up' command list
		 */
		if(argv[index][0] == '@') {

			/*
			 * unroll file - leads to interresting recursion!
			 */
			count += UnRoll(argv[index] + 1, list);
		} else {

			new_node = (t_cla *)BrResAllocate(list, sizeof(*new_node), BR_MEMORY_APPLICATION);

			new_node->command = argv[index];

			/*
			 * discard comments - lines starting with '#' or ';'
			 */
			switch(new_node->command[0]) {
	
				case '#':
				case ';':
					BrResFree(new_node);
					break;

				default:
					T_AddTail(list, new_node);
					count++;
	
					break;
			}
		}
	}
	
	return count;    
}

br_uint_32 T_DecodeCommand(char *command, t_list *list)
{
	char *argument;
	char *delim = ",";
	t_cla *new_node;
	br_uint_32 count = 0;

	

	argument = strtok(BrMemStrDup(command), delim);
	while(argument != NULL) {

		new_node = BrResAllocate(list, sizeof(*new_node), BR_MEMORY_APPLICATION);

		new_node->command = argument;
		sscanf(argument, "%d", &new_node->ivalue);
		sscanf(argument, "%f", &new_node->fvalue);

		T_AddTail(list, new_node);
		
		count++;

		argument = strtok(NULL, delim);
	}
	
	return count;
}
