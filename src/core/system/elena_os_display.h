/**
 * @file elena_os_display.h
 * @brief 显示器设置
 * @author Sab1e
 * @date 2025-10-23
 */

#ifndef ELENA_OS_DISPLAY_H
#define ELENA_OS_DISPLAY_H

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
 * @brief 设置亮度（平滑过渡）
 * @param start_brightness 起始亮度
 * @param end_brightness 最终亮度
 * @param duration_ms 动画时长（毫秒）
 */
void eos_display_set_brightness_smooth(uint8_t start_brightness, uint8_t end_brightness, uint32_t duration_ms);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_DISPLAY_H */
