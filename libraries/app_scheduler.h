/* Copyright 2018 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */
#ifndef _APP_SCHEDULER_H_
#define _APP_SCHEDULER_H_

#include <stdint.h>
#include <stdbool.h>
#include "api.h"

/**
 * \brief   Task callback to be registered
 * \return  Delay before being executed again in ms
 * \note    Return value in ms is aligned on system coarse timesatmp
 *          boundaries that has a 1/128s granularity. So asking 3 or 7 ms will
 *          result in same scheduling.
 *          If a better accuracy is needed, hardware timer must be used
 */
typedef uint32_t (*task_cb_f)();

/**
 * \brief   Value to return from task to remove it
 */
#define APP_SCHEDULER_STOP_TASK     ((uint32_t)(-1))

/**
 * \brief   Value to return from task or as initial time to be executed ASAP
 */
#define APP_SCHEDULER_SCHEDULE_ASAP (0)

/**
 * \brief   List of return code
 */
typedef enum
{
    /** Operation is successful */
    APP_SCHEDULER_RES_OK = 0,
    /** No more tasks available */
    APP_SCHEDULER_RES_NO_MORE_TASK = 1,
    /** Trying to cancel a task that doesn't exist */
    APP_SCHEDULER_RES_UNKNOWN_TASK = 2
} app_scheduler_res_e;

/**
 * \brief   Initialize scheduler
 *
 * Example on use:
 * @code
 * void App_init(const app_global_functions_t * functions)
 * {
 *     // Open public API
 *     API_Open(functions);
 *     ...
 *     App_Scheduler_init();
 *     ...
 *     // Start the stack
 *     lib_state->startStack();
 * }
 * @endcode
 *
 * \note    If App scheduler is used in application, the periodicWork offered
 *          by system library MUST NOT be used outside of this module
 */
void App_Scheduler_init(void);

/**
 * \brief   Add a task
 *
 * Example on use:
 *
 * @code
 *
 * static uint32_t periodic_task_50ms()
 * {
 *     ...
 *     return 50;
 * }
 *
 * static uint32_t periodic_task_500ms()
 * {
 *     ...
 *     return 500;
 * }
 *
 * void App_init(const app_global_functions_t * functions)
 * {
 *     // Open public API
 *     API_Open(functions);
 *     ...
 *     App_Scheduler_init();
 *     // Launch two periodic task with different period
 *     App_Scheduler_addTask(periodic_task_50ms, APP_SCHEDULER_SCHEDULE_ASAP);
 *     App_Scheduler_addTask(periodic_task_500ms, APP_SCHEDULER_SCHEDULE_ASAP);
 *     ...
 *     // Start the stack
 *     lib_state->startStack();
 * }
 * @endcode
 *
 *
 * \param   cb
 *          Callback to be called from main periodic task.
 *          Same cb can only be added once. Calling this function with an already
 *          registered cb will update the next scheduled time.
 * \param   delay_ms
 *          delay in ms to be scheduled (0 to be scheduled asap)
 * \return  True if able to add, false otherwise
 */
app_scheduler_res_e App_Scheduler_addTask(task_cb_f cb, uint32_t delay_ms);

/**
 * \brief   Cancel a task
 * \param   cb
 *          Callback already registered from App_Scheduler_addTask.
 * \return  True if able to cancel, false otherwise (not existing)
 */
app_scheduler_res_e App_Scheduler_cancelTask(task_cb_f cb);

#endif //_APP_SCHEDULER_H_
