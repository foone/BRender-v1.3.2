/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: bios.c 1.1 1997/12/10 16:53:21 jon Exp $
 * $Locker: $
 *
 * BIOS interface functions for drivers
 */
#include "drv.h"
#include "host.h"

BR_RCS_ID("$Id: bios.c 1.1 1997/12/10 16:53:21 jon Exp $")

static host_regs regs;

br_uint_16 BIOSVideoGetMode(void)
{
	regs.h.ah = 0x0F;
	HostRealInterruptCall(0x10, &regs);
	return (br_uint_16)regs.h.al;
}

br_error BIOSVideoSetMode(br_uint_16 mode)
{
	regs.w.ax = mode;
	HostRealInterruptCall(0x10, &regs);
	
	return (regs.x.flags & HOST_FLAG_CARRY)?BRE_FAIL:BRE_OK;
}


