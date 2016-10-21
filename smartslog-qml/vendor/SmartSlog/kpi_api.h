/**
 * @file    kpi_api.h
 * @author  Aleksandr A. Lomov <lomov@cs.karelia.ru>
 * @date    10 April, 2015
 * @brief   Describe API functions to implement low-level local functions and access to smart space.
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
 * along with SmartSlog KP Library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 *
 *
 * @section DESCRIPTION
 *
 * This interface is used to support a low-level API
 * to work with KPI through SmartSlog.
 * Low-level API is based on triples.
 *
 * This file is part of SmartSlog Library.
 *
 * Copyright (C) 2009 PetrSU (SmartSlog team). All rights reserved.
 *
 */

#ifndef _SSLOG_KPI_API_H
#define	_SSLOG_KPI_API_H

#include "utils/bool.h"
#include "utils/debug.h"
#include "utils/list.h"
#include "session.h"
#include "triple.h"


#if defined(SSLOG_EXPORT)
#define SSLOG_EXTERN __declspec(dllexport)
#elif defined (SSLOG_IMPORT)
#define SSLOG_EXTERN __declspec(dllimport)
#else
#define SSLOG_EXTERN
#endif


#ifdef	__cplusplus
extern "C" {
#endif


/******************************** Definitions ********************************/
/******************* Defines, structures, constatnts and etc.*****************/
/**
 @brief Struct contains one row of SPARQL SELECT query.
 * Each row contains variables values and types of values.
 */
typedef struct sslog_sparql_result_row_s
{
    int *types;                        /**< Value type of the binding variable. */
    char **values;                     /**< Value of the binding variable. */

} sslog_sparql_result_row_t;


/**
 * @brief Struct contains results from SPARQL SELECT query.
 * The SELECT result is a table. Bnding variables names are as columns
 * and rows are represented with values and types of values.
 */
typedef struct sslog_sparql_result_s
{
  int bindings_count;                   /**< Number of binding variables in the result (columns). */
  int rows_count;                       /**< Number of rows in the result. */
  char **names;                         /**< Names of the binding variable (columns names). */
  sslog_sparql_result_row_t **rows;     /**< Rows with result data. */
} sslog_sparql_result_t;
/*****************************************************************************/


	
/******************************** Definitions ********************************/
/**************************** External functions *****************************/
/// @cond DOXY_EXTERNAL_API
/**
 * @brief Creates a new triple.
 * Created triple is stored in the internal triple-store.
 * This triple is managed by SmartSlog.
 * This function can't be used to create triple-template with 'any value' (#SSLOG_TRIPLE_ANY).
 * Use a #sslog_new_triple_detached function to create a standalone triple or triple-template
 * (such triple does not maneged by SmartSlog and does not stored in the internal triple-store).
 *
 * Function sets information about errors (#errors.h).
 *
 * @param[in] subject. Subject of the triple.
 * @param[in] predicate. Predicate of the triple.
 * @param[in] object. Object of the triple
 * @param[in] object_type. Possible values are macros RDF_TYPE_URI, RDF_TYPE_BNODE and RDF_TYPE_LIT.
 * @return new triple on success or NULL otherwise.
 */
SSLOG_EXTERN sslog_triple_t* sslog_new_triple(const char *subject, const char *predicate, const char *object,
                                              sslog_rdf_type subject_type, sslog_rdf_type object_type);


/**
 * @brief Creates a new standalone triple.
 * Standalone triple does not stored in the internal triple-store and must be deleted manually.
 * This function can create triple-template with 'any value' (#SSLOG_TRIPLE_ANY).
 *
 * Function sets information about errors (#errors.h).
 *
 * @param[in] subject. Subject of the triple.
 * @param[in] predicate. Predicate of the triple.
 * @param[in] object. Object of the triple
 * @param[in] object_type. Possible values are macros RDF_TYPE_URI, RDF_TYPE_BNODE and RDF_TYPE_LIT.
 * @return new triple on success or NULL otherwise.
 */
SSLOG_EXTERN sslog_triple_t* sslog_new_triple_detached(const char *subject, const char *predicate, const char *object,
                                                       sslog_rdf_type subject_type, sslog_rdf_type object_type);


/**
 * @brief Frees memory allocated for triple.
 * This function is safe for stored triples in the local store (does not free it).
 * Stored triples are freed with removing from local store.
 * To free list with triples #sslog_free_triples macros can be used.
 *
 * @param[in] triple. Triple to free.
 * @see #sslog_new_triple_detached
 * @see #sslog_free_triples
 */
SSLOG_EXTERN void sslog_free_triple(sslog_triple_t *triple);


/**
 * @brief Frees memory allocated for SPARQL SELECT result.
 * @param triple. Result to free.
 */
SSLOG_EXTERN void sslog_free_sparql_result(sslog_sparql_result_t *result);


/******************* Functions for operations in smart space ********************/
/**
 * @brief Inserts a triple into a smart space.
 * Function sets information about errors (#errors.h).
 *
 * @param[in] node. Node to process operation.
 * @param[in] triple. Triple to insert.
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 */
SSLOG_EXTERN int sslog_node_insert_triple(sslog_node_t *node, sslog_triple_t *triple);


/**
 * @brief Inserts triples list into a smart space.
 * Function sets information about errors (#errors.h).
 * @param[in] node. Node to process operation.
 * @param[in] triples. List with triples to insert.
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 */
SSLOG_EXTERN int sslog_node_insert_triples(sslog_node_t *node, list_t *triples);


/**
 * @brief Removes a triples from a smart space based on one triple-template.
 * A triple-template a regular triple, but elements of the triple (subject, predicate, object)
 * can be set with '*' mask. The mask means 'any value' (use #SSLOG_TRIPLE_ANY).
 *
 * Function sets information about errors (#errors.h).
 *
 * @param[in] node. Node to process operation.
 * @param[in] triple_template. Template of the triple to remove triples.
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 */
SSLOG_EXTERN int sslog_node_remove_triple(sslog_node_t *node, sslog_triple_t *triple_template);


/**
 * @brief Removes a triples from a smart space based on triple-templates list.
 * A triple-template a regular triple, but elements of the triple (subject, predicate, object)
 * can be set with '*' mask. The mask means 'any value' (use #SSLOG_TRIPLE_ANY).
 *
 * Function sets information about errors (#errors.h).
 *
 * @param[in] node. Node to process operation.
 * @param[in] triple_templates. Templates to remove triples.
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 */
SSLOG_EXTERN int sslog_node_remove_triples(sslog_node_t *node, list_t *triple_templates);


/**
 * @brief Queries triples from a smart space based on one triple-template.
 * A triple-template a regular triple, but elements of the triple (subject, predicate, object)
 * can be set with '*' mask. The mask means 'any value' (use #SSLOG_TRIPLE_ANY).
 *
 * Function sets information about errors (#errors.h).
 *
 * @param[in] node. Node to process operation.
 * @param[in] triple_template. Template of the triple to query triples.
 * @return list with founded triples or NULL otherwise.
 */
SSLOG_EXTERN list_t* sslog_node_query_triple(sslog_node_t *node, sslog_triple_t *triple_template);


/**
 * @brief Queries triples from a smart space based on triple-templates list.
 * A triple-template a regular triple, but elements of the triple (subject, predicate, object)
 * can be set with '*' mask. The mask means 'any value' (use #SSLOG_TRIPLE_ANY).
 *
 * Function sets information about errors (#errors.h).
 *
 * @param[in] node. Node to process operation.
 * @param[in] triple_templates. Templates of the triples to query triples.
 * @return list with founded triples or NULL otherwise.
 */
SSLOG_EXTERN list_t *sslog_node_query_triples(sslog_node_t *node, list_t *triple_templates);


/**
 * @brief Update one triple in the smart space and locally.
 * If the current_triple is stored in the internal triple-store,
 * then it will be deleted and can not be used anymore.
 * If the current_triple is detached (#sslog_new_triple_detached),
 * then the function does not delete it.
 * New triple will be stored in the internal triple-store.
 *
 * Function sets information about errors (#errors.h).
 *
 * @param[in] node. Node to process operation.
 * @param[in] current_triple. Current triple for updating.
 * @param[in] new_triple. New triple for updating.
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 */
SSLOG_EXTERN int sslog_node_update_triple(sslog_node_t *node, sslog_triple_t *current_triple, sslog_triple_t *new_triple);


/**
 * @brief Update triples in the smart space and locally.
 * If the current_triples is stored in the internal triple-store,
 * then it will be deleted and can not be used anymore.
 * If the current_triples are detached (#sslog_new_triple_detached),
 * then the function does not delete it.
 * New triples will be stored in the internal triple-store.
 *
 * Function sets information about errors (#errors.h).
 *
 * @param[in] node. Node to process operation.
 * @param[in] remove_triples. List with triples or triple-templates for removing in update operation.
 * @param[in] insert_triples. List with triples for inserting in update operation.
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 */
SSLOG_EXTERN int sslog_node_update_triples(sslog_node_t *node, list_t *current_triples, list_t *new_triples);


// TODO: comment
// TODO: remove
/**
 * @brief Subscribes to track changes of triples in the smart space based on triple-templates list.
 * The operation works as a query, but it stores the query (triples-templates) on the smart space side
 * and then the data has changed
 * A triple-template a regular triple, but elements of the triple (subject, predicate, object)
 * can be set with '*' mask. The mask means 'any value' (use #SSLOG_TRIPLE_ANY).
 *
 * Function sets information about errors (#errors.h).
 *
 * @param[in] node. Node to process operation.
 * @param[in] triple_templates. Templates of the triples to subscribe.
 * @return SSLOG_ERROR_NO on success or error code otherwise.
 */
SSLOG_EXTERN list_t *sslog_node_subscribe_triples(sslog_node_t *node, list_t *triples_templates);
SSLOG_EXTERN list_t *sslog_node_subscribe_triple(sslog_node_t *node, sslog_triple_t *triple_template);
/*****************************************************************************/


/******************** Node functions for SPARQL queries **********************/
/**
 * @brief Performs SPARQL SELECT query to SPARQL-endpoint.
 * Returns all, or a subset of, the variables bound in a query pattern match.
 * Values  of variables are returned as a table there columns are variables and
 * rows contains values for variables.
 *
 * To store triples in the local store it is need to set vriables for triples
 * (simple form of CONSTRUCT).
 * For example 5 variables are selected: 'SELECT ?a ?b ?c ?d ?e ...' and you wont to
 * store triples by using variables '?a ?c ?d' and '?b ?c ?e'. Set triples_template
 * parameters as 'a c d . b c e' (dot is used as a separator of triples).
 * In this way function gets variables from each row and constructs 2 triples to store.
 *
 * Function sets information about errors (#errors.h).
 *
 * @param[in] endpoint_url. URL for SPARQL-endpoint, for example "http://dbpedia.org/sparql".
 * @param[in] query. SPARQL SELECT query in text format.
 * @param[in] extra_parameters. Extra parameters for endpoint, for example format=application%2Fxml.
 * @param[in] triples_template. Template to construct and store triples from vriables
 * in a form '?a ?b ?c . ?d ?e ?f . ?q ?w ?e'. If the parameter is NULL then
 * no triples will be constructed and stored in the local store.
 *
 * @return result for SELECT query on success and NULL otherwise (on error).
 * @see sslog_sparql_result_t
 */
sslog_sparql_result_t* sslog_node_sparql_enpoint_select(sslog_node_t *node, const char *endpoint_address, const char *query, const char *extra_parameters, const char *triples_template);


/**
 * @brief Performs SPARQL ASK query.
 * ASK form of SPARQL query tests whether or not a query pattern has a solution.
 * No information is returned about the possible query solutions, just whether
 * or not a solution exists.
 *
 * Function sets information about errors (#errors.h).
 *
 * @param[in] node. Node to process operation.
 * @param query. String with ASK query form.
 * @return true if data exists in the smart space or false otherwise (error, not exists).
 */
SSLOG_EXTERN bool sslog_node_sparql_ask(sslog_node_t *node, const char *query);

//FIXME: Commnent parameters.
/**
 * @brief Performs SPARQL SELECT query.
 * Returns all, or a subset of, the variables bound in a query pattern match.
 * Values  of variables are returned as a table there columns are variables and
 * rows contains values for variables.
 *
 * To store triples in the local store it is need to set vriables for triples
 * (simple form of CONSTRUCT).
 * For example 5 variables are selected: 'SELECT ?a ?b ?c ?d ?e ...' and you wont to
 * store triples by using variables '?a ?c ?d' and '?b ?c ?e'. Set triples_template
 * parameters as 'a c d . b c e' (dot is used as a separator of triples).
 * In this way function gets variables from each row and constructs 2 triples to store.
 *
 * Function sets information about errors (#errors.h).
 *
 * @param[in] node. Node to process operation.
 * @param[in] query. String with SELECT query form.
 * @param[in] triples_template. Template to construct and store triples from vriables
 * in a form '?a ?b ?c . ?d ?e ?f . ?q ?w ?e'. If the parameter is NULL then
 * no triples will be constructed and stored in the local store.
 * @return result for SELECT query on success and NULL otherwise (on error).
 * @see sslog_sparql_result_t
 */
SSLOG_EXTERN sslog_sparql_result_t* sslog_node_sparql_select(sslog_node_t *node, const char *query, const char *triples_template);


/**
 * @brief Performs SPARQL CONSTRUCT query.
 * The CONSTRUCT query form returns a single RDF graph specified by a graph template.
 * The result is an RDF graph formed by taking each query solution in the solution
 * sequence, substituting for the variables in the graph template, and combining
 * the triples into a single RDF graph by set union.
 *
 * Function sets information about last error (#errors.h).
 *
 * @param[in] node. Node to process operation.
 * @param query. String with CONSTRUCT query form.
 * @return list with constructed triples on success, empty list if there are
 * no data to construct triples and NULL otherwise (on error).
 */
SSLOG_EXTERN list_t* sslog_node_sparql_construct(sslog_node_t *node, const char *query);
/*****************************************************************************/


/******************* Functions for operations in local store ********************/
SSLOG_EXTERN int sslog_insert_triple(sslog_triple_t *triple);
SSLOG_EXTERN int sslog_insert_triples(list_t *triples);
SSLOG_EXTERN int sslog_remove_triple(sslog_triple_t *triple_template);
SSLOG_EXTERN int sslog_remove_triples(list_t *triple_templates);
SSLOG_EXTERN list_t *sslog_query_triple(sslog_triple_t *triple_template);
SSLOG_EXTERN list_t *sslog_query_triples(list_t *triples_templates);
SSLOG_EXTERN int sslog_update_triple(sslog_triple_t *current_triple, sslog_triple_t *new_triple);
SSLOG_EXTERN int sslog_update_triples(list_t *current_triples, list_t *new_triples);


/******************* Functions for late implementation ********************/
/**
 * @brief Executes the SSAP format insert graph in RDF-XML notation operation.
 *
 * @param[in] char* graph. A string that contained graph in RDF-XML notation.
 * @return int status. Status of the operation when completed (0 if successfull,
 *         otherwise < 0).
 */
//SSLOG_EXTERN int sslog_kpi_graph_insert(sslog_node_t *node, char *graph);


/**
 * @brief Executes the SSAP format update operation.
 *
 * @param[in] ss_triple_t * inserted_triples. Pointer to the first triple in the triple list to be inserted.
 * @param[in] ss_triple_t * removed_triples. Pointer to the first triple in the triple list to be removed.
 * @return int status. Status of the operation when completed (0 if successfull,
 *         otherwise -1).
 */
//SSLOG_EXTERN int sslog_kpi_update(sslog_node_t *node, ss_triple_t *inserted_triples, ss_triple_t *removed_triples);


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
//SSLOG_EXTERN int sslog_kpi_graph_update(sslog_node_t *node, char *inserted_graph, char *removed_graph);




/**
 * @brief Executes the SSAP format remove graph in RDF-XML notation operation.
 *
 * @param[in] char* graph. A string that contained graph in RDF-XML notation.
 * @return int status. Status of the operation when completed (0 if successfull,
 *         otherwise < 0).
 */
//SSLOG_EXTERN int ss_kpi_graph_remove(char * graph);


/// Queries

/**
 * @brief Executes the query operation to smart space.
 *
 * @param[in] ss_triple_t * requested_triples. Pointer to the first triple requested from the SIB.
 * @param[out] ss_triple_t * returned_triples. Pointer to the first triple returned by the SIB.
 *
 * @return int status. Status of the operation when completed (0 if successfull, otherwise -1).
 */
//SSLOG_EXTERN int sslog_kpi_query(sslog_triple_t *requested_triples, sslog_triple_t **returned_triples);

/**
 * @brief  Executes the SSAP format sparql construct query operation.
 *
 * \param[in] char * query. SPARQL CONSRUCT query in text format.
 * \param[out] ss_triple_t ** returned_triples. Pointer to the first triple returned by the SIB.
 *
 * \return int status. Status of the operation when completed (0 if successfull, otherwise -1).
 */
//SSLOG_EXTERN int sslog_sparql_construct_query(char *query, sslog_triple_t **returned_triples);


/**
 * @brief  Executes the SSAP format sparql ask query operation.
 *
 * @param[in] char * query. SPARQL ASK query in text format.
 * @param[out] int * result. Pointer to the answer returned by the SIB.(0 if true, 1 if false)
 *
 * @return int status. Status of the operation when completed (1 if data exists, 0 when no data and -1 otherwise).
 */
//SSLOG_EXTERN int sslog_sparql_ask_query(char *query);

/**
  * @brief  Executes the SSAP format sparql select query operation.
  *
  * @param[in] char * query. SPARQL SELECT query in text format.
  * @param[out] ss_sparql_result_t * results. Pointer to results structure.
  * @param[out] int * number_of_bindings. Pointer to variable which will contain number of variables returned by SIB (if nessesary)
  *
  * @return int status. Status of the operation when completed (0 if successfull, otherwise -1).
  */
//SSLOG_EXTERN int ss_sparql_select_query(char *query, ss_sparql_result_t **results, int *number_of_bindings);

/// @endcond
//  DOXY_EXTERNAL_API


#ifdef	__cplusplus
}
#endif

#endif	/* _SSLOG_KPI_API_H */
