/**
 * @file lv_bindings_timer.c
 * @brief 定时器绑定
 * @author Sab1e
 * @date 2026-01-10
 */

#include "lv_bindings_timer.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #define EOS_LOG_DISABLE
#define EOS_LOG_TAG "LV Bindings Timer"
#include "elena_os_log.h"
#include "lv_bindings.h"
#include "elena_os_port.h"
#include "elena_os_mem.h"
#include "lvgl_js_bridge.h"
#include "script_engine_lv.h"
#include "script_engine_core.h"
#include "lvgl_private.h"

/* Macros and Definitions -------------------------------------*/

typedef struct
{
    lv_timer_t *timer;
    jerry_value_t js_cb;
    jerry_value_t user_data;
} timer_js_data_t;

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

static void _lv_timer_auto_delete_cb(lv_event_t *e);

void _lv_timer_dump(lv_timer_t *timer)
{
    if (!timer)
    {
        EOS_LOG_W("lv_timer is NULL");
        return;
    }

    EOS_LOG_I("lv_timer dump begin ----------------");
    EOS_LOG_I("System tick  : %u", lv_tick_get());
    EOS_LOG_I("period       : %u", timer->period);
    EOS_LOG_I("last_run     : %u", timer->last_run);
    EOS_LOG_I("timer_cb     : %p", (void *)timer->timer_cb);
    EOS_LOG_I("user_data    : %p", timer->user_data);
    EOS_LOG_I("repeat_count : %d", timer->repeat_count);
    EOS_LOG_I("paused       : %u", timer->paused);
    EOS_LOG_I("auto_delete  : %u", timer->auto_delete);
    EOS_LOG_I("lv_timer dump end ------------------");
}

/**
 * @brief LVGL 定时器回调包装函数
 * @param timer LVGL 定时器对象
 */
static void lv_timer_js_cb(lv_timer_t *timer)
{
    timer_js_data_t *data = lv_timer_get_user_data(timer);

    if (data && !jerry_value_is_undefined(data->js_cb))
    {
        jerry_value_t args[1] = {data->user_data};

        jerry_value_t ret = jerry_call(data->js_cb, jerry_null(), args, 1);
        if (jerry_value_is_error(ret))
        {
            EOS_LOG_E("Timer callback encounter an error");
        }
        jerry_value_free(ret);
    }

    // 当定时器开启自动删除并且 repeat_count 减到 0 时就会被删除
    if (timer->auto_delete && timer->repeat_count == 0)
    {
        eos_event_remove_global_cb_with_user_data(
            EOS_EVENT_SCRIPT_EXITED,
            _lv_timer_auto_delete_cb,
            timer);
    }
}

static void _lv_timer_auto_delete_cb(lv_event_t *e)
{
    lv_timer_t *timer = lv_event_get_user_data(e);
    if (!timer)
        return;
    timer_js_data_t *data = (timer_js_data_t *)lv_timer_get_user_data(timer);
    if (data)
    {
        if (!jerry_value_is_undefined(data->js_cb))
            jerry_value_free(data->js_cb);
        eos_free(data);
    }
    lv_timer_delete(timer);
    eos_event_remove_global_cb_with_user_data(
        EOS_EVENT_SCRIPT_EXITED,
        _lv_timer_auto_delete_cb,
        timer);
}

static timer_js_data_t *eos_timer_prepare_js_cb(const jerry_value_t js_cb,
                                                timer_js_data_t *reuse)
{
    // js_cb 必须是函数
    if (!jerry_value_is_function(js_cb))
        return NULL;

    timer_js_data_t *data = reuse;

    // 如果没有旧数据，则新建
    if (!data)
    {
        data = (timer_js_data_t *)eos_malloc(sizeof(timer_js_data_t));
        if (!data)
            return NULL;

        data->js_cb = jerry_undefined();
        data->timer = NULL;
    }

    // 释放旧 JS 回调
    if (!jerry_value_is_undefined(data->js_cb))
    {
        jerry_value_free(data->js_cb);
        data->js_cb = jerry_undefined();
    }

    // 保存新的 JS 回调
    data->js_cb = jerry_value_copy(js_cb);

    return data;
}

static jerry_value_t js_lv_timer_create(const jerry_call_info_t *call_info_p,
                                        const jerry_value_t args[],
                                        const jerry_length_t argc)
{
    if (argc < 2 || !jerry_value_is_function(args[0]) || !jerry_value_is_number(args[1]))
    {
        return script_engine_throw_error("Invalid arguments");
    }

    uint32_t period = (uint32_t)jerry_value_as_number(args[1]);

    // 准备 timer_js_data，仅绑定 JS 回调
    timer_js_data_t *timer_data =
        eos_timer_prepare_js_cb(args[0], NULL);

    if (!timer_data)
    {
        return script_engine_throw_error("Failed to prepare timer callback");
    }

    // 创建 LVGL timer，user_data 指向 timer_js_data_t
    lv_timer_t *timer = lv_timer_create(lv_timer_js_cb, period, timer_data);
    if (!timer)
    {
        jerry_value_free(timer_data->js_cb);
        eos_free(timer_data);
        return script_engine_throw_error("Failed to create timer");
    }

    // 绑定自动清理
    eos_event_add_global_cb(_lv_timer_auto_delete_cb,
                            EOS_EVENT_SCRIPT_EXITED,
                            timer);

    timer_data->timer = timer;

    return lv_js_bridge_ptr_2_obj(timer, LV_TYPE_TIMER);
}

static jerry_value_t js_lv_timer_set_cb(const jerry_call_info_t *call_info_p,
                                        const jerry_value_t args[],
                                        const jerry_length_t argc)
{
    if (argc < 2 || !jerry_value_is_function(args[1]))
    {
        return script_engine_throw_error("lv_timer_set_cb: Invalid arguments");
    }

    // 解析 timer
    lv_timer_t *timer =
        (lv_timer_t *)lv_js_bridge_obj_2_ptr(args[0], LV_TYPE_TIMER);

    if (!timer)
    {
        return script_engine_throw_error("lv_timer_set_cb: Invalid timer");
    }

    // 取出现有的 timer_js_data
    timer_js_data_t *data =
        (timer_js_data_t *)lv_timer_get_user_data(timer);

    // 更新 JS 回调
    data = eos_timer_prepare_js_cb(args[1], data);
    if (!data)
    {
        return script_engine_throw_error("lv_timer_set_cb: Failed to set callback");
    }

    // 确保 user_data 正确（理论上 reuse 时不变，但这里显式设置更安全）
    lv_timer_set_user_data(timer, data);

    // 强制使用桥接回调
    lv_timer_set_cb(timer, lv_timer_js_cb);

    return jerry_undefined();
}

/**
 * @brief Create an "empty" timer. It needs to be initialized with at least lv_timer_set_cb and lv_timer_set_period  pointer to the created timer
 */
static jerry_value_t js_lv_timer_create_basic(const jerry_call_info_t *call_info_p,
                                              const jerry_value_t args[],
                                              const jerry_length_t argc)
{
    // 调用底层函数
    lv_timer_t *ret_value = lv_timer_create_basic();

    // 处理返回值
    jerry_value_t js_result;
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_TIMER);

    return js_result;
}

/**
 * @brief Set custom parameter to the lv_timer.
 */
static jerry_value_t js_lv_timer_set_user_data(const jerry_call_info_t *call_info_p,
                                               const jerry_value_t args[],
                                               const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 2)
    {
        return script_engine_throw_error("lv_timer_set_user_data: Insufficient arguments");
    }

    // 解析参数: timer (lv_timer_t*)
    // 对象类型参数，支持null
    lv_timer_t *arg_timer = (lv_timer_t *)lv_js_bridge_obj_2_ptr(args[0], LV_TYPE_TIMER);

    // 获取timer_js_data结构体
    timer_js_data_t *timer_data = (timer_js_data_t *)lv_timer_get_user_data(arg_timer);
    if (!timer_data)
    {
        // 创建timer_js_data结构体
        timer_data = eos_malloc(sizeof(timer_js_data_t));
    }
    else
    {
        // 释放之前的user_data
        if (!jerry_value_is_undefined(timer_data->user_data))
        {
            jerry_value_free(timer_data->user_data);
        }
    }

    // 保存新的user_data
    timer_data->user_data = jerry_value_copy(args[1]);

    return jerry_undefined();
}

/**
 * @brief Get the user_data passed when the timer was created pointer to the user_data
 */
static jerry_value_t js_lv_timer_get_user_data(const jerry_call_info_t *call_info_p,
                                               const jerry_value_t args[],
                                               const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 1)
    {
        return script_engine_throw_error("lv_timer_get_user_data: Insufficient arguments");
    }

    // 解析参数: timer (lv_timer_t*)
    // 对象类型参数，支持null
    lv_timer_t *arg_timer = (lv_timer_t *)lv_js_bridge_obj_2_ptr(args[0], LV_TYPE_TIMER);

    // 获取timer_js_data结构体
    timer_js_data_t *timer_data = (timer_js_data_t *)lv_timer_get_user_data(arg_timer);
    if (!timer_data)
    {
        return script_engine_throw_error("Failed to get timer user data structure");
    }

    // 返回保存的user_data
    return jerry_value_copy(timer_data->user_data);
}

/********************************** 绑定注册 **********************************/

const script_engine_func_entry_t lvgl_binding_timer_funcs[] = {
    {"timer", "createBasic", js_lv_timer_create_basic},
    {"timer", "create", js_lv_timer_create},
    {"timer", "setCb", js_lv_timer_set_cb},
    {"timer", "setUserData", js_lv_timer_set_user_data},
    {"timer", "getUserData", js_lv_timer_get_user_data}};

/********************************** 初始化 **********************************/

void lv_bindings_timer_init(jerry_value_t parent)
{
    // 初始化函数
    script_engine_register_functions(parent, lvgl_binding_timer_funcs, sizeof(lvgl_binding_timer_funcs) / sizeof(script_engine_func_entry_t));
}
