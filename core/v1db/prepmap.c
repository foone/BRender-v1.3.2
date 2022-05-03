/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: prepmap.c 1.2 1998/09/25 15:37:40 johng Exp $
 * $Locker: $
 *
 * Precompute information for texture maps
 */
#include "v1db.h"
#include "brassert.h"
#include "shortcut.h"

BR_RCS_ID("$Id: prepmap.c 1.2 1998/09/25 15:37:40 johng Exp $")

void BR_PUBLIC_ENTRY BrMapUpdate(br_pixelmap *map, br_uint_16 flags)
{
    UASSERT_MESSAGE("Invalid BrMapUpdate pointer", map != NULL);
	BrBufferUpdate(map, BRT_COLOUR_MAP_O, flags);
}

void BrBufferUpdate(br_pixelmap *pm, br_token use, br_uint_16 flags)
{
	br_token_value tv[] = {
		{BRT_PREFER_SHARE_B,	BR_FALSE},
                {BRT_CAN_SHARE_B,       BR_TRUE},
                {BRT_UPDATE_DATA_B,     BR_FALSE},
		{0}
	};

    ASSERT_MESSAGE("Invalid BrBufferUpdate pointer", pm != NULL);
	if(v1db.renderer == NULL)
		return;

	if(flags & BR_MAPU_SHARED)
		tv[0].v.b = BR_TRUE;

	if(flags & BR_MAPU_DATA)
		tv[2].v.b = BR_TRUE;

	/*
	 * If there is a stored buffer, update that, otherwise allocate a new buffer
	 */
	if(pm->stored)
		BufferStoredUpdate(pm->stored, (br_device_pixelmap *)pm, tv); 
	else
		RendererBufferStoredNew(v1db.renderer, (br_buffer_stored **)&pm->stored, use, (br_device_pixelmap *)pm, tv);
}

void BrBufferClear(br_pixelmap *pm)
{
	if(pm->stored) {
		ObjectFree(pm->stored);
		pm->stored = NULL;
	}
}
