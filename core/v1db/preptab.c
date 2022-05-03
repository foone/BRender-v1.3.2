/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: preptab.c 1.1 1997/12/10 16:41:33 jon Exp $
 * $Locker: $
 *
 * Precompute information for tables
 */
#include "v1db.h"
#include "brassert.h"
#include "shortcut.h"

BR_RCS_ID("$Id: preptab.c 1.1 1997/12/10 16:41:33 jon Exp $")

void BR_PUBLIC_ENTRY BrTableUpdate(br_pixelmap *table, br_uint_16 flags)
{
    UASSERT_MESSAGE("Invalid BrTableUpdate pointer", table != NULL);
	BrBufferUpdate(table, BRT_UNKNOWN, flags);
}
