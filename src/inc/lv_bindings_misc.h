
/**
 * @lv_bindings_misc.h
 * @brief 特殊 LVGL 绑定头文件
 * @author Sab1e
 * @date 2025-07-29
 */
#ifndef LV_BINDINGS_MISC_H
#define LV_BINDINGS_MISC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
#include "jerryscript.h"
// 类型声明
typedef struct {
    const char* name;
    jerry_external_handler_t handler;
} LVBindingJerryscriptFuncEntry_t;
// 函数声明
bool lv_bindings_is_js_running(void);
void lv_bindings_misc_init();
lv_color_t js_to_lv_color(jerry_value_t js_color);
jerry_value_t lv_color_to_js(lv_color_t color);
void lv_binding_jerryscript_register_functions(const LVBindingJerryscriptFuncEntry_t* entry,const size_t funcs_count);

#ifdef __cplusplus
}
#endif

#endif // LV_BINDINGS_MISC_H
