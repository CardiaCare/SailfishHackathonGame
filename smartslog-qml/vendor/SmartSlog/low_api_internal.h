/**
 * @file    low_api.h
 * @author  Aleksandr A. Lomov <lomov@cs.karelia.ru>
 * @date    4 August, 2015
 * @brief   Defines internal (for SmartSlog using) part of low-level API.
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
 * This file is used to defines internal part of low-level API
 * to work with KPI through SmartSlog.
 * Low-level API is based on triples.
 *
 * This file is part of the SmartSlog project.
 *
 * Copyright (C) 2015 by PetrSU SmartSlog team.
 */


#ifndef _SSLOG_LOW_API_INTERNAL_H
#define	_SSLOG_LOW_API_INTERNAL_H

#include "kpi_api.h"

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

/******************************* Definitions *********************************/
/**************************** Internal functions *****************************/
/// @cond DOXY_INTERNAL_API

/**
 * @brief Creates a new structure for SPARQL SELECT results.
 * @return new structure on success or NULL otherwise.
 */
sslog_sparql_result_t *sslog_new_sparql_result(const char **variables_names, int number_of_bindings, int rows_count);
sslog_sparql_result_row_t *sslog_new_sparql_result_row(int number_of_bindings);


/**
 * @brief Convers sparql template for storing triples in string form to triples structures.
 * This function calls #sslog_sparql_template_to_triple function to convert each template
 * for triple.
 * @param triple_template. Triples template to create triples.
 * @return list with created triples on success or NULL otherwise (template is incorrect).
 */
list_t *sslog_sparql_template_to_triples(const char *triple_template);


/**
 * @brief Converts SPARQL SELECT result to triples and stores it in local store.
 * @param result. Result of SPARQL SELECT query.
 * @param triples_variables. Triples with elements (subject, prediacte, object) that represent
 * names of variables in the SPARQL results.
 * @return number of stored triples in the local store.
 */
int sslog_store_sparql_results(sslog_sparql_result_t *result, list_t *triples_variables);

/**
 * @brief Converts SPARQL SELECT result to triples.
 * @param result. Result of SPARQL SELECT query.
 * @param triples_variables. Triples with elements (subject, prediacte, object) that represent
 * names of variables in the SPARQL results.
 * @return list with converted triples on success or NULL otherwise.
 */
list_t* sslog_sparql_result_to_triples(sslog_sparql_result_t *result, list_t *triples_variables);

/// @endcond
//  endcond of DOXY_INTERNAL_API

#ifdef	__cplusplus
}
#endif

#endif	/* _SSLOG_LOW_API_INTERNAL_H */
