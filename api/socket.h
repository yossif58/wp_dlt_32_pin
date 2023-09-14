/* Copyright 2017 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

#ifndef SOCKET_H
#define SOCKET_H

#include <sys/types.h>
#include "sys_socket.h"

#ifdef __WCF_INTERNAL_SOCKETS

/* Implemented socket API functions.
 * Direct access to these functions is possible only
 *  from stack internal code when compiled with IPv6 support
 */
int     bind(int fd, const struct sockaddr *addr, socklen_t addr_len);
ssize_t recvfrom(int fd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen);
ssize_t sendto(int fd, const void *buf, size_t len,
               int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
int     socket(int domain, int type, int protocol);
int     close(int fd);

#endif

#endif
