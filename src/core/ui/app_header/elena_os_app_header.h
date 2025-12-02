/**
 * @file elena_os_app_header.h
 * @brief 应用的顶部导航头
 * @author Sab1e
 * @date 2025-11-25
 * @details
 *
 * # 应用头（AppHeader）
 *
 * ## 简介
 *
 * 应用头上方显示应用信息、当前时间以及返回按钮的控件。
 *
 * ## 使用方法
 *
 * 系统启动时使用`eos_app_header_init()`来初始化应用头，只能初始化一次。
 *
 * 可以直接使用`eos_app_header_bind_screen()`或`eos_app_header_bind_screen_str_id()`将当前的 Screen 绑定以显示应用头。
 *
 * >[!NOTE]: 以`_str_id`结尾的函数即输入 ID 以便适配多语言系统。
 *
 * 后续如果想修改应用头的标题，则需要使用`eos_app_header_set_title()`或`eos_app_header_set_title_str_id()`来设置新的标题。
 *
 * 当被绑定的 Screen 删除时，会自动清理资源（字符串）。
 *
 * >[!NOTE]: 应用头标题字符串会自动复制传入的字符串，以防止源字符串被清除。
 */

#ifndef ELENA_OS_APP_HEADER_H
#define ELENA_OS_APP_HEADER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
#include "elena_os_lang.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

void eos_app_header_set_title_anim(lv_obj_t *current_scr, lv_obj_t *next_scr, bool is_anim_entering);
/**
 * @brief 应用头设置标题名称
 * @param scr Screen 对象
 * @param title 标题字符串
 */
void eos_app_header_set_title(lv_obj_t *scr, const char *title);
/**
 * @brief 应用头设置标题名称
 * @param scr Screen 对象
 * @param id 多语言的字符串 ID
 */
void eos_app_header_set_title_str_id(lv_obj_t *scr, language_id_t id);
/**
 * @brief 隐藏应用头
 */
void eos_app_header_hide(void);
/**
 * @brief 显示应用头
 */
void eos_app_header_show(void);
/**
 * @brief 初始化应用头
 *
 * 应用头将被放置在 lv_layer_top() 层中
 *
 * 隐藏应用头请使用`eos_app_header_hide`
 *
 * 显示应用头请使用`eos_app_header_show`
 *
 * @warning 应用头只能初始化一次
 */
void eos_app_header_init(void);
/**
 * @brief 将目标 screen 与应用头绑定，以便 screen 加载时显示应用头，screen 删除时隐藏应用头
 * @param scr 目标应用头
 * @param title 标题 字符串（一般是应用名称），可以通过`eos_app_header_set_title`进行修改
 */
void eos_app_header_bind_screen(lv_obj_t *scr, const char *title);
/**
 * @brief 将目标 screen 与应用头绑定，以便 screen 加载时显示应用头，screen 删除时隐藏应用头
 * @param scr 目标应用头
 * @param id 标题 ID（一般是应用名称），可以通过`eos_app_header_set_title`进行修改
 */
void eos_app_header_bind_screen_str_id(lv_obj_t *scr, lang_string_id_t id);
void eos_app_header_set_parent(lv_obj_t *parent);
bool eos_app_header_is_visible(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_APP_HEADER_H */
