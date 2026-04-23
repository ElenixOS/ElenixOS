/**
 * @file elena_os_settings.c
 * @brief Settings page
 * @author Sab1e
 * @date 2025-09-24
 */

#include "elena_os_settings.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #define EOS_LOG_DISABLE
#define EOS_LOG_TAG "Settings"
#include "elena_os_log.h"
#include "lvgl.h"
#include "cJSON.h"
#include "elena_os_img.h"
#include "elena_os_msg_list.h"
#include "elena_os_lang.h"
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
#include "elena_os_display.h"
#include "elena_os_toast.h"
#include "elena_os_fs.h"
#include "elena_os_pm.h"
#include "elena_os_app_header.h"
#include "elena_os_radio_list.h"
#include "elena_os_lang.h"
#include "elena_os_mem.h"
#include "elena_os_font.h"
#include "elena_os_std_widgets.h"
#include "elena_os_vibrator.h"
#include "elena_os_activity.h"

/* Macros and Definitions -------------------------------------*/
#define _BRIGHTNESS_SMOOTH_DURATION 200
/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

/************************** General Functions **************************/

void eos_settings_slient_mode_on(void)
{
    eos_speaker_set_volume(0);
    eos_sys_cfg_set_bool(EOS_SYS_CFG_KEY_MUTE_BOOL, true);
}

void eos_settings_slient_mode_off(void)
{
    uint8_t volume = eos_sys_cfg_get_number(EOS_SYS_CFG_KEY_SPEAKER_VOLUME_NUMBER, 20);
    eos_speaker_set_volume(volume);
    eos_sys_cfg_set_bool(EOS_SYS_CFG_KEY_MUTE_BOOL, false);
}

/************************** Helper Functions **************************/

lv_obj_t *_auto_get_config_switch_create(lv_obj_t *list, const char *txt, const char *config_key, bool default_val)
{
    lv_obj_t *sw = eos_list_add_switch(list, txt);
    lv_obj_set_state(sw, LV_STATE_CHECKED, eos_sys_cfg_get_bool(config_key, default_val));
    return sw;
}

static eos_activity_t *_create_activity_with_header(lang_string_id_t id, lv_obj_t **out_view)
{
    eos_activity_t *a = eos_activity_create(NULL);
    EOS_CHECK_PTR_RETURN_VAL(a, NULL);
    lv_obj_t *view = eos_activity_get_view(a);
    EOS_CHECK_PTR_RETURN_VAL(view, NULL);
    eos_activity_set_title_id(a, id);
    eos_activity_set_app_header_visible(a, true);
    if (out_view)
    {
        *out_view = view;
    }
    return a;
}

/************************** Bluetooth **************************/
static void _bluetooth_enable_switch_cb(lv_event_t *e)
{
    lv_obj_t *bt_sw = lv_event_get_target(e);
    EOS_CHECK_PTR_RETURN(bt_sw);
    if (lv_obj_has_state(bt_sw, LV_STATE_CHECKED))
    {
        eos_bluetooth_enable();
        eos_sys_cfg_set_bool(EOS_SYS_CFG_KEY_BLUETOOTH_BOOL, true);
    }
    else
    {
        eos_bluetooth_disable();
        eos_sys_cfg_set_bool(EOS_SYS_CFG_KEY_BLUETOOTH_BOOL, false);
    }
}

static void _settings_view_bluetooth(lv_event_t *e)
{
    lv_obj_t *view = NULL;
    eos_activity_t *a = _create_activity_with_header(STR_ID_SETTINGS_BLUETOOTH, &view);
    EOS_CHECK_PTR_RETURN(a && view);
    lv_obj_t *list = eos_list_create(view);
    lv_obj_t *bt_sw = _auto_get_config_switch_create(
        list, eos_lang_get_text(STR_ID_SETTINGS_BLUETOOTH_ENABLE), EOS_SYS_CFG_KEY_BLUETOOTH_BOOL, false);
    lv_obj_add_event_cb(bt_sw, _bluetooth_enable_switch_cb, LV_EVENT_VALUE_CHANGED, NULL);
    eos_activity_enter(a);
}
/************************** Display Settings **************************/

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
    eos_sys_cfg_set_number(EOS_SYS_CFG_KEY_DISPLAY_BRIGHTNESS_NUMBER, val);
}

static void _brightness_slider_minus_cb(lv_event_t *e)
{
    lv_obj_t *slider = (lv_obj_t *)lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(slider);
    int32_t min = lv_slider_get_min_value(slider);
    int32_t val = lv_slider_get_value(slider);
    int32_t prev = val;
    if (val == min)
        return;
    val -= 5;
    lv_slider_set_value(slider, val, LV_ANIM_ON);
    eos_sys_cfg_set_number(EOS_SYS_CFG_KEY_DISPLAY_BRIGHTNESS_NUMBER, val);
    eos_display_set_brightness_smooth(prev, val, _BRIGHTNESS_SMOOTH_DURATION);
}

static void _brightness_slider_plus_cb(lv_event_t *e)
{
    lv_obj_t *slider = (lv_obj_t *)lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(slider);
    int32_t max = lv_slider_get_max_value(slider);
    int32_t val = lv_slider_get_value(slider);
    int32_t prev = val;
    if (val == max)
        return;
    val += 5;
    lv_slider_set_value(slider, val, LV_ANIM_ON);
    eos_sys_cfg_set_number(EOS_SYS_CFG_KEY_DISPLAY_BRIGHTNESS_NUMBER, val);
    eos_display_set_brightness_smooth(prev, val, _BRIGHTNESS_SMOOTH_DURATION);
}

static void _aod_mode_switch_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    EOS_CHECK_PTR_RETURN(obj);
    if (lv_obj_has_state(obj, LV_STATE_CHECKED))
    {
        eos_pm_set_aod_mode(true);
    }
    else
    {
        eos_pm_set_aod_mode(false);
    }
}

static void _wake_on_raise_switch_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    EOS_CHECK_PTR_RETURN(obj);
    if (lv_obj_has_state(obj, LV_STATE_CHECKED))
    {
        eos_sys_cfg_set_bool(EOS_SYS_CFG_KEY_WAKE_ON_RAISE_BOOL, true);
    }
    else
    {
        eos_sys_cfg_set_bool(EOS_SYS_CFG_KEY_WAKE_ON_RAISE_BOOL, false);
    }
}

static void _wake_duration_radio_list_selection_changed_cb(lv_event_t *e)
{
    uint32_t index = (uint32_t)lv_event_get_param(e);
    uint32_t wake_duration = 15;
    if (index == 0)
    {
        wake_duration = 15;
    }
    else
    {
        wake_duration = 70;
    }
    eos_pm_set_sleep_timeout(wake_duration);
    eos_sys_cfg_set_number(EOS_SYS_CFG_KEY_SLEEP_TIMEOUT_SEC_NUMBER, wake_duration);
}

static void _wake_duration_entry_button_clicked_cb(lv_event_t *e)
{
    eos_radio_list_t *rl = eos_radio_list_enter(eos_lang_get_text(STR_ID_SETTINGS_WAKE_DURATION));
    char str[32];
    snprintf(str, sizeof(str), eos_lang_get_text(STR_ID_SETTINGS_WAKE_FOR_N_SECONDS), 15);
    uint32_t timeout_15_item_index = eos_radio_list_add_item(rl, str);
    snprintf(str, sizeof(str), eos_lang_get_text(STR_ID_SETTINGS_WAKE_FOR_N_SECONDS), 70);
    uint32_t timeout_70_item_index = eos_radio_list_add_item(rl, str);
    eos_radio_list_add_event_cb(rl, _wake_duration_radio_list_selection_changed_cb, NULL);
    eos_radio_list_set_subtitle(rl, eos_lang_get_text(STR_ID_SETTINGS_WAKE_ON_TAP));
    eos_radio_list_set_comment(rl, eos_lang_get_text(STR_ID_SETTINGS_WAKE_ON_TAP_COMMENT));
    uint32_t timeout = eos_sys_cfg_get_number(EOS_SYS_CFG_KEY_SLEEP_TIMEOUT_SEC_NUMBER, 15);
    if (timeout == 15)
    {
        eos_radio_list_check(rl, timeout_15_item_index);
    }
    else
    {
        eos_radio_list_check(rl, timeout_70_item_index);
    }
}

static void _settings_view_display(lv_event_t *e)
{
    lv_obj_t *view = NULL;
    eos_activity_t *a = _create_activity_with_header(STR_ID_SETTINGS_DISPLAY, &view);
    EOS_CHECK_PTR_RETURN(a && view);

    lv_obj_t *list = eos_list_create(view);

    eos_list_slider_t *ls = eos_list_add_slider(list, eos_lang_get_text(STR_ID_SETTINGS_DISPLAY_BRIGHTNESS));
    lv_label_set_text(ls->minus_label, RI_SUN_LINE);
    lv_label_set_text(ls->plus_label, RI_SUN_FILL);
    eos_list_slider_set_minus_label_scale(ls, 200);
    lv_slider_set_value(ls->slider, eos_sys_cfg_get_number(EOS_SYS_CFG_KEY_DISPLAY_BRIGHTNESS_NUMBER, 50), LV_ANIM_ON);
    lv_slider_set_range(ls->slider, EOS_DISPLAY_BRIGHTNESS_MIN, EOS_DISPLAY_BRIGHTNESS_MAX);
    lv_obj_add_event_cb(ls->slider, _brightness_slider_value_changed_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(ls->slider, _brightness_slider_released_cb, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(ls->minus_btn, _brightness_slider_minus_cb, LV_EVENT_CLICKED, ls->slider);
    lv_obj_add_event_cb(ls->plus_btn, _brightness_slider_plus_cb, LV_EVENT_CLICKED, ls->slider);

    eos_list_add_placeholder(list, EOS_LIST_SECTION_PLACEHOLDER_HEIGHT);

    lv_obj_t *sw = _auto_get_config_switch_create(
        list, eos_lang_get_text(STR_ID_SETTINGS_DISPLAY_AOD), EOS_SYS_CFG_KEY_AOD_MODE_BOOL, false);
    lv_obj_add_event_cb(sw, _aod_mode_switch_cb, LV_EVENT_VALUE_CHANGED, NULL);
    eos_list_add_comment(list, eos_lang_get_text(STR_ID_SETTINGS_DISPLAY_AOD_COMMENT));
    eos_list_add_placeholder(list, EOS_LIST_SECTION_PLACEHOLDER_HEIGHT);

    eos_list_add_title(list, eos_lang_get_text(STR_ID_SETTINGS_WAKE));
    sw = _auto_get_config_switch_create(
        list, eos_lang_get_text(STR_ID_SETTINGS_WAKE_ON_RAISE), EOS_SYS_CFG_KEY_WAKE_ON_RAISE_BOOL, true);
    lv_obj_add_event_cb(sw, _wake_on_raise_switch_cb, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t *wd_btn = eos_list_add_entry_button(list, eos_lang_get_text(STR_ID_SETTINGS_WAKE_DURATION));
    lv_obj_add_event_cb(wd_btn, _wake_duration_entry_button_clicked_cb, LV_EVENT_CLICKED, NULL);
    eos_activity_enter(a);
}
/************************** Notification **************************/
static void _settings_view_notification(lv_event_t *e)
{
    lv_obj_t *view = NULL;
    eos_activity_t *a = _create_activity_with_header(STR_ID_SETTINGS_NOTIFICATION, &view);
    EOS_CHECK_PTR_RETURN(a && view);
    lv_obj_t *list = eos_list_create(view);
    // TODO: Notification settings
    eos_activity_enter(a);
}

/************************** Sound and Haptic Feedback **************************/

static void _volume_slider_value_changed_cb(lv_event_t *e)
{
    lv_obj_t *sl = lv_event_get_target(e);
    eos_speaker_set_volume(lv_slider_get_value(sl));
}

static void _volume_slider_released_cb(lv_event_t *e)
{
    lv_obj_t *sl = lv_event_get_target(e);
    int32_t val = lv_slider_get_value(sl);
    eos_speaker_set_volume(val);
    eos_sys_cfg_set_number(EOS_SYS_CFG_KEY_SPEAKER_VOLUME_NUMBER, val);
}

static void _volume_slider_minus_cb(lv_event_t *e)
{
    lv_obj_t *slider = (lv_obj_t *)lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(slider);
    int32_t min = lv_slider_get_min_value(slider);
    int32_t val = lv_slider_get_value(slider);
    int32_t prev = val;
    if (val == min)
        return;
    val -= 5;
    lv_slider_set_value(slider, val, LV_ANIM_ON);
    eos_sys_cfg_set_number(EOS_SYS_CFG_KEY_SPEAKER_VOLUME_NUMBER, val);
    eos_speaker_set_volume(val);
}

static void _volume_slider_plus_cb(lv_event_t *e)
{
    lv_obj_t *slider = (lv_obj_t *)lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(slider);
    int32_t max = lv_slider_get_max_value(slider);
    int32_t val = lv_slider_get_value(slider);
    int32_t prev = val;
    if (val == max)
        return;
    val += 5;
    lv_slider_set_value(slider, val, LV_ANIM_ON);
    eos_sys_cfg_set_number(EOS_SYS_CFG_KEY_SPEAKER_VOLUME_NUMBER, val);
    eos_speaker_set_volume(val);
}

static void _mute_switch_cb(lv_event_t *e)
{
    lv_obj_t *bt_sw = lv_event_get_target(e);
    EOS_CHECK_PTR_RETURN(bt_sw);
    if (lv_obj_has_state(bt_sw, LV_STATE_CHECKED))
    {
        eos_settings_slient_mode_on();
    }
    else
    {
        eos_settings_slient_mode_off();
    }
}

static void _haptics_radio_list_selection_changed_cb(lv_event_t *e)
{
    uint32_t index = (uint32_t)lv_event_get_param(e);
    eos_vibrator_strength_t strength = EOS_VIBRATOR_STRENGTH_NORMAL;
    switch (index)
    {
    case 0:
        strength = EOS_VIBRATOR_STRENGTH_OFF;
        break;
    case 1:
        strength = EOS_VIBRATOR_STRENGTH_NORMAL;
        break;
    case 2:
        strength = EOS_VIBRATOR_STRENGTH_INTENSE;
        break;
    default:
        break;
    }
    eos_vibrator_set_strength(strength);
}

static void _haptics_entry_button_clicked_cb(lv_event_t *e)
{
    eos_radio_list_t *rl = eos_radio_list_enter(eos_lang_get_text(STR_ID_SETTINGS_HAPTICS));
    eos_radio_list_add_item(rl, eos_lang_get_text(STR_ID_OFF));
    eos_radio_list_add_item(rl, eos_lang_get_text(STR_ID_NORMAL));
    eos_radio_list_add_item(rl, eos_lang_get_text(STR_ID_INTENSE));
    eos_radio_list_set_subtitle(rl, eos_lang_get_text(STR_ID_SETTINGS_HAPTICS_STRENGTH));
    eos_radio_list_add_event_cb(rl, _haptics_radio_list_selection_changed_cb, NULL);
    eos_vibrator_strength_t s = eos_sys_cfg_get_number(
        EOS_SYS_CFG_KEY_VIBRATOR_STRENGTH_NUMBER,
        EOS_VIBRATOR_STRENGTH_NORMAL);
    switch (s)
    {
    case EOS_VIBRATOR_STRENGTH_OFF:
        eos_radio_list_check(rl, 0);
        break;
    case EOS_VIBRATOR_STRENGTH_NORMAL:
        eos_radio_list_check(rl, 1);
        break;
    case EOS_VIBRATOR_STRENGTH_INTENSE:
        eos_radio_list_check(rl, 2);
        break;
    default:
        break;
    }
}

static void _settings_view_sound_and_haptics(lv_event_t *e)
{
    lv_obj_t *view = NULL;
    eos_activity_t *a = _create_activity_with_header(STR_ID_SETTINGS_SOUNDS_AND_HAPTICS, &view);
    EOS_CHECK_PTR_RETURN(a && view);

    lv_obj_t *list = eos_list_create(view);

    eos_list_add_title(list, eos_lang_get_text(STR_ID_SETTINGS_SOUNDS_AND_ALERTS));

    lv_obj_t *bt_sw = _auto_get_config_switch_create(
        list, eos_lang_get_text(STR_ID_SETTINGS_SOUNDS_AND_HAPTICS_SILENT_MODE), EOS_SYS_CFG_KEY_MUTE_BOOL, false);
    lv_obj_add_event_cb(bt_sw, _mute_switch_cb, LV_EVENT_VALUE_CHANGED, NULL);

    eos_list_slider_t *ls = eos_list_add_slider(list, eos_lang_get_text(STR_ID_SETTINGS_SOUNDS_AND_HAPTICS_VOLUME));
    lv_label_set_text(ls->minus_label, RI_VOLUME_DOWN_FILL);
    lv_label_set_text(ls->plus_label, RI_VOLUME_UP_FILL);

    lv_slider_set_value(ls->slider, eos_sys_cfg_get_number(EOS_SYS_CFG_KEY_SPEAKER_VOLUME_NUMBER, 50), LV_ANIM_ON);
    lv_slider_set_range(ls->slider, EOS_SPEAKER_VOLUME_MIN, EOS_SPEAKER_VOLUME_MAX);

    lv_obj_add_event_cb(ls->slider, _volume_slider_value_changed_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(ls->slider, _volume_slider_released_cb, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(ls->minus_btn, _volume_slider_minus_cb, LV_EVENT_CLICKED, ls->slider);
    lv_obj_add_event_cb(ls->plus_btn, _volume_slider_plus_cb, LV_EVENT_CLICKED, ls->slider);

    eos_list_add_placeholder(list, EOS_LIST_SECTION_PLACEHOLDER_HEIGHT);

    lv_obj_t *h_btn = eos_list_add_entry_button(list, eos_lang_get_text(STR_ID_SETTINGS_HAPTICS));
    lv_obj_add_event_cb(h_btn, _haptics_entry_button_clicked_cb, LV_EVENT_CLICKED, NULL);
    eos_activity_enter(a);
}

/************************** App List **************************/

/**
 * @brief Uninstall button callback
 */
static void _uninstall_btn_cb(lv_event_t *e)
{
    const char *app_id = (const char *)lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(app_id);
    eos_app_uninstall(app_id);
    eos_activity_back();
}

static void _clear_data_btn_cb(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e);
    const char *app_id = (const char *)lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(app_id);
    char data_path[PATH_MAX];
    snprintf(data_path, sizeof(data_path), EOS_APP_DATA_DIR "%s", app_id);
    if (eos_fs_rm_recursive(data_path) != 0)
    {
        EOS_LOG_E("Remove data failed");
        return;
    }
    lv_obj_add_state(btn, LV_STATE_DISABLED);
    eos_toast_show_char_icon(
        RI_CHECKBOX_CIRCLE_FILL,
        EOS_COLOR_GREEN,
        eos_lang_get_text(STR_ID_SETTINGS_APPS_CLEAR_DATA_SUCCESS));
}

/**
 * @brief App list callback, opens app details
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
        eos_pkg_free(&pkg);
        return;
    }

    // 创建新的页面用于绘制应用详情页
    eos_activity_t *a = eos_activity_create(NULL);
    EOS_CHECK_PTR_RETURN(a);
    lv_obj_t *view = eos_activity_get_view(a);
    EOS_CHECK_PTR_RETURN(view);
    eos_activity_set_title(a, pkg.name);
    eos_activity_set_app_header_visible(a, true);

    lv_obj_t *list = eos_list_create(view);

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

    eos_row_create(container, eos_lang_get_text(STR_ID_SETTINGS_APPS_APPID), app_id, NULL, 0, 0);
    eos_row_create(container, eos_lang_get_text(STR_ID_SETTINGS_APPS_AUTHOR), pkg.author, NULL, 0, 0);
    eos_row_create(container, eos_lang_get_text(STR_ID_SETTINGS_APPS_VERSION), pkg.version, NULL, 0, 0);

    if (strcmp(pkg.description, "") != 0)
    {
        lv_obj_t *inner_container = eos_list_add_title_container(list, eos_lang_get_text(STR_ID_SETTINGS_APPS_DESCRIPTON));
        lv_obj_set_size(inner_container, lv_pct(100), LV_SIZE_CONTENT);

        lv_obj_t *desc_label = lv_label_create(inner_container);
        lv_label_set_text(desc_label, pkg.description);
        lv_obj_set_width(desc_label, lv_pct(100));
        lv_label_set_long_mode(desc_label, LV_LABEL_LONG_WRAP);
    }

    eos_list_add_placeholder(list, EOS_LIST_SECTION_PLACEHOLDER_HEIGHT);

    lv_obj_t *clear_data_btn = eos_button_create_ex(
        list,
        EOS_THEME_SECONDARY_COLOR,
        eos_lang_get_text(STR_ID_SETTINGS_APPS_CLEAR_DATA),
        EOS_THEME_DANGEROS_COLOR,
        _clear_data_btn_cb,
        app_id);

    eos_list_add_placeholder(list, 20);

    char data_path[PATH_MAX];
    snprintf(data_path, sizeof(data_path), EOS_APP_DATA_DIR "%s", app_id);
    if (!eos_is_dir(data_path))
    {
        lv_obj_add_state(clear_data_btn, LV_STATE_DISABLED);
    }

    lv_obj_t *uninstall_btn = eos_button_create_ex(
        list,
        EOS_THEME_SECONDARY_COLOR,
        eos_lang_get_text(STR_ID_SETTINGS_APPS_UNINSTALL),
        EOS_THEME_DANGEROS_COLOR,
        _uninstall_btn_cb,
        app_id);

    (void)uninstall_btn;
    eos_activity_enter(a);
    eos_pkg_free(&pkg);
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
    eos_pkg_free(&pkg);
}

static void _app_installed_cb(lv_event_t *e)
{
    lv_obj_t *parent = lv_event_get_target(e);
    const char *installed_app_id = lv_event_get_param(e);
    EOS_CHECK_PTR_RETURN(parent && installed_app_id);
    _app_btn_create(parent, installed_app_id);
}

/**
 * @brief App list in system settings
 */
static void _settings_view_apps(lv_event_t *e)
{
    // Create a new page to draw the app list
    lv_obj_t *view = NULL;
    eos_activity_t *a = _create_activity_with_header(STR_ID_SETTINGS_APPS, &view);
    EOS_CHECK_PTR_RETURN(a && view);

    lv_obj_t *app_list = eos_list_create(view);
    eos_event_add_cb(app_list, _app_installed_cb, EOS_EVENT_APP_INSTALLED, NULL);

    size_t app_list_size = eos_app_get_installed();
    for (size_t i = 0; i < app_list_size; i++)
    {
        _app_btn_create(app_list, eos_app_list_get_id(i));
    }

    eos_activity_enter(a);
}

/************************** General Settings **************************/

static void _language_roller_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_VALUE_CHANGED)
    {
        char buf[64];
        lv_roller_get_selected_str(obj, buf, sizeof(buf));
        language_id_t lang_id = eos_lang_parse_name(buf);
        switch (lang_id)
        {
        case LANG_EN:
            EOS_LOG_D("Select English");
            eos_lang_set_current_id(LANG_EN);
            eos_sys_cfg_set_string(EOS_SYS_CFG_KEY_LANGUAGE_STR, eos_lang_get_name(LANG_EN));
            break;
        case LANG_ZH:
            EOS_LOG_D("Select Simplify Chinese");
            eos_lang_set_current_id(LANG_ZH);
            eos_sys_cfg_set_string(EOS_SYS_CFG_KEY_LANGUAGE_STR, eos_lang_get_name(LANG_ZH));
            break;
        default:
            break;
        }
    }
}

static void _settings_view_language(lv_event_t *e)
{
    lv_obj_t *view = NULL;
    eos_activity_t *a = _create_activity_with_header(STR_ID_SETTINGS_GENERAL_LANGUAGE, &view);
    EOS_CHECK_PTR_RETURN(a && view);

    lv_obj_t *roller = lv_roller_create(view);
    lv_obj_set_size(roller, lv_pct(80), 200);
    char lang_options[64];
    snprintf(lang_options,
             sizeof(lang_options),
             "%s\n%s",
             eos_lang_get_name(LANG_EN),
             eos_lang_get_name(LANG_ZH));
    lv_roller_set_options(roller, lang_options, LV_ROLLER_MODE_NORMAL);
    lv_obj_align(roller, LV_ALIGN_CENTER, 0, 30);
    lv_roller_set_visible_row_count(roller, 5);

    char *sel_str = eos_sys_cfg_get_string(EOS_SYS_CFG_KEY_LANGUAGE_STR, "English");
    uint32_t sel_opt = (uint32_t)eos_lang_parse_name(sel_str);
    lv_roller_set_selected(roller, sel_opt, LV_ANIM_OFF);
    lv_obj_add_event_cb(roller, _language_roller_event_handler, LV_EVENT_ALL, NULL);
    eos_free(sel_str);
    eos_activity_enter(a);
}

static void _settings_view_device_info(lv_event_t *e)
{
    lv_obj_t *view = NULL;
    eos_activity_t *a = _create_activity_with_header(STR_ID_SETTINGS_GENERAL_DEVICE_INFO, &view);
    EOS_CHECK_PTR_RETURN(a && view);
    eos_activity_set_title_color(a, EOS_THEME_LOGO_PRIMARY_COLOR);

    lv_obj_t *list = eos_list_create(view);
    lv_obj_set_style_pad_row(list, 0, 0);

    eos_list_add_placeholder(list, 20);
    lv_obj_t *logo = lv_image_create(list);
    eos_img_set_src(logo, EOS_IMG_LOGO);
    eos_list_add_placeholder(list, 30);

    char *device_name = eos_sys_cfg_get_string(
        EOS_SYS_CFG_KEY_DEVICE_NAME_STR,
        EOS_SYS_DEFAULT_DEVICE_NAME);
    eos_std_title_comment_create(list,
                                 eos_lang_get_text(STR_ID_SETTINGS_GENERAL_DEVICE_NAME),
                                 device_name);
    if (device_name)
        eos_free(device_name);
    eos_list_add_placeholder(list, 20);

    eos_std_title_comment_create(list,
                                 eos_lang_get_text(STR_ID_SETTINGS_GENERAL_EOS_VER),
                                 ELENA_OS_VERSION_FULL);
    eos_list_add_placeholder(list, 20);

    eos_std_title_comment_create(list,
                                 eos_lang_get_text(STR_ID_SETTINGS_GENERAL_MARKETING_NAME),
                                 ELENA_WATCH_MARKETING_NAME);
    eos_list_add_placeholder(list, 20);

    eos_std_title_comment_create(list,
                                 eos_lang_get_text(STR_ID_SETTINGS_GENERAL_MODEL_NUMBER),
                                 ELENA_WATCH_MODEL_NUMBER);
    eos_list_add_placeholder(list, 20);

    char install_number_str[32];
    snprintf(install_number_str, sizeof(install_number_str), "%d", eos_app_get_installed());
    eos_std_title_comment_create(list,
                                 eos_lang_get_text(STR_ID_SETTINGS_APPS),
                                 install_number_str);
    eos_list_add_placeholder(list, 20);

    eos_std_title_comment_create(list,
                                 eos_lang_get_text(STR_ID_SETTINGS_GENERAL_OPEN_SOURCE),
                                 eos_lang_get_text(STR_ID_SETTINGS_GENERAL_OPEN_SOURCE_CONTENT));

#if LV_USE_QRCODE
    lv_obj_t *qr = lv_qrcode_create(list);
    lv_qrcode_set_size(qr, 200);
    lv_qrcode_set_dark_color(qr, EOS_COLOR_BLACK);
    lv_qrcode_set_light_color(qr, EOS_COLOR_WHITE);
    lv_obj_set_style_margin_all(qr, 20, 0);
    const char *repo_data = "https://github.com/Sab1e-dev/ElenaOS";
    lv_qrcode_update(qr, repo_data, strlen(repo_data));
    lv_obj_set_style_border_color(qr, EOS_COLOR_WHITE, 0);
    lv_obj_set_style_border_width(qr, 8, 0);
#endif /* LV_USE_QRCODE */

    eos_std_title_comment_create(list,
                                 eos_lang_get_text(STR_ID_SETTINGS_GENERAL_LEGAL_INFO),
                                 eos_lang_get_text(STR_ID_SETTINGS_GENERAL_LEGAL_INFO_CONTENT));
#if LV_USE_QRCODE
    qr = lv_qrcode_create(list);
    lv_qrcode_set_size(qr, 200);
    lv_qrcode_set_dark_color(qr, EOS_COLOR_BLACK);
    lv_qrcode_set_light_color(qr, EOS_COLOR_WHITE);
    lv_obj_set_style_margin_all(qr, 20, 0);
    const char *legal_data = "https://www.apache.org/licenses/LICENSE-2.0";
    lv_qrcode_update(qr, legal_data, strlen(legal_data));
    lv_obj_set_style_border_color(qr, EOS_COLOR_WHITE, 0);
    lv_obj_set_style_border_width(qr, 8, 0);
#endif /* LV_USE_QRCODE */

    eos_activity_enter(a);
}

static void _settings_view_general(lv_event_t *e)
{
    lv_obj_t *view = NULL;
    eos_activity_t *a = _create_activity_with_header(STR_ID_SETTINGS_GENERAL, &view);
    EOS_CHECK_PTR_RETURN(a && view);

    lv_obj_t *list = eos_list_create(view);

    lv_obj_t *btn;
    // 语言设置
    btn = eos_list_add_entry_button_str_id(list, STR_ID_SETTINGS_GENERAL_LANGUAGE);
    lv_obj_add_event_cb(btn, _settings_view_language, LV_EVENT_CLICKED, NULL);
    btn = eos_list_add_entry_button_str_id(list, STR_ID_SETTINGS_GENERAL_DEVICE_INFO);
    lv_obj_add_event_cb(btn, _settings_view_device_info, LV_EVENT_CLICKED, NULL);
    eos_activity_enter(a);
}

/************************** System Settings Program Entry **************************/
static const eos_activity_lifecycle_t _settings_lifecycle = {
    .on_enter = NULL,
    .on_destroy = NULL,
};
void eos_settings_enter(void)
{
    eos_activity_t *a = eos_activity_create(&_settings_lifecycle);
    EOS_CHECK_PTR_RETURN(a);
    lv_obj_t *view = eos_activity_get_view(a);
    EOS_CHECK_PTR_RETURN(view);
    eos_activity_set_title_id(a, STR_ID_SETTINGS);
    eos_activity_set_app_header_visible(a, true);

    lv_obj_t *settings_list = eos_list_create(view);

    lv_obj_t *btn;
    // 蓝牙设置
    btn = eos_list_add_round_icon_button_str_id(settings_list, EOS_COLOR_BLUE, RI_BLUETOOTH_FILL, STR_ID_SETTINGS_BLUETOOTH);
    lv_obj_add_event_cb(btn, _settings_view_bluetooth, LV_EVENT_CLICKED, NULL);
    // 显示设置
    btn = eos_list_add_round_icon_button_str_id(settings_list, EOS_COLOR_ORANGE, RI_SUN_FILL, STR_ID_SETTINGS_DISPLAY);
    lv_obj_add_event_cb(btn, _settings_view_display, LV_EVENT_CLICKED, NULL);
    // 通知设置
    btn = eos_list_add_round_icon_button_str_id(settings_list, EOS_COLOR_RED, RI_NOTIFICATION_2_FILL, STR_ID_SETTINGS_NOTIFICATION);
    lv_obj_add_event_cb(btn, _settings_view_notification, LV_EVENT_CLICKED, NULL);
    // 声音与触感反馈
    btn = eos_list_add_round_icon_button_str_id(settings_list, EOS_COLOR_PINK, RI_VOLUME_UP_FILL, STR_ID_SETTINGS_SOUNDS_AND_HAPTICS);
    lv_obj_add_event_cb(btn, _settings_view_sound_and_haptics, LV_EVENT_CLICKED, NULL);
    // 应用列表
    btn = eos_list_add_round_icon_button_str_id(settings_list, EOS_COLOR_GREEN, RI_FILE_LIST_LINE, STR_ID_SETTINGS_APPS);
    lv_obj_add_event_cb(btn, _settings_view_apps, LV_EVENT_CLICKED, NULL);
    // 其他设置
    btn = eos_list_add_round_icon_button_str_id(settings_list, EOS_COLOR_GREY, RI_TOOLS_FILL, STR_ID_SETTINGS_GENERAL);
    lv_obj_add_event_cb(btn, _settings_view_general, LV_EVENT_CLICKED, NULL);

    eos_activity_enter(a);
}
