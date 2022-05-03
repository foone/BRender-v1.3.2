/*
 * Copyright (c) 1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: cla.h 1.1 1997/12/10 16:58:47 jon Exp $
 * $Locker: $
 *
 */
#ifndef _CLA_H_
#define _CLA_H_

typedef struct t_cla {

	t_list_node node;
	char *command;

	br_int_32 ivalue;
	br_float fvalue;
    
} t_cla;

br_uint_32 T_DecodeCLA(int argc, char **argv, t_list *list);
br_uint_32 T_DecodeCommand(char *command, t_list *list);

#endif
