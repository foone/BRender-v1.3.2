/*
 * Copyright (c) 1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: lists.h 1.1 1997/12/10 16:58:52 jon Exp $
 * $Locker: $
 *
 */
#ifndef _LISTS_H_
#define _LISTS_H_


typedef struct t_list_node {

	struct t_list_node *next;
	struct t_list_node *prev;
    
} t_list_node;

typedef struct t_list {

	t_list_node *head;
	t_list_node *tail;
    
} t_list;

t_list *T_NewList(t_list *list);
t_list *T_AddHead(t_list *list, void *add_node);
t_list *T_AddTail(t_list *list, void *add_node);
void *T_RemoveHead(t_list *list);
void *T_RemoveTail(t_list *list);


#endif
