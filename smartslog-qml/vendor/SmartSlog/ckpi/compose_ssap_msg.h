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
  * compose_ssap_msg.h
  *
  * \brief API for constructing SSAP transaction messages.
  *
  * Author: Jussi Kiljander, VTT Technical Research Centre of Finland
  */
 
#ifndef COMPOSE_SSAP_MSG_H
#define COMPOSE_SSAP_MSG_H

#include "ckpi.h"
 
/*
 *****************************************************************************
 *  EXPORTED FUNCTION PROTOTYPES
 *****************************************************************************
 */
 
 /**
   * \fn void make_join_msg(ss_info_t * ss_info)
   *
   * \brief Constructs the SSAP format join message.
   *
   * \param[in]  ss_info_t * ss_info. A pointer to the struct holding neccessary node_id 
   *             and space_id information.
   */
 void make_join_msg(ss_info_t * ss_info);

/**
  * \fn void make_leave_msg(ss_info_t * ss_info)
  *
  * \brief Constructs the SSAP format leave message.
  *
  * \param[in]  ss_info_t * ss_info. A pointer to the struct holding neccessary 
  *             node_id and   space_id information.
  */
void make_leave_msg(ss_info_t * ss_info);

/**
  * \fn void make_query_msg(ss_info_t * ss_info, ss_triple_t * requested_triples)
  *
  * \brief Constructs the SSAP format query message.
  *
  * \param[in] ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and space_id information.
  * \param[in] ss_triple_t * requested_triples.  
  */
void make_query_msg(ss_info_t * ss_info, ss_triple_t * requested_triples);
/**
  * \fn void make_sparql_msg(ss_info_t * ss_info, char * query)
  *
  * \brief Constructs the SSAP format SPARQL query message.
  *
  * \param[in] ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and space_id information.
  * \param[in] char * query.  
  */
void make_sparql_msg(ss_info_t * ss_info, char * query);
/**
  * \fn void make_insert_msg(ss_info_t * ss_info, ss_triple_t * triple)
  *
  * \brief Constructs the SSAP format insert message.
  *
  * \param[in]  ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and 
  *             space_id information.
  * \param[out] char * message. Buffer holding the ready insert message.
  * \param[in]  ss_triple_t * triple. Pointer to the frist triple in triple list.
  */
void make_insert_msg(ss_info_t * ss_info, ss_triple_t * triple);

/**
  * \fn void make_graph_insert_msg(ss_info_t * ss_info, char * graph)
  *
  * \brief Constructs the SSAP format insert graph in RDF-XML notation message.
  *
  * \param[in]  ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and 
  *             space_id information.
  * \param[in]  char * graph. string that contained RDF-XML graph.
  */
void make_graph_insert_msg(ss_info_t * ss_info, char * graph);
 
/**
  * \fn void make_update_msg(ss_info_t * ss_info, ss_triple_t * inserted_triples, ss_triple_t * removed_triples)
  *
  * \brief Constructs the SSAP format update message.
  *
  * \param[in]  ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and 
  *             space_id information.
  * \param[in]  ss_triple_t * inserted_triples. Pointer to the frist triple to be inserted.
  * \param[in]  ss_triple_t * removed_triples. Pointer to the frist triple to be removed.
  */
void make_update_msg(ss_info_t * ss_info, ss_triple_t * inserted_triples, ss_triple_t * removed_triples);

/**
  * \fn void make_graph_update_msg(ss_info_t * ss_info, char * inserted_graph, char * removed_graph)
  *
  * \brief Constructs the SSAP format update message.
  *
  * \param[in]  ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and 
  *             space_id information.
  * \param[in]  char * inserted_graph. Pointer to rdf-xml notation of triples to be inserted.
  * \param[in]  char * removed_graph. Pointer to rdf-xml notation of triples to be removed.
  */
void make_graph_update_msg(ss_info_t * ss_info, char * inserted_graph, char * removed_graph);

/**
  * \fn void make_remove_msg(ss_info_t * ss_info, ss_triple_t * removed_triples)
  *
  * \brief Constructs the SSAP format remove message.
  *
  * \param[in]  ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and 
  *             space_id information.
  * \param[in]  ss_triple_t * removed_triples. Pointer to the frist triple to be removed.
  */
void make_remove_msg(ss_info_t * ss_info, ss_triple_t * removed_triples);

/**
  * \fn void make_graph_remove_msg(ss_info_t * ss_info, char * graph)
  *
  * \brief Constructs the SSAP format remove graph in RDF-XML notation message.
  *
  * \param[in]  ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and 
  *             space_id information.
  * \param[in]  char * graph. string that contained RDF-XML graph.
  */
void make_graph_remove_msg(ss_info_t * ss_info, char * graph);

/**
  * \fn void make_subscribe_msg(ss_info_t * ss_info, ss_triple_t * requested_triples)
  *
  * \brief Constructs the SSAP format subscribe message.
  *
  * \param[in]  ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and 
  *            space_id information.
  * \param[in] ss_triple_t * requested_triples. Pointer to the first triple requested from the SIB.
  */
void make_subscribe_msg(ss_info_t * ss_info, ss_triple_t * requested_triples);					  

/**
  * \fn void make_sparql_subscribe_msg(ss_info_t * ss_info, char * query)
  *
  * \brief Constructs the SSAP format subscribe message.
  *
  * \param[in]  ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and 
  *            space_id information.
  * \param[in] char * query. SPARQL query to be requested from SIB.
  */
void make_sparql_subscribe_msg(ss_info_t * ss_info, char * query);

/**
  * \fn void make_unsubscribe_msg(ss_info_t * ss_info, char * subscribe_id)
  *
  * \brief Constructs the SSAP format unsubscribe message.
  *
  * \param[in]  ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and 
  *            space_id information.
  * \param[in] char * subscribe_id. ID of the subscribe operation to be terminated.
  */
void make_unsubscribe_msg(ss_info_t * ss_info, char * subscribe_id);

#endif

