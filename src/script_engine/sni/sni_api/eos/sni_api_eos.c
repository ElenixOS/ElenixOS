/**
 * @file sni_api_eos.c
 * @brief ElenixOS API
 */

#include "sni_api_eos.h"

/* Includes ---------------------------------------------------*/
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "lvgl.h"
#include "sni_type_bridge.h"
#include "sni_types.h"
#include "sni_context.h"
#include "sni_callback_runtime.h"
#include "sni_api_export.h"
#include "eos_log.h"
#include "eos_mem.h"
#include "script_engine_core.h"
#include "eos_font.h"
#include "eos_activity.h"
#include "eos_service_time.h"
#include "eos_app.h"
#include "eos_watchface.h"
#include "eos_service_storage.h"
#include "eos_app_header.h"
#include "sni_api_eos_ww.h"
#include "sni_api_eos_metric.h"
#include "sni_api_eos_permission.h"
#include "eos_service_sensor.h"
/* Macros and Definitions -------------------------------------*/
#define EOS_API_NAME "eos"
#define CONSOLE_LOG_TAG script_engine_get_current_script_id()
/* Variables --------------------------------------------------*/
static jerry_value_t eos_api_obj;

static bool _sni_api_eos_number_to_u32(jerry_value_t value, uint32_t *out)
{
    double number;

    if (!out || !jerry_value_is_number(value))
        return false;

    number = jerry_value_as_number(value);
    if (!isfinite(number) || number < 0.0 || number > (double)UINT32_MAX || floor(number) != number)
        return false;

    *out = (uint32_t)number;
    return true;
}

static bool _sni_api_eos_number_to_sensor_type(jerry_value_t value, eos_sensor_type_t *out)
{
    uint32_t type;

    if (!out || !_sni_api_eos_number_to_u32(value, &type) || type <= (uint32_t)EOS_SENSOR_TYPE_UNKNOWN
        || type >= (uint32_t)EOS_SENSOR_TYPE_MAX)
        return false;

    *out = (eos_sensor_type_t)type;
    return true;
}

typedef enum
{
    EOS_CONSOLE_LEVEL_LOG,
    EOS_CONSOLE_LEVEL_ERROR,
    EOS_CONSOLE_LEVEL_WARN,
    EOS_CONSOLE_LEVEL_DEBUG,
} eos_console_level_t;

static bool sni_api_eos_to_c_string(jerry_value_t js_val, char **out_str)
{
    if (!out_str || !jerry_value_is_string(js_val))
    {
        return false;
    }

    jerry_size_t str_len = jerry_string_size(js_val, JERRY_ENCODING_UTF8);
    char *str = eos_malloc(str_len + 1);
    if (!str)
    {
        return false;
    }

    jerry_string_to_buffer(js_val, JERRY_ENCODING_UTF8, (jerry_char_t *)str, str_len);
    str[str_len] = '\0';

    *out_str = str;
    return true;
}

static char *sni_api_eos_get_assets_file_str(jerry_value_t js_val)
{
    char *src = NULL;
    char path[EOS_FS_PATH_MAX];
    char *ret = NULL;
    const char *script_id = NULL;

    if (!sni_api_eos_to_c_string(js_val, &src))
    {
        return NULL;
    }

    script_id = script_engine_get_current_script_id();
    if (!script_id)
    {
        eos_free(src);
        return NULL;
    }

    if (script_engine_get_current_script_type() == SCRIPT_TYPE_APPLICATION)
    {
        snprintf(path, sizeof(path), EOS_APP_INSTALLED_DIR "%s/assets/%s", script_id, src);
    }
    else if (script_engine_get_current_script_type() == SCRIPT_TYPE_WATCHFACE)
    {
        snprintf(path, sizeof(path), EOS_WATCHFACE_INSTALLED_DIR "%s/assets/%s", script_id, src);
    }
    else
    {
        eos_free(src);
        return NULL;
    }

    eos_free(src);

    if (!eos_storage_is_file(path))
    {
        return NULL;
    }

    ret = eos_malloc(strlen(path) + 1);
    if (!ret)
    {
        return NULL;
    }

    strcpy(ret, path);
    return ret;
}

static bool sni_api_eos_config_write_to_file(cJSON *root)
{
    char *json_str = NULL;
    char config_file_path[EOS_FS_PATH_MAX];
    bool ret;

    if (!root)
    {
        return false;
    }

    json_str = cJSON_PrintUnformatted(root);
    if (!json_str)
    {
        return false;
    }

    if (script_engine_get_current_script_type() == SCRIPT_TYPE_APPLICATION)
    {
        snprintf(config_file_path,
                 sizeof(config_file_path),
                 EOS_APP_DATA_DIR "%s",
                 script_engine_get_current_script_id());
        eos_storage_mkdir_if_not_exist(config_file_path);
        snprintf(config_file_path,
                 sizeof(config_file_path),
                 EOS_APP_DATA_DIR "%s/config.json",
                 script_engine_get_current_script_id());
    }
    else if (script_engine_get_current_script_type() == SCRIPT_TYPE_WATCHFACE)
    {
        snprintf(config_file_path,
                 sizeof(config_file_path),
                 EOS_WATCHFACE_DATA_DIR "%s",
                 script_engine_get_current_script_id());
        eos_storage_mkdir_if_not_exist(config_file_path);
        snprintf(config_file_path,
                 sizeof(config_file_path),
                 EOS_WATCHFACE_DATA_DIR "%s/config.json",
                 script_engine_get_current_script_id());
    }
    else
    {
        eos_free(json_str);
        return false;
    }

    ret = (eos_storage_write_file(config_file_path, json_str, strlen(json_str)) == EOS_OK);

    eos_free(json_str);
    return ret;
}

static cJSON *sni_api_eos_config_load_from_file(void)
{
    char config_file_path[EOS_FS_PATH_MAX];
    char *data = NULL;
    cJSON *root = NULL;

    if (script_engine_get_current_script_type() == SCRIPT_TYPE_APPLICATION)
    {
        eos_storage_mkdir_if_not_exist(EOS_APP_DATA_DIR);
        snprintf(config_file_path,
                 sizeof(config_file_path),
                 EOS_APP_DATA_DIR "%s/config.json",
                 script_engine_get_current_script_id());
    }
    else if (script_engine_get_current_script_type() == SCRIPT_TYPE_WATCHFACE)
    {
        eos_storage_mkdir_if_not_exist(EOS_WATCHFACE_DATA_DIR);
        snprintf(config_file_path,
                 sizeof(config_file_path),
                 EOS_WATCHFACE_DATA_DIR "%s/config.json",
                 script_engine_get_current_script_id());
    }
    else
    {
        return NULL;
    }

    if (!eos_storage_is_file(config_file_path))
    {
        return cJSON_CreateObject();
    }

    data = eos_storage_read_file(config_file_path);

    if (!data)
    {
        return cJSON_CreateObject();
    }

    root = cJSON_Parse(data);
    eos_free(data);

    if (!root)
    {
        return cJSON_CreateObject();
    }

    return root;
}

static jerry_value_t sni_api_eos_console_write(const jerry_value_t args_p[],
                                               const jerry_length_t args_count,
                                               eos_console_level_t level)
{
    const char *str;

    if (args_count < 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_string(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }

    if (!sni_tb_js2c(args_p[0], SNI_T_STRING, &str))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    switch (level)
    {
        case EOS_CONSOLE_LEVEL_LOG:
            EOS_LOG_I("[%s] %s", CONSOLE_LOG_TAG, str);
            break;
        case EOS_CONSOLE_LEVEL_ERROR:
            EOS_LOG_E("[%s] %s", CONSOLE_LOG_TAG, str);
            break;
        case EOS_CONSOLE_LEVEL_WARN:
            EOS_LOG_W("[%s] %s", CONSOLE_LOG_TAG, str);
            break;
        case EOS_CONSOLE_LEVEL_DEBUG:
            EOS_LOG_D("[%s] %s", CONSOLE_LOG_TAG, str);
            break;
        default:
            eos_free((void *)str);
            return sni_api_throw_error("Invalid console log level");
    }

    eos_free((void *)str);

    return jerry_undefined();
}
/* Function Implementations -----------------------------------*/

jerry_value_t sni_api_eos_view_active(const jerry_call_info_t *call_info_p,
                                      const jerry_value_t args_p[],
                                      const jerry_length_t args_count)
{
    (void)args_p;
    (void)call_info_p;

    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    lv_obj_t *result = eos_view_active();
    if (!result)
    {
        return jerry_null();
    }
    return sni_tb_c2js(&result, SNI_H_EOS_VIEW);
}

jerry_value_t sni_api_eos_config_set_str(const jerry_call_info_t *call_info_p,
                                         const jerry_value_t args_p[],
                                         const jerry_length_t args_count)
{
    char *key = NULL;
    char *value = NULL;
    cJSON *root = NULL;
    cJSON *item = NULL;

    (void)call_info_p;

    if (args_count != 2 || !jerry_value_is_string(args_p[0]) || !jerry_value_is_string(args_p[1]))
    {
        return sni_api_throw_error("Usage: config.setStr(key, value)");
    }

    if (!sni_api_eos_to_c_string(args_p[0], &key) || !sni_api_eos_to_c_string(args_p[1], &value))
    {
        if (key)
        {
            eos_free(key);
        }
        if (value)
        {
            eos_free(value);
        }
        return sni_api_throw_error("Failed to convert argument");
    }

    root = sni_api_eos_config_load_from_file();
    if (!root)
    {
        eos_free(key);
        eos_free(value);
        return sni_api_throw_error("Can't load config");
    }

    item = cJSON_GetObjectItem(root, key);
    if (item)
    {
        cJSON_ReplaceItemInObject(root, key, cJSON_CreateString(value));
    }
    else
    {
        cJSON_AddItemToObject(root, key, cJSON_CreateString(value));
    }

    sni_api_eos_config_write_to_file(root);
    cJSON_Delete(root);
    eos_free(key);
    eos_free(value);
    return jerry_undefined();
}

jerry_value_t sni_api_eos_config_set_bool(const jerry_call_info_t *call_info_p,
                                          const jerry_value_t args_p[],
                                          const jerry_length_t args_count)
{
    char *key = NULL;
    bool value;
    cJSON *root = NULL;
    cJSON *item = NULL;

    (void)call_info_p;

    if (args_count != 2 || !jerry_value_is_string(args_p[0]) || !jerry_value_is_boolean(args_p[1]))
    {
        return sni_api_throw_error("Usage: config.setBool(key, bool)");
    }

    if (!sni_api_eos_to_c_string(args_p[0], &key))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    value = jerry_value_is_true(args_p[1]);
    root = sni_api_eos_config_load_from_file();
    if (!root)
    {
        eos_free(key);
        return sni_api_throw_error("Can't load config");
    }

    item = cJSON_GetObjectItem(root, key);
    if (item)
    {
        cJSON_ReplaceItemInObject(root, key, cJSON_CreateBool(value));
    }
    else
    {
        cJSON_AddItemToObject(root, key, cJSON_CreateBool(value));
    }

    sni_api_eos_config_write_to_file(root);
    cJSON_Delete(root);
    eos_free(key);
    return jerry_undefined();
}

jerry_value_t sni_api_eos_config_set_number(const jerry_call_info_t *call_info_p,
                                            const jerry_value_t args_p[],
                                            const jerry_length_t args_count)
{
    char *key = NULL;
    double value;
    cJSON *root = NULL;
    cJSON *item = NULL;

    (void)call_info_p;

    if (args_count != 2 || !jerry_value_is_string(args_p[0]) || !jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Usage: config.setNumber(key, number)");
    }

    if (!sni_api_eos_to_c_string(args_p[0], &key))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    value = jerry_value_as_number(args_p[1]);
    root = sni_api_eos_config_load_from_file();
    if (!root)
    {
        eos_free(key);
        return sni_api_throw_error("Can't load config");
    }

    item = cJSON_GetObjectItem(root, key);
    if (item)
    {
        cJSON_ReplaceItemInObject(root, key, cJSON_CreateNumber(value));
    }
    else
    {
        cJSON_AddItemToObject(root, key, cJSON_CreateNumber(value));
    }

    sni_api_eos_config_write_to_file(root);
    cJSON_Delete(root);
    eos_free(key);
    return jerry_undefined();
}

jerry_value_t sni_api_eos_config_get_str(const jerry_call_info_t *call_info_p,
                                         const jerry_value_t args_p[],
                                         const jerry_length_t args_count)
{
    char *key = NULL;
    cJSON *root = NULL;
    cJSON *item = NULL;
    jerry_value_t ret = jerry_undefined();

    (void)call_info_p;

    if (args_count != 1 || !jerry_value_is_string(args_p[0]))
    {
        return sni_api_throw_error("Usage: config.getStr(key)");
    }

    if (!sni_api_eos_to_c_string(args_p[0], &key))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    root = sni_api_eos_config_load_from_file();
    if (!root)
    {
        eos_free(key);
        return sni_api_throw_error("Can't load config");
    }

    item = cJSON_GetObjectItem(root, key);
    if (item && cJSON_IsString(item))
    {
        ret = jerry_string_sz(item->valuestring);
    }

    cJSON_Delete(root);
    eos_free(key);
    return ret;
}

jerry_value_t sni_api_eos_config_get_bool(const jerry_call_info_t *call_info_p,
                                          const jerry_value_t args_p[],
                                          const jerry_length_t args_count)
{
    char *key = NULL;
    cJSON *root = NULL;
    cJSON *item = NULL;
    jerry_value_t ret = jerry_boolean(false);

    (void)call_info_p;

    if (args_count != 1 || !jerry_value_is_string(args_p[0]))
    {
        return sni_api_throw_error("Usage: config.getBool(key)");
    }

    if (!sni_api_eos_to_c_string(args_p[0], &key))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    root = sni_api_eos_config_load_from_file();
    if (!root)
    {
        eos_free(key);
        return sni_api_throw_error("Can't load config");
    }

    item = cJSON_GetObjectItem(root, key);
    if (item && cJSON_IsBool(item))
    {
        ret = jerry_boolean(item->valueint != 0);
    }

    cJSON_Delete(root);
    eos_free(key);
    return ret;
}

jerry_value_t sni_api_eos_config_get_number(const jerry_call_info_t *call_info_p,
                                            const jerry_value_t args_p[],
                                            const jerry_length_t args_count)
{
    char *key = NULL;
    cJSON *root = NULL;
    cJSON *item = NULL;
    jerry_value_t ret = jerry_number(0);

    (void)call_info_p;

    if (args_count != 1 || !jerry_value_is_string(args_p[0]))
    {
        return sni_api_throw_error("Usage: config.getNumber(key)");
    }

    if (!sni_api_eos_to_c_string(args_p[0], &key))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    root = sni_api_eos_config_load_from_file();
    if (!root)
    {
        eos_free(key);
        return sni_api_throw_error("Can't load config");
    }

    item = cJSON_GetObjectItem(root, key);
    if (item && cJSON_IsNumber(item))
    {
        ret = jerry_number(item->valuedouble);
    }

    cJSON_Delete(root);
    eos_free(key);
    return ret;
}

jerry_value_t sni_api_eos_time_get_now(const jerry_call_info_t *call_info_p,
                                       const jerry_value_t args_p[],
                                       const jerry_length_t args_count)
{
    eos_datetime_t dt;
    jerry_value_t obj;

    (void)call_info_p;
    (void)args_p;

    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    dt = eos_time_get();
    obj = jerry_object();
    script_engine_set_prop_number(obj, "year", dt.year);
    script_engine_set_prop_number(obj, "month", dt.month);
    script_engine_set_prop_number(obj, "day", dt.day);
    script_engine_set_prop_number(obj, "hour", dt.hour);
    script_engine_set_prop_number(obj, "min", dt.min);
    script_engine_set_prop_number(obj, "sec", dt.sec);
    script_engine_set_prop_number(obj, "ms", dt.ms);
    script_engine_set_prop_number(obj, "day_of_week", dt.day_of_week);

    return obj;
}

jerry_value_t sni_api_eos_app_header_set_title(const jerry_call_info_t *call_info_p,
                                               const jerry_value_t args_p[],
                                               const jerry_length_t args_count)
{
    eos_activity_t *current;
    char *title = NULL;
    lv_obj_t *view;

    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Usage: appHeader.setTitle(view, title)");
    }

    if (!jerry_value_is_null(args_p[0]) && !jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid view argument type");
    }

    if (!jerry_value_is_null(args_p[0]))
    {
        if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &view))
        {
            return sni_api_throw_error("Invalid view argument");
        }
    }

    if (!jerry_value_is_null(args_p[1]) && !jerry_value_is_undefined(args_p[1]))
    {
        if (!sni_api_eos_to_c_string(args_p[1], &title))
        {
            return sni_api_throw_error("Invalid title argument");
        }
    }

    current = eos_activity_get_current();
    if (!current)
    {
        if (title)
        {
            eos_free(title);
        }
        return sni_api_throw_error("No current activity");
    }

    eos_activity_set_title(current, title);
    if (title)
    {
        eos_free(title);
    }

    return jerry_undefined();
}

jerry_value_t sni_api_eos_app_header_hide(const jerry_call_info_t *call_info_p,
                                          const jerry_value_t args_p[],
                                          const jerry_length_t args_count)
{
    (void)call_info_p;
    (void)args_p;

    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    eos_app_header_hide();
    return jerry_undefined();
}

jerry_value_t sni_api_eos_app_header_show(const jerry_call_info_t *call_info_p,
                                          const jerry_value_t args_p[],
                                          const jerry_length_t args_count)
{
    eos_activity_t *current;

    (void)call_info_p;
    (void)args_p;

    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    current = eos_activity_get_current();
    if (!current)
    {
        return sni_api_throw_error("No current activity");
    }

    eos_app_header_show(current);
    return jerry_undefined();
}

jerry_value_t sni_api_eos_activity_create(const jerry_call_info_t *call_info_p,
                                          const jerry_value_t args_p[],
                                          const jerry_length_t args_count)
{
    eos_activity_t *activity;

    (void)call_info_p;
    (void)args_p;

    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    activity = eos_activity_create(NULL);
    if (!activity)
    {
        return sni_api_throw_error("Failed to create activity");
    }
    return sni_tb_c2js(&activity, SNI_H_EOS_ACTIVITY);
}

jerry_value_t sni_api_eos_activity_destroy(const jerry_call_info_t *call_info_p,
                                           const jerry_value_t args_p[],
                                           const jerry_length_t args_count)
{
    eos_activity_t *activity;

    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Usage: activity.destroy(activity)");
    }

    if (!sni_tb_js2c(args_p[0], SNI_H_EOS_ACTIVITY, &activity))
    {
        return sni_api_throw_error("Invalid activity argument");
    }

    /* AppRoot destruction is an App lifecycle operation, not a generic
     * Activity operation.  Calling it from the program's own JS callback
     * would enter on_destroy while SPM is dispatching the callback; the
     * resulting stop request cannot complete synchronously and would leave a
     * live program without its root Activity.  Use activity.back() for normal
     * navigation, or the host App lifecycle API for close/terminate. */
    if (eos_activity_get_app_id(activity) && eos_activity_get_app_root(activity) == activity)
    {
        return sni_api_throw_error("Cannot destroy AppRoot directly; use activity.back()");
    }

    /* Remove both managed-resource entries before native teardown. The
     * Activity's lifecycle callback may synchronously stop the SPM program;
     * leaving either node linked until after eos_activity_destroy() lets the
     * nested SNI sweep see a pointer that is already being freed. */
    lv_obj_t *view = eos_activity_get_view(activity);
    sni_context_t *ctx = sni_get_current_context();
    if (ctx)
    {
        if (view)
        {
            sni_context_remove_resource(ctx, view, SNI_H_EOS_VIEW);
        }
        sni_context_remove_resource(ctx, activity, SNI_H_EOS_ACTIVITY);
    }

    eos_activity_destroy(activity);

    return jerry_undefined();
}

jerry_value_t sni_api_eos_activity_current(const jerry_call_info_t *call_info_p,
                                           const jerry_value_t args_p[],
                                           const jerry_length_t args_count)
{
    eos_activity_t *activity;

    (void)call_info_p;
    (void)args_p;

    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    activity = eos_activity_get_current();
    return sni_tb_c2js(&activity, SNI_H_EOS_ACTIVITY);
}

jerry_value_t sni_api_eos_activity_visible(const jerry_call_info_t *call_info_p,
                                           const jerry_value_t args_p[],
                                           const jerry_length_t args_count)
{
    eos_activity_t *activity;

    (void)call_info_p;
    (void)args_p;

    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    activity = eos_activity_get_visible();
    return sni_tb_c2js(&activity, SNI_H_EOS_ACTIVITY);
}

jerry_value_t sni_api_eos_activity_bottom(const jerry_call_info_t *call_info_p,
                                          const jerry_value_t args_p[],
                                          const jerry_length_t args_count)
{
    eos_activity_t *activity;

    (void)call_info_p;
    (void)args_p;

    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    activity = eos_activity_get_bottom();
    return sni_tb_c2js(&activity, SNI_H_EOS_ACTIVITY);
}

jerry_value_t sni_api_eos_activity_watchface(const jerry_call_info_t *call_info_p,
                                             const jerry_value_t args_p[],
                                             const jerry_length_t args_count)
{
    eos_activity_t *activity;

    (void)call_info_p;
    (void)args_p;

    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    activity = eos_activity_get_watchface();
    return sni_tb_c2js(&activity, SNI_H_EOS_ACTIVITY);
}

jerry_value_t sni_api_eos_activity_get_view(const jerry_call_info_t *call_info_p,
                                            const jerry_value_t args_p[],
                                            const jerry_length_t args_count)
{
    eos_activity_t *activity;
    lv_obj_t *view;

    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Usage: activity.getView(activity)");
    }

    if (!sni_tb_js2c(args_p[0], SNI_H_EOS_ACTIVITY, &activity))
    {
        return sni_api_throw_error("Invalid activity argument");
    }

    view = eos_activity_get_view(activity);
    return sni_tb_c2js(&view, SNI_H_EOS_VIEW);
}

jerry_value_t sni_api_eos_activity_set_view(const jerry_call_info_t *call_info_p,
                                            const jerry_value_t args_p[],
                                            const jerry_length_t args_count)
{
    eos_activity_t *activity;
    lv_obj_t *view;

    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Usage: activity.setView(activity, view)");
    }

    if (!sni_tb_js2c(args_p[0], SNI_H_EOS_ACTIVITY, &activity) || !sni_tb_js2c_parent(args_p[1], (void **)&view))
    {
        return sni_api_throw_error("Invalid argument type");
    }

    /* Capture the old view so we can clean up its EOS_VIEW resource
     * after eos_activity_set_view deletes it via lv_obj_delete. */
    lv_obj_t *old_view = eos_activity_get_view(activity);

    if (old_view && old_view != view)
    {
        sni_context_t *ctx = sni_get_current_context();
        if (ctx)
        {
            sni_context_remove_resource(ctx, old_view, SNI_H_EOS_VIEW);
        }
    }
    eos_activity_set_view(activity, view);
    return jerry_undefined();
}

jerry_value_t sni_api_eos_activity_get_title(const jerry_call_info_t *call_info_p,
                                             const jerry_value_t args_p[],
                                             const jerry_length_t args_count)
{
    eos_activity_t *activity;
    const char *title;

    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Usage: activity.getTitle(activity)");
    }

    if (!sni_tb_js2c(args_p[0], SNI_H_EOS_ACTIVITY, &activity))
    {
        return sni_api_throw_error("Invalid activity argument");
    }

    title = eos_activity_get_title(activity);
    if (!title)
    {
        return jerry_undefined();
    }

    return jerry_string_sz(title);
}

jerry_value_t sni_api_eos_activity_set_title(const jerry_call_info_t *call_info_p,
                                             const jerry_value_t args_p[],
                                             const jerry_length_t args_count)
{
    eos_activity_t *activity;
    char *title = NULL;

    (void)call_info_p;

    if (args_count != 2 || !jerry_value_is_string(args_p[1]))
    {
        return sni_api_throw_error("Usage: activity.setTitle(activity, title)");
    }

    if (!sni_tb_js2c(args_p[0], SNI_H_EOS_ACTIVITY, &activity) || !sni_api_eos_to_c_string(args_p[1], &title))
    {
        return sni_api_throw_error("Invalid argument type");
    }

    eos_activity_set_title(activity, title);
    eos_free(title);
    return jerry_undefined();
}

jerry_value_t sni_api_eos_activity_set_type(const jerry_call_info_t *call_info_p,
                                            const jerry_value_t args_p[],
                                            const jerry_length_t args_count)
{
    eos_activity_t *activity;
    int32_t type;

    (void)call_info_p;

    if (args_count != 2 || !jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Usage: activity.setType(activity, type)");
    }

    if (!sni_tb_js2c(args_p[0], SNI_H_EOS_ACTIVITY, &activity))
    {
        return sni_api_throw_error("Invalid activity argument");
    }

    type = (int32_t)jerry_value_as_number(args_p[1]);
    eos_activity_set_type(activity, (eos_activity_type_t)type);
    return jerry_undefined();
}

jerry_value_t sni_api_eos_activity_get_type(const jerry_call_info_t *call_info_p,
                                            const jerry_value_t args_p[],
                                            const jerry_length_t args_count)
{
    eos_activity_t *activity;
    eos_activity_type_t type;

    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Usage: activity.getType(activity)");
    }

    if (!sni_tb_js2c(args_p[0], SNI_H_EOS_ACTIVITY, &activity))
    {
        return sni_api_throw_error("Invalid activity argument");
    }

    type = eos_activity_get_type(activity);
    return jerry_number((double)type);
}

jerry_value_t sni_api_eos_activity_set_app_header_visible(const jerry_call_info_t *call_info_p,
                                                          const jerry_value_t args_p[],
                                                          const jerry_length_t args_count)
{
    eos_activity_t *activity;
    bool visible;

    (void)call_info_p;

    if (args_count != 2 || !jerry_value_is_boolean(args_p[1]))
    {
        return sni_api_throw_error("Usage: activity.setAppHeaderVisible(activity, visible)");
    }

    if (!sni_tb_js2c(args_p[0], SNI_H_EOS_ACTIVITY, &activity))
    {
        return sni_api_throw_error("Invalid activity argument");
    }

    visible = jerry_value_is_true(args_p[1]);
    eos_activity_set_app_header_visible(activity, visible);
    return jerry_undefined();
}

jerry_value_t sni_api_eos_activity_is_app_header_visible(const jerry_call_info_t *call_info_p,
                                                         const jerry_value_t args_p[],
                                                         const jerry_length_t args_count)
{
    eos_activity_t *activity;
    bool visible;

    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Usage: activity.isAppHeaderVisible(activity)");
    }

    if (!sni_tb_js2c(args_p[0], SNI_H_EOS_ACTIVITY, &activity))
    {
        return sni_api_throw_error("Invalid activity argument");
    }

    visible = eos_activity_is_app_header_visible(activity);
    return jerry_boolean(visible);
}

jerry_value_t sni_api_eos_activity_enter(const jerry_call_info_t *call_info_p,
                                         const jerry_value_t args_p[],
                                         const jerry_length_t args_count)
{
    eos_activity_t *activity;

    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Usage: activity.enter(activity)");
    }

    if (!sni_tb_js2c(args_p[0], SNI_H_EOS_ACTIVITY, &activity))
    {
        return sni_api_throw_error("Invalid activity argument");
    }

    eos_activity_enter(activity);
    return jerry_undefined();
}

jerry_value_t sni_api_eos_activity_back(const jerry_call_info_t *call_info_p,
                                        const jerry_value_t args_p[],
                                        const jerry_length_t args_count)
{
    eos_result_t ret;

    (void)call_info_p;
    (void)args_p;

    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    ret = eos_activity_back();
    return jerry_boolean(ret == EOS_OK);
}

jerry_value_t sni_api_eos_activity_root_screen(const jerry_call_info_t *call_info_p,
                                               const jerry_value_t args_p[],
                                               const jerry_length_t args_count)
{
    (void)call_info_p;
    (void)args_p;
    (void)args_count;

    return sni_api_throw_error("eos.activity.rootScreen() is not available from JS scripts");
}

jerry_value_t sni_api_eos_activity_is_transition_in_progress(const jerry_call_info_t *call_info_p,
                                                             const jerry_value_t args_p[],
                                                             const jerry_length_t args_count)
{
    (void)call_info_p;
    (void)args_p;

    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    return jerry_boolean(eos_activity_is_transition_in_progress());
}

jerry_value_t sni_api_eos_console_log(const jerry_call_info_t *call_info_p,
                                      const jerry_value_t args_p[],
                                      const jerry_length_t args_count)
{
    (void)call_info_p;

    return sni_api_eos_console_write(args_p, args_count, EOS_CONSOLE_LEVEL_LOG);
}

jerry_value_t sni_api_eos_console_error(const jerry_call_info_t *call_info_p,
                                        const jerry_value_t args_p[],
                                        const jerry_length_t args_count)
{
    (void)call_info_p;

    return sni_api_eos_console_write(args_p, args_count, EOS_CONSOLE_LEVEL_ERROR);
}

jerry_value_t sni_api_eos_console_warn(const jerry_call_info_t *call_info_p,
                                       const jerry_value_t args_p[],
                                       const jerry_length_t args_count)
{
    (void)call_info_p;

    return sni_api_eos_console_write(args_p, args_count, EOS_CONSOLE_LEVEL_WARN);
}

jerry_value_t sni_api_eos_console_debug(const jerry_call_info_t *call_info_p,
                                        const jerry_value_t args_p[],
                                        const jerry_length_t args_count)
{
    (void)call_info_p;

    return sni_api_eos_console_write(args_p, args_count, EOS_CONSOLE_LEVEL_DEBUG);
}

/**
 * @brief Expose one canonical Sensor sample to JavaScript
 *
 * The native service stores fixed-point values documented in
 * eos_dev_sensor.h.  This binding performs only the fixed, platform-neutral
 * language conversion below:
 *
 *   ACCE/GYRO/TEMP/BARO.temperature/HUMIDITY: milli-unit / 1000
 *   MAG: nT / 1000 -> uT
 *   BARO.pressure, LIGHT, PROXIMITY, HR, SPO2, STEP: unchanged
 *   PPG: red/ir ADC counts unchanged, unit="adc"
 *   ECG/CAP: source counts unchanged; no physical conversion is defined
 *
 * A missing latest sample is returned as JavaScript null, preserving the
 * existing binding convention.  A real zero-valued sample is still an object.
 */
jerry_value_t sni_api_eos_sensor_read_latest(const jerry_call_info_t *call_info_p,
                                             const jerry_value_t args_p[],
                                             const jerry_length_t args_count)
{
    eos_sensor_type_t type;
    eos_sensor_raw_data_t raw;
    eos_result_t result;
    jerry_value_t obj;

    (void)call_info_p;

    if (args_count != 1 || !_sni_api_eos_number_to_sensor_type(args_p[0], &type))
    {
        return sni_api_throw_error("Usage: sensor.readLatest(type)");
    }

    result = eos_sensor_read_latest(type, &raw);
    if (result != EOS_OK)
    {
        return jerry_null();
    }

    obj = jerry_object();
    script_engine_set_prop_number(obj, "timestamp", (double)raw.timestamp);

    switch ((eos_sensor_type_t)type)
    {
        case EOS_SENSOR_TYPE_ACCE:
            script_engine_set_prop_number(obj, "x", (double)raw.data.acce.x / 1000.0);
            script_engine_set_prop_number(obj, "y", (double)raw.data.acce.y / 1000.0);
            script_engine_set_prop_number(obj, "z", (double)raw.data.acce.z / 1000.0);
            script_engine_set_prop_string(obj, "unit", "m/s2");
            break;
        case EOS_SENSOR_TYPE_GYRO:
            script_engine_set_prop_number(obj, "x", (double)raw.data.gyro.x / 1000.0);
            script_engine_set_prop_number(obj, "y", (double)raw.data.gyro.y / 1000.0);
            script_engine_set_prop_number(obj, "z", (double)raw.data.gyro.z / 1000.0);
            script_engine_set_prop_string(obj, "unit", "deg/s");
            break;
        case EOS_SENSOR_TYPE_MAG:
            script_engine_set_prop_number(obj, "x", (double)raw.data.mag.x / 1000.0);
            script_engine_set_prop_number(obj, "y", (double)raw.data.mag.y / 1000.0);
            script_engine_set_prop_number(obj, "z", (double)raw.data.mag.z / 1000.0);
            script_engine_set_prop_string(obj, "unit", "uT");
            break;
        case EOS_SENSOR_TYPE_HR:
            script_engine_set_prop_number(obj, "heart_rate", (double)raw.data.hr.heart_rate);
            break;
        case EOS_SENSOR_TYPE_SPO2:
            script_engine_set_prop_number(obj, "spo2", (double)raw.data.spo2.spo2);
            break;
        case EOS_SENSOR_TYPE_LIGHT:
            script_engine_set_prop_number(obj, "lux", (double)raw.data.light.lux);
            break;
        case EOS_SENSOR_TYPE_TEMP:
            script_engine_set_prop_number(obj, "temp", (double)raw.data.temp.temp / 1000.0);
            script_engine_set_prop_string(obj, "unit", "degC");
            break;
        case EOS_SENSOR_TYPE_BARO:
            script_engine_set_prop_number(obj, "pressure", (double)raw.data.baro.pressure);
            script_engine_set_prop_number(obj, "temperature", (double)raw.data.baro.temperature / 1000.0);
            script_engine_set_prop_string(obj, "unit", "Pa");
            break;
        case EOS_SENSOR_TYPE_HUMIDITY:
            script_engine_set_prop_number(obj, "humidity", (double)raw.data.humidity.humidity / 1000.0);
            script_engine_set_prop_string(obj, "unit", "%RH");
            break;
        case EOS_SENSOR_TYPE_STEP:
            script_engine_set_prop_number(obj, "steps", (double)raw.data.step.steps);
            break;
        case EOS_SENSOR_TYPE_PROXIMITY:
            script_engine_set_prop_number(obj, "distance_mm", (double)raw.data.proximity.distance_mm);
            break;
        case EOS_SENSOR_TYPE_ECG:
            script_engine_set_prop_number(obj, "ecg", (double)raw.data.ecg.ecg);
            break;
        case EOS_SENSOR_TYPE_CAP:
            script_engine_set_prop_number(obj, "cap", (double)raw.data.cap.cap);
            break;
        case EOS_SENSOR_TYPE_PPG:
            script_engine_set_prop_number(obj, "red", (double)raw.data.ppg.red);
            script_engine_set_prop_number(obj, "ir", (double)raw.data.ppg.ir);
            script_engine_set_prop_string(obj, "unit", "adc");
            break;
        default:
            jerry_value_free(obj);
            return jerry_null();
    }

    return obj;
}

jerry_value_t sni_api_eos_sensor_set_sample_period(const jerry_call_info_t *call_info_p,
                                                   const jerry_value_t args_p[],
                                                   const jerry_length_t args_count)
{
    eos_sensor_type_t type;
    uint32_t period_ms;
    eos_result_t result;

    (void)call_info_p;

    if (args_count != 2 || !_sni_api_eos_number_to_sensor_type(args_p[0], &type)
        || !_sni_api_eos_number_to_u32(args_p[1], &period_ms))
    {
        return sni_api_throw_error("Usage: sensor.setSamplePeriod(type, period_ms)");
    }

    result = eos_sensor_set_sample_period(type, period_ms);

    return jerry_boolean(result == EOS_OK);
}

jerry_value_t sni_api_eos_sensor_get_sample_period(const jerry_call_info_t *call_info_p,
                                                   const jerry_value_t args_p[],
                                                   const jerry_length_t args_count)
{
    eos_sensor_type_t type;
    uint32_t period_ms;

    (void)call_info_p;

    if (args_count != 1 || !_sni_api_eos_number_to_sensor_type(args_p[0], &type))
    {
        return sni_api_throw_error("Usage: sensor.getSamplePeriod(type)");
    }

    period_ms = eos_sensor_get_sample_period(type);
    return jerry_number((double)period_ms);
}

static jerry_value_t _sni_api_eos_sensor_request_start(const jerry_value_t args_p[],
                                                       jerry_length_t args_count,
                                                       uint8_t kind)
{
    sni_context_t *context;
    void *callback_context = NULL;
    uint32_t timeout_ms = 0U;
    eos_sensor_request_id_t request_id = EOS_SENSOR_REQUEST_INVALID;
    eos_result_t result;

    if ((args_count != 1U && args_count != 2U) || !jerry_value_is_function(args_p[0]))
        return sni_api_throw_error("Usage: sensor operation(callback, timeout_ms?)");

    if (args_count == 2U)
    {
        if (!_sni_api_eos_number_to_u32(args_p[1], &timeout_ms))
            return sni_api_throw_error("Invalid timeout");
    }

    context = sni_cb_get_context();
    if (!context)
        return sni_api_throw_error("No current script context");
    if (!sni_cb_sensor_request_register(context, EOS_SENSOR_REQUEST_INVALID, kind, args_p[0], &callback_context))
    {
        return sni_api_throw_error("Out of memory");
    }

    if (kind == 0U)
    {
        result =
            eos_sensor_heart_rate_start(sni_cb_sensor_heart_rate_dispatch, callback_context, timeout_ms, &request_id);
    }
    else
    {
        result = eos_sensor_compass_start(sni_cb_sensor_compass_dispatch, callback_context, timeout_ms, &request_id);
    }
    if (result != EOS_OK)
    {
        sni_cb_sensor_request_release(callback_context);
        return jerry_null();
    }

    sni_cb_sensor_request_bind(callback_context, request_id);
    return jerry_number((double)request_id);
}

jerry_value_t sni_api_eos_sensor_heart_rate(const jerry_call_info_t *call_info_p,
                                            const jerry_value_t args_p[],
                                            const jerry_length_t args_count)
{
    (void)call_info_p;
    return _sni_api_eos_sensor_request_start(args_p, args_count, 0U);
}

jerry_value_t sni_api_eos_sensor_compass(const jerry_call_info_t *call_info_p,
                                         const jerry_value_t args_p[],
                                         const jerry_length_t args_count)
{
    (void)call_info_p;
    return _sni_api_eos_sensor_request_start(args_p, args_count, 1U);
}

jerry_value_t sni_api_eos_sensor_cancel(const jerry_call_info_t *call_info_p,
                                        const jerry_value_t args_p[],
                                        const jerry_length_t args_count)
{
    sni_context_t *context;
    uint32_t request_number;

    (void)call_info_p;
    if (args_count != 1U || !jerry_value_is_number(args_p[0]))
        return sni_api_throw_error("Usage: sensor.cancel(request_id)");
    if (!_sni_api_eos_number_to_u32(args_p[0], &request_number) || request_number == 0U)
        return sni_api_throw_error("Invalid request id");
    context = sni_cb_get_context();
    return jerry_boolean(context && sni_cb_sensor_request_cancel(context, (eos_sensor_request_id_t)request_number));
}

const sni_method_desc_t eos_class_static_methods_view[] = {
    {.name = "active", .handler = sni_api_eos_view_active},
    {.name = NULL, .handler = NULL},
};

const sni_method_desc_t eos_class_static_methods_console[] = {
    {.name = "log", .handler = sni_api_eos_console_log},
    {.name = "error", .handler = sni_api_eos_console_error},
    {.name = "warn", .handler = sni_api_eos_console_warn},
    {.name = "info", .handler = sni_api_eos_console_log},
    {.name = "debug", .handler = sni_api_eos_console_debug},
    {.name = NULL, .handler = NULL},
};

const sni_method_desc_t eos_class_static_methods_config[] = {
    {.name = "setStr", .handler = sni_api_eos_config_set_str},
    {.name = "setBool", .handler = sni_api_eos_config_set_bool},
    {.name = "setNumber", .handler = sni_api_eos_config_set_number},
    {.name = "getStr", .handler = sni_api_eos_config_get_str},
    {.name = "getBool", .handler = sni_api_eos_config_get_bool},
    {.name = "getNumber", .handler = sni_api_eos_config_get_number},
    {.name = NULL, .handler = NULL},
};

const sni_method_desc_t eos_class_static_methods_time[] = {
    {.name = "getNow", .handler = sni_api_eos_time_get_now},
    {.name = NULL, .handler = NULL},
};

const sni_method_desc_t eos_class_static_methods_app_header[] = {
    {.name = "setTitle", .handler = sni_api_eos_app_header_set_title},
    {.name = "hide", .handler = sni_api_eos_app_header_hide},
    {.name = "show", .handler = sni_api_eos_app_header_show},
    {.name = NULL, .handler = NULL},
};

const sni_method_desc_t eos_class_static_methods_activity[] = {
    {.name = "create", .handler = sni_api_eos_activity_create},
    {.name = "destroy", .handler = sni_api_eos_activity_destroy},
    {.name = "current", .handler = sni_api_eos_activity_current},
    {.name = "visible", .handler = sni_api_eos_activity_visible},
    {.name = "bottom", .handler = sni_api_eos_activity_bottom},
    {.name = "watchface", .handler = sni_api_eos_activity_watchface},
    {.name = "rootScreen", .handler = sni_api_eos_activity_root_screen},
    {.name = "getView", .handler = sni_api_eos_activity_get_view},
    {.name = "setView", .handler = sni_api_eos_activity_set_view},
    {.name = "getTitle", .handler = sni_api_eos_activity_get_title},
    {.name = "setTitle", .handler = sni_api_eos_activity_set_title},
    {.name = "getType", .handler = sni_api_eos_activity_get_type},
    {.name = "setType", .handler = sni_api_eos_activity_set_type},
    {.name = "setAppHeaderVisible", .handler = sni_api_eos_activity_set_app_header_visible},
    {.name = "isAppHeaderVisible", .handler = sni_api_eos_activity_is_app_header_visible},
    {.name = "enter", .handler = sni_api_eos_activity_enter},
    {.name = "back", .handler = sni_api_eos_activity_back},
    {.name = "isTransitionInProgress", .handler = sni_api_eos_activity_is_transition_in_progress},
    {.name = NULL, .handler = NULL},
};

const sni_class_desc_t eos_class_desc_view = {
    .name = "view",
    .constructor = NULL,
    .base_class = NULL,
    .methods = NULL,
    .properties = NULL,
    .static_methods = eos_class_static_methods_view,
    .constants = NULL,
};

const sni_class_desc_t eos_class_desc_console = {
    .name = "console",
    .constructor = NULL,
    .base_class = NULL,
    .methods = NULL,
    .properties = NULL,
    .static_methods = eos_class_static_methods_console,
    .constants = NULL,
};

const sni_class_desc_t eos_class_desc_config = {
    .name = "config",
    .constructor = NULL,
    .base_class = NULL,
    .methods = NULL,
    .properties = NULL,
    .static_methods = eos_class_static_methods_config,
    .constants = NULL,
};

const sni_class_desc_t eos_class_desc_time = {
    .name = "time",
    .constructor = NULL,
    .base_class = NULL,
    .methods = NULL,
    .properties = NULL,
    .static_methods = eos_class_static_methods_time,
    .constants = NULL,
};

const sni_class_desc_t eos_class_desc_app_header = {
    .name = "appHeader",
    .constructor = NULL,
    .base_class = NULL,
    .methods = NULL,
    .properties = NULL,
    .static_methods = eos_class_static_methods_app_header,
    .constants = NULL,
};

const sni_class_desc_t eos_class_desc_ww = {
    .name = "ww",
    .constructor = NULL,
    .base_class = NULL,
    .methods = NULL,
    .properties = NULL,
    .static_methods = eos_ww_static_methods,
    .constants = NULL,
};

const sni_class_desc_t eos_class_desc_activity = {
    .name = "activity",
    .constructor = NULL,
    .base_class = NULL,
    .methods = NULL,
    .properties = NULL,
    .static_methods = eos_class_static_methods_activity,
    .constants = NULL,
};

const sni_method_desc_t eos_class_static_methods_permission[] = {
    {.name = "request", .handler = sni_api_eos_permission_request},
    {.name = "check", .handler = sni_api_eos_permission_check},
    {.name = NULL, .handler = NULL},
};

const sni_method_desc_t eos_class_static_methods_sensor[] = {
    {.name = "readLatest", .handler = sni_api_eos_sensor_read_latest},
    {.name = "setSamplePeriod", .handler = sni_api_eos_sensor_set_sample_period},
    {.name = "getSamplePeriod", .handler = sni_api_eos_sensor_get_sample_period},
    {.name = "heartRate", .handler = sni_api_eos_sensor_heart_rate},
    {.name = "compass", .handler = sni_api_eos_sensor_compass},
    {.name = "cancel", .handler = sni_api_eos_sensor_cancel},
    {.name = NULL, .handler = NULL},
};

const sni_class_desc_t eos_class_desc_permission = {
    .name = "permission",
    .constructor = NULL,
    .base_class = NULL,
    .methods = NULL,
    .properties = NULL,
    .static_methods = eos_class_static_methods_permission,
    .constants = NULL,
};

const sni_class_desc_t eos_class_desc_sensor = {
    .name = "sensor",
    .constructor = NULL,
    .base_class = NULL,
    .methods = NULL,
    .properties = NULL,
    .static_methods = eos_class_static_methods_sensor,
    .constants = NULL,
};

const sni_class_desc_t eos_class_desc_metric = {
    .name = "metric",
    .constructor = NULL,
    .base_class = NULL,
    .methods = NULL,
    .properties = NULL,
    .static_methods = eos_metric_static_methods,
    .constants = NULL,
};

const sni_class_desc_t *const eos_api_classes[] = {
    &eos_class_desc_view,
    &eos_class_desc_console,
    &eos_class_desc_config,
    &eos_class_desc_time,
    &eos_class_desc_app_header,
    &eos_class_desc_ww,
    &eos_class_desc_activity,
    &eos_class_desc_permission,
    &eos_class_desc_sensor,
    &eos_class_desc_metric,
    NULL,
};

const sni_constant_desc_t eos_root_constants[] = {
    {.name = "FONT_SIZE_LARGE", .type = SNI_CONST_INT, .value.i = EOS_FONT_SIZE_LARGE},
    {.name = "FONT_SIZE_MEDIUM", .type = SNI_CONST_INT, .value.i = EOS_FONT_SIZE_MEDIUM},
    {.name = "FONT_SIZE_SMALL", .type = SNI_CONST_INT, .value.i = EOS_FONT_SIZE_SMALL},
    {.name = "DISPLAY_WIDTH", .type = SNI_CONST_INT, .value.i = EOS_DISPLAY_WIDTH},
    {.name = "DISPLAY_HEIGHT", .type = SNI_CONST_INT, .value.i = EOS_DISPLAY_HEIGHT},
    {.name = "ACTIVITY_TYPE_NULL", .type = SNI_CONST_INT, .value.i = EOS_ACTIVITY_TYPE_NULL},
    {.name = "ACTIVITY_TYPE_APP", .type = SNI_CONST_INT, .value.i = EOS_ACTIVITY_TYPE_APP},
    {.name = "ACTIVITY_TYPE_APP_LIST", .type = SNI_CONST_INT, .value.i = EOS_ACTIVITY_TYPE_APP_LIST},
    {.name = "ACTIVITY_TYPE_WATCHFACE", .type = SNI_CONST_INT, .value.i = EOS_ACTIVITY_TYPE_WATCHFACE},
    {.name = "ACTIVITY_TYPE_WATCHFACE_LIST", .type = SNI_CONST_INT, .value.i = EOS_ACTIVITY_TYPE_WATCHFACE_LIST},
    {.name = "SENSOR_ACCE", .type = SNI_CONST_INT, .value.i = EOS_SENSOR_TYPE_ACCE},
    {.name = "SENSOR_GYRO", .type = SNI_CONST_INT, .value.i = EOS_SENSOR_TYPE_GYRO},
    {.name = "SENSOR_MAG", .type = SNI_CONST_INT, .value.i = EOS_SENSOR_TYPE_MAG},
    {.name = "SENSOR_HR", .type = SNI_CONST_INT, .value.i = EOS_SENSOR_TYPE_HR},
    {.name = "SENSOR_SPO2", .type = SNI_CONST_INT, .value.i = EOS_SENSOR_TYPE_SPO2},
    {.name = "SENSOR_LIGHT", .type = SNI_CONST_INT, .value.i = EOS_SENSOR_TYPE_LIGHT},
    {.name = "SENSOR_TEMP", .type = SNI_CONST_INT, .value.i = EOS_SENSOR_TYPE_TEMP},
    {.name = "SENSOR_BARO", .type = SNI_CONST_INT, .value.i = EOS_SENSOR_TYPE_BARO},
    {.name = "SENSOR_HUMIDITY", .type = SNI_CONST_INT, .value.i = EOS_SENSOR_TYPE_HUMIDITY},
    {.name = "SENSOR_STEP", .type = SNI_CONST_INT, .value.i = EOS_SENSOR_TYPE_STEP},
    {.name = "SENSOR_PROXIMITY", .type = SNI_CONST_INT, .value.i = EOS_SENSOR_TYPE_PROXIMITY},
    {.name = "SENSOR_ECG", .type = SNI_CONST_INT, .value.i = EOS_SENSOR_TYPE_ECG},
    {.name = "SENSOR_CAP", .type = SNI_CONST_INT, .value.i = EOS_SENSOR_TYPE_CAP},
    {.name = "SENSOR_PPG", .type = SNI_CONST_INT, .value.i = EOS_SENSOR_TYPE_PPG},
    {.name = NULL, .type = SNI_CONST_INT, .value.i = 0},
};

void sni_api_eos_init(void)
{
    eos_api_obj = sni_api_build(eos_api_classes);
    if (!jerry_value_is_object(eos_api_obj))
    {
        EOS_LOG_E("Failed to build ElenixOS API object");
        return;
    }
    if (!sni_api_register_constants(eos_root_constants, eos_api_obj))
    {
        EOS_LOG_E("Failed to register ElenixOS API constants");
    }
}

void sni_api_eos_mount(jerry_value_t realm)
{
    bool result = sni_api_mount(realm, eos_api_obj, EOS_API_NAME);
    if (!result)
    {
        EOS_LOG_E("Failed to mount ElenixOS API");
    }
}
