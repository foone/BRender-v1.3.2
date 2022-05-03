/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: resreg.c 1.1 1997/12/10 16:41:10 jon Exp $
 * $Locker: $
 *
 * Resource class management
 */
#include "fw.h"
#include "brassert.h"

BR_RCS_ID("$Id: resreg.c 1.1 1997/12/10 16:41:10 jon Exp $")

/*
 * Resource Classes
 */
br_resource_class * BR_PUBLIC_ENTRY BrResClassAdd(br_resource_class *rclass)
{
	br_resource_class *r;

	UASSERT(rclass != NULL);

	/*
	 * The registry resource is initally faked
	 */
	UASSERT(rclass->res_class == BR_MEMORY_REGISTRY ||
			rclass->res_class == BR_MEMORY_ANCHOR ||
			fw.resource_class_index[rclass->res_class] == NULL);

	r = BrRegistryAdd(&fw.reg_resource_classes,rclass);

	if(r != NULL)
		fw.resource_class_index[rclass->res_class] = r;

	return r; r;
}

br_resource_class * BR_PUBLIC_ENTRY BrResClassRemove(br_resource_class *rclass)
{
	br_resource_class *r;

	UASSERT(rclass != NULL);
	UASSERT(fw.resource_class_index[rclass->res_class] != NULL);

	r = BrRegistryRemove(&fw.reg_resource_classes,rclass);

	if(r != NULL)
		fw.resource_class_index[rclass->res_class] = NULL;

	return r;
}

br_resource_class * BR_PUBLIC_ENTRY BrResClassFind(char *pattern)
{
	return BrRegistryFind(&fw.reg_resource_classes,pattern);
}

br_resclass_find_cbfn * BR_PUBLIC_ENTRY BrResClassFindHook(br_resclass_find_cbfn *hook)
{
	br_resclass_find_cbfn * old =
		(br_resclass_find_cbfn *) fw.reg_resource_classes.find_failed_hook;

	fw.reg_resource_classes.find_failed_hook = (br_find_failed_cbfn *)hook;

	return old;
}

br_uint_32 BR_PUBLIC_ENTRY BrResClassAddMany(br_resource_class **items, int n)
{
	int i;

	for(i=0; i < n; i++)
		BrResClassAdd(*items++);

	return n;
}

br_uint_32 BR_PUBLIC_ENTRY BrResClassRemoveMany(br_resource_class **items, int n)
{
	int i,r;

	for(i=0, r=0; i < n; i++)
		if(BrResClassRemove(*items++))
			r++;

	return r;
}

br_uint_32 BR_PUBLIC_ENTRY BrResClassFindMany(char *pattern, br_resource_class **items, int max)
{
	return BrRegistryFindMany(&fw.reg_resource_classes, pattern, (void **)items, max);
}

br_uint_32 BR_PUBLIC_ENTRY BrResClassCount(char *pattern)
{
	return BrRegistryCount(&fw.reg_resource_classes,pattern);
}

br_uint_32 BR_PUBLIC_ENTRY BrResClassEnum(char *pattern,
		br_resclass_enum_cbfn *callback, void *arg)
{
	return BrRegistryEnum(&fw.reg_resource_classes,pattern,(br_enum_cbfn *)callback,arg);
}




