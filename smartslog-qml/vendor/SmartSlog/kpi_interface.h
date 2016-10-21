/**
 * @file    kpi_interface.h
 * @author  Aleksandr A. Lomov <lomov@cs.karelia.ru>
 * @date    23 March, 2015
 * @brief   Describe interface (structures and functions) for Low-level KPI.
 *
 *
 * @section LICENSE
 *
 * SmartSlog is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * SmartSlog distributed in the hope that it will be useful,
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
 * This interface is used to support a low-level API
 * to work with C KPI through SmartSlog.
 * Low-level API is based on triples.
 *
 * This file is part of SmartSlog project.
 *
 * Copyright (C) 2009 PetrSU (SmartSlog team). All rights reserved.
 */



#ifndef _SSLOG_KPI_INTERFACE_H
#define	_SSLOG_KPI_INTERFACE_H

#include "session.h"
#include "utils/debug.h"
#include "utils/list.h"
#include "triple.h"
#include "ckpi/ckpi.h"
#include "high_api.h"


#if !defined(SSLOG_EXTERN)
#if defined(SSLOG_EXPORT)
#define SSLOG_EXTERN __declspec(dllexport)
#elif defined (SSLOG_IMPORT)
#define SSLOG_EXTERN __declspec(dllimport)
#else
#define SSLOG_EXTERN
#endif
#endif


/******************************** Definitions *********************************/
/******************* Defines, structures, constatnts and etc.******************/

#ifdef	__cplusplus
extern "C" {
#endif

/** @brief Describes ifromation for KPI based on C KPI info struct.
 * To simplify implementation there is no standalone info structure
 * for SmartSlog, now it is possible to cast one structure to another.
 */
typedef ss_info_t sslog_kpi_info_t;

typedef ss_subs_info_t sslog_subs_info_t;



/******************************* Definitions **********************************/
/**************************** External functions ******************************/
/// @cond DOXY_EXTERNAL_API



/**
 * @brief Executes the SSAP format insert operation for one triple.
 * @param[in] kpi_info. Information for KPI to process operation.
 * @param[in] triple. Triple to be inserted.
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 */
SSLOG_EXTERN int sslog_kpi_insert_triple(sslog_kpi_info_t *kpi_info, sslog_triple_t *triple);

/**
 * @brief Executes the SSAP format insert operation for triples list.
 * @param[in] kpi_info. Information for KPI to process operation.
 * @param[in] triples. List of triples to be inserted.
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 */
SSLOG_EXTERN int sslog_kpi_insert_triples(sslog_kpi_info_t *kpi_info, list_t *triples);

//TODO: Remove
SSLOG_EXTERN sslog_triple_t* sslog_kpi_new_triple(const char *subject, const char *predicate, const char *object,
                                      sslog_rdf_type object_type);




SSLOG_EXTERN const char *sslog_kpi_triple_get_subject(sslog_triple_t *triple);


SSLOG_EXTERN int sslog_kpi_join(sslog_kpi_info_t *kpi_info, const char *node_id);

SSLOG_EXTERN int sslog_kpi_leave(sslog_kpi_info_t *kpi_info);



SSLOG_EXTERN sslog_kpi_info_t *sslog_new_kpi(const char *ss_id, const char *address, int port);


SSLOG_EXTERN int sslog_kpi_get_error(int kpi_errno);
SSLOG_EXTERN const char *sslog_kpi_get_error_text(int sslog_errno);

/**
 * @brief Executes the SSAP format insert graph in RDF-XML notation operation.
 *
 * @param[in] char* graph. A string that contained graph in RDF-XML notation.
 * @return int status. Status of the operation when completed (0 if successfull,
 *         otherwise < 0).
 */
SSLOG_EXTERN int sslog_kpi_graph_insert(char *graph);


//TODO: implement or not
/**
 * @brief Executes the SSAP format update operation.
 *
 * @param[in] ss_triple_t * inserted_triples. Pointer to the first triple in the triple list to be inserted.
 * @param[in] ss_triple_t * removed_triples. Pointer to the first triple in the triple list to be removed.
 * @return int status. Status of the operation when completed (0 if successfull,
 *         otherwise -1).
 */
//SSLOG_EXTERN int sslog_kpi_update(ss_triple_t *inserted_triples, ss_triple_t *removed_triples);


/**
 * @brief Executes the SSAP format rdf-xml update operation.
 *
 *  Function executes SSAP UPDATE operation using rdf-xml notation.
 *
 * @param[in] ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and
 *            space_id information.
 * @param[in] char * inserted_graph. A pointer to rdf-xml notation for triplets needed to be inserted.
 * @param[in] char * removed_graph. A pointer to rdf-xml notation for triplets that needed to be removed.
 * @return int status. Status of the operation when completed (0 if successfull,
 *         otherwise -1).
 */
SSLOG_EXTERN int sslog_kpi_graph_update(char *inserted_graph, char *removed_graph);


/**
 * @brief Executes the SSAP format remove operation.
 *
 * @param[in] kpi_info. Information about smart space to execute operation.
 * @param[in] triples_templates. Templates to remove triples from the smart space.
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 */
SSLOG_EXTERN int sslog_kpi_remove_triples(sslog_kpi_info_t *kpi_info, list_t *triples_templates);


/**
 * @brief Executes the SSAP format update operation.
 * This function makes a removing and inserting triples as a one atomic operation.
 *
 * @param[in] kpi_info. Information about smart space to execute operation.
 * @param[in] current_triples. List with triples to be deleted from a smart space.
 * @param[in] new_triples. List with triples to be inserted to a smart space.
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 */
SSLOG_EXTERN int sslog_kpi_update_triples(sslog_kpi_info_t *kpi_info, list_t *current_triples, list_t *new_triples);


/**
 * @brief Executes the SSAP format remove graph in RDF-XML notation operation.
 *
 * @param[in] char* graph. A string that contained graph in RDF-XML notation.
 * @return int status. Status of the operation when completed (0 if successfull,
 *         otherwise < 0).
 */
SSLOG_EXTERN int ss_kpi_graph_remove(char * graph);


/// Queries

/**
 * @brief Executes the query operation to smart space.
 *
 * @param[in] ss_triple_t * requested_triples. Pointer to the first triple requested from the SIB.
 * @param[out] ss_triple_t * returned_triples. Pointer to the first triple returned by the SIB.
 *
 * @return int status. Status of the operation when completed (0 if successfull, otherwise -1).
 */
SSLOG_EXTERN int sslog_kpi_query_triple(sslog_kpi_info_t *kpi_info, sslog_triple_t *requested_triple, list_t **returned_triples);

SSLOG_EXTERN int sslog_kpi_query_triples(sslog_kpi_info_t *kpi_info, list_t *requested_triples, list_t **returned_triples);

SSLOG_EXTERN int sslog_kpi_subscribe_triples(sslog_kpi_info_t *kpi_info, sslog_subs_info_t *subs_info, list_t *subscribed_triple, list_t **returned_triples);

int sslog_kpi_subscribe_sparql_select(sslog_kpi_info_t *kpi_info, sslog_subs_info_t *subs_info, const char *select_query, sslog_sparql_result_t **result);

SSLOG_EXTERN int sslog_kpi_unsubscribe(sslog_kpi_info_t *kpi_info, sslog_subs_info_t *subs_info);

SSLOG_EXTERN int sslog_kpi_subscribe_indication(sslog_kpi_info_t *kpi_info, sslog_subs_info_t *subs_info,
                                             list_t **new_triples, list_t **old_triples, int ms_timeout);


int sslog_kpi_sparql_subscribe_indication(sslog_kpi_info_t *kpi_info, sslog_subs_info_t *subs_info,
                                          sslog_sparql_result_t **new_result,
                                          sslog_sparql_result_t **old_result,
                                          int bindings_count,
                                          int ms_timeout);

/**
 * @brief Closes a connection created for subscription without unsubscribing.
 * @param[in] subs_info. Information about subscription.
 */
void sslog_kpi_close_subscription(ss_subs_info_t *subs_info);



//SSLOG_EXTERN int sslog_kpi_query(sslog_triple_t *requested_triples, sslog_triple_t **returned_triples);


/****************************** SPARQL queries *******************************/
/**
  * @brief Creates a GET query for SPARQL-endpoint and executes query operation.
  *
  *  Function composes and sends HTTP-GET with SPARQL SELECT query to the SARQL-endpoint.
  *  Function also parses esponse and returns pointer to SPARQL-results.
  *  Returned triple lists must be freed with the ss_delete_sparql_results() function when no longer needed.
  *  Function supports only xml format of SPARQL results.
  *
  * @param[in] const char *endpoint_url. URL for SPARQL-endpoint, for example "http://dbpedia.org/sparql".
  * @param[in] const char *query. SPARQL SELECT query in text format.
  * @param[in] const char *extra_parameters. Extra parameters for endpoint, for example format=application%2Fxml.
  * @param[out] results. Out structure to store results.
  *
  * @return int status. Status of the operation when completed (0 if successfull, otherwise -1).
  * @see sslog_sparql_result_t
  */
int sslog_kpi_sparql_endpoint_select(const char *endpoint_address, const char *query, const char *extra_parameters,
                                     sslog_sparql_result_t **result);

/**
 * @brief Executes the SSAP format sparql ask query operation.
 *
 * @param[in] kpi_info. Information about smart space to execute operation.
 * @param[in] query. SPARQL ASK query in text format.
 * @param[out] query_result. Result of the query (1 - exists, 0 - not exists, -1 - error).
 *
 * @return int status. SSLOG_ERROR_NO on success or error code otherwise.
 */
int sslog_kpi_sparql_ask_query(sslog_kpi_info_t *kpi_info, const char *query, int *query_result);


/**
  * @brief  Executes the SSAP format sparql select query operation.
  *
  * @param[in] kpi_info. Information about smart space to execute operation.
  * @param[in] query. SPARQL SELECT query in text format.
  * @param[out] results. Out structure to store results..
  *
  * @return int status. Status of the operation when completed (0 if successfull, otherwise -1).
  * @see sslog_sparql_result_t
  */
int sslog_kpi_sparql_select(sslog_kpi_info_t *kpi_info, const char *query, sslog_sparql_result_t **result);


/**
 * @brief  Executes the SSAP format sparql construct query operation.
 *
 * \param[in] char * query. SPARQL CONSRUCT query in text format.
 * \param[out] ss_triple_t ** returned_triples. Pointer to the first triple returned by the SIB.
 *
 * \return int status. Status of the operation when completed (0 if successfull, otherwise -1).
 */
int sslog_kpi_sparql_construct(sslog_kpi_info_t *kpi_info, const char *query, list_t **result_triples);
/*****************************************************************************/


/******************************* Definitions **********************************/
/**************************** internal functions ******************************/
/// @cond DOXY_INTERNAL_API

/**
 * @brief Initializes a KPI.
 * @return error code if the initialization is failed and SSLOG_ERROR_NO otherwise.
 */
int sslog_kpi_init();

/// @endcond


#ifdef	__cplusplus
}
#endif

#endif	/* _SSLOG_KPI_INTERFACE_H */
