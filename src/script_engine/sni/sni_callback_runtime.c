/**
 * @file sni_callback_runtime.c
 * @brief SNI callback runtime
 */

#include "sni_callback_runtime.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#define EOS_LOG_TAG "SNI Callback Runtime"
#include "eos_mem.h"
#include "eos_log.h"
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
static void sni_cb_anim_handle_destroy_hook(void *native_ptr);
static void sni_cb_style_handle_destroy_cb(void *native_ptr);

void sni_cb_runtime_init(void)
{
    sni_tb_register_handle_destroy_cb(SNI_H_LV_EVENT_DSC, sni_cb_event_dsc_destroy_cb);
    sni_tb_register_handle_destroy_cb(SNI_H_LV_TIMER, sni_cb_timer_handle_destroy_cb);
    sni_tb_register_handle_destroy_cb(SNI_H_LV_STYLE, sni_cb_style_handle_destroy_cb);
    sni_tb_register_handle_destroy_cb(SNI_H_LV_ANIM, sni_cb_anim_handle_destroy_hook);
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
        // ctx is still in hash table, user did not explicitly delete it, clean up here
        sni_cb_timer_cleanup_impl(ctx);
        lv_timer_delete(timer);
    }
    // ctx is not in hash table, it has been explicitly deleted
}

static void sni_cb_style_handle_destroy_cb(void *native_ptr)
{
    lv_style_t *style = (lv_style_t *)native_ptr;
    if (!style)
    {
        return;
    }

    lv_style_reset(style);
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

/* Anim Callback Implementation ------------------------------*/

static lv_anim_path_cb_t s_anim_path_table[SNI_ANIM_PATH_ENUM_MAX] =
{
    NULL,
    lv_anim_path_linear,
    lv_anim_path_ease_in,
    lv_anim_path_ease_out,
    lv_anim_path_ease_in_out,
    lv_anim_path_overshoot,
    lv_anim_path_bounce,
    lv_anim_path_step,
    lv_anim_path_custom_bezier3,
};

static bool sni_cb_anim_has_js_cb(sni_anim_callback_ctx_t *ctx, sni_anim_cb_slot_t slot)
{
    return ctx && !ctx->js_refs_freed && jerry_value_is_function(ctx->cb_slots[slot]);
}

static jerry_value_t sni_cb_anim_make_js_anim(sni_anim_callback_ctx_t *ctx)
{
    sni_anim_callback_ctx_t *ctx_ptr = ctx;
    return sni_tb_c2js(&ctx_ptr, SNI_H_LV_ANIM);
}

static void sni_cb_anim_clear_slot(sni_anim_callback_ctx_t *ctx, sni_anim_cb_slot_t slot)
{
    if (!ctx || ctx->js_refs_freed)
    {
        return;
    }

    jerry_value_free(ctx->cb_slots[slot]);
    ctx->cb_slots[slot] = jerry_undefined();
}

static bool sni_cb_anim_store_slot(sni_anim_callback_ctx_t *ctx, sni_anim_cb_slot_t slot, jerry_value_t js_cb)
{
    if (!ctx || !jerry_value_is_function(js_cb))
    {
        return false;
    }

    sni_cb_anim_clear_slot(ctx, slot);
    ctx->cb_slots[slot] = jerry_value_copy(js_cb);
    return true;
}

static void sni_cb_anim_free_js_refs(sni_anim_callback_ctx_t *ctx)
{
    if (!ctx || ctx->js_refs_freed)
    {
        return;
    }

    for (uint32_t i = 0; i < SNI_ANIM_CB_SLOT_COUNT; i++)
    {
        jerry_value_free(ctx->cb_slots[i]);
        ctx->cb_slots[i] = jerry_undefined();
    }

    ctx->js_refs_freed = true;
}

static void sni_cb_anim_call_void_slot(sni_anim_callback_ctx_t *ctx, sni_anim_cb_slot_t slot)
{
    if (!sni_cb_anim_has_js_cb(ctx, slot))
    {
        return;
    }

    jerry_value_t js_anim = sni_cb_anim_make_js_anim(ctx);
    jerry_value_t args[1] = {js_anim};
    jerry_value_t ret = jerry_call(ctx->cb_slots[slot], jerry_undefined(), args, 1);

    if (jerry_value_is_error(ret) || jerry_value_is_exception(ret))
    {
        EOS_LOG_E("Anim callback encountered an error");
    }

    jerry_value_free(ret);
    jerry_value_free(js_anim);
}

static void sni_cb_anim_custom_exec_dispatch(lv_anim_t *a, int32_t value)
{
    sni_anim_callback_ctx_t *ctx = (sni_anim_callback_ctx_t *)lv_anim_get_user_data(a);
    if (!sni_cb_anim_has_js_cb(ctx, SNI_ANIM_CB_SLOT_CUSTOM_EXEC))
    {
        return;
    }

    jerry_value_t js_anim = sni_cb_anim_make_js_anim(ctx);
    jerry_value_t js_value = sni_tb_c2js(&value, SNI_T_INT32);
    jerry_value_t args[2] = {js_anim, js_value};
    jerry_value_t ret = jerry_call(ctx->cb_slots[SNI_ANIM_CB_SLOT_CUSTOM_EXEC], jerry_undefined(), args, 2);

    if (jerry_value_is_error(ret) || jerry_value_is_exception(ret))
    {
        EOS_LOG_E("Anim custom_exec callback encountered an error");
    }

    jerry_value_free(ret);
    jerry_value_free(js_value);
    jerry_value_free(js_anim);
}

static void sni_cb_anim_start_dispatch(lv_anim_t *a)
{
    sni_anim_callback_ctx_t *ctx = (sni_anim_callback_ctx_t *)lv_anim_get_user_data(a);
    sni_cb_anim_call_void_slot(ctx, SNI_ANIM_CB_SLOT_START);
}

static void sni_cb_anim_completed_dispatch(lv_anim_t *a)
{
    sni_anim_callback_ctx_t *ctx = (sni_anim_callback_ctx_t *)lv_anim_get_user_data(a);
    sni_cb_anim_call_void_slot(ctx, SNI_ANIM_CB_SLOT_COMPLETED);
}

static void sni_cb_anim_deleted_dispatch(lv_anim_t *a)
{
    sni_anim_callback_ctx_t *ctx = (sni_anim_callback_ctx_t *)lv_anim_get_user_data(a);
    if (!ctx)
    {
        return;
    }

    lv_anim_set_user_data(a, NULL);
    ctx->active_anim = NULL;
    ctx->lvgl_alive = false;
    sni_cb_anim_call_void_slot(ctx, SNI_ANIM_CB_SLOT_DELETED);
}

static int32_t sni_cb_anim_call_int_slot(sni_anim_callback_ctx_t *ctx, sni_anim_cb_slot_t slot, int32_t fallback)
{
    if (!sni_cb_anim_has_js_cb(ctx, slot))
    {
        return fallback;
    }

    int32_t result = fallback;
    jerry_value_t js_anim = sni_cb_anim_make_js_anim(ctx);
    jerry_value_t args[1] = {js_anim};
    jerry_value_t ret = jerry_call(ctx->cb_slots[slot], jerry_undefined(), args, 1);

    if (jerry_value_is_error(ret) || jerry_value_is_exception(ret))
    {
        EOS_LOG_E("Anim callback encountered an error");
    }
    else if (jerry_value_is_number(ret))
    {
        result = jerry_value_as_int32(ret);
    }

    jerry_value_free(ret);
    jerry_value_free(js_anim);
    return result;
}

static int32_t sni_cb_anim_get_value_dispatch(lv_anim_t *a)
{
    sni_anim_callback_ctx_t *ctx = (sni_anim_callback_ctx_t *)lv_anim_get_user_data(a);
    return sni_cb_anim_call_int_slot(ctx, SNI_ANIM_CB_SLOT_GET_VALUE, 0);
}

static int32_t sni_cb_anim_path_dispatch(const lv_anim_t *a)
{
    sni_anim_callback_ctx_t *ctx = (sni_anim_callback_ctx_t *)lv_anim_get_user_data(a);
    if (!ctx)
    {
        return lv_anim_path_linear(a);
    }

    if (ctx->path_kind == SNI_ANIM_PATH_KIND_BUILTIN && ctx->builtin_path_fn)
    {
        return ctx->builtin_path_fn(a);
    }

    if (ctx->path_kind == SNI_ANIM_PATH_KIND_JS)
    {
        return sni_cb_anim_call_int_slot(ctx, SNI_ANIM_CB_SLOT_PATH, lv_anim_path_linear(a));
    }

    return lv_anim_path_linear(a);
}

static void sni_cb_anim_handle_destroy_hook(void *native_ptr)
{
    sni_anim_callback_ctx_t *ctx = (sni_anim_callback_ctx_t *)native_ptr;
    if (!ctx)
    {
        return;
    }

    if (ctx->lvgl_alive && ctx->active_anim)
    {
        lv_anim_set_user_data(ctx->active_anim, NULL);
    }

    sni_cb_anim_free_js_refs(ctx);
    eos_free(ctx);
}

bool sni_cb_anim_create(sni_anim_callback_ctx_t **out_ctx)
{
    if (!out_ctx)
    {
        return false;
    }

    sni_anim_callback_ctx_t *ctx = eos_malloc_zeroed(sizeof(sni_anim_callback_ctx_t));
    if (!ctx)
    {
        return false;
    }

    for (uint32_t i = 0; i < SNI_ANIM_CB_SLOT_COUNT; i++)
    {
        ctx->cb_slots[i] = jerry_undefined();
    }

    lv_anim_init(&ctx->pre_anim);
    lv_anim_set_user_data(&ctx->pre_anim, ctx);
    lv_anim_set_deleted_cb(&ctx->pre_anim, sni_cb_anim_deleted_dispatch);

    *out_ctx = ctx;
    return true;
}

lv_anim_t *sni_cb_anim_get_lv_anim(sni_anim_callback_ctx_t *ctx)
{
    if (!ctx)
    {
        return NULL;
    }

    if (ctx->lvgl_alive && ctx->active_anim)
    {
        return ctx->active_anim;
    }

    return &ctx->pre_anim;
}

bool sni_cb_anim_set_cb(sni_anim_callback_ctx_t *ctx, sni_anim_cb_slot_t slot, jerry_value_t js_cb)
{
    lv_anim_t *anim = NULL;

    if (!ctx || slot >= SNI_ANIM_CB_SLOT_COUNT || slot == SNI_ANIM_CB_SLOT_PATH)
    {
        return false;
    }

    if (!sni_cb_anim_store_slot(ctx, slot, js_cb))
    {
        return false;
    }

    anim = sni_cb_anim_get_lv_anim(ctx);
    if (!anim)
    {
        return false;
    }

    switch (slot)
    {
    case SNI_ANIM_CB_SLOT_CUSTOM_EXEC:
        lv_anim_set_custom_exec_cb(anim, sni_cb_anim_custom_exec_dispatch);
        break;
    case SNI_ANIM_CB_SLOT_START:
        lv_anim_set_start_cb(anim, sni_cb_anim_start_dispatch);
        break;
    case SNI_ANIM_CB_SLOT_COMPLETED:
        lv_anim_set_completed_cb(anim, sni_cb_anim_completed_dispatch);
        break;
    case SNI_ANIM_CB_SLOT_DELETED:
        lv_anim_set_deleted_cb(anim, sni_cb_anim_deleted_dispatch);
        break;
    case SNI_ANIM_CB_SLOT_GET_VALUE:
        lv_anim_set_get_value_cb(anim, sni_cb_anim_get_value_dispatch);
        break;
    default:
        return false;
    }

    return true;
}

bool sni_cb_anim_set_path_builtin(sni_anim_callback_ctx_t *ctx, sni_anim_path_builtin_t path_kind)
{
    lv_anim_t *anim = NULL;

    if (!ctx || path_kind >= SNI_ANIM_PATH_ENUM_MAX)
    {
        return false;
    }

    anim = sni_cb_anim_get_lv_anim(ctx);
    if (!anim)
    {
        return false;
    }

    sni_cb_anim_clear_slot(ctx, SNI_ANIM_CB_SLOT_PATH);
    ctx->path_kind = SNI_ANIM_PATH_KIND_NONE;
    ctx->builtin_path_fn = NULL;

    if (path_kind == SNI_ANIM_PATH_NONE)
    {
        lv_anim_set_path_cb(anim, lv_anim_path_linear);
        return true;
    }

    if (!s_anim_path_table[path_kind])
    {
        return false;
    }

    ctx->path_kind = SNI_ANIM_PATH_KIND_BUILTIN;
    ctx->builtin_path_fn = s_anim_path_table[path_kind];
    lv_anim_set_path_cb(anim, sni_cb_anim_path_dispatch);
    return true;
}

bool sni_cb_anim_set_path_js(sni_anim_callback_ctx_t *ctx, jerry_value_t js_cb)
{
    lv_anim_t *anim = NULL;

    if (!ctx || !jerry_value_is_function(js_cb))
    {
        return false;
    }

    if (!sni_cb_anim_store_slot(ctx, SNI_ANIM_CB_SLOT_PATH, js_cb))
    {
        return false;
    }

    anim = sni_cb_anim_get_lv_anim(ctx);
    if (!anim)
    {
        return false;
    }

    ctx->path_kind = SNI_ANIM_PATH_KIND_JS;
    ctx->builtin_path_fn = NULL;
    lv_anim_set_path_cb(anim, sni_cb_anim_path_dispatch);
    return true;
}

bool sni_cb_anim_start(sni_anim_callback_ctx_t *ctx)
{
    lv_anim_t *active_anim = NULL;

    if (!ctx)
    {
        return false;
    }

    if (ctx->lvgl_alive && ctx->active_anim)
    {
        return false;
    }

    lv_anim_set_user_data(&ctx->pre_anim, ctx);
    active_anim = lv_anim_start(&ctx->pre_anim);
    if (!active_anim)
    {
        return false;
    }

    ctx->active_anim = active_anim;
    ctx->lvgl_alive = true;
    lv_anim_set_user_data(active_anim, ctx);
    return true;
}
