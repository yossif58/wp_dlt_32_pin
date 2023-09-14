/* Copyright 2017 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

/**
 * \file otap.h
 *
 * The Over-The-Air-Programming (OTAP) library allows reading and writing the
 * OTAP scratchpad memory area of a Wirepas Mesh node, as well as sending remote
 * status and update requests to the network. Using the OTAP library, it is
 * possible to wirelessly upgrade the stack firmware and applications on a
 * running network.
 *
 * To gain better understanding of the OTAP library, getting familiar with
 * concepts outlined in WP-RM-108 - OTAP Reference Manual is recommended.
 */
#ifndef APP_LIB_OTAP_H_
#define APP_LIB_OTAP_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "app/app.h"

/**
 * This is the library name for use with \ref
 * app_global_functions_t.openLibrary */
#define APP_LIB_OTAP_NAME 0x000f2338 //!< "OTAP"

/**
 * This is the library version for use with \ref
 * app_global_functions_t.openLibrary */
#define APP_LIB_OTAP_VERSION 0x200

/**
 * The scratchpad type. This value can either be queried locally with the
 * \ref app_lib_otap_get_type_f "lib_otap->getType"() function or remotely, in \ref
 * app_lib_otap_remote_status_t.
 */
typedef enum
{
    /** No valid scratchpad stored */
    APP_LIB_OTAP_TYPE_BLANK = 0,
    /** The is a valid scratchpad, but it has not yet been marked for
     * processing */
    APP_LIB_OTAP_TYPE_PRESENT = 1,
    /** There is a valid scratchpad and it has been marked for processing */
    APP_LIB_OTAP_TYPE_PROCESS = 2,
} app_lib_otap_type_e;

/**
 * Status code from the bootloader. Other small positive values may be
 * returned as error codes from the bootloader. This value can either be
 * queried locally with the \ref app_lib_otap_get_status_f "lib_otap->getStatus"() function or
 * remotely, in \ref app_lib_otap_remote_status_t.
 */
typedef enum
{
    /** The bootloader processed the scratchpad and everything went well */
    APP_LIB_OTAP_STATUS_OK = 0x00000000,
    /** The bootloader has not yet processed the scratchpad */
    APP_LIB_OTAP_STATUS_NEW = UINT32_MAX,
} app_lib_otap_status_e;

/**
 * A return type from the \ref app_lib_otap_write_f "lib_otap->write"() function
 */
typedef enum
{
    /** Write succeeded */
    APP_LIB_OTAP_WRITE_RES_OK = 0,
    /** Write succeeded and was the last one */
    APP_LIB_OTAP_WRITE_RES_COMPLETED_OK = 1,
    /** Error: write was the last one but the CRC check failed */
    APP_LIB_OTAP_WRITE_RES_COMPLETED_ERROR = 2,
    /** Error: there is no write ongoing */
    APP_LIB_OTAP_WRITE_RES_NOT_ONGOING = 3,
    /** Error: start is invalid */
    APP_LIB_OTAP_WRITE_RES_INVALID_START = 4,
    /** Error: num_bytes is invalid */
    APP_LIB_OTAP_WRITE_RES_INVALID_NUM_BYTES = 5,
    /** Error: header is invalid */
    APP_LIB_OTAP_WRITE_RES_INVALID_HEADER = 6,
    /** Error: bytes is NULL */
    APP_LIB_OTAP_WRITE_RES_INVALID_NULL_BYTES = 7,
} app_lib_otap_write_res_e;

/**
 * Type for OTAP sequence number. Two numbers are special:
 *
 * <table>
 * <tr><th>Value<th>Description
 * <tr><td>0<td>Node does not participate in OTAP
 * <tr><td>255<td>Node accepts any scratchpad from the network as newer
 * </table>
 */
typedef uint8_t app_lib_otap_seq_t;

/**
 * Information about stored scratchpad and the processed scratchpad that
 * produced the running stack firmware. This struct is passed to the remote
 * status reception callback \ref app_lib_otap_set_remote_status_received_cb_f
 * when remote status is received.
 */
typedef struct
{
    /** Size of stored scratchpad, in bytes */
    size_t num_bytes;
    /** Size of the scratchpad that produced the running stack firmware, in bytes */
    size_t processed_num_bytes;
    /** Node address of the sender of the remote status */
    app_addr_t source;
    /** Processed scratchpad status, \ref app_lib_otap_status_e */
    uint32_t status;
    /** Processed scratchpad area ID */
    uint32_t area_id;
    /** 16-bit CRC of the stored scratchpad */
    uint16_t crc;
    /** 16-bit CRC of the scratchpad that produced the running stack firmware */
    uint16_t processed_crc;
    /** Update request timeout in seconds, or 0 if update request not running */
    uint16_t update_req_timeout;
    /** OTAP sequence number of stored scratchpad, or 0 if no stored
     * scratchpad */
    app_lib_otap_seq_t seq;
    /** OTAP sequence number of the scratchpad that produced the running stack
     * firmware */
    app_lib_otap_seq_t processed_seq;
    /** Type of stored scratchpad, \ref app_lib_otap_type_e */
    uint8_t type;
    /** Major version of currently running stack firmware */
    uint8_t major_version;
    /** Minor version of currently running stack firmware */
    uint8_t minor_version;
    /** Maintenance version of currently running stack firmware */
    uint8_t maint_version;
    /** Development version of currently running stack firmware */
    uint8_t devel_version;
} app_lib_otap_remote_status_t;

/**
 * The remote status reception callback. The application sets a remote status
 * reception callback by calling \ref
 * app_lib_otap_set_remote_status_received_cb_f "lib_otap->setRemoteStatusReceivedCb"().
 *
 * The remote status is represented as a pointer to \ref
 * app_lib_otap_remote_status_t struct.
 */
typedef void
    (*app_lib_otap_remote_status_received_cb_f)(const app_lib_otap_remote_status_t * status);

/**
 * Return the maximum possible scratchpad size in bytes.
 */
typedef size_t
    (*app_lib_otap_get_max_num_bytes_f)(void);

/**
 * Return the size of stored scratchpad in bytes, or 0 if there is no stored
 * scratchpad.
 * \return  Number of bytes in scratchpad, or zero if scratchpad is not valid
 */
typedef size_t
    (*app_lib_otap_get_num_bytes_f)(void);

/**
 * Return the maximum block, in bytes, that can be fed to the \ref
 * app_lib_otap_write_f "lib_otap->write"() function
 *
 * \return  Maximum number of bytes for read() or write()
 */
typedef size_t
    (*app_lib_otap_get_max_block_num_bytes_f)(void);

/**
 * Get the OTAP sequence number \ref app_lib_otap_seq_t of the stored
 * scratchpad, or 0 if there is no stored scratchpad
 *
 * \return  Sequence number, or zero if scratchpad is not valid
 */
typedef app_lib_otap_seq_t
    (*app_lib_otap_get_seq_f)(void);

/**
 * Get the 16-bit CRC of the stored scratchpad, or 0 if there is no stored
 * scratchpad.
 *
 * Calling this function for the first time after a scratchpad has changed may
 * take several tens of milliseconds.
 *
 * \return  CRC of data in scratchpad, or zero if scratchpad is not valid
 * \note    Calling this function may take several tens of milliseconds
 */
typedef uint16_t
    (*app_lib_otap_get_crc_f)(void);

/**
 * Get the type of the stored scratchpad.
 *
 * \return  Type of stored scratchpad, \ref app_lib_otap_type_e
 */
typedef app_lib_otap_type_e
    (*app_lib_otap_get_type_f)(void);

/**
 * Get the bootloader status of the stored scratchpad.
 *
 * \return  Status, or \ref APP_LIB_OTAP_STATUS_OK
 *          if scratchpad is not valid
 */
typedef app_lib_otap_status_e
    (*app_lib_otap_get_status_f)(void);

/**
 * Return the size of scratchpad, in bytes, that produced the running stack
 * firmware. The bootloader stores this information in the stack firmware area,
 * when it writes the firmware.
 *
 * \return  Number of bytes in processed scratchpad, or zero if not known
 */
typedef size_t
    (*app_lib_otap_get_processed_num_bytes_f)(void);

/**
 * Return the OTAP sequence number of the scratchpad that produced the running
 * stack firmware. The bootloader stores this information in the stack firmware
 * area, when it writes the firmware.
 *
 * \return  Sequence number of processed scratchpad, or zero if not known
 */
typedef app_lib_otap_seq_t
    (*app_lib_otap_get_processed_seq_f)(void);

/**
 * Return the 16-bit CRC of the scratchpad that produced the running stack
 * firmware. The bootloader stores this information in the stack firmware area,
 * when it writes the firmware.
 *
 * \return  CRC of processed scratchpad, or zero if not known
 */
typedef uint16_t
    (*app_lib_otap_get_processed_crc_f)(void);

/**
 * Return the area ID of the file in the scratchpad that produced the running
 * stack firmware. The bootloader stores this information in the stack firmware
 * area, when it writes the firmware.
 *
 * \return  Area ID of running stach firmware, or zero if not known
 */
typedef uint32_t
    (*app_lib_otap_get_processed_area_id_f)(void);

/**
 * Return true if there is a stored scratchpad, false otherwise. The same
 * information is available via the more generic function \ref
 * app_lib_otap_get_type_f "lib_otap->getType"().
 *
 * \note    Valid data isn't necessarily a firmware image
 * \return  True, if data in scratchpad is valid
 */
typedef bool
    (*app_lib_otap_is_valid_f)(void);

/**
 * Return true if the stored scratchpad has been processed by the bootloader,
 * false otherwise. The same information is available via the more generic
 * functions \ref app_lib_otap_get_type_f "lib_otap->getType"() and \ref app_lib_otap_get_status_f "lib_otap->getStatus"().
 *
 * \return  True, if the scratchpad has been processed by the bootloader
 */
typedef bool
    (*app_lib_otap_is_processed_f)(void);

/**
 * \brief   Check if scratchpad is set to be processed
 * \return  True, if scratchpad data is valid and marked to be processed
 */
typedef bool
    (*app_lib_otap_is_set_to_be_processed_f)(void);

/**
 * Read a block of scratchpad. Reading can be done in a random-access fashion
 * and stack can remain running.
 *
 * \p start is the byte offset within the scratchpad. \p num_bytes is the number
 * of bytes to read. bytes must point to a buffer of suitable size. Maximum
 * number of bytes to read at a time can be queried with the \ref
 * app_lib_otap_get_max_block_num_bytes_f "lib_otap->getMaxBlockNumBytes"() function.
 *
 * Simultaneous reading and writing of scratchpad is not supported. A \ref
 * app_lib_otap_begin_f "lib_otap->begin"() / \ref app_lib_otap_write_f "lib_otap->write"() cycle must not be
 * ongoing when calling this function.
 *
 * \param   start
 *          Byte offset from the beginning of scratchpad memory
 * \param   num_bytes
 *          Number of bytes to read
 * \param   bytes
 *          Pointer to buffer for reading bytes
 * \return  Result code, \ref APP_RES_OK if successful,
 *          \ref APP_RES_INVALID_VALUE if start or num_bytes is invalid,
 *          \ref APP_RES_INVALID_NULL_POINTER if bytes is NULL,
 *          \ref APP_RES_RESOURCE_UNAVAILABLE if scratchpad is not valid
 */
typedef app_res_e
    (*app_lib_otap_read_f)(uint32_t start,
                           size_t num_bytes,
                           void * bytes);

/**
 * Erase a stored scratchpad from memory. Erasing the scratchpad is only
 * possible when the stack is stopped. On some platforms, this function may
 * take several seconds to complete.
 *
 * When the stack is started, unless feature lock bits prevent it, a scratchpad
 * is immediately copied from any neighbor node that has one.
 *
 * \return  Result code, \ref APP_RES_OK if successful,
 *          \ref APP_RES_INVALID_STACK_STATE if stack is running
 */
typedef app_res_e
    (*app_lib_otap_clear_f)(void);

/**
 * Start writing a new scratchpad. Any old scratchpad is implicitly erased,
 * which may take several seconds on some platforms.
 *
 * Scratchpad can only be written when the stack is stopped. \p num_bytes must
 * be divisible by four and a minimum of 96 bytes, which is the minimum valid
 * scratchpad size. Maximum scratchpad size can be queried with the \ref
 * app_lib_otap_get_max_num_bytes_f "lib_otap->getMaxNumBytes"() function.
 *
 * When writing the scratchpad to a node, with the \ref app_lib_otap_write_f "lib_otap->write"()
 * function described below, the bytes written contain a scratchpad header, with
 * an OTAP sequence number field. The OTAP sequence number in that header is
 * replaced with the seq parameter specified here.
 *
 * Returns \ref APP_RES_INVALID_STACK_STATE if stack is running. Returns \ref
 * APP_RES_INVALID_VALUE if num_bytes is invalid. Otherwise, returns \ref
 * APP_RES_OK.
 *
 * \param   num_bytes
 *          Total number of bytes to write
 * \param   seq
 *          Scratchpad sequence number
 * \return  Result code, \ref APP_RES_OK if successful,
 *          \ref APP_RES_INVALID_VALUE if num_bytes or seq is invalid,
 *          \ref APP_RES_INVALID_STACK_STATE if stack is running
 * \note    Scratchpad memory is implicitly cleared if this call succeeds
 */
typedef app_res_e
    (*app_lib_otap_begin_f)(size_t num_bytes,
                            app_lib_otap_seq_t seq);

/**
 * Write a block of scratchpad. Write must have been started with the \ref
 * app_lib_otap_begin_f "lib_otap->begin"() function.
 *
 * \p start must follow the previous block exactly and \p num_bytes must be
 * divisible by four. The first block must start from zero. Maximum number of
 * bytes to write at a time can be queried with the \ref
 * app_lib_otap_get_max_block_num_bytes_f "lib_otap->getMaxBlockNumBytes"() function.
 *
 * \param   start
 *          Byte offset from the beginning of scratchpad memory
 * \param   num_bytes
 *          Number of bytes to write
 * \param   bytes
 *          Pointer to bytes to write
 * \return  A return code: \ref app_lib_otap_write_res_e
 * \note    \p num_bytes needs to be divisible by four
 * \note    \p start byte offset needs to be right after
 *          previous write, with no gaps or overlap
 */
typedef app_lib_otap_write_res_e
    (*app_lib_otap_write_f)(uint32_t start,
                            size_t num_bytes,
                            const void * bytes);

/**
 * Mark the stored scratchpad to be eligible for processing by the bootloader.
 * Whenever the node will be rebooted, the bootloader will process the
 * scratchpad. If the scratchpad was already marked for processing or was
 * already processed, this function does nothing.
 *
 * \note    Does nothing if scratchpad was already set to be bootable
 * \return  Result code, \ref APP_RES_OK if successful,
 *          \ref APP_RES_RESOURCE_UNAVAILABLE if scratchpad data is invalid
 */
typedef app_res_e
    (*app_lib_otap_set_to_be_processed_f)(void);

/**
 * Set the callback function to be called when new remote status is received.
 * If NULL is passed, the callback is disabled.
 *
 * This function always returns APP_RES_OK.
 *
 * \param   cb
 *          The function to be executed, or NULL to unset
 * \return  Result code, always \ref APP_RES_OK
 */
typedef app_res_e
    (*app_lib_otap_set_remote_status_received_cb_f)(app_lib_otap_remote_status_received_cb_f cb);

/**
 * Send a remote status request to a given unicast address, or all nodes on the
 * network when target is \ref APP_ADDR_BROADCAST.
 *
 * Returns \ref APP_RES_INVALID_STACK_STATE if stack is not running. Returns
 * \ref APP_RES_INVALID_CONFIGURATION if node is not a sink. Returns \ref
 * APP_RES_INVALID_VALUE if target is zero or \ref APP_ADDR_ANYSINK. Returns
 * \ref APP_RES_RESOURCE_UNAVAILABLE if there is no memory to send the request.
 * Otherwise, returns \ref APP_RES_OK.
 *
 * \param   target
 *          The node to send the request to, or \ref APP_ADDR_BROADCAST
 * \return  Result code, \ref APP_RES_OK if successful,
 *          \ref APP_RES_INVALID_VALUE if address is invalid,
 *          \ref APP_RES_INVALID_STACK_STATE if stack is not running,
 *          \ref APP_RES_INVALID_CONFIGURATION if node is not a sink,
 *          \ref APP_RES_RESOURCE_UNAVAILABLE if out of memory
 */
typedef app_res_e
    (*app_lib_otap_send_remote_status_req_f)(app_addr_t target);

/**
 * Send a remote update request to a given unicast address, or all nodes on the
 * network when target is \ref APP_ADDR_BROADCAST.
 *
 * \p target must not be zero or APP_ADDR_ANYSINK. \p seq must not be 0 or 255.
 * \p timeout is in seconds and must be either 0 to disable a running countdown
 * or be between 10 and 32767 to start (or restart) a countdown.
 *
 * Returns \ref APP_RES_INVALID_STACK_STATE if stack is not running. Returns
 * \ref APP_RES_INVALID_CONFIGURATION if node is not a sink. Returns \ref
 * APP_RES_INVALID_VALUE if target, seq or timeout is invalid. Returns \ref
 * APP_RES_RESOURCE_UNAVAILABLE if there is no memory to send the request.
 * Otherwise, returns \ref APP_RES_OK.
 *
 * \param   target
 *          The node to send the request to, or \ref APP_ADDR_BROADCAST
 * \param   seq
 *          OTAP sequence number, 0 and 255 are invalid
 * \param   timeout
 *          Number of seconds from 10 to 32767 before the node is rebooted
 *          or 0 to cancel a running countdown
 * \return  Result code, \ref APP_RES_OK if successful,
 *          \ref APP_RES_INVALID_VALUE if address, seq or timeout is invalid,
 *          \ref APP_RES_INVALID_STACK_STATE if stack is not running,
 *          \ref APP_RES_INVALID_CONFIGURATION if node is not a sink,
 *          \ref APP_RES_RESOURCE_UNAVAILABLE if out of memory
 */
typedef app_res_e
    (*app_lib_otap_send_remote_update_req_f)(app_addr_t target,
                                             app_lib_otap_seq_t seq,
                                             uint16_t timeout);

/**
 * The function table returned from \ref app_open_library_f
 */
typedef struct
{
    app_lib_otap_get_max_num_bytes_f getMaxNumBytes;
    app_lib_otap_get_num_bytes_f getNumBytes;
    app_lib_otap_get_max_block_num_bytes_f getMaxBlockNumBytes;
    app_lib_otap_get_seq_f getSeq;
    app_lib_otap_get_crc_f getCrc;
    app_lib_otap_get_type_f getType;
    app_lib_otap_get_status_f getStatus;
    app_lib_otap_get_processed_num_bytes_f getProcessedNumBytes;
    app_lib_otap_get_processed_seq_f getProcessedSeq;
    app_lib_otap_get_processed_crc_f getProcessedCrc;
    app_lib_otap_get_processed_area_id_f getProcessedAreaId;
    app_lib_otap_is_valid_f isValid;
    app_lib_otap_is_processed_f isProcessed;
    app_lib_otap_is_set_to_be_processed_f isSetToBeProcessed;
    app_lib_otap_read_f read;
    app_lib_otap_clear_f clear;
    app_lib_otap_begin_f begin;
    app_lib_otap_write_f write;
    app_lib_otap_set_to_be_processed_f setToBeProcessed;
    app_lib_otap_set_remote_status_received_cb_f setRemoteStatusReceivedCb;
    app_lib_otap_send_remote_status_req_f sendRemoteStatusReq;
    app_lib_otap_send_remote_update_req_f sendRemoteUpdateReq;
} app_lib_otap_t;

#endif /* APP_LIB_OTAP_H_ */
