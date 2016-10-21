/**
 * @file    high_api.h
 * @author  Aleksandr A. Lomov <lomov@cs.karelia.ru>
 * @date    2 June, 2015
 * @brief   Describes API functions to implement high-level access to smart space and for local store.
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
 * The high API based on entities (classes, individual, properies).
 * These entities are used as wrapper for triples.
 * All operations on entities are performed on triples on local store
 * and in smart spcae.
 *
 * This file is part of the SmartSlog project.
 *
 * Copyright (C) 2015 by PetrSU SmartSlog team.
 */

#include "high_api.h"
#include "high_api_internal.h"

#include <stdlib.h>

#include "session_internal.h"
#include "triplestore.h"
#include "entity_internal.h"

#include "utils/errors_internal.h"
#include "utils/util_func.h"
#include "utils/list.h"

#if !defined(SSLOG_EXTERN)
#if defined(SSLOG_EXPORT)
#define SSLOG_EXTERN __declspec(dllexport)
#elif defined (SSLOG_IMPORT)
#define SSLOG_EXTERN __declspec(dllimport)
#else
#define SSLOG_EXTERN
#endif
#endif


/******************************** Definitions ********************************/
/***************************** Static functions ******************************/
static inline char *sslog_object_get_uri(void *object);
static inline char *sslog_object_get_individual_uri(void *object);
static inline void sslog_prepare_prop_value(sslog_property_t *property, void *value, char **prepared_value, sslog_rdf_type *type);
static inline void sslog_prepare_prop_value_with_any(sslog_property_t *property, void *value, char **prepared_value, sslog_rdf_type *type);


/****************************** Implementations ******************************/
/**************************** External functions *****************************/
/// @cond DOXY_EXTERNAL_API

/// @endcond
//  endcond of DOXY_EXTERNAL_API

/******************* Entity-triple converting functions ***********************/
sslog_entity_t *sslog_entity_from_triple(sslog_triple_t *triple)
{
    if (triple == NULL) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_NULL_ARGUMENT "triple.");
        return NULL;
    }

    if (triple == NULL) {
        return NULL;
    }

    if (sslog_triple_is_individual(triple) == true) {
       return sslog_individual_as_entity(sslog_individual_from_triple(triple));
    }

    if (sslog_triple_is_property_value(triple) == true) {
       return sslog_individual_as_entity(sslog_individual_from_triple(triple));
    }

    if (sslog_triple_is_class(triple) == true) {
       return sslog_class_as_entity(sslog_class_from_triple(triple));
    }

    if (sslog_triple_is_property(triple) == true) {
       return sslog_property_as_entity(sslog_property_from_triple(triple));
    }

    return NULL;
}


sslog_individual_t *sslog_individual_from_triple(sslog_triple_t *triple)
{
    if (triple == NULL) {
        return NULL;
    }

    // Triple is rdf:type or individual property value.
    // If the triple represents an individual then get linked entity
    // (if it exists) or create a new individual based on triple,
    // while creating andividual will be linked with given triple.
    // If the triple represent a property value|individual property
    // (individualURI-propertyURI-value),
    // then get rdf:type triple and create an individual.
    if (sslog_triple_is_individual(triple) != true
            && sslog_triple_is_property_value(triple) != true) {
        return NULL;
    }

    sslog_triple_to_internal(triple, int_triple);

    if (int_triple->linked_entity != NULL) {
        return sslog_entity_as_individual(int_triple->linked_entity);
    }

    if (sslog_triple_is_individual(triple) == true) {
        return sslog_new_individual_from_triple(triple);
    }

    //TODO: change code to sslog_store_get_individual
    if (sslog_triple_is_property_value(triple) == true) {

        sslog_triple_t *rdftype = sslog_store_get_rdftype(sslog_session_get_default()->store, triple->subject);

        if (rdftype == NULL) {
            SSLOG_DEBUG_FUNC("No rdftype for individual: %s", triple->subject);
            return NULL;
        }

        return sslog_new_individual_from_triple(rdftype);
    }

    SSLOG_DEBUG_FUNC("Triple is not an indvidual or property value: %s - %s - %s (o-type: %d)",
                    triple->subject, triple->predicate, triple->object, triple->object_type);

    return NULL;
}


sslog_class_t *sslog_class_from_triple(sslog_triple_t *triple)
{
    if (triple == NULL) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_NULL_ARGUMENT "triple.");
        return NULL;
    }

    sslog_triple_to_internal(triple, int_triple);

    char *class_uri = NULL;

    // If the triple represents a class (ClassURI-rdf:type-rdfs:Class),
    // then checks linked entity: returns if it exists or get URI to get a class.
    // If the triple represents an individual (IndURI-rdf:type-ClassURI),
    // then get an object (ClassURI) to get a class.
    if (sslog_triple_is_class(triple) == true) {

        if (int_triple->linked_entity == NULL) {
            class_uri = triple->subject;
        } else {
            return sslog_entity_as_class(int_triple->linked_entity);
        }

    } else if (sslog_triple_is_individual(triple) == true) {
        class_uri = triple->object;
    }

    if (class_uri == NULL) {
        return NULL;
    }

    return sslog_store_get_class(sslog_session_get_default()->store, class_uri);
}


sslog_property_t *sslog_property_from_triple(sslog_triple_t *triple)
{
    if (triple == NULL) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_NULL_ARGUMENT "triple.");
        return NULL;
    }

    sslog_triple_to_internal(triple, int_triple);

    char *property_uri = NULL;


    // Checks linked triple or gets URI of property
    // from rdf:type triple or property value (predicate).
    if (sslog_triple_is_property(triple) == true) {

        if (int_triple->linked_entity == NULL) {
            property_uri = triple->subject;
        } else {
            return sslog_entity_as_property(int_triple->linked_entity);
        }

    } else if (sslog_triple_is_property_value(triple) == true) {
        property_uri = triple->predicate;
    }

    if (property_uri != NULL) {
        return sslog_store_get_property(sslog_session_get_default()->store, property_uri);
    }

    return NULL;
}


sslog_triple_t* sslog_entity_to_triple(sslog_entity_t *entity)
{
    if (entity == NULL) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_NULL_ARGUMENT "entity.");
        return NULL;
    }

    return entity->linked_triple;
}


sslog_triple_t * sslog_individual_to_triple(sslog_individual_t *individual)
{
    if (individual == NULL) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_NULL_ARGUMENT "individual.");
        return NULL;
    }

    return sslog_individual_as_entity(individual)->linked_triple;
}


sslog_triple_t * sslog_class_to_triple(sslog_class_t *ont_class)
{
    if (ont_class == NULL) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_NULL_ARGUMENT "ont_class.");
        return NULL;
    }

    return sslog_class_as_entity(ont_class)->linked_triple;
}


sslog_triple_t * sslog_property_to_triple(sslog_property_t *property)
{
    if (property == NULL) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_NULL_ARGUMENT "property.");
        return NULL;
    }

    return sslog_property_as_entity(property)->linked_triple;
}
/*****************************************************************************/


/******************* Functions for individual properties *********************/
const void *sslog_get_property(sslog_individual_t *individual, sslog_property_t *property)
{
    if (individual == NULL) {
        sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "individul");
        return NULL;
    }

    if (property == NULL) {
        sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "property");
        return NULL;
    }

    sslog_error_reset(NULL);

    list_t *query_triples = sslog_store_query_triples_by_data(
                sslog_session_get_default()->store,
                individual->entity.uri, property->entity.uri, SSLOG_TRIPLE_ANY,
				SSLOG_RDF_TYPE_URI, (sslog_rdf_type)property->type, 1);

    if (list_is_null_or_empty(query_triples) == true) {
        list_free(query_triples);
        SSLOG_DEBUG_FUNC("No properties value for individual (%s - %s).",
                         individual->entity.uri, property->entity.uri);
        return NULL;
    }

    sslog_internal_triple_t *triple = (sslog_internal_triple_t *) list_get_first_prev_node(query_triples)->data;
    list_free_with_nodes(query_triples, NULL);

    // If property is literal then return object of triple.
    if (sslog_property_is_object(property) != true) {
        return triple->data.object;
    }

    // If property is object then get rdf:type triple from store.
    // If there is no such triple, then we can't crete an individual
    // and return NULL.
    sslog_internal_triple_t *rdftype_triple = (sslog_internal_triple_t *) sslog_store_get_rdftype(sslog_session_get_default()->store, triple->data.object);

    if (rdftype_triple == NULL) {
        SSLOG_DEBUG_FUNC("No found rdf:type triple for individual property (%s - %s). Individual canot be created.",
                         individual->entity.uri, property->entity.uri);
        sslog_error_set(NULL, SSLOG_ERROR_INCORRECT_INDIVIDUAL,
                        SSLOG_ERROR_TEXT_INCORRECT_INDIVIDUAL "No found rdf:type triple for individual property. Individual canot be created.");

        return NULL;
    }

    if (rdftype_triple->linked_entity != NULL) {
        return rdftype_triple->linked_entity;
    }

    return sslog_new_individual_from_triple((sslog_triple_t *) rdftype_triple);
}


list_t *sslog_get_properties(sslog_individual_t *individual, sslog_property_t *property)
{
    if (individual == NULL) {
        sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "individul");
        return NULL;
    }

    if (property == NULL) {
        sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "property");
        return NULL;
    }

    list_t *query_triples = sslog_store_query_triples_by_data(
                sslog_session_get_default()->store,
                individual->entity.uri, property->entity.uri, SSLOG_TRIPLE_ANY,
				SSLOG_RDF_TYPE_URI, (sslog_rdf_type) property->type, -1);

    if (list_is_empty(query_triples) == 1) {
        SSLOG_DEBUG_FUNC("No values for property '%s'", sslog_entity_get_uri(property));
        return query_triples;
    }

    SSLOG_DEBUG_FUNC("Founded '%d' values for individual (%s) property (%s)",
                     list_count(query_triples),
                     sslog_entity_get_uri(individual),
                     sslog_entity_get_uri(property));

    list_t *values = list_new();


    // Gets values from object (individuals) or data property.
    if (sslog_property_is_object(property) == false) {

        list_head_t *list_walker = NULL;

        list_for_each(list_walker, &query_triples->links) {
            list_t *node = list_entry(list_walker, list_t, links);
            sslog_triple_t *triple = (sslog_triple_t *) node->data;

            list_add_data(values, triple->object);
        }


    } else {

        list_head_t *list_walker = NULL;

        list_for_each(list_walker, &query_triples->links) {
            list_t *node = list_entry(list_walker, list_t, links);
            sslog_triple_t *triple = (sslog_triple_t *) node->data;

            sslog_individual_t *individual = sslog_store_get_individual(sslog_session_get_default()->store, triple->object);

            if (individual != NULL) {
                list_add_data(values, individual);
            }
        }
    } // End IF

    // No need to remove stored triples, remove only list and nouds.
    list_free_with_nodes(query_triples, NULL);

    return values;
}


int sslog_insert_property(sslog_individual_t *individual, sslog_property_t *property, void *value)
{
    if (individual == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "individul");
    }

    if (property == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "property");
    }

    if (value == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "value");
    }

    char *prop_value = NULL;

    if (property->type == SSLOG_PROPERTY_TYPE_OBJECT) {
        sslog_entity_t *entity = (sslog_entity_t *) (value);

        if (entity->type != SSLOG_ENTITY_INDIVIDUAL) {
            SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_INCORRECT_ARGUMENT "value is not an individual");
            return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_INCORRECT_ARGUMENT "value is not an individual");
        }

        prop_value = entity->uri;

    }else {
        prop_value = (char *)value;
    }

    sslog_new_triple(individual->entity.uri, property->entity.uri, prop_value,
		SSLOG_RDF_TYPE_URI, (sslog_rdf_type) property->type);

    return sslog_error_get_last_code();
}


int sslog_remove_property(sslog_individual_t *individual, sslog_property_t *property, void *value)
{
    if (individual == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "individul");
    }

    if (property == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "property");
    }

    if (value == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "value");
    }

    char *object = (char *) value;
    sslog_rdf_type object_type = SSLOG_RDF_TYPE_LIT;

    if (sslog_property_is_object(property) == true) {
        object = sslog_object_get_individual_uri(value);
        object_type = SSLOG_RDF_TYPE_URI;
    }

    if (object == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_INCORRECT_ARGUMENT, SSLOG_ERROR_TEXT_INCORRECT_ARGUMENT "value is not an individual");
    }


    sslog_triple_t *triple_teplate = sslog_new_triple_detached(individual->entity.uri, property->entity.uri, object,
                                                               SSLOG_RDF_TYPE_URI, object_type);

    list_t *list = list_new();
    list_add_data(list, triple_teplate);

    sslog_store_remove_triples(sslog_session_get_default()->store, list);

    list_free_with_nodes(list, SSLOG_FREE_FUNC sslog_free_triple);

    return sslog_error_reset(NULL);
}


int sslog_remove_properties(sslog_individual_t *individual, sslog_property_t *property)
{
    if (individual == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "individul");
    }

    if (property == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "property");
    }


    sslog_triple_t *triple_teplate = sslog_new_triple_detached(individual->entity.uri, property->entity.uri, SSLOG_TRIPLE_ANY,
                                                               SSLOG_RDF_TYPE_URI, SSLOG_RDF_TYPE_URI);

    list_t *list = list_new();
    list_add_data(list, triple_teplate);

    sslog_store_remove_triples(sslog_session_get_default()->store, list);

    list_free_with_nodes(list, SSLOG_FREE_FUNC sslog_free_triple);

    return sslog_error_reset(NULL);
}


int sslog_update_property(sslog_individual_t *individual, sslog_property_t *property, void *current_value, void *new_value)
{
    if (current_value == NULL && new_value == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT,
                               SSLOG_ERROR_TEXT_NULL_ARGUMENT "'current_value' and 'new value' cannot be NULL at the same time.");
    }

    if (current_value == NULL) {
        return sslog_insert_property(individual, property, new_value);
    } else if (new_value == NULL) {
        return sslog_remove_property(individual, property, current_value);
    }

    if (individual == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "individul");
    }

    if (property == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "property");
    }

    char *current_uri = NULL;
    char *new_uri = NULL;

    if (sslog_property_is_object(property) == true) {
        current_uri = sslog_object_get_uri(current_value);
        new_uri = sslog_object_get_uri(new_value);
    } else {
        current_uri = (char *) current_value;
        new_uri = (char *) new_value;
    }

	sslog_triple_t *current_triple = sslog_new_triple_detached(individual->entity.uri, property->entity.uri, current_uri, SSLOG_RDF_TYPE_URI, (sslog_rdf_type) property->type);
    sslog_triple_t *new_triple = sslog_new_triple_detached(individual->entity.uri, property->entity.uri, new_uri, SSLOG_RDF_TYPE_URI, (sslog_rdf_type) property->type);

    list_t *current_triples = list_new();
    list_t *new_triples = list_new();

    list_add_data(current_triples, current_triple);
    list_add_data(new_triples, new_triple);

    int result = sslog_store_update_triples(sslog_session_get_default()->store, current_triples, new_triples);

    list_free_with_nodes(current_triples, SSLOG_FREE_FUNC sslog_free_triple);
    list_free_with_nodes(new_triples, SSLOG_FREE_FUNC sslog_free_triple);

    if (result != SSLOG_ERROR_NO) {
        return sslog_error_set(NULL, result, SSLOG_ERROR_TEXT_NO);
    }

    return sslog_error_reset(NULL);
}
/*****************************************************************************/


/********************** Functions for whole individual ***********************/
sslog_individual_t *sslog_get_individual(const char *uri)
{
    if (uri == NULL) {
        return NULL;
    }

    return sslog_store_get_individual(sslog_session_get_default()->store, uri);
}


int sslog_remove_individual(sslog_individual_t *individual)
{
    if (individual == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "individul");
    }

    int result = sslog_store_remove_individual(sslog_session_get_default()->store, individual);

    return sslog_error_set(NULL, result, NULL);
}
/*****************************************************************************/


/**************** Node functions for individual properties *******************/
const void *sslog_node_get_property(sslog_node_t *node, sslog_individual_t *individual, sslog_property_t *property)
{
    if (node == NULL) {
        sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "node");
        return NULL;
    }

    if (individual == NULL) {
        sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "individul");
        return NULL;
    }

    if (property == NULL) {
        sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "property");
        return NULL;
    }

    sslog_triple_t *query_triple = sslog_new_triple_detached(individual->entity.uri, property->entity.uri, SSLOG_TRIPLE_ANY,
                                                       SSLOG_RDF_TYPE_URI, SSLOG_RDF_TYPE_URI);

    list_t *result_triples = NULL;
    SSLOG_DEBUG_FUNC("Triple: %s - %s - %s (%d)", query_triple->subject,query_triple->predicate, query_triple->object, query_triple->object_type);
    int result = sslog_kpi_query_triple(node->kpi, query_triple, &result_triples);

    sslog_free_triple(query_triple);

    if (result != SSLOG_ERROR_NO) {
        sslog_error_set(&node->last_error, result, sslog_kpi_get_error_text(result));
        return NULL;
    }

    if (list_is_null_or_empty(result_triples) == 1) {
        list_free(result_triples);
        return NULL;
    }

    list_t *value_list = list_get_first_prev_node(result_triples);

    sslog_triple_t * value_triple = (sslog_triple_t *) value_list->data;

    value_list->data = NULL;

    list_free_with_nodes(result_triples, LIST_CAST_TO_FREE_FUNC sslog_free_triple);
    result_triples = NULL;

    if (sslog_property_is_object(property) == false) {
        sslog_triple_t *stored_triple = sslog_store_add_get_triple(node->session->store, value_triple);
        sslog_free_triple(value_triple);
        return stored_triple->object;
    }

    // If the property is object then get an individual from URI.
    sslog_individual_t *value_individual = sslog_store_get_individual(node->session->store, value_triple->object);

    if (value_individual != NULL) {
        sslog_free_triple(value_triple);
        return value_individual;
    }

    // Individual is not in the local store - get it from smart space.
    // Create rdf:type triple to get individual's triple.
    value_individual = sslog_node_get_individual_by_uri(node, value_triple->object);

    // Save triple for property.
    if (value_individual != NULL) {
        sslog_store_add_triple(node->session->store, value_triple);
    }

    sslog_free_triple(value_triple);

    return value_individual;
}


int sslog_node_insert_property(sslog_node_t *node, sslog_individual_t *individual, sslog_property_t *property, void *value)
{
    if (node == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "node.");
    }

    if (individual == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "individul");
    }

    if (property == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "property");
    }

    if (value == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "value");
    }

    if (sslog_property_is_object(property) == true
		&& sslog_entity_get_type(sslog_object_as_entity(value)) != SSLOG_ENTITY_INDIVIDUAL) {
        return sslog_error_set(NULL, SSLOG_ERROR_INCORRECT_ARGUMENT,
                               SSLOG_ERROR_TEXT_INCORRECT_ARGUMENT "value is not an individual for object property.");
    }

//    int result = sslog_insert_property(individual, property, value);

//    if (result != SSLOG_ERROR_NO) {
//        return result;
//    }

    char *prop_val = (char *) value;
	sslog_rdf_type object_type = SSLOG_RDF_TYPE_LIT;

    list_t *triples = list_new();

    if (sslog_property_is_object(property) == true) {
        sslog_entity_t *entity = (sslog_entity_t *) (value);
        prop_val = entity->uri;
        list_add_data(triples, entity->linked_triple);
        object_type = SSLOG_RDF_TYPE_URI;
    }

    sslog_triple_t *triple = sslog_new_triple_detached(individual->entity.uri, property->entity.uri, prop_val,
                                                       SSLOG_RDF_TYPE_URI, object_type);

    list_add_data(triples, triple);
    list_add_data(triples, individual->entity.linked_triple);


    int result = sslog_node_insert_triples(node, triples);

    list_free_with_nodes(triples, LIST_CAST_TO_FREE_FUNC sslog_free_triple);

    return result;
}


int sslog_node_remove_property(sslog_node_t *node, sslog_individual_t *individual, sslog_property_t *property, void *value)
{
    if (node == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "node.");
    }

    if (individual == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "individual.");
    }

    if (property == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "property.");
    }


    // Prepare property value^ data or object.
    // For object property get an URI of an individual.
    char *prop_val = (char *) value;
	sslog_rdf_type object_type = SSLOG_RDF_TYPE_LIT;

    if (value == NULL) {
        SSLOG_DEBUG_FUNC("Value of individual (%s) property (%s) is NULL - removing all such properties.",
                         individual->entity.uri, property->entity.uri);
        prop_val = SSLOG_TRIPLE_ANY;
        object_type = SSLOG_RDF_TYPE_URI;
    } else if (sslog_property_is_object(property) == true) {
        prop_val = sslog_object_get_individual_uri(value);
        object_type = SSLOG_RDF_TYPE_URI;
    }

    if (prop_val == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_INCORRECT_ARGUMENT,
                               SSLOG_ERROR_TEXT_INCORRECT_ARGUMENT "preparing of value of property failed.");
    }


    sslog_triple_t *triple = sslog_new_triple_detached(individual->entity.uri, property->entity.uri, prop_val,
                                                       SSLOG_RDF_TYPE_URI, object_type);

    int result = sslog_node_remove_triple(node, triple);

    sslog_free_triple(triple);

    return result;
}


SSLOG_EXTERN int sslog_node_update_property(sslog_node_t *node, sslog_individual_t *individual, sslog_property_t *property,
                                            void *current_value, void *new_value)
{
    sslog_error_reset(&node->last_error);

    if (node == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "node.");
    }

    if (individual == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "individual.");
    }

    if (property == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "property.");
    }

    if (current_value == NULL && new_value == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT,
                               SSLOG_ERROR_TEXT_NULL_ARGUMENT "'current_value' and 'new_value'.");
    }

    if (current_value == new_value) {
        return sslog_error_reset(&node->last_error);
    }

    char *cur_prop_val = NULL;
    sslog_rdf_type cur_prop_type = SSLOG_RDF_TYPE_INCORRECT;
    sslog_prepare_prop_value(property, current_value, &cur_prop_val, &cur_prop_type);

    char *new_prop_val = NULL;
    sslog_rdf_type new_prop_type = SSLOG_RDF_TYPE_INCORRECT;
    sslog_prepare_prop_value(property, new_value, &new_prop_val, &new_prop_type);


    if (cur_prop_val == NULL && new_prop_val == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_INCORRECT_ARGUMENT,
                               SSLOG_ERROR_TEXT_INCORRECT_ARGUMENT "'current_value' and 'new_value'.");
    }

    list_t *new_triples = list_new();
    list_t *cur_triples = list_new();

    if (cur_prop_val != NULL) {
        sslog_triple_t *triple = sslog_new_triple_detached(individual->entity.uri, property->entity.uri, cur_prop_val,
                                                           SSLOG_RDF_TYPE_URI, cur_prop_type);
        list_add_data(cur_triples, triple);
    }

    if (new_prop_val != NULL) {
        sslog_triple_t *triple = sslog_new_triple_detached(individual->entity.uri, property->entity.uri, new_prop_val,
                                                           SSLOG_RDF_TYPE_URI, new_prop_type);
        list_add_data(new_triples, triple);
    }

    list_add_data(new_triples, individual->entity.linked_triple);   // rdf:type

    int result = sslog_kpi_update_triples(node->kpi, cur_triples, new_triples);

    if (result != SSLOG_ERROR_NO) {
        SSLOG_DEBUG_FUNC("KPI update failed: %s", sslog_kpi_get_error_text(result));
        list_free_with_nodes(new_triples, LIST_CAST_TO_FREE_FUNC sslog_free_triple);
        list_free_with_nodes(cur_triples, LIST_CAST_TO_FREE_FUNC sslog_free_triple);

        return sslog_error_set(&node->last_error, result, sslog_kpi_get_error_text(result));
    }

    result = sslog_store_update_triples(node->session->store, cur_triples, new_triples);

    if (result != SSLOG_ERROR_NO) {
        sslog_error_set(&node->last_error, result, NULL);
        SSLOG_DEBUG_FUNC("Local update failed: %s", sslog_error_get_last_text());
        return sslog_error_set(&node->last_error, result, sslog_kpi_get_error_text(result));
    }
    list_free_with_nodes(new_triples, LIST_CAST_TO_FREE_FUNC sslog_free_triple);
    list_free_with_nodes(cur_triples, LIST_CAST_TO_FREE_FUNC sslog_free_triple);

    return sslog_error_get_last_code();

}
/*****************************************************************************/


/******************** Node functions for whole individual ********************/
sslog_individual_t *sslog_node_get_individual_by_uri(sslog_node_t *node, const char *uri)
{
    if (node == NULL) {
        return NULL;
    }

    if (uri == NULL) {
        return NULL;
    }

    sslog_triple_t *query_triple = sslog_new_triple_detached(uri, SSLOG_TRIPLE_RDF_TYPE, SSLOG_TRIPLE_ANY,
                                                             SSLOG_RDF_TYPE_URI, SSLOG_RDF_TYPE_URI);

    list_t *result_triples = NULL;
    int result = sslog_kpi_query_triple(node->kpi, query_triple, &result_triples);

    sslog_free_triple(query_triple);

    if (result != SSLOG_ERROR_NO) {
        return NULL;
    }

    if (result_triples == NULL) {
        return NULL;
    }

    list_t *first_node = list_get_first_prev_node(result_triples);
    sslog_triple_t *rdftype_triple = (sslog_triple_t *) first_node->data;
    first_node->data = NULL;
    list_free_with_nodes(result_triples, LIST_CAST_TO_FREE_FUNC sslog_free_triple);

    sslog_triple_t *stored_triple = sslog_store_add_get_triple(node->session->store, rdftype_triple);
    sslog_free_triple(rdftype_triple);

    return sslog_new_individual_from_triple(stored_triple);
}


list_t* sslog_node_get_individuals_by_class(sslog_node_t *node, sslog_class_t *ont_class)
{
    sslog_error_reset(&node->last_error);

    if (node == NULL) {
        sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "node.");
        return NULL;
    }

    if (ont_class == NULL) {
        sslog_error_set(&node->last_error, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "ont_class.");
        return NULL;
    }

    sslog_triple_t *query_triple = sslog_new_triple_detached(SSLOG_TRIPLE_ANY, SSLOG_TRIPLE_RDF_TYPE, ont_class->entity.uri,
                                                             SSLOG_RDF_TYPE_URI, SSLOG_RDF_TYPE_URI);

    list_t *result_triple = NULL;
    int result = sslog_kpi_query_triple(node->kpi, query_triple, &result_triple);

    sslog_free_triple(query_triple);

    if (result != SSLOG_ERROR_NO) {
        SSLOG_DEBUG_FUNC("KPI query failed: %s", sslog_kpi_get_error_text(result));
        sslog_error_set(&node->last_error, result, sslog_kpi_get_error_text(result));
        return NULL;
    }

    // If no triples found, then return empty list.
    if (result_triple == NULL) {
        SSLOG_DEBUG_FUNC("No individuals with base class: '%s'.", ont_class->entity.uri);
        return list_new();
    }

    result = sslog_insert_triples(result_triple);

    //TODO: make more elegante: get individudals from triples.
    list_t *stored_triples = sslog_store_query_triples(node->session->store, result_triple);
    list_t *individuals = list_new();


    list_head_t *list_walker = NULL;
    list_for_each(list_walker, &stored_triples->links) {
        list_t *node = list_entry(list_walker, list_t, links);
        sslog_triple_t *triple = (sslog_triple_t *) node->data;

        sslog_individual_t *individual = sslog_individual_from_triple(triple);

        if (individual != NULL) {
            list_add_data(individuals, individual);
        }
    }

    list_free_with_nodes(result_triple, LIST_CAST_TO_FREE_FUNC sslog_free_triple);
    list_free_with_nodes(stored_triples, NULL);

    return individuals;
}


int sslog_node_populate(sslog_node_t *node, sslog_individual_t *individual)
{
    if (node == NULL) {
         return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "node");
    }

    if (individual == NULL) {
        return sslog_error_set(&node->last_error, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "individul");
    }

    sslog_triple_t *query_triple = sslog_new_triple_detached(individual->entity.uri, SSLOG_TRIPLE_ANY, SSLOG_TRIPLE_ANY,
                                                             SSLOG_RDF_TYPE_URI, SSLOG_RDF_TYPE_URI);

    list_t *result_triples = NULL;
    int result = sslog_kpi_query_triple(node->kpi, query_triple, &result_triples);

    sslog_free_triple(query_triple);

    if (result != SSLOG_ERROR_NO) {
        return sslog_error_set(&node->last_error, result, sslog_kpi_get_error_text(result));
    }

    if (list_is_null_or_empty(result_triples) == 1) {
        SSLOG_DEBUG_FUNC("No triples in smart space for individual (%s).", individual->entity.uri)
        list_free_with_nodes(result_triples, NULL);
        return sslog_error_reset(&node->last_error);
    }

    list_t *linked_triples = list_new();

    list_head_t *list_walker = NULL;
    list_for_each (list_walker, &result_triples->links) {
        list_t *entry = list_entry(list_walker, list_t, links);
        sslog_triple_t *triple = (sslog_triple_t *) entry->data;

        if (sslog_triple_is_property_value(triple) == false) {
            SSLOG_DEBUG_FUNC("Triple is not property value: %s - %s - %s (%d)",
                             triple->subject, triple->predicate, triple->object, triple->object_type);
            continue;
        }

        sslog_property_t *property = sslog_store_get_property(node->session->store, triple->predicate);

        if (property == NULL) {
            continue;
        }

        if (sslog_property_is_object(property) == true) {
            query_triple = sslog_new_triple_detached(triple->object, SSLOG_TRIPLE_RDF_TYPE, SSLOG_TRIPLE_ANY,
                                                     SSLOG_RDF_TYPE_URI, SSLOG_RDF_TYPE_URI);
            list_add_data(linked_triples, query_triple);
        }
    }

    list_t *result_linked_triples = NULL;
    result = sslog_kpi_query_triples(node->kpi, linked_triples, &result_linked_triples);

    list_free_with_nodes(linked_triples, LIST_CAST_TO_FREE_FUNC sslog_free_triple);

    if (result != SSLOG_ERROR_NO) {
        list_free_with_nodes(result_triples, LIST_CAST_TO_FREE_FUNC sslog_free_triple);
        return sslog_error_set(&node->last_error, result, sslog_kpi_get_error_text(result));
    }

    sslog_store_add_triples(node->session->store, result_triples);

    if (result_linked_triples != NULL) {
        sslog_store_add_triples(node->session->store, result_linked_triples);
    }

    list_free_with_nodes(result_triples, LIST_CAST_TO_FREE_FUNC sslog_free_triple);
    list_free_with_nodes(result_linked_triples, LIST_CAST_TO_FREE_FUNC sslog_free_triple);

    return sslog_error_reset(&node->last_error);
}


int sslog_node_insert_individual(sslog_node_t *node, sslog_individual_t *individual)
{
    if (node == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "node");
    }

    if (individual == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "individul");
    }

    list_t *triples = sslog_store_get_individual_triples(sslog_session_get_default()->store, individual, true);

    if (triples == NULL) {
        SSLOG_DEBUG_FUNC("Error while getting individual triples.");
        return sslog_error_set(&node->last_error, SSLOG_ERROR_UNKNOWN, SSLOG_ERROR_TEXT_UNKNOWN);
    }

    int result = sslog_node_insert_triples(node, triples);

    list_free_with_nodes(triples, NULL);

    return result;
}


int sslog_node_remove_individual(sslog_node_t *node, sslog_individual_t *individual)
{
    if (node == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "node");
    }

    if (individual == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "individul");
    }

    //list_t *triples = sslog_store_get_individual_triples(node->session->store, individual, true);

//    list_t *triples = sslog_store_query_triples_by_data(node->session->store,
//                                                        individual->entity.uri, SSLOG_TRIPLE_ANY, SSLOG_TRIPLE_ANY,
//                                                        SSLOG_RDF_TYPE_URI, SSLOG_RDF_TYPE_URI, -1);

//    if (triples == NULL) {
//        SSLOG_DEBUG_FUNC("Error while getting individual triples.");
//        return sslog_error_set(&node->last_error, SSLOG_ERROR_UNKNOWN, SSLOG_ERROR_TEXT_UNKNOWN);
//    }

    // Template to remove all properties and rdf:type triple from the smart space.
    sslog_triple_t *triple_template = sslog_new_triple_detached(individual->entity.uri, SSLOG_TRIPLE_ANY, SSLOG_TRIPLE_ANY,
                                                                SSLOG_RDF_TYPE_URI, SSLOG_RDF_TYPE_URI);

    list_t *triple_templates = list_new();
    list_add_data(triple_templates, triple_template);

    int result = sslog_kpi_remove_triples(node->kpi, triple_templates);

    if (result == SSLOG_ERROR_NO) {
        // Save rdf:type for individual, remove only property.
        list_t *local_triples = sslog_store_query_triples(node->session->store, triple_templates);

        sslog_triple_t *rdftype = sslog_store_get_rdftype(node->session->store, individual->entity.uri);

        list_del_and_free_nodes_with_data(local_triples, rdftype, NULL);

        sslog_remove_triples(local_triples);
        list_free_with_nodes(local_triples, LIST_CAST_TO_FREE_FUNC sslog_free_triple);

        sslog_error_reset(&node->last_error);
    } else {
        sslog_error_set(&node->last_error, result, sslog_kpi_get_error_text(result));
    }

    //int result = sslog_node_remove_triples(node, triples);

    list_free_with_nodes(triple_templates, LIST_CAST_TO_FREE_FUNC sslog_free_triple);

    return result;
}


int sslog_node_remove_individual_with_local(sslog_node_t *node, sslog_individual_t *individual)
{
    if (node == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "node");
    }

    if (individual == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "individul");
    }

    // Template to remove all properties and rdf:type triple from the smart space and local store.
    sslog_triple_t *triple_template = sslog_new_triple_detached(individual->entity.uri, SSLOG_TRIPLE_ANY, SSLOG_TRIPLE_ANY,
                                                                SSLOG_RDF_TYPE_URI, SSLOG_RDF_TYPE_URI);

    list_t *triple_templates = list_new();
    list_add_data(triple_templates, triple_template);

    int result = sslog_kpi_remove_triples(node->kpi, triple_templates);

    if (result == SSLOG_ERROR_NO) {
        sslog_remove_triple(triple_template);
        sslog_free_triples(triple_templates);

        return sslog_error_reset(&node->last_error);
    }

    sslog_free_triples(triple_templates);

    return sslog_error_set(&node->last_error, result, sslog_kpi_get_error_text(result));
}


/****************************** Implementations ******************************/
/****************************** Static functions *****************************/
static inline char *sslog_object_get_uri(void *object)
{
    sslog_entity_type *type = (sslog_entity_type *) object;

    if (*type == SSLOG_ENTITY_CLASS
            || *type == SSLOG_ENTITY_PROPERTY
            || *type == SSLOG_ENTITY_INDIVIDUAL) {
        return ((sslog_entity_t *) object)->uri;
    }

    return NULL;
}

static inline char *sslog_object_get_individual_uri(void *object)
{
    if (object == NULL) {
        return NULL;
    }

    sslog_entity_type *type = (sslog_entity_type *) object;

    if (*type == SSLOG_ENTITY_INDIVIDUAL) {
        return ((sslog_entity_t *) object)->uri;
    }

    return NULL;
}




static inline void sslog_prepare_prop_value(sslog_property_t *property, void *value, char **prepared_value, sslog_rdf_type *type)
{
    if (sslog_property_is_object(property) == true) {
        *prepared_value =  sslog_object_get_individual_uri(value);
        *type = SSLOG_RDF_TYPE_URI;
    } else {
        *prepared_value = (char *) value;
        *type = SSLOG_RDF_TYPE_LIT;
    }
}


static inline void sslog_prepare_prop_value_with_any(sslog_property_t *property, void *value, char **prepared_value, sslog_rdf_type *type)
{
    if (value == NULL) {
        *prepared_value = SSLOG_TRIPLE_ANY;
        *type = SSLOG_RDF_TYPE_URI;
    } else if (sslog_property_is_object(property) == true) {
        *prepared_value = sslog_object_get_individual_uri(value);
        *type = SSLOG_RDF_TYPE_URI;
    } else {
        *prepared_value = (char *) value;
        *type = SSLOG_RDF_TYPE_LIT;
    }
}





















