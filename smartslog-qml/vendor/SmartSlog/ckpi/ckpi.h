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
 * \file ckpi.h
 *
 * \brief API provides low level interface for Knowledge Processors. The interface includes Smart Space discovery and SSAP format transaction operations with the SIB.
 *
 * This implementation supports only hardcoded discovery. Supported transactions are ss_join, ss_leave, ss_insert, ss_update, ss_remove, ss_query (template), ss_subscribe (template) and
 * ss unsubscribe. API provides also ss_add_triple() function for creating the triples used in insert, remove, update, query and subscribe operations. The memory for these triples is allocated
 * dynamically and must be freed by using the ss_delete_triples() function. The triples returned by ss_query, ss_subscribe and ss_subscribe_indiactions are also reserved dynamically and must be free
 * by using the ss_delete_triples() function.
 *
 * KP is able to communicate with the SIB by using plain TCP/IP or NoTA. To select the
 * appropriate protocol modify the makefile variable ACCESS_PROTO. This version of KPI_low is compatible with Smart-M3 SIB release 0.94 (tcp and plain-nota only)
 *
 * TODO: 1. Implement automatic Smart Space discovery.
 *       2. Add sensible values for macros defining buffer lengths.
 *       3. Implement other types of query formats (wilbur etc..)
 *
 * ATTENTION: Because the KPI_low utilises only ANSI C functionality is is not completely thread-safe. When you want to communicate with the SIB with multiple parallel threads you should use own  
 *            ss_info structure for each thread and handle each thread as separate node with unique node ID. Each thread must therefore connect to the SIB separately with a unique node id. Even if you   
 *            are only using threads to evaluate subscribe indications you must still use own ss_info structure that is a copy of the initial ss_info structure.
 *
 * Author: Jussi Kiljander, VTT Technical Research Centre of Finland, SmartSlog Team (Aleksandr A. Lomov - lomov@cs.karelia.ru)
 */

#ifndef CKPI_H
#define CKPI_H

#if defined(WIN32) || defined(WINCE) || defined(CKPI_WIN_EXPORTS)
#define EXTERN __declspec(dllexport)
#else
#define EXTERN
#endif

  /*
*****************************************************************************
*  MACROS
*****************************************************************************
*/

#define DEBUG  (1)

#if DEBUG
  #define SS_DEBUG_PRINT(x, ...) printf(x, ## __VA_ARGS__)
#else
  #define SS_DEBUG_PRINT(x, ...) 
#endif

  /* RDF element types */
#define SS_RDF_TYPE_URI   (1)
#define SS_RDF_TYPE_LIT   (2)
#define SS_RDF_TYPE_BNODE (3)
#define SS_RDF_TYPE_UNBOUND (4)


#define SS_RDF_SIB_ANY    "http://www.nokia.com/NRC/M3/sib#any"


  /* These values should be corrected ones some specs defines them */
#define SS_SUBJECT_MAX_LEN   (1000)
#define SS_PREDICATE_MAX_LEN (1000)
#define SS_OBJECT_MAX_LEN    (1000)

#define SS_RDF_TYPE_MAX_LEN  (8)

#define SS_URI_MAX_LEN       (100)

#define SS_SUB_ID_MAX_LEN    (512)

#define SS_NODE_ID_MAX_LEN   (512)
#define SS_SPACE_ID_MAX_LEN  (512)

#define SS_MAX_MESSAGE_SIZE  (50000) // orig. 4096

#define SS_SIB_DISCOVERY_MESSAGE "SIB_CMD_DISCOVERY"
#define SS_MAX_DISCOVERY_RESPONSE_LEN (512)
//#define SS_MAX_MESSAGE_SIZE  (1000)

#define SS_RECV_TIMEOUT_MSECS (10000)

#define SS_END_TAG "</SSAP_message>"
#define SS_SPARQL_END_TAG "</sparql>"


/* Smart Space discovery message parts (for parsing). */
#define SS_DISCOVERY_PARAM_DELIMETER ","
#define SS_DISCOVERY_PARAM_BLOCK_DELIMETER "|"
#define SS_DISCOVERY_SS_ID "space_id:"
#define SS_DISCOVERY_SS_ID_LEN 9
#define SS_DISCOVERY_IP "sib_ip:"
#define SS_DISCOVERY_IP_LEN 7
#define SS_DISCOVERY_PORT "sib_port:"
#define SS_DISCOVERY_PORT_LEN 9

/* Smart Space discovery information for hard coded discovery */
#define SS_SPACE_ID   "X"
#ifdef ACCESS_NOTA
#define SS_ADDRESS  "10"
#define SS_PORT_N     0
#else
#define SS_ADDRESS  "192.168.112.104"
  //  #define SS_ADDRESS  "192.168.1.3"
#define SS_PORT_N     10010
#define SS_DEFAULT_PORT     10010
#endif
/* SPARQL answer character limit */
#define SS_SPARQL_MAX_CHARACTERS 5000
/* Constant for ASK parsing (true\false)*/
#define SS_ASK_ANSWER 6
  /*
*****************************************************************************
*  DATA TYPES
*****************************************************************************
*/

#ifdef ACCESS_NOTA
//#include "sib_access_nota.h"
#else
#include "sib_access_tcp.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

 /**
   * \struct ss_info
   *
   * \brief A Struct holding Smart Space related information.
   *
   * Currently only one Smart Space is supported.
   * ATTENTION: When using parallel processing remember to use own ss_info structure for each thread, otherwise it is possible that some important data (message, transaction id, etc.)
   * is overwritten in another thread.
   *
   */
  typedef struct ss_info
  {
    int free;
    char node_id[SS_NODE_ID_MAX_LEN];
    char space_id[SS_SPACE_ID_MAX_LEN];
    int transaction_id;

    int socket;
    sib_address_t address;

    char ssap_msg[SS_MAX_MESSAGE_SIZE];
    int ss_errno;

  }ss_info_t;

  /**
   * \struct ss_triple
   *
   * \brief Struct contains RDF format triple.
   *
   * The triples consist of subject, predicate and object. The subject can have type SS_RDF_TYPE_URI or SS_RDF_TYPE_BNODE. Possible types for object are SS_RDF_TYPE_URI,
   * SS_RDF_TYPE_BNODE or SS_RDF_TYPE_LIT.
   *
   */
  typedef struct ss_triple
  {
    char subject[SS_SUBJECT_MAX_LEN];
    char predicate[SS_PREDICATE_MAX_LEN];
    char object[SS_OBJECT_MAX_LEN];
    int subject_type;
    int object_type;

    struct ss_triple *next;
  }ss_triple_t;

  /**
   * \struct ss_sparql_result
   *
   * \brief Struct contains results from SPARQL SELECT query.
   * This structure contains SPARQL SELECT query results. It consists of three arrays for type, value and name of requested data for one result, and link to next result.
   * 
   *
   */
  typedef struct ss_sparql_result
  {
    int * type;
    char ** value;
    char ** name;
    struct ss_sparql_result * next;
  }ss_sparql_result_t;
  
  /**
   * \struct ss_bnode
   *
   * \brief Struct contains "bnode" returned by insert or update operation.
   *
   * Bnodes are presented in label and URI pairs. Label is a string given by the user in insert or update operation.
   * URI is the real URI allocated by the SIB to the resource specified by the label.
   *
   */
  typedef struct ss_bnode
  {
    char label[SS_SUBJECT_MAX_LEN];
    char uri[SS_URI_MAX_LEN];

  }ss_bnode_t;

  /**
   * \struct ss_subs_info
   *
   * \brief Struct contains subscribe info of a subscribe operation.
   *
   */
  typedef struct ss_subs_info
  {
    char id[SS_SUB_ID_MAX_LEN];
    int socket; /* the socket descriptor of the subscribe transaction. */

     multi_msg_t * fmsg;  /* required to keep track of multiple messages */

  }ss_subs_info_t;
	
/*
*****************************************************************************
*  EXPORTED FUNCTION PROTOTYPES
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
  EXTERN ss_info_t** ss_discovery(const char *broadcast_address, int port);


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
  EXTERN int ss_join(ss_info_t * ss_info, char * node_id);

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
  EXTERN int ss_leave(ss_info_t * ss_info);

  /**
   * \fn int ss_query(ss_info_t * ss_info, ss_triple_t * requested_triples, ss_triple_t ** returned_triples)
   *
   * \brief  Executes the SSAP format query operation.
   *
   *  Function composes and send SSAP query messsage to the SIB, whose address information is
   *  found in the ss_info struct. Function also extracts the RDF triples from the query
   *  response. Function returns pointer to the first triple and the triples form a linked
   *  list that can be traversed. The triples to be requested with query operation can be constructed
   *  with ss_add_triple function. Both the requested and returned triple lists must be freed with the ss_delete_triples()
   *  function when no longer needed.
   *
   * \param[in] ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and
   *            space_id information.
   * \param[in] ss_triple_t * first_triple. Pointer to the first triple requested from the SIB.
   * \param[out] ss_triple_t ** returned_triples. Pointer to the first triple returned by the SIB.
   *
   * \return int status. Status of the operation when completed (0 if successfull, otherwise -1).
   */
  EXTERN int ss_query(ss_info_t * ss_info, ss_triple_t * requested_triples, ss_triple_t ** returned_triples);

  /**
   * \fn int ss_sparql_construct_query(ss_info_t * ss_info, char * query, ss_triple_t ** returned_triples)
   *
   * \brief  Executes the SSAP format sparql construct query operation.
   *
   *  Function composes and send SSAP query messsage to the SIB, whose address information is
   *  found in the ss_info struct. Function also extracts the RDF triples from the query
   *  response. Function returns pointer to the first triple and the triples form a linked
   *  list that can be traversed. Query needs to be formed manualy. Returned triple lists must be freed with the ss_delete_triples() function when no longer needed.
   *  If there is an error in query, returned_triples will be NULL.
   *
   * \param[in] ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and
   *            space_id information.
   * \param[in] char * query. SPARQL CONSRUCT query in text format.
   * \param[out] ss_triple_t ** returned_triples. Pointer to the first triple returned by the SIB.
   *
   * \return int status. Status of the operation when completed (0 if successfull, otherwise -1).
   */
EXTERN int ss_sparql_construct_query(ss_info_t * ss_info, char * query, ss_triple_t ** returned_triples);

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
 EXTERN int ss_sparql_ask_query(ss_info_t * ss_info, char * query, int * result);
 
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
 EXTERN int ss_sparql_select_query(ss_info_t * ss_info, char * query, ss_sparql_result_t ** results, int * number_of_bindings);
  
 
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
 EXTERN int ss_sparql_endpoint_query(const char *endpoint_address, const char *query, const char *extra_parameters, ss_sparql_result_t **result, int *number_of_bindings);

  /**
   * \fn int ss_insert(ss_info_t * ss_info, ss_triple_t * first_triple, ss_bnode_t * bnodes)
   *
   * \brief Executes the SSAP format insert operation.
   *
   *  The triples to be inserted to the Smart Space can be constructed with the ss_add_triple() function. The triples must be freed
   *  with ss_delete_triples() when no longer needed. If the triples contain "bnodes" the list of label and allocated URI pairs
   *  is returned. User must reserve enough memory for all "blanodes" she/he is inserting.
   *
   * \param[in] ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and
   *            space_id information.
   * \param[in] ss_triple_t * first_triple. Pointer to the first triple in the triplelist to be inserted.
   * \param[out] ss_bnode_t * bnodes. Pointer to the bnode struct(s), where bnode label and URI are copied.
   * \return int status. Status of the operation when completed (0 if successfull,
   *         otherwise < 0).
   */
  EXTERN int ss_insert(ss_info_t * ss_info, ss_triple_t * first_triple, ss_bnode_t * bnodes);


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
  EXTERN int ss_graph_insert(ss_info_t * ss_info, char * graph);


  /**
   * \fn int ss_update(ss_info_t * ss_info, ss_triple_t * inserted_triples, ss_triple_t * removed_triples, ss_bnode_t * bnodes)
   *
   * \brief Executes the SSAP format update operation.
   *
   *  The triples to be inserted/removed to/from the Smart Space can be constructed with the ss_add_triple() function. The triples must be freed
   *  with ss_delete_triples() when no longer needed. If the triples contain "bnodes" the list of label and allocated URI pairs
   *  is returned. User must reserve enough memory for all "blanodes" she/he is inserting.
   *
   * \param[in] ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and
   *            space_id information.
   * \param[in] ss_triple_t * inserted_triples. Pointer to the first triple in the triple list to be inserted.
   * \param[in] ss_triple_t * removed_triples. Pointer to the first triple in the triple list to be removed.
   * \param[out] ss_bnode_t * bnodes. Pointer to the bnode struct(s), where bnode label and URI are copied.
   * \return int status. Status of the operation when completed (0 if successfull,
   *         otherwise -1).
   */
  EXTERN int ss_update(ss_info_t * ss_info, ss_triple_t * inserted_triples, ss_triple_t * removed_triples, ss_bnode_t * bnodes);


  /**
   * \fn int int ss_graph_update(ss_info_t * ss_info, char * insert_graph, char * removed_graph)
   * \brief Executes the SSAP format rdf-xml update operation.
   *
   *  Function executes SSAP UPDATE operation using rdf-xml notation.
   * 
   * \param[in] ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and
   *            space_id information.
   * \param[in] char * insert_graph. A pointer to rdf-xml notation for triplets needed to be inserted.
   * \param[in] char * removed_graph. A pointer to rdf-xml notation for triplets that needed to be removed.
   * \return int status. Status of the operation when completed (0 if successfull,
   *         otherwise -1).
   */
  EXTERN int ss_graph_update(ss_info_t * ss_info, char * insert_graph, char * removed_graph);
  

  /**
   * \fn int ss_remove(ss_info_t * ss_info, ss_triple_t * removed_triples)
   *
   * \brief Executes the SSAP format remove operation.
   *
   *  The triples to be removed from the Smart Space can be constructed with the ss_add_triple() function. The triples must be freed
   *  with ss_delete_triples() when no longer needed.

   * \param[in] ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and
   *            space_id information.
   * \param[in] ss_triple_t * removed_triples. Pointer to the first triple in the triple list to be removed.
   * \return int status. Status of the operation when completed (0 if successfull,
   *         otherwise -1).
   */
  EXTERN int ss_remove(ss_info_t * ss_info, ss_triple_t * removed_triples);


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
  EXTERN int ss_graph_remove(ss_info_t * ss_info, char * graph);


  /**
   * \fn int ss_subscribe(ss_info_t * ss_info, ss_subs_info_t * subs_info, ss_triple_t * requested_triples, ss_triple_t ** returned_triples)
   *
   * \brief  Executes the SSAP format subsrcibe operation.
   *
   *  Function composes and send SSAP subsrcibe messsage to the SIB, whose address
   *  information is found in the ss_info struct. Function returns the requested information
   *  in triple format. Subscribe / unsubscribe indications can be later checked using ss_subscribe_indication function.
   *  The triples to be requested with subscribe operation can be constructed with ss_add_triple function.
   *  Both the requested and returned triple lists must be freed with the ss_delete_triples() function when no longer needed.
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
  EXTERN int ss_subscribe(ss_info_t * ss_info, ss_subs_info_t * subs_info, ss_triple_t * requested_triples, ss_triple_t ** returned_triples);


  /**
   * \fn int ss_subscribe_indication(ss_info_t * ss_info, ss_subs_info_t * subs_info, ss_triple_t ** new_triples, ss_triple_t ** obsolete_triples, int to_msecs)
   *
   * \brief  Function checks if the subscribe or unsubscribe indication has been received.
   *
   *  This function is used to check whether subscribe or unsubscribe indication has been received. Function waits on the select until a message
   *  is received to the socket or timeout occurs. The timeout value can be given as a parameter. If indication has been received function handles the indication.
   *  The results are passed to user via two pointers called new_triples and obsolete_triples. The memory for these triples is reserved dynamically
   *  and must be freed using ss_delete_triples() function.
   *
   * \param[in] ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and
   *            space_id information.
   * \param[in] ss_subs_info_t * subs_info.
   * \param[out] ss_triple_t ** new_results. Pointer to the first triple of the new result list.
   * \param[out] ss_triple_t ** obsolete_results. Pointer to the first triple of the obsolete result list.
   * \param[in] int to_msecs. Timeout value in milliseconds.
   *
   * \return int status. Unsubscribe: 2
   *                     Subscribe:   1
   *                     Timeout:     0
   *                     Error:      -1
   */
  EXTERN int ss_subscribe_indication(ss_info_t * ss_info, ss_subs_info_t * subs_info, ss_triple_t ** new_triples, ss_triple_t ** obsolete_triples, int to_msecs);


  /**
   * \fn int ss_unsubscribe(ss_info_t * ss_info, ss_subs_info_t * subs_info)
   *
   * \brief  Terminates the SSAP format subsrcibe operation.
   *
   * \param[in] ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and
   *            space_id information.
   * \param[in] char * subscribe_id. ID of the subscribe operation to be terminated.
   *
   * \return int status. Status of the operation when completed (0 if successfull,
   *                     otherwise -1).
   */
  EXTERN int ss_unsubscribe(ss_info_t * ss_info, ss_subs_info_t * subs_info);


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
  EXTERN int ss_sparql_select_subscribe(ss_info_t * ss_info, ss_subs_info_t * subs_info, char * query, ss_sparql_result_t ** returned_results, int * nob);


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
  EXTERN int ss_sparql_select_subscribe_indication(ss_info_t * ss_info, ss_subs_info_t * subs_info, ss_sparql_result_t ** new_results, ss_sparql_result_t ** obsolete_results, int to_msecs);


  /**
   * \fn int ss_add_triple(ss_triple_t ** first_triple, char * subject, char * predicate, char * object, char * rdf_subject_type, char * rdf_object_type)
   *
   * \brief Adds new triple to the triple list pointed by first_triple.
   *
   * Function adds the new triple to the top of the triple list pointed by first_triple. The memory
   * is reserved dynamically and must be freed using ss_delete_triples function. Make sure that the first_triple
   * is not pointing to anything (points to NULL) when you are adding your first triple.
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
  
  EXTERN int ss_add_triple(ss_triple_t ** first_triple, char * subject, char * predicate, char * object, int rdf_subject_type, int rdf_object_type);


  /**
   * \fn void ss_delete_triples(ss_triple_t * first_triple)
   *
   * \brief Deletes all ss_triple struct nodes.
   *
   * \param[in] ss_triple_t * first_triple. A pointer to the first ss_triple struct.
   */
  EXTERN void ss_delete_triples(ss_triple_t * first_triple);


   /**
   * \fn void ss_delete_sparql_results(ss_triple_t * first_triple)
   *
   * \brief Deletes all ss_sparql_result struct nodes.
   *
   * \param[in] ss_sparql_result_t * first_result. A pointer to the first ss_sparql_result struct.
   * \param[in] int number_of_bindings. Number of bindings int the result.
   */
  EXTERN void ss_delete_sparql_results(ss_sparql_result_t * first_result, int number_of_bindings);


  /**
   * \fn void init()
   *
   * \brief Initialization.
   *
   */
#if defined(WIN32) || defined (WINCE)
  EXTERN void init();
#endif

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
            const char *ss_id, const char *ss_address, int ss_port); 


/**
 * \fn void ss_delete_infos(ss_info_t **infos)
 *
 * \brief Deletes NULL-ended list with info structs.
 *
 * \param[in] ss_info_t **info. NULL-ended list with smart space info structures.
 */
EXTERN void ss_delete_infos(ss_info_t **infos);


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
EXTERN int ss_close_subscription(ss_subs_info_t *subs_info);
  
#ifdef __cplusplus
}
#endif

#endif /* CKPI_H */
