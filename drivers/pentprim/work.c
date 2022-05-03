/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: work.c 1.1 1997/12/10 16:49:21 jon Exp $
 * $Locker: $
 *
 */
#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: work.c 1.1 1997/12/10 16:49:21 jon Exp $");

/*
 * The static work area for all the renderers
 */
_prim_work BR_ASM_DATA work;

/*
 * Exported entry point that provides a pointer to the static area for this image
 * 
 * Used to set up autoloaded primitives
 */
struct prim_work * BR_EXPORT PrimLibWork(void)
{
	return &work;
}

