/**
 * @file elena_os_swipe_panel.h
 * @brief 滑动面板头文件
 * @author Sab1e
 * @date 2025-08-10
 * @details
 *
 * # Swipe Panel
 *
 * ## 简述
 *
 * 滑动面板为单向滑动面板，用于将`swipe_obj`从屏幕外面划入屏幕，支持触摸划入以及函数控制划入，并带有滑动动画。
 *
 * ## 使用方法
 *
 * 使用`eos_swipe_panel_create()`即可创建一个新的滑动面板。
 *
 * ## 注意
 *
 * 此代码为`slide_widget`的功能封装。
 */

#ifndef ELENA_OS_SWIPE_PANEL_H
#define ELENA_OS_SWIPE_PANEL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "elena_os_core.h"
#include "lvgl.h"
#include "elena_os_slide_widget.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/
/**
 * @brief SwipePanel 拖拽的方向
 */
typedef enum{
    EOS_SWIPE_DIR_UP=0,      // 从下往上滑拉出 swipe_obj
    EOS_SWIPE_DIR_DOWN=1,    // 从上往下滑拉出 swipe_obj
    EOS_SWIPE_DIR_LEFT=2,    // 从右往左滑拉出 swipe_obj
    EOS_SWIPE_DIR_RIGHT=3    // 从左往右滑拉出 swipe_obj
} eos_swipe_dir_t;
/**
 * @brief SwipePanel 结构体定义
 */
typedef struct {
    eos_slide_widget_t *sw;
    lv_obj_t *swipe_obj;
    lv_obj_t *handle_bar;
    eos_swipe_dir_t dir;
} eos_swipe_panel_t;
/* Public function prototypes --------------------------------*/

/**
 * @brief 滑动面板拉入屏幕
 * @param sp 目标滑动面板
 */
void eos_swipe_panel_slide_down(eos_swipe_panel_t *sp);
/**
 * @brief 滑动面板拉出屏幕
 * @param sp 目标滑动面板
 */
void eos_swipe_panel_slide_up(eos_swipe_panel_t *sp);
/**
 * @brief 删除 SwipePanel
 * @param swipe_panel 滑动面板
 */
void eos_swipe_panel_delete(eos_swipe_panel_t *swipe_panel);

/**
 * @brief 创建 SwipePanel
 * @param parent 滑动面板的父级对象
 * @return 指向创建成功的 SwipePanel
 * @note 不使用时需要使用 eos_swipe_panel_delete 删除此控件，否则可能内存泄漏
 */
eos_swipe_panel_t *eos_swipe_panel_create(lv_obj_t *parent);

/**
 * @brief 设置拖拽方向
 * @param swipe_panel 滑动面板
 * @param dir 拖拽方向，例如 EOS_SWIPE_DIR_DOWN 就是向下拖拽拉出 swipe_obj
 */
void eos_swipe_panel_set_dir(eos_swipe_panel_t *swipe_panel, const eos_swipe_dir_t dir);

/**
 * @brief 隐藏 HandleBar (小白条)
 * @param swipe_panel 滑动面板
 */
void eos_swipe_panel_hide_handle_bar(eos_swipe_panel_t *swipe_panel);

/**
 * @brief 显示 HandleBar (小白条)
 * @param swipe_panel 滑动面板
 */
void eos_swipe_panel_show_handle_bar(eos_swipe_panel_t *swipe_panel);

/**
 * @brief 外部触发拉回动画，自动拉回屏幕外面
 * @param swipe_panel 滑动面板
 */
void eos_swipe_panel_pull_back(eos_swipe_panel_t *swipe_panel);

/**
 * @brief 触发拉动动画，将滑动面板拉到指定像素位置（绝对坐标）
 *
 * 滑动面板为水平方向时，target 控制的是x轴坐标
 *
 * 滑动面板为垂直方向时，target 控制的是y轴坐标
 * @param swipe_panel 滑动面板
 * @param target 目标位置，单位像素
 * @param anim 是否启用动画
 */
void eos_swipe_panel_move(eos_swipe_panel_t *swipe_panel, int32_t target, bool anim);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_SWIPE_PANEL_H */
