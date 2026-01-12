/**
 * @file lv_bindings_anim.c
 * @brief 动画系统绑定
 * @author Sab1e
 * @date 2025-12-28
 */

#include "lv_bindings_anim.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define EOS_LOG_TAG "LV Bindings Anim"
#include "elena_os_log.h"
#include "lv_bindings.h"
#include "elena_os_nav.h"
#include "elena_os_watchface.h"
#include "elena_os_port.h"
#include "elena_os_mem.h"
#include "lvgl_js_bridge.h"
#include "script_engine_lv.h"
#include "elena_os_app.h"
#include "elena_os_fs.h"
#include "script_engine_utils.h"

/* Macros and Definitions -------------------------------------*/

typedef enum
{
    ANIM_PATH_NONE = 0,
    ANIM_PATH_LINEAR,
    ANIM_PATH_EASE_IN,
    ANIM_PATH_EASE_OUT,
    ANIM_PATH_EASE_IN_OUT,
    ANIM_PATH_OVERSHOOT,
    ANIM_PATH_BOUNCE,
    ANIM_PATH_STEP,
    ANIM_PATH_CUSTOM_BEZIER3,
    ANIM_PATH_ENUM_MAX
} anim_path_t;

/* Variables --------------------------------------------------*/
static const lv_anim_path_cb_t lv_anim_path_def[ANIM_PATH_ENUM_MAX] =
    {
        NULL,
        lv_anim_path_linear,
        lv_anim_path_ease_in,
        lv_anim_path_ease_out,
        lv_anim_path_ease_in_out,
        lv_anim_path_overshoot,
        lv_anim_path_bounce,
        lv_anim_path_step,
        lv_anim_path_custom_bezier3};
/* Function Implementations -----------------------------------*/

/************************** 回调 **************************/

void lv_bindings_anim_custom_exec_cb_handler(lv_anim_t *a, int32_t v)
{
    if (script_engine_get_state() != SCRIPT_STATE_RUNNING && script_engine_get_state() != SCRIPT_STATE_SUSPEND)
    {
        return;
    }
    js_anim_user_data_t *js_ud = lv_anim_get_user_data(a);
    if (!js_ud)
        return;
    jerry_value_t func = js_ud->custom_exec_cb;
    if (jerry_value_is_exception(func) && !jerry_value_is_object(func) && !jerry_value_is_function(func))
        return;

    static const jerry_size_t argc = 2;
    jerry_value_t val = jerry_number(v);
    jerry_value_t args[argc] = {js_ud->anim_obj, val};

    jerry_value_t ret = jerry_call(js_ud->custom_exec_cb, jerry_null(), args, argc);
    if (jerry_value_is_error(ret))
    {
        EOS_LOG_E("Callback encounter an error");
    }
    jerry_value_free(ret);
    jerry_value_free(val);
}

void lv_bindings_anim_completed_cb_handler(lv_anim_t *a)
{
    if (script_engine_get_state() != SCRIPT_STATE_RUNNING && script_engine_get_state() != SCRIPT_STATE_SUSPEND)
    {
        return;
    }
    js_anim_user_data_t *js_ud = lv_anim_get_user_data(a);
    if (!js_ud)
        return;
    jerry_value_t func = js_ud->completed_cb;
    if (jerry_value_is_exception(func) || (!jerry_value_is_object(func) && !jerry_value_is_function(func)))
        return;

    static const jerry_size_t argc = 1;
    jerry_value_t args[argc] = {js_ud->anim_obj};

    jerry_value_t ret = jerry_call(js_ud->completed_cb, jerry_null(), args, argc);

    if (jerry_value_is_error(ret))
    {
        EOS_LOG_E("Callback encounter an error");
    }

    jerry_value_free(ret);
}

void lv_bindings_anim_start_cb_handler(lv_anim_t *a)
{
    if (script_engine_get_state() != SCRIPT_STATE_RUNNING && script_engine_get_state() != SCRIPT_STATE_SUSPEND)
    {
        return;
    }
    js_anim_user_data_t *js_ud = lv_anim_get_user_data(a);
    if (!js_ud)
        return;
    jerry_value_t func = js_ud->start_cb;
    if (jerry_value_is_exception(func) || (!jerry_value_is_object(func) && !jerry_value_is_function(func)))
        return;

    static const jerry_size_t argc = 1;
    jerry_value_t args[argc] = {js_ud->anim_obj};

    jerry_value_t ret = jerry_call(js_ud->start_cb, jerry_null(), args, argc);

    if (jerry_value_is_error(ret))
    {
        EOS_LOG_E("Callback encounter an error");
    }

    jerry_value_free(ret);
}

int32_t lv_bindings_anim_get_value_cb_handler(lv_anim_t *a)
{
    if (script_engine_get_state() != SCRIPT_STATE_RUNNING && script_engine_get_state() != SCRIPT_STATE_SUSPEND)
    {
        return 0;
    }
    js_anim_user_data_t *js_ud = lv_anim_get_user_data(a);
    if (!js_ud)
        return 0;
    jerry_value_t func = js_ud->get_value_cb;
    if (jerry_value_is_exception(func) || (!jerry_value_is_object(func) && !jerry_value_is_function(func)))
        return 0;

    static const jerry_size_t argc = 1;
    jerry_value_t args[argc] = {js_ud->anim_obj};

    jerry_value_t ret = jerry_call(js_ud->get_value_cb, jerry_null(), args, argc);

    int32_t result = 0;
    if (jerry_value_is_error(ret))
    {
        EOS_LOG_E("Callback encounter an error");
    }
    else if (jerry_value_is_number(ret))
    {
        result = jerry_value_as_int32(ret);
    }

    jerry_value_free(ret);
    return result;
}

void lv_bindings_anim_deleted_cb_handler(lv_anim_t *a)
{
    if (script_engine_get_state() != SCRIPT_STATE_RUNNING && script_engine_get_state() != SCRIPT_STATE_SUSPEND)
    {
        return;
    }
    js_anim_user_data_t *js_ud = lv_anim_get_user_data(a);
    if (!js_ud)
        return;
    jerry_value_t func = js_ud->deleted_cb;
    if (jerry_value_is_exception(func) || (!jerry_value_is_object(func) && !jerry_value_is_function(func)))
        return;

    static const jerry_size_t argc = 1;
    jerry_value_t args[argc] = {js_ud->anim_obj};

    jerry_value_t ret = jerry_call(js_ud->deleted_cb, jerry_null(), args, argc);

    if (jerry_value_is_error(ret))
    {
        EOS_LOG_E("Callback encounter an error");
    }

    jerry_value_free(ret);

    eos_free(js_ud);
}

int32_t lv_bindings_anim_path_cb_hander(const lv_anim_t *a)
{
    if (script_engine_get_state() != SCRIPT_STATE_RUNNING && script_engine_get_state() != SCRIPT_STATE_SUSPEND)
    {
        return 0;
    }
    js_anim_user_data_t *js_ud = lv_anim_get_user_data(a);
    if (!js_ud)
        return 0;
    jerry_value_t func = js_ud->path_cb;
    if (jerry_value_is_exception(func) || (!jerry_value_is_object(func) && !jerry_value_is_function(func)))
        return 0;

    jerry_value_t anim_obj = lv_js_bridge_anim_2_obj((lv_anim_t *)a);
    static const jerry_size_t argc = 1;
    jerry_value_t args[argc] = {anim_obj};

    jerry_value_t ret = jerry_call(js_ud->path_cb, jerry_null(), args, argc);

    int32_t result = 0;
    if (jerry_value_is_error(ret))
    {
        EOS_LOG_E("Callback encounter an error");
    }
    else if (jerry_value_is_number(ret))
    {
        result = jerry_value_as_int32(ret);
    }

    jerry_value_free(ret);
    jerry_value_free(anim_obj);
    return result;
}

/************************** 函数绑定 **************************/

/**
 * @brief Set a function call when the animation is completed
 */
static jerry_value_t js_lv_anim_set_completed_cb(const jerry_call_info_t *call_info_p,
                                                 const jerry_value_t args[],
                                                 const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 2)
    {
        return script_engine_throw_error("lv_anim_set_completed_cb: Insufficient arguments");
    }

    // 解析参数: a (lv_anim_t*)
    lv_anim_t *arg_a = lv_js_bridge_obj_2_ptr(args[0], LV_TYPE_ANIM);

    if (!jerry_value_is_function(args[1]))
    {
        return script_engine_throw_error("Argument 1 must be a function");
    }

    // 获取 JS 对象
    jerry_value_t js_obj = lv_js_bridge_ptr_2_obj(arg_a, LV_TYPE_ANIM);

    // 设置回调属性
    jerry_value_t key = jerry_string_sz("completedCb");
    jerry_value_t val = jerry_value_copy(args[1]);
    jerry_value_free(jerry_object_set(js_obj, key, val));
    jerry_value_free(val);
    jerry_value_free(key);

    // 设置 C 回调
    lv_anim_set_completed_cb(arg_a, lv_bindings_anim_completed_cb_handler);

    jerry_value_free(js_obj);

    return jerry_undefined();
}

/**
 * @brief Similar to lv_anim_set_exec_cb but :ref:`lv_anim_custom_exec_cb_t` receives :ref:`lv_anim_t` * as its first parameter instead of void * . This function might be used when LVGL is bound to other languages because it's more consistent to have :ref:`lv_anim_t` * as first parameter.
 */
static jerry_value_t js_lv_anim_set_custom_exec_cb(const jerry_call_info_t *call_info_p,
                                                   const jerry_value_t args[],
                                                   const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 2)
    {
        return script_engine_throw_error("lv_anim_set_custom_exec_cb: Insufficient arguments");
    }

    // 解析参数: a (lv_anim_t*)
    // 对象类型参数，支持null
    lv_anim_t *arg_a = lv_js_bridge_obj_2_ptr(args[0], LV_TYPE_ANIM);

    // 调用底层函数
    lv_anim_set_custom_exec_cb(arg_a, lv_bindings_anim_custom_exec_cb_handler);

    return jerry_undefined();
}

/**
 * @brief Set a function call when the animation is deleted.
 */
static jerry_value_t js_lv_anim_set_deleted_cb(const jerry_call_info_t *call_info_p,
                                               const jerry_value_t args[],
                                               const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 2)
    {
        return script_engine_throw_error("lv_anim_set_deleted_cb: Insufficient arguments");
    }

    // 解析参数: a (lv_anim_t*)
    lv_anim_t *arg_a = lv_js_bridge_obj_2_ptr(args[0], LV_TYPE_ANIM);

    if (!jerry_value_is_function(args[1]))
    {
        return script_engine_throw_error("Argument 1 must be a function");
    }

    // 获取 JS 对象
    jerry_value_t js_obj = lv_js_bridge_ptr_2_obj(arg_a, LV_TYPE_ANIM);

    // 设置回调属性
    jerry_value_t key = jerry_string_sz("deletedCb");
    jerry_value_t val = jerry_value_copy(args[1]);
    jerry_value_free(jerry_object_set(js_obj, key, val));
    jerry_value_free(val);
    jerry_value_free(key);

    // 设置 C 回调
    lv_anim_set_deleted_cb(arg_a, lv_bindings_anim_deleted_cb_handler);

    jerry_value_free(js_obj);

    return jerry_undefined();
}

/**
 * @brief Set a function to use the current value of the variable and make start and end value relative to the returned current value.
 */
static jerry_value_t js_lv_anim_set_get_value_cb(const jerry_call_info_t *call_info_p,
                                                 const jerry_value_t args[],
                                                 const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 2)
    {
        return script_engine_throw_error("lv_anim_set_get_value_cb: Insufficient arguments");
    }

    // 解析参数: a (lv_anim_t*)
    lv_anim_t *arg_a = lv_js_bridge_obj_2_ptr(args[0], LV_TYPE_ANIM);

    if (!jerry_value_is_function(args[1]))
    {
        return script_engine_throw_error("Argument 1 must be a function");
    }

    // 获取 JS 对象
    jerry_value_t js_obj = lv_js_bridge_ptr_2_obj(arg_a, LV_TYPE_ANIM);

    // 设置回调属性
    jerry_value_t key = jerry_string_sz("getValue");
    jerry_value_t val = jerry_value_copy(args[1]);
    jerry_value_free(jerry_object_set(js_obj, key, val));
    jerry_value_free(val);
    jerry_value_free(key);

    // 设置 C 回调
    lv_anim_set_get_value_cb(arg_a, lv_bindings_anim_get_value_cb_handler);

    jerry_value_free(js_obj);

    return jerry_undefined();
}

/**
 * @brief Set the path (curve) of the animation.
 */
static jerry_value_t js_lv_anim_set_path_cb(const jerry_call_info_t *call_info_p,
                                            const jerry_value_t args[],
                                            const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 2)
    {
        return script_engine_throw_error("lv_anim_set_path_cb: Insufficient arguments");
    }

    // 解析参数: a (lv_anim_t*)
    lv_anim_t *arg_a = lv_js_bridge_obj_2_ptr(args[0], LV_TYPE_ANIM);

    if (!jerry_value_is_function(args[1]))
    {
        return script_engine_throw_error("Argument 1 must be a function");
    }

    // 获取 JS 对象
    jerry_value_t js_obj = lv_js_bridge_ptr_2_obj(arg_a, LV_TYPE_ANIM);

    if (jerry_value_is_function(args[1]))
    {
        // 设置回调属性
        jerry_value_t key = jerry_string_sz("pathCb");
        jerry_value_t val = jerry_value_copy(args[1]);
        jerry_value_free(jerry_object_set(js_obj, key, val));
        jerry_value_free(val);
        jerry_value_free(key);

        // 设置 C 回调
        lv_anim_set_path_cb(arg_a, lv_bindings_anim_path_cb_hander);
    }
    else if (jerry_value_is_number(args[1]))
    {
        uint32_t i = jerry_value_as_uint32(args[1]);
        if (i > ANIM_PATH_NONE && i < ANIM_PATH_ENUM_MAX)
        {
            lv_anim_set_path_cb(arg_a, lv_anim_path_def[i]);
        }
        else
        {
            return script_engine_throw_error("Argument 1 is out of range");
        }
    }
    else
    {
        return script_engine_throw_error("Argument 1 must be a function or a number");
    }

    jerry_value_free(js_obj);

    return jerry_undefined();
}

/**
 * @brief Set a function call when the animation really starts (considering delay )
 */
static jerry_value_t js_lv_anim_set_start_cb(const jerry_call_info_t *call_info_p,
                                             const jerry_value_t args[],
                                             const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 2)
    {
        return script_engine_throw_error("lv_anim_set_start_cb: Insufficient arguments");
    }

    // 解析参数: a (lv_anim_t*)
    lv_anim_t *arg_a = lv_js_bridge_obj_2_ptr(args[0], LV_TYPE_ANIM);

    if (!jerry_value_is_function(args[1]))
    {
        return script_engine_throw_error("Argument 1 must be a function");
    }

    // 获取 JS 对象
    jerry_value_t js_obj = lv_js_bridge_ptr_2_obj(arg_a, LV_TYPE_ANIM);

    // 设置回调属性
    jerry_value_t key = jerry_string_sz("startCb");
    jerry_value_t val = jerry_value_copy(args[1]);
    jerry_value_free(jerry_object_set(js_obj, key, val));
    jerry_value_free(val);
    jerry_value_free(key);

    // 设置 C 回调
    lv_anim_set_start_cb(arg_a, lv_bindings_anim_start_cb_handler);

    jerry_value_free(js_obj);

    return jerry_undefined();
}

/**
 * @brief Set the custom user data field of the animation.
 */
static jerry_value_t js_lv_anim_set_user_data(const jerry_call_info_t *call_info_p,
                                              const jerry_value_t args[],
                                              const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 2)
    {
        return script_engine_throw_error("lv_anim_set_user_data: Insufficient arguments");
    }

    // 解析参数: a (lv_anim_t*)
    // 对象类型参数，支持null
    lv_anim_t *arg_a = lv_js_bridge_obj_2_ptr(args[0], LV_TYPE_ANIM);
    // 解析参数: user_data (void*)
    // void*/字符串 类型参数，支持null
    void *arg_user_data = NULL;
    char *arg_user_data_str = NULL; // 用于字符串参数的临时存储

    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1]))
    {
        if (jerry_value_is_string(args[1]))
        {
            // 处理字符串类型的符号（如LV_SYMBOL_MINUS）
            jerry_size_t arg_user_data_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
            arg_user_data_str = (char *)eos_malloc(arg_user_data_len + 1);
            if (!arg_user_data_str)
            {
                return script_engine_throw_error("Failed to allocate memory for string argument");
            }
            jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t *)arg_user_data_str, arg_user_data_len);
            arg_user_data_str[arg_user_data_len] = '\0';
            arg_user_data = (void *)arg_user_data_str;
        }
        else if (jerry_value_is_object(args[1]))
        {
            // 尝试从对象获取指针
            arg_user_data = lv_js_bridge_obj_2_ptr(args[1], LV_TYPE_ANY);
        }
        else if (jerry_value_is_number(args[1]))
        {
            // 直接传递指针数值
            uintptr_t ptr_num = (uintptr_t)jerry_value_as_number(args[1]);
            arg_user_data = (void *)ptr_num;
        }
        else
        {
            return script_engine_throw_error("Argument 1 must be string, object or number");
        }
    }
    // 调用底层函数
    lv_anim_set_user_data(arg_a, arg_user_data);

    // 释放临时内存
    if (arg_user_data_str)
        eos_free(arg_user_data_str);

    return jerry_undefined();
}

/**
 * @brief Get the user_data field of the animation the pointer to the custom user_data of the animation
 */
static jerry_value_t js_lv_anim_get_user_data(const jerry_call_info_t *call_info_p,
                                              const jerry_value_t args[],
                                              const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 1)
    {
        return script_engine_throw_error("lv_anim_get_user_data: Insufficient arguments");
    }

    // 解析参数: a (const lv_anim_t*)
    lv_anim_t *arg_a = lv_js_bridge_obj_2_ptr(args[0], LV_TYPE_ANIM);
    // 调用底层函数
    js_anim_user_data_t *ret_value = lv_anim_get_user_data(arg_a);

    if (!ret_value || !ret_value->user_data)
    {
        return jerry_null();
    }

    // 处理返回值
    jerry_value_t js_result;
    // 包装为通用指针对象
    js_result = lv_js_bridge_ptr_2_obj(ret_value->user_data, LV_TYPE_ANY);

    return js_result;
}

jerry_value_t js_lv_anim_create(const jerry_call_info_t *info,
                                const jerry_value_t args[],
                                const jerry_length_t argc)
{
    jerry_value_t obj = jerry_object();

    // 创建 bezier3 子对象
    jerry_value_t bezier3 = jerry_object();

    jerry_value_t key;
    jerry_value_t val;

    key = jerry_string_sz("bezier3");
    jerry_value_free(jerry_object_set(obj, key, bezier3));
    jerry_value_free(key);
    jerry_value_free(bezier3);

    return obj;
}


static void _script_exited_cb(lv_event_t *e)
{
    void *var = lv_event_get_user_data(e);
    if (!var)
        return;
    lv_anim_delete(var, NULL);
    eos_event_remove_global_cb_with_user_data(
        EOS_EVENT_SCRIPT_EXITED,
        _script_exited_cb,
        var);
}

jerry_value_t js_lv_anim_start(
    const jerry_call_info_t *info,
    const jerry_value_t args[],
    const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 1)
    {
        return script_engine_throw_error("lv_anim_start: Insufficient arguments");
    }

    js_arg_type_t type_arr[] = {
        ARG_TYPE_OBJ,
    };

    js_args_check_type(args, type_arr, argc);

    lv_anim_t a = lv_js_bridge_obj_2_anim(args[0]);

    eos_event_add_global_cb(_script_exited_cb, EOS_EVENT_SCRIPT_EXITED, a.var);

    lv_anim_start(&a);

    return jerry_undefined();
}

const script_engine_func_entry_t lvgl_binding_anim_funcs[] = {
    {"anim", "setCustomExecCb", js_lv_anim_set_custom_exec_cb},
    {"anim", "setPathCb", js_lv_anim_set_path_cb},
    {"anim", "setStartCb", js_lv_anim_set_start_cb},
    {"anim", "setGetValueCb", js_lv_anim_set_get_value_cb},
    {"anim", "setCompletedCb", js_lv_anim_set_completed_cb},
    {"anim", "setDeletedCb", js_lv_anim_set_deleted_cb},
    {"anim", "setUserData", js_lv_anim_set_user_data},
    {"anim", "getUserData", js_lv_anim_get_user_data},
    {"anim", "setReadyCb", js_lv_anim_set_completed_cb},
    {"anim", "create", js_lv_anim_create},
    {"anim", "start", js_lv_anim_start}};

lv_anim_path_cb_t lv_bindings_anim_get_path_cb(uint32_t id)
{
    if (id >= 0 && id < ANIM_PATH_ENUM_MAX)
    {
        return lv_anim_path_def[id];
    }
    else
    {
        return NULL;
    }
}

static void lvgl_binding_set_enum(jerry_value_t parent, const char *key, int32_t val)
{
    jerry_value_t jkey = jerry_string_sz(key);
    jerry_value_t jval = jerry_number(val);
    jerry_value_free(jerry_object_set(parent, jkey, jval));
    jerry_value_free(jkey);
    jerry_value_free(jval);
}

void lv_bindings_anim_init(jerry_value_t parent)
{
    // 初始化函数
    script_engine_register_functions(parent,
                                     lvgl_binding_anim_funcs,
                                     sizeof(lvgl_binding_anim_funcs) / sizeof(script_engine_func_entry_t));

    typedef struct
    {
        const char *name;
        int value;
    } lvgl_enum_entry_t;

    static const lvgl_enum_entry_t enum_entries[ANIM_PATH_ENUM_MAX] = {
        {"ANIM_PATH_NONE", ANIM_PATH_NONE},
        {"ANIM_PATH_LINEAR", ANIM_PATH_LINEAR},
        {"ANIM_PATH_EASE_IN", ANIM_PATH_EASE_IN},
        {"ANIM_PATH_EASE_OUT", ANIM_PATH_EASE_OUT},
        {"ANIM_PATH_EASE_IN_OUT", ANIM_PATH_EASE_IN_OUT},
        {"ANIM_PATH_OVERSHOOT", ANIM_PATH_OVERSHOOT},
        {"ANIM_PATH_BOUNCE", ANIM_PATH_BOUNCE},
        {"ANIM_PATH_STEP", ANIM_PATH_STEP},
        {"ANIM_PATH_CUSTOM_BEZIER3", ANIM_PATH_CUSTOM_BEZIER3}};

    for (size_t i = ANIM_PATH_NONE + 1; i < ANIM_PATH_ENUM_MAX; ++i)
    {

        lvgl_binding_set_enum(parent, enum_entries[i].name, enum_entries[i].value);
    }
}
