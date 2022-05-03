/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: readmse.c 1.2 1998/07/13 21:49:54 jon Exp $
 * $Locker: $
 *
 * Read mouse position from BIOS
 */
#include "brender.h"
#include "host.h"
#include "dosio.h"

BR_RCS_ID("$Id: readmse.c 1.2 1998/07/13 21:49:54 jon Exp $")

/*
 * Real mode interrupt handler for mouse
 */
static unsigned char mouseHandler[] = {
#include "mouint.hex"
};

static br_boolean mouseActive = BR_FALSE;

static host_real_memory mouseMemory;

extern host_real_memory _DOSEventMemory;
extern int _DOSEventQEnabled;

static host_regs regs;

static br_int_16 ox, oy;

br_error BR_PUBLIC_ENTRY DOSMouseBegin(void)
{
	host_info hi;

	if(mouseActive)
		return BRE_ALLREADY_ACTIVE;

	/*
	 * Make sure host is up to it
	 */
	HostInfo(&hi,sizeof(hi));

	if(!((hi.capabilities & HOST_CAPS_REAL_MEMORY) &&
         (hi.capabilities & HOST_CAPS_REAL_INT_CALL)))
		return BRE_FAIL;

	/* Allocate a block of DOS memory for handler
	 */
	if(HostRealAllocate(&mouseMemory, sizeof(mouseHandler)))
		return BRE_NO_MEMORY;

	/*
	 * Copy real mode handler into block
	 */
	HostFarBlockWrite(mouseMemory.pm_off, mouseMemory.pm_seg, mouseHandler, sizeof(mouseHandler));

	/*
	 * If the event queue is initialised, then hook that into handler
	 */
	if(_DOSEventQEnabled) {
		HostFarWordWrite(SYM_MouseEventQOff+mouseMemory.pm_off, mouseMemory.pm_seg, _DOSEventMemory.rm_off);
		HostFarWordWrite(SYM_MouseEventQSeg+mouseMemory.pm_off, mouseMemory.pm_seg, _DOSEventMemory.rm_seg);
	}

	/*
	 * Reset mouse driver
	 */
	regs.w.ax = 0;
	HostRealInterruptCall(0x33,&regs);

	/*
	 * Check mouse is installed
	 */
	if (regs.w.ax != 0xffff)
		return BRE_FAIL;

	/*
	 * Set real-mode handler
	 */
	regs.w.ax = 0x0C;
	regs.w.cx = 0x7F;
	regs.w.dx = mouseMemory.rm_off;
	regs.w.es = mouseMemory.rm_seg;

	HostRealInterruptCall(0x33,&regs);

	/*
	 * Read initial mouse status
	 */
	regs.w.ax = 0x03;

	HostRealInterruptCall(0x33,&regs);

	ox = regs.w.cx;
	oy = regs.w.dx;

	HostFarWordWrite(SYM_MouseX+mouseMemory.pm_off, mouseMemory.pm_seg, ox);
	HostFarWordWrite(SYM_MouseY+mouseMemory.pm_off, mouseMemory.pm_seg, oy);
	HostFarWordWrite(SYM_MouseButtons+mouseMemory.pm_off, mouseMemory.pm_seg, 0);

	mouseActive = BR_TRUE;

	return BRE_OK;
}

void BR_PUBLIC_ENTRY DOSMouseEnd(void)
{
	if(!mouseActive)
		return;

	/*
	 * Reset mouse driver
	 */
	regs.w.ax = 0;
	HostRealInterruptCall(0x33,&regs);

	/*
	 * Free handler
	 */
	HostRealFree(&mouseMemory);

	mouseActive = BR_FALSE;
}

br_error BR_PUBLIC_ENTRY DOSMouseRead(br_int_32 *mouse_x,br_int_32 *mouse_y,br_uint_32 *mouse_buttons)
{
	br_int_16 mx,my;

	if(!mouseActive)
		return BRE_NOT_ACTIVE;

	mx = HostFarWordRead(SYM_MouseX+mouseMemory.pm_off, mouseMemory.pm_seg);
	my = HostFarWordRead(SYM_MouseY+mouseMemory.pm_off, mouseMemory.pm_seg);
	*mouse_buttons = HostFarWordRead(SYM_MouseButtons+mouseMemory.pm_off, mouseMemory.pm_seg);

	*mouse_x += mx - ox;
	*mouse_y += my - oy;

	ox = mx;
	oy = my;

	return BRE_OK;
}

