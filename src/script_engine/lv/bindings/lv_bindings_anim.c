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

    // 检查参数类型
    if (!jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Argument 0 must be an object");
    }

    if (!jerry_value_is_function(args[1]))
    {
        return script_engine_throw_error("Argument 1 must be a function");
    }

    // 直接设置 JS 对象属性
    jerry_value_t key = jerry_string_sz("completedCb");
    jerry_value_t val = jerry_value_copy(args[1]);
    jerry_value_free(jerry_object_set(args[0], key, val));
    jerry_value_free(val);
    jerry_value_free(key);

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

    // 检查参数类型
    if (!jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Argument 0 must be an object");
    }

    if (!jerry_value_is_function(args[1]))
    {
        return script_engine_throw_error("Argument 1 must be a function");
    }

    // 直接设置 JS 对象属性
    jerry_value_t key = jerry_string_sz("customExecCb");
    jerry_value_t val = jerry_value_copy(args[1]);
    jerry_value_free(jerry_object_set(args[0], key, val));
    jerry_value_free(val);
    jerry_value_free(key);

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

    // 检查参数类型
    if (!jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Argument 0 must be an object");
    }

    if (!jerry_value_is_function(args[1]))
    {
        return script_engine_throw_error("Argument 1 must be a function");
    }

    // 直接设置 JS 对象属性
    jerry_value_t key = jerry_string_sz("deletedCb");
    jerry_value_t val = jerry_value_copy(args[1]);
    jerry_value_free(jerry_object_set(args[0], key, val));
    jerry_value_free(val);
    jerry_value_free(key);

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

    // 检查参数类型
    if (!jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Argument 0 must be an object");
    }

    if (!jerry_value_is_function(args[1]))
    {
        return script_engine_throw_error("Argument 1 must be a function");
    }

    // 直接设置 JS 对象属性
    jerry_value_t key = jerry_string_sz("getValue");
    jerry_value_t val = jerry_value_copy(args[1]);
    jerry_value_free(jerry_object_set(args[0], key, val));
    jerry_value_free(val);
    jerry_value_free(key);

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

    // 检查参数类型
    if (!jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Argument 0 must be an object");
    }

    // 直接设置 JS 对象属性
    jerry_value_t key = jerry_string_sz("pathCb");
    jerry_value_t val = jerry_value_copy(args[1]);
    jerry_value_free(jerry_object_set(args[0], key, val));
    jerry_value_free(val);
    jerry_value_free(key);

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

    // 检查参数类型
    if (!jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Argument 0 must be an object");
    }

    if (!jerry_value_is_function(args[1]))
    {
        return script_engine_throw_error("Argument 1 must be a function");
    }

    // 直接设置 JS 对象属性
    jerry_value_t key = jerry_string_sz("startCb");
    jerry_value_t val = jerry_value_copy(args[1]);
    jerry_value_free(jerry_object_set(args[0], key, val));
    jerry_value_free(val);
    jerry_value_free(key);

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

    // 检查参数类型
    if (!jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Argument 0 must be an object");
    }

    // 直接设置 JS 对象属性
    jerry_value_t key = jerry_string_sz("userData");
    jerry_value_t val = jerry_value_copy(args[1]);
    jerry_value_free(jerry_object_set(args[0], key, val));
    jerry_value_free(val);
    jerry_value_free(key);

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

    // 检查参数类型
    if (!jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Argument 0 must be an object");
    }

    // 直接获取 JS 对象属性
    jerry_value_t key = jerry_string_sz("userData");
    jerry_value_t val = jerry_object_get(args[0], key);
    jerry_value_free(key);

    // 如果属性不存在，返回 null
    if (jerry_value_is_undefined(val))
    {
        jerry_value_free(val);
        return jerry_null();
    }

    return val;
}

static jerry_value_t js_lv_anim_set_var(const jerry_call_info_t *call_info_p,
                                        const jerry_value_t args[],
                                        const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 2)
    {
        return script_engine_throw_error("lv_anim_set_var: Insufficient arguments");
    }

    // 检查参数类型
    if (!jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Argument 0 must be an object");
    }

    // 直接设置 JS 对象属性
    jerry_value_t key = jerry_string_sz("var");
    jerry_value_t val = jerry_value_copy(args[1]);
    jerry_value_free(jerry_object_set(args[0], key, val));
    jerry_value_free(val);
    jerry_value_free(key);

    return jerry_undefined();
}

static jerry_value_t js_lv_anim_set_exec_cb(const jerry_call_info_t *call_info_p,
                                           const jerry_value_t args[],
                                           const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 2)
    {
        return script_engine_throw_error("lv_anim_set_exec_cb: Insufficient arguments");
    }

    // 检查参数类型
    if (!jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Argument 0 must be an object");
    }

    if (!jerry_value_is_function(args[1]))
    {
        return script_engine_throw_error("Argument 1 must be a function");
    }

    // 直接设置 JS 对象属性
    jerry_value_t key = jerry_string_sz("execCb");
    jerry_value_t val = jerry_value_copy(args[1]);
    jerry_value_free(jerry_object_set(args[0], key, val));
    jerry_value_free(val);
    jerry_value_free(key);

    return jerry_undefined();
}

static jerry_value_t js_lv_anim_set_duration(const jerry_call_info_t *call_info_p,
                                            const jerry_value_t args[],
                                            const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 2)
    {
        return script_engine_throw_error("lv_anim_set_duration: Insufficient arguments");
    }

    // 检查参数类型
    if (!jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Argument 0 must be an object");
    }

    if (!jerry_value_is_number(args[1]))
    {
        return script_engine_throw_error("Argument 1 must be a number");
    }

    // 直接设置 JS 对象属性
    jerry_value_t key = jerry_string_sz("duration");
    jerry_value_t val = jerry_value_copy(args[1]);
    jerry_value_free(jerry_object_set(args[0], key, val));
    jerry_value_free(val);
    jerry_value_free(key);

    return jerry_undefined();
}

static jerry_value_t js_lv_anim_set_time(const jerry_call_info_t *call_info_p,
                                         const jerry_value_t args[],
                                         const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 2)
    {
        return script_engine_throw_error("lv_anim_set_time: Insufficient arguments");
    }

    // 检查参数类型
    if (!jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Argument 0 must be an object");
    }

    if (!jerry_value_is_number(args[1]))
    {
        return script_engine_throw_error("Argument 1 must be a number");
    }

    // 直接设置 JS 对象属性
    jerry_value_t key = jerry_string_sz("actTime");
    jerry_value_t val = jerry_value_copy(args[1]);
    jerry_value_free(jerry_object_set(args[0], key, val));
    jerry_value_free(val);
    jerry_value_free(key);

    return jerry_undefined();
}

static jerry_value_t js_lv_anim_set_delay(const jerry_call_info_t *call_info_p,
                                          const jerry_value_t args[],
                                          const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 2)
    {
        return script_engine_throw_error("lv_anim_set_delay: Insufficient arguments");
    }

    // 检查参数类型
    if (!jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Argument 0 must be an object");
    }

    if (!jerry_value_is_number(args[1]))
    {
        return script_engine_throw_error("Argument 1 must be a number");
    }

    // 直接设置 JS 对象属性
    jerry_value_t key = jerry_string_sz("delay");
    jerry_value_t val = jerry_value_copy(args[1]);
    jerry_value_free(jerry_object_set(args[0], key, val));
    jerry_value_free(val);
    jerry_value_free(key);

    return jerry_undefined();
}

static jerry_value_t js_lv_anim_set_values(const jerry_call_info_t *call_info_p,
                                          const jerry_value_t args[],
                                          const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 3)
    {
        return script_engine_throw_error("lv_anim_set_values: Insufficient arguments");
    }

    // 检查参数类型
    if (!jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Argument 0 must be an object");
    }

    if (!jerry_value_is_number(args[1]) || !jerry_value_is_number(args[2]))
    {
        return script_engine_throw_error("Arguments 1 and 2 must be numbers");
    }

    // 直接设置 JS 对象属性
    jerry_value_t key = jerry_string_sz("startValue");
    jerry_value_t val = jerry_value_copy(args[1]);
    jerry_value_free(jerry_object_set(args[0], key, val));
    jerry_value_free(val);
    jerry_value_free(key);

    key = jerry_string_sz("endValue");
    val = jerry_value_copy(args[2]);
    jerry_value_free(jerry_object_set(args[0], key, val));
    jerry_value_free(val);
    jerry_value_free(key);

    return jerry_undefined();
}

static jerry_value_t js_lv_anim_set_playback_duration(const jerry_call_info_t *call_info_p,
                                                     const jerry_value_t args[],
                                                     const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 2)
    {
        return script_engine_throw_error("lv_anim_set_playback_duration: Insufficient arguments");
    }

    // 检查参数类型
    if (!jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Argument 0 must be an object");
    }

    if (!jerry_value_is_number(args[1]))
    {
        return script_engine_throw_error("Argument 1 must be a number");
    }

    // 直接设置 JS 对象属性
    jerry_value_t key = jerry_string_sz("playbackDuration");
    jerry_value_t val = jerry_value_copy(args[1]);
    jerry_value_free(jerry_object_set(args[0], key, val));
    jerry_value_free(val);
    jerry_value_free(key);

    return jerry_undefined();
}

static jerry_value_t js_lv_anim_set_playback_time(const jerry_call_info_t *call_info_p,
                                                 const jerry_value_t args[],
                                                 const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 2)
    {
        return script_engine_throw_error("lv_anim_set_playback_time: Insufficient arguments");
    }

    // 检查参数类型
    if (!jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Argument 0 must be an object");
    }

    if (!jerry_value_is_number(args[1]))
    {
        return script_engine_throw_error("Argument 1 must be a number");
    }

    // 直接设置 JS 对象属性
    jerry_value_t key = jerry_string_sz("playbackDuration");
    jerry_value_t val = jerry_value_copy(args[1]);
    jerry_value_free(jerry_object_set(args[0], key, val));
    jerry_value_free(val);
    jerry_value_free(key);

    return jerry_undefined();
}

static jerry_value_t js_lv_anim_set_playback_delay(const jerry_call_info_t *call_info_p,
                                                  const jerry_value_t args[],
                                                  const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 2)
    {
        return script_engine_throw_error("lv_anim_set_playback_delay: Insufficient arguments");
    }

    // 检查参数类型
    if (!jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Argument 0 must be an object");
    }

    if (!jerry_value_is_number(args[1]))
    {
        return script_engine_throw_error("Argument 1 must be a number");
    }

    // 直接设置 JS 对象属性
    jerry_value_t key = jerry_string_sz("playbackDelay");
    jerry_value_t val = jerry_value_copy(args[1]);
    jerry_value_free(jerry_object_set(args[0], key, val));
    jerry_value_free(val);
    jerry_value_free(key);

    return jerry_undefined();
}

static jerry_value_t js_lv_anim_set_repeat_count(const jerry_call_info_t *call_info_p,
                                                const jerry_value_t args[],
                                                const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 2)
    {
        return script_engine_throw_error("lv_anim_set_repeat_count: Insufficient arguments");
    }

    // 检查参数类型
    if (!jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Argument 0 must be an object");
    }

    if (!jerry_value_is_number(args[1]))
    {
        return script_engine_throw_error("Argument 1 must be a number");
    }

    // 直接设置 JS 对象属性
    jerry_value_t key = jerry_string_sz("repeatCnt");
    jerry_value_t val = jerry_value_copy(args[1]);
    jerry_value_free(jerry_object_set(args[0], key, val));
    jerry_value_free(val);
    jerry_value_free(key);

    return jerry_undefined();
}

static jerry_value_t js_lv_anim_set_repeat_delay(const jerry_call_info_t *call_info_p,
                                                const jerry_value_t args[],
                                                const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 2)
    {
        return script_engine_throw_error("lv_anim_set_repeat_delay: Insufficient arguments");
    }

    // 检查参数类型
    if (!jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Argument 0 must be an object");
    }

    if (!jerry_value_is_number(args[1]))
    {
        return script_engine_throw_error("Argument 1 must be a number");
    }

    // 直接设置 JS 对象属性
    jerry_value_t key = jerry_string_sz("repeatDelay");
    jerry_value_t val = jerry_value_copy(args[1]);
    jerry_value_free(jerry_object_set(args[0], key, val));
    jerry_value_free(val);
    jerry_value_free(key);

    return jerry_undefined();
}

static jerry_value_t js_lv_anim_set_early_apply(const jerry_call_info_t *call_info_p,
                                               const jerry_value_t args[],
                                               const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 2)
    {
        return script_engine_throw_error("lv_anim_set_early_apply: Insufficient arguments");
    }

    // 检查参数类型
    if (!jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Argument 0 must be an object");
    }

    // 直接设置 JS 对象属性
    jerry_value_t key = jerry_string_sz("earlyApply");
    jerry_value_t val = jerry_boolean(jerry_value_is_true(args[1]));
    jerry_value_free(jerry_object_set(args[0], key, val));
    jerry_value_free(val);
    jerry_value_free(key);

    return jerry_undefined();
}

static jerry_value_t js_lv_anim_set_bezier3_param(const jerry_call_info_t *call_info_p,
                                                 const jerry_value_t args[],
                                                 const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 5)
    {
        return script_engine_throw_error("lv_anim_set_bezier3_param: Insufficient arguments");
    }

    // 检查参数类型
    if (!jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Argument 0 must be an object");
    }

    if (!jerry_value_is_number(args[1]) || !jerry_value_is_number(args[2]) ||
        !jerry_value_is_number(args[3]) || !jerry_value_is_number(args[4]))
    {
        return script_engine_throw_error("Arguments 1-4 must be numbers");
    }

    // 获取或创建 bezier3 对象
    jerry_value_t bezier3_key = jerry_string_sz("bezier3");
    jerry_value_t bezier3_obj = jerry_object_get(args[0], bezier3_key);

    if (jerry_value_is_undefined(bezier3_obj))
    {
        jerry_value_free(bezier3_obj);
        bezier3_obj = jerry_object();
        jerry_value_free(jerry_object_set(args[0], bezier3_key, bezier3_obj));
    }

    jerry_value_free(bezier3_key);

    // 设置 bezier3 参数
    jerry_value_t key = jerry_string_sz("x1");
    jerry_value_t val = jerry_value_copy(args[1]);
    jerry_value_free(jerry_object_set(bezier3_obj, key, val));
    jerry_value_free(val);
    jerry_value_free(key);

    key = jerry_string_sz("y1");
    val = jerry_value_copy(args[2]);
    jerry_value_free(jerry_object_set(bezier3_obj, key, val));
    jerry_value_free(val);
    jerry_value_free(key);

    key = jerry_string_sz("x2");
    val = jerry_value_copy(args[3]);
    jerry_value_free(jerry_object_set(bezier3_obj, key, val));
    jerry_value_free(val);
    jerry_value_free(key);

    key = jerry_string_sz("y2");
    val = jerry_value_copy(args[4]);
    jerry_value_free(jerry_object_set(bezier3_obj, key, val));
    jerry_value_free(val);
    jerry_value_free(key);

    jerry_value_free(bezier3_obj);

    return jerry_undefined();
}

static jerry_value_t js_lv_anim_get_delay(const jerry_call_info_t *call_info_p,
                                         const jerry_value_t args[],
                                         const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 1)
    {
        return script_engine_throw_error("lv_anim_get_delay: Insufficient arguments");
    }

    // 检查参数类型
    if (!jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Argument 0 must be an object");
    }

    // 直接获取 JS 对象属性
    jerry_value_t key = jerry_string_sz("delay");
    jerry_value_t val = jerry_object_get(args[0], key);
    jerry_value_free(key);

    // 如果属性不存在，返回 0
    if (jerry_value_is_undefined(val))
    {
        jerry_value_free(val);
        return jerry_number(0);
    }

    return val;
}

static jerry_value_t js_lv_anim_get_playtime(const jerry_call_info_t *call_info_p,
                                            const jerry_value_t args[],
                                            const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 1)
    {
        return script_engine_throw_error("lv_anim_get_playtime: Insufficient arguments");
    }

    // 检查参数类型
    if (!jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Argument 0 must be an object");
    }

    // 直接获取 JS 对象属性
    jerry_value_t key = jerry_string_sz("duration");
    jerry_value_t val = jerry_object_get(args[0], key);
    jerry_value_free(key);

    // 如果属性不存在，返回 0
    if (jerry_value_is_undefined(val))
    {
        jerry_value_free(val);
        return jerry_number(0);
    }

    return val;
}

static jerry_value_t js_lv_anim_get_time(const jerry_call_info_t *call_info_p,
                                        const jerry_value_t args[],
                                        const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 1)
    {
        return script_engine_throw_error("lv_anim_get_time: Insufficient arguments");
    }

    // 检查参数类型
    if (!jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Argument 0 must be an object");
    }

    // 直接获取 JS 对象属性
    jerry_value_t key = jerry_string_sz("actTime");
    jerry_value_t val = jerry_object_get(args[0], key);
    jerry_value_free(key);

    // 如果属性不存在，返回 0
    if (jerry_value_is_undefined(val))
    {
        jerry_value_free(val);
        return jerry_number(0);
    }

    return val;
}

static jerry_value_t js_lv_anim_get_repeat_count(const jerry_call_info_t *call_info_p,
                                               const jerry_value_t args[],
                                               const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 1)
    {
        return script_engine_throw_error("lv_anim_get_repeat_count: Insufficient arguments");
    }

    // 检查参数类型
    if (!jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Argument 0 must be an object");
    }

    // 直接获取 JS 对象属性
    jerry_value_t key = jerry_string_sz("repeatCnt");
    jerry_value_t val = jerry_object_get(args[0], key);
    jerry_value_free(key);

    // 如果属性不存在，返回 0
    if (jerry_value_is_undefined(val))
    {
        jerry_value_free(val);
        return jerry_number(0);
    }

    return val;
}

static jerry_value_t js_lv_anim_delete(const jerry_call_info_t *call_info_p,
                                      const jerry_value_t args[],
                                      const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 1)
    {
        return script_engine_throw_error("lv_anim_delete: Insufficient arguments");
    }

    // 检查参数类型
    if (!jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Argument 0 must be an object");
    }

    // 这里简化处理，实际删除动画的逻辑由底层处理
    return jerry_undefined();
}

static jerry_value_t js_lv_anim_delete_all(const jerry_call_info_t *call_info_p,
                                          const jerry_value_t args[],
                                          const jerry_length_t argc)
{
    // 调用底层函数删除所有动画
    lv_anim_delete_all();

    return jerry_undefined();
}

static jerry_value_t js_lv_anim_get(const jerry_call_info_t *call_info_p,
                                   const jerry_value_t args[],
                                   const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 1)
    {
        return script_engine_throw_error("lv_anim_get: Insufficient arguments");
    }

    // 简化处理，返回 null
    return jerry_null();
}

static jerry_value_t js_lv_anim_get_timer(const jerry_call_info_t *call_info_p,
                                         const jerry_value_t args[],
                                         const jerry_length_t argc)
{
    // 调用底层函数获取动画定时器
    lv_timer_t *timer = lv_anim_get_timer();

    // 转换为 JS 对象
    if (timer)
    {
        return lv_js_bridge_ptr_2_obj(timer, LV_TYPE_TIMER);
    }
    else
    {
        return jerry_null();
    }
}

static jerry_value_t js_lv_anim_custom_delete(const jerry_call_info_t *call_info_p,
                                             const jerry_value_t args[],
                                             const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 2)
    {
        return script_engine_throw_error("lv_anim_custom_delete: Insufficient arguments");
    }

    // 简化处理
    return jerry_undefined();
}

static jerry_value_t js_lv_anim_custom_get(const jerry_call_info_t *call_info_p,
                                          const jerry_value_t args[],
                                          const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 2)
    {
        return script_engine_throw_error("lv_anim_custom_get: Insufficient arguments");
    }

    // 简化处理，返回 null
    return jerry_null();
}

static jerry_value_t js_lv_anim_count_running(const jerry_call_info_t *call_info_p,
                                             const jerry_value_t args[],
                                             const jerry_length_t argc)
{
    // 调用底层函数获取运行中的动画数量
    uint32_t count = lv_anim_count_running();
    return jerry_number(count);
}

static jerry_value_t js_lv_anim_speed(const jerry_call_info_t *call_info_p,
                                     const jerry_value_t args[],
                                     const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 1)
    {
        return script_engine_throw_error("lv_anim_speed: Insufficient arguments");
    }

    // 检查参数类型
    if (!jerry_value_is_number(args[0]))
    {
        return script_engine_throw_error("Arguments must be numbers");
    }

    // 调用底层函数计算动画速度
    uint32_t speed = lv_anim_speed(jerry_value_as_number(args[0]));
    return jerry_number(speed);
}

static jerry_value_t js_lv_anim_speed_clamped(const jerry_call_info_t *call_info_p,
                                             const jerry_value_t args[],
                                             const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 3)
    {
        return script_engine_throw_error("lv_anim_speed_clamped: Insufficient arguments");
    }

    // 检查参数类型
    if (!jerry_value_is_number(args[0]) || !jerry_value_is_number(args[1]) || !jerry_value_is_number(args[2]))
    {
        return script_engine_throw_error("Arguments must be numbers");
    }

    // 调用底层函数计算动画速度（带限制）
    uint32_t speed = lv_anim_speed_clamped(jerry_value_as_number(args[0]),
                                           jerry_value_as_number(args[1]),
                                           jerry_value_as_number(args[2]));
    return jerry_number(speed);
}

static jerry_value_t js_lv_anim_speed_to_time(const jerry_call_info_t *call_info_p,
                                             const jerry_value_t args[],
                                             const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 3)
    {
        return script_engine_throw_error("lv_anim_speed_to_time: Insufficient arguments");
    }

    // 检查参数类型
    if (!jerry_value_is_number(args[0]) || !jerry_value_is_number(args[1]) || !jerry_value_is_number(args[2]))
    {
        return script_engine_throw_error("Arguments must be numbers");
    }

    // 调用底层函数将速度转换为时间
    uint32_t time = lv_anim_speed_to_time(jerry_value_as_number(args[0]),
                                         jerry_value_as_number(args[1]),
                                         jerry_value_as_number(args[2]));
    return jerry_number(time);
}

static jerry_value_t js_lv_anim_refr_now(const jerry_call_info_t *call_info_p,
                                         const jerry_value_t args[],
                                         const jerry_length_t argc)
{
    // 调用底层函数立即刷新动画
    lv_anim_refr_now();

    return jerry_undefined();
}

static jerry_value_t js_lv_anim_path_linear(const jerry_call_info_t *call_info_p,
                                           const jerry_value_t args[],
                                           const jerry_length_t argc)
{
    // 返回线性路径 ID
    return jerry_number(ANIM_PATH_LINEAR);
}

static jerry_value_t js_lv_anim_path_ease_in(const jerry_call_info_t *call_info_p,
                                            const jerry_value_t args[],
                                            const jerry_length_t argc)
{
    // 返回 ease-in 路径 ID
    return jerry_number(ANIM_PATH_EASE_IN);
}

static jerry_value_t js_lv_anim_path_ease_out(const jerry_call_info_t *call_info_p,
                                             const jerry_value_t args[],
                                             const jerry_length_t argc)
{
    // 返回 ease-out 路径 ID
    return jerry_number(ANIM_PATH_EASE_OUT);
}

static jerry_value_t js_lv_anim_path_ease_in_out(const jerry_call_info_t *call_info_p,
                                                const jerry_value_t args[],
                                                const jerry_length_t argc)
{
    // 返回 ease-in-out 路径 ID
    return jerry_number(ANIM_PATH_EASE_IN_OUT);
}

static jerry_value_t js_lv_anim_path_overshoot(const jerry_call_info_t *call_info_p,
                                              const jerry_value_t args[],
                                              const jerry_length_t argc)
{
    // 返回 overshoot 路径 ID
    return jerry_number(ANIM_PATH_OVERSHOOT);
}

static jerry_value_t js_lv_anim_path_bounce(const jerry_call_info_t *call_info_p,
                                           const jerry_value_t args[],
                                           const jerry_length_t argc)
{
    // 返回 bounce 路径 ID
    return jerry_number(ANIM_PATH_BOUNCE);
}

static jerry_value_t js_lv_anim_path_step(const jerry_call_info_t *call_info_p,
                                         const jerry_value_t args[],
                                         const jerry_length_t argc)
{
    // 返回 step 路径 ID
    return jerry_number(ANIM_PATH_STEP);
}

static jerry_value_t js_lv_anim_path_custom_bezier3(const jerry_call_info_t *call_info_p,
                                                  const jerry_value_t args[],
                                                  const jerry_length_t argc)
{
    // 返回自定义 bezier3 路径 ID
    return jerry_number(ANIM_PATH_CUSTOM_BEZIER3);
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

jerry_value_t js_lv_anim_init(const jerry_call_info_t *info,
                              const jerry_value_t args[],
                              const jerry_length_t argc)
{
    return jerry_undefined();
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
    {"anim", "init", js_lv_anim_init},
    {"anim", "start", js_lv_anim_start},
    {"anim", "setVar", js_lv_anim_set_var},
    {"anim", "setExecCb", js_lv_anim_set_exec_cb},
    {"anim", "setDuration", js_lv_anim_set_duration},
    {"anim", "setTime", js_lv_anim_set_time},
    {"anim", "setDelay", js_lv_anim_set_delay},
    {"anim", "setValues", js_lv_anim_set_values},
    {"anim", "setPlaybackDuration", js_lv_anim_set_playback_duration},
    {"anim", "setPlaybackTime", js_lv_anim_set_playback_time},
    {"anim", "setPlaybackDelay", js_lv_anim_set_playback_delay},
    {"anim", "setRepeatCount", js_lv_anim_set_repeat_count},
    {"anim", "setRepeatDelay", js_lv_anim_set_repeat_delay},
    {"anim", "setEarlyApply", js_lv_anim_set_early_apply},
    {"anim", "setBezier3Param", js_lv_anim_set_bezier3_param},
    {"anim", "getDelay", js_lv_anim_get_delay},
    {"anim", "getPlaytime", js_lv_anim_get_playtime},
    {"anim", "getTime", js_lv_anim_get_time},
    {"anim", "getRepeatCount", js_lv_anim_get_repeat_count},
    {"anim", "delete", js_lv_anim_delete},
    {"anim", "deleteAll", js_lv_anim_delete_all},
    {"anim", "get", js_lv_anim_get},
    {"anim", "getTimer", js_lv_anim_get_timer},
    {"anim", "customDelete", js_lv_anim_custom_delete},
    {"anim", "customGet", js_lv_anim_custom_get},
    {"anim", "countRunning", js_lv_anim_count_running},
    {"anim", "speed", js_lv_anim_speed},
    {"anim", "speedClamped", js_lv_anim_speed_clamped},
    {"anim", "speedToTime", js_lv_anim_speed_to_time},
    {"anim", "refrNow", js_lv_anim_refr_now},
    {"anim", "pathLinear", js_lv_anim_path_linear},
    {"anim", "pathEaseIn", js_lv_anim_path_ease_in},
    {"anim", "pathEaseOut", js_lv_anim_path_ease_out},
    {"anim", "pathEaseInOut", js_lv_anim_path_ease_in_out},
    {"anim", "pathOvershoot", js_lv_anim_path_overshoot},
    {"anim", "pathBounce", js_lv_anim_path_bounce},
    {"anim", "pathStep", js_lv_anim_path_step},
    {"anim", "pathCustomBezier3", js_lv_anim_path_custom_bezier3}};

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
