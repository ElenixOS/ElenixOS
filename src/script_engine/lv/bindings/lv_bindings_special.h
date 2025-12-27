/**
 * @file lv_bindings_special.h
 * @brief 特殊绑定层
 * @author Sab1e
 * @date 2025-12-25
 */

#ifndef LV_BINDINGS_SPECIAL_H
#define LV_BINDINGS_SPECIAL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
#include "jerryscript.h"

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

/**
 * @brief 初始化特殊绑定
 * @param parent 父对象
 */
void lv_bindings_special_init(jerry_value_t parent);
/**
 * @brief 工具函数，将 js 对象转换为 lv_color_t
 * @param js_color
 * @return lv_color_t
 */
lv_color_t js_to_lv_color(jerry_value_t js_color);
jerry_value_t lv_color_to_js(lv_color_t color);
#ifdef __cplusplus
}
#endif

#endif /* LV_BINDINGS_SPECIAL_H */
