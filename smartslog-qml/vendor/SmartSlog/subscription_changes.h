/**
 * @file subscription_changes.h
 * @author Aleksandr A. Lomov <lomov@cs.karelia.ru>
 * @date   Created on April 28, 2012, 12:56 PM
 * @brief  Interface for changes of the subscription.
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
 * subscription_changes.h - 
 * <b>Interface for changes of the subscription.</b> <br />
 * Subscription changes contains information about chnanges that were perfomed
 * under subscribed data. <br />
 *
 * This file is part of SmartSlog project.
 *
 * Copyright (C) 2015 by PetrSU SmartSlog team.
 */


#include "entity.h"
#include "kpi_api.h"
#include "utils/list.h"

#ifndef SUBSCRIPTION_CHANGES_H
#define	SUBSCRIPTION_CHANGES_H

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

/// @cond DOXY_EXTERNAL_API

// It is needed only for doxygen, to generate stract docs without
// internal fields and internal *.h files.
#ifdef DOXY_EXTERNAL_ONLY_DATA
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
};
#endif

/**
 * @brief Type that represets the subscription changes structure.
 * @see subscription_changes_data_s
 */
typedef struct sslog_sbcr_changes_s sslog_sbcr_changes_t;

/** @endcond */ // DOXY_EXTERNAL_API

/******************************************************************************/
/****************************** Functions list ********************************/
/// @cond DOXY_EXTERNAL_API

/**
 * @brief Gets a suquence number of the changes.
 *
 * Changes are made based on data that were received with notification.
 * This number is sequence of indication that was used to make changes.
 * Firts changes are made while perfoming subscription operation,
 * this changes has '1' as this number.
 *
 * This function sets a global error if it can't perfome operation,
 * you can get an error code using #sslog_get_error_code function.
 *
 * @param[in] changes subscription chnages data structure.
 *
 * @return sequence number of the chnages.
 */
SSLOG_EXTERN int sslog_sbcr_ch_get_sequence_number(sslog_sbcr_changes_t *changes);

/**
 * @brief Gets all individual's URIs by action.
 *
 * Individuals can be inserted, removed or updated. Updated means that some
 * properties of the individual were changed.
 *
 * <b>Do not modify or free returned list. </b>
 *
 * This function sets a global error if it can't perfome operation,
 * you can get an error code using #sslog_get_error_code function.
 *
 * @param[in] changes. Subscription chnages data to get information.
 * @param[in] action. Action that was perfom under individuals: insert, remove,
 * update.
 *
 * @return list with individuals URIs (list with strings) or
 * NULL if given changes or action is incorrect.
 */
SSLOG_EXTERN const list_t *sslog_sbcr_ch_get_individual_by_action(sslog_sbcr_changes_t *changes, sslog_action_type action);

/**
 * @brief Gets triples by action.
 *
 * <b>You need to free the returned list, but without data relesed.</b>
 * Use #list_free_with_nodes with NULL as argument for free function.
 *
 * @param[in] changes. Subscription chnages data to get information.
 * @param[in] action. Action that was perfom under triples: insert, remove,
 * update. It is possible to use bitwise OR "|" operator for action.
 * @return list with triples on success or NULL otherwise.
 */
SSLOG_EXTERN list_t *sslog_sbcr_ch_get_triples(sslog_sbcr_changes_t *changes, sslog_action_type action);

/**
 * @brief Gets all individual's UUID.
 *
 * It make one list with all individual's UUIDs, that are were inserted,
 * updated or removed.
 *
 * <b>You need to free the returned list, but without data relesed.</b>
 * Use #list_free_with_nodes with NULL as argument for free function.
 *
 * <b>Known bug:</b> if under the individual was perfomed more that one actions,
 * for example it was inserted and updated, then UUID will be inserted
 * to list more that once. This will be fixed later.
 *
 * This function sets a global error if it can't perfome operation,
 * you can get an error code using #sslog_get_error_code function.
 *
 * @param[in] changes. Changes to get individuals UUIDs.
 *
 * @return list with UUIDs (list with strings) on success or NULL otherwise.
 */
SSLOG_EXTERN list_t *sslog_sbcr_ch_get_individual_all(sslog_sbcr_changes_t *changes);

/**
 * @brief Gets a new result for SPARQL SELECT subscription.
 * The result contains information with new data (inserted).
 * @param[in] changes Changes to get SELECT result.
 * @return new result on success or NULL otherwise (no information, subscription is not based on SPARQL SELECT).
 */
sslog_sparql_result_t *sslog_sbcr_ch_get_new_sparql_select(sslog_sbcr_changes_t *changes);

/**
 * @brief Gets an old result for SPARQL SELECT subscription.
 * The result contains information with old data (removed).
 * @param[in] changes Changes to get SELECT result.
 * @return old result on success or NULL otherwise (no information, subscription is not based on SPARQL SELECT).
 */
sslog_sparql_result_t *sslog_sbcr_ch_get_old_sparql_select(sslog_sbcr_changes_t *changes);


//FIXME: change implemenattion (based on triples)
/**
 * @brief Gets all changed properties by action.
 *
 * <b>Do not modify or free returned list. </b>
 *
 * This function sets a global error if it can't perfome operation,
 * you can get an error code using #sslog_get_error_code function.
 *
 * @param[in] changes changes to get properties.
 * @param[in] action action that was perfom under properties: insert, remove,
 * update.
 *
 * @return list with properties changes (list with #property_changes_data_s) or
 * NULL if given changes or action is incorrect.
 *
 * @see property_changes_data_s
 */
//SSLOG_EXTERN const list_t *sslog_sbcr_ch_get_property_by_action(sslog_sbcr_changes_t *changes,
//        sslog_action_type action);

//FIXME: change implemenattion (based on triples)
/**
 * @brief Gets all changed properties.
 *
 * <b>You need to free the returned list, but without data relesed.</b>
 * Use #list_free_with_nodes with NULL as argument for free function.
 *
 * This function sets a global error if it can't perfome operation,
 * you can get an error code using #sslog_get_error_code function.
 *
 * @param[in] changes changes to get properties.
 *
 * @return list with properties changes (list with #property_changes_data_s)
 * on success or NULL otherwise.
 *
 * @see property_changes_data_s
 */
//SSLOG_EXTERN list_t *sslog_sbcr_ch_get_property_all(sslog_sbcr_changes_t *changes);


//FIXME: change implemenattion (based on triples)
/**
 * @brief Gets all changed properties by action for individual's UUID.
 *
 * This function can returns list for updated individuals, for other returned
 * list will be empty.
 *
 * <b>You need to free the returned list, but without data relesed.</b>
 * Use #list_free_with_nodes with NULL as argument for free function.
 *
 * This function sets a global error if it can't perfome operation,
 * you can get an error code using #sslog_get_error_code function.
 *
 * @param[in] changes changes to get properties.
 * @param[in] uuid individual's UUID to get properties.
 * @param[in] action action that was perfom under properties: insert, remove,
 * update.
 *
 * @return list with properties changes (list with #property_changes_data_s) or
 * NULL if given changes or action is incorrect.
 *
 * @see property_changes_data_s
 */
//SSLOG_EXTERN list_t *sslog_sbcr_ch_get_property_by_uuid(sslog_sbcr_changes_t *changes,
//        const char *uuid, sslog_action_type action);


//FIXME: change implemenattion (based on triples)
/**
 * @brief Gets all changed properties for individual's UUID.
 *
 * This function can returns list for updated individuals, for other returned
 * list will be empty.
 *
 * <b>You need to free the returned list, but without data relesed.</b>
 * Use #list_free_with_nodes with NULL as argument for free function.
 *
 * This function sets a global error if it can't perfome operation,
 * you can get an error code using #sslog_get_error_code function.
 *
 * @param[in] changes changes to get properties.
 * @param[in] uuid individual's UUID to get properties.
 *
 * @return list with properties changes (list with #property_changes_data_s) or
 * NULL if given changes or action is incorrect.
 *
 * @see property_changes_data_s
 */
//SSLOG_EXTERN list_t *sslog_sbcr_ch_get_property_by_uuid_all(sslog_sbcr_changes_t *changes,
//        const char *uuid);

/**
 * @brief Prints information about changes.
 *
 * It prints sequence number and how manu individuals and properties were
 * inserted, updated or removed.
 *
 * @param[in] changes changes to print information about changes.
 */
SSLOG_EXTERN void sslog_sbcr_ch_print(sslog_sbcr_changes_t *changes);

/** @endcond */ // DOXY_EXTERNAL_API



#ifdef	__cplusplus
}
#endif

#endif	/* SUBSCRIPTION_CHANGES_H */

