/**
 * @file lv_bindings.h
 * @brief LVGL 绑定头文件
 * @author Sab1e
 * @date 2025-12-25
 */

#ifndef LV_BINDINGS_H
#define LV_BINDINGS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "jerryscript.h"

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

/**
 * @brief 初始化绑定层
 * @param parent 父对象
 */
void lv_binding_init(jerry_value_t parent);
#ifdef __cplusplus
}
#endif

#endif /* LV_BINDINGS_H */
