/**
 * @file repository.h
 * @author Aleksandr A. Lomov <lomov@cs.karelia.ru>
 * @date   23 March, 2015
 * @brief  Describe functions for work with classes.
 *
 *
 * @section LICENSE
 *
 * SmartSlog KP Library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * SmartSlog KP Library is distributed in the hope that it will be useful,
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
 * Describe functions for work with reository.
 * This file is part of SmartSlog KP Library.
 *
 * Copyright (C) 2009 PetrSU (SmartSlog team). All rights reserved.
 *
 */

#include "kpi_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "low_api_internal.h"

#include "utils/debug.h"
#include "utils/errors.h"
#include "utils/errors_decl.h"
#include "utils/util_func.h"





#include <ckpi/ckpi.h>
#include <ckpi/sskp_errno.h>


// For socket initialization in windows, see sslog_kpi_init() function.
#ifdef SSLOG_WIN
#include <winsock2.h>
WSADATA wsaData;
#endif

#define SSLOG_CAST_TO_KPI_TRIPLE (ss_triple_t *)
#define SSLOG_CAST_TO_KPI_INFO (ss_info_t *)
#define SSLOG_CAST_TO_SUBS_INFO (ss_subs_info_t *)


/* Network errors (throw KPI) */
#define KPI_ERROR_TEXT_SOCKET_OPEN "KPI error: opening socket."
#define KPI_ERROR_TEXT_SOCKET_SEND "KPI error: sending message."
#define KPI_ERROR_TEXT_SOCKET_RECV "KPI error: receiving message."
#define KPI_ERROR_TEXT_RECV_TIMEOUT "KPI error: receive timeout."
#define KPI_ERROR_TEXT_SOCKET_CLOSE "KPI error: closing socket."

/* KPI ERRORS */
#define KPI_ERROR_TEXT_UNKNOWN "KPI error: unknown error."
#define KPI_ERROR_TEXT_WRONG_SUBSCRIBE_ID "KPI error: wrong subscribe ID."
#define KPI_ERROR_TEXT_NO_SUBSCRIBE_ID "KPI error: no subscribe ID."
#define KPI_ERROR_TEXT_TRANSACTION_FAILED "KPI error: transaction failed."
#define KPI_ERROR_TEXT_UNKNOWN_NODE "KPI error: unknown node."
#define KPI_ERROR_TEXT_UNKNOWN_SS "KPI error: unknown SS."
#define KPI_ERROR_TEXT_SS_ERROR_MESSAGE_TYPE "KPI error: wrong message type."
#define KPI_ERROR_TEXT_TRANSACTION_TYPE "KPI error: wrong transaction type."
#define KPI_ERROR_TEXT_SSAP_MSG_FORMAT "KPI error: wrong SSAP message format."
/*@}*/ // Group ends




/* External functions  */
/// @cond INTERNAL_FUNCTIONS

/// @endcond


/* Functions for add/remove entities to repositories */
/// @cond EXTERNAL_FUNCTIONS

/// @endcond

/* Functions for get data from repositories */
/// @cond EXTERNAL_FUNCTIONS
//SSLOG_EXTERN const property_t* sslog_repo_get_property_by_name(const char *name);
//SSLOG_EXTERN const class_t* sslog_repo_get_class_by_classtype(const char *classtype);
//SSLOG_EXTERN const individual_t* sslog_repo_get_individual_by_uuid(const char *uuid);
//SSLOG_EXTERN const list_t* sslog_repo_get_individual_by_classtype_all(const char *classtype);
/// @endcond



/******************************** Definitions ********************************/
/***************************** Static functions ******************************/
static int sslog_to_sslog_triples(ss_triple_t *triples, list_t **sslog_triples);
static ss_triple_t* sslog_to_kpi_triples(list_t *triples);
inline static ss_triple_t* sslog_to_kpi_triple(sslog_triple_t *triple);
static sslog_sparql_result_t *sslog_to_sslog_sparql(ss_sparql_result_t *kpi_results, int number_of_bindings);
/*****************************************************************************/



int sslog_kpi_query_triple(sslog_kpi_info_t *kpi_info, sslog_triple_t *triple, list_t **returned_triples)
{
    ss_triple_t *query_triples = NULL;

    ss_triple_t *kpi_triples = sslog_to_kpi_triple(triple);

    int result = ss_query(SSLOG_CAST_TO_KPI_INFO kpi_info, kpi_triples, &query_triples);

    ss_delete_triples(kpi_triples);

    if (result < 0) {
        return sslog_kpi_get_error(result);
    }

    sslog_to_sslog_triples(query_triples, returned_triples);
    ss_delete_triples(query_triples);

    return SSLOG_ERROR_NO;
}


int sslog_kpi_query_triples(sslog_kpi_info_t *kpi_info, list_t *requested_triples, list_t **returned_triples)
{
    ss_triple_t *result_triples = NULL;

    ss_triple_t *kpi_triles = sslog_to_kpi_triples(requested_triples);

    int result = ss_query(SSLOG_CAST_TO_KPI_INFO kpi_info, kpi_triles, &result_triples);

    ss_delete_triples(kpi_triles);

    if (result != 0) {
        return sslog_kpi_get_error(result);
    }

    sslog_to_sslog_triples(result_triples, returned_triples);
    ss_delete_triples(result_triples);

    return SSLOG_ERROR_NO;
}






SSLOG_EXTERN int sslog_kpi_subscribe_triples(sslog_kpi_info_t *kpi_info, sslog_subs_info_t *subs_info,
                                             list_t *subscribed_triples, list_t **returned_triples)
{
    ss_triple_t *result_triples = NULL;

    ss_triple_t *kpi_triles = sslog_to_kpi_triples(subscribed_triples);

    int result = ss_subscribe(SSLOG_CAST_TO_KPI_INFO kpi_info, SSLOG_CAST_TO_SUBS_INFO subs_info,
                                  kpi_triles, &result_triples);

    ss_delete_triples(kpi_triles);

    if (result != 0) {
        return sslog_kpi_get_error(result);
    }

    sslog_to_sslog_triples(result_triples, returned_triples);
    ss_delete_triples(result_triples);

    return SSLOG_ERROR_NO;
}


int sslog_kpi_subscribe_sparql_select(sslog_kpi_info_t *kpi_info, sslog_subs_info_t *subs_info, const char *select_query, sslog_sparql_result_t **select_result)
{
    ss_sparql_result_t *kpi_result = NULL;
    int number_of_bindings = 0;

    int result = ss_sparql_select_subscribe(SSLOG_CAST_TO_KPI_INFO kpi_info, SSLOG_CAST_TO_SUBS_INFO subs_info,
                                            (char *) select_query, &kpi_result, &number_of_bindings);

    SSLOG_DEBUG_FUNC("Number of bindings: %i", number_of_bindings);

    if (result != 0) {
        select_result = NULL;
        return sslog_kpi_get_error(result);
    }

    if (kpi_result == NULL) {
        *select_result = sslog_new_sparql_result(NULL, number_of_bindings, 0);
    } else {
        *select_result = sslog_to_sslog_sparql(kpi_result, number_of_bindings);
    }

    //ss_delete_sparql_results(kpi_result, number_of_bindings);

    // This is a CKPI part.
    // ss_sparql_result_t * result_current = kpi_result;
    // ss_sparql_result_t * result_next = NULL;

    // while(result_current) {

        // for (int i = 0; i < number_of_bindings; ++i) {
            // free (result_current->name[i]);
            // free (result_current->value[i]);
        // }

        // free (result_current->type);
        // free (result_current->name);
        // free (result_current->value);

        // result_next = result_current->next;
        // free(result_current);
        // result_current = result_next;
    // }

    return SSLOG_ERROR_NO;
}


int sslog_kpi_unsubscribe(sslog_kpi_info_t *kpi_info, sslog_subs_info_t *subs_info)
{
    int result = ss_unsubscribe(SSLOG_CAST_TO_KPI_INFO kpi_info,
                              SSLOG_CAST_TO_SUBS_INFO subs_info);

    if (result != 0) {
        return sslog_kpi_get_error(result);
    }

    return SSLOG_ERROR_NO;
}


int sslog_kpi_subscribe_indication(sslog_kpi_info_t *kpi_info, sslog_subs_info_t *subs_info,
                                             list_t **new_triples, list_t **old_triples, int ms_timeout)
{
    ss_triple_t *new_kpi_triples = NULL;
    ss_triple_t *old_kpi_triples = NULL;

    int result = ss_subscribe_indication(SSLOG_CAST_TO_KPI_INFO kpi_info, SSLOG_CAST_TO_SUBS_INFO subs_info,
                            &new_kpi_triples, &old_kpi_triples, ms_timeout);

    if (result != 0) {
       // return sslog_get_error_from_kpi(result);
    }

    if (result == 1) { // Indication
        sslog_to_sslog_triples(new_kpi_triples, new_triples);
        sslog_to_sslog_triples(old_kpi_triples, old_triples);
    }

    ss_delete_triples(new_kpi_triples);
    ss_delete_triples(old_kpi_triples);

    return result;
}


int sslog_kpi_sparql_subscribe_indication(sslog_kpi_info_t *kpi_info, sslog_subs_info_t *subs_info,
                                          sslog_sparql_result_t **new_result,
                                          sslog_sparql_result_t **old_result,
                                          int bindings_count,
                                          int ms_timeout)
{
    ss_sparql_result_t *new_kpi_results = NULL;
    ss_sparql_result_t *old_kpi_results = NULL;

    int result = ss_sparql_select_subscribe_indication(SSLOG_CAST_TO_KPI_INFO kpi_info, 
                                                       SSLOG_CAST_TO_SUBS_INFO subs_info,
                                                       &new_kpi_results, &old_kpi_results, ms_timeout);

    
    if (result != 0) {
       // return sslog_get_error_from_kpi(result);
    }



    if (result == 1) { // Indication
        if (new_kpi_results != NULL) {
            SSLOG_DEBUG_FUNC("INDICATION\n");
            *new_result = sslog_to_sslog_sparql(new_kpi_results, bindings_count);
        }

        if (old_kpi_results != NULL) {
            *old_result = sslog_to_sslog_sparql(old_kpi_results, bindings_count);
        }
    }

    //FIXME: delete results
///////////////////////
    //ss_delete_sparql_results(new_kpi_results, bindings_count);
    
    // This is a CKPI part.
    // ss_sparql_result_t *result_current = new_kpi_results;
    // ss_sparql_result_t *result_next = NULL;

    // while(result_current) {

        // for (int i = 0; i < bindings_count; ++i) {
            // free (result_current->name[i]);
            // free (result_current->value[i]);
        // }

        // free (result_current->type);
        // free (result_current->name);
        // free (result_current->value);

        // result_next = result_current->next;
        // free(result_current);
        // result_current = result_next;
    // }

    ///////////
    //ss_delete_sparql_results(old_kpi_results, bindings_count);
    // result_current = old_kpi_results;
    // result_next = NULL;

    // while(result_current) {

        // for (int i = 0; i < bindings_count; ++i) {
            // free (result_current->name[i]);
            // free (result_current->value[i]);
        // }

        // free (result_current->type);
        // free (result_current->name);
        // free (result_current->value);

        // result_next = result_current->next;
        // free(result_current);
        // result_current = result_next;
    // }

    return result;
}

//int set_error_from_kpi(int ss_errno)
//{
//    int error_code = SSLOG_ERROR_NO;

//    switch(ss_errno) {
//    case SS_OK:
//        error_code = SSLOG_ERROR_NO;
//        break;
//    case SS_ERROR_SSAP_MSG_FORMAT:
//        error_code = SSLOG_ERROR_KPI_SSAP_MSG_FORMAT;
//        break;
//    case SS_ERROR_TRANSACTION_TYPE:
//        error_code = SSLOG_ERROR_KPI_TRANSACTION_TYPE;
//        break;
//    case SS_ERROR_MESSAGE_TYPE:
//        error_code = SSLOG_ERROR_KPI_SS_ERROR_MESSAGE_TYPE;
//        break;
//    case SS_ERROR_UNKNOWN_SS:
//        error_code = SSLOG_ERROR_KPI_UNKNOWN_SS;
//        break;
//    case SS_ERROR_UNKNOWN_NODE:
//        error_code = SSLOG_ERROR_KPI_UNKNOWN_NODE;
//        break;
//    case SS_ERROR_TRANSACTION_FAILED:
//        error_code = SSLOG_ERROR_KPI_TRANSACTION_FAILED;
//        break;
//    case SS_ERROR_NO_SUBSCRIBE_ID:
//        error_code = SSLOG_ERROR_KPI_NO_SUBSCRIBE_ID;
//        break;
//    case SS_ERROR_WRONG_SUBSCRIBE_ID:
//        error_code = SSLOG_ERROR_KPI_WRONG_SUBSCRIBE_ID;
//        break;
//    case SS_ERROR_SOCKET_OPEN:
//        error_code = SSLOG_ERROR_SOCKET_OPEN;
//        break;
//    case SS_ERROR_SOCKET_SEND:
//        error_code = SSLOG_ERROR_SOCKET_SEND;
//        break;
//    case SS_ERROR_SOCKET_RECV:
//        error_code = SSLOG_ERROR_SOCKET_RECV;
//        break;
//    case SS_ERROR_RECV_TIMEOUT:
//        error_code = SSLOG_ERROR_RECV_TIMEOUT;
//        break;
//    case SS_ERROR_SOCKET_CLOSE:
//        error_code = SSLOG_ERROR_SOCKET_CLOSE;
//        break;
//    default:
//        error_code = SSLOG_ERROR_KPI_UNKNOWN;
//        break;
//    }

//    return set_error(error_code);
//}




sslog_triple_t* sslog_kpi_new_triple(const char *subject, const char *predicate, const char *object, sslog_rdf_type object_type)
{
    ss_triple_t *triple = NULL;
    ss_add_triple(&triple, (char *) subject, (char *) predicate, (char *) object, SS_RDF_TYPE_URI, object_type);

    return (sslog_triple_t *) triple;
}




const char *sslog_kpi_triple_get_subject(sslog_triple_t *triple)
{
    return (SSLOG_CAST_TO_KPI_TRIPLE triple)->subject;
}


int sslog_kpi_join(sslog_kpi_info_t *kpi_info, const char *node_id)
{
    int result = ss_join(SSLOG_CAST_TO_KPI_INFO kpi_info, (char *) node_id);

    return sslog_kpi_get_error(result);
}


int sslog_kpi_leave(sslog_kpi_info_t *kpi_info)
{
    int result = ss_leave(SSLOG_CAST_TO_KPI_INFO kpi_info);

    return sslog_kpi_get_error(result);
}



sslog_kpi_info_t *sslog_new_kpi(const char *ss_id, const char *address, int port)
{
    ss_info_t *info = (ss_info_t *) malloc(sizeof(ss_info_t));
    ss_init_space_info(info, ss_id, address, port);

    return (sslog_kpi_info_t *) info;
}




int sslog_kpi_init()
{
    SSLOG_DEBUG_START

    // Sockets in Windows must be initialized before using.
    // See https://msdn.microsoft.com/library/windows/desktop/ms738566(v=vs.85).aspx
#if SSLOG_WIN

    // Initialize Winsock
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return SSLOG_ERROR_KPI_INIT;
    }
#endif

    SSLOG_DEBUG_END

    return SSLOG_ERROR_NO;
}


int sslog_kpi_insert_triples(sslog_kpi_info_t *kpi_info, list_t *triples)
{
    ss_triple_t *kpi_triples = sslog_to_kpi_triples(triples);

    int result = ss_insert(SSLOG_CAST_TO_KPI_INFO kpi_info, kpi_triples, NULL);

    ss_delete_triples(kpi_triples);

    return sslog_kpi_get_error(result);
}



const char *sslog_kpi_get_error_text(int sslog_errno)
{
    switch (sslog_errno){
    case SSLOG_ERROR_NO:
        return SSLOG_ERROR_TEXT_NO;
        break;
    case SSLOG_ERROR_KPI_SSAP_MSG_FORMAT:
        return KPI_ERROR_TEXT_SSAP_MSG_FORMAT;
        break;
    case SSLOG_ERROR_KPI_TRANSACTION_TYPE:
        return KPI_ERROR_TEXT_TRANSACTION_TYPE;
        break;
    case SSLOG_ERROR_KPI_SS_ERROR_MESSAGE_TYPE:
        return KPI_ERROR_TEXT_SS_ERROR_MESSAGE_TYPE;
        break;
    case SSLOG_ERROR_KPI_UNKNOWN_SS:
        return KPI_ERROR_TEXT_UNKNOWN_SS;
        break;
    case SSLOG_ERROR_KPI_UNKNOWN_NODE:
        return KPI_ERROR_TEXT_UNKNOWN_NODE;
        break;
    case SSLOG_ERROR_KPI_TRANSACTION_FAILED:
        return KPI_ERROR_TEXT_TRANSACTION_FAILED;
        break;
    case SSLOG_ERROR_KPI_NO_SUBSCRIBE_ID:
        return KPI_ERROR_TEXT_NO_SUBSCRIBE_ID;
        break;
    case SSLOG_ERROR_KPI_WRONG_SUBSCRIBE_ID:
        return KPI_ERROR_TEXT_WRONG_SUBSCRIBE_ID;
        break;
    case SSLOG_ERROR_SOCKET_OPEN:
        return KPI_ERROR_TEXT_SOCKET_OPEN;
        break;
    case SSLOG_ERROR_SOCKET_SEND:
        return KPI_ERROR_TEXT_SOCKET_SEND;
        break;
    case SSLOG_ERROR_SOCKET_RECV:
        return KPI_ERROR_TEXT_SOCKET_RECV;
        break;
    case SSLOG_ERROR_RECV_TIMEOUT:
        return KPI_ERROR_TEXT_RECV_TIMEOUT;
        break;
    case SSLOG_ERROR_SOCKET_CLOSE:
        return KPI_ERROR_TEXT_SOCKET_CLOSE;
        break;
    default:
        return KPI_ERROR_TEXT_UNKNOWN;
        break;
    }

    return KPI_ERROR_TEXT_UNKNOWN;
}




/**
 * @brief Sets code of error from KPI library
 *
 * @return given error code(usefull for end of function: return set_error(code))
 */
int sslog_kpi_get_error(int kpi_errno)
{
    switch(kpi_errno)
    {
        case SS_OK:
            return SSLOG_ERROR_NO;
            break;
        case SS_ERROR_SSAP_MSG_FORMAT:
            return SSLOG_ERROR_KPI_SSAP_MSG_FORMAT;
            break;
        case SS_ERROR_TRANSACTION_TYPE:
            return SSLOG_ERROR_KPI_TRANSACTION_TYPE;
            break;
        case SS_ERROR_MESSAGE_TYPE:
            return SSLOG_ERROR_KPI_SS_ERROR_MESSAGE_TYPE;
            break;
        case SS_ERROR_UNKNOWN_SS:
            return SSLOG_ERROR_KPI_UNKNOWN_SS;
            break;
        case SS_ERROR_UNKNOWN_NODE:
            return SSLOG_ERROR_KPI_UNKNOWN_NODE;
            break;
        case SS_ERROR_TRANSACTION_FAILED:
            return SSLOG_ERROR_KPI_TRANSACTION_FAILED;
            break;
        case SS_ERROR_NO_SUBSCRIBE_ID:
            return SSLOG_ERROR_KPI_NO_SUBSCRIBE_ID;
            break;
        case SS_ERROR_WRONG_SUBSCRIBE_ID:
            return SSLOG_ERROR_KPI_WRONG_SUBSCRIBE_ID;
            break;
        case SS_ERROR_SOCKET_OPEN:
            return SSLOG_ERROR_SOCKET_OPEN;
            break;
        case SS_ERROR_SOCKET_SEND:
            return SSLOG_ERROR_SOCKET_SEND;
            break;
        case SS_ERROR_SOCKET_RECV:
            return SSLOG_ERROR_SOCKET_RECV;
            break;
        case SS_ERROR_RECV_TIMEOUT:
            return SSLOG_ERROR_RECV_TIMEOUT;
            break;
        case SS_ERROR_SOCKET_CLOSE:
            return SSLOG_ERROR_SOCKET_CLOSE;
            break;
        default:
            return SSLOG_ERROR_KPI_UNKNOWN;
            break;
    }

    return SSLOG_ERROR_KPI_UNKNOWN;
}



int sslog_to_sslog_triples(ss_triple_t *triples, list_t **sslog_triples)
{
    if (triples == NULL) {
        return 0;
    }

    if(*sslog_triples == NULL) {
        *sslog_triples = list_new();
    }

    while(triples != NULL) {
        sslog_triple_t *sslog_triple = sslog_new_triple_detached(triples->subject, triples->predicate, triples->object,
			(sslog_rdf_type) triples->subject_type, (sslog_rdf_type) triples->object_type);

        list_add_data(*sslog_triples, sslog_triple);
        triples = triples->next;
    }

    return list_count(*sslog_triples);
}



static ss_triple_t* sslog_to_kpi_triples(list_t *triples)
{
    list_head_t *list_walker = NULL;

    ss_triple_t *kpi_triples = NULL;

    list_for_each (list_walker, &triples->links) {
        list_t *entry = list_entry(list_walker, list_t, links);
        sslog_triple_t *triple = (sslog_triple_t *) entry->data;

        ss_add_triple(&kpi_triples, triple->subject, triple->predicate, triple->object,
                      triple->subject_type, triple->object_type);
    }

    return kpi_triples;
}



inline static ss_triple_t* sslog_to_kpi_triple(sslog_triple_t *triple)
{
    ss_triple_t *kpi_triples = NULL;

    ss_add_triple(&kpi_triples, triple->subject, triple->predicate, triple->object,
                  triple->subject_type, triple->object_type);

    return kpi_triples;
}








int sslog_kpi_remove_triples(sslog_kpi_info_t *kpi_info, list_t *triples_templates)
{
    ss_triple_t *kpi_triples = sslog_to_kpi_triples(triples_templates);

    int result = ss_remove(SSLOG_CAST_TO_KPI_INFO kpi_info, kpi_triples);

    ss_delete_triples(kpi_triples);

    return sslog_kpi_get_error(result);
}




int sslog_kpi_update_triples(sslog_kpi_info_t *kpi_info, list_t *current_triples, list_t *new_triples)
{
    ss_triple_t *kpi_current_triples = sslog_to_kpi_triples(current_triples);
    ss_triple_t *kpi_new_triples = sslog_to_kpi_triples(new_triples);

    int result = ss_update(SSLOG_CAST_TO_KPI_INFO kpi_info, kpi_new_triples, kpi_current_triples, NULL);

    ss_delete_triples(kpi_current_triples);
    ss_delete_triples(kpi_new_triples);

    return sslog_kpi_get_error(result);
}











void sslog_kpi_close_subscription(ss_subs_info_t *subs_info)
{
    if (subs_info == NULL) {
        return;
    }

    ss_close_subscription(subs_info);
}

/******************** Node functions for SPARQL queries **********************/
int sslog_kpi_sparql_ask_query(sslog_kpi_info_t *kpi_info, const char *query, int *query_result)
{
    int result = ss_sparql_ask_query(SSLOG_CAST_TO_KPI_INFO kpi_info, (char *) query, query_result);

    if (result != 0) {
        *query_result = -1;
        return sslog_kpi_get_error(result);
    }

    // KPI function returns 0 when data exists and 1 when there are no data.
    // This function return vice verse.
    *query_result = (*query_result == 0) ? 1 : 0;

    return SSLOG_ERROR_NO;
}


int sslog_kpi_sparql_endpoint_select(const char *endpoint_address, const char *query, const char *extra_parameters, sslog_sparql_result_t **result)
{
    ss_sparql_result_t *kpi_results = NULL;
    int number_of_bindings = 0;

    int kpi_result = ss_sparql_endpoint_query(endpoint_address, query, extra_parameters, &kpi_results, &number_of_bindings);

    if (kpi_result != 0) {
        *result = NULL;
        return sslog_kpi_get_error(kpi_result);
    }

    *result = sslog_to_sslog_sparql(kpi_results, number_of_bindings);

    ss_delete_sparql_results(kpi_results, number_of_bindings);

    return SSLOG_ERROR_NO;
}


int sslog_kpi_sparql_select(sslog_kpi_info_t *kpi_info, const char *query, sslog_sparql_result_t **result)
{
    ss_sparql_result_t *kpi_results = NULL;
    int number_of_bindings = 0;

    int kpi_result = ss_sparql_select_query(SSLOG_CAST_TO_KPI_INFO kpi_info, (char *) query, &kpi_results, &number_of_bindings);

    if (kpi_result != 0) {
        *result = NULL;
        return sslog_kpi_get_error(kpi_result);
    }

    *result = sslog_to_sslog_sparql(kpi_results, number_of_bindings);

    ss_delete_sparql_results(kpi_results, number_of_bindings);

    return SSLOG_ERROR_NO;
}


int sslog_kpi_sparql_construct(sslog_kpi_info_t *kpi_info, const char *query, list_t **result_triples)
{
    ss_triple_t *kpi_result_triples = NULL;

    int result = ss_sparql_construct_query(SSLOG_CAST_TO_KPI_INFO kpi_info, (char *) query, &kpi_result_triples);

    if (result != 0) {
        *result_triples = NULL;
        return sslog_kpi_get_error(result);
    }

    // If there are no result triples, then return empty list.
    if (kpi_result_triples == NULL) {
       *result_triples = list_new();
    } else {
        sslog_to_sslog_triples(kpi_result_triples, result_triples);
    }

    ss_delete_triples(kpi_result_triples);

    return SSLOG_ERROR_NO;
}
/*****************************************************************************/


/****************************** Implementations ******************************/
/****************************** Static functions *****************************/
static inline int sslog_sparql_get_column_index(sslog_sparql_result_t *result, const char *variable_name)
{
    for (int i = 0; i < result->bindings_count; ++i) {
        if (strncmp(result->names[i], variable_name, SSLOG_TRIPLE_URI_LEN) == 0) {
            return i;
        }
    }

    return -1;
}

static sslog_sparql_result_t* sslog_to_sslog_sparql(ss_sparql_result_t *kpi_results, int number_of_bindings)
{
    int rows_count = 0;

    ss_sparql_result_t *result_walker = kpi_results;

    while (result_walker != NULL) {
        ++rows_count;
        result_walker = result_walker->next;
    }

    sslog_sparql_result_t *result = sslog_new_sparql_result((const char **) kpi_results->name, number_of_bindings, rows_count);

    if (kpi_results == NULL) {
        return result;
    }

    int row_index =0;
    while (kpi_results != NULL) {

        sslog_sparql_result_row_t *row = sslog_new_sparql_result_row(number_of_bindings);

        for (int i = 0; i < number_of_bindings; ++i) {

            int index = sslog_sparql_get_column_index(result, kpi_results->name[i]);

            if (index >= 0) {
                row->types[i] = kpi_results->type[i];

                // Do not copy string, just set kpi pointer to NULL.
                if (row->types[i] == SSLOG_RDF_TYPE_UNBOUND) {
                    row->values[i] = NULL;//sslog_strndup("", 1);
                } else {
                    row->values[i] = kpi_results->value[i];
                    kpi_results->value[i] = NULL;
                }

            }

            //result->name[i] = kpi_results->name[i];
            //kpi_results->name[i] = NULL;
        }

        result->rows[row_index] = row;
        ++row_index;
        //list_add_data(&result->rows, row);

        kpi_results = kpi_results->next;
    }

    return result;
}
/*****************************************************************************/
