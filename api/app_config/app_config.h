/* Copyright 2017 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

#include <stdint.h>

/**
 * In addition to NodeAPI interests, DREQs contain generic app layer
 * configuration data, which is passed to all nodes, just like interests.
 * The stack does not know or care what the actual configuration payload means.
 */

/**
 * size of the header describing the type of app config data that follows
 */
#define ACS_COMMON_SIZE     sizeof(app_config_common_t)

/**
 * Align app config to this size
 * This only matters if there are type casts applied to app config data
 */
#define ACS_ALIGN_SIZE      sizeof(int32_t)

/**
 * used in alignment check
 */
#define ACS_CHECK_SIZE      (int32_t)(ACS_ALIGN_SIZE-ACS_COMMON_SIZE)

/**
 * Max space available for actual app config (e.g. DREQ PDU space after headers)
 * Type casts should be applied only from app_config_storage_t.align onwards
 * But it is not fool proof. It aligns only to ACS_ALIGN_SIZE.
 * So, with these definitions only int32_t (and int64_t) types are quaranteed
 *  to be aligned if performing a cast.
 * 80 bytes reserved for actual app config data in DREQ PDU.
 * !! Note that this is the amount of space reserved from flash storage also
 *     for app_config_t !!
 */
#define ACS_LEN             80

/**
 * used for correct alignment of app_config_storage_t.align
 */
#define ACS_PAD1_SIZE       (ACS_ALIGN_SIZE - ACS_COMMON_SIZE)

/*
 * rest of the space reserved for app config
 */
#define ACS_PAD2_SIZE       ((ACS_LEN - (ACS_COMMON_SIZE +            \
                                         ACS_PAD1_SIZE + ACS_ALIGN_SIZE)))

typedef struct
{
/**
 * Type of the app config data that follows
 */
    uint8_t type;
#define APP_CONFIG_TYPE_NONE    0
#define APP_CONFIG_TYPE_APP     1
#define APP_CONFIG_TYPE_IPV6    2
#define APP_CONFIG_TYPE_END     0xff
/**
 * length of the app config data that follows
 */
    uint8_t len;
} app_config_common_t;
/* static compile time check that alignment is correct (for ACS_ALIGN_SIZE) */
typedef char app_config_common_t_size_vs_ACS_ALIGN_SIZE[ACS_CHECK_SIZE];

/**
 * space reservation for storing app configs
 * type casts will be aligned only from member align onwards to
 *  \see ACS_ALIGN_SIZE
 * But it is best not to use type casts at all
*/
typedef struct
{
    app_config_common_t base;
    uint8_t pad1[ACS_PAD1_SIZE];
    int32_t align;
    uint8_t pad2[ACS_PAD2_SIZE];
} app_config_storage_t;

/**
 * generic base type for all app configs
 * Actual app config will consist of several (packed) app_config_t structs
 * This is the only fool proof type cast that can be applied
 *  and app_config_gen_t.data should really be accessed bytewise
 */
typedef struct
{
    app_config_common_t         base;
    uint8_t                     data[];
} app_config_gen_t;

typedef union
{
    app_config_common_t         common;
    app_config_storage_t        storage;
} app_config_t;

/**
 * \brief   Get specific application data part from app_config_t
 * \param   cfg
 *          App_config_t containing app data segments
 * \param   type
 *          Which type of app data to look for
 * \param   ret
 *          Pointer that is set to data (if found)
 * \return  Size of the app config data, 0 if not found
 */
uint32_t AppConfig_getData(app_config_t const * cfg, uint8_t type,
                           void const ** ret_ptr);

#endif /* APP_CONFIG_H_ */
