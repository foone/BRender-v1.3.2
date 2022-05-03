/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: brbhook.c 1.1 1997/12/10 16:59:28 jon Exp $
 * $Locker: $
 *
 * Default begin/end hooks
 */
#include <stdlib.h>
#include "brender.h"

void BR_CALLBACK _BrBeginHook(void)
{
	struct br_device * BR_EXPORT BrDrv1MCGABegin(char *arguments);
        BrDevAddStatic(NULL, BrDrv1MCGABegin, NULL);
}

void BR_CALLBACK _BrEndHook(void)
{
}

