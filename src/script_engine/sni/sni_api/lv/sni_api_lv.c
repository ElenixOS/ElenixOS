/**
 * @file sni_api_lv.c
 * @brief LVGL API 导出
 * @author Auto Generated
 * @date 2026-03-10
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

jerry_value_t sni_api_lv_button_create(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* parent;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &parent))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_t* result = lv_button_create(parent);
    return sni_tb_c2js(&result, SNI_H_LV_OBJ);
}

jerry_value_t sni_api_lv_obj_add_event_cb(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 4)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_event_cb_t event_cb;
    if (!sni_tb_js2c(args_p[1], SNI_H_LV_EVENT_CB, &event_cb))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_event_code_t filter;
    filter = sni_tb_js2c_int32(args_p[2]);

    if (!jerry_value_is_object(args_p[3]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    void* user_data;
    if (!sni_tb_js2c(args_p[3], SNI_T_PTR, &user_data))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_event_dsc_t* result = lv_obj_add_event_cb(obj, event_cb, filter, user_data);
    return sni_tb_c2js(&result, SNI_H_LV_EVENT_DSC);
}

jerry_value_t sni_api_lv_obj_add_flag(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_flag_t f;
    f = sni_tb_js2c_int32(args_p[1]);

    lv_obj_add_flag(obj, f);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_add_state(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_state_t state;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_STATE, &state))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_add_state(obj, state);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_add_style(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_style_t* style;
    if (!sni_tb_js2c(args_p[1], SNI_H_LV_STYLE, &style))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_add_style(obj, style, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_align(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 4)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_align_t align;
    align = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_number(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t x_ofs;
    x_ofs = sni_tb_js2c_int32(args_p[2]);

    if (!jerry_value_is_number(args_p[3]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t y_ofs;
    y_ofs = sni_tb_js2c_int32(args_p[3]);

    lv_obj_align(obj, align, x_ofs, y_ofs);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_align_to(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 5)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* base;
    if (!sni_tb_js2c(args_p[1], SNI_H_LV_OBJ, &base))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_align_t align;
    align = sni_tb_js2c_int32(args_p[2]);

    if (!jerry_value_is_number(args_p[3]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t x_ofs;
    x_ofs = sni_tb_js2c_int32(args_p[3]);

    if (!jerry_value_is_number(args_p[4]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t y_ofs;
    y_ofs = sni_tb_js2c_int32(args_p[4]);

    lv_obj_align_to(obj, base, align, x_ofs, y_ofs);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_allocate_spec_attr(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_allocate_spec_attr(obj);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_area_is_visible(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_area_t* area;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_AREA, &area))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    bool result = lv_obj_area_is_visible(obj, area);
    return sni_tb_c2js_boolean(result);
}

jerry_value_t sni_api_lv_obj_bind_checked(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_subject_t* subject;
    if (!sni_tb_js2c(args_p[1], SNI_H_LV_SUBJECT, &subject))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_observer_t* result = lv_obj_bind_checked(obj, subject);
    return sni_tb_c2js(&result, SNI_H_LV_OBSERVER);
}

jerry_value_t sni_api_lv_obj_bind_flag_if_eq(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 4)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_subject_t* subject;
    if (!sni_tb_js2c(args_p[1], SNI_H_LV_SUBJECT, &subject))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_flag_t flag;
    flag = sni_tb_js2c_int32(args_p[2]);

    if (!jerry_value_is_number(args_p[3]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t ref_value;
    ref_value = sni_tb_js2c_int32(args_p[3]);

    lv_observer_t* result = lv_obj_bind_flag_if_eq(obj, subject, flag, ref_value);
    return sni_tb_c2js(&result, SNI_H_LV_OBSERVER);
}

jerry_value_t sni_api_lv_obj_bind_flag_if_not_eq(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 4)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_subject_t* subject;
    if (!sni_tb_js2c(args_p[1], SNI_H_LV_SUBJECT, &subject))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_flag_t flag;
    flag = sni_tb_js2c_int32(args_p[2]);

    if (!jerry_value_is_number(args_p[3]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t ref_value;
    ref_value = sni_tb_js2c_int32(args_p[3]);

    lv_observer_t* result = lv_obj_bind_flag_if_not_eq(obj, subject, flag, ref_value);
    return sni_tb_c2js(&result, SNI_H_LV_OBSERVER);
}

jerry_value_t sni_api_lv_obj_bind_state_if_eq(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 4)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_subject_t* subject;
    if (!sni_tb_js2c(args_p[1], SNI_H_LV_SUBJECT, &subject))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_state_t state;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STATE, &state))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[3]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t ref_value;
    ref_value = sni_tb_js2c_int32(args_p[3]);

    lv_observer_t* result = lv_obj_bind_state_if_eq(obj, subject, state, ref_value);
    return sni_tb_c2js(&result, SNI_H_LV_OBSERVER);
}

jerry_value_t sni_api_lv_obj_bind_state_if_not_eq(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 4)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_subject_t* subject;
    if (!sni_tb_js2c(args_p[1], SNI_H_LV_SUBJECT, &subject))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_state_t state;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STATE, &state))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[3]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t ref_value;
    ref_value = sni_tb_js2c_int32(args_p[3]);

    lv_observer_t* result = lv_obj_bind_state_if_not_eq(obj, subject, state, ref_value);
    return sni_tb_c2js(&result, SNI_H_LV_OBSERVER);
}

jerry_value_t sni_api_lv_obj_calculate_ext_draw_size(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_calculate_ext_draw_size(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_calculate_style_text_align(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_string(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const char* txt;
    txt = sni_tb_js2c_string(args_p[2]);

    lv_text_align_t result = lv_obj_calculate_style_text_align(obj, part, txt);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_center(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_center(obj);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_check_type(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_class_t* class_p;
    if (!sni_tb_js2c(args_p[1], SNI_H_LV_OBJ_CLASS, &class_p))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    bool result = lv_obj_check_type(obj, class_p);
    return sni_tb_c2js_boolean(result);
}

jerry_value_t sni_api_lv_obj_class_create_obj(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_class_t* class_p;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ_CLASS, &class_p))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* parent;
    if (!sni_tb_js2c(args_p[1], SNI_H_LV_OBJ, &parent))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_t* result = lv_obj_class_create_obj(class_p, parent);
    return sni_tb_c2js(&result, SNI_H_LV_OBJ);
}

jerry_value_t sni_api_lv_obj_class_init_obj(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_class_init_obj(obj);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_clean(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_clean(obj);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_create(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* parent;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &parent))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_t* result = lv_obj_create(parent);
    return sni_tb_c2js(&result, SNI_H_LV_OBJ);
}

jerry_value_t sni_api_lv_obj_delete(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_delete(obj);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_delete_anim_completed_cb(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_anim_t* a;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_ANIM, &a))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_delete_anim_completed_cb(a);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_delete_async(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_delete_async(obj);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_delete_delayed(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    uint32_t delay_ms;
    delay_ms = sni_tb_js2c_uint32(args_p[1]);

    lv_obj_delete_delayed(obj, delay_ms);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_dump_tree(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* start_obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &start_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_dump_tree(start_obj);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_enable_style_refresh(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_boolean(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    bool en;
    en = sni_tb_js2c_boolean(args_p[0]);

    lv_obj_enable_style_refresh(en);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_event_base(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_class_t* class_p;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ_CLASS, &class_p))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_event_t* e;
    if (!sni_tb_js2c(args_p[1], SNI_H_LV_EVENT, &e))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_result_t result = lv_obj_event_base(class_p, e);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_fade_in(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    uint32_t time;
    time = sni_tb_js2c_uint32(args_p[1]);

    if (!jerry_value_is_number(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    uint32_t delay;
    delay = sni_tb_js2c_uint32(args_p[2]);

    lv_obj_fade_in(obj, time, delay);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_fade_out(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    uint32_t time;
    time = sni_tb_js2c_uint32(args_p[1]);

    if (!jerry_value_is_number(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    uint32_t delay;
    delay = sni_tb_js2c_uint32(args_p[2]);

    lv_obj_fade_out(obj, time, delay);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_get_child(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t idx;
    idx = sni_tb_js2c_int32(args_p[1]);

    lv_obj_t* result = lv_obj_get_child(obj, idx);
    return sni_tb_c2js(&result, SNI_H_LV_OBJ);
}

jerry_value_t sni_api_lv_obj_get_child_by_type(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t idx;
    idx = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_class_t* class_p;
    if (!sni_tb_js2c(args_p[2], SNI_H_LV_OBJ_CLASS, &class_p))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_t* result = lv_obj_get_child_by_type(obj, idx, class_p);
    return sni_tb_c2js(&result, SNI_H_LV_OBJ);
}

jerry_value_t sni_api_lv_obj_get_child_count(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    uint32_t result = lv_obj_get_child_count(obj);
    return sni_tb_c2js(&result, SNI_T_UINT32);
}

jerry_value_t sni_api_lv_obj_get_child_count_by_type(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_class_t* class_p;
    if (!sni_tb_js2c(args_p[1], SNI_H_LV_OBJ_CLASS, &class_p))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    uint32_t result = lv_obj_get_child_count_by_type(obj, class_p);
    return sni_tb_c2js(&result, SNI_T_UINT32);
}

jerry_value_t sni_api_lv_obj_get_class(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    const lv_obj_class_t* result = lv_obj_get_class(obj);
    return sni_tb_c2js(&result, SNI_H_LV_OBJ_CLASS);
}

jerry_value_t sni_api_lv_obj_get_click_area(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_area_t* area;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_AREA, &area))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_get_click_area(obj, area);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_get_content_coords(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_area_t* area;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_AREA, &area))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_get_content_coords(obj, area);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_get_content_height(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_content_height(obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_coords(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_area_t* coords;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_AREA, &coords))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_get_coords(obj, coords);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_get_display(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_display_t* result = lv_obj_get_display(obj);
    return sni_tb_c2js(&result, SNI_H_LV_DISPLAY);
}

jerry_value_t sni_api_lv_obj_get_event_count(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    uint32_t result = lv_obj_get_event_count(obj);
    return sni_tb_c2js(&result, SNI_T_UINT32);
}

jerry_value_t sni_api_lv_obj_get_event_dsc(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    uint32_t index;
    index = sni_tb_js2c_uint32(args_p[1]);

    lv_event_dsc_t* result = lv_obj_get_event_dsc(obj, index);
    return sni_tb_c2js(&result, SNI_H_LV_EVENT_DSC);
}

jerry_value_t sni_api_lv_obj_get_group(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_group_t* result = lv_obj_get_group(obj);
    return sni_tb_c2js(&result, SNI_H_LV_GROUP);
}

jerry_value_t sni_api_lv_obj_get_height(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_height(obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_index(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_index(obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_index_by_type(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_class_t* class_p;
    if (!sni_tb_js2c(args_p[1], SNI_H_LV_OBJ_CLASS, &class_p))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_index_by_type(obj, class_p);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_local_style_prop(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 4)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_prop_t prop;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_STYLE_PROP, &prop))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_value_t* value;
    if (!sni_tb_js2c(args_p[2], SNI_H_LV_STYLE_VALUE, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[3]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[3], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_style_res_t result = lv_obj_get_local_style_prop(obj, prop, value, selector);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_parent(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_t* result = lv_obj_get_parent(obj);
    return sni_tb_c2js(&result, SNI_H_LV_OBJ);
}

jerry_value_t sni_api_lv_obj_get_screen(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_t* result = lv_obj_get_screen(obj);
    return sni_tb_c2js(&result, SNI_H_LV_OBJ);
}

jerry_value_t sni_api_lv_obj_get_scroll_bottom(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_scroll_bottom(obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_scroll_dir(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_dir_t result = lv_obj_get_scroll_dir(obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_scroll_end(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_point_t* end;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_POINT, &end))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_get_scroll_end(obj, end);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_get_scroll_left(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_scroll_left(obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_scroll_right(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_scroll_right(obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_scroll_snap_x(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_scroll_snap_t result = lv_obj_get_scroll_snap_x(obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_scroll_snap_y(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_scroll_snap_t result = lv_obj_get_scroll_snap_y(obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_scroll_top(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_scroll_top(obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_scroll_x(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_scroll_x(obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_scroll_y(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_scroll_y(obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_scrollbar_area(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_area_t* hor;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_AREA, &hor))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_area_t* ver;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_AREA, &ver))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_get_scrollbar_area(obj, hor, ver);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_get_scrollbar_mode(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_scrollbar_mode_t result = lv_obj_get_scrollbar_mode(obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_self_height(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_self_height(obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_sibling(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t idx;
    idx = sni_tb_js2c_int32(args_p[1]);

    lv_obj_t* result = lv_obj_get_sibling(obj, idx);
    return sni_tb_c2js(&result, SNI_H_LV_OBJ);
}

jerry_value_t sni_api_lv_obj_get_sibling_by_type(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t idx;
    idx = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_class_t* class_p;
    if (!sni_tb_js2c(args_p[2], SNI_H_LV_OBJ_CLASS, &class_p))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_t* result = lv_obj_get_sibling_by_type(obj, idx, class_p);
    return sni_tb_c2js(&result, SNI_H_LV_OBJ);
}

jerry_value_t sni_api_lv_obj_get_state(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_state_t result = lv_obj_get_state(obj);
    return sni_tb_c2js(&result, SNI_V_LV_STATE);
}

jerry_value_t sni_api_lv_obj_get_style_align(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_align_t result = lv_obj_get_style_align(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_anim(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    const lv_anim_t* result = lv_obj_get_style_anim(obj, part);
    return sni_tb_c2js(&result, SNI_H_LV_ANIM);
}

jerry_value_t sni_api_lv_obj_get_style_anim_duration(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    uint32_t result = lv_obj_get_style_anim_duration(obj, part);
    return sni_tb_c2js(&result, SNI_T_UINT32);
}

jerry_value_t sni_api_lv_obj_get_style_arc_color(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_color_t result = lv_obj_get_style_arc_color(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_COLOR);
}

jerry_value_t sni_api_lv_obj_get_style_arc_color_filtered(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_color_t result = lv_obj_get_style_arc_color_filtered(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_COLOR);
}

jerry_value_t sni_api_lv_obj_get_style_arc_image_src(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    const void* result = lv_obj_get_style_arc_image_src(obj, part);
    return sni_tb_c2js(&result, SNI_T_PTR);
}

jerry_value_t sni_api_lv_obj_get_style_arc_opa(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_opa_t result = lv_obj_get_style_arc_opa(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_OPA);
}

jerry_value_t sni_api_lv_obj_get_style_arc_rounded(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    bool result = lv_obj_get_style_arc_rounded(obj, part);
    return sni_tb_c2js_boolean(result);
}

jerry_value_t sni_api_lv_obj_get_style_base_dir(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_base_dir_t result = lv_obj_get_style_base_dir(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_bg_color(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_color_t result = lv_obj_get_style_bg_color(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_COLOR);
}

jerry_value_t sni_api_lv_obj_get_style_bg_color_filtered(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_color_t result = lv_obj_get_style_bg_color_filtered(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_COLOR);
}

jerry_value_t sni_api_lv_obj_get_style_bg_grad(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    const lv_grad_dsc_t* result = lv_obj_get_style_bg_grad(obj, part);
    return sni_tb_c2js(&result, SNI_H_LV_GRAD_DSC);
}

jerry_value_t sni_api_lv_obj_get_style_bg_grad_color(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_color_t result = lv_obj_get_style_bg_grad_color(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_COLOR);
}

jerry_value_t sni_api_lv_obj_get_style_bg_grad_color_filtered(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_color_t result = lv_obj_get_style_bg_grad_color_filtered(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_COLOR);
}

jerry_value_t sni_api_lv_obj_get_style_bg_grad_dir(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_grad_dir_t result = lv_obj_get_style_bg_grad_dir(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_bg_grad_opa(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_opa_t result = lv_obj_get_style_bg_grad_opa(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_OPA);
}

jerry_value_t sni_api_lv_obj_get_style_bg_grad_stop(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_bg_grad_stop(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_bg_image_opa(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_opa_t result = lv_obj_get_style_bg_image_opa(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_OPA);
}

jerry_value_t sni_api_lv_obj_get_style_bg_image_recolor(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_color_t result = lv_obj_get_style_bg_image_recolor(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_COLOR);
}

jerry_value_t sni_api_lv_obj_get_style_bg_image_recolor_filtered(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_color_t result = lv_obj_get_style_bg_image_recolor_filtered(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_COLOR);
}

jerry_value_t sni_api_lv_obj_get_style_bg_image_recolor_opa(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_opa_t result = lv_obj_get_style_bg_image_recolor_opa(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_OPA);
}

jerry_value_t sni_api_lv_obj_get_style_bg_image_src(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    const void* result = lv_obj_get_style_bg_image_src(obj, part);
    return sni_tb_c2js(&result, SNI_T_PTR);
}

jerry_value_t sni_api_lv_obj_get_style_bg_image_tiled(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    bool result = lv_obj_get_style_bg_image_tiled(obj, part);
    return sni_tb_c2js_boolean(result);
}

jerry_value_t sni_api_lv_obj_get_style_bg_main_opa(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_opa_t result = lv_obj_get_style_bg_main_opa(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_OPA);
}

jerry_value_t sni_api_lv_obj_get_style_bg_main_stop(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_bg_main_stop(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_bg_opa(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_opa_t result = lv_obj_get_style_bg_opa(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_OPA);
}

jerry_value_t sni_api_lv_obj_get_style_bitmap_mask_src(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    const void* result = lv_obj_get_style_bitmap_mask_src(obj, part);
    return sni_tb_c2js(&result, SNI_T_PTR);
}

jerry_value_t sni_api_lv_obj_get_style_blend_mode(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_blend_mode_t result = lv_obj_get_style_blend_mode(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_border_color(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_color_t result = lv_obj_get_style_border_color(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_COLOR);
}

jerry_value_t sni_api_lv_obj_get_style_border_color_filtered(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_color_t result = lv_obj_get_style_border_color_filtered(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_COLOR);
}

jerry_value_t sni_api_lv_obj_get_style_border_opa(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_opa_t result = lv_obj_get_style_border_opa(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_OPA);
}

jerry_value_t sni_api_lv_obj_get_style_border_post(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    bool result = lv_obj_get_style_border_post(obj, part);
    return sni_tb_c2js_boolean(result);
}

jerry_value_t sni_api_lv_obj_get_style_clip_corner(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    bool result = lv_obj_get_style_clip_corner(obj, part);
    return sni_tb_c2js_boolean(result);
}

jerry_value_t sni_api_lv_obj_get_style_color_filter_dsc(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    const lv_color_filter_dsc_t* result = lv_obj_get_style_color_filter_dsc(obj, part);
    return sni_tb_c2js(&result, SNI_H_LV_COLOR_FILTER_DSC);
}

jerry_value_t sni_api_lv_obj_get_style_color_filter_opa(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_opa_t result = lv_obj_get_style_color_filter_opa(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_OPA);
}

jerry_value_t sni_api_lv_obj_get_style_flex_cross_place(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_flex_align_t result = lv_obj_get_style_flex_cross_place(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_flex_flow(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_flex_flow_t result = lv_obj_get_style_flex_flow(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_flex_grow(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    uint8_t result = lv_obj_get_style_flex_grow(obj, part);
    return sni_tb_c2js(&result, SNI_T_UINT32);
}

jerry_value_t sni_api_lv_obj_get_style_flex_main_place(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_flex_align_t result = lv_obj_get_style_flex_main_place(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_flex_track_place(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_flex_align_t result = lv_obj_get_style_flex_track_place(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_height(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_height(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_image_opa(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_opa_t result = lv_obj_get_style_image_opa(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_OPA);
}

jerry_value_t sni_api_lv_obj_get_style_image_recolor(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_color_t result = lv_obj_get_style_image_recolor(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_COLOR);
}

jerry_value_t sni_api_lv_obj_get_style_image_recolor_filtered(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_color_t result = lv_obj_get_style_image_recolor_filtered(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_COLOR);
}

jerry_value_t sni_api_lv_obj_get_style_image_recolor_opa(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_opa_t result = lv_obj_get_style_image_recolor_opa(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_OPA);
}

jerry_value_t sni_api_lv_obj_get_style_layout(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    uint16_t result = lv_obj_get_style_layout(obj, part);
    return sni_tb_c2js(&result, SNI_T_UINT32);
}

jerry_value_t sni_api_lv_obj_get_style_length(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_length(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_line_color(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_color_t result = lv_obj_get_style_line_color(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_COLOR);
}

jerry_value_t sni_api_lv_obj_get_style_line_color_filtered(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_color_t result = lv_obj_get_style_line_color_filtered(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_COLOR);
}

jerry_value_t sni_api_lv_obj_get_style_line_dash_gap(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_line_dash_gap(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_line_opa(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_opa_t result = lv_obj_get_style_line_opa(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_OPA);
}

jerry_value_t sni_api_lv_obj_get_style_line_rounded(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    bool result = lv_obj_get_style_line_rounded(obj, part);
    return sni_tb_c2js_boolean(result);
}

jerry_value_t sni_api_lv_obj_get_style_margin_bottom(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_margin_bottom(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_margin_left(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_margin_left(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_margin_right(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_margin_right(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_margin_top(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_margin_top(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_max_height(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_max_height(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_min_height(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_min_height(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_opa(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_opa_t result = lv_obj_get_style_opa(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_OPA);
}

jerry_value_t sni_api_lv_obj_get_style_opa_layered(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_opa_t result = lv_obj_get_style_opa_layered(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_OPA);
}

jerry_value_t sni_api_lv_obj_get_style_opa_recursive(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_opa_t result = lv_obj_get_style_opa_recursive(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_OPA);
}

jerry_value_t sni_api_lv_obj_get_style_outline_color(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_color_t result = lv_obj_get_style_outline_color(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_COLOR);
}

jerry_value_t sni_api_lv_obj_get_style_outline_color_filtered(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_color_t result = lv_obj_get_style_outline_color_filtered(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_COLOR);
}

jerry_value_t sni_api_lv_obj_get_style_outline_opa(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_opa_t result = lv_obj_get_style_outline_opa(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_OPA);
}

jerry_value_t sni_api_lv_obj_get_style_outline_pad(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_outline_pad(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_pad_bottom(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_pad_bottom(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_pad_column(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_pad_column(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_pad_left(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_pad_left(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_pad_right(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_pad_right(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_pad_row(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_pad_row(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_pad_top(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_pad_top(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_prop(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_prop_t prop;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_PROP, &prop))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_style_value_t result = lv_obj_get_style_prop(obj, part, prop);
    return sni_tb_c2js(&result, SNI_H_LV_STYLE_VALUE);
}

jerry_value_t sni_api_lv_obj_get_style_radius(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_radius(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_rotary_sensitivity(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    uint32_t result = lv_obj_get_style_rotary_sensitivity(obj, part);
    return sni_tb_c2js(&result, SNI_T_UINT32);
}

jerry_value_t sni_api_lv_obj_get_style_shadow_color(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_color_t result = lv_obj_get_style_shadow_color(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_COLOR);
}

jerry_value_t sni_api_lv_obj_get_style_shadow_color_filtered(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_color_t result = lv_obj_get_style_shadow_color_filtered(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_COLOR);
}

jerry_value_t sni_api_lv_obj_get_style_shadow_offset_x(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_shadow_offset_x(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_shadow_offset_y(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_shadow_offset_y(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_shadow_opa(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_opa_t result = lv_obj_get_style_shadow_opa(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_OPA);
}

jerry_value_t sni_api_lv_obj_get_style_shadow_spread(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_shadow_spread(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_space_bottom(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_space_bottom(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_space_left(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_space_left(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_space_right(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_space_right(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_space_top(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_space_top(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_text_align(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_text_align_t result = lv_obj_get_style_text_align(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_text_color(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_color_t result = lv_obj_get_style_text_color(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_COLOR);
}

jerry_value_t sni_api_lv_obj_get_style_text_color_filtered(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_color_t result = lv_obj_get_style_text_color_filtered(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_COLOR);
}

jerry_value_t sni_api_lv_obj_get_style_text_decor(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_text_decor_t result = lv_obj_get_style_text_decor(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_text_font(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    const lv_font_t* result = lv_obj_get_style_text_font(obj, part);
    return sni_tb_c2js(&result, SNI_H_LV_FONT);
}

jerry_value_t sni_api_lv_obj_get_style_text_letter_space(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_text_letter_space(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_text_line_space(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_text_line_space(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_text_opa(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_opa_t result = lv_obj_get_style_text_opa(obj, part);
    return sni_tb_c2js(&result, SNI_V_LV_OPA);
}

jerry_value_t sni_api_lv_obj_get_style_transform_height(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_transform_height(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_transform_pivot_x(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_transform_pivot_x(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_transform_pivot_y(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_transform_pivot_y(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_transform_rotation(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_transform_rotation(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_transform_scale_x(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_transform_scale_x(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_transform_scale_x_safe(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_transform_scale_x_safe(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_transform_scale_y(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_transform_scale_y(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_transform_scale_y_safe(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_transform_scale_y_safe(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_transform_skew_x(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_transform_skew_x(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_transform_skew_y(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_transform_skew_y(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_transition(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    const lv_style_transition_dsc_t* result = lv_obj_get_style_transition(obj, part);
    return sni_tb_c2js(&result, SNI_H_LV_STYLE_TRANSITION_DSC);
}

jerry_value_t sni_api_lv_obj_get_style_translate_x(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_translate_x(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_translate_y(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_translate_y(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_x(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_x(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_style_y(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_style_y(obj, part);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_transformed_area(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_area_t* area;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_AREA, &area))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_point_transform_flag_t flags;
    flags = sni_tb_js2c_int32(args_p[2]);

    lv_obj_get_transformed_area(obj, area, flags);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_get_user_data(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    void* result = lv_obj_get_user_data(obj);
    return sni_tb_c2js(&result, SNI_T_PTR);
}

jerry_value_t sni_api_lv_obj_get_x(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_x(obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_x2(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_x2(obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_x_aligned(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_x_aligned(obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_y(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_y(obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_y2(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_y2(obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_get_y_aligned(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    int32_t result = lv_obj_get_y_aligned(obj);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_has_class(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_class_t* class_p;
    if (!sni_tb_js2c(args_p[1], SNI_H_LV_OBJ_CLASS, &class_p))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    bool result = lv_obj_has_class(obj, class_p);
    return sni_tb_c2js_boolean(result);
}

jerry_value_t sni_api_lv_obj_has_flag(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_flag_t f;
    f = sni_tb_js2c_int32(args_p[1]);

    bool result = lv_obj_has_flag(obj, f);
    return sni_tb_c2js_boolean(result);
}

jerry_value_t sni_api_lv_obj_has_flag_any(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_flag_t f;
    f = sni_tb_js2c_int32(args_p[1]);

    bool result = lv_obj_has_flag_any(obj, f);
    return sni_tb_c2js_boolean(result);
}

jerry_value_t sni_api_lv_obj_has_state(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_state_t state;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_STATE, &state))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    bool result = lv_obj_has_state(obj, state);
    return sni_tb_c2js_boolean(result);
}

jerry_value_t sni_api_lv_obj_has_style_prop(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_prop_t prop;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_PROP, &prop))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    bool result = lv_obj_has_style_prop(obj, selector, prop);
    return sni_tb_c2js_boolean(result);
}

jerry_value_t sni_api_lv_obj_hit_test(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_point_t* point;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_POINT, &point))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    bool result = lv_obj_hit_test(obj, point);
    return sni_tb_c2js_boolean(result);
}

jerry_value_t sni_api_lv_obj_init_draw_arc_dsc(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_draw_arc_dsc_t* draw_dsc;
    if (!sni_tb_js2c(args_p[2], SNI_H_LV_DRAW_ARC_DSC, &draw_dsc))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_init_draw_arc_dsc(obj, part, draw_dsc);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_init_draw_image_dsc(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_draw_image_dsc_t* draw_dsc;
    if (!sni_tb_js2c(args_p[2], SNI_H_LV_DRAW_IMAGE_DSC, &draw_dsc))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_init_draw_image_dsc(obj, part, draw_dsc);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_init_draw_label_dsc(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_draw_label_dsc_t* draw_dsc;
    if (!sni_tb_js2c(args_p[2], SNI_H_LV_DRAW_LABEL_DSC, &draw_dsc))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_init_draw_label_dsc(obj, part, draw_dsc);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_init_draw_line_dsc(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_draw_line_dsc_t* draw_dsc;
    if (!sni_tb_js2c(args_p[2], SNI_H_LV_DRAW_LINE_DSC, &draw_dsc))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_init_draw_line_dsc(obj, part, draw_dsc);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_init_draw_rect_dsc(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_draw_rect_dsc_t* draw_dsc;
    if (!sni_tb_js2c(args_p[2], SNI_H_LV_DRAW_RECT_DSC, &draw_dsc))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_init_draw_rect_dsc(obj, part, draw_dsc);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_is_editable(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    bool result = lv_obj_is_editable(obj);
    return sni_tb_c2js_boolean(result);
}

jerry_value_t sni_api_lv_obj_is_group_def(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    bool result = lv_obj_is_group_def(obj);
    return sni_tb_c2js_boolean(result);
}

jerry_value_t sni_api_lv_obj_is_layout_positioned(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    bool result = lv_obj_is_layout_positioned(obj);
    return sni_tb_c2js_boolean(result);
}

jerry_value_t sni_api_lv_obj_is_scrolling(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    bool result = lv_obj_is_scrolling(obj);
    return sni_tb_c2js_boolean(result);
}

jerry_value_t sni_api_lv_obj_is_visible(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    bool result = lv_obj_is_visible(obj);
    return sni_tb_c2js_boolean(result);
}

jerry_value_t sni_api_lv_obj_mark_layout_as_dirty(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_mark_layout_as_dirty(obj);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_move_background(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_move_background(obj);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_move_children_by(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 4)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t x_diff;
    x_diff = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_number(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t y_diff;
    y_diff = sni_tb_js2c_int32(args_p[2]);

    if (!jerry_value_is_boolean(args_p[3]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    bool ignore_floating;
    ignore_floating = sni_tb_js2c_boolean(args_p[3]);

    lv_obj_move_children_by(obj, x_diff, y_diff, ignore_floating);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_move_foreground(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_move_foreground(obj);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_move_to(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t x;
    x = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_number(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t y;
    y = sni_tb_js2c_int32(args_p[2]);

    lv_obj_move_to(obj, x, y);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_move_to_index(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t index;
    index = sni_tb_js2c_int32(args_p[1]);

    lv_obj_move_to_index(obj, index);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_null_on_delete(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t** obj_ptr;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj_ptr))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_null_on_delete(obj_ptr);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_readjust_scroll(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_anim_enable_t anim_en;
    anim_en = sni_tb_js2c_int32(args_p[1]);

    lv_obj_readjust_scroll(obj, anim_en);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_redraw(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_layer_t* layer;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_LAYER, &layer))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[1], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_redraw(layer, obj);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_refr_pos(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_refr_pos(obj);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_refr_size(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    bool result = lv_obj_refr_size(obj);
    return sni_tb_c2js_boolean(result);
}

jerry_value_t sni_api_lv_obj_refresh_ext_draw_size(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_refresh_ext_draw_size(obj);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_refresh_self_size(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    bool result = lv_obj_refresh_self_size(obj);
    return sni_tb_c2js_boolean(result);
}

jerry_value_t sni_api_lv_obj_refresh_style(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_prop_t prop;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_PROP, &prop))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_refresh_style(obj, part, prop);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_remove_event(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    uint32_t index;
    index = sni_tb_js2c_uint32(args_p[1]);

    bool result = lv_obj_remove_event(obj, index);
    return sni_tb_c2js_boolean(result);
}

jerry_value_t sni_api_lv_obj_remove_event_cb(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_event_cb_t event_cb;
    if (!sni_tb_js2c(args_p[1], SNI_H_LV_EVENT_CB, &event_cb))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    bool result = lv_obj_remove_event_cb(obj, event_cb);
    return sni_tb_c2js_boolean(result);
}

jerry_value_t sni_api_lv_obj_remove_event_cb_with_user_data(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_event_cb_t event_cb;
    if (!sni_tb_js2c(args_p[1], SNI_H_LV_EVENT_CB, &event_cb))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    void* user_data;
    if (!sni_tb_js2c(args_p[2], SNI_T_PTR, &user_data))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    uint32_t result = lv_obj_remove_event_cb_with_user_data(obj, event_cb, user_data);
    return sni_tb_c2js(&result, SNI_T_UINT32);
}

jerry_value_t sni_api_lv_obj_remove_event_dsc(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_event_dsc_t* dsc;
    if (!sni_tb_js2c(args_p[1], SNI_H_LV_EVENT_DSC, &dsc))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    bool result = lv_obj_remove_event_dsc(obj, dsc);
    return sni_tb_c2js_boolean(result);
}

jerry_value_t sni_api_lv_obj_remove_flag(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_flag_t f;
    f = sni_tb_js2c_int32(args_p[1]);

    lv_obj_remove_flag(obj, f);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_remove_from_subject(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_subject_t* subject;
    if (!sni_tb_js2c(args_p[1], SNI_H_LV_SUBJECT, &subject))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_remove_from_subject(obj, subject);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_remove_local_style_prop(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_prop_t prop;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_STYLE_PROP, &prop))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    bool result = lv_obj_remove_local_style_prop(obj, prop, selector);
    return sni_tb_c2js_boolean(result);
}

jerry_value_t sni_api_lv_obj_remove_state(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_state_t state;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_STATE, &state))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_remove_state(obj, state);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_remove_style(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_style_t* style;
    if (!sni_tb_js2c(args_p[1], SNI_H_LV_STYLE, &style))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_remove_style(obj, style, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_remove_style_all(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_remove_style_all(obj);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_replace_style(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 4)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_style_t* old_style;
    if (!sni_tb_js2c(args_p[1], SNI_H_LV_STYLE, &old_style))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_style_t* new_style;
    if (!sni_tb_js2c(args_p[2], SNI_H_LV_STYLE, &new_style))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[3]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[3], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    bool result = lv_obj_replace_style(obj, old_style, new_style, selector);
    return sni_tb_c2js_boolean(result);
}

jerry_value_t sni_api_lv_obj_report_style_change(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_t* style;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_STYLE, &style))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_report_style_change(style);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_scroll_by(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 4)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t x;
    x = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_number(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t y;
    y = sni_tb_js2c_int32(args_p[2]);

    if (!jerry_value_is_number(args_p[3]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_anim_enable_t anim_en;
    anim_en = sni_tb_js2c_int32(args_p[3]);

    lv_obj_scroll_by(obj, x, y, anim_en);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_scroll_by_bounded(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 4)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t dx;
    dx = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_number(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t dy;
    dy = sni_tb_js2c_int32(args_p[2]);

    if (!jerry_value_is_number(args_p[3]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_anim_enable_t anim_en;
    anim_en = sni_tb_js2c_int32(args_p[3]);

    lv_obj_scroll_by_bounded(obj, dx, dy, anim_en);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_scroll_to(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 4)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t x;
    x = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_number(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t y;
    y = sni_tb_js2c_int32(args_p[2]);

    if (!jerry_value_is_number(args_p[3]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_anim_enable_t anim_en;
    anim_en = sni_tb_js2c_int32(args_p[3]);

    lv_obj_scroll_to(obj, x, y, anim_en);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_scroll_to_view(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_anim_enable_t anim_en;
    anim_en = sni_tb_js2c_int32(args_p[1]);

    lv_obj_scroll_to_view(obj, anim_en);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_scroll_to_view_recursive(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_anim_enable_t anim_en;
    anim_en = sni_tb_js2c_int32(args_p[1]);

    lv_obj_scroll_to_view_recursive(obj, anim_en);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_scroll_to_x(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t x;
    x = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_number(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_anim_enable_t anim_en;
    anim_en = sni_tb_js2c_int32(args_p[2]);

    lv_obj_scroll_to_x(obj, x, anim_en);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_scroll_to_y(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t y;
    y = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_number(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_anim_enable_t anim_en;
    anim_en = sni_tb_js2c_int32(args_p[2]);

    lv_obj_scroll_to_y(obj, y, anim_en);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_send_event(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_event_code_t event_code;
    event_code = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    void* param;
    if (!sni_tb_js2c(args_p[2], SNI_T_PTR, &param))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_result_t result = lv_obj_send_event(obj, event_code, param);
    return sni_tb_c2js(&result, SNI_T_INT32);
}

jerry_value_t sni_api_lv_obj_set_align(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_align_t align;
    align = sni_tb_js2c_int32(args_p[1]);

    lv_obj_set_align(obj, align);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_content_height(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t h;
    h = sni_tb_js2c_int32(args_p[1]);

    lv_obj_set_content_height(obj, h);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_ext_click_area(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t size;
    size = sni_tb_js2c_int32(args_p[1]);

    lv_obj_set_ext_click_area(obj, size);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_flex_align(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 4)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_flex_align_t main_place;
    main_place = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_number(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_flex_align_t cross_place;
    cross_place = sni_tb_js2c_int32(args_p[2]);

    if (!jerry_value_is_number(args_p[3]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_flex_align_t track_cross_place;
    track_cross_place = sni_tb_js2c_int32(args_p[3]);

    lv_obj_set_flex_align(obj, main_place, cross_place, track_cross_place);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_flex_flow(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_flex_flow_t flow;
    flow = sni_tb_js2c_int32(args_p[1]);

    lv_obj_set_flex_flow(obj, flow);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_flex_grow(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    uint8_t grow;
    grow = sni_tb_js2c_uint32(args_p[1]);

    lv_obj_set_flex_grow(obj, grow);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_height(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t h;
    h = sni_tb_js2c_int32(args_p[1]);

    lv_obj_set_height(obj, h);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_layout(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    uint32_t layout;
    layout = sni_tb_js2c_uint32(args_p[1]);

    lv_obj_set_layout(obj, layout);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_local_style_prop(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 4)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_prop_t prop;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_STYLE_PROP, &prop))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_value_t value;
    if (!sni_tb_js2c(args_p[2], SNI_H_LV_STYLE_VALUE, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[3]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[3], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_local_style_prop(obj, prop, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_parent(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* parent;
    if (!sni_tb_js2c(args_p[1], SNI_H_LV_OBJ, &parent))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_parent(obj, parent);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_pos(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t x;
    x = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_number(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t y;
    y = sni_tb_js2c_int32(args_p[2]);

    lv_obj_set_pos(obj, x, y);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_scroll_dir(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_dir_t dir;
    dir = sni_tb_js2c_int32(args_p[1]);

    lv_obj_set_scroll_dir(obj, dir);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_scroll_snap_x(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_scroll_snap_t align;
    align = sni_tb_js2c_int32(args_p[1]);

    lv_obj_set_scroll_snap_x(obj, align);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_scroll_snap_y(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_scroll_snap_t align;
    align = sni_tb_js2c_int32(args_p[1]);

    lv_obj_set_scroll_snap_y(obj, align);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_scrollbar_mode(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_scrollbar_mode_t mode;
    mode = sni_tb_js2c_int32(args_p[1]);

    lv_obj_set_scrollbar_mode(obj, mode);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_size(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t w;
    w = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_number(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t h;
    h = sni_tb_js2c_int32(args_p[2]);

    lv_obj_set_size(obj, w, h);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_state(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_state_t state;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_STATE, &state))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_boolean(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    bool v;
    v = sni_tb_js2c_boolean(args_p[2]);

    lv_obj_set_state(obj, state, v);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_align(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_align_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_align(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_anim(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_anim_t* value;
    if (!sni_tb_js2c(args_p[1], SNI_H_LV_ANIM, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_anim(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_anim_duration(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    uint32_t value;
    value = sni_tb_js2c_uint32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_anim_duration(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_arc_color(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_color_t value;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_COLOR, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_arc_color(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_arc_image_src(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const void* value;
    if (!sni_tb_js2c(args_p[1], SNI_T_PTR, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_arc_image_src(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_arc_opa(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_opa_t value;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_OPA, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_arc_opa(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_arc_rounded(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_boolean(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    bool value;
    value = sni_tb_js2c_boolean(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_arc_rounded(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_base_dir(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_base_dir_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_base_dir(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_bg_color(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_color_t value;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_COLOR, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_bg_color(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_bg_grad(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_grad_dsc_t* value;
    if (!sni_tb_js2c(args_p[1], SNI_H_LV_GRAD_DSC, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_bg_grad(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_bg_grad_color(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_color_t value;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_COLOR, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_bg_grad_color(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_bg_grad_dir(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_grad_dir_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_bg_grad_dir(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_bg_grad_opa(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_opa_t value;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_OPA, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_bg_grad_opa(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_bg_grad_stop(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_bg_grad_stop(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_bg_image_opa(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_opa_t value;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_OPA, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_bg_image_opa(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_bg_image_recolor(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_color_t value;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_COLOR, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_bg_image_recolor(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_bg_image_recolor_opa(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_opa_t value;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_OPA, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_bg_image_recolor_opa(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_bg_image_src(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const void* value;
    if (!sni_tb_js2c(args_p[1], SNI_T_PTR, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_bg_image_src(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_bg_image_tiled(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_boolean(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    bool value;
    value = sni_tb_js2c_boolean(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_bg_image_tiled(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_bg_main_opa(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_opa_t value;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_OPA, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_bg_main_opa(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_bg_main_stop(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_bg_main_stop(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_bg_opa(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_opa_t value;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_OPA, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_bg_opa(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_bitmap_mask_src(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const void* value;
    if (!sni_tb_js2c(args_p[1], SNI_T_PTR, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_bitmap_mask_src(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_blend_mode(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_blend_mode_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_blend_mode(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_border_color(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_color_t value;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_COLOR, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_border_color(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_border_opa(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_opa_t value;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_OPA, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_border_opa(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_border_post(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_boolean(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    bool value;
    value = sni_tb_js2c_boolean(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_border_post(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_clip_corner(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_boolean(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    bool value;
    value = sni_tb_js2c_boolean(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_clip_corner(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_color_filter_dsc(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_color_filter_dsc_t* value;
    if (!sni_tb_js2c(args_p[1], SNI_H_LV_COLOR_FILTER_DSC, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_color_filter_dsc(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_color_filter_opa(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_opa_t value;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_OPA, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_color_filter_opa(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_flex_cross_place(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_flex_align_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_flex_cross_place(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_flex_flow(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_flex_flow_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_flex_flow(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_flex_grow(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    uint8_t value;
    value = sni_tb_js2c_uint32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_flex_grow(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_flex_main_place(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_flex_align_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_flex_main_place(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_flex_track_place(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_flex_align_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_flex_track_place(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_height(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_height(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_image_opa(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_opa_t value;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_OPA, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_image_opa(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_image_recolor(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_color_t value;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_COLOR, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_image_recolor(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_image_recolor_opa(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_opa_t value;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_OPA, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_image_recolor_opa(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_layout(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    uint16_t value;
    value = sni_tb_js2c_uint32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_layout(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_length(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_length(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_line_color(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_color_t value;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_COLOR, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_line_color(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_line_dash_gap(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_line_dash_gap(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_line_opa(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_opa_t value;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_OPA, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_line_opa(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_line_rounded(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_boolean(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    bool value;
    value = sni_tb_js2c_boolean(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_line_rounded(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_margin_all(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_margin_all(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_margin_bottom(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_margin_bottom(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_margin_hor(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_margin_hor(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_margin_left(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_margin_left(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_margin_right(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_margin_right(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_margin_top(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_margin_top(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_margin_ver(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_margin_ver(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_max_height(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_max_height(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_min_height(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_min_height(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_opa(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_opa_t value;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_OPA, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_opa(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_opa_layered(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_opa_t value;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_OPA, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_opa_layered(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_outline_color(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_color_t value;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_COLOR, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_outline_color(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_outline_opa(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_opa_t value;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_OPA, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_outline_opa(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_outline_pad(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_outline_pad(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_pad_all(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_pad_all(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_pad_bottom(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_pad_bottom(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_pad_column(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_pad_column(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_pad_gap(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_pad_gap(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_pad_hor(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_pad_hor(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_pad_left(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_pad_left(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_pad_right(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_pad_right(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_pad_row(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_pad_row(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_pad_top(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_pad_top(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_pad_ver(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_pad_ver(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_radius(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_radius(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_rotary_sensitivity(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    uint32_t value;
    value = sni_tb_js2c_uint32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_rotary_sensitivity(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_shadow_color(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_color_t value;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_COLOR, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_shadow_color(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_shadow_offset_x(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_shadow_offset_x(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_shadow_offset_y(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_shadow_offset_y(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_shadow_opa(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_opa_t value;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_OPA, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_shadow_opa(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_shadow_spread(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_shadow_spread(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_size(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 4)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t width;
    width = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_number(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t height;
    height = sni_tb_js2c_int32(args_p[2]);

    if (!jerry_value_is_object(args_p[3]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[3], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_size(obj, width, height, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_text_align(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_text_align_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_text_align(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_text_color(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_color_t value;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_COLOR, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_text_color(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_text_decor(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_text_decor_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_text_decor(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_text_font(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_font_t* value;
    if (!sni_tb_js2c(args_p[1], SNI_H_LV_FONT, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_text_font(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_text_letter_space(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_text_letter_space(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_text_line_space(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_text_line_space(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_text_opa(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_opa_t value;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_OPA, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_text_opa(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_transform_height(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_transform_height(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_transform_pivot_x(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_transform_pivot_x(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_transform_pivot_y(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_transform_pivot_y(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_transform_rotation(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_transform_rotation(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_transform_scale(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_transform_scale(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_transform_scale_x(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_transform_scale_x(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_transform_scale_y(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_transform_scale_y(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_transform_skew_x(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_transform_skew_x(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_transform_skew_y(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_transform_skew_y(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_transition(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_style_transition_dsc_t* value;
    if (!sni_tb_js2c(args_p[1], SNI_H_LV_STYLE_TRANSITION_DSC, &value))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_transition(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_translate_x(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_translate_x(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_translate_y(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_translate_y(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_x(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_x(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_style_y(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t value;
    value = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[2], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_style_y(obj, value, selector);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_user_data(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    void* user_data;
    if (!sni_tb_js2c(args_p[1], SNI_T_PTR, &user_data))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_set_user_data(obj, user_data);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_x(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t x;
    x = sni_tb_js2c_int32(args_p[1]);

    lv_obj_set_x(obj, x);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_set_y(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    int32_t y;
    y = sni_tb_js2c_int32(args_p[1]);

    lv_obj_set_y(obj, y);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_style_apply_color_filter(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_part_t part;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_PART, &part))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_value_t v;
    if (!sni_tb_js2c(args_p[2], SNI_H_LV_STYLE_VALUE, &v))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_style_value_t result = lv_obj_style_apply_color_filter(obj, part, v);
    return sni_tb_c2js(&result, SNI_H_LV_STYLE_VALUE);
}

jerry_value_t sni_api_lv_obj_style_get_selector_part(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[0], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_part_t result = lv_obj_style_get_selector_part(selector);
    return sni_tb_c2js(&result, SNI_V_LV_PART);
}

jerry_value_t sni_api_lv_obj_style_get_selector_state(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_style_selector_t selector;
    if (!sni_tb_js2c(args_p[0], SNI_V_LV_STYLE_SELECTOR, &selector))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_state_t result = lv_obj_style_get_selector_state(selector);
    return sni_tb_c2js(&result, SNI_V_LV_STATE);
}

jerry_value_t sni_api_lv_obj_swap(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj1;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj1))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj2;
    if (!sni_tb_js2c(args_p[1], SNI_H_LV_OBJ, &obj2))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_swap(obj1, obj2);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_transform_point(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_point_t* p;
    if (!sni_tb_js2c(args_p[1], SNI_V_LV_POINT, &p))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_point_transform_flag_t flags;
    flags = sni_tb_js2c_int32(args_p[2]);

    lv_obj_transform_point(obj, p, flags);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_transform_point_array(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 4)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    void* points;
    if (!sni_tb_js2c(args_p[1], SNI_T_PTR, &points))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    size_t count;
    count = sni_tb_js2c_uint32(args_p[2]);

    if (!jerry_value_is_number(args_p[3]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_point_transform_flag_t flags;
    flags = sni_tb_js2c_int32(args_p[3]);

    lv_obj_transform_point_array(obj, points, count, flags);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_tree_walk(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* start_obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &start_obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_tree_walk_cb_t cb;
    if (!sni_tb_js2c(args_p[1], SNI_H_LV_OBJ_TREE_WALK_CB, &cb))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_object(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    void* user_data;
    if (!sni_tb_js2c(args_p[2], SNI_T_PTR, &user_data))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_tree_walk(start_obj, cb, user_data);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_update_flag(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 3)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_flag_t f;
    f = sni_tb_js2c_int32(args_p[1]);

    if (!jerry_value_is_boolean(args_p[2]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    bool v;
    v = sni_tb_js2c_boolean(args_p[2]);

    lv_obj_update_flag(obj, f, v);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_update_layout(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    const lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    lv_obj_update_layout(obj);
    return jerry_undefined();
}

jerry_value_t sni_api_lv_obj_update_snap(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    (void)call_info_p;

    if (args_count != 2)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!jerry_value_is_object(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_obj_t* obj;
    if (!sni_tb_js2c(args_p[0], SNI_H_LV_OBJ, &obj))
    {
        return sni_api_throw_error("Failed to convert argument");
    }

    if (!jerry_value_is_number(args_p[1]))
    {
        return sni_api_throw_error("Invalid argument type");
    }
    lv_anim_enable_t anim_en;
    anim_en = sni_tb_js2c_int32(args_p[1]);

    lv_obj_update_snap(obj, anim_en);
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

const sni_api_entry_t lv_api_ns_button[] = {
    {.type = SNI_ENTRY_FUNCTION, .name = "create", .value.function = sni_api_lv_button_create},
    {.name = NULL},
};

const sni_api_entry_t lv_api_ns_obj[] = {
    {.type = SNI_ENTRY_FUNCTION, .name = "add_event_cb", .value.function = sni_api_lv_obj_add_event_cb},
    {.type = SNI_ENTRY_FUNCTION, .name = "add_flag", .value.function = sni_api_lv_obj_add_flag},
    {.type = SNI_ENTRY_FUNCTION, .name = "add_state", .value.function = sni_api_lv_obj_add_state},
    {.type = SNI_ENTRY_FUNCTION, .name = "add_style", .value.function = sni_api_lv_obj_add_style},
    {.type = SNI_ENTRY_FUNCTION, .name = "align", .value.function = sni_api_lv_obj_align},
    {.type = SNI_ENTRY_FUNCTION, .name = "align_to", .value.function = sni_api_lv_obj_align_to},
    {.type = SNI_ENTRY_FUNCTION, .name = "allocate_spec_attr", .value.function = sni_api_lv_obj_allocate_spec_attr},
    {.type = SNI_ENTRY_FUNCTION, .name = "area_is_visible", .value.function = sni_api_lv_obj_area_is_visible},
    {.type = SNI_ENTRY_FUNCTION, .name = "bind_checked", .value.function = sni_api_lv_obj_bind_checked},
    {.type = SNI_ENTRY_FUNCTION, .name = "bind_flag_if_eq", .value.function = sni_api_lv_obj_bind_flag_if_eq},
    {.type = SNI_ENTRY_FUNCTION, .name = "bind_flag_if_not_eq", .value.function = sni_api_lv_obj_bind_flag_if_not_eq},
    {.type = SNI_ENTRY_FUNCTION, .name = "bind_state_if_eq", .value.function = sni_api_lv_obj_bind_state_if_eq},
    {.type = SNI_ENTRY_FUNCTION, .name = "bind_state_if_not_eq", .value.function = sni_api_lv_obj_bind_state_if_not_eq},
    {.type = SNI_ENTRY_FUNCTION, .name = "calculate_ext_draw_size", .value.function = sni_api_lv_obj_calculate_ext_draw_size},
    {.type = SNI_ENTRY_FUNCTION, .name = "calculate_style_text_align", .value.function = sni_api_lv_obj_calculate_style_text_align},
    {.type = SNI_ENTRY_FUNCTION, .name = "center", .value.function = sni_api_lv_obj_center},
    {.type = SNI_ENTRY_FUNCTION, .name = "check_type", .value.function = sni_api_lv_obj_check_type},
    {.type = SNI_ENTRY_FUNCTION, .name = "class_create_obj", .value.function = sni_api_lv_obj_class_create_obj},
    {.type = SNI_ENTRY_FUNCTION, .name = "class_init_obj", .value.function = sni_api_lv_obj_class_init_obj},
    {.type = SNI_ENTRY_FUNCTION, .name = "clean", .value.function = sni_api_lv_obj_clean},
    {.type = SNI_ENTRY_FUNCTION, .name = "create", .value.function = sni_api_lv_obj_create},
    {.type = SNI_ENTRY_FUNCTION, .name = "delete", .value.function = sni_api_lv_obj_delete},
    {.type = SNI_ENTRY_FUNCTION, .name = "delete_anim_completed_cb", .value.function = sni_api_lv_obj_delete_anim_completed_cb},
    {.type = SNI_ENTRY_FUNCTION, .name = "delete_async", .value.function = sni_api_lv_obj_delete_async},
    {.type = SNI_ENTRY_FUNCTION, .name = "delete_delayed", .value.function = sni_api_lv_obj_delete_delayed},
    {.type = SNI_ENTRY_FUNCTION, .name = "dump_tree", .value.function = sni_api_lv_obj_dump_tree},
    {.type = SNI_ENTRY_FUNCTION, .name = "enable_style_refresh", .value.function = sni_api_lv_obj_enable_style_refresh},
    {.type = SNI_ENTRY_FUNCTION, .name = "event_base", .value.function = sni_api_lv_obj_event_base},
    {.type = SNI_ENTRY_FUNCTION, .name = "fade_in", .value.function = sni_api_lv_obj_fade_in},
    {.type = SNI_ENTRY_FUNCTION, .name = "fade_out", .value.function = sni_api_lv_obj_fade_out},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_child", .value.function = sni_api_lv_obj_get_child},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_child_by_type", .value.function = sni_api_lv_obj_get_child_by_type},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_child_count", .value.function = sni_api_lv_obj_get_child_count},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_child_count_by_type", .value.function = sni_api_lv_obj_get_child_count_by_type},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_class", .value.function = sni_api_lv_obj_get_class},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_click_area", .value.function = sni_api_lv_obj_get_click_area},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_content_coords", .value.function = sni_api_lv_obj_get_content_coords},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_content_height", .value.function = sni_api_lv_obj_get_content_height},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_coords", .value.function = sni_api_lv_obj_get_coords},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_display", .value.function = sni_api_lv_obj_get_display},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_event_count", .value.function = sni_api_lv_obj_get_event_count},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_event_dsc", .value.function = sni_api_lv_obj_get_event_dsc},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_group", .value.function = sni_api_lv_obj_get_group},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_height", .value.function = sni_api_lv_obj_get_height},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_index", .value.function = sni_api_lv_obj_get_index},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_index_by_type", .value.function = sni_api_lv_obj_get_index_by_type},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_local_style_prop", .value.function = sni_api_lv_obj_get_local_style_prop},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_parent", .value.function = sni_api_lv_obj_get_parent},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_screen", .value.function = sni_api_lv_obj_get_screen},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_scroll_bottom", .value.function = sni_api_lv_obj_get_scroll_bottom},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_scroll_dir", .value.function = sni_api_lv_obj_get_scroll_dir},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_scroll_end", .value.function = sni_api_lv_obj_get_scroll_end},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_scroll_left", .value.function = sni_api_lv_obj_get_scroll_left},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_scroll_right", .value.function = sni_api_lv_obj_get_scroll_right},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_scroll_snap_x", .value.function = sni_api_lv_obj_get_scroll_snap_x},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_scroll_snap_y", .value.function = sni_api_lv_obj_get_scroll_snap_y},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_scroll_top", .value.function = sni_api_lv_obj_get_scroll_top},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_scroll_x", .value.function = sni_api_lv_obj_get_scroll_x},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_scroll_y", .value.function = sni_api_lv_obj_get_scroll_y},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_scrollbar_area", .value.function = sni_api_lv_obj_get_scrollbar_area},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_scrollbar_mode", .value.function = sni_api_lv_obj_get_scrollbar_mode},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_self_height", .value.function = sni_api_lv_obj_get_self_height},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_sibling", .value.function = sni_api_lv_obj_get_sibling},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_sibling_by_type", .value.function = sni_api_lv_obj_get_sibling_by_type},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_state", .value.function = sni_api_lv_obj_get_state},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_align", .value.function = sni_api_lv_obj_get_style_align},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_anim", .value.function = sni_api_lv_obj_get_style_anim},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_anim_duration", .value.function = sni_api_lv_obj_get_style_anim_duration},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_arc_color", .value.function = sni_api_lv_obj_get_style_arc_color},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_arc_color_filtered", .value.function = sni_api_lv_obj_get_style_arc_color_filtered},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_arc_image_src", .value.function = sni_api_lv_obj_get_style_arc_image_src},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_arc_opa", .value.function = sni_api_lv_obj_get_style_arc_opa},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_arc_rounded", .value.function = sni_api_lv_obj_get_style_arc_rounded},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_base_dir", .value.function = sni_api_lv_obj_get_style_base_dir},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_bg_color", .value.function = sni_api_lv_obj_get_style_bg_color},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_bg_color_filtered", .value.function = sni_api_lv_obj_get_style_bg_color_filtered},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_bg_grad", .value.function = sni_api_lv_obj_get_style_bg_grad},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_bg_grad_color", .value.function = sni_api_lv_obj_get_style_bg_grad_color},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_bg_grad_color_filtered", .value.function = sni_api_lv_obj_get_style_bg_grad_color_filtered},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_bg_grad_dir", .value.function = sni_api_lv_obj_get_style_bg_grad_dir},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_bg_grad_opa", .value.function = sni_api_lv_obj_get_style_bg_grad_opa},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_bg_grad_stop", .value.function = sni_api_lv_obj_get_style_bg_grad_stop},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_bg_image_opa", .value.function = sni_api_lv_obj_get_style_bg_image_opa},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_bg_image_recolor", .value.function = sni_api_lv_obj_get_style_bg_image_recolor},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_bg_image_recolor_filtered", .value.function = sni_api_lv_obj_get_style_bg_image_recolor_filtered},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_bg_image_recolor_opa", .value.function = sni_api_lv_obj_get_style_bg_image_recolor_opa},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_bg_image_src", .value.function = sni_api_lv_obj_get_style_bg_image_src},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_bg_image_tiled", .value.function = sni_api_lv_obj_get_style_bg_image_tiled},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_bg_main_opa", .value.function = sni_api_lv_obj_get_style_bg_main_opa},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_bg_main_stop", .value.function = sni_api_lv_obj_get_style_bg_main_stop},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_bg_opa", .value.function = sni_api_lv_obj_get_style_bg_opa},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_bitmap_mask_src", .value.function = sni_api_lv_obj_get_style_bitmap_mask_src},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_blend_mode", .value.function = sni_api_lv_obj_get_style_blend_mode},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_border_color", .value.function = sni_api_lv_obj_get_style_border_color},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_border_color_filtered", .value.function = sni_api_lv_obj_get_style_border_color_filtered},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_border_opa", .value.function = sni_api_lv_obj_get_style_border_opa},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_border_post", .value.function = sni_api_lv_obj_get_style_border_post},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_clip_corner", .value.function = sni_api_lv_obj_get_style_clip_corner},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_color_filter_dsc", .value.function = sni_api_lv_obj_get_style_color_filter_dsc},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_color_filter_opa", .value.function = sni_api_lv_obj_get_style_color_filter_opa},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_flex_cross_place", .value.function = sni_api_lv_obj_get_style_flex_cross_place},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_flex_flow", .value.function = sni_api_lv_obj_get_style_flex_flow},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_flex_grow", .value.function = sni_api_lv_obj_get_style_flex_grow},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_flex_main_place", .value.function = sni_api_lv_obj_get_style_flex_main_place},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_flex_track_place", .value.function = sni_api_lv_obj_get_style_flex_track_place},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_height", .value.function = sni_api_lv_obj_get_style_height},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_image_opa", .value.function = sni_api_lv_obj_get_style_image_opa},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_image_recolor", .value.function = sni_api_lv_obj_get_style_image_recolor},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_image_recolor_filtered", .value.function = sni_api_lv_obj_get_style_image_recolor_filtered},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_image_recolor_opa", .value.function = sni_api_lv_obj_get_style_image_recolor_opa},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_layout", .value.function = sni_api_lv_obj_get_style_layout},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_length", .value.function = sni_api_lv_obj_get_style_length},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_line_color", .value.function = sni_api_lv_obj_get_style_line_color},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_line_color_filtered", .value.function = sni_api_lv_obj_get_style_line_color_filtered},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_line_dash_gap", .value.function = sni_api_lv_obj_get_style_line_dash_gap},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_line_opa", .value.function = sni_api_lv_obj_get_style_line_opa},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_line_rounded", .value.function = sni_api_lv_obj_get_style_line_rounded},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_margin_bottom", .value.function = sni_api_lv_obj_get_style_margin_bottom},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_margin_left", .value.function = sni_api_lv_obj_get_style_margin_left},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_margin_right", .value.function = sni_api_lv_obj_get_style_margin_right},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_margin_top", .value.function = sni_api_lv_obj_get_style_margin_top},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_max_height", .value.function = sni_api_lv_obj_get_style_max_height},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_min_height", .value.function = sni_api_lv_obj_get_style_min_height},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_opa", .value.function = sni_api_lv_obj_get_style_opa},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_opa_layered", .value.function = sni_api_lv_obj_get_style_opa_layered},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_opa_recursive", .value.function = sni_api_lv_obj_get_style_opa_recursive},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_outline_color", .value.function = sni_api_lv_obj_get_style_outline_color},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_outline_color_filtered", .value.function = sni_api_lv_obj_get_style_outline_color_filtered},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_outline_opa", .value.function = sni_api_lv_obj_get_style_outline_opa},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_outline_pad", .value.function = sni_api_lv_obj_get_style_outline_pad},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_pad_bottom", .value.function = sni_api_lv_obj_get_style_pad_bottom},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_pad_column", .value.function = sni_api_lv_obj_get_style_pad_column},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_pad_left", .value.function = sni_api_lv_obj_get_style_pad_left},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_pad_right", .value.function = sni_api_lv_obj_get_style_pad_right},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_pad_row", .value.function = sni_api_lv_obj_get_style_pad_row},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_pad_top", .value.function = sni_api_lv_obj_get_style_pad_top},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_prop", .value.function = sni_api_lv_obj_get_style_prop},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_radius", .value.function = sni_api_lv_obj_get_style_radius},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_rotary_sensitivity", .value.function = sni_api_lv_obj_get_style_rotary_sensitivity},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_shadow_color", .value.function = sni_api_lv_obj_get_style_shadow_color},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_shadow_color_filtered", .value.function = sni_api_lv_obj_get_style_shadow_color_filtered},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_shadow_offset_x", .value.function = sni_api_lv_obj_get_style_shadow_offset_x},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_shadow_offset_y", .value.function = sni_api_lv_obj_get_style_shadow_offset_y},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_shadow_opa", .value.function = sni_api_lv_obj_get_style_shadow_opa},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_shadow_spread", .value.function = sni_api_lv_obj_get_style_shadow_spread},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_space_bottom", .value.function = sni_api_lv_obj_get_style_space_bottom},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_space_left", .value.function = sni_api_lv_obj_get_style_space_left},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_space_right", .value.function = sni_api_lv_obj_get_style_space_right},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_space_top", .value.function = sni_api_lv_obj_get_style_space_top},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_text_align", .value.function = sni_api_lv_obj_get_style_text_align},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_text_color", .value.function = sni_api_lv_obj_get_style_text_color},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_text_color_filtered", .value.function = sni_api_lv_obj_get_style_text_color_filtered},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_text_decor", .value.function = sni_api_lv_obj_get_style_text_decor},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_text_font", .value.function = sni_api_lv_obj_get_style_text_font},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_text_letter_space", .value.function = sni_api_lv_obj_get_style_text_letter_space},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_text_line_space", .value.function = sni_api_lv_obj_get_style_text_line_space},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_text_opa", .value.function = sni_api_lv_obj_get_style_text_opa},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_transform_height", .value.function = sni_api_lv_obj_get_style_transform_height},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_transform_pivot_x", .value.function = sni_api_lv_obj_get_style_transform_pivot_x},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_transform_pivot_y", .value.function = sni_api_lv_obj_get_style_transform_pivot_y},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_transform_rotation", .value.function = sni_api_lv_obj_get_style_transform_rotation},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_transform_scale_x", .value.function = sni_api_lv_obj_get_style_transform_scale_x},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_transform_scale_x_safe", .value.function = sni_api_lv_obj_get_style_transform_scale_x_safe},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_transform_scale_y", .value.function = sni_api_lv_obj_get_style_transform_scale_y},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_transform_scale_y_safe", .value.function = sni_api_lv_obj_get_style_transform_scale_y_safe},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_transform_skew_x", .value.function = sni_api_lv_obj_get_style_transform_skew_x},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_transform_skew_y", .value.function = sni_api_lv_obj_get_style_transform_skew_y},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_transition", .value.function = sni_api_lv_obj_get_style_transition},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_translate_x", .value.function = sni_api_lv_obj_get_style_translate_x},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_translate_y", .value.function = sni_api_lv_obj_get_style_translate_y},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_x", .value.function = sni_api_lv_obj_get_style_x},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_style_y", .value.function = sni_api_lv_obj_get_style_y},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_transformed_area", .value.function = sni_api_lv_obj_get_transformed_area},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_user_data", .value.function = sni_api_lv_obj_get_user_data},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_x", .value.function = sni_api_lv_obj_get_x},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_x2", .value.function = sni_api_lv_obj_get_x2},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_x_aligned", .value.function = sni_api_lv_obj_get_x_aligned},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_y", .value.function = sni_api_lv_obj_get_y},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_y2", .value.function = sni_api_lv_obj_get_y2},
    {.type = SNI_ENTRY_FUNCTION, .name = "get_y_aligned", .value.function = sni_api_lv_obj_get_y_aligned},
    {.type = SNI_ENTRY_FUNCTION, .name = "has_class", .value.function = sni_api_lv_obj_has_class},
    {.type = SNI_ENTRY_FUNCTION, .name = "has_flag", .value.function = sni_api_lv_obj_has_flag},
    {.type = SNI_ENTRY_FUNCTION, .name = "has_flag_any", .value.function = sni_api_lv_obj_has_flag_any},
    {.type = SNI_ENTRY_FUNCTION, .name = "has_state", .value.function = sni_api_lv_obj_has_state},
    {.type = SNI_ENTRY_FUNCTION, .name = "has_style_prop", .value.function = sni_api_lv_obj_has_style_prop},
    {.type = SNI_ENTRY_FUNCTION, .name = "hit_test", .value.function = sni_api_lv_obj_hit_test},
    {.type = SNI_ENTRY_FUNCTION, .name = "init_draw_arc_dsc", .value.function = sni_api_lv_obj_init_draw_arc_dsc},
    {.type = SNI_ENTRY_FUNCTION, .name = "init_draw_image_dsc", .value.function = sni_api_lv_obj_init_draw_image_dsc},
    {.type = SNI_ENTRY_FUNCTION, .name = "init_draw_label_dsc", .value.function = sni_api_lv_obj_init_draw_label_dsc},
    {.type = SNI_ENTRY_FUNCTION, .name = "init_draw_line_dsc", .value.function = sni_api_lv_obj_init_draw_line_dsc},
    {.type = SNI_ENTRY_FUNCTION, .name = "init_draw_rect_dsc", .value.function = sni_api_lv_obj_init_draw_rect_dsc},
    {.type = SNI_ENTRY_FUNCTION, .name = "is_editable", .value.function = sni_api_lv_obj_is_editable},
    {.type = SNI_ENTRY_FUNCTION, .name = "is_group_def", .value.function = sni_api_lv_obj_is_group_def},
    {.type = SNI_ENTRY_FUNCTION, .name = "is_layout_positioned", .value.function = sni_api_lv_obj_is_layout_positioned},
    {.type = SNI_ENTRY_FUNCTION, .name = "is_scrolling", .value.function = sni_api_lv_obj_is_scrolling},
    {.type = SNI_ENTRY_FUNCTION, .name = "is_visible", .value.function = sni_api_lv_obj_is_visible},
    {.type = SNI_ENTRY_FUNCTION, .name = "mark_layout_as_dirty", .value.function = sni_api_lv_obj_mark_layout_as_dirty},
    {.type = SNI_ENTRY_FUNCTION, .name = "move_background", .value.function = sni_api_lv_obj_move_background},
    {.type = SNI_ENTRY_FUNCTION, .name = "move_children_by", .value.function = sni_api_lv_obj_move_children_by},
    {.type = SNI_ENTRY_FUNCTION, .name = "move_foreground", .value.function = sni_api_lv_obj_move_foreground},
    {.type = SNI_ENTRY_FUNCTION, .name = "move_to", .value.function = sni_api_lv_obj_move_to},
    {.type = SNI_ENTRY_FUNCTION, .name = "move_to_index", .value.function = sni_api_lv_obj_move_to_index},
    {.type = SNI_ENTRY_FUNCTION, .name = "null_on_delete", .value.function = sni_api_lv_obj_null_on_delete},
    {.type = SNI_ENTRY_FUNCTION, .name = "readjust_scroll", .value.function = sni_api_lv_obj_readjust_scroll},
    {.type = SNI_ENTRY_FUNCTION, .name = "redraw", .value.function = sni_api_lv_obj_redraw},
    {.type = SNI_ENTRY_FUNCTION, .name = "refr_pos", .value.function = sni_api_lv_obj_refr_pos},
    {.type = SNI_ENTRY_FUNCTION, .name = "refr_size", .value.function = sni_api_lv_obj_refr_size},
    {.type = SNI_ENTRY_FUNCTION, .name = "refresh_ext_draw_size", .value.function = sni_api_lv_obj_refresh_ext_draw_size},
    {.type = SNI_ENTRY_FUNCTION, .name = "refresh_self_size", .value.function = sni_api_lv_obj_refresh_self_size},
    {.type = SNI_ENTRY_FUNCTION, .name = "refresh_style", .value.function = sni_api_lv_obj_refresh_style},
    {.type = SNI_ENTRY_FUNCTION, .name = "remove_event", .value.function = sni_api_lv_obj_remove_event},
    {.type = SNI_ENTRY_FUNCTION, .name = "remove_event_cb", .value.function = sni_api_lv_obj_remove_event_cb},
    {.type = SNI_ENTRY_FUNCTION, .name = "remove_event_cb_with_user_data", .value.function = sni_api_lv_obj_remove_event_cb_with_user_data},
    {.type = SNI_ENTRY_FUNCTION, .name = "remove_event_dsc", .value.function = sni_api_lv_obj_remove_event_dsc},
    {.type = SNI_ENTRY_FUNCTION, .name = "remove_flag", .value.function = sni_api_lv_obj_remove_flag},
    {.type = SNI_ENTRY_FUNCTION, .name = "remove_from_subject", .value.function = sni_api_lv_obj_remove_from_subject},
    {.type = SNI_ENTRY_FUNCTION, .name = "remove_local_style_prop", .value.function = sni_api_lv_obj_remove_local_style_prop},
    {.type = SNI_ENTRY_FUNCTION, .name = "remove_state", .value.function = sni_api_lv_obj_remove_state},
    {.type = SNI_ENTRY_FUNCTION, .name = "remove_style", .value.function = sni_api_lv_obj_remove_style},
    {.type = SNI_ENTRY_FUNCTION, .name = "remove_style_all", .value.function = sni_api_lv_obj_remove_style_all},
    {.type = SNI_ENTRY_FUNCTION, .name = "replace_style", .value.function = sni_api_lv_obj_replace_style},
    {.type = SNI_ENTRY_FUNCTION, .name = "report_style_change", .value.function = sni_api_lv_obj_report_style_change},
    {.type = SNI_ENTRY_FUNCTION, .name = "scroll_by", .value.function = sni_api_lv_obj_scroll_by},
    {.type = SNI_ENTRY_FUNCTION, .name = "scroll_by_bounded", .value.function = sni_api_lv_obj_scroll_by_bounded},
    {.type = SNI_ENTRY_FUNCTION, .name = "scroll_to", .value.function = sni_api_lv_obj_scroll_to},
    {.type = SNI_ENTRY_FUNCTION, .name = "scroll_to_view", .value.function = sni_api_lv_obj_scroll_to_view},
    {.type = SNI_ENTRY_FUNCTION, .name = "scroll_to_view_recursive", .value.function = sni_api_lv_obj_scroll_to_view_recursive},
    {.type = SNI_ENTRY_FUNCTION, .name = "scroll_to_x", .value.function = sni_api_lv_obj_scroll_to_x},
    {.type = SNI_ENTRY_FUNCTION, .name = "scroll_to_y", .value.function = sni_api_lv_obj_scroll_to_y},
    {.type = SNI_ENTRY_FUNCTION, .name = "send_event", .value.function = sni_api_lv_obj_send_event},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_align", .value.function = sni_api_lv_obj_set_align},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_content_height", .value.function = sni_api_lv_obj_set_content_height},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_ext_click_area", .value.function = sni_api_lv_obj_set_ext_click_area},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_flex_align", .value.function = sni_api_lv_obj_set_flex_align},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_flex_flow", .value.function = sni_api_lv_obj_set_flex_flow},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_flex_grow", .value.function = sni_api_lv_obj_set_flex_grow},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_height", .value.function = sni_api_lv_obj_set_height},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_layout", .value.function = sni_api_lv_obj_set_layout},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_local_style_prop", .value.function = sni_api_lv_obj_set_local_style_prop},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_parent", .value.function = sni_api_lv_obj_set_parent},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_pos", .value.function = sni_api_lv_obj_set_pos},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_scroll_dir", .value.function = sni_api_lv_obj_set_scroll_dir},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_scroll_snap_x", .value.function = sni_api_lv_obj_set_scroll_snap_x},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_scroll_snap_y", .value.function = sni_api_lv_obj_set_scroll_snap_y},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_scrollbar_mode", .value.function = sni_api_lv_obj_set_scrollbar_mode},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_size", .value.function = sni_api_lv_obj_set_size},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_state", .value.function = sni_api_lv_obj_set_state},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_align", .value.function = sni_api_lv_obj_set_style_align},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_anim", .value.function = sni_api_lv_obj_set_style_anim},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_anim_duration", .value.function = sni_api_lv_obj_set_style_anim_duration},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_arc_color", .value.function = sni_api_lv_obj_set_style_arc_color},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_arc_image_src", .value.function = sni_api_lv_obj_set_style_arc_image_src},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_arc_opa", .value.function = sni_api_lv_obj_set_style_arc_opa},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_arc_rounded", .value.function = sni_api_lv_obj_set_style_arc_rounded},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_base_dir", .value.function = sni_api_lv_obj_set_style_base_dir},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_bg_color", .value.function = sni_api_lv_obj_set_style_bg_color},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_bg_grad", .value.function = sni_api_lv_obj_set_style_bg_grad},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_bg_grad_color", .value.function = sni_api_lv_obj_set_style_bg_grad_color},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_bg_grad_dir", .value.function = sni_api_lv_obj_set_style_bg_grad_dir},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_bg_grad_opa", .value.function = sni_api_lv_obj_set_style_bg_grad_opa},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_bg_grad_stop", .value.function = sni_api_lv_obj_set_style_bg_grad_stop},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_bg_image_opa", .value.function = sni_api_lv_obj_set_style_bg_image_opa},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_bg_image_recolor", .value.function = sni_api_lv_obj_set_style_bg_image_recolor},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_bg_image_recolor_opa", .value.function = sni_api_lv_obj_set_style_bg_image_recolor_opa},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_bg_image_src", .value.function = sni_api_lv_obj_set_style_bg_image_src},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_bg_image_tiled", .value.function = sni_api_lv_obj_set_style_bg_image_tiled},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_bg_main_opa", .value.function = sni_api_lv_obj_set_style_bg_main_opa},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_bg_main_stop", .value.function = sni_api_lv_obj_set_style_bg_main_stop},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_bg_opa", .value.function = sni_api_lv_obj_set_style_bg_opa},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_bitmap_mask_src", .value.function = sni_api_lv_obj_set_style_bitmap_mask_src},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_blend_mode", .value.function = sni_api_lv_obj_set_style_blend_mode},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_border_color", .value.function = sni_api_lv_obj_set_style_border_color},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_border_opa", .value.function = sni_api_lv_obj_set_style_border_opa},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_border_post", .value.function = sni_api_lv_obj_set_style_border_post},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_clip_corner", .value.function = sni_api_lv_obj_set_style_clip_corner},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_color_filter_dsc", .value.function = sni_api_lv_obj_set_style_color_filter_dsc},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_color_filter_opa", .value.function = sni_api_lv_obj_set_style_color_filter_opa},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_flex_cross_place", .value.function = sni_api_lv_obj_set_style_flex_cross_place},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_flex_flow", .value.function = sni_api_lv_obj_set_style_flex_flow},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_flex_grow", .value.function = sni_api_lv_obj_set_style_flex_grow},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_flex_main_place", .value.function = sni_api_lv_obj_set_style_flex_main_place},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_flex_track_place", .value.function = sni_api_lv_obj_set_style_flex_track_place},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_height", .value.function = sni_api_lv_obj_set_style_height},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_image_opa", .value.function = sni_api_lv_obj_set_style_image_opa},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_image_recolor", .value.function = sni_api_lv_obj_set_style_image_recolor},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_image_recolor_opa", .value.function = sni_api_lv_obj_set_style_image_recolor_opa},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_layout", .value.function = sni_api_lv_obj_set_style_layout},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_length", .value.function = sni_api_lv_obj_set_style_length},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_line_color", .value.function = sni_api_lv_obj_set_style_line_color},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_line_dash_gap", .value.function = sni_api_lv_obj_set_style_line_dash_gap},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_line_opa", .value.function = sni_api_lv_obj_set_style_line_opa},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_line_rounded", .value.function = sni_api_lv_obj_set_style_line_rounded},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_margin_all", .value.function = sni_api_lv_obj_set_style_margin_all},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_margin_bottom", .value.function = sni_api_lv_obj_set_style_margin_bottom},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_margin_hor", .value.function = sni_api_lv_obj_set_style_margin_hor},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_margin_left", .value.function = sni_api_lv_obj_set_style_margin_left},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_margin_right", .value.function = sni_api_lv_obj_set_style_margin_right},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_margin_top", .value.function = sni_api_lv_obj_set_style_margin_top},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_margin_ver", .value.function = sni_api_lv_obj_set_style_margin_ver},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_max_height", .value.function = sni_api_lv_obj_set_style_max_height},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_min_height", .value.function = sni_api_lv_obj_set_style_min_height},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_opa", .value.function = sni_api_lv_obj_set_style_opa},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_opa_layered", .value.function = sni_api_lv_obj_set_style_opa_layered},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_outline_color", .value.function = sni_api_lv_obj_set_style_outline_color},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_outline_opa", .value.function = sni_api_lv_obj_set_style_outline_opa},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_outline_pad", .value.function = sni_api_lv_obj_set_style_outline_pad},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_pad_all", .value.function = sni_api_lv_obj_set_style_pad_all},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_pad_bottom", .value.function = sni_api_lv_obj_set_style_pad_bottom},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_pad_column", .value.function = sni_api_lv_obj_set_style_pad_column},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_pad_gap", .value.function = sni_api_lv_obj_set_style_pad_gap},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_pad_hor", .value.function = sni_api_lv_obj_set_style_pad_hor},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_pad_left", .value.function = sni_api_lv_obj_set_style_pad_left},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_pad_right", .value.function = sni_api_lv_obj_set_style_pad_right},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_pad_row", .value.function = sni_api_lv_obj_set_style_pad_row},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_pad_top", .value.function = sni_api_lv_obj_set_style_pad_top},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_pad_ver", .value.function = sni_api_lv_obj_set_style_pad_ver},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_radius", .value.function = sni_api_lv_obj_set_style_radius},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_rotary_sensitivity", .value.function = sni_api_lv_obj_set_style_rotary_sensitivity},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_shadow_color", .value.function = sni_api_lv_obj_set_style_shadow_color},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_shadow_offset_x", .value.function = sni_api_lv_obj_set_style_shadow_offset_x},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_shadow_offset_y", .value.function = sni_api_lv_obj_set_style_shadow_offset_y},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_shadow_opa", .value.function = sni_api_lv_obj_set_style_shadow_opa},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_shadow_spread", .value.function = sni_api_lv_obj_set_style_shadow_spread},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_size", .value.function = sni_api_lv_obj_set_style_size},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_text_align", .value.function = sni_api_lv_obj_set_style_text_align},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_text_color", .value.function = sni_api_lv_obj_set_style_text_color},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_text_decor", .value.function = sni_api_lv_obj_set_style_text_decor},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_text_font", .value.function = sni_api_lv_obj_set_style_text_font},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_text_letter_space", .value.function = sni_api_lv_obj_set_style_text_letter_space},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_text_line_space", .value.function = sni_api_lv_obj_set_style_text_line_space},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_text_opa", .value.function = sni_api_lv_obj_set_style_text_opa},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_transform_height", .value.function = sni_api_lv_obj_set_style_transform_height},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_transform_pivot_x", .value.function = sni_api_lv_obj_set_style_transform_pivot_x},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_transform_pivot_y", .value.function = sni_api_lv_obj_set_style_transform_pivot_y},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_transform_rotation", .value.function = sni_api_lv_obj_set_style_transform_rotation},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_transform_scale", .value.function = sni_api_lv_obj_set_style_transform_scale},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_transform_scale_x", .value.function = sni_api_lv_obj_set_style_transform_scale_x},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_transform_scale_y", .value.function = sni_api_lv_obj_set_style_transform_scale_y},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_transform_skew_x", .value.function = sni_api_lv_obj_set_style_transform_skew_x},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_transform_skew_y", .value.function = sni_api_lv_obj_set_style_transform_skew_y},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_transition", .value.function = sni_api_lv_obj_set_style_transition},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_translate_x", .value.function = sni_api_lv_obj_set_style_translate_x},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_translate_y", .value.function = sni_api_lv_obj_set_style_translate_y},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_x", .value.function = sni_api_lv_obj_set_style_x},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_style_y", .value.function = sni_api_lv_obj_set_style_y},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_user_data", .value.function = sni_api_lv_obj_set_user_data},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_x", .value.function = sni_api_lv_obj_set_x},
    {.type = SNI_ENTRY_FUNCTION, .name = "set_y", .value.function = sni_api_lv_obj_set_y},
    {.type = SNI_ENTRY_FUNCTION, .name = "style_apply_color_filter", .value.function = sni_api_lv_obj_style_apply_color_filter},
    {.type = SNI_ENTRY_FUNCTION, .name = "style_get_selector_part", .value.function = sni_api_lv_obj_style_get_selector_part},
    {.type = SNI_ENTRY_FUNCTION, .name = "style_get_selector_state", .value.function = sni_api_lv_obj_style_get_selector_state},
    {.type = SNI_ENTRY_FUNCTION, .name = "swap", .value.function = sni_api_lv_obj_swap},
    {.type = SNI_ENTRY_FUNCTION, .name = "transform_point", .value.function = sni_api_lv_obj_transform_point},
    {.type = SNI_ENTRY_FUNCTION, .name = "transform_point_array", .value.function = sni_api_lv_obj_transform_point_array},
    {.type = SNI_ENTRY_FUNCTION, .name = "tree_walk", .value.function = sni_api_lv_obj_tree_walk},
    {.type = SNI_ENTRY_FUNCTION, .name = "update_flag", .value.function = sni_api_lv_obj_update_flag},
    {.type = SNI_ENTRY_FUNCTION, .name = "update_layout", .value.function = sni_api_lv_obj_update_layout},
    {.type = SNI_ENTRY_FUNCTION, .name = "update_snap", .value.function = sni_api_lv_obj_update_snap},
    {.name = NULL},
};

const sni_api_entry_t lv_api_ns_screen[] = {
    {.type = SNI_ENTRY_FUNCTION, .name = "active", .value.function = sni_api_lv_screen_active},
    {.name = NULL},
};

const sni_api_entry_t lv_api_entry[] = {
    {.type = SNI_ENTRY_NAMESPACE, .name = "button", .value.sub_entries = lv_api_ns_button},
    {.type = SNI_ENTRY_NAMESPACE, .name = "obj", .value.sub_entries = lv_api_ns_obj},
    {.type = SNI_ENTRY_NAMESPACE, .name = "screen", .value.sub_entries = lv_api_ns_screen},
    {.name = NULL},
};

void sni_api_lv_init(void)
{
    lv_api_obj = sni_api_build(lv_api_entry);
}

void sni_api_lv_mount(jerry_value_t realm)
{
    bool result = sni_api_mount(realm, lv_api_obj, LV_API_NAME);
    if (!result)
    {
        EOS_LOG_E("Failed to mount LVGL API");
    }
}
