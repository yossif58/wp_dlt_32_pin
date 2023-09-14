/* Copyright 2017 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

/**
 * \file api.h
 *
 * Implements global API for Wirepas library services
 *
 * One can use the libraries individually, or use this interface that binds
 * the libraries to function like macros for convenient (and safe) use
 */
#ifndef API_H_
#define API_H_

#include "app.h"
#include "data.h"
#include "otap.h"
#include "settings.h"
#include "state.h"
#include "storage.h"
#include "system.h"
#include "time.h"
#include "hardware.h"
// BLE beacons
#ifdef WITH_BEACON
#include "beacon_tx.h"
#include "beacon_rx.h"
#endif
#include "sleep.h"
#include "memory_area.h"
// IPV6
//#include "ipv6.h"
//#include "in6.h"
//#include "socket_errno.h"
//#include "sys_socket.h"

// Expose common APIs
extern const app_global_functions_t *   global_func     __attribute((weak));
extern const app_lib_data_t *           lib_data        __attribute((weak));
extern const app_lib_otap_t *           lib_otap        __attribute((weak));
extern const app_lib_settings_t *       lib_settings    __attribute((weak));
extern const app_lib_state_t *          lib_state       __attribute((weak));
extern const app_lib_storage_t *        lib_storage     __attribute((weak));
extern const app_lib_system_t *         lib_system      __attribute((weak));
extern const app_lib_time_t *           lib_time        __attribute((weak));
extern const app_lib_hardware_t *       lib_hw          __attribute((weak));
// BLE beacons
#ifdef WITH_BEACON
extern const app_lib_beacon_tx_t *      lib_beacon_tx   __attribute((weak));
extern const app_lib_beacon_rx_t *      lib_beacon_rx   __attribute((weak));
#endif
extern const app_lib_sleep_t *          lib_sleep       __attribute((weak));
extern const app_lib_memory_area_t *    lib_memory_area __attribute((weak));

// IPV6
//extern const app_lib_ipv6_t *           lib_ipv6        __attribute((weak));


/**
 * \brief   Open API (open all libraries)
 *
 * This function opens all libraries and stores the handle to those for function
 * pointers (<code>app_lib_<library>_t *</code>). Then, application may
 * continue using those pointers.
 *
 * Example on using this function. After opening, the
 * @ref system.h "system library" can be used by using <code>
 * @ref lib_system </code> handle.
 *
 * @code
 * void App_init(const app_global_functions_t * functions)
 * {
 *     // Open Wirepas public API
 *     API_Open(functions);
 *
 *     ...
 *
 *     // Enable interrupt
 *     lib_system->enableAppIrq(false, TEMP_IRQn, APP_LIB_SYSTEM_IRQ_PRIO_LO, temp_interrupt_handler);
 *
 *     ...
 * }
 * @endcode
 *
 * \param   functions
 *          The root library address passed to \ref app_init App_init()
 * \return  status of operation
 */
bool API_Open(const app_global_functions_t * functions);

// Define error value for functions
#define ERR_NOT_OPEN APP_RES_INVALID_NULL_POINTER

// System library
#define Sys_enterCriticalSection() \
    lib_system ? lib_system->enterCriticalSection() : ERR_NOT_OPEN

#define Sys_exitCriticalSection() \
    lib_system ? lib_system->exitCriticalSection() : ERR_NOT_OPEN

#define Sys_disableDs(_dis) \
    lib_system ? lib_system->disableDeepSleep(_dis) : ERR_NOT_OPEN

#define Sys_enableAppIrq(_irqn, _isr) \
    lib_system ? lib_system->enableAppIrq(false, _irqn, APP_LIB_SYSTEM_IRQ_PRIO_LO, _isr) : ERR_NOT_OPEN

#define Sys_disableAppIrq(_irqn) \
    lib_system ? lib_system->disableAppIrq(_irqn) : ERR_NOT_OPEN

#define Sys_enableFastAppIrq(_irqn, _prio, _isr) \
    lib_system ? lib_system->enableAppIrq(true, _irqn, _prio, _isr) : ERR_NOT_OPEN

#define Sys_clearFastAppIrq(_irqn) \
    lib_system ? lib_system->clearPendingFastAppIrq(_irqn) : ERR_NOT_OPEN

// BLE beacon scanner library
#ifdef WITH_BEACON
#define Ble_beaconRx_setRxCb(_cb) \
    lib_beacon_rx ? lib_beacon_rx->setBeaconReceivedCb(_cb) : ERR_NOT_OPEN

#define Ble_beaconRx_startScanner(_chan) \
    lib_beacon_rx ? lib_beacon_rx->startScanner(_chan) : ERR_NOT_OPEN

#define Ble_beaconRx_stopScanner() \
    lib_beacon_rx ? lib_beacon_rx->stopScanner() : ERR_NOT_OPEN

#define Ble_beaconRx_isStarted() \
    (lib_beacon_rx ? lib_beacon_rx->isScannerStarted() : ERR_NOT_OPEN)
#endif


#endif /* API_H_ */
