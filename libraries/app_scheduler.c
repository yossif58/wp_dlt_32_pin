/* Copyright 2018 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */
#include "app_scheduler.h"
#include "util.h"

#include <string.h>

/**
 *  Time needed to execute the scheduler in us
 *  It must be set to the longest execution time of tasks
 */
#ifndef APP_SCHEDULER_MAX_EXEC_TIME_US
// Must be defined from application
#error "Please define APP_SCHEDULER_MAX_EXEC_TIME_US from your application makefile"
#endif

/**
 * Maximum periodic task that can be registered at the same time
 * It is application specific
 */
#ifndef APP_SCHEDULER_MAX_TASKS
// Must be defined from application
#error "Please define APP_SCHEDULER_MAX_TASKS from your application makefile"
#endif

/**
 * Maximum time in ms for periodic work  to be scheduled due to internal
 * stack maximum delay comparison (~30 minutes). Computed in init function
 */
static uint32_t m_max_time_ms;

/** Structure of a task */
typedef struct
{
    task_cb_f                           func;
    app_lib_time_timestamp_coarse_t     next_ts;
} task_t;


/**  List of tasks */
static task_t m_tasks[APP_SCHEDULER_MAX_TASKS];

/**
 * \brief   Get a coarse timestamp in future
 * \param   ms
 *          In how many ms is the timestamp in future
 * \return  The computed coarse timestamp
 * \note    Timestamp is rounded up. Ie, 1 ms is 1 coarse period
 *          in future
 */
static app_lib_time_timestamp_coarse_t get_timestamp(uint32_t ms)
{
    app_lib_time_timestamp_coarse_t ts;

    // Initialize timestamp to now
    ts = lib_time->getTimestampCoarse();

    ts += (ms * 128 / 1000);

    // Ceil the value to upper boundary
    // (so in 1ms => ~7.8ms)
    if ((ms * 128) % 1000)
    {
        ts +=1;
    }

    return ts;
}

/**
 * \brief   Execute the first ready task
 */
static void perform_single_task()
{
    app_lib_time_timestamp_coarse_t now = lib_time->getTimestampCoarse();
    task_t * task = NULL;
    uint32_t next = APP_SCHEDULER_STOP_TASK;

    for (uint8_t i = 0; i < APP_SCHEDULER_MAX_TASKS; i++)
    {
        if (m_tasks[i].func != NULL &&
            Util_isGtOrEqUint32(now, m_tasks[i].next_ts))
        {
            // We have a task to execute
            task = &m_tasks[i];
            break;
        }
    }

    if (task == NULL)
    {
        // Nothing to execute
        return;
    }

    // Execute the task
    next = task->func();

    if (next == APP_SCHEDULER_STOP_TASK)
    {
        task->func = NULL;
    }
    else
    {
        // Compute next execution time
        task->next_ts = get_timestamp(next);
    }
}

/**
 * \brief   Get the next coarse timestamp for execution
 * \return  Next execution timestamp
 */
static app_lib_time_timestamp_coarse_t compute_next_execution()
{
    // Initialize next schedule to maximum time without scheduling
    app_lib_time_timestamp_coarse_t next = get_timestamp(m_max_time_ms);

    for (uint8_t i = 0; i < APP_SCHEDULER_MAX_TASKS; i++)
    {
        if (m_tasks[i].func != NULL &&
            Util_isLtUint32(m_tasks[i].next_ts, next))
        {
            // Update next
            next = m_tasks[i].next_ts;
        }
    }

    return next;
}

/**
 * \brief   Periodic callback that will run every tick or when an
 *          asynchronous action is added
 */
static uint32_t periodic_work(void)
{
    app_lib_time_timestamp_coarse_t next, now;
    uint32_t delay_ms;

    now = lib_time->getTimestampCoarse();

    // Execute one task
    perform_single_task();

    // Compute next timestamp time in coarse value
    next = compute_next_execution();

    // Convert it to ms delay
    if (Util_isGtUint32(next, now))
    {
        delay_ms = ((next - now) * 1000) / 128;
    }
    else
    {
        // Another task is ready, schedule ASAP
        delay_ms = 0;
    }

    // Return time in us
    return delay_ms * 1000;
}

static bool add_task_to_table(task_t * task_p)
{
    bool res = false;
    task_t * first_free = NULL;

    // Under critical section to avoid writing the same task
    Sys_enterCriticalSection();
    for (uint8_t i = 0; i < APP_SCHEDULER_MAX_TASKS; i++)
    {
        // First check if task already exist
        if (m_tasks[i].func == task_p->func)
        {
            // Task found, just update the next timestamp and exit
            m_tasks[i].next_ts = task_p->next_ts;
            res = true;
            break;
        }

        // Check for first free room in case task is not founded
        if (m_tasks[i].func == NULL && first_free == NULL)
        {
            first_free = &m_tasks[i];
        }
    }

    if (!res && first_free != NULL)
    {
        memcpy(first_free, task_p, sizeof(task_t));
        res = true;
    }

    Sys_exitCriticalSection();
    return res;
}

static bool remove_task_from_table(task_cb_f cb)
{
    bool res = false;

    Sys_enterCriticalSection();
    for (uint8_t i = 0; i < APP_SCHEDULER_MAX_TASKS; i++)
    {
        if (m_tasks[i].func == cb)
        {
            m_tasks[i].func = NULL;
            res = true;
            break;
        }
    }

    Sys_exitCriticalSection();

    return res;
}

/**
 * Initialization of scheduler
 */
void App_Scheduler_init()
{
    // Maximum time to postpone the periodic work
    m_max_time_ms = lib_time->getMaxHpDelay() / 1000;

    for (uint8_t i = 0; i < APP_SCHEDULER_MAX_TASKS; i++)
    {
        m_tasks[i].func = NULL;
    }
}

app_scheduler_res_e App_Scheduler_addTask(task_cb_f cb, uint32_t delay_ms)
{
    task_t new_task;
    new_task.func = cb;
    new_task.next_ts = get_timestamp(delay_ms);

    if (!add_task_to_table(&new_task))
    {
        return APP_SCHEDULER_RES_NO_MORE_TASK;
    }

    // Re-Schedule periodic task asap to update next execution
    lib_system->setPeriodicCb(periodic_work,
                              0,
                              APP_SCHEDULER_MAX_EXEC_TIME_US);
    return APP_SCHEDULER_RES_OK;
}

app_scheduler_res_e App_Scheduler_cancelTask(task_cb_f cb)
{
    if (remove_task_from_table(cb))
    {
        return APP_SCHEDULER_RES_OK;
    }
    else
    {
        return APP_SCHEDULER_RES_UNKNOWN_TASK;
    }
}
