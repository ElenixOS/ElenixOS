/**
 * @file elena_os_app_list.c
 * @brief 应用列表页面 - 使用bubble_grid布局
 * @author Sab1e
 * @date 2025-08-21
 */

#include "elena_os_app_list.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "lvgl.h"
#include "cJSON.h"
#define EOS_LOG_TAG "AppList"
#include "elena_os_log.h"
#include "elena_os_app.h"
#include "elena_os_basic_widgets.h"
#include "elena_os_misc.h"
#include "elena_os_img.h"
#include "elena_os_port.h"
#include "elena_os_anim.h"
#include "script_engine_core.h"
#include "elena_os_sys.h"
#include "elena_os_event.h"
#include "elena_os_lang.h"
#include "script_engine_core.h"
#include "elena_os_settings.h"
#include "elena_os_flash_light.h"
#include "elena_os_fs.h"
#include "elena_os_app_header.h"
#include "elena_os_mem.h"
#include "elena_os_crown.h"
#include "elena_os_theme.h"
#include "elena_os_icon.h"
#include "elena_os_font.h"
#include "elena_os_std_widgets.h"
#include "elena_os_activity.h"
#include "../ui/bubble_grid/elena_os_bubble_grid.h"
/* Macros and Definitions -------------------------------------*/
#define _APP_ICON_ANIM_DURATION 200
#define _APP_ICON_ANIM_DELAY 75

#define _APP_LIST_ANIM_DURATION 150
#define _APP_LIST_ANIM_FOCUS_SCALE 1024
#define _APP_LIST_ANIM_MIN_SACLE 64
#define _APP_LIST_ANIM_SPLIT_PCT 15
#define _APP_LIST_ANIM_FROM_OPA_START 255
#define _APP_LIST_ANIM_FROM_OPA_END 0
#define _APP_LIST_ANIM_TO_OPA_START 0
#define _APP_LIST_ANIM_TO_OPA_END 255
/* Variables --------------------------------------------------*/

const char *eos_sys_app_id_list[EOS_SYS_APP_LAST] = {
    "sys.settings",
    "sys.flash_light"};

const char *eos_sys_app_icon_list[EOS_SYS_APP_LAST] = {
    EOS_IMG_SETTINGS,
    EOS_IMG_FLASH_LIGHT};

const eos_sys_app_entry_t eos_sys_app_entry_list[EOS_SYS_APP_LAST] = {
    eos_settings_enter,
    eos_flash_light_enter};

static void _app_list_on_resueme(eos_activity_t *a);

static void _app_on_enter(eos_activity_t *a);
static eos_activity_lifecycle_t app_list_lifecycle = {
    .on_enter = NULL,
    .on_destroy = NULL,
    .on_pause = NULL,
    .on_resume = _app_list_on_resueme,
};

static void _app_on_destroy(eos_activity_t *a);

static eos_activity_lifecycle_t app_lifecycle = {
    .on_enter = _app_on_enter,
    .on_destroy = _app_on_destroy,
    .on_pause = NULL,
    .on_resume = NULL,
};

typedef struct
{
    script_pkg_t pkg;
    char *app_id;
} app_launch_ctx_t;

/* Function Implementations -----------------------------------*/
static void _app_list_icon_clicked_cb(lv_event_t *e);
static void _app_installed_cb(lv_event_t *e);
static void _container_delete_cb(lv_event_t *e);
static void _app_list_refresh(lv_obj_t *bubble_grid);
static void _app_list_open_app_anim_cb(lv_anim_timeline_t *at, eos_activity_t *from, eos_activity_t *to);
static void _app_list_close_app_anim_cb(lv_anim_timeline_t *at, eos_activity_t *from, eos_activity_t *to);
static void _register_anim_routes_once(void);
static const char *_app_list_get_launch_app_id(eos_activity_t *activity);
static void _app_list_set_last_launch_app_id(const char *app_id);
static lv_obj_t *_app_list_get_bubble_grid(eos_activity_t *activity);
static void _app_list_record_icon_center_point(int32_t x, int32_t y);
static bool _app_list_calc_focus_pivot(lv_obj_t *snapshot_obj, lv_obj_t *icon_obj, int32_t *pivot_x, int32_t *pivot_y);
static bool _app_list_calc_focus_pivot_by_global_center(lv_obj_t *obj, int32_t *pivot_x, int32_t *pivot_y);
static void _app_list_set_transform_scale_cb(void *var, int32_t value);
static void _app_list_set_translate_x_cb(void *var, int32_t value);
static void _app_list_set_translate_y_cb(void *var, int32_t value);
static void _app_list_set_opa_cb(void *var, int32_t value);
static void _app_list_init_scale_anim(lv_anim_t *anim, lv_obj_t *obj, int32_t start, int32_t end, uint32_t duration);
static void _app_list_init_translate_x_anim(lv_anim_t *anim, lv_obj_t *obj, int32_t start, int32_t end, uint32_t duration);
static void _app_list_init_translate_y_anim(lv_anim_t *anim, lv_obj_t *obj, int32_t start, int32_t end, uint32_t duration);
static void _app_list_init_opa_anim(lv_anim_t *anim, lv_obj_t *obj, int32_t start, int32_t end, uint32_t duration);
static void _app_list_play_transition_anim(lv_anim_timeline_t *at, eos_activity_t *from, eos_activity_t *to, bool opening);

static bool _anim_routes_registered = false;
static bool _app_list_last_icon_center_valid = false;
static int32_t _app_list_last_icon_center_x = 0;
static int32_t _app_list_last_icon_center_y = 0;
static int32_t _app_list_last_click_index = -1;
static char _app_list_last_launch_app_id[64] = {0};
static uint32_t _app_list_icon_count = 0;

/************************** 生命周期 **************************/

static void _app_on_destroy(eos_activity_t *a)
{
    app_launch_ctx_t *ctx = eos_activity_get_user_data(a);
    if (ctx)
    {
        eos_pkg_free(&ctx->pkg);
        eos_free(ctx->app_id);
        eos_free(ctx);
        eos_activity_set_user_data(a, NULL);
    }

    // 退出脚本引擎
    script_engine_request_stop();
}

static void _app_on_enter(eos_activity_t *a)
{
    app_launch_ctx_t *ctx = eos_activity_get_user_data(a);
    EOS_CHECK_PTR_RETURN(ctx);

    lv_obj_t *app_view = eos_activity_get_view(a);
    EOS_CHECK_PTR_RETURN(app_view);

    script_engine_result_t ret = script_engine_run(&ctx->pkg);
    if (ret != SE_OK)
    {
        lv_obj_clean(app_view);
        lv_obj_remove_style_all(app_view);
        lv_obj_add_style(app_view, eos_theme_get_view_style(), 0);
        eos_activity_set_title_color(a, EOS_COLOR_RED);
        eos_activity_set_title_id(a, STR_ID_ERROR);

        lv_obj_t *list = eos_std_info_create(
            app_view,
            EOS_COLOR_RED,
            RI_BUG_LINE,
            eos_lang_get_text(STR_ID_APP_RUN_ERR_TITLE),
            eos_lang_get_text(STR_ID_APP_RUN_ERR));
        char info_str[1024];
        snprintf(info_str, sizeof(info_str), "Code: %d\nAppID: %s\nError: %s", ret, ctx->app_id, script_engine_get_error_info());
        lv_obj_t *err_label = eos_list_add_comment(list, info_str);
        lv_obj_t *btn = eos_button_create(list, eos_lang_get_text(STR_ID_BACK), eos_activity_back_cb, NULL);
        EOS_LOG_E("Application encounter a fatal error");
    }
}

static const char *_app_list_get_launch_app_id(eos_activity_t *activity)
{
    app_launch_ctx_t *ctx = eos_activity_get_user_data(activity);
    if (ctx && ctx->app_id)
    {
        return ctx->app_id;
    }

    return _app_list_last_launch_app_id[0] ? _app_list_last_launch_app_id : NULL;
}

static void _app_list_set_last_launch_app_id(const char *app_id)
{
    if (!app_id)
    {
        _app_list_last_launch_app_id[0] = '\0';
        return;
    }

    snprintf(_app_list_last_launch_app_id,
             sizeof(_app_list_last_launch_app_id),
             "%s",
             app_id);
}

static lv_obj_t *_app_list_get_bubble_grid(eos_activity_t *activity)
{
    if (!activity)
    {
        return NULL;
    }

    lv_obj_t *bubble_grid = (lv_obj_t *)eos_activity_get_user_data(activity);
    if (bubble_grid)
    {
        return bubble_grid;
    }

    lv_obj_t *view = eos_activity_get_view(activity);
    if (!view)
    {
        return NULL;
    }

    return lv_obj_get_child(view, 0);
}

static void _app_list_record_icon_center_point(int32_t x, int32_t y)
{
    _app_list_last_icon_center_x = x;
    _app_list_last_icon_center_y = y;
    _app_list_last_icon_center_valid = true;
}

static bool _app_list_calc_focus_pivot_by_global_center(lv_obj_t *obj, int32_t *pivot_x, int32_t *pivot_y)
{
    if (!(obj && pivot_x && pivot_y && _app_list_last_icon_center_valid))
    {
        return false;
    }

    int32_t max_x = lv_obj_get_width(obj);
    int32_t max_y = lv_obj_get_height(obj);
    int32_t local_x = _app_list_last_icon_center_x;
    int32_t local_y = _app_list_last_icon_center_y;

    if (local_x < 0)
        local_x = 0;
    if (local_y < 0)
        local_y = 0;
    if (local_x > max_x)
        local_x = max_x;
    if (local_y > max_y)
        local_y = max_y;

    *pivot_x = local_x;
    *pivot_y = local_y;
    return true;
}

static bool _app_list_calc_focus_pivot(lv_obj_t *snapshot_obj, lv_obj_t *icon_obj, int32_t *pivot_x, int32_t *pivot_y)
{
    if (!(snapshot_obj && pivot_x && pivot_y))
    {
        return false;
    }

    lv_area_t snapshot_area;
    lv_obj_get_coords(snapshot_obj, &snapshot_area);

    if (!icon_obj)
    {
        *pivot_x = lv_area_get_width(&snapshot_area) / 2;
        *pivot_y = lv_area_get_height(&snapshot_area) / 2;
        return false;
    }

    lv_area_t icon_area;
    lv_obj_get_coords(icon_obj, &icon_area);

    int32_t icon_mid_x = icon_area.x1 + (lv_area_get_width(&icon_area) / 2);
    int32_t icon_mid_y = icon_area.y1 + (lv_area_get_height(&icon_area) / 2);

    *pivot_x = icon_mid_x - snapshot_area.x1;
    *pivot_y = icon_mid_y - snapshot_area.y1;
    return true;
}

static void _app_list_set_transform_scale_cb(void *var, int32_t value)
{
    lv_obj_set_style_transform_scale((lv_obj_t *)var, value, 0);
}

static void _app_list_set_translate_x_cb(void *var, int32_t value)
{
    lv_obj_set_style_translate_x((lv_obj_t *)var, value, 0);
}

static void _app_list_set_translate_y_cb(void *var, int32_t value)
{
    lv_obj_set_style_translate_y((lv_obj_t *)var, value, 0);
}

static void _app_list_set_opa_cb(void *var, int32_t value)
{
    lv_obj_set_style_opa((lv_obj_t *)var, (lv_opa_t)value, 0);
}

static void _app_list_init_scale_anim(lv_anim_t *anim, lv_obj_t *obj, int32_t start, int32_t end, uint32_t duration)
{
    lv_anim_init(anim);
    lv_anim_set_var(anim, obj);
    lv_anim_set_values(anim, start, end);
    lv_anim_set_exec_cb(anim, _app_list_set_transform_scale_cb);
    lv_anim_set_path_cb(anim, lv_anim_path_ease_in_out);
    lv_anim_set_duration(anim, duration);
}

static void _app_list_init_translate_x_anim(lv_anim_t *anim, lv_obj_t *obj, int32_t start, int32_t end, uint32_t duration)
{
    lv_anim_init(anim);
    lv_anim_set_var(anim, obj);
    lv_anim_set_values(anim, start, end);
    lv_anim_set_exec_cb(anim, _app_list_set_translate_x_cb);
    lv_anim_set_path_cb(anim, lv_anim_path_ease_in_out);
    lv_anim_set_duration(anim, duration);
}

static void _app_list_init_translate_y_anim(lv_anim_t *anim, lv_obj_t *obj, int32_t start, int32_t end, uint32_t duration)
{
    lv_anim_init(anim);
    lv_anim_set_var(anim, obj);
    lv_anim_set_values(anim, start, end);
    lv_anim_set_exec_cb(anim, _app_list_set_translate_y_cb);
    lv_anim_set_path_cb(anim, lv_anim_path_ease_in_out);
    lv_anim_set_duration(anim, duration);
}

static void _app_list_init_opa_anim(lv_anim_t *anim, lv_obj_t *obj, int32_t start, int32_t end, uint32_t duration)
{
    lv_anim_init(anim);
    lv_anim_set_var(anim, obj);
    lv_anim_set_values(anim, start, end);
    lv_anim_set_exec_cb(anim, _app_list_set_opa_cb);
    lv_anim_set_path_cb(anim, lv_anim_path_ease_in_out);
    lv_anim_set_duration(anim, duration);
}

static void _app_list_play_transition_anim(lv_anim_timeline_t *at, eos_activity_t *from, eos_activity_t *to, bool opening)
{
    if (!(at && from && to))
    {
        return;
    }

    const char *focus_app_id = opening ? _app_list_get_launch_app_id(to) : _app_list_get_launch_app_id(from);
    lv_obj_t *list_view = opening ? eos_activity_get_view(from) : eos_activity_get_view(to);
    lv_obj_t *bubble_grid = _app_list_get_bubble_grid(opening ? from : to);
    lv_obj_t *focus_icon = NULL;
    if (bubble_grid && _app_list_last_click_index >= 0)
    {
        focus_icon = eos_bubble_get_icon_obj(bubble_grid, (uint32_t)_app_list_last_click_index);
    }
    LV_UNUSED(focus_app_id);

    /* Closing animation (APP -> APP_LIST) should not render app header on top. */
    bool include_header_in_snapshot = opening;
    lv_obj_t *app_snapshot = eos_activity_take_snapshot(opening ? to : from, include_header_in_snapshot);
    if (!app_snapshot)
    {
        return;
    }

    if (!opening && list_view)
    {
        lv_obj_remove_flag(list_view, LV_OBJ_FLAG_HIDDEN);
        lv_obj_move_foreground(list_view);
    }

    int32_t pivot_x = 0;
    int32_t pivot_y = 0;
    if (!_app_list_calc_focus_pivot_by_global_center(list_view ? list_view : app_snapshot, &pivot_x, &pivot_y))
    {
        _app_list_calc_focus_pivot(list_view ? list_view : app_snapshot, focus_icon, &pivot_x, &pivot_y);
    }

    if (list_view)
    {
        lv_obj_set_style_transform_pivot_x(list_view, pivot_x, 0);
        lv_obj_set_style_transform_pivot_y(list_view, pivot_y, 0);
    }
    lv_obj_set_style_transform_pivot_x(app_snapshot, pivot_x, 0);
    lv_obj_set_style_transform_pivot_y(app_snapshot, pivot_y, 0);

    EOS_LOG_D("Pivot(%d,%d)", pivot_x, pivot_y);

    uint32_t total_duration = (uint32_t)_APP_LIST_ANIM_DURATION;
    if (total_duration == 0U)
    {
        total_duration = 1U;
    }

    uint32_t split_delay = (total_duration * (uint32_t)_APP_LIST_ANIM_SPLIT_PCT) / 100U;
    if (split_delay >= total_duration)
    {
        split_delay = total_duration > 1U ? total_duration - 1U : 0U;
    }

    uint32_t from_scale_duration = total_duration;
    uint32_t to_duration = (total_duration > split_delay) ? (total_duration - split_delay) : 1U;

    int32_t focus_translate_x = 0;
    int32_t focus_translate_y = 0;
    if (_app_list_last_icon_center_valid && list_view)
    {
        int32_t view_center_x = EOS_DISPLAY_WIDTH / 2;
        int32_t view_center_y = EOS_DISPLAY_HEIGHT / 2;
        focus_translate_x = view_center_x - _app_list_last_icon_center_x;
        focus_translate_y = view_center_y - _app_list_last_icon_center_y;
    }

    lv_anim_t list_scale_anim;
    lv_anim_t list_translate_x_anim;
    lv_anim_t list_translate_y_anim;
    lv_anim_t icon_opa_anim;
    lv_anim_t app_scale_anim;
    lv_anim_t app_opa_anim;

    if (opening)
    {
        if (list_view)
        {
            lv_obj_set_style_transform_scale(list_view, 256, 0);
            lv_obj_set_style_translate_x(list_view, 0, 0);
            lv_obj_set_style_translate_y(list_view, 0, 0);
            _app_list_init_scale_anim(&list_scale_anim, list_view, 256, _APP_LIST_ANIM_FOCUS_SCALE, from_scale_duration);
            _app_list_init_translate_x_anim(&list_translate_x_anim, list_view, 0, focus_translate_x, from_scale_duration);
            _app_list_init_translate_y_anim(&list_translate_y_anim, list_view, 0, focus_translate_y, from_scale_duration);
            lv_anim_timeline_add(at, 0, &list_scale_anim);
            lv_anim_timeline_add(at, 0, &list_translate_x_anim);
            lv_anim_timeline_add(at, 0, &list_translate_y_anim);
        }

        if (focus_icon)
        {
            lv_obj_set_style_opa(focus_icon, (lv_opa_t)_APP_LIST_ANIM_FROM_OPA_START, 0);
            _app_list_init_opa_anim(&icon_opa_anim,
                                    focus_icon,
                                    _APP_LIST_ANIM_FROM_OPA_START,
                                    _APP_LIST_ANIM_FROM_OPA_END,
                                    total_duration);
            lv_anim_timeline_add(at, 0, &icon_opa_anim);
        }

        lv_obj_set_style_transform_scale(app_snapshot, _APP_LIST_ANIM_MIN_SACLE, 0);
        lv_obj_set_style_opa(app_snapshot, (lv_opa_t)_APP_LIST_ANIM_TO_OPA_START, 0);
        _app_list_init_scale_anim(&app_scale_anim, app_snapshot, _APP_LIST_ANIM_MIN_SACLE, 256, to_duration);
        _app_list_init_opa_anim(&app_opa_anim,
                                app_snapshot,
                                _APP_LIST_ANIM_TO_OPA_START,
                                _APP_LIST_ANIM_TO_OPA_END,
                                to_duration);
        lv_anim_timeline_add(at, split_delay, &app_scale_anim);
        lv_anim_timeline_add(at, split_delay, &app_opa_anim);
    }
    else
    {
        if (list_view)
        {
            lv_obj_set_style_transform_scale(list_view, _APP_LIST_ANIM_FOCUS_SCALE, 0);
            lv_obj_set_style_translate_x(list_view, focus_translate_x, 0);
            lv_obj_set_style_translate_y(list_view, focus_translate_y, 0);
            _app_list_init_scale_anim(&list_scale_anim, list_view, _APP_LIST_ANIM_FOCUS_SCALE, 256, to_duration);
            _app_list_init_translate_x_anim(&list_translate_x_anim, list_view, focus_translate_x, 0, to_duration);
            _app_list_init_translate_y_anim(&list_translate_y_anim, list_view, focus_translate_y, 0, to_duration);
            lv_anim_timeline_add(at, split_delay, &list_scale_anim);
            lv_anim_timeline_add(at, split_delay, &list_translate_x_anim);
            lv_anim_timeline_add(at, split_delay, &list_translate_y_anim);
        }

        if (focus_icon)
        {
            lv_obj_set_style_opa(focus_icon, (lv_opa_t)_APP_LIST_ANIM_FROM_OPA_END, 0);
            _app_list_init_opa_anim(&icon_opa_anim,
                                    focus_icon,
                                    _APP_LIST_ANIM_FROM_OPA_END,
                                    _APP_LIST_ANIM_FROM_OPA_START,
                                    total_duration);
            lv_anim_timeline_add(at, 0, &icon_opa_anim);
        }

        lv_obj_set_style_transform_scale(app_snapshot, 256, 0);
        lv_obj_set_style_opa(app_snapshot, (lv_opa_t)_APP_LIST_ANIM_TO_OPA_END, 0);
        _app_list_init_scale_anim(&app_scale_anim, app_snapshot, 256, _APP_LIST_ANIM_MIN_SACLE, to_duration);
        _app_list_init_opa_anim(&app_opa_anim,
                                app_snapshot,
                                _APP_LIST_ANIM_TO_OPA_END,
                                _APP_LIST_ANIM_TO_OPA_START,
                                to_duration);
        lv_anim_timeline_add(at, 0, &app_scale_anim);
        lv_anim_timeline_add(at, 0, &app_opa_anim);
    }
}

static void _app_list_on_resueme(eos_activity_t *a)
{
    // 初始化应用列表
    lv_obj_t *bubble_grid = _app_list_get_bubble_grid(a);
    EOS_CHECK_PTR_RETURN(bubble_grid);
    _app_list_refresh(bubble_grid);
}

/************************** 应用入口 **************************/
/**
 * @brief 应用点击事件回调（处理系统应用和脚本应用）
 * 从bubble_grid的LV_EVENT_CLICKED事件中获取应用ID
 */
static void _app_list_icon_clicked_cb(lv_event_t *e)
{
    lv_obj_t *bubble_grid = lv_event_get_current_target(e);
    EOS_CHECK_PTR_RETURN(bubble_grid);

    eos_bubble_click_event_t *click_event = (eos_bubble_click_event_t *)lv_event_get_param(e);
    EOS_CHECK_PTR_RETURN(click_event);

    const char *app_id = (const char *)click_event->icon_user_data;
    EOS_CHECK_PTR_RETURN(app_id);

    _app_list_set_last_launch_app_id(app_id);
    _app_list_last_click_index = (int32_t)click_event->index;

    lv_indev_t *indev = lv_indev_get_act();
    if (indev)
    {
        lv_point_t p;
        lv_indev_get_point(indev, &p);
        _app_list_record_icon_center_point(p.x, p.y);
    }

    // 检查是否为系统内置应用
    for (int i = 0; i < EOS_SYS_APP_LAST; i++)
    {
        if (strcmp(app_id, eos_sys_app_id_list[i]) == 0)
        {
            if (eos_sys_app_entry_list[i])
                eos_sys_app_entry_list[i]();
            return;
        }
    }

    // 脚本应用入口逻辑
    if (script_engine_get_state() != SCRIPT_STATE_STOPPED)
    {
        EOS_LOG_E("Another script running");
        return;
    }

    // 获取清单文件
    char manifest_path[PATH_MAX];
    snprintf(manifest_path, sizeof(manifest_path), EOS_APP_INSTALLED_DIR "%s/" EOS_APP_MANIFEST_FILE_NAME,
             app_id);
    script_pkg_t pkg = {0};

    pkg.type = SCRIPT_TYPE_APPLICATION;
    if (script_engine_get_manifest(manifest_path, &pkg) != SE_OK)
    {
        EOS_LOG_E("Read manifest failed: %s", manifest_path);
        eos_pkg_free(&pkg);
        return;
    }

    char script_path[PATH_MAX];
    snprintf(script_path, sizeof(script_path), EOS_APP_INSTALLED_DIR "%s/" EOS_APP_SCRIPT_ENTRY_FILE_NAME,
             app_id);

    // 设置脚本基础路径，用于解析相对路径的模块导入
    char base_path[PATH_MAX];
    snprintf(base_path, sizeof(base_path), EOS_APP_INSTALLED_DIR "%s/", app_id);
    pkg.base_path = eos_strdup(base_path);

    if (!eos_is_file(script_path))
    {
        EOS_LOG_E("Can't find script: %s", script_path);
        eos_pkg_free(&pkg);
        return;
    }

    pkg.script_str = eos_fs_read_file(script_path);

    app_launch_ctx_t *ctx = eos_malloc_zeroed(sizeof(app_launch_ctx_t));
    if (!ctx)
    {
        EOS_LOG_E("Failed to allocate app launch context");
        eos_pkg_free(&pkg);
        return;
    }

    ctx->pkg = pkg;
    ctx->app_id = eos_strdup(app_id);
    if (!ctx->app_id)
    {
        EOS_LOG_E("Failed to copy app id");
        eos_pkg_free(&ctx->pkg);
        eos_free(ctx);
        return;
    }

    // 创建脚本应用的 Activity
    eos_activity_t *a = eos_activity_create(&app_lifecycle);
    if (!a)
    {
        EOS_LOG_E("Failed to create activity");
        eos_pkg_free(&ctx->pkg);
        eos_free(ctx->app_id);
        eos_free(ctx);
        return;
    }

    lv_obj_t *app_view = eos_activity_get_view(a);
    lv_obj_set_size(app_view, EOS_DISPLAY_WIDTH, EOS_DISPLAY_HEIGHT);
    eos_activity_set_type(a, EOS_ACTIVITY_TYPE_APP);
    eos_activity_set_user_data(a, ctx);
    eos_activity_set_title(a, pkg.name);
    eos_activity_set_app_header_visible(a, true);

    EOS_LOG_D("view_size: %d, %d", lv_obj_get_width(app_view), lv_obj_get_height(app_view));

    // 进入应用页面
    eos_activity_enter(a);
}

static void _register_anim_routes_once(void)
{
    if (_anim_routes_registered)
    {
        return;
    }

    eos_activity_register_anim_route(EOS_ACTIVITY_TYPE_APP_LIST, EOS_ACTIVITY_TYPE_APP, _app_list_open_app_anim_cb);
    eos_activity_register_anim_route(EOS_ACTIVITY_TYPE_APP, EOS_ACTIVITY_TYPE_APP_LIST, _app_list_close_app_anim_cb);
    _anim_routes_registered = true;
}

/************************** 刷新应用列表 **************************/
/**
 * @brief 刷新应用列表 - 使用bubble_grid
 * @param bubble_grid 应用列表的bubble_grid对象
 */
static void _app_list_refresh(lv_obj_t *bubble_grid)
{
    if (!bubble_grid)
    {
        return;
    }

    // 清空上一次图标槽位，避免删除组件内部对象导致悬挂指针。
    for (uint32_t i = 0; i < _app_list_icon_count; ++i)
    {
        eos_bubble_set_icon_src(bubble_grid, i, NULL);
        eos_bubble_set_icon_user_data(bubble_grid, i, NULL);
    }

    uint32_t icon_index = 0;

    // 加载应用顺序
    char *json_str = eos_fs_read_file(EOS_APP_LIST_APP_ORDER_PATH);
    cJSON *app_order = json_str ? cJSON_Parse(json_str) : NULL;
    eos_free(json_str);

    // 按JSON顺序添加应用图标
    if (app_order)
    {
        cJSON *item = NULL;
        cJSON_ArrayForEach(item, app_order)
        {
            if (cJSON_IsString(item))
            {
                const char *order_id = item->valuestring;

                // 检查是否为系统内置应用
                bool is_sys = false;
                for (int si = 0; si < EOS_SYS_APP_LAST; si++)
                {
                    if (strcmp(order_id, eos_sys_app_id_list[si]) == 0)
                    {
                        eos_bubble_set_icon_src(bubble_grid, icon_index, eos_sys_app_icon_list[si]);
                        eos_bubble_set_icon_user_data(bubble_grid, icon_index, (void *)eos_sys_app_id_list[si]);
                        icon_index++;
                        is_sys = true;
                        break;
                    }
                }
                if (is_sys)
                    continue;

                // 非系统应用：从已安装列表中查找现有 id
                const char *app_id = eos_app_list_get_existing_id(order_id);
                if (!app_id)
                {
                    continue;
                }

                char icon_path[PATH_MAX];
                snprintf(icon_path, sizeof(icon_path), EOS_APP_INSTALLED_DIR "%s/" EOS_APP_ICON_FILE_NAME,
                         app_id);
                if (!eos_is_file(icon_path))
                {
                    snprintf(icon_path, sizeof(icon_path), "%s", EOS_IMG_APP);
                }
                eos_bubble_set_icon_src(bubble_grid, icon_index, icon_path);
                eos_bubble_set_icon_user_data(bubble_grid, icon_index, (void *)app_id);
                icon_index++;
            }
        }
        cJSON_Delete(app_order);
    }
    else
    {
        // 如果没有JSON顺序文件，按默认顺序添加
        size_t app_list_size = eos_app_get_installed();
        for (size_t i = 0; i < app_list_size; i++)
        {
            const char *app_id = eos_app_list_get_id(i);
            if (!app_id)
                continue;

            // 系统内置应用使用内置图标
            bool is_sys = false;
            for (int si = 0; si < EOS_SYS_APP_LAST; si++)
            {
                if (strcmp(app_id, eos_sys_app_id_list[si]) == 0)
                {
                    eos_bubble_set_icon_src(bubble_grid, icon_index, eos_sys_app_icon_list[si]);
                    eos_bubble_set_icon_user_data(bubble_grid, icon_index, (void *)eos_sys_app_id_list[si]);
                    icon_index++;
                    is_sys = true;
                    break;
                }
            }
            if (is_sys)
                continue;

            // 非系统应用
            char icon_path[PATH_MAX];
            snprintf(icon_path, sizeof(icon_path), EOS_APP_INSTALLED_DIR "%s/" EOS_APP_ICON_FILE_NAME,
                     app_id);
            if (!eos_is_file(icon_path))
            {
                snprintf(icon_path, sizeof(icon_path), "%s", EOS_IMG_APP);
            }
            eos_bubble_set_icon_src(bubble_grid, icon_index, icon_path);
            eos_bubble_set_icon_user_data(bubble_grid, icon_index, (void *)app_id);
            icon_index++;
        }
    }

    _app_list_icon_count = icon_index;
}

/************************** 动画 **************************/

static void _app_list_open_app_anim_cb(lv_anim_timeline_t *at, eos_activity_t *from, eos_activity_t *to)
{
    _app_list_play_transition_anim(at, from, to, true);
}

static void _app_list_close_app_anim_cb(lv_anim_timeline_t *at, eos_activity_t *from, eos_activity_t *to)
{
    _app_list_play_transition_anim(at, from, to, false);
}

/************************** 辅助函数 **************************/

/**
 * @brief 当应用安装时自动调用此回调以便显示新的应用
 */
static void _app_installed_cb(lv_event_t *e)
{
    lv_obj_t *bubble_grid = lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(bubble_grid);
    _app_list_refresh(bubble_grid);
}

static void _container_delete_cb(lv_event_t *e)
{
    lv_obj_t *bubble_grid = lv_event_get_target(e);
    EOS_CHECK_PTR_RETURN(bubble_grid);
    eos_event_remove_cb(bubble_grid, EOS_EVENT_APP_INSTALLED, _app_installed_cb);
}

void eos_app_list_enter(void)
{
    _register_anim_routes_once();
    _app_list_icon_count = 0;

    eos_activity_t *a = eos_activity_create(&app_list_lifecycle);
    if (!a)
    {
        EOS_LOG_E("Failed to create activity");
        return;
    }
    eos_activity_set_type(a, EOS_ACTIVITY_TYPE_APP_LIST);

    lv_obj_t *view = eos_activity_get_view(a);
    lv_obj_set_size(view, lv_pct(100), lv_pct(100));

    // 创建bubble_grid作为应用列表容器
    lv_obj_t *bubble_grid = eos_bubble_create(view);
    if (!bubble_grid)
    {
        EOS_LOG_E("Failed to create bubble_grid");
        eos_activity_back();
        return;
    }

    // 设置bubble_grid大小和位置
    lv_obj_set_size(bubble_grid, EOS_DISPLAY_WIDTH, EOS_DISPLAY_HEIGHT);
    lv_obj_center(bubble_grid);
    eos_activity_set_user_data(a, bubble_grid);

    // 注册点击事件回调
    lv_obj_add_event_cb(bubble_grid, _app_list_icon_clicked_cb, LV_EVENT_CLICKED, NULL);

    // 设置回调
    lv_obj_add_event_cb(bubble_grid, _container_delete_cb, LV_EVENT_DELETE, NULL);
    eos_event_add_cb(bubble_grid, _app_installed_cb, EOS_EVENT_APP_INSTALLED, (void *)bubble_grid);

    // 刷新应用列表
    _app_list_refresh(bubble_grid);

    eos_activity_enter(a);
}
