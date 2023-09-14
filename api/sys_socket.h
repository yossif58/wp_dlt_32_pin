/* Copyright 2017 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

#ifndef __SYS_SOCKET_H
#define __SYS_SOCKET_H

#include <stdint.h>

typedef uint32_t socklen_t;

typedef uint16_t sa_family_t;

/* Supported socket families */
#define AF_INET6        0

/* Supported socket/protocol families */
#define PF_INET6        AF_INET6

#define _SS_MAXSIZE 64
    /* Implementation-defined maximum size. */
#define _SS_ALIGNSIZE (sizeof(int32_t))
    /* Implementation-defined desired alignment. */

/*
 *  Definitions used for sockaddr_storage structure paddings design.
 */
#define _SS_PAD1SIZE (_SS_ALIGNSIZE - sizeof(sa_family_t))
#define _SS_PAD2SIZE ((_SS_MAXSIZE - (sizeof(sa_family_t) +              \
                                      _SS_PAD1SIZE + _SS_ALIGNSIZE)))

struct sockaddr
/* abstract type, base for all sock addresses, not to be used directly */
{
    sa_family_t sa_family;
    char _sa_pad[_SS_PAD1SIZE];
    char sa_data[];
};

struct sockaddr_storage
{
    sa_family_t  ss_family;
    char _ss_pad1[_SS_PAD1SIZE];
    int32_t _ss_align;
    char _ss_pad2[_SS_PAD2SIZE];
};

/*
The <sys/socket.h> header shall define the msghdr structure that includes at least the following members:

void          *msg_name        Optional address.
socklen_t      msg_namelen     Size of address.
struct iovec  *msg_iov         Scatter/gather array.
int            msg_iovlen      Members in msg_iov.
void          *msg_control     Ancillary data; see below.
socklen_t      msg_controllen  Ancillary data buffer len.
int            msg_flags       Flags on received message.
*/

/*
The <sys/socket.h> header shall define the cmsghdr structure that includes at least the following members:

socklen_t  cmsg_len    Data byte count, including the cmsghdr.
int        cmsg_level  Originating protocol.
int        cmsg_type   Protocol-specific type.
*/

/*
SCM_RIGHTS
Indicates that the data array contains the access rights to be sent or received.
*/

/*
CMSG_DATA(cmsg)

If the argument is a pointer to a cmsghdr structure, this macro shall return an unsigned character pointer to the data array associated with the cmsghdr structure.
CMSG_NXTHDR(mhdr,cmsg)

If the first argument is a pointer to a msghdr structure and the second argument is a pointer to a cmsghdr structure in the ancillary data pointed to by the msg_control field of that msghdr structure, this macro shall return a pointer to the next cmsghdr structure, or a null pointer if this structure is the last cmsghdr in the ancillary data.
CMSG_FIRSTHDR(mhdr)

If the argument is a pointer to a msghdr structure, this macro shall return a pointer to the first cmsghdr structure in the ancillary data associated with this msghdr structure, or a null pointer if there is no ancillary data associated with the msghdr structure.
*/

/*
The <sys/socket.h> header shall define the linger structure that includes at least the following members:

int  l_onoff   Indicates whether linger option is enabled.
int  l_linger  Linger time, in seconds.
*/


#define SOCK_DGRAM 0
/* Datagram socket. */

/*
  SOCK_RAW
  Raw Protocol Interface.
  SOCK_SEQPACKET
  Sequenced-packet socket.
  SOCK_STREAM
  Byte-stream socket.
*/

#define SOL_SOCKET 0
/* ptions to be accessed at socket level, not protocol level. */

#define SO_ACCEPTCONN -1
/* Socket is accepting connections. */

#define SO_BROADCAST -1
/* Transmission of broadcast messages is supported. */

#define SO_DEBUG -1
/*Debugging information is being recorded. */

#define SO_DONTROUTE -1
/* Bypass normal routing. */

#define SO_ERROR 4
/* Socket error status. */

#define SO_KEEPALIVE -1
/* Connections are kept alive with periodic messages. */

#define SO_LINGER -1
/* Socket lingers on close. */

#define SO_OOBINLINE -1
/* Out-of-band data is transmitted in line. */

#define SO_RCVBUF -1
/* Receive buffer size. */

#define SO_RCVLOWAT -1
/* Receive ``low water mark''. */

#define SO_RCVTIMEO -1
/* Receive timeout. */

#define SO_REUSEADDR 11
/* Reuse of local addresses is supported. */

#define SO_SNDBUF -1
/* Send buffer size. */

#define SO_SNDLOWAT -1
/* Send ``low water mark''. */

#define SO_SNDTIMEO -1
/* Send timeout. */

#define SO_TYPE1 -1
/* Socket type. */

#define SOMAXCONN -1
/*   The maximum backlog queue length. ?? */

#define   MSG_CTRUNC -1
/*   Control data truncated. */

#define   MSG_DONTROUTE -1
/*   Send without using routing tables. */

#define   MSG_EOR -1
/*   Terminates a record (if supported by the protocol). */

#define   MSG_OOB -1
/*   Out-of-band data. */

#define   MSG_PEEK -1
/*   Leave received data in queue. */

#define   MSG_TRUNC -1
/*   Normal data truncated. */

#define   MSG_WAITALL -1
/*   Attempt to fill the read buffer. */


#define   AF_INET -1
/*   Internet domain sockets for use with IPv4 addresses. */

/*
  AF_INET6
  [IP6] [Option Start] Internet domain sockets for use with IPv6 addresses. [Option End]
*/

#define   AF_UNIX -1
/*   UNIX domain sockets. */

#define   AF_UNSPEC -1
/*   Unspecified. */


#define SHUT_RD -1
/* Disables further receive operations.*/

#define SHUT_RDWR -1
/* Disables further send and receive operations. */

#define SHUT_WR -1
/* Disables further send operations. */

enum socket_event
{
    SE_DATA_RECV,
    SE_DATA_SENT,
    SE_IF_UP,
    SE_IF_DOWN,
    SE_ERROR,
};

struct socket_callback_data
{
    enum socket_event code;
    uint8_t code_arg1;
    uint8_t socket;
    uint8_t interface;
    void *code_arg2;
    uint32_t code_arg2_len;
};

typedef int (* const socket_cb_t)(struct socket_callback_data *);


#define SOCKETS_NMAX 10

#ifndef ENOSYS
#define ENOSYS 88
#endif

/* Not implemented (stream socket oriented) */
/* int accept(int, struct sockaddr *restrict, socklen_t *restrict); */
#define accept(a,b,c)  (({errno=ENOSYS; -1;}))
/* int listen(int, int); */
#define listen(a,b)    (({errno=ENOSYS; -1;}))
/* int sockatmark(int); */
#define sockatmark(a)  (({errno=ENOSYS; -1;}))
/* int shutdown(int, int); */
#define shutdown(a,b)  (({errno=ENOSYS; -1;}))

/* Should be implemented */
/* int connect(int, const struct sockaddr *, socklen_t); */
#define connect(a,b,c) (({errno=ENOSYS; -1;}))
/* ssize_t recv(int, void *, size_t, int); */
#define recv(a,b,c,d)  (({errno=ENOSYS; -1;}))
/* ssize_t send(int, const void *, size_t, int); */
#define send(a,b,c,d)  (({errno=ENOSYS; -1;}))
/* ssize_t recvmsg(int, struct msghdr *, int); */
#define recvmsg(a,b,c) (({errno=ENOSYS; -1;}))
/* ssize_t sendmsg(int, const struct msghdr *, int); */
#define sendmsg(a,b,c) (({errno=ENOSYS; -1;}))
/* int socketpair(int, int, int, int[2]); */
#define socketpair(a,b,c,d)  (({errno=ENOSYS; -1;}))
/* int setsockopt(int, int, int, const void *, socklen_t); */
#define setsockopt(a,b,c,d,e) (({errno=ENOSYS; -1;}))
/* int getsockopt(int, int, int, void *restrict, socklen_t *restrict); */
#define setsockopt(a,b,c,d,e) (({errno=ENOSYS; -1;}))
/* int getpeername(int, struct sockaddr *restrict, socklen_t *restrict); */
#define getpeername(a,b,c) (({errno=ENOSYS; -1;}))
/* int getsockname(int, struct sockaddr *restrict, socklen_t *restrict); */
#define getsockname(a,b,c) (({errno=ENOSYS; -1;}))


#endif
