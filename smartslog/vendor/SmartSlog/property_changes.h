/**
 * @file property_changes.h
 * @Author Aleksandr A. Lomov <lomov@cs.karelia.ru>
 * @date   4/26/12 7:06 PM
 * @brief  Interface for changes of the property.
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
 * property_changes.h - interface for changes of the property.
 * This file is part of SmartSlog KP Library.
 *
 * Copyright (C) 2012 - SmartSlog Team (Aleksandr A. Lomov). All rights reserved.
 */


#ifndef PROPERTY_CHANGES_H
#define	PROPERTY_CHANGES_H

#include "entity.h"

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

/******************************************************************************/
/****************************** Structures list *******************************/

/// @cond EXTERNAL_STRUCTURES

/**
 * @struct property_changes_data_s
 * @brief Struct contains information about changes of a property.
 *
 * Property can be inserted(set)/removed/updated for an individual. This struct stores
 * UUID of the property owner (individual's UUID),
 * previous (before perfoming action) and current (after action) values of
 * the property. For the object property values will be stored as UUIDs.
 */
typedef struct property_changes_data_s {
    sslog_property_t *property;   /**< Property that was changed. */
    char *owner_uri;       /**< Individual's UUID that has this property. */
    char *previous_value;   /**< Old value of the property. */
    char *current_value;    /**< Current value of the property. */
    sslog_action_type action;     /**< Action that was perfomed under the property. */
} property_changes_data_t;

/// @endcond

property_changes_data_t* new_property_changes(
        sslog_property_t *property,
        sslog_individual_t *owner,
        const void *old_value,
        const void *new_value,
        sslog_action_type action);
void free_property_changes(property_changes_data_t *changes);

#ifdef	__cplusplus
}
#endif

#endif	/* PROPERTY_CHANGES_H */

