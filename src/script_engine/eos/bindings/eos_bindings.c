/**
 * @file eos_bindings.c
 * @brief EOS 绑定
 * @author Sab1e
 * @date 2025-12-27
 */

#include "eos_bindings.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jerryscript.h"
#include "cJSON.h"
#include "lvgl.h"
#include "script_engine_core.h"
#include "elena_os_port.h"
#include "elena_os_nav.h"
#include "elena_os_img.h"
#include "elena_os_app.h"
#include "elena_os_watchface.h"
#include "elena_os_misc.h"
#define EOS_LOG_TAG "EOS Bindings"
#include "elena_os_log.h"
#include "elena_os_port.h"
#include "elena_os_basic_widgets.h"
#include "elena_os_fs.h"
#include "elena_os_dfw.h"
#include "elena_os_app_header.h"
#include "elena_os_screen_mgr.h"
#include "elena_os_mem.h"
#include "lvgl_js_bridge.h"
#include "elena_os_ww_clock_hand.h"
#include "script_engine_core.h"
#include "script_engine_utils.h"

/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

/********************************** 辅助函数 **********************************/

static char *_get_assets_file_str(jerry_value_t obj)
{
    if (jerry_value_is_undefined(obj) || jerry_value_is_null(obj))
    {
        return NULL;
    }

    if (!jerry_value_is_string(obj))
    {
        return NULL;
    }

    // 取 JS 字符串
    jerry_size_t src_len = jerry_string_size(obj, JERRY_ENCODING_UTF8);
    char *src = eos_malloc(src_len + 1);
    if (!src)
    {
        return NULL;
    }

    jerry_string_to_buffer(obj, JERRY_ENCODING_UTF8,
                           (jerry_char_t *)src, src_len);
    src[src_len] = '\0';

    // 生成完整路径
    char path[PATH_MAX];

    const char *script_id = script_engine_get_current_script_id();
    if (!script_id)
    {
        eos_free(src);
        return NULL;
    }

    if (script_engine_get_current_script_type() == SCRIPT_TYPE_APPLICATION)
    {
        snprintf(path, sizeof(path),
                 EOS_APP_INSTALLED_DIR "%s/assets/%s",
                 script_id, src);
    }
    else if (script_engine_get_current_script_type() == SCRIPT_TYPE_WATCHFACE)
    {
        snprintf(path, sizeof(path),
                 EOS_WATCHFACE_INSTALLED_DIR "%s/assets/%s",
                 script_id, src);
    }
    else
    {
        eos_free(src);
        return NULL;
    }

    eos_free(src);

    if (!eos_is_file(path))
    {
        return NULL;
    }

    char *ret = eos_malloc(strlen(path) + 1);
    if (!ret)
    {
        return NULL;
    }

    strcpy(ret, path);
    EOS_LOG_D("Image Path: %s", ret);
    return ret;
}

// 内部工具函数：写入配置文件
static bool config_write_to_file(cJSON *root)
{
    char *json_str = cJSON_PrintUnformatted(root);
    if (!json_str)
        return false;
    char config_file_path[PATH_MAX];
    if (script_engine_get_current_script_type() == SCRIPT_TYPE_APPLICATION)
    {
        snprintf(config_file_path, sizeof(config_file_path), EOS_APP_DATA_DIR "%s",
                 script_engine_get_current_script_id());
        eos_fs_mkdir_if_not_exist(config_file_path);
        snprintf(config_file_path, sizeof(config_file_path), EOS_APP_DATA_DIR "%s/config.json",
                 script_engine_get_current_script_id());
    }
    else if (script_engine_get_current_script_type() == SCRIPT_TYPE_WATCHFACE)
    {
        snprintf(config_file_path, sizeof(config_file_path), EOS_WATCHFACE_DATA_DIR "%s",
                 script_engine_get_current_script_id());
        eos_fs_mkdir_if_not_exist(config_file_path);
        snprintf(config_file_path, sizeof(config_file_path), EOS_WATCHFACE_DATA_DIR "%s/config.json",
                 script_engine_get_current_script_id());
    }
    else
    {
        EOS_LOG_E("Unknown script type");
        return false;
    }
    EOS_LOG_D("Writing file: %s", config_file_path);

    size_t json_len = strlen(json_str);
#if EOS_DFW_ENABLE
    bool ret = eos_dfw_write(config_file_path, json_str, json_len);
#else
    bool ret = eos_fs_write_file(config_file_path, json_str, json_len) > 0 ? true : false;
#endif /* EOS_DFW_ENABLE */
    eos_free(json_str);

    if (!ret)
    {
        EOS_LOG_E("Write file failed");
        return false;
    }

    return true;
}

// 内部工具函数：加载配置文件
static cJSON *config_load_from_file(void)
{
    char config_file_path[PATH_MAX];

    if (script_engine_get_current_script_type() == SCRIPT_TYPE_APPLICATION)
    {
        eos_fs_mkdir_if_not_exist(EOS_APP_DATA_DIR);
        snprintf(config_file_path, sizeof(config_file_path), EOS_APP_DATA_DIR "%s/config.json",
                 script_engine_get_current_script_id());
    }
    else if (script_engine_get_current_script_type() == SCRIPT_TYPE_WATCHFACE)
    {
        eos_fs_mkdir_if_not_exist(EOS_WATCHFACE_DATA_DIR);
        snprintf(config_file_path, sizeof(config_file_path), EOS_WATCHFACE_DATA_DIR "%s/config.json",
                 script_engine_get_current_script_id());
    }
    else
    {
        EOS_LOG_E("Unknown script type");
        return NULL;
    }
    EOS_LOG_D("Load from file: %s", config_file_path);

    if (!eos_is_file(config_file_path))
    {
        return cJSON_CreateObject();
    }
#if EOS_DFW_ENABLE
    char *data = eos_dfw_read(config_file_path);
#else
    char *data = eos_fs_read_file(config_file_path);
#endif /* EOS_DFW_ENABLE */
    if (!data)
    {
        EOS_LOG_E("Read file failed");
        return cJSON_CreateObject();
    }

    cJSON *root = cJSON_Parse(data);
    eos_free(data);

    if (!root)
    {
        return cJSON_CreateObject();
    }
    return root;
}
/********************************** 原生函数定义 **********************************/
/**
 * @brief 处理 JavaScript 的 print 调用
 */
jerry_value_t js_print_handler(const jerry_call_info_t *call_info_p,
                               const jerry_value_t args_p[],
                               const jerry_length_t args_count)
{
    (void)call_info_p; // 当前未用到 this/func_obj，可忽略

    for (jerry_length_t i = 0; i < args_count; i++)
    {
        jerry_value_t str_val;

        if (jerry_value_is_string(args_p[i]))
        {
            str_val = jerry_value_copy(args_p[i]);
        }
        else
        {
            str_val = jerry_value_to_string(args_p[i]); // 转为字符串
        }

        jerry_size_t size = jerry_string_size(str_val, JERRY_ENCODING_UTF8);
        jerry_char_t *buf = (jerry_char_t *)eos_malloc(size + 1); // Explicitly cast void* to jerry_char_t*
        if (!buf)
        {
            jerry_value_free(str_val);
            continue;
        }

        jerry_string_to_buffer(str_val, JERRY_ENCODING_UTF8, buf, size);
        buf[size] = '\0';

        printf("%s", buf);
        if (i < args_count - 1)
        {
            printf(" ");
        }

        eos_free(buf);
        jerry_value_free(str_val);
    }

    printf("\n");
    return jerry_undefined();
}
/**
 * @brief Native 延时
 */
jerry_value_t js_delay_handler(const jerry_call_info_t *call_info_p,
                               const jerry_value_t args_p[],
                               const jerry_length_t args_count)
{
    eos_delay(args_p[0]);
}
/**
 * @brief 在脚本专用的导航栈上创建新的 screen
 * @param void 无参数
 * @return lv_obj_t* 返回 LVGL 对象指针
 */
static jerry_value_t js_eos_nav_scr_create(const jerry_call_info_t *call_info_p,
                                           const jerry_value_t args[],
                                           const jerry_length_t argc)
{
    if (script_engine_get_current_script_type() == SCRIPT_TYPE_WATCHFACE)
    {
        return script_engine_throw_error("Watchface can't create screen");
    }
    // 调用底层函数
    lv_obj_t *ret_value = eos_nav_scr_create();

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_OBJ);

    return js_result;
}
/**
 * @brief 在导航栈上返回上一级
 * @param void 无参数
 * @return bool 是否返回成功
 */
static jerry_value_t js_eos_nav_back(const jerry_call_info_t *call_info_p,
                                     const jerry_value_t args[],
                                     const jerry_length_t argc)
{
    if (script_engine_get_current_script_type() == SCRIPT_TYPE_WATCHFACE)
    {
        return script_engine_throw_error("Using navigation in the watchface is prohibited");
    }
    // 调用底层函数
    script_engine_result_t ret = eos_nav_back_clean();

    jerry_value_t js_result;

    if (ret == SE_OK)
    {
        js_result = jerry_boolean(true);
    }
    else
    {
        js_result = jerry_boolean(false);
    }

    return js_result;
}

static jerry_value_t js_lv_image_set_src(const jerry_call_info_t *call_info_p,
                                         const jerry_value_t args[],
                                         const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 2)
    {
        return script_engine_throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void *arg_obj = lv_js_bridge_obj_2_ptr(args[0], LV_TYPE_OBJ);
    // 解析参数: src (const char*)
    char *img_path = _get_assets_file_str(args[1]);
    // 调用底层函数
    eos_img_set_src(arg_obj, img_path);

    // 释放临时字符串内存
    if (img_path)
        eos_free(img_path);

    return jerry_undefined();
}

// 设置字符串配置项
static jerry_value_t js_config_set_str(const jerry_call_info_t *call_info_p,
                                       const jerry_value_t args[],
                                       const jerry_length_t argc)
{
    if (argc < 2 || !jerry_value_is_string(args[0]) || !jerry_value_is_string(args[1]))
    {
        return script_engine_throw_error("Usage: config_set_str(key, value)");
    }

    // key
    jerry_size_t key_len = jerry_string_size(args[0], JERRY_ENCODING_UTF8);
    char *key = eos_malloc(key_len + 1);
    jerry_string_to_buffer(args[0], JERRY_ENCODING_UTF8, (jerry_char_t *)key, key_len);
    key[key_len] = '\0';

    // value
    jerry_size_t val_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
    char *value = eos_malloc(val_len + 1);
    jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t *)value, val_len);
    value[val_len] = '\0';

    // JSON 写入
    cJSON *root = config_load_from_file();
    cJSON *item = cJSON_GetObjectItem(root, key);

    if (item)
    {
        cJSON_ReplaceItemInObject(root, key, cJSON_CreateString(value));
    }
    else
    {
        cJSON_AddItemToObject(root, key, cJSON_CreateString(value));
    }
    config_write_to_file(root);
    cJSON_Delete(root);

    eos_free(key);
    eos_free(value);
    return jerry_undefined();
}

// 设置布尔
static jerry_value_t js_config_set_bool(const jerry_call_info_t *call_info_p,
                                        const jerry_value_t args[],
                                        const jerry_length_t argc)
{
    if (argc < 2 || !jerry_value_is_string(args[0]) || !jerry_value_is_boolean(args[1]))
    {
        return script_engine_throw_error("Usage: config_set_bool(key, bool)");
    }

    // key
    jerry_size_t key_len = jerry_string_size(args[0], JERRY_ENCODING_UTF8);
    char *key = eos_malloc(key_len + 1);
    jerry_string_to_buffer(args[0], JERRY_ENCODING_UTF8, (jerry_char_t *)key, key_len);
    key[key_len] = '\0';

    bool value = jerry_value_is_true(args[1]);

    // JSON 写入
    cJSON *root = config_load_from_file();
    if (!root)
    {
        return script_engine_throw_error("Can't load config");
    }
    cJSON *item = cJSON_GetObjectItem(root, key);

    if (item)
    {
        cJSON_ReplaceItemInObject(root, key, cJSON_CreateBool(value));
    }
    else
    {
        cJSON_AddItemToObject(root, key, cJSON_CreateBool(value));
    }
    config_write_to_file(root);
    cJSON_Delete(root);

    eos_free(key);
    return jerry_undefined();
}

// 设置数字
static jerry_value_t js_config_set_number(const jerry_call_info_t *call_info_p,
                                          const jerry_value_t args[],
                                          const jerry_length_t argc)
{
    if (argc < 2 || !jerry_value_is_string(args[0]) || !jerry_value_is_number(args[1]))
    {
        return script_engine_throw_error("Usage: config_set_number(key, number)");
    }

    // key
    jerry_size_t key_len = jerry_string_size(args[0], JERRY_ENCODING_UTF8);
    char *key = eos_malloc(key_len + 1);
    jerry_string_to_buffer(args[0], JERRY_ENCODING_UTF8, (jerry_char_t *)key, key_len);
    key[key_len] = '\0';

    double value = jerry_value_as_number(args[1]);

    // JSON 写入
    cJSON *root = config_load_from_file();
    if (!root)
    {
        return script_engine_throw_error("Can't load config");
    }
    cJSON *item = cJSON_GetObjectItem(root, key);

    if (item)
    {
        EOS_LOG_D("Replace item");
        cJSON_ReplaceItemInObject(root, key, cJSON_CreateNumber(value));
    }
    else
    {
        EOS_LOG_D("Create item");
        cJSON_AddItemToObject(root, key, cJSON_CreateNumber(value));
    }
    config_write_to_file(root);

    cJSON_Delete(root);

    eos_free(key);
    return jerry_undefined();
}

// 获取字符串
static jerry_value_t js_config_get_str(const jerry_call_info_t *call_info_p,
                                       const jerry_value_t args[],
                                       const jerry_length_t argc)
{
    if (argc < 1 || !jerry_value_is_string(args[0]))
    {
        return script_engine_throw_error("Usage: config_get_str(key)");
    }

    jerry_size_t key_len = jerry_string_size(args[0], JERRY_ENCODING_UTF8);
    char *key = eos_malloc(key_len + 1);
    jerry_string_to_buffer(args[0], JERRY_ENCODING_UTF8, (jerry_char_t *)key, key_len);
    key[key_len] = '\0';

    cJSON *root = config_load_from_file();
    if (!root)
    {
        return script_engine_throw_error("Can't load config");
    }
    cJSON *item = cJSON_GetObjectItem(root, key);

    jerry_value_t ret = jerry_undefined();
    if (item && cJSON_IsString(item))
    {
        ret = jerry_string_sz(item->valuestring);
    }
    else
    {
        EOS_LOG_E("Can't get item");
    }

    cJSON_Delete(root);
    eos_free(key);
    return ret;
}

// 获取布尔
static jerry_value_t js_config_get_bool(const jerry_call_info_t *call_info_p,
                                        const jerry_value_t args[],
                                        const jerry_length_t argc)
{
    if (argc < 1 || !jerry_value_is_string(args[0]))
    {
        return script_engine_throw_error("Usage: config_get_bool(key)");
    }

    jerry_size_t key_len = jerry_string_size(args[0], JERRY_ENCODING_UTF8);
    char *key = eos_malloc(key_len + 1);
    jerry_string_to_buffer(args[0], JERRY_ENCODING_UTF8, (jerry_char_t *)key, key_len);
    key[key_len] = '\0';

    cJSON *root = config_load_from_file();
    if (!root)
    {
        return script_engine_throw_error("Can't load config");
    }
    cJSON *item = cJSON_GetObjectItem(root, key);

    jerry_value_t ret = jerry_boolean(false);
    if (item && cJSON_IsBool(item))
    {
        ret = item->valueint ? jerry_boolean(true) : jerry_boolean(false);
    }
    else
    {
        EOS_LOG_E("Can't get item");
    }

    cJSON_Delete(root);
    eos_free(key);
    return ret;
}

// 获取数字
static jerry_value_t js_config_get_number(const jerry_call_info_t *call_info_p,
                                          const jerry_value_t args[],
                                          const jerry_length_t argc)
{
    if (argc < 1 || !jerry_value_is_string(args[0]))
    {
        return script_engine_throw_error("Usage: config_get_number(key)");
    }

    jerry_size_t key_len = jerry_string_size(args[0], JERRY_ENCODING_UTF8);
    char *key = eos_malloc(key_len + 1);
    jerry_string_to_buffer(args[0], JERRY_ENCODING_UTF8, (jerry_char_t *)key, key_len);
    key[key_len] = '\0';

    cJSON *root = config_load_from_file();
    if (!root)
    {
        return script_engine_throw_error("Can't load config");
    }
    cJSON *item = cJSON_GetObjectItem(root, key);

    jerry_value_t ret = jerry_number(0);
    if (item && cJSON_IsNumber(item))
    {
        ret = jerry_number(item->valuedouble);
    }
    else
    {
        EOS_LOG_E("Can't get item");
    }

    cJSON_Delete(root);
    eos_free(key);
    return ret;
}

// 返回时间对象给 JS
static jerry_value_t js_eos_time_get(const jerry_call_info_t *call_info_p,
                                     const jerry_value_t args[],
                                     const jerry_length_t argc)
{
    eos_datetime_t dt = eos_time_get();

    // 创建 JS 对象
    jerry_value_t obj = jerry_object();

    script_engine_set_prop_number(obj, "year", dt.year);
    script_engine_set_prop_number(obj, "month", dt.month);
    script_engine_set_prop_number(obj, "day", dt.day);
    script_engine_set_prop_number(obj, "hour", dt.hour);
    script_engine_set_prop_number(obj, "min", dt.min);
    script_engine_set_prop_number(obj, "sec", dt.sec);
    script_engine_set_prop_number(obj, "day_of_week", dt.day_of_week);

    return obj;
}

// 设置字符串配置项
static jerry_value_t js_eos_app_header_set_title(const jerry_call_info_t *call_info_p,
                                                 const jerry_value_t args[],
                                                 const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 2)
    {
        return script_engine_throw_error("eos_app_header_set_title: Insufficient arguments");
    }

    // 解析参数: scr (lv_obj_t*)
    // 对象类型参数，支持null
    void *arg_obj = lv_js_bridge_obj_2_ptr(args[0], LV_TYPE_OBJ);

    // 解析参数: text (const char*)

    char *arg_text_str = NULL;
    const char *arg_text = NULL;
    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1]))
    {
        if (!jerry_value_is_string(args[1]))
        {
            return script_engine_throw_error("Argument 1 must be a string");
        }
        jerry_size_t arg_text_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
        arg_text_str = (char *)eos_malloc(arg_text_len + 1);
        jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t *)arg_text_str, arg_text_len);
        arg_text_str[arg_text_len] = '\0';
        arg_text = arg_text_str;
    }

    // 调用底层函数
    eos_app_header_set_title(arg_obj, arg_text);

    // 释放临时字符串内存
    if (arg_text_str)
        eos_free(arg_text_str);

    return jerry_undefined();
}

static jerry_value_t js_eos_app_header_hide(const jerry_call_info_t *call_info_p,
                                            const jerry_value_t args[],
                                            const jerry_length_t argc)
{
    eos_app_header_hide();
    return jerry_undefined();
}

static jerry_value_t js_eos_app_header_show(const jerry_call_info_t *call_info_p,
                                            const jerry_value_t args[],
                                            const jerry_length_t argc)
{
    eos_app_header_show();
    return jerry_undefined();
}

static jerry_value_t js_eos_screen_active(const jerry_call_info_t *call_info_p,
                                          const jerry_value_t args[],
                                          const jerry_length_t argc)
{
    // 调用底层函数
    lv_obj_t *ret_value = eos_screen_active();

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_OBJ);

    return js_result;
}

static jerry_value_t js_eos_clock_hand_create(const jerry_call_info_t *call_info_p,
                                              const jerry_value_t args[],
                                              const jerry_length_t argc)
{
    if (argc < 5)
    {
        return script_engine_throw_error("clockHand.create(obj, src, type, cx, cy)");
    }

    js_arg_type_t type_arr[] = {
        ARG_TYPE_OBJ,
        ARG_TYPE_STR,
        ARG_TYPE_NUMBER,
        ARG_TYPE_NUMBER,
        ARG_TYPE_NUMBER,
    };

    int32_t ret = js_args_check_type(args, type_arr, argc);
    if (ret >= 0)
    {
        char err[128];
        snprintf(err, sizeof(err),
                 "Argument %d type mismatch, expected %s",
                 ret, js_get_arg_type_str(type_arr[ret]));
        return script_engine_throw_error(err);
    }

    lv_obj_t *obj = lv_js_bridge_obj_2_ptr(args[0], LV_TYPE_OBJ);
    if (!obj)
    {
        return script_engine_throw_error("Invalid LVGL object");
    }

    char *src = _get_assets_file_str(args[1]);
    if (!src)
    {
        return script_engine_throw_error("Invalid image source");
    }

    int type = (int)jerry_value_as_number(args[2]);
    int cx = (int)jerry_value_as_number(args[3]);
    int cy = (int)jerry_value_as_number(args[4]);

    lv_obj_t *ret_obj = eos_clock_hand_create(obj, src, type, cx, cy);

    jerry_value_t ret_val = lv_js_bridge_ptr_2_obj(ret_obj, LV_TYPE_OBJ);

    eos_free(src);
    return ret_val;
}

static jerry_value_t js_eos_clock_hand_center(const jerry_call_info_t *call_info_p,
                                              const jerry_value_t args[],
                                              const jerry_length_t argc)
{
    if (argc < 1)
    {
        return script_engine_throw_error("clockHand.center(obj)");
    }

    js_arg_type_t type_arr[] = {
        ARG_TYPE_OBJ};

    int32_t ret = js_args_check_type(args, type_arr, argc);
    if (ret >= 0)
    {
        char err[128];
        snprintf(err, sizeof(err),
                 "Argument %d type mismatch, expected %s",
                 ret, js_get_arg_type_str(type_arr[ret]));
        return script_engine_throw_error(err);
    }

    lv_obj_t *obj = lv_js_bridge_obj_2_ptr(args[0], LV_TYPE_OBJ);
    if (!obj)
    {
        return script_engine_throw_error("Invalid LVGL object");
    }

    eos_clock_hand_center(obj);

    return jerry_undefined();
}

static jerry_value_t js_eos_clock_hand_place_pivot(const jerry_call_info_t *call_info_p,
                                                   const jerry_value_t args[],
                                                   const jerry_length_t argc)
{
    if (argc < 3)
    {
        return script_engine_throw_error("clockHand.placePivot(obj, x, y)");
    }

    js_arg_type_t type_arr[] = {
        ARG_TYPE_OBJ,
        ARG_TYPE_NUMBER,
        ARG_TYPE_NUMBER};

    int32_t ret = js_args_check_type(args, type_arr, argc);
    if (ret >= 0)
    {
        char err[128];
        snprintf(err, sizeof(err),
                 "Argument %d type mismatch, expected %s",
                 ret, js_get_arg_type_str(type_arr[ret]));
        return script_engine_throw_error(err);
    }

    lv_obj_t *obj = lv_js_bridge_obj_2_ptr(args[0], LV_TYPE_OBJ);
    if (!obj)
    {
        return script_engine_throw_error("Invalid LVGL object");
    }

    eos_clock_hand_place_pivot(obj, jerry_value_as_number(args[1]), jerry_value_as_number(args[2]));

    return jerry_undefined();
}

/************************** 注册常量 **************************/

static void register_eos_enums(jerry_value_t parent)
{

    typedef struct
    {
        const char *name;
        int value;
    } eos_enum_entry_t;

    static const eos_enum_entry_t enum_entries[] = {
        {"CLOCK_HAND_HOUR", EOS_CLOCK_HAND_HOUR},
        {"CLOCK_HAND_MINUTE", EOS_CLOCK_HAND_MINUTE},
        {"CLOCK_HAND_SECOND", EOS_CLOCK_HAND_SECOND},
        {NULL, 0}};

    // 注册枚举条目
    for (size_t i = 0; enum_entries[i].name != NULL; ++i)
    {
        jerry_value_t jkey = jerry_string_sz(enum_entries[i].name);
        jerry_value_t jval = jerry_number(enum_entries[i].value);
        jerry_value_free(jerry_object_set(parent, jkey, jval));
        jerry_value_free(jkey);
        jerry_value_free(jval);
    }
}

/********************************** 注册原生函数 **********************************/

/**
 * @brief 原生函数列表
 */
const script_engine_func_entry_t script_engine_native_funcs[] = {
    {NULL, "print", js_print_handler},
    {NULL, "delay", js_delay_handler},
    {"nav", "scrCreate", js_eos_nav_scr_create},
    {"nav", "back", js_eos_nav_back},
    {"appHeader", "setTitle", js_eos_app_header_set_title},
    {"appHeader", "hide", js_eos_app_header_hide},
    {"appHeader", "show", js_eos_app_header_show},
    {"image", "setSrc", js_lv_image_set_src},
    {"config", "setStr", js_config_set_str},
    {"config", "setBool", js_config_set_bool},
    {"config", "setNumber", js_config_set_number},
    {"config", "getStr", js_config_get_str},
    {"config", "getBool", js_config_get_bool},
    {"config", "getNumber", js_config_get_number},
    {"time", "getNow", js_eos_time_get},
    {"screen", "getActive", js_eos_screen_active},
    {"clockHand", "create", js_eos_clock_hand_create},
    {"clockHand", "center", js_eos_clock_hand_center},
    {"clockHand", "placePivot", js_eos_clock_hand_place_pivot}};

void eos_binding_init(jerry_value_t parent)
{
    script_engine_register_functions(parent,
                                     script_engine_native_funcs,
                                     sizeof(script_engine_native_funcs) / sizeof(script_engine_func_entry_t));
    register_eos_enums(parent);
}
