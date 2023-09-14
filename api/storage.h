/* Copyright 2017 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

/**
 * \file storage.h
 *
 * A small number of bytes is reserved for the application in the same area
 * where the stack firmware stores its own persistent settings (the Settings
 * library, \ref settings.h). The Storage library can be used to read and write
 * this data. Currently, 32 bytes is reserved for the application.
 */
#ifndef APP_LIB_STORAGE_H_
#define APP_LIB_STORAGE_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "app/app.h"

/**
 * This is the library name for use with \ref
 * app_global_functions_t.openLibrary */
#define APP_LIB_STORAGE_NAME 0x7713d6af //!< "STORAG"

/**
 * This is the library version for use with \ref
 * app_global_functions_t.openLibrary */
#define APP_LIB_STORAGE_VERSION 0x200

/**
 * Write new data to the persistent area. All old data is erased, even if new
 * data does not extend to the end of the end of the persistent area.
 * \param   bytes
 *          Pointer to bytes to write
 * \param   num_bytes
 *          Total number of bytes to write
 * \return  Result code, \ref APP_RES_OK if successful
 *          If size is too large or data is NULL, \ref APP_RES_INVALID_VALUE
 */
typedef app_res_e
    (*app_lib_storage_write_persistent_f)(const void * bytes,
                                          size_t num_bytes);

/**
 * \brief   Service to read generic data written to persistent storage
 * \param   bytes
 *          Pointer to buffer for reading bytes
 * \param   num_bytes
 *          Total number of bytes to read
 * \return  Result code, \ref APP_RES_OK if successful
 *          If size is too large or data is NULL, \ref APP_RES_INVALID_VALUE
 */
typedef app_res_e
    (*app_lib_storage_read_persistent_f)(void * bytes,
                                         size_t num_bytes);

/**
 * Return the size of persistent area reserved for application, in bytes.
 * Currently, returns 32.
 * \return  The maximum size in bytes of the storage area
 */
typedef size_t
    (*app_lib_storage_get_persistent_max_size_f)(void);

/**
 * The function table returned from \ref app_open_library_f
 */
typedef struct
{
    app_lib_storage_write_persistent_f          writePersistent;
    app_lib_storage_read_persistent_f           readPersistent;
    app_lib_storage_get_persistent_max_size_f   getPersistentMaxSize;
} app_lib_storage_t;

#endif /* APP_LIB_STORAGE_H_ */
