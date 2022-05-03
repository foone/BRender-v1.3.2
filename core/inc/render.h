/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: render.h 1.1 1997/12/10 16:41:19 jon Exp $
 * $Locker: $
 *
 * Standard declarations relating to renderers
 */
#ifndef _RENDER_H_
#define _RENDER_H_

enum {
	BR_STATE_SURFACE			= 0x00000001,
	BR_STATE_MATRIX				= 0x00000002,
	BR_STATE_ENABLE				= 0x00000004,
	BR_STATE_LIGHT				= 0x00000008,
	BR_STATE_CLIP				= 0x00000010,
	BR_STATE_BOUNDS				= 0x00000020,
	BR_STATE_CULL				= 0x00000040,
	BR_STATE_STATS				= 0x00000080,

	BR_STATE_CACHE				= 0x00000100,

	BR_STATE_OUTPUT				= 0x40000000,
	BR_STATE_PRIMITIVE			= 0x80000000,

	BR_STATE_ALL				= 0xFFFFFFFF
};

#endif

/* 
 * Local Variables:
 * tab-width: 4
 * End:
 */

