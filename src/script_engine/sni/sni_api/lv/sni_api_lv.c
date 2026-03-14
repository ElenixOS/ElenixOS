/**
 * @file sni_api_lv.c
 * @brief LVGL API 导出
 * @author Auto Generated
 * @date 2026-03-14
 */

#include "sni_api_lv.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "lvgl.h"
#include "sni_type_bridge.h"
#include "sni_types.h"
#include "sni_api_export.h"
#include "elena_os_log.h"
/* Macros and Definitions -------------------------------------*/
#define LV_API_NAME "lv"
/* Variables --------------------------------------------------*/
static jerry_value_t lv_api_obj;
/* Function Implementations -----------------------------------*/

jerry_value_t sni_api_ctor_obj(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    if (jerry_value_is_undefined(call_info_p->new_target))
    {
        return sni_api_throw_error("Constructor must be called with new");
    }

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    lv_obj_t* arg_parent;
    if (jerry_value_is_null(args_p[0]))
    {
        arg_parent = NULL;
    }
    else if (jerry_value_is_object(args_p[0]))
    {
        if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &arg_parent))
        {
            return sni_api_throw_error("Failed to convert argument");
        }
    }
    else
    {
        return sni_api_throw_error("Invalid argument type");
    }

    lv_obj_t* native_obj = lv_obj_create(arg_parent);
    if (!sni_tb_c2js_set_object(&native_obj, SNI_H_LV_OBJ, call_info_p->this_value))
    {
        return sni_api_throw_error("Failed to bind native object");
    }
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_center(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_center(self_obj);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_size(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t arg_w;
    arg_w = sni_tb_js2c_int32(args_p[0]);

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t arg_h;
    arg_h = sni_tb_js2c_int32(args_p[1]);

    lv_obj_set_size(self_obj, arg_w, arg_h);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_pos(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t arg_x;
    arg_x = sni_tb_js2c_int32(args_p[0]);

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t arg_y;
    arg_y = sni_tb_js2c_int32(args_p[1]);

    lv_obj_set_pos(self_obj, arg_x, arg_y);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_bg_color(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_color_t arg_value;
    if (!sni_tb_js2c(args_p[0], SNI_V_LV_COLOR, &arg_value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t arg_selector;
    arg_selector = sni_tb_js2c_uint32(args_p[1]);

    lv_obj_set_style_bg_color(self_obj, arg_value, arg_selector);
    return jerry_undefined();
}

jerry_value_t sni_api_prop_set_obj_align(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_align_t prop_value;
    prop_value = sni_tb_js2c_int32(args_p[0]);

    lv_obj_set_align(self_obj, prop_value);
    return jerry_undefined();
}

jerry_value_t sni_api_prop_get_obj_child_count(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    uint32_t result = lv_obj_get_child_count(self_obj);
    return sni_tb_c2js(&result, SNI_T_UINT32);
}

jerry_value_t sni_api_prop_get_obj_class(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    const lv_obj_class_t* result = lv_obj_get_class(self_obj);
    return sni_tb_c2js(&result, SNI_H_LV_OBJ_CLASS);
}

jerry_value_t sni_api_prop_get_obj_content_height(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_content_height(self_obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_prop_set_obj_content_height(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t prop_value;
    prop_value = sni_tb_js2c_int32(args_p[0]);

    lv_obj_set_content_height(self_obj, prop_value);
    return jerry_undefined();
}

jerry_value_t sni_api_prop_get_obj_content_width(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_content_width(self_obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_prop_set_obj_content_width(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t prop_value;
    prop_value = sni_tb_js2c_int32(args_p[0]);

    lv_obj_set_content_width(self_obj, prop_value);
    return jerry_undefined();
}

jerry_value_t sni_api_prop_get_obj_display(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_display_t* result = lv_obj_get_display(self_obj);
    return sni_tb_c2js(&result, SNI_H_LV_DISPLAY);
}

jerry_value_t sni_api_prop_get_obj_event_count(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    uint32_t result = lv_obj_get_event_count(self_obj);
    return sni_tb_c2js(&result, SNI_T_UINT32);
}

jerry_value_t sni_api_prop_set_obj_ext_click_area(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t prop_value;
    prop_value = sni_tb_js2c_int32(args_p[0]);

    lv_obj_set_ext_click_area(self_obj, prop_value);
    return jerry_undefined();
}

jerry_value_t sni_api_prop_set_obj_flex_flow(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_flex_flow_t prop_value;
    prop_value = sni_tb_js2c_int32(args_p[0]);

    lv_obj_set_flex_flow(self_obj, prop_value);
    return jerry_undefined();
}

jerry_value_t sni_api_prop_set_obj_flex_grow(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    uint8_t prop_value;
    prop_value = sni_tb_js2c_uint32(args_p[0]);

    lv_obj_set_flex_grow(self_obj, prop_value);
    return jerry_undefined();
}

jerry_value_t sni_api_prop_get_obj_group(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_group_t* result = lv_obj_get_group(self_obj);
    return sni_tb_c2js(&result, SNI_H_LV_GROUP);
}

jerry_value_t sni_api_prop_get_obj_height(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_height(self_obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_prop_set_obj_height(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t prop_value;
    prop_value = sni_tb_js2c_int32(args_p[0]);

    lv_obj_set_height(self_obj, prop_value);
    return jerry_undefined();
}

jerry_value_t sni_api_prop_get_obj_index(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_index(self_obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_prop_set_obj_layout(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    uint32_t prop_value;
    prop_value = sni_tb_js2c_uint32(args_p[0]);

    lv_obj_set_layout(self_obj, prop_value);
    return jerry_undefined();
}

jerry_value_t sni_api_prop_get_obj_parent(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_t* result = lv_obj_get_parent(self_obj);
    return sni_tb_c2js(&result, SNI_H_LV_OBJ);
}

jerry_value_t sni_api_prop_set_obj_parent(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* prop_value;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &prop_value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_parent(self_obj, prop_value);
    return jerry_undefined();
}

jerry_value_t sni_api_prop_get_obj_screen(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_t* result = lv_obj_get_screen(self_obj);
    return sni_tb_c2js(&result, SNI_H_LV_OBJ);
}

jerry_value_t sni_api_prop_get_obj_scroll_bottom(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_scroll_bottom(self_obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_prop_get_obj_scroll_dir(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_dir_t result = lv_obj_get_scroll_dir(self_obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_prop_set_obj_scroll_dir(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_dir_t prop_value;
    prop_value = sni_tb_js2c_int32(args_p[0]);

    lv_obj_set_scroll_dir(self_obj, prop_value);
    return jerry_undefined();
}

jerry_value_t sni_api_prop_get_obj_scroll_left(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_scroll_left(self_obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_prop_get_obj_scroll_right(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_scroll_right(self_obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_prop_get_obj_scroll_snap_x(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_scroll_snap_t result = lv_obj_get_scroll_snap_x(self_obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_prop_set_obj_scroll_snap_x(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_scroll_snap_t prop_value;
    prop_value = sni_tb_js2c_int32(args_p[0]);

    lv_obj_set_scroll_snap_x(self_obj, prop_value);
    return jerry_undefined();
}

jerry_value_t sni_api_prop_get_obj_scroll_snap_y(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_scroll_snap_t result = lv_obj_get_scroll_snap_y(self_obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_prop_set_obj_scroll_snap_y(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_scroll_snap_t prop_value;
    prop_value = sni_tb_js2c_int32(args_p[0]);

    lv_obj_set_scroll_snap_y(self_obj, prop_value);
    return jerry_undefined();
}

jerry_value_t sni_api_prop_get_obj_scroll_top(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_scroll_top(self_obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_prop_get_obj_scroll_x(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_scroll_x(self_obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_prop_get_obj_scroll_y(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_scroll_y(self_obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_prop_get_obj_scrollbar_mode(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_scrollbar_mode_t result = lv_obj_get_scrollbar_mode(self_obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_prop_set_obj_scrollbar_mode(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_scrollbar_mode_t prop_value;
    prop_value = sni_tb_js2c_int32(args_p[0]);

    lv_obj_set_scrollbar_mode(self_obj, prop_value);
    return jerry_undefined();
}

jerry_value_t sni_api_prop_get_obj_self_height(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_self_height(self_obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_prop_get_obj_self_width(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_self_width(self_obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_prop_get_obj_state(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_state_t result = lv_obj_get_state(self_obj);
    return sni_tb_c2js(&result, SNI_T_UINT32);
}

jerry_value_t sni_api_prop_get_obj_user_data(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    void* result = lv_obj_get_user_data(self_obj);
    return sni_tb_c2js(&result, SNI_T_PTR);
}

jerry_value_t sni_api_prop_set_obj_user_data(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    void* prop_value;
    if (!sni_tb_js2c(args_p[0], SNI_T_PTR, &prop_value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_user_data(self_obj, prop_value);
    return jerry_undefined();
}

jerry_value_t sni_api_prop_get_obj_width(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_width(self_obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_prop_set_obj_width(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t prop_value;
    prop_value = sni_tb_js2c_int32(args_p[0]);

    lv_obj_set_width(self_obj, prop_value);
    return jerry_undefined();
}

jerry_value_t sni_api_prop_get_obj_x(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_x(self_obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_prop_set_obj_x(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t prop_value;
    prop_value = sni_tb_js2c_int32(args_p[0]);

    lv_obj_set_x(self_obj, prop_value);
    return jerry_undefined();
}

jerry_value_t sni_api_prop_get_obj_x2(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_x2(self_obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_prop_get_obj_x_aligned(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_x_aligned(self_obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_prop_get_obj_y(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_y(self_obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_prop_set_obj_y(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t prop_value;
    prop_value = sni_tb_js2c_int32(args_p[0]);

    lv_obj_set_y(self_obj, prop_value);
    return jerry_undefined();
}

jerry_value_t sni_api_prop_get_obj_y2(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_y2(self_obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_prop_get_obj_y_aligned(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_y_aligned(self_obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_ctor_button(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    if (jerry_value_is_undefined(call_info_p->new_target))
    {
        return sni_api_throw_error("Constructor must be called with new");
    }

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    lv_obj_t* arg_parent;
    if (jerry_value_is_null(args_p[0]))
    {
        arg_parent = NULL;
    }
    else if (jerry_value_is_object(args_p[0]))
    {
        if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &arg_parent))
        {
            return sni_api_throw_error("Failed to convert argument");
        }
    }
    else
    {
        return sni_api_throw_error("Invalid argument type");
    }

    lv_obj_t* native_obj = lv_button_create(arg_parent);
    if (!sni_tb_c2js_set_object(&native_obj, SNI_H_LV_OBJ, call_info_p->this_value))
    {
        return sni_api_throw_error("Failed to bind native object");
    }
    return jerry_undefined();
}

jerry_value_t sni_api_ctor_label(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    if (jerry_value_is_undefined(call_info_p->new_target))
    {
        return sni_api_throw_error("Constructor must be called with new");
    }

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    lv_obj_t* arg_parent;
    if (jerry_value_is_null(args_p[0]))
    {
        arg_parent = NULL;
    }
    else if (jerry_value_is_object(args_p[0]))
    {
        if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &arg_parent))
        {
            return sni_api_throw_error("Failed to convert argument");
        }
    }
    else
    {
        return sni_api_throw_error("Invalid argument type");
    }

    lv_obj_t* native_obj = lv_label_create(arg_parent);
    if (!sni_tb_c2js_set_object(&native_obj, SNI_H_LV_OBJ, call_info_p->this_value))
    {
        return sni_api_throw_error("Failed to bind native object");
    }
    return jerry_undefined();
}

jerry_value_t sni_api_prop_get_label_long_mode(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_label_long_mode_t result = lv_label_get_long_mode(self_obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_prop_set_label_long_mode(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_label_long_mode_t prop_value;
    prop_value = sni_tb_js2c_int32(args_p[0]);

    lv_label_set_long_mode(self_obj, prop_value);
    return jerry_undefined();
}

jerry_value_t sni_api_prop_get_label_text(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    char* result = lv_label_get_text(self_obj);
    return sni_tb_c2js_string(result);
}

jerry_value_t sni_api_prop_set_label_text(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_string(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const char* prop_value;
    prop_value = sni_tb_js2c_string(args_p[0]);

    lv_label_set_text(self_obj, prop_value);
    return jerry_undefined();
}

jerry_value_t sni_api_prop_get_label_text_selection_end(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    uint32_t result = lv_label_get_text_selection_end(self_obj);
    return sni_tb_c2js(&result, SNI_T_UINT32);
}

jerry_value_t sni_api_prop_set_label_text_selection_end(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    uint32_t prop_value;
    prop_value = sni_tb_js2c_uint32(args_p[0]);

    lv_label_set_text_selection_end(self_obj, prop_value);
    return jerry_undefined();
}

jerry_value_t sni_api_prop_get_label_text_selection_start(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)args_p;
    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    uint32_t result = lv_label_get_text_selection_start(self_obj);
    return sni_tb_c2js(&result, SNI_T_UINT32);
}

jerry_value_t sni_api_prop_set_label_text_selection_start(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    uint32_t prop_value;
    prop_value = sni_tb_js2c_uint32(args_p[0]);

    lv_label_set_text_selection_start(self_obj, prop_value);
    return jerry_undefined();
}

jerry_value_t sni_api_prop_set_label_text_static(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(call_info_p->this_value))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* self_obj;
    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &self_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_string(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const char* prop_value;
    prop_value = sni_tb_js2c_string(args_p[0]);

    lv_label_set_text_static(self_obj, prop_value);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_screen_active(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 0)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    lv_obj_t* result = lv_screen_active();
    return sni_tb_c2js(&result, SNI_H_LV_OBJ);
}

jerry_value_t sni_api_lv_color_hex(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_number(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    uint32_t arg_c;
    arg_c = sni_tb_js2c_uint32(args_p[0]);

    lv_color_t result = lv_color_hex(arg_c);
    return sni_tb_c2js(&result, SNI_V_LV_COLOR);
}

const sni_method_desc_t lv_class_methods_obj[] = {
    {.name = "center", .handler = sni_api_lv_obj_center},
    {.name = "setSize", .handler = sni_api_lv_obj_set_size},
    {.name = "setPos", .handler = sni_api_lv_obj_set_pos},
    {.name = "setStyleBgColor", .handler = sni_api_lv_obj_set_style_bg_color},
    {.name = NULL, .handler = NULL},
};

const sni_method_desc_t lv_class_static_methods_obj[] = {
    {.name = NULL, .handler = NULL},
};

const sni_property_desc_t lv_class_properties_obj[] = {
    {.name = "align", .getter = NULL, .setter = sni_api_prop_set_obj_align},
    {.name = "childCount", .getter = sni_api_prop_get_obj_child_count, .setter = NULL},
    {.name = "class", .getter = sni_api_prop_get_obj_class, .setter = NULL},
    {.name = "contentHeight", .getter = sni_api_prop_get_obj_content_height, .setter = sni_api_prop_set_obj_content_height},
    {.name = "contentWidth", .getter = sni_api_prop_get_obj_content_width, .setter = sni_api_prop_set_obj_content_width},
    {.name = "display", .getter = sni_api_prop_get_obj_display, .setter = NULL},
    {.name = "eventCount", .getter = sni_api_prop_get_obj_event_count, .setter = NULL},
    {.name = "extClickArea", .getter = NULL, .setter = sni_api_prop_set_obj_ext_click_area},
    {.name = "flexFlow", .getter = NULL, .setter = sni_api_prop_set_obj_flex_flow},
    {.name = "flexGrow", .getter = NULL, .setter = sni_api_prop_set_obj_flex_grow},
    {.name = "group", .getter = sni_api_prop_get_obj_group, .setter = NULL},
    {.name = "height", .getter = sni_api_prop_get_obj_height, .setter = sni_api_prop_set_obj_height},
    {.name = "index", .getter = sni_api_prop_get_obj_index, .setter = NULL},
    {.name = "layout", .getter = NULL, .setter = sni_api_prop_set_obj_layout},
    {.name = "parent", .getter = sni_api_prop_get_obj_parent, .setter = sni_api_prop_set_obj_parent},
    {.name = "screen", .getter = sni_api_prop_get_obj_screen, .setter = NULL},
    {.name = "scrollBottom", .getter = sni_api_prop_get_obj_scroll_bottom, .setter = NULL},
    {.name = "scrollDir", .getter = sni_api_prop_get_obj_scroll_dir, .setter = sni_api_prop_set_obj_scroll_dir},
    {.name = "scrollLeft", .getter = sni_api_prop_get_obj_scroll_left, .setter = NULL},
    {.name = "scrollRight", .getter = sni_api_prop_get_obj_scroll_right, .setter = NULL},
    {.name = "scrollSnapX", .getter = sni_api_prop_get_obj_scroll_snap_x, .setter = sni_api_prop_set_obj_scroll_snap_x},
    {.name = "scrollSnapY", .getter = sni_api_prop_get_obj_scroll_snap_y, .setter = sni_api_prop_set_obj_scroll_snap_y},
    {.name = "scrollTop", .getter = sni_api_prop_get_obj_scroll_top, .setter = NULL},
    {.name = "scrollX", .getter = sni_api_prop_get_obj_scroll_x, .setter = NULL},
    {.name = "scrollY", .getter = sni_api_prop_get_obj_scroll_y, .setter = NULL},
    {.name = "scrollbarMode", .getter = sni_api_prop_get_obj_scrollbar_mode, .setter = sni_api_prop_set_obj_scrollbar_mode},
    {.name = "selfHeight", .getter = sni_api_prop_get_obj_self_height, .setter = NULL},
    {.name = "selfWidth", .getter = sni_api_prop_get_obj_self_width, .setter = NULL},
    {.name = "state", .getter = sni_api_prop_get_obj_state, .setter = NULL},
    {.name = "userData", .getter = sni_api_prop_get_obj_user_data, .setter = sni_api_prop_set_obj_user_data},
    {.name = "width", .getter = sni_api_prop_get_obj_width, .setter = sni_api_prop_set_obj_width},
    {.name = "x", .getter = sni_api_prop_get_obj_x, .setter = sni_api_prop_set_obj_x},
    {.name = "x2", .getter = sni_api_prop_get_obj_x2, .setter = NULL},
    {.name = "xAligned", .getter = sni_api_prop_get_obj_x_aligned, .setter = NULL},
    {.name = "y", .getter = sni_api_prop_get_obj_y, .setter = sni_api_prop_set_obj_y},
    {.name = "y2", .getter = sni_api_prop_get_obj_y2, .setter = NULL},
    {.name = "yAligned", .getter = sni_api_prop_get_obj_y_aligned, .setter = NULL},
    {.name = NULL, .getter = NULL, .setter = NULL},
};

const sni_constant_desc_t lv_class_constants_obj[] = {
    {.name = "LV_PART_MAIN", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "LV_PART_SCROLLBAR", .type = SNI_CONST_INT, .value.i = 65536},
    {.name = "LV_PART_INDICATOR", .type = SNI_CONST_INT, .value.i = 131072},
    {.name = "LV_PART_KNOB", .type = SNI_CONST_INT, .value.i = 196608},
    {.name = "LV_PART_SELECTED", .type = SNI_CONST_INT, .value.i = 262144},
    {.name = "LV_PART_ITEMS", .type = SNI_CONST_INT, .value.i = 327680},
    {.name = "LV_PART_CURSOR", .type = SNI_CONST_INT, .value.i = 393216},
    {.name = "LV_PART_CUSTOM_FIRST", .type = SNI_CONST_INT, .value.i = 524288},
    {.name = "LV_PART_ANY", .type = SNI_CONST_INT, .value.i = 983040},
    {.name = NULL, .type = SNI_CONST_INT, .value.i = 0},
};

const sni_method_desc_t lv_class_methods_button[] = {
    {.name = NULL, .handler = NULL},
};

const sni_method_desc_t lv_class_static_methods_button[] = {
    {.name = NULL, .handler = NULL},
};

const sni_property_desc_t lv_class_properties_button[] = {
    {.name = NULL, .getter = NULL, .setter = NULL},
};

const sni_constant_desc_t lv_class_constants_button[] = {
    {.name = NULL, .type = SNI_CONST_INT, .value.i = 0},
};

const sni_method_desc_t lv_class_methods_label[] = {
    {.name = NULL, .handler = NULL},
};

const sni_method_desc_t lv_class_static_methods_label[] = {
    {.name = NULL, .handler = NULL},
};

const sni_property_desc_t lv_class_properties_label[] = {
    {.name = "longMode", .getter = sni_api_prop_get_label_long_mode, .setter = sni_api_prop_set_label_long_mode},
    {.name = "text", .getter = sni_api_prop_get_label_text, .setter = sni_api_prop_set_label_text},
    {.name = "textSelectionEnd", .getter = sni_api_prop_get_label_text_selection_end, .setter = sni_api_prop_set_label_text_selection_end},
    {.name = "textSelectionStart", .getter = sni_api_prop_get_label_text_selection_start, .setter = sni_api_prop_set_label_text_selection_start},
    {.name = "textStatic", .getter = NULL, .setter = sni_api_prop_set_label_text_static},
    {.name = NULL, .getter = NULL, .setter = NULL},
};

const sni_constant_desc_t lv_class_constants_label[] = {
    {.name = NULL, .type = SNI_CONST_INT, .value.i = 0},
};

const sni_method_desc_t lv_class_methods_screen[] = {
    {.name = NULL, .handler = NULL},
};

const sni_method_desc_t lv_class_static_methods_screen[] = {
    {.name = "active", .handler = sni_api_lv_screen_active},
    {.name = NULL, .handler = NULL},
};

const sni_property_desc_t lv_class_properties_screen[] = {
    {.name = NULL, .getter = NULL, .setter = NULL},
};

const sni_constant_desc_t lv_class_constants_screen[] = {
    {.name = NULL, .type = SNI_CONST_INT, .value.i = 0},
};

const sni_method_desc_t lv_class_methods_color[] = {
    {.name = NULL, .handler = NULL},
};

const sni_method_desc_t lv_class_static_methods_color[] = {
    {.name = "hex", .handler = sni_api_lv_color_hex},
    {.name = NULL, .handler = NULL},
};

const sni_property_desc_t lv_class_properties_color[] = {
    {.name = NULL, .getter = NULL, .setter = NULL},
};

const sni_constant_desc_t lv_class_constants_color[] = {
    {.name = NULL, .type = SNI_CONST_INT, .value.i = 0},
};

const sni_constant_desc_t lv_root_constants[] = {
    {.name = "ALIGN_BOTTOM_LEFT", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "ALIGN_BOTTOM_MID", .type = SNI_CONST_INT, .value.i = 5},
    {.name = "ALIGN_BOTTOM_RIGHT", .type = SNI_CONST_INT, .value.i = 6},
    {.name = "ALIGN_CENTER", .type = SNI_CONST_INT, .value.i = 9},
    {.name = "ALIGN_DEFAULT", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "ALIGN_LEFT_MID", .type = SNI_CONST_INT, .value.i = 7},
    {.name = "ALIGN_OUT_BOTTOM_LEFT", .type = SNI_CONST_INT, .value.i = 13},
    {.name = "ALIGN_OUT_BOTTOM_MID", .type = SNI_CONST_INT, .value.i = 14},
    {.name = "ALIGN_OUT_BOTTOM_RIGHT", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "ALIGN_OUT_LEFT_BOTTOM", .type = SNI_CONST_INT, .value.i = 18},
    {.name = "ALIGN_OUT_LEFT_MID", .type = SNI_CONST_INT, .value.i = 17},
    {.name = "ALIGN_OUT_LEFT_TOP", .type = SNI_CONST_INT, .value.i = 16},
    {.name = "ALIGN_OUT_RIGHT_BOTTOM", .type = SNI_CONST_INT, .value.i = 21},
    {.name = "ALIGN_OUT_RIGHT_MID", .type = SNI_CONST_INT, .value.i = 20},
    {.name = "ALIGN_OUT_RIGHT_TOP", .type = SNI_CONST_INT, .value.i = 19},
    {.name = "ALIGN_OUT_TOP_LEFT", .type = SNI_CONST_INT, .value.i = 10},
    {.name = "ALIGN_OUT_TOP_MID", .type = SNI_CONST_INT, .value.i = 11},
    {.name = "ALIGN_OUT_TOP_RIGHT", .type = SNI_CONST_INT, .value.i = 12},
    {.name = "ALIGN_RIGHT_MID", .type = SNI_CONST_INT, .value.i = 8},
    {.name = "ALIGN_TOP_LEFT", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "ALIGN_TOP_MID", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "ALIGN_TOP_RIGHT", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "ANIM_IMAGE_PART_MAIN", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "ANIM_OFF", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "ANIM_ON", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "ARC_MODE_NORMAL", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "ARC_MODE_REVERSE", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "ARC_MODE_SYMMETRICAL", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "ARRAY_DEFAULT_CAPACITY", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "ARRAY_DEFAULT_SHRINK_RATIO", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "ATTRIBUTE_MEM_ALIGN_SIZE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "BAR_MODE_NORMAL", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "BAR_MODE_RANGE", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "BAR_MODE_SYMMETRICAL", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "BAR_ORIENTATION_AUTO", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "BAR_ORIENTATION_HORIZONTAL", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "BAR_ORIENTATION_VERTICAL", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "BASE_DIR_AUTO", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "BASE_DIR_LTR", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "BASE_DIR_NEUTRAL", .type = SNI_CONST_INT, .value.i = 32},
    {.name = "BASE_DIR_RTL", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "BASE_DIR_WEAK", .type = SNI_CONST_INT, .value.i = 33},
    {.name = "BEZIER_VAL_SHIFT", .type = SNI_CONST_INT, .value.i = 10},
    {.name = "BIDI_LRO", .type = SNI_CONST_STRING, .value.s = "\xE2\x80\xAD"},
    {.name = "BIDI_RLO", .type = SNI_CONST_STRING, .value.s = "\xE2\x80\xAE"},
    {.name = "BIG_ENDIAN_SYSTEM", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "BIN_DECODER_RAM_LOAD", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "BLEND_MODE_ADDITIVE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "BLEND_MODE_MULTIPLY", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "BLEND_MODE_NORMAL", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "BLEND_MODE_SUBTRACTIVE", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "BORDER_SIDE_BOTTOM", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "BORDER_SIDE_FULL", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "BORDER_SIDE_INTERNAL", .type = SNI_CONST_INT, .value.i = 16},
    {.name = "BORDER_SIDE_LEFT", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "BORDER_SIDE_NONE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "BORDER_SIDE_RIGHT", .type = SNI_CONST_INT, .value.i = 8},
    {.name = "BORDER_SIDE_TOP", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "BUILD_EXAMPLES", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "BUTTONMATRIX_CTRL_CHECKABLE", .type = SNI_CONST_INT, .value.i = 128},
    {.name = "BUTTONMATRIX_CTRL_CHECKED", .type = SNI_CONST_INT, .value.i = 256},
    {.name = "BUTTONMATRIX_CTRL_CLICK_TRIG", .type = SNI_CONST_INT, .value.i = 512},
    {.name = "BUTTONMATRIX_CTRL_CUSTOM_1", .type = SNI_CONST_INT, .value.i = 16384},
    {.name = "BUTTONMATRIX_CTRL_CUSTOM_2", .type = SNI_CONST_INT, .value.i = 32768},
    {.name = "BUTTONMATRIX_CTRL_DISABLED", .type = SNI_CONST_INT, .value.i = 64},
    {.name = "BUTTONMATRIX_CTRL_HIDDEN", .type = SNI_CONST_INT, .value.i = 16},
    {.name = "BUTTONMATRIX_CTRL_NO_REPEAT", .type = SNI_CONST_INT, .value.i = 32},
    {.name = "BUTTONMATRIX_CTRL_POPOVER", .type = SNI_CONST_INT, .value.i = 1024},
    {.name = "BUTTONMATRIX_CTRL_RESERVED_1", .type = SNI_CONST_INT, .value.i = 2048},
    {.name = "BUTTONMATRIX_CTRL_RESERVED_2", .type = SNI_CONST_INT, .value.i = 4096},
    {.name = "BUTTONMATRIX_CTRL_RESERVED_3", .type = SNI_CONST_INT, .value.i = 8192},
    {.name = "CACHE_DEF_SIZE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "CACHE_RESERVE_COND_ERROR", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "CACHE_RESERVE_COND_NEED_VICTIM", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "CACHE_RESERVE_COND_OK", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "CACHE_RESERVE_COND_TOO_LARGE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "CALENDAR_WEEK_STARTS_MONDAY", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "CFF_CONFIG_OPTION_DARKENING_PARAMETER_X1", .type = SNI_CONST_INT, .value.i = 500},
    {.name = "CFF_CONFIG_OPTION_DARKENING_PARAMETER_X2", .type = SNI_CONST_INT, .value.i = 1000},
    {.name = "CFF_CONFIG_OPTION_DARKENING_PARAMETER_X3", .type = SNI_CONST_INT, .value.i = 1667},
    {.name = "CFF_CONFIG_OPTION_DARKENING_PARAMETER_X4", .type = SNI_CONST_INT, .value.i = 2333},
    {.name = "CFF_CONFIG_OPTION_DARKENING_PARAMETER_Y1", .type = SNI_CONST_INT, .value.i = 400},
    {.name = "CFF_CONFIG_OPTION_DARKENING_PARAMETER_Y2", .type = SNI_CONST_INT, .value.i = 275},
    {.name = "CFF_CONFIG_OPTION_DARKENING_PARAMETER_Y3", .type = SNI_CONST_INT, .value.i = 275},
    {.name = "CFF_CONFIG_OPTION_DARKENING_PARAMETER_Y4", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "CHART_AXIS_LAST", .type = SNI_CONST_INT, .value.i = 5},
    {.name = "CHART_AXIS_PRIMARY_X", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "CHART_AXIS_PRIMARY_Y", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "CHART_AXIS_SECONDARY_X", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "CHART_AXIS_SECONDARY_Y", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "CHART_TYPE_BAR", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "CHART_TYPE_LINE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "CHART_TYPE_NONE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "CHART_TYPE_SCATTER", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "CHART_UPDATE_MODE_CIRCULAR", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "CHART_UPDATE_MODE_SHIFT", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "COLOR_DEPTH", .type = SNI_CONST_INT, .value.i = 16},
    {.name = "COLOR_FORMAT_A1", .type = SNI_CONST_INT, .value.i = 11},
    {.name = "COLOR_FORMAT_A2", .type = SNI_CONST_INT, .value.i = 12},
    {.name = "COLOR_FORMAT_A4", .type = SNI_CONST_INT, .value.i = 13},
    {.name = "COLOR_FORMAT_A8", .type = SNI_CONST_INT, .value.i = 14},
    {.name = "COLOR_FORMAT_AL88", .type = SNI_CONST_INT, .value.i = 21},
    {.name = "COLOR_FORMAT_ARGB8565", .type = SNI_CONST_INT, .value.i = 19},
    {.name = "COLOR_FORMAT_ARGB8888", .type = SNI_CONST_INT, .value.i = 16},
    {.name = "COLOR_FORMAT_I1", .type = SNI_CONST_INT, .value.i = 7},
    {.name = "COLOR_FORMAT_I2", .type = SNI_CONST_INT, .value.i = 8},
    {.name = "COLOR_FORMAT_I4", .type = SNI_CONST_INT, .value.i = 9},
    {.name = "COLOR_FORMAT_I400", .type = SNI_CONST_INT, .value.i = 35},
    {.name = "COLOR_FORMAT_I420", .type = SNI_CONST_INT, .value.i = 32},
    {.name = "COLOR_FORMAT_I422", .type = SNI_CONST_INT, .value.i = 33},
    {.name = "COLOR_FORMAT_I444", .type = SNI_CONST_INT, .value.i = 34},
    {.name = "COLOR_FORMAT_I8", .type = SNI_CONST_INT, .value.i = 10},
    {.name = "COLOR_FORMAT_L8", .type = SNI_CONST_INT, .value.i = 6},
    {.name = "COLOR_FORMAT_NATIVE", .type = SNI_CONST_INT, .value.i = 18},
    {.name = "COLOR_FORMAT_NATIVE_WITH_ALPHA", .type = SNI_CONST_INT, .value.i = 20},
    {.name = "COLOR_FORMAT_NV12", .type = SNI_CONST_INT, .value.i = 37},
    {.name = "COLOR_FORMAT_NV21", .type = SNI_CONST_INT, .value.i = 36},
    {.name = "COLOR_FORMAT_RAW", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "COLOR_FORMAT_RAW_ALPHA", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "COLOR_FORMAT_RGB565", .type = SNI_CONST_INT, .value.i = 18},
    {.name = "COLOR_FORMAT_RGB565A8", .type = SNI_CONST_INT, .value.i = 20},
    {.name = "COLOR_FORMAT_RGB888", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "COLOR_FORMAT_UNKNOWN", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "COLOR_FORMAT_UYVY", .type = SNI_CONST_INT, .value.i = 39},
    {.name = "COLOR_FORMAT_XRGB8888", .type = SNI_CONST_INT, .value.i = 17},
    {.name = "COLOR_FORMAT_YUV_END", .type = SNI_CONST_INT, .value.i = 39},
    {.name = "COLOR_FORMAT_YUV_START", .type = SNI_CONST_INT, .value.i = 32},
    {.name = "COLOR_FORMAT_YUY2", .type = SNI_CONST_INT, .value.i = 38},
    {.name = "COLOR_MIX_ROUND_OFS", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "COLOR_NATIVE_WITH_ALPHA_SIZE", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "COORD_TYPE_SHIFT", .type = SNI_CONST_INT, .value.i = 29},
    {.name = "COVER_RES_COVER", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "COVER_RES_MASKED", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "COVER_RES_NOT_COVER", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "DIR_ALL", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "DIR_BOTTOM", .type = SNI_CONST_INT, .value.i = 8},
    {.name = "DIR_HOR", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "DIR_LEFT", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "DIR_NONE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "DIR_RIGHT", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "DIR_TOP", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "DIR_VER", .type = SNI_CONST_INT, .value.i = 12},
    {.name = "DISPLAY_RENDER_MODE_DIRECT", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "DISPLAY_RENDER_MODE_FULL", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "DISPLAY_RENDER_MODE_PARTIAL", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "DISPLAY_ROTATION_0", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "DISPLAY_ROTATION_180", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "DISPLAY_ROTATION_270", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "DISPLAY_ROTATION_90", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "DRAW_BUF_ALIGN", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "DRAW_BUF_STRIDE_ALIGN", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "DRAW_SW_ASM_CUSTOM", .type = SNI_CONST_INT, .value.i = 255},
    {.name = "DRAW_SW_ASM_HELIUM", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "DRAW_SW_ASM_NEON", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "DRAW_SW_ASM_NONE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "DRAW_SW_CIRCLE_CACHE_SIZE", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "DRAW_SW_COMPLEX", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "DRAW_SW_DRAW_UNIT_CNT", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "DRAW_SW_MASK_LINE_SIDE_BOTTOM", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "DRAW_SW_MASK_LINE_SIDE_LEFT", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "DRAW_SW_MASK_LINE_SIDE_RIGHT", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "DRAW_SW_MASK_LINE_SIDE_TOP", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "DRAW_SW_MASK_RES_CHANGED", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "DRAW_SW_MASK_RES_FULL_COVER", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "DRAW_SW_MASK_RES_TRANSP", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "DRAW_SW_MASK_RES_UNKNOWN", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "DRAW_SW_MASK_TYPE_ANGLE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "DRAW_SW_MASK_TYPE_FADE", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "DRAW_SW_MASK_TYPE_LINE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "DRAW_SW_MASK_TYPE_MAP", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "DRAW_SW_MASK_TYPE_RADIUS", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "DRAW_SW_SHADOW_CACHE_SIZE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "DRAW_SW_SUPPORT_A8", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "DRAW_SW_SUPPORT_AL88", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "DRAW_SW_SUPPORT_ARGB8888", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "DRAW_SW_SUPPORT_I1", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "DRAW_SW_SUPPORT_L8", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "DRAW_SW_SUPPORT_RGB565", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "DRAW_SW_SUPPORT_RGB565A8", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "DRAW_SW_SUPPORT_RGB888", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "DRAW_SW_SUPPORT_XRGB8888", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "DRAW_TASK_STATE_IN_PROGRESS", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "DRAW_TASK_STATE_QUEUED", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "DRAW_TASK_STATE_READY", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "DRAW_TASK_STATE_WAITING", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "DRAW_TASK_TYPE_ARC", .type = SNI_CONST_INT, .value.i = 8},
    {.name = "DRAW_TASK_TYPE_BORDER", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "DRAW_TASK_TYPE_BOX_SHADOW", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "DRAW_TASK_TYPE_FILL", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "DRAW_TASK_TYPE_IMAGE", .type = SNI_CONST_INT, .value.i = 5},
    {.name = "DRAW_TASK_TYPE_LABEL", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "DRAW_TASK_TYPE_LAYER", .type = SNI_CONST_INT, .value.i = 6},
    {.name = "DRAW_TASK_TYPE_LINE", .type = SNI_CONST_INT, .value.i = 7},
    {.name = "DRAW_TASK_TYPE_MASK_BITMAP", .type = SNI_CONST_INT, .value.i = 11},
    {.name = "DRAW_TASK_TYPE_MASK_RECTANGLE", .type = SNI_CONST_INT, .value.i = 10},
    {.name = "DRAW_TASK_TYPE_NONE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "DRAW_TASK_TYPE_TRIANGLE", .type = SNI_CONST_INT, .value.i = 9},
    {.name = "DRAW_TASK_TYPE_VECTOR", .type = SNI_CONST_INT, .value.i = 12},
    {.name = "DRAW_TRANSFORM_USE_MATRIX", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "DRAW_UNIT_IDLE", .type = SNI_CONST_INT, .value.i = -1},
    {.name = "DRAW_UNIT_NONE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "ENABLE_GLOBAL_CUSTOM", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "EVENT_ALL", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "EVENT_CANCEL", .type = SNI_CONST_INT, .value.i = 36},
    {.name = "EVENT_CHILD_CHANGED", .type = SNI_CONST_INT, .value.i = 39},
    {.name = "EVENT_CHILD_CREATED", .type = SNI_CONST_INT, .value.i = 40},
    {.name = "EVENT_CHILD_DELETED", .type = SNI_CONST_INT, .value.i = 41},
    {.name = "EVENT_CLICKED", .type = SNI_CONST_INT, .value.i = 7},
    {.name = "EVENT_COLOR_FORMAT_CHANGED", .type = SNI_CONST_INT, .value.i = 52},
    {.name = "EVENT_COVER_CHECK", .type = SNI_CONST_INT, .value.i = 23},
    {.name = "EVENT_CREATE", .type = SNI_CONST_INT, .value.i = 37},
    {.name = "EVENT_DEFOCUSED", .type = SNI_CONST_INT, .value.i = 17},
    {.name = "EVENT_DELETE", .type = SNI_CONST_INT, .value.i = 38},
    {.name = "EVENT_DRAW_MAIN", .type = SNI_CONST_INT, .value.i = 26},
    {.name = "EVENT_DRAW_MAIN_BEGIN", .type = SNI_CONST_INT, .value.i = 25},
    {.name = "EVENT_DRAW_MAIN_END", .type = SNI_CONST_INT, .value.i = 27},
    {.name = "EVENT_DRAW_POST", .type = SNI_CONST_INT, .value.i = 29},
    {.name = "EVENT_DRAW_POST_BEGIN", .type = SNI_CONST_INT, .value.i = 28},
    {.name = "EVENT_DRAW_POST_END", .type = SNI_CONST_INT, .value.i = 30},
    {.name = "EVENT_DRAW_TASK_ADDED", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "EVENT_FLUSH_FINISH", .type = SNI_CONST_INT, .value.i = 59},
    {.name = "EVENT_FLUSH_START", .type = SNI_CONST_INT, .value.i = 58},
    {.name = "EVENT_FLUSH_WAIT_FINISH", .type = SNI_CONST_INT, .value.i = 61},
    {.name = "EVENT_FLUSH_WAIT_START", .type = SNI_CONST_INT, .value.i = 60},
    {.name = "EVENT_FOCUSED", .type = SNI_CONST_INT, .value.i = 16},
    {.name = "EVENT_GESTURE", .type = SNI_CONST_INT, .value.i = 13},
    {.name = "EVENT_GET_SELF_SIZE", .type = SNI_CONST_INT, .value.i = 49},
    {.name = "EVENT_HIT_TEST", .type = SNI_CONST_INT, .value.i = 19},
    {.name = "EVENT_HOVER_LEAVE", .type = SNI_CONST_INT, .value.i = 22},
    {.name = "EVENT_HOVER_OVER", .type = SNI_CONST_INT, .value.i = 21},
    {.name = "EVENT_INDEV_RESET", .type = SNI_CONST_INT, .value.i = 20},
    {.name = "EVENT_INSERT", .type = SNI_CONST_INT, .value.i = 33},
    {.name = "EVENT_INVALIDATE_AREA", .type = SNI_CONST_INT, .value.i = 50},
    {.name = "EVENT_KEY", .type = SNI_CONST_INT, .value.i = 14},
    {.name = "EVENT_LAST", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "EVENT_LAYOUT_CHANGED", .type = SNI_CONST_INT, .value.i = 48},
    {.name = "EVENT_LEAVE", .type = SNI_CONST_INT, .value.i = 18},
    {.name = "EVENT_LONG_PRESSED", .type = SNI_CONST_INT, .value.i = 5},
    {.name = "EVENT_LONG_PRESSED_REPEAT", .type = SNI_CONST_INT, .value.i = 6},
    {.name = "EVENT_PREPROCESS", .type = SNI_CONST_INT, .value.i = 32768},
    {.name = "EVENT_PRESSED", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "EVENT_PRESSING", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "EVENT_PRESS_LOST", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "EVENT_READY", .type = SNI_CONST_INT, .value.i = 35},
    {.name = "EVENT_REFRESH", .type = SNI_CONST_INT, .value.i = 34},
    {.name = "EVENT_REFR_EXT_DRAW_SIZE", .type = SNI_CONST_INT, .value.i = 24},
    {.name = "EVENT_REFR_READY", .type = SNI_CONST_INT, .value.i = 55},
    {.name = "EVENT_REFR_REQUEST", .type = SNI_CONST_INT, .value.i = 53},
    {.name = "EVENT_REFR_START", .type = SNI_CONST_INT, .value.i = 54},
    {.name = "EVENT_RELEASED", .type = SNI_CONST_INT, .value.i = 8},
    {.name = "EVENT_RENDER_READY", .type = SNI_CONST_INT, .value.i = 57},
    {.name = "EVENT_RENDER_START", .type = SNI_CONST_INT, .value.i = 56},
    {.name = "EVENT_RESOLUTION_CHANGED", .type = SNI_CONST_INT, .value.i = 51},
    {.name = "EVENT_ROTARY", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "EVENT_SCREEN_LOADED", .type = SNI_CONST_INT, .value.i = 44},
    {.name = "EVENT_SCREEN_LOAD_START", .type = SNI_CONST_INT, .value.i = 43},
    {.name = "EVENT_SCREEN_UNLOADED", .type = SNI_CONST_INT, .value.i = 45},
    {.name = "EVENT_SCREEN_UNLOAD_START", .type = SNI_CONST_INT, .value.i = 42},
    {.name = "EVENT_SCROLL", .type = SNI_CONST_INT, .value.i = 12},
    {.name = "EVENT_SCROLL_BEGIN", .type = SNI_CONST_INT, .value.i = 9},
    {.name = "EVENT_SCROLL_END", .type = SNI_CONST_INT, .value.i = 11},
    {.name = "EVENT_SCROLL_THROW_BEGIN", .type = SNI_CONST_INT, .value.i = 10},
    {.name = "EVENT_SHORT_CLICKED", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "EVENT_SIZE_CHANGED", .type = SNI_CONST_INT, .value.i = 46},
    {.name = "EVENT_STYLE_CHANGED", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "EVENT_VALUE_CHANGED", .type = SNI_CONST_INT, .value.i = 32},
    {.name = "EVENT_VSYNC", .type = SNI_CONST_INT, .value.i = 62},
    {.name = "FLEX_ALIGN_CENTER", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "FLEX_ALIGN_END", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "FLEX_ALIGN_SPACE_AROUND", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "FLEX_ALIGN_SPACE_BETWEEN", .type = SNI_CONST_INT, .value.i = 5},
    {.name = "FLEX_ALIGN_SPACE_EVENLY", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "FLEX_ALIGN_START", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FLEX_FLOW_COLUMN", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "FLEX_FLOW_COLUMN_REVERSE", .type = SNI_CONST_INT, .value.i = 9},
    {.name = "FLEX_FLOW_COLUMN_WRAP", .type = SNI_CONST_INT, .value.i = 5},
    {.name = "FLEX_FLOW_COLUMN_WRAP_REVERSE", .type = SNI_CONST_INT, .value.i = 13},
    {.name = "FLEX_FLOW_ROW", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FLEX_FLOW_ROW_REVERSE", .type = SNI_CONST_INT, .value.i = 8},
    {.name = "FLEX_FLOW_ROW_WRAP", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "FLEX_FLOW_ROW_WRAP_REVERSE", .type = SNI_CONST_INT, .value.i = 12},
    {.name = "FONT_FMT_TXT_CMAP_FORMAT0_FULL", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FONT_FMT_TXT_CMAP_FORMAT0_TINY", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "FONT_FMT_TXT_CMAP_SPARSE_FULL", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "FONT_FMT_TXT_CMAP_SPARSE_TINY", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "FONT_FMT_TXT_COMPRESSED", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "FONT_FMT_TXT_COMPRESSED_NO_PREFILTER", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "FONT_FMT_TXT_LARGE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FONT_FMT_TXT_PLAIN", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FONT_GLYPH_FORMAT_A1", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "FONT_GLYPH_FORMAT_A2", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "FONT_GLYPH_FORMAT_A4", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "FONT_GLYPH_FORMAT_A8", .type = SNI_CONST_INT, .value.i = 8},
    {.name = "FONT_GLYPH_FORMAT_CUSTOM", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FONT_GLYPH_FORMAT_IMAGE", .type = SNI_CONST_INT, .value.i = 9},
    {.name = "FONT_GLYPH_FORMAT_NONE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FONT_GLYPH_FORMAT_SVG", .type = SNI_CONST_INT, .value.i = 11},
    {.name = "FONT_GLYPH_FORMAT_VECTOR", .type = SNI_CONST_INT, .value.i = 10},
    {.name = "FONT_KERNING_NONE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "FONT_KERNING_NORMAL", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FONT_MONTSERRAT_10", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FONT_MONTSERRAT_12", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FONT_MONTSERRAT_14", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "FONT_MONTSERRAT_16", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FONT_MONTSERRAT_18", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FONT_MONTSERRAT_20", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FONT_MONTSERRAT_22", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FONT_MONTSERRAT_24", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FONT_MONTSERRAT_26", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FONT_MONTSERRAT_28", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FONT_MONTSERRAT_30", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FONT_MONTSERRAT_32", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FONT_MONTSERRAT_34", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FONT_MONTSERRAT_36", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FONT_MONTSERRAT_38", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FONT_MONTSERRAT_40", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FONT_MONTSERRAT_42", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FONT_MONTSERRAT_44", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FONT_MONTSERRAT_46", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FONT_MONTSERRAT_48", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FONT_MONTSERRAT_8", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FONT_SUBPX_BOTH", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "FONT_SUBPX_HOR", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "FONT_SUBPX_NONE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FONT_SUBPX_VER", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "FONT_UNSCII_16", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FONT_UNSCII_8", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FREETYPE_CACHE_FT_GLYPH_CNT", .type = SNI_CONST_INT, .value.i = 256},
    {.name = "FREETYPE_FONT_RENDER_MODE_BITMAP", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FREETYPE_FONT_RENDER_MODE_OUTLINE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "FREETYPE_FONT_STYLE_BOLD", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "FREETYPE_FONT_STYLE_ITALIC", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "FREETYPE_FONT_STYLE_NORMAL", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FREETYPE_OUTLINE_CONIC_TO", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "FREETYPE_OUTLINE_CUBIC_TO", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "FREETYPE_OUTLINE_END", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FREETYPE_OUTLINE_LINE_TO", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "FREETYPE_OUTLINE_MOVE_TO", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "FREETYPE_USE_LVGL_PORT", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FS_MAX_FN_LENGTH", .type = SNI_CONST_INT, .value.i = 64},
    {.name = "FS_MAX_PATH_LENGTH", .type = SNI_CONST_INT, .value.i = 256},
    {.name = "FS_MODE_RD", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "FS_MODE_WR", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "FS_RES_BUSY", .type = SNI_CONST_INT, .value.i = 7},
    {.name = "FS_RES_DENIED", .type = SNI_CONST_INT, .value.i = 6},
    {.name = "FS_RES_FS_ERR", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "FS_RES_FULL", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "FS_RES_HW_ERR", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "FS_RES_INV_PARAM", .type = SNI_CONST_INT, .value.i = 11},
    {.name = "FS_RES_LOCKED", .type = SNI_CONST_INT, .value.i = 5},
    {.name = "FS_RES_NOT_EX", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "FS_RES_NOT_IMP", .type = SNI_CONST_INT, .value.i = 9},
    {.name = "FS_RES_OK", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FS_RES_OUT_OF_MEM", .type = SNI_CONST_INT, .value.i = 10},
    {.name = "FS_RES_TOUT", .type = SNI_CONST_INT, .value.i = 8},
    {.name = "FS_RES_UNKNOWN", .type = SNI_CONST_INT, .value.i = 12},
    {.name = "FS_SEEK_CUR", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "FS_SEEK_END", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "FS_SEEK_SET", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "FT_MAX_MODULES", .type = SNI_CONST_INT, .value.i = 32},
    {.name = "FT_RENDER_POOL_SIZE", .type = SNI_CONST_INT, .value.i = 16384},
    {.name = "GIF_CACHE_DECODE_DATA", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "GRADIENT_MAX_STOPS", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "GRAD_DIR_CONICAL", .type = SNI_CONST_INT, .value.i = 5},
    {.name = "GRAD_DIR_HOR", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "GRAD_DIR_LINEAR", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "GRAD_DIR_NONE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "GRAD_DIR_RADIAL", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "GRAD_DIR_VER", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "GRAD_EXTEND_PAD", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "GRAD_EXTEND_REFLECT", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "GRAD_EXTEND_REPEAT", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "GRIDNAV_CTRL_HORIZONTAL_MOVE_ONLY", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "GRIDNAV_CTRL_NONE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "GRIDNAV_CTRL_ROLLOVER", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "GRIDNAV_CTRL_SCROLL_FIRST", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "GRIDNAV_CTRL_VERTICAL_MOVE_ONLY", .type = SNI_CONST_INT, .value.i = 8},
    {.name = "GRID_ALIGN_CENTER", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "GRID_ALIGN_END", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "GRID_ALIGN_SPACE_AROUND", .type = SNI_CONST_INT, .value.i = 5},
    {.name = "GRID_ALIGN_SPACE_BETWEEN", .type = SNI_CONST_INT, .value.i = 6},
    {.name = "GRID_ALIGN_SPACE_EVENLY", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "GRID_ALIGN_START", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "GRID_ALIGN_STRETCH", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "GROUP_REFOCUS_POLICY_NEXT", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "GROUP_REFOCUS_POLICY_PREV", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "IMAGEBUTTON_STATE_CHECKED_DISABLED", .type = SNI_CONST_INT, .value.i = 5},
    {.name = "IMAGEBUTTON_STATE_CHECKED_PRESSED", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "IMAGEBUTTON_STATE_CHECKED_RELEASED", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "IMAGEBUTTON_STATE_DISABLED", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "IMAGEBUTTON_STATE_NUM", .type = SNI_CONST_INT, .value.i = 6},
    {.name = "IMAGEBUTTON_STATE_PRESSED", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "IMAGEBUTTON_STATE_RELEASED", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "IMAGE_ALIGN_AUTO_TRANSFORM", .type = SNI_CONST_INT, .value.i = 10},
    {.name = "IMAGE_ALIGN_BOTTOM_LEFT", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "IMAGE_ALIGN_BOTTOM_MID", .type = SNI_CONST_INT, .value.i = 5},
    {.name = "IMAGE_ALIGN_BOTTOM_RIGHT", .type = SNI_CONST_INT, .value.i = 6},
    {.name = "IMAGE_ALIGN_CENTER", .type = SNI_CONST_INT, .value.i = 9},
    {.name = "IMAGE_ALIGN_DEFAULT", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "IMAGE_ALIGN_LEFT_MID", .type = SNI_CONST_INT, .value.i = 7},
    {.name = "IMAGE_ALIGN_RIGHT_MID", .type = SNI_CONST_INT, .value.i = 8},
    {.name = "IMAGE_ALIGN_STRETCH", .type = SNI_CONST_INT, .value.i = 11},
    {.name = "IMAGE_ALIGN_TILE", .type = SNI_CONST_INT, .value.i = 12},
    {.name = "IMAGE_ALIGN_TOP_LEFT", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "IMAGE_ALIGN_TOP_MID", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "IMAGE_ALIGN_TOP_RIGHT", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "IMAGE_COMPRESS_LZ4", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "IMAGE_COMPRESS_NONE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "IMAGE_COMPRESS_RLE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "IMAGE_FLAGS_ALLOCATED", .type = SNI_CONST_INT, .value.i = 16},
    {.name = "IMAGE_FLAGS_COMPRESSED", .type = SNI_CONST_INT, .value.i = 8},
    {.name = "IMAGE_FLAGS_MODIFIABLE", .type = SNI_CONST_INT, .value.i = 32},
    {.name = "IMAGE_FLAGS_PREMULTIPLIED", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "IMAGE_FLAGS_USER1", .type = SNI_CONST_INT, .value.i = 256},
    {.name = "IMAGE_FLAGS_USER2", .type = SNI_CONST_INT, .value.i = 512},
    {.name = "IMAGE_FLAGS_USER3", .type = SNI_CONST_INT, .value.i = 1024},
    {.name = "IMAGE_FLAGS_USER4", .type = SNI_CONST_INT, .value.i = 2048},
    {.name = "IMAGE_FLAGS_USER5", .type = SNI_CONST_INT, .value.i = 4096},
    {.name = "IMAGE_FLAGS_USER6", .type = SNI_CONST_INT, .value.i = 8192},
    {.name = "IMAGE_FLAGS_USER7", .type = SNI_CONST_INT, .value.i = 16384},
    {.name = "IMAGE_FLAGS_USER8", .type = SNI_CONST_INT, .value.i = 32768},
    {.name = "IMAGE_HEADER_CACHE_DEF_CNT", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "IMAGE_HEADER_MAGIC", .type = SNI_CONST_INT, .value.i = 25},
    {.name = "IMAGE_SRC_FILE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "IMAGE_SRC_SYMBOL", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "IMAGE_SRC_UNKNOWN", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "IMAGE_SRC_VARIABLE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "INDEV_MODE_EVENT", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "INDEV_MODE_NONE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "INDEV_MODE_TIMER", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "INDEV_STATE_PRESSED", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "INDEV_STATE_RELEASED", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "INDEV_TYPE_BUTTON", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "INDEV_TYPE_ENCODER", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "INDEV_TYPE_KEYPAD", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "INDEV_TYPE_NONE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "INDEV_TYPE_POINTER", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "INV_BUF_SIZE", .type = SNI_CONST_INT, .value.i = 32},
    {.name = "JD_FASTDECODE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "JD_FORMAT", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "JD_SZBUF", .type = SNI_CONST_INT, .value.i = 512},
    {.name = "JD_TBLCLIP", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "JD_USE_SCALE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "KEYBOARD_MODE_NUMBER", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "KEYBOARD_MODE_SPECIAL", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "KEYBOARD_MODE_TEXT_LOWER", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "KEYBOARD_MODE_TEXT_UPPER", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "KEYBOARD_MODE_USER_1", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "KEYBOARD_MODE_USER_2", .type = SNI_CONST_INT, .value.i = 5},
    {.name = "KEYBOARD_MODE_USER_3", .type = SNI_CONST_INT, .value.i = 6},
    {.name = "KEYBOARD_MODE_USER_4", .type = SNI_CONST_INT, .value.i = 7},
    {.name = "KEY_BACKSPACE", .type = SNI_CONST_INT, .value.i = 8},
    {.name = "KEY_DEL", .type = SNI_CONST_INT, .value.i = 7},
    {.name = "KEY_DOWN", .type = SNI_CONST_INT, .value.i = 18},
    {.name = "KEY_END", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "KEY_ENTER", .type = SNI_CONST_INT, .value.i = 10},
    {.name = "KEY_ESC", .type = SNI_CONST_INT, .value.i = 27},
    {.name = "KEY_HOME", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "KEY_LEFT", .type = SNI_CONST_INT, .value.i = 20},
    {.name = "KEY_NEXT", .type = SNI_CONST_INT, .value.i = 9},
    {.name = "KEY_PREV", .type = SNI_CONST_INT, .value.i = 11},
    {.name = "KEY_RIGHT", .type = SNI_CONST_INT, .value.i = 19},
    {.name = "KEY_UP", .type = SNI_CONST_INT, .value.i = 17},
    {.name = "LABEL_DEFAULT_TEXT", .type = SNI_CONST_STRING, .value.s = "Text"},
    {.name = "LABEL_DOT_NUM", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "LABEL_LONG_CLIP", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "LABEL_LONG_DOT", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "LABEL_LONG_SCROLL", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "LABEL_LONG_SCROLL_CIRCULAR", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "LABEL_LONG_WRAP", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "LAYER_TYPE_NONE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "LAYER_TYPE_SIMPLE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "LAYER_TYPE_TRANSFORM", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "LAYOUT_FLEX", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "LAYOUT_GRID", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "LAYOUT_LAST", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "LAYOUT_NONE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "LED_BRIGHT_MAX", .type = SNI_CONST_INT, .value.i = 255},
    {.name = "LED_BRIGHT_MIN", .type = SNI_CONST_INT, .value.i = 80},
    {.name = "LOG_LEVEL_ERROR", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "LOG_LEVEL_INFO", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "LOG_LEVEL_NONE", .type = SNI_CONST_INT, .value.i = 5},
    {.name = "LOG_LEVEL_NUM", .type = SNI_CONST_INT, .value.i = 6},
    {.name = "LOG_LEVEL_TRACE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "LOG_LEVEL_USER", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "LOG_LEVEL_WARN", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "LOG_PRINTF", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "LOG_TRACE_ANIM", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "LOG_TRACE_CACHE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "LOG_TRACE_DISP_REFR", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "LOG_TRACE_EVENT", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "LOG_TRACE_INDEV", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "LOG_TRACE_LAYOUT", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "LOG_TRACE_MEM", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "LOG_TRACE_OBJ_CREATE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "LOG_TRACE_TIMER", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "LOG_USE_FILE_LINE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "LOG_USE_TIMESTAMP", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "MASK_ID_INV", .type = SNI_CONST_INT, .value.i = -1},
    {.name = "MASK_MAX_NUM", .type = SNI_CONST_INT, .value.i = 16},
    {.name = "MEM_POOL_EXPAND_SIZE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "MENU_HEADER_BOTTOM_FIXED", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "MENU_HEADER_TOP_FIXED", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "MENU_HEADER_TOP_UNFIXED", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "MENU_ROOT_BACK_BUTTON_DISABLED", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "MENU_ROOT_BACK_BUTTON_ENABLED", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "OBJ_CLASS_EDITABLE_FALSE", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "OBJ_CLASS_EDITABLE_INHERIT", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "OBJ_CLASS_EDITABLE_TRUE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "OBJ_CLASS_GROUP_DEF_FALSE", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "OBJ_CLASS_GROUP_DEF_INHERIT", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "OBJ_CLASS_GROUP_DEF_TRUE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "OBJ_CLASS_THEME_INHERITABLE_FALSE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "OBJ_CLASS_THEME_INHERITABLE_TRUE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "OBJ_FLAG_ADV_HITTEST", .type = SNI_CONST_INT, .value.i = 65536},
    {.name = "OBJ_FLAG_CHECKABLE", .type = SNI_CONST_INT, .value.i = 8},
    {.name = "OBJ_FLAG_CLICKABLE", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "OBJ_FLAG_CLICK_FOCUSABLE", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "OBJ_FLAG_EVENT_BUBBLE", .type = SNI_CONST_INT, .value.i = 16384},
    {.name = "OBJ_FLAG_FLEX_IN_NEW_TRACK", .type = SNI_CONST_INT, .value.i = 2097152},
    {.name = "OBJ_FLAG_FLOATING", .type = SNI_CONST_INT, .value.i = 262144},
    {.name = "OBJ_FLAG_GESTURE_BUBBLE", .type = SNI_CONST_INT, .value.i = 32768},
    {.name = "OBJ_FLAG_HIDDEN", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "OBJ_FLAG_IGNORE_LAYOUT", .type = SNI_CONST_INT, .value.i = 131072},
    {.name = "OBJ_FLAG_LAYOUT_1", .type = SNI_CONST_INT, .value.i = 8388608},
    {.name = "OBJ_FLAG_LAYOUT_2", .type = SNI_CONST_INT, .value.i = 16777216},
    {.name = "OBJ_FLAG_OVERFLOW_VISIBLE", .type = SNI_CONST_INT, .value.i = 1048576},
    {.name = "OBJ_FLAG_PRESS_LOCK", .type = SNI_CONST_INT, .value.i = 8192},
    {.name = "OBJ_FLAG_SCROLLABLE", .type = SNI_CONST_INT, .value.i = 16},
    {.name = "OBJ_FLAG_SCROLL_CHAIN", .type = SNI_CONST_INT, .value.i = 768},
    {.name = "OBJ_FLAG_SCROLL_CHAIN_HOR", .type = SNI_CONST_INT, .value.i = 256},
    {.name = "OBJ_FLAG_SCROLL_CHAIN_VER", .type = SNI_CONST_INT, .value.i = 512},
    {.name = "OBJ_FLAG_SCROLL_ELASTIC", .type = SNI_CONST_INT, .value.i = 32},
    {.name = "OBJ_FLAG_SCROLL_MOMENTUM", .type = SNI_CONST_INT, .value.i = 64},
    {.name = "OBJ_FLAG_SCROLL_ONE", .type = SNI_CONST_INT, .value.i = 128},
    {.name = "OBJ_FLAG_SCROLL_ON_FOCUS", .type = SNI_CONST_INT, .value.i = 1024},
    {.name = "OBJ_FLAG_SCROLL_WITH_ARROW", .type = SNI_CONST_INT, .value.i = 2048},
    {.name = "OBJ_FLAG_SEND_DRAW_TASK_EVENTS", .type = SNI_CONST_INT, .value.i = 524288},
    {.name = "OBJ_FLAG_SNAPPABLE", .type = SNI_CONST_INT, .value.i = 4096},
    {.name = "OBJ_FLAG_USER_1", .type = SNI_CONST_INT, .value.i = 134217728},
    {.name = "OBJ_FLAG_USER_2", .type = SNI_CONST_INT, .value.i = 268435456},
    {.name = "OBJ_FLAG_USER_3", .type = SNI_CONST_INT, .value.i = 536870912},
    {.name = "OBJ_FLAG_USER_4", .type = SNI_CONST_INT, .value.i = 1073741824},
    {.name = "OBJ_FLAG_WIDGET_1", .type = SNI_CONST_INT, .value.i = 33554432},
    {.name = "OBJ_FLAG_WIDGET_2", .type = SNI_CONST_INT, .value.i = 67108864},
    {.name = "OBJ_POINT_TRANSFORM_FLAG_INVERSE", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "OBJ_POINT_TRANSFORM_FLAG_INVERSE_RECURSIVE", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "OBJ_POINT_TRANSFORM_FLAG_NONE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "OBJ_POINT_TRANSFORM_FLAG_RECURSIVE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "OBJ_STYLE_CACHE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "OBJ_TREE_WALK_END", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "OBJ_TREE_WALK_NEXT", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "OBJ_TREE_WALK_SKIP_CHILDREN", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "OPA_0", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "OPA_10", .type = SNI_CONST_INT, .value.i = 25},
    {.name = "OPA_100", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "OPA_20", .type = SNI_CONST_INT, .value.i = 51},
    {.name = "OPA_30", .type = SNI_CONST_INT, .value.i = 76},
    {.name = "OPA_40", .type = SNI_CONST_INT, .value.i = 102},
    {.name = "OPA_50", .type = SNI_CONST_INT, .value.i = 7},
    {.name = "OPA_60", .type = SNI_CONST_INT, .value.i = 153},
    {.name = "OPA_70", .type = SNI_CONST_INT, .value.i = 178},
    {.name = "OPA_80", .type = SNI_CONST_INT, .value.i = 204},
    {.name = "OPA_90", .type = SNI_CONST_INT, .value.i = 229},
    {.name = "OPA_COVER", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "OPA_MAX", .type = SNI_CONST_INT, .value.i = 253},
    {.name = "OPA_MIN", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "OPA_TRANSP", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "OS_CMSIS_RTOS2", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "OS_CUSTOM", .type = SNI_CONST_INT, .value.i = 255},
    {.name = "OS_FREERTOS", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "OS_MQX", .type = SNI_CONST_INT, .value.i = 6},
    {.name = "OS_NONE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "OS_PTHREAD", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "OS_RTTHREAD", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "OS_WINDOWS", .type = SNI_CONST_INT, .value.i = 5},
    {.name = "PALETTE_AMBER", .type = SNI_CONST_INT, .value.i = 13},
    {.name = "PALETTE_BLUE", .type = SNI_CONST_INT, .value.i = 5},
    {.name = "PALETTE_BLUE_GREY", .type = SNI_CONST_INT, .value.i = 17},
    {.name = "PALETTE_BROWN", .type = SNI_CONST_INT, .value.i = 16},
    {.name = "PALETTE_CYAN", .type = SNI_CONST_INT, .value.i = 7},
    {.name = "PALETTE_DEEP_ORANGE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "PALETTE_DEEP_PURPLE", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "PALETTE_GREEN", .type = SNI_CONST_INT, .value.i = 9},
    {.name = "PALETTE_GREY", .type = SNI_CONST_INT, .value.i = 18},
    {.name = "PALETTE_INDIGO", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "PALETTE_LAST", .type = SNI_CONST_INT, .value.i = 19},
    {.name = "PALETTE_LIGHT_BLUE", .type = SNI_CONST_INT, .value.i = 6},
    {.name = "PALETTE_LIGHT_GREEN", .type = SNI_CONST_INT, .value.i = 10},
    {.name = "PALETTE_LIME", .type = SNI_CONST_INT, .value.i = 11},
    {.name = "PALETTE_NONE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "PALETTE_ORANGE", .type = SNI_CONST_INT, .value.i = 14},
    {.name = "PALETTE_PINK", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "PALETTE_PURPLE", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "PALETTE_RED", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "PALETTE_TEAL", .type = SNI_CONST_INT, .value.i = 8},
    {.name = "PALETTE_YELLOW", .type = SNI_CONST_INT, .value.i = 12},
    {.name = "PART_ANY", .type = SNI_CONST_INT, .value.i = 983040},
    {.name = "PART_CURSOR", .type = SNI_CONST_INT, .value.i = 393216},
    {.name = "PART_CUSTOM_FIRST", .type = SNI_CONST_INT, .value.i = 524288},
    {.name = "PART_INDICATOR", .type = SNI_CONST_INT, .value.i = 131072},
    {.name = "PART_ITEMS", .type = SNI_CONST_INT, .value.i = 327680},
    {.name = "PART_KNOB", .type = SNI_CONST_INT, .value.i = 196608},
    {.name = "PART_MAIN", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "PART_SCROLLBAR", .type = SNI_CONST_INT, .value.i = 65536},
    {.name = "PART_SELECTED", .type = SNI_CONST_INT, .value.i = 262144},
    {.name = "PART_TEXTAREA_PLACEHOLDER", .type = SNI_CONST_INT, .value.i = 524288},
    {.name = "PRIX32", .type = SNI_CONST_STRING, .value.s = "X"},
    {.name = "PRId32", .type = SNI_CONST_STRING, .value.s = "d"},
    {.name = "PRIu32", .type = SNI_CONST_STRING, .value.s = "u"},
    {.name = "PRIx32", .type = SNI_CONST_STRING, .value.s = "x"},
    {.name = "RADIUS_CIRCLE", .type = SNI_CONST_INT, .value.i = 7},
    {.name = "RB_COLOR_BLACK", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "RB_COLOR_RED", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "RESULT_INVALID", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "RESULT_OK", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "ROLLER_MODE_INFINITE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "ROLLER_MODE_NORMAL", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "SCALE_LABEL_ENABLED_DEFAULT", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "SCALE_MAJOR_TICK_EVERY_DEFAULT", .type = SNI_CONST_INT, .value.i = 5},
    {.name = "SCALE_MODE_HORIZONTAL_BOTTOM", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "SCALE_MODE_HORIZONTAL_TOP", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "SCALE_MODE_LAST", .type = SNI_CONST_INT, .value.i = 6},
    {.name = "SCALE_MODE_ROUND_INNER", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "SCALE_MODE_ROUND_OUTER", .type = SNI_CONST_INT, .value.i = 5},
    {.name = "SCALE_MODE_VERTICAL_LEFT", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "SCALE_MODE_VERTICAL_RIGHT", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "SCALE_NONE", .type = SNI_CONST_INT, .value.i = 256},
    {.name = "SCALE_TOTAL_TICK_COUNT_DEFAULT", .type = SNI_CONST_INT, .value.i = 11},
    {.name = "SCROLLBAR_MODE_ACTIVE", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "SCROLLBAR_MODE_AUTO", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "SCROLLBAR_MODE_OFF", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "SCROLLBAR_MODE_ON", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "SCROLL_SNAP_CENTER", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "SCROLL_SNAP_END", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "SCROLL_SNAP_NONE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "SCROLL_SNAP_START", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "SCR_LOAD_ANIM_FADE_IN", .type = SNI_CONST_INT, .value.i = 9},
    {.name = "SCR_LOAD_ANIM_FADE_ON", .type = SNI_CONST_INT, .value.i = 9},
    {.name = "SCR_LOAD_ANIM_FADE_OUT", .type = SNI_CONST_INT, .value.i = 10},
    {.name = "SCR_LOAD_ANIM_MOVE_BOTTOM", .type = SNI_CONST_INT, .value.i = 8},
    {.name = "SCR_LOAD_ANIM_MOVE_LEFT", .type = SNI_CONST_INT, .value.i = 5},
    {.name = "SCR_LOAD_ANIM_MOVE_RIGHT", .type = SNI_CONST_INT, .value.i = 6},
    {.name = "SCR_LOAD_ANIM_MOVE_TOP", .type = SNI_CONST_INT, .value.i = 7},
    {.name = "SCR_LOAD_ANIM_NONE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "SCR_LOAD_ANIM_OUT_BOTTOM", .type = SNI_CONST_INT, .value.i = 14},
    {.name = "SCR_LOAD_ANIM_OUT_LEFT", .type = SNI_CONST_INT, .value.i = 11},
    {.name = "SCR_LOAD_ANIM_OUT_RIGHT", .type = SNI_CONST_INT, .value.i = 12},
    {.name = "SCR_LOAD_ANIM_OUT_TOP", .type = SNI_CONST_INT, .value.i = 13},
    {.name = "SCR_LOAD_ANIM_OVER_BOTTOM", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "SCR_LOAD_ANIM_OVER_LEFT", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "SCR_LOAD_ANIM_OVER_RIGHT", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "SCR_LOAD_ANIM_OVER_TOP", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "SLIDER_MODE_NORMAL", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "SLIDER_MODE_RANGE", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "SLIDER_MODE_SYMMETRICAL", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "SPAN_MODE_BREAK", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "SPAN_MODE_EXPAND", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "SPAN_MODE_FIXED", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "SPAN_MODE_LAST", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "SPAN_OVERFLOW_CLIP", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "SPAN_OVERFLOW_ELLIPSIS", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "SPAN_OVERFLOW_LAST", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "SPAN_SNIPPET_STACK_SIZE", .type = SNI_CONST_INT, .value.i = 64},
    {.name = "SPINBOX_MAX_DIGIT_COUNT", .type = SNI_CONST_INT, .value.i = 10},
    {.name = "STATE_ANY", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "STATE_CHECKED", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "STATE_DEFAULT", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "STATE_DISABLED", .type = SNI_CONST_INT, .value.i = 128},
    {.name = "STATE_EDITED", .type = SNI_CONST_INT, .value.i = 8},
    {.name = "STATE_FOCUSED", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "STATE_FOCUS_KEY", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "STATE_HOVERED", .type = SNI_CONST_INT, .value.i = 16},
    {.name = "STATE_PRESSED", .type = SNI_CONST_INT, .value.i = 32},
    {.name = "STATE_SCROLLED", .type = SNI_CONST_INT, .value.i = 64},
    {.name = "STATE_USER_1", .type = SNI_CONST_INT, .value.i = 4096},
    {.name = "STATE_USER_2", .type = SNI_CONST_INT, .value.i = 8192},
    {.name = "STATE_USER_3", .type = SNI_CONST_INT, .value.i = 16384},
    {.name = "STATE_USER_4", .type = SNI_CONST_INT, .value.i = 32768},
    {.name = "STBRP__MAXVAL", .type = SNI_CONST_INT, .value.i = 7},
    {.name = "STBTT_MACSTYLE_BOLD", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "STBTT_MACSTYLE_DONTCARE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "STBTT_MACSTYLE_ITALIC", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "STBTT_MACSTYLE_NONE", .type = SNI_CONST_INT, .value.i = 8},
    {.name = "STBTT_MACSTYLE_UNDERSCORE", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "STB_RECT_PACK_VERSION", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "STDLIB_BUILTIN", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "STDLIB_CLIB", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "STDLIB_CUSTOM", .type = SNI_CONST_INT, .value.i = 255},
    {.name = "STDLIB_MICROPYTHON", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "STDLIB_RTTHREAD", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "STRIDE_AUTO", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "STYLE_ALIGN", .type = SNI_CONST_INT, .value.i = 10},
    {.name = "STYLE_ANIM", .type = SNI_CONST_INT, .value.i = 99},
    {.name = "STYLE_ANIM_DURATION", .type = SNI_CONST_INT, .value.i = 100},
    {.name = "STYLE_ARC_COLOR", .type = SNI_CONST_INT, .value.i = 82},
    {.name = "STYLE_ARC_IMAGE_SRC", .type = SNI_CONST_INT, .value.i = 84},
    {.name = "STYLE_ARC_OPA", .type = SNI_CONST_INT, .value.i = 83},
    {.name = "STYLE_ARC_ROUNDED", .type = SNI_CONST_INT, .value.i = 81},
    {.name = "STYLE_ARC_WIDTH", .type = SNI_CONST_INT, .value.i = 80},
    {.name = "STYLE_BASE_DIR", .type = SNI_CONST_INT, .value.i = 39},
    {.name = "STYLE_BG_COLOR", .type = SNI_CONST_INT, .value.i = 28},
    {.name = "STYLE_BG_GRAD", .type = SNI_CONST_INT, .value.i = 38},
    {.name = "STYLE_BG_GRAD_COLOR", .type = SNI_CONST_INT, .value.i = 35},
    {.name = "STYLE_BG_GRAD_DIR", .type = SNI_CONST_INT, .value.i = 32},
    {.name = "STYLE_BG_GRAD_OPA", .type = SNI_CONST_INT, .value.i = 37},
    {.name = "STYLE_BG_GRAD_STOP", .type = SNI_CONST_INT, .value.i = 34},
    {.name = "STYLE_BG_IMAGE_OPA", .type = SNI_CONST_INT, .value.i = 41},
    {.name = "STYLE_BG_IMAGE_RECOLOR", .type = SNI_CONST_INT, .value.i = 42},
    {.name = "STYLE_BG_IMAGE_RECOLOR_OPA", .type = SNI_CONST_INT, .value.i = 43},
    {.name = "STYLE_BG_IMAGE_SRC", .type = SNI_CONST_INT, .value.i = 40},
    {.name = "STYLE_BG_IMAGE_TILED", .type = SNI_CONST_INT, .value.i = 44},
    {.name = "STYLE_BG_MAIN_OPA", .type = SNI_CONST_INT, .value.i = 36},
    {.name = "STYLE_BG_MAIN_STOP", .type = SNI_CONST_INT, .value.i = 33},
    {.name = "STYLE_BG_OPA", .type = SNI_CONST_INT, .value.i = 29},
    {.name = "STYLE_BITMAP_MASK_SRC", .type = SNI_CONST_INT, .value.i = 115},
    {.name = "STYLE_BLEND_MODE", .type = SNI_CONST_INT, .value.i = 103},
    {.name = "STYLE_BORDER_COLOR", .type = SNI_CONST_INT, .value.i = 49},
    {.name = "STYLE_BORDER_OPA", .type = SNI_CONST_INT, .value.i = 50},
    {.name = "STYLE_BORDER_POST", .type = SNI_CONST_INT, .value.i = 53},
    {.name = "STYLE_BORDER_SIDE", .type = SNI_CONST_INT, .value.i = 52},
    {.name = "STYLE_BORDER_WIDTH", .type = SNI_CONST_INT, .value.i = 48},
    {.name = "STYLE_CLIP_CORNER", .type = SNI_CONST_INT, .value.i = 45},
    {.name = "STYLE_COLOR_FILTER_DSC", .type = SNI_CONST_INT, .value.i = 97},
    {.name = "STYLE_COLOR_FILTER_OPA", .type = SNI_CONST_INT, .value.i = 98},
    {.name = "STYLE_FLEX_CROSS_PLACE", .type = SNI_CONST_INT, .value.i = 7},
    {.name = "STYLE_FLEX_FLOW", .type = SNI_CONST_INT, .value.i = 125},
    {.name = "STYLE_FLEX_GROW", .type = SNI_CONST_INT, .value.i = 129},
    {.name = "STYLE_FLEX_MAIN_PLACE", .type = SNI_CONST_INT, .value.i = 126},
    {.name = "STYLE_FLEX_TRACK_PLACE", .type = SNI_CONST_INT, .value.i = 128},
    {.name = "STYLE_GRID_CELL_COLUMN_POS", .type = SNI_CONST_INT, .value.i = 134},
    {.name = "STYLE_GRID_CELL_COLUMN_SPAN", .type = SNI_CONST_INT, .value.i = 135},
    {.name = "STYLE_GRID_CELL_ROW_POS", .type = SNI_CONST_INT, .value.i = 137},
    {.name = "STYLE_GRID_CELL_ROW_SPAN", .type = SNI_CONST_INT, .value.i = 138},
    {.name = "STYLE_GRID_CELL_X_ALIGN", .type = SNI_CONST_INT, .value.i = 136},
    {.name = "STYLE_GRID_CELL_Y_ALIGN", .type = SNI_CONST_INT, .value.i = 139},
    {.name = "STYLE_GRID_COLUMN_ALIGN", .type = SNI_CONST_INT, .value.i = 130},
    {.name = "STYLE_GRID_COLUMN_DSC_ARRAY", .type = SNI_CONST_INT, .value.i = 133},
    {.name = "STYLE_GRID_ROW_ALIGN", .type = SNI_CONST_INT, .value.i = 131},
    {.name = "STYLE_GRID_ROW_DSC_ARRAY", .type = SNI_CONST_INT, .value.i = 132},
    {.name = "STYLE_HEIGHT", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "STYLE_IMAGE_OPA", .type = SNI_CONST_INT, .value.i = 68},
    {.name = "STYLE_IMAGE_RECOLOR", .type = SNI_CONST_INT, .value.i = 69},
    {.name = "STYLE_IMAGE_RECOLOR_OPA", .type = SNI_CONST_INT, .value.i = 70},
    {.name = "STYLE_LAST_BUILT_IN_PROP", .type = SNI_CONST_INT, .value.i = 140},
    {.name = "STYLE_LAYOUT", .type = SNI_CONST_INT, .value.i = 22},
    {.name = "STYLE_LENGTH", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "STYLE_LINE_COLOR", .type = SNI_CONST_INT, .value.i = 76},
    {.name = "STYLE_LINE_DASH_GAP", .type = SNI_CONST_INT, .value.i = 74},
    {.name = "STYLE_LINE_DASH_WIDTH", .type = SNI_CONST_INT, .value.i = 73},
    {.name = "STYLE_LINE_OPA", .type = SNI_CONST_INT, .value.i = 77},
    {.name = "STYLE_LINE_ROUNDED", .type = SNI_CONST_INT, .value.i = 75},
    {.name = "STYLE_LINE_WIDTH", .type = SNI_CONST_INT, .value.i = 72},
    {.name = "STYLE_MARGIN_BOTTOM", .type = SNI_CONST_INT, .value.i = 25},
    {.name = "STYLE_MARGIN_LEFT", .type = SNI_CONST_INT, .value.i = 26},
    {.name = "STYLE_MARGIN_RIGHT", .type = SNI_CONST_INT, .value.i = 27},
    {.name = "STYLE_MARGIN_TOP", .type = SNI_CONST_INT, .value.i = 24},
    {.name = "STYLE_MAX_HEIGHT", .type = SNI_CONST_INT, .value.i = 7},
    {.name = "STYLE_MAX_WIDTH", .type = SNI_CONST_INT, .value.i = 5},
    {.name = "STYLE_MIN_HEIGHT", .type = SNI_CONST_INT, .value.i = 6},
    {.name = "STYLE_MIN_WIDTH", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "STYLE_NUM_BUILT_IN_PROPS", .type = SNI_CONST_INT, .value.i = 141},
    {.name = "STYLE_OPA", .type = SNI_CONST_INT, .value.i = 5},
    {.name = "STYLE_OPA_LAYERED", .type = SNI_CONST_INT, .value.i = 96},
    {.name = "STYLE_OUTLINE_COLOR", .type = SNI_CONST_INT, .value.i = 57},
    {.name = "STYLE_OUTLINE_OPA", .type = SNI_CONST_INT, .value.i = 58},
    {.name = "STYLE_OUTLINE_PAD", .type = SNI_CONST_INT, .value.i = 59},
    {.name = "STYLE_OUTLINE_WIDTH", .type = SNI_CONST_INT, .value.i = 56},
    {.name = "STYLE_PAD_BOTTOM", .type = SNI_CONST_INT, .value.i = 17},
    {.name = "STYLE_PAD_COLUMN", .type = SNI_CONST_INT, .value.i = 21},
    {.name = "STYLE_PAD_LEFT", .type = SNI_CONST_INT, .value.i = 18},
    {.name = "STYLE_PAD_RIGHT", .type = SNI_CONST_INT, .value.i = 19},
    {.name = "STYLE_PAD_ROW", .type = SNI_CONST_INT, .value.i = 20},
    {.name = "STYLE_PAD_TOP", .type = SNI_CONST_INT, .value.i = 16},
    {.name = "STYLE_PROP_ANY", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "STYLE_PROP_CONST", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "STYLE_PROP_FLAG_ALL", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "STYLE_PROP_FLAG_NONE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "STYLE_PROP_INV", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "STYLE_RADIUS", .type = SNI_CONST_INT, .value.i = 12},
    {.name = "STYLE_RES_FOUND", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "STYLE_RES_NOT_FOUND", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "STYLE_ROTARY_SENSITIVITY", .type = SNI_CONST_INT, .value.i = 116},
    {.name = "STYLE_SENTINEL_VALUE", .type = SNI_CONST_INT, .value.i = 2864434397},
    {.name = "STYLE_SHADOW_COLOR", .type = SNI_CONST_INT, .value.i = 61},
    {.name = "STYLE_SHADOW_OFFSET_X", .type = SNI_CONST_INT, .value.i = 64},
    {.name = "STYLE_SHADOW_OFFSET_Y", .type = SNI_CONST_INT, .value.i = 65},
    {.name = "STYLE_SHADOW_OPA", .type = SNI_CONST_INT, .value.i = 62},
    {.name = "STYLE_SHADOW_SPREAD", .type = SNI_CONST_INT, .value.i = 66},
    {.name = "STYLE_SHADOW_WIDTH", .type = SNI_CONST_INT, .value.i = 60},
    {.name = "STYLE_STATE_CMP_DIFF_DRAW_PAD", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "STYLE_STATE_CMP_DIFF_LAYOUT", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "STYLE_STATE_CMP_DIFF_REDRAW", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "STYLE_STATE_CMP_SAME", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "STYLE_TEXT_ALIGN", .type = SNI_CONST_INT, .value.i = 94},
    {.name = "STYLE_TEXT_COLOR", .type = SNI_CONST_INT, .value.i = 88},
    {.name = "STYLE_TEXT_DECOR", .type = SNI_CONST_INT, .value.i = 93},
    {.name = "STYLE_TEXT_FONT", .type = SNI_CONST_INT, .value.i = 90},
    {.name = "STYLE_TEXT_LETTER_SPACE", .type = SNI_CONST_INT, .value.i = 91},
    {.name = "STYLE_TEXT_LINE_SPACE", .type = SNI_CONST_INT, .value.i = 92},
    {.name = "STYLE_TEXT_OPA", .type = SNI_CONST_INT, .value.i = 89},
    {.name = "STYLE_TRANSFORM_HEIGHT", .type = SNI_CONST_INT, .value.i = 105},
    {.name = "STYLE_TRANSFORM_PIVOT_X", .type = SNI_CONST_INT, .value.i = 6},
    {.name = "STYLE_TRANSFORM_PIVOT_Y", .type = SNI_CONST_INT, .value.i = 112},
    {.name = "STYLE_TRANSFORM_ROTATION", .type = SNI_CONST_INT, .value.i = 110},
    {.name = "STYLE_TRANSFORM_SCALE_X", .type = SNI_CONST_INT, .value.i = 108},
    {.name = "STYLE_TRANSFORM_SCALE_Y", .type = SNI_CONST_INT, .value.i = 109},
    {.name = "STYLE_TRANSFORM_SKEW_X", .type = SNI_CONST_INT, .value.i = 113},
    {.name = "STYLE_TRANSFORM_SKEW_Y", .type = SNI_CONST_INT, .value.i = 114},
    {.name = "STYLE_TRANSFORM_WIDTH", .type = SNI_CONST_INT, .value.i = 104},
    {.name = "STYLE_TRANSITION", .type = SNI_CONST_INT, .value.i = 102},
    {.name = "STYLE_TRANSLATE_X", .type = SNI_CONST_INT, .value.i = 106},
    {.name = "STYLE_TRANSLATE_Y", .type = SNI_CONST_INT, .value.i = 107},
    {.name = "STYLE_WIDTH", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "STYLE_X", .type = SNI_CONST_INT, .value.i = 8},
    {.name = "STYLE_Y", .type = SNI_CONST_INT, .value.i = 9},
    {.name = "SUBJECT_TYPE_COLOR", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "SUBJECT_TYPE_GROUP", .type = SNI_CONST_INT, .value.i = 5},
    {.name = "SUBJECT_TYPE_INT", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "SUBJECT_TYPE_INVALID", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "SUBJECT_TYPE_NONE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "SUBJECT_TYPE_POINTER", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "SUBJECT_TYPE_STRING", .type = SNI_CONST_INT, .value.i = 6},
    {.name = "SWITCH_KNOB_EXT_AREA_CORRECTION", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "SYMBOL_AUDIO", .type = SNI_CONST_STRING, .value.s = "\xEF\x80\x81"},
    {.name = "SYMBOL_BACKSPACE", .type = SNI_CONST_STRING, .value.s = "\xEF\x95\x9A"},
    {.name = "SYMBOL_BARS", .type = SNI_CONST_STRING, .value.s = "\xEF\x83\x89"},
    {.name = "SYMBOL_BATTERY_1", .type = SNI_CONST_STRING, .value.s = "\xEF\x89\x83"},
    {.name = "SYMBOL_BATTERY_2", .type = SNI_CONST_STRING, .value.s = "\xEF\x89\x82"},
    {.name = "SYMBOL_BATTERY_3", .type = SNI_CONST_STRING, .value.s = "\xEF\x89\x81"},
    {.name = "SYMBOL_BATTERY_EMPTY", .type = SNI_CONST_STRING, .value.s = "\xEF\x89\x84"},
    {.name = "SYMBOL_BATTERY_FULL", .type = SNI_CONST_STRING, .value.s = "\xEF\x89\x80"},
    {.name = "SYMBOL_BELL", .type = SNI_CONST_STRING, .value.s = "\xEF\x83\xB3"},
    {.name = "SYMBOL_BLUETOOTH", .type = SNI_CONST_STRING, .value.s = "\xEF\x8a\x93"},
    {.name = "SYMBOL_BULLET", .type = SNI_CONST_STRING, .value.s = "\xE2\x80\xA2"},
    {.name = "SYMBOL_CALL", .type = SNI_CONST_STRING, .value.s = "\xEF\x82\x95"},
    {.name = "SYMBOL_CHARGE", .type = SNI_CONST_STRING, .value.s = "\xEF\x83\xA7"},
    {.name = "SYMBOL_CLOSE", .type = SNI_CONST_STRING, .value.s = "\xEF\x80\x8D"},
    {.name = "SYMBOL_COPY", .type = SNI_CONST_STRING, .value.s = "\xEF\x83\x85"},
    {.name = "SYMBOL_CUT", .type = SNI_CONST_STRING, .value.s = "\xEF\x83\x84"},
    {.name = "SYMBOL_DIRECTORY", .type = SNI_CONST_STRING, .value.s = "\xEF\x81\xBB"},
    {.name = "SYMBOL_DOWN", .type = SNI_CONST_STRING, .value.s = "\xEF\x81\xB8"},
    {.name = "SYMBOL_DOWNLOAD", .type = SNI_CONST_STRING, .value.s = "\xEF\x80\x99"},
    {.name = "SYMBOL_DRIVE", .type = SNI_CONST_STRING, .value.s = "\xEF\x80\x9C"},
    {.name = "SYMBOL_DUMMY", .type = SNI_CONST_STRING, .value.s = "\xEF\xA3\xBF"},
    {.name = "SYMBOL_EDIT", .type = SNI_CONST_STRING, .value.s = "\xEF\x8C\x84"},
    {.name = "SYMBOL_EJECT", .type = SNI_CONST_STRING, .value.s = "\xEF\x81\x92"},
    {.name = "SYMBOL_ENVELOPE", .type = SNI_CONST_STRING, .value.s = "\xEF\x83\xA0"},
    {.name = "SYMBOL_EYE_CLOSE", .type = SNI_CONST_STRING, .value.s = "\xEF\x81\xB0"},
    {.name = "SYMBOL_EYE_OPEN", .type = SNI_CONST_STRING, .value.s = "\xEF\x81\xAE"},
    {.name = "SYMBOL_FILE", .type = SNI_CONST_STRING, .value.s = "\xEF\x85\x9B"},
    {.name = "SYMBOL_GPS", .type = SNI_CONST_STRING, .value.s = "\xEF\x84\xA4"},
    {.name = "SYMBOL_HOME", .type = SNI_CONST_STRING, .value.s = "\xEF\x80\x95"},
    {.name = "SYMBOL_IMAGE", .type = SNI_CONST_STRING, .value.s = "\xEF\x80\xBE"},
    {.name = "SYMBOL_KEYBOARD", .type = SNI_CONST_STRING, .value.s = "\xEF\x84\x9C"},
    {.name = "SYMBOL_LEFT", .type = SNI_CONST_STRING, .value.s = "\xEF\x81\x93"},
    {.name = "SYMBOL_LIST", .type = SNI_CONST_STRING, .value.s = "\xEF\x80\x8B"},
    {.name = "SYMBOL_LOOP", .type = SNI_CONST_STRING, .value.s = "\xEF\x81\xB9"},
    {.name = "SYMBOL_MINUS", .type = SNI_CONST_STRING, .value.s = "\xEF\x81\xA8"},
    {.name = "SYMBOL_MUTE", .type = SNI_CONST_STRING, .value.s = "\xEF\x80\xA6"},
    {.name = "SYMBOL_NEW_LINE", .type = SNI_CONST_STRING, .value.s = "\xEF\xA2\xA2"},
    {.name = "SYMBOL_NEXT", .type = SNI_CONST_STRING, .value.s = "\xEF\x81\x91"},
    {.name = "SYMBOL_OK", .type = SNI_CONST_STRING, .value.s = "\xEF\x80\x8C"},
    {.name = "SYMBOL_PASTE", .type = SNI_CONST_STRING, .value.s = "\xEF\x83\xAA"},
    {.name = "SYMBOL_PAUSE", .type = SNI_CONST_STRING, .value.s = "\xEF\x81\x8C"},
    {.name = "SYMBOL_PLAY", .type = SNI_CONST_STRING, .value.s = "\xEF\x81\x8B"},
    {.name = "SYMBOL_PLUS", .type = SNI_CONST_STRING, .value.s = "\xEF\x81\xA7"},
    {.name = "SYMBOL_POWER", .type = SNI_CONST_STRING, .value.s = "\xEF\x80\x91"},
    {.name = "SYMBOL_PREV", .type = SNI_CONST_STRING, .value.s = "\xEF\x81\x88"},
    {.name = "SYMBOL_REFRESH", .type = SNI_CONST_STRING, .value.s = "\xEF\x80\xA1"},
    {.name = "SYMBOL_RIGHT", .type = SNI_CONST_STRING, .value.s = "\xEF\x81\x94"},
    {.name = "SYMBOL_SAVE", .type = SNI_CONST_STRING, .value.s = "\xEF\x83\x87"},
    {.name = "SYMBOL_SD_CARD", .type = SNI_CONST_STRING, .value.s = "\xEF\x9F\x82"},
    {.name = "SYMBOL_SETTINGS", .type = SNI_CONST_STRING, .value.s = "\xEF\x80\x93"},
    {.name = "SYMBOL_SHUFFLE", .type = SNI_CONST_STRING, .value.s = "\xEF\x81\xB4"},
    {.name = "SYMBOL_STOP", .type = SNI_CONST_STRING, .value.s = "\xEF\x81\x8D"},
    {.name = "SYMBOL_TINT", .type = SNI_CONST_STRING, .value.s = "\xEF\x81\x83"},
    {.name = "SYMBOL_TRASH", .type = SNI_CONST_STRING, .value.s = "\xEF\x8B\xAD"},
    {.name = "SYMBOL_UP", .type = SNI_CONST_STRING, .value.s = "\xEF\x81\xB7"},
    {.name = "SYMBOL_UPLOAD", .type = SNI_CONST_STRING, .value.s = "\xEF\x82\x93"},
    {.name = "SYMBOL_USB", .type = SNI_CONST_STRING, .value.s = "\xEF\x8a\x87"},
    {.name = "SYMBOL_VIDEO", .type = SNI_CONST_STRING, .value.s = "\xEF\x80\x88"},
    {.name = "SYMBOL_VOLUME_MAX", .type = SNI_CONST_STRING, .value.s = "\xEF\x80\xA8"},
    {.name = "SYMBOL_VOLUME_MID", .type = SNI_CONST_STRING, .value.s = "\xEF\x80\xA7"},
    {.name = "SYMBOL_WARNING", .type = SNI_CONST_STRING, .value.s = "\xEF\x81\xB1"},
    {.name = "SYMBOL_WIFI", .type = SNI_CONST_STRING, .value.s = "\xEF\x87\xAB"},
    {.name = "T1_MAX_CHARSTRINGS_OPERANDS", .type = SNI_CONST_INT, .value.i = 256},
    {.name = "T1_MAX_DICT_DEPTH", .type = SNI_CONST_INT, .value.i = 5},
    {.name = "T1_MAX_SUBRS_CALLS", .type = SNI_CONST_INT, .value.i = 16},
    {.name = "TABLE_CELL_CTRL_CUSTOM_1", .type = SNI_CONST_INT, .value.i = 16},
    {.name = "TABLE_CELL_CTRL_CUSTOM_2", .type = SNI_CONST_INT, .value.i = 32},
    {.name = "TABLE_CELL_CTRL_CUSTOM_3", .type = SNI_CONST_INT, .value.i = 64},
    {.name = "TABLE_CELL_CTRL_CUSTOM_4", .type = SNI_CONST_INT, .value.i = 128},
    {.name = "TABLE_CELL_CTRL_MERGE_RIGHT", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "TABLE_CELL_CTRL_TEXT_CROP", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "TEXT_ALIGN_AUTO", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "TEXT_ALIGN_CENTER", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "TEXT_ALIGN_LEFT", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "TEXT_ALIGN_RIGHT", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "TEXT_DECOR_NONE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "TEXT_DECOR_STRIKETHROUGH", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "TEXT_DECOR_UNDERLINE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "TEXT_FLAG_BREAK_ALL", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "TEXT_FLAG_EXPAND", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "TEXT_FLAG_FIT", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "TEXT_FLAG_NONE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "THEME_DEFAULT_DARK", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "THEME_DEFAULT_GROW", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "THEME_DEFAULT_TRANSITION_TIME", .type = SNI_CONST_INT, .value.i = 80},
    {.name = "THORVG_SW_RASTER_SUPPORT", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "THORVG_VERSION_STRING", .type = SNI_CONST_STRING, .value.s = "0.13.5"},
    {.name = "THREAD_PRIO_HIGH", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "THREAD_PRIO_HIGHEST", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "THREAD_PRIO_LOW", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "THREAD_PRIO_LOWEST", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "THREAD_PRIO_MID", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "TINY_TTF_CACHE_GLYPH_CNT", .type = SNI_CONST_INT, .value.i = 256},
    {.name = "TINY_TTF_FILE_SUPPORT", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "TRIGO_SHIFT", .type = SNI_CONST_INT, .value.i = 15},
    {.name = "TRIGO_SIN_MAX", .type = SNI_CONST_INT, .value.i = 32768},
    {.name = "TT_CONFIG_OPTION_MAX_RUNNABLE_OPCODES", .type = SNI_CONST_INT, .value.i = 1000000},
    {.name = "TT_CONFIG_OPTION_SUBPIXEL_HINTING", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "TXT_BREAK_CHARS", .type = SNI_CONST_STRING, .value.s = " ,.;:-_)]}"},
    {.name = "TXT_ENC_ASCII", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "TXT_ENC_UTF8", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "TXT_LINE_BREAK_LONG_LEN", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "TXT_LINE_BREAK_LONG_POST_MIN_LEN", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "TXT_LINE_BREAK_LONG_PRE_MIN_LEN", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "USE_ANIMIMG", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_ARABIC_PERSIAN_CHARS", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_ARC", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_BAR", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_BARCODE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_BIDI", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_BMP", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_BUTTON", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_BUTTONMATRIX", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_CALENDAR", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_CALENDAR_CHINESE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_CALENDAR_HEADER_ARROW", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_CALENDAR_HEADER_DROPDOWN", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_CANVAS", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_CHART", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_CHECKBOX", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_DEMO_BENCHMARK", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_DEMO_FLEX_LAYOUT", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_DEMO_KEYPAD_AND_ENCODER", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_DEMO_MULTILANG", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_DEMO_MUSIC", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_DEMO_RENDER", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_DEMO_SCROLL", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_DEMO_STRESS", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_DEMO_TRANSFORM", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_DEMO_VECTOR_GRAPHIC", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_DEMO_WIDGETS", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_DRAW_ARM2D_SYNC", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_DRAW_DAVE2D", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_DRAW_SDL", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_DRAW_SW", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_DRAW_SW_COMPLEX_GRADIENTS", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_DRAW_VGLITE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_DRAW_VG_LITE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_EVDEV", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_FFMPEG", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_FILE_EXPLORER", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_FLEX", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_FLOAT", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_FONT_COMPRESSED", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_FONT_PLACEHOLDER", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_FRAGMENT", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_FREETYPE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_FS_ARDUINO_ESP_LITTLEFS", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_FS_ARDUINO_SD", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_FS_FATFS", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_FS_LITTLEFS", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_FS_MEMFS", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_FS_POSIX", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_FS_STDIO", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_FS_WIN32", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_GIF", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_GRID", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_GRIDNAV", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_ILI9341", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_IMAGEBUTTON", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_IME_PINYIN", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_IMGFONT", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_KEYBOARD", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_LABEL", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_LAYER_DEBUG", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_LED", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_LIBINPUT", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_LIBJPEG_TURBO", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_LIBPNG", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_LINE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_LINUX_DRM", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_LINUX_FBDEV", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_LIST", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_LODEPNG", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_LOG", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_LZ4_EXTERNAL", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_LZ4_INTERNAL", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_MATRIX", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_MEM_MONITOR", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_MENU", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_MONKEY", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_MSGBOX", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_NATIVE_HELIUM_ASM", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_NUTTX", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_OBJ_ID", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_OBJ_ID_BUILTIN", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_OBJ_PROPERTY", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_OBJ_PROPERTY_NAME", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_OBSERVER", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_OPENGLES", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_PARALLEL_DRAW_DEBUG", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_PERF_MONITOR", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_PRIVATE_API", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_PROFILER", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_PXP", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_QNX", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_QRCODE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_REFR_DEBUG", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_RENESAS_GLCDC", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_RLE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_RLOTTIE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_SCALE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_SDL", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_SNAPSHOT", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_SPAN", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_SPINBOX", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_SPINNER", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_ST7735", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_ST7789", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_ST7796", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_SWITCH", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_SYSMON", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_TABLE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_TABVIEW", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_TFT_ESPI", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_THEME_DEFAULT", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_THEME_MONO", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_THEME_SIMPLE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_THORVG_EXTERNAL", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_THORVG_INTERNAL", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_TILEVIEW", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_TINY_TTF", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_TJPGD", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_VECTOR_GRAPHIC", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_VG_LITE_THORVG", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_WAYLAND", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_WIN", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "USE_WINDOWS", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "USE_X11", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "VGLITE_HEADER_VERSION", .type = SNI_CONST_INT, .value.i = 7},
    {.name = "VGLITE_VERSION_3_0", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "VGL_FALSE", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "VGL_TRUE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "VG_LITE_ERROR", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "VLC_OP_BREAK", .type = SNI_CONST_INT, .value.i = 10},
    {.name = "VLC_OP_CLOSE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "VLC_OP_CUBIC", .type = SNI_CONST_INT, .value.i = 8},
    {.name = "VLC_OP_CUBIC_REL", .type = SNI_CONST_INT, .value.i = 9},
    {.name = "VLC_OP_END", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "VLC_OP_HLINE", .type = SNI_CONST_INT, .value.i = 11},
    {.name = "VLC_OP_HLINE_REL", .type = SNI_CONST_INT, .value.i = 12},
    {.name = "VLC_OP_LCCWARC", .type = SNI_CONST_INT, .value.i = 23},
    {.name = "VLC_OP_LCCWARC_REL", .type = SNI_CONST_INT, .value.i = 24},
    {.name = "VLC_OP_LCWARC", .type = SNI_CONST_INT, .value.i = 25},
    {.name = "VLC_OP_LCWARC_REL", .type = SNI_CONST_INT, .value.i = 26},
    {.name = "VLC_OP_LINE", .type = SNI_CONST_INT, .value.i = 4},
    {.name = "VLC_OP_LINE_REL", .type = SNI_CONST_INT, .value.i = 5},
    {.name = "VLC_OP_MOVE", .type = SNI_CONST_INT, .value.i = 2},
    {.name = "VLC_OP_MOVE_REL", .type = SNI_CONST_INT, .value.i = 3},
    {.name = "VLC_OP_QUAD", .type = SNI_CONST_INT, .value.i = 6},
    {.name = "VLC_OP_QUAD_REL", .type = SNI_CONST_INT, .value.i = 7},
    {.name = "VLC_OP_SCCWARC", .type = SNI_CONST_INT, .value.i = 19},
    {.name = "VLC_OP_SCCWARC_REL", .type = SNI_CONST_INT, .value.i = 20},
    {.name = "VLC_OP_SCUBIC", .type = SNI_CONST_INT, .value.i = 17},
    {.name = "VLC_OP_SCUBIC_REL", .type = SNI_CONST_INT, .value.i = 18},
    {.name = "VLC_OP_SCWARC", .type = SNI_CONST_INT, .value.i = 21},
    {.name = "VLC_OP_SCWARC_REL", .type = SNI_CONST_INT, .value.i = 22},
    {.name = "VLC_OP_SQUAD", .type = SNI_CONST_INT, .value.i = 0},
    {.name = "VLC_OP_SQUAD_REL", .type = SNI_CONST_INT, .value.i = 16},
    {.name = "VLC_OP_VLINE", .type = SNI_CONST_INT, .value.i = 13},
    {.name = "VLC_OP_VLINE_REL", .type = SNI_CONST_INT, .value.i = 14},
    {.name = "WIDGETS_HAS_DEFAULT_VALUE", .type = SNI_CONST_INT, .value.i = 1},
    {.name = "ZERO_MEM_SENTINEL", .type = SNI_CONST_INT, .value.i = 2712847316},
    {.name = "qrcodegen_VERSION_MAX", .type = SNI_CONST_INT, .value.i = 40},
    {.name = "qrcodegen_VERSION_MIN", .type = SNI_CONST_INT, .value.i = 1},
    {.name = NULL, .type = SNI_CONST_INT, .value.i = 0},
};

const sni_class_desc_t lv_class_desc_obj = {
    .name = "obj",
    .constructor = sni_api_ctor_obj,
    .base_class = NULL,
    .methods = lv_class_methods_obj,
    .properties = lv_class_properties_obj,
    .static_methods = lv_class_static_methods_obj,
    .constants = lv_class_constants_obj,
};

const sni_class_desc_t lv_class_desc_button = {
    .name = "button",
    .constructor = sni_api_ctor_button,
    .base_class = &lv_class_desc_obj,
    .methods = lv_class_methods_button,
    .properties = lv_class_properties_button,
    .static_methods = lv_class_static_methods_button,
    .constants = lv_class_constants_button,
};

const sni_class_desc_t lv_class_desc_label = {
    .name = "label",
    .constructor = sni_api_ctor_label,
    .base_class = &lv_class_desc_obj,
    .methods = lv_class_methods_label,
    .properties = lv_class_properties_label,
    .static_methods = lv_class_static_methods_label,
    .constants = lv_class_constants_label,
};

const sni_class_desc_t lv_class_desc_screen = {
    .name = "screen",
    .constructor = NULL,
    .base_class = NULL,
    .methods = NULL,
    .properties = NULL,
    .static_methods = lv_class_static_methods_screen,
    .constants = lv_class_constants_screen,
};

const sni_class_desc_t lv_class_desc_color = {
    .name = "color",
    .constructor = NULL,
    .base_class = NULL,
    .methods = NULL,
    .properties = NULL,
    .static_methods = lv_class_static_methods_color,
    .constants = lv_class_constants_color,
};

const sni_class_desc_t *const lv_api_classes[] = {
    &lv_class_desc_obj,
    &lv_class_desc_button,
    &lv_class_desc_label,
    &lv_class_desc_screen,
    &lv_class_desc_color,
    NULL,
};

void sni_api_lv_init(void)
{
    lv_api_obj = sni_api_build(lv_api_classes);
    if (!sni_api_register_constants(lv_root_constants, lv_api_obj))
    {
        EOS_LOG_E("Failed to register LV root constants");
    }
}

void sni_api_lv_mount(jerry_value_t realm)
{
    bool result = sni_api_mount(realm, lv_api_obj, LV_API_NAME);
    if (!result)
    {
        EOS_LOG_E("Failed to mount LVGL API");
    }
}
