/**
 * @file triple.c
 * @author Aleksandr A. Lomov <lomov@cs.karelia.ru>
 * @date   03 March, 2015
 * @brief Implementation of the triple.
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
 * along with SmartSlog; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 *
 *
 * @section DESCRIPTION
 *
 * Implements a functions to work with ontology entities (class, property. individual).
 *
 * This file is part of the SmartSlog project.
 *
 * Copyright (C) 2015 by PetrSU SmartSlog team.
 *
 */

#include "triple.h"
#include "entity_internal.h"

#include "kpi_api.h"
#include "triple_internal.h"

#include "utils/errors_internal.h"
#include "utils/util_func.h"

#include <stdlib.h>

/****************************** Implementation *******************************/
/**************************** External functions *****************************/

/******************** Constructors ('_new_' functions) ***********************/
sslog_class_t *sslog_new_class(const char *uri)
{
    if (sslog_is_str_null_empty(uri) == true) {
        sslog_error_set(NULL, SSLOG_ERROR_INCORRECT_ARGUMENT,
                        SSLOG_ERROR_TEXT_INCORRECT_ARGUMENT "'uri' cannot be null or empty.");
        return NULL;
    }

    sslog_triple_t *triple = sslog_new_triple(uri, SSLOG_TRIPLE_RDF_TYPE, SSLOG_TRIPLE_RDFS_CLASS,
                                              SSLOG_RDF_TYPE_URI, SSLOG_RDF_TYPE_URI);

    sslog_triple_to_internal(triple, int_triple);

    if (int_triple->linked_entity != NULL) {
        return sslog_entity_as_class(int_triple->linked_entity);
    }

    sslog_class_t *sslog_class = (sslog_class_t *) malloc(sizeof(sslog_class_t));

    if (sslog_class == NULL) {
        sslog_error_set(NULL, SSLOG_ERROR_OUT_OF_MEMORY, SSLOG_ERROR_TEXT_OUT_OF_MEMORY);
        return NULL;
    }

    sslog_class->entity.type = SSLOG_ENTITY_CLASS;
    sslog_class->entity.uri = sslog_strndup(uri, SSLOG_ENTITY_URI_MAX_LEN);

    sslog_class->properties = NULL;
    //sslog_class->superclasses = NULL;
   // sslog_class->linked_triple = NULL;

    sslog_entity_set_link(sslog_class_as_entity(sslog_class), int_triple);

    return sslog_class;
}


sslog_property_t *sslog_new_property(const char *uri, sslog_property_type type)
{
    if (sslog_is_str_null_empty(uri) == true) {
        sslog_error_set(NULL, SSLOG_ERROR_INCORRECT_ARGUMENT,
                        SSLOG_ERROR_TEXT_INCORRECT_ARGUMENT "'uri' cannot be null or empty.");
        return NULL;
    }

    // Create or get rdf:type triple for property and return linked entity, then it exists,
    // otherwise crete a new property and link it with individual.
    sslog_triple_t *triple = sslog_new_triple(uri, SSLOG_TRIPLE_RDF_TYPE, SSLOG_TRIPLE_RDF_PROPERTY,
                                              SSLOG_RDF_TYPE_URI, SSLOG_RDF_TYPE_URI);

    sslog_triple_to_internal(triple, int_triple);

    if (int_triple->linked_entity != NULL) {
        return sslog_entity_as_property(int_triple->linked_entity);
    }

    sslog_property_t *property = (sslog_property_t *) malloc(sizeof(sslog_property_t));

    if (property == NULL) {
        sslog_error_set(NULL, SSLOG_ERROR_OUT_OF_MEMORY, SSLOG_ERROR_TEXT_OUT_OF_MEMORY);
        return NULL;
    }

    property->entity.type = SSLOG_ENTITY_PROPERTY;
    property->entity.uri = sslog_strndup(uri, SSLOG_ENTITY_URI_MAX_LEN);

    property->domain = NULL;
    property->maxcardinality = -1;
    property->mincardinality = -1;
	property->type = type;

    sslog_entity_set_link(sslog_property_as_entity(property), int_triple);

    return property;
}


sslog_individual_t *sslog_new_individual(const sslog_class_t *parent_class, const char *uri)
{
    if (parent_class == NULL) {
        sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT, SSLOG_ERROR_TEXT_NULL_ARGUMENT "parent_class");
        return NULL;
    }

    sslog_triple_t *triple = NULL;

    // Genereate an URI if it is not passed and create or get a rdf:type triple for individual
    // based on URI ('new' function does not create a new triple if such triple is in the store).
    if (sslog_is_str_null_empty(uri) == true) {
       char *generated_uri = sslog_generate_uri(parent_class);

       triple = sslog_new_triple(generated_uri, SSLOG_TRIPLE_RDF_TYPE, parent_class->entity.uri,
                                   SSLOG_RDF_TYPE_URI, SSLOG_RDF_TYPE_URI);

       free(generated_uri);
    } else {
       triple = sslog_new_triple(uri, SSLOG_TRIPLE_RDF_TYPE, parent_class->entity.uri,
                                    SSLOG_RDF_TYPE_URI, SSLOG_RDF_TYPE_URI);
    }

    sslog_triple_to_internal(triple, int_triple);

    // Check if the entity (individual) is exist and create a new if it is needed.
    if (int_triple->linked_entity != NULL) {
        return sslog_entity_as_individual( int_triple->linked_entity);
    }

    sslog_individual_t *individual = (sslog_individual_t *) malloc(sizeof(sslog_individual_t));

    if (individual == NULL) {
        sslog_error_set(NULL, SSLOG_ERROR_OUT_OF_MEMORY, SSLOG_ERROR_TEXT_OUT_OF_MEMORY);
        return NULL;
    }

    individual->entity.type = SSLOG_ENTITY_INDIVIDUAL;
    individual->entity.uri = sslog_strndup(triple->subject, SSLOG_ENTITY_URI_MAX_LEN);
    individual->class_uri = sslog_strndup(parent_class->entity.uri, SSLOG_ENTITY_URI_MAX_LEN);

    // Links entity and individual.
    sslog_entity_set_link(sslog_individual_as_entity(individual), int_triple);

    return individual;
}


/***************************** Setters/Getters *******************************/
sslog_entity_type sslog_entity_get_type(sslog_entity_t *entity)
{
    if (entity == NULL) {
        return SSLOG_ENTITY_UNKNOWN;
    }

    return *((sslog_entity_type *) entity);
}


const char *sslog_entity_get_uri(void *entity)
{
    if (entity == NULL) {
        return NULL;
    }

    return ((sslog_entity_t *) entity)->uri;
}


bool sslog_property_is_object(sslog_property_t *property)
{
    if (property == NULL) {
        return false;
    }

    if (property->type == SSLOG_PROPERTY_TYPE_OBJECT) {
        return true;
    }

    return false;
}


void sslog_property_set_domain(sslog_property_t *property, const char *uri)
{
    if (property == NULL) {
        return;
    }

    if (uri == NULL) {
        return;
    }

    property->domain = sslog_strndup(uri, SSLOG_ENTITY_URI_MAX_LEN);
}


/****************************** Implementation *******************************/
/**************************** Internal functions *****************************/

/******************** Constructors ('_new_' functions) ***********************/
sslog_individual_t *sslog_new_individual_detached(const char *class_uri, const char *uri)
{
    if (class_uri == NULL || uri == NULL) {
        return NULL;
    }

    sslog_individual_t *individual = (sslog_individual_t *) malloc(sizeof(sslog_individual_t));

    if (individual == NULL) {
        return NULL;
    }

    individual->entity.type = SSLOG_ENTITY_INDIVIDUAL;
    individual->entity.uri = sslog_strndup(uri, SSLOG_ENTITY_URI_MAX_LEN);
    individual->entity.linked_triple = NULL;
    individual->class_uri = sslog_strndup(class_uri, SSLOG_ENTITY_URI_MAX_LEN);

    return individual;
}


sslog_individual_t *sslog_new_individual_from_triple(sslog_triple_t *triple)
{
    if (triple == NULL) {
        return NULL;
    }

    if (sslog_triple_is_individual(triple) != true) {
        return NULL;
    }

    sslog_triple_to_internal(triple, int_triple);

    if (int_triple->linked_entity != NULL) {
        return sslog_entity_as_individual(int_triple->linked_entity);
    }

    sslog_individual_t *individual = sslog_new_individual_detached(triple->object, triple->subject);

    if (individual == NULL) {
        return NULL;
    }

    sslog_entity_set_link(sslog_individual_as_entity(individual), int_triple);

    return individual;
}


/******************** Destructors ('_free_' functions) ***********************/
void sslog_free_entity(sslog_entity_t *entity)
{
    if (entity == NULL) {
        return;
    }

    if (entity->type == SSLOG_ENTITY_CLASS) {
        sslog_free_class(sslog_entity_as_class(entity));
    } else if (entity->type == SSLOG_ENTITY_PROPERTY) {
        sslog_free_property(sslog_entity_as_property(entity));
    } else if (entity->type == SSLOG_ENTITY_INDIVIDUAL) {
        sslog_free_individual(sslog_entity_as_individual(entity));
    }
}


void sslog_free_individual(sslog_individual_t *individual)
{
    if (individual == NULL) {
        return;
    }

    free(individual->entity.uri);
    free(individual->class_uri);
    individual->entity.linked_triple = NULL;

    free(individual);
}


void sslog_free_class(sslog_class_t *ont_class)
{
    if (ont_class == NULL) {
        return;
    }

    free(ont_class->entity.uri);
    ont_class->entity.linked_triple = NULL;

    list_free_with_nodes(ont_class->properties, LIST_CAST_TO_FREE_FUNC sslog_free_property);

    free(ont_class);
}


void sslog_free_property(sslog_property_t *property)
{
    if (property == NULL) {
        return;
    }

    free(property->entity.uri);
    property->entity.linked_triple = NULL;

    free(property->domain);

    free(property);
}


/***************************** Setters/Getters *******************************/
void sslog_entity_set_link(sslog_entity_t *entity, sslog_internal_triple_t *triple)
{
    entity->linked_triple = (sslog_triple_t *) triple;
    triple->linked_entity = entity;
}


