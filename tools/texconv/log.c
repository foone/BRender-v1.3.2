/*
 * Copyright (c) 1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: log.c 1.1 1997/12/10 16:58:55 jon Exp $
 * $Locker: $
 *
 *
 */
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>

#include "brender.h"
#include "dosio.h"

#include "texconv.h"

static char _scratch_string[4096];


void T_Log(char *fmt, ...)
{
	int n;
	va_list args;

	va_start(args, fmt);
	vsprintf(_scratch_string, fmt, args);
	va_end(args);

        fprintf(stdout, _scratch_string);
}

