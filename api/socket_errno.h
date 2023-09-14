/* Copyright 2017 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

#ifndef SOCKET_ERRNO_H
#define SOCKET_ERRNO_H

#include <errno.h>

enum socket_errno {

    S_EPERM     = EPERM,
    S_ENOENT    = ENOENT,
    S_EIO       = EIO,
    S_ENXIO     = ENXIO,
    S_EADDRINUSE = EADDRINUSE,
    S_EBADF     = EBADF,
    S_EAGAIN    = EAGAIN,
    S_ENOMEM    = ENOMEM,
    S_EACCES    = EACCES,
    S_EFAULT    = EFAULT,
    S_EBUSY     = EBUSY,
    S_ENODEV    = ENODEV,
    S_EINVAL    = EINVAL,
    S_ENFILE    = ENFILE,
    S_EMFILE    = EMFILE,
    S_ENOSPC    = ENOSPC,
    S_EPROTO    = EPROTO,
    S_ENOPROTOOPT = ENOPROTOOPT,
    S_EPROTONOSUPPORT = EPROTONOSUPPORT,
    S_EAFNOSUPPORT = EAFNOSUPPORT,
    S_EADDRNOTAVAIL = EADDRNOTAVAIL,
    S_EDESTADDRREQ = EDESTADDRREQ,

    S_FORCE_U32  =      0xffffffff,
};

#endif
