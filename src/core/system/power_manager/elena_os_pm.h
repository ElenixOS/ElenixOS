/**
 * @file elena_os_pm.h
 * @brief 电源管理器（Power Manager）
 * @author Sab1e
 * @date 2025-11-24
 */

#ifndef ELENA_OS_PM_H
#define ELENA_OS_PM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/
typedef enum
{
    EOS_PM_DISPLAY_ON,      /**< 唤醒模式 */
    EOS_PM_DISPLAY_AOD,     /**< 屏幕常亮（Always-On Display）模式 */
    EOS_PM_SLEEP,           /**< 睡眠模式 */
} eos_pm_state_t;
/* Public function prototypes --------------------------------*/

/**
 * @brief 初始化电源管理器
 */
void eos_pm_init(void);

/**
 * @brief 唤醒设备
 */
void eos_pm_wake_up(void);

/**
 * @brief 设置睡眠超时时间
 * @param sec 时长，单位：秒
 */
void eos_pm_set_sleep_timeout(uint32_t sec);

/**
 * @brief 请求进入睡眠模式（如果 AOD 启用，则进入 AOD 模式）
 */
void eos_pm_request_sleep(void);

/**
 * @brief 重置定时器
 */
void eos_pm_reset_timer(void);

/**
 * @brief 获取电源管理器状态
 */
eos_pm_state_t eos_pm_get_state(void);

/**
 * @brief 设置 AOD 模式
 * @param enable true = 启用 AOD 模式，flase = 关闭 AOD 模式
 */
void eos_pm_set_aod_mode(bool enable);

#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_PM_H */
