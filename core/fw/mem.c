/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: mem.c 1.1 1997/12/10 16:41:08 jon Exp $
 * $Locker: $
 *
 * Low level wrappers for allocator access
 */
#include "fw.h"
#include "brassert.h"

BR_RCS_ID("$Id: mem.c 1.1 1997/12/10 16:41:08 jon Exp $")

#define MEM_LOG 0

void * BR_RESIDENT_ENTRY BrMemAllocate(br_size_t size, br_uint_8 type)
{
	void *b;

	UASSERT(fw.resource_class_index[type] != NULL);

	ASSERT(fw.mem->allocate != NULL);

	b = fw.mem->allocate(size,type);

#if MEM_LOG
	BrLogPrintf("BrMemAllocate(%d,%s) = %p\n",size,fw.resource_class_index[type]->identifier,b);
#endif

	/*
	 * Clear block
	 */
	BrMemSet(b,0,size);

	return b;
}

void BR_RESIDENT_ENTRY BrMemFree(void *block)
{
	UASSERT(block != NULL);

#if 0
	/*
	 * Breaks the modularity a bit - but make sure
	 * we are not trying to free a resource
	 */
	UASSERT(!BrResCheck(block,0));
#endif

#if MEM_LOG
	BrLogPrintf("BrMemFree(%p)\n",block);
#endif

	ASSERT(fw.mem->free != NULL);

	fw.mem->free(block);
}

br_size_t BR_RESIDENT_ENTRY BrMemInquire(br_uint_8 type)
{
	br_size_t i;

	UASSERT(fw.resource_class_index[type] != NULL);

	ASSERT(fw.mem->inquire != NULL);

	i = fw.mem->inquire(type);

#if MEM_LOG
	BrLogPrintf("BrMemInquire(%s) = %d\n",fw.resource_class_index[type]->identifier,i);
#endif

	return i;
}

br_int_32 BR_RESIDENT_ENTRY BrMemAlign(br_uint_8 type)
{
	br_int_32 i = 0;

	UASSERT(fw.resource_class_index[type] != NULL);

	if(fw.mem->align)
		i = fw.mem->align(type);

	return i;
}

/*
 * calloc() equivalent
 */
void * BR_RESIDENT_ENTRY BrMemCalloc(int nelems, br_size_t size, br_uint_8 type)
{
	void *b;

	UASSERT(fw.resource_class_index[type] != NULL);

	ASSERT(fw.mem->allocate != NULL);

	b = fw.mem->allocate(size * nelems,type);
	BrMemSet(b,0,size * nelems);

#if MEM_LOG
	BrLogPrintf("BrMemCalloc(%d,%d,%s) = %p\n",nelems, size, fw.resource_class_index[type]->identifier ,b);
#endif

	return b;
}

/*
 * strdup() equivalent
 */
char * BR_RESIDENT_ENTRY BrMemStrDup(char *str)
{
	int l;
	char *nstr;

	UASSERT(str != NULL);

	l = BrStrLen(str);

#if MEM_LOG
	BrLogPrintf("BrMemStrDup(%s)\n",str);
#endif

	nstr = BrMemAllocate(l+1,BR_MEMORY_STRING);

	BrStrCpy(nstr,str);

	return nstr;
}
