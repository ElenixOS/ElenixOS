/**
 * @file sni_api_lv_dropdown.c
 * @brief LVGL dropdown SNI special wrappers
 */

#include "sni_api_lv_special.h"

/* Includes ---------------------------------------------------*/
#include "lvgl.h"
#include "sni_api_export.h"
#include "sni_type_bridge.h"
#include "sni_types.h"

/* Function Implementations -----------------------------------*/

jerry_value_t sni_api_lv_dropdown_set_symbol(const jerry_call_info_t *call_info_p,
                                             const jerry_value_t args_p[],
                                             const jerry_length_t args_count)
{
    lv_obj_t *self_obj = NULL;
    const char *arg_symbol = NULL;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_null(args_p[0]))
    {
        if (!jerry_value_is_string(args_p[0]))
        {
            return sni_api_throw_error("Invalid argument type");
        }
        arg_symbol = sni_tb_js2c_string(args_p[0]);
    }

    lv_dropdown_set_symbol(self_obj, arg_symbol);
    return jerry_undefined();
}

jerry_value_t sni_api_prop_set_dropdown_symbol(const jerry_call_info_t *call_info_p,
                                               const jerry_value_t args_p[],
                                               const jerry_length_t args_count)
{
    return sni_api_lv_dropdown_set_symbol(call_info_p, args_p, args_count);
}
