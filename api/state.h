/* Copyright 2017 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

/**
 * \file state.h
 *
 * Application library for viewing and controlling stack runtime state
 */
#ifndef APP_LIB_STATE_H_
#define APP_LIB_STATE_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "app/app.h"

/** \brief Library symbolic name  */
#define APP_LIB_STATE_NAME 0x02f9c165 //!< "STATE"

/**
 * This is the library version for use with \ref
 * app_global_functions_t.openLibrary "*/
#define APP_LIB_STATE_VERSION 0x201

/**
 * \brief   Maximum number of neighbors in a request
 */
#define APP_LIB_STATE_MAX_NUM_NEIGHBORS 8

/**
 * \brief   Neighbor type
 * \note    Most reliable information is always from next hop and members
 *          Other entries might be very old
 */
typedef enum
{
    /** Neighbor is specifically a next hop cluster */
    APP_LIB_STATE_NEIGHBOR_IS_NEXT_HOP  = 0,
    /** Neighbor is specifically a member of this node */
    APP_LIB_STATE_NEIGHBOR_IS_MEMBER    = 1,
    /** Neighbor is synced or cluster from network scan */
    APP_LIB_STATE_NEIGHBOR_IS_CLUSTER   = 2,
} app_lib_state_nbor_type_e;

/**
 * \brief   Scan neighbor type to specify the scans that trigger the callback
 */
typedef enum
{
    /** All scans will trigger the registered callback */
    APP_LIB_STATE_SCAN_NBORS_ALL = 0,
    /** Only explicitely requested scans from app will trigger the registered callback */
    APP_LIB_STATE_SCAN_NBORS_ONLY_REQUESTED = 1,
} app_lib_state_scan_nbors_type_e;

/**
 * \brief   Neighbors info definition
 */
typedef struct
{
    uint32_t address;
    uint16_t last_update;
    uint8_t  link_reliability;
    uint8_t  norm_rssi;
    uint8_t  cost;
    uint8_t  channel;
    uint8_t  type;              /**< \ref app_neighbor_type_e */
    uint8_t  tx_power;
    uint8_t  rx_power;
    uint8_t  pad[3];
} app_lib_state_nbor_info_t;

/**
 * \brief   Neighbors list definition
 */
typedef struct
{
    uint32_t number_nbors;
    app_lib_state_nbor_info_t nbors[APP_LIB_STATE_MAX_NUM_NEIGHBORS];
} app_lib_state_nbor_list_t;

/**
 * \brief   Stack state flags
 */
typedef enum
{
    APP_LIB_STATE_STARTED = 0,                  //!< Stack is started
    APP_LIB_STATE_STOPPED = 1,                  //!< Stack is stopped
    APP_LIB_STATE_NODE_ADDRESS_NOT_SET = 2,     //!< Node address is not set
    APP_LIB_STATE_NETWORK_ADDRESS_NOT_SET = 4,  //!< Network address is not set
    APP_LIB_STATE_NETWORK_CHANNEL_NOT_SET = 8,  //!< Network channel is not set
    APP_LIB_STATE_ROLE_NOT_SET = 16,            //!< Node role is not set
    APP_LIB_STATE_APP_CONFIG_DATA_NOT_SET = 32, //!< App config data is
                                                //!< not set (sink only)
    APP_LIB_STATE_ACCESS_DENIED = 128           //!< Operation is not allowed
} app_lib_state_stack_state_e;

/**
 * \brief   Structure to hold the information about received beacons
 */
typedef struct
{
    app_addr_t  address; //!< Address of the beacon sender
    int16_t     rssi;    //!< rssi in dBm
    int8_t      txpower; //!< tx power in dB
    uint8_t     pad;
} app_lib_state_beacon_rx_t;

/**
 * \brief    Function type for a neighbor scan completion callback
 */
typedef void (*app_lib_state_on_scan_nbors_cb_f)(void);

/**
 * \brief   Function type for a beacon reception callback
 * \param   beacon
 *          Information about received beacon
 */
typedef void (*app_lib_state_on_beacon_cb_f)(const app_lib_state_beacon_rx_t * beacon);

/**
 * \brief   Start the stack
 *
 * This is most commonly used in the end of the @ref app_init "App_init()
 * function to start the radio operation.
 *
 * Example on use:
 *
 * @code
 * void App_init(const app_global_functions_t * functions)
 * {
 *     // Open Wirepas public API
 *     API_Open(functions);
 *
 *     ...
 *
 *     // Start the stack
 *     lib_state->startStack();
 * }
 * @endcode
 *
 * \return  Result code, \ref APP_RES_OK if successful
 */
typedef app_res_e (*app_lib_state_start_stack_f)(void);

/**
 * \brief   Stop the stack
 * \return  Result code, \ref APP_RES_OK if successful
 * \note    Stopping the stack will reboot the system. The system shutdown
 *          callback is called just before rebooting. Node configuration may
 *          be done in the callback. This function never returns.
 */
typedef app_res_e (*app_lib_state_stop_stack_f)(void);

/**
 * \brief   Get the stack state
 * \return  Bit field of stack state, \ref app_lib_state_stack_state_e
 */
typedef uint8_t (*app_lib_state_get_stack_state_f)(void);

/**
 * \brief   Get the number of routes this node has to a sink
 * \param   count_p
 *          Pointer to store the result
 * \return  Result code, \ref APP_RES_OK if successful,
 *          \ref APP_RES_INVALID_STACK_STATE if stack not running
 */
typedef uint8_t (*app_lib_state_get_route_count_f)(size_t * count_p);

/**
 * \brief   Get diagnostics interval
 * \return  Diagnostics interval in seconds
 */
typedef uint16_t (*app_lib_state_get_diag_interval_f)(void);

/**
 * \brief   Get current access cycle
 * \param   ac_value_p
 *          Pointer to store the current access cycle value in milliseconds
 *          Updated if return code is \ref APP_RES_OK
 * \return  Result code, \ref APP_RES_OK if successful,
 *          \ref APP_RES_INVALID_STACK_STATE if stack not running
 */
typedef app_res_e (*app_lib_state_get_access_cycle_f)(uint16_t * ac_value_p);

/**
 * \brief   Set a callback to be called when neighbor scan is complete
 * \param   cb
 *          The function to be executed, or NULL to unset
 * \param   type
 *          The type of scan event app is interested (Unused if cb is NULL)
 * \return  Result code
 */
typedef app_res_e
    (*app_lib_state_set_on_scan_nbors_with_type_cb_f)(app_lib_state_on_scan_nbors_cb_f cb,
                                                      app_lib_state_scan_nbors_type_e type);

/**
 * \brief   Start neighbor scan
 * \return  Result code, always \ref APP_RES_OK
 */
typedef app_res_e (*app_lib_state_start_scan_nbors_f)(void);

/**
 * \brief   Get list of neighbors
 * \param   nbors_list
 *          Pointer to store the list of neighbors
 * \return  Result code, always \ref APP_RES_OK
 */
typedef app_res_e (*app_lib_state_get_nbors_f)(app_lib_state_nbor_list_t * nbors_list);

/**
 * \brief   Set a callback to be called when a beacon is received
 * \param   cb
 *          The function to be executed, or NULL to unset
 * \return  Result code, always \ref APP_RES_OK
 */
typedef app_res_e
    (*app_lib_state_set_on_beacon_cb_f)(app_lib_state_on_beacon_cb_f cb);

/**
 * \brief   Get available energy
 * \param   energy_p
 *          Pointer to store the available energy as a
 *          proportional value from 0 to 255
 * \return  Result code, \ref APP_RES_OK if successful,
 *          \ref APP_RES_INVALID_STACK_STATE if the stack is not running
 *          \ref APP_RES_INVALID_NULL_POINTER if \energy_p is NULL
 */
typedef app_res_e
    (*app_lib_state_get_energy_f)(uint8_t * energy_p);

/**
 * \brief   Set available energy
 * \param   energy
 *          Available energy as a proportional value from 0 to 255
 * \return  Result code, \ref APP_RES_OK if successful,
 *          \ref APP_RES_INVALID_STACK_STATE if the stack is not running
 */
typedef app_res_e
    (*app_lib_state_set_energy_f)(uint8_t energy);

/**
 * \brief   Get sink initial cost
 * \param   cost_p
 *          Pointer to store the current initial cost
 * \return  Result code, \ref APP_RES_OK if successful,
 *          \ref APP_RES_INVALID_CONFIGURATION if node is not a sink,
 *          \ref APP_RES_INVALID_NULL_POINTER if \p cost_p is NULL
 */
typedef app_res_e
    (*app_lib_state_get_sink_cost_f)(uint8_t * cost_p);

/**
 * \brief   Set sink initial cost
 * \param   cost
 *          Initial cost
 * \return  Result code, \ref APP_RES_OK if successful,
 *          \ref APP_RES_INVALID_CONFIGURATION if node is not a sink
 */
typedef app_res_e
    (*app_lib_state_set_sink_cost_f)(const uint8_t cost);

/**
 * \brief   List of library functions of v2 version (0x201)
 */
typedef struct
{
    app_lib_state_start_stack_f                     startStack;
    app_lib_state_stop_stack_f                      stopStack;
    app_lib_state_get_stack_state_f                 getStackState;
    app_lib_state_get_route_count_f                 getRouteCount;
    app_lib_state_get_diag_interval_f               getDiagInterval;
    app_lib_state_get_access_cycle_f                getAccessCycle;
    app_lib_state_set_on_scan_nbors_with_type_cb_f  setOnScanNborsCb;
    app_lib_state_start_scan_nbors_f                startScanNbors;
    app_lib_state_get_nbors_f                       getNbors;
    app_lib_state_set_on_beacon_cb_f                setOnBeaconCb;
    app_lib_state_get_energy_f                      getEnergy;
    app_lib_state_set_energy_f                      setEnergy;
    app_lib_state_get_sink_cost_f                   getSinkCost;
    app_lib_state_set_sink_cost_f                   setSinkCost;
} app_lib_state_t;

#endif /* APP_LIB_STATE_H_ */
