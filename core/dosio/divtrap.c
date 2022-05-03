/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: divtrap.c 1.1 1997/12/10 16:40:58 jon Exp $
 * $Locker: $
 *
 * BIOS interface functions for drivers
 */
#include "brender.h"
#include "host.h"
#include "dosio.h"

BR_RCS_ID("$Id: divtrap.c 1.1 1997/12/10 16:40:58 jon Exp $")

static host_exception_hook div_hook;
extern void BR_ASM_CALL DivTrapHandler(void);
static int trapActive = BR_FALSE;

br_error BR_PUBLIC_ENTRY DOSDivTrapBegin(void)
{
	br_error r;
	host_info hi;
 	br_uint_16 cseg;

	if(trapActive)
		return BRE_ALLREADY_ACTIVE;

	/*
	 * Make sure host is up to it
	 */
	HostInfo(&hi,sizeof(hi));

	if(!(hi.capabilities & HOST_CAPS_EXCEPTION_HOOK))
		return BRE_FAIL;

	HostSelectorCS(&cseg);
	r = HostExceptionHook(&div_hook, 0, (br_uint_32)DivTrapHandler, cseg);

	if(r != BRE_OK)
		return r;

	trapActive = BR_TRUE;

	return BRE_OK;
}

void BR_PUBLIC_ENTRY DOSDivTrapEnd(void)
{
	if(!trapActive)
		return;

	HostExceptionUnhook(&div_hook);
}


