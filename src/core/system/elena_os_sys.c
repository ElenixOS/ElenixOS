/**
 * @file elena_os_sys.c
 * @brief 系统配置
 * @author Sab1e
 * @date 2025-08-21
 */

#include "elena_os_sys.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lvgl.h"
#include "cJSON.h"
#include "elena_os_img.h"
#include "elena_os_msg_list.h"
#include "elena_os_lang.h"
#define EOS_LOG_TAG "System"
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
#include "elena_os_fs.h"
#include "elena_os_afw.h"
#include "elena_os_dfw.h"
/* Macros and Definitions -------------------------------------*/
#define EOS_SYS_DEFAULT_LANG_STR "English"
#define EOS_SYS_DEFAULT_WATCHFACE_ID_STR "cn.sab1e.clock"
/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

/* Helper: load/save config JSON using eos_fs_* APIs */
static inline cJSON *eos_sys_load_config(int *err_code)
{
    if (err_code)
        *err_code = -EOS_ERR_FILE_ERROR;

    if (!eos_is_file(EOS_SYS_CONFIG_FILE_PATH))
    {
        EOS_LOG_E("Config file does not exist");
        return NULL;
    }

    char *file_content = eos_dfw_read(EOS_SYS_CONFIG_FILE_PATH);
    if (!file_content)
    {
        EOS_LOG_E("Failed to read config file");
        return NULL;
    }

    cJSON *root = cJSON_Parse(file_content);
    eos_free(file_content);

    if (!root)
    {
        EOS_LOG_E("Failed to parse JSON");
        return NULL;
    }

    if (err_code)
        *err_code = EOS_OK;
    return root;
}

static inline int eos_sys_save_config(cJSON *root)
{
    if (!root)
        return -EOS_ERR_JSON_ERROR;

    char *new_json = cJSON_PrintUnformatted(root);
    if (!new_json)
    {
        EOS_LOG_E("Failed to generate JSON");
        return -EOS_ERR_JSON_ERROR;
    }

    size_t json_len = strlen(new_json);
    int ret = eos_dfw_write(EOS_SYS_CONFIG_FILE_PATH, new_json, json_len);

    cJSON_free(new_json);

    if (ret < 0)
    {
        EOS_LOG_E("Write json failed, error: %d", ret);
        return -EOS_ERR_FILE_ERROR;
    }

    return EOS_OK;
}

/* Refactored config functions using helpers */

eos_result_t eos_sys_cfg_set_bool(const char *key, bool value)
{
    EOS_CHECK_PTR_RETURN_VAL(key, EOS_ERR_VAR_NULL);

    EOS_LOG_I("Try set \"%s\" = \"%s\"", key, value ? "true" : "false");

    int err;
    cJSON *root = eos_sys_load_config(&err);
    if (!root)
        return err;

    cJSON *item = cJSON_GetObjectItem(root, key);
    if (item)
        cJSON_SetBoolValue(item, value);
    else
        cJSON_AddBoolToObject(root, key, value);

    int save_ret = eos_sys_save_config(root);
    cJSON_Delete(root);

    if (save_ret != EOS_OK)
        return EOS_ERR_FILE_ERROR;

    EOS_LOG_I("Successfully set config item: %s=%s", key, value ? "true" : "false");
    return EOS_OK;
}

eos_result_t eos_sys_cfg_set_string(const char *key, const char *value)
{
    EOS_CHECK_PTR_RETURN_VAL(key && value, EOS_ERR_VAR_NULL);

    EOS_LOG_I("Try set \"%s\" = \"%s\"", key, value);

    int err;
    cJSON *root = eos_sys_load_config(&err);
    if (!root)
        return err;

    cJSON *item = cJSON_GetObjectItem(root, key);
    if (item)
        cJSON_SetValuestring(item, value);
    else
        cJSON_AddStringToObject(root, key, value);

    int save_ret = eos_sys_save_config(root);
    cJSON_Delete(root);

    if (save_ret != EOS_OK)
        return EOS_ERR_FILE_ERROR;

    EOS_LOG_I("Successfully set config item: %s=%s", key, value);
    return EOS_OK;
}

eos_result_t eos_sys_cfg_set_number(const char *key, double value)
{
    EOS_CHECK_PTR_RETURN_VAL(key, EOS_ERR_VAR_NULL);

    EOS_LOG_I("Try set \"%s\" = \"%f\"", key, value);

    int err;
    cJSON *root = eos_sys_load_config(&err);
    if (!root)
        return err;

    cJSON *item = cJSON_GetObjectItem(root, key);
    if (item)
        cJSON_SetNumberValue(item, value);
    else
        cJSON_AddNumberToObject(root, key, value);

    int save_ret = eos_sys_save_config(root);
    cJSON_Delete(root);

    if (save_ret != EOS_OK)
        return EOS_ERR_FILE_ERROR;

    EOS_LOG_I("Successfully set config item: %s=%f", key, value);
    return EOS_OK;
}

bool eos_sys_cfg_get_bool(const char *key, bool default_value)
{
    EOS_CHECK_PTR_RETURN_VAL(key, default_value);

    int err;
    cJSON *root = eos_sys_load_config(&err);
    if (!root)
    {
        EOS_LOG_W("Returning default value for key '%s'", key);
        return default_value;
    }

    cJSON *item = cJSON_GetObjectItem(root, key);
    if (!item || !cJSON_IsBool(item))
    {
        if (!item)
            EOS_LOG_D("Key '%s' not found in config, returning default", key);
        else
            EOS_LOG_W("Value for key '%s' is not a boolean, returning default", key);

        cJSON_Delete(root);
        return default_value;
    }

    bool result = cJSON_IsTrue(item);
    cJSON_Delete(root);

    EOS_LOG_D("Successfully got boolean config item: %s=%s", key, result ? "true" : "false");
    return result;
}

char *eos_sys_cfg_get_string(const char *key, const char *default_value)
{
    EOS_CHECK_PTR_RETURN_VAL(key, default_value);

    int err;
    cJSON *root = eos_sys_load_config(&err);
    if (!root)
    {
        EOS_LOG_W("Returning default value for key '%s'", key);
        return eos_strdup(default_value);
    }

    cJSON *item = cJSON_GetObjectItem(root, key);
    if (!item || !cJSON_IsString(item))
    {
        if (!item)
            EOS_LOG_D("Key '%s' not found in config, returning default", key);
        else
            EOS_LOG_W("Value for key '%s' is not a string, returning default", key);

        cJSON_Delete(root);
        return eos_strdup(default_value);
    }

    char *result = eos_strdup(item->valuestring);
    cJSON_Delete(root);

    if (!result)
    {
        EOS_LOG_E("Failed to duplicate string, returning default");
        return eos_strdup(default_value);
    }

    EOS_LOG_D("Successfully got string config item: %s=%s", key, result);
    return result;
}

double eos_sys_cfg_get_number(const char *key, double default_value)
{
    EOS_CHECK_PTR_RETURN_VAL(key, default_value);

    int err;
    cJSON *root = eos_sys_load_config(&err);
    if (!root)
    {
        EOS_LOG_W("Returning default value for key '%s'", key);
        return default_value;
    }

    cJSON *item = cJSON_GetObjectItem(root, key);
    if (!item || !cJSON_IsNumber(item))
    {
        if (!item)
            EOS_LOG_D("Key '%s' not found in config, returning default", key);
        else
            EOS_LOG_W("Value for key '%s' is not a number, returning default", key);

        cJSON_Delete(root);
        return default_value;
    }

    double result = item->valuedouble;
    cJSON_Delete(root);

    EOS_LOG_D("Successfully got number config item: %s=%f", key, result);
    return result;
}

eos_result_t _create_default_cfg_json(const char *path)
{
    // 创建 JSON 对象
    cJSON *root = cJSON_CreateObject();
    if (!root)
    {
        return -EOS_ERR_JSON_ERROR;
    }

    cJSON_AddStringToObject(root, EOS_SYS_CFG_KEY_VERSION_STR, ELENA_OS_VERSION_FULL);
    cJSON_AddStringToObject(root, EOS_SYS_CFG_KEY_LANGUAGE_STR, EOS_SYS_DEFAULT_LANG_STR);
    cJSON_AddStringToObject(root, EOS_SYS_CFG_KEY_WATCHFACE_ID_STR, EOS_SYS_DEFAULT_WATCHFACE_ID_STR);
    // 转换为字符串
    char *json_str = cJSON_PrintUnformatted(root);
    if (!json_str)
    {
        cJSON_Delete(root);
        return -EOS_ERR_JSON_ERROR;
    }

    // 写入文件
    int ret = eos_create_file_if_not_exist(path, json_str);

    // 释放内存
    cJSON_free(json_str);
    cJSON_Delete(root);

    return ret;
}

void eos_sys_init()
{
    EOS_LOG_D("Init eos_sys");
    // 判断系统文件是否存在
    eos_fs_mkdir_if_not_exist(EOS_SYS_DIR);
    eos_fs_mkdir_if_not_exist(EOS_SYS_CONFIG_DIR);

    eos_fs_mkdir_if_not_exist(EOS_APP_DIR);
    eos_fs_mkdir_if_not_exist(EOS_APP_INSTALLED_DIR);
    eos_fs_mkdir_if_not_exist(EOS_APP_DATA_DIR);

    eos_fs_mkdir_if_not_exist(EOS_WATCHFACE_DIR);
    eos_fs_mkdir_if_not_exist(EOS_WATCHFACE_INSTALLED_DIR);
    eos_fs_mkdir_if_not_exist(EOS_WATCHFACE_DATA_DIR);

    eos_fs_mkdir_if_not_exist(EOS_SYS_RES_DIR);
    eos_fs_mkdir_if_not_exist(EOS_SYS_RES_IMG_DIR);
    eos_fs_mkdir_if_not_exist(EOS_SYS_RES_FONT_DIR);

    // 如果系统文件不存在则创建
    eos_file_t fp = eos_fs_open_read(EOS_SYS_CONFIG_FILE_PATH);
    uint32_t size = 0;
    eos_fs_size(fp, &size);
    eos_fs_close(fp);

    if ((!eos_is_file(EOS_SYS_CONFIG_FILE_PATH)) || size == 0)
    {
        eos_fs_remove(EOS_SYS_CONFIG_FILE_PATH);
        if (_create_default_cfg_json(EOS_SYS_CONFIG_FILE_PATH) != EOS_OK)
        {
            EOS_LOG_E("Create default config json failed");
            EOS_ASSERT(0);
        }
    }

    /************************** 加载系统设置 **************************/
    // 蓝牙设置
    if (eos_sys_cfg_get_bool(EOS_SYS_CFG_KEY_BLUETOOTH_BOOL, false))
    {
        eos_bluetooth_enable();
        EOS_LOG_I("Bluetooth enable");
    }
    else
    {
        eos_bluetooth_disable();
        EOS_LOG_I("Bluetooth disable");
    }

    // 显示设置
    uint8_t brightness = eos_sys_cfg_get_number(EOS_SYS_CFG_KEY_DISPLAY_BRIGHTNESS_NUMBER, 50);
    if (brightness < EOS_DISPLAY_BRIGHTNESS_MIN || brightness > EOS_DISPLAY_BRIGHTNESS_MAX)
        brightness = 50;
    eos_display_set_brightness(brightness);
    EOS_LOG_I("Display brightness set: %d", brightness);

    // 静音设置
    bool mute = eos_sys_cfg_get_bool(EOS_SYS_CFG_KEY_MUTE_BOOL, false);
    if (mute)
    {
        eos_speaker_set_volume(0);
        EOS_LOG_I("Silent mode ON");
    }
    else
    {
        uint8_t volume = eos_sys_cfg_get_number(EOS_SYS_CFG_KEY_SPEAKER_VOLUME_NUMBER, 20);
        eos_speaker_set_volume(volume);
        EOS_LOG_I("Volume: %d", volume);
    }
}

/**
 * @brief 系统恢复出厂设置
 * @warning 未测试
 */
void eos_sys_factory_reset()
{
    // TODO: 完成此功能
    // int ret = rmdir(EOS_SYS_DIR);
    // if (ret != 0)
    // {
    //     EOS_LOG_E("Can't delete " EOS_SYS_DIR);
    //     return;
    // }
    // eos_cpu_reset();
}

eos_result_t eos_sys_add_config_item(const char *key, const char *value)
{
    if (!key || !value)
    {
        EOS_LOG_E("Invalid parameters: key or value is NULL");
        return -EOS_ERR_VAR_NULL;
    }

    int err;
    cJSON *root = eos_sys_load_config(&err);
    if (!root)
        return err;

    if (cJSON_HasObjectItem(root, key))
    {
        EOS_LOG_W("Key '%s' already exists in config", key);
        cJSON_Delete(root);
        return -EOS_ERR_JSON_ERROR;
    }

    cJSON_AddStringToObject(root, key, value);

    int save_ret = eos_sys_save_config(root);
    cJSON_Delete(root);

    if (save_ret != EOS_OK)
        return EOS_ERR_FILE_ERROR;

    EOS_LOG_I("Successfully added new config item: %s=%s", key, value);
    return EOS_OK;
}
