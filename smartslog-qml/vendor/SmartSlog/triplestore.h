/**
 * @file   repository.h
 * @author Alexandr A. Lomov <lomov@cs.karelia.ru>
 * @author Pavel I. Vanag <vanag@cs.karelia.ru>
 * @date   05 December, 2009
 * @brief  Describe functions for work with classes.
 *
 *
 * @section LICENSE
 *
 * SmartSlog is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * SmartSlog is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SmartSlog KP Library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 *
 *
 * @section DESCRIPTION
 *
 * Describe functions to work with reository.
 * This file is part of SmartSlog KP Library.
 *
 * Copyright (C) 2009 - SmartSlog. All rights reserved.
 *
 */

//#include "structures.h"
//#include "utils/kp_debug.h"
#include "triple.h"
#include "utils/list.h"
#include "entity.h"

#ifndef _SSLOG_TRIPLESTORE_H
#define	_SSLOG_TRIPLESTORE_H

#if !defined(SSLOG_EXTERN)
#if defined(SSLOG_EXPORT)
#define SSLOG_EXTERN __declspec(dllexport)
#elif defined (SSLOG_IMPORT)
#define SSLOG_EXTERN __declspec(dllimport)
#else
#define SSLOG_EXTERN
#endif
#endif


typedef list_t sslog_store_t;


#ifdef	__cplusplus
extern "C" {
#endif
	
/* External functions  */
	
//SSLOG_EXTERN list_t sslog_repo_add_triple()
	
/* External functions  */
/// @cond INTERNAL_FUNCTIONS


sslog_store_t *repo_new();
//TODO: opne for tests only
SSLOG_EXTERN sslog_store_t* sslog_store_new();
SSLOG_EXTERN void sslog_store_free(sslog_store_t *store);
SSLOG_EXTERN int sslog_store_add_triple(sslog_store_t *store, sslog_triple_t *triple);
int sslog_store_add_triples(sslog_store_t *store, list_t *triples);

int sslog_store_update_triples(sslog_store_t *store, list_t *current_triples, list_t *new_triples);
int sslog_store_remove_triples(sslog_store_t *store, list_t *triples_templates);

bool sslog_store_exists(sslog_store_t *store, sslog_triple_t *triple);
bool sslog_store_exists_template_data(sslog_store_t *store, const char *subject, const char *predicate, const char *object,
                                               sslog_rdf_type subject_type, sslog_rdf_type object_type);
bool sslog_store_exists_pointer(sslog_store_t *store, sslog_triple_t *triple);


sslog_triple_t * sslog_store_get_triple_by_data(sslog_store_t *store,
                                               const char *subject, const char *predicate, const char *object,
                                               sslog_rdf_type subject_type, sslog_rdf_type object_type);
sslog_triple_t * sslog_store_get_triple(sslog_store_t *store, list_t *triples_templates);

list_t* sslog_store_query_triples(sslog_store_t *store, list_t *triples_templates);
list_t* sslog_store_query_triples_by_data(sslog_store_t *store,
                                          const char *subject, const char *predicate, const char *object,
                                          sslog_rdf_type subject_type, sslog_rdf_type object_type, int max_triples_count);

sslog_triple_t * sslog_store_get_rdftype(sslog_store_t *store, const char *uri);



/**
 * @brief Removes individual from local store.
 * @param store. Store with triples.
 * @param individual. Individual to remove.
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 */
int sslog_store_remove_individual(sslog_store_t *store, sslog_individual_t *individual);



/**
 * @brief Gets triples that represent an individual.
 * @param store. Store with triples.
 * @param individual. Individual to get triples.
 * @return lsit with triples on success or NULL otherwise.
 */
list_t *sslog_store_get_individual_triples(sslog_store_t *store, sslog_individual_t *individual, bool with_rdftype);


/**
 * @brief Adds triple to store and returns a new triple or already stored triple.
 * If the triple found in the store then stored triple returned and passed triple does not stored,
 * if there is no such triple, then it will be stored and returned.
 * @param store. Store with triples.
 * @param triple. Triple to add.
 * @return triple on success or NULL otherwise.
 */
sslog_triple_t* sslog_store_add_get_triple(sslog_store_t *store, sslog_triple_t *triple);



void repo_add(sslog_triple_t *triple);
void repo_del(sslog_triple_t *triple);
/// @endcond



/* Functions for add/remove entities to repositories */
/// @cond EXTERNAL_FUNCTIONS

/**
 * @brief Gets an individual by URI.
 * @param store. Store with triples.
 * @param uri. URI of the indivdual.
 * @return individual on success or NULL otherwise (error, no triple that describes individual).
 */
SSLOG_EXTERN sslog_individual_t* sslog_store_get_individual(sslog_store_t *store, const char *uri);

/**
 * @brief Gets a property by URI.
 * @param store. Store with triples.
 * @param uri. URI of the property.
 * @return property on success or NULL otherwise (error, no triple that describes property).
 */
SSLOG_EXTERN sslog_property_t* sslog_store_get_property(sslog_store_t *store, const char *uri);


/**
 * @brief Gets a class by URI.
 * @param store. Store with triples.
 * @param uri. URI of the indivdual.
 * @return class on success or NULL otherwise (error, no triple that describes class).
 */
SSLOG_EXTERN sslog_class_t* sslog_store_get_class(sslog_store_t *store, const char *uri);


//SSLOG_EXTERN int sslog_repo_add_entity(void *entity);
//SSLOG_EXTERN int sslog_repo_add_class(class_t *ont_class);
//SSLOG_EXTERN int sslog_repo_add_property(property_t *property);
//SSLOG_EXTERN void sslog_repo_remove_entity(void *entity);
//SSLOG_EXTERN void sslog_repo_clean_all();
//SSLOG_EXTERN int sslog_repo_count_individual_references(individual_t *individual);
/// @endcond

/* Functions for get data from repositories */
/// @cond EXTERNAL_FUNCTIONS
//SSLOG_EXTERN const property_t* sslog_repo_get_property_by_name(const char *name);
//SSLOG_EXTERN const class_t* sslog_repo_get_class_by_classtype(const char *classtype);
//SSLOG_EXTERN const individual_t* sslog_repo_get_individual_by_uuid(const char *uuid);
//SSLOG_EXTERN const list_t* sslog_repo_get_individual_by_classtype_all(const char *classtype);
/// @endcond

#ifdef	__cplusplus
}
#endif

#endif	//_SSLOG_TRIPLESTORE_H
