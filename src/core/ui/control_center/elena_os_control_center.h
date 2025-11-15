/**
 * @file elena_os_control_center.h
 * @brief 上拉控制台
 * @author Sab1e
 * @date 2025-09-22
 */

#ifndef ELENA_OS_CONTROL_CENTER_H
#define ELENA_OS_CONTROL_CENTER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
#include "elena_os_swipe_panel.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/
typedef struct
{
    eos_swipe_panel_t *swipe_panel;
    lv_obj_t *container;
    lv_obj_t *bl_btn;
    lv_obj_t *brightness_btn;
    lv_obj_t *bat_btn;
    lv_obj_t *locate_phone_btn;
    lv_obj_t *mute_btn;
    lv_obj_t *volume_btn;
    lv_obj_t *flash_light_btn;
    lv_obj_t *settings_btn;
} eos_control_center_t;
/* Public function prototypes --------------------------------*/

/**
 * @brief 将控制中心拉入屏幕
 */
void eos_control_panel_slide_change(void);
/**
 * @brief 显示控制中心
 */
void eos_control_center_show(void);
/**
 * @brief 隐藏控制中心
 */
void eos_control_center_hide(void);
/**
 * @brief 获取控制中心实例
 * @return eos_control_center_t*
 */
eos_control_center_t *eos_control_center_get_instance(void);
/**
 * @brief 初始化控制中心，创建一个控制中心实例
 */
void eos_control_center_init(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_CONTROL_CENTER_H */
