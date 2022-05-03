/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: object.c 1.1 1997/12/10 16:45:44 jon Exp $
 * $Locker: $
 *
 * Local methods for default object object
 *
 */
#include <stddef.h>
#include <string.h>

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: object.c 1.1 1997/12/10 16:45:44 jon Exp $");

/*
 * Get identifier associated with a object
 */
char * BR_CMETHOD_DECL(br_object_vga, identifier)(br_object *self)
{
	return self->identifier;
}

/*
 * Find the device assocaited with a object
 */
br_device *	BR_CMETHOD_DECL(br_object_vga, device)(br_object *self)
{
	return self->device;
}


