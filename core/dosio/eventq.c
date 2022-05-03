/*
 * Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: eventq.c 1.1 1997/12/10 16:40:59 jon Exp $
 * $Locker: $
 *
 * Event queue for mouse and keyboard events - events can be added to queue
 * from both real-mode and protected mode
 */
#include <stddef.h>

#include "brender.h"
#include "host.h"
#include "dosio.h"

BR_RCS_ID("$Id: eventq.c 1.1 1997/12/10 16:40:59 jon Exp $")

#define MAX_EVENTS 50

host_real_memory _DOSEventMemory;
int _DOSEventQEnabled = 0;

br_error BR_PUBLIC_ENTRY DOSEventBegin(void)
{
	host_info hi;
	int s = sizeof(dosio_event_queue) + sizeof(dosio_event) * MAX_EVENTS;

	if(_DOSEventQEnabled)
		return BRE_ALLREADY_ACTIVE;

	/*
	 * Make sure host is up to it
	 */
	HostInfo(&hi,sizeof(hi));

	if(!(hi.capabilities & HOST_CAPS_REAL_MEMORY))
		return BRE_FAIL;

	/*
	 * Put event queue in a real mode buffer
	 */
	if(HostRealAllocate(&_DOSEventMemory, s))
		return BRE_NO_MEMORY;

	HostFarBlockFill(_DOSEventMemory.pm_off, _DOSEventMemory.pm_seg, 0, s);
	HostFarWordWrite(_DOSEventMemory.pm_off + offsetof(dosio_event_queue,total), _DOSEventMemory.pm_seg, MAX_EVENTS);

	_DOSEventQEnabled = 1;
	return BRE_OK;
}

void BR_PUBLIC_ENTRY DOSEventEnd(void)
{
	if(_DOSEventQEnabled == 0)
		return;

	/*
	 * Free up event queue
	 */
	_DOSEventQEnabled = 0;
	HostRealFree(&_DOSEventMemory);
}

#define EVENTQ_GET(field)\
 HostFarWordRead(_DOSEventMemory.pm_off + offsetof(dosio_event_queue,field), _DOSEventMemory.pm_seg)
#define EVENTQ_SET(field,value)\
 HostFarWordWrite(_DOSEventMemory.pm_off + offsetof(dosio_event_queue,field), _DOSEventMemory.pm_seg,(br_uint_16)(value))

br_boolean BR_PUBLIC_ENTRY DOSEventWait(dosio_event *event, br_boolean block)
{
	int t;

	if(_DOSEventQEnabled == 0)
		return BR_FALSE;

	if(block) {
		/*
		 * Busy wait on an incoming event
		 */
		while(EVENTQ_GET(head) == EVENTQ_GET(tail))
			;
	} else {
		if(EVENTQ_GET(head) == EVENTQ_GET(tail))
			return BR_FALSE;
	}

	t = EVENTQ_GET(tail);

	/*
	 * Hostract event from queue
	 */
	HostFarBlockRead(
		_DOSEventMemory.pm_off + offsetof(dosio_event_queue,slots) +
		sizeof(*event) * t,
		_DOSEventMemory.pm_seg,
		(unsigned char *)event,
		sizeof(*event));

	/*
	 * Bump tail pointer over event
	 */
	t += 1;

	if( t >= EVENTQ_GET(total))
		t = 0;

	EVENTQ_SET(tail, t);

	EVENTQ_SET(count, EVENTQ_GET(count)-1);

	return BR_TRUE;
}

