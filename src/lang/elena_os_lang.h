/**
 * @file elena_os_lang.h
 * @brief 多语言系统
 * @author Sab1e
 * @date 2025-08-14
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
    STR_ID_OK,
    STR_ID_CANCEL,
    STR_ID_MSG_LIST_CLEAR_ALL,
    STR_ID_MSG_LIST_NO_MSG,
    STR_ID_MSG_LIST_ITEM_MARK_AS_READ,
    STR_ID_BASE_ITEM_BACK,
    STR_ID_TEST_LANG_STR,
    STR_ID_SCRIPT_RUN_ERR,
    STR_ID_SETTINGS,
    STR_ID_SETTINGS_BLUETOOTH,
    STR_ID_SETTINGS_BLUETOOTH_ENABLE,
    STR_ID_SETTINGS_DISPLAY,
    STR_ID_SETTINGS_DISPLAY_BRIGHTNESS,
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
    STR_ID_SETTINGS_SOUND_AND_HAPTICS,
    STR_ID_SETTINGS_SOUND_AND_HAPTICS_SILENT_MODE,
    STR_ID_SETTINGS_SOUND_AND_HAPTICS_VOLUME,
    STR_ID_APP_FLASH_LIGHT_DISMISS,
    STR_ID_TOAST_SHOW_MUTE,
    STR_ID_TOAST_SHOW_UNMUTE,
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
 * @brief 创建一个支持多语言的 LVGL label
 * @param parent label 的父级 LVGL 对象
 * @param str_id 字符串 ID
 * @return lv_obj_t* 返回创建的 label
 */
lv_obj_t * eos_lang_label_create(lv_obj_t * parent, uint32_t str_id);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_LANG_H */
