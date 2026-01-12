/**
 * @file lv_bindings_event.h
 * @brief 事件绑定
 * @author Sab1e
 * @date 2026-01-12
 */

#ifndef LV_BINDINGS_EVENT_H
#define LV_BINDINGS_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "jerryscript.h"
#include "lvgl.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/
void lv_bindings_event_init(jerry_value_t parent);
#ifdef __cplusplus
}
#endif

#endif /* LV_BINDINGS_EVENT_H */
