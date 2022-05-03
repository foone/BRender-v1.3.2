/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: register.c 1.1 1997/12/10 16:41:09 jon Exp $
 * $Locker: $
 *
 * Generic code for lists of registered items. Used for models, materials,
 * textures, tables, cameras, lights, scenes
 *
 * Each registry is a doubly linked list of chunks of data, the second
 * member of an item should be a pointer the the item's name
 */
#include "fw.h"
#include "brassert.h"

BR_RCS_ID("$Id: register.c 1.1 1997/12/10 16:41:09 jon Exp $")

/*
 * Initialise a registry
 */
void *BrRegistryNew(br_registry *reg)
{
	UASSERT(reg != NULL);

	/*
	 * Intitialise linked list
	 */
	BrNewList(&reg->list);
	
	/*
	 * List is empty
	 */
	reg->count = 0;

	return reg;
}

/*
 * Release all items on a registry
 */
void *BrRegistryClear(br_registry *reg)
{
	br_registry_entry *e;

	UASSERT(reg != NULL);

	while(e = BR_HEAD(&reg->list), BR_NEXT(e)) {
		BR_REMOVE(e);
		BrResFree(e);
	}

	/*
	 * List is empty
	 */
	reg->count = 0;

	return reg;
}

/*
 * Add one new item to a registry
 */
void *BrRegistryAdd(br_registry *reg, void *item)
{
	br_registry_entry *e;

	UASSERT(reg != NULL);
	UASSERT(item != NULL);

	e = BrResAllocate(fw.res,sizeof(*e),BR_MEMORY_REGISTRY);
	e->item = item;
	BR_ADDHEAD(reg,e);

	reg->count++;

	return item;
}

/*
 * Add items from a table of pointers to a registry
 */
int BrRegistryAddMany(br_registry *reg, void **items, int n)
{
	int i;
	UASSERT(reg != NULL);
	UASSERT(items != NULL);

	/*
	 * Walk through table adding items
	 */
	for(i=0; i < n; i++)
		BrRegistryAdd(reg,*items++);

	return n;
}

/*
 * Remove a single item from the registry
 */
void *BrRegistryRemove(br_registry *reg, void *item)
{
	br_registry_entry *e;
	void *r;

	UASSERT(reg != NULL);
	UASSERT(item != NULL);

	/*
	 * Find item in list
	 */
	BR_FOR_LIST(&reg->list,e)
		if(e->item == item)
			break;

	/*
	 * If item was not in list, return NULL
	 */
	if(!BR_NEXT(e))
		return NULL;

	/*
	 * Take item of list, remember contents, and free node
	 */
	BR_REMOVE(e);
	r = e->item;
	BrResFree(e);

	reg->count--;

	return r;
}

/*
 * Remove a table of referenced items from a registry
 */
int BrRegistryRemoveMany(br_registry *reg, void **items, int n)
{
	int i,r;

	UASSERT(reg != NULL);
	UASSERT(items != NULL);

	/*
	 * Remove all the items from a table, keeping a count of how
	 * many were actually removed
	 */
	for(i=0, r=0; i < n; i++)
		if(BrRegistryRemove(reg,*items++))
			r++;

	return r;
}

/*
 * Find the first item in registry whose name matches a given pattern
 *
 * If no item can be found, then call a find_failed hook if it exists
 */
void *BrRegistryFind(br_registry *reg, char *pattern)
{
	br_registry_entry *e;

	UASSERT(reg != NULL);

	/*
	 * Find item in list
	 */
	BR_FOR_LIST(&reg->list,e)
		if(BrNamePatternMatch(pattern,e->item->identifier))
			return e->item;

	if(reg->find_failed_hook)
		return reg->find_failed_hook(pattern);
	else
		return NULL;
}

int BrRegistryFindMany(br_registry *reg, char *pattern, void **items, int max)
{
	br_registry_entry *e;
	int n=0;

	/*
	 * Find all matching items in list
	 */
	BR_FOR_LIST(&reg->list,e) {
		/*
		 * make sure there is space in output table
		 */
		if(n >= max)
			break;

		/*
		 * If entry matches, add to table
		 */
		if(BrNamePatternMatch(pattern,e->item->identifier)) {
			*items++ = e->item;
			n++;
		}
	}

	return n;
}

/*
 * Count how many items in registry match pattern
 *
 * If pattern == NULL, return total in registry
 */
int BrRegistryCount(br_registry *reg, char *pattern)
{
	br_registry_entry *e;
	int n;

	UASSERT(reg != NULL);

	if(pattern == NULL) 
		return reg->count;

	/*
	 * Find all matching items in list
	 */
	n = 0;

	BR_FOR_LIST(&reg->list,e)
		if(BrNamePatternMatch(pattern,e->item->identifier))
			n++;

	return n;
}

/*
 * Call a function for every item in a registry. Stop early if callback
 * returns !=0, and return that value
 */
int BrRegistryEnum(br_registry *reg, char *pattern,
		br_enum_cbfn *callback, void *arg)
{
	br_registry_entry *e;
	int r;

	UASSERT(reg != NULL);
	UASSERT(callback != NULL);

	/*
	 * If pattern in NULL, invoke callback for _EVERYTHING_
	 * else invoke callback for items that match pattern
	 */
	if(pattern == NULL) {
		BR_FOR_LIST_R(&reg->list,e)
			if(r = callback(e->item,arg))
				return r;
	} else {
		BR_FOR_LIST_R(&reg->list,e)
			if(BrNamePatternMatch(pattern,e->item->identifier))
				if(r = callback(e->item,arg))
					return r;
	}
	return 0;
}

/**
 ** Variants for statically allocated registries
 **/
#if EVAL
/*
 * Initialise a registry
 */
void *BrRegistryNewStatic(br_registry *reg, br_registry_entry *base, int limit)
{
	int i;

	UASSERT(reg != NULL);
	UASSERT(base != NULL);

	/*
	 * Intitialise linked list
	 */
	BrNewList(&reg->list);

	/*
	 * Clear static entries
	 */
	for(i = 0; i < limit; i++)
		base[i].node.next = NULL;

	/*
	 * List is empty
	 */
	reg->count = 0;

	return reg;
}

/*
 * Add one new item to a registry - assumes at least one unused entry exists
 */
void *BrRegistryAddStatic(br_registry *reg, br_registry_entry *base, void *item)
{
	br_registry_entry *e;

	UASSERT(reg != NULL);
	UASSERT(base != NULL);
	UASSERT(item != NULL);

	for(e = base; e->node.next != NULL; e++);
	e->item = item;
	BR_ADDHEAD(reg, e);

	reg->count++;

	return item;
}

/*
 * Remove a single item from the registry
 */
void *BrRegistryRemoveStatic(br_registry *reg, void *item)
{
	br_registry_entry *e;
	void *r;

	UASSERT(reg != NULL);
	UASSERT(item != NULL);

	/*
	 * Find item in list
	 */
	BR_FOR_LIST(&reg->list,e)
		if(e->item == item)
			break;

	/*
	 * If item was not in list, return NULL
	 */
	if(!BR_NEXT(e))
		return NULL;

	/*
	 * Take item off list, remember contents, and free node
	 */
	BR_REMOVE(e);
	r = e->item;
	e->node.next = NULL;

	reg->count--;

	return r;
}

#else
void *BrRegistryNewStatic(br_registry *reg, br_registry_entry *base, int limit)
{
	return NULL;
}

void *BrRegistryAddStatic(br_registry *reg, br_registry_entry *base, void *item)
{
	return NULL;
}

void *BrRegistryRemoveStatic(br_registry *reg, void *item)
{
	return NULL;
}
#endif

