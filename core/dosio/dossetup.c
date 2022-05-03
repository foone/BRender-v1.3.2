/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: dossetup.c 1.1 1997/12/10 16:40:59 jon Exp $
 * $Locker: $
 *
 * Setup for DOSIO (registers DLLs for drivers)
 */
#include "brender.h"
#include "dosio.h"

BR_RCS_ID("$Id: dossetup.c 1.1 1997/12/10 16:40:59 jon Exp $")

br_error BR_PUBLIC_ENTRY DOSBegin(void)
{
	return BRE_OK;
}

void BR_PUBLIC_ENTRY DOSEnd(void)
{
}

