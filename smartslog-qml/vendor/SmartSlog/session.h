/**
* @file session.h
* @author Aleksandr A. Lomov <lomov@cs.karelia.ru>
* @date   11.03.2015
* @brief  Describes SmartSlog session.
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
* @section DESCRIPTION
*
* Session is used to manage nodes, subscriptions and store.
* Session implements independent scope for
* nodes, subscription and has own triple store.
* Now several sessions in one KP is not supported.
*
* This file is part of the SmartSlog project.
*
* Copyright (C) 2015 by PetrSU SmartSlog team.
*/


#ifndef _SSLOG_SESSION_H
#define	_SSLOG_SESSION_H

#if !defined(SSLOG_EXTERN)
#if defined(SSLOG_EXPORT)
#define SSLOG_EXTERN __declspec(dllexport)
#elif defined (SSLOG_IMPORT)
#define SSLOG_EXTERN __declspec(dllimport)
#else
#define SSLOG_EXTERN
#endif
#endif

// It is needed only for doxygen, to generate struct docs without
// internal fields and internal *.h files.
#ifdef DOXY_EXTERNAL_ONLY_DATA
/**
 * @brief Contains session's specfic data: nodes, subscriptions, store.
 * @see sslog_session_t
 * @see sslog_node_t
 * @see sslog_subscription_t
 */
struct sslog_session_s {
};

/**
 * @brief Node is used for connection to smart space.
 */
struct sslog_node_s {
};
#endif

/**
* @brief Type that represents a SmartSlog session.
* Each session has own nodes to access smart spaces, subscriptions and store.
*/
typedef struct sslog_session_s sslog_session_t;

/** @brief Type that represents a SmartSlog node. */
typedef struct sslog_node_s sslog_node_t;

#ifdef	__cplusplus
extern "C" {
#endif



/******************************* Definitions **********************************/
/**************************** External functions ******************************/
/// @cond DOXY_EXTERNAL_API

/**
 * @brief Initializes the SmartSlog library.
 * Initialize SmartSlog library and KPI library.
 * Must be used as a first function before other SmartSlog functions.
 * Function is used in pair with #sslog_shutdown().
 *
 * Function sets information about last error (#errors.h).
 *
 * @return error code if the initialization is failed and SSLOG_ERROR_NO otherwise.
 * @see sslog_shutdown()
 */
SSLOG_EXTERN int sslog_init();


/**
 * @brief Shutdown the SmartSlog library.
 * Stops all process in SmartSlog library, KPI library and make cleaning of resources.
 * Function is used in pair with #sslog_shutdown() as a last calling function.
 *
 * After calling this function it is possible to reinit library again with #sslog_init().
 *
 * Function sets information about last error (#errors.h).
 */
SSLOG_EXTERN void sslog_shutdown();


/**
 * @ingroup Constructors-Destructores
 * @brief Creates a new session.
 * Function sets information about last error (#errors.h).
 * @return new session on success or NULL otherwise (get errno with #sslog_error_get_last_code()).
 */
SSLOG_EXTERN sslog_session_t* sslog_new_session();


//TODO: MOVE, make intrnal, open for tests
//void sslog_free(void *pointer);

/**
 * @brief Free session.
 * Functions disconnect all nodes from the smart space, stops subscriptions.
 * All nodes and subscritions will bee deleted too.
 *
 * Function sets information about last error (#errors.h).
 * @param[in] session. Session to free.
 */
SSLOG_EXTERN void sslog_free_session(sslog_session_t *session);

/**
 * @brief Creates a new node in the default session.
 * Function sets information about last error (#errors.h).
 * @param[in] id. Identifier of the node, this id must be uniq in the session.
 * @param[in] ss_id. Smart space identifier.
 * @param[in] address. Address of the SIB to connect.
 * @param[in] port. Port of the SIB to connect.
 * @return new node on success or NULL otherwise (get errno with #sslog_error_get_last_code()).
 * @see sslog_session_t
 */
SSLOG_EXTERN sslog_node_t* sslog_new_node(const char *id, const char *ss_id, const char *address, int port);

/**
 * @brief Creates a new node in the spicific session.
 * Function sets information about last error (#errors.h).
 * @param[in] session. Session of new node.
 * @param[in] id. Identifier of the node, this id must be uniq in the session.
 * @param[in] ss_id. Smart space identifier.
 * @param[in] address. Address of the SIB to connect.
 * @param[in] port. Port of the SIB to connect.
 * @return new node on success or NULL otherwise (get errno with #sslog_error_get_last_code()).
 * @see sslog_session_t
 */
SSLOG_EXTERN sslog_node_t* sslog_new_session_node(sslog_session_t *session, const char *id, const char *ss_id, const char *address, int port);

/**
 * @brief Free session.
 * Functions disconnect all nodes from the smart space, stops subscriptions.
 * All nodes and subscritions will bee deleted too.
 *
 * Function sets information about last error (#errors.h).
 * @param[in] session. Session to free.
 */
SSLOG_EXTERN void sslog_free_node(sslog_node_t *node);

/**
 * @brief Joins to the smart space.
 * Function sets information about last error and node error (#errors.h).
 * @param[in] node. Node to join.
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 */
SSLOG_EXTERN int sslog_node_join(sslog_node_t *node);

/**
 * @brief Leaves the smart space.
 * When the node leaves the smart space then all subscriptions will be unsubscribed.
 *
 * Function sets information about last error and node error (#errors.h).
 * @param[in] node. Node to leave.
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 */
SSLOG_EXTERN int sslog_node_leave(sslog_node_t *node);
/// @endcond


#ifdef	__cplusplus
}
#endif


#endif	/* _SSLOG_SESSION_H */
