/**
 * @file elena_os_nav.h
 * @brief 导航栈
 * @author Sab1e
 * @date 2025-08-16
 * @details
 *
 * # 导航栈
 *
 * ## 简介
 *
 * 用于实现返回上页。
 * 系统级页面不会使用导航栈，只有应用级页面才会启用导航栈。
 * 系统只能存在一个导航栈实例。
 *
 * ## 使用方法
 *
 * 首先需要使用`eos_nav_init()`初始化导航栈，输入参数 Launcher Screen，会返回一个创建好的 Home 页面。
 * 使用`eos_nav_scr_create()`即可在导航栈创建新的页面。
 * 推荐使用`eos_nav_back_clean()`返回上级页面，避免内存泄漏。
 *
 * Home 页面下返回上级会退出应用并返回到 Launcher Screen。
 */

#ifndef ELENA_OS_NAV_H
#define ELENA_OS_NAV_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
#include "elena_os_core.h"
#include "script_engine_core.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

/**
 * @brief 初始化导航栈，并创建一个根页面
 * @param launcher_screen 启动页面，触发栈启动的页面，当栈清空时返回启动页面
 * @return lv_obj_t* 返回创建的根页面
 * @note 此 scr 将会放在栈底，作为根页面（root screen），设置后无法修改。
 */
lv_obj_t *eos_nav_init(lv_obj_t *launcher_screen);
/**
 * @brief 创建新页面并压入导航栈
 * @return lv_obj_t* 创建成功则返回 scr 指针，失败则返回 NULL
 */
lv_obj_t *eos_nav_scr_create(void);
/**
 * @brief 清理栈内的所有页面
 */
eos_result_t eos_nav_clean_up(void);
/**
 * @brief 返回上一页面并立即销毁 screen 对象
 */
eos_result_t eos_nav_back_clean(void);
/**
 * @brief 仅返回上一级，不销毁 screen 对象
 * @warning 在不需要 screen 时，需要手动调用`lv_obj_delete`清除 screen，否则可能导致内存泄漏。
 */
eos_result_t eos_nav_back(void);
/**
 * @brief 获取当前根页面
 * @return lv_obj_t* 根页面对象
 */
lv_obj_t *eos_nav_get_home_screen(void);

#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_NAV_H */
