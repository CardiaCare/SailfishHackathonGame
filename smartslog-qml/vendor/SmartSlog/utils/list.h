/**
 * @file list.h
 * @Author Alaxandr A. Lomov <lomov@cs.karelia.ru>
 * @date   20 December, 2009
 * @brief  Interface for operations with list.
 *
 *
 * @section LICENSE
 *
 * PetrSU KP library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * PetrSU KP library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PetrSU KP library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 *
 *
 * @section DESCRIPTION
 *
 * Interfaces for list.
 * This file is part of PetrSU KP library.
 * Copyright (C) 2009 - Alexandr A. Lomov.
 *
 * Some code of this file is from Linux Kernel (include/linux/list.h)
 * and modified by simply removing hardware prefetching of list items.
 * Here by copyright, credits attributed to wherever they belong.
 * Kulesh Shanmugasundaram (kulesh [squiggly] isis.poly.edu)
 */

#include "bool.h"

#ifndef __LIST_H
#define __LIST_H

#if !defined(SSLOG_EXTERN)
#if defined(SSLOG_EXPORT)
#define SSLOG_EXTERN __declspec(dllexport)
#elif defined (SSLOG_IMPORT)
#define SSLOG_EXTERN __declspec(dllimport)
#else
#define SSLOG_EXTERN
#endif
#endif

#ifdef	__cplusplus
extern "C" {
#endif

/// @cond EXTAERNAL_STRUCTURES
struct list_head {
	struct list_head *next, *prev;
};

typedef struct list_head list_head_t;

typedef struct list_s {
    void *data;
    list_head_t links;
} list_t;
/// @endcond


/// @cond EXTERNAL_FUNCTIONS
#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

#define INIT_LIST_HEAD(ptr) do { \
	(ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)


#define LIST_CAST_TO_EQUAL_FUNC  (int (*)(const void *, const void *))
#define LIST_CAST_TO_EQUAL_BOOL_FUNC  (bool (*)(const void *, const void *))
#define LIST_CAST_TO_FREE_FUNC  (void (*)(void *))


/**
 * list_entry - get the struct for this entry
 * @ptr:	the &struct list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 */
#define list_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

/**
 * list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop counter.
 * @head:	the head for your list.
 */
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); \
        	pos = pos->next)
/**
 * list_for_each_prev	-	iterate over a list backwards
 * @pos:	the &struct list_head to use as a loop counter.
 * @head:	the head for your list.
 */
#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); \
        	pos = pos->prev)

/**
 * list_for_each_safe	-	iterate over a list safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop counter.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)



/**
 * list_for_each_entry	-	iterate over list of given type
 * @pos:	the type * to use as a loop counter.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
/* typeof - non c99 standart.
#define list_for_each_entry(pos, head, member)				\
	for (pos = list_entry((head)->next, typeof(*pos), member);	\
	     &pos->member != (head); 					\
	     pos = list_entry(pos->member.next, typeof(*pos), member))
*/

/**
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop counter.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
/* typeof - non c99 standart.
#define list_for_each_entry_safe(pos, n, head, member)			\
	for (pos = list_entry((head)->next, typeof(*pos), member),	\
		n = list_entry(pos->member.next, typeof(*pos), member);	\
	     &pos->member != (head); 					\
	     pos = n, n = list_entry(n->member.next, typeof(*n), member))
*/


SSLOG_EXTERN void list_add(struct list_head *new_element, struct list_head *head);
SSLOG_EXTERN void list_add_tail(struct list_head *new_element, struct list_head *head);
SSLOG_EXTERN void list_del(struct list_head *entry);
SSLOG_EXTERN void list_del_init(struct list_head *entry);
SSLOG_EXTERN void list_move(struct list_head *list, struct list_head *head);
SSLOG_EXTERN void list_move_tail(struct list_head *list, struct list_head *head);
SSLOG_EXTERN void list_splice(struct list_head *list, struct list_head *head);
SSLOG_EXTERN void list_splice_init(struct list_head *list, struct list_head *head);
SSLOG_EXTERN int list_empty(struct list_head *head);
/// @endcond



/**************************** New added functions *****************************/
/**************** This functions work with list_t struct **********************/
/// @cond EXTERNAL_FUNCTIONS
SSLOG_EXTERN list_t* list_get_new_node(void* data);
SSLOG_EXTERN list_t* list_get_first_prev_node(list_t *list);
SSLOG_EXTERN list_t* list_get_first_next_node(list_t *list);
SSLOG_EXTERN list_t *list_get_next_node(list_t *list, list_t *previous_node, void **out_data);
SSLOG_EXTERN list_t* list_new();
SSLOG_EXTERN list_t* list_get_new_list_if_null(list_t *list);

SSLOG_EXTERN void list_free(list_t *list);
SSLOG_EXTERN list_t* list_free_empty(list_t *list);
SSLOG_EXTERN void list_free_node(list_t *node, void (*free_data_func)(void *data));
SSLOG_EXTERN void list_free_with_nodes(list_t *list, void (*free_data_func)(void *data));
SSLOG_EXTERN void list_del_and_free_nodes(list_t *list, void (*free_data_func)(void *data));
SSLOG_EXTERN void list_del_and_free_nodes_with_data(list_t *list, void *ref_to_data, 
        void (*free_data_func)(void *data));
SSLOG_EXTERN void list_del_and_free_node(list_t *node, void (*free_data_func)(void *data));

SSLOG_EXTERN void list_add_node(list_t *new_node, list_t *list);

/**
 * @brief Add a new entry.
 *
 * Create new node with given data and add it to list.
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 *
 * @param data data for new node.
 * @param list list head to add it after.
 */
SSLOG_EXTERN void list_add_data(list_t *list, void *data);

SSLOG_EXTERN void list_add_list(list_t *list, list_t *node);
SSLOG_EXTERN list_t* list_copy_list(list_t *list);



/**
 * @brief Create new list from another using a filter function to determine,
 * which data will be in the new list.
 *
 * The function doesn't copy data, it only creates new list with new nodes.
 * You need to free data only in one list.
 *
 * The filter function is used to determine nodes'data which will be inserted to
 * the new list. If the function returns 0, then the current node is ignored,
 * if it return not null, then new node with current data is created and
 * inserted to the new list.
 *
 * If the filter function is passed as NULL, then this function works as
 * #list_copy_list.
 *
 * @param list list for copy.
 * @param filter_func function to determine which data will be in the new list.
 *
 * @return new list on success or NULL otherwise.
 */
SSLOG_EXTERN list_t* list_copy_list_with_filter(list_t *list,
    int (*filter_func)(void* data, void *filter_data), void *filter_data);

SSLOG_EXTERN int list_is_empty(list_t* list);
SSLOG_EXTERN int list_is_null_or_empty(list_t* list);
SSLOG_EXTERN int list_is_last_node(list_t *list, list_t *node);

SSLOG_EXTERN int list_has_data(list_t *list, void *ref_to_data);
SSLOG_EXTERN bool list_has_data_with_equals(list_t *list, const void *ref_to_data, bool equal_func(const void *, const void *));
SSLOG_EXTERN list_t* list_get_node_with_equals(list_t *list, const void *ref_to_data, bool equal_func(const void *, const void *));
SSLOG_EXTERN int list_count(list_t *list);
SSLOG_EXTERN int list_count_nodes_with_data(list_t *list, void *ref_to_data);

/// @endcond

#ifdef	__cplusplus
}
#endif

#endif


