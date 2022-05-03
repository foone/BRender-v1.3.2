/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: stdmem.c 1.1 1997/12/10 16:59:24 jon Exp $
 * $Locker: $
 *
 * Default memory handler that uses malloc()/free() from C library
 */
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

#include "brender.h"
#include "trackmem.h"

BR_RCS_ID("$Id: stdmem.c 1.1 1997/12/10 16:59:24 jon Exp $")

/*
 * Glue functions for malloc()/free()
 */
static void * BR_CALLBACK BrStdlibAllocate(br_size_t size, br_uint_8 type)
{
	void *m;

	m = _track_alloc(size,caller(5),"BrStdlib",BrResClassIdentifier(type));

//	m = malloc(size);

	if(m == NULL)
		BR_ERROR2("BrStdlibAllocate: failed with size=%d, type=%d",size,type);

	return m;
}

static void BR_CALLBACK BrStdlibFree(void *mem)
{
	_track_free(mem,caller(6));
//	free(mem);
}

static br_size_t BR_CALLBACK BrStdlibInquire(br_uint_8 type)
{
	/* XXX */
	return 0;
}

static br_uint_32 BR_CALLBACK BrStdlibAlign(br_uint_8 type)
{
#if defined(__WATCOMC__)
	return 4;
#elif defined (_MSC_VER)
	return 4;
#elif defined (__BORLANDC__)
	return 4;
#else
	return 1;
#endif
}

/*
 * Allocator structure
 */
br_allocator BrStdlibAllocator = {
	"malloc",
	BrStdlibAllocate,
	BrStdlibFree,
	BrStdlibInquire,
	BrStdlibAlign,
};

/*
 * Override global variable s.t. this is the default allocator
 */
br_allocator * BR_ASM_DATA _BrDefaultAllocator = &BrStdlibAllocator;


