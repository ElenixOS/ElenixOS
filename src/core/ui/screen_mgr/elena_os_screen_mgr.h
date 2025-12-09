/**
 * @file elena_os_screen_mgr.h
 * @brief Screen 管理器
 * @author Sab1e
 * @date 2025-12-02
 */

#ifndef ELENA_OS_SCREEN_MGR_H
#define ELENA_OS_SCREEN_MGR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
/* Public macros ----------------------------------------------*/
#define EOS_ANIM_PLAY_COEFFICIENT 1
#define EOS_APP_LIST_ENTER_APP_ANIM_DELAY 100 * EOS_ANIM_PLAY_COEFFICIENT
#define EOS_SCREEN_ENTER_APP_ANIM_DURATION 200 * EOS_ANIM_PLAY_COEFFICIENT
/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

/**
 * @brief 设置上一次按下的点
 */
void eos_screen_set_last_clicked_point(lv_coord_t x, lv_coord_t y);
/**
 * @brief 获取当前活跃的屏幕
 *
 * 如果设置了`lv_screen_load_anim()`，则返回动画执行的目标屏幕；
 * 否则直接返回当前活跃屏幕。
 *
 * 此函数用于解决`lv_screen_load_anim()`动画完成前获取的屏幕是旧屏幕的问题。
 * @return lv_obj_t* 当前活跃的屏幕对象
 */
lv_obj_t *eos_screen_active(void);
/**
 * @brief 加载屏幕并广播全局加载事件
 * @param scr 要加载的屏幕
 */
void eos_screen_load(lv_obj_t *scr);
/**
 * @brief 创建新的`screen`并设置默认style
 * @return lv_obj_t*
 */
lv_obj_t *eos_screen_create(void);

#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_SCREEN_MGR_H */
