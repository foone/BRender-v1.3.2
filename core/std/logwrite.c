/*
 * Copyright (c) 1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: logwrite.c 1.1 1997/12/10 16:41:28 jon Exp $
 * $Locker: $
 *
 * Glue to stdlib functions
 */
#include <stdio.h>

#include "brender.h"

BR_RCS_ID("$Id: logwrite.c 1.1 1997/12/10 16:41:28 jon Exp $")

int BR_PUBLIC_ENTRY BrLogWrite(void *buffer, br_size_t s, br_size_t n)
{
	return fwrite(buffer, s, n, stderr);
}

