/*

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
 * process_ssap_cnf.h
 *
 * \brief API for processing the received SSAP confirmation messages.
 *
 * Author: Jussi Kiljander, VTT Technical Research Centre of Finland
 *
 */

#ifndef PROCESS_SSAP_CNF_H
#define PROCESS_SSAP_CNF_H

#include "ckpi.h"
#include "parse_ssap_msg.h"

/*
*****************************************************************************
*  EXPORTED FUNCTION PROTOTYPES
*****************************************************************************
*/

/**
 * \fn int handle_join_response(ss_info_t * ss_info, ssap_msg_t * msg_i)
 *
 * \brief Handles the SSAP format join response message.
 *
 * \param[in]  ss_info_t * ss_info. Pointer to the Smart Space info.
 * \return int status. 0 if appropriate response received, otherwise -1.
 */
int handle_join_response(ss_info_t * ss_info, ssap_msg_t * msg_i);

/**
 * \fn int handle_leave_response(ss_info_t * ss_info, ssap_msg_t * msg_i)
 *
 * \brief Handles the SSAP format leave response message.
 *
 * \param[in]  ss_info_t * ss_info. Pointer to the Smart Space info.
 * \param[in]  ssap_msg_t * msg_i. Pointer to the parsed message info.
 * \return int status. 0 if appropriate response received, otherwise - 1.
 */
int handle_leave_response(ss_info_t * ss_info, ssap_msg_t * msg_i);

/**
 * \fn int handle_insert_response(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_bnode_t * bnodes)
 *
 * \brief Handles the SSAP format insert response message.
 *
 * \param[in]  ss_info_t * ss_info. A pointer to the SS info struct.
 * \param[in]  ssap_msg_t * msg_i. Pointer to the parsed message info.
 * \param[out] ss_bnode_t * bnodes. Pointer to the bnode struct(s), where bnode label and URI are copied.
 * \return int status. 0 if appropriate response received, otherwise -1.
 */
int handle_insert_response(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_bnode_t * bnodes);

/**
 * \fn int handle_graph_insert_response(ss_info_t * ss_info, ssap_msg_t * msg_i)
 *
 * \brief Handles the SSAP format insert RDF-XML notationed graph response message.
 *
 * \param[in]  ss_info_t * ss_info. A pointer to the SS info struct.
 * \param[in]  ssap_msg_t * msg_i. Pointer to the parsed message info.
 * \return int status. 0 if appropriate response received, otherwise -1.
 */
int handle_graph_insert_response(ss_info_t * ss_info, ssap_msg_t * msg_i);

/**
 * \fn int handle_remove_response(ss_info_t * ss_info, ssap_msg_t * msg_i)
 *
 * \brief Handles the SSAP format remove response message.
 *
 * \param[in]  ss_info_t * ss_info. A pointer to the SS info struct.
 * \param[in]  ssap_msg_t * msg_i. Pointer to the parsed message info.
 * \return int status. 0 if appropriate response received, otherwise -1.
 */
int handle_remove_response(ss_info_t * ss_info, ssap_msg_t * msg_i);

/**
 * \fn int handle_graph_remove_response(ss_info_t * ss_info, ssap_msg_t * msg_i)
 *
 * \brief Handles the SSAP format remove RDF-XML notationed graph response message.
 *
 * \param[in]  ss_info_t * ss_info. A pointer to the SS info struct.
 * \param[in]  ssap_msg_t * msg_i. Pointer to the parsed message info.
 * \return int status. 0 if appropriate response received, otherwise -1.
 */
int handle_graph_remove_response(ss_info_t * ss_info, ssap_msg_t * msg_i);

/**
 * \fn int handle_update_response(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_bnode_t * bnodes)
 *
 * \brief Handles the SSAP format update response message.
 *
 * \param[in]  ss_info_t * ss_info. A pointer to the SS info struct.
 * \param[in]  ssap_msg_t * msg_i. Pointer to the parsed message info.
 * \param[out] ss_bnode_t * bnodes. Pointer to the bnode struct(s), where bnode label and URI are copied.
 * \return int status. 0 if appropriate response received, otherwise -1.
 */
int handle_update_response(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_bnode_t * bnodes);

/**
 * \fn int handle_graph_update_response(ss_info_t * ss_info, ssap_msg_t * msg_i)
 *
 * \brief Handles the SSAP format update response message.
 *
 * \param[in]  ss_info_t * ss_info. A pointer to the SS info struct.
 * \param[in]  ssap_msg_t * msg_i. Pointer to the parsed message info.
 * \return int status. 0 if appropriate response received, otherwise -1.
 */
int handle_graph_update_response(ss_info_t * ss_info, ssap_msg_t * msg_i);

/**
 * \fn int handle_query_response(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_triple_t ** results)
 *
 * \brief Handles the SSAP format query response message.
 *
 * \param[in]  ss_info_t * ss_info. A pointer to the SS info struct.
 * \param[in]  ssap_msg_t * msg_i. Pointer to the parsed message info.
 * \param[out] ss_triple_t ** results. The results of the query.
 * \return int status. 0 if appropriate response received, otherwise < 0.
 */
int handle_query_response(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_triple_t ** results);

/**
 * \fn int handle_sparql_construct_response(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_triple_t ** results)
 *
 * \brief Handles the SSAP format SPARQL query response message.
 *
 * \param[in]  ss_info_t * ss_info. A pointer to the SS info struct.
 * \param[in]  ssap_msg_t * msg_i. Pointer to the parsed message info.
 * \param[out] ss_triple_t ** results. The results of the query.
 * \return int status. 0 if appropriate response received, otherwise < 0.
 */
int handle_sparql_construct_response(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_triple_t ** results);

/**
 * \fn int handle_sparql_ask_response(ss_info_t * ss_info, ssap_msg_t * msg_i, int * result)
 *
 * \brief Handles the SSAP format SPARQL query response message.
 *
 * \param[in]  ss_info_t * ss_info. A pointer to the SS info struct.
 * \param[in]  ssap_msg_t * msg_i. Pointer to the parsed message info.
 * \param[out] int * result. The result of the query.
 * \return int status. 0 if appropriate response received, otherwise < 0.
 */
int handle_sparql_ask_response(ss_info_t * ss_info, ssap_msg_t * msg_i, int* result);

/**
 * \fn int handle_sparql_select_response(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_sparql_result_t ** results)
 *
 * \brief Handles the SSAP format SPARQL query response message.
 *
 * \param[in]  ss_info_t * ss_info. A pointer to the SS info struct.
 * \param[in]  ssap_msg_t * msg_i. Pointer to the parsed message info.
 * \param[out] ss_sparql_result_t ** results. The results of the query.
 * \return int status. 0 if appropriate response received, otherwise < 0.
 */
int handle_sparql_select_response(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_sparql_result_t ** results);

/**
 * \fn int handle_subscribe_response(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_subs_info_t * subs_info, ss_triple_t ** results)
 *
 * \brief Handles the SSAP format subscribe response message.
 *
 * \param[in]  ss_info_t * ss_info. A pointer to the SS info struct.
 * \param[in]  ssap_msg_t * msg_i. Pointer to the parsed message info.
 * \param[out] ss_subs_info_t * subs_info. Subscribe info of the subscribtion.
 * \param[out] ss_triple_t ** results. Pointer to the first triple of the result list.
 * \return int status. 0 if appropriate response received, otherwise -1.
 */
int handle_subscribe_response(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_subs_info_t * subs_info, ss_triple_t ** results);

/**
 * \fn int handle_sparql_select_subscribe_response(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_subs_info_t * subs_info, ss_sparql_result_t ** results)
 *
 * \brief Handles the SSAP format subscribe response message.
 *
 * \param[in]  ss_info_t * ss_info. A pointer to the SS info struct.
 * \param[in]  ssap_msg_t * msg_i. Pointer to the parsed message info.
 * \param[out] ss_subs_info_t * subs_info. Subscribe info of the subscribtion.
 * \param[out] ss_sparql_result_t ** results. Pointer to the first query result in result list.
 * \return int status. 0 if appropriate response received, otherwise -1.
 */
int handle_sparql_select_subscribe_response(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_subs_info_t * subs_info, ss_sparql_result_t ** results);

/**
 * \fn int handle_subscribe_indication(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_subs_info_t * subs_info, ss_triple_t ** new_results, ss_triple_t ** obsolete_results)
 *
 * \brief Handles the SSAP format subscribe indication message.
 *
 * \param[in] ss_info_t * ss_info. Smart Space info.
 * \param[in]  ssap_msg_t * msg_i. Pointer to the parsed message info.
 * \param[in] ss_subs_info_t * subs_info. Subscribe info.
 * \param[out] ss_triple_t ** new_results. Pointer to the first triple of the new result list.
 * \param[out] ss_triple_t ** obsolete_results. Pointer to the first triple of the obsolete result list.
 * \return int status. 0 if appropriate response received, otherwise - 1.
 */
int handle_subscribe_indication(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_subs_info_t * subs_info, ss_triple_t ** new_results, ss_triple_t ** obsolete_results);

/**
 * \fn int handle_sparql_select_subscribe_indication(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_subs_info_t * subs_info, ss_sparql_result_t ** new_results, ss_sparql_result_t ** obsolete_results)
 *
 * \brief Handles the SSAP format subscribe indication message.
 *
 * \param[in] ss_info_t * ss_info. Smart Space info.
 * \param[in]  ssap_msg_t * msg_i. Pointer to the parsed message info.
 * \param[in] ss_subs_info_t * subs_info. Subscribe info.
 * \param[out] ss_sparql_result_t ** new_results. Pointer to the first query result of the new result list.
 * \param[out] ss_sparql_result_t ** obsolete_results. Pointer to the first query result of the obsolete result list.
 * \return int status. 0 if appropriate response received, otherwise - 1.
 */
int handle_sparql_select_subscribe_indication(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_subs_info_t * subs_info, ss_sparql_result_t ** new_results, ss_sparql_result_t ** obsolete_results);

/**
 * \fn int handle_unsubscribe_response(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_subs_info_t * subs_info)
 *
 * \brief Handles the SSAP format unsubscribe response message.
 *
 * \param[in] ss_info_t * ss_info. Smart Space info.
 * \param[in]  ssap_msg_t * msg_i. Pointer to the parsed message info.
 * \param[in] ss_subs_info_t * subs_i. Subscribe info of the subscribtion.
 * \return int status. 0 if appropriate response received, otherwise -1.
 */
int handle_unsubscribe_response(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_subs_info_t * subs_info);

#endif /* HANDLE_SSAP_RESPONSES_H */
