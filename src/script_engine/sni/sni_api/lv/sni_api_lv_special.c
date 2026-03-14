/**
 * @file sni_api_lv_special.c
 * @brief LVGL SNI 手写特殊包装函数实现
 * @author Sab1e
 * @date 2026-03-14
 */

#include "sni_api_lv_special.h"

#include "lvgl.h"
#include "elena_os_mem.h"
#include "sni_api_export.h"
#include "sni_callback_runtime.h"
#include "sni_type_bridge.h"
#include "sni_types.h"

static sni_anim_callback_ctx_t *sni_anim_get_ctx(jerry_value_t this_value)
{
    sni_anim_callback_ctx_t *ctx = NULL;
    if (!sni_tb_js2c(this_value, SNI_H_LV_ANIM, &ctx))
    {
        return NULL;
    }

    return ctx;
}

jerry_value_t sni_api_ctor_timer(const jerry_call_info_t *call_info_p,
                                 const jerry_value_t args_p[],
                                 const jerry_length_t args_count)
{
    if (jerry_value_is_undefined(call_info_p->new_target))
    {
        return sni_api_throw_error("Constructor must be called with new");
    }

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_function(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }

    uint32_t arg_period = sni_tb_js2c_uint32(args_p[1]);
    lv_timer_t *native_timer = NULL;
    if (!sni_cb_timer_create(args_p[0], arg_period, args_p[2], &native_timer))
    {
        return sni_api_throw_error("Failed to create timer");
    }

    if (!sni_tb_c2js_set_object(&native_timer, SNI_H_LV_TIMER, call_info_p->this_value))
    {
        return sni_api_throw_error("Failed to bind native object");
    }

    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_add_event_cb(const jerry_call_info_t *call_info_p,
                                          const jerry_value_t args_p[],
                                          const jerry_length_t args_count)
{
    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    lv_obj_t *self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_function(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }

    lv_event_code_t arg_filter = sni_tb_js2c_int32(args_p[1]);
    lv_event_dsc_t *result = NULL;
    if (!sni_cb_event_add(self_obj, args_p[0], arg_filter, args_p[2], &result))
    {
        return sni_api_throw_error("Failed to register event callback");
    }

    return sni_tb_c2js(&result, SNI_H_LV_EVENT_DSC);
}

jerry_value_t sni_api_lv_obj_remove_event_cb(const jerry_call_info_t *call_info_p,
                                             const jerry_value_t args_p[],
                                             const jerry_length_t args_count)
{
    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    lv_obj_t *self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_function(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }

    bool result = sni_cb_event_remove_by_js_cb(self_obj, args_p[0]);
    return sni_tb_c2js_boolean(result);
}

jerry_value_t sni_api_lv_obj_remove_event_dsc(const jerry_call_info_t *call_info_p,
                                              const jerry_value_t args_p[],
                                              const jerry_length_t args_count)
{
    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    lv_obj_t *self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_event_dsc_t *arg_dsc;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_EVENT_DSC, &arg_dsc))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    bool result = sni_cb_event_remove_dsc(self_obj, arg_dsc);
    return sni_tb_c2js_boolean(result);
}

jerry_value_t sni_api_lv_obj_remove_event_cb_with_user_data(const jerry_call_info_t *call_info_p,
                                                            const jerry_value_t args_p[],
                                                            const jerry_length_t args_count)
{
    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    lv_obj_t *self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_function(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }

    uint32_t result = sni_cb_event_remove_by_js_cb_user_data(self_obj, args_p[0], args_p[1]);
    return sni_tb_c2js(&result, SNI_T_UINT32);
}

jerry_value_t sni_api_lv_timer_set_cb(const jerry_call_info_t *call_info_p,
                                      const jerry_value_t args_p[],
                                      const jerry_length_t args_count)
{
    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    lv_timer_t *self_timer;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_TIMER, &self_timer))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_function(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }

    bool result = sni_cb_timer_set_cb(self_timer, args_p[0]);
    return sni_tb_c2js_boolean(result);
}

jerry_value_t sni_api_lv_timer_delete(const jerry_call_info_t *call_info_p,
                                      const jerry_value_t args_p[],
                                      const jerry_length_t args_count)
{
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    lv_timer_t *self_timer;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_TIMER, &self_timer))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    sni_cb_timer_delete(self_timer);
    return jerry_undefined();
}

jerry_value_t sni_api_prop_set_timer_cb(const jerry_call_info_t *call_info_p,
                                        const jerry_value_t args_p[],
                                        const jerry_length_t args_count)
{
    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    lv_timer_t *self_timer;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_TIMER, &self_timer))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_function(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }

    bool result = sni_cb_timer_set_cb(self_timer, args_p[0]);
    if (!result)
    {
        return sni_api_throw_error("Failed to set timer callback");
    }

    return jerry_undefined();
}

jerry_value_t sni_api_ctor_anim(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    sni_anim_callback_ctx_t *ctx = NULL;

    if (jerry_value_is_undefined(call_info_p->new_target))
    {
        return sni_api_throw_error("Constructor must be called with new");
    }

    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!sni_cb_anim_create(&ctx))
    {
        return sni_api_throw_error("Failed to create anim");
    }

    if (!sni_tb_c2js_set_object(&ctx, SNI_H_LV_ANIM, call_info_p->this_value))
    {
        eos_free(ctx);
        return sni_api_throw_error("Failed to bind native object");
    }

    return jerry_undefined();
}

jerry_value_t sni_api_lv_anim_set_values(const jerry_call_info_t *call_info_p,
                                         const jerry_value_t args_p[],
                                         const jerry_length_t args_count)
{
    sni_anim_callback_ctx_t *ctx = NULL;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    ctx = sni_anim_get_ctx(call_info_p->this_value);
    if (!ctx)
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[0]) || !jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }

    lv_anim_set_values(sni_cb_anim_get_lv_anim(ctx),
                       sni_tb_js2c_int32(args_p[0]),
                       sni_tb_js2c_int32(args_p[1]));
    return jerry_undefined();
}

jerry_value_t sni_api_lv_anim_set_duration(const jerry_call_info_t *call_info_p,
                                           const jerry_value_t args_p[],
                                           const jerry_length_t args_count)
{
    sni_anim_callback_ctx_t *ctx = NULL;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    ctx = sni_anim_get_ctx(call_info_p->this_value);
    if (!ctx)
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }

    lv_anim_set_duration(sni_cb_anim_get_lv_anim(ctx), sni_tb_js2c_uint32(args_p[0]));
    return jerry_undefined();
}

jerry_value_t sni_api_lv_anim_set_delay(const jerry_call_info_t *call_info_p,
                                        const jerry_value_t args_p[],
                                        const jerry_length_t args_count)
{
    sni_anim_callback_ctx_t *ctx = NULL;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    ctx = sni_anim_get_ctx(call_info_p->this_value);
    if (!ctx)
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }

    lv_anim_set_delay(sni_cb_anim_get_lv_anim(ctx), sni_tb_js2c_uint32(args_p[0]));
    return jerry_undefined();
}

jerry_value_t sni_api_lv_anim_set_repeat_count(const jerry_call_info_t *call_info_p,
                                               const jerry_value_t args_p[],
                                               const jerry_length_t args_count)
{
    sni_anim_callback_ctx_t *ctx = NULL;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    ctx = sni_anim_get_ctx(call_info_p->this_value);
    if (!ctx)
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }

    lv_anim_set_repeat_count(sni_cb_anim_get_lv_anim(ctx), sni_tb_js2c_uint32(args_p[0]));
    return jerry_undefined();
}

jerry_value_t sni_api_lv_anim_start(const jerry_call_info_t *call_info_p,
                                    const jerry_value_t args_p[],
                                    const jerry_length_t args_count)
{
    sni_anim_callback_ctx_t *ctx = NULL;
    (void)args_p;

    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    ctx = sni_anim_get_ctx(call_info_p->this_value);
    if (!ctx)
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!sni_cb_anim_start(ctx))
    {
        return sni_api_throw_error("Failed to start anim");
    }

    return jerry_undefined();
}

jerry_value_t sni_api_lv_anim_set_custom_exec_cb(const jerry_call_info_t *call_info_p,
                                                 const jerry_value_t args_p[],
                                                 const jerry_length_t args_count)
{
    sni_anim_callback_ctx_t *ctx = NULL;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    ctx = sni_anim_get_ctx(call_info_p->this_value);
    if (!ctx)
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_function(args_p[0]) || !sni_cb_anim_set_cb(ctx, SNI_ANIM_CB_SLOT_CUSTOM_EXEC, args_p[0]))
    {
        return sni_api_throw_error("Failed to set anim callback");
    }

    return jerry_undefined();
}

jerry_value_t sni_api_lv_anim_set_start_cb(const jerry_call_info_t *call_info_p,
                                           const jerry_value_t args_p[],
                                           const jerry_length_t args_count)
{
    sni_anim_callback_ctx_t *ctx = NULL;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    ctx = sni_anim_get_ctx(call_info_p->this_value);
    if (!ctx)
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_function(args_p[0]) || !sni_cb_anim_set_cb(ctx, SNI_ANIM_CB_SLOT_START, args_p[0]))
    {
        return sni_api_throw_error("Failed to set anim callback");
    }

    return jerry_undefined();
}

jerry_value_t sni_api_lv_anim_set_completed_cb(const jerry_call_info_t *call_info_p,
                                               const jerry_value_t args_p[],
                                               const jerry_length_t args_count)
{
    sni_anim_callback_ctx_t *ctx = NULL;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    ctx = sni_anim_get_ctx(call_info_p->this_value);
    if (!ctx)
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_function(args_p[0]) || !sni_cb_anim_set_cb(ctx, SNI_ANIM_CB_SLOT_COMPLETED, args_p[0]))
    {
        return sni_api_throw_error("Failed to set anim callback");
    }

    return jerry_undefined();
}

jerry_value_t sni_api_lv_anim_set_deleted_cb(const jerry_call_info_t *call_info_p,
                                             const jerry_value_t args_p[],
                                             const jerry_length_t args_count)
{
    sni_anim_callback_ctx_t *ctx = NULL;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    ctx = sni_anim_get_ctx(call_info_p->this_value);
    if (!ctx)
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_function(args_p[0]) || !sni_cb_anim_set_cb(ctx, SNI_ANIM_CB_SLOT_DELETED, args_p[0]))
    {
        return sni_api_throw_error("Failed to set anim callback");
    }

    return jerry_undefined();
}

jerry_value_t sni_api_lv_anim_set_get_value_cb(const jerry_call_info_t *call_info_p,
                                               const jerry_value_t args_p[],
                                               const jerry_length_t args_count)
{
    sni_anim_callback_ctx_t *ctx = NULL;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    ctx = sni_anim_get_ctx(call_info_p->this_value);
    if (!ctx)
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_function(args_p[0]) || !sni_cb_anim_set_cb(ctx, SNI_ANIM_CB_SLOT_GET_VALUE, args_p[0]))
    {
        return sni_api_throw_error("Failed to set anim callback");
    }

    return jerry_undefined();
}

jerry_value_t sni_api_lv_anim_set_path_cb(const jerry_call_info_t *call_info_p,
                                          const jerry_value_t args_p[],
                                          const jerry_length_t args_count)
{
    sni_anim_callback_ctx_t *ctx = NULL;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    ctx = sni_anim_get_ctx(call_info_p->this_value);
    if (!ctx)
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (jerry_value_is_number(args_p[0]))
    {
        if (!sni_cb_anim_set_path_builtin(ctx, (sni_anim_path_builtin_t)sni_tb_js2c_int32(args_p[0])))
        {
            return sni_api_throw_error("Failed to set anim path");
        }

        return jerry_undefined();
    }

    if (jerry_value_is_function(args_p[0]))
    {
        if (!sni_cb_anim_set_path_js(ctx, args_p[0]))
        {
            return sni_api_throw_error("Failed to set anim path");
        }

        return jerry_undefined();
    }

    return sni_api_throw_error("Invalid argument type");
}
