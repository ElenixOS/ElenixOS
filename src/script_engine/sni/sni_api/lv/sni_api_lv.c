/**
 * @file sni_api_lv.c
 * @brief LVGL API 导出
 * @author Auto Generated
 * @date 2026-03-13
 */

#include "sni_api_lv.h"

#include <stdio.h>
#include <stdlib.h>
#include "lvgl.h"
#include "sni_type_bridge.h"
#include "sni_types.h"
#include "sni_api_export.h"
#include "elena_os_log.h"

#define LV_API_NAME "lv"

static jerry_value_t lv_api_obj;

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

    if (!jerry_value_is_object(args_p[0]) && !jerry_value_is_null(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* arg_parent;
    if (jerry_value_is_null(args_p[0]))
    {
        arg_parent = NULL;
    }
    else if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &arg_parent))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_t* native_obj = lv_obj_create(arg_parent);
    lv_obj_set_pos(native_obj, 100, 100);
    lv_obj_set_style_bg_color(native_obj, lv_color_hex(0xFF0000), 0);
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
    return sni_tb_c2js(&result, SNI_V_LV_STATE);
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

    if (!jerry_value_is_object(args_p[0]) && !jerry_value_is_null(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* arg_parent;
    if (jerry_value_is_null(args_p[0]))
    {
        arg_parent = NULL;
    }
    else if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &arg_parent))
    {
        return sni_api_throw_error("Failed to convert argument");
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

    if (!jerry_value_is_object(args_p[0]) && !jerry_value_is_null(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* arg_parent;
    if (jerry_value_is_null(args_p[0]))
    {
        arg_parent = NULL;
    }
    else if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &arg_parent))
    {
        return sni_api_throw_error("Failed to convert argument");
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

const sni_method_desc_t lv_class_methods_obj[] = {
    {.name = "center", .handler = sni_api_lv_obj_center},
    {.name = NULL, .handler = NULL},
};

const sni_method_desc_t lv_class_static_methods_obj[] = {
    {.name = NULL, .handler = NULL},
};

const sni_property_desc_t lv_class_properties_obj[] = {
    {.name = "align", .getter = NULL, .setter = sni_api_prop_set_obj_align},
    {.name = "child_count", .getter = sni_api_prop_get_obj_child_count, .setter = NULL},
    {.name = "class", .getter = sni_api_prop_get_obj_class, .setter = NULL},
    {.name = "content_height", .getter = sni_api_prop_get_obj_content_height, .setter = sni_api_prop_set_obj_content_height},
    {.name = "content_width", .getter = sni_api_prop_get_obj_content_width, .setter = sni_api_prop_set_obj_content_width},
    {.name = "display", .getter = sni_api_prop_get_obj_display, .setter = NULL},
    {.name = "event_count", .getter = sni_api_prop_get_obj_event_count, .setter = NULL},
    {.name = "ext_click_area", .getter = NULL, .setter = sni_api_prop_set_obj_ext_click_area},
    {.name = "flex_flow", .getter = NULL, .setter = sni_api_prop_set_obj_flex_flow},
    {.name = "flex_grow", .getter = NULL, .setter = sni_api_prop_set_obj_flex_grow},
    {.name = "group", .getter = sni_api_prop_get_obj_group, .setter = NULL},
    {.name = "height", .getter = sni_api_prop_get_obj_height, .setter = sni_api_prop_set_obj_height},
    {.name = "index", .getter = sni_api_prop_get_obj_index, .setter = NULL},
    {.name = "layout", .getter = NULL, .setter = sni_api_prop_set_obj_layout},
    {.name = "parent", .getter = sni_api_prop_get_obj_parent, .setter = sni_api_prop_set_obj_parent},
    {.name = "screen", .getter = sni_api_prop_get_obj_screen, .setter = NULL},
    {.name = "scroll_bottom", .getter = sni_api_prop_get_obj_scroll_bottom, .setter = NULL},
    {.name = "scroll_dir", .getter = sni_api_prop_get_obj_scroll_dir, .setter = sni_api_prop_set_obj_scroll_dir},
    {.name = "scroll_left", .getter = sni_api_prop_get_obj_scroll_left, .setter = NULL},
    {.name = "scroll_right", .getter = sni_api_prop_get_obj_scroll_right, .setter = NULL},
    {.name = "scroll_snap_x", .getter = sni_api_prop_get_obj_scroll_snap_x, .setter = sni_api_prop_set_obj_scroll_snap_x},
    {.name = "scroll_snap_y", .getter = sni_api_prop_get_obj_scroll_snap_y, .setter = sni_api_prop_set_obj_scroll_snap_y},
    {.name = "scroll_top", .getter = sni_api_prop_get_obj_scroll_top, .setter = NULL},
    {.name = "scroll_x", .getter = sni_api_prop_get_obj_scroll_x, .setter = NULL},
    {.name = "scroll_y", .getter = sni_api_prop_get_obj_scroll_y, .setter = NULL},
    {.name = "scrollbar_mode", .getter = sni_api_prop_get_obj_scrollbar_mode, .setter = sni_api_prop_set_obj_scrollbar_mode},
    {.name = "self_height", .getter = sni_api_prop_get_obj_self_height, .setter = NULL},
    {.name = "self_width", .getter = sni_api_prop_get_obj_self_width, .setter = NULL},
    {.name = "state", .getter = sni_api_prop_get_obj_state, .setter = NULL},
    {.name = "user_data", .getter = sni_api_prop_get_obj_user_data, .setter = sni_api_prop_set_obj_user_data},
    {.name = "width", .getter = sni_api_prop_get_obj_width, .setter = sni_api_prop_set_obj_width},
    {.name = "x", .getter = sni_api_prop_get_obj_x, .setter = sni_api_prop_set_obj_x},
    {.name = "x2", .getter = sni_api_prop_get_obj_x2, .setter = NULL},
    {.name = "x_aligned", .getter = sni_api_prop_get_obj_x_aligned, .setter = NULL},
    {.name = "y", .getter = sni_api_prop_get_obj_y, .setter = sni_api_prop_set_obj_y},
    {.name = "y2", .getter = sni_api_prop_get_obj_y2, .setter = NULL},
    {.name = "y_aligned", .getter = sni_api_prop_get_obj_y_aligned, .setter = NULL},
    {.name = NULL, .getter = NULL, .setter = NULL},
};

const sni_constant_desc_t lv_class_constants_obj[] = {
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
    {.name = "long_mode", .getter = sni_api_prop_get_label_long_mode, .setter = sni_api_prop_set_label_long_mode},
    {.name = "text", .getter = sni_api_prop_get_label_text, .setter = sni_api_prop_set_label_text},
    {.name = "text_selection_end", .getter = sni_api_prop_get_label_text_selection_end, .setter = sni_api_prop_set_label_text_selection_end},
    {.name = "text_selection_start", .getter = sni_api_prop_get_label_text_selection_start, .setter = sni_api_prop_set_label_text_selection_start},
    {.name = "text_static", .getter = NULL, .setter = sni_api_prop_set_label_text_static},
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

const sni_class_desc_t *const lv_api_classes[] = {
    &lv_class_desc_obj,
    &lv_class_desc_button,
    &lv_class_desc_label,
    &lv_class_desc_screen,
    NULL,
};

void sni_api_lv_init(void)
{
    lv_api_obj = sni_api_build(lv_api_classes);
}

void sni_api_lv_mount(jerry_value_t realm)
{
    bool result = sni_api_mount(realm, lv_api_obj, LV_API_NAME);
    if (!result)
    {
        EOS_LOG_E("Failed to mount LVGL API");
    }
}
