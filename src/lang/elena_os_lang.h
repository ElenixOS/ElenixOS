/**
 * @file elena_os_lang.h
 * @brief 多语言系统
 * @author Sab1e
 * @date 2025-08-14
 * @details
 *
 * # Lang
 *
 * ## 简述
 *
 * 多语言系统主要用于多种语言的动态切换。
 *
 * ## 使用方法
 *
 * 只需要在需要字符串的标签`lv_label`的文字处，使用`current_lang[<String ID>]`即可获取当前语言的字符串。
 *
 * 示例：
 *
 * ```c
 *
 * // System init
 * eos_lang_init();
 *
 * // Somewhere
 * eos_lang_set(LANG_EN);
 * lv_obj_t *label = lv_label_create(eos_screen_active());
 * lv_label_set_text(label, current_lang[STR_ID_LANGUAGE]);
 * lv_obj_center(label);
 *
 * ```
 *
 * 显示：
 * -------------
 * |           |
 * |  English  |
 * |           |
 * -------------
 *
 */

#ifndef ELENA_OS_LANG_H
#define ELENA_OS_LANG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/**
 * @brief 语言类型
 */
typedef enum{
    LANG_EN=0,
    LANG_ZH,
    LANG_MAX_NUMBER
} language_id_t;

/**
 * @brief 字符串索引
 * @note 此处可添加新的字符串ID
 */
typedef enum {
    STR_ID_LANGUAGE,
    STR_ID_ERROR,
    STR_ID_OK,
    STR_ID_CANCEL,
    STR_ID_MSG_LIST_CLEAR_ALL,
    STR_ID_MSG_LIST_NO_MSG,
    STR_ID_MSG_LIST_ITEM_MARK_AS_READ,
    STR_ID_BACK,
    STR_ID_TEST_LANG_STR,
    STR_ID_APP_RUN_ERR_TITLE,
    STR_ID_APP_RUN_ERR,
    STR_ID_WATCHFACE_RUN_ERR_TITLE,
    STR_ID_WATCHFACE_RUN_ERR,
    STR_ID_WATCHFACE_SWITCH,
    STR_ID_SETTINGS,
    STR_ID_SETTINGS_BLUETOOTH,
    STR_ID_SETTINGS_BLUETOOTH_ENABLE,
    STR_ID_SETTINGS_DISPLAY,
    STR_ID_SETTINGS_DISPLAY_BRIGHTNESS,
    STR_ID_SETTINGS_DISPLAY_AOD,
    STR_ID_SETTINGS_DISPLAY_AOD_COMMENT,
    STR_ID_SETTINGS_WAKE,
    STR_ID_SETTINGS_WAKE_DURATION,
    STR_ID_SETTINGS_WAKE_FOR_N_SECONDS,
    STR_ID_SETTINGS_WAKE_ON_TAP,
    STR_ID_SETTINGS_WAKE_ON_TAP_COMMENT,
    STR_ID_SETTINGS_WAKE_ON_RAISE,
    STR_ID_SETTINGS_NOTIFICATION,
    STR_ID_SETTINGS_APPS,
    STR_ID_SETTINGS_APPS_DETAILS,
    STR_ID_SETTINGS_APPS_APPID,
    STR_ID_SETTINGS_APPS_AUTHOR,
    STR_ID_SETTINGS_APPS_VERSION,
    STR_ID_SETTINGS_APPS_DESCRIPTON,
    STR_ID_SETTINGS_APPS_UINSTALL,
    STR_ID_SETTINGS_APPS_CLEAR_DATA,
    STR_ID_SETTINGS_GENERAL,
    STR_ID_SETTINGS_GENERAL_LANGUAGE,
    STR_ID_SETTINGS_GENERAL_DEVICE_INFO,
    STR_ID_SETTINGS_GENERAL_DEVICE_NAME,
    STR_ID_SETTINGS_GENERAL_EOS_VER,
    STR_ID_SETTINGS_GENERAL_MARKETING_NAME,
    STR_ID_SETTINGS_GENERAL_MODEL_NUMBER,
    STR_ID_SETTINGS_GENERAL_LEGAL_INFO,
    STR_ID_SETTINGS_GENERAL_LEGAL_INFO_CONTENT,
    STR_ID_SETTINGS_GENERAL_OPEN_SOURCE,
    STR_ID_SETTINGS_GENERAL_OPEN_SOURCE_CONTENT,
    STR_ID_SETTINGS_SOUND_AND_HAPTICS,
    STR_ID_SETTINGS_SOUND_AND_HAPTICS_SILENT_MODE,
    STR_ID_SETTINGS_SOUND_AND_HAPTICS_VOLUME,
    STR_ID_APP_FLASH_LIGHT_DISMISS,
    STR_ID_TOAST_SHOW_MUTE,
    STR_ID_TOAST_SHOW_UNMUTE,
    STR_ID_SECOND,

    STR_ID_SENSOR_START,
    STR_ID_SENSOR_ACCE,      /**< 加速度传感器     */
    STR_ID_SENSOR_GYRO,      /**< 重力传感器       */
    STR_ID_SENSOR_MAG,       /**< 磁传感器         */
    STR_ID_SENSOR_TEMP,      /**< 温度传感器       */
    STR_ID_SENSOR_HUMI,      /**< 相对湿度传感器   */
    STR_ID_SENSOR_BARO,      /**< 气压传感器       */
    STR_ID_SENSOR_LIGHT,     /**< 环境光传感器     */
    STR_ID_SENSOR_PROXIMITY, /**< 距离传感器       */
    STR_ID_SENSOR_HR,        /**< 心率传感器       */
    STR_ID_SENSOR_TVOC,      /**< TOVC传感器       */
    STR_ID_SENSOR_NOISE,     /**< 噪声传感器       */
    STR_ID_SENSOR_STEP,      /**< 计步传感器       */
    STR_ID_SENSOR_FORCE,     /**< 力传感器         */
    STR_ID_SENSOR_BAT,        /**< 电池电量传感器    */
    /* 此处可添加新的字符串ID */
    STR_ID_MAX_NUMBER   /**< 字符串ID最大值 */
} lang_string_id_t;

/* Public function prototypes --------------------------------*/

/**
 * @brief 获取当前语言的字符串
 *
 * 例如当前语言是英语，那么 `current_lang[STR_ID_LANGUAGE]` 等于 `"English"`
 */
extern const char** current_lang;
extern const char *language_list[LANG_MAX_NUMBER];
/**
 * @brief 初始化语言系统
 */
void eos_lang_init(void);
/**
 * @brief 设置当前语言
 * @param lang 目标语言类型 `language_id_t`
 * @warning 需要先初始化语言系统
 */
void eos_lang_set(language_id_t lang);
/**
 * @brief 获取当前语言类型
 * @return language_id_t 语言类型
 */
language_id_t eos_lang_get(void);
/**
 * @brief 通过语言字符串获得语言类型
 *
 * 例如`English`返回`LANG_EN`
 * @param language_str 字符串
 * @return language_id_t 语言类型
 */
language_id_t eos_lang_get_with_str(const char *language_str);
/**
 * @brief 获取当前语言字符串
 * @return char* 语言字符串（例如：简体中文）
 */
char *eos_lang_get_language_str(void);
/**
 * @brief 通过字符串 ID 设置标签的字符串
 * @param label 标签对象
 * @param str_id 字符串 ID
 */
void eos_label_set_text_id(lv_obj_t *label, uint32_t str_id);
/**
 * @brief 支持格式化字符串
 * @param label
 * @param fmt
 * @return eos_label_lang_fmt_t*
 */
void eos_label_set_text_fmt(lv_obj_t *label, const char *fmt, ...);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_LANG_H */
