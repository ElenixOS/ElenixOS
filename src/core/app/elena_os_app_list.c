/**
 * @file elena_os_app_list.c
 * @brief 应用列表页面
 * @author Sab1e
 * @date 2025-08-21
 */

#include "elena_os_app_list.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "lvgl.h"
#include "cJSON.h"
#define EOS_LOG_DISABLE
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
#include "elena_os_scene.h"
#include "elena_os_fs.h"
#include "elena_os_app_header.h"
#include "elena_os_screen_mgr.h"
#include "elena_os_mem.h"
#include "elena_os_crown.h"
#include "elena_os_theme.h"
#include "elena_os_icon.h"
#include "elena_os_font.h"
#include "elena_os_std_widgets.h"
#include "elena_os_activity.h"

/* Macros and Definitions -------------------------------------*/
#define _APP_ICON_ANIM_DURATION 250 * EOS_ANIM_PLAY_COEFFICIENT
#define _APP_ICON_ANIM_DELAY 75 * EOS_ANIM_PLAY_COEFFICIENT
/* Variables --------------------------------------------------*/
static lv_obj_t *last_clicked_icon = NULL;
static bool needs_reset_anim = false;

const char *eos_sys_app_id_list[EOS_SYS_APP_LAST] = {
    "sys.settings",
    "sys.flash_light"};

const char *eos_sys_app_icon_list[EOS_SYS_APP_LAST] = {
    EOS_IMG_SETTINGS,
    EOS_IMG_FLASH_LIGHT};

const eos_sys_app_entry_t eos_sys_app_entry_list[EOS_SYS_APP_LAST] = {
    eos_settings_create,
    eos_flash_light_create};

/* Function Implementations -----------------------------------*/
static void _app_list_icon_clicked_cb(lv_event_t *e);
static void _app_list_settings_cb(lv_event_t *e);
static lv_obj_t *_app_icon_create(lv_obj_t *parent, const char *icon_path);
static void _app_installed_cb(lv_event_t *e);
static void _container_delete_cb(lv_event_t *e);
static void _app_list_play_icon_anim(lv_obj_t *obj, bool reverse);
static void _app_list_sys_app_async_cb(void *user_data);

/**
 * @brief 系统内置应用点击回调
 */
static void _app_list_sys_app_cb(lv_event_t *e)
{
    const char *app_id = (const char *)lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(app_id);

    lv_async_call(_app_list_sys_app_async_cb, (void *)app_id);
}

static void _app_list_sys_app_async_cb(void *user_data)
{
    const char *app_id = (const char *)user_data;
    EOS_CHECK_PTR_RETURN(app_id);

    for (int i = 0; i < EOS_SYS_APP_LAST; i++)
    {
        if (strcmp(app_id, eos_sys_app_id_list[i]) == 0)
        {
            if (eos_sys_app_entry_list[i])
                eos_sys_app_entry_list[i]();
            return;
        }
    }
}

/**
 * @brief 刷新应用列表
 * @param container 应用列表的容器对象
 */
static void _app_list_refresh(lv_obj_t *container)
{
    lv_obj_clean(container);
    // 加载应用顺序
    char *json_str = eos_fs_read_file(EOS_APP_LIST_APP_ORDER_PATH);
    cJSON *app_order = json_str ? cJSON_Parse(json_str) : NULL;
    eos_free(json_str);
    // 按JSON顺序添加其他应用
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
                        char icon_path[PATH_MAX];
                        snprintf(icon_path, sizeof(icon_path), "%s", eos_sys_app_icon_list[si]);
                        lv_obj_t *sys_icon = _app_icon_create(container, icon_path);
                        lv_obj_add_event_cb(sys_icon, _app_list_sys_app_cb, LV_EVENT_CLICKED, (void *)eos_sys_app_id_list[si]);
                        lv_obj_set_user_data(sys_icon, (void *)eos_sys_app_id_list[si]);
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
                lv_obj_t *app_icon = _app_icon_create(container, icon_path);
                lv_obj_add_event_cb(app_icon, _app_list_icon_clicked_cb, LV_EVENT_CLICKED, (void *)app_id);
                eos_app_obj_auto_delete(app_icon, app_id);
            }
        }
        cJSON_Delete(app_order);
    }
    else
    {
        // 如果没有JSON顺序文件，按默认顺序添加（app_list 已包含系统应用与已安装应用）
        size_t app_list_size = eos_app_get_installed();
        for (size_t i = 0; i < app_list_size; i++)
        {
            const char *app_id = eos_app_list_get_id(i);
            if (!app_id)
                continue;

            // 系统内置应用使用内置图标与入口
            bool is_sys = false;
            for (int si = 0; si < EOS_SYS_APP_LAST; si++)
            {
                if (strcmp(app_id, eos_sys_app_id_list[si]) == 0)
                {
                    char icon_path[PATH_MAX];
                    snprintf(icon_path, sizeof(icon_path), "%s", eos_sys_app_icon_list[si]);
                    lv_obj_t *sys_icon = _app_icon_create(container, icon_path);
                    lv_obj_add_event_cb(sys_icon, _app_list_sys_app_cb, LV_EVENT_CLICKED, (void *)eos_sys_app_id_list[si]);
                    lv_obj_set_user_data(sys_icon, (void *)eos_sys_app_id_list[si]);
                    is_sys = true;
                    break;
                }
            }
            if (is_sys)
                continue;

            // 非系统应用：从安装目录读取 icon
            char icon_path[PATH_MAX];
            snprintf(icon_path, sizeof(icon_path), EOS_APP_INSTALLED_DIR "%s/" EOS_APP_ICON_FILE_NAME,
                     app_id);
            if (!eos_is_file(icon_path))
            {
                snprintf(icon_path, sizeof(icon_path), "%s", EOS_IMG_APP);
            }
            lv_obj_t *app_icon = _app_icon_create(container, icon_path);
            lv_obj_add_event_cb(app_icon, _app_list_icon_clicked_cb, LV_EVENT_CLICKED, (void *)app_id);
            eos_app_obj_auto_delete(app_icon, app_id);
        }
    }
}

/**
 * @brief 应用图标按下后的回调
 */
static void _app_list_icon_clicked_cb(lv_event_t *e)
{
    const char *app_id = (const char *)lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(app_id);

    eos_activity_t *a = eos_activity_create(NULL, NULL);
    lv_obj_t *app_view = eos_activity_get_view(a);

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
    lv_obj_t *scr = NULL;
    eos_app_header_bind_screen(scr, pkg.name);
    eos_screen_load(scr);
    // 进入应用页面
    eos_activity_enter(a);
    script_engine_result_t ret = script_engine_run(&pkg);
    if (ret != SE_OK)
    {
        lv_obj_clean(app_view);
        lv_obj_remove_style_all(app_view);
        eos_app_header_bind_screen_str_id(app_view, STR_ID_ERROR);
        eos_app_header_set_title_color_once(EOS_COLOR_RED);
        lv_obj_t *list = eos_std_info_create(
            app_view,
            EOS_COLOR_RED,
            RI_BUG_LINE,
            current_lang[STR_ID_APP_RUN_ERR_TITLE],
            current_lang[STR_ID_APP_RUN_ERR]);
        char info_str[1024];
        snprintf(info_str, sizeof(info_str), "Code: %d\nAppID: %s\nError: %s", ret, app_id, script_engine_get_error_info());
        lv_obj_t *err_label = eos_list_add_comment(list, info_str);
        lv_obj_t *btn = eos_button_create(list, current_lang[STR_ID_BACK], eos_activity_back_cb, NULL);
        EOS_LOG_E("Application encounter a fatal error");
    }

    eos_pkg_free(&pkg);
}

/**
 * @brief 设置图标按下时的回调
 */
static void _app_list_settings_cb(lv_event_t *e)
{
    eos_settings_create();
}

void eos_app_list_get_clicked_icon_center_pos(lv_coord_t *x, lv_coord_t *y)
{
    eos_obj_get_coord_center(last_clicked_icon, x, y);
}

static void _app_list_play_icon_anim(lv_obj_t *obj, bool reverse)
{
    lv_coord_t x, y;
    eos_app_list_get_clicked_icon_center_pos(&x, &y);
    eos_screen_set_last_clicked_point(x, y);
    int32_t scale_start, scale_end;
    lv_opa_t fade_start, fade_end;
    uint32_t delay;
    if (reverse)
    {
        scale_start = 1500;
        scale_end = 256;
        fade_start = LV_OPA_TRANSP;
        fade_end = LV_OPA_COVER;
        needs_reset_anim = false;
        delay = _APP_ICON_ANIM_DELAY;
    }
    else
    {
        scale_start = 256;
        scale_end = 1500;
        fade_start = LV_OPA_COVER;
        fade_end = LV_OPA_TRANSP;
        needs_reset_anim = true;
        delay = 0;
    }
    // lv_obj_set_style_transform_pivot_x(app_list_screen, x, 0);
    // lv_obj_set_style_transform_pivot_y(app_list_screen, y, 0);
    // eos_lite_anim_transform_scale_start(app_list_screen,
    //                                     scale_start, scale_end,
    //                                     _APP_ICON_ANIM_DURATION, delay,
    //                                     NULL, NULL);
    eos_lite_anim_fade_layered_start(obj,
                                     fade_start, fade_end,
                                     _APP_ICON_ANIM_DURATION, delay,
                                     NULL, NULL);
}

static void _app_icon_clicked_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    last_clicked_icon = obj;
    _app_list_play_icon_anim(obj, false);
}

/**
 * @brief 创建应用图标
 */
static lv_obj_t *_app_icon_create(lv_obj_t *parent, const char *icon_path)
{
    lv_obj_t *app_icon = lv_image_create(parent);
    lv_obj_set_size(app_icon, 100, 100);
    lv_obj_set_style_shadow_width(app_icon, 0, 0);
    lv_obj_set_style_margin_all(app_icon, 0, 0);
    lv_obj_set_style_pad_all(app_icon, 0, 0);
    lv_obj_add_flag(app_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(app_icon, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    eos_img_set_src(app_icon, icon_path);
    eos_img_set_size(app_icon, 100, 100);
    lv_obj_center(app_icon);
    lv_obj_add_event_cb(app_icon, _app_icon_clicked_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_update_layout(app_icon);
    lv_obj_set_style_transform_pivot_x(app_icon, lv_obj_get_width(app_icon) / 2, 0);
    lv_obj_set_style_transform_pivot_y(app_icon, lv_obj_get_height(app_icon) / 2, 0);
    lv_obj_set_style_transform_scale(app_icon, 230, LV_STATE_PRESSED);
    lv_obj_set_style_image_recolor(app_icon, EOS_COLOR_BLACK, LV_STATE_PRESSED);
    lv_obj_set_style_image_recolor_opa(app_icon, LV_OPA_40, LV_STATE_PRESSED);

    return app_icon;
}

/**
 * @brief 当应用安装时自动调用此回调以便显示新的应用
 */
static void _app_installed_cb(lv_event_t *e)
{
    lv_obj_t *container = lv_event_get_user_data(e);
    _app_list_refresh(container);
}

static void _container_delete_cb(lv_event_t *e)
{
    lv_obj_t *container = lv_event_get_target(e);
    EOS_CHECK_PTR_RETURN(container);
    eos_event_remove_cb(container, EOS_EVENT_APP_INSTALLED, _app_installed_cb);
}

static void _screen_loaded_cb(lv_event_t *e)
{
    if (needs_reset_anim)
    {
        _app_list_play_icon_anim(last_clicked_icon, true);
    }
    // eos_crown_encoder_set_target_obj(app_list_screen);
}

eos_activity_t *eos_app_list_create(void)
{
    eos_activity_t *a = eos_activity_create(NULL, NULL);
    if (!a)
    {
        EOS_LOG_E("Failed to create activity");
        return NULL;
    }

    lv_obj_t *container = eos_activity_get_view(a);
    lv_obj_set_style_pad_all(container, 20, 0);
    lv_obj_set_style_pad_column(container, 20, 0); // 列间距
    lv_obj_set_style_pad_row(container, 20, 0);
    lv_obj_set_size(container, lv_pct(100), lv_pct(100));
    lv_obj_set_scroll_dir(container, LV_DIR_VER);
    lv_obj_center(container);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(container,
                          LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_START);
    lv_obj_add_event_cb(container, _container_delete_cb, LV_EVENT_DELETE, NULL);
    eos_event_add_cb(container, _app_installed_cb, EOS_EVENT_APP_INSTALLED, (void *)container);

    _app_list_refresh(container);
    // lv_obj_add_event_cb(container, _screen_loaded_cb, LV_EVENT_SCREEN_LOADED, NULL);
    return a;
}
