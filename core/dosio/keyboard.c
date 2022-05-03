/*
 * Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: keyboard.c 1.2 1998/02/17 23:01:15 jon Exp $
 * $Locker: $
 *
 * keyboard interrupt interface
 */
#include "brender.h"
#include "host.h"
#include "dosio.h"

BR_RCS_ID("$Id: keyboard.c 1.2 1998/02/17 23:01:15 jon Exp $")

/*
 * Real mode interrupt handler for INT15
 */
static br_uint_8 keyboardHandler[] = {
#include "kbdint.hex"
};

static br_boolean keyboardActive = BR_FALSE;

static host_real_memory keyboardMemory;

static br_uint_16 keyboardOldOff;
static br_uint_16 keyboardOldSeg;

extern host_real_memory _DOSEventMemory;
extern br_boolean _DOSEventQEnabled;

/*
 * Install a real mode interrupt handler on INT 15
 */
br_error BR_PUBLIC_ENTRY DOSKeyBegin(void)
{
	host_info hi;

	if(keyboardActive)
		return BRE_ALLREADY_ACTIVE;

	/*
	 * Make sure host is up to it
	 */
	HostInfo(&hi,sizeof(hi));

	if(!((hi.capabilities & HOST_CAPS_REAL_MEMORY) &&
         (hi.capabilities & HOST_CAPS_REAL_INT_HOOK) &&
         (hi.capabilities & HOST_CAPS_REAL_INT_CALL)))
		return BRE_FAIL;

	/* Allocate a block of DOS memory for handler
	 */
	if(HostRealAllocate(&keyboardMemory, sizeof(keyboardHandler)))
		return BRE_NO_MEMORY;

	/*
	 * Copy real mode handler into block
	 */
	HostFarBlockWrite(keyboardMemory.pm_off, keyboardMemory.pm_seg, keyboardHandler, sizeof(keyboardHandler));

	/*
	 * Get old real mode vector
	 */
	HostRealInterruptGet(0x15,&keyboardOldOff,&keyboardOldSeg);

	/*
	 * Put chain address in realmode code
	 */
	HostFarWordWrite(SYM_KeyboardOldOff+keyboardMemory.pm_off, keyboardMemory.pm_seg, keyboardOldOff);
	HostFarWordWrite(SYM_KeyboardOldSeg+keyboardMemory.pm_off, keyboardMemory.pm_seg, keyboardOldSeg);

	/*
	 * If the event queue is initialised, then hook that
	 * into handler
	 */
	if(_DOSEventQEnabled) {
		HostFarWordWrite(SYM_KeyboardEventQOff+keyboardMemory.pm_off, keyboardMemory.pm_seg, _DOSEventMemory.rm_off);
		HostFarWordWrite(SYM_KeyboardEventQSeg+keyboardMemory.pm_off, keyboardMemory.pm_seg, _DOSEventMemory.rm_seg);
	}

	/* Set new real mode vector
	 */
	if(HostRealInterruptSet(0x15,(br_uint_16)(keyboardMemory.rm_off+SYM_KeyboardHandler),keyboardMemory.rm_seg)) {
		HostRealFree(&keyboardMemory);
		return BRE_FAIL;
	}

	keyboardActive = BR_TRUE;

	return BRE_OK;
}

/*
 * Clear down the custom keyboard handler
 */
void BR_PUBLIC_ENTRY DOSKeyEnd(void)
{
	if(!keyboardActive)
		return;

	/* Set new real mode vector
	 */
	HostRealInterruptSet(0x15,keyboardOldOff,keyboardOldSeg);

	/*
	 * Release memeory block
	 */
	HostRealFree(&keyboardMemory);

	keyboardActive = BR_FALSE;
}

/*
 * Enable or diable BIOS keystrokes, if enabled, then the
 * keyboard handler passes all scancode on to the BIOS
 */
br_error BR_PUBLIC_ENTRY DOSKeyEnableBIOS(br_boolean flag)
{
	if(!keyboardActive)
		return BRE_NOT_ACTIVE;

	HostFarWordWrite(SYM_KeyboardEnableBIOS+keyboardMemory.pm_off, keyboardMemory.pm_seg, (br_uint_16)(flag != 0));

	return BRE_OK;
}

/*
 *  Test if a key on keyboard is depressed
 */
br_boolean BR_PUBLIC_ENTRY DOSKeyTest(br_uint_8 scancode,br_uint_8 qualifiers, br_uint_8 repeats)
{
	br_uint_8 r;
	br_uint_8 m;

	m = HostFarByteRead(scancode + SYM_KeyboardMap + keyboardMemory.pm_off,keyboardMemory.pm_seg);

	if(qualifiers == 0)
		r = (m & (QUAL_NONE | repeats)) == (QUAL_NONE | repeats);
	else
		r = (m & (QUAL_ALL | repeats)) == (QUAL_NONE | qualifiers | repeats);

	if (r) {

		m &= ~repeats;

		HostFarByteWrite(scancode + SYM_KeyboardMap + keyboardMemory.pm_off,keyboardMemory.pm_seg,m);
	}

	return r;
}

br_uint_8 BR_PUBLIC_ENTRY DOSKeyMap(br_uint_8 scancode)
{
	return HostFarByteRead(scancode + SYM_KeyboardMap + keyboardMemory.pm_off,keyboardMemory.pm_seg);
}

