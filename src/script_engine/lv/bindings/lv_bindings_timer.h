/**
 * @file lv_bindings_timer.h
 * @brief 定时器绑定
 * @author Sab1e
 * @date 2026-01-10
 */

#ifndef LV_BINDINGS_TIMER_H
#define LV_BINDINGS_TIMER_H

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
 * @brief 初始化定时器绑定
 */
void lv_bindings_timer_init(jerry_value_t parent);
#ifdef __cplusplus
}
#endif

#endif /* LV_BINDINGS_TIMER_H */
