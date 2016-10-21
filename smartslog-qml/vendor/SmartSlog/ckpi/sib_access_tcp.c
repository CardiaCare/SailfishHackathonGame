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
* \fn sib_access_tcp.c
*
* \brief Implementations  of function for sending/reveiving the SSAP transaction messages
*        using TCP/IP.
*
* Author: Jussi Kiljander, VTT Technical Research Centre of Finland
*/


#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>

#if defined(WIN32) || defined(WINCE)
#include <ws2tcpip.h>
#include <iphlpapi.h>
#else
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h> /* close() */
#include <stdlib.h>
#include <ifaddrs.h> // getifaddrs
#endif


#include "ckpi.h"
#include "sib_access_tcp.h"


/*
*****************************************************************************
*  LOCAL FUNCTION PROTOTYPES
*****************************************************************************
*/

/**
* \fn timeout_recv()
*
* \brief recv() with timeout implemented using select().
*
* \param[in] int socket. File descriptor of the socket to receive from.
* \param[in/out] char * recv_buf. Received data is copied to this buffer.
* \param[in] int len. Length of the recv_buf.
* \param[in] int to_msecs. Timeout value in milliseconds.
*/
static int timeout_recv(int socket, char * recv_buf, int len, int to_msecs);

/**
 * \fn int send_receive_discovery()
 *
 * \brief Creates a broadcast socket, sends discovery message and handles responses
 * Received messages are added to the buffer-list (messages) starting from given index (messages_count).
 *
 * \param[in] const char *ip_address. String with broadcast IP address.
 * \param[in] int port. Port to send broadcast.
 * \param[in|out] char ***messages. Buffer-list to store responses messages from SIBs. 
 * If it equals NULL, then it will be created. Free buffer-list after using (free each message and list itself).
 * \param[in|out] int *messages_count. Count af received messages in the messages buffer. 
 * Starting from this index new messages will be inserted to buffer-list. It will be updated after 
 * inserting.
 *
 * \return int. If successful returns 0, otherwise -1.
 */
static int send_receive_discovery(const char *ip_address, int port, char ***messages, int* messages_count);

/**
 * \fn int open_broadcast()
 *
 * \brief Creates a broadcast socket and and prepare it to work.
 *
 * \param[in] const char* broadcast_address. Broadcast ip address for socket.
 * \param[in] int port. Port for socket.
 * \param[in/out] struct sockaddr_in *address. Structure for prepared address of socket.
 *
 * \return int. If successful returns the socket descriptor, otherwise -1.
 */
static int open_broadcast(const char *broadcast_address, int port, struct sockaddr_in *address);


/*
*****************************************************************************
*  EXPORTED FUNCTION IMPLEMENTATIONS
*****************************************************************************
*/

/**
* \fn int ss_open()
*
* \brief Creates a socket and connects socket to the IP and port specified in
*        sib_address_t struct.
*
* \param[in] sib_address_t *tcpip_i. A pointer to the struct holding neccessary address
*            information.
*
* \return int. If successfull returns the socket descriptor, otherwise -1.
*/
int ss_open(sib_address_t *tcpip_i)
{
	int sockfd;
	struct sockaddr_in sib_addr;

	sockfd = socket(PF_INET, SOCK_STREAM, 0);

	if(sockfd < 0)
	{
		SS_DEBUG_PRINT("ERROR: unable to create socket\n");
		return -1;
	}

	sib_addr.sin_family = AF_INET;
	sib_addr.sin_port = htons(tcpip_i->port);
	sib_addr.sin_addr.s_addr = inet_addr(tcpip_i->ip);

	memset(sib_addr.sin_zero, '\0', sizeof sib_addr.sin_zero);

	if(connect( sockfd, (struct sockaddr *)&sib_addr, sizeof(sib_addr)) < 0)
	{
#if defined(WIN32) || defined (WINCE)
		closesocket(sockfd);
#else
		close(sockfd);
#endif
		SS_DEBUG_PRINT("ERROR: unable to connect socket\n");
		return -1;
	}

	return sockfd;
}

/**
* \fn int ss_send()
*
* \brief Sends data to the Smart Space (SIB).
*
* \param[in] int socket. File descriptor of the socket to send.
* \param[in] char * send_buf. A pointer to the data to be send.
*
* \return int. 0 if successful, otherwise -1.
*/
int ss_send(int socket, char * send_buf)
{

	int bytes_sent = 0;
	int bytes_left = strlen(send_buf);
	int bytes;

	while(bytes_left > 0)
	{
		bytes = send(socket, send_buf + bytes_sent, bytes_left, 0);
		if(bytes < 0)
			return -1;

		bytes_sent += bytes;
		bytes_left -= bytes;
	}

	return 0;
}

/**
* \fn int ss_recv()
*
* \brief Receives data to the Smart Space (SIB).
*
* \param[in] int socket. The socket descriptor of the socket where data is received from.
* \param[in] char * recv_buf. A pointer to the data to be received.
* \param[in] int to_msecs. Timeout value in milliseconds.
*
* \return int. Success: 1
*              Timeout: 0
*              ERROR:  -1
*/
int ss_recv(int socket, char * recv_buf, int to_msecs)
{
	int offset = 0;
	int bytes = 0;
	char * msg_end = NULL;
	int len = SS_MAX_MESSAGE_SIZE - 1;

	do{
		bytes = timeout_recv(socket, recv_buf + offset, len, to_msecs);
		if(bytes <= 0)
			return bytes;

		offset += bytes;
		len -= bytes;

		msg_end = strstr(recv_buf, SS_END_TAG);

	}while(!msg_end && len > 1);


	return bytes;
}


/**
* \fn int ss_recv()
*
* \brief Receives data to the Smart Space (SIB).
*
* \param[in] int socket. The socket descriptor of the socket where data is received from.
* \param[in] char * recv_buf. A pointer to the data to be received.
* \param[in] int to_msecs. Timeout value in milliseconds.
*
* \return int. Success: 1
*              Timeout: 0
*              ERROR:  -1
*/
int ss_recv_sparql(int socket, char * recv_buf, int to_msecs)
{
	int offset = 0;
	int bytes = 0;
	char * msg_end = NULL;
	int len = SS_MAX_MESSAGE_SIZE - 1;

	do{
		bytes = timeout_recv(socket, recv_buf + offset, len, to_msecs);
		if(bytes <= 0)
			return bytes;

		offset += bytes;
		len -= bytes;

		msg_end = strstr(recv_buf, SS_SPARQL_END_TAG);

	}while(!msg_end && len > 1);


	return bytes;
}


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
int ss_send_to_address(const char *addrrss, const char *port, const char *request, char **result_buf)
{
	struct addrinfo hints;
	struct addrinfo *ai = NULL;//  ss_get_ip_address((char*)addrrss, (char*)port);  

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int result = getaddrinfo(addrrss, port, &hints, &ai);

	if (result != 0) {
		SS_DEBUG_PRINT("ERROR: getaddrinfo() error: %s\n", (char *) gai_strerror(result));
		return -1;
	}

	struct sockaddr_in *sin = (struct sockaddr_in *) (ai->ai_addr);

	const char *addr = inet_ntoa(sin->sin_addr);
	if (addr == NULL) { 
		return -1;
	}

	sib_address_t sa;
	strncpy(sa.ip, addr, MAX_IP_LEN);
	sa.port = atoi(port);

	freeaddrinfo(ai);

	int sockfd = ss_open(&sa);

	if (ss_send(sockfd, (char *)request) < 0) 
	{
		SS_DEBUG_PRINT("ERROR: Sending error.");
		return -1;
	}

	if (ss_recv_sparql(sockfd, *result_buf, SS_RECV_TIMEOUT_MSECS) <= 0) {
		SS_DEBUG_PRINT("ERROR: Receiving error.");		
		return -1;
	}

	return 0;
}


// Discovery implementation for Windows OS.
#if defined(WIN32) || defined (WINCE)

/**
* \fn ntoa()
*
* \brief Converts a numeric IP address into its string representation.
*
* \param[in] unsigned long address. Numeric IP address representation.
* \param[in/out] char *ip_buffer. Buffer for the text IP repsentation.
*/
static void ntoa(unsigned long address, char *ip_buffer)
{
	sprintf(ip_buffer, "%li.%li.%li.%li", (address >> 24) & 0xFF, (address >> 16) & 0xFF, (address >> 8) & 0xFF, (address >> 0) & 0xFF);
}


/**
* \fn ss_discovery_sib()
*
* \brief Discoveries SIB.
* If given broadcast is null then gets broadcast address of active network interfaces 
* and sends discovery packets. After receiving response message function store it in list. 
* List with strings (packets-messages from SIBs) returns to caller.
*
* \param[in] const char *broadcast_aaddress. Broadcast IP address.
* \param[in] int port. Port to send.
* \param[in] int *messages_count. Count of packets-messages from SIBs in the returned list. 
*
* \return char**. List with messages-packets from SIB if successful, otherwise NULL.
*/
char** ss_discovery_sib(const char *broadcast_aaddress, int port, int *messages_count)
{
	// Buffer for receiving SIB discovery responses.
	char **messages = NULL;
    *messages_count = 0;
	
	// Process discovery for given address.
	if (broadcast_aaddress != NULL) 
	{
		int result = send_receive_discovery(broadcast_aaddress, port, &messages, messages_count);

		if (result < 0) 
		{
			SS_DEBUG_PRINT("ERROR: Could not process discovery broadcast for '%s'.\n", broadcast_aaddress);
			return NULL;        
		}

        return messages;
	}

	// If broadcast address is not given then get broadcast addresses
	// of active network interfaces and send discovery message.

	// Adapted from example code at http://msdn2.microsoft.com/en-us/library/aa365917.aspx
	// Now get Windows' IPv4 addresses table.  We gotta call GetIpAddrTable()
	// multiple times in order to deal with potential race conditions properly.
	MIB_IPADDRTABLE * ip_table = NULL;
	ULONG buffer_len = 0;

	for (int i=0; i<5; i++)
	{
		DWORD ipRet = GetIpAddrTable(ip_table, &buffer_len, false);
		if (ipRet == ERROR_INSUFFICIENT_BUFFER)
		{
			free(ip_table);  // In case we had previously allocated it.
			ip_table = (MIB_IPADDRTABLE *) malloc(buffer_len);
		}
		else if (ipRet == NO_ERROR) 
		{
			break;
		}
		else
		{
			free(ip_table);
			ip_table = NULL;
			break;
		}
	}

	if (ip_table == NULL) 
	{
		SS_DEBUG_PRINT("ERROR: can't et ip address table.");
	}

	for (DWORD i = 0; i < ip_table->dwNumEntries; ++i)
	{
		const MIB_IPADDRROW &row = ip_table->table[i];

		unsigned long ip_address  = ntohl(row.dwAddr);
		unsigned long netmask = ntohl(row.dwMask);
		unsigned long broadcast   = ip_address & netmask;

		if (row.dwBCastAddr) broadcast |= ~netmask;

		char broadcast_text[32];  
		ntoa(broadcast, broadcast_text);

		SS_DEBUG_PRINT("  Found interface: broadcast=[%s]\n", broadcast_text);

		int result = send_receive_discovery(broadcast_text, port, &messages, messages_count);

		if (result < 0) {
			SS_DEBUG_PRINT("ERROR: Could not process discovery broadcast for '%s'.\n", broadcast_text);
			continue;        
		}
	}

	free(ip_table);

	return messages;
}

#else // Discovery implementation for Linux/Unix like OS.

/**
* \fn ss_discovery_sib()
*
* \brief Discoveries SIB.
* If given broadcast is null then gets broadcast address of active network interfaces 
* and sends discovery packets. After receiving response message function store it in list. 
* List with strings (packets-messages from SIBs) returns to caller.
*
* \param[in] const char *broadcast_aaddress. Broadcast IP address.
* \param[in] int port. Port to send.
* \param[in] int *messages_count. Count of packets-messages from SIBs in the returned list. 
*
* \return char**. List with messages-packets from SIB if successful, otherwise NULL.
*/
char** ss_discovery_sib(const char *broadcast_aaddress, int port, int *messages_count)
{
	// Buffer for receiving SIB discovery response.
	char **messages = NULL;
	*messages_count = 0;

	// Process discovery for given address.
	if (broadcast_aaddress != NULL) 
	{
		int result = send_receive_discovery(broadcast_aaddress, port, &messages, messages_count);

		if (result < 0) 
		{
			SS_DEBUG_PRINT("ERROR: Could not process discovery broadcast for '%s'.\n", broadcast_aaddress);
			return NULL;        
		}
        
		return messages;
	}

	// If broadcast address is not given then get broadcast addresses
	// of active network interfaces and send discovery message.

	struct ifaddrs *ifaddr = NULL;

	// Get information about network interfaces.
	if (getifaddrs(&ifaddr) == -1) 
	{
		SS_DEBUG_PRINT("ERROR: failed to get network addresses.");
		return NULL;
	}

	struct ifaddrs *ifaddr_walker = NULL;
	int max_messages_count = 0;

	for (ifaddr_walker = ifaddr; ifaddr_walker != NULL; ifaddr_walker = ifaddr_walker->ifa_next) 
	{
		++max_messages_count;
	}

	// Get broadcast addresses from interfaces and send messages.
	for (ifaddr_walker = ifaddr; ifaddr_walker != NULL; ifaddr_walker = ifaddr_walker->ifa_next) 
	{
		if (ifaddr_walker->ifa_broadaddr == NULL) 
		{
			continue;  
		}

		if (ifaddr_walker->ifa_broadaddr->sa_family != AF_INET
			|| &((struct sockaddr_in *) ifaddr_walker->ifa_broadaddr)->sin_addr == NULL)
		{
			continue;
		}

		char ip[INET6_ADDRSTRLEN];

		inet_ntop(ifaddr_walker->ifa_broadaddr->sa_family, 
			&((struct sockaddr_in *) ifaddr_walker->ifa_broadaddr)->sin_addr, 
			ip, sizeof ip);

		SS_DEBUG_PRINT("\tInterface name: <%s>.\n", ifaddr_walker->ifa_name );
		SS_DEBUG_PRINT("\tInterface broadcast: <%s>.\n", ip);

		int result = send_receive_discovery(ip, port, &messages, messages_count);
        
		if (result < 0) {
			SS_DEBUG_PRINT("ERROR: Could not process discovery broadcast for '%s'.\n", ip);
			continue;        
		}
	}

	freeifaddrs(ifaddr);

	return messages;
}

#endif


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
int ss_mrecv(multi_msg_t ** mfirst, int socket, char * recv_buf, int to_msecs)
{
	int offset = 0;
	int bytes = 0;
	int len = SS_MAX_MESSAGE_SIZE - 1;
	char * msg_begin = recv_buf;
	char * msg_end = NULL;
	multi_msg_t * m = NULL;
	multi_msg_t * m_prev = NULL;

	while(1)
	{
		bytes = timeout_recv(socket, recv_buf + offset, len, to_msecs);
		if(bytes <= 0)
			return bytes;

		offset += bytes;
		len -= bytes;

		while(1)
		{
			msg_end = strstr(msg_begin, SS_END_TAG);

			if(msg_end == NULL)
				break;
			else
			{
				m = (multi_msg_t *)malloc(sizeof(multi_msg_t));

				if(!m)
				{
					SS_DEBUG_PRINT("ERROR: malloc()\n");
					return -1;
				}
				m->size = msg_end + strlen(SS_END_TAG) - msg_begin;
				m->next = NULL;

				if(*mfirst == NULL)
					*mfirst = m;
				else
				{
					for(m_prev = *mfirst; m_prev; m_prev = m_prev->next)
						if(m_prev->next == NULL) break;

					m_prev->next = m;
				}

				if(offset == (msg_end + strlen(SS_END_TAG) - recv_buf)) /* whole message received */
					return bytes;

				/* move to new message */
				msg_begin = msg_end + strlen(SS_END_TAG);

			} 
		}

		if(len <= 1)
			return -1;

	}

	return bytes;
}

/**
* \fn ss_close()
*
* \brief Closes the socket.
*
* \param[in] int socket. File descriptor of the socket to be closed
*
* \return int. 0 if successful, otherwise -1.
*/
int ss_close(int socket)
{
#if defined(WIN32) || defined (WINCE)
	return closesocket(socket);
#else
	return close(socket);
#endif
}

/*
*****************************************************************************
*  LOCAL FUNCTION IMPLEMENTATIONS
*****************************************************************************
*/

/**
* \fn timeout_recv()
*
* \brief recv() with timeout implemented using select().
*
* \param[in] int socket. File descriptor of the socket to receive from.
* \param[in/out] char * recv_buf. Received data is copied to this buffer.
* \param[in] int len. Length of the recv_buf.
* \param[in] int to_msecs. Timeout value in milliseconds.
*/
static int timeout_recv(int socket, char *recv_buf, int len, int to_msecs)
{
	int recv_bytes = 0;
	int err = 0;
	struct timeval tv;
	fd_set readfds;

	tv.tv_sec = (int)(to_msecs / 1000);
	tv.tv_usec = (int)(((to_msecs / 1000) - tv.tv_sec) * 1000000);
	FD_ZERO(&readfds);
	FD_SET(socket, &readfds);

	err = select(socket + 1, &readfds, NULL, NULL, &tv);

	if(err < 0)
	{
		SS_DEBUG_PRINT("ERROR: select()\n");
		return -1;
	}
	if(err == 0) 
		return 0;
	if(FD_ISSET(socket, &readfds))
	{
		recv_bytes = recv(socket, recv_buf, len-1, 0);

		if(recv_bytes < 0)
			return -1;

		recv_buf[recv_bytes] = 0;
	}

	return recv_bytes;
}


/** 
 * \fn int send_receive_discovery()
 *
 * \brief Creates a broadcast socket, sends discovery message and handles responses
 * Received messages are added to the buffer-list (messages) starting from given index (messages_count).
 *
 * \param[in] const char *ip_address. String with broadcast IP address.
 * \param[in] int port. Port to send broadcast.
 * \param[in|out] char ***messages. Buffer-list to store responses messages from SIBs. 
 * If it equals NULL, then it will be created. Free buffer-list after using (free each message and list itself).
 * \param[in|out] int *messages_count. Count af received messages in the messages buffer. 
 * Starting from this index new messages will be inserted to buffer-list. It will be updated after 
 * inserting.
 *
 * \return int. If successful returns 0, otherwise -1.
 */
static int send_receive_discovery(const char *ip_address, int port, char ***messages, int* messages_count)
{
	struct sockaddr_in address;
	memset(&address, 0, sizeof address);

	int socketfd = open_broadcast(ip_address, port, &address);

	if (socketfd < 0) {
		SS_DEBUG_PRINT("ERROR: failed open socket for '%s'.\n", ip_address);
		return -1;
	}

	// Send the broadcast request.
	int result = sendto(socketfd, 
		SS_SIB_DISCOVERY_MESSAGE, strlen(SS_SIB_DISCOVERY_MESSAGE), 
		0, 
		(struct sockaddr *) &address, sizeof address);

	if (result < 0) {
		SS_DEBUG_PRINT("ERROR: Could not send broadcast for '%s'.\n", ip_address);

		ss_close(socketfd);
		return -1;
	}
    
    // Buffer for message and for all messages. 
    // Buffer for all messages is reallocates if it is needed.
    char buffer[SS_MAX_DISCOVERY_RESPONSE_LEN + 1];
    char **all_messages = *messages;
    
    // Increment count of messages on reallocation.
    const int messages_increment = 5; 
    int messages_max_count = *messages_count;
    int messages_index = *messages_count;
          
	// Get responses here using recvfrom.
	while (1) {
        buffer[0] = '\0';
        result = recvfrom(socketfd, buffer, SS_MAX_DISCOVERY_RESPONSE_LEN, 0, NULL, NULL);

        printf("%s\n", buffer);
        if (result <= 0){
            SS_DEBUG_PRINT("No more messages (%i received) or error for '%s'.\n", (messages_index - *messages_count), ip_address);
            break;        
        }
        
        // Reallocate buffer to store more messages.
        if (messages_max_count <= messages_index) {
            messages_max_count += messages_increment;
            all_messages = (char **) realloc (all_messages, sizeof(char *) * messages_max_count);
        }
        
        all_messages[messages_index] = (char *) calloc(sizeof(char), result + 1);
        memcpy(all_messages[messages_index], buffer, result);
        ++messages_index;
        all_messages[messages_index] = NULL;
    }
        
	ss_close(socketfd);
    
    *messages = all_messages;
    *messages_count = messages_index; 

	return 0;
}


/**
* \fn int open_broadcast()
*
* \brief Creates a broadcast socket and and prepare it to work.
*
* \param[in] const char* broadcast_address. Broadcast ip address for socket.
* \param[in] int port. Port for socket.
* \param[inout] struct sockaddr_in *address. Structure for prepared address of socket.
*
* \return int. If successful returns the socket descriptor, otherwise -1.
*/
static int open_broadcast(const char *broadcast_address, int port, struct sockaddr_in *address)
{
	int socketfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (socketfd <= 0) {
		SS_DEBUG_PRINT("ERROR: Could not open UDP socket.\n");
		return -1;
	}

	// Set socket options: enable broadcast.
	int broadcast_enable = 1;


#if defined(WIN32) || defined (WINCE)
	int ret = setsockopt(socketfd, SOL_SOCKET, SO_BROADCAST, 
							(const char *) &broadcast_enable, sizeof(broadcast_enable));

	DWORD timeout = 5000;
	if (setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, (const char *) &timeout, sizeof(timeout)) < 0) 
	{
		ret = -1;
	}
#else
	int ret = setsockopt(socketfd, SOL_SOCKET, SO_BROADCAST, 
							&broadcast_enable, sizeof(broadcast_enable));

	// For timeout.
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 5000;

	if (setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) 
	{
		ret = -1;
	}
#endif

	if (ret != 0) {
		SS_DEBUG_PRINT("ERROR: Could not open socket in broadcast mode.\n");
		ss_close(socketfd);

		return -1;
	}

	// Configure the port and ip we want to send to.
	address->sin_family = AF_INET;

#if defined(WIN32) || defined (WINCE)
	InetPtonA(AF_INET, (PCSTR) broadcast_address, &address->sin_addr); // Set the broadcast IP. address
#else
	inet_pton(AF_INET, broadcast_address, &address->sin_addr); // Set the broadcast IP address.
#endif
	address->sin_port = htons(port); 

	int len_inet = sizeof (struct sockaddr_in);

	bind(socketfd, (struct sockaddr *) address, len_inet);

	return socketfd;
}