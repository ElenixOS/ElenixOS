/**
 * @file sni_callback_runtime.c
 * @brief SNI 回调运行时
 * @author Sab1e
 * @date 2026-03-14
 */

#include "sni_callback_runtime.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#define EOS_LOG_TAG "SNI Callback Runtime"
#include "elena_os_mem.h"
#include "elena_os_log.h"
#include "sni_api_export.h"
#include "sni_type_bridge.h"
#include "sni_types.h"
#include "uthash.h"

/* Macros and Definitions -------------------------------------*/
typedef struct sni_event_callback_ctx
{
    lv_obj_t *owner;
    lv_event_dsc_t *dsc;
    jerry_value_t js_cb;
    jerry_value_t js_user_data;
    bool alive;
    UT_hash_handle hh;
} sni_event_callback_ctx_t;

typedef struct sni_timer_callback_ctx
{
    lv_timer_t *timer;
    jerry_value_t js_cb;
    jerry_value_t js_user_data;
    bool alive;
    UT_hash_handle hh;
} sni_timer_callback_ctx_t;

/* Variables --------------------------------------------------*/
static sni_event_callback_ctx_t *s_event_ctx_map = NULL;
static sni_timer_callback_ctx_t *s_timer_ctx_map = NULL;

/* Function Implementations -----------------------------------*/

static bool sni_cb_js_strict_equal(jerry_value_t lhs, jerry_value_t rhs)
{
    jerry_value_t eq = jerry_binary_op(JERRY_BIN_OP_STRICT_EQUAL, lhs, rhs);
    if (jerry_value_is_exception(eq))
    {
        jerry_value_free(eq);
        return false;
    }

    bool result = jerry_value_is_true(eq);
    jerry_value_free(eq);
    return result;
}

static void sni_cb_event_prepare_js_event(jerry_value_t event_obj, lv_event_t *e, jerry_value_t js_user_data)
{
    jerry_value_t flag = jerry_object_get_sz(event_obj, "__inited");
    if (jerry_value_is_boolean(flag))
    {
        jerry_value_free(flag);
        return;
    }
    jerry_value_free(flag);

    jerry_value_t inited = jerry_boolean(true);
    jerry_value_free(jerry_object_set_sz(event_obj, "__inited", inited));
    jerry_value_free(inited);

    lv_obj_t *target_ptr = lv_event_get_target(e);
    jerry_value_t js_target = sni_tb_c2js(&target_ptr, SNI_H_LV_OBJ);
    jerry_value_free(jerry_object_set_sz(event_obj, "target", js_target));
    jerry_value_free(js_target);

    jerry_value_t js_type = jerry_number((double)lv_event_get_code(e));
    jerry_value_free(jerry_object_set_sz(event_obj, "type", js_type));
    jerry_value_free(js_type);

    jerry_value_t copied_user_data = jerry_value_copy(js_user_data);
    jerry_value_free(jerry_object_set_sz(event_obj, "user_data", copied_user_data));
    jerry_value_free(copied_user_data);
}

static void sni_cb_event_free_ctx(sni_event_callback_ctx_t *ctx)
{
    if (!ctx || !ctx->alive)
    {
        return;
    }

    ctx->alive = false;

    if (ctx->dsc)
    {
        HASH_DEL(s_event_ctx_map, ctx);
    }

    jerry_value_free(ctx->js_cb);
    jerry_value_free(ctx->js_user_data);
    eos_free(ctx);
}

static void sni_cb_event_dispatch(lv_event_t *e)
{
    sni_event_callback_ctx_t *ctx = (sni_event_callback_ctx_t *)lv_event_get_user_data(e);
    if (!ctx || !ctx->alive)
    {
        return;
    }

    if (lv_event_get_code(e) == LV_EVENT_DELETE)
    {
        sni_cb_event_free_ctx(ctx);
        return;
    }

    lv_event_t *event_ptr = e;
    jerry_value_t event_obj = sni_tb_c2js(&event_ptr, SNI_H_LV_EVENT);
    sni_cb_event_prepare_js_event(event_obj, e, ctx->js_user_data);

    jerry_value_t args[1] = {event_obj};
    jerry_value_t ret = jerry_call(ctx->js_cb, jerry_undefined(), args, 1);

    if (jerry_value_is_error(ret) || jerry_value_is_exception(ret))
    {
        EOS_LOG_E("Event callback encounter an error");
    }

    jerry_value_free(ret);
    jerry_value_free(event_obj);
}

void sni_cb_event_cleanup_descriptor(lv_event_dsc_t *dsc)
{
    if (!dsc)
    {
        return;
    }

    sni_event_callback_ctx_t *ctx = NULL;
    HASH_FIND_PTR(s_event_ctx_map, &dsc, ctx);
    if (!ctx)
    {
        return;
    }

    sni_cb_event_free_ctx(ctx);
}

static void sni_cb_event_dsc_destroy_cb(void *native_ptr)
{
    lv_event_dsc_t *dsc = (lv_event_dsc_t *)native_ptr;
    sni_cb_event_cleanup_descriptor(dsc);
}

static void sni_cb_timer_handle_destroy_cb(void *native_ptr); /* forward declaration */

void sni_cb_runtime_init(void)
{
    sni_tb_register_handle_destroy_cb(SNI_H_LV_EVENT_DSC, sni_cb_event_dsc_destroy_cb);
    sni_tb_register_handle_destroy_cb(SNI_H_LV_TIMER, sni_cb_timer_handle_destroy_cb);
}

/* Timer Callback Implementation ------------------------------*/

static void sni_cb_timer_dispatch(lv_timer_t *t)
{
    sni_timer_callback_ctx_t *ctx = (sni_timer_callback_ctx_t *)lv_timer_get_user_data(t);
    if (!ctx || !ctx->alive)
    {
        return;
    }

    lv_timer_t *timer_ptr = t;
    jerry_value_t js_timer = sni_tb_c2js(&timer_ptr, SNI_H_LV_TIMER);
    jerry_value_t args[2] = {js_timer, ctx->js_user_data};
    jerry_value_t ret = jerry_call(ctx->js_cb, jerry_undefined(), args, 2);

    if (jerry_value_is_error(ret) || jerry_value_is_exception(ret))
    {
        EOS_LOG_E("Timer callback encountered an error");
    }

    jerry_value_free(ret);
    jerry_value_free(js_timer);
}

static void sni_cb_timer_cleanup_impl(sni_timer_callback_ctx_t *ctx)
{
    if (!ctx || !ctx->alive)
    {
        return;
    }

    ctx->alive = false;
    HASH_DEL(s_timer_ctx_map, ctx);
    jerry_value_free(ctx->js_cb);
    jerry_value_free(ctx->js_user_data);
    eos_free(ctx);
}

static void sni_cb_timer_handle_destroy_cb(void *native_ptr)
{
    lv_timer_t *timer = (lv_timer_t *)native_ptr;
    sni_timer_callback_ctx_t *ctx = NULL;
    HASH_FIND_PTR(s_timer_ctx_map, &timer, ctx);
    if (ctx)
    {
        /* ctx 还在 hash 中，说明用户未主动删除，这里负责完整清理 */
        sni_cb_timer_cleanup_impl(ctx);
        lv_timer_delete(timer);
    }
    /* ctx 不在 hash 中说明已通过 sni_cb_timer_delete 显式清理，跳过 */
}


bool sni_cb_event_add(lv_obj_t *obj,
                      jerry_value_t js_cb,
                      lv_event_code_t filter,
                      jerry_value_t js_user_data,
                      lv_event_dsc_t **out_dsc)
{
    if (!obj || !out_dsc || !jerry_value_is_function(js_cb))
    {
        return false;
    }

    sni_event_callback_ctx_t *ctx = eos_malloc_zeroed(sizeof(sni_event_callback_ctx_t));
    if (!ctx)
    {
        return false;
    }

    ctx->owner = obj;
    ctx->js_cb = jerry_value_copy(js_cb);
    ctx->js_user_data = jerry_value_copy(js_user_data);
    ctx->alive = true;

    lv_event_dsc_t *dsc = lv_obj_add_event_cb(obj, sni_cb_event_dispatch, filter, ctx);
    if (!dsc)
    {
        jerry_value_free(ctx->js_cb);
        jerry_value_free(ctx->js_user_data);
        eos_free(ctx);
        return false;
    }

    ctx->dsc = dsc;
    HASH_ADD_PTR(s_event_ctx_map, dsc, ctx);

    *out_dsc = dsc;
    return true;
}

bool sni_cb_event_remove_dsc(lv_obj_t *obj, lv_event_dsc_t *dsc)
{
    if (!obj || !dsc)
    {
        return false;
    }

    bool removed = lv_obj_remove_event_dsc(obj, dsc);
    sni_cb_event_cleanup_descriptor(dsc);
    return removed;
}

bool sni_cb_event_remove_by_js_cb(lv_obj_t *obj, jerry_value_t js_cb)
{
    if (!obj || !jerry_value_is_function(js_cb))
    {
        return false;
    }

    bool removed_any = false;
    sni_event_callback_ctx_t *ctx = NULL;
    sni_event_callback_ctx_t *tmp = NULL;

    HASH_ITER(hh, s_event_ctx_map, ctx, tmp)
    {
        if (ctx->owner != obj)
        {
            continue;
        }

        if (!sni_cb_js_strict_equal(ctx->js_cb, js_cb))
        {
            continue;
        }

        lv_obj_remove_event_dsc(obj, ctx->dsc);
        sni_cb_event_free_ctx(ctx);
        removed_any = true;
    }

    return removed_any;
}

uint32_t sni_cb_event_remove_by_js_cb_user_data(lv_obj_t *obj,
                                                jerry_value_t js_cb,
                                                jerry_value_t js_user_data)
{
    if (!obj || !jerry_value_is_function(js_cb))
    {
        return 0;
    }

    uint32_t removed = 0;
    sni_event_callback_ctx_t *ctx = NULL;
    sni_event_callback_ctx_t *tmp = NULL;

    HASH_ITER(hh, s_event_ctx_map, ctx, tmp)
    {
        if (ctx->owner != obj)
        {
            continue;
        }

        if (!sni_cb_js_strict_equal(ctx->js_cb, js_cb))
        {
            continue;
        }

        if (!sni_cb_js_strict_equal(ctx->js_user_data, js_user_data))
        {
            continue;
        }

        lv_obj_remove_event_dsc(obj, ctx->dsc);
        sni_cb_event_free_ctx(ctx);
        removed++;
    }

    return removed;
}

bool sni_cb_timer_create(jerry_value_t js_cb,
                         uint32_t period,
                         jerry_value_t js_user_data,
                         lv_timer_t **out_timer)
{
    if (!jerry_value_is_function(js_cb) || !out_timer)
    {
        return false;
    }

    sni_timer_callback_ctx_t *ctx = eos_malloc_zeroed(sizeof(sni_timer_callback_ctx_t));
    if (!ctx)
    {
        return false;
    }

    ctx->js_cb = jerry_value_copy(js_cb);
    ctx->js_user_data = jerry_value_copy(js_user_data);
    ctx->alive = true;

    lv_timer_t *timer = lv_timer_create(sni_cb_timer_dispatch, period, ctx);
    if (!timer)
    {
        jerry_value_free(ctx->js_cb);
        jerry_value_free(ctx->js_user_data);
        eos_free(ctx);
        return false;
    }

    ctx->timer = timer;
    HASH_ADD_PTR(s_timer_ctx_map, timer, ctx);

    *out_timer = timer;
    return true;
}

bool sni_cb_timer_set_cb(lv_timer_t *timer, jerry_value_t js_cb)
{
    if (!timer || !jerry_value_is_function(js_cb))
    {
        return false;
    }

    sni_timer_callback_ctx_t *ctx = NULL;
    HASH_FIND_PTR(s_timer_ctx_map, &timer, ctx);
    if (!ctx || !ctx->alive)
    {
        return false;
    }

    jerry_value_free(ctx->js_cb);
    ctx->js_cb = jerry_value_copy(js_cb);
    return true;
}

void sni_cb_timer_delete(lv_timer_t *timer)
{
    if (!timer)
    {
        return;
    }

    sni_timer_callback_ctx_t *ctx = NULL;
    HASH_FIND_PTR(s_timer_ctx_map, &timer, ctx);
    if (ctx)
    {
        sni_cb_timer_cleanup_impl(ctx);
    }
    lv_timer_delete(timer);
}
