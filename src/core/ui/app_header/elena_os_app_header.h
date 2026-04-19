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
#include "elena_os_activity.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

/**
 * @brief 隐藏应用头
 */
void eos_app_header_hide(void);
/**
 * @brief 显示应用头
 * @param a 要显示应用头的活动
 */
void eos_app_header_show(eos_activity_t *a);
/**
 * @brief 动画显示或隐藏应用头
 * @param a 目标活动（显示时用于刷新标题与颜色，隐藏时可传NULL）
 * @param visible 是否显示
 * @param duration_ms 动画时长（毫秒），为0时立即切换
 */
void eos_app_header_set_visible_animated(eos_activity_t *a, bool visible, uint32_t duration_ms);
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
 * @brief 判断应用头当前是否可见
 */
bool eos_app_header_is_visible(void);
/**
 * @brief 附加app header到指定View
 * @param view 要附加的View
 */
void eos_app_header_attach_to_view(lv_obj_t *view);
/**
 * @brief 从View中分离app header，恢复到原始父对象
 */
void eos_app_header_detach_from_view(void);
/**
 * @brief 播放标题变化动画
 * @param from 从哪个activity切换
 * @param to 切换到哪个activity
 * @param need_anim 是否需要动画
 * @param reverse_anim 是否反向执行动画
 */
void _play_title_changed_anim(eos_activity_t *from, eos_activity_t *to, bool need_anim, bool reverse_anim);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_APP_HEADER_H */
