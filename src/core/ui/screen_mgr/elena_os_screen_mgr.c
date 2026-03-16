/**
 * @file elena_os_screen_mgr.c
 * @brief Screen 管理器
 * @author Sab1e
 * @date 2025-12-02
 */

#include "elena_os_screen_mgr.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#define EOS_LOG_TAG "ScreenManager"
#include "elena_os_log.h"
#include "elena_os_theme.h"
#include "elena_os_nav.h"
#include "elena_os_basic_widgets.h"
#include "elena_os_scene.h"
#include "elena_os_app_list.h"
#include "elena_os_app_header.h"
#include "elena_os_anim.h"

/* Macros and Definitions -------------------------------------*/
#define _ENTER_APP_ANIM_SCALE_START 128
#define _ENTER_APP_ANIM_SCALE_END 256
/* Variables --------------------------------------------------*/
static lv_obj_t *scr_to_load = NULL;
static bool is_app_header_visible_before_anim = false;
static lv_point_t last_clicked_point;
static void _pending_screen_lifecycle_cb(lv_event_t *e);
/* Function Implementations -----------------------------------*/

static void _detach_pending_screen_callbacks(lv_obj_t *scr)
{
    if (!(scr && lv_obj_is_valid(scr) && lv_obj_has_class(scr, &lv_obj_class)))
        return;

    lv_obj_remove_event_cb(scr, _pending_screen_lifecycle_cb);
}

static void _clear_pending_screen(lv_obj_t *scr)
{
    if (scr && scr_to_load != scr)
        return;

    if (scr_to_load && lv_obj_is_valid(scr_to_load) && lv_obj_has_class(scr_to_load, &lv_obj_class))
    {
        _detach_pending_screen_callbacks(scr_to_load);
    }

    scr_to_load = NULL;
}

static void _pending_screen_lifecycle_cb(lv_event_t *e)
{
    lv_obj_t *scr = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    if (scr != scr_to_load)
        return;

    switch (code)
    {
    case LV_EVENT_SCREEN_LOADED:
    case LV_EVENT_SCREEN_UNLOAD_START:
    case LV_EVENT_DELETE:
        EOS_LOG_D("Clear pending screen[%p] by event %d", scr, code);
        _clear_pending_screen(scr);
        break;

    default:
        break;
    }
}

static void _set_pending_screen(lv_obj_t *scr)
{
    if (!(scr && lv_obj_is_valid(scr) && lv_obj_has_class(scr, &lv_obj_class)))
    {
        _clear_pending_screen(NULL);
        return;
    }

    if (scr_to_load == scr)
        return;

    _clear_pending_screen(NULL);

    lv_obj_add_event_cb(scr, _pending_screen_lifecycle_cb, LV_EVENT_SCREEN_LOADED, NULL);
    lv_obj_add_event_cb(scr, _pending_screen_lifecycle_cb, LV_EVENT_SCREEN_UNLOAD_START, NULL);
    lv_obj_add_event_cb(scr, _pending_screen_lifecycle_cb, LV_EVENT_DELETE, NULL);
    scr_to_load = scr;
}

static void _snapshot_obj_delete_cb(lv_event_t *e)
{
    lv_draw_buf_t *snapshot = lv_event_get_user_data(e);
    if (snapshot)
        eos_draw_buf_destory(snapshot);
}

static void _anim_complete_cb(lv_anim_t *a)
{
    lv_obj_t *pending_scr = (lv_obj_t *)a->user_data;

    lv_obj_delete_async((lv_obj_t *)a->var);
    if (!(pending_scr && lv_obj_is_valid(pending_scr) && lv_obj_has_class(pending_scr, &lv_obj_class)))
    {
        _clear_pending_screen(pending_scr);
        eos_anim_blocker_hide();
        return;
    }

    lv_screen_load(pending_scr);
    eos_anim_blocker_hide();
    if (pending_scr == eos_app_list_get_screen())
    {
        eos_app_header_hide();
    }
    else
    {
        if (is_app_header_visible_before_anim)
            eos_app_header_show();
        else
            eos_app_header_hide();
    }
}

static void _anim_exec_cb(void *var, int32_t value)
{
    lv_image_set_scale(var, value);
    lv_obj_set_style_opa(var, (value - 128) * 255 / 128, 0);
}

static void _is_app_header_visible(void)
{
    if (eos_app_header_is_visible())
    {
        EOS_LOG_D("App header is visible before anim");
        is_app_header_visible_before_anim = true;
    }
    else
    {
        EOS_LOG_D("App header is hidden before anim");
        is_app_header_visible_before_anim = false;
    }
}

void eos_screen_set_last_clicked_point(lv_coord_t x, lv_coord_t y)
{
    last_clicked_point.x = x;
    last_clicked_point.y = y;
}

static void _play_zoom_anim(lv_obj_t *scr,
                            int32_t scale_start,
                            int32_t scale_end,
                            uint32_t duration,
                            uint32_t delay)
{
    _is_app_header_visible();
    // 显示动画阻塞层
    eos_anim_blocker_show();

    // 创建快照对象到 layer_top
    lv_obj_t *snapshot_obj = lv_image_create(lv_layer_top());

    // 切换 app header 父对象使其可以被截图
    eos_app_header_set_parent(scr);
    lv_draw_buf_t *snapshot = eos_draw_buf_create(
        lv_obj_get_width(scr),
        lv_obj_get_height(scr),
        lv_display_get_color_format(lv_display_get_default()),
        0);

    lv_snapshot_take_to_draw_buf(
        scr,
        lv_display_get_color_format(lv_display_get_default()), snapshot);
    eos_app_header_hide();
    eos_app_header_parent_reset();

    lv_obj_add_event_cb(snapshot_obj, _snapshot_obj_delete_cb, LV_EVENT_DELETE, snapshot);
    lv_image_set_src(snapshot_obj, snapshot);
    lv_obj_set_style_radius(snapshot_obj, EOS_DISPLAY_RADIUS, 0);
    lv_obj_set_style_clip_corner(snapshot_obj, true, 0);

    // 计算 pivot
    lv_image_set_pivot(snapshot_obj, last_clicked_point.x, last_clicked_point.y);

    // 配置动画
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, snapshot_obj);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)_anim_exec_cb);
    lv_anim_set_completed_cb(&a, _anim_complete_cb);

    lv_anim_set_user_data(&a, scr_to_load);

    lv_anim_set_values(&a, scale_start, scale_end);
    lv_anim_set_duration(&a, duration);
    lv_anim_set_delay(&a, delay);

    lv_anim_start(&a);
}

lv_obj_t *eos_screen_active(void)
{
    EOS_LOG_D("eos_screen_active");
    if (scr_to_load && lv_obj_is_valid(scr_to_load) && lv_obj_has_class(scr_to_load, &lv_obj_class))
    {
        EOS_LOG_D("scr_to_load[%p]", scr_to_load);
        return scr_to_load;
    }
    EOS_LOG_D("lv_screen_active[%p]", lv_screen_active());
    return lv_screen_active();
}

static void _enter_app_anim_async_cb(void *user_data)
{
    lv_obj_t *pending_scr = (lv_obj_t *)user_data;

    if (pending_scr != scr_to_load)
        return;

    if (!(pending_scr && lv_obj_is_valid(pending_scr) && lv_obj_has_class(pending_scr, &lv_obj_class)))
    {
        _clear_pending_screen(pending_scr);
        return;
    }

    _play_zoom_anim(pending_scr,
                    _ENTER_APP_ANIM_SCALE_START,
                    _ENTER_APP_ANIM_SCALE_END,
                    EOS_SCREEN_ENTER_APP_ANIM_DURATION,
                    EOS_APP_LIST_ENTER_APP_ANIM_DELAY);
}

void eos_screen_load(lv_obj_t *scr)
{
    EOS_LOG_D("eos_screen_load scr[%p]", scr);
    if (!(scr && lv_obj_is_valid(scr) && lv_obj_has_class(scr, &lv_obj_class)))
        return;

    _set_pending_screen(scr);
    eos_event_broadcast(EOS_EVENT_GLOBAL_SCREEN_LOAD_START, scr);

    if (eos_nav_get_state() == EOS_NAV_STATE_ENTER_NEXT_SCREEN && eos_nav_is_state_completed())
    {
        EOS_LOG_D("Play ENTER_NEXT anim");
        lv_screen_load_anim(scr, LV_SCR_LOAD_ANIM_OVER_LEFT, EOS_SCREEN_SWITCH_DURATION, 0, false);
    }
    else if (eos_nav_get_state() == EOS_NAV_STATE_BACK_PREV_SCREEN)
    {
        EOS_LOG_D("Play BACK_PREV anim");
        lv_screen_load_anim(scr, LV_SCR_LOAD_ANIM_MOVE_RIGHT, EOS_SCREEN_SWITCH_DURATION, 0, false);
    }
    else
    {
        EOS_LOG_D("Normal Load");
        if (eos_secne_is_equal(EOS_SCENE_NAVIGATION, EOS_SCENE_APP_LIST) || eos_secne_is_equal(EOS_SCENE_NAVIGATION, EOS_SCENE_WATCHFACE))
        {
            // 必须异步调用，让scr渲染一帧以便截图
            lv_async_call(_enter_app_anim_async_cb, scr);
        }
        else if (eos_secne_is_equal(EOS_SCENE_APP_LIST, EOS_SCENE_NAVIGATION) || eos_secne_is_equal(EOS_SCENE_WATCHFACE, EOS_SCENE_NAVIGATION))
        {
            lv_screen_load(scr);
            lv_obj_t *nav_home_scr = eos_nav_get_home_screen();
            _play_zoom_anim(nav_home_scr,
                            _ENTER_APP_ANIM_SCALE_END,
                            _ENTER_APP_ANIM_SCALE_START,
                            EOS_SCREEN_ENTER_APP_ANIM_DURATION,
                            0);
        }
        else if (eos_secne_is_equal(EOS_SCENE_APP_LIST, EOS_SCENE_WATCHFACE))
            lv_screen_load_anim(scr, LV_SCR_LOAD_ANIM_OVER_TOP, EOS_SCREEN_SWITCH_DURATION, 0, false);
        else if (eos_secne_is_equal(EOS_SCENE_WATCHFACE, EOS_SCENE_APP_LIST))
            lv_screen_load_anim(scr, LV_SCR_LOAD_ANIM_OUT_BOTTOM, EOS_SCREEN_SWITCH_DURATION, 0, false);
        else
        {
            lv_screen_load_anim(scr, LV_SCR_LOAD_ANIM_FADE_IN, EOS_SCREEN_SWITCH_DURATION, 0, false);
        }
    }
    eos_event_broadcast(EOS_EVENT_GLOBAL_SCREEN_LOADED, scr);
}

void eos_screen_load_without_anim(lv_obj_t *scr)
{
    EOS_LOG_D("eos_screen_load_without_anim scr[%p]", scr);
    if (!(scr && lv_obj_is_valid(scr) && lv_obj_has_class(scr, &lv_obj_class)))
        return;

    _set_pending_screen(scr);
    eos_event_broadcast(EOS_EVENT_GLOBAL_SCREEN_LOAD_START, scr);
    lv_screen_load(scr);
    eos_event_broadcast(EOS_EVENT_GLOBAL_SCREEN_LOADED, scr);
}

lv_obj_t *eos_screen_create(void)
{
    lv_obj_t *scr = lv_obj_create(NULL);
    EOS_LOG_D("eos_screen_create scr[%p]", scr);
    lv_obj_add_style(scr, eos_theme_get_screen_style(), 0);
    lv_obj_set_style_clip_corner(scr, true, 0);
    return scr;
}
