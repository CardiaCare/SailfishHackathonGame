/**
 * @file errors.h
 * @author Alaksandr A. Lomov <lomov@cs.karelia.ru>
 * @date   20 December, 2009
 * @brief  Functions and definitions for error handling.
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
 * SmartSlog stores information about last errors occured in the API functions.
 * It is possible to get information abount error from 'last error' or from 'node error'.
 * When some function is faled then it sets a 'last error', also if a finction
 * works with Node then it sets error information for node ('node error').
 * When the functions works without error it resets error information for the 'last error'
 * and for the 'node error' (if it works with node).
 *
 * This file is part of SmartSlog.
 *
 * Copyright (C) 2009-2015 PetrSU SmatSlog team (Aleksandr A. Lomov).
 */

#include "bool.h"
#include "../session.h" // For sslog_node_t.

#ifndef _SSLOG_ERRORS_H
#define _SSLOG_ERRORS_H


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
	

/******************************* Definitions **********************************/
/******************************* errors codes *********************************/
/// @cond DOXY_EXTERNAL_API
/**
 * @brief Enum with error codes.
 */
enum sslog_errors {
    /* General errors */
    SSLOG_ERROR_NO = 0,             /**< No errors occured. */
    SSLOG_ERROR_UNKNOWN,            /**< Unknown error. */
    SSLOG_ERROR_INCORRECT_ARGUMENT, /**< Passing incorrect argumet to function. */
    SSLOG_ERROR_NULL_ARGUMENT,      /**< Passing NULL argument to function. */
    SSLOG_ERROR_NOT_IMPLEMENTED,    /**< Not implemented error. */
    SSLOG_ERROR_OUT_OF_MEMORY,      /**< Memory allocation error. */
    SSLOG_ERROR_ALREADY_EXISTS,     /**< Some data is already exists. */
    SSLOG_ERROR_NOT_FOUND,          /**< Some data cannot be founs. */

    /* Errors for OWL-entities */
    SSLOG_ERROR_INCORRECT_CLASS = 100,  /**< Incorrect structure of OWL-class. */
    SSLOG_ERROR_INCORRECT_CLASSTYPE,    /**< Class-type (rdf:type) of entiy is incorrect. */
    SSLOG_ERROR_INCORRECT_URI,          /**< Incorrect URI of entity. */
    SSLOG_ERROR_INCORRECT_PROPERTY,     /**< Incorrect OWL property. */
    SSLOG_ERROR_INCORRECT_INDIVIDUAL,   /**< Incorrect individual. */

    /* Cardinality errors */
    SSLOG_ERROR_MINCARDINALITY_VIOLATED = 200,  /**< Individual must have more properties than min cardinality. */
    SSLOG_ERROR_MAXCARDINALITY_VIOLATED,        /**< Individual can't have more properties than max cardinality. */

    /* Subscription errors */
    SSLOG_ERROR_INCORRECT_SUBSCRIPTION = 300,   /**< Incorrect subscription. */
    SSLOG_ERROR_SUBSCRIPTION_IS_INACTIVE,       /**< Subscription has an inactive state. */
    SSLOG_ERROR_SUBSCRIPTION_IS_ACTIVE,         /**< Subscription has an active state. */
    SSLOG_ERROR_INCORRECT_SUBSCRIPTION_TYPE,    /**< Type of subscription is incorrect. */
    SSLOG_ERROR_CANT_SUBSCRIBE,                 /**< Error occure in subscribe process. */
    SSLOG_ERROR_CANT_UNSUBSCRIBE,               /**< Error occure in unsubscribe process. */

    /* Network errors (throws by KPI) */
    SSLOG_ERROR_SOCKET_OPEN = 400,
    SSLOG_ERROR_SOCKET_SEND,
    SSLOG_ERROR_SOCKET_RECV,
    SSLOG_ERROR_RECV_TIMEOUT,
    SSLOG_ERROR_SOCKET_CLOSE,

    /* KPI ERRORS */
    SSLOG_ERROR_KPI = 900,
    SSLOG_ERROR_KPI_UNKNOWN,
    SSLOG_ERROR_KPI_INIT,
    SSLOG_ERROR_KPI_WRONG_SUBSCRIBE_ID,
    SSLOG_ERROR_KPI_NO_SUBSCRIBE_ID,
    SSLOG_ERROR_KPI_TRANSACTION_FAILED,
    SSLOG_ERROR_KPI_UNKNOWN_NODE,
    SSLOG_ERROR_KPI_UNKNOWN_SS,
    SSLOG_ERROR_KPI_SS_ERROR_MESSAGE_TYPE,
    SSLOG_ERROR_KPI_TRANSACTION_TYPE,
    SSLOG_ERROR_KPI_SSAP_MSG_FORMAT
};
/// @endcond


/******************************* Definitions **********************************/
/**************************** External functions *******************************/
/// @cond DOXY_EXTERNAL_API

/**
 * @brief Gets a code of an error that occured in last calling function.
 * @return code of the error or SSLOG_ERROR_NO if the last calling function
 * completes successfully.
 */
SSLOG_EXTERN int sslog_error_get_last_code();

/**
 * @brief Gets a comment text, that describes last error.
 * @return Comment of the error.
 */
SSLOG_EXTERN const char *sslog_error_get_last_text();

/**
 * @brief Gets a code of the node's last error.
 * Node stores information of the error, that is occurred in the last function
 * with this node.
 * @param node. Node to get a last error.
 * @return code of error or SSLOG_ERROR_NO if the last calling function
 * completes successfully.
 * @see sslog_node_t
 */
SSLOG_EXTERN int sslog_error_get_code(sslog_node_t *node);

/**
 * @brief Gets a comment of the node's last error.
 * Node stores information of the error, that is occurred in the last function
 * with this node.
 * @param node. Node to get a comment for the last error.
 * @return comment for the last error.
 * @see sslog_node_t
 */
SSLOG_EXTERN const char* sslog_error_get_text(sslog_node_t *node);

/// @endcond

#ifdef	__cplusplus
}
#endif

#endif /* _SSLOG_ERRORS_H */
