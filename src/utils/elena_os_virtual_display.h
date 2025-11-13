/**
 * @file elena_os_virtual_display.h
 * @brief 虚拟显示器
 * @author Sab1e
 * @date 2025-11-12
 * @details
 *
 * # Virtual Display
 *
 * ## 简述
 *
 * 实现 LVGL 虚拟显示器（Virtual Display），支持在主显示中嵌入独立 LVGL 渲染与交互环境
 *
 * 本模块通过 LVGL 的 Canvas 对象与自定义 Display 驱动结合，
 * 实现一个可点击、可渲染的虚拟显示器，用于在主显示器中嵌套显示其他 LVGL 场景。
 *
 * 功能特性：
 * - 使用 LVGL Canvas 作为渲染目标，显示虚拟显示内容；
 * - 注册自定义 lv_display_t 驱动，处理 LVGL 局部渲染回调；
 * - 注册虚拟输入设备 lv_indev_t，实现 Canvas 区域内的触摸事件；
 * - 自动将全局触摸坐标转换为相对 Canvas 的局部坐标；
 * - 支持 PRESS、PRESSING、RELEASED 等 LVGL 事件；
 * - 可用于模拟器或多层 LVGL 界面系统中的嵌套显示需求。
 *
 * 典型应用场景：
 * - 智能手表模拟器（嵌入虚拟屏幕）；
 * - 多 Display 场景切换或小窗口预览；
 * - LVGL 应用运行时中的独立渲染容器。
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
