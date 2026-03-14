/**
 * @file sni_api_lv_timer.c
 * @brief LVGL timer 类特殊包装函数实现
 * @author Sab1e
 * @date 2026-03-15
 */

#include "sni_api_lv_special.h"

/* Includes ---------------------------------------------------*/
#include "lvgl.h"
#include "sni_api_export.h"
#include "sni_callback_runtime.h"
#include "sni_type_bridge.h"
#include "sni_types.h"

/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

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

    return sni_tb_c2js_boolean(sni_cb_timer_set_cb(self_timer, args_p[0]));
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

    if (!sni_cb_timer_set_cb(self_timer, args_p[0]))
    {
        return sni_api_throw_error("Failed to set timer callback");
    }

    return jerry_undefined();
}
