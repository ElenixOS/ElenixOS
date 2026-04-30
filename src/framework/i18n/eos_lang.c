/**
 * @file eos_lang.c
 * @brief Multi-language system
 */

#include "eos_lang.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eos_event.h"
#define EOS_LOG_DISABLE
#define EOS_LOG_TAG "Language"
#include "eos_log.h"
#include "lvgl.h"
#include "eos_sys.h"
#include "eos_icon.h"
#include "eos_mem.h"

/* Macros and Definitions -------------------------------------*/
/**
 * @brief English language array
 * @note Add strings here as needed
 */
const char *lang_en[STR_ID_MAX_NUMBER] = {
    [STR_ID_LANGUAGE] = "English",
    [STR_ID_ERROR] = "Error",
    [STR_ID_OK] = "OK",
    [STR_ID_CANCEL] = "Cancel",
    [STR_ID_OFF] = "OFF",
    [STR_ID_NORMAL] = "Normal",
    [STR_ID_INTENSE] = "Intense",
    [STR_ID_MSG_LIST_CLEAR_ALL] = "Clear all",
    [STR_ID_MSG_LIST_NO_MSG] = "No notifications",
    [STR_ID_MSG_LIST_ITEM_MARK_AS_READ] = "Mark as read",
    [STR_ID_BACK] = "Back",
    [STR_ID_TEST_LANG_STR] = "On a late-summer afternoon, the wind set the chimes on the balcony jingling, like some unintentional signal.",
    [STR_ID_APP_RUN_ERR_TITLE] = "Application Stopped",
    [STR_ID_APP_RUN_ERR] = "The application encountered a critical error and cannot continue running. Please report this issue to the developer.",
    [STR_ID_WATCHFACE_RUN_ERR_TITLE] = "Watchface Stopped",
    [STR_ID_WATCHFACE_RUN_ERR] = "The watchface encountered a critical error and cannot continue running. Please report this issue to the developer.",
    [STR_ID_WATCHFACE_SWITCH] = "Switch Watch Face",
    [STR_ID_SETTINGS] = "Settings",
    [STR_ID_SETTINGS_BLUETOOTH] = "Bluetooth",
    [STR_ID_SETTINGS_BLUETOOTH_ENABLE] = "Bluetooth",
    [STR_ID_SETTINGS_DISPLAY] = "Display",
    [STR_ID_SETTINGS_DISPLAY_BRIGHTNESS] = "Brightness",
    [STR_ID_SETTINGS_DISPLAY_AOD] = "AOD",
    [STR_ID_SETTINGS_DISPLAY_AOD_COMMENT] = "Elenix Watch can show the time at all times.",
    [STR_ID_SETTINGS_WAKE] = "Wake",
    [STR_ID_SETTINGS_WAKE_DURATION] = "Wake Duration",
    [STR_ID_SETTINGS_WAKE_ON_RAISE] = "Wake on raise",
    [STR_ID_SETTINGS_WAKE_FOR_N_SECONDS] = "Wake for %d seconds",
    [STR_ID_SETTINGS_WAKE_ON_TAP] = "On tap",
    [STR_ID_SETTINGS_WAKE_ON_TAP_COMMENT] = "Choose 'Tap to Wake' on the Elenix Watch, then set the wake duration.",
    [STR_ID_SETTINGS_HAPTICS_STRENGTH] = "Haptics strength",
    [STR_ID_SETTINGS_HAPTICS] = "Haptics",
    [STR_ID_SETTINGS_NOTIFICATION] = "Notification",
    [STR_ID_SETTINGS_APPS] = "Apps",
    [STR_ID_SETTINGS_APPS_DETAILS] = "App Details",
    [STR_ID_SETTINGS_APPS_APPID] = "ID",
    [STR_ID_SETTINGS_APPS_AUTHOR] = "Author",
    [STR_ID_SETTINGS_APPS_VERSION] = "Version",
    [STR_ID_SETTINGS_APPS_DESCRIPTON] = "Description",
    [STR_ID_SETTINGS_APPS_UNINSTALL] = "Uinstall",
    [STR_ID_SETTINGS_APPS_CLEAR_DATA] = "Clear data",
    [STR_ID_SETTINGS_GENERAL] = "General",
    [STR_ID_SETTINGS_GENERAL_LANGUAGE] = "Language",
    [STR_ID_SETTINGS_GENERAL_DEVICE_INFO] = "Device info",
    [STR_ID_SETTINGS_GENERAL_DEVICE_NAME] = "Device name",
    [STR_ID_SETTINGS_GENERAL_EOS_VER] = "ElenixOS version",
    [STR_ID_SETTINGS_GENERAL_MARKETING_NAME] = "Marketing name",
    [STR_ID_SETTINGS_GENERAL_MODEL_NUMBER] = "Model number",
    [STR_ID_SETTINGS_GENERAL_OPEN_SOURCE] = "Open Source",
    [STR_ID_SETTINGS_GENERAL_OPEN_SOURCE_CONTENT] = "ElenixOS is open-sourced on GitHub:\n"
                                                    "https://github.com/\nElenixOS/ElenixOS",
    [STR_ID_SETTINGS_GENERAL_LEGAL_INFO] = "Legal info",
    [STR_ID_SETTINGS_GENERAL_LEGAL_INFO_CONTENT] = "ElenixOS is licensed under the Apache License, Version 2.0.\n"
                                                   "See full license at:\n"
                                                   "https://www.apache.org/\nlicenses/LICENSE-2.0",
    [STR_ID_SETTINGS_SOUNDS_AND_HAPTICS] = "Sounds & Haptics",
    [STR_ID_SETTINGS_SOUNDS_AND_ALERTS] = "Sounds & Alerts",
    [STR_ID_SETTINGS_SOUNDS_AND_HAPTICS_SILENT_MODE] = "Silent Mode",
    [STR_ID_SETTINGS_SOUNDS_AND_HAPTICS_VOLUME] = "Volume",
    [STR_ID_SETTINGS_APPS_CLEAR_DATA_SUCCESS] = "App data cleared",
    [STR_ID_APP_FLASH_LIGHT_DISMISS] = "Dismiss",
    [STR_ID_TOAST_SHOW_MUTE] = "Muted",
    [STR_ID_TOAST_SHOW_UNMUTE] = "Unmuted",
    [STR_ID_SECOND] = "sec",
    [STR_ID_SYS_INIT_FAILED] = "System initialization failed",
    [STR_ID_SYS_INIT_FAILED_CONTENT] = "An fatal error occurred during system initialization. Please restart the device. If the problem persists, contact support.",
    [STR_ID_SENSOR_ACCE] = "Accelerometer",
    [STR_ID_SENSOR_GYRO] = "Gyroscope",
    [STR_ID_SENSOR_MAG] = "Magnetometer",
    [STR_ID_SENSOR_TEMP] = "Temperature Sensor",
    [STR_ID_SENSOR_HUMI] = "Humidity Sensor",
    [STR_ID_SENSOR_BARO] = "Barometer",
    [STR_ID_SENSOR_LIGHT] = "Ambient Light Sensor",
    [STR_ID_SENSOR_PROXIMITY] = "Proximity Sensor",
    [STR_ID_SENSOR_HR] = "Heart Rate Sensor",
    [STR_ID_SENSOR_TVOC] = "TVOC Sensor",
    [STR_ID_SENSOR_NOISE] = "Noise Sensor",
    [STR_ID_SENSOR_STEP] = "Step Counter",
    [STR_ID_SENSOR_FORCE] = "Force Sensor",
    [STR_ID_SENSOR_BAT] = "Battery Sensor",
    // Add new string IDs and English translations here as needed
};

/**
 * @brief Chinese language array
 * @note Add new strings here
 */
const char *lang_zh[STR_ID_MAX_NUMBER] = {
    [STR_ID_LANGUAGE] = "简体中文",
    [STR_ID_ERROR] = "出错了",
    [STR_ID_OK] = "确定",
    [STR_ID_CANCEL] = "取消",
    [STR_ID_OFF] = "关闭",
    [STR_ID_NORMAL] = "正常",
    [STR_ID_INTENSE] = "强",
    [STR_ID_MSG_LIST_CLEAR_ALL] = "全部清除",
    [STR_ID_MSG_LIST_NO_MSG] = "没有消息",
    [STR_ID_MSG_LIST_ITEM_MARK_AS_READ] = "标记为已读",
    [STR_ID_BACK] = "返回",
    [STR_ID_TEST_LANG_STR] = "在夏末的午后，风把阳台上的风铃吹得叮当作响，像是某种不经意的暗号。",
    [STR_ID_APP_RUN_ERR_TITLE] = "应用程序已停止运行",
    [STR_ID_APP_RUN_ERR] = "应用程序在运行过程中出现了严重错误，无法继续执行。请将此问题反馈给开发者。",
    [STR_ID_WATCHFACE_RUN_ERR_TITLE] = "表盘已停止运行",
    [STR_ID_WATCHFACE_RUN_ERR] = "表盘在运行过程中出现了严重错误，无法继续执行。请将此问题反馈给开发者。",
    [STR_ID_WATCHFACE_SWITCH] = "切换表盘",
    [STR_ID_SETTINGS] = "设置",
    [STR_ID_SETTINGS_BLUETOOTH] = "蓝牙",
    [STR_ID_SETTINGS_BLUETOOTH_ENABLE] = "蓝牙",
    [STR_ID_SETTINGS_DISPLAY] = "显示",
    [STR_ID_SETTINGS_DISPLAY_BRIGHTNESS] = "亮度",
    [STR_ID_SETTINGS_DISPLAY_AOD] = "全天候显示",
    [STR_ID_SETTINGS_DISPLAY_AOD_COMMENT] = "Elenix Watch可始终显示时间。",
    [STR_ID_SETTINGS_WAKE] = "唤醒",
    [STR_ID_SETTINGS_WAKE_DURATION] = "唤醒时长",
    [STR_ID_SETTINGS_WAKE_ON_RAISE] = "抬腕时唤醒",
    [STR_ID_SETTINGS_WAKE_FOR_N_SECONDS] = "唤醒 %d 秒",
    [STR_ID_SETTINGS_WAKE_ON_TAP] = "轻点时",
    [STR_ID_SETTINGS_WAKE_ON_TAP_COMMENT] = "选择轻点唤醒Elenix Watch的屏幕后，保持唤醒的时长。",
    [STR_ID_SETTINGS_HAPTICS_STRENGTH] = "触感强度",
    [STR_ID_SETTINGS_HAPTICS] = "触感",
    [STR_ID_SETTINGS_NOTIFICATION] = "通知",
    [STR_ID_SETTINGS_APPS] = "应用程序",
    [STR_ID_SETTINGS_APPS_DETAILS] = "应用详情",
    [STR_ID_SETTINGS_APPS_APPID] = "ID",
    [STR_ID_SETTINGS_APPS_AUTHOR] = "作者",
    [STR_ID_SETTINGS_APPS_VERSION] = "版本",
    [STR_ID_SETTINGS_APPS_DESCRIPTON] = "描述",
    [STR_ID_SETTINGS_APPS_UNINSTALL] = "卸载",
    [STR_ID_SETTINGS_APPS_CLEAR_DATA] = "删除应用数据",
    [STR_ID_SETTINGS_GENERAL] = "通用",
    [STR_ID_SETTINGS_GENERAL_LANGUAGE] = "语言",
    [STR_ID_SETTINGS_GENERAL_DEVICE_INFO] = "设备信息",
    [STR_ID_SETTINGS_GENERAL_DEVICE_NAME] = "设备名称",
    [STR_ID_SETTINGS_GENERAL_EOS_VER] = "ElenixOS 版本",
    [STR_ID_SETTINGS_GENERAL_MARKETING_NAME] = "型号名称",
    [STR_ID_SETTINGS_GENERAL_MODEL_NUMBER] = "型号",
    [STR_ID_SETTINGS_GENERAL_OPEN_SOURCE] = "开源信息",
    [STR_ID_SETTINGS_GENERAL_OPEN_SOURCE_CONTENT] = "ElenixOS 已经在 GitHub 开源：\n"
                                                    "https://github.com/\nElenixOS/ElenixOS",
    [STR_ID_SETTINGS_GENERAL_LEGAL_INFO] = "法律信息",
    [STR_ID_SETTINGS_GENERAL_LEGAL_INFO_CONTENT] = "ElenixOS 采用 Apache License 2.0 授权。\n"
                                                   "更多信息请查看：\n"
                                                   "https://www.apache.org/\nlicenses/LICENSE-2.0",
    [STR_ID_SETTINGS_SOUNDS_AND_HAPTICS] = "声效与触感反馈",
    [STR_ID_SETTINGS_SOUNDS_AND_ALERTS] = "铃声与提醒",
    [STR_ID_SETTINGS_SOUNDS_AND_HAPTICS_SILENT_MODE] = "静音模式",
    [STR_ID_SETTINGS_SOUNDS_AND_HAPTICS_VOLUME] = "音量",
    [STR_ID_SETTINGS_APPS_CLEAR_DATA_SUCCESS] = "应用数据已清除",
    [STR_ID_APP_FLASH_LIGHT_DISMISS] = "忽略",
    [STR_ID_TOAST_SHOW_MUTE] = "已开启静音",
    [STR_ID_TOAST_SHOW_UNMUTE] = "已关闭静音",
    [STR_ID_SECOND] = "秒",
    [STR_ID_SYS_INIT_FAILED] = "系统初始化失败",
    [STR_ID_SYS_INIT_FAILED_CONTENT] = "系统初始化过程中发生了致命错误，请重启设备或联系开发者。",
    [STR_ID_SENSOR_ACCE] = "加速度传感器",
    [STR_ID_SENSOR_GYRO] = "重力传感器",
    [STR_ID_SENSOR_MAG] = "磁传感器",
    [STR_ID_SENSOR_TEMP] = "温度传感器",
    [STR_ID_SENSOR_HUMI] = "相对湿度传感器",
    [STR_ID_SENSOR_BARO] = "气压传感器",
    [STR_ID_SENSOR_LIGHT] = "环境光传感器",
    [STR_ID_SENSOR_PROXIMITY] = "距离传感器",
    [STR_ID_SENSOR_HR] = "心率传感器",
    [STR_ID_SENSOR_TVOC] = "TVOC传感器",
    [STR_ID_SENSOR_NOISE] = "噪声传感器",
    [STR_ID_SENSOR_STEP] = "计步传感器",
    [STR_ID_SENSOR_FORCE] = "力传感器",
    [STR_ID_SENSOR_BAT] = "Battery Sensor",
    // Add new string IDs and Chinese translations here as needed
};

static const char *const language_list[LANG_MAX_NUMBER] = {
    [LANG_EN] = "English",
    [LANG_ZH] = "简体中文"};

static const char **current_lang = NULL;   // Current language pointer
static bool lang_initialized = false; // Language system initialized flag

// Function declarations
static void lang_event_cb(lv_event_t *e);

/* Function Implementations -----------------------------------*/
void eos_lang_set_current_id(language_id_t lang);
language_id_t eos_lang_parse_name(const char *language_name);

void eos_lang_init(void)
{
    EOS_LOG_D("Init eos_lang");
    if (!lang_initialized)
    {
        const char *lang_str = eos_sys_cfg_get_string(EOS_SYS_CFG_KEY_LANGUAGE_STR, "English");
        eos_lang_set_current_id(eos_lang_parse_name(lang_str));
        eos_free(lang_str);
        lang_initialized = true;
    }
}

void eos_lang_set_current_id(language_id_t lang)
{
    switch (lang)
    {
    case LANG_EN:
        current_lang = lang_en;
        break;
    case LANG_ZH:
        current_lang = lang_zh;
        break;
    default:
        current_lang = lang_en;
        break;
    }

    // Use event broadcast system to refresh all labels
    eos_event_broadcast(LV_EVENT_REFRESH, NULL);

    EOS_LOG_D("Language changed");
}

language_id_t eos_lang_get_current_id(void)
{
    if (current_lang == lang_zh)
    {
        return LANG_ZH;
    }
    else if (current_lang == lang_en)
    {
        return LANG_EN;
    }
    else
    {
        return LANG_EN;
    }
}

language_id_t eos_lang_parse_name(const char *language_name)
{
    EOS_CHECK_PTR_RETURN_VAL(language_name, LANG_EN);

    if (strcmp(language_list[LANG_EN], language_name) == 0)
    {
        return LANG_EN;
    }
    else if (strcmp(language_list[LANG_ZH], language_name) == 0)
    {
        return LANG_ZH;
    }
    else
    {
        EOS_LOG_E("Language not found: %s", language_name);
        return LANG_EN;
    }
}

const char *eos_lang_get_name(language_id_t lang)
{
    // Get language name by ID
    if (lang < 0 || lang >= LANG_MAX_NUMBER)
    {
        return language_list[LANG_EN];
    }

    return language_list[lang];
}

const char *eos_lang_get_current_name(void)
{
    return eos_lang_get_name(eos_lang_get_current_id());
}

const char *eos_lang_get_text(lang_string_id_t id)
{
    if (id < 0 || id >= STR_ID_MAX_NUMBER || !current_lang || !current_lang[id])
        return NULL;

    return current_lang[id];
}

language_id_t eos_lang_get_current_id_with_str(const char *language_str)
{
    return eos_lang_parse_name(language_str);
}

static void lang_event_cb(lv_event_t *e)
{
    lv_obj_t *label = lv_event_get_target(e);

    // user_data stores str_id + 1 to avoid STR_ID_LANGUAGE(0) conflict with NULL
    uintptr_t raw_id = (uintptr_t)lv_event_get_user_data(e);
    if (raw_id == 0)
    {
        EOS_LOG_E("No id for label");
        return;
    }

    lang_string_id_t id = (lang_string_id_t)(raw_id - 1U);
    const char *text = eos_lang_get_text(id);
    if (text)
    {
        lv_label_set_text(label, text);
    }
}

static void _lang_label_delete_cb(lv_event_t *e)
{
    lv_obj_t *label = lv_event_get_target(e);
    eos_event_remove_cb(label, LV_EVENT_REFRESH, lang_event_cb);
}

void eos_label_set_text_id(lv_obj_t *label, lang_string_id_t str_id)
{
    EOS_CHECK_PTR_RETURN(label);

    const char *text = eos_lang_get_text(str_id);
    if (text)
    {
        lv_label_set_text(label, text);
    }

    eos_event_remove_cb(label, LV_EVENT_REFRESH, lang_event_cb);
    eos_event_add_cb(label, lang_event_cb, LV_EVENT_REFRESH, (void *)((uintptr_t)str_id + 1U));
}

lv_obj_t *eos_lang_label_create(lv_obj_t *parent, lang_string_id_t str_id)
{
    EOS_CHECK_PTR_RETURN_VAL(parent, NULL);

    // Create label
    lv_obj_t *label = lv_label_create(parent);
    if (!label)
        return NULL;

    eos_label_set_text_id(label, str_id);

    return label;
}
