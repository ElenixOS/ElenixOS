
/**
 * @lv_bindings .h
 * @brief LVGL 绑定头文件
 * @author Sab1e
 * @date 2025-07-30
 */
#ifndef LV_BINDINGS_H
#define LV_BINDINGS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "jerryscript.h"
// 类型声明

// 函数声明
void lv_binding_init();

#ifdef __cplusplus
}
#endif

#endif // LV_BINDINGS_H
