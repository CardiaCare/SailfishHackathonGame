/**
 * @file    kpi_init.h
 * @author  Aleksandr A. Lomov <lomov@cs.karelia.ru>
 * @date    10 April, 2015
 * @brief   Initialize KPI for SmartSlog.
 *
 *
 * @section LICENSE
 *
 * SmartSlog Library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * SmartSlog Library is distributed in the hope that it will be useful,
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
 * Describe functions to initialize KPI. 
 * SmartSlog uses a KPI based on special structure. The structure contains 
 * poiters to functions from KPI. This module initialize such structure.
 *
 *
 * This file is part of SmartSlog Library.
 *
 * Copyright (C) 2009 PetrSU (SmartSlog team). All rights reserved.
 *
 */

//#include "structures.h"
#include "utils/debug.h"
#include "triple.h"


#ifndef _REPOSITORY_H
#define	_REPOSITORY_H

//TODO: dllexports
#if defined(WIN32) || defined(WINCE) || defined(SMARTSLOG_WIN_EXPORTS)
#define SSLOG_EXTERN //__declspec(dllexport)
#else
#define SSLOG_EXTERN // __declspec(dllimport)
#endif

#ifdef	__cplusplus
extern "C" {
#endif

	
    typedef struct kpi_s {
        void (*insert) ()
        
    } kpi_t;
	
    
    void sslog_init_kpi()
    {
        ss_in
    }
	
/*** External functions  ***/

/**
 * @brief Executes the SSAP format insert operation.
 *
 * @param[in] ss_triple_t * first_triple. Pointer to the first triple in the triple list to be inserted.
 * @return int status. Status of the operation when completed (0 if successfull,
 *         otherwise < 0).
 */
SSLOG_EXTERN int ss_kpi_insert(sslog_triple_t *first_triple);

/**
 * @brief Executes the SSAP format insert graph in RDF-XML notation operation.
 *
 * @param[in] char* graph. A string that contained graph in RDF-XML notation.
 * @return int status. Status of the operation when completed (0 if successfull,
 *         otherwise < 0).
 */
SSLOG_EXTERN int sslog_kpi_graph_insert(char *graph);


/**
 * @brief Executes the SSAP format update operation.
 *
 * @param[in] ss_triple_t * inserted_triples. Pointer to the first triple in the triple list to be inserted.
 * @param[in] ss_triple_t * removed_triples. Pointer to the first triple in the triple list to be removed.
 * @return int status. Status of the operation when completed (0 if successfull,
 *         otherwise -1).
 */
SSLOG_EXTERN int sslog_kpi_update(ss_triple_t *inserted_triples, ss_triple_t *removed_triples);


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
 * @param[in] ss_triple_t * removed_triples. Pointer to the first triple in the triple list to be removed.
 * @return int status. Status of the operation when completed (0 if successfull,
 *         otherwise -1).
 */
SSLOG_EXTERN int ss_kpi_remove(ss_triple_t *removed_triples);


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
SSLOG_EXTERN int sslog_kpi_query(sslog_triple_t *requested_triples, sslog_triple_t **returned_triples);

/**
 * @brief  Executes the SSAP format sparql construct query operation.
 *
 * \param[in] char * query. SPARQL CONSRUCT query in text format.
 * \param[out] ss_triple_t ** returned_triples. Pointer to the first triple returned by the SIB.
 *
 * \return int status. Status of the operation when completed (0 if successfull, otherwise -1).
 */
SSLOG_EXTERN int sslog_sparql_construct_query(char *query, sslog_triple_t **returned_triples);


/**
 * @brief  Executes the SSAP format sparql ask query operation.
 *
 * @param[in] char * query. SPARQL ASK query in text format.
 * @param[out] int * result. Pointer to the answer returned by the SIB.(0 if true, 1 if false)
 *
 * @return int status. Status of the operation when completed (1 if data exists, 0 when no data and -1 otherwise).
 */
SSLOG_EXTERN int sslog_sparql_ask_query(char *query);

/**
  * @brief  Executes the SSAP format sparql select query operation.
  *
  * @param[in] char * query. SPARQL SELECT query in text format.
  * @param[out] ss_sparql_result_t * results. Pointer to results structure.
  * @param[out] int * number_of_bindings. Pointer to variable which will contain number of variables returned by SIB (if nessesary)
  *
  * @return int status. Status of the operation when completed (0 if successfull, otherwise -1).
  */
SSLOG_EXTERN int ss_sparql_select_query(char *query, ss_sparql_result_t **results, int *number_of_bindings);


	
/* External functions  */
/// @cond INTERNAL_FUNCTIONS

list_t *get_global_repository_by_rtti(int rtti);

/// @endcond

	``																		

/* Functions for add/remove entities to repositories */
/// @cond EXTERNAL_FUNCTIONS
SSLOG_EXTERN int sslog_repo_add_entity(void *entity);
SSLOG_EXTERN int sslog_repo_add_class(class_t *ont_class);
SSLOG_EXTERN int sslog_repo_add_property(property_t *property);
SSLOG_EXTERN void sslog_repo_remove_entity(void *entity);
SSLOG_EXTERN void sslog_repo_clean_all();
SSLOG_EXTERN int sslog_repo_count_individual_references(individual_t *individual);
/// @endcond

/* Functions for get data from repositories */
/// @cond EXTERNAL_FUNCTIONS
SSLOG_EXTERN const property_t* sslog_repo_get_property_by_name(const char *name);
SSLOG_EXTERN const class_t* sslog_repo_get_class_by_classtype(const char *classtype);
SSLOG_EXTERN const individual_t* sslog_repo_get_individual_by_uuid(const char *uuid);
SSLOG_EXTERN const list_t* sslog_repo_get_individual_by_classtype_all(const char *classtype);
/// @endcond

#ifdef	__cplusplus
}
#endif

#endif	/* _REPOSITORY_H */
