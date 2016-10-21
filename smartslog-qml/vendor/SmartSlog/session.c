/**
* @author Aleksandr A. Lomov <lomov@cs.karelia.ru>
* @date   11.03.2015
* @brief  Implementation of the SmartSlog session.
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
* KP can use several session, each session creates independent scope for
* nodes, subscription and has own triple store.
*
* This file is part of the SmartSlog project.
*
* Copyright (C) 2015 by PetrSU SmartSlog team.
**/

#include "session_internal.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "subscription_internal.h"

#include "triplestore.h"
#include "utils/list.h"
#include "utils/util_func.h"
#include "utils/errors_internal.h"

#include "ckpi/ckpi.h"

#include "utils/debug.h"


/******************************** Definitions *********************************/
/**************** Defines, structures, constatnts and etc.*********************/
/**
 * @brief Default session for nodes.
 * Each node works in the session.
 * If the session doesn't set explicitly to node when node works
 * in the default session.
 */
static sslog_session_t *g_session_default = NULL;

/** @brief Contains all sessions and default session (#g_session_default). */
static list_t g_sessions = {NULL, LIST_HEAD_INIT(g_sessions.links)};


/******************************** Definitions *********************************/
/***************************** Static functions *******************************/
static bool sslog_session_has_id(sslog_session_t *session, const char *nodeId);


/****************************** Implementations *********************************/
/***************************** External functions *******************************/
/// @cond DOXY_EXTERNAL_API
sslog_session_t *sslog_new_session()
{
    sslog_error_reset(NULL);

    sslog_session_t *session = (sslog_session_t *) malloc(sizeof(sslog_session_t));

    if (session == NULL) {
        sslog_error_set(NULL, SSLOG_ERROR_OUT_OF_MEMORY, NULL);
        return NULL;
    }

    session->store = sslog_store_new();

    if (session->store == NULL) {
        sslog_error_set(NULL, SSLOG_ERROR_OUT_OF_MEMORY, NULL);
        return NULL;
    }

    INIT_LIST_HEAD(&session->nodes.links);
    INIT_LIST_HEAD(&session->subscriptions.links);

    list_add_data(&g_sessions, session);

    SSLOG_DEBUG_FUNC("Sessions in use: %d\n", list_count(&g_sessions));

    return session;
}


void sslog_free(void *pointer)
{
    char *cpointer  = (char *)pointer;
    cpointer -= sizeof(sslog_object_t);
    sslog_object_t *object = (sslog_object_t *)cpointer;

    if (object->is_detached == false) {
        return;
    }

    if (object->free_func != NULL) {
        object->free_func(object);
    } else {
        free(object);
    }
}


void sslog_free_session(sslog_session_t *session)
{
    if (session == NULL) {
        return;
    }

    sslog_store_free(session->store);

    list_del_and_free_nodes(&session->nodes, LIST_CAST_TO_FREE_FUNC sslog_free_node);
    list_del_and_free_nodes_with_data(&g_sessions, session, NULL);
    free(session);
    SSLOG_DEBUG_FUNC("Sessions in use: %d\n", list_count(&g_sessions));
}

sslog_node_t* sslog_new_session_node(sslog_session_t *session,
                                     const char *id, const char *ss_id, const char *address, int port)
{
    sslog_error_reset(NULL);

    if (session == NULL) {
        sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT,
                        SSLOG_ERROR_TEXT_NULL_ARGUMENT "'session'.");
        return NULL;
    }

    if (sslog_is_str_null_empty(id) == true) {
        sslog_error_set(NULL, SSLOG_ERROR_INCORRECT_ARGUMENT,
                        SSLOG_ERROR_TEXT_INCORRECT_ARGUMENT "'id' is NULL or empty.");
        return NULL;
    }

    if (sslog_is_str_null_empty(ss_id) == true) {
        sslog_error_set(NULL, SSLOG_ERROR_INCORRECT_ARGUMENT,
                        SSLOG_ERROR_TEXT_INCORRECT_ARGUMENT "'ss_id' is NULL or empty.");
        return NULL;
    }

    if (sslog_is_str_null_empty(address) == true) {
        sslog_error_set(NULL, SSLOG_ERROR_INCORRECT_ARGUMENT,
                        SSLOG_ERROR_TEXT_INCORRECT_ARGUMENT "'address' is NULL or empty.");
        return NULL;
    }

    if (port <= 0) {
        sslog_error_set(NULL, SSLOG_ERROR_INCORRECT_ARGUMENT,
                        SSLOG_ERROR_TEXT_INCORRECT_ARGUMENT "'port' is less then 0.");
        return NULL;
    }

    if (sslog_session_has_id(session, id) == true) {
        sslog_error_set(NULL, SSLOG_ERROR_INCORRECT_ARGUMENT,
                        SSLOG_ERROR_TEXT_INCORRECT_ARGUMENT "'id' already is used.");
        return NULL;
    }


    sslog_node_t *node = (sslog_node_t *) malloc(sizeof(sslog_node_t));

    node->session = session;
    node->id = sslog_strndup(id, 255);
    node->ss_id = sslog_strndup(ss_id, 255);
    node->ss_address = sslog_strndup(address, 255);
    node->port = port;

    node->kpi = sslog_new_kpi(ss_id, address, port);

    list_add_data(&node->session->nodes, node);

    INIT_LIST_HEAD(&node->subscriptions.links);

    return node;


}


void sslog_free_node(sslog_node_t *node)
{
    if (node == NULL) {
        return;
    }

    //FIXME:unsubscribe all subscription.
    // Remove all subscription.
    list_del_and_free_nodes(&node->subscriptions, LIST_CAST_TO_FREE_FUNC sslog_free_subscription);

    sslog_error_reset(&node->last_error);

    // Remove node from the session.
    list_del_and_free_nodes_with_data(&node->session->nodes, node, NULL);

    // Prepare node to free.
    node->session = NULL;
    free(node->id);
    free(node->ss_id);
    free(node->ss_address);
    node->port = -1;

    free(node->kpi);

    free(node);
}


sslog_node_t *sslog_new_node(const char *id, const char *ss_id, const char *address, int port)
{
    sslog_error_reset(NULL);

    if (g_session_default == NULL) {
        g_session_default = sslog_new_session();
    }

    if (g_session_default == NULL) {
        sslog_error_set(NULL, SSLOG_ERROR_OUT_OF_MEMORY, SSLOG_ERROR_TEXT_OUT_OF_MEMORY);
        return NULL;
    }

    return sslog_new_session_node(g_session_default, id, ss_id, address, port);
}


int sslog_node_join(sslog_node_t *node)
{
    if (node == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT,
                               SSLOG_ERROR_TEXT_NULL_ARGUMENT "node.'");
    }

    int result = sslog_kpi_join(node->kpi, node->id);

    return sslog_error_set(&node->last_error, result, NULL);
}


int sslog_node_leave(sslog_node_t *node)
{
    if (node == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_NULL_ARGUMENT,
                               SSLOG_ERROR_TEXT_NULL_ARGUMENT "node.'");
    }

    int result = sslog_kpi_leave(node->kpi);

    if (result == SSLOG_ERROR_NO) {
        return sslog_error_reset(&node->last_error);
    }

    return sslog_error_set(&node->last_error, result, sslog_kpi_get_error_text(result));
}



int sslog_init()
{
    // Initialize random generator based on current time.
    time_t t;
    srand((unsigned) time(&t));

    // Create a default session if it is needed
    // and a make initialization for low-level KPI.
    if (g_session_default == NULL) {
        g_session_default = sslog_new_session();
    }

    if (g_session_default == NULL) {
        return sslog_error_set(NULL, SSLOG_ERROR_OUT_OF_MEMORY,
                        SSLOG_ERROR_TEXT_OUT_OF_MEMORY " Can't create the default session.");
    }


    if (sslog_kpi_init() != SSLOG_ERROR_NO) {
        return sslog_error_set(NULL, SSLOG_ERROR_KPI_INIT,
                               SSLOG_ERROR_TEXT_KPI "initializtion failed.");
    }

    return sslog_error_reset(NULL);
}


void sslog_shutdown()
{
//    sslog_store_free(g_session_default->store);
//    list_del_and_free_nodes(&g_session_default->nodes, LIST_CAST_TO_FREE_FUNC sslog_free_node);
//    free(g_session_default);

    //TODO: change asyncronous process to free resources automatically (detached thread?).
    sslog_sbcr_shutdown();

    list_del_and_free_nodes(&g_sessions, LIST_CAST_TO_FREE_FUNC sslog_free_session);
    g_session_default = NULL;
}
/// @endcond
// DOXY_EXTERNAL_API


/****************************** Implementation ********************************/
/**************************** Internal functions ******************************/
sslog_session_t *sslog_session_get_default()
{
    return g_session_default;
}



/****************************** Implementation ********************************/
/***************************** Static functions *******************************/
static bool sslog_session_has_id(sslog_session_t *session, const char *nodeId)
{
    list_head_t *list_walker = NULL;
    list_for_each(list_walker, &session->nodes.links) {
        list_t *node = list_entry(list_walker, list_t, links);
        sslog_node_t *check_node = (sslog_node_t *) node->data;

        if (strncmp(check_node->id, nodeId, 255) == 0) {
            return true;
        }
    }

    return false;
}

