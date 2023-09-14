/* Copyright 2017 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

/**
 * \file ipv6.h
 *
 * The IPv6 library provides a familiar socket API for sending and receiving
 * data. It is an alternative to the Data library. The IPv6 architecture is
 * explained in Document WP-RM-118 – Wirepas Mesh IPv6 Architecture Description.
 *
 * The IPv6 library is optional and not available on all platforms.
 */
#ifndef APP_LIB_IPV6_H_
#define APP_LIB_IPV6_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>      // For ssize_t

#include "app/app.h"        // For __STATIC_INLINE

#ifdef IPV6
#include "sys_socket.h"     // For struct sockaddr, socklen_t
#include "socket_errno.h"   // For errno
#else   /* IPV6 */

/**
 * A forward declaration of socket address type for \ref app_lib_ipv6.bind(),
 * \ref app_lib_ipv6.sendto() and \ref app_lib_ipv6.recvfrom(). Actual socket
 * address struct definitions come from outside of the IPv6 library header.
 */
struct sockaddr;
/**
 * Type for the length of the socket address struct for \ref app_lib_ipv6.bind()
 * , \ref app_lib_ipv6.sendto() and \ref app_lib_ipv6.recvfrom().
 */
typedef uint32_t socklen_t;
#endif  /* IPV6 */

/**
 * This is the library name for use with \ref
 * app_global_functions_t.openLibrary */
#define APP_LIB_IPV6_NAME 0x00093194 //!< "IPV6"

/**
 * This is the library version for use with \ref
 * app_global_functions_t.openLibrary "*/
#define APP_LIB_IPV6_VERSION 0x200

/**
 * Return the error value errno of last failed socket operation.
 * \return  Error number
 */
typedef int
    (*app_lib_ipv6_get_errno_f)(void);

/**
 * Create a socket. Supported socket types and protocols are platform specific.
 * Returns -1 and sets errno if socket could not be created. Otherwise, returns
 * a file handle.
 *
 * \param   domain
 *          socket domain
 * \param   type
 *          socket type
 * \param   protocol
 *          socket protocol
 * \return  id of the socket or -1 on error
 */
typedef int
    (*app_lib_ipv6_socket_f)(int domain, int type, int protocol);

/**
 * Close a socket.
 *
 * Returns -1 and sets errno if a file handle \p fd was not open. Otherwise,
 * returns 0.
 *
 * \param   fd
 *          socket id
 * \return  0 if successful, or -1 on error
 */
typedef int
    (*app_lib_ipv6_close_f)(int fd);

/**
 * Binds a socket to a given address. Supported address types are platform
 * specific.
 *
 * Returns -1 and sets errno if socket could not be bound to a given address.
 * Otherwise, returns 0.
 *
 * \param   fd
 *          socket id
 * \param   addr
 *          pointer to the location of the address to bind the socket with
 * \param   addr_len
 *          length of the buffer pointed by addr
 * \return  0 if successful, or -1 on error
 */
typedef int
    (*app_lib_ipv6_bind_f)(int fd,
                           const struct sockaddr * addr,
                           socklen_t addr_len);

/**
 * Send data to a peer. Supported address types are platform specific.
 *
 * Returns -1 and sets errno if no data could be sent. Otherwise, returns the
 * number of bytes sent.
 *
 * \param   fd
 *          socket id
 * \param   buf
 *          address of the buffer to send
 * \param   len
 *          length of buffer pointed by buf
 * \param   flags
 *          sending flags
 * \param   src_addr
 *          address where the destination address is stored
 * \param   addr_len
 *          length of the buffer pointed by src_addr
 * \return  number of data bytes sent or -1 on error
 */
typedef ssize_t
    (*app_lib_ipv6_sendto_f)(int fd,
                             const void * buf,
                             size_t len,
                             int flags,
                             const struct sockaddr * dest_addr,
                             socklen_t addr_len);

/**
 * Receive data from a peer. Supported address types are platform specific.
 *
 * Returns -1 and sets errno if no data could be received. Otherwise,
 * returns the number of bytes received.
 *
 * \param   fd
 *          socket id
 * \param   buf
 *          address of the buffer where data is written
 * \param   len
 *          length of buffer pointed by buf
 * \param   flags
 *          receiving flags
 * \param   src_addr
 *          address where to store the sender info
 * \param   addr_len
 *          length of the buffer pointed by src_addr
 * \return  number of data bytes received or -1 on error
 */
typedef ssize_t
    (*app_lib_ipv6_recvfrom_f)(int fd,
                               void * buf,
                               size_t len,
                               int flags,
                               struct sockaddr * src_addr,
                               socklen_t *addr_len);

/**
 * The function table returned from \ref app_open_library_f
 */
typedef struct
{
    app_lib_ipv6_get_errno_f getErrno;
    app_lib_ipv6_socket_f socket;
    app_lib_ipv6_close_f close;
    app_lib_ipv6_bind_f bind;
    app_lib_ipv6_sendto_f sendto;
    app_lib_ipv6_recvfrom_f recvfrom;
} app_lib_ipv6_t;


/*
 * Things that follow are only useful in the public use of this API.
 * Compilation will fail if these are included when compiling the
 * stack firmware.
 */
#ifndef APP_LIB_IPV6_OMIT_PUBLIC_API_

/**
 * Supported socket API functions
 *
 * Usage in app:
 *
 *  #include "ipv6.h"
 *
 *  static const app_lib_ipv6_t * app_lib_ipv6 = NULL;
 *
 *  // Set up support for traditional socket function names (optional)
 *  APP_LIB_IPV6_SET_LIBRARY(&app_lib_ipv6);
 *
 *  void App_init(const app_global_functions_t * functions)
 *  {
 *      app_lib_ipv6 = functions->openLibrary(APP_LIB_IPV6_NAME,
 *                                            APP_API_VERSION);
 *      if (app_lib_ipv6 == NULL)
 *      {
 *          // Could not open the IPv6 library
 *          return;
 *      }
 *
 *
 *      //
 *      // ---------------- Style A ----------------
 *      //
 *
 *      // Using APP_LIB_IPV6_SET_LIBRARY() and
 *      // traditional socket function names
 *      int sock = socket(AF_INET6, SOCK_DGRAM, 0);
 *
 *      // ...
 *
 *      // errno is a global variable
 *      if (errno == EINVAL)
 *      {
 *          // ...
 *      }
 *
 *
 *      //
 *      // ---------------- Style B ----------------
 *      //
 *
 *      // Not using APP_LIB_IPV6_SET_LIBRARY()
 *      int sock = app_lib_ipv6->socket(AF_INET6, SOCK_DGRAM, 0);
 *
 *      // ...
 *
 *      // errno is accessed through a function
 *      if (app_lib_ipv6->getErrno() == EINVAL)
 *      {
 *          // ...
 *      }
 *
 *
 *      // ...
 *  }
 **/

/**
 * \brief   Pointer to the IPv6 library, for helper macros
 */
extern const app_lib_ipv6_t * const * _app_lib_ipv6_proxy_;

/**
 * \brief   A macro to set the opened IPv6 library, for \ref APP_LIB_IPV6_CALL()
 * \param   lib_p_p
 *          A pointer to a pointer to an opened IPv6 library
 */
#define APP_LIB_IPV6_SET_LIBRARY(lib_p_p) \
    const app_lib_ipv6_t * const * _app_lib_ipv6_proxy_ = lib_p_p

/**
 * \brief   A macro call IPv6 functions with cleaner syntax
 * \param   rtype
 *          Return type of function
 * \param   f
 *          Function to call: socket, close, bind, sendto, recvfrom
 */
#define APP_LIB_IPV6_CALL(rtype, f, ...) \
    ({ rtype e = (*_app_lib_ipv6_proxy_)->f(__VA_ARGS__); \
       errno = (*_app_lib_ipv6_proxy_)->getErrno(); \
       e;})

__STATIC_INLINE int bind(int fd,
                         const struct sockaddr * addr,
                         socklen_t addr_len)
{
    return APP_LIB_IPV6_CALL(int, bind, fd, addr, addr_len);
}

__STATIC_INLINE ssize_t recvfrom(int fd,
                                 void *buf,
                                 size_t len,
                                 int flags,
                                 struct sockaddr * src_addr,
                                 socklen_t *addr_len)
{
    return APP_LIB_IPV6_CALL(ssize_t,
                             recvfrom,
                             fd,
                             buf,
                             len,
                             flags,
                             src_addr,
                             addr_len);
}

__STATIC_INLINE ssize_t sendto(int fd,
                               const void * buf,
                               size_t len,
                               int flags,
                               const struct sockaddr * dest_addr,
                               socklen_t addr_len)
{
    return APP_LIB_IPV6_CALL(ssize_t,
                             sendto,
                             fd,
                             buf,
                             len,
                             flags,
                             dest_addr,
                             addr_len);
}

__STATIC_INLINE int socket(int domain, int type, int protocol)
{
    return APP_LIB_IPV6_CALL(int, socket, domain, type, protocol);
}

__STATIC_INLINE int close(int fd)
{
    return APP_LIB_IPV6_CALL(int, close, fd);
}

#endif /* APP_LIB_IPV6_OMIT_PUBLIC_API_ */

#endif /* APP_LIB_IPV6_H_ */
