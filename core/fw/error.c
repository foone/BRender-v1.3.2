/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: error.c 1.1 1997/12/10 16:41:06 jon Exp $
 * $Locker: $
 *
 * Error value support
 */

#include "fw.h"

BR_RCS_ID("$Id: error.c 1.1 1997/12/10 16:41:06 jon Exp $")

br_error BR_RESIDENT_ENTRY BrLastErrorGet(void **valuep)
{
	if(valuep)
		valuep = fw.last_error_value;
	
	return fw.last_error_type;
}

void BR_RESIDENT_ENTRY BrLastErrorSet(br_error type, void *value)
{
	fw.last_error_type = type;
	fw.last_error_value = value;
}
