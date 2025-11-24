/**
 * @file elena_os_touch.h
 * @brief 获取触摸设备
 * @author Sab1e
 * @date 2025-11-24
 */

#ifndef ELENA_OS_TOUCH_H
#define ELENA_OS_TOUCH_H

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
 * @brief 获取第一个触摸设备
 * @return lv_indev_t* 获取成功则返回触摸设备指针，否则返回 NULL
 */
lv_indev_t *eos_touch_get_indev(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_TOUCH_H */
