/**
 * @file elena_os_time.h
 * @brief 系统时间基准
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
 * @brief 时间结构体定义
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
    uint8_t day_of_week;    /**< 取值范围[0,6] */
} eos_datetime_t;

/* Public function prototypes --------------------------------*/

/**
 * @brief 获取系统时间
 * @note
 * `eos_time_get_core()`负责“现在是第几秒”
 *
 * `eos_time_get()`负责“这一秒已经过去了多少毫秒”
 */
eos_datetime_t eos_time_get(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_TIME_H */
