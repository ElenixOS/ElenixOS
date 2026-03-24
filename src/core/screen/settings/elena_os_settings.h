/**
 * @file elena_os_settings.h
 * @brief 系统设置
 * @author Sab1e
 * @date 2025-09-24
 */

#ifndef ELENA_OS_SETTINGS_H
#define ELENA_OS_SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "elena_os_core.h"

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

/**
 * @brief 进入设置页面
 * @return eos_activity_t* 设置页面活动指针，失败时返回 NULL
 */
void eos_settings_enter(void);
/**
 * @brief 启用静音模式
 */
void eos_settings_slient_mode_on(void);
/**
 * @brief 关闭静音模式
 */
void eos_settings_slient_mode_off(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_SETTINGS_H */
