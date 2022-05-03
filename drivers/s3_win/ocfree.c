/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: ocfree.c 1.1 1997/12/10 16:51:10 jon Exp $
 * $Locker: $
 *
 * Device methods
 */
#include <stddef.h>
#include <string.h>
#include <windows.h>
#include <ddraw.h>

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

/*
 * Utility function to free all the objects in a container
 */
br_error ObjectContainerFree(br_object_container *self, br_token type, char *pattern, br_token_value *tv)
{
	br_error r;
	br_object **handles;
	br_int_32 count,n,i;

	r = ObjectContainerCount(self, &count, type, pattern, tv);

	if(r != BRE_OK)
		return r;

	if(count == 0)
		return BRE_OK;

	handles = BrMemAllocate(count * sizeof(*handles), BR_MEMORY_DRIVER);

	r = ObjectContainerFindMany(self, handles, count, &n, type, pattern, tv);

	for(i=0; i < n; i++)
		ObjectFree(handles[i]);

	BrMemFree(handles);

	return BRE_OK;
}

