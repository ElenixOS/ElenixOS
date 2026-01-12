/**
 * @file lv_bindings_event.c
 * @brief 事件绑定
 * @author Sab1e
 * @date 2026-01-12
 */

#include "lv_bindings_event.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "lvgl_js_bridge.h"
#include "script_engine_core.h"
#include "elena_os_mem.h"
/* Macros and Definitions -------------------------------------*/

typedef struct
{
    jerry_value_t js_cb;
    jerry_value_t js_user_data;
} lv_event_js_data_t;

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

static void lv_js_event_prepare(jerry_value_t obj, lv_event_t *e)
{
    // 防止重复初始化
    jerry_value_t flag =
        jerry_object_get_sz(obj, "__inited");

    if (jerry_value_is_boolean(flag))
    {
        jerry_value_free(flag);
        return;
    }
    jerry_value_free(flag);

    jerry_value_t t = jerry_boolean(true);
    jerry_value_free(jerry_object_set_sz(obj, "__inited", t));
    jerry_value_free(t);

    jerry_value_t target =
        lv_js_bridge_ptr_2_obj(lv_event_get_target(e), LV_TYPE_OBJ);

    jerry_value_free(jerry_object_set_sz(obj, "target", target));
    jerry_value_free(target);

    jerry_value_free(jerry_object_set_sz(obj, "type",
                                         jerry_number(lv_event_get_code(e))));

    lv_event_js_data_t *data = lv_event_get_user_data(e);
    jerry_value_free(jerry_object_set_sz(obj, "user_data",
                                         jerry_value_copy(data->js_user_data)));
}

/**
 * @brief 处理 LVGL 的事件回调
 * @param e 由 LVGL 传入的事件对象
 */
static void lv_event_handler(lv_event_t *e)
{
    int event = lv_event_get_code(e);
    lv_event_js_data_t *data = lv_event_get_user_data(e);

    if (event == LV_EVENT_DELETE)
    {
        jerry_value_free(data->js_cb);
        jerry_value_free(data->js_user_data);
        eos_free(data);
        return;
    }

    // 统一桥接：lv_event_t*
    jerry_value_t event_obj =
        lv_js_bridge_ptr_2_obj(e, LV_TYPE_EVENT);

    // 设置事件属性
    lv_js_event_prepare(event_obj, e);

    static const jerry_size_t argc = 1;
    jerry_value_t args[argc] = {event_obj};

    jerry_value_t ret = jerry_call(data->js_cb,
                                   jerry_undefined(),
                                   args, argc);

    if (jerry_value_is_error(ret))
    {
        script_engine_throw_error("Callback encounter an error");
    }

    jerry_value_free(ret);
    jerry_value_free(event_obj);
}

/**
 * @brief 注册 LVGL 事件处理函数
 * @param args[0] lv_obj_t 对象
 * @param args[1] JavaScript 函数作为事件处理器
 * @param args[2] LVGL 事件类型（整数）
 * @param args[3] （可选）JS用户数据
 * @return 封装的事件描述符对象
 */
static jerry_value_t register_lv_event_handler(
    const jerry_call_info_t *call_info_p,
    const jerry_value_t args[],
    const jerry_length_t arg_cnt)
{
    if (arg_cnt < 3 ||
        !jerry_value_is_object(args[0]) ||
        !jerry_value_is_function(args[1]) ||
        !jerry_value_is_number(args[2]))
    {
        return script_engine_throw_error("Invalid arguments");
    }

    lv_obj_t *obj =
        lv_js_bridge_obj_2_ptr(args[0], LV_TYPE_OBJ);

    int event = (int)jerry_value_as_number(args[2]);

    lv_event_js_data_t *data =
        eos_malloc(sizeof(lv_event_js_data_t));

    data->js_cb = jerry_value_copy(args[1]);
    data->js_user_data =
        (arg_cnt >= 4 && !jerry_value_is_undefined(args[3]))
            ? jerry_value_copy(args[3])
            : jerry_undefined();

    lv_event_dsc_t *dsc =
        lv_obj_add_event_cb(obj, lv_event_handler, event, data);

    return lv_js_bridge_ptr_2_obj(dsc, LV_TYPE_EVENT_DSC);
}

/**
 * @brief 取消注册 LVGL 事件处理函数
 * @param args[0] lv_obj_t 对象
 * @param args[1] 封装的事件描述符对象
 * @return 无返回或抛出异常
 */
static jerry_value_t unregister_lv_event_handler(
    const jerry_call_info_t *call_info_p,
    const jerry_value_t args[],
    const jerry_length_t arg_cnt)
{
    if (arg_cnt < 2)
        return script_engine_throw_error("Invalid arguments");

    lv_obj_t *obj =
        lv_js_bridge_obj_2_ptr(args[0], LV_TYPE_OBJ);

    lv_event_dsc_t *dsc =
        lv_js_bridge_obj_2_ptr(args[1], LV_TYPE_EVENT_DSC);

    if (!obj || !dsc)
        return script_engine_throw_error("Invalid object");

    lv_event_js_data_t *data =
        lv_event_dsc_get_user_data(dsc);

    lv_obj_remove_event_dsc(obj, dsc);

    jerry_value_free(data->js_cb);
    jerry_value_free(data->js_user_data);
    eos_free(data);

    return jerry_undefined();
}

/********************************** 绑定注册 **********************************/

const script_engine_func_entry_t lvgl_binding_event_funcs[] = {
    {"obj", "addEventCb", register_lv_event_handler},
    {"obj", "removeEventCb", unregister_lv_event_handler}};

/********************************** 初始化 **********************************/

void lv_bindings_event_init(jerry_value_t parent)
{
    // 初始化函数
    script_engine_register_functions(parent,
         lvgl_binding_event_funcs,
          sizeof(lvgl_binding_event_funcs) / sizeof(script_engine_func_entry_t));

}
