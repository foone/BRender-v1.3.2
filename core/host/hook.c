/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: hook.c 1.1 1997/12/10 16:41:12 jon Exp $
 * $Locker: $
 *
 * Wrappers for hooking interrupts and exceptions
 */
#include "host.h"
#include "host_ip.h"

br_error BR_RESIDENT_ENTRY HostInterruptHook(host_interrupt_hook *h, br_uint_8 vector, br_uint_32  off,  br_uint_16  sel)
{
	br_error r;

	if(h->active)
		return BRE_ALLREADY_ACTIVE;

	h->vector = vector;

	r = HostInterruptGet(vector, &h->old_offset, &h->old_sel);
	if(r != BRE_OK)
		return BRE_FAIL;

	r = HostInterruptSet(vector, off, sel);
	if(r != BRE_OK)
		return BRE_FAIL;

	h->active = BR_TRUE;
	return BRE_OK;
}

br_error BR_RESIDENT_ENTRY HostInterruptUnhook(host_interrupt_hook *h)
{
	br_error r;

	if(!h->active)
		return BRE_NOT_ACTIVE;

	r = HostInterruptSet(h->vector, h->old_offset, h->old_sel);
	if(r != BRE_OK)
		return BRE_FAIL;

	h->active = BR_FALSE;
	return BRE_FAIL;
}

br_error BR_RESIDENT_ENTRY HostExceptionHook(host_exception_hook *h, br_uint_8 exception, br_uint_32  off,  br_uint_16  sel)
{
	br_error r;

	if(h->active)
		return BRE_ALLREADY_ACTIVE;

	h->exception = exception;

	r = HostExceptionGet(exception, &h->old_offset, &h->old_sel);
	if(r != BRE_OK)
		return BRE_FAIL;

	r = HostExceptionSet(exception, off, sel);
	if(r != BRE_OK)
		return BRE_FAIL;

	h->active = BR_TRUE;
	return BRE_OK;
}

br_error BR_RESIDENT_ENTRY HostExceptionUnhook(host_exception_hook *h)
{
	br_error r;

	if(!h->active)
		return BRE_NOT_ACTIVE;

	r = HostExceptionSet(h->exception, h->old_offset, h->old_sel);
	if(r != BRE_OK)
		return BRE_FAIL;

	h->active = BR_FALSE;
	return BRE_FAIL;
}

