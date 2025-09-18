/**
 * @file elena_os_nav.h
 * @brief 导航栈
 * @author Sab1e
 * @date 2025-08-16
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
 * @brief 初始化导航栈
 * @param scr 要初始化的目标 screen
 * @note 此 scr 将会放在栈底，作为根页面（root screen），设置后无法修改。
 */
eos_result_t eos_nav_init(const char *header_title);
/**
 * @brief 创建新页面并压入导航栈
 * @param header_title app_header 的标题
 * @return lv_obj_t* 创建成功则返回 scr 指针，失败则返回 NULL
 */
lv_obj_t *eos_nav_scr_create(const char *header_title);
/**
 * @brief 清理栈内除 root screen 的所有页面
 */
eos_result_t eos_nav_clean_up(void);
/**
 * @brief 返回上一页面并立即销毁 screen 对象
 */
eos_result_t eos_nav_back_clean(void);
/**
 * @brief 仅返回上一级，不销毁 screen 对象
 * @warning 在不需要 screen 时，需要手动调用`lv_obj_del`清除 screen，否则可能导致内存泄漏。
 */
eos_result_t eos_nav_back(void);

#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_NAV_H */
