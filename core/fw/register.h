/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: register.h 1.1 1997/12/10 16:41:10 jon Exp $
 * $Locker: $
 *
 * 
 */

#ifndef _REGISTER_H_
#define _REGISTER_H_

/*
 * An entry in a registry - doubly linked list and pointers to data
 *
 * It is assumed that the second first thing in the data structure is a pointer
 * to the item's name
 */
typedef struct br_registry_entry {
		struct br_node node;
		struct {
			br_uint_32 reserved;
			char *identifier;
		} *item;
} br_registry_entry;

typedef void * BR_CALLBACK br_find_failed_cbfn(char *pattern);
typedef br_uint_32 BR_CALLBACK br_enum_cbfn (void *item, void *arg);

/*
 * Base structure for registry
 */
typedef struct br_registery {
		/*
		 * Anchor structure
		 */
		struct br_list list;

		/*
		 * Number of items in list
		 */
		int count;

		/*
		 * Hook that is called when RegistryFind fails
		 */
		br_find_failed_cbfn *find_failed_hook;

} br_registry;

#endif
