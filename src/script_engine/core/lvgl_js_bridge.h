/**
 * @file lvgl_js_bridge.h
 * @brief LVGL 与 JS 的桥接层
 *
 * LVGL <=[ BRIDGE ]=> JerryScript
 *
 * @author Sab1e
 * @date 2025-12-21
 */

#ifndef LVGL_JS_BRIDGE_H
#define LVGL_JS_BRIDGE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "jerryscript.h"

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

typedef enum{
    LV_TYPE_UNKNWON,
    LV_TYPE_COMMON_PTR,
    LV_TYPE_OBJ,
    LV_TYPE_FONT,
    LV_TYPE_EVENT,
    LV_TYPE_EVENT_DSC
}lv_type_t;

/* Public function prototypes --------------------------------*/

/**
 * @brief 将 js 对象转换为指针
 * @param obj 目标 js 对象
 * @param type 指定此对象类型，并确定指针类型
 * @return void* 转换成功则返回指定类型的指针，否则返回 NULL
 */
void *lv_js_bridge_obj_2_ptr(const jerry_value_t obj, lv_type_t type);
/**
 * @brief 将指针转换为 js 对象
 * @param ptr 目标指针
 * @param type 指定此指针的类型
 * @return jerry_value_t 转换成功则返回 js 对象，否则返回`jerry_null()`
 */
jerry_value_t lv_js_bridge_ptr_2_obj(void *ptr, lv_type_t type);
/**
 * @brief 获取指定对象的类型
 * @param obj 目标对象
 * @return lv_type_t 获取成功则返回对应类型，否则返回`LV_TYPE_UNKNOWN`
 */
lv_type_t lv_js_bridge_obj_get_type(const jerry_value_t obj);
#ifdef __cplusplus
}
#endif

#endif /* LVGL_JS_BRIDGE_H */
