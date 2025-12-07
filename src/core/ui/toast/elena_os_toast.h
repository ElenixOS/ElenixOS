/**
 * @file elena_os_toast.h
 * @brief 临时消息提示框
 * @author Sab1e
 * @date 2025-10-24
 */

#ifndef ELENA_OS_TOAST_H
#define ELENA_OS_TOAST_H

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
 * @brief 显示一个 Toast 提示
 * @param icon_src 图标文件路径，设置为 NULL 时不显示图标
 * @param message 消息内容
 * @return lv_obj_t* 返回创建的 Toast 对象指针
 */
lv_obj_t * eos_toast_show(const char *icon_src, const char *message);
/**
 * @brief 支持格式化字符串的 Toast 提示
 */
lv_obj_t *eos_toast_show_fmt(const char *icon_src, const char *fmt, ...);
/**
 * @brief 初始化 Toast 系统
 */
void eos_toast_init(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_TOAST_H */
