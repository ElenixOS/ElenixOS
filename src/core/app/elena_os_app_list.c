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
#include "elena_os_nav.h"
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

/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/
extern lv_group_t *encoder_group;
static lv_obj_t *app_list_screen = NULL;


const char *eos_sys_app_id_list[EOS_SYS_APP_LAST] = {
    "sys.settings",
    "sys.flash_light"
};

const char *eos_sys_app_icon_list[EOS_SYS_APP_LAST] = {
    EOS_IMG_SETTINGS,
    EOS_IMG_FLASH_LIGHT
};


const eos_sys_app_entry_t eos_sys_app_entry_list[EOS_SYS_APP_LAST] = {
    eos_settings_create,
    eos_flash_light_create
};


/* Function Implementations -----------------------------------*/
static void _app_list_icon_clicked_cb(lv_event_t *e);
static void _app_list_settings_cb(lv_event_t *e);
static lv_obj_t *_app_icon_create(lv_obj_t *parent, const char *icon_path);
static void _app_installed_cb(lv_event_t *e);
static void _container_delete_cb(lv_event_t *e);

/**
 * @brief 系统内置应用点击回调
 */
static void _app_list_sys_app_cb(lv_event_t *e)
{
    const char *app_id = (const char *)lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(app_id);

    for (int i = 0; i < EOS_SYS_APP_LAST; i++)
    {
        if (strcmp(app_id, eos_sys_app_id_list[i]) == 0)
        {
            // 调用系统内置应用的入口函数
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
        size_t app_list_size = eos_app_list_size();
        for (size_t i = 0; i < app_list_size; i++)
        {
            const char *app_id = eos_app_list_get_id(i);
            if (!app_id) continue;

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
    script_pkg_t *pkg = eos_malloc_zeroed(sizeof(script_pkg_t));
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
    lv_obj_t *scr = eos_nav_init(app_list_screen);
    eos_screen_bind_header(scr, pkg->name);
    eos_screen_load(scr);
    script_engine_result_t ret = script_engine_run(pkg);
    if (ret != SE_OK)
    {
        eos_nav_clean_up();
        eos_scene_switch(EOS_SCENE_APP_LIST);
        EOS_LOG_E("Script encounter a fatal error");
        // TODO: 错误处理
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
    eos_settings_create();
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
    eos_event_remove_cb(container, EOS_EVENT_APP_INSTALLED, _app_installed_cb);
}

lv_obj_t *eos_app_list_get_screen(void)
{
    return app_list_screen;
}

void eos_app_list_create(void)
{
    if (app_list_screen)
    {
        lv_obj_delete(app_list_screen);
    }
    app_list_screen = eos_screen_create();
    // 创建新的页面用于绘制应用列表
    eos_screen_load(app_list_screen);

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
    eos_event_add_cb(container, _app_installed_cb, EOS_EVENT_APP_INSTALLED, (void *)container);

    _app_list_refresh(container);
}
