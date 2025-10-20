/**
 * @file elena_os_slide_widget.h
 * @brief 滑动组件
 * @author Sab1e
 * @date 2025-10-18
 * @details
 *
 * # Slide Widget
 *
 * ## 简述
 *
 * 此组件为最基本的滑动组件，用于将目标对象从base滑动到target。
 * 当触摸长度（垂直方向或水平方向）/(target-base) 超过阈值时，触发滑动动画，自动滑至target，否则自动滑至base。
 *
 */

#ifndef ELENA_OS_SLIDE_WIDGET_H
#define ELENA_OS_SLIDE_WIDGET_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
#include "elena_os_utils.h"

/* Public macros ----------------------------------------------*/
#define EOS_THRESHOLD_SCALE 256
/* Public typedefs --------------------------------------------*/

typedef uint8_t eos_threshold_t;

enum{
    EOS_THRESHOLD_0      = 0,
    EOS_THRESHOLD_10     = 25,
    EOS_THRESHOLD_20     = 51,
    EOS_THRESHOLD_30     = 76,
    EOS_THRESHOLD_40     = 102,
    EOS_THRESHOLD_50     = 127,
    EOS_THRESHOLD_60     = 153,
    EOS_THRESHOLD_70     = 178,
    EOS_THRESHOLD_80     = 204,
    EOS_THRESHOLD_90     = 229,
    EOS_THRESHOLD_100    = 255,
};

typedef enum
{
    EOS_SLIDE_DIR_VER,
    EOS_SLIDE_DIR_HOR,
} eos_slide_widget_dir_t;

typedef enum
{
    EOS_SLIDE_WIDGET_STATE_IDLE = 0,      /**< 空闲，未滑动 */
    EOS_SLIDE_WIDGET_STATE_DRAGGING,  /**< 正在滑动中（手势拖动） */
    EOS_SLIDE_WIDGET_STATE_THRESHOLD, /**< 超过阈值，滑动确认（执行展开/触发操作） */
    EOS_SLIDE_WIDGET_STATE_REVERTING, /**< 正在回弹（未超过阈值自动返回） */
    EOS_SLIDE_WIDGET_STATE_ANIMATING, /**< 手动触发动画 */
} eos_slide_widget_state_t;

typedef struct
{
    lv_obj_t *touch_obj;
    lv_obj_t *target_obj;
    eos_slide_widget_dir_t dir;
    lv_coord_t base;   /**< 基点：目标对象的默认位置 */
    lv_coord_t target; /**< 目标点：目标对象最终要移动到的位置 */
    lv_coord_t touch_obj_base;
    lv_coord_t touch_obj_target;
    eos_threshold_t threshold; /**< 阈值：(当前点 - 基点)/（目标点 - 基点）> threshold时触发阈值 */
    eos_slide_widget_state_t state;
    lv_coord_t _indev_start;    /**< 触摸起始点 */
} eos_slide_widget_t;

/* Public function prototypes --------------------------------*/

/**
 * @brief 从开始位置移动到结束位置
 * @param sw 目标滑动
 * @param start 开始坐标
 * @param end 结束坐标
 * @param duration 持续时间，设置为0则没有动画
 */
void eos_slide_widget_move(eos_slide_widget_t *sw, lv_coord_t start, lv_coord_t end, uint32_t duration);
/**
 * @brief 翻转目标对象的`target`和`base`位置
 * @param sw 目标滑动组件
 */
void eos_slide_widget_reverse(eos_slide_widget_t *sw);
/**
 * @brief 创建滑动组件
 * @param parent        触摸对象的父级对象
 * @param target_obj    目标对象（会跟随触摸区域滑动）
 * @param dir           滑动方向（只支持垂直方向`EOS_SLIDE_DIR_VER`和水平方向`EOS_SLIDE_DIR_HOR`）
 * @param target        目标坐标点（`dir`为垂直时，target指的是y轴坐标；为水平时，指的是x轴坐标）
 *
 * 目标对象的阈值超过`abs(当前坐标 - 触摸起始坐标)/abs(target-base)`触发全部拉出动画，
 * 自动从目标对象的当前位置拉到target位置。
 * @param threshold 阈值（0～255）参见`EOS_THRESHOLD_XXX`
 * @return eos_slide_widget_t* 创建成功返回滑动面板对象，否则为 NULL
 * @warning 请勿对目标对象使用`lv_obj_center`，否则会导致坐标移动混乱。
 */
eos_slide_widget_t *eos_slide_widget_create(
    lv_obj_t *parent,
    lv_obj_t *target_obj,
    eos_slide_widget_dir_t dir,
    lv_coord_t target,
    eos_threshold_t threshold);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_SLIDE_WIDGET_H */
