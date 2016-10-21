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
 * Implementation if the triple and functions for it.
 * This file is part of SmartSlog.
 *
 * Copyright (C) 2015 - SmartSlog. All rights reserved.
 *
 */

#include "triple_internal.h"

#include <stdlib.h>
#include <string.h>

#include "kpi_interface.h"
#include "entity_internal.h"

#include "utils/bool.h"
#include "utils/util_func.h"





bool sslog_triple_is_type(sslog_triple_t *triple)
{
    if (triple == NULL) {
        return false;
    }

    if (strncmp(triple->predicate, SSLOG_TRIPLE_RDF_TYPE, SSLOG_TRIPLE_RDF_TYPE_LEN) == 0) {
        return true;
    }

    return false;
}


bool sslog_triple_is_individual(sslog_triple_t *triple)
{
    if (triple == NULL) {
        return false;
    }

    // Triple must have a type of individual.
    if (strncmp(triple->predicate, SSLOG_TRIPLE_RDF_TYPE, SSLOG_TRIPLE_RDF_TYPE_LEN) != 0) {
        return false;
    }

    // Triple is not describes a property.
    if (strncmp(triple->object, SSLOG_TRIPLE_RDF_PROPERTY, SSLOG_TRIPLE_RDF_PROPERTY_LEN) == 0) {
        return false;
    }

    // Triple is not describe a class.
    if (strncmp(triple->object, SSLOG_TRIPLE_RDFS_CLASS, SSLOG_TRIPLE_RDFS_CLASS_LEN) == 0) {
        return false;
    }

    // Object is uri to class entity.
    if (triple->object_type != SSLOG_RDF_TYPE_URI) {
        return false;
    }

    return true;
}


bool sslog_triple_is_property(sslog_triple_t *triple)
{
    if (triple == NULL) {
        return false;
    }

    if (strncmp(triple->predicate, SSLOG_TRIPLE_RDF_TYPE, SSLOG_TRIPLE_RDF_TYPE_LEN) != 0) {
        return false;
    }

    if (strncmp(triple->object, SSLOG_TRIPLE_RDF_PROPERTY, SSLOG_TRIPLE_RDF_PROPERTY_LEN) != 0) {
        return false;
    }

    return true;
}

bool sslog_triple_is_class(sslog_triple_t *triple)
{
    if (triple == NULL) {
        return false;
    }

    if (strncmp(triple->predicate, SSLOG_TRIPLE_RDF_TYPE, SSLOG_TRIPLE_RDF_TYPE_LEN) != 0) {
        return false;
    }

    if (strncmp(triple->object, SSLOG_TRIPLE_RDFS_CLASS, SSLOG_TRIPLE_RDFS_CLASS_LEN) != 0) {
        return false;
    }

    return true;
}





void sslog_free_triple_force(sslog_triple_t* triple)
{
    if (triple == NULL) {
        return;
    }

    if (sslog_triple_is_property_value(triple) == true) {
        sslog_triple_as_internal(triple)->linked_entity = NULL;
    } else {
        sslog_free_entity(sslog_triple_as_internal(triple)->linked_entity);
    }

    free(triple->subject);
    free(triple->predicate);
    free(triple->object);

    triple->subject_type = SSLOG_RDF_TYPE_INCORRECT;
    triple->object_type = SSLOG_RDF_TYPE_INCORRECT;

    free(triple);
}


bool sslog_triple_is_template(sslog_triple_t *triple)
{
    if (strncmp(triple->subject, SSLOG_TRIPLE_ANY, SSLOG_TRIPLE_ANY_LEN) == 0
            || strncmp(triple->predicate, SSLOG_TRIPLE_ANY, SSLOG_TRIPLE_ANY_LEN) == 0
            || strncmp(triple->object, SSLOG_TRIPLE_ANY, SSLOG_TRIPLE_ANY_LEN) == 0) {
        return true;
    }

    return false;
}





sslog_triple_t* sslog_triple_copy(sslog_triple_t *triple)
{
    if (triple == NULL) {
        return NULL;
    }

    sslog_internal_triple_t *new_triple = (sslog_internal_triple_t *) malloc(sizeof(sslog_internal_triple_t));

    new_triple->linked_entity = NULL;
    new_triple->is_stored = false;
    new_triple->data.subject = sslog_strndup(triple->subject, SSLOG_TRIPLE_SUBJECT_LEN);
    new_triple->data.predicate = sslog_strndup(triple->predicate, SSLOG_TRIPLE_PREDICATE_LEN);
    new_triple->data.object = sslog_strndup(triple->object, SSLOG_TRIPLE_OBJECT_LEN);

    new_triple->data.subject_type = triple->subject_type;
    new_triple->data.object_type = triple->object_type;

    return (sslog_triple_t *) new_triple;
}


bool sslog_triple_is_property_value(sslog_triple_t *triple)
{
    if (triple == NULL) {
        return false;
    }

    // Must not have rdf:type URI.
    if (strncmp(triple->predicate, SSLOG_TRIPLE_RDF_TYPE, SSLOG_TRIPLE_RDF_TYPE_LEN) == 0) {
        return false;
    }

    // Must not have rdfs:Class URI.
    if (strncmp(triple->object, SSLOG_TRIPLE_RDFS_CLASS, SSLOG_TRIPLE_RDFS_CLASS_LEN) == 0) {
        return false;
    }

    // Must not have rdf:Property URI.
    if (strncmp(triple->object, SSLOG_TRIPLE_RDF_PROPERTY, SSLOG_TRIPLE_RDF_PROPERTY_LEN) == 0) {
        return false;
    }

    // Object must be URI or literal.
    if (triple->object_type == SSLOG_RDF_TYPE_BNODE) {
        return false;
    }

    return true;
}



/****************************** Implementation *******************************/
/**************************** Internal functions *****************************/

/******************** Constructors ('_new_' functions) ***********************/
sslog_triple_t *sslog_new_triple_empty()
{
    sslog_internal_triple_t *int_triple = (sslog_internal_triple_t *) malloc(sizeof(sslog_internal_triple_t));

    sslog_triple_t* triple = (sslog_triple_t *) &int_triple->data;

    triple->subject = NULL;
    triple->predicate = NULL;
    triple->object = NULL;

    triple->subject_type = SSLOG_RDF_TYPE_INCORRECT;
    triple->object_type = SSLOG_RDF_TYPE_INCORRECT;
    int_triple->is_stored = false;
    int_triple->linked_entity = NULL;

    return triple;
}

