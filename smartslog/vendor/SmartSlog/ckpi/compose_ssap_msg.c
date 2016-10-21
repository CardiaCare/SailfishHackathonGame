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
  * compose_ssap_msg.c
  *
  * \brief Implementations of functions constructing SSAP transaction messages.
  *
  * Author: Jussi Kiljander, VTT Technical Research Centre of Finland
  */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <scew/str.h>

#include "ckpi.h"

#include "compose_ssap_msg.h"
#include "ssap_msg_common.h"

/*
 *****************************************************************************
 *  MACROS
 *****************************************************************************
 */

#define QUERY_FOOTER_SIZE   (43)
#define INSERT_FOOTER_SIZE  (89)
#define UPDATE_MIDDLE_SIZE  (93)

#define XML_TRIPLE_OVERHEAD (103)  /*  <triple><subject type = ""></subject><predicate></predicate><object type = ""></object></triple> */

/*
 *****************************************************************************
 *  LOCAL FUNCTIONS
 *****************************************************************************
 */

int make_xml_triple_list(char * xml, ss_triple_t * triple_list, int free_space)
{ 
  int bytes_req = 0;
  int bytes = 0;
  int total_bytes = free_space;
  ss_triple_t * triple = triple_list;

  while(triple && (free_space > 0))
  {
    /* check whether there is enough free space to add one more triple */
    bytes_req = XML_TRIPLE_OVERHEAD;
    bytes_req += strlen(triple->subject);
    bytes_req += (triple->subject_type == SS_RDF_TYPE_URI) ? strlen(URI_STRING) : strlen(BNODE_STRING);
    bytes_req += strlen(triple->predicate);
    bytes_req += strlen(triple->object);
    bytes_req += (triple->object_type == SS_RDF_TYPE_URI) ? strlen(URI_STRING) : ((triple->object_type == SS_RDF_TYPE_LIT) ? strlen(LITERAL_STRING) : strlen(BNODE_STRING));
   
    if(free_space < bytes_req)
    {
      SS_DEBUG_PRINT(("WARNING: buffer too small for all triples\n"));
      break;
    }
    if(triple->subject_type == SS_RDF_TYPE_URI)
      bytes = sprintf(xml, "<triple><subject type = \"%s\">%s</subject><predicate>%s</predicate>", URI_STRING, triple->subject, triple->predicate);
    else
      bytes = sprintf(xml, "<triple><subject type = \"%s\">%s</subject><predicate>%s</predicate>", BNODE_STRING, triple->subject, triple->predicate);

    xml += bytes;

    free_space -= bytes;

    if(triple->object_type == SS_RDF_TYPE_URI)
      bytes = sprintf(xml, "<object type = \"%s\">%s</object></triple>", URI_STRING, triple->object);
    else if(triple->object_type == SS_RDF_TYPE_LIT)
      bytes = sprintf(xml, "<object type = \"%s\"><![CDATA[%s]]></object></triple>", LITERAL_STRING, triple->object);
    else
      bytes = sprintf(xml, "<object type = \"%s\">%s</object></triple>", BNODE_STRING, triple->object);

    xml += bytes;

    free_space -= bytes;
    triple = triple->next;
  }
  
  return (total_bytes - free_space);
}

/*
 *****************************************************************************
 *  EXPORTED FUNCTION IMPLEMENTATIONS
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
 void make_join_msg(ss_info_t * ss_info)
 {
   /* ATTENTION: make sure that the message buffer is big enough to hold the message written by sprintf */
   /* maybe snprintf should be used instead */
   sprintf(ss_info->ssap_msg, "<SSAP_message>\
<transaction_type>JOIN</transaction_type>\
<message_type>REQUEST</message_type>\
<transaction_id>%d</transaction_id>\
<node_id>%s</node_id>\
<space_id>%s</space_id>\
</SSAP_message>", ss_info->transaction_id, ss_info -> node_id, ss_info -> space_id);
 }

/**
  * \fn void make_leave_msg(ss_info_t * ss_info)
  *
  * \brief Constructs the SSAP format leave message.
  *
  * \param[in]  ss_info_t * ss_info. A pointer to the struct holding neccessary 
  *             node_id and   space_id information.
  */
void make_leave_msg(ss_info_t * ss_info)
{
  sprintf(ss_info->ssap_msg, "<SSAP_message>\
<transaction_type>LEAVE</transaction_type>\
<message_type>REQUEST</message_type>\
<transaction_id>%d</transaction_id>\
<node_id>%s</node_id>\
<space_id>%s</space_id>\
</SSAP_message>", ss_info->transaction_id, ss_info -> node_id, ss_info -> space_id);
}
 
 
/**
  * \fn void make_query_msg(ss_info_t * ss_info, ss_triple_t * requested_triples)
  *
  * \brief Constructs the SSAP format query message.
  *
  * \param[in] ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and space_id information.
  * \param[in] ss_triple_t * requested_triples.  
  */
void make_query_msg(ss_info_t * ss_info, ss_triple_t * requested_triples)
{
  char * header = NULL;
  char * triple = NULL;
  char * footer = NULL;
  int bytes = 0;
  header = ss_info->ssap_msg;
 
  bytes = sprintf(header, "<SSAP_message>\
<transaction_type>QUERY</transaction_type>\
<message_type>REQUEST</message_type>\
<transaction_id>%d</transaction_id>\
<node_id>%s</node_id>\
<space_id>%s</space_id>\
<parameter name = \"type\">RDF-M3</parameter>\
<parameter name = \"query\">\
<triple_list>"
,ss_info->transaction_id, ss_info -> node_id, ss_info -> space_id);

  triple = header + bytes;
  
  bytes = make_xml_triple_list(triple, requested_triples, SS_MAX_MESSAGE_SIZE - QUERY_FOOTER_SIZE - bytes);

  footer = triple + bytes;
  sprintf(footer, "</triple_list></parameter></SSAP_message>"); 
}

void make_sparql_msg(ss_info_t * ss_info, char * query)
{
    char *escaped_query = scew_strescape(query);

    sprintf(ss_info -> ssap_msg, "<SSAP_message>\
            <transaction_type>QUERY</transaction_type>\
            <message_type>REQUEST</message_type>\
            <transaction_id>%d</transaction_id>\
            <node_id>%s</node_id>\
            <space_id>%s</space_id>\
            <parameter name = \"type\">sparql</parameter>\
            <parameter name = \"query\">%s</parameter></SSAP_message>",
            ss_info->transaction_id, ss_info -> node_id, ss_info -> space_id, escaped_query);

    free (escaped_query);
}


/**
  * \fn void make_insert_msg(ss_info_t * ss_info, ss_triple_t * triple)
  *
  * \brief Constructs the SSAP format insert message.
  *
  * \param[in]  ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and 
  *             space_id information.
  * \param[in]  ss_triple_t * triple. Pointer to the frist triple in triple list.
  */
void make_insert_msg(ss_info_t * ss_info, ss_triple_t * triple)
{
  int bytes = 0;
  char * insert_header = NULL;
  char * insert_triple = NULL;
  char * insert_footer = NULL;

  insert_header = ss_info->ssap_msg;
 
  bytes = sprintf(insert_header, "<SSAP_message>\
<transaction_type>INSERT</transaction_type>\
<message_type>REQUEST</message_type>\
<transaction_id>%d</transaction_id>\
<node_id>%s</node_id>\
<space_id>%s</space_id>\
<parameter name = \"insert_graph\" encoding = \"RDF-M3\">\
<triple_list>", ss_info->transaction_id, ss_info -> node_id, ss_info -> space_id);

  insert_triple = insert_header + bytes;

  bytes = make_xml_triple_list(insert_triple, triple, SS_MAX_MESSAGE_SIZE - INSERT_FOOTER_SIZE- bytes);

  insert_footer = insert_triple + bytes;
  sprintf(insert_footer, "</triple_list></parameter><parameter name = \"confirm\">TRUE</parameter></SSAP_message>"); 
 } 

/**
  * \fn void make_graph_insert_msg(ss_info_t * ss_info, char * graph)
  *
  * \brief Constructs the SSAP format insert graph in RDF-XML notation message.
  *
  * \param[in]  ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and 
  *             space_id information.
  * \param[in]  char * graph. string that contained RDF-XML graph.
  */
void make_graph_insert_msg(ss_info_t * ss_info, char * graph)
{
  sprintf(ss_info->ssap_msg, "<SSAP_message>\
  <transaction_type>INSERT</transaction_type>\
  <message_type>REQUEST</message_type>\
  <transaction_id>%d</transaction_id>\
  <node_id>%s</node_id>\
  <space_id>%s</space_id>\
  <parameter name = \"insert_graph\" encoding = \"RDF-XML\">\
  %s</parameter><parameter name = \"confirm\">TRUE</parameter></SSAP_message>", 
  ss_info->transaction_id, ss_info -> node_id, ss_info -> space_id, graph);
} 
 
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
void make_update_msg(ss_info_t * ss_info, ss_triple_t * inserted_triples, ss_triple_t * removed_triples)
{
  int bytes = 0;
  char * header = NULL;
  char * payload = NULL;
  char * footer = NULL;
   
  header = ss_info->ssap_msg;
 
  bytes = sprintf(header, "<SSAP_message>\
<transaction_type>UPDATE</transaction_type>\
<message_type>REQUEST</message_type>\
<transaction_id>%d</transaction_id>\
<node_id>%s</node_id>\
<space_id>%s</space_id>\
<parameter name = \"insert_graph\" encoding = \"RDF-M3\">\
<triple_list>", ss_info->transaction_id, ss_info -> node_id, ss_info -> space_id);

  payload = header + bytes;

  bytes += make_xml_triple_list(payload, inserted_triples, SS_MAX_MESSAGE_SIZE - INSERT_FOOTER_SIZE - UPDATE_MIDDLE_SIZE - bytes);

  payload = header + bytes;
  bytes += sprintf(payload, "</triple_list></parameter><parameter name = \"remove_graph\" encoding = \"RDF-M3\"><triple_list>");

  payload = header + bytes;

  bytes = make_xml_triple_list(payload, removed_triples, SS_MAX_MESSAGE_SIZE - INSERT_FOOTER_SIZE - bytes);

  footer = payload + bytes;
  sprintf(footer, "</triple_list></parameter><parameter name = \"confirm\">TRUE</parameter></SSAP_message>");  

}

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
void make_graph_update_msg(ss_info_t * ss_info, char * inserted_graph, char * removed_graph)
{
sprintf(ss_info->ssap_msg, "<SSAP_message>\
<transaction_type>UPDATE</transaction_type>\
<message_type>REQUEST</message_type>\
<transaction_id>%d</transaction_id>\
<node_id>%s</node_id>\
<space_id>%s</space_id>\
<parameter name = \"insert_graph\" encoding = \"RDF-XML\">%s</parameter>\
<parameter name = \"remove_graph\" encoding = \"RDF-XML\">%s</parameter>\
<parameter name = \"confirm\">TRUE</parameter></SSAP_message>\
", ss_info->transaction_id, ss_info -> node_id, ss_info -> space_id, inserted_graph, removed_graph);

}

/**
  * \fn void make_remove_msg(ss_info_t * ss_info, ss_triple_t * removed_triples)
  *
  * \brief Constructs the SSAP format remove message.
  *
  * \param[in]  ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and 
  *             space_id information.
  * \param[in]  ss_triple_t * removed_triples. Pointer to the frist triple to be removed.
  */
void make_remove_msg(ss_info_t * ss_info, ss_triple_t * removed_triples)
{
  int bytes = 0;
  char * header = NULL;
  char * triple = NULL;
  char * footer = NULL;

  header = ss_info->ssap_msg;
 
  bytes = sprintf(header,"<SSAP_message>\
<transaction_type>REMOVE</transaction_type>\
<message_type>REQUEST</message_type>\
<transaction_id>%d</transaction_id>\
<node_id>%s</node_id>\
<space_id>%s</space_id>\
<parameter name = \"remove_graph\" encoding = \"RDF-M3\">\
<triple_list>", ss_info->transaction_id, ss_info -> node_id, ss_info -> space_id);

  triple = header + bytes;

  bytes = make_xml_triple_list(triple, removed_triples, SS_MAX_MESSAGE_SIZE - INSERT_FOOTER_SIZE- bytes);

  footer = triple + bytes;

  sprintf(footer, "</triple_list></parameter><parameter name = \"confirm\">TRUE</parameter></SSAP_message>");
}

/**
  * \fn void make_graph_remove_msg(ss_info_t * ss_info, char * graph)
  *
  * \brief Constructs the SSAP format remove graph in RDF-XML notation message.
  *
  * \param[in]  ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and 
  *             space_id information.
  * \param[in]  char * graph. string that contained RDF-XML graph.
  */
void make_graph_remove_msg(ss_info_t * ss_info, char * graph)
{
  sprintf(ss_info->ssap_msg, "<SSAP_message>\
  <transaction_type>remove</transaction_type>\
  <message_type>REQUEST</message_type>\
  <transaction_id>%d</transaction_id>\
  <node_id>%s</node_id>\
  <space_id>%s</space_id>\
  <parameter name = \"remove_graph\" encoding = \"RDF-XML\">\
  %s</parameter><parameter name = \"confirm\">TRUE</parameter></SSAP_message>", 
  ss_info->transaction_id, ss_info -> node_id, ss_info -> space_id, graph);
} 

/**
  * \fn void make_subscribe_msg(ss_info_t * ss_info, ss_triple_t * requested_triples)
  *
  * \brief Constructs the SSAP format subscribe message.
  *
  * \param[in]  ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and 
  *            space_id information.
  * \param[in] ss_triple_t * requested_triples. Pointer to the first triple requested from the SIB.
  */
void make_subscribe_msg(ss_info_t * ss_info, ss_triple_t * requested_triples)
{
  int bytes = 0;
  char * header = NULL;
  char * triple = NULL;
  char * footer = NULL;
  header = ss_info->ssap_msg;
 
  bytes = sprintf(header, "<SSAP_message>\
<transaction_type>SUBSCRIBE</transaction_type>\
<message_type>REQUEST</message_type>\
<transaction_id>%d</transaction_id>\
<node_id>%s</node_id>\
<space_id>%s</space_id>\
<parameter name = \"type\">RDF-M3</parameter>\
<parameter name = \"query\">\
<triple_list>", ss_info->transaction_id, ss_info -> node_id, ss_info -> space_id);

  triple = header + bytes;
  
  bytes = make_xml_triple_list(triple, requested_triples, SS_MAX_MESSAGE_SIZE - QUERY_FOOTER_SIZE- bytes);

  footer = triple + bytes;
  sprintf(footer, "</triple_list></parameter></SSAP_message>"); 
}

/**
  * \fn void make_sparql_subscribe_msg(ss_info_t * ss_info, char * query)
  *
  * \brief Constructs the SSAP format subscribe message.
  *
  * \param[in]  ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and 
  *            space_id information.
  * \param[in] char * query. SPARQL query to be requested from SIB.
  */
void make_sparql_subscribe_msg(ss_info_t * ss_info, char * query)
{
    char * escaped_query = scew_strescape(query);

    sprintf(ss_info->ssap_msg, "<SSAP_message>\
            <transaction_type>SUBSCRIBE</transaction_type>\
            <message_type>REQUEST</message_type>\
            <transaction_id>%d</transaction_id>\
            <node_id>%s</node_id>\
            <space_id>%s</space_id>\
            <parameter name = \"type\">sparql</parameter>\
            <parameter name = \"query\">%s</parameter>\
            </SSAP_message>",
            ss_info->transaction_id, ss_info -> node_id, ss_info -> space_id, escaped_query);

    free (escaped_query);
}

/**
  * \fn void make_unsubscribe_msg(ss_info_t * ss_info, char * subscribe_id)
  *
  * \brief Constructs the SSAP format unsubscribe message.
  *
  * \param[in]  ss_info_t * ss_info. A pointer to the struct holding neccessary node_id and 
  *            space_id information.
  * \param[in] char * subscribe_id. ID of the subscribe operation to be terminated.
  */
void make_unsubscribe_msg(ss_info_t * ss_info, char * subscribe_id)
{
  sprintf(ss_info->ssap_msg, "<SSAP_message>\
<transaction_type>UNSUBSCRIBE</transaction_type>\
<message_type>REQUEST</message_type>\
<transaction_id>%d</transaction_id>\
<node_id>%s</node_id>\
<space_id>%s</space_id>\
<parameter name = \"subscription_id\">%s</parameter>\
</SSAP_message>", ss_info->transaction_id, ss_info -> node_id, ss_info -> space_id, subscribe_id);
}
