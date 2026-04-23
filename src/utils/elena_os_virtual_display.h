/**
 * @file elena_os_virtual_display.h
 * @brief 虚拟显示器
 * @author Sab1e
 * @date 2025-11-12
 */
#ifndef ELENA_OS_VIRTUAL_DISPLAY_H
#define ELENA_OS_VIRTUAL_DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include "elena_os_config.h"
#if EOS_USE_VIRTUAL_DISPLAY
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/
typedef struct eos_virtual_display_t;
/* Public function prototypes --------------------------------*/

/**
 * @brief 创建一个虚拟显示器，可以放在 LVGL 对象内。
 * @param parent 父级对象
 * @param hor_res 显示器水平分辨率
 * @param ver_res 显示器垂直分辨率
 * @return lv_display_t* 创建成功则返回显示器对象，否则返回 NULL
 */
lv_display_t *eos_virtual_display_create(lv_obj_t *parent, lv_coord_t hor_res, lv_coord_t ver_res);
#endif /* EOS_USE_VIRTUAL_DISPLAY */

#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_VIRTUAL_DISPLAY_H */
