/* Copyright 2017 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include "api.h"

/* All the libraries */
const app_global_functions_t *  global_func;
const app_lib_data_t *          lib_data;
const app_lib_otap_t *          lib_otap;
#ifdef WITH_FRAG
//* For the future. Not corrently supported
/** The SDU library */
const app_lib_sdu_t *           lib_sdu;
#endif
const app_lib_settings_t *      lib_settings;
const app_lib_state_t *         lib_state;
const app_lib_storage_t *       lib_storage;
const app_lib_system_t *        lib_system;
const app_lib_time_t *          lib_time;
const app_lib_hardware_t *      lib_hw;
#ifdef WITH_BEACON
const app_lib_beacon_tx_t *     lib_beacon_tx;
const app_lib_beacon_rx_t *     lib_beacon_rx;
#endif
const app_lib_sleep_t *         lib_sleep;
const app_lib_memory_area_t *   lib_memory_area;

bool API_Open(const app_global_functions_t * functions)
{
    // The root library
    global_func = functions;

    // Open the libraries
    lib_data = functions->openLibrary(APP_LIB_DATA_NAME,
                                      APP_LIB_DATA_VERSION);

    lib_settings = functions->openLibrary(APP_LIB_SETTINGS_NAME,
                                          APP_LIB_SETTINGS_VERSION);

    lib_state = functions->openLibrary(APP_LIB_STATE_NAME,
                                       APP_LIB_STATE_VERSION);

    lib_system = functions->openLibrary(APP_LIB_SYSTEM_NAME,
                                        APP_LIB_SYSTEM_VERSION);

    lib_time = functions->openLibrary(APP_LIB_TIME_NAME,
                                      APP_LIB_TIME_VERSION);

    lib_hw = functions->openLibrary(APP_LIB_HARDWARE_NAME,
                                    APP_LIB_HARDWARE_VERSION);

    lib_storage = functions->openLibrary(APP_LIB_STORAGE_NAME,
                                         APP_LIB_STORAGE_VERSION);

    lib_otap = functions->openLibrary(APP_LIB_OTAP_NAME,
                                      APP_LIB_OTAP_VERSION);

#ifdef WITH_BEACON
    lib_beacon_tx = functions->openLibrary(APP_LIB_BEACON_TX_NAME,
                                           APP_LIB_BEACON_TX_VERSION);

    lib_beacon_rx = functions->openLibrary(APP_LIB_BEACON_RX_NAME,
                                           APP_LIB_BEACON_RX_VERSION);
#endif

    lib_sleep = functions->openLibrary(APP_LIB_LONGSLEEP_NAME,
                                       APP_LIB_LONGSLEEP_VERSION);

   lib_memory_area = functions->openLibrary(APP_LIB_MEMORY_AREA_NAME,
                                            APP_LIB_MEMORY_AREA_VERSION);

    // Currently the status is a success if all libs are opened
    // TODO: add an api.mk / config file that tells which libraries are to be
    // opened and if a library fails to open if it should be handled as an
    // error or not
    return (lib_data &&
            lib_settings &&
            lib_state &&
            lib_system &&
            lib_time &&
            lib_hw &&
            lib_storage &&
#ifdef WITH_BEACON
            lib_beacon_tx &&
            lib_beacon_rx &&
#endif
            lib_otap &&
            lib_sleep &&
            lib_memory_area);
}
