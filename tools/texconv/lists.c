/*
 * Copyright (c) 1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: lists.c 1.1 1997/12/10 16:58:51 jon Exp $
 * $Locker: $
 *
 */

#include <stddef.h>
#include <stdio.h>

#include "brender.h"
#include "lists.h"

/*
 * setup new list structure
 */
t_list *T_NewList(t_list *list)
{
	list->head = NULL;
	list->tail = NULL;

	return list;
}

/*
 * add node to head of list
 */
t_list *T_AddHead(t_list *list, t_list_node *add_node)
{
	t_list_node *node = (t_list_node *)add_node;
	
	node->prev = NULL;
	node->next = list->head;

	/*
	 * trap case of empty list
	 */
	if(list->head == NULL)
		list->tail = node;

	list->head = node;

	return list;
}

/*
 * add node to tail of list
 */
t_list *T_AddTail(t_list *list, void *add_node)
{
	t_list_node *node = (t_list_node *)add_node;
    
	node->prev = list->tail;
	node->next = NULL;

	/*
	 * trap case of empty list
	 */
	if(list->head == NULL)
		list->head = node;
	else
		list->tail->next = node;

	list->tail = node;
    
	return list;    
}

/*
 * remove node from head of list
 */
void *T_RemoveHead(t_list *list)
{
	t_list_node *node;

	node = list->head;

	/*
	 * trap case of empty list
	 */
	if(list->head != NULL) {

		list->head = list->head->next;
		
		if(list->head == NULL)
			list->tail = NULL;
		else 
			list->head->prev = NULL;
	}

	return node;    
}

/*
 * remove node from tail of list
 */
void *T_RemoveTail(t_list *list)
{
	t_list_node *node;

	node = list->tail;

	/*
	 * trap case of empty list
	 */
	if(list->head != NULL) {

		list->tail = list->tail->prev;

		if(list->tail == NULL)
			list->head = NULL;
		else
			list->tail->next = NULL;
	}

	return node;
}
