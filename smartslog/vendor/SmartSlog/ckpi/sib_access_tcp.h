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
 * \file sib_access_tcp.h
 *
 * \brief API for sending/reveiving the SSAP transaction messages using TCP/IP.
 *
 * Author: Jussi Kiljander, VTT Technical Research Centre of Finland
 */

#ifndef SIB_ACCESS_TCP_H
#define SIB_ACCESS_TCP_H

/*
*****************************************************************************
*  MACROS
*****************************************************************************
*/

#define MAX_IP_LEN (16)

/*
*****************************************************************************
*  DATA TYPES
*****************************************************************************
*/

/**
 * \struct sib_address
 *
 * \brief A struct containing NoTA address information of the sib.
 *
 */
typedef struct sib_address
{
  char ip[MAX_IP_LEN];
  int  port;

}sib_address_t;

/**
   *  \struct multi_msg
   *
   * \brief Struct contains size information of the subscribe indication messages.
   *
   * This struct is neccessary to cope with multiple simultaneuous indication messages.
   */
  typedef struct multi_msg
  {
    int size;
    struct multi_msg * next;

  }multi_msg_t;


/*
*****************************************************************************
*  EXPORTED FUNCTION PROTOTYPES
*****************************************************************************
*/


char** ss_discovery_sib(const char *broadcast_aaddress, int port, int *messages_count);


/**
 * \fn ss_open()
 *
 * \brief Creates a socket and connects socket to the IP and port specified in
 *        sib_address_t struct.
 *
 * \param[in] sib_address_t *tcpip_i. A pointer to the struct holding neccessary address
 *            information.
 *
 * \return int. If successfull returns the socket descriptor, otherwise -1.
 */
int ss_open(sib_address_t *tcpip_i);

/**
 * \fn ss_send()
 *
 * \brief Sends data to the Smart Space (SIB).
 *
 * \param[in] int socket. File descriptor of the socket to send.
 * \param[in] char * send_buf. A pointer to the data to be send.
 *
 * \return int. 0 if successful, otherwise -1.
 */
int ss_send(int socket, char * send_buf);

/**
 * \fn ss_send_to_address()
 *
 * \brief Sends data to the some URL-address.
 *
 * \param[in] const char *addrrss. Address (without protocol and path) to get IP address and send (ya.ru, google.ru).
 * \param[in] int port. Port to send.
 * \param[in] char *request. Request to send.
 * \param[in] char **result_buf. Buffer for response.
 *
 * \return int. 0 if successful, otherwise -1.
 */
int ss_send_to_address(const char *addrrss, const char *port, const char *request, char **result_buf);

void ss_get_broadcast_addreses();

/**
 * \fn int ss_recv()
 *
 * \brief Receives SSAP message from the Smart Space (SIB).
 *
 * \param[in] int socket. The socket descriptor of the socket where data is received from.
 * \param[in] char * recv_buf. A pointer to the data to be received.
 * \param[in] int to_msecs. Timeout value in milliseconds.
 *
 * \return int. Success: 1
 *              Timeout: 0
 *              ERROR:  -1
 */
int ss_recv(int socket, char * recv_buf, int to_msecs);


/**
 * \fn int ss_mrecv()
 *
 * \brief Receives (possibly) multiple SSAP messages from the Smart Space (SIB).
 *
 * \param[in] multi_msg_t * m. Pointer to the multi_msg_t struct.
 * \param[in] int socket. The socket descriptor of the socket where data is received from.
 * \param[in] char * recv_buf. A pointer to the data to be received.
 * \param[in] int to_msecs. Timeout value in milliseconds.
 *
 * \return int. Success: 1
 *              Timeout: 0
 *              ERROR:  -1
 */
int ss_mrecv(multi_msg_t ** m, int socket, char * recv_buf, int to_msecs);


/**
 * \fn ss_close()
 *
 * \brief Closes the socket.
 *
 * \param[in] int socket. File descriptor of the socket to be closed
 *
 * \return int. 0 if successful, otherwise -1.
 */
int ss_close(int socket);

#endif /* SIB_ACCESS_TCP_H */
