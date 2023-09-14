/* Copyright 2019 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

/**
 * \file settings.h
 *
 * The Settings library provides access to node settings, which are stored in
 * nonvolatile memory. When a node starts up it automatically uses these stored
 * settings.
 *
 * Settings such as node role, unique node address, network address and channel,
 * encryption and authentication keys as well performance-related settings such
 * as access cycle limits can be stored and recalled. Also see the State library
 * \ref state.h for starting and stopping the stack.
 */
#ifndef APP_LIB_SETTINGS_H_
#define APP_LIB_SETTINGS_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "app/app.h"

/**
 * This is the library name for use with \ref
 * app_global_functions_t.openLibrary */

#define APP_LIB_SETTINGS_NAME 0x74ced676 //!< "SETTIN"

/**
 * This is the library version for use with \ref
 * app_global_functions_t.openLibrary "*/
#define APP_LIB_SETTINGS_VERSION 0x203

/*
 * This macro can be used as a buffer size for storing or copying a 128-bit AES
 * key, or the 16-byte feature lock key. The feature lock key is not an AES-128
 * key, but it is guaranteed to be the same size as an AES-128 key.
 */
#define APP_LIB_SETTINGS_AES_KEY_NUM_BYTES  16

// Device address definition is in app/app.h */

/**
 * Network address type definition. All nodes on the network must have the same
 * network address.
 */
typedef uint32_t app_lib_settings_net_addr_t;

/**
 * Network channel type definition. All nodes on the network must have the same
 * network channel.
 */
typedef uint8_t app_lib_settings_net_channel_t;

/**
 * Node's base role is used by the \ref
 * app_lib_settings_create_role() utility function to create
 * a role value.
 */
typedef enum
{
    // Node is a sink.
    APP_LIB_SETTINGS_ROLE_SINK = 0,
    // Node is a headnode, i.e. it routes data for other nodes.
    APP_LIB_SETTINGS_ROLE_HEADNODE,
    // Node is a subnode, i.e. it does not route data for other nodes
    APP_LIB_SETTINGS_ROLE_SUBNODE,
} app_lib_settings_base_role_e;

/**
 * Additional role flag bits. Role flag bits are used by the \ref
 * app_lib_settings_create_role() utility function to create
 * a role value.
 */
typedef enum
{
    // Enable CB-MAC (low-latency) operation for this node
    APP_LIB_SETTINGS_ROLE_FLAG_LL = 0x10,
    // Reserved bitmask, do not set
    APP_LIB_SETTINGS_ROLE_FLAG_RESV = 0x20,
    // Automatically switch between headnode and subnode roles, as needed.
    APP_LIB_SETTINGS_ROLE_FLAG_AUTOROLE = 0x40,
} app_lib_settings_flag_role_e;

/**
 * \brief   Flags to enable and disable periodic scan
 */
typedef enum
{
    APP_LIB_SETTINGS_PERIODIC_SCAN_DISABLE = 0,
    APP_LIB_SETTINGS_PERIODIC_SCAN_ENABLED = 1
} app_lib_settings_periodic_scan_control_e;

/**
 * Bottom four bits of the role value are used as the base role, i.e. whether
 * the node is a sink, headnode or subnode. This macro is used by the \ref
 * app_lib_settings_create_role() utility function to create
 * a role value.
 */
#define APP_LIB_SETTINGS_BASE_ROLE_MASK 0x0f

/**
 * Top four bits of the role value are used as additional role flags. This macro
 * is used by the \ref app_lib_settings_create_role() utility
 * function to create a role value.
 */
#define APP_LIB_SETTINGS_FLAG_ROLE_MASK 0xf0

/**
 * The node role value which is a combination of a base role, \ref
 * app_lib_settings_base_role_e and role flag bits, \ref
 * app_lib_settings_flag_role_e. Utility function \ref
 * app_lib_settings_create_role() can be used to create a
 * role value.
 */
typedef uint8_t app_lib_settings_role_t;

/**
 * A utility function to create a role value from base role, \ref
 * app_lib_settings_base_role_e and role flag bits, \ref
 * app_lib_settings_flag_role_e. The role value can be used to set a node role
 * using \ref app_lib_settings_set_node_role_f "lib_settings->setNodeRole"().
 *
 * \param   base
 *          Base role
 * \param   flags
 *          Flags for the role
 * \return  The full role
 */
__STATIC_INLINE app_lib_settings_role_t
    app_lib_settings_create_role(app_lib_settings_base_role_e base,
                                 app_lib_settings_flag_role_e flags)
{
    app_lib_settings_role_t role = (base & APP_LIB_SETTINGS_BASE_ROLE_MASK) |
                                   (flags & APP_LIB_SETTINGS_FLAG_ROLE_MASK);
    return role;
}

/**
 * A utility function for extracting the base role out of a role value.
 *
 * \param   role
 *          Full role
 * \return  Base role
 * \param   flags
 *          Flags for the role
 */
__STATIC_INLINE app_lib_settings_base_role_e
    app_lib_settings_get_base_role(app_lib_settings_role_t role)
{
    return role & APP_LIB_SETTINGS_BASE_ROLE_MASK;
}

/**
 * A utility function for extracting the role flags out of a role value.
 *
 * \param   role
 *          Full role
 * \return  Flags role
 * \param   flags
 *          Flags for the role
 */
__STATIC_INLINE app_lib_settings_role_t
    app_lib_settings_get_flags_role(app_lib_settings_role_t role)
{
    return role & APP_LIB_SETTINGS_FLAG_ROLE_MASK;
}

/**
 * Callback used for determining on which multicast groups the device belongs.
 * As an argument, the stack sets the address of the multicast group. If device
 * belongs to that group, callback function returns true. If not, callback
 * returns false.
 *
 * This callback is called when device receives multicast packet. The return
 * value is then determined whether data shall be received by standard means
 * (i.e. data reception callback, see /ref app_lib_data_set_bcast_data_received_cb_f "lib_data->setBcastDataReceivedCb").
 *
 * \param   group_addr
 *          Group address
 * \return  true: is in multicast group, false: is not in multicast group
 * \note    Keep the function execution time moderately short, i.e. do not
 *          execute any time-consuming operations directly in this callback!
 */
typedef bool
    (*app_lib_settings_is_group_cb_f)(app_addr_t group_addr);

/**
 * Reset all settings to default values.
 * - Feature lock bits: not set
 * - Node address: not set
 * - Network address: not set
 * - Network channel: not set
 * - Node role: headnode, autorole flag set
 * - Authentication key: not set
 * - Encryption key: not set
 * - Access cycle range: Minimum value according to profile. Max value 8000 ms.
 *
 * \return  Result code, \ref APP_RES_OK if successful,
 *          \ref APP_RES_INVALID_STACK_STATE if stack is running
 */
typedef app_res_e
    (*app_lib_settings_reset_all_f)(void);

/**
 * Get feature lock bits. Feature lock bits determine which features are
 * permitted at runtime. A cleared bit marks that a feature is locked.
 * Some features are governed by the stack, some checks are implemented on the
 * application side, where applicable. Feature lock bits are active only when
 * feature lock key is set. Feature lock bits are documented in WP-RM-100
 * Wirepas Mesh Dual-MCU API Reference Manual.
 *
 * \param   bits_p
 *          Pointer to store the result
 * \return  Result code, \ref APP_RES_OK if successful,
 *          APP_RES_INVALID_NULL_POINTER if bits_p is NULL
 */
typedef app_res_e
    (*app_lib_settings_get_feature_lock_bits_f)(uint32_t * bits_p);

/**
 * Set feature lock bits. See \ref app_lib_settings_get_feature_lock_bits_f "lib_settings->getFeatureLockBits"() for a
 * description of feature lock bits. A cleared bit marks a feature locked.
 * Reserved bits must remain set.
 *
 * \param   bits
 *          Feature lock bits
 * \return  Result code, \ref APP_RES_OK if successful,
 *          \ref APP_RES_INVALID_VALUE if an unsupported lock bit is set to 0
 */
typedef app_res_e
    (*app_lib_settings_set_feature_lock_bits_f)(uint32_t bits);

/**
 * Check if feature lock key is set, i.e. if the feature lock is locked. It is
 * not possible to actually read the key from the stack. The \p key_p parameter
 * is ignored.
 *
 * \param   key_p
 *          A dummy parameter, reserved for future, set to NULL
 * \return  Result code, \ref APP_RES_OK if a key set,
 *          APP_RES_INVALID_CONFIGURATION if the key is all 0xff, i.e. not set
 * \note    Reading the actual key value is not possible, for security reasons
 */
typedef app_res_e
    (*app_lib_settings_get_feature_lock_key_f)(uint8_t * key_p);

/**
 * Set feature lock key, i.e. lock or unlock the feature lock. \p key_p must
 * point to \ref APP_LIB_SETTINGS_AES_KEY_NUM_BYTES bytes. The feature lock key
 * is not an AES-128 key, but it is guaranteed to be the same size as an AES-128
 * key.
 *
 * Feature lock key can only be set when the feature lock is unlocked. Unlocking
 * is done by setting the key using the same key as when locking it. A key of
 * all <cde>0xff</code> (hex) bytes is considered an unset key. Setting such a key does not
 * lock the feature lock.
 *
 * \param   key_p
 *          Pointer to key, \ref APP_LIB_SETTINGS_AES_KEY_NUM_BYTES bytes
 * \return  Result code, \ref APP_RES_OK if successful,
 *          \ref APP_RES_INVALID_VALUE if a key is set and trying to unlock with
 *          a wrong key, \ref APP_RES_INVALID_NULL_POINTER if \p key_p is NULL
 */
typedef app_res_e
    (*app_lib_settings_set_feature_lock_key_f)(const uint8_t * key_p);

/**
 * Get node address. See WP-RM-100 - Wirepas Mesh Dual-MCU API Reference Manual
 * for an explanation on node addressing.
 *
 * \param   addr_p
 *          Pointer to store the result
 * \return  Result code, \ref APP_RES_OK if successful,
 *          \ref APP_RES_INVALID_CONFIGURATION if node address not set,
 *          \ref APP_RES_INVALID_NULL_POINTER if \op addr_p is NULL
 */
typedef app_res_e
    (*app_lib_settings_get_node_address_f)(app_addr_t * addr_p);

/**
 * Set node address. See WP-RM-100 - Wirepas Mesh Dual-MCU API Reference Manual
 * for an explanation on node addressing. There is no default node address.
 * Function must be called with a valid node address before the stack can be
 * started.
 *
 * \param   addr
 *          Own node address to set
 * \return  Result code, \ref APP_RES_OK if successful, \ref APP_RES_INVALID_VALUE
 *          if \p addr is invalid, \ref APP_RES_INVALID_STACK_STATE if stack is running
 */
typedef app_res_e
    (*app_lib_settings_set_node_address_f)(app_addr_t addr);

/**
 * Get network address. See WP-RM-100 - Wirepas Mesh Dual-MCU API Reference
 * Manual [1] for an explanation on network addressing.
 *
 * \param   addr_p
 *          Pointer to store the result
 * \return  Result code, \ref APP_RES_OK if successful,
 *          APP_RES_INVALID_CONFIGURATION if network address not set,
 *          APP_RES_INVALID_NULL_POINTER if addr_p is NULL
 */
typedef app_res_e
    (*app_lib_settings_get_network_address_f)(app_lib_settings_net_addr_t * addr_p);

/**
 * Set network address. See WP-RM-100 - Wirepas Mesh Dual-MCU API Reference
 * Manual [1] for an explanation on network addressing. There is no default
 * network address. Function must be called with a valid network address before
 * the stack can be started.
 *
 * \param   addr
 *          Network address to set
 * \return  Result code, \ref APP_RES_OK if successful, APP_RES_INVALID_VALUE
 *          if addr is invalid, APP_RES_INVALID_STACK_STATE if stack is running
 */
typedef app_res_e
    (*app_lib_settings_set_network_address_f)(app_lib_settings_net_addr_t addr);

/**
 * \brief   Get network channel
 * \param   channel_p
 *          Pointer to store the result
 * \return  Result code, \ref APP_RES_OK if successful,
 *          \ref APP_RES_INVALID_CONFIGURATION if network channel not set,
 *          \ref APP_RES_INVALID_NULL_POINTER if \p channel_p is NULL
 */
typedef app_res_e
    (*app_lib_settings_get_network_channel_f)(app_lib_settings_net_channel_t * channel_p);

/**
 * Set network channel. Different radio architectures have different number of
 * channels available. Function \ref
 * app_lib_settings_get_network_channel_limits_f "lib_settings->getNetworkChannelLimits"() can be used to determine the
 * minimum and maximum channel number available. There is no default network
 * channel. Function must be called with a valid network channel before the
 * stack can be started.
 *
 * \param   channel
 *          Network channel to set
 * \return  Result code, \ref APP_RES_OK if successful, \ref APP_RES_INVALID_VALUE
 *          if \p channel is invalid, \ref APP_RES_INVALID_STACK_STATE if stack is
 *          running
 */
typedef app_res_e
    (*app_lib_settings_set_network_channel_f)(app_lib_settings_net_channel_t channel);

/**
 * Get node role. Utility functions \ref
 * app_lib_settings_get_base_role() and \ref
 * app_lib_settings_get_flags_role() can be used to split the
 * node value to a base role and role flag bits, respectively.
 *
 * \param   role_p
 *          Pointer to store the result
 * \return  Result code, \ref APP_RES_OK if successful,
 *          \ref APP_RES_INVALID_CONFIGURATION if node role not set,
 *          \ref APP_RES_INVALID_NULL_POINTER if \p role_p is NULL
 */
typedef app_res_e
    (*app_lib_settings_get_node_role_f)(app_lib_settings_role_t * role_p);

/**
 * Set node role. Utility function \ref
 * app_lib_settings_create_role() can be used to create a
 * node role value from base role, \ref app_lib_settings_base_role_e and role
 * flag bits, \ref app_lib_settings_flag_role_e. Default node role is headnode
 * with the autorole flag set.
 *
 * Code example:
 *
 * @code
 *
 * #define NODE_ROLE \
 *     app_lib_settings_create_role(APP_LIB_SETTINGS_ROLE_HEADNODE, APP_LIB_SETTINGS_ROLE_FLAG_LL)
 *
 * void App_init(const app_global_functions_t * functions)
 * {
 *     // Open Wirepas public API
 *     API_Open(functions);
 *
 *     ...
 *
 *     // Configure node as Headnode, low-latency
 *     // This call force the role, and prevent RemoteAPI to change it
 *     lib_settings->setNodeRole(NODE_ROLE);
 *
 *     ...
 * }
 * @endcode
 *
 * \param   role
 *          New role
 * \return  Result code, \ref APP_RES_OK if successful, \ref APP_RES_INVALID_VALUE
 *          if \p role is invalid, \ref APP_RES_INVALID_STACK_STATE if stack is running
 */
typedef app_res_e
    (*app_lib_settings_set_node_role_f)(app_lib_settings_role_t role);

/**
 * Check if authentication key is set. It is not possible to actually read the
 * key from the stack. The \p key_p parameter is ignored.
 *
 * \param   key_p
 *          A dummy parameter, reserved for future, set to NULL
 * \return  Result code, \ref APP_RES_OK if a key set,
 *          APP_RES_INVALID_CONFIGURATION if the key is all 0xff, i.e. not set
 * \note    Reading the actual key value is not possible, for security reasons
 */
typedef app_res_e
    (*app_lib_settings_get_authentication_key_f)(uint8_t * key_p);

/**
 * Set authentication key. \p key_p must point to \ref
 * APP_LIB_SETTINGS_AES_KEY_NUM_BYTES bytes. By default, no authentication key
 * is set.
 *
 * A key of all <code>0xff</code> (hex) bytes is considered an unset key. Setting such a
 * key disables encryption and authentication.
 *
 * \param   key_p
 *          Pointer to AES-128 key,
 *          \ref APP_LIB_SETTINGS_AES_KEY_NUM_BYTES bytes
 * \return  Result code, \ref APP_RES_OK if successful,
 *          \ref APP_RES_INVALID_STACK_STATE if stack is running,
 *          \ref APP_RES_INVALID_NULL_POINTER if \p key_p is NULL
 * \note    Note that both the encryption and authentication keys must be set
 *          for the encryption or authentication to be enabled. It is NOT enough
 *          to set just one key.
 */
typedef app_res_e
    (*app_lib_settings_set_authentication_key_f)(const uint8_t * key_p);

/**
 * Check if encryption key is set. It is not possible to actually read the key
 * from the stack. The \p key_p parameter is ignored.
 *
 * \param   key_p
 *          A dummy parameter, reserved for future, set to NULL
 * \return  Result code, \ref APP_RES_OK if a key set,
 *          APP_RES_INVALID_CONFIGURATION if the key is all 0xff, i.e. not set
 * \note    Reading the actual key value is not possible, for security reasons
 */
typedef app_res_e
    (*app_lib_settings_get_encryption_key_f)(uint8_t * key_p);

/**
 * Set encryption key. \p key_p must point to \ref
 * APP_LIB_SETTINGS_AES_KEY_NUM_BYTES bytes. By default, no encryption key is
 * set.
 *
 * A key of all <code>0xff</code> (hex) bytes is considered an unset key. Setting such a key
 * disables encryption and authentication.
 *
 * \param   key_p
 *          Pointer to AES-128 key,
 *          \ref APP_LIB_SETTINGS_AES_KEY_NUM_BYTES bytes
 * \return  Result code, \ref APP_RES_OK if successful,
 *          \ref APP_RES_INVALID_STACK_STATE if stack is running,
 *          \ref APP_RES_INVALID_NULL_POINTER if \p key_p is NULL
 * \note    Note that both the encryption and authentication keys must be set
 *          for the encryption or authentication to be enabled. It is NOT enough
 *          to set just one key.
 */
typedef app_res_e
    (*app_lib_settings_set_encryption_key_f)(const uint8_t * key_p);

/**
 * Get the access cycle range that this node uses to serve its neighbors.
 * The values are in milliseconds. This setting is only meaningful for nodes
 * that route data for others, i.e. sinks and headnodes.
 *
 * \param   ac_min_value_p
 *          Pointer to store the minimum current access cycle value
 * \param   ac_max_value_p
 *          Pointer to store the maximum current access cycle value
 * \return  Result code, \ref APP_RES_OK if successful,
 *          APP_RES_INVALID_CONFIGURATION if access cycle range not set,
 *          APP_RES_INVALID_NULL_POINTER if ac_min_value_p or ac_max_value_p
 *          is NULL
 */
typedef app_res_e
    (*app_lib_settings_get_ac_range_f)(uint16_t * ac_min_value_p,
                                       uint16_t * ac_max_value_p);

/**
 * Set the access cycle range that this node uses to serve its neighbors. This
 * setting is only meaningful for nodes that route data for others, i.e. sinks
 * and headnodes.
 *
 * The values are in milliseconds. Function \ref
 * app_lib_settings_get_ac_range_limits_f "lib_settings->getAcRangeLimits"() can also be used to query the limits.
 * Default range is min. 2000 ms, max. 8000 ms.
 *
 * Valid values are:
 * <table>
 * <tr><th>Value<th>Description
 * <tr><td>2000<td>2 seconds
 * <tr><td>4000<td>4 seconds
 * <tr><td>8000<td>8 seconds
 * </table>
 *
 * \param   ac_min_value
 *          Minimum access cycle value to set
 * \param   ac_max_value
 *          Maximum access cycle value to set
 * \return  Result code, \ref APP_RES_OK if successful, \ref APP_RES_INVALID_VALUE
 *          if \p ac_min_value or \p ac_max_value is invalid
 */
typedef app_res_e
    (*app_lib_settings_set_ac_range_f)(uint16_t ac_min_value,
                                       uint16_t ac_max_value);

/**
 * Get the offline scan interval, in seconds. The offline scan interval
 * determines how often a node scans for neighbors when it has no route to a
 * sink.
 *
 * \param   max_scan_p
 *          Pointer to store the scanning interval value
 * \return  Result code, \ref APP_RES_OK if successful,
 *          \ref APP_RES_INVALID_STACK_STATE if stack is not running,
 *          \ref APP_RES_INVALID_NULL_POINTER if \p max_scan_p is NULL
 */
typedef app_res_e
    (*app_lib_settings_get_offline_scan_f)(uint16_t * max_scan_p);

/**
 * Set the offline scan interval, in seconds. The offline scan interval
 * determines how often a node scans for neighbors when it has no route to a
 * sink. Value is automatically limited to a valid range. The default value,
 * before calling \ref app_lib_settings_set_offline_scan_f "lib_settings->setOfflineScan"() is 600 seconds (10
 * minutes).
 *
 * Valid offline scan values:
 * <table>
 * <tr><th>Value<th>Description
 * <tr><td>20<td>Minimum: 20 seconds
 * <tr><td>600<td>Maximum: 600 seconds (10 minutes), default
 * </table>
 *
 * To manually start a neighbor scan, function startScanNbors() in the State
 * library (\ref state.h) can be used.
 *
 * \param   max_scan
 *          Minimum maximum scanning interval value
 * \return  Result code, \ref APP_RES_OK if successful, \ref APP_RES_INVALID_VALUE
 *          if \p max_scan is invalid
 */
typedef app_res_e
    (*app_lib_settings_set_offline_scan_f)(uint16_t max_scan);

/**
 * Get the allocation between CF-MAC ("low-energy") and CB-MAC ("low-latency")
 * channels. See WP-RM-100 - Wirepas Mesh Dual-MCU API Reference Manual for an
 * explanation on CF-MAC and CB-MAC operation. LSB is the first available
 * channel. A set bit indicates CB-MAC mode.
 *
 * \param   channelmap_p
 *          Pointer to store the channelmap
 *          Each set bit allocates the channel for CB-MAC. LSB is channel 1,
 *          MSB is channel 32
 * \return  Result code, \ref APP_RES_OK if successful,
 *          APP_RES_INVALID_NULL_POINTER if \ref channelmap_p is NULL
 * \note    WM FW releases 3.5 and onwards calling this functions will return
 *          an error code.
 */
typedef app_res_e
    (*app_lib_settings_get_channel_map_f)(uint32_t * channelmap_p);

/**
 * Set the allocation between CF-MAC ("low-energy") and CB-MAC ("low-latency")
 * channels. See \ref app_lib_settings_get_channel_map_f "lib_settings->getChannelMap"() function.
 *
 * At least one channel needs to be reserved for CF-MAC use, i.e. a bit must be
 * zero.
 *
 * \param   channelmap
 *          Each set bit allocates the channel for CB-MAC. LSB is channel 1,
 *          MSB is channel 32
 * \return  Result code, \ref APP_RES_OK if successful, \ref APP_RES_INVALID_VALUE
 *          if \p channelmap is invalid, \ref APP_RES_INVALID_STACK_STATE if stack
 *          is running
 * \note    WM FW releases 3.5 and onwards calling this functions will return
 *          an error code.
 */
typedef app_res_e
    (*app_lib_settings_set_channel_map_f)(uint32_t channelmap);

/**
 * Return the minimum and maximum network channel value that can be used when
 * setting the network channel with the \ref
 * app_lib_settings_set_network_channel_f "lib_settings->setNetworkChannel"() function
 *
 * \param   min_value_p
 *          Pointer to store the minimum network channel value allowed
 * \param   max_value_p
 *          Pointer to store the maximum network channel value allowed
 * \return  Result code, \ref APP_RES_OK if successful,
 *          APP_RES_INVALID_NULL_POINTER if min_value_p or max_value_p is NULL
 */
typedef app_res_e
    (*app_lib_settings_get_network_channel_limits_f)(uint16_t * min_value_p,
                                                     uint16_t * max_value_p);

/**
 * Return the minimum and maximum access cycle value, in milliseconds, that can
 * be used when setting the access cycle range with the \ref
 * app_lib_settings_set_ac_range_f "lib_settings->setAcRange"() function.
 *
 * \param   min_value_p
 *          Pointer to store the minimum access cycle value allowed
 * \param   max_value_p
 *          Pointer to store the maximum access cycle value allowed
 * \return  Result code, \ref APP_RES_OK if successful,
 *          APP_RES_INVALID_NULL_POINTER if min_value_p or max_value_p is NULL
 */
typedef app_res_e
    (*app_lib_settings_get_ac_range_limits_f)(uint16_t * min_value_p,
                                              uint16_t * max_value_p);

/**
 * Set the callback function that is called when stack needs to determine on
 * which multicast groups the device belongs to. If callback is not defined,
 * device does not belong to any multicast groups.
 *
 * \param   cb
 *          The function to be executed, or NULL to unset
 * \return  Result code, always \ref APP_RES_OK
 */
typedef app_res_e
    (*app_lib_settings_set_group_query_cb_f)(app_lib_settings_is_group_cb_f cb);

/**
 * Get a bit array of reserved channels, or channels that are marked to be
 * avoided by the Wirepas Mesh protocol. Each set bit marks a channel that is
 * to be avoided. The LSB of the first byte is channel 1, the LSB of the next
 * byte is channel 8 and so forth.
 *
 * \param   channels_p
 *          Pointer to store the reserved channels bit array
 *          Each set bit marks the channel as reserved
 *          LSB of first byte is channel 1, MSB of first byte is channel 7,
 *          LSB of second byte is channel 8, an so on
 *          etc
 * \param   num_bytes
 *          Number of bytes pointed by \ref channels_p
 * \return  Result code, \ref APP_RES_OK if successful,
 *          \ref APP_RES_INVALID_NULL_POINTER if \p channels_p is NULL,
 *          \ref APP_RES_INVALID_VALUE if last reserved channel does not fit in
 *          \p num_bytes
 * \note    \ref channels_p bit array can be longer than the maximum
 *          reserved channel. Remaining channels are marked as not reserved
 */
typedef app_res_e
    (*app_lib_settings_get_reserved_channels_f)(uint8_t * channels_p,
                                                size_t num_bytes);

/**
 * Mark channels as reserved, or to be avoided by the Wirepas Mesh protocol.
 * Each set bit marks a channel that is to be avoided. The LSB of the first
 * byte is channel 1, the LSB of the next byte is channel 8 and so forth. The
 * \p channels_p bit array may be shorter than the number of channels. In that
 * case, the remaining channels are marked as not reserved. The bit array may be
 * longer too, provided that the highest bit set in it corresponds to a valid
 * channel number (see section 3.6.4.27), i.e. extra zeros are ignored.
 *
 * A node may still transmit on a reserved channel if it has a neighbor that has
 * not been configured to avoid the channel. For best results, all nodes in a
 * network should be configured to have the same reserved channels. Reserving
 * the network channel will result in undefined behavior.
 *
 * The reserved channels array is not stored in permanent memory. To reserve
 * channels, function \ref app_lib_settings_set_reserved_channels_f "lib_settings->setReservedChannels"() has to be
 * called in App_init() before the stack is started.
 *
 * \param   channels
 *          Pointer to bit array to load the reserved channels
 *          Each set bit marks the channel as reserved
 *          LSB of first byte is channel 1, MSB of first byte is channel 7,
 *          LSB of second byte is channel 8, an so on
 * \param   num_bytes
 *          Number of bytes pointed by \ref channels_p
 * \return  Result code, \ref APP_RES_OK if successful,
 *          \ref APP_RES_INVALID_NULL_POINTER if \p channels_p is NULL,
 *          \ref APP_RES_INVALID_STACK_STATE if stack is running,
 *          \ref APP_RES_INVALID_VALUE if a bit in \ref channels_p is set for a
 *          channel larger than the maximum channel number
 * \note    \ref channels_p bit array can be shorter than the maximum number of
 *          channels. Remaining channels are marked as not reserved
 * \note    In the current implementation, reserved channels are not stored
 *          in persistent memory. Application must call setReservedChannels()
 *          in App_init()
 */
typedef app_res_e
    (*app_lib_settings_set_reserved_channels_f)(const uint8_t * channels_p,
                                                size_t num_bytes);


/**
 * \brief   Enables or disables periodic neighbor scanning.
 *          Note! Scan state is restored into default state (enabled) at boot.
 * \param   state
 *          see app_lib_settings_periodic_scan_control_e
 * \return  Result code, \ref APP_RES_OK if successful, \ref APP_RES_INVALID_VALUE
 *
 */
typedef app_res_e (*app_lib_settings_set_periodic_scan_state_f)
                  (app_lib_settings_periodic_scan_control_e state);

/**
 * \brief   Returns current state of periodic neighbor scanning.
 * \param   state
 *          Pointer where to store result
 * \return  Result code, \ref APP_RES_OK
 */
typedef app_res_e (*app_lib_settings_get_periodic_scan_state_f)
                  (app_lib_settings_periodic_scan_control_e *state);

/**
 * The function table returned from \ref app_open_library_f
 */
typedef struct
{
    app_lib_settings_reset_all_f                    resetAll;
    app_lib_settings_get_feature_lock_bits_f        getFeatureLockBits;
    app_lib_settings_set_feature_lock_bits_f        setFeatureLockBits;
    app_lib_settings_get_feature_lock_key_f         getFeatureLockKey;
    app_lib_settings_set_feature_lock_key_f         setFeatureLockKey;
    app_lib_settings_get_node_address_f             getNodeAddress;
    app_lib_settings_set_node_address_f             setNodeAddress;
    app_lib_settings_get_network_address_f          getNetworkAddress;
    app_lib_settings_set_network_address_f          setNetworkAddress;
    app_lib_settings_get_network_channel_f          getNetworkChannel;
    app_lib_settings_set_network_channel_f          setNetworkChannel;
    app_lib_settings_get_node_role_f                getNodeRole;
    app_lib_settings_set_node_role_f                setNodeRole;
    app_lib_settings_get_authentication_key_f       getAuthenticationKey;
    app_lib_settings_set_authentication_key_f       setAuthenticationKey;
    app_lib_settings_get_encryption_key_f           getEncryptionKey;
    app_lib_settings_set_encryption_key_f           setEncryptionKey;
    app_lib_settings_get_ac_range_f                 getAcRange;
    app_lib_settings_set_ac_range_f                 setAcRange;
    app_lib_settings_get_offline_scan_f             getOfflineScan;
    app_lib_settings_set_offline_scan_f             setOfflineScan;
    app_lib_settings_get_channel_map_f              getChannelMap;
    app_lib_settings_set_channel_map_f              setChannelMap;
    app_lib_settings_get_network_channel_limits_f   getNetworkChannelLimits;
    app_lib_settings_get_ac_range_limits_f          getAcRangeLimits;
    app_lib_settings_set_group_query_cb_f           registerGroupQuery;
    app_lib_settings_get_reserved_channels_f        getReservedChannels;
    app_lib_settings_set_reserved_channels_f        setReservedChannels;
} app_lib_settings_t;

#endif /* APP_LIB_SETTINGS_H_ */
