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
 * @brief 创建设置页面
 */
void eos_settings_create(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_SETTINGS_H */
