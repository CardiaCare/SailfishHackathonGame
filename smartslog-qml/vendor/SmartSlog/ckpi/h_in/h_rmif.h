/*
 * Copyright (C) 2009 Nokia Corporation
 * All rights reserved
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * Author: Mikko Hurskainen, 2009
 *
 * $Id$
 */

#ifndef __H_RMIF_H__
#define __H_RMIF_H__

#include "h_in/h_bsdapi.h"

/* maximum length of the certificate */
#define CERT_MAX_LEN 1024

struct h_rmif;
typedef struct h_rmif h_rmif_t;

/*
 * This is the policy for all RM related requests.
 */
typedef enum h_rmif_policy
  {
    /* H_IN will not do any security checking */
    H_RMIF_POLICY_ACCEPT_ALL,
    /* H_IN will validate the requests with RM */
    H_RMIF_POLICY_VALIDATE
  } h_rmif_policy_t;

/* message types */
typedef enum h_rmif_message_type
  {
    RMIF_VALIDATE_REGISTER_ind = 0,
    RMIF_VALIDATE_REGISTER_rsp,
    RMIF_VALIDATE_DEREGISTER_ind,
    RMIF_VALIDATE_DEREGISTER_rsp,
    RMIF_VALIDATE_DISCOVERY_ind,
    RMIF_VALIDATE_DISCOVERY_rsp,
    RMIF_VALIDATE_ACCESS_ind,
    RMIF_VALIDATE_ACCESS_rsp,
    RMIF_SET_POLICY_req,
    RMIF_SET_POLICY_cnf
  } h_rmif_message_type_t;

/* message status */
typedef enum h_rmif_message_status
  {
    RMIF_STATUS_ACCEPT = 0,
    RMIF_STATUS_ACCEPT_REQUIRE_SEC = 1, /* special response for register
					 * validation. Requires access to
					 * service be authenticated
					 */
    RMIF_STATUS_DENY
  } h_rmif_message_status_t;

/* this is a message structure between RM&H_IN */
typedef struct h_rmif_message
{
  /* message type */
  h_rmif_message_type_t type;
  
  /* argument list, reply back as is */
  int source_ia;                     /* only for VALIDATE_*/
  int target_ia;                     /* only for VALIDATE_*/
  int sid_ia;                        /* only for VALIDATE_REGISTER_ind */
  int request_id;                    /* only for VALIDATE_*/
  sid_t sid;                         /* only for VALIDATE_*/
  h_rmif_policy_t policy;            /* only for SET_POLICY_req */  
  int local; /* whether the request is local */
  int remote_connection_id;          /* only for VALIDATE_ACCESS_ind */

  /* reply */
  h_rmif_message_status_t status;    /* only for _cnf & _rsp */

  /* certificate */
  unsigned char cert[CERT_MAX_LEN];  /* only for VALIDATE_*/
  int cert_len;                      /* only for VALIDATE_*/

} h_rmif_message_t;

/* the unix socket to use */
#define H_RMIF_UNIX_SOCKET_PATH "/tmp/h_rmif"

/* requests reference to the rmif instance */
/* the interface should not be used after Hdestroy() command */
h_rmif_t* h_in_get_rmif(h_in_t* h_in);

/* prototope for rmif send function */
typedef int (*h_rmif_send_function)(void* userarg, h_rmif_message_t* msg);

/* sets the query function for sending queries (and resposnes to requests) */
/* the function will be called when H_IN wants to send message to RM */
int h_rmif_set_send_function(h_rmif_t* rmif, h_rmif_send_function func,
			     void* userarg);

/* sends message to the H_IN from RM */
int h_rmif_process_message(h_rmif_t* rmif, h_rmif_message_t* msg);

#endif /*__H_RMIF_H__*/
