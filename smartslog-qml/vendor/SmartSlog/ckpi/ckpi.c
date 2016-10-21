/*

  ANSI C KPI library is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
 
  ANSI C KPI library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with ANSI C KPI library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor,
  Boston, MA  02110-1301  USA

  Copyright (C) 2012 -  SmartSlog Team (Aleksandr A. Lomov). All rights reserved.
  
  This library is based on KPI_Low sources from VTT, see the copyright below.
  New features, functions or modifications are marked with relevant comments, 
  other source codes are written by VTT.
  KPI_Low project is available here: http://sourceforge.net/projects/kpilow/

  
  VTT Copyrights: 
  Copyright (c) 2009, VTT Technical Research Center of Finland
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.
  * Neither the name of the VTT Technical Research Center of Finland nor the
  names of its contributors may be used to endorse or promote products
  derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS ''AS IS'' AND ANY
  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

/**
 * @file ckpi.c
 *
 * @brief API provides Smart Space discovery and SSAP format transaction operations with
 *  the discovered Smart Space for the KP application.
 *
 * Authors: Jussi Kiljander, VTT Technical Research Centre of Finland, 
 *          SmartSlog Team (Aleksandr A. Lomov - lomov@cs.karelia.ru)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ckpi.h"
#include "compose_ssap_msg.h"
#include "parse_ssap_msg.h"
#include "process_ssap_cnf.h"
#include "sskp_errno.h"

#ifdef ACCESS_NOTA
#include "sib_access_nota.h"
#else
#include "sib_access_tcp.h"
#endif

#if defined(WIN32) || defined (WINCE)
#include "pthread.h"
#endif

/* For SPARQL HTTP-endpoints. */
#define SS_HTTP_PROTOOL "http://"
#define SS_HTTP_SIZE 7
#define SS_HTTP_PORT "80"


/*
*****************************************************************************
*  LOCAL FUNCTION PROTOTYPES
*****************************************************************************
*/

/**
 * \fn int parse_discovery_message(char *message)
 *
 * \brief Parses one discovery message.
 *
 * Discovery message format: space_id:<value>;sib_ip:<value>;sib_port:<value>.
 * This functuin uses 'strtok' and changes source message (parameter).
 *
 * \param[in] char *message. Messae to parse.
 * \return ss_info_t*. Information about smart space on success or NULL otherwise.
 */
static ss_info_t* parse_discovery_message(char *message);


/*
*****************************************************************************
*  EXPORTED FUNCTION IMPLEMENTATIONS
*****************************************************************************
*/


/**
 * \fn ss_ifos_t** ss_discovery(const char *broadcast_address, int port)
 *
 * \brief Discoveres the existing Smart Spaces.
 *
 * Sends a broadcast discovery message to get response from the smart space 
 * with connection information. 
 * Function supports IPv4 networks.  
 *
 * \param[in] const char *broadcast_address. Text representation of a broadcast IP address 
 * to send discovery message. When the parameter is NULL then broadcast ip addreses 
 * of all active network interfaces will be used. 
 * \param[in] int port. Port to send message. When the parameter <= 0 then #SS_DEFAULT_PORT is used.
 * \return ss_ifo_t**. NULL-ended list with information about smart spaces. 
 *  You need to free list with #ss_delete_infos function.
 */
EXTERN ss_info_t** ss_discovery(const char *broadcast_address, int port)
{
    if (port <= 0) 
    {
        port = SS_DEFAULT_PORT;
    }
        
    // Message format: sib_ip:127.0.0.1;sib_port:10010.
	int messages_count = 0;
	char **messages = ss_discovery_sib(broadcast_address, port, &messages_count);
	
    if (messages == NULL) 
    {
        return NULL;
    }
    
    
    const int infos_increment = 5; 
    int infos_max_count = infos_increment;
    int infos_index = 0;
    
    ss_info_t **infos = (ss_info_t **) malloc(sizeof(ss_info_t) * infos_max_count + 1);
   
    for (int i = 0; i < messages_count; ++i) 
    {
        ss_info_t *info = parse_discovery_message(messages[i]);
        free(messages[i]);
        
        
        if (info != NULL && infos_index >= infos_max_count) 
        {
            int tmp_max = infos_max_count;
            infos_max_count += infos_increment;
            ss_info_t **tmp_infos = (ss_info_t **) malloc(sizeof(ss_info_t) * infos_max_count + 1);
            
            memcpy(tmp_infos, infos, sizeof(ss_info_t) * tmp_max);
            
            free(infos);
            
            infos = tmp_infos;
        }
        
        if (info != NULL) 
        {
            infos[infos_index] = info;
            ++infos_index;
        }
    }
    
    infos[infos_index] = NULL;
    
    free(messages);
    
    if (infos_index == 0) {
        free (infos);
        infos = NULL;
    }
    
	return infos;
}


/**
 * \fn int ss_join(ss_info_t * ss_info, char * node_id)
 *
 * \brief Executes the SSAP format join operation.
 *
 * \param[in]  ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and space_id information.
 * \param[in]  char * node_id. ID of your node.

 * \return int status. Status of the operation when completed (0 if successfull,
 *                     otherwise - 1).
 */
EXTERN int ss_join(ss_info_t * ss_info, char * node_id)
{
  ssap_msg_t msg_i;
  int socket,status;

  strcpy(ss_info->node_id, node_id);
  ss_info->transaction_id = 1;
  make_join_msg(ss_info);

  if((socket = ss_open(&(ss_info->address))) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SOCKET_OPEN;
    return -1;
  }
  ss_info->socket = socket;

  if(ss_send(socket, ss_info->ssap_msg) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SOCKET_SEND;
    return -1;
  }
  if((status = ss_recv(socket, ss_info->ssap_msg, SS_RECV_TIMEOUT_MSECS)) <= 0)
  {
    if(status < 0)
      ss_info->ss_errno = SS_ERROR_SOCKET_RECV;
    else
      ss_info->ss_errno = SS_ERROR_RECV_TIMEOUT;

    ss_close(socket);
    return -1;
  
  }

  if(parse_ssap_msg(ss_info->ssap_msg, strlen(ss_info->ssap_msg), &msg_i) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SSAP_MSG_FORMAT;
    return -1;
  }

  if(strcmp("JOIN", msg_i.transaction_type) != 0)
  {
    ss_info->ss_errno = SS_ERROR_TRANSACTION_TYPE;
    return -1;
  }

  return handle_join_response(ss_info, &msg_i);
}

/**
 * \fn int ss_leave(ss_info_t * ss_info)
 *
 * \brief Executes the SSAP format leave operation.
 *
 * \param[in]  ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and
 *             space_id information.
 *
 * \return int status. Status of the operation when completed (0 if successfull,
 *                     otherwise -1).
 */
EXTERN int ss_leave(ss_info_t * ss_info)
{
  ssap_msg_t msg_i;
  int status;

  ss_info->transaction_id++;
  make_leave_msg(ss_info);

  if(ss_send(ss_info->socket, ss_info->ssap_msg) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SOCKET_SEND;
    return -1;
  }
  if((status = ss_recv(ss_info->socket, ss_info->ssap_msg, SS_RECV_TIMEOUT_MSECS)) <= 0)
  {
    if(status < 0)
      ss_info->ss_errno = SS_ERROR_SOCKET_RECV;
    else
      ss_info->ss_errno = SS_ERROR_RECV_TIMEOUT;
    ss_close(ss_info->socket);
    return -1;
  }

  if(ss_close(ss_info->socket) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SOCKET_CLOSE;
    return -1;
  }

  if(parse_ssap_msg(ss_info->ssap_msg, strlen(ss_info->ssap_msg), &msg_i) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SSAP_MSG_FORMAT;
    return -1;
  }

  if(strcmp("LEAVE", msg_i.transaction_type) != 0)
  {
    ss_info->ss_errno = SS_ERROR_TRANSACTION_TYPE;
    return -1;
  }

  return handle_leave_response(ss_info, &msg_i);
}

/**
 * \fn int ss_query(ss_info_t * ss_info, ss_triple_t * requested_triples, ss_triple_t ** returned_triples)
 *
 * \brief  Executes the SSAP format query operation.
 *
 *  Function composes and send SSAP query messsage to the SIB, whose address information is
 *  found in the ss_info struct. Function also extracts the RDF triples from the query
 *  response. Function returns pointer to the first triple and the triples form a linked
 *  list that can be traversed.
 *
 * \param[in] ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and
 *            space_id information.
 * \param[in] ss_triple_t * requested_triples. Pointer to the first triple requested from the SIB.
 * \param[out] ss_triple_t * returned_triples. Pointer to the first triple returned by the SIB.
 *
 * \return int status. Status of the operation when completed (0 if successfull, otherwise -1).
 */
EXTERN int ss_query(ss_info_t * ss_info, ss_triple_t * requested_triples, ss_triple_t ** returned_triples)
{
  ssap_msg_t msg_i;
  int status;

  ss_info->transaction_id++;
  make_query_msg(ss_info, requested_triples);

  if(ss_send(ss_info->socket, ss_info->ssap_msg) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SOCKET_SEND;
    return -1;
  }

  if((status = ss_recv(ss_info->socket, ss_info->ssap_msg, SS_RECV_TIMEOUT_MSECS)) <= 0)
  {
    if(status < 0)
      ss_info->ss_errno = SS_ERROR_SOCKET_RECV;
    else
      ss_info->ss_errno = SS_ERROR_RECV_TIMEOUT;
    return -1;
  }

  if(parse_ssap_msg(ss_info->ssap_msg, strlen(ss_info->ssap_msg), &msg_i) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SSAP_MSG_FORMAT;
    return -1;
  }

  if(strcmp("QUERY", msg_i.transaction_type) != 0)
  {
    ss_info->ss_errno = SS_ERROR_TRANSACTION_TYPE;
    return -1;
  }

  return handle_query_response(ss_info, &msg_i, returned_triples);
}
/**
   * \fn int ss_sparql_construct_query(ss_info_t * ss_info, char * query, ss_triple_t ** returned_triples)
   *
   * \brief  Executes the SSAP format sparql construct query operation.
   *
   *  Function composes and send SSAP query messsage to the SIB, whose address information is
   *  found in the ss_info struct. Function also extracts the RDF triples from the query
   *  response. Function returns pointer to the first triple and the triples form a linked
   *  list that can be traversed. Query needs to be formed manualy. Returned triple lists must be freed with the ss_delete_triples() function when no longer needed.
   *
   * \param[in] ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and
   *            space_id information.
   * \param[in] char * query. SPARQL CONSRUCT query in text format.
   * \param[out] ss_triple_t ** returned_triples. Pointer to the first triple returned by the SIB.
   *
   * \return int status. Status of the operation when completed (0 if successfull, otherwise -1).
   */
 EXTERN int ss_sparql_construct_query(ss_info_t * ss_info, char * query, ss_triple_t ** returned_triples)
{
  ssap_msg_t msg_i;
  int status;
  
  ss_info->transaction_id++;
  make_sparql_msg(ss_info, query);

  if(ss_send(ss_info->socket, ss_info->ssap_msg) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SOCKET_SEND;
    return -1;
  }

  if((status = ss_recv(ss_info->socket, ss_info->ssap_msg, SS_RECV_TIMEOUT_MSECS)) <= 0)
  {
    if(status < 0)
      ss_info->ss_errno = SS_ERROR_SOCKET_RECV;
    else
      ss_info->ss_errno = SS_ERROR_RECV_TIMEOUT;
    return -1;
  }

  if(parse_ssap_msg(ss_info->ssap_msg, strlen(ss_info->ssap_msg), &msg_i) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SSAP_MSG_FORMAT;
    return -1;
  }

  if(strcmp("QUERY", msg_i.transaction_type) != 0)
  {
    ss_info->ss_errno = SS_ERROR_TRANSACTION_TYPE;
    return -1;
  }

  return handle_sparql_construct_response(ss_info, &msg_i, returned_triples);
}


static char *special_convert_sparql_query(const char *sparql_query){
  int i = 0; int j = 0;

  char *result_query = (char *) malloc(strlen(sparql_query)*3*sizeof(char) + 1);
  memset(result_query, 0, sizeof(char)*strlen(sparql_query)*3 + 1);

  while (i < strlen(sparql_query)) 
  {
    if (sparql_query[i] == ' ') 
    { 
      result_query[j] = '+'; 
      j++;
    } 
    else if (sparql_query[i] == '?') 
    { 
      result_query[j] = '%'; 
      result_query[j + 1] = '3'; 
      result_query[j + 2] = 'F';
      j += 3;
    } 
    else if (sparql_query[i] == ':') 
    { 
      result_query[j] = '%';	
      result_query[j + 1] = '3'; 
      result_query[j + 2] = 'A';
      j += 3;
    } 
    else if (sparql_query[i] == '"') 
    { 
      result_query[j] = '%';	
      result_query[j + 1] = '2'; 
      result_query[j + 2] = '2';
      j+=3;
    } 
    else if (sparql_query[i] == '@')
    { 
      result_query[j] = '%';	
      result_query[j + 1] = '4';
      result_query[j + 2] = '0';
      j += 3;
    } 
    else 
    {
      result_query[j] = sparql_query[i]; 
      j++;
    }
    
    i++;
  }

  return result_query;
}


 /**
   * \fn int ss_sparql_endpoint_query(const char *endpoint_address, const char *query, const char *extra_parameters)
   *
   * \brief Creates a GET query for SPARQL-endpoint and executes query operation.
   *
   *  Function composes and sends HTTP-GET with SPARQL SELECT query to the SARQL-endpoint. 
   *  Function also parses esponse and returns pointer to SPARQL-results. 
   *  Returned triple lists must be freed with the ss_delete_sparql_results() function when no longer needed. 
   *  Function supports only xml format of SPARQL results.
   *
   * \param[in] const char *endpoint_url. URL for SPARQL-endpoint, for example "http://dbpedia.org/sparql".
   * \param[in] const char *query. SPARQL SELECT query in text format.
   * \param[in] const char *extra_parameters. Extra parameters for endpoint, for example format=application%2Fxml.
   * \param[out] ss_sparql_result_t **results. Pointer to results structure.
   * \param[out] int *number_of_bindings. Pointer to variable which will contain number of variables returned by endpoint.
   *
   * \return int status. Status of the operation when completed (0 if successfull, otherwise -1).
   */
 EXTERN int ss_sparql_endpoint_query(const char *endpoint_url, const char *query, const char *extra_parameters, ss_sparql_result_t **result, int *number_of_bindings)
 {
	 char *endpoint_query = special_convert_sparql_query(query);
	 char *extra = special_convert_sparql_query(extra_parameters);

	 // 74 for "&format=application%2FxmlHTTP/1.1\r\n\r\n". Chenge it to extra_parameters
	 int query_len = 74 + strlen(endpoint_query) + strlen(endpoint_url) + strlen(extra_parameters) + 1;
	 char *endpoint_request = (char *) malloc(query_len * sizeof(char));
	 endpoint_request[0] = '\0';

	 strcat(endpoint_request, "GET ");
	 strcat(endpoint_request, endpoint_url);
	 strcat(endpoint_request, "?default-graph-uri=&query=");
	 strcat(endpoint_request, endpoint_query);
	 strcat(endpoint_request, extra);
	 strcat(endpoint_request, "HTTP/1.1\r\n\r\n");

	 free(endpoint_query);
	 free(extra);

	 // Make 'dbpedia.org' from 'http://dbpedia.com/search'
	 char *pointer = strstr((char *) endpoint_url, SS_HTTP_PROTOOL);

	 int http_shift = 0;
	 if (pointer != NULL) {
		 http_shift += sizeof(char) * SS_HTTP_SIZE; 
	 }

	 char *address = (char *) malloc(strlen(endpoint_url) * sizeof(char));
	 address[0] = '\0';

	 // Copy string without protocol (http://)
	 strcat(address, endpoint_url + SS_HTTP_SIZE);

	 // Set first '/' to '0' (dbpedia.org/search -> dbpedia.org\0search)
	 pointer = strstr(address, "/");
	 *pointer = '\0';
	 
	 char *buf = (char*) malloc(SS_MAX_MESSAGE_SIZE * sizeof(char));
	
	 int func_res = ss_send_to_address(address, SS_HTTP_PORT, endpoint_request, &buf);

	 if (func_res == 0) {
		 func_res = parse_sparql_xml_result(buf, result, number_of_bindings);
	 }

	 free(endpoint_request);
	 free(address);
	 free(buf);

	 return (func_res == 0) ? 0 : -1;
 }

 
/**
   * \fn int ss_sparql_ask_query(ss_info_t * ss_info, char * query, int * result)
   *
   * \brief  Executes the SSAP format sparql ask query operation.
   *
   *  Function composes and send SSAP query messsage to the SIB, whose address information is
   *  found in the ss_info struct. Query needs to be formed manualy.
   *
   * \param[in] ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and
   *            space_id information.
   * \param[in] char * query. SPARQL ASK query in text format.
   * \param[out] int * result. Pointer to the answer returned by the SIB.(0 if true, 1 if false)
   *
   * \return int status. Status of the operation when completed (0 if successfull, otherwise -1).
   */
 EXTERN int ss_sparql_ask_query(ss_info_t * ss_info, char * query, int * result)
{
  ssap_msg_t msg_i;
  int status;
  
  ss_info->transaction_id++;
  make_sparql_msg(ss_info, query);

  if(ss_send(ss_info->socket, ss_info->ssap_msg) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SOCKET_SEND;
    return -1;
  }

  if((status = ss_recv(ss_info->socket, ss_info->ssap_msg, SS_RECV_TIMEOUT_MSECS)) <= 0)
  {
    if(status < 0)
      ss_info->ss_errno = SS_ERROR_SOCKET_RECV;
    else
      ss_info->ss_errno = SS_ERROR_RECV_TIMEOUT;
    return -1;
  }

  if(parse_ssap_msg(ss_info->ssap_msg, strlen(ss_info->ssap_msg), &msg_i) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SSAP_MSG_FORMAT;
    return -1;
  }

  if(strcmp("QUERY", msg_i.transaction_type) != 0)
  {
    ss_info->ss_errno = SS_ERROR_TRANSACTION_TYPE;
    return -1;
  }

  return handle_sparql_ask_response(ss_info, &msg_i, result);
}
  /**
   * \fn int ss_sparql_select_query(ss_info_t * ss_info, char * query, ss_sparql_result_t ** results, int * number_of_bindings)
   *
   * \brief  Executes the SSAP format sparql select query operation.
   *
   *  Function composes and send SSAP query messsage to the SIB, whose address information is
   *  found in the ss_info struct. Query needs to be formed manualy.
   *
   * \param[in] ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and
   *            space_id information.
   * \param[in] char * query. SPARQL SELECT query in text format.
   * \param[out] ss_sparql_result_t * results. Pointer to results structure.
   * \param[out] int * number_of_bindings. Pointer to variable which will contain number of variables returned by SIB (if nessesary)
   * 
   * \return int status. Status of the operation when completed (0 if successfull, otherwise -1).
   */
int ss_sparql_select_query(ss_info_t * ss_info, char * query, ss_sparql_result_t ** results, int * number_of_bindings)
{
  ssap_msg_t msg_i;
  int status;
  
  ss_info->transaction_id++;
  make_sparql_msg(ss_info, query);

  if(ss_send(ss_info->socket, ss_info->ssap_msg) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SOCKET_SEND;
    return -1;
  }

  if((status = ss_recv(ss_info->socket, ss_info->ssap_msg, SS_RECV_TIMEOUT_MSECS)) <= 0)
  {
    if(status < 0)
      ss_info->ss_errno = SS_ERROR_SOCKET_RECV;
    else
      ss_info->ss_errno = SS_ERROR_RECV_TIMEOUT;
    return -1;
  }

  if(parse_ssap_msg(ss_info->ssap_msg, strlen(ss_info->ssap_msg), &msg_i) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SSAP_MSG_FORMAT;
    return -1;
  }

  if(strcmp("QUERY", msg_i.transaction_type) != 0)
  {
    ss_info->ss_errno = SS_ERROR_TRANSACTION_TYPE;
    return -1;
  }
  
  *number_of_bindings = msg_i.number_of_bindings;
  
  return handle_sparql_select_response(ss_info, &msg_i, results);
}
/**
 * \fn int ss_insert(ss_info_t * ss_info, ss_triple_t * first_triple, ss_bnode_t * bnodes)
 *
 * \brief Executes the SSAP format insert operation.
 *
 * \param[in] ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and
 *            space_id information.
 * \param[in] ss_triple_t * first_triple. Pointer to the first triple in the triple list to be inserted.
 * \param[out] ss_bnode_t * bnodes. Pointer to the bnode struct(s), where bnode label and URI are copied.
 * \return int status. Status of the operation when completed (0 if successfull,
 *         otherwise < 0).
 */
EXTERN int ss_insert(ss_info_t * ss_info, ss_triple_t * first_triple, ss_bnode_t * bnodes)
{
  ssap_msg_t msg_i;
  int status = 0;

  ss_info->transaction_id++;
  make_insert_msg(ss_info, first_triple);

  if(ss_send(ss_info->socket, ss_info->ssap_msg) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SOCKET_SEND;
    return -1;
  }
  if((status = ss_recv(ss_info->socket, ss_info->ssap_msg, SS_RECV_TIMEOUT_MSECS)) <= 0)
  {
    if(status < 0)
      ss_info->ss_errno = SS_ERROR_SOCKET_RECV;
    else
      ss_info->ss_errno = SS_ERROR_RECV_TIMEOUT;
    return -1;
  }

  if(parse_ssap_msg(ss_info->ssap_msg, strlen(ss_info->ssap_msg), &msg_i) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SSAP_MSG_FORMAT;
    return -1;
  }

  if(strcmp("INSERT", msg_i.transaction_type) != 0)
  {
    ss_info->ss_errno = SS_ERROR_TRANSACTION_TYPE;
    return -1;
  }

  return handle_insert_response(ss_info, &msg_i, bnodes);
}

/**
 * \fn int ss_graph_insert(ss_info_t * ss_info, char* graph)
 *
 * \brief Executes the SSAP format insert graph in RDF-XML notation operation.
 *
 * \param[in] ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and
 *            space_id information.
 * \param[in] char* graph. A string that contained graph in RDF-XML notation.
 * \return int status. Status of the operation when completed (0 if successfull,
 *         otherwise < 0).
 */
EXTERN int ss_graph_insert(ss_info_t * ss_info, char * graph){
  ssap_msg_t msg_i;
  int status = 0;

  ss_info->transaction_id++;
  make_graph_insert_msg(ss_info, graph);

  if(ss_send(ss_info->socket, ss_info->ssap_msg) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SOCKET_SEND;
    return -1;
  }
  if((status = ss_recv(ss_info->socket, ss_info->ssap_msg, SS_RECV_TIMEOUT_MSECS)) <= 0)
  {
    if(status < 0)
      ss_info->ss_errno = SS_ERROR_SOCKET_RECV;
    else
      ss_info->ss_errno = SS_ERROR_RECV_TIMEOUT;
    return -1;
  }

  if(parse_ssap_msg(ss_info->ssap_msg, strlen(ss_info->ssap_msg), &msg_i) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SSAP_MSG_FORMAT;
    return -1;
  }

  if(strcmp("INSERT", msg_i.transaction_type) != 0)
  {
    ss_info->ss_errno = SS_ERROR_TRANSACTION_TYPE;
    return -1;
  }

  return handle_graph_insert_response(ss_info, &msg_i);
}

/**
 * \fn int ss_update(ss_info_t * ss_info, ss_triple_t * inserted_triples, ss_triple_t * removed_triples, ss_bnode_t * bnodes)
 *
 * \brief Executes the SSAP format update operation.
 *
 * \param[in] ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and
 *            space_id information.
 * \param[in] ss_triple_t * inserted_triples. Pointer to the first triple in the triple list to be inserted.
 * \param[in] ss_triple_t * removed_triples. Pointer to the first triple in the triple list to be removed.
 * \param[out] ss_bnode_t * bnodes. Pointer to the bnode struct(s), where bnode label and URI are copied.
 * \return int status. Status of the operation when completed (0 if successfull,
 *         otherwise -1).
 */
EXTERN int ss_update(ss_info_t * ss_info, ss_triple_t * inserted_triples, ss_triple_t * removed_triples, ss_bnode_t * bnodes)
{
  ssap_msg_t msg_i;
  int status;

  ss_info->transaction_id++;
  make_update_msg(ss_info, inserted_triples, removed_triples);

  if(ss_send(ss_info->socket, ss_info->ssap_msg) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SOCKET_SEND;
    return -1;
  }

  if((status = ss_recv(ss_info->socket, ss_info->ssap_msg, SS_RECV_TIMEOUT_MSECS)) <= 0)
  {
    if(status < 0)
      ss_info->ss_errno = SS_ERROR_SOCKET_RECV;
    else
      ss_info->ss_errno = SS_ERROR_RECV_TIMEOUT;
    return -1;
  }

  if(parse_ssap_msg(ss_info->ssap_msg, strlen(ss_info->ssap_msg), &msg_i) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SSAP_MSG_FORMAT;
    return -1;
  }

  if(strcmp("UPDATE", msg_i.transaction_type) != 0)
  {
    ss_info->ss_errno = SS_ERROR_TRANSACTION_TYPE;
    return -1;
  }
  int r = handle_update_response(ss_info, &msg_i, bnodes);

  return r;
}

    /**
   * \fn int int ss_graph_update(ss_info_t * ss_info, char * inserted_graph, char * removed_graph)
   * \brief Executes the SSAP format rdf-xml update operation.
   *
   *  Function executes SSAP UPDATE operation using rdf-xml notation.
   * 
   * \param[in] ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and
   *            space_id information.
   * \param[in] char * inserted_graph. A pointer to rdf-xml notation for triplets needed to be inserted.
   * \param[in] char * removed_graph. A pointer to rdf-xml notation for triplets that needed to be removed.
   * \return int status. Status of the operation when completed (0 if successfull,
   *         otherwise -1).
   */
EXTERN int ss_graph_update(ss_info_t * ss_info, char * inserted_graph, char * removed_graph)
{
  ssap_msg_t msg_i;
  int status;

  ss_info->transaction_id++;
  make_graph_update_msg(ss_info, inserted_graph, removed_graph);

  if(ss_send(ss_info->socket, ss_info->ssap_msg) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SOCKET_SEND;
    return -1;
  }

  if((status = ss_recv(ss_info->socket, ss_info->ssap_msg, SS_RECV_TIMEOUT_MSECS)) <= 0)
  {
    if(status < 0)
      ss_info->ss_errno = SS_ERROR_SOCKET_RECV;
    else
      ss_info->ss_errno = SS_ERROR_RECV_TIMEOUT;
    return -1;
  }

  if(parse_ssap_msg(ss_info->ssap_msg, strlen(ss_info->ssap_msg), &msg_i) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SSAP_MSG_FORMAT;
    return -1;
  }

  if(strcmp("UPDATE", msg_i.transaction_type) != 0)
  {
    ss_info->ss_errno = SS_ERROR_TRANSACTION_TYPE;
    return -1;
  }

  return handle_graph_update_response(ss_info, &msg_i);
}
/**
 * \fn int ss_remove(ss_info_t * ss_info, ss_triple_t * removed_triples)
 *
 * \brief Executes the SSAP format remove operation.
 *
 * \param[in] ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and
 *            space_id information.
 * \param[in] ss_triple_t * removed_triples. Pointer to the first triple in the triple list to be removed.
 * \return int status. Status of the operation when completed (0 if successfull,
 *         otherwise -1).
 */
EXTERN int ss_remove(ss_info_t * ss_info, ss_triple_t * removed_triples)
{
  ssap_msg_t msg_i;
  int status;

  ss_info->transaction_id++;
  make_remove_msg(ss_info, removed_triples);

  if(ss_send(ss_info->socket, ss_info->ssap_msg) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SOCKET_SEND;
    return -1;
  }

  if((status = ss_recv(ss_info->socket, ss_info->ssap_msg, SS_RECV_TIMEOUT_MSECS)) <= 0)
  {
    if(status < 0)
      ss_info->ss_errno = SS_ERROR_SOCKET_RECV;
    else
      ss_info->ss_errno = SS_ERROR_RECV_TIMEOUT;
    return -1;
  }

  if(parse_ssap_msg(ss_info->ssap_msg, strlen(ss_info->ssap_msg), &msg_i) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SSAP_MSG_FORMAT;
    return -1;
  }

  if(strcmp("REMOVE", msg_i.transaction_type) != 0)
  {
    ss_info->ss_errno = SS_ERROR_TRANSACTION_TYPE;
    return -1;
  }

  return handle_remove_response(ss_info, &msg_i);
}

/**
 * \fn int ss_graph_remove(ss_info_t * ss_info, char* graph)
 *
 * \brief Executes the SSAP format remove graph in RDF-XML notation operation.
 *
 * \param[in] ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and
 *            space_id information.
 * \param[in] char* graph. A string that contained graph in RDF-XML notation.
 * \return int status. Status of the operation when completed (0 if successfull,
 *         otherwise < 0).
 */
EXTERN int ss_graph_remove(ss_info_t * ss_info, char * graph){
  ssap_msg_t msg_i;
  int status = 0;

  ss_info->transaction_id++;
  make_graph_remove_msg(ss_info, graph);

  if(ss_send(ss_info->socket, ss_info->ssap_msg) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SOCKET_SEND;
    return -1;
  }
  if((status = ss_recv(ss_info->socket, ss_info->ssap_msg, SS_RECV_TIMEOUT_MSECS)) <= 0)
  {
    if(status < 0)
      ss_info->ss_errno = SS_ERROR_SOCKET_RECV;
    else
      ss_info->ss_errno = SS_ERROR_RECV_TIMEOUT;
    return -1;
  }

  if(parse_ssap_msg(ss_info->ssap_msg, strlen(ss_info->ssap_msg), &msg_i) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SSAP_MSG_FORMAT;
    return -1;
  }

  if(strcmp("remove", msg_i.transaction_type) != 0)
  {
    ss_info->ss_errno = SS_ERROR_TRANSACTION_TYPE;
    return -1;
  }

  return handle_graph_remove_response(ss_info, &msg_i);
}

/**
 * \fn int ss_subscribe(ss_info_t * ss_info, ss_subs_info_t * subs_info, ss_triple_t * requested_triples, ss_triple_t ** returned_triples)
 *
 * \brief  Executes the SSAP format subsrcibe operation.
 *
 *  Function composes and send SSAP subsrcibe messsage to the SIB, whose address
 *  information is found in the ss_info struct. Function returns the requested information
 *  in triple format. Subscribe / unsubscribe indications can be later checked using ss_subscribe_indication function.
 *
 * \param[in] ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and
 *            space_id information.
 * \param[in] ss_subs_info_t * subs_info. A pointer to subscribe info struct.
 * \param[in] ss_triple_t * requested_triples. Pointer to the first triple requested from the SIB.
 * \param[out] ss_triple_t ** returned_triples. Pointer to the first triple returned by the SIB.
 *
 * \return int status. Status of the operation when completed (0 if successfull,
 *         otherwise -1).
 */
EXTERN int ss_subscribe(ss_info_t * ss_info, ss_subs_info_t * subs_info, ss_triple_t * requested_triples, ss_triple_t ** returned_triples)
{
  ssap_msg_t msg_i;
  int socket, status;

  ss_info->transaction_id++;
  make_subscribe_msg(ss_info, requested_triples);

  if((socket = ss_open(&(ss_info->address))) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SOCKET_OPEN;
    return -1;
  }

  if(ss_send(socket, ss_info->ssap_msg) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SOCKET_SEND;
    return -1;
  }
  if((status = ss_recv(socket, ss_info->ssap_msg, SS_RECV_TIMEOUT_MSECS)) <= 0)
  {
    if(status < 0)
      ss_info->ss_errno = SS_ERROR_SOCKET_RECV;
    else
      ss_info->ss_errno = SS_ERROR_RECV_TIMEOUT;
    ss_close(socket);
    return -1;
  }

  if(parse_ssap_msg(ss_info->ssap_msg, strlen(ss_info->ssap_msg), &msg_i) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SSAP_MSG_FORMAT;
    return -1;
  }

  if(strcmp("SUBSCRIBE", msg_i.transaction_type) != 0)
  {
    ss_info->ss_errno = SS_ERROR_TRANSACTION_TYPE;
    return -1;
  }

  if((handle_subscribe_response(ss_info, &msg_i, subs_info, returned_triples)) < 0)
    return -1;

  subs_info -> socket = socket;

  return 0;
}

/**
   * \fn int ss_sparql_select_subscribe(ss_info_t * ss_info, ss_subs_info_t * subs_info, char * query, ss_sparql_result_t ** returned_results, int * nob)
   *
   * \brief  Executes the SSAP format subsrcibe operation.
   *
   *  Function composes and send SSAP subsrcibe messsage to the SIB, whose address
   *  information is found in the ss_info struct. Function returns the requested information
   *  in sparql select result format. Subscribe / unsubscribe indications can be later checked using ss_sparql_select_subscribe_indication function.
   *  Returned triple lists must be freed with the ss_delete_sparql_results() function when no longer needed.
   *
   * \param[in] ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and
   *            space_id information.
   * \param[in] ss_subs_info_t * subs_info. A pointer to subscribe info struct.
   * \param[in] char * query. SPARQL SELECT query.
   * \param[out] ss_sparql_result_t ** returned_results. Pointer to the first query result returned by the SIB.
   * \param[out] int * nob. Number of bindings returned by sparql select query.
   *
   * \return int status. Status of the operation when completed (0 if successfull,
   *         otherwise -1).
   */
EXTERN int ss_sparql_select_subscribe(ss_info_t * ss_info, ss_subs_info_t * subs_info, char * query, ss_sparql_result_t ** returned_results, int * nob)
{
  ssap_msg_t msg_i;
  int socket, status;

  ss_info->transaction_id++;
  make_sparql_subscribe_msg(ss_info, query);

  if((socket = ss_open(&(ss_info->address))) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SOCKET_OPEN;
    return -1;
  }

  if(ss_send(socket, ss_info->ssap_msg) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SOCKET_SEND;
    return -1;
  }
  if((status = ss_recv(socket, ss_info->ssap_msg, SS_RECV_TIMEOUT_MSECS)) <= 0)
  {
    if(status < 0)
      ss_info->ss_errno = SS_ERROR_SOCKET_RECV;
    else
      ss_info->ss_errno = SS_ERROR_RECV_TIMEOUT;
    ss_close(socket);
    return -1;
  }

  if(parse_ssap_msg(ss_info->ssap_msg, strlen(ss_info->ssap_msg), &msg_i) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SSAP_MSG_FORMAT;
    return -1;
  }

  if(strcmp("SUBSCRIBE", msg_i.transaction_type) != 0)
  {
    ss_info->ss_errno = SS_ERROR_TRANSACTION_TYPE;
    return -1;
  }

  if((handle_sparql_select_subscribe_response(ss_info, &msg_i, subs_info, returned_results)) < 0)
    return -1;

  subs_info -> socket = socket;
  *nob = msg_i.number_of_bindings;

  return 0;
}


/**
 * \fn int ss_subscribe_indication(ss_info_t * ss_info, ss_subs_info_t * subs_info, ss_triple_t ** new_triples, ss_triple_t ** obsolete_triples, int to_msecs)
 *
 * \brief  Function checks if the subscribe or unsubscribe indication has been received.
 *
 *  This function is used to check whether subscribe or unsubscribe indication has been received. Function waits on the select until a message
 *  is received to the socket or timeout occurs. The timeout value can be given as a parameter. If indication has been received function handles the indication.
 *  The results are passed to user via two pointers called new_triples and obsolete_triples. The memory for these triples is reserved dynamically
 *  and must be freed using ss_delete_triples function.
 *
 * \param[in] ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and
 *            space_id information.
 * \param[in] ss_subs_info_t * subs_info.
 * \param[out] ss_triple_t ** new_results. Pointer to the first triple of the new result list.
 * \param[out] ss_triple_t ** obsolete_results. Pointer to the first triple of the obsolete result list.
 * \param[in] int to_msecs. Timeout value in milliseconds.
 *
 * \return int status. Unsubscribe: 2
 *                     Indication:  1
 *                     Timeout:     0
 *                     Error:      -1
 */

EXTERN int ss_subscribe_indication(ss_info_t * ss_info, ss_subs_info_t * subs_info, ss_triple_t ** new_triples, ss_triple_t ** obsolete_triples, int to_msecs)
{
  ssap_msg_t msg_i;
  int status = -1;
  int offset = 0;
  multi_msg_t * m = NULL;
  multi_msg_t * m_prev = NULL;

  *new_triples = NULL; 
  *obsolete_triples = NULL;
  subs_info->fmsg = NULL;

  if((status = ss_mrecv(&subs_info->fmsg, subs_info->socket, ss_info->ssap_msg, to_msecs)) <= 0)
  {
    if(status < 0)
      ss_info->ss_errno = SS_ERROR_SOCKET_RECV;

    return status;
  }

  for(m = subs_info->fmsg; m; m = m->next)
  {
    if(parse_ssap_msg(&(ss_info->ssap_msg[offset]), m->size, &msg_i) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SSAP_MSG_FORMAT;
      status = -1;
      break;
  }

    offset += m->size;

  if(strcmp("SUBSCRIBE", msg_i.transaction_type) == 0)
  {
    if(handle_subscribe_indication(ss_info, &msg_i, subs_info, new_triples, obsolete_triples) == 0)
      status = 1;
    else
      status = -1;
  }
  else if(strcmp("UNSUBSCRIBE", msg_i.transaction_type) == 0)
  {
    if(handle_unsubscribe_response(ss_info, &msg_i, subs_info) == 0)
    {
      if(ss_close(subs_info->socket) < 0)
      {
        ss_info->ss_errno = SS_ERROR_SOCKET_CLOSE;
          status = -1;
          break;
      }
      
      /*  SmartSlog Team: reset subscription info. */
        subs_info->id[0] = '\0';
        subs_info->socket = -1; 
      
      status = 2;
        break;
    }
    else
      {
      status = -1;
        break;
      }
  }
  else
  {
    ss_info->ss_errno = SS_ERROR_TRANSACTION_TYPE;
    status = -1;
      break;
    }
  }

  m = subs_info->fmsg;
  while(m)
  {
     m_prev = m;
     m = m->next;
     free(m_prev);
  }

  return status;
}
/**
   * \fn int ss_sparql_select_subscribe_indication(ss_info_t * ss_info, ss_subs_info_t * subs_info, ss_sparql_result_t ** new_results, ss_sparql_result_t ** obsolete_results, int to_msecs)
   *
   * \brief  Function checks if the subscribe or unsubscribe indication has been received.
   *
   *  This function is used to check whether subscribe or unsubscribe indication has been received. Function waits on the select until a message
   *  is received to the socket or timeout occurs. The timeout value can be given as a parameter. If indication has been received function handles the indication.
   *  The results are passed to user via two pointers called new_results and obsolete_results. The memory for these query results is reserved dynamically
   *  and must be freed using ss_delete_sparql_results() function.
   *
   * \param[in] ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and
   *            space_id information.
   * \param[in] ss_subs_info_t * subs_info.
   * \param[out] ss_sparql_result_t ** new_results. Pointer to the first query result of the new result list.
   * \param[out] ss_sparql_result_t ** obsolete_results. Pointer to the first query result of the obsolete result list.
   * \param[in] int to_msecs. Timeout value in milliseconds.
   *
   * \return int status. Unsubscribe: 2
   *                     Subscribe:   1
   *                     Timeout:     0
   *                     Error:      -1
   */
EXTERN int ss_sparql_select_subscribe_indication(ss_info_t * ss_info, ss_subs_info_t * subs_info, ss_sparql_result_t ** new_results, ss_sparql_result_t ** obsolete_results, int to_msecs)
{
  ssap_msg_t msg_i;
  int status = -1;
  int offset = 0;
  multi_msg_t * m = NULL;
  multi_msg_t * m_prev = NULL;

  *new_results = NULL; 
  *obsolete_results = NULL;
  subs_info->fmsg = NULL;

  if((status = ss_mrecv(&subs_info->fmsg, subs_info->socket, ss_info->ssap_msg, to_msecs)) <= 0)
  {
    if(status < 0)
      ss_info->ss_errno = SS_ERROR_SOCKET_RECV;

    return status;
  }

  for(m = subs_info->fmsg; m; m = m->next)
  {
    if(parse_ssap_msg(&(ss_info->ssap_msg[offset]), m->size, &msg_i) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SSAP_MSG_FORMAT;
      status = -1;
      break;
  }

    offset += m->size;

  if(strcmp("SUBSCRIBE", msg_i.transaction_type) == 0)
  {
    if(handle_sparql_select_subscribe_indication(ss_info, &msg_i, subs_info, new_results, obsolete_results) == 0)
    {
      status = 1;
    }
    else
    {
      status = -1;
    }
  }
  else if(strcmp("UNSUBSCRIBE", msg_i.transaction_type) == 0)
  {
    if(handle_unsubscribe_response(ss_info, &msg_i, subs_info) == 0)
    {
      if(ss_close(subs_info->socket) < 0)
      {
        ss_info->ss_errno = SS_ERROR_SOCKET_CLOSE;
          status = -1;
          break;
      }
      
      /*  SmartSlog Team: reset subscription info. */
        subs_info->id[0] = '\0';
        subs_info->socket = -1; 
      
      status = 2;
        break;
    }
    else
      {
      status = -1;
        break;
      }
  }
  else
  {
    ss_info->ss_errno = SS_ERROR_TRANSACTION_TYPE;
    status = -1;
      break;
    }
  }

  m = subs_info->fmsg;
  while(m)
  {
     m_prev = m;
     m = m->next;
     free(m_prev);
  }

  return status;
}

/**
 * \fn int ss_unsubscribe(ss_info_t * ss_info, ss_subs_info_t * subs_info)
 *
 * \brief  Terminates the SSAP format subsrcibe operation.
 *
 * \param[in] ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and
 *            space_id information.
 * \param[in] ss_subs_info_t * subs_info. Subscribe to be terminated.
 *
 * \return int status. Status of the operation when completed (0 if successfull,
 *                     otherwise -1).
 */
EXTERN int ss_unsubscribe(ss_info_t * ss_info, ss_subs_info_t * subs_info)
{
  //int socket;

  ss_info->transaction_id++;
  make_unsubscribe_msg(ss_info, subs_info->id);

  /* SIB sends response to the subscribe message to the socket where subscribe
     message was sent. */
  /*
  if((socket = ss_open(&(ss_info->address))) < 0)
  {
     ss_info->ss_errno = SS_ERROR_SOCKET_OPEN;
     return -1;
  }
  */

  if(ss_send(ss_info->socket, ss_info->ssap_msg) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SOCKET_SEND;
    return -1;
  }
  /*
  if(ss_close(socket) < 0)
  {
    ss_info->ss_errno = SS_ERROR_SOCKET_CLOSE;
    return -1;
  }
  */
  return 0;
}


/**
 * \fn int ss_add_triple(ss_triple_t ** first_triple, char * subject, char * predicate, char * object, char * rdf_subject_type, char * rdf_object_type)
 *
 * \brief Adds new triple to the triple list pointed by first_triple.
 *
 * Function adds the new triple to the top of the triple list pointed by first_triple. The memory
 * is reserved dynamically and must be freed using ss_delete_triples function. Make sure that the first_triple
 * is not pointing to anything (points to NULL) when there is no previous triples.
 *
 * \param[in/out]  ss_triple_t ** first_triple. A pointer to the first ss_triple
 *                 struct pointer.
 * \param[in] char * subject.
 * \param[in] char * predicate.
 * \param[in] char * object.
 * \param[in] char * rdf_subject_type. Possible values are macros RDF_TYPE_URI and
 *            RDF_TYPE_BNODE.
 * \param[in] char * rdf_object_type. Possible values are macros RDF_TYPE_URI,
 *            RDF_TYPE_BNODE and RDF_TYPE_LIT.
 *
 * \return int. 0 when successfull, otherwise -1.
 */
EXTERN int ss_add_triple(ss_triple_t ** first_triple, char * subject, char * predicate, char * object, int rdf_subject_type, int rdf_object_type)
{
  ss_triple_t * triple_new = NULL;


  triple_new = (ss_triple_t *)malloc(sizeof(ss_triple_t));

  if(!triple_new)
  {
    SS_DEBUG_PRINT(("ERROR: unable to reserve memory for ss_triple_t\n"));
    return -1;
  }

  strcpy(triple_new->subject, subject);
  strcpy(triple_new->predicate, predicate);
  strcpy(triple_new->object, object);
  triple_new->subject_type = rdf_subject_type;
  triple_new->object_type = rdf_object_type;

  /* Add the new triple to the top */
  triple_new -> next = *first_triple;
  *first_triple = triple_new;

  return 0;
}

/**
 * \fn void ss_delete_triples(ss_triple_t * first_triple)
 *
 * \brief Deletes all ss_triple struct nodes.
 *
 * \param[in] ss_triple_t * first_triple. A pointer to the first ss_triple.
 */
EXTERN void ss_delete_triples(ss_triple_t * first_triple)
{
  ss_triple_t * triple_current = first_triple;
  ss_triple_t * triple_next = NULL;

  while(triple_current)
    {
      triple_next = triple_current->next;
      free(triple_current);
      triple_current = triple_next;
    }

}

/**
 * \fn void ss_delete_sparql_results(ss_triple_t * first_triple)
 *
 * \brief Deletes all ss_sparql_result struct nodes.
 *
 * \param[in] ss_sparql_result_t * first_result. A pointer to the first ss_sparql_result struct.
 * \param[in] int number_of_bindings. Number of bindings int the result.
 */
EXTERN void ss_delete_sparql_results(ss_sparql_result_t * first_result, int number_of_bindings)
{
    ss_sparql_result_t * result_current = first_result;
    ss_sparql_result_t * result_next = NULL;

    while(result_current) {

        int i = 0;
        for (i = 0; i < number_of_bindings; ++i) {
            free(result_current->name[i]);
            free(result_current->value[i]);
        }

        free(result_current->type);
        free(result_current->name);
        free(result_current->value);

        result_next = result_current->next;
        free(result_current);
        result_current = result_next;
    }
}

/*** Functions from the SmartSlog team ***/
/**
 * \fn void init_ss_info()
 *
 * \brief Initialize information about the smart space with given information.
 *
 * \param[in] ss_info_t *info. Structure for initializing.
 * \param[in] const char *ss_id. SmartSpace identifier.
 * \param[in] const char *ss_address. Address of the SmartSpace.
 * \param[in] int ss_port. Port of the SmartSpace.
 */
EXTERN void ss_init_space_info(ss_info_t *info, 
            const char *ss_id, const char *ss_address, int ss_port)
{
    if (info == NULL) {
        return;
    }
 
    info->free = 0;
    info->transaction_id = 0;
    info->socket = 0;
    info->node_id[0] = '\0';
    info->ssap_msg[0] = '\0';
    info->ss_errno = 0;
    
    strncpy(info->space_id, ss_id, SS_SPACE_ID_MAX_LEN);

#ifdef ACCESS_NOTA
    strncpy(info->address.sid, ss_address, MAX_SID_LEN);
#else
    strncpy(info->address.ip, ss_address, MAX_IP_LEN);
#endif

    info->address.port = ss_port;
} 
 

/**
 * \fn void ss_delete_infos(ss_info_t **infos)
 *
 * \brief Deletes NULL-ended list witn info structs.
 *
 * \param[in] ss_info_t **info. NULL-ended list with smart space info structures.
 */
EXTERN void ss_delete_infos(ss_info_t **infos)
{
	if (infos == NULL) 
	{
		return;
	}

	int infos_index = 1;
	ss_info_t *info = *infos;

	while (info != NULL) 
	{
		free(info);
		info = infos[infos_index];
		++infos_index;
	}
    
    free(infos);
}


/**
 * \fn int ss_close_subscription()
 *
 * \brief Closes the subscription.  Use this function to force close subscription.  
 *
 * \param[in] iss_subs_info_t *subs_info. Subscribe to be closed.
 *
 * \return int. 0 if successful or if the subscription is not active, otherwise -1 
 * (error while closing or if the given subscription is null).
 */
EXTERN int ss_close_subscription(ss_subs_info_t *subs_info)
{
    int status = 0;

	if (subs_info == NULL) {
        return -1;
    }

    status =  ss_close(subs_info->socket);
    
    subs_info->id[0] = '\0';
    subs_info->socket = -1;

    return status;
}


#if defined(WIN32) || defined (WINCE)
#if defined(ACCESS_NOTA)
EXTERN void init()
{
  pthread_win32_process_attach_np();
}
#endif
#endif


/**
 * \fn int parse_discovery_message(char *message)
 *
 * \brief Parses one discovery message.
 *
 * Discovery message format: space_id:<value>;sib_ip:<value>;sib_port:<value>.
 * This functuin uses 'strtok' and changes source message (parameter).
 *
 * \param[in] char *message. Messae to parse.
 * \return ss_info_t*. Information about smart space on success or NULL otherwise.
 */
static ss_info_t* parse_discovery_message(char *message) 
{
    char *ip = NULL;
    char *ss_id = NULL;
    int port = 0;
    
    char *token = strtok(message, SS_DISCOVERY_PARAM_DELIMETER);
    
    // Walk through other tokens.
    while (token != NULL) 
    {
        // Get parameters.
        if (strstr(token, SS_DISCOVERY_SS_ID) > 0)
        {
            ss_id = token + SS_DISCOVERY_SS_ID_LEN;
        }
        else if (strstr(token, SS_DISCOVERY_IP) > 0)
        {
            ip = token + SS_DISCOVERY_IP_LEN;
        } 
        else if (strstr(token, SS_DISCOVERY_PORT) > 0) 
        {
            port = atoi(token + SS_DISCOVERY_PORT_LEN);
        }
     
        token = strtok(NULL, SS_DISCOVERY_PARAM_DELIMETER);
    }
    
    if (ss_id != NULL && ip != NULL && port > 0) 
    {
        ss_info_t *info = (ss_info_t *) malloc(sizeof(ss_info_t));
        ss_init_space_info(info, ss_id, ip, port);
        return info;
    }
    
    SS_DEBUG_PRINT("Can't parse discovery message: %s\n", message);
    
    return NULL;
}
