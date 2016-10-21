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
 * parse_ssap_msg.h
 *
 * \brief API for parsing the SSAP transaction messages using expat and scew.
 *
 * Author: Jussi Kiljander, VTT Technical Research Centre of Finland
 */

#ifndef PARSE_SSAP_MSG_H
#define PARSE_SSAP_MSG_H

#include "ckpi.h"

/*
*****************************************************************************
*  DATA TYPES
*****************************************************************************
*/

#define MAX_MSG_TYPE_LEN    (11)
#define MAX_ACTION_TYPE_LEN (12)
#define MAX_ACTION_ID_LEN   (50)
#define MAX_STATUS_LEN      (50)

#define M3_SUCCESS "m3:Success"

/*
*****************************************************************************
*  DATA TYPES
*****************************************************************************
*/

typedef struct bnode_tmp
{
  char label[SS_SUBJECT_MAX_LEN];
  char uri[SS_URI_MAX_LEN];
  struct bnode_tmp * next;
}bnode_tmp_t;

typedef struct ssap_msg {
  char message_type[MAX_MSG_TYPE_LEN];
  char transaction_type[MAX_ACTION_TYPE_LEN];
  char transaction_id[MAX_ACTION_ID_LEN];
  char space_id[SS_SPACE_ID_MAX_LEN];
  char node_id[SS_NODE_ID_MAX_LEN];
  char transaction_status[MAX_STATUS_LEN];
  char subscribe_id[SS_SUB_ID_MAX_LEN];

  bnode_tmp_t * bnodes;

  ss_triple_t *n_result;
  ss_triple_t *o_result;
  
  int n_bool_result;
  int o_bool_result;
  
  ss_sparql_result_t * n_sparql_select_result;
  ss_sparql_result_t * o_sparql_select_result;
  int number_of_bindings;
}ssap_msg_t;


/*
*****************************************************************************
*  EXPORTED FUNCTION PROTOTYPES
*****************************************************************************
*/

int parse_ssap_msg(char *xml, int len, struct ssap_msg *msg);
int parse_sparql_xml_result(char *xml, ss_sparql_result_t **result, int *number_of_bindings);

#endif
