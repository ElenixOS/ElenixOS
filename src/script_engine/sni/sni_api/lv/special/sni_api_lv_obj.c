/**
 * @file sni_api_lv_obj.c
 * @brief LVGL obj 类特殊包装函数实现
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

    return sni_tb_c2js_boolean(sni_cb_event_remove_by_js_cb(self_obj, args_p[0]));
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
    lv_event_dsc_t *arg_dsc;

    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!sni_tb_js2c(args_p[0], SNI_H_LV_EVENT_DSC, &arg_dsc))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    return sni_tb_c2js_boolean(sni_cb_event_remove_dsc(self_obj, arg_dsc));
}

jerry_value_t sni_api_lv_obj_remove_event_cb_with_user_data(const jerry_call_info_t *call_info_p,
                                                            const jerry_value_t args_p[],
                                                            const jerry_length_t args_count)
{
    uint32_t result;

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

    result = sni_cb_event_remove_by_js_cb_user_data(self_obj, args_p[0], args_p[1]);
    return sni_tb_c2js(&result, SNI_T_UINT32);
}
