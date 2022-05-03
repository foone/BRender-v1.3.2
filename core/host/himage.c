/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: himage.c 1.1 1997/12/10 16:41:12 jon Exp $
 * $Locker: $
 *
 * Using images from host environment
 */
#include "brender.h"

#include "host.h"
#include "host_ip.h"

#ifdef __DOS__

void * BR_RESIDENT_ENTRY HostImageLoad(char *name)
{
	return NULL;
}

void BR_RESIDENT_ENTRY HostImageUnload(void *image)
{
}

void * BR_RESIDENT_ENTRY HostImageLookupName(void *img, char *name, br_uint_32 hint)
{
	return NULL;
}

void * BR_RESIDENT_ENTRY HostImageLookupOrdinal(void *img, br_uint_32 ordinal)
{
	return NULL;
}
#endif

#ifdef __WIN_32__

#include <windows.h>

void * BR_RESIDENT_ENTRY HostImageLoad(char *name)
{
	return (void *)LoadLibrary(name);
}

void BR_RESIDENT_ENTRY HostImageUnload(void *image)
{
	FreeLibrary((HMODULE)image);
}

void * BR_RESIDENT_ENTRY HostImageLookupName(void *img, char *name, br_uint_32 hint)
{
	return (void *)GetProcAddress((HMODULE)img, name);
}

void * BR_RESIDENT_ENTRY HostImageLookupOrdinal(void *img, br_uint_32 ordinal)
{
	return NULL;
}
#endif

#ifdef __OS2__

#include <os2.h>

void * BR_RESIDENT_ENTRY HostImageLoad(char *name)
{
	return NULL;
}

void BR_RESIDENT_ENTRY HostImageUnload(void *image)
{
}

void * BR_RESIDENT_ENTRY HostImageLookupName(void *img, char *name, br_uint_32 hint)
{
	return NULL;
}

void * BR_RESIDENT_ENTRY HostImageLookupOrdinal(void *img, br_uint_32 ordinal)
{
	return NULL;
}
#endif

