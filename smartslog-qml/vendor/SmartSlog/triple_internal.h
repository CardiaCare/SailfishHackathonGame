/**
 * @file triple.h
 * @Author Aleksandr A. Lomov <lomov@cs.karelia.ru>
 * @date   03 March, 2015
 * @brief  Describe triple.
 *
 *
 * @section LICENSE
 *
 * SmartSlog KP Library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * SmartSlog KP Library is distributed in the hope that it will be useful,
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
 * Describe triple to store RDF content.
 * This file is part of SmartSlog KP Library.
 *
 * Copyright (C) 2009 - SmartSlog. All rights reserved.
 *
 */


#include "triple.h"

#include <string.h>

#include "entity.h"
#include "utils/bool.h"
#include "utils/list.h"


#ifndef _SSLOG_TRIPLE_INTERNAL_H
#define	_SSLOG_TRIPLE_INTERNAL_H


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




SSLOG_EXTERN bool sslog_triple_is_type(sslog_triple_t *triple);

#define sslog_triple_to_internal(triple_to_convert, variable) \
    sslog_internal_triple_t *variable = (sslog_internal_triple_t *) triple_to_convert;
#define sslog_triple_as_internal(triple) ((sslog_internal_triple_t *) triple)

typedef struct sslog_internal_triple_s {
    sslog_triple_t data;
//    struct {
        //sslog_triple_t data;
         /* Plus internals: */
        bool is_stored;
        sslog_entity_t* linked_entity;
//    } i;
} sslog_internal_triple_t;



/** @brief Types for triple elements. */
//typedef enum sslog_rdf_type_e {
//    SSLOG_RDF_TYPE_INCORRECT = -1, /**< Incorrect type. */
//    SSLOG_RDF_TYPE_URI = 1,    /**< URI to some resource (subject, object). */
//    SSLOG_RDF_TYPE_LIT,        /**< Specific value (only for object). */
//    SSLOG_RDF_TYPE_BNODE,      /**< Blank node (subject, object). */
//    SSLOG_RDF_TYPE_UNBOUND     /**< For SPARQL. */
//} sslog_rdf_type;


#define SS_RDF_SIB_ANY    "http://www.nokia.com/NRC/M3/sib#any"


//typedef struct sslog_triple_s
//{
//    char *subject;
//    char *predicate;
//    char *object;
//    sslog_rdf_type subject_type;
//    sslog_rdf_type object_type;
//}sslog_triple_t;

//typedef struct sslog_triple_s {

	
/******************************* Definitions *********************************/
/**************************** Internal functions *****************************/

/**
 * @brief Creates a new empty triple.
 * Triples has NULL values for subject, predicate, object and incorrect value
 * for subject and object types.
 * @return new triple on success or NULL otherwise.
 */
sslog_triple_t *sslog_new_triple_empty();

/*****************************************************************************/

/*************** External functions  ******************/
	
/**
 * @brief Creates a new triple.
 * @param[in] subject.
 * @param[in] predicate.
 * @param[in] object.
 * @param[in] subject_type. Possible values are macros RDF_TYPE_URI and RDF_TYPE_BNODE.
 * @param[in] object_type. Possible values are macros RDF_TYPE_URI, RDF_TYPE_BNODE and RDF_TYPE_LIT. *
 * @return new created triple on success or NULL otherwise.
 */
SSLOG_EXTERN sslog_triple_t* sslog_store_new_triple(const char *subject, const char *predicate, const char *object,
                                                       sslog_rdf_type subject_type, sslog_rdf_type object_type);


/**
 * @brief Frees a triple, a triple can't be used anymore.
 * @param[in] triple. Triple to free.
 * @param[in] char* predicate.
 * @param[in] char* object.
 * @param[in] char* subject_type. Possible values are macros RDF_TYPE_URI and
 *            RDF_TYPE_BNODE.
 * @param[in] char* object_type. Possible values are macros RDF_TYPE_URI,
 *            RDF_TYPE_BNODE and RDF_TYPE_LIT. *
 * @return new created triple on success or NULL otherwise.
 */
SSLOG_EXTERN void sslog_store_free_triple(sslog_triple_t* triple);


SSLOG_EXTERN void sslog_free_triple_force(sslog_triple_t* triple);


/**
 * @brief Gets a sibject of the given triple.
 * Function does not make a copy of subject value.
 * It needs to make a copy of the value for working with it.
 * @param[in] triple. Triple to get a subject.
 * @return subject if the given triple is correct or NULL otherwise.
 */
SSLOG_EXTERN const char* sslog_triple_get_subject(sslog_triple_t* triple);
	

/**
 * @brief Gets a predicate of the given triple.
 * Function does not make a copy of predicate value.
 * It needs to make a copy of the value for working with it.
 * @param[in] triple. Triple to get a predicate.
 * @return predicate if the given triple is correct or NULL otherwise.
 */
SSLOG_EXTERN const char* sslog_triple_get_predicate(sslog_triple_t* triple); 


/**
 * @brief Gets an object of the given triple.
 * Function does not make a copy of the object value.
 * It needs to make a copy of the value for working with it.
 * @param[in] triple. Triple to get an object.
 * @return object if the given triple is correct or NULL otherwise.
 */
SSLOG_EXTERN const char* sslog_triple_get_object(sslog_triple_t* triple);
	

/**
 * @brief Gets a subject type of the given triple.
 * @param[in] triple. Triple to get a subject type.
 * @return subject type if the given triple is correct or SSLOG_RDF_TYPE_INCORRECT otherwise.
 * @see sslog_rdf_type
 */
SSLOG_EXTERN sslog_rdf_type sslog_triple_get_subject_type(sslog_triple_t* triple);

/**
 * @brief Gets a object type of the given triple.
 * @param[in] triple. Triple to get an object type.
 * @return object type if the given triple is correct or SSLOG_RDF_TYPE_INCORRECT otherwise.
 * @see sslog_rdf_type
 */
SSLOG_EXTERN sslog_rdf_type sslog_triple_get_object_type(sslog_triple_t* triple);
	
	
SSLOG_EXTERN bool sslog_triple_compare(sslog_triple_t *a, sslog_triple_t *b);


bool sslog_triple_is_template(sslog_triple_t *triple);
	
/* External functions  */
/// @cond INTERNAL_FUNCTIONS


/// @endcond


/* Functions for add/remove entities to repositories */
/// @cond EXTERNAL_FUNCTIONS
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


#endif	/* _SSLOG_TRIPLE_INTERNAL_H */



