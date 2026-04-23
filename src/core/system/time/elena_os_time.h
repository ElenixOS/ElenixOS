/**
 * @file elena_os_time.h
 * @brief System time base
 * @author Sab1e
 * @date 2025-12-18
 */

#ifndef ELENA_OS_TIME_H
#define ELENA_OS_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/**
 * @brief Date time structure definition
 */
typedef struct
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint16_t ms;
    uint8_t day_of_week;    /**< Value range [0,6] */
} eos_datetime_t;

/* Public function prototypes --------------------------------*/

/**
 * @brief Get system time
 * @note
 * `eos_time_get_core()` is responsible for "what second is it now"
 *
 * `eos_time_get()` is responsible for "how many milliseconds have passed in this second"
 */
eos_datetime_t eos_time_get(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_TIME_H */