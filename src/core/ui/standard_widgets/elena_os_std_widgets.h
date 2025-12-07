/**
 * @file elena_os_std_widgets.h
 * @brief 系统标准组件
 * @author Sab1e
 * @date 2025-12-07
 */

#ifndef ELENA_OS_STD_WIDGETS_H
#define ELENA_OS_STD_WIDGETS_H

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
 * @brief 创建消息页面
 * @param scr Screen 对象
 * @param icon_bg_color 圆形图标背景色
 * @param icon 图标源
 * @param title_txt 标题文本
 * @param txt 正文文本
 * @param clear_nav 返回按钮是否清理整个导航栈
 * @return lv_obj_t* 返回消息页面容器列表
 */
lv_obj_t *eos_std_info_create(lv_obj_t *scr,
                              lv_color_t icon_bg_color,
                              const char *icon,
                              const char *title_txt,
                              const char *txt,
                              bool clear_nav);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_STD_WIDGETS_H */
