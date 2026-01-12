/**
 * @file lv_bindings_anim.h
 * @brief 动画系统绑定
 * @author Sab1e
 * @date 2025-12-28
 */

#ifndef LV_BINDINGS_ANIM_H
#define LV_BINDINGS_ANIM_H

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

typedef struct
{
    jerry_value_t this_val;
    jerry_value_t anim_obj;
    /* 没有 exec_cb，因为 JS 有闭包，无需单独获取执行对象 */
    jerry_value_t custom_exec_cb; /**< 主执行回调 */
    jerry_value_t start_cb;       /**< Call it when the animation is starts (considering `delay`)*/
    jerry_value_t completed_cb;   /**< Call it when the animation is fully completed*/
    jerry_value_t deleted_cb;     /**< Call it when the animation is deleted*/
    jerry_value_t get_value_cb;   /**< Get the current value in relative mode*/
    uint32_t path_cb_id;
    jerry_value_t path_cb;
    void *user_data;
} js_anim_user_data_t;

/* Public function prototypes --------------------------------*/
void lv_bindings_anim_custom_exec_cb_handler(lv_anim_t *a, int32_t v);
void lv_bindings_anim_completed_cb_handler(lv_anim_t *a);
void lv_bindings_anim_start_cb_handler(lv_anim_t *a);
int32_t lv_bindings_anim_get_value_cb_handler(lv_anim_t *a);
void lv_bindings_anim_deleted_cb_handler(lv_anim_t *a);
int32_t lv_bindings_anim_path_cb_hander(const lv_anim_t *a);
void lv_bindings_anim_init(jerry_value_t parent);
lv_anim_path_cb_t lv_bindings_anim_get_path_cb(uint32_t id);
#ifdef __cplusplus
}
#endif

#endif /* LV_BINDINGS_ANIM_H */
