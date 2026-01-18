/**
 * @file lv_bindings_style.c
 * @brief 样式绑定
 * @author Sab1e
 * @date 2026-01-18
 */

#include "lv_bindings_style.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define EOS_LOG_DISABLE
#define EOS_LOG_TAG "LV Bindings Style"
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

static jerry_value_t js_lv_style_create(const jerry_call_info_t *call_info_p,
                                        const jerry_value_t args[],
                                        const jerry_length_t argc)
{
    return jerry_object();
}

/**
 * @brief 样式初始化
 */
static jerry_value_t js_lv_style_init(const jerry_call_info_t *call_info_p,
                                      const jerry_value_t args[],
                                      const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 1 || !jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Usage: lv_style_init(style)");
    }

    lv_style_t *style = eos_malloc(sizeof(lv_style_t));

    // 调用初始化函数
    lv_style_init(style);

    lv_js_bridge_obj_set_ptr(args[0], style, LV_TYPE_STYLE);

    return jerry_undefined();
}

/********************************** 绑定注册 **********************************/

const script_engine_func_entry_t lvgl_binding_style_funcs[] = {
    {"style", "create", js_lv_style_create},
    {"style", "init", js_lv_style_init}};

/********************************** 初始化 **********************************/

void lv_bindings_style_init(jerry_value_t parent)
{
    // 初始化函数
    script_engine_register_functions(parent, lvgl_binding_style_funcs, sizeof(lvgl_binding_style_funcs) / sizeof(script_engine_func_entry_t));
}
