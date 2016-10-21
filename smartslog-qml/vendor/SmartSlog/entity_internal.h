/**
 * @file triple.h
 * @author Aleksandr A. Lomov <lomov@cs.karelia.ru>
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
 * Copyright (C) 2009 PetrSU (SmartSlog team). All rights reserved.
 *
 */

#include "entity.h"

#include "triple_internal.h"
#include "utils/bool.h"

#ifndef _SSLOG_ENTITY_INTERNAL_H
#define	_SSLOG_ENTITY_INTERNAL_H

#if !defined(SSLOG_EXTERN)
#if defined(SSLOG_EXPORT)
#define SSLOG_EXTERN __declspec(dllexport)
#elif defined (SSLOG_IMPORT)
#define SSLOG_EXTERN __declspec(dllimport)
#else
#define SSLOG_EXTERN
#endif
#endif

/// @cond DOXY_INTERNAL

/******************************* Definitions *********************************/
/****************** Defines, structures, constatnts and etc.******************/
/** @brief Constant: max lenght for entity uri. */
#define SSLOG_ENTITY_URI_MAX_LEN SSLOG_TRIPLE_URI_LEN

/**
 * @brief Base structure of entity: class, property or individual.
 */
struct sslog_entity_s {
    sslog_entity_type type;         /**< Type of the entity: class, property or individual. */
    char *uri;                      /**< URI of class. */
    sslog_triple_t *linked_triple;  /**< Linked triple for entity */
};

/**
 * @brief Class structure.
 */
struct sslog_class_s {
    sslog_entity_t entity;      /**< Base structure of entity */
    // list_t *superclasses;   /**< List of superclasses. */
    list_t *properties;         /**< Properties list for class. */
};

/**
 * @brief Individual structure.
 */
struct sslog_individual_s {
    sslog_entity_t entity;      /**< Base structure of entity */
    char *class_uri;            /**< Individual's class uri. */
};

/**
 * @brief Property structure.
 */
struct sslog_property_s {
    sslog_entity_t entity;  /**< Base structure of entity */
    char *domain;           /**< Property domain. */
    int mincardinality;     /**< Minimal cardinality. */
    int maxcardinality;     /**< Maximum cardinality. */
	sslog_property_type type;    /**< Type of property: literal (data) or object. */
};


/******************************* Definitions *********************************/
/**************************** Internal functions *****************************/

/******************** Constructors ('_new_' functions) ***********************/
/**
 * @brief Creates a new individual without creating a base triple and linking with it.
 * @param[in] class_uri. URI of the base class.
 * @param[in] uri. URI of the individual.
 * @return new individual on success or NULL otherwise.
 */
sslog_individual_t *sslog_new_individual_detached(const char *class_uri, const char *uri);


/**
 * @brief Creates a new individual based on rdf:type triple and links to this triple.
 * If the triple already linked when a new individual dosen't created and linked individual
 * will be returned.
 * @param[in] triple. RDF:Type triple describes individual.
 * @return individual on success or NULL otherwise.
 */
sslog_individual_t *sslog_new_individual_from_triple(sslog_triple_t *triple);


//TODO: Implement or not
/**
 * @brief Creates a new individual based on rdf:type triple withoutn linkging with it.
 * @param[in] triple. RDF:Type triple describes individual.
 * @return individual on success or NULL otherwise.
 */
sslog_individual_t *sslog_new_individual_from_triple_detached(sslog_triple_t *triple);


/******************** Destructors ('_free_' functions) ***********************/
/**
 * @brief Frees resources allocated for the entity.
 * The entity can be individual, property or class.
 * It does not free the linked with entity triple.
 * @param[in] entity. Entity to free.
 */
void sslog_free_entity(sslog_entity_t *entity);


/**
 * @brief Frees resources allocated for the individual.
 * Function does not free the linked with individual triples.
 * @param individual. Individual to free.
 */
void sslog_free_individual(sslog_individual_t *individual);


/**
 * @brief Frees resources allocated for the class.
 * Function does not free the linked with class triples.
 * @param ont_class. Class to free.
 */
void sslog_free_class(sslog_class_t *ont_class);


/**
 * @brief Frees resources allocated for the property.
 * Function does not free the linked with property triples.
 * @param property. Property to free.
 */
void sslog_free_property(sslog_property_t *property);


/***************************** Setters/Getters *******************************/
/**
 * @brief Links entity with triple without checking is triple or entity already linked.
 * @param entity. Entity to link.
 * @param triple. Triple to link.
 */
void sslog_entity_set_link(sslog_entity_t *entity, sslog_internal_triple_t *triple);

/// @endcond
//  endcond of DOXY_INTERNAL

#endif	/* _SSLOG_ENTITY_INTERNAL_H */


