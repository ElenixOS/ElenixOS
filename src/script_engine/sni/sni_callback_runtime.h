/**
 * @file sni_callback_runtime.h
 * @brief SNI 回调运行时
 * @author Sab1e
 * @date 2026-03-14
 */

#ifndef SNI_CALLBACK_RUNTIME_H
#define SNI_CALLBACK_RUNTIME_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include "jerryscript.h"
#include "lvgl.h"

/* Public function prototypes --------------------------------*/

/**
 * @brief 初始化回调运行时
 */
void sni_cb_runtime_init(void);

/**
 * @brief 注册 LVGL 事件回调（JS Function）
 * @param obj LVGL 对象
 * @param js_cb JS 回调函数
 * @param filter 事件过滤码
 * @param js_user_data JS 用户数据（可为 undefined）
 * @param out_dsc 返回的事件描述符
 * @return bool 是否成功
 */
bool sni_cb_event_add(lv_obj_t *obj,
                      jerry_value_t js_cb,
                      lv_event_code_t filter,
                      jerry_value_t js_user_data,
                      lv_event_dsc_t **out_dsc);

/**
 * @brief 按描述符移除事件回调
 * @param obj LVGL 对象
 * @param dsc 事件描述符
 * @return bool 是否成功
 */
bool sni_cb_event_remove_dsc(lv_obj_t *obj, lv_event_dsc_t *dsc);

/**
 * @brief 按 JS 回调函数移除事件回调
 * @param obj LVGL 对象
 * @param js_cb JS 回调函数
 * @return bool 是否至少移除一个回调
 */
bool sni_cb_event_remove_by_js_cb(lv_obj_t *obj, jerry_value_t js_cb);

/**
 * @brief 按 JS 回调函数和 JS user_data 移除事件回调
 * @param obj LVGL 对象
 * @param js_cb JS 回调函数
 * @param js_user_data JS 用户数据
 * @return uint32_t 移除数量
 */
uint32_t sni_cb_event_remove_by_js_cb_user_data(lv_obj_t *obj,
                                                jerry_value_t js_cb,
                                                jerry_value_t js_user_data);

/**
 * @brief 通过事件描述符清理回调上下文（用于句柄销毁回调）
 * @param dsc 事件描述符
 */
void sni_cb_event_cleanup_descriptor(lv_event_dsc_t *dsc);

/* Timer Callback API ----------------------------------------*/

/**
 * @brief 创建 LVGL 定时器并绑定 JS 回调
 * @param js_cb JS 回调函数，签名 (timer, user_data) => void
 * @param period 定时周期（毫秒）
 * @param js_user_data JS 用户数据（可为 undefined）
 * @param out_timer 返回创建的定时器指针
 * @return bool 是否成功
 */
bool sni_cb_timer_create(jerry_value_t js_cb,
                         uint32_t period,
                         jerry_value_t js_user_data,
                         lv_timer_t **out_timer);

/**
 * @brief 替换定时器的 JS 回调函数
 * @param timer LVGL 定时器
 * @param js_cb 新的 JS 回调函数
 * @return bool 是否成功
 */
bool sni_cb_timer_set_cb(lv_timer_t *timer, jerry_value_t js_cb);

/**
 * @brief 删除定时器并清理 JS 上下文
 * @param timer LVGL 定时器
 */
void sni_cb_timer_delete(lv_timer_t *timer);

/* Anim Callback API -----------------------------------------*/

typedef enum
{
    SNI_ANIM_CB_SLOT_CUSTOM_EXEC = 0,
    SNI_ANIM_CB_SLOT_START,
    SNI_ANIM_CB_SLOT_COMPLETED,
    SNI_ANIM_CB_SLOT_DELETED,
    SNI_ANIM_CB_SLOT_GET_VALUE,
    SNI_ANIM_CB_SLOT_PATH,
    SNI_ANIM_CB_SLOT_COUNT,
} sni_anim_cb_slot_t;

typedef enum
{
    SNI_ANIM_PATH_KIND_NONE = 0,
    SNI_ANIM_PATH_KIND_BUILTIN,
    SNI_ANIM_PATH_KIND_JS,
} sni_anim_path_kind_t;

typedef enum
{
    SNI_ANIM_PATH_NONE = 0,
    SNI_ANIM_PATH_LINEAR = 1,
    SNI_ANIM_PATH_EASE_IN,
    SNI_ANIM_PATH_EASE_OUT,
    SNI_ANIM_PATH_EASE_IN_OUT,
    SNI_ANIM_PATH_OVERSHOOT,
    SNI_ANIM_PATH_BOUNCE,
    SNI_ANIM_PATH_STEP,
    SNI_ANIM_PATH_CUSTOM_BEZIER3,
    SNI_ANIM_PATH_ENUM_MAX,
} sni_anim_path_builtin_t;

typedef struct sni_anim_callback_ctx
{
    lv_anim_t pre_anim;
    lv_anim_t *active_anim;
    jerry_value_t cb_slots[SNI_ANIM_CB_SLOT_COUNT];
    sni_anim_path_kind_t path_kind;
    lv_anim_path_cb_t builtin_path_fn;
    bool js_refs_freed;
    bool lvgl_alive;
} sni_anim_callback_ctx_t;

bool sni_cb_anim_create(sni_anim_callback_ctx_t **out_ctx);
bool sni_cb_anim_set_cb(sni_anim_callback_ctx_t *ctx, sni_anim_cb_slot_t slot, jerry_value_t js_cb);
bool sni_cb_anim_set_path_builtin(sni_anim_callback_ctx_t *ctx, sni_anim_path_builtin_t path_kind);
bool sni_cb_anim_set_path_js(sni_anim_callback_ctx_t *ctx, jerry_value_t js_cb);
bool sni_cb_anim_start(sni_anim_callback_ctx_t *ctx);
lv_anim_t *sni_cb_anim_get_lv_anim(sni_anim_callback_ctx_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* SNI_CALLBACK_RUNTIME_H */
