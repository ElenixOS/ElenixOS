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
#include "elena_os_dispatcher.h"
#define EOS_LOG_DISABLE
#define EOS_LOG_TAG "Crown"
#include "elena_os_log.h"
#include "elena_os_pm.h"
#include "elena_os_vibrator.h"
#include "elena_os_theme.h"
#include "elena_os_touch.h"
#include "elena_os_anim.h"
#include "elena_os_activity.h"
#include "elena_os_app_list.h"
#include "elena_os_control_center.h"
#include "elena_os_msg_list.h"
#include "elena_os_swipe_panel.h"
#include "elena_os_slide_widget.h"
#include "elena_os_event.h"
/* Macros and Definitions -------------------------------------*/
#define _CROWN_ENCODER_SCROLL_COEFFICIENT 50
#define _VIBRATOR_TICK_DY_THRESHOLD 15

#define _SCROLLBAR_WIDTH 12
#define _SCROLLBAR_HEIGHT 90
#define _SCROLLBAR_RADIUS 8
#define _SCROLLBAR_MARGIN_TOP 90
#define _SCROLLBAR_FADE_IN_DURATION 80
#define _SCROLLBAR_FADE_OUT_DURATION 160
#define _SCROLLBAR_HIDE_DELAY 260
/* Variables --------------------------------------------------*/
static lv_obj_t *scrollable_obj = NULL;
static lv_obj_t *scrollable_root = NULL;
static int8_t encoder_reverse = -1;
static lv_obj_t *scrollbar = NULL;
static lv_timer_t *scrollbar_hide_timer = NULL;
static lv_obj_t *pending_rebind_target = NULL;
static bool pending_rebind_is_view = true;
static bool pending_rebind_scheduled = false;

static void _scrollable_obj_scrolled_cb(lv_event_t *e);
static void _scrollable_obj_scroll_start_cb(lv_event_t *e);
static void _scrollable_obj_scroll_end_cb(lv_event_t *e);
static void _clear_scrollable_obj_cb(lv_event_t *e);
static void _clear_scrollable_obj_async_cb(void *user_data);
static void _activity_view_switched_cb(lv_event_t *e);
static void _activity_view_switched_async_cb(void *user_data);
static void _slide_widget_state_changed_cb(lv_event_t *e);
static void _slide_widget_state_changed_async_cb(void *user_data);
static void _apply_pending_rebind_async_cb(void *user_data);
static void _scrollbar_hide_timer_cb(lv_timer_t *t);
static void _scrollbar_fade_out_done_cb(lv_anim_t *a);
static lv_obj_t *_find_scrollable_obj(lv_obj_t *root);
static bool _is_descendant_of(lv_obj_t *obj, lv_obj_t *ancestor);
static void _set_target_obj_immediate(lv_obj_t *obj);
static void _set_target_view_immediate(lv_obj_t *view);
static bool _obj_is_visible_for_crown(lv_obj_t *obj);
/* Function Implementations -----------------------------------*/

static void _scrollbar_schedule_hide(void)
{
    if (!(scrollbar_hide_timer && scrollbar && lv_obj_is_valid(scrollbar)))
        return;

    lv_timer_set_period(scrollbar_hide_timer, _SCROLLBAR_HIDE_DELAY);
    lv_timer_reset(scrollbar_hide_timer);
    lv_timer_resume(scrollbar_hide_timer);
}

static void _scrollbar_cancel_hide(void)
{
    if (!scrollbar_hide_timer)
        return;

    lv_timer_pause(scrollbar_hide_timer);
    lv_timer_reset(scrollbar_hide_timer);
}

static void _scrollbar_show_now(void)
{
    if (!(scrollbar && lv_obj_is_valid(scrollbar)))
        return;

    _scrollbar_cancel_hide();
    lv_anim_delete(scrollbar, NULL);
    lv_obj_clear_flag(scrollbar, LV_OBJ_FLAG_HIDDEN);

    eos_lite_anim_fade_layered_start(scrollbar,
                                     lv_obj_get_style_opa_layered(scrollbar, 0),
                                     LV_OPA_100,
                                     _SCROLLBAR_FADE_IN_DURATION,
                                     0,
                                     NULL,
                                     NULL);
}

static void _scrollbar_hide_now(void)
{
    if (!(scrollbar && lv_obj_is_valid(scrollbar)))
        return;

    _scrollbar_cancel_hide();
    lv_anim_delete(scrollbar, NULL);
    lv_obj_set_style_opa_layered(scrollbar, LV_OPA_0, 0);
    lv_obj_add_flag(scrollbar, LV_OBJ_FLAG_HIDDEN);
}

static void _scrollbar_fade_out_done_cb(lv_anim_t *a)
{
    lv_obj_t *obj = (lv_obj_t *)a->var;
    if (!(obj && lv_obj_is_valid(obj)))
        return;

    lv_obj_set_style_opa_layered(obj, LV_OPA_0, 0);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
}

static void _scrollbar_hide_timer_cb(lv_timer_t *t)
{
    if (!t)
        return;

    /* Keep timer instance persistent and emulate one-shot behavior safely. */
    lv_timer_pause(t);
    lv_timer_reset(t);

    if (!(scrollbar && lv_obj_is_valid(scrollbar)))
        return;

    lv_anim_delete(scrollbar, NULL);
    eos_lite_anim_fade_layered_start(scrollbar,
                                     lv_obj_get_style_opa_layered(scrollbar, 0),
                                     LV_OPA_0,
                                     _SCROLLBAR_FADE_OUT_DURATION,
                                     0,
                                     _scrollbar_fade_out_done_cb,
                                     NULL);
}

static void _clear_scrollable_obj(void)
{
    if (scrollable_obj && lv_obj_is_valid(scrollable_obj))
    {
        while (lv_obj_remove_event_cb(scrollable_obj, _scrollable_obj_scrolled_cb))
        {
        }
        while (lv_obj_remove_event_cb(scrollable_obj, _scrollable_obj_scroll_start_cb))
        {
        }
        while (lv_obj_remove_event_cb(scrollable_obj, _scrollable_obj_scroll_end_cb))
        {
        }
        while (lv_obj_remove_event_cb(scrollable_obj, _clear_scrollable_obj_cb))
        {
        }
    }

    if (scrollable_root && lv_obj_is_valid(scrollable_root))
    {
        while (lv_obj_remove_event_cb(scrollable_root, _clear_scrollable_obj_cb))
        {
        }
    }

    scrollable_obj = NULL;
    scrollable_root = NULL;

    _scrollbar_hide_now();
}

static void _clear_scrollable_obj_cb(lv_event_t *e)
{
    lv_obj_t *target = lv_event_get_target(e);
    lv_async_call(_clear_scrollable_obj_async_cb, target);
}

static void _clear_scrollable_obj_async_cb(void *user_data)
{
    lv_obj_t *target = (lv_obj_t *)user_data;
    /* 延迟清理的旧滚动对象 */
    if (target != scrollable_obj && target != scrollable_root)
        return;

    _clear_scrollable_obj();
}

static void _scrollbar_set_focused(void)
{
    if (!(scrollbar && lv_obj_is_valid(scrollbar)))
        return;
    lv_obj_set_style_bg_color(scrollbar, EOS_COLOR_GREEN, LV_PART_MAIN);
    lv_obj_set_style_bg_color(scrollbar, EOS_COLOR_GREEN, LV_PART_INDICATOR);
}

static void _scrollbar_set_unfocused(void)
{
    if (!(scrollbar && lv_obj_is_valid(scrollbar)))
        return;
    lv_obj_set_style_bg_color(scrollbar, EOS_COLOR_WHITE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(scrollbar, EOS_COLOR_WHITE, LV_PART_INDICATOR);
}

static void _scrollbar_hide_set_anim(void)
{
    _scrollbar_schedule_hide();
}

static void _crown_button_async_cb(void *user_data)
{
    eos_control_center_t *cc = NULL;
    eos_msg_list_t *msg_list = NULL;

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
    {
        bool from_watchface = (eos_activity_get_current() == eos_activity_get_watchface());

        // 检查控制中心
        cc = eos_control_center_get_instance();
        if (cc && cc->swipe_panel && cc->swipe_panel->sw)
        {
            if (cc->swipe_panel->sw->state == EOS_SLIDE_WIDGET_STATE_OPEN)
            {
                eos_swipe_panel_pull_back(cc->swipe_panel);
            }
        }

        // 检查消息列表
        msg_list = eos_msg_list_get_instance();
        if (msg_list && msg_list->swipe_panel && msg_list->swipe_panel->sw)
        {
            if (msg_list->swipe_panel->sw->state == EOS_SLIDE_WIDGET_STATE_OPEN)
            {
                eos_swipe_panel_pull_back(msg_list->swipe_panel);
            }
        }

        if (from_watchface)
        {
            // 从表盘进入其他 Activity 时，先拉回 overlay，再继续进入。
            eos_app_list_enter();
        }
        else
        {
            eos_activity_back();
        }
        break;
    }
    default:
        break;
    }
}

static void _crown_encoder_async_cb(void *user_data)
{
    if (scrollable_obj)
    {
        if (!lv_obj_is_valid(scrollable_obj) || !lv_obj_is_visible(scrollable_obj))
        {
            _clear_scrollable_obj();
            return;
        }

        if (scrollable_root && lv_obj_is_valid(scrollable_root) && !lv_obj_is_visible(scrollable_root))
        {
            return;
        }

        eos_crown_encoder_diff_t diff = (eos_crown_encoder_diff_t)(intptr_t)user_data;
        int32_t dy = diff * encoder_reverse * _CROWN_ENCODER_SCROLL_COEFFICIENT;
        if (abs(dy) > _VIBRATOR_TICK_DY_THRESHOLD)
        {
            eos_vibrator_tick();
            _scrollbar_set_focused();
        }
        if (scrollable_obj && lv_obj_is_valid(scrollable_obj))
            lv_obj_scroll_by_bounded(scrollable_obj, 0, dy, LV_ANIM_ON);
    }
}

static void _scrollable_obj_scrolled_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    if (!obj || !lv_obj_is_valid(obj) || !(scrollbar && lv_obj_is_valid(scrollbar)))
        return;

    int32_t view_h = lv_obj_get_height(obj);
    int32_t top = lv_obj_get_scroll_top(obj);
    int32_t bottom = lv_obj_get_scroll_bottom(obj);
    int32_t scroll_y = lv_obj_get_scroll_y(obj);

    int32_t content_h = top + view_h + bottom;

    if (content_h <= view_h)
    {
        _scrollbar_hide_now();
        return;
    }

    _scrollbar_show_now();
    _scrollbar_schedule_hide();

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
    LV_UNUSED(e);
    _scrollbar_show_now();
    _scrollbar_schedule_hide();
}

static void _scrollable_obj_scroll_end_cb(lv_event_t *e)
{
    LV_UNUSED(e);
    _scrollbar_schedule_hide();
}

static void _activity_view_switched_cb(lv_event_t *e)
{
    lv_obj_t *view = (lv_obj_t *)lv_event_get_param(e);
    lv_async_call(_activity_view_switched_async_cb, view);
}

static void _activity_view_switched_async_cb(void *user_data)
{
    lv_obj_t *view = (lv_obj_t *)user_data;
    eos_crown_encoder_set_target_view(view);
}

static void _slide_widget_state_changed_cb(lv_event_t *e)
{
    lv_async_call(_slide_widget_state_changed_async_cb, lv_event_get_param(e));
}

static void _slide_widget_state_changed_async_cb(void *user_data)
{
    eos_slide_widget_t *sw = (eos_slide_widget_t *)user_data;
    if (sw && sw->state == EOS_SLIDE_WIDGET_STATE_OPEN && sw->target_obj && lv_obj_is_valid(sw->target_obj))
    {
        lv_obj_t *target = _find_scrollable_obj(sw->target_obj);
        if (target)
        {
            /* 滑动面板已打开，优先作为滚动对象 */
            eos_crown_encoder_set_target_obj(target);
            return;
        }
    }
}

static bool _obj_is_visible_for_crown(lv_obj_t *obj)
{
    return obj && lv_obj_is_valid(obj) && !lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN) && lv_obj_is_visible(obj);
}

static void _apply_pending_rebind_async_cb(void *user_data)
{
    LV_UNUSED(user_data);

    lv_obj_t *target = pending_rebind_target;
    bool is_view = pending_rebind_is_view;

    pending_rebind_scheduled = false;
    pending_rebind_target = NULL;

    if (is_view)
    {
        _set_target_view_immediate(target);
    }
    else
    {
        _set_target_obj_immediate(target);
    }
}

static bool _is_descendant_of(lv_obj_t *obj, lv_obj_t *ancestor)
{
    if (!(obj && ancestor && lv_obj_is_valid(obj) && lv_obj_is_valid(ancestor)))
        return false;

    lv_obj_t *cur = obj;
    while (cur && lv_obj_is_valid(cur))
    {
        if (cur == ancestor)
            return true;
        cur = lv_obj_get_parent(cur);
    }

    return false;
}

static void _apply_scrollable_obj(lv_obj_t *obj, bool enforce_active_view_scope)
{
    lv_obj_t *root;
    lv_obj_t *active_view;

    if (!(obj && lv_obj_is_valid(obj)))
    {
        _clear_scrollable_obj();
        return;
    }

    root = lv_obj_get_screen(obj);
    if (!(root && lv_obj_is_valid(root)))
    {
        _clear_scrollable_obj();
        return;
    }

    if (enforce_active_view_scope)
    {
        active_view = eos_view_active();
        if (!(active_view && lv_obj_is_valid(active_view) && _is_descendant_of(obj, active_view)))
        {
            EOS_LOG_D("Skip non-active-view scrollable: obj=%p view=%p", obj, active_view);
            _clear_scrollable_obj();
            return;
        }
    }

    lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
    _clear_scrollable_obj();

    scrollable_obj = obj;
    scrollable_root = root;
    lv_obj_add_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_event_cb(obj, _scrollable_obj_scrolled_cb, LV_EVENT_SCROLL, NULL);
    lv_obj_add_event_cb(obj, _scrollable_obj_scroll_start_cb, LV_EVENT_SCROLL_BEGIN, NULL);
    lv_obj_add_event_cb(obj, _scrollable_obj_scroll_end_cb, LV_EVENT_SCROLL_END, NULL);
    lv_obj_add_event_cb(obj, _clear_scrollable_obj_cb, LV_EVENT_DELETE, NULL);
    lv_obj_add_event_cb(root, _clear_scrollable_obj_cb, LV_EVENT_SCREEN_UNLOAD_START, NULL);
    lv_obj_add_event_cb(root, _clear_scrollable_obj_cb, LV_EVENT_DELETE, NULL);
}

static lv_obj_t *_find_scrollable_obj(lv_obj_t *root)
{
    if (!(root && lv_obj_is_valid(root)))
        return NULL;

    /* Keep legacy preference: bind to list first when present. */
    lv_obj_t *list = lv_obj_get_child_by_type(root, 0, &lv_list_class);
    if (list && lv_obj_is_valid(list))
        return list;

    uint32_t child_cnt = lv_obj_get_child_count(root);
    for (uint32_t i = 0; i < child_cnt; i++)
    {
        lv_obj_t *child = lv_obj_get_child(root, i);
        if (!(child && lv_obj_is_valid(child)))
            continue;

        if (lv_obj_has_flag(child, LV_OBJ_FLAG_SCROLLABLE))
            return child;

        lv_obj_t *nested = _find_scrollable_obj(child);
        if (nested)
            return nested;
    }

    return NULL;
}

static void _set_target_obj_immediate(lv_obj_t *obj)
{
    if (obj && lv_obj_is_valid(obj) && lv_obj_has_class(obj, &lv_obj_class))
    {
        if (!lv_obj_get_parent(obj))
        {
            _set_target_view_immediate(obj);
            return;
        }
        _apply_scrollable_obj(obj, false);
    }
    else
    {
        _clear_scrollable_obj();
    }
}

static void _set_target_view_immediate(lv_obj_t *view)
{
    if (view && lv_obj_is_valid(view) && lv_obj_has_class(view, &lv_obj_class))
    {
        lv_obj_t *target = _find_scrollable_obj(view);
        EOS_LOG_D("Scrollable target=%p", target);
        if (target)
        {
            _apply_scrollable_obj(target, true);
            return;
        }
    }

    _clear_scrollable_obj();
}

void eos_crown_encoder_set_target_obj(lv_obj_t *obj)
{
    pending_rebind_target = obj;
    pending_rebind_is_view = false;
    if (pending_rebind_scheduled)
    {
        return;
    }

    pending_rebind_scheduled = true;
    lv_async_call(_apply_pending_rebind_async_cb, NULL);
}

void eos_crown_encoder_set_target_view(lv_obj_t *view)
{
    pending_rebind_target = view;
    pending_rebind_is_view = true;
    if (pending_rebind_scheduled)
    {
        return;
    }

    pending_rebind_scheduled = true;
    lv_async_call(_apply_pending_rebind_async_cb, NULL);
}

void eos_crown_encoder_activate_current_overlay_scrollable(void)
{
    eos_control_center_t *cc = eos_control_center_get_instance();
    if (cc && cc->swipe_panel && _obj_is_visible_for_crown(cc->swipe_panel->swipe_obj) &&
        cc->container && _obj_is_visible_for_crown(cc->container))
    {
        eos_crown_encoder_set_target_obj(cc->container);
        return;
    }

    eos_msg_list_t *msg_list = eos_msg_list_get_instance();
    if (msg_list && msg_list->swipe_panel && _obj_is_visible_for_crown(msg_list->swipe_panel->swipe_obj) &&
        msg_list->list && _obj_is_visible_for_crown(msg_list->list))
    {
        eos_crown_encoder_set_target_obj(msg_list->list);
        return;
    }

    eos_crown_encoder_set_target_view(eos_view_active());
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
    lv_obj_add_flag(scrollbar, LV_OBJ_FLAG_HIDDEN);
    scrollbar_hide_timer = lv_timer_create(_scrollbar_hide_timer_cb, _SCROLLBAR_HIDE_DELAY, NULL);
    if (scrollbar_hide_timer)
    {
        lv_timer_pause(scrollbar_hide_timer);
    }

    eos_event_add_global_cb(_activity_view_switched_cb,
                            EOS_EVENT_ACTIVITY_SCREEN_SWITCHED,
                            NULL);
    eos_event_add_global_cb(_slide_widget_state_changed_cb,
                            EOS_EVENT_SLIDE_WIDGET_OPENED,
                            NULL);
    eos_event_add_global_cb(_slide_widget_state_changed_cb,
                            EOS_EVENT_SLIDE_WIDGET_CLOSED,
                            NULL);
}
