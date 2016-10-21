/**
 * @file triple.h
 * @author Aleksandr A. Lomov <lomov@cs.karelia.ru>
 * @date   03 March, 2015
 * @brief  Describes a RDF-triple.
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
 * The RDF data model is similar to classical conceptual modeling approaches such as entity–relationship or class diagrams,
 * as it is based upon the idea of making statements about resources (in particular web resources) in the form
 * of subject–predicate–object expressions.
 * These expressions are known as triples in RDF terminology.
 * The subject denotes the resource, and the predicate denotes traits or aspects of the resource and expresses
 * a relationship between the subject and the object.
 *
 * For example, one way to represent the notion "The sky has the color blue" in RDF is as the triple:
 * a subject denoting "the sky",
 * a predicate denoting "has",
 * and an object denoting "the color blue".
 *
 * This file is part of the SmartSlog project.
 *
 * <b>Copyright (C) 2015 by PetrSU SmartSlog team.</b>
 *
 */

// NOTE: C KPI code.
// Include C KPI for some constants.
// Now there are no other KPIs for SmartSlog C-version.
// For supporting other KPIs this include could be removed and
// values of constants could be updated.
#include <ckpi/ckpi.h>

#include "utils/bool.h"


#ifndef _SSLOG_TRIPLE_H
#define	_SSLOG_TRIPLE_H

/// @cond NO_DOCS
#if !defined(SSLOG_EXTERN)
#if defined(SSLOG_EXPORT)
#define SSLOG_EXTERN __declspec(dllexport)
#elif defined (SSLOG_IMPORT)
#define SSLOG_EXTERN __declspec(dllimport)
#else
#define SSLOG_EXTERN
#endif
#endif
/// @endcond NO_DOCS


/******************************** Definitions ********************************/
/******************* Defines, structures, constatnts and etc.*****************/
/**
 * @defgroup TripleURIs URIs for triples (ANY, RDF and RDF-Scheme)
 * @{
 */

/**
 * @brief Defines 'any value' (special URI) for triple-template.
 * Triple-templates are used for query, remove operations
 * or for subscription operations.
 * A triple-template a regular triple, but elements of the triple
 * (subject, predicate, object) can be set with mask as '*' ('any value').
 */
#define SSLOG_TRIPLE_ANY SS_RDF_SIB_ANY

/** @brief Namespace for the RDF URI. */
#define SSLOG_TRIPLE_RDF_NAMESPACE "http://www.w3.org/1999/02/22-rdf-syntax-ns#"

/** @brief Namespace for the RDF-Scheme URI. */
#define SSLOG_TRIPLE_RDFS_NAMESPACE "http://www.w3.org/2000/01/rdf-schema#"

/** @brief URI of rdf:type. */
#define SSLOG_TRIPLE_RDF_TYPE SSLOG_TRIPLE_RDF_NAMESPACE"type"

/** @brief URI of rdfs:Class. */
#define SSLOG_TRIPLE_RDFS_CLASS SSLOG_TRIPLE_RDFS_NAMESPACE"Class"

/** @brief URI of rdf:Property. */
#define SSLOG_TRIPLE_RDF_PROPERTY SSLOG_TRIPLE_RDF_NAMESPACE"Property"
/**@}*/ // Group TripleURIs


/**
 * @ingroup TripleURIs
 * @defgroup TripleLengths Length for triple URIs
 * @{
 */

/**
 * @brief Length of the URI for 'any value'.
 * @see SSLOG_TRIPLE_ANY
 */
#define SSLOG_TRIPLE_ANY_LEN 50

/** @brief Maximum URI length for triples elements. */
#define SSLOG_TRIPLE_URI_LEN (SS_SUBJECT_MAX_LEN - 1)

/** @brief Maximum length of the subject. */
#define SSLOG_TRIPLE_SUBJECT_LEN (SS_SUBJECT_MAX_LEN - 1)

/** @brief Maximum length of the object. */
#define SSLOG_TRIPLE_OBJECT_LEN (SS_OBJECT_MAX_LEN - 1)

/** @brief Maximum length of the preficate. */
#define SSLOG_TRIPLE_PREDICATE_LEN (SS_PREDICATE_MAX_LEN - 1)

/** @brief Maximum length of rdf:type URI. */
#define SSLOG_TRIPLE_RDF_TYPE_LEN 50

/** @brief Maximum length of rdf:Property URI. */
#define SSLOG_TRIPLE_RDF_PROPERTY_LEN 50

/** @brief Maximum length of rdfs:Class URI. */
#define SSLOG_TRIPLE_RDFS_CLASS_LEN 50
/**@}*/ // Group TripleLengths


/**
 * @brief Structure for represting a triple.
 * Triple contains of 3 elements: subject, predicate and object.
 * <ol>
 *      <li> <b>subject</b> can be URI or blank node and represents some resource.</li>
 *      <li> <b>predicate</b> is always URI and denotes traits or aspects of the resource.
 *           It expresses a relationship between the subject and the object.</li>
 *      <li> <b>object</b> can be URI, blank node or literal.</li>
 * </ol>
 *  Blank node (also called bnode) is a node in an RDF graph representing
 * a resource for which a URI or literal is not given.
 * The resource represented by a blank node is also called an anonymous resource.
 */
typedef struct sslog_triple_s
{
    char *subject;      /**< String represents a some URI or BNode of resource. */
    char *predicate;    /**< String represents URI of property for resource. */
    char *object;       /**< String represents a data value of property or URI/BNode of resource. */
    int subject_type;   /**< Type of the subject: URI or BNode. */
    int object_type;    /**< Type of the object: URI, BNode or Literal. */
} sslog_triple_t;


/** @brief Types for triple elements. */
typedef enum sslog_rdf_type_e {
    SSLOG_RDF_TYPE_INCORRECT = -1,               /**< Incorrect type. */
    SSLOG_RDF_TYPE_URI = SS_RDF_TYPE_URI,        /**< URI of some resource (subject, object). */
    SSLOG_RDF_TYPE_LIT = SS_RDF_TYPE_LIT,        /**< Specific data-value (only for object). */
    SSLOG_RDF_TYPE_BNODE = SS_RDF_TYPE_BNODE,    /**< Blank node (subject, object). */
    SSLOG_RDF_TYPE_UNBOUND                       /**< For SPARQL, when a variable is not bounded in result. */
} sslog_rdf_type;


/**
 * @brief Macros to free list with triples.
 * @see slog_triples_t
 * @see sslog_free_triples
 * @see list_s
 */
#define sslog_free_triples(triples_list) list_free_with_nodes(triples_list, LIST_CAST_TO_FREE_FUNC sslog_free_triple);


#ifdef	__cplusplus
extern "C" {
#endif


/**
 * @defgroup TripleIsFunctions Functions to check type of triple (entity type represented by a triple)
 * @{
 */

/**
 * @brief Checks that a triple is rdf:type (represents class, property or indicidual).
 * Predicate of such triple is equal to #SSLOG_TRIPLE_RDF_TYPE
 * @param[in] triple Triple to check.
 * @return true if the predicate is rdf:type URI or false otherwise.
 * @see SSLOG_TRIPLE_RDF_TYPE
 */
SSLOG_EXTERN bool sslog_triple_is_type(sslog_triple_t *triple);

/**
 * @brief Checks that a triple represents an individual.
 * Predicate of such triple is equal to #SSLOG_TRIPLE_RDF_TYPE
 * and triple is not represent a class or a property
 * (object must be URI and not be equal to #SSLOG_TRIPLE_RDFS_CLASS and  #SSLOG_TRIPLE_RDF_PROPERTY).
 * @param[in] triple Triple to check.
 * @return true if the triple represents an individual or false otherwise.
 * @see SSLOG_TRIPLE_RDF_TYPE
 * @see SSLOG_TRIPLE_RDFS_CLASS
 * @see SSLOG_TRIPLE_RDF_PROPERTY
 */
SSLOG_EXTERN bool sslog_triple_is_individual(sslog_triple_t *triple);

/**
 * @brief Checks that a triple represents a class.
 * Predicate of such triple is equal to #SSLOG_TRIPLE_RDF_TYPE
 * and triple object is URI and equals to #SSLOG_TRIPLE_RDFS_CLASS.
 * @param[in] triple Triple to check.
 * @return true if the triple represents a class or false otherwise.
 * @see SSLOG_TRIPLE_RDF_TYPE
 * @see SSLOG_TRIPLE_RDFS_CLASS
 */
SSLOG_EXTERN bool sslog_triple_is_class(sslog_triple_t *triple);

/**
 * @brief Checks that a triple represents a property.
 * Predicate of such triple is equal to #SSLOG_TRIPLE_RDF_TYPE
 * and triple object is URI and equals to #SSLOG_TRIPLE_RDF_PROPERTY.
 * @param[in] triple Triple to check.
 * @return true if the triple represents a property or false otherwise.
 * @see SSLOG_TRIPLE_RDF_TYPE
 * @see SSLOG_TRIPLE_RDF_PROPERTY
 */
SSLOG_EXTERN bool sslog_triple_is_property(sslog_triple_t *triple);

/**
 * @brief Checks that a triple represents a value for individual property (object or literal).
 * Subject of such triple is URI and predicate is not equal to #SSLOG_TRIPLE_RDF_TYPE.
 * @param[in] triple Triple to check.
 * @return true if triple is a value of individual property or false otherwise.
 */
SSLOG_EXTERN bool sslog_triple_is_property_value(sslog_triple_t *triple);
/**@}*/ // Group TripleIsFunctions


/**
 * @brief Makes a detached copy of triple.
 * @param triple Triple to make a copy.
 * @return copy of given triples on success or NULL therwise.
 */
SSLOG_EXTERN sslog_triple_t *sslog_triple_copy(sslog_triple_t *triple);

#ifdef	__cplusplus
}
#endif


#endif	/* _SSLOG_TRIPLE_H */

