/**
 * @file property_changes.c
 * @Author Aleksandr A. Lomov <lomov@cs.karelia.ru>
 * @date   4/26/12 7:46 PM
 * @brief  Changes of the property.
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
 * property_changes.c - Functions to manipulate with changes of the property.
 * This file is part of SmartSlog KP Library.
 *
 * Copyright (C) 2012 - SmartSlog Team (Aleksandr A. Lomov). All rights reserved.
 */

#include <stdlib.h>
#include <string.h>

#include "utils/debug.h"
#include "utils/util_func.h"
#include "utils/errors_internal.h"
#include "property_changes.h"
#include "entity_internal.h"

/******************************************************************************/
/*************************** Static functions list ****************************/
static char* prepare_value(sslog_property_t * property, const void *value);
static const char* get_uuid(const sslog_individual_t* individual);



/******************************************************************************/
/***************************** Internal functions *****************************/
/// @cond INTERNAL_FUNCTIONS

/**
 * @brief New data for subscription container.
 *
 * Subscription data contains individual and list of properties or NULL for all
 * properties of individual.
 *
 * @param ind individual.
 * @param properties list of properties for subscribe or NULL for all properties.
 *
 * @return new subscription data on success or NULL otherwise.
 */
property_changes_data_t* new_property_changes(sslog_property_t *property,
        sslog_individual_t *owner,
        const void *old_value,
        const void *new_value,
        sslog_action_type action)
{
    if (property == NULL || owner == NULL) {
        return NULL;
    }

    property_changes_data_t *changes =
            (property_changes_data_t *) malloc(sizeof (property_changes_data_t));

    if (changes == NULL) {
        SSLOG_DEBUG_FUNC(SSLOG_ERROR_TEXT_OUT_OF_MEMORY);
        return NULL;
    }

    changes->property = property;

    const char *uuid = get_uuid(owner);
    //TODO: KPLIB_UUID_MAX_LEN
    changes->owner_uri = sslog_strndup(uuid, 300);

    changes->current_value = prepare_value(property, new_value);
    changes->previous_value = prepare_value(property, old_value);

    changes->action = action;
    
    return changes;
}

/**
 * @brief Free subscription container data.
 *
 * Free subscription data structure, individual and properties not freed. You
 * can free properties list (only list structure will be freed, not properties),
 * using parameter free_prop_list.
 *
 * @param data subscription data for free.
 * @param free_prop_list if it equals true - list structure will be freed, without properties.
 */
void free_property_changes(property_changes_data_t *changes)
{
    free(changes->current_value);
    free(changes->previous_value);
    free(changes->owner_uri);

    changes->current_value = NULL;
    changes->previous_value = NULL;
    changes->owner_uri = NULL;

    free(changes);
}

/// @endcond
/******************************************************************************/


/******************************************************************************/
/***************************** Static functions *******************************/

//TODO: rename to URI
static const char* get_uuid(const sslog_individual_t *individual)
{
    if (individual == NULL) {
        return NULL;
    }

    return individual->entity.uri;
}

static char* prepare_value(sslog_property_t *property, const void *value)
{
    if (value == NULL) {
        return NULL;
    }

    if (sslog_property_is_object(property) !=true) {
        return sslog_strndup((const char *) value, SS_OBJECT_MAX_LEN);
    }

    const char *uuid = get_uuid((const sslog_individual_t *)value);

    if (uuid == NULL) {
        return NULL;
    }

    //TODO: KPLIB_UUID_MAX_LEN
    return sslog_strndup(uuid, 300);

}

