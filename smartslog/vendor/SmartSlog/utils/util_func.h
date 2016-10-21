/**
 * @file util_func.h
 * @Author Alaxandr A. Lomov <lomov@cs.karelia.ru>, Vanag Pavel <vanag@cs.karelia.ru>
 * @date   26 December, 2009
 * @brief  Interfaces for util functions.
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
 * Interfaces for util functions.
 * This file is part of PetrSU KP library.
 *
 * Copyright (C) 2009 - Alexander A. Lomov. All rights reserved.
 */

#ifndef _UTIL_FUNC_H_
#define _UTIL_FUNC_H_

#include <stdio.h>
#include <string.h>
//#include "kp_error.h"
#include "bool.h"
//#include "../structures.h"
//#include "../properties.h"
#include <sys/types.h>
#include "../triple.h"
#include "list.h"
#include "../entity.h"

#if !defined(SSLOG_EXTERN)
#if defined(SSLOG_EXPORT)
#define SSLOG_EXTERN __declspec(dllexport)
#elif defined (SSLOG_IMPORT)
#define SSLOG_EXTERN __declspec(dllimport)
#else
#define SSLOG_EXTERN
#endif
#endif

#define SSLOG_FREE_FUNC  (void (*)(void *))


//typedef struct reference_count_s memory_object_t;

typedef struct sslog_object_s {
    bool is_detached;
    void (*free_func)(void *data);
    void *data;
} sslog_object_t;



#ifdef	__cplusplus
extern "C" {
#endif

/******************************************************************************/
/**************************** External functions ******************************/
/** @cond EXTERNAL_FUNCTIONS */

/**
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
SSLOG_EXTERN char *sslog_strndup(const char *str, size_t n);


/**
 * @brief Checks equality of triples.
 * Function checks: equality of pointers, equality of subject/object types
 * and equality of subject, predicate, object values of triples.
 * @param[in] a. Triple A to compare.
 * @param[in] b. Triple B to compare.
 * @return  true if the triplets are equal or both are NULL pointers, otherwise returns false.
 */
SSLOG_EXTERN bool sslog_equal_triples(sslog_triple_t *a, sslog_triple_t *b);


SSLOG_EXTERN list_t* sslog_copy_triples(list_t *triples);

/// @endcond


/******************************************************************************/
/****************************** Functions list ********************************/
/// @cond INTERNAL_FUNCTIONS
bool is_in_property_list(list_t *list, sslog_triple_t *item);
sslog_triple_t *concat_triplets(sslog_triple_t *a, sslog_triple_t *b);

void *sslog_alloc(size_t size, void (*free_data_func)(void *data));
bool sslog_triple_is_stored(sslog_triple_t *triple);
void sslog_triple_stored(sslog_triple_t *triple, bool is_stored);
void sslog_free_force(void *pointer);
void sslog_free(void *pointer);

bool sslog_compare_triple_with_any(sslog_triple_t *a, sslog_triple_t *b, const char *any_value);

bool sslog_is_str_null_empty(const char *string);


/**
 * @brief Generate URI according given class.
 * Gets a part of class uri before '#' and adds a generated string to the end.
 * Result: part_of_uri_class#generated_part
 * @param ont_class. Class is used to get a start part of URI.
 * @return new URI on success or NULL otherwise.
 */
SSLOG_EXTERN char *sslog_generate_uri(const sslog_class_t *ont_class);
/// @endcond

#ifdef	__cplusplus
}
#endif

#endif /* _UTIL_FUNC_H_ */
