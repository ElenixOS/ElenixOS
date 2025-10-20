/**
 * @file elena_os_slide_widget.c
 * @brief 滑动组件
 * @author Sab1e
 * @date 2025-10-18
 */

#include "elena_os_slide_widget.h"

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define EOS_LOG_TAG "SlideWidget"
#include "elena_os_log.h"
#include "elena_os_config.h"
#include "elena_os_theme.h"
#include "elena_os_event.h"

// Macros and Definitions
#define DEBUG_TOUCH_AREA 0 /**< 突出显示触摸区域 */
#define SLIDE_ANIM_DURATION 120

// Variables

// Function Implementations

/************************** PRESSED **************************/

static void _touch_obj_pressed_cb(lv_event_t *e)
{
    eos_slide_widget_t *sw = (eos_slide_widget_t *)lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(sw);
    eos_event_broadcast(EOS_EVENT_SLIDE_WIDGET_TOUCH_LOCK, sw);
    sw->state = EOS_SLIDE_WIDGET_STATE_DRAGGING;

    lv_point_t p;
    lv_indev_get_point(lv_indev_active(), &p);

    if (sw->dir == EOS_SLIDE_DIR_VER)
    {
        sw->_indev_start = p.y;
    }
    else
    {
        sw->_indev_start = p.x;
    }

    lv_obj_move_foreground(sw->target_obj);
    lv_obj_move_foreground(sw->touch_obj);
}

/************************** PRESSING **************************/

static void _touch_obj_pressing_cb(lv_event_t *e)
{
    eos_slide_widget_t *sw = (eos_slide_widget_t *)lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(sw);
    sw->state = EOS_SLIDE_WIDGET_STATE_DRAGGING;
    lv_point_t p;
    lv_indev_get_point(lv_indev_active(), &p);

    lv_coord_t touch_diff = (sw->dir == EOS_SLIDE_DIR_VER)
                                ? (p.y - sw->_indev_start)
                                : (p.x - sw->_indev_start);

    lv_coord_t new_pos = sw->base;
    new_pos += touch_diff;

    // 设置新的位置
    if (sw->dir == EOS_SLIDE_DIR_VER)
    {
        lv_obj_set_y(sw->target_obj, new_pos);
    }
    else
    {
        lv_obj_set_x(sw->target_obj, new_pos);
    }
    lv_obj_send_event(sw->target_obj, EOS_EVENT_SLIDE_WIDGET_MOVING, NULL);
}

/************************** RELEASED **************************/

static void _slide_widget_timer_cb(lv_timer_t *timer)
{
    eos_slide_widget_t *sw = lv_timer_get_user_data(timer);
    lv_obj_send_event(sw->target_obj, EOS_EVENT_SLIDE_WIDGET_MOVING, NULL);

    if (sw->state == EOS_SLIDE_WIDGET_STATE_THRESHOLD)
    {
        lv_obj_send_event(sw->target_obj, EOS_EVENT_SLIDE_WIDGET_REACHED_THRESHOLD, NULL);
    }
    else if (sw->state == EOS_SLIDE_WIDGET_STATE_REVERTING)
    {
        lv_obj_send_event(sw->target_obj, EOS_EVENT_SLIDE_WIDGET_REVERTED, NULL);
    }
    sw->state = EOS_SLIDE_WIDGET_STATE_IDLE;

    eos_event_broadcast(EOS_EVENT_SLIDE_WIDGET_TOUCH_UNLOCK, NULL);
}

static void _slide_widget_anim_completed_cb(lv_anim_t *a)
{
    eos_slide_widget_t *sw = (eos_slide_widget_t *)lv_anim_get_user_data(a);
    EOS_CHECK_PTR_RETURN(sw);

    lv_obj_send_event(sw->target_obj, EOS_EVENT_SLIDE_WIDGET_DONE, NULL);

    // 避免操作过快
    lv_timer_t *t = lv_timer_create(_slide_widget_timer_cb, 50, sw);
    lv_timer_set_repeat_count(t, 1); // 只触发一次
}

static void _moving_set_x(void *obj, int32_t value)
{
    lv_obj_set_x(obj, value);
    lv_obj_send_event(obj, EOS_EVENT_SLIDE_WIDGET_MOVING, NULL);
}

static void _moving_set_y(void *obj, int32_t value)
{
    lv_obj_set_y(obj, value);
    lv_obj_send_event(obj, EOS_EVENT_SLIDE_WIDGET_MOVING, NULL);
}

static void _touch_obj_released_cb(lv_event_t *e)
{
    eos_slide_widget_t *sw = (eos_slide_widget_t *)lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(sw);

    // 获取当前位置
    lv_coord_t cur;
    if (sw->dir == EOS_SLIDE_DIR_VER)
    {
        cur = lv_obj_get_y(sw->target_obj);
    }
    else
    {
        cur = lv_obj_get_x(sw->target_obj);
    }

    // 当前触摸位置
    lv_point_t p;
    lv_indev_get_point(lv_indev_active(), &p);
    lv_coord_t indev_cur = (sw->dir == EOS_SLIDE_DIR_VER) ? p.y : p.x;

    // 计算滑动方向和目标方向
    lv_coord_t swipe_delta = indev_cur - sw->_indev_start; // 触摸滑动距离
    lv_coord_t move_delta = sw->target - sw->base;        // target 相对 base 的偏移

    lv_coord_t target;

    if (swipe_delta * move_delta <= 0)
    {
        // 反向滑动，直接回到 base
        sw->state = EOS_SLIDE_WIDGET_STATE_REVERTING;
        EOS_LOG_I("Reverse swipe: return to base");

        target = sw->base;
    }
    else
    {
        // 正向滑动，按阈值判断
        uint32_t total_move_range = abs(move_delta);
        uint32_t scaled_swipe_dist = abs(swipe_delta) * EOS_THRESHOLD_SCALE;

        if (total_move_range == 0)
        {
            EOS_LOG_E("DIV0");
            return;
        }

        if (scaled_swipe_dist / total_move_range > sw->threshold)
        {
            sw->state = EOS_SLIDE_WIDGET_STATE_THRESHOLD;
            EOS_LOG_I("EOS_SLIDE_WIDGET_STATE_THRESHOLD");
            target = sw->target;
        }
        else
        {
            sw->state = EOS_SLIDE_WIDGET_STATE_REVERTING;
            EOS_LOG_I("EOS_SLIDE_WIDGET_STATE_REVERTING");
            target = sw->base;
        }
    }

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, sw->target_obj);
    lv_anim_set_values(&a, cur, target);
    EOS_LOG_I("Move from %d to %d", cur, target);

    if (sw->dir == EOS_SLIDE_DIR_VER)
    {
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)_moving_set_y);
    }
    else
    {
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)_moving_set_x);
    }

    lv_anim_set_time(&a, SLIDE_ANIM_DURATION);
    lv_anim_set_user_data(&a, sw);
    lv_anim_set_completed_cb(&a, _slide_widget_anim_completed_cb);
    lv_anim_start(&a);
}

/************************** MISC **************************/

static void _sw_touch_lock_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    eos_slide_widget_t *sw = (eos_slide_widget_t *)lv_event_get_user_data(e);
    eos_slide_widget_t *active_sw = (eos_slide_widget_t *)lv_event_get_param(e);
    if (active_sw != NULL && active_sw == sw)
        return;
    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
}

static void _sw_touch_unlock_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    eos_slide_widget_t *sw = (eos_slide_widget_t *)lv_event_get_user_data(e);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_HIDDEN);
}

void eos_slide_widget_reverse(eos_slide_widget_t *sw)
{
    lv_coord_t tmp;
    tmp = sw->base;
    sw->base = sw->target;
    sw->target = tmp;
    tmp = sw->touch_obj_base;
    sw->touch_obj_base = sw->touch_obj_target;
    sw->touch_obj_target = tmp;
}

static void _slide_widget_delete_cb(lv_event_t *e)
{
    eos_slide_widget_t *sw = (eos_slide_widget_t *)lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(sw);
    EOS_LOG_I("Deleting slide panel for target obj %p", lv_event_get_target(e));

    if (sw->touch_obj)
    {
        lv_obj_remove_event_cb(sw->touch_obj, _touch_obj_pressed_cb);
        lv_obj_remove_event_cb(sw->touch_obj, _touch_obj_pressing_cb);
        lv_obj_remove_event_cb(sw->touch_obj, _touch_obj_released_cb);

        if (lv_obj_is_valid(sw->touch_obj))
        {
            lv_obj_del(sw->touch_obj);
        }

        sw->touch_obj = NULL;
    }

    sw->target_obj = NULL;
    lv_free(sw);
}

void eos_slide_widget_move(eos_slide_widget_t *sw, lv_coord_t start, lv_coord_t end, uint32_t duration)
{
    sw->state = EOS_SLIDE_WIDGET_STATE_ANIMATING;
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, sw->target_obj);
    lv_anim_set_values(&a,
                       start, end);
    EOS_LOG_I("Move from %d to %d", start, end);

    if (sw->dir == EOS_SLIDE_DIR_VER)
    {
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)_moving_set_y);
    }
    else
    {
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)_moving_set_x);
    }

    lv_anim_set_time(&a, duration);
    lv_anim_set_user_data(&a, sw);
    lv_anim_set_completed_cb(&a, _slide_widget_anim_completed_cb);
    lv_anim_start(&a);
}

eos_slide_widget_t *eos_slide_widget_create(
    lv_obj_t *parent,
    lv_obj_t *target_obj,
    eos_slide_widget_dir_t dir,
    lv_coord_t target,
    eos_threshold_t threshold)
{
    eos_slide_widget_t *sw = lv_malloc(sizeof(eos_slide_widget_t));
    EOS_CHECK_PTR_RETURN_VAL(sw, NULL);
    memset(sw, 0, sizeof(eos_slide_widget_t));

    sw->dir = dir;
    sw->state = EOS_SLIDE_WIDGET_STATE_IDLE;
    sw->threshold = threshold;

    sw->target_obj = target_obj;
    sw->base = (dir == EOS_SLIDE_DIR_VER)
                   ? lv_obj_get_y(target_obj)
                   : lv_obj_get_x(target_obj);
    sw->target = target;

    // Touch Object
    lv_obj_t *t = lv_obj_create(parent);
    lv_obj_remove_style_all(t);
#if DEBUG_TOUCH_AREA
    lv_obj_set_style_bg_color(t, EOS_COLOR_RED, 0);
    lv_obj_set_style_bg_opa(t, LV_OPA_50, 0);
#else
    lv_obj_set_style_bg_opa(t, LV_OPA_TRANSP, 0);
#endif
    lv_obj_set_pos(t, 0, 0);

    lv_obj_add_event_cb(t, _touch_obj_pressed_cb, LV_EVENT_PRESSED, sw);
    lv_obj_add_event_cb(t, _touch_obj_pressing_cb, LV_EVENT_PRESSING, sw);
    lv_obj_add_event_cb(t, _touch_obj_released_cb, LV_EVENT_RELEASED, sw);
    eos_event_add_cb(t, _sw_touch_lock_cb, EOS_EVENT_SLIDE_WIDGET_TOUCH_LOCK, sw);
    eos_event_add_cb(t, _sw_touch_unlock_cb, EOS_EVENT_SLIDE_WIDGET_TOUCH_UNLOCK, sw);
    lv_obj_add_event_cb(target_obj, _slide_widget_delete_cb, LV_EVENT_DELETE, sw);

    sw->touch_obj = t;

    return sw;
}
