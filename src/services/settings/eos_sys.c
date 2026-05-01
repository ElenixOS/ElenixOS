/**
 * @file eos_sys.c
 * @brief System configuration
 */

#include "eos_sys.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lvgl.h"
#include "cJSON.h"
#include "eos_img.h"
#include "eos_msg_list.h"
#include "eos_lang.h"
#define EOS_LOG_TAG "System"
#include "eos_log.h"
#include "eos_basic_widgets.h"
#include "eos_event.h"
#include "eos_test.h"
#include "eos_version.h"
#include "eos_port.h"
#include "eos_swipe_panel.h"
#include "eos_app.h"
#include "eos_watchface.h"
#include "eos_misc.h"
#include "eos_theme.h"
#include "eos_pkg_mgr.h"
#include "eos_sensor.h"
#include "eos_service_storage.h"
#include "eos_mem.h"
#include "eos_vibrator.h"
/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

static cJSON *eos_sys_load_config(int *err_code)
{
    if (err_code)
        *err_code = EOS_ERR_FILE_ERROR;

    if (!eos_storage_is_file(EOS_SYS_CONFIG_FILE_PATH))
    {
        EOS_LOG_E("Config file does not exist");
        return NULL;
    }
    char *file_content = eos_storage_read_file(EOS_SYS_CONFIG_FILE_PATH);

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

static int eos_sys_save_config(cJSON *root)
{
    if (!root)
        return EOS_ERR_JSON_ERROR;

    char *new_json = cJSON_PrintUnformatted(root);
    if (!new_json)
    {
        EOS_LOG_E("Failed to generate JSON");
        return EOS_ERR_JSON_ERROR;
    }

    eos_result_t ret = eos_storage_write_file(EOS_SYS_CONFIG_FILE_PATH, new_json, strlen(new_json));

    cJSON_free(new_json);

    if (ret != EOS_OK)
    {
        EOS_LOG_E("Write json failed, error: %d", ret);
        return EOS_ERR_FILE_ERROR;
    }
    eos_event_broadcast(EOS_EVENT_SYSTEM_CONFIG_UPDATE, NULL);

    return EOS_OK;
}

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
    cJSON *root = cJSON_CreateObject();
    if (!root)
    {
        return EOS_ERR_JSON_ERROR;
    }

    cJSON_AddStringToObject(root, EOS_SYS_CFG_KEY_DEVICE_NAME_STR, EOS_SYS_DEFAULT_DEVICE_NAME);
    cJSON_AddStringToObject(root, EOS_SYS_CFG_KEY_LANGUAGE_STR, EOS_SYS_DEFAULT_LANG_STR);
    cJSON_AddStringToObject(root, EOS_SYS_CFG_KEY_WATCHFACE_ID_STR, EOS_SYS_DEFAULT_WATCHFACE_ID_STR);
    char *json_str = cJSON_PrintUnformatted(root);
    if (!json_str)
    {
        cJSON_Delete(root);
        return EOS_ERR_JSON_ERROR;
    }

    eos_result_t ret = eos_storage_create_file_if_not_exist(path, json_str);

    cJSON_free(json_str);
    cJSON_Delete(root);

    return ret;
}

void eos_sys_init()
{
    EOS_LOG_D("Init eos_sys");
    eos_storage_mkdir_if_not_exist(EOS_SYS_DIR);
    eos_storage_mkdir_if_not_exist(EOS_SYS_CONFIG_DIR);

    eos_storage_mkdir_if_not_exist(EOS_APP_DIR);
    eos_storage_mkdir_if_not_exist(EOS_APP_INSTALLED_DIR);
    eos_storage_mkdir_if_not_exist(EOS_APP_DATA_DIR);

    eos_storage_mkdir_if_not_exist(EOS_WATCHFACE_DIR);
    eos_storage_mkdir_if_not_exist(EOS_WATCHFACE_INSTALLED_DIR);
    eos_storage_mkdir_if_not_exist(EOS_WATCHFACE_DATA_DIR);

    eos_storage_mkdir_if_not_exist(EOS_SYS_RES_DIR);
    eos_storage_mkdir_if_not_exist(EOS_SYS_RES_IMG_DIR);
    eos_storage_mkdir_if_not_exist(EOS_SYS_RES_FONT_DIR);

    eos_file_t fp = eos_fs_open_read(EOS_SYS_CONFIG_FILE_PATH);
    uint32_t size = 0;
    eos_fs_size(fp, &size);
    eos_fs_close(fp);

    if ((!eos_storage_is_file(EOS_SYS_CONFIG_FILE_PATH)) || size == 0)
    {
        eos_fs_remove(EOS_SYS_CONFIG_FILE_PATH);
        if (_create_default_cfg_json(EOS_SYS_CONFIG_FILE_PATH) != EOS_OK)
        {
            EOS_LOG_E("Create default config json failed");
            EOS_ASSERT(0);
        }
    }

    /************************** Load system settings **************************/
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

    uint8_t brightness = eos_sys_cfg_get_number(EOS_SYS_CFG_KEY_DISPLAY_BRIGHTNESS_NUMBER, 50);
    if (brightness < EOS_DISPLAY_BRIGHTNESS_MIN || brightness > EOS_DISPLAY_BRIGHTNESS_MAX)
        brightness = 50;
    eos_display_set_brightness(brightness);
    EOS_LOG_I("Display brightness set: %d", brightness);

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

    uint8_t strength = eos_sys_cfg_get_number(EOS_SYS_CFG_KEY_VIBRATOR_STRENGTH_NUMBER, EOS_VIBRATOR_STRENGTH_NORMAL);
    eos_vibrator_set_strength(strength);
    EOS_LOG_I("Vibrator strength: %d", strength);
}

/**
 * @brief System factory reset
 * @warning Untested
 */
void eos_sys_factory_reset()
{
    // TODO: Complete this function
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
        return EOS_ERR_VAR_NULL;
    }

    int err;
    cJSON *root = eos_sys_load_config(&err);
    if (!root)
        return err;

    if (cJSON_HasObjectItem(root, key))
    {
        EOS_LOG_W("Key '%s' already exists in config", key);
        cJSON_Delete(root);
        return EOS_ERR_JSON_ERROR;
    }

    cJSON_AddStringToObject(root, key, value);

    int save_ret = eos_sys_save_config(root);
    cJSON_Delete(root);

    if (save_ret != EOS_OK)
        return EOS_ERR_FILE_ERROR;

    EOS_LOG_I("Successfully added new config item: %s=%s", key, value);
    return EOS_OK;
}
