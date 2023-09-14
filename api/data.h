/* Copyright 2017 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

/**
 * \file data.h
 *
 * The Data library contains functions for sending and receiving data packets.
 * Also contained within are functions for sending and receiving app config
 * data, which is a small bit of data that gets distributed to all nodes on the
 * network. Any new nodes will receive app config data quickly during joining
 * process to the network. App config very lightweight and can also be
 * considered as 'network persistent data'.
 */
#ifndef APP_LIB_DATA_H_
#define APP_LIB_DATA_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "app/app.h"

/**
 * This is the library name for use with \ref
 * app_global_functions_t.openLibrary */
#define APP_LIB_DATA_NAME 0x0003f161 //!< "DATA"

/**
 * This is the library version for use with \ref
 * app_global_functions_t.openLibrary "*/
#define APP_LIB_DATA_VERSION    0x204

/**
 * A type of tracking ID for sent data packets, to keep track of packets sent
 * through local processing, until packet is finally sent or discarded. The
 * valid range for this id is [0, 65534 (0xFFFF -1) ]. The value 65535 (0xFFFF)
 * is used to disable the id tracking (\ref APP_LIB_DATA_NO_TRACKING_ID).
 */
typedef uint16_t app_lib_data_tracking_id_t;

/**
 * When sending data and no tracking of packet is requested, this ID may be
 * used.
 */
#define APP_LIB_DATA_NO_TRACKING_ID (app_lib_data_tracking_id_t)(-1)

/**
 * This is a safe size to use for app config data static buffers. Use
 * \ref app_lib_data_get_app_config_num_bytes_f
 * "lib_data->getAppConfigNumBytes()" to get actual app config data size in
 * bytes, which may be smaller.
 */
#define APP_LIB_DATA_MAX_APP_CONFIG_NUM_BYTES   80

/**
 * Data quality of service class. Used when sending and receiving data packets.
 */
typedef enum
{
    /** Normal quality of service */
    APP_LIB_DATA_QOS_NORMAL = 0,
    /** High quality of service, i.e. takes priority over normal quality of
     * service packets */
    APP_LIB_DATA_QOS_HIGH   = 1,
} app_lib_data_qos_e;

/**
 * Flags to use with \ref app_lib_data_t.sendData()
 */
typedef enum
{
    /** Default value, no tracking of packet */
    APP_LIB_DATA_SEND_FLAG_NONE   = 0,
    /** Track packet through local processing, i.e. call tracking callback when
     * packet is finally sent or discarded */
    APP_LIB_DATA_SEND_FLAG_TRACK  = 1,
    /** \ref app_lib_data_to_send_t.hop_limit field contains the value of hop
     * limit to be used in sending
     *
     * Hop limit sets the upper value to the number of hops executed for packet
     * to reach the destination. By using hop limiting, it is possible to limit
     * the distance how far the packet is transmitted to and avoiding causing
     * unnecessary traffic to network. Hop limit value of 0 is used to disable
     * the hop limiting. Hop limiting value does not have any impact when using
     * \ref APP_ADDR_ANYSINK address as destination node address but is
     * discarded.
     * */
    APP_LIB_DATA_SEND_SET_HOP_LIMITING = 4,
    /** The unacknowledged CSMA-CA transmission method can be used in a mixed
     * network (i.e. network consisting of both CSMA-CA and TDMA devices) by
     * CSMA-CA device originated packets transmission only to CSMA-CA devices.
     * The purpose of this method is to avoid a performance bottleneck by NOT
     * transmitting to TDMA devices. Also, if used with sink-originated
     * transmissions (by CSMA-CA mode sinks), the throughput is better when
     * compared to a 'normal' transmission, however there is some penalty in
     * reliability (due to unacknowledged nature of transmission). */
    APP_LIB_DATA_SEND_FLAG_UNACK_CSMA_CA = 8,
} app_lib_data_send_flags_e;

/**
 * A result code returned from \ref app_lib_data_t.sendData
 */
typedef enum
{
    /** Data was accepted in stack buffers */
    APP_LIB_DATA_SEND_RES_SUCCESS              = 0,
    /** Error: stack is not running */
    APP_LIB_DATA_SEND_RES_INVALID_STACK_STATE  = 1,
    /** Error: QoS parameter is invalid */
    APP_LIB_DATA_SEND_RES_INVALID_QOS          = 2,
    /** Error: flags parameter is invalid */
    APP_LIB_DATA_SEND_RES_INVALID_FLAGS        = 3,
    /** Error: there is no space for data in stack buffers */
    APP_LIB_DATA_SEND_RES_OUT_OF_MEMORY        = 4,
    /** Error: destination address parameter is invalid */
    APP_LIB_DATA_SEND_RES_INVALID_DEST_ADDRESS = 5,
    /** Error: number of bytes parameter is invalid */
    APP_LIB_DATA_SEND_RES_INVALID_NUM_BYTES    = 6,
    /** Error: tracking ID already in use, or there are no more tracking IDs
     * available */
    APP_LIB_DATA_SEND_RES_OUT_OF_TRACKING_IDS  = 7,
    /** Tracking ID already in use or invalid ID */
    APP_LIB_DATA_SEND_RES_INVALID_TRACKING_ID  = 8,
    /** Error: one of the endpoints is invalid, reserved for stack internal
     * use */
    APP_LIB_DATA_SEND_RES_RESERVED_ENDPOINT    = 9,
    /** Error: data sending is forbidden, disabled by feature lock bits */
    APP_LIB_DATA_SEND_RES_ACCESS_DENIED        = 10,
    /** Error: Hop limit value is invalid */
    APP_LIB_DATA_SEND_RES_INVALID_HOP_LIMIT    = 11,
} app_lib_data_send_res_e;

/**
 * This result code needs to be returned from the reception callbacks
 */
typedef enum
{
    /** Packet was for the application and it was handled successfully. Stack
     * may now discard the packet. */
    APP_LIB_DATA_RECEIVE_RES_HANDLED     = 0,
    /** Packet was not for the application. Stack may offer the packet to some
     * other module, if present, or discard it. */
    APP_LIB_DATA_RECEIVE_RES_NOT_FOR_APP = 1,
    /** Packet was for the application, but the application cannot handle it
     * right now. Stack is requested to keep the packet in its buffers, until
     * \ref app_lib_data_t.allowReception(true) is called. */
    APP_LIB_DATA_RECEIVE_RES_NO_SPACE    = 2,
} app_lib_data_receive_res_e;

/**
 * A result code returned from \ref app_lib_data_t.readAppConfig() and
 * parameter for \ref app_lib_data_t.writeAppConfig()
 */
typedef enum
{
    /** Reading or writing app config data was successful */
    APP_LIB_DATA_APP_CONFIG_RES_SUCCESS              = 0,
    /** Error: cannot write app config data: node is not a sink */
    APP_LIB_DATA_APP_CONFIG_RES_INVALID_ROLE         = 1,
    /** Error: cannot read app config data: no app config data set or received
    */
    APP_LIB_DATA_APP_CONFIG_RES_INVALID_APP_CONFIG   = 2,
    /** Error: invalid sequence number parameter */
    APP_LIB_DATA_APP_CONFIG_RES_INVALID_SEQ          = 3,
    /** Error: invalid interval parameter */
    APP_LIB_DATA_APP_CONFIG_RES_INVALID_INTERVAL     = 4,
    /** Error: invalid NULL pointer parameter */
    APP_LIB_DATA_APP_CONFIG_RES_INVALID_NULL_POINTER = 5,
} app_lib_data_app_config_res_e;

/**
 * This macro declares special value represented in hops- field of
 * \ref app_lib_data_t.app_lib_data_received_t structure when device has been
 * unable to determine the hop count.
 */
#define APP_LIB_DATA_RX_HOPS_UNDETERMINED 0

/**
 * This struct is passed to the data reception callbacks \ref
 * app_lib_data_data_received_cb_f and \ref
 * app_lib_data_bcast_data_received_cb_f
 */
typedef struct
{
    /** Received bytes */
    const uint8_t * bytes;
    /** Number of bytes received */
    size_t num_bytes;
    /** Address of node that sent the packet */
    app_addr_t src_address;
    /** End-to-end transmission delay, in 1 / 128 of seconds */
    uint32_t delay;
    /** Packet quality of service class, see \ref app_lib_data_qos_e */
    app_lib_data_qos_e qos;
    /** Source endpoint of packet */
    uint8_t src_endpoint;
    /** Destination endpoint of packet */
    uint8_t dest_endpoint;
    /** Amount of hops that were used when routing packet to the destination */
    uint8_t hops;
} app_lib_data_received_t;


/**
 * A struct for \ref app_lib_data_t.sendData()
 */
typedef struct
{
    /** Bytes to send */
    const uint8_t * bytes;
    /** Number of bytes to send */
    size_t num_bytes;
    /** Destination address of packet */
    app_addr_t dest_address;
    /** Initial end-to-end transmission delay, in 1 / 128 seconds */
    uint32_t delay;
    /**
     * Packet tracking ID
     */
    app_lib_data_tracking_id_t tracking_id;
    /** Packet quality of service class */
    app_lib_data_qos_e qos;
    /** Send flags, see \ref app_lib_data_send_flags_e */
    uint8_t flags;
    /** Source endpoint of packet */
    uint8_t src_endpoint;
    /** Destination endpoint of packet */
    uint8_t dest_endpoint;
    /** Maximum amount of hops allowed for transmission. Requires also flag
     * \ref APP_LIB_DATA_SEND_SET_HOP_LIMITING to be set in flags field in order
     * to be active. When used, value must be >0. */
    uint8_t hop_limit;
} app_lib_data_to_send_t;

/**
 * This struct is passed to the tracking callback (\ref
 * app_lib_data_data_sent_cb_f) when a packet is either sent or discarded.
 */
typedef struct
{
    /** Destination address of packet */
    app_addr_t dest_address;
    /** Time the packet spent in the local buffer, in 1 / 128 seconds. This also
     * includes the value set in the delay field for \ref
     * app_lib_data_t.sendData().
    */
    uint32_t queue_time;
    /**
     * Packet tracking ID. This may be used to inform when packet has left the
     * device. Note: it does *not* mean that packet has reached the destination!
     * If tracking is enabled (value != \ref APP_LIB_DATA_NO_TRACKING_ID), the
     * \ref app_lib_data_data_sent_cb_f callback is called when packet has left
     * the device */
    app_lib_data_tracking_id_t tracking_id;
    /** Source endpoint of packet */
    uint8_t src_endpoint;
    /** Destination endpoint of packet */
    uint8_t dest_endpoint;
    /** True if packet was sent, false if packet was discarded */
    bool success;
} app_lib_data_sent_status_t;

/**
 * The unicast data reception callback. The application sets a data reception
 * callback by calling \ref app_lib_data_t.setDataReceivedCb().
 *
 * The received packet is represented as a pointer to \ref
 * app_lib_data_received_t struct. Depending on the return value, the stack
 * either keeps or discards the packet.
 *
 * \param   data
 *          Received data
 * \return  Result code, \ref app_lib_data_receive_res_e
 */
typedef app_lib_data_receive_res_e
    (*app_lib_data_data_received_cb_f)(const app_lib_data_received_t * data);

/**
 * The broadcast data reception callback. The application sets a data reception
 * callback by calling \ref app_lib_data_t.setBcastDataReceivedCb().
 *
 * The received packet is represented as a pointer to \ref
 * app_lib_data_received_t struct. Depending on the return value, the stack
 * either keeps or discards the packet.
 *
 * \param   data
 *          Received data
 * \return  Result code, \ref app_lib_data_receive_res_e
 */
typedef app_lib_data_receive_res_e
    (*app_lib_data_bcast_data_received_cb_f)
    (const app_lib_data_received_t * data);



/**
 * The sent packet tracking callback. The application sets a tracking callback
 * by calling \ref app_lib_data_t.setDataSentCb(). Status of the sent packet is
 * represented as a pointer to \ref app_lib_data_sent_status_t struct. There is
 * no return value from the callback.
 */
typedef void
    (*app_lib_data_data_sent_cb_f)(const app_lib_data_sent_status_t * status);

/**
 * The new app config callback. The application sets a new app config callback
 * by calling \ref app_lib_data_t.setNewAppConfigCb().
 *
 * New app config callback is called whenever new app config is received and
 * when the node first joins a network. There is no return value from the
 * callback.
 *
 * \param   bytes
 *          New app config data
 * \param   seq
 *          New app config data sequence number
 * \param   interval
 *          New app config data diagnostic interval, in seconds
 */
typedef void (*app_lib_data_new_app_config_cb_f)(const uint8_t * bytes,
                                                 uint8_t seq,
                                                 uint16_t interval);

/**
 * Set the callback function to be called when new unicast data is received.
 * Unicast data is data that is addressed directly to a specific node.
 * If NULL is passed, the callback is disabled.
 *
 * Example on use. Application handles destination endpoint of 12 as own
 * incoming data and triggers temperature measurement.
 * @code
 *
 * #define GET_TEMPERATURE_EP  12
 *
 * static app_lib_data_receive_res_e unicastDataReceivedCb(
 *     const app_lib_data_received_t * data)
 * {
 *     if (data->dest_endpoint == GET_TEMPERATURE_EP)
 *     {
 *         //start_temperature_measurement();
 *         return APP_LIB_DATA_RECEIVE_RES_HANDLED;
 *     }
 *
 *     return APP_LIB_DATA_RECEIVE_RES_NOT_FOR_APP;
 * }
 *
 * void App_init(const app_global_functions_t * functions)
 * {
 *     // Open Wirepas public API
 *     API_Open(functions);
 *
 *     ...
 *
 *     // Register for unicast messages
 *     lib_data->setDataReceivedCb(unicastDataReceivedCb);
 *
 *     // Start the stack
 *     lib_state->startStack();
 * }
 * @endcode
 *
 * \param   cb
 *          The function to be executed, or NULL to unset
 * \return  Result code, always \ref APP_RES_OK
 */
typedef app_res_e
    (*app_lib_data_set_data_received_cb_f)(app_lib_data_data_received_cb_f cb);

/**
 * Set the callback function to be called when new broadcast data is received.
 * Broadcast data is data that is addressed to all nodes on the network.
 * Alternatively, multicast data is also received by this callback. If NULL is
 * passed, the callback is disabled.
 * \param   cb
 *          The function to be executed, or NULL to unset
 * \return  Result code, always \ref APP_RES_OK
 */
typedef app_res_e
    (*app_lib_data_set_bcast_data_received_cb_f)
    (app_lib_data_bcast_data_received_cb_f cb);


/**
 * \brief   Reserved location in library
 */
typedef void (*app_lib_data_reserved_f)(void);

/**
 * Set the callback function to be called when a packet has gone through local
 * processing and has finally been sent or discarded. If NULL is passed, the
 * callback is disabled
 * \param   cb
 *          The function to be executed, or NULL to unset
 * \return  Result code, always \ref APP_RES_OK
 */
typedef app_res_e
    (*app_lib_data_set_data_sent_cb_f)(app_lib_data_data_sent_cb_f cb);

/**
 * Set the callback function to be called when new @ref appconfig
 * "app config data" is received.
 * App config data is a small bit of data that gets set on sinks and then gets
 * distributed to all nodes on the network. If NULL is passed, the callback is
 * disabled.
 * \param   cb
 *          The function to be executed, or NULL to unset
 * \return  Result code, always \ref APP_RES_OK
 */
typedef app_res_e
    (*app_lib_data_set_new_app_config_cb_f)
    (app_lib_data_new_app_config_cb_f cb);

/**
 * Return the maximum number of bytes per data packet. Different platforms have
 * differently sized radio packets, so this value varies by platform.
 * \return  Maximum size of data packet in bytes
 */
typedef size_t
    (*app_lib_data_get_data_max_num_bytes_f)(void);

/**
 * Return the total number of packet buffers for sent data. See
 * \ref app_lib_data_t.getNumFreeBuffers() below for reading the number of
 * available buffers. This function applies for transmitted packets that are
 * tracked.
 * \return  Total number of data packets that can be buffered
 * \note    When packets are send with \ref app_lib_data_t.sendData without
 *          tracking (tracking_id == \ref APP_LIB_DATA_NO_TRACKING_ID), there
 *          are plenty of more buffers available than with tracking enabled.
 *          For tracking enabled, there is only 16 buffers available.
 */
typedef size_t
    (*app_lib_data_get_num_buffers_f)(void);

/**
 * Query the number of currently available buffers for sending data. If there
 * are no buffers left, sending data is not possible. Function \ref
 * app_lib_data_t.getNumBuffers() above returns the maximum number of buffers
 * available. This function applies for transmitted packets that are tracked.
 *
 * \param   num_buffers_p
 *          Pointer to a value where the number of data packets
 *          that can be buffered will be stored
 * \return  Result code, \ref APP_RES_OK if stack is running, otherwise
 *          \ref APP_RES_INVALID_STACK_STATE
 * \note    When packets are send with \ref app_lib_data_t.sendData without
 *          tracking (tracking_id == \ref APP_LIB_DATA_NO_TRACKING_ID), there
 *          are plenty of more buffers available than with tracking enabled.
 *          For tracking enabled, there is only 16 buffers available.
 */
typedef app_res_e
    (*app_lib_data_get_num_free_buffers_f)(size_t * num_buffers_p);

/**
 * Send data. The packet to send is represented as \ref app_lib_data_to_send_t
 * struct.
 * \param   data
 *          Data to send
 * \return  Result code, \ref APP_LIB_DATA_SEND_RES_SUCCESS means that data
 *          was accepted for sending. See \ref app_lib_data_send_res_e for
 *          other result codes.
 */
typedef app_lib_data_send_res_e
    (*app_lib_data_send_data_f)(const app_lib_data_to_send_t * data);

/**
 * Allow or block reception. The application may temporarily tell the stack to
 * not call the reception callback, if an external interface is not responding,
 * for example. The stack will then keep the received packets in its internal
 * buffers until reception is allowed again.
 *
 * If the reception callback returns \ref APP_LIB_DATA_RECEIVE_RES_NO_SPACE, it
 * is the same as calling allowReception(false).
 *
 * \param   allow
 *          True if it is OK to call the reception callback, false otherwise
 * \note    If reception is not allowed and there is buffered data, calling
 *          this function with a parameter of true will cause the reception
 *          callback to be called right away
 */
typedef void
    (*app_lib_data_allow_reception_f)(bool allow);

/**
 * \brief   Read @ref appconfig "app config"
 * \param   bytes
 *          Pointer to a buffer for app config data
 * \param   seq
 *          Pointer to app config sequence
 * \param   interval
 *          Pointer to diagnostic interval in seconds
 * \return  Result code, \ref APP_LIB_DATA_APP_CONFIG_RES_SUCCESS if
 *          successful.  See \ref app_lib_data_app_config_res_e for
 *          other result codes.
 */
typedef app_lib_data_app_config_res_e
    (*app_lib_data_read_app_config_f)(uint8_t * bytes,
                                      uint8_t * seq,
                                      uint16_t * interval);

/**
 * \brief   Write @ref appconfig "app config"
 * \param   bytes
 *          Pointer to app config data to write
 * \param   seq
 *          App config sequence
 * \param   interval
 *          Diagnostic interval in seconds
 * \return  Result code, \ref APP_LIB_DATA_APP_CONFIG_RES_SUCCESS if
 *          successful.  See \ref app_lib_data_app_config_res_e for
 *          other result codes.
 */
typedef app_lib_data_app_config_res_e
    (*app_lib_data_write_app_config_f)(const uint8_t * bytes,
                                       uint8_t seq,
                                       uint16_t interval);

/**
 * \brief   Get size of @ref appconfig "app config"
 * \return  App config size in bytes
 */
typedef size_t
    (*app_lib_data_get_app_config_num_bytes_f)(void);


/**
 * \brief   Set maximum queuing time for messages
 * \param   priority
 *          Message priority which queuing time to be set
 * \param   time
 *          Queuing time in seconds. Accepted range: 2 - 65534s.
 *          Select queuing time carefully, too short value might cause
 *          unnecessary message drops and too big value filling up message
 *          queues. For consistent performance it is recommended to use the
 *          same queuing time in the whole network.
 *
 *          Note! Minimum queuing time shall be bigger than access cycle
 *          interval in TDMA networks. It is recommended to use multiples of
 *          access cycle interval (+ extra) to give time for message
 *          repetitions, higher priority messages taking over the access
 *          slot etc. Access cycle is not limiting the minimum value in
 *          CSMA-CA networks.
 * \return  Result code, \ref APP_RES_OK if successful
 *          APP_RES_INVALID_VALUE if unsupported message priority or time
 *
 */
typedef app_res_e
    (*app_lib_data_set_max_msg_queuing_time_f)(app_lib_data_qos_e priority,
                                               uint16_t time);

/**
 * \brief   Get maximum queuing time of messages
 * \param   priority
 *          Message priority which queuing time to be read
 * \param   time_p
 *          Pointer where to store maximum queuing time
 * \return  Result code, \ref APP_RES_OK
 *          APP_RES_INVALID_VALUE if unsupported message priority
 *          APP_RES_INVALID_NULL_POINTER if time_p is null
 */
typedef app_res_e
    (*app_lib_data_get_max_msg_queuing_time_f)(app_lib_data_qos_e priority,
                                               uint16_t * time_p);

/**
 * The function table returned from \ref app_open_library_f
 */
typedef struct
{
    app_lib_data_set_data_received_cb_f setDataReceivedCb;
    app_lib_data_set_bcast_data_received_cb_f setBcastDataReceivedCb;
    app_lib_data_set_data_sent_cb_f setDataSentCb;
    app_lib_data_set_new_app_config_cb_f setNewAppConfigCb;
    app_lib_data_get_data_max_num_bytes_f getDataMaxNumBytes;
    app_lib_data_get_num_buffers_f getNumBuffers;
    app_lib_data_get_num_free_buffers_f getNumFreeBuffers;
    app_lib_data_send_data_f sendData;
    app_lib_data_allow_reception_f allowReception;
    app_lib_data_read_app_config_f readAppConfig;
    app_lib_data_write_app_config_f writeAppConfig;
    app_lib_data_get_app_config_num_bytes_f getAppConfigNumBytes;
    app_lib_data_reserved_f reserved;
    app_lib_data_set_max_msg_queuing_time_f  setMaxMsgQueuingTime;
    app_lib_data_get_max_msg_queuing_time_f  getMaxMsgQueuingTime;
} app_lib_data_t;

#endif /* APP_LIB_DATA_H_ */
