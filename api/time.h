/* Copyright 2017 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

/**
 * \file time.h
 *
 * The Time library provides functions for keeping track of time, in various
 * levels of granularity. It also has functions to do time arithmetic and
 * comparisons with a high-precision timestamp type that is specific to each
 * platform.
 */
#ifndef APP_LIB_TIME_H_
#define APP_LIB_TIME_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * This is the library name for use with \ref
 * app_global_functions_t.openLibrary */
#define APP_LIB_TIME_NAME 0x0013c24d //!< "TIME"

/** \brief Supported library version */
#define APP_LIB_TIME_VERSION 0x200

/**
 * Highest-precision timestamp type available on the platform. As implementation
 * of this timestamp depends on the platform, direct manipulation of the
 * timestamp value in the application is not possible. Instead, functions for
 * arithmetic and comparisons are provided in this library.
 */
typedef uint32_t app_lib_time_timestamp_hp_t;

/**
 * The coarse timestamp type, with a resolution of 1 / 128 s. This timestamp
 * resolution is used e.g. by the Data library for reporting buffering and
 * end-to-end delays.
 */
typedef uint32_t app_lib_time_timestamp_coarse_t;

/**
 * Get current time as a high-precision timestamp. The time starts counting from
 * zero when the node starts up.
 *
 * \return  a timestamp with high precision
 * \note    This timestamp is the highest precision you can have
 *          on the platform. It doesn't have unity and must be handled
 *          with associated services.
 */
typedef app_lib_time_timestamp_hp_t
    (*app_lib_time_get_timestamp_hp_f)(void);

/**
 * Get current time as a coarse timestamp. The value starts counting from zero
 * when the node starts up and wraps back to zero in about 388 days.
 *
 * \return  A coarse timestamp
 */
typedef app_lib_time_timestamp_coarse_t
    (*app_lib_time_get_timestamp_coarse_f)(void);

/**
 * Get current time as a number of seconds since the node started up. The wrap
 * cycle is long enough (136 years) to be of no concern.
 *
 * \return  the current timestamp in s
 */
typedef uint32_t
    (*app_lib_time_get_timestamp_s_f)(void);

/**
 * Add a given number of microseconds (parameter \p time_to_add_us) to a
 * high-precision timestamp base. As the high-precision timestamp \ref
 * app_lib_time_timestamp_hp_t implementation depends on the platform, all
 * arithmetic and comparisons must be done using functions in this library.
 *
 * \param   base
 *          The base timestamp
 * \param   time_to_add_us
 *          The time to add in us
 * \return  The new timestamp
 */
typedef app_lib_time_timestamp_hp_t
    (*app_lib_time_add_us_to_timestamp_hp_f)(app_lib_time_timestamp_hp_t base,
                                             uint32_t time_to_add_us);

/**
 * \brief   Compare two high-precision timestamps
 * \param   time1
 *          The first timestamp
 * \param   time2
 *          The second timestamp
 * \return  true if \p time1 is before \p time2
 * \note    High-precision timestamps have limited range. When two
 *          high-precision timestamps are too far apart in time, this function
 *          no longer gives correct results. Function \ref
 *          app_lib_time_get_max_delay_hp_us_f "lib_time->getMaxHpDelay"() can
 *          be used to query the maximum time between two high-precision
 *          timestamps that can be used in arithmetic and comparisons.
 */
typedef bool
    (*app_lib_time_is_timestamp_hp_before_f)(app_lib_time_timestamp_hp_t time1,
                                             app_lib_time_timestamp_hp_t time2);

/**
 * Calculate and return the difference between two high-precision timestamps,
 * in microseconds.
 *
 * \param   time1
 *          The first timestamp
 * \param   time2
 *          The second timestamp
 * \return  The time in us between time1 and time2
 * \note    High-precision timestamps have limited range. When two
 *          high-precision timestamps are too far apart in time, this function
 *          no longer gives correct results. Function \ref
 *          app_lib_time_get_max_delay_hp_us_f "lib_time->getMaxHpDelay"() can
 *          be used to query the maximum time between two high-precision
 *          timestamps that can be used in arithmetic and comparisons.
 */
typedef uint32_t
    (*app_lib_time_get_time_difference_us_f)(app_lib_time_timestamp_hp_t time1,
                                             app_lib_time_timestamp_hp_t time2);

/**
 * Return the maximum time between two high-precision timestamps that can be
 * used in arithmetic and comparisons functions \ref
 * app_lib_time_get_timestamp_hp_f "lib_time->getTimestampHp", \ref
 * app_lib_time_is_timestamp_hp_before_f "lib_time->isHpTimestampBefore" and
 * \ref app_lib_time_get_time_difference_us_f "lib_time->getTimeDiffUs".
 *
 * \return  The maximun possible time in us for timestamps
 *          comparison or time difference
 * \note    if this time is not enough, consider using the
 *          coarse or second timestamps instead
 */
typedef uint32_t
    (*app_lib_time_get_max_delay_hp_us_f)(void);

/**
 * The function table returned from \ref app_open_library_f
 */
typedef struct
{
    app_lib_time_get_timestamp_hp_f         getTimestampHp;
    app_lib_time_get_timestamp_coarse_f     getTimestampCoarse;
    app_lib_time_get_timestamp_s_f          getTimestampS;
    app_lib_time_add_us_to_timestamp_hp_f   addUsToHpTimestamp;
    app_lib_time_is_timestamp_hp_before_f   isHpTimestampBefore;
    app_lib_time_get_time_difference_us_f   getTimeDiffUs;
    app_lib_time_get_max_delay_hp_us_f      getMaxHpDelay;
} app_lib_time_t;

#endif /* APP_LIB_TIME_H_ */
