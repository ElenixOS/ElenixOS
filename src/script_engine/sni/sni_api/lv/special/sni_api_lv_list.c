/**
 * @file sni_api_lv_list.c
 * @brief LVGL List SNI safe convenience wrappers
 */

#include "sni_api_lv_special.h"

/* Includes ---------------------------------------------------*/
#include "eos_mem.h"
#include "lvgl.h"
#include "sni_api_export.h"
#include "sni_type_bridge.h"
#include "sni_types.h"

/* Function Implementations -----------------------------------*/

/**
 * @brief Create a column-flow list container without calling deprecated lv_list APIs.
 * @param call_info_p JerryScript call information
 * @param args_p JavaScript arguments
 * @param args_count Number of JavaScript arguments
 * @return Undefined on success, or a JavaScript error
 */
jerry_value_t sni_api_ctor_list(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count)
{
    lv_obj_t *parent = NULL;
    lv_obj_t *list = NULL;

    if (jerry_value_is_undefined(call_info_p->new_target))
    {
        return sni_api_throw_error("Constructor must be called with new");
    }

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!sni_tb_js2c_parent(args_p[0], (void **)&parent))
    {
        return sni_api_throw_error("Parent argument is required");
    }

    list = lv_obj_create(parent);
    if (!list)
    {
        return sni_api_throw_error("Failed to create list container");
    }

    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    if (!sni_tb_c2js_set_object(&list, SNI_H_LV_OBJ, call_info_p->this_value))
    {
        lv_obj_delete(list);
        return sni_api_throw_error("Failed to bind native object");
    }

    return jerry_undefined();
}

/**
 * @brief Add a text label to a list container.
 * @param call_info_p JerryScript call information
 * @param args_p JavaScript arguments
 * @param args_count Number of JavaScript arguments
 * @return The new label handle, or a JavaScript error
 */
jerry_value_t sni_api_lv_list_add_text(const jerry_call_info_t *call_info_p,
                                       const jerry_value_t args_p[],
                                       const jerry_length_t args_count)
{
    lv_obj_t *list = NULL;
    lv_obj_t *label = NULL;
    const char *text = NULL;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &list))
    {
        return sni_api_throw_error("Failed to convert list");
    }

    if (!jerry_value_is_string(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }

    text = sni_tb_js2c_string(args_p[0]);
    if (!text)
    {
        return sni_api_throw_error("Out of memory");
    }

    label = lv_label_create(list);
    if (!label)
    {
        eos_free((void *)text);
        return sni_api_throw_error("Failed to create list text");
    }

    lv_obj_set_width(label, lv_pct(100));
    lv_label_set_text(label, text);
    eos_free((void *)text);

    return sni_tb_c2js(&label, SNI_H_LV_OBJ);
}

/**
 * @brief Add a text-only button to a list container.
 * @param call_info_p JerryScript call information
 * @param args_p JavaScript arguments
 * @param args_count Number of JavaScript arguments
 * @return The new button handle, or a JavaScript error
 */
jerry_value_t sni_api_lv_list_add_button_text(const jerry_call_info_t *call_info_p,
                                              const jerry_value_t args_p[],
                                              const jerry_length_t args_count)
{
    lv_obj_t *list = NULL;
    lv_obj_t *button = NULL;
    lv_obj_t *label = NULL;
    const char *text = NULL;

    if (args_count != 1)
    {
        return sni_api_throw_error("Invalid argument count");
    }

    if (!sni_tb_js2c(call_info_p->this_value, SNI_H_LV_OBJ, &list))
    {
        return sni_api_throw_error("Failed to convert list");
    }

    if (!jerry_value_is_string(args_p[0]))
    {
        return sni_api_throw_error("Invalid argument type");
    }

    text = sni_tb_js2c_string(args_p[0]);
    if (!text)
    {
        return sni_api_throw_error("Out of memory");
    }

    button = lv_button_create(list);
    if (!button)
    {
        eos_free((void *)text);
        return sni_api_throw_error("Failed to create list button");
    }

    lv_obj_set_width(button, lv_pct(100));
    lv_obj_set_height(button, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(button, LV_FLEX_FLOW_ROW);

    label = lv_label_create(button);
    if (!label)
    {
        eos_free((void *)text);
        lv_obj_delete(button);
        return sni_api_throw_error("Failed to create list button label");
    }

    lv_label_set_long_mode(label, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
    lv_obj_set_flex_grow(label, 1);
    lv_label_set_text(label, text);
    eos_free((void *)text);

    return sni_tb_c2js(&button, SNI_H_LV_OBJ);
}
