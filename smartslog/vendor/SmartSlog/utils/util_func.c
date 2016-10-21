/**
 * @file util_func.c
 * @Author Alaxandr A. Lomov <lomov@cs.karelia.ru>, Vanag Pavel <vanag@cs.karelia.ru>
 * @date   26 December, 2009
 * @brief  Useful functions.
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
 * along with <program name>; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 *
 *
 * @section DESCRIPTION
 *
 * Some small useful functions.
 * This file is part of PetrSU KP library.
 *
 * Copyright (C) 2009 - Alexander A. Lomov. All rights reserved.
 */

#ifndef _UTIL_FUNC_C_
#define _UTIL_FUNC_C_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

#include "list.h"
#include "util_func.h"

#include "../kpi_api.h"
#include "../entity_internal.h"
#include "../triple_internal.h"


#define SSLOG_URI_GENPART_LENGTH 5
#define SSLOG_NULL_BYTE_LENGTH 1

typedef struct memory_object_s {
    unsigned int count;
    void (*free_func)(void *data);
    void *data;
} memory_object_t;



/******************************** Definitions *********************************/
/***************************** Static functions *******************************/
static int string_to_sharp_length(char *);


/**
 * @brief Generate some hash ID.
 *
 * @return new ID on success or NULL otherwise.
 */
static char* sslog_generate_hash();


/******************************************************************************/
/**************************** External functions ******************************/
/** @cond EXTERNAL_FUNCTIONS */

/**
 *
 * @brief Duplicate string: allocates memory of defined size and copies string at most size characters.
 *        If string is longer than size, only size characters would be copied,
 *                                     terminating null byte ('\0') would be added.
 *        Internal function "strndup" not in ANSI C c99 standart, so SmartSlog has own implementation.
 *
 *
 * @param const char *str - string to copy
 * @param size_t len -  size of string to copy.
 *
 * @return returns duplicated string or NULL if error accures.
 */
SSLOG_EXTERN char *sslog_strndup(const char *str, size_t n)
{
    size_t len = strlen (str);

    if (n < len)
        len = n;

    char *result = (char *) malloc (len + 1);
    if (result == NULL) {
        return NULL;
    }

    result[len] = '\0';
    return (char *) memcpy (result, str, len);
}

/// @endcond



/******************************************************************************/
/**************************** Internal functions ******************************/
/// @cond INTERNAL_FUNCTIONS



/**
 * @brief Checks is property value in the list.
 *
 * @param prop_val_t alue to check.
 * @param list_t list with values.
 *
 * @return returns true if the item founded in the list and false otherwise.
 */
//bool is_in_property_values_list(prop_val_t *item, list_t *property_list)
//{
//    list_head_t *pos = NULL;

//    if ((item == NULL) || (property_list == NULL)) {
//        return false;
//    }

//    if (list_empty(&property_list->links)) {
//        return false;
//    }

//    list_for_each(pos, &property_list->links)
//    {
//        list_t *node = list_entry(pos, list_t, links);
//        prop_val_t *prop = (prop_val_t *) node->data;

//        if (is_prop_names_equal(item, prop)) {
//            return true;
//        }
//    }

//    return false;
//}


/// @endcond
#endif /* _UTIL_FUNC_C_ */


bool sslog_is_str_null_empty(const char *string)
{
    if (string == NULL) {
        return true;
    }

    if (string[0] == '\0') {
        return true;
    }

    return false;
}


bool sslog_equal_triples(sslog_triple_t *a, sslog_triple_t *b)
{
    if (a == b) {
        return true;
    }

    if (a == NULL) {
        return false;
    }

    if (b == NULL) {
        return false;
    }

    if ((a->subject_type != b->subject_type)
            || (a->object_type != b->object_type)) {
        return false;
    }

    if ((strcmp(a->subject, b->subject) != 0)
            || (strcmp(a->predicate, b->predicate) != 0)
            || (strcmp(a->object, b->object) != 0)) {
        return false;
    }

    return true;
}


bool sslog_compare_strings_with_any(const char *a, const char *b, const char *any_value)
{
    if (a == b) {
        return true;
    }

    if (a == NULL) {
        return false;
    }

    if (b == NULL) {
        return false;
    }

    if ((strcmp(a, any_value) == 0)
            || (strcmp(b, any_value) == 0)
            || (strcmp(a, b) == 0)) {
        return true;
    }

    return false;
}

bool sslog_compare_triple_with_any(sslog_triple_t *a, sslog_triple_t *b, const char *any_value)
{
    if (a == b) {
        return true;
    }

    if (a == NULL) {
        return false;
    }

    if (b == NULL) {
        return false;
    }

    if ((a->subject_type != b->subject_type)
            && (strcmp(a->subject, any_value) != 0)
            && (strcmp(b->subject, any_value) != 0)) {
        return false;
    }

    if ((a->object_type != b->object_type)
            && (strcmp(a->object, any_value) != 0)
            && (strcmp(b->object, any_value) != 0)) {
        return false;
    }

    if (sslog_compare_strings_with_any(a->subject, b->subject, any_value) == false
            || sslog_compare_strings_with_any(a->predicate, b->predicate, any_value) == false
            || sslog_compare_strings_with_any(a->object, b->object, any_value) == false ) {
        return false;
    }

//    if (((strcmp(a->subject, any_value) == 0) || (strcmp(a->subject, b->subject) == 0))
//            && ((strcmp(a->predicate, any_value) == 0) || (strcmp(a->predicate, b->predicate) == 0))
//            && ((strcmp(a->object, any_value) == 0) || (strcmp(a->object, b->object) == 0))) {
//        return true;
//    }

    return true;
}


void * sslog_alloc2(size_t size, void (*free_data_func)(void *data))
{
    memory_object_t *object = (memory_object_t *) calloc(sizeof(memory_object_t) + size, 1);
    char *ptr = (char *) object;
    ptr += sizeof(memory_object_t);
    object->count = 1;
    object->free_func = free_data_func;
    object->data = ptr;

    return (void *)ptr;
}


bool sslog_triple_is_stored(sslog_triple_t *triple)
{
//    char *cpointer  = (char *) pointer;
//    cpointer -= sizeof(sslog_object_t);
//    sslog_object_t *object = (sslog_object_t *) cpointer;

//    return object->is_detached;
      return ((sslog_internal_triple_t *) triple)->is_stored;
}

void sslog_triple_stored(sslog_triple_t *triple, bool is_stored)
{
//    char *cpointer  = (char *) pointer;
//    cpointer -= sizeof(sslog_object_t);

//    sslog_object_t *object = (sslog_object_t *) cpointer;
//    object->is_detached = is_detached;
    ((sslog_internal_triple_t *) triple)->is_stored = is_stored;

}


void *sslog_alloc(size_t size, void (*free_data_func)(void *data))
{
    sslog_object_t *object = (sslog_object_t *) calloc(sizeof(sslog_object_t) + size, 1);

    char *ptr = (char *) object;
    ptr += sizeof(sslog_object_t);

    object->is_detached = false;
    object->free_func = free_data_func;
    object->data = ptr;

    return (void *)ptr;
}




void sslog_free_force(void *pointer)
{
    char *cpointer  = (char *)pointer;
    cpointer -= sizeof(sslog_object_t);
    sslog_object_t *object = (sslog_object_t *)cpointer;

    object->is_detached = true;

    if (object->free_func != NULL) {
        object->free_func(object);
    } else {
        free(object);
    }
}



void sslog_retain(void *pointer)
{
    char *cpointer  = (char *) pointer;
    cpointer -= sizeof(memory_object_t);
    memory_object_t *object = (memory_object_t *) cpointer;

    object->count++;
}



void sslog_release(void *pointer)
{
    char *cpointer  = ( char * )pointer;
    cpointer -= sizeof(memory_object_t);
    memory_object_t *object = (memory_object_t *)cpointer;

    object->count--;

    if (object->count > 0) {
        return;
    }

    if (object->free_func != NULL) {
        object->free_func(object);
    } else {
        free(object);
    }
}



list_t *sslog_copy_triples(list_t *triples)
{
    list_t *copy_triples = list_new();

    if (list_is_null_or_empty(triples) == true) {
        return copy_triples;
    }

    list_head_t *list_walker = NULL;
    list_for_each(list_walker, &triples->links) {
        list_t *node = list_entry(list_walker, list_t, links);
        sslog_triple_t *triple = (sslog_triple_t *) node->data;

        sslog_triple_t *copy_triple = sslog_new_triple_detached(triple->subject, triple->predicate, triple->object,
                                                                (sslog_rdf_type) triple->subject_type, (sslog_rdf_type) triple->object_type);

        if (copy_triple != NULL) {
            list_add_data(copy_triples, copy_triple);
        }

    }

    return copy_triples;
}



char *sslog_generate_uri(const sslog_class_t *ont_class)
{
    if (ont_class == NULL) {
        return NULL;
    }

    bool sharp_exists = true;

    // Find lenght of string before '#', if there is no '#' then use all tring.
    int sharp_place = string_to_sharp_length(ont_class->entity.uri);

    if (sharp_place == 0) {
        sharp_exists = false;
        sharp_place = strlen(sslog_entity_get_uri((sslog_class_t *)ont_class)) + 1;
    }

    // Create a string for URI: class_uri_part#generated_part
    char *uri = (char *) calloc(sizeof(char) * (sharp_place + SSLOG_URI_GENPART_LENGTH + SSLOG_NULL_BYTE_LENGTH),1);

    if (uri == NULL) {
        return NULL;
    }

    char *hash = sslog_generate_hash();

    if (hash == NULL) {
        return NULL;
    }

    strncpy(uri, ont_class->entity.uri, sharp_place);

    if (sharp_exists == false) {
        uri[sharp_place - 1] =  '#';
    }

    strncat(uri, hash, SSLOG_URI_GENPART_LENGTH);

    free(hash);

    return uri;
}




/****************************** Implementation ********************************/
/***************************** Static functions *******************************/
/**
 * @brief Findes sharp ("#") in string and returns it place in given uri.
 * Function returns length on given uri if there is no sharp.
 *
 * @param uri URI to search in
 *
 * @return number > 0 on success or 0 otherwise.
 */
static int string_to_sharp_length(char *uri)
{
    size_t size = strlen(uri);

    if (size == 0) {
        return 0;
    }

    int i = 0;

    for (i = size - 1; i >= 0; i--) {
        if (uri[i] == '#') break;
    }

    return (i == 0) ? size : i + 1;
}


// TODO: Write better code, find uuid library.
static char* sslog_generate_hash()
{
    int rnd_num = rand() % 100000;

    char *hash = (char *) malloc(sizeof(char) * (SSLOG_URI_GENPART_LENGTH + SSLOG_NULL_BYTE_LENGTH));

    if (hash == NULL) return NULL;

    sprintf(hash, "%i", rnd_num);

    return hash;
}


