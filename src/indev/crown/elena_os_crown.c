/**
 * @file elena_os_crown.c
 * @brief 表冠
 * @author Sab1e
 * @date 2025-11-13
 */

#include "elena_os_crown.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "elena_os_scene.h"
#include "elena_os_dispatcher.h"
// #define EOS_LOG_DISABLE
#define EOS_LOG_TAG "Crown"
#include "elena_os_log.h"
#include "elena_os_pm.h"
#include "elena_os_vibrator.h"
#include "elena_os_theme.h"
#include "elena_os_touch.h"
#include "elena_os_anim.h"
/* Macros and Definitions -------------------------------------*/
#define _CROWN_ENCODER_SCROLL_COEFFICIENT 50
#define _VIBRATOR_TICK_DY_THRESHOLD 15

#define _SCROLLBAR_WIDTH 12
#define _SCROLLBAR_HEIGHT 90
#define _SCROLLBAR_RADIUS 8
#define _SCROLLBAR_MARGIN_TOP 90
/* Variables --------------------------------------------------*/
static lv_obj_t *scrollable_obj = NULL;
static int8_t encoder_reverse = -1;
static lv_obj_t *scrollbar = NULL;
/* Function Implementations -----------------------------------*/

static void _scrollbar_set_focused(void)
{
    lv_obj_set_style_bg_color(scrollbar, EOS_COLOR_GREEN, LV_PART_MAIN);
    lv_obj_set_style_bg_color(scrollbar, EOS_COLOR_GREEN, LV_PART_INDICATOR);
}

static void _scrollbar_set_unfocused(void)
{
    lv_obj_set_style_bg_color(scrollbar, EOS_COLOR_WHITE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(scrollbar, EOS_COLOR_WHITE, LV_PART_INDICATOR);
}

static void _scrollbar_hide_set_anim(void)
{
}

static void _crown_button_async_cb(void *user_data)
{
    if (eos_pm_get_state() == EOS_PM_SLEEP)
    {
        eos_pm_wake_up();
        return;
    }
    eos_pm_reset_timer();
    eos_button_state_t state = (eos_button_state_t)(intptr_t)user_data;
    switch (state)
    {
    case EOS_BUTTON_STATE_CLICKED:
        eos_scene_auto_switch();
    default:
        break;
    }
}

static void _crown_encoder_async_cb(void *user_data)
{
    if (scrollable_obj)
    {
        eos_crown_encoder_diff_t diff = (eos_crown_encoder_diff_t)(intptr_t)user_data;
        int32_t dy = diff * encoder_reverse * _CROWN_ENCODER_SCROLL_COEFFICIENT;
        if (abs(dy) > _VIBRATOR_TICK_DY_THRESHOLD)
        {
            eos_vibrator_tick();
            _scrollbar_set_focused();
        }
        lv_obj_scroll_by_bounded(scrollable_obj, 0, dy, LV_ANIM_ON);
    }
}

static void _scrollable_obj_scrolled_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    if (!obj || !lv_obj_is_valid(obj))
        return;

    int32_t view_h = lv_obj_get_height(obj);
    int32_t top = lv_obj_get_scroll_top(obj);
    int32_t bottom = lv_obj_get_scroll_bottom(obj);
    int32_t scroll_y = lv_obj_get_scroll_y(obj);

    int32_t content_h = top + view_h + bottom;

    if (content_h <= view_h)
    {
        lv_obj_add_flag(scrollbar, LV_OBJ_FLAG_HIDDEN);
        return;
    }

    lv_obj_clear_flag(scrollbar, LV_OBJ_FLAG_HIDDEN);

    int32_t scroll_max = content_h - view_h;

    if (scroll_y < 0)
        scroll_y = 0;
    if (scroll_y > scroll_max)
        scroll_y = scroll_max;

    int32_t bar_size =
        (int32_t)(view_h * _SCROLLBAR_HEIGHT / content_h);

    if (bar_size < 4)
        bar_size = 4;

    int32_t bar_top = top * _SCROLLBAR_HEIGHT / content_h;

    int32_t start_val = LV_CLAMP(0, bar_top, _SCROLLBAR_HEIGHT - 2);
    int32_t val = LV_CLAMP(0, bar_top + bar_size, _SCROLLBAR_HEIGHT - 2);
    lv_bar_set_start_value(scrollbar, start_val, LV_ANIM_OFF);
    lv_bar_set_value(scrollbar, val, LV_ANIM_OFF);
    EOS_LOG_D("Bar[%d-%d]/[0-%d]", start_val, val, _SCROLLBAR_HEIGHT);
}

static void _indev_touched_cb(lv_event_t *e)
{
    _scrollbar_set_unfocused();
}

static void _scrollable_obj_scroll_start_cb(lv_event_t *e)
{
    lv_anim_delete(scrollbar, NULL);
    eos_lite_anim_fade_layered_start(scrollbar, lv_obj_get_style_opa_layered(scrollbar, 0), LV_OPA_100, 200, 100, NULL, NULL);
}

static void _scrollable_obj_scroll_end_cb(lv_event_t *e)
{
    lv_anim_delete(scrollbar, NULL);
    eos_lite_anim_fade_layered_start(scrollbar, LV_OPA_100, LV_OPA_0, 200, 300, NULL, NULL);
}

static void _apply_scrollable_obj(lv_obj_t *obj)
{
    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
    if (scrollable_obj && lv_obj_is_valid(scrollable_obj))
    {
        lv_obj_remove_event_cb(scrollable_obj, _scrollable_obj_scrolled_cb);
        lv_obj_remove_event_cb(scrollable_obj, _scrollable_obj_scroll_start_cb);
        lv_obj_remove_event_cb(scrollable_obj, _scrollable_obj_scroll_end_cb);
    }

    scrollable_obj = obj;
    lv_obj_add_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_event_cb(obj, _scrollable_obj_scrolled_cb, LV_EVENT_SCROLL, NULL);
    lv_obj_add_event_cb(obj, _scrollable_obj_scroll_start_cb, LV_EVENT_SCROLL_BEGIN, NULL);
    lv_obj_add_event_cb(obj, _scrollable_obj_scroll_end_cb, LV_EVENT_SCROLL_END, NULL);
}

void eos_crown_encoder_set_target_obj(lv_obj_t *obj)
{
    if (obj && lv_obj_is_valid(obj) && lv_obj_has_class(obj, &lv_obj_class))
    {
        _apply_scrollable_obj(obj);
    }
    else
    {
        scrollable_obj = NULL;
    }
}

void eos_crown_encoder_set_target_screen(lv_obj_t *screen)
{
    if (screen && lv_obj_is_valid(screen) && lv_obj_has_class(screen, &lv_obj_class))
    {
        lv_obj_t *list = lv_obj_get_child_by_type(screen, 0, &lv_list_class);
        EOS_LOG_D("List=%p", list);
        if (list)
        {
            _apply_scrollable_obj(list);
        }
    }
    else
    {
        scrollable_obj = NULL;
    }
}

void eos_crown_encoder_set_reverse(bool reverse)
{
    if (reverse)
    {
        encoder_reverse = 1;
    }
    else
    {
        encoder_reverse = -1;
    }
}

void eos_crown_encoder_report(eos_crown_encoder_diff_t diff)
{
    eos_dispatcher_call(_crown_encoder_async_cb, (void *)(intptr_t)diff);
}

void eos_crown_button_report(eos_button_state_t state)
{
    eos_dispatcher_call(_crown_button_async_cb, (void *)(intptr_t)state);
}

void eos_crown_init(void)
{
    scrollbar = lv_bar_create(lv_layer_sys());
    lv_bar_set_mode(scrollbar, LV_BAR_MODE_RANGE);
    lv_bar_set_range(scrollbar, _SCROLLBAR_HEIGHT, 0);
    lv_obj_align(scrollbar, LV_ALIGN_TOP_RIGHT, -10, _SCROLLBAR_MARGIN_TOP);
    lv_bar_set_orientation(scrollbar, LV_BAR_ORIENTATION_VERTICAL);
    lv_obj_set_size(scrollbar, _SCROLLBAR_WIDTH, _SCROLLBAR_HEIGHT);
    _scrollbar_set_focused();
    lv_indev_add_event_cb(eos_touch_get_indev(), _indev_touched_cb, LV_EVENT_PRESSED, NULL);
    lv_obj_set_style_opa_layered(scrollbar, LV_OPA_0, 0);
}
