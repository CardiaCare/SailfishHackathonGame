/*
 * Copyright (C) 2009 Nokia Corporation
 * All rights reserved
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * Author(s): Mikko Hurskainen, Dirk-Jan C. Binnema
 *
 * $Id$
 */
 
#ifndef __H_BSDAPI_H__
#define __H_BSDAPI_H__

#ifndef NOTA_SYSTEM_NO_BSD_SOCKETS
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#if !defined (WINCE)
#include <errno.h>
#endif
#else /*NOTA_SYSTEM_NO_BSD_SOCKETS*/
typedef int socklen_t;
struct sockaddr;
#endif /*!NOTA_SYSTEM_NO_BSD_SOCKETS*/

struct h_in;
typedef struct h_in h_in_t;

#ifdef __cplusplus
extern "C" {
#endif
  /* get the instance to the API */
  h_in_t* Hgetinstance(void);

  /* BSD socket API prefixed with H and with instance */
  int Hsocket(h_in_t* core, int domain, int type, int protocol);
  int Hbind(h_in_t* core, int sockfd, struct sockaddr* my_addr, 
            socklen_t addrlen);
  int Hclose(h_in_t* core, int sockid);
  int Hconnect(h_in_t* core, int sockid, struct sockaddr* addr,
               socklen_t addrlen);
  int Hlisten(h_in_t* core, int sockid, int backlog);
  int Haccept(h_in_t* core, int sockid, struct sockaddr* addr,
	      socklen_t* addrlen);
  int Hsend(h_in_t* core, int sockid, const void* buf, int len, int flags);
  int Hrecv(h_in_t* core, int sockid, void* buf, int len, int flags);
  int Hselect(h_in_t* core, int nfd, fd_set* read_fds, fd_set* write_fds, 
	      fd_set* error_fds, struct timeval* tv);
  int Hgetsockopt(h_in_t* core, int sockid, int level, int optname, 
		  void* optval, socklen_t* optlen);
  int Hsetsockopt(h_in_t* core, int sockid, int level, int optname, 
		  void* optval, socklen_t optlen);
#ifdef __cplusplus
};
#endif


#define AF_NOTA 34
#define PF_NOTA 34

typedef unsigned int sid_t;
typedef unsigned int vdid_t;

struct nota_addr {
  sid_t        sid;     /* service identifier */
  unsigned int port;      /* port of the sid */
  vdid_t       device; /* virtual device id */
};
typedef struct nota_addr nota_addr_t;


/* some definitions that may not exist on all systems;
 * for the numerical values, we take them from Linux
 */  
#ifndef MSG_WAITALL
#define MSG_WAITALL 0x100
#endif /*MSG_WAITALL*/

#ifndef MSG_DONTWAIT
#define MSG_DONTWAIT 0x40
#endif /*MSG_DONTWAIT*/


/* eg., OpenBSD does not define MSG_NOSIGNAL (and, presumably,
 * does not support it). Thus, we set it to zero there */
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif /*MSG_NOSIGNAL*/


#ifndef SOCK_STREAM
#define SOCK_STREAM     1
#endif /*SOCK_STREAM*/

#ifndef SOCK_SEQPACKET
#define SOCK_SEQPACKET  2
#endif /*SOCK_SEQPACKET*/

#ifndef EINVAL
#define EINVAL          22
#endif /*EINVAL*/

#ifndef EBUSY
#define EBUSY           16
#endif /*EBUSY*/

#ifndef EAFNOSUPPORT
#define EAFNOSUPPORT    3
#endif /*EAFNOSUPPORT*/

#ifndef EPROTONOSUPPORT
#define EPROTONOSUPPORT 93
#endif /*EPROTONOSUPPORT*/

#ifndef EBADF 
#define EBADF           9
#endif /*EBADF*/

#ifndef EFAULT
#define EFAULT          14
#endif /*EFAULT*/

#ifndef ENONET
#define ENONET 64
#endif

#ifndef SO_PASSCRED
#define SO_PASSCRED 16
#endif /*SO_PASSCRED*/


#endif /*__H_BSDAPI_H__*/
