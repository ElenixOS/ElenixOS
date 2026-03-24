/**
 * @file elena_os_flash_light.h
 * @brief 手电筒
 * @author Sab1e
 * @date 2025-10-14
 */

#ifndef ELENA_OS_FLASH_LIGHT_H
#define ELENA_OS_FLASH_LIGHT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

/**
 * @brief 快捷显示手电筒，主要用于`control_center`。
 */
void eos_flash_light_show(void);
/**
 * @brief 进入手电筒页面，用于应用列表。
 */
void eos_flash_light_enter(void);

#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_FLASH_LIGHT_H */
