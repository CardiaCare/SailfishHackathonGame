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
 * \fn process_ssap_cnf.c
 *
 * \brief Provides functions for processing confirm messages.
 *
 * Author: Jussi Kiljander, VTT Technical Research Centre of Finland
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ckpi.h"
#include "parse_ssap_msg.h"
#include "process_ssap_cnf.h"
#include "sskp_errno.h"

/*
*****************************************************************************
*  EXPORTED FUNCTIONS
*****************************************************************************
*/

/**
 * \fn int handle_join_response(ss_info_t * ss_info, ssap_msg_t * msg_i)
 *
 * \brief Handles the SSAP format join response message.
 *
 * \param[in]  ss_info_t * ss_info. Pointer to the Smart Space info.
 * \param[in]  ssap_msg_t * msg_i. Pointer to the parsed message info.
 * \return int status. 0 if appropriate response received, otherwise -1.
 */
int handle_join_response(ss_info_t * ss_info, ssap_msg_t * msg_i)
{

  if(strcmp("CONFIRM", msg_i->message_type) != 0)
  {
    ss_info->ss_errno = SS_ERROR_MESSAGE_TYPE;
    return -1;
  }

  if(strcmp(ss_info->space_id, msg_i->space_id) != 0)
  {
    ss_info->ss_errno = SS_ERROR_UNKNOWN_SS;
    return -1;
  }

  if(strcmp(ss_info->node_id, msg_i->node_id) != 0)
  {
    ss_info->ss_errno = SS_ERROR_UNKNOWN_NODE;
    return -1;
  }

  if(strcmp(M3_SUCCESS, msg_i->transaction_status) != 0)
  {
    ss_info->ss_errno = SS_ERROR_TRANSACTION_FAILED;
    return -1;
  }

  ss_info->ss_errno = SS_OK;

  return 0;
}

/**
 * \fn int handle_leave_response(ss_info_t * ss_info, ssap_msg_t * msg_i)
 *
 * \brief Handles the SSAP format leave response message.
 *
 * \param[in]  ss_info_t * ss_info. Pointer to the Smart Space info.
 * \param[in]  ssap_msg_t * msg_i. Pointer to the parsed message info.
 * \return int status. 0 if appropriate response received, otherwise - 1.
 */
int handle_leave_response(ss_info_t * ss_info, ssap_msg_t * msg_i)
{

  if(strcmp("CONFIRM", msg_i->message_type) != 0)
    {
      ss_info->ss_errno = SS_ERROR_MESSAGE_TYPE;
      return -1;
    }

  if(strcmp(ss_info->space_id, msg_i->space_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_SS;
      return -1;
    }

  if(strcmp(ss_info->node_id, msg_i->node_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_NODE;
      return -1;
    }

  if(strcmp(M3_SUCCESS, msg_i->transaction_status) != 0)
    {
      ss_info->ss_errno = SS_ERROR_TRANSACTION_FAILED;
      return -1;
    }

  ss_info->ss_errno = SS_OK;

  return 0;
}

/**
 * \fn int handle_insert_response(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_bnode_t * bnodes)
 *
 * \brief Handles the SSAP format insert response message.
 *
 * \param[in]  ss_info_t * ss_info. A pointer to the SS info struct.
 * \param[in]  ssap_msg_t * msg_i. Pointer to the parsed message info.
 * \param[out] char * bnode_uri. The bnode uri is copied to this buffer.
 * \return int status. 0 if appropriate response received, otherwise -1.
 */
int handle_insert_response(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_bnode_t * bnodes)
{
  int i = 0;
  bnode_tmp_t * bnode = NULL;
  bnode_tmp_t * bnode_tmp = NULL;

  if(strcmp("CONFIRM", msg_i->message_type) != 0)
    {
      ss_info->ss_errno = SS_ERROR_MESSAGE_TYPE;
      return -1;
    }

  if(strcmp(ss_info->space_id, msg_i->space_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_SS;
      return -1;
    }

  if(strcmp(ss_info->node_id, msg_i->node_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_NODE;
      return -1;
    }

  if(strcmp(M3_SUCCESS, msg_i->transaction_status) != 0)
    {
      ss_info->ss_errno = SS_ERROR_TRANSACTION_FAILED;
      return -1;
    }

  bnode = msg_i->bnodes;

  while(bnode)
    {
      strncpy(bnodes[i].label, bnode->label, SS_SUBJECT_MAX_LEN);
      strncpy(bnodes[i].uri, bnode->uri, SS_URI_MAX_LEN);
      bnode_tmp = bnode;
      bnode = bnode -> next;
      free(bnode_tmp);
      i++;
    }

  ss_info->ss_errno = SS_OK;

  return 0;

}

/**
 * \fn int handle_graph_insert_response(ss_info_t * ss_info, ssap_msg_t * msg_i)
 *
 * \brief Handles the SSAP format insert RDF-XML notationed graph response message.
 *
 * \param[in]  ss_info_t * ss_info. A pointer to the SS info struct.
 * \param[in]  ssap_msg_t * msg_i. Pointer to the parsed message info.
 * \return int status. 0 if appropriate response received, otherwise -1.
 */
int handle_graph_insert_response(ss_info_t * ss_info, ssap_msg_t * msg_i)
{

  if(strcmp("CONFIRM", msg_i->message_type) != 0)
    {
      ss_info->ss_errno = SS_ERROR_MESSAGE_TYPE;
      return -1;
    }

  if(strcmp(ss_info->space_id, msg_i->space_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_SS;
      return -1;
    }

  if(strcmp(ss_info->node_id, msg_i->node_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_NODE;
      return -1;
    }

  if(strcmp(M3_SUCCESS, msg_i->transaction_status) != 0)
  {
     ss_info->ss_errno = SS_ERROR_TRANSACTION_FAILED;
     return -1;
  }

  ss_info->ss_errno = SS_OK;

  return 0;
}


/**
 * \fn int handle_remove_response(ss_info_t * ss_info, ssap_msg_t * msg_i)
 *
 * \brief Handles the SSAP format remove response message.
 *
 * \param[in]  ss_info_t * ss_info. A pointer to the SS info struct.
 * \param[in]  ssap_msg_t * msg_i. Pointer to the parsed message info.
 * \return int status. 0 if appropriate response received, otherwise -1.
 */
int handle_remove_response(ss_info_t * ss_info, ssap_msg_t * msg_i)
{

  if(strcmp("CONFIRM", msg_i->message_type) != 0)
    {
      ss_info->ss_errno = SS_ERROR_MESSAGE_TYPE;
      return -1;
    }

  if(strcmp(ss_info->space_id, msg_i->space_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_SS;
      return -1;
    }

  if(strcmp(ss_info->node_id, msg_i->node_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_NODE;
      return -1;
    }

  if(strcmp(M3_SUCCESS, msg_i->transaction_status) != 0)
    {
      ss_info->ss_errno = SS_ERROR_TRANSACTION_FAILED;
      return -1;
    }

  ss_info->ss_errno = SS_OK;

  return 0;
}

/**
 * \fn int handle_graph_remove_response(ss_info_t * ss_info, ssap_msg_t * msg_i)
 *
 * \brief Handles the SSAP format remove RDF-XML notationed graph response message.
 *
 * \param[in]  ss_info_t * ss_info. A pointer to the SS info struct.
 * \param[in]  ssap_msg_t * msg_i. Pointer to the parsed message info.
 * \return int status. 0 if appropriate response received, otherwise -1.
 */
int handle_graph_remove_response(ss_info_t * ss_info, ssap_msg_t * msg_i)
{

  if(strcmp("CONFIRM", msg_i->message_type) != 0)
    {
      ss_info->ss_errno = SS_ERROR_MESSAGE_TYPE;
      return -1;
    }

  if(strcmp(ss_info->space_id, msg_i->space_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_SS;
      return -1;
    }

  if(strcmp(ss_info->node_id, msg_i->node_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_NODE;
      return -1;
    }

  if(strcmp(M3_SUCCESS, msg_i->transaction_status) != 0)
  {
     ss_info->ss_errno = SS_ERROR_TRANSACTION_FAILED;
     return -1;
  }

  ss_info->ss_errno = SS_OK;

  return 0;
}


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
int handle_update_response(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_bnode_t * bnodes)
{
  int i = 0;
  bnode_tmp_t * bnode = NULL;

  if(strcmp("CONFIRM", msg_i->message_type) != 0)
    {
      ss_info->ss_errno = SS_ERROR_MESSAGE_TYPE;
      return -1;
    }

  if(strcmp(ss_info->space_id, msg_i->space_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_SS;
      return -1;
    }

  if(strcmp(ss_info->node_id, msg_i->node_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_NODE;
      return -1;
    }

  if(strcmp(M3_SUCCESS, msg_i->transaction_status) != 0)
    {
      ss_info->ss_errno = SS_ERROR_TRANSACTION_FAILED;
      return -1;
    }

  bnode = msg_i->bnodes;
  while(bnode)
    {
      strncpy(bnodes[i].label, bnode->label, SS_SUBJECT_MAX_LEN);
      strncpy(bnodes[i].uri, bnode->uri, SS_URI_MAX_LEN);
      bnode = bnode -> next;
      i++;
    }


  ss_info->ss_errno = SS_OK;

  return 0;
}

/**
 * \fn int handle_graph_update_response(ss_info_t * ss_info, ssap_msg_t * msg_i)
 *
 * \brief Handles the SSAP format update response message.
 *
 * \param[in]  ss_info_t * ss_info. A pointer to the SS info struct.
 * \param[in]  ssap_msg_t * msg_i. Pointer to the parsed message info.
 * \return int status. 0 if appropriate response received, otherwise -1.
 */
int handle_graph_update_response(ss_info_t * ss_info, ssap_msg_t * msg_i)
{
  if(strcmp("CONFIRM", msg_i->message_type) != 0)
    {
      ss_info->ss_errno = SS_ERROR_MESSAGE_TYPE;
      return -1;
    }

  if(strcmp(ss_info->space_id, msg_i->space_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_SS;
      return -1;
    }

  if(strcmp(ss_info->node_id, msg_i->node_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_NODE;
      return -1;
    }

  if(strcmp(M3_SUCCESS, msg_i->transaction_status) != 0)
    {
      ss_info->ss_errno = SS_ERROR_TRANSACTION_FAILED;
      return -1;
    }
  ss_info->ss_errno = SS_OK;

  return 0;
}

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
int handle_query_response(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_triple_t ** results)
{
  if(strcmp("CONFIRM", msg_i->message_type) != 0)
    {
      ss_info->ss_errno = SS_ERROR_MESSAGE_TYPE;
      return -1;
    }

  if(strcmp(ss_info->space_id, msg_i->space_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_SS;
      return -1;
    }

  if(strcmp(ss_info->node_id, msg_i->node_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_NODE;
      return -1;
    }

  if(strcmp(M3_SUCCESS, msg_i->transaction_status) != 0)
    {
      ss_info->ss_errno = SS_ERROR_TRANSACTION_FAILED;
      return -1;
    }

  *results = msg_i->n_result;
  ss_info->ss_errno = SS_OK;

  return 0;
}
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
int handle_sparql_construct_response(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_triple_t ** results)
{
  if(strcmp("CONFIRM", msg_i->message_type) != 0)
    {
      ss_info->ss_errno = SS_ERROR_MESSAGE_TYPE;
      return -1;
    }

  if(strcmp(ss_info->space_id, msg_i->space_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_SS;
      return -1;
    }

  if(strcmp(ss_info->node_id, msg_i->node_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_NODE;
      return -1;
    }

  if(strcmp(M3_SUCCESS, msg_i->transaction_status) != 0)
    {
      ss_info->ss_errno = SS_ERROR_TRANSACTION_FAILED;
      return -1;
    }

  *results = msg_i->n_result;
  ss_info->ss_errno = SS_OK;

  return 0;
}

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
int handle_sparql_ask_response(ss_info_t * ss_info, ssap_msg_t * msg_i, int* result)
{
  if(strcmp("CONFIRM", msg_i->message_type) != 0)
    {
      ss_info->ss_errno = SS_ERROR_MESSAGE_TYPE;
      return -1;
    }

  if(strcmp(ss_info->space_id, msg_i->space_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_SS;
      return -1;
    }

  if(strcmp(ss_info->node_id, msg_i->node_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_NODE;
      return -1;
    }

  if(strcmp(M3_SUCCESS, msg_i->transaction_status) != 0)
    {
      ss_info->ss_errno = SS_ERROR_TRANSACTION_FAILED;
      return -1;
    }

  *result = msg_i->n_bool_result;
  ss_info->ss_errno = SS_OK;

  return 0;
}

/**
 * \fn int handle_sparql_select_response(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_sparql_result_t ** results)
 *
 * \brief Handles the SSAP format SPARQL query response message.
 *
 * \param[in]  ss_info_t * ss_info. A pointer to the SS info struct.
 * \param[in]  ssap_msg_t * msg_i. Pointer to the parsed message info.
 * \param[out] ss_triple_t ** results. The results of the query.
 * \return int status. 0 if appropriate response received, otherwise < 0.
 */
int handle_sparql_select_response(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_sparql_result_t ** results)
{
  if(strcmp("CONFIRM", msg_i->message_type) != 0)
    {
      ss_info->ss_errno = SS_ERROR_MESSAGE_TYPE;
      return -1;
    }

  if(strcmp(ss_info->space_id, msg_i->space_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_SS;
      return -1;
    }

  if(strcmp(ss_info->node_id, msg_i->node_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_NODE;
      return -1;
    }

  if(strcmp(M3_SUCCESS, msg_i->transaction_status) != 0)
    {
      ss_info->ss_errno = SS_ERROR_TRANSACTION_FAILED;
      return -1;
    }

  *results = msg_i->n_sparql_select_result;
  ss_info->ss_errno = SS_OK;

  return 0;
}
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
int handle_subscribe_response(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_subs_info_t * subs_info, ss_triple_t ** results)
{
  if(strcmp("CONFIRM", msg_i->message_type) != 0)
    {
      ss_info->ss_errno = SS_ERROR_MESSAGE_TYPE;
      return -1;
    }

  if(strcmp(ss_info->space_id, msg_i->space_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_SS;
      return -1;
    }

  if(strcmp(ss_info->node_id, msg_i->node_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_NODE;
      return -1;
    }

  if(strcmp(M3_SUCCESS, msg_i->transaction_status) != 0)
    {
      ss_info->ss_errno = SS_ERROR_TRANSACTION_FAILED;
      return -1;
    }

  if(!msg_i->subscribe_id)
    {
      ss_info->ss_errno = SS_ERROR_NO_SUBSCRIBE_ID;
      return -1;
    }

  if(subs_info)
    {
      strncpy(subs_info->id, msg_i->subscribe_id, SS_SUB_ID_MAX_LEN);
    }

  *results = msg_i->n_result;
  ss_info->ss_errno = SS_OK;

  return 0;
}

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
int handle_sparql_select_subscribe_response(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_subs_info_t * subs_info, ss_sparql_result_t ** results)

{
  if(strcmp("CONFIRM", msg_i->message_type) != 0)
    {
      ss_info->ss_errno = SS_ERROR_MESSAGE_TYPE;
      return -1;
    }

  if(strcmp(ss_info->space_id, msg_i->space_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_SS;
      return -1;
    }

  if(strcmp(ss_info->node_id, msg_i->node_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_NODE;
      return -1;
    }

  if(strcmp(M3_SUCCESS, msg_i->transaction_status) != 0)
    {
      ss_info->ss_errno = SS_ERROR_TRANSACTION_FAILED;
      return -1;
    }

  if(!msg_i->subscribe_id)
    {
      ss_info->ss_errno = SS_ERROR_NO_SUBSCRIBE_ID;
      return -1;
    }

  if(subs_info)
    {
      strncpy(subs_info->id, msg_i->subscribe_id, SS_SUB_ID_MAX_LEN);
    }

  *results = msg_i->n_sparql_select_result;
  ss_info->ss_errno = SS_OK;

  return 0;
}

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
int handle_subscribe_indication(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_subs_info_t * subs_info, ss_triple_t ** new_results, ss_triple_t ** obsolete_results)
{

  ss_triple_t * tmp = NULL;
  ss_triple_t * tmp_prev = NULL;

  if(strcmp("INDICATION", msg_i->message_type) != 0)
    {
      ss_info->ss_errno = SS_ERROR_MESSAGE_TYPE;
      return -1;
    }

  if(strcmp(ss_info->space_id, msg_i->space_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_SS;
      return -1;
    }

  if(strcmp(ss_info->node_id, msg_i->node_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_NODE;
      return -1;
    }

  if(strcmp(subs_info->id, msg_i->subscribe_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_WRONG_SUBSCRIBE_ID;
      return -1;
    }
  
  if(!(*new_results))
  {
    *new_results = msg_i->n_result;  
  }
  else
  {
    tmp = *new_results;
    while(tmp)
    {
      tmp_prev = tmp;
      tmp = tmp->next;
    }

    tmp_prev->next = msg_i->n_result;

  }

  if(!(*obsolete_results))
  {
    *obsolete_results = msg_i->o_result;  
  }
  else
  {
    tmp = *obsolete_results;
    while(tmp)
    {
      tmp_prev = tmp;
      tmp = tmp->next;
    }

    tmp_prev->next = msg_i->o_result;

  }


  ss_info->ss_errno = SS_OK;

  return 0;
}

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
int handle_sparql_select_subscribe_indication(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_subs_info_t * subs_info, ss_sparql_result_t ** new_results, ss_sparql_result_t ** obsolete_results)
{
  ss_sparql_result_t * tmp;
  ss_sparql_result_t * tmp_prev;
  if(strcmp("INDICATION", msg_i->message_type) != 0)
    {
      ss_info->ss_errno = SS_ERROR_MESSAGE_TYPE;
      return -1;
    }

  if(strcmp(ss_info->space_id, msg_i->space_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_SS;
      return -1;
    }

  if(strcmp(ss_info->node_id, msg_i->node_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_NODE;
      return -1;
    }

  if(strcmp(subs_info->id, msg_i->subscribe_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_WRONG_SUBSCRIBE_ID;
      return -1;
    }
  
  if(!(*new_results))
  {
    *new_results = msg_i->n_sparql_select_result;  
  }
  else
  {
    tmp = *new_results;
    while(tmp)
    {
      tmp_prev = tmp;
      tmp = tmp->next;
    }

    tmp_prev->next = msg_i->n_sparql_select_result;

  }

  if(!(*obsolete_results))
  {
    *obsolete_results = msg_i->o_sparql_select_result;
  }
  else
  {
    tmp = *obsolete_results;
    while(tmp)
    {
      tmp_prev = tmp;
      tmp = tmp->next;
    }

    tmp_prev->next = msg_i->o_sparql_select_result;
  }


  ss_info->ss_errno = SS_OK;

  return 0;
}

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
int handle_unsubscribe_response(ss_info_t * ss_info, ssap_msg_t * msg_i, ss_subs_info_t * subs_info)
{
  if(strcmp("CONFIRM", msg_i->message_type) != 0)
    {
      ss_info->ss_errno = SS_ERROR_MESSAGE_TYPE;
      return -1;
    }

  if(strcmp(ss_info->space_id, msg_i->space_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_SS;
      return -1;
    }

  if(strcmp(ss_info->node_id, msg_i->node_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_UNKNOWN_NODE;
      return -1;
    }

  if(strcmp(M3_SUCCESS, msg_i->transaction_status) != 0)
    {
      ss_info->ss_errno = SS_ERROR_TRANSACTION_FAILED;
      return -1;
    }

  if(strcmp(subs_info->id, msg_i->subscribe_id) != 0)
    {
      ss_info->ss_errno = SS_ERROR_WRONG_SUBSCRIBE_ID;
      return -1;
    }

  ss_info->ss_errno = SS_OK;

  return 0;
}
