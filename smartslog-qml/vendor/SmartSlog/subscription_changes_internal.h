/**
 * @file subscription_changes_internal.h
 * @author Aleksandr A. Lomov <lomov@cs.karelia.ru>
 * @date   Created on April 28, 2012, 5:07 PM
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
 * subscription_changes_internal.h - 
 * <b>internal interface for changes of the subscription.</b> <br />
 * Subscription changes contains iformation about chnages that were perfomed
 * under subscribed data.<br>
 * This file is part of SmartSlog KP Library.
 *
 * <b>Copyright (C) SmartSlog Team (Aleksandr A. Lomov).
 * All rights reserved.</b> <br />
 * Mail-list: smartslog@cs.karelia.ru
 */


#ifndef SUBSCRIPTION_CHANGES_INTERNAL_H
#define	SUBSCRIPTION_CHANGES_INTERNAL_H

#include "subscription_changes.h"
#include "kpi_api.h"
#include "utils/list.h"

#ifdef	__cplusplus
extern "C" {
#endif


/******************************** Definitions ********************************/
/******************* Defines, structures, constatnts and etc.*****************/
/// @cond INTERNAL_STRUCTURES

/**
 * @brief Struct contains information about subscription changes.
 *
 * Every time then indication is received, then subscribed data can be changed.
 * This structure contains information about this changes.
 * It contains individual's UUID, individuals can be inserted, updated and
 * removed from the smart space.
 *
 * Updated individuals are individuals that have
 * changed properties. The property changes are represented with
 * #property_changes_data_s structure.
 *
 * @see subscription_changes_data_t
 */
struct sslog_sbcr_changes_s {
    int sequence_number;           /**< Sequence number of changes. */
    list_t updated_props;          /**< List with updated properties. */
    list_t removed_props;          /**< List with removed properties. */
    list_t inserted_props;         /**< List with inserted(new) properties. */
    list_t updated_individuals;    /**< UUIDs of individuals with changed properties. */
    list_t removed_individuals;    /**< UUIDs of Individuals that were removed from the smart space. */
    list_t inserted_individuals;   /**< UUIDs of Individuals that were inserted to the smart space. */
    list_t inserted_classes;       /**< UUIDs of Individuals that were inserted to the smart space. */
    list_t removed_classes;        /**< UUIDs of Individuals that were inserted to the smart space. */
    list_t inserted_triples;       /**< Triples that were inserted to the smart space. */
    list_t removed_triples;        /**< Triples that were removed from the smart space. */
	list_t updated_triples;		   /**< Triples that were updated in the smart space. */

    sslog_sparql_result_t *new_result;   /**< New result for SPARQL SELECT subscription. */
    sslog_sparql_result_t *old_result;   /**< New result for SPARQL SELECT subscription. */
};

/// @endcond
/*****************************************************************************/


/******************************* Definitions *********************************/
/**************************** Internal functions *****************************/
/// @cond INTERNAL_FUNCTIONS

sslog_sbcr_changes_t* new_subscription_changes();
void free_subscription_changes(sslog_sbcr_changes_t *changes);

void set_sequence_number(sslog_sbcr_changes_t *changes,
        int sequence_number);

void inc_sequence_number(sslog_sbcr_changes_t *changes);


bool add_individual_to_changes(sslog_sbcr_changes_t *changes,
        sslog_individual_t *individual, sslog_action_type action);
bool add_individual_uuid_to_changes(sslog_sbcr_changes_t *changes,
        char *uri, sslog_action_type action);
bool add_property_to_changes(sslog_sbcr_changes_t *changes,
        sslog_individual_t *owner, sslog_property_t* property,
        const void *new_data, const void *old_data,
        sslog_action_type action);


void clean_changes_data(sslog_sbcr_changes_t *changes);

void sslog_sbcr_ch_add_triples(list_t *triples, sslog_action_type action);

/// @endcond
/*****************************************************************************/

#ifdef	__cplusplus
}
#endif

#endif	/* SUBSCRIPTION_CHANGES_INTERNAL_H */

