/**
 * @file elena_os_app_list.c
 * @brief 应用列表页面
 * @author Sab1e
 * @date 2025-08-21
 */

#include "elena_os_app_list.h"

// Includes
#include <stdio.h>
#include <stdlib.h>
#include "lvgl.h"
#include "cJSON.h"
#include "elena_os_nav.h"
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
// Macros and Definitions

// Variables
extern lv_group_t *encoder_group;
static lv_obj_t *app_list_screen = NULL;
// Function Implementations
EOS_DEFINE_SCREEN_ASYNC(eos_app_list_create);
static void _app_list_icon_clicked_cb(lv_event_t *e);
static void _app_list_settings_cb(lv_event_t *e);
static lv_obj_t *_app_icon_create(lv_obj_t *parent, const char *icon_path);
static void _app_installed_cb(lv_event_t *e);
static void _container_delete_cb(lv_event_t *e);

/**
 * @brief 刷新应用列表
 * @param container 应用列表的容器对象
 */
static void _app_list_refresh(lv_obj_t *container)
{
    lv_obj_clean(container);
    // 加载应用顺序
    char *json_str = eos_read_file(EOS_APP_LIST_APP_ORDER_PATH);
    cJSON *app_order = json_str ? cJSON_Parse(json_str) : NULL;
    eos_free_large(json_str);
    // 按JSON顺序添加其他应用
    if (app_order)
    {
        cJSON *item = NULL;
        cJSON_ArrayForEach(item, app_order)
        {
            if (cJSON_IsString(item))
            {
                if (strcmp(item->valuestring, "sys.settings") == 0)
                {
                    char icon_path[PATH_MAX];
                    memcpy(icon_path, EOS_IMG_SETTINGS, sizeof(EOS_IMG_SETTINGS));
                    lv_obj_t *settings_icon = _app_icon_create(container, icon_path);
                    lv_obj_add_event_cb(settings_icon, _app_list_settings_cb, LV_EVENT_CLICKED, NULL);
                    // 设置系统应用的ID
                    lv_obj_set_user_data(settings_icon, (void *)"sys.settings");
                    continue;
                }

                const char *app_id = eos_app_list_get_existing_id(item->valuestring);
                ;

                // 检查应用是否存在
                if (!eos_app_list_contains(app_id))
                {
                    continue;
                }

                char icon_path[PATH_MAX];
                snprintf(icon_path, sizeof(icon_path), EOS_APP_INSTALLED_DIR "%s/" EOS_APP_ICON_FILE_NAME,
                         app_id);
                if (!eos_is_file(icon_path))
                {
                    memcpy(icon_path, EOS_IMG_APP, sizeof(EOS_IMG_APP));
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
        // 如果没有JSON顺序文件，按默认顺序添加
        size_t app_list_size = eos_app_list_size();
        for (size_t i = 0; i < app_list_size; i++)
        {
            const char *app_id = eos_app_list_get_id(i);
            char icon_path[PATH_MAX];
            snprintf(icon_path, sizeof(icon_path), EOS_APP_INSTALLED_DIR "%s/" EOS_APP_ICON_FILE_NAME,
                     app_id);
            if (!eos_is_file(icon_path))
            {
                memcpy(icon_path, EOS_IMG_APP, sizeof(EOS_IMG_APP));
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
    if (script_engine_get_state() != SCRIPT_STATE_STOPPED)
    {
        EOS_LOG_E("Another script running");
        return;
    }
    const char *app_id = (const char *)lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(app_id);

    // 获取清单文件
    char manifest_path[PATH_MAX];
    snprintf(manifest_path, sizeof(manifest_path), EOS_APP_INSTALLED_DIR "%s/" EOS_APP_MANIFEST_FILE_NAME,
             app_id);
    script_pkg_t *pkg = malloc(sizeof(script_pkg_t));
    memset((void *)pkg, 0, sizeof(script_pkg_t));
    pkg->type = SCRIPT_TYPE_APPLICATION;
    if (script_engine_get_manifest(manifest_path, pkg) != SE_OK)
    {
        EOS_LOG_E("Read manifest failed: %s", manifest_path);
        return;
    }
    EOS_LOG_D("App Info:\n"
              "id=%s | name=%s | version=%s |\n"
              "author:%s | description:%s",
              pkg->id, pkg->name, pkg->version,
              pkg->author, pkg->description);
    char script_path[PATH_MAX];
    snprintf(script_path, sizeof(script_path), EOS_APP_INSTALLED_DIR "%s/" EOS_APP_SCRIPT_ENTRY_FILE_NAME,
             app_id);
    if (!eos_is_file(script_path))
    {
        EOS_LOG_E("Can't find script: %s", script_path);
        return;
    }

    pkg->script_str = eos_read_file(script_path);
    // 无需清理字符串，脚本运行结束后自动清理
    lv_obj_t *scr = eos_nav_init();
    eos_screen_bind_header(scr, pkg->name);
    lv_screen_load(scr);
    script_engine_result_t ret = script_engine_run(pkg);
    if (ret != SE_OK)
    {
        eos_nav_clean_up();
        eos_app_list_create();
        EOS_LOG_E("Script encounter a fatal error");
        // lv_obj_t *mbox = lv_msgbox_create(NULL);
        // lv_obj_set_size(mbox, lv_pct(90),lv_pct(90));
        // lv_msgbox_add_title(mbox, "Scrip Runtime");

        // lv_msgbox_add_text(mbox, current_lang[STR_ID_SCRIPT_RUN_ERR]);
        // lv_msgbox_add_close_button(mbox);
    }
    EOS_LOG_D("Script OK");
}

/**
 * @brief 设置图标按下时的回调
 */
static void _app_list_settings_cb(lv_event_t *e)
{
    eos_sys_settings_create();
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
    eos_event_remove_cb(container, eos_event_get_code(EOS_EVENT_APP_INSTALLED), _app_installed_cb);
}

lv_obj_t *eos_app_list_get_screen(void)
{
    return app_list_screen;
}

EOS_DECLARE_SCREEN_ASYNC(eos_app_list_create)
{
    if (app_list_screen)
    {
        lv_obj_del(app_list_screen);
    }
    app_list_screen = lv_obj_create(NULL);
    // 创建新的页面用于绘制应用列表
    lv_screen_load(app_list_screen);

    lv_obj_t *container = lv_list_create(app_list_screen);
    lv_obj_set_style_pad_all(container, 20, 0);
    lv_obj_set_style_pad_column(container, 20, 0); // 列间距
    lv_obj_set_style_pad_row(container, 20, 0);
    lv_obj_set_size(container, lv_pct(100), lv_pct(100));
    lv_obj_set_scroll_dir(container, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(container, LV_SCROLLBAR_MODE_OFF);
    lv_obj_center(container);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(container,
                          LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_START);
    lv_obj_add_event_cb(container, _container_delete_cb, LV_EVENT_DELETE, NULL);
    eos_event_add_cb(container, _app_installed_cb, eos_event_get_code(EOS_EVENT_APP_INSTALLED), (void *)container);

    _app_list_refresh(container);
}
