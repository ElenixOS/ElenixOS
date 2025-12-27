/**
 * @file lv_bindings_special.c
 * @brief 特殊绑定层
 * @author Sab1e
 * @date 2025-12-25
 */

#include "lv_bindings_special.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define EOS_LOG_DISABLE
#define EOS_LOG_TAG "LV Bindings Special"
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

/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/
/********************************** 回调系统 **********************************/
typedef struct
{
    jerry_value_t js_cb;        // JS回调函数
    jerry_value_t js_user_data; // JS用户数据
} lv_event_js_data_t;


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
    jerry_object_set_sz(obj, "__inited", t);
    jerry_value_free(t);

    jerry_value_t target =
        lv_js_bridge_ptr_2_obj(lv_event_get_target(e), LV_TYPE_OBJ);

    jerry_object_set_sz(obj, "target", target);
    jerry_value_free(target);

    jerry_object_set_sz(obj, "type",
                        jerry_number(lv_event_get_code(e)));

    lv_event_js_data_t *data = lv_event_get_user_data(e);
    jerry_object_set_sz(obj, "user_data",
                        jerry_value_copy(data->js_user_data));
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

    // ★ 统一桥接：lv_event_t*
    jerry_value_t event_obj =
        lv_js_bridge_ptr_2_obj(e, LV_TYPE_EVENT);

    // ★ 设置事件属性（一次性，或懒设置）
    lv_js_event_prepare(event_obj, e);

    jerry_value_t args[1] = {event_obj};

    jerry_value_t ret = jerry_call(data->js_cb,
                                   jerry_undefined(),
                                   args, 1);

    if (jerry_value_is_error(ret))
    {
        EOS_LOG_E("Callback encounter an error");
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

/********************************** 定时器系统 **********************************/

// 定时器数据结构
typedef struct
{
    lv_timer_t *timer;
    jerry_value_t js_cb;
    jerry_value_t user_data;
} timer_js_data_t;

/**
 * @brief LVGL 定时器回调包装函数
 * @param timer LVGL 定时器对象
 */
static void lv_timer_js_cb(lv_timer_t *timer)
{
    timer_js_data_t *data = (timer_js_data_t *)lv_timer_get_user_data(timer);

    if (data && !jerry_value_is_undefined(data->js_cb))
    {
        jerry_value_t args[1] = {data->user_data};

        jerry_value_t ret = jerry_call(data->js_cb, script_engine_lv_get_root(), args, 1);
        if (jerry_value_is_error(ret))
        {
            EOS_LOG_E("Timer callback encounter an error");
        }
        jerry_value_free(ret);
    }
}

static void _lv_timer_auto_delete_cb(lv_event_t *e)
{
    EOS_LOG_I("Timer del cb");
    lv_timer_t *timer = lv_event_get_user_data(e);
    if (!timer)
    {
        return;
    }
    lv_timer_delete(timer);
    EOS_LOG_I("Timer auto deleted");
}

/**
 * @brief 创建 LVGL 定时器
 * @param args[0] JavaScript 函数作为定时器回调
 * @param args[1] 定时器周期（毫秒）
 * @param args[2] （可选）用户数据
 * @return 定时器对象或抛出异常
 */
static jerry_value_t js_lv_timer_create(const jerry_call_info_t *call_info_p,
                                        const jerry_value_t args[],
                                        const jerry_length_t arg_cnt)
{
    if (arg_cnt < 2 || !jerry_value_is_function(args[0]) || !jerry_value_is_number(args[1]))
    {
        return script_engine_throw_error("Invalid arguments");
    }

    // 获取定时器周期
    uint32_t period = (uint32_t)jerry_value_as_number(args[1]);

    // 准备用户数据
    jerry_value_t user_data = jerry_undefined();
    if (arg_cnt >= 3)
    {
        user_data = jerry_value_copy(args[2]);
    }

    // 创建 JavaScript 回调的副本
    jerry_value_t js_cb = jerry_value_copy(args[0]);

    // 创建定时器数据结构
    timer_js_data_t *timer_data = (timer_js_data_t *)eos_malloc(sizeof(timer_js_data_t));
    if (!timer_data)
    {
        jerry_value_free(js_cb);
        jerry_value_free(user_data);
        return script_engine_throw_error("Failed to allocate memory for timer data");
    }

    timer_data->js_cb = js_cb;
    timer_data->user_data = user_data;

    // 创建 LVGL 定时器
    lv_timer_t *timer = lv_timer_create(lv_timer_js_cb, period, timer_data);
    if (!timer)
    {
        eos_free(timer_data);
        return script_engine_throw_error("Failed to create timer");
    }

    // 绑定到根页面以便实现自动删除
    if (script_engine_get_current_script_type() == SCRIPT_TYPE_APPLICATION)
    {
        lv_obj_t *home_screen = eos_nav_get_home_screen();
        if (!home_screen)
        {
            return script_engine_throw_error("Home screen is NULL");
        }
        lv_obj_add_event_cb(home_screen, _lv_timer_auto_delete_cb, LV_EVENT_DELETE, (void *)timer);
        EOS_LOG_I("App timer auto delete registered");
    }
    else if (script_engine_get_current_script_type() == SCRIPT_TYPE_WATCHFACE)
    {
        lv_obj_t *home_screen = eos_watchface_get_screen();
        if (!home_screen)
        {
            return script_engine_throw_error("Watchface screen is NULL");
        }
        lv_obj_add_event_cb(home_screen, _lv_timer_auto_delete_cb, LV_EVENT_DELETE, (void *)timer);
        EOS_LOG_I("Screen[%p]Watchface timer auto delete registered", eos_watchface_get_screen());
    }

    timer_data->timer = timer;

    // 创建 JavaScript 定时器对象
    jerry_value_t js_timer = jerry_object();

    // 添加指针属性
    jerry_value_t ptr_prop = jerry_string_sz("__ptr");
    jerry_value_t ptr_val = jerry_number((uintptr_t)timer);
    jerry_value_free(jerry_object_set(js_timer, ptr_prop, ptr_val));
    jerry_value_free(ptr_prop);
    jerry_value_free(ptr_val);

    // 添加类型标记
    jerry_value_t type_prop = jerry_string_sz("__type");
    jerry_value_t type_val = jerry_string_sz("lv_timer");
    jerry_value_free(jerry_object_set(js_timer, type_prop, type_val));
    jerry_value_free(type_prop);
    jerry_value_free(type_val);

    return js_timer;
}

/**
 * @brief 删除 LVGL 定时器
 * @param args[0] 定时器对象
 * @return 无返回或抛出异常
 */
static jerry_value_t js_lv_timer_delete(const jerry_call_info_t *call_info_p,
                                        const jerry_value_t args[],
                                        const jerry_length_t arg_cnt)
{
    if (arg_cnt < 1 || !jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Invalid arguments");
    }

    // 获取定时器指针
    jerry_value_t ptr_prop = jerry_string_sz("__ptr");
    jerry_value_t ptr_val = jerry_object_get(args[0], ptr_prop);
    jerry_value_free(ptr_prop);

    if (!jerry_value_is_number(ptr_val))
    {
        jerry_value_free(ptr_val);
        return script_engine_throw_error("Invalid timer object");
    }

    lv_timer_t *timer = (lv_timer_t *)(uintptr_t)jerry_value_as_number(ptr_val);
    jerry_value_free(ptr_val);

    // 获取定时器数据
    timer_js_data_t *timer_data = (timer_js_data_t *)lv_timer_get_user_data(timer);

    // 释放 JavaScript 资源
    if (timer_data)
    {
        jerry_value_free(timer_data->js_cb);
        jerry_value_free(timer_data->user_data);
        eos_free(timer_data);
    }

    // 删除定时器
    lv_timer_delete(timer);

    return jerry_undefined();
}

/**
 * @brief 设置定时器周期
 * @param args[0] 定时器对象
 * @param args[1] 新的周期（毫秒）
 * @return 无返回或抛出异常
 */
static jerry_value_t js_lv_timer_set_period(const jerry_call_info_t *call_info_p,
                                            const jerry_value_t args[],
                                            const jerry_length_t arg_cnt)
{
    if (arg_cnt < 2 || !jerry_value_is_object(args[0]) || !jerry_value_is_number(args[1]))
    {
        return script_engine_throw_error("Invalid arguments");
    }

    // 获取定时器指针
    jerry_value_t ptr_prop = jerry_string_sz("__ptr");
    jerry_value_t ptr_val = jerry_object_get(args[0], ptr_prop);
    jerry_value_free(ptr_prop);

    if (!jerry_value_is_number(ptr_val))
    {
        jerry_value_free(ptr_val);
        return script_engine_throw_error("Invalid timer object");
    }

    lv_timer_t *timer = (lv_timer_t *)(uintptr_t)jerry_value_as_number(ptr_val);
    jerry_value_free(ptr_val);

    // 设置定时器周期
    uint32_t period = (uint32_t)jerry_value_as_number(args[1]);
    lv_timer_set_period(timer, period);

    return jerry_undefined();
}

/**
 * @brief 设置定时器重复次数
 * @param args[0] 定时器对象
 * @param args[1] 重复次数（-1表示无限重复）
 * @return 无返回或抛出异常
 */
static jerry_value_t js_lv_timer_set_repeat_count(const jerry_call_info_t *call_info_p,
                                                  const jerry_value_t args[],
                                                  const jerry_length_t arg_cnt)
{
    if (arg_cnt < 2 || !jerry_value_is_object(args[0]) || !jerry_value_is_number(args[1]))
    {
        return script_engine_throw_error("Invalid arguments");
    }

    // 获取定时器指针
    jerry_value_t ptr_prop = jerry_string_sz("__ptr");
    jerry_value_t ptr_val = jerry_object_get(args[0], ptr_prop);
    jerry_value_free(ptr_prop);

    if (!jerry_value_is_number(ptr_val))
    {
        jerry_value_free(ptr_val);
        return script_engine_throw_error("Invalid timer object");
    }

    lv_timer_t *timer = (lv_timer_t *)(uintptr_t)jerry_value_as_number(ptr_val);
    jerry_value_free(ptr_val);

    // 设置定时器重复次数
    int32_t repeat_count = (int32_t)jerry_value_as_number(args[1]);
    lv_timer_set_repeat_count(timer, repeat_count);

    return jerry_undefined();
}

/**
 * @brief 重置定时器
 * @param args[0] 定时器对象
 * @return 无返回或抛出异常
 */
static jerry_value_t js_lv_timer_reset(const jerry_call_info_t *call_info_p,
                                       const jerry_value_t args[],
                                       const jerry_length_t arg_cnt)
{
    if (arg_cnt < 1 || !jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Invalid arguments");
    }

    // 获取定时器指针
    jerry_value_t ptr_prop = jerry_string_sz("__ptr");
    jerry_value_t ptr_val = jerry_object_get(args[0], ptr_prop);
    jerry_value_free(ptr_prop);

    if (!jerry_value_is_number(ptr_val))
    {
        jerry_value_free(ptr_val);
        return script_engine_throw_error("Invalid timer object");
    }

    lv_timer_t *timer = (lv_timer_t *)(uintptr_t)jerry_value_as_number(ptr_val);
    jerry_value_free(ptr_val);

    // 重置定时器
    lv_timer_reset(timer);

    return jerry_undefined();
}

/********************************** 色彩转换函数 **********************************/
/**
 * @brief 解析JS颜色值到lv_color_t
 */
lv_color_t js_to_lv_color(jerry_value_t js_color)
{
    lv_color_t color = {0}; // 初始化为黑色

    // 处理undefined/null
    if (jerry_value_is_undefined(js_color) || jerry_value_is_null(js_color))
    {
        return color;
    }

    // 获取RGB值
    uint8_t r = 0, g = 0, b = 0;

    // 处理数字输入（直接作为hex值）
    if (jerry_value_is_number(js_color))
    {
        uint32_t hex = (uint32_t)jerry_value_as_number(js_color);
        r = (hex >> 16) & 0xFF;
        g = (hex >> 8) & 0xFF;
        b = hex & 0xFF;
    }
    // 处理对象输入
    else if (jerry_value_is_object(js_color))
    {
        // 优先检查hex属性
        jerry_value_t hex_val = jerry_object_get(js_color, jerry_string_sz("hex"));
        if (jerry_value_is_number(hex_val))
        {
            uint32_t hex = (uint32_t)jerry_value_as_number(hex_val);
            r = (hex >> 16) & 0xFF;
            g = (hex >> 8) & 0xFF;
            b = hex & 0xFF;
        }
        jerry_value_free(hex_val);

        // 其次检查rgb属性
        if (r == 0 && g == 0 && b == 0)
        {
            jerry_value_t r_val = jerry_object_get(js_color, jerry_string_sz("r"));
            jerry_value_t g_val = jerry_object_get(js_color, jerry_string_sz("g"));
            jerry_value_t b_val = jerry_object_get(js_color, jerry_string_sz("b"));

            if (jerry_value_is_number(r_val))
                r = (uint8_t)jerry_value_as_number(r_val);
            if (jerry_value_is_number(g_val))
                g = (uint8_t)jerry_value_as_number(g_val);
            if (jerry_value_is_number(b_val))
                b = (uint8_t)jerry_value_as_number(b_val);

            jerry_value_free(r_val);
            jerry_value_free(g_val);
            jerry_value_free(b_val);
        }
    }

    // 填充颜色结构体（注意顺序：blue, green, red）
    color.blue = b;
    color.green = g;
    color.red = r;

    return color;
}

/**
 * @brief 将lv_color_t转换为JS对象
 */
jerry_value_t lv_color_to_js(lv_color_t color)
{
    jerry_value_t js_color = jerry_object();

    // 添加RGB分量（注意顺序与结构体相反）
    jerry_value_free(jerry_object_set(js_color, jerry_string_sz("r"), jerry_number(color.red)));
    jerry_value_free(jerry_object_set(js_color, jerry_string_sz("g"), jerry_number(color.green)));
    jerry_value_free(jerry_object_set(js_color, jerry_string_sz("b"), jerry_number(color.blue)));

    // 添加十六进制颜色值
    uint32_t hex = (color.red << 16) | (color.green << 8) | color.blue;
    jerry_value_free(jerry_object_set(js_color, jerry_string_sz("hex"), jerry_number(hex)));

    // 标记为LVGL颜色对象
    jerry_value_free(jerry_object_set(js_color, jerry_string_sz("__type"), jerry_string_sz("lv_color")));

    return js_color;
}
/********************************** 特殊 LVGL 函数 **********************************/
/**
 * @brief 样式初始化
 */
static jerry_value_t js_lv_style_init(const jerry_call_info_t *call_info_p,
                                      const jerry_value_t args[],
                                      const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 1)
    {
        return script_engine_throw_error("Insufficient arguments");
    }

    // 检查参数是否为对象
    if (!jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Argument must be a style object");
    }

    // 检查对象是否已经分配了内存
    jerry_value_t ptr_prop = jerry_string_sz("__ptr");
    jerry_value_t ptr_val = jerry_object_get(args[0], ptr_prop);
    jerry_value_free(ptr_prop);

    lv_style_t *style = NULL;

    if (jerry_value_is_number(ptr_val))
    {
        // 已有指针的情况
        uintptr_t ptr = (uintptr_t)jerry_value_as_number(ptr_val);
        style = (lv_style_t *)ptr;
    }
    else
    {
        // 没有指针的情况，分配新内存
        style = (lv_style_t *)eos_malloc(sizeof(lv_style_t));
        if (!style)
        {
            jerry_value_free(ptr_val);
            return script_engine_throw_error("Failed to allocate memory for style");
        }

        // 将指针保存回JS对象
        ptr_val = jerry_number((uintptr_t)style);
        ptr_prop = jerry_string_sz("__ptr");
        jerry_value_free(jerry_object_set(args[0], ptr_prop, ptr_val));
        jerry_value_free(ptr_prop);
        jerry_value_free(ptr_val);

        // 添加类型标记
        jerry_value_t type_prop = jerry_string_sz("__type");
        jerry_value_t type_val = jerry_string_sz("lv_style");
        jerry_value_free(jerry_object_set(args[0], type_prop, type_val));
        jerry_value_free(type_prop);
        jerry_value_free(type_val);
    }

    // 调用初始化函数
    lv_style_init(style);

    // 返回JS对象本身，支持链式调用
    return jerry_value_copy(args[0]);
}
/**
 * @brief 样式清除
 */
static jerry_value_t js_lv_style_delete(const jerry_call_info_t *call_info_p,
                                        const jerry_value_t args[],
                                        const jerry_length_t argc)
{
    if (argc < 1 || !jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Invalid arguments");
    }

    jerry_value_t ptr_prop = jerry_string_sz("__ptr");
    jerry_value_t ptr_val = jerry_object_get(args[0], ptr_prop);
    jerry_value_free(ptr_prop);

    if (jerry_value_is_number(ptr_val))
    {
        lv_style_t *style = (lv_style_t *)(uintptr_t)jerry_value_as_number(ptr_val);
        eos_free(style);

        // 清除指针引用
        ptr_prop = jerry_string_sz("__ptr");
        jerry_value_free(jerry_object_set(args[0], ptr_prop, jerry_number(0)));
        jerry_value_free(ptr_prop);
    }

    jerry_value_free(ptr_val);
    return jerry_undefined();
}

/********************************** 字体系统 **********************************/
static void register_lvgl_fonts(jerry_value_t parent)
{
    // 创建字体对象容器
    jerry_value_t fonts = jerry_object();

    // 注册Montserrat字体集
#define REGISTER_FONT(name)                                               \
    do                                                                    \
    {                                                                     \
        jerry_value_t font_obj = jerry_object();                          \
                                                                          \
        jerry_value_t key_ptr = jerry_string_sz("__ptr");                 \
        jerry_value_t key_type = jerry_string_sz("__type");               \
        jerry_value_t key_name = jerry_string_sz(#name);                  \
                                                                          \
        jerry_value_t val_ptr = jerry_number((uintptr_t)&name);           \
        jerry_value_t val_type = jerry_string_sz("lv_font");              \
                                                                          \
        jerry_value_free(jerry_object_set(font_obj, key_ptr, val_ptr));   \
        jerry_value_free(jerry_object_set(font_obj, key_type, val_type)); \
        jerry_value_free(jerry_object_set(fonts, key_name, font_obj));    \
                                                                          \
        jerry_value_free(key_ptr);                                        \
        jerry_value_free(key_type);                                       \
        jerry_value_free(key_name);                                       \
        jerry_value_free(val_ptr);                                        \
        jerry_value_free(val_type);                                       \
        jerry_value_free(font_obj);                                       \
    } while (0)
#if LV_FONT_MONTSERRAT_8
    REGISTER_FONT(lv_font_montserrat_8);
#endif
#if LV_FONT_MONTSERRAT_10
    REGISTER_FONT(lv_font_montserrat_10);
#endif
#if LV_FONT_MONTSERRAT_12
    REGISTER_FONT(lv_font_montserrat_12);
#endif
#if LV_FONT_MONTSERRAT_14
    REGISTER_FONT(lv_font_montserrat_14);
#endif
#if LV_FONT_MONTSERRAT_16
    REGISTER_FONT(lv_font_montserrat_16);
#endif
#if LV_FONT_MONTSERRAT_18
    REGISTER_FONT(lv_font_montserrat_18);
#endif
#if LV_FONT_MONTSERRAT_20
    REGISTER_FONT(lv_font_montserrat_20);
#endif
#if LV_FONT_MONTSERRAT_22
    REGISTER_FONT(lv_font_montserrat_22);
#endif
#if LV_FONT_MONTSERRAT_24
    REGISTER_FONT(lv_font_montserrat_24);
#endif
#if LV_FONT_MONTSERRAT_26
    REGISTER_FONT(lv_font_montserrat_26);
#endif
#if LV_FONT_MONTSERRAT_28
    REGISTER_FONT(lv_font_montserrat_28);
#endif
#if LV_FONT_MONTSERRAT_30
    REGISTER_FONT(lv_font_montserrat_30);
#endif
#if LV_FONT_MONTSERRAT_32
    REGISTER_FONT(lv_font_montserrat_32);
#endif
#if LV_FONT_MONTSERRAT_34
    REGISTER_FONT(lv_font_montserrat_34);
#endif
#if LV_FONT_MONTSERRAT_36
    REGISTER_FONT(lv_font_montserrat_36);
#endif
#if LV_FONT_MONTSERRAT_38
    REGISTER_FONT(lv_font_montserrat_38);
#endif
#if LV_FONT_MONTSERRAT_40
    REGISTER_FONT(lv_font_montserrat_40);
#endif
#if LV_FONT_MONTSERRAT_42
    REGISTER_FONT(lv_font_montserrat_42);
#endif
#if LV_FONT_MONTSERRAT_44
    REGISTER_FONT(lv_font_montserrat_44);
#endif
#if LV_FONT_MONTSERRAT_46
    REGISTER_FONT(lv_font_montserrat_46);
#endif
#if LV_FONT_MONTSERRAT_48
    REGISTER_FONT(lv_font_montserrat_48);
#endif

#undef REGISTER_FONT

    // 将字体容器挂载到全局对象
    jerry_value_t font_key = jerry_string_sz("lv_font");
    jerry_value_free(jerry_object_set(parent, font_key, fonts));
    jerry_value_free(font_key);
    jerry_value_free(fonts);
}

static jerry_value_t js_lv_tiny_ttf_create_file(const jerry_call_info_t *call_info_p,
                                                const jerry_value_t args[],
                                                const jerry_length_t argc)
{
#if LV_TINY_TTF_FILE_SUPPORT
    // 参数数量检查
    if (argc < 2)
    {
        return script_engine_throw_error("Insufficient arguments");
    }

    // 解析参数: src (const char*)
    char *arg_src_str = NULL;
    const char *arg_src = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0]))
    {
        if (!jerry_value_is_string(args[0]))
        {
            return script_engine_throw_error("Argument 0 must be a string");
        }
        jerry_size_t arg_src_len = jerry_string_size(args[0], JERRY_ENCODING_UTF8);
        arg_src_str = (char *)eos_malloc(arg_src_len + 0);
        jerry_string_to_buffer(args[0], JERRY_ENCODING_UTF8, (jerry_char_t *)arg_src_str, arg_src_len);
        arg_src_str[arg_src_len] = '\0';
        arg_src = arg_src_str;
    }

    // 解析参数: font_size (uint32_t)
    if (!jerry_value_is_number(args[1]))
    {
        if (arg_src_str)
            eos_free(arg_src_str);
        return script_engine_throw_error("Argument 1 must be a number");
    }
    uint32_t font_size = (uint32_t)jerry_value_as_number(args[1]);

    char font_path[PATH_MAX];
    if (script_engine_get_current_script_id() == NULL)
    {
        if (arg_src_str)
            eos_free(arg_src_str);
        return script_engine_throw_error("Script package info is NULL");
    }

    if (script_engine_get_current_script_type() == SCRIPT_TYPE_APPLICATION)
    {
        snprintf(font_path, sizeof(font_path), EOS_APP_INSTALLED_DIR "%s/assets/%s",
                 script_engine_get_current_script_id(), arg_src);
    }
    else if (script_engine_get_current_script_type() == SCRIPT_TYPE_WATCHFACE)
    {
        snprintf(font_path, sizeof(font_path), EOS_WATCHFACE_INSTALLED_DIR "%s/assets/%s",
                 script_engine_get_current_script_id(), arg_src);
    }
    else
    {
        if (arg_src_str)
            eos_free(arg_src_str);
        return script_engine_throw_error("Unknown script type");
    }

    if (!eos_is_file(font_path))
    {
        if (arg_src_str)
            eos_free(arg_src_str);
        return script_engine_throw_error("Font file not found");
    }

    EOS_LOG_D("Font Path: %s", font_path);

    // 调用底层函数创建字体
    lv_font_t *font = lv_tiny_ttf_create_file(font_path, font_size);

    // 释放临时字符串内存
    if (arg_src_str)
        eos_free(arg_src_str);

    if (!font)
    {
        return script_engine_throw_error("Failed to create font");
    }

    // 包装为LVGL字体对象返回
    jerry_value_t js_result = lv_js_bridge_ptr_2_obj(font, LV_TYPE_FONT);

    return js_result;
#else
    return script_engine_throw_error("LV_TINY_TTF_FILE_SUPPORT is not enabled");
#endif
}

/********************************** 绑定注册 **********************************/

const script_engine_func_entry_t lvgl_binding_special_funcs[] = {
    {"obj", "addEventCb", register_lv_event_handler},
    {"obj", "removeEventCb", unregister_lv_event_handler},
    {"style", "init", js_lv_style_init},
    {"style", "delete", js_lv_style_delete},
    {"timer", "create", js_lv_timer_create},
    {"timer", "delete", js_lv_timer_delete},
    {"timer", "setPeriod", js_lv_timer_set_period},
    {"timer", "setRepeatCount", js_lv_timer_set_repeat_count},
    {"timer", "reset", js_lv_timer_reset},
    {"tinyTTF","createFile", js_lv_tiny_ttf_create_file}};

/********************************** 初始化 **********************************/

void lv_bindings_special_init(jerry_value_t parent)
{
    // 初始化函数
    script_engine_register_functions(parent, lvgl_binding_special_funcs, sizeof(lvgl_binding_special_funcs) / sizeof(script_engine_func_entry_t));
    register_lvgl_fonts(parent);
}
