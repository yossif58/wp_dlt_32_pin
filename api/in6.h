/* Copyright 2017 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

#ifndef IN6_H
#define IN6_H

#include <stdint.h>

/* supported IPv6 protocols e.g IPv6 Header NH values */
enum ipv6_proto_numbers
{
    IP_PROTO_UNDEFINED = 0,
    IP_PROTO_IP = 59,
    IP_PROTO_ICMP = 58,
    IP_PROTO_UDP = 17,
    IP_PROTO_RESERVED = 0xff,
};

struct in6_addr
{
    union {
        uint8_t   addr8[16];
        uint16_t  addr16_be[8];
    };
};

#define IN6_ADDR_SIZE (sizeof(struct in6_addr))

struct sockaddr_in6
{
    uint16_t         sin6_family;    /* AF_INET6 */
    uint16_t         sin6_port;      /* Transport layer port # */
    uint32_t         sin6_flowinfo;  /* IPv6 flow information */
    struct in6_addr  sin6_addr;      /* IPv6 address */
    uint32_t         sin6_scope_id;  /* scope id */
};


extern const struct in6_addr in6addr_any;
#define IN6ADDR_ANY_INIT                                    \
    { { .addr8 = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } } }
extern const struct in6_addr in6addr_loopback;
#define IN6ADDR_LOOPBACK_INIT                               \
    { { .addr8 = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 } } }
extern const struct in6_addr in6addr_linklocal_allnodes;
#define IN6ADDR_LINKLOCAL_ALLNODES_INIT                     \
    { { .addr8 = { 0xff,2,0,0,0,0,0,0,0,0,0,0,0,0,0,1 } } }
extern const struct in6_addr in6addr_linklocal_allrouters;
#define IN6ADDR_LINKLOCAL_ALLROUTERS_INIT                   \
    { { .addr8 = { 0xff,2,0,0,0,0,0,0,0,0,0,0,0,0,0,2 } } }
extern const struct in6_addr in6addr_interfacelocal_allnodes;
#define IN6ADDR_INTERFACELOCAL_ALLNODES_INIT                \
    { { .addr8 = { 0xff,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 } } }
extern const struct in6_addr in6addr_interfacelocal_allrouters;
#define IN6ADDR_INTERFACELOCAL_ALLROUTERS_INIT              \
    { { .addr8 = { 0xff,1,0,0,0,0,0,0,0,0,0,0,0,0,0,2 } } }
extern const struct in6_addr in6addr_sitelocal_allrouters;
#define IN6ADDR_SITELOCAL_ALLROUTERS_INIT                   \
    { { .addr8 = { 0xff,5,0,0,0,0,0,0,0,0,0,0,0,0,0,2 } } }


struct sockaddr;
/* in6 address match function */
int sockaddr_in6_match(struct sockaddr const *_addr,
                       struct sockaddr const *_local);

#endif

