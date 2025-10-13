/**
 * @file elena_os_settings.c
 * @brief 设置页面
 * @author Sab1e
 * @date 2025-09-24
 */

#include "elena_os_settings.h"

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "lvgl.h"
#include "cJSON.h"
#include "elena_os_img.h"
#include "elena_os_msg_list.h"
#include "elena_os_lang.h"
#include "elena_os_log.h"
#include "elena_os_nav.h"
#include "elena_os_basic_widgets.h"
#include "elena_os_event.h"
#include "elena_os_test.h"
#include "elena_os_version.h"
#include "elena_os_port.h"
#include "elena_os_swipe_panel.h"
#include "elena_os_app.h"
#include "elena_os_watchface.h"
#include "elena_os_misc.h"
#include "elena_os_theme.h"
#include "elena_os_pkg_mgr.h"
#include "elena_os_sensor.h"
#include "elena_os_config.h"
#include "elena_os_icon.h"

// Macros and Definitions

// Variables

// Function Implementations

/************************** 蓝牙 **************************/
static void _bluetooth_enable_switch_cb(lv_event_t *e)
{
    lv_obj_t *bt_sw = lv_event_get_target(e);
    EOS_CHECK_PTR_RETURN(bt_sw);
    if (lv_obj_has_state(bt_sw, LV_STATE_CHECKED))
    {
        eos_bluetooth_enable();
        eos_sys_cfg_set_bool(EOS_SYS_CFG_KEY_BLUETOOTH, true);
    }
    else
    {
        eos_bluetooth_disable();
        eos_sys_cfg_set_bool(EOS_SYS_CFG_KEY_BLUETOOTH, false);
    }
}

static void _settings_screen_bluetooth(lv_event_t *e)
{
    lv_obj_t *scr = eos_nav_scr_create();
    eos_screen_bind_header_str_id(scr, STR_ID_SETTINGS_BLUETOOTH);
    lv_screen_load(scr);

    lv_obj_t *list = eos_list_create(scr);

    lv_obj_t *bt_sw = eos_list_add_switch(list, current_lang[STR_ID_SETTINGS_BLUETOOTH_ENABLE]);
    lv_obj_set_state(bt_sw, LV_STATE_CHECKED, eos_sys_cfg_get_bool(EOS_SYS_CFG_KEY_BLUETOOTH, false));
    lv_obj_add_event_cb(bt_sw, _bluetooth_enable_switch_cb, LV_EVENT_VALUE_CHANGED, NULL);
}
/************************** 显示设置 **************************/
static void _brightness_slider_value_changed_cb(lv_event_t *e)
{
    lv_obj_t *sl = lv_event_get_target(e);
    eos_display_set_brightness(lv_slider_get_value(sl));
}

static void _brightness_slider_released_cb(lv_event_t *e)
{
    lv_obj_t *sl = lv_event_get_target(e);
    int32_t val = lv_slider_get_value(sl);
    eos_display_set_brightness(val);
    eos_sys_cfg_set_number(EOS_SYS_CFG_KEY_DISPLAY_BRIGHTNESS, val);
}

static void _list_slider_minus_cb(lv_event_t *e)
{
    lv_obj_t *slider = (lv_obj_t *)lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(slider);
    int32_t min = lv_slider_get_min_value(slider);
    int32_t val = lv_slider_get_value(slider);
    if (val == min)
        return;
    val -= 5;
    lv_slider_set_value(slider, val, LV_ANIM_ON);
    eos_sys_cfg_set_number(EOS_SYS_CFG_KEY_DISPLAY_BRIGHTNESS, val);
    eos_display_set_brightness(val);
}

static void _list_slider_plus_cb(lv_event_t *e)
{
    lv_obj_t *slider = (lv_obj_t *)lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(slider);
    int32_t max = lv_slider_get_max_value(slider);
    int32_t val = lv_slider_get_value(slider);
    if (val == max)
        return;
    val += 5;
    lv_slider_set_value(slider, val, LV_ANIM_ON);
    eos_sys_cfg_set_number(EOS_SYS_CFG_KEY_DISPLAY_BRIGHTNESS, val);
    eos_display_set_brightness(val);
}

static void _settings_screen_display(lv_event_t *e)
{
    lv_obj_t *scr = eos_nav_scr_create();
    eos_screen_bind_header_str_id(scr, STR_ID_SETTINGS_DISPLAY);
    lv_screen_load(scr);

    lv_obj_t *list = eos_list_create(scr);

    eos_list_slider_t *brightness_slider = eos_list_add_slider(list, current_lang[STR_ID_SETTINGS_DISPLAY_BRIGHTNESS]);
    lv_slider_set_value(brightness_slider->slider, eos_sys_cfg_get_number(EOS_SYS_CFG_KEY_DISPLAY_BRIGHTNESS, 50), LV_ANIM_ON);
    lv_slider_set_range(brightness_slider->slider, EOS_DISPLAY_BRIGHTNESS_MIN, EOS_DISPLAY_BRIGHTNESS_MAX);
    lv_obj_add_event_cb(brightness_slider->slider, _brightness_slider_value_changed_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(brightness_slider->slider, _brightness_slider_released_cb, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(brightness_slider->minus_btn, _list_slider_minus_cb, LV_EVENT_CLICKED, brightness_slider->slider);
    lv_obj_add_event_cb(brightness_slider->plus_btn, _list_slider_plus_cb, LV_EVENT_CLICKED, brightness_slider->slider);
}
/************************** 通知 **************************/
static void _settings_screen_notification(lv_event_t *e)
{
    lv_obj_t *scr = eos_nav_scr_create();
    eos_screen_bind_header_str_id(scr, STR_ID_SETTINGS_DISPLAY);
    lv_screen_load(scr);
}
/************************** 应用列表 **************************/

/**
 * @brief 卸载按钮回调
 */
static void _uninstall_btn_cb(lv_event_t *e)
{
    const char *app_id = (const char *)lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(app_id);
    eos_app_uninstall(app_id);
    eos_nav_back_clean();
}

static void _clear_data_btn_cb(lv_event_t *e)
{
    const char *app_id = (const char *)lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(app_id);
    char data_path[PATH_MAX];
    snprintf(data_path, sizeof(data_path), EOS_APP_DATA_DIR "%s", app_id);
    eos_rm_recursive(data_path);
}

/**
 * @brief 应用列表回调，打开应用详情
 * @param e
 */
static void _settings_app_list_btn_cb(lv_event_t *e)
{
    const char *app_id = (const char *)lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(app_id);

    // 获取清单文件
    char manifest_path[PATH_MAX];
    snprintf(manifest_path, sizeof(manifest_path), EOS_APP_INSTALLED_DIR "%s/" EOS_APP_MANIFEST_FILE_NAME,
             app_id);
    script_pkg_t pkg = {0};
    if (script_engine_get_manifest(manifest_path, &pkg) != SE_OK)
    {
        EOS_LOG_E("Read manifest failed: %s", manifest_path);
        return;
    }
    EOS_LOG_D("App Info:\n"
              "id=%s | name=%s | version=%s |\n"
              "author:%s | description:%s",
              pkg.id, pkg.name, pkg.version,
              pkg.version, pkg.description);
    char script_path[PATH_MAX];
    snprintf(script_path, sizeof(script_path), EOS_APP_INSTALLED_DIR "%s/" EOS_APP_SCRIPT_ENTRY_FILE_NAME,
             app_id);
    if (!eos_is_file(script_path))
    {
        EOS_LOG_E("Can't find script: %s", script_path);
        return;
    }

    // 创建新的页面用于绘制应用详情页
    lv_obj_t *scr = eos_nav_scr_create();
    eos_screen_bind_header(scr, pkg.name);
    lv_screen_load(scr);

    lv_obj_t *list = lv_list_create(scr);
    lv_obj_set_size(list, lv_pct(100), lv_pct(100));
    lv_obj_set_style_pad_hor(list, 20, 0);
    lv_obj_set_style_pad_ver(list, 0, 0);
    lv_obj_center(list);
    lv_obj_set_scrollbar_mode(list, LV_SCROLLBAR_MODE_OFF);

    eos_list_add_placeholder(list, 110);

    lv_obj_t *container = eos_list_add_container(list);
    lv_obj_set_size(container, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(container,
                          LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_CENTER);

    char icon_path[PATH_MAX];
    snprintf(icon_path, sizeof(icon_path), EOS_APP_INSTALLED_DIR "%s/" EOS_APP_ICON_FILE_NAME,
             app_id);
    if (!eos_is_file(icon_path))
    {
        memcpy(icon_path, EOS_IMG_APP, sizeof(EOS_IMG_APP));
    }
    eos_row_create(container, NULL, pkg.name, icon_path, 100, 100);

    eos_row_create(container, current_lang[STR_ID_SETTINGS_APPS_APPID], app_id, NULL, 0, 0);
    eos_row_create(container, current_lang[STR_ID_SETTINGS_APPS_AUTHOR], pkg.author, NULL, 0, 0);
    eos_row_create(container, current_lang[STR_ID_SETTINGS_APPS_VERSION], pkg.version, NULL, 0, 0);

    if (strcmp(pkg.description, "") != 0)
    {
        lv_obj_t *inner_container = eos_list_add_title_container(list, current_lang[STR_ID_SETTINGS_APPS_DESCRIPTON]);
        lv_obj_set_size(inner_container, lv_pct(100), LV_SIZE_CONTENT);

        lv_obj_t *desc_label = lv_label_create(inner_container);
        lv_label_set_text(desc_label, pkg.description);
        lv_obj_set_width(desc_label, lv_pct(100));
        lv_label_set_long_mode(desc_label, LV_LABEL_LONG_WRAP);
    }

    lv_obj_t *clear_data_btn = lv_button_create(list);
    lv_obj_set_size(clear_data_btn, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_style_margin_bottom(clear_data_btn, 20, 0);
    lv_obj_set_style_bg_color(clear_data_btn, EOS_THEME_SECONDARY_COLOR, 0);
    lv_obj_set_style_radius(clear_data_btn,LV_RADIUS_CIRCLE,0);

    lv_obj_t *clear_data_btn_label = lv_label_create(clear_data_btn);
    lv_label_set_text(clear_data_btn_label, current_lang[STR_ID_SETTINGS_APPS_CLEAR_DATA]);
    lv_obj_add_event_cb(clear_data_btn, _clear_data_btn_cb, LV_EVENT_CLICKED, (void *)app_id);
    lv_obj_set_style_text_color(clear_data_btn_label, EOS_THEME_DANGEROS_COLOR, 0);
    lv_obj_center(clear_data_btn_label);

    lv_obj_t *uninstall_btn = lv_button_create(list);
    lv_obj_set_size(uninstall_btn, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_color(uninstall_btn, EOS_THEME_SECONDARY_COLOR, 0);
    lv_obj_set_style_radius(uninstall_btn,LV_RADIUS_CIRCLE,0);

    lv_obj_t *uninstall_btn_label = lv_label_create(uninstall_btn);
    lv_label_set_text(uninstall_btn_label, current_lang[STR_ID_SETTINGS_APPS_UINSTALL]);
    lv_obj_add_event_cb(uninstall_btn, _uninstall_btn_cb, LV_EVENT_CLICKED, (void *)app_id);
    lv_obj_set_style_text_color(uninstall_btn_label, EOS_THEME_DANGEROS_COLOR, 0);
    lv_obj_center(uninstall_btn_label);

    eos_list_add_placeholder(list, 50);
}

static void _app_btn_create(lv_obj_t *parent, const char *app_id)
{
    char icon_path[PATH_MAX];
    snprintf(icon_path, sizeof(icon_path), EOS_APP_INSTALLED_DIR "%s/" EOS_APP_ICON_FILE_NAME,
             app_id);
    if (!eos_is_file(icon_path))
    {
        memcpy(icon_path, EOS_IMG_APP, sizeof(EOS_IMG_APP));
    }
    EOS_LOG_D("Icon: %s", icon_path);

    // 获取清单文件
    char manifest_path[PATH_MAX];
    snprintf(manifest_path, sizeof(manifest_path), EOS_APP_INSTALLED_DIR "%s/" EOS_APP_MANIFEST_FILE_NAME,
             app_id);
    script_pkg_t pkg = {0};
    if (script_engine_get_manifest(manifest_path, &pkg) != SE_OK)
    {
        EOS_LOG_E("Read manifest failed: %s", manifest_path);
        return;
    }

    EOS_LOG_I("name = %s\n", pkg.name);

    lv_obj_t *btn = eos_list_add_button(parent, icon_path, pkg.name);
    lv_obj_add_event_cb(btn, _settings_app_list_btn_cb, LV_EVENT_CLICKED, (void *)app_id);
    eos_app_obj_auto_delete(btn, app_id);
}

static void _app_installed_cb(lv_event_t *e)
{
    lv_obj_t *parent = lv_event_get_target(e);
    const char *installed_app_id = lv_event_get_param(e);
    EOS_CHECK_PTR_RETURN(parent && installed_app_id);
    _app_btn_create(parent, installed_app_id);
}

/**
 * @brief 系统设置中的应用列表
 */
static void _settings_screen_apps(lv_event_t *e)
{
    // 创建新的页面用于绘制应用列表
    lv_obj_t *scr = eos_nav_scr_create();
    eos_screen_bind_header_str_id(scr, STR_ID_SETTINGS_APPS);
    lv_screen_load(scr);

    lv_obj_t *app_list = lv_list_create(scr);
    lv_obj_set_size(app_list, lv_pct(100), lv_pct(100));
    lv_obj_set_style_pad_hor(app_list, 20, 0);
    lv_obj_set_style_pad_ver(app_list, 0, 0);
    lv_obj_center(app_list);
    lv_obj_set_scrollbar_mode(app_list, LV_SCROLLBAR_MODE_OFF);
    eos_event_add_cb(app_list, _app_installed_cb, eos_event_get_code(EOS_EVENT_APP_INSTALLED), NULL);
    eos_list_add_placeholder(app_list, 110);

    size_t app_list_size = eos_app_list_size();
    for (size_t i = 0; i < app_list_size; i++)
    {
        _app_btn_create(app_list, eos_app_list_get_id(i));
    }
}

/************************** 传感器测试 **************************/

static void _settings_screen_sensor(lv_event_t *e)
{
    eos_sensor_tester_create();
}

/************************** 更多设置 **************************/

static void _language_roller_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_VALUE_CHANGED)
    {
        char buf[64];
        lv_roller_get_selected_str(obj, buf, sizeof(buf));
        language_id_t lang_id = eos_lang_get_with_str(buf);
        switch (lang_id)
        {
        case LANG_EN:
            EOS_LOG_D("Select English");
            eos_sys_cfg_set_string(EOS_SYS_CFG_KEY_LANGUAGE, buf);
            eos_lang_set(LANG_EN);
            break;
        case LANG_ZH:
            EOS_LOG_D("Select Simplify Chinese");
            eos_sys_cfg_set_string(EOS_SYS_CFG_KEY_LANGUAGE, buf);
            eos_lang_set(LANG_ZH);
            break;
        default:
            break;
        }
    }
}

static void _settings_screen_language(lv_event_t *e)
{
    lv_obj_t *scr = eos_nav_scr_create();
    eos_screen_bind_header_str_id(scr, STR_ID_SETTINGS_ADDITIONAL_SETTINGS_LANGUAGE);
    lv_screen_load(scr);

    lv_obj_t *roller1 = lv_roller_create(scr);
    lv_obj_set_size(roller1, lv_pct(90), 300);
    lv_roller_set_options(roller1,
                          "English\n"
                          "简体中文",
                          LV_ROLLER_MODE_NORMAL);
    lv_obj_align(roller1, LV_ALIGN_CENTER, 0, 0);
    lv_roller_set_visible_row_count(roller1, 5);

    const char *sel_str = eos_sys_cfg_get_string(EOS_SYS_CFG_KEY_LANGUAGE, "English");
    uint32_t sel_opt = (uint32_t)eos_lang_get_with_str(sel_str);
    lv_roller_set_selected(roller1, sel_opt, LV_ANIM_OFF);
    lv_obj_add_event_cb(roller1, _language_roller_event_handler, LV_EVENT_ALL, NULL);
    free(sel_str);
}

static void _settings_screen_additional_settings(lv_event_t *e)
{
    lv_obj_t *scr = eos_nav_scr_create();
    eos_screen_bind_header_str_id(scr, STR_ID_SETTINGS_ADDITIONAL_SETTINGS);
    lv_screen_load(scr);

    lv_obj_t *list = eos_list_create(scr);

    lv_obj_t *btn;
    // 语言设置
    btn = eos_list_add_entry_button_str_id(list, STR_ID_SETTINGS_ADDITIONAL_SETTINGS_LANGUAGE);
    lv_obj_add_event_cb(btn, _settings_screen_language, LV_EVENT_CLICKED, NULL);
}

/************************** 系统设置 **************************/
void eos_settings_create(void)
{
    lv_obj_t *scr = eos_nav_init(lv_screen_active());
    eos_screen_bind_header_str_id(scr, STR_ID_SETTINGS);
    lv_screen_load(scr);

    lv_obj_t *settings_list = eos_list_create(scr);

    lv_obj_t *btn;
    // 蓝牙设置
    btn = eos_list_add_circle_icon_button_str_id(settings_list, lv_color_hex(0x3988ff), RI_BLUETOOTH_FILL, STR_ID_SETTINGS_BLUETOOTH);
    lv_obj_add_event_cb(btn, _settings_screen_bluetooth, LV_EVENT_CLICKED, NULL);
    // 显示设置
    btn = eos_list_add_circle_icon_button_str_id(settings_list, lv_color_hex(0xffbb39), RI_SUN_FILL, STR_ID_SETTINGS_DISPLAY);
    lv_obj_add_event_cb(btn, _settings_screen_display, LV_EVENT_CLICKED, NULL);
    // 通知设置
    btn = eos_list_add_circle_icon_button_str_id(settings_list, lv_color_hex(0xff3939), RI_NOTIFICATION_2_FILL, STR_ID_SETTINGS_NOTIFICATION);
    lv_obj_add_event_cb(btn, _settings_screen_notification, LV_EVENT_CLICKED, NULL);
    // 应用列表
    btn = eos_list_add_circle_icon_button_str_id(settings_list, lv_color_hex(0x00b8a9), RI_FILE_LIST_LINE, STR_ID_SETTINGS_APPS);
    lv_obj_add_event_cb(btn, _settings_screen_apps, LV_EVENT_CLICKED, NULL);
    // 传感器测试
    btn = eos_list_add_circle_icon_button(settings_list, lv_color_hex(0x00b8a9), RI_SENSOR_LINE, "SensorTester");
    lv_obj_add_event_cb(btn, _settings_screen_sensor, LV_EVENT_CLICKED, NULL);
    // 其他设置
    btn = eos_list_add_circle_icon_button_str_id(settings_list, lv_color_hex(0x00b8a9), RI_TOOLS_FILL, STR_ID_SETTINGS_ADDITIONAL_SETTINGS);
    lv_obj_add_event_cb(btn, _settings_screen_additional_settings, LV_EVENT_CLICKED, NULL);

    eos_list_add_placeholder(settings_list, 50);
}
