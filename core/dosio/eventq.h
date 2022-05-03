/*
 * Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: eventq.h 1.1 1997/12/10 16:40:59 jon Exp $
 * $Locker: $
 *
 * Event queue for mouse and keyboard events - events can be added to queue
 * from both real-mode and protected mode
 *
 * NB: the structures and equates in eventq.inc must match
 */
#ifndef _EVENTQ_H_
#define _EVENTQ_H_

typedef struct dosio_event {
		br_uint_16	type;
		br_uint_16	qualifiers;
		br_uint_32	value_1;
		br_uint_32	value_2;
} dosio_event;

/*
 * Event types - matches those in brwap.h
 */
enum dosio_event_type {
	DOSIO_EVENT_KEY_DOWN,
	DOSIO_EVENT_KEY_UP,
	/* value_1 == key code		*/

	DOSIO_EVENT_POINTER1_DOWN,
	DOSIO_EVENT_POINTER1_UP,
	DOSIO_EVENT_POINTER2_DOWN,
	DOSIO_EVENT_POINTER2_UP,
	DOSIO_EVENT_POINTER3_DOWN,
	DOSIO_EVENT_POINTER3_UP,
	/* value_1 == x				*/
	/* value_2 == y				*/

	DOSIO_EVENT_POINTER_MOVE,
	/* value_1 == x				*/
	/* value_2 == y				*/

	DOSIO_EVENT_MAX
};

enum dosio_event_qual {
	DOSIO_QUAL_SHIFT	= 1,
	DOSIO_QUAL_CONTROL	= 2,
	DOSIO_QUAL_ALT		= 4,
};

/*
 * An event queue
 *
 */
typedef struct dosio_event_queue {
	/*
	 * Maximum number of events in buffer
	 */
	br_uint_16	total;

	/*
	 * Number of events in queue
	 */
	br_uint_16	count;

	/*
	 * Next free slot in which to add an event
	 */
	br_uint_16	head;

	/*
	 * Next event slot to remove from queue
	 */
	br_uint_16	tail;

	/*
	 * Array of event slots
	 */
	dosio_event slots[1];
} dosio_event_queue;

#endif
