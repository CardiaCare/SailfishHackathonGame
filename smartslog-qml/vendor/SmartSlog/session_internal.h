/**
* @author Aleksandr A. Lomov <lomov@cs.karelia.ru>
* @date   11.03.2015
* @brief  Describes session for SmartSlog.
*
* This file is part of the SmartSlog project.
*
* Copyright (C) 2015 by PetrSU.
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
* nodes, subscription and has own information store.
**/

#include "session.h"
#include "triplestore.h"
#include "kpi_interface.h"

#include "utils/errors_internal.h"

#ifndef _SSLOG_SESSION_INTERNAL_H
#define	_SSLOG_SESSION_INTERNAL_H


/**
 * @brief The session struct.
 * The session is used as a context for nodes and triple store.
 * Each session is independent from another sessions.
 * There is default session (@see #g_session_default),
 * this session useful when a KP doesn't use several sessions.
 * All nodes works in the default session if the session
 * explicitly doesn't set to nodes.
 */
struct sslog_session_s {
    list_t nodes;           /**< Session nodes. */
    list_t subscriptions;   /**< Session subscriptions. */
    sslog_store_t *store;   /**< Session information store (triple-store). */
};



struct sslog_node_s {
    char *id;
    char *ss_address;
    char *ss_id;
    int port;
    sslog_kpi_info_t *kpi;
    sslog_session_t *session;
    sslog_error_t last_error;
    list_t subscriptions;
};



sslog_session_t *sslog_session_get_default();


#endif	/* _SSLOG_SESSION_H */
