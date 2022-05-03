/*
 * Copyright (c) 1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: savepix.h 1.1 1997/12/10 16:59:09 jon Exp $
 * $Locker: $
 *
 */
#ifndef _SAVEPIX_H_
#define _SAVEPIX_H_

typedef struct Output_Types {
	char *name;
	br_uint_32 type;
} Output_Types;

extern Output_Types OutputFileTypes[];
extern int OutputFileTypesSize;
extern int OutputType;

void T_OutputPixelmap(char *filename,br_uint_32 type);

#endif
