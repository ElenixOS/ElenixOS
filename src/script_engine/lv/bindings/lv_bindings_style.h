/**
 * @file lv_bindings_style.h
 * @brief 样式绑定
 * @author Sab1e
 * @date 2026-01-18
 */

#ifndef LV_BINDINGS_STYLE_H
#define LV_BINDINGS_STYLE_H

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
void lv_bindings_style_init(jerry_value_t parent);
#ifdef __cplusplus
}
#endif

#endif /* LV_BINDINGS_STYLE_H */
