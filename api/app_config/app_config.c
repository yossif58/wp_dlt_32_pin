/* Copyright 2017 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

#include <stdint.h>
#include <stddef.h>
#include "app_config.h"

#define APP_CONFIG_NEXT_ITEM(item_ptr)               \
    ((app_config_common_t const *)&((uint8_t *)(item_ptr))\
    [item_ptr->len + sizeof(app_config_common_t)])

uint32_t AppConfig_getData(app_config_t const * cfg, uint8_t type,
                           void const ** ret_ptr)
{
    app_config_common_t const *acc = &cfg->common;
    uint32_t total_size = 0;
    app_config_gen_t *acg;

    *ret_ptr = NULL;
    while((acc->type != type) && (acc->type != APP_CONFIG_TYPE_END) &&
          (acc->type != APP_CONFIG_TYPE_NONE) &&
          (total_size < sizeof(app_config_t)))
    {
        total_size += (sizeof(app_config_common_t) + acc->len);
        acc = APP_CONFIG_NEXT_ITEM(acc);
    }
    if(acc->type == type)
    {
        acg = (app_config_gen_t *)acc;
        *ret_ptr = acg->data;
        return acg->base.len;
    }
    return 0;
}
