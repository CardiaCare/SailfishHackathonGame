/**
 * @file list.c
 * @Author Alaxandr A. Lomov <lomov@cs.karelia.ru>
 * @date   20 December, 2009
 * @brief  Operations and functions for list.
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2009 - Alexander A. Lomov. All rights reserved.
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
 * @section DESCRIPTION
 *
 * Contains functions and operations for for list.
 * This file is part of PetrSU KP library.
 * Copyright (C) 2009 - Alexandr A. Lomov.
 *
 * Some code of this file is from Linux Kernel (include/linux/list.h)
 * and modified by simply removing hardware prefetching of list items.
 * Here by copyright, credits attributed to wherever they belong.
 * Kulesh Shanmugasundaram (kulesh [squiggly] isis.poly.edu)
 */

#include "list.h"
#include <stdlib.h>
#include <stdio.h>

static void __list_add(struct list_head *new_entry, struct list_head *prev,
        struct list_head *next);
static void __list_del(struct list_head *prev, struct list_head *next);
static void __list_splice(struct list_head *list, struct list_head *head);



/******************************************************************************/
/**************************** External functions ******************************/
/// @cond EXTERNAL_FUNCTIONS

/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
SSLOG_EXTERN void list_add(struct list_head *new_entry, struct list_head *head)
{
    __list_add(new_entry, head, head->next);
}

/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
SSLOG_EXTERN void list_add_tail(struct list_head *new_entry, struct list_head *head)
{
    __list_add(new_entry, head->prev, head);
}

/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty on entry does not return true after this, the entry is in an undefined state.
 */
SSLOG_EXTERN void list_del(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
    entry->next = NULL;
    entry->prev = NULL;
}

/**
 * list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
SSLOG_EXTERN void list_del_init(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
    INIT_LIST_HEAD(entry);
}

/**
 * list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
SSLOG_EXTERN void list_move(struct list_head *list, struct list_head *head)
{
    __list_del(list->prev, list->next);
    list_add(list, head);
}

/**
 * list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
SSLOG_EXTERN void list_move_tail(struct list_head *list, struct list_head *head)
{
    __list_del(list->prev, list->next);
    list_add_tail(list, head);
}

/**
 * list_splice - join two lists
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
SSLOG_EXTERN void list_splice(struct list_head *list, struct list_head *head)
{
    if (!list_empty(list))
        __list_splice(list, head);
}

/**
 * list_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
SSLOG_EXTERN void list_splice_init(struct list_head *list, struct list_head *head)
{
    if (!list_empty(list)) {
        __list_splice(list, head);
        INIT_LIST_HEAD(list);
    }
}

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
SSLOG_EXTERN int list_empty(struct list_head *head)
{
    if (head == NULL) return 0;
    return head->next == head;
}
/// @endcond


/**************************** New added functions *****************************/
/*** This functions work with list_t struct, see list.h for the information ***/
/// @cond EXTERNAL_FUNCTIONS

/**
 * @brief Create new list node with given data.
 *
 * @param data for storage in the node.
 *
 * @return new list element on success or NULL otherwise.
 */
SSLOG_EXTERN list_t* list_get_new_node(void* data)
{
    list_t *node = (list_t *) malloc(sizeof (list_t));

    if (node == NULL) {
        return NULL;
    }

    node->data = data;
    INIT_LIST_HEAD(&node->links);

    return node;
}

/**
 * @brief Gets first previous element from list.
 *
 * @param list list of elements.
 *
 * @return list element.
 */
SSLOG_EXTERN list_t* list_get_first_prev_node(list_t *list)
{
    if (list == NULL) {
        return NULL;
    }

    if (list_empty(&list->links) == 1) {
        return NULL;
    }

    if (list->links.prev == NULL) {
        return NULL;
    }

    return list_entry(list->links.prev, list_t, links);
}


/**
 * @brief Gets first next element from list.
 *
 * @param list list of elements.
 *
 * @return list element.
 */
SSLOG_EXTERN list_t* list_get_first_next_node(list_t *list)
{
    if (list == NULL) {
        return NULL;
    }

    if (list_empty(&list->links) == 1) {
        return NULL;
    }

    if (list->links.next == NULL) {
        return NULL;
    }

    return list_entry(list->links.next, list_t, links);
}



/**
 * @brief Gets first previous element from list.
 *
 * @param list list of elements.
 *
 * @return list element.
 */
SSLOG_EXTERN list_t* list_get_prev_node(list_t *list)
{
    if (list == NULL) {
        return NULL;
    }

    if (list_empty(&list->links) == 1) {
        return NULL;
    }

    if (list->links.prev == NULL) {
        return NULL;
    }

    return list_entry(list->links.prev, list_t, links);
}

/**
 * @brief Gets next node for given node.
 *
 * Gets next node, if previous_node equals NULL or it is last element in the list,
 * the function gets firs element.
 *
 * @param list list.
 * @param previous_node node for get next node in the list or NULL.
 * @param out_data if not NULL, it will be equals a data field of new (next) node.
 * It will be NULL if list equals NULL or empty.
 *
 * @return node after previous_node or NULL if list equals NULL or empty.
 */
SSLOG_EXTERN list_t *list_get_next_node(list_t *list, list_t *previous_node, void **out_data)
{
    if (list_is_null_or_empty(list) == 1) {
        *out_data = NULL;
        return NULL;
    }

    // Check: if node is last
    if (previous_node != NULL) {
        if (list_is_last_node(list, previous_node) != 0) {
            previous_node = NULL;
        }
    }

    list_t *node = NULL;
    // First node
    if (previous_node == NULL) {
        node = list_entry(list->links.next, list_t, links);
    } else {
        node = list_entry(previous_node->links.next, list_t, links);
    }

    if (out_data != NULL) {
        *out_data = node->data;
    }

    return node;
}

/**
 * @brief Create and init list, it's node with null data.
 *
 * @return new created list, or NULL of there is no memory.
 */
SSLOG_EXTERN list_t* list_new()
{
    return list_get_new_node(NULL);
}

/**
 * @brief Create and init list, it is node with null data.
 *
 * Create new list if given equals NULL, otherwise return given list.
 *
 * @param list list for check.
 *
 * @return new created list or given list if it not NULL.
 */
SSLOG_EXTERN list_t* list_get_new_list_if_null(list_t *list)
{
    if (list != NULL) {
        return list;
    }

    return list_new();
}

/**
 * @brief Free list, without nodes. Nodes not freed.
 *
 * @param list list.
 */
SSLOG_EXTERN void list_free(list_t *list)
{
    if (list == NULL) {
        return;
    }

    list->data = NULL;
    list->links.next = NULL;
    list->links.prev = NULL;
    free(list);
}

/**
 * @brief Free list.
 *
 * Free list only if list is empty, no nodes.
 *
 * @param list list.
 *
 * @return list or NULL if it was freed.
 */
SSLOG_EXTERN list_t* list_free_empty(list_t *list)
{
    if (list == NULL) {
        return NULL;
    }

    if (list_empty(&list->links) != 1) {
        return list;
    }

    list_free(list);

    return NULL;
}

/**
 * @brief Free node.
 *
 * @param node node for free.
 * @param free_data_func reference for node data free function or NULL.
 */
SSLOG_EXTERN void list_free_node(list_t *node, void (*free_data_func)(void *data))
{
    if (node == NULL) {
        return;
    }

    if (free_data_func != NULL && node->data != NULL) {
        free_data_func(node->data);
    }

    node->data = NULL;
    node->links.next = NULL;
    node->links.prev = NULL;
    free(node);
}

/**
 * @brief Free list and all nodes.
 *
 * @param list list for free.
 * @param free_data_func reference for node data free function or NULL.
 */
SSLOG_EXTERN void list_free_with_nodes(list_t *list, void (*free_data_func)(void *data))
{
    if (list == NULL) {
        return;
    }

    list_head_t *list_walker = NULL;
    list_head_t *cur_pos = NULL;

    list_for_each_safe(list_walker, cur_pos, &list->links)
    {
        list_t *node = list_entry(list_walker, list_t, links);
        list_del(list_walker);
        list_free_node(node, free_data_func);
    }

    list_free(list);
}

/**
 * @brief Free nodes with give data.
 *
 * If node has a data field with a reference equals given, it will be removed.
 * This function also delete data of the founded node if even one node is founded,
 * If you do not delete data, than pass NULL as parameter of the free function.
 *
 * @param list list for check.
 * @param ref_to_data reference to data or NULL, all nodes with data field equals NULL will be removed.
 * @param free_data_func reference for node data free function or NULL.
 */
SSLOG_EXTERN void list_del_and_free_nodes_with_data(list_t *list, void *ref_to_data, void (*free_data_func)(void *data))
{
    if (list == NULL) {
        return;
    }

    // Flag: set to 1 if nodes are founded.
    int is_found = 0;

    // Find nodes with given data and delete them.
    list_head_t *list_walker = NULL;
    list_head_t *safe_pos = NULL;

    list_for_each_safe(list_walker, safe_pos, &list->links)
    {
        list_t *node = list_entry(list_walker, list_t, links);

        if (node->data == ref_to_data) {
            list_del_and_free_node(node, free_data_func);
            is_found = 1;
        }
    }

    // Free data if nodes were founded and we have function to delete.
    if (is_found == 1 && free_data_func != NULL) {
        free_data_func(ref_to_data);
    }

}

/**
 * @brief Delete and free all nodes in the list, it doesn't free the list.
 *
 * This function also can delete data in the list nodes if you pass free function.
 * If you do not want to delete data, than pass NULL as parameter of the free function.
 *
 * @param list list for check.
 * @param free_data_func reference for node data free function or NULL.
 */
SSLOG_EXTERN void list_del_and_free_nodes(list_t *list, void (*free_data_func)(void *data))
{
    if (list == NULL) {
        return;
    }

    // Find nodes with given data and delete them.
    list_head_t *list_walker = NULL;
    list_head_t *safe_pos = NULL;

    list_for_each_safe(list_walker, safe_pos, &list->links)
    {
        list_t *node = list_entry(list_walker, list_t, links);

        list_del_and_free_node(node, free_data_func);
    }
}

/**
 * @brief Removes node from list and free it.
 *
 * @param node node for remove and free.
 * @param free_data_func reference for node data free function or NULL.
 */
SSLOG_EXTERN void list_del_and_free_node(list_t *node, void (*free_data_func)(void *data))
{
    list_del(&node->links);
    list_free_node(node, free_data_func);
}

/**
 * @brief Add a new entry.
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 *
 * @param new_node entry to be added.
 * @param list list head to add it after.
 */
SSLOG_EXTERN void list_add_node(list_t *new_node, list_t *list)
{
    list_add(&new_node->links, &list->links);
}

SSLOG_EXTERN void list_add_data(list_t *list, void *data)
{
    if (list == NULL) {
        return;
    }

    list_t *node = list_get_new_node(data);
    list_add_node(node, list);
}

/**
 * @brief Add one list to another.
 *
 * It not delete a list_t struct of added list, you need to remove it by youself,
 * don't delete nodes (use @see list_free()) or you can reinit list and use it as new empty list.
 *
 * @param list the new list to add.
 * @param node the place to add it in the first list.
 */
SSLOG_EXTERN void list_add_list(list_t *list, list_t *node)
{
    if (list == NULL || node == NULL) {
        return;
    }

    if (!list_empty(&list->links))
        __list_splice(&list->links, &node->links);
}

/**
 * @brief Create new list from another.
 *
 * The function creates a new list and sets node's data field equals given list's nodes data.
 * New memory is not allocating for data fields, do not free data fields in both lists.
 *
 * @param list list for copy.
 *
 * @return new list on success or NULL otherwise.
 */
SSLOG_EXTERN list_t* list_copy_list(list_t *list)
{
    if (list == NULL) {
        return NULL;
    }

    list_t *new_list = list_new();

    if (new_list == NULL) {
        return NULL;
    }

    list_head_t *list_walker = NULL;

    list_for_each_prev(list_walker, &list->links)
    {
        list_t *node = list_entry(list_walker, list_t, links);
        list_add_data(new_list, node->data);
    }
    return new_list;
}

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
 * @param[in] list list for copy.
 * @param[in] filter_data extra data for the filter function.
 * @param[in] filter_func function to determine which data will be in the new list.
 *
 * @return new list on success or NULL otherwise.
 */
SSLOG_EXTERN list_t* list_copy_list_with_filter(list_t *list, 
    int (*filter_func)(void* data, void *filter_data), void *filter_data)
{
    if (list == NULL) {
        return NULL;
    }

    if (filter_func == NULL) {
        return list_copy_list(list);
    }

    list_t *new_list = list_new();

    if (new_list == NULL) {
        return NULL;
    }

    list_head_t *list_walker = NULL;

    list_for_each(list_walker, &list->links)
    {
        list_t *node = list_entry(list_walker, list_t, links);

        if(filter_func(node->data, filter_data) != 0) {
            list_add_data((list_t *) node->data, new_list);
        }
    }
    return new_list;
}

/**
 * @brief Tests whether a list is empty.
 *
 * @param list the list to test.
 *
 * @return 1 if list is empty, or 0 otherwise.
 */
SSLOG_EXTERN int list_is_empty(list_t *list)
{
    return list_empty(&list->links);
}

/**
 * @brief Tests whether a list is empty or equals NULL.
 *
 * @param list the list to test.
 *
 * @return 1 if list is empty or 0 otherwise.
 */
SSLOG_EXTERN int list_is_null_or_empty(list_t *list)
{
    if (list == NULL) {
        return 1;
    }

    return list_empty(&list->links);
}

/**
 * @brief Tests whether a node is last in the list.
 *
 * @param list the list to test.
 * @param node node to test.
 *
 * @return -1 on error (list or node equals NULL), 0 - node is not last, 1 - last.
 */
SSLOG_EXTERN int list_is_last_node(list_t *list, list_t *node)
{
    if (list == NULL || node == NULL) {
        return -1;
    }

    return (node->links.next == &list->links) ? 1 : 0;
}

/**
 * @brief Checks: is there given data in the list.
 *
 * @param list list for checking.
 * @param ref_to_data reference to data.
 *
 * @return 1 if data in the list or 0 otherwise.
 */
SSLOG_EXTERN int list_has_data(list_t *list, void *ref_to_data)
{
    if (list == NULL) {
        return 0;
    }

    list_head_t *list_walker = NULL;

    list_for_each(list_walker, &list->links)
    {
        list_t *node = list_entry(list_walker, list_t, links);

        if (node->data == ref_to_data) {
            return 1;
        }
    }

    return 0;
}

/**
 * @brief Checks: is there given data in the list.
 *
 * This function uses equals function to determine equality between data 
 * in the nodes and the given data.
 *
 * @param list list for checking.
 * @param ref_to_data reference to data.
 * @param equal_func function to determine equality.
 *
 * @return true if data is in the list or false otherwise.
 */
SSLOG_EXTERN bool list_has_data_with_equals(list_t *list, const void *ref_to_data, bool equal_func(const void *, const void *))
{
    if (list == NULL || equal_func == NULL) {
        return false;
    }

    list_head_t *list_walker = NULL;

    list_for_each(list_walker, &list->links)
    {
        list_t *node = list_entry(list_walker, list_t, links);

        if (equal_func(node->data, ref_to_data) == true) {
            return true;
        }
    }

    return false;
}

/**
 * @brief Gets a node with given data using equal function.
 * This function uses equals function to determine equality between data
 * in the nodes and the given data. Then given function returns not null, then
 * it means that data is found. If the equal_func is NULL then the function
 * uses '==' operator to check equality.
 *
 * @param list list for checking.
 * @param ref_to_data reference to data.
 * @param equal_func function to determine equality.
 *
 * @return node if data is in the list or NULL otherwise.
 */
SSLOG_EXTERN list_t* list_get_node_with_equals(list_t *list, const void *ref_to_data, bool equal_func(const void *, const void *))
{
    if (list == NULL || equal_func == NULL) {
        return false;
    }

    list_head_t *list_walker = NULL;

    list_for_each(list_walker, &list->links)
    {
        list_t *node = list_entry(list_walker, list_t, links);

        if (equal_func(node->data, ref_to_data) == true) {
            return node;
        }
    }

    return NULL;
}


/**
 * @brief Count nodes in the list.
 *
 * @param list list to count.
 *
 * @return quantity of nodes, if list is NULL, then it returns 0.
 */
SSLOG_EXTERN int list_count(list_t *list)
{
    if (list == NULL) {
        return 0;
    }

    int counter = 0;

    list_head_t *list_walker = NULL;

    list_for_each(list_walker, &list->links) {
            ++counter;
    }

    return counter;
}

/**
 * @brief Count nodes with given reference to data.
 *
 * @param list list for check.
 * @param ref_to_data reference to data.
 *
 * @return quantity of nodes.
 */
SSLOG_EXTERN int list_count_nodes_with_data(list_t *list, void *ref_to_data)
{
    if (list == NULL) {
        return 0;
    }

    int counter = 0;

    list_head_t *list_walker = NULL;

    list_for_each(list_walker, &list->links)
    {
        list_t *node = list_entry(list_walker, list_t, links);

        if (node->data == ref_to_data) {
            ++counter;
        }
    }

    return counter;
}


/// @endcond


/******************************************************************************/
/***************************** Static functions *******************************/

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static void __list_add(struct list_head *new_entry,
        struct list_head *prev,
        struct list_head *next)
{
    next->prev = new_entry;
    new_entry->next = next;
    new_entry->prev = prev;
    prev->next = new_entry;
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static void __list_del(struct list_head *prev, struct list_head *next)
{
    next->prev = prev;
    prev->next = next;
}

static void __list_splice(struct list_head *list, struct list_head *head)
{
    struct list_head *first = list->next;
    struct list_head *last = list->prev;
    struct list_head *at = head->next;

    first->prev = head;
    head->next = first;

    last->next = at;
    at->prev = last;
}


