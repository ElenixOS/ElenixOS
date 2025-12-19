/**
 * @file elena_os_vibrator.h
 * @brief 震动器驱动
 * @author Sab1e
 * @date 2025-12-10
 */

#ifndef ELENA_OS_VIBRATOR_H
#define ELENA_OS_VIBRATOR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "elena_os_port_vibrator.h"
#include "lvgl.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/
typedef enum{
    EOS_VIBRATOR_STRENGTH_OFF = 0,
    EOS_VIBRATOR_STRENGTH_NORMAL = 128,
    EOS_VIBRATOR_STRENGTH_INTENSE = 255,
} eos_vibrator_strength_t;
/* Public function prototypes --------------------------------*/

/**
 * @brief 短而轻的滴答震动
 */
void eos_vibrator_tick(void);
/**
 * @brief 稍强且时间长的震动
 */
void eos_vibrator_buzz(void);
/**
 * @brief 震动最强且时间最长的震动
 */
void eos_vibrator_vibrate_long(void);
/**
 * @brief 震动器初始化
 */
void eos_vibrator_init(void);
/**
 * @brief 设置全局震动强度
 */
void eos_vibrator_set_strength(eos_vibrator_strength_t s);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_VIBRATOR_H */
