
/**
 * @file lv_bindings.c
 * @brief 将 LVGL 绑定到 JerryScript 的实现文件，此文件使用脚本自动生成。
 * @author Sab1e
 * @date 2025-09-08
 */
// Application System header files
#include "lv_bindings.h"
#include "lv_bindings_misc.h"
#include "script_engine_core.h"
// Third party header files
#include "jerryscript.h"
#include "lvgl.h"
#include <stdlib.h>
#include <string.h>

/********************************** 错误处理辅助函数 **********************************/
static jerry_value_t throw_error(const char* message) {
    jerry_value_t error_obj = jerry_error_sz(JERRY_ERROR_TYPE, (const jerry_char_t*)message);
    return jerry_throw_value(error_obj, true);
}

/********************************** 宏定义处理辅助函数 **********************************/

static void lvgl_binding_set_enum(jerry_value_t global, const char* key, int32_t val) {
    jerry_value_t jkey = jerry_string_sz(key);
    jerry_value_t jval = jerry_number(val);
    jerry_object_set(global, jkey, jval);
    jerry_value_free(jkey);
    jerry_value_free(jval);
}

// 函数声明
static jerry_value_t js_lv_delay_ms(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_timer_handler(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_color_hex(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_style_set_text_color(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_event_get_target(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_event_get_code(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_event_get_user_data(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_clean(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_pos(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_size(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_width(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_align(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_center(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_add_style(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_pad_row(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_pad_column(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_bg_color(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_border_color(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_border_width(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_text_color(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_text_font(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_radius(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_pad_all(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_create(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_add_flag(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_add_state(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_remove_state(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_state(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_has_state(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_arc_create(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_arc_set_bg_angles(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_arc_set_value(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_arc_set_range(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_label_create(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_label_set_text(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_label_set_long_mode(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_label_get_text(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_bar_create(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_bar_set_value(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_bar_set_range(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_chart_create(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_chart_set_type(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_chart_set_point_count(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_chart_set_range(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_checkbox_create(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_checkbox_set_text(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_dropdown_create(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_dropdown_set_options(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_dropdown_set_selected(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_dropdown_get_selected(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_msgbox_create(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_msgbox_add_title(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_msgbox_add_header_button(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_msgbox_add_text(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_msgbox_add_footer_button(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_msgbox_add_close_button(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_msgbox_close(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_roller_create(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_roller_set_options(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_roller_set_selected(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_slider_create(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_slider_set_value(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_slider_set_range(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_slider_get_value(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_textarea_create(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_textarea_add_text(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_textarea_set_text(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_textarea_set_placeholder_text(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_switch_create(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_table_create(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_table_set_cell_value(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_del(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_clear_flag(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_scr_act(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_disp_get_scr_act(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_scr_load(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_img_create(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_img_set_angle(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_img_set_zoom(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_btn_create(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_table_set_col_cnt(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_table_set_row_cnt(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);

// 函数实现

/**
 * @brief Delay for the given milliseconds. By default it's a blocking delay, but with :ref:`lv_delay_set_cb()` a custom delay function can be set too
 */
static jerry_value_t js_lv_delay_ms(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: ms (uint32_t)
    jerry_value_t js_arg_ms = args[0];
    if (!jerry_value_is_number(js_arg_ms)) {
        return throw_error("Argument 0 must be a number");
    }
    
    uint32_t arg_ms = (uint32_t)jerry_value_as_number(js_arg_ms);
    
    // 调用底层函数
    lv_delay_ms(arg_ms);

    return jerry_undefined();
}



/**
 * @brief lv_timer_handler function
 */
static jerry_value_t js_lv_timer_handler(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 调用底层函数
    uint32_t ret_value = lv_timer_handler();

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Create a color from 0x000000..0xffffff input the color
 */
static jerry_value_t js_lv_color_hex(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: c (uint32_t)
    jerry_value_t js_arg_c = args[0];
    if (!jerry_value_is_number(js_arg_c)) {
        return throw_error("Argument 0 must be a number");
    }
    
    uint32_t arg_c = (uint32_t)jerry_value_as_number(js_arg_c);
    
    // 调用底层函数
    lv_color_t ret_value = lv_color_hex(arg_c);

    // 处理返回值
    jerry_value_t js_result;
    // 转换为JS颜色对象
    js_result = lv_color_to_js(ret_value);

    return js_result;
}



/**
 * @brief lv_style_set_text_color function
 */
static jerry_value_t js_lv_style_set_text_color(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: style (lv_style_t*)
    // 对象类型参数，支持null
    void* arg_style = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_style = args[0];
        if (!jerry_value_is_object(js_arg_style)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_style_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_style_ptr_val = jerry_object_get(js_arg_style, arg_style_ptr_prop);
        jerry_value_free(arg_style_ptr_prop);
        
        if (!jerry_value_is_number(arg_style_ptr_val)) {
            jerry_value_free(arg_style_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_style_ptr = (uintptr_t)jerry_value_as_number(arg_style_ptr_val);
        jerry_value_free(arg_style_ptr_val);
        arg_style = (void*)arg_style_ptr;
    }
    
    // 解析参数: value (lv_color_t)
    lv_color_t arg_value = js_to_lv_color(args[1]);

    // 调用底层函数
    lv_style_set_text_color(arg_style, arg_value);

    return jerry_undefined();
}



/**
 * @brief Get the object originally targeted by the event. It's the same even if the event is bubbled. the target of the event_code
 */
static jerry_value_t js_lv_event_get_target(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: e (lv_event_t*)
    // 对象类型参数，支持null
    void* arg_e = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_e = args[0];
        if (!jerry_value_is_object(js_arg_e)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_e_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_e_ptr_val = jerry_object_get(js_arg_e, arg_e_ptr_prop);
        jerry_value_free(arg_e_ptr_prop);
        
        if (!jerry_value_is_number(arg_e_ptr_val)) {
            jerry_value_free(arg_e_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_e_ptr = (uintptr_t)jerry_value_as_number(arg_e_ptr_val);
        jerry_value_free(arg_e_ptr_val);
        arg_e = (void*)arg_e_ptr;
    }
    
    // 调用底层函数
    void* ret_value = lv_event_get_target(arg_e);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为通用指针对象
    js_result = jerry_object();
    jerry_value_t ptr = jerry_number((double)(uintptr_t)ret_value);
    jerry_object_set(js_result, jerry_string_sz("__ptr"), ptr);
    jerry_object_set(js_result, jerry_string_sz("__type"), jerry_string_sz("void*"));
    jerry_value_free(ptr);

    return js_result;
}



/**
 * @brief Get the event code of an event the event code. (E.g. LV_EVENT_CLICKED , LV_EVENT_FOCUSED , etc)
 */
static jerry_value_t js_lv_event_get_code(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: e (lv_event_t*)
    // 对象类型参数，支持null
    void* arg_e = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_e = args[0];
        if (!jerry_value_is_object(js_arg_e)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_e_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_e_ptr_val = jerry_object_get(js_arg_e, arg_e_ptr_prop);
        jerry_value_free(arg_e_ptr_prop);
        
        if (!jerry_value_is_number(arg_e_ptr_val)) {
            jerry_value_free(arg_e_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_e_ptr = (uintptr_t)jerry_value_as_number(arg_e_ptr_val);
        jerry_value_free(arg_e_ptr_val);
        arg_e = (void*)arg_e_ptr;
    }
    
    // 调用底层函数
    lv_event_code_t ret_value = lv_event_get_code(arg_e);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the user_data passed when the event was registered on the object pointer to the user_data
 */
static jerry_value_t js_lv_event_get_user_data(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: e (lv_event_t*)
    // lv_event_t* 类型参数处理
    lv_event_t* arg_e = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_e = args[0];
        if (!jerry_value_is_object(js_arg_e)) {
            return throw_error("Argument 0 must be an event object");
        }
        
        // 检查类型标记
        jerry_value_t type_prop = jerry_string_sz("__type");
        jerry_value_t type_val = jerry_object_get(js_arg_e, type_prop);
        jerry_value_free(type_prop);
        
        jerry_size_t type_len = jerry_string_size(type_val, JERRY_ENCODING_UTF8);
        char type_str[32];
        jerry_string_to_buffer(type_val, JERRY_ENCODING_UTF8, (jerry_char_t*)type_str, type_len);
        type_str[type_len] = '\0';
        jerry_value_free(type_val);
        
        if (strcmp(type_str, "lv_event") != 0) {
            return throw_error("Argument 0 must be an event object");
        }
        
        // 获取事件指针
        jerry_value_t ptr_prop = jerry_string_sz("__event_ptr");
        jerry_value_t ptr_val = jerry_object_get(js_arg_e, ptr_prop);
        jerry_value_free(ptr_prop);
        
        if (!jerry_value_is_number(ptr_val)) {
            jerry_value_free(ptr_val);
            return throw_error("Invalid event pointer");
        }
        
        uintptr_t ptr = (uintptr_t)jerry_value_as_number(ptr_val);
        jerry_value_free(ptr_val);
        arg_e = (lv_event_t*)ptr;
    }
    
    // 调用底层函数
    void* ret_value = lv_event_get_user_data(arg_e);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为通用指针对象
    js_result = jerry_object();
    jerry_value_t ptr = jerry_number((double)(uintptr_t)ret_value);
    jerry_object_set(js_result, jerry_string_sz("__ptr"), ptr);
    jerry_object_set(js_result, jerry_string_sz("__type"), jerry_string_sz("void*"));
    jerry_value_free(ptr);

    return js_result;
}



/**
 * @brief Delete all children of an object. Also remove the objects from their group and remove all animations (if any). Send LV_EVENT_DELETED to deleted objects.
 */
static jerry_value_t js_lv_obj_clean(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 调用底层函数
    lv_obj_clean(arg_obj);

    return jerry_undefined();
}



/**
 * @brief Set the position of an object relative to the set alignment. With default alignment it's the distance from the top left corner  E.g. LV_ALIGN_CENTER alignment it's the offset from the center of the parent  The position is interpreted on the content area of the parent  The values can be set in pixel or in percentage of parent size with lv_pct(v)
 */
static jerry_value_t js_lv_obj_set_pos(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: x (int32_t)
    jerry_value_t js_arg_x = args[1];
    if (!jerry_value_is_number(js_arg_x)) {
        return throw_error("Argument 1 must be a number");
    }
    
    int32_t arg_x = (int32_t)jerry_value_as_number(js_arg_x);
    
    // 解析参数: y (int32_t)
    jerry_value_t js_arg_y = args[2];
    if (!jerry_value_is_number(js_arg_y)) {
        return throw_error("Argument 2 must be a number");
    }
    
    int32_t arg_y = (int32_t)jerry_value_as_number(js_arg_y);
    
    // 调用底层函数
    lv_obj_set_pos(arg_obj, arg_x, arg_y);

    return jerry_undefined();
}



/**
 * @brief Set the size of an object. possible values are: pixel simple set the size accordingly LV_SIZE_CONTENT set the size to involve all children in the given direction lv_pct(x) to set size in percentage of the parent's content area size (the size without paddings). x should be in [0..1000]% range
 */
static jerry_value_t js_lv_obj_set_size(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: w (int32_t)
    jerry_value_t js_arg_w = args[1];
    if (!jerry_value_is_number(js_arg_w)) {
        return throw_error("Argument 1 must be a number");
    }
    
    int32_t arg_w = (int32_t)jerry_value_as_number(js_arg_w);
    
    // 解析参数: h (int32_t)
    jerry_value_t js_arg_h = args[2];
    if (!jerry_value_is_number(js_arg_h)) {
        return throw_error("Argument 2 must be a number");
    }
    
    int32_t arg_h = (int32_t)jerry_value_as_number(js_arg_h);
    
    // 调用底层函数
    lv_obj_set_size(arg_obj, arg_w, arg_h);

    return jerry_undefined();
}



/**
 * @brief Set the width of an object possible values are: pixel simple set the size accordingly LV_SIZE_CONTENT set the size to involve all children in the given direction lv_pct(x) to set size in percentage of the parent's content area size (the size without paddings). x should be in [0..1000]% range
 */
static jerry_value_t js_lv_obj_set_width(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: w (int32_t)
    jerry_value_t js_arg_w = args[1];
    if (!jerry_value_is_number(js_arg_w)) {
        return throw_error("Argument 1 must be a number");
    }
    
    int32_t arg_w = (int32_t)jerry_value_as_number(js_arg_w);
    
    // 调用底层函数
    lv_obj_set_width(arg_obj, arg_w);

    return jerry_undefined();
}



/**
 * @brief Change the alignment of an object and set new coordinates. Equivalent to: lv_obj_set_align(obj, align); lv_obj_set_pos(obj, x_ofs, y_ofs);
 */
static jerry_value_t js_lv_obj_align(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 4) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: align (lv_align_t)
    jerry_value_t js_arg_align = args[1];
    if (!jerry_value_is_number(js_arg_align)) {
        return throw_error("Argument 1 must be a number");
    }
    
    int arg_align = (int)jerry_value_as_number(js_arg_align);
    
    // 解析参数: x_ofs (int32_t)
    jerry_value_t js_arg_x_ofs = args[2];
    if (!jerry_value_is_number(js_arg_x_ofs)) {
        return throw_error("Argument 2 must be a number");
    }
    
    int32_t arg_x_ofs = (int32_t)jerry_value_as_number(js_arg_x_ofs);
    
    // 解析参数: y_ofs (int32_t)
    jerry_value_t js_arg_y_ofs = args[3];
    if (!jerry_value_is_number(js_arg_y_ofs)) {
        return throw_error("Argument 3 must be a number");
    }
    
    int32_t arg_y_ofs = (int32_t)jerry_value_as_number(js_arg_y_ofs);
    
    // 调用底层函数
    lv_obj_align(arg_obj, arg_align, arg_x_ofs, arg_y_ofs);

    return jerry_undefined();
}



/**
 * @brief Align an object to the center on its parent. if the parent size changes obj needs to be aligned manually again
 */
static jerry_value_t js_lv_obj_center(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 调用底层函数
    lv_obj_center(arg_obj);

    return jerry_undefined();
}



/**
 * @brief Add a style to an object. lv_obj_add_style(btn, &style_btn, 0); //Default button style lv_obj_add_style(btn, &btn_red, LV_STATE_PRESSED); //Overwrite only some colors to red when pressed
 */
static jerry_value_t js_lv_obj_add_style(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: style (const lv_style_t*)
    // 对象类型参数，支持null
    void* arg_style = NULL;
    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1])) {
        jerry_value_t js_arg_style = args[1];
        if (!jerry_value_is_object(js_arg_style)) {
            return throw_error("Argument 1 must be an object or null");
        }
        
        jerry_value_t arg_style_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_style_ptr_val = jerry_object_get(js_arg_style, arg_style_ptr_prop);
        jerry_value_free(arg_style_ptr_prop);
        
        if (!jerry_value_is_number(arg_style_ptr_val)) {
            jerry_value_free(arg_style_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_style_ptr = (uintptr_t)jerry_value_as_number(arg_style_ptr_val);
        jerry_value_free(arg_style_ptr_val);
        arg_style = (void*)arg_style_ptr;
    }
    
    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return throw_error("Argument 2 must be a number");
    }
    
    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);
    
    // 调用底层函数
    lv_obj_add_style(arg_obj, arg_style, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_pad_row function
 */
static jerry_value_t js_lv_obj_set_style_pad_row(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return throw_error("Argument 1 must be a number");
    }
    
    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);
    
    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return throw_error("Argument 2 must be a number");
    }
    
    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);
    
    // 调用底层函数
    lv_obj_set_style_pad_row(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_pad_column function
 */
static jerry_value_t js_lv_obj_set_style_pad_column(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return throw_error("Argument 1 must be a number");
    }
    
    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);
    
    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return throw_error("Argument 2 must be a number");
    }
    
    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);
    
    // 调用底层函数
    lv_obj_set_style_pad_column(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_bg_color function
 */
static jerry_value_t js_lv_obj_set_style_bg_color(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: value (lv_color_t)
    lv_color_t arg_value = js_to_lv_color(args[1]);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return throw_error("Argument 2 must be a number");
    }
    
    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);
    
    // 调用底层函数
    lv_obj_set_style_bg_color(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_border_color function
 */
static jerry_value_t js_lv_obj_set_style_border_color(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: value (lv_color_t)
    lv_color_t arg_value = js_to_lv_color(args[1]);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return throw_error("Argument 2 must be a number");
    }
    
    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);
    
    // 调用底层函数
    lv_obj_set_style_border_color(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_border_width function
 */
static jerry_value_t js_lv_obj_set_style_border_width(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return throw_error("Argument 1 must be a number");
    }
    
    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);
    
    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return throw_error("Argument 2 must be a number");
    }
    
    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);
    
    // 调用底层函数
    lv_obj_set_style_border_width(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_text_color function
 */
static jerry_value_t js_lv_obj_set_style_text_color(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: value (lv_color_t)
    lv_color_t arg_value = js_to_lv_color(args[1]);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return throw_error("Argument 2 must be a number");
    }
    
    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);
    
    // 调用底层函数
    lv_obj_set_style_text_color(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_text_font function
 */
static jerry_value_t js_lv_obj_set_style_text_font(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: value ( lv_font_t*)
    // lv_font_t* 类型参数处理
    const lv_font_t* arg_value = NULL;
    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1])) {
        jerry_value_t js_arg_value = args[1];
        if (!jerry_value_is_object(js_arg_value)) {
            return throw_error("Argument 1 must be a font object or null");
        }
        
        // 检查类型标记
        jerry_value_t type_prop = jerry_string_sz("__type");
        jerry_value_t type_val = jerry_object_get(js_arg_value, type_prop);
        jerry_value_free(type_prop);
        
        jerry_size_t type_len = jerry_string_size(type_val, JERRY_ENCODING_UTF8);
        char type_str[32];
        jerry_string_to_buffer(type_val, JERRY_ENCODING_UTF8, (jerry_char_t*)type_str, type_len);
        type_str[type_len] = '\0';
        jerry_value_free(type_val);
        
        if (strcmp(type_str, "lv_font") != 0) {
            return throw_error("Argument 1 must be a font object");
        }
        
        // 获取字体指针
        jerry_value_t ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t ptr_val = jerry_object_get(js_arg_value, ptr_prop);
        jerry_value_free(ptr_prop);
        
        if (!jerry_value_is_number(ptr_val)) {
            jerry_value_free(ptr_val);
            return throw_error("Invalid font pointer");
        }
        
        uintptr_t ptr = (uintptr_t)jerry_value_as_number(ptr_val);
        jerry_value_free(ptr_val);
        arg_value = (const lv_font_t*)ptr;
    }
    
    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return throw_error("Argument 2 must be a number");
    }
    
    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);
    
    // 调用底层函数
    lv_obj_set_style_text_font(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_radius function
 */
static jerry_value_t js_lv_obj_set_style_radius(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return throw_error("Argument 1 must be a number");
    }
    
    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);
    
    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return throw_error("Argument 2 must be a number");
    }
    
    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);
    
    // 调用底层函数
    lv_obj_set_style_radius(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_pad_all function
 */
static jerry_value_t js_lv_obj_set_style_pad_all(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return throw_error("Argument 1 must be a number");
    }
    
    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);
    
    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return throw_error("Argument 2 must be a number");
    }
    
    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);
    
    // 调用底层函数
    lv_obj_set_style_pad_all(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief Create a base object (a rectangle) pointer to the new object
 */
static jerry_value_t js_lv_obj_create(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: parent (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_parent = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_parent = args[0];
        if (!jerry_value_is_object(js_arg_parent)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_parent_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_parent_ptr_val = jerry_object_get(js_arg_parent, arg_parent_ptr_prop);
        jerry_value_free(arg_parent_ptr_prop);
        
        if (!jerry_value_is_number(arg_parent_ptr_val)) {
            jerry_value_free(arg_parent_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_parent_ptr = (uintptr_t)jerry_value_as_number(arg_parent_ptr_val);
        jerry_value_free(arg_parent_ptr_val);
        arg_parent = (void*)arg_parent_ptr;
    }
    
    // 调用底层函数
    lv_obj_t* ret_value = lv_obj_create(arg_parent);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = jerry_object();
    jerry_value_t ptr = jerry_number((double)(uintptr_t)ret_value);
    jerry_value_t cls = jerry_string_sz("lv_obj");
    jerry_object_set(js_result, jerry_string_sz("__ptr"), ptr);
    jerry_object_set(js_result, jerry_string_sz("__class"), cls);
    jerry_value_free(ptr);
    jerry_value_free(cls);

    return js_result;
}



/**
 * @brief Set one or more flags
 */
static jerry_value_t js_lv_obj_add_flag(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: f (lv_obj_flag_t)
    jerry_value_t js_arg_f = args[1];
    if (!jerry_value_is_number(js_arg_f)) {
        return throw_error("Argument 1 must be a number");
    }
    
    int arg_f = (int)jerry_value_as_number(js_arg_f);
    
    // 调用底层函数
    lv_obj_add_flag(arg_obj, arg_f);

    return jerry_undefined();
}



/**
 * @brief Add one or more states to the object. The other state bits will remain unchanged. If specified in the styles, transition animation will be started from the previous state to the current.
 */
static jerry_value_t js_lv_obj_add_state(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: state (lv_state_t)
    jerry_value_t js_arg_state = args[1];
    if (!jerry_value_is_number(js_arg_state)) {
        return throw_error("Argument 1 must be a number");
    }
    
    uint16_t arg_state = (uint16_t)jerry_value_as_number(js_arg_state);
    
    // 调用底层函数
    lv_obj_add_state(arg_obj, arg_state);

    return jerry_undefined();
}



/**
 * @brief Remove one or more states to the object. The other state bits will remain unchanged. If specified in the styles, transition animation will be started from the previous state to the current.
 */
static jerry_value_t js_lv_obj_remove_state(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: state (lv_state_t)
    jerry_value_t js_arg_state = args[1];
    if (!jerry_value_is_number(js_arg_state)) {
        return throw_error("Argument 1 must be a number");
    }
    
    uint16_t arg_state = (uint16_t)jerry_value_as_number(js_arg_state);
    
    // 调用底层函数
    lv_obj_remove_state(arg_obj, arg_state);

    return jerry_undefined();
}



/**
 * @brief Add or remove one or more states to the object. The other state bits will remain unchanged.
 */
static jerry_value_t js_lv_obj_set_state(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: state (lv_state_t)
    jerry_value_t js_arg_state = args[1];
    if (!jerry_value_is_number(js_arg_state)) {
        return throw_error("Argument 1 must be a number");
    }
    
    uint16_t arg_state = (uint16_t)jerry_value_as_number(js_arg_state);
    
    // 解析参数: v (bool)
    // 布尔类型参数: v
    bool arg_v = false;
    if (!jerry_value_is_undefined(args[2])) {
        if (jerry_value_is_boolean(args[2])) {
            arg_v = jerry_value_to_boolean(args[2]);
        }
        else if (jerry_value_is_number(args[2])) {
            arg_v = (jerry_value_as_number(args[2]) != 0);
        }
        else {
            return throw_error("Argument 2 must be boolean or number for bool");
        }
    }
    
    // 调用底层函数
    lv_obj_set_state(arg_obj, arg_state, arg_v);

    return jerry_undefined();
}



/**
 * @brief Check if the object is in a given state or not. true: obj is in state ; false: obj is not in state
 */
static jerry_value_t js_lv_obj_has_state(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: state (lv_state_t)
    jerry_value_t js_arg_state = args[1];
    if (!jerry_value_is_number(js_arg_state)) {
        return throw_error("Argument 1 must be a number");
    }
    
    uint16_t arg_state = (uint16_t)jerry_value_as_number(js_arg_state);
    
    // 调用底层函数
    bool ret_value = lv_obj_has_state(arg_obj, arg_state);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Create an arc object pointer to the created arc
 */
static jerry_value_t js_lv_arc_create(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: parent (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_parent = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_parent = args[0];
        if (!jerry_value_is_object(js_arg_parent)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_parent_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_parent_ptr_val = jerry_object_get(js_arg_parent, arg_parent_ptr_prop);
        jerry_value_free(arg_parent_ptr_prop);
        
        if (!jerry_value_is_number(arg_parent_ptr_val)) {
            jerry_value_free(arg_parent_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_parent_ptr = (uintptr_t)jerry_value_as_number(arg_parent_ptr_val);
        jerry_value_free(arg_parent_ptr_val);
        arg_parent = (void*)arg_parent_ptr;
    }
    
    // 调用底层函数
    lv_obj_t* ret_value = lv_arc_create(arg_parent);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = jerry_object();
    jerry_value_t ptr = jerry_number((double)(uintptr_t)ret_value);
    jerry_value_t cls = jerry_string_sz("lv_obj");
    jerry_object_set(js_result, jerry_string_sz("__ptr"), ptr);
    jerry_object_set(js_result, jerry_string_sz("__class"), cls);
    jerry_value_free(ptr);
    jerry_value_free(cls);

    return js_result;
}



/**
 * @brief Set the start and end angles of the arc background
 */
static jerry_value_t js_lv_arc_set_bg_angles(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: start (lv_value_precise_t)
    jerry_value_t js_arg_start = args[1];
    if (!jerry_value_is_number(js_arg_start)) {
        return throw_error("Argument 1 must be a number");
    }
    
    float arg_start = (float)jerry_value_as_number(js_arg_start);
    
    // 解析参数: end (lv_value_precise_t)
    jerry_value_t js_arg_end = args[2];
    if (!jerry_value_is_number(js_arg_end)) {
        return throw_error("Argument 2 must be a number");
    }
    
    float arg_end = (float)jerry_value_as_number(js_arg_end);
    
    // 调用底层函数
    lv_arc_set_bg_angles(arg_obj, arg_start, arg_end);

    return jerry_undefined();
}



/**
 * @brief Set a new value on the arc
 */
static jerry_value_t js_lv_arc_set_value(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return throw_error("Argument 1 must be a number");
    }
    
    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);
    
    // 调用底层函数
    lv_arc_set_value(arg_obj, arg_value);

    return jerry_undefined();
}



/**
 * @brief Set minimum and the maximum values of an arc
 */
static jerry_value_t js_lv_arc_set_range(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: min (int32_t)
    jerry_value_t js_arg_min = args[1];
    if (!jerry_value_is_number(js_arg_min)) {
        return throw_error("Argument 1 must be a number");
    }
    
    int32_t arg_min = (int32_t)jerry_value_as_number(js_arg_min);
    
    // 解析参数: max (int32_t)
    jerry_value_t js_arg_max = args[2];
    if (!jerry_value_is_number(js_arg_max)) {
        return throw_error("Argument 2 must be a number");
    }
    
    int32_t arg_max = (int32_t)jerry_value_as_number(js_arg_max);
    
    // 调用底层函数
    lv_arc_set_range(arg_obj, arg_min, arg_max);

    return jerry_undefined();
}



/**
 * @brief Create a label object pointer to the created button
 */
static jerry_value_t js_lv_label_create(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: parent (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_parent = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_parent = args[0];
        if (!jerry_value_is_object(js_arg_parent)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_parent_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_parent_ptr_val = jerry_object_get(js_arg_parent, arg_parent_ptr_prop);
        jerry_value_free(arg_parent_ptr_prop);
        
        if (!jerry_value_is_number(arg_parent_ptr_val)) {
            jerry_value_free(arg_parent_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_parent_ptr = (uintptr_t)jerry_value_as_number(arg_parent_ptr_val);
        jerry_value_free(arg_parent_ptr_val);
        arg_parent = (void*)arg_parent_ptr;
    }
    
    // 调用底层函数
    lv_obj_t* ret_value = lv_label_create(arg_parent);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = jerry_object();
    jerry_value_t ptr = jerry_number((double)(uintptr_t)ret_value);
    jerry_value_t cls = jerry_string_sz("lv_obj");
    jerry_object_set(js_result, jerry_string_sz("__ptr"), ptr);
    jerry_object_set(js_result, jerry_string_sz("__class"), cls);
    jerry_value_free(ptr);
    jerry_value_free(cls);

    return js_result;
}



/**
 * @brief Set a new text for a label. Memory will be allocated to store the text by the label.
 */
static jerry_value_t js_lv_label_set_text(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: text (const char*)

    char* arg_text_str = NULL;
    const char* arg_text = NULL;
    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1])) {
        if (!jerry_value_is_string(args[1])) {
            return throw_error("Argument 1 must be a string");
        }
        jerry_size_t arg_text_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
        arg_text_str = (char*)malloc(arg_text_len + 1);
        jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_text_str, arg_text_len);
        arg_text_str[arg_text_len] = '\0';
        arg_text = arg_text_str;
    }

    // 调用底层函数
    lv_label_set_text(arg_obj, arg_text);

    // 释放临时字符串内存
    if (arg_text_str) free(arg_text_str);

    return jerry_undefined();
}



/**
 * @brief Set the behavior of the label with text longer than the object size
 */
static jerry_value_t js_lv_label_set_long_mode(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: long_mode (lv_label_long_mode_t)
    jerry_value_t js_arg_long_mode = args[1];
    if (!jerry_value_is_number(js_arg_long_mode)) {
        return throw_error("Argument 1 must be a number");
    }
    
    lv_label_long_mode_t arg_long_mode = (lv_label_long_mode_t)jerry_value_as_number(js_arg_long_mode);
    
    // 调用底层函数
    lv_label_set_long_mode(arg_obj, arg_long_mode);

    return jerry_undefined();
}



/**
 * @brief Get the text of a label the text of the label
 */
static jerry_value_t js_lv_label_get_text(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 调用底层函数
    char* ret_value = lv_label_get_text(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    if (ret_value == NULL) {
        js_result = jerry_string_sz("");
    } else {
        js_result = jerry_string_sz((const jerry_char_t*)ret_value);
    }

    return js_result;
}



/**
 * @brief Create a bar object pointer to the created bar
 */
static jerry_value_t js_lv_bar_create(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: parent (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_parent = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_parent = args[0];
        if (!jerry_value_is_object(js_arg_parent)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_parent_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_parent_ptr_val = jerry_object_get(js_arg_parent, arg_parent_ptr_prop);
        jerry_value_free(arg_parent_ptr_prop);
        
        if (!jerry_value_is_number(arg_parent_ptr_val)) {
            jerry_value_free(arg_parent_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_parent_ptr = (uintptr_t)jerry_value_as_number(arg_parent_ptr_val);
        jerry_value_free(arg_parent_ptr_val);
        arg_parent = (void*)arg_parent_ptr;
    }
    
    // 调用底层函数
    lv_obj_t* ret_value = lv_bar_create(arg_parent);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = jerry_object();
    jerry_value_t ptr = jerry_number((double)(uintptr_t)ret_value);
    jerry_value_t cls = jerry_string_sz("lv_obj");
    jerry_object_set(js_result, jerry_string_sz("__ptr"), ptr);
    jerry_object_set(js_result, jerry_string_sz("__class"), cls);
    jerry_value_free(ptr);
    jerry_value_free(cls);

    return js_result;
}



/**
 * @brief Set a new value on the bar
 */
static jerry_value_t js_lv_bar_set_value(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return throw_error("Argument 1 must be a number");
    }
    
    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);
    
    // 解析参数: anim (lv_anim_enable_t)
    jerry_value_t js_arg_anim = args[2];
    if (!jerry_value_is_number(js_arg_anim)) {
        return throw_error("Argument 2 must be a number");
    }
    
    int arg_anim = (int)jerry_value_as_number(js_arg_anim);
    
    // 调用底层函数
    lv_bar_set_value(arg_obj, arg_value, arg_anim);

    return jerry_undefined();
}



/**
 * @brief Set minimum and the maximum values of a bar If min is greater than max, the drawing direction becomes to the opposite direction.
 */
static jerry_value_t js_lv_bar_set_range(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: min (int32_t)
    jerry_value_t js_arg_min = args[1];
    if (!jerry_value_is_number(js_arg_min)) {
        return throw_error("Argument 1 must be a number");
    }
    
    int32_t arg_min = (int32_t)jerry_value_as_number(js_arg_min);
    
    // 解析参数: max (int32_t)
    jerry_value_t js_arg_max = args[2];
    if (!jerry_value_is_number(js_arg_max)) {
        return throw_error("Argument 2 must be a number");
    }
    
    int32_t arg_max = (int32_t)jerry_value_as_number(js_arg_max);
    
    // 调用底层函数
    lv_bar_set_range(arg_obj, arg_min, arg_max);

    return jerry_undefined();
}



/**
 * @brief Create a chart object pointer to the created chart
 */
static jerry_value_t js_lv_chart_create(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: parent (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_parent = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_parent = args[0];
        if (!jerry_value_is_object(js_arg_parent)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_parent_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_parent_ptr_val = jerry_object_get(js_arg_parent, arg_parent_ptr_prop);
        jerry_value_free(arg_parent_ptr_prop);
        
        if (!jerry_value_is_number(arg_parent_ptr_val)) {
            jerry_value_free(arg_parent_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_parent_ptr = (uintptr_t)jerry_value_as_number(arg_parent_ptr_val);
        jerry_value_free(arg_parent_ptr_val);
        arg_parent = (void*)arg_parent_ptr;
    }
    
    // 调用底层函数
    lv_obj_t* ret_value = lv_chart_create(arg_parent);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = jerry_object();
    jerry_value_t ptr = jerry_number((double)(uintptr_t)ret_value);
    jerry_value_t cls = jerry_string_sz("lv_obj");
    jerry_object_set(js_result, jerry_string_sz("__ptr"), ptr);
    jerry_object_set(js_result, jerry_string_sz("__class"), cls);
    jerry_value_free(ptr);
    jerry_value_free(cls);

    return js_result;
}



/**
 * @brief Set a new type for a chart
 */
static jerry_value_t js_lv_chart_set_type(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: type (lv_chart_type_t)
    jerry_value_t js_arg_type = args[1];
    if (!jerry_value_is_number(js_arg_type)) {
        return throw_error("Argument 1 must be a number");
    }
    
    int arg_type = (int)jerry_value_as_number(js_arg_type);
    
    // 调用底层函数
    lv_chart_set_type(arg_obj, arg_type);

    return jerry_undefined();
}



/**
 * @brief Set the number of points on a data line on a chart
 */
static jerry_value_t js_lv_chart_set_point_count(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: cnt (uint32_t)
    jerry_value_t js_arg_cnt = args[1];
    if (!jerry_value_is_number(js_arg_cnt)) {
        return throw_error("Argument 1 must be a number");
    }
    
    uint32_t arg_cnt = (uint32_t)jerry_value_as_number(js_arg_cnt);
    
    // 调用底层函数
    lv_chart_set_point_count(arg_obj, arg_cnt);

    return jerry_undefined();
}



/**
 * @brief Set the minimal and maximal y values on an axis
 */
static jerry_value_t js_lv_chart_set_range(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 4) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: axis (lv_chart_axis_t)
    jerry_value_t js_arg_axis = args[1];
    if (!jerry_value_is_number(js_arg_axis)) {
        return throw_error("Argument 1 must be a number");
    }
    
    int arg_axis = (int)jerry_value_as_number(js_arg_axis);
    
    // 解析参数: min (int32_t)
    jerry_value_t js_arg_min = args[2];
    if (!jerry_value_is_number(js_arg_min)) {
        return throw_error("Argument 2 must be a number");
    }
    
    int32_t arg_min = (int32_t)jerry_value_as_number(js_arg_min);
    
    // 解析参数: max (int32_t)
    jerry_value_t js_arg_max = args[3];
    if (!jerry_value_is_number(js_arg_max)) {
        return throw_error("Argument 3 must be a number");
    }
    
    int32_t arg_max = (int32_t)jerry_value_as_number(js_arg_max);
    
    // 调用底层函数
    lv_chart_set_range(arg_obj, arg_axis, arg_min, arg_max);

    return jerry_undefined();
}



/**
 * @brief Create a check box object pointer to the created check box
 */
static jerry_value_t js_lv_checkbox_create(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: parent (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_parent = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_parent = args[0];
        if (!jerry_value_is_object(js_arg_parent)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_parent_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_parent_ptr_val = jerry_object_get(js_arg_parent, arg_parent_ptr_prop);
        jerry_value_free(arg_parent_ptr_prop);
        
        if (!jerry_value_is_number(arg_parent_ptr_val)) {
            jerry_value_free(arg_parent_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_parent_ptr = (uintptr_t)jerry_value_as_number(arg_parent_ptr_val);
        jerry_value_free(arg_parent_ptr_val);
        arg_parent = (void*)arg_parent_ptr;
    }
    
    // 调用底层函数
    lv_obj_t* ret_value = lv_checkbox_create(arg_parent);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = jerry_object();
    jerry_value_t ptr = jerry_number((double)(uintptr_t)ret_value);
    jerry_value_t cls = jerry_string_sz("lv_obj");
    jerry_object_set(js_result, jerry_string_sz("__ptr"), ptr);
    jerry_object_set(js_result, jerry_string_sz("__class"), cls);
    jerry_value_free(ptr);
    jerry_value_free(cls);

    return js_result;
}



/**
 * @brief Set the text of a check box. txt will be copied and may be deallocated after this function returns.
 */
static jerry_value_t js_lv_checkbox_set_text(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: txt (const char*)

    char* arg_txt_str = NULL;
    const char* arg_txt = NULL;
    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1])) {
        if (!jerry_value_is_string(args[1])) {
            return throw_error("Argument 1 must be a string");
        }
        jerry_size_t arg_txt_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
        arg_txt_str = (char*)malloc(arg_txt_len + 1);
        jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_txt_str, arg_txt_len);
        arg_txt_str[arg_txt_len] = '\0';
        arg_txt = arg_txt_str;
    }

    // 调用底层函数
    lv_checkbox_set_text(arg_obj, arg_txt);

    // 释放临时字符串内存
    if (arg_txt_str) free(arg_txt_str);

    return jerry_undefined();
}



/**
 * @brief Create a drop-down list object pointer to the created drop-down list
 */
static jerry_value_t js_lv_dropdown_create(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: parent (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_parent = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_parent = args[0];
        if (!jerry_value_is_object(js_arg_parent)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_parent_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_parent_ptr_val = jerry_object_get(js_arg_parent, arg_parent_ptr_prop);
        jerry_value_free(arg_parent_ptr_prop);
        
        if (!jerry_value_is_number(arg_parent_ptr_val)) {
            jerry_value_free(arg_parent_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_parent_ptr = (uintptr_t)jerry_value_as_number(arg_parent_ptr_val);
        jerry_value_free(arg_parent_ptr_val);
        arg_parent = (void*)arg_parent_ptr;
    }
    
    // 调用底层函数
    lv_obj_t* ret_value = lv_dropdown_create(arg_parent);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = jerry_object();
    jerry_value_t ptr = jerry_number((double)(uintptr_t)ret_value);
    jerry_value_t cls = jerry_string_sz("lv_obj");
    jerry_object_set(js_result, jerry_string_sz("__ptr"), ptr);
    jerry_object_set(js_result, jerry_string_sz("__class"), cls);
    jerry_value_free(ptr);
    jerry_value_free(cls);

    return js_result;
}



/**
 * @brief Set the options in a drop-down list from a string. The options will be copied and saved in the object so the options can be destroyed after calling this function
 */
static jerry_value_t js_lv_dropdown_set_options(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: options (const char*)

    char* arg_options_str = NULL;
    const char* arg_options = NULL;
    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1])) {
        if (!jerry_value_is_string(args[1])) {
            return throw_error("Argument 1 must be a string");
        }
        jerry_size_t arg_options_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
        arg_options_str = (char*)malloc(arg_options_len + 1);
        jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_options_str, arg_options_len);
        arg_options_str[arg_options_len] = '\0';
        arg_options = arg_options_str;
    }

    // 调用底层函数
    lv_dropdown_set_options(arg_obj, arg_options);

    // 释放临时字符串内存
    if (arg_options_str) free(arg_options_str);

    return jerry_undefined();
}



/**
 * @brief Set the selected option
 */
static jerry_value_t js_lv_dropdown_set_selected(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: sel_opt (uint32_t)
    jerry_value_t js_arg_sel_opt = args[1];
    if (!jerry_value_is_number(js_arg_sel_opt)) {
        return throw_error("Argument 1 must be a number");
    }
    
    uint32_t arg_sel_opt = (uint32_t)jerry_value_as_number(js_arg_sel_opt);
    
    // 调用底层函数
    lv_dropdown_set_selected(arg_obj, arg_sel_opt);

    return jerry_undefined();
}



/**
 * @brief Get the index of the selected option index of the selected option (0 ... number of option - 1);
 */
static jerry_value_t js_lv_dropdown_get_selected(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 调用底层函数
    uint32_t ret_value = lv_dropdown_get_selected(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Create an empty message box the created message box
 */
static jerry_value_t js_lv_msgbox_create(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: parent (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_parent = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_parent = args[0];
        if (!jerry_value_is_object(js_arg_parent)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_parent_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_parent_ptr_val = jerry_object_get(js_arg_parent, arg_parent_ptr_prop);
        jerry_value_free(arg_parent_ptr_prop);
        
        if (!jerry_value_is_number(arg_parent_ptr_val)) {
            jerry_value_free(arg_parent_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_parent_ptr = (uintptr_t)jerry_value_as_number(arg_parent_ptr_val);
        jerry_value_free(arg_parent_ptr_val);
        arg_parent = (void*)arg_parent_ptr;
    }
    
    // 调用底层函数
    lv_obj_t* ret_value = lv_msgbox_create(arg_parent);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = jerry_object();
    jerry_value_t ptr = jerry_number((double)(uintptr_t)ret_value);
    jerry_value_t cls = jerry_string_sz("lv_obj");
    jerry_object_set(js_result, jerry_string_sz("__ptr"), ptr);
    jerry_object_set(js_result, jerry_string_sz("__class"), cls);
    jerry_value_free(ptr);
    jerry_value_free(cls);

    return js_result;
}



/**
 * @brief Add title to the message box. It also creates a header for the title. the created title label
 */
static jerry_value_t js_lv_msgbox_add_title(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: title (const char*)

    char* arg_title_str = NULL;
    const char* arg_title = NULL;
    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1])) {
        if (!jerry_value_is_string(args[1])) {
            return throw_error("Argument 1 must be a string");
        }
        jerry_size_t arg_title_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
        arg_title_str = (char*)malloc(arg_title_len + 1);
        jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_title_str, arg_title_len);
        arg_title_str[arg_title_len] = '\0';
        arg_title = arg_title_str;
    }

    // 调用底层函数
    lv_obj_t* ret_value = lv_msgbox_add_title(arg_obj, arg_title);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = jerry_object();
    jerry_value_t ptr = jerry_number((double)(uintptr_t)ret_value);
    jerry_value_t cls = jerry_string_sz("lv_obj");
    jerry_object_set(js_result, jerry_string_sz("__ptr"), ptr);
    jerry_object_set(js_result, jerry_string_sz("__class"), cls);
    jerry_value_free(ptr);
    jerry_value_free(cls);

    // 释放临时字符串内存
    if (arg_title_str) free(arg_title_str);

    return js_result;
}



/**
 * @brief Add a button to the header of to the message box. It also creates a header. the created button
 */
static jerry_value_t js_lv_msgbox_add_header_button(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: icon (const void*)
    // void*/字符串 类型参数，支持null
    void* arg_icon = NULL;
    char* arg_icon_str = NULL;  // 用于字符串参数的临时存储
    
    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1])) {
        if (jerry_value_is_string(args[1])) {
            // 处理字符串类型的符号（如LV_SYMBOL_MINUS）
            jerry_size_t arg_icon_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
            arg_icon_str = (char*)malloc(arg_icon_len + 1);
            if (!arg_icon_str) {
                return throw_error("Failed to allocate memory for string argument");
            }
            jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_icon_str, arg_icon_len);
            arg_icon_str[arg_icon_len] = '\0';
            arg_icon = (void*)arg_icon_str;
        }
        else if (jerry_value_is_object(args[1])) {
            // 尝试从对象获取指针
            jerry_value_t ptr_prop = jerry_string_sz("__ptr");
            jerry_value_t ptr_val = jerry_object_get(args[1], ptr_prop);
            jerry_value_free(ptr_prop);
            
            if (jerry_value_is_number(ptr_val)) {
                uintptr_t ptr_num = (uintptr_t)jerry_value_as_number(ptr_val);
                arg_icon = (void*)ptr_num;
            }
            jerry_value_free(ptr_val);
        }
        else if (jerry_value_is_number(args[1])) {
            // 直接传递指针数值
            uintptr_t ptr_num = (uintptr_t)jerry_value_as_number(args[1]);
            arg_icon = (void*)ptr_num;
        }
        else {
            if (arg_icon_str) free(arg_icon_str);
            return throw_error("Argument 1 must be string, object or number");
        }
    }
    
    // 注意：需要在函数末尾添加 free(arg_icon_str);
    // 调用底层函数
    lv_obj_t* ret_value = lv_msgbox_add_header_button(arg_obj, arg_icon);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = jerry_object();
    jerry_value_t ptr = jerry_number((double)(uintptr_t)ret_value);
    jerry_value_t cls = jerry_string_sz("lv_obj");
    jerry_object_set(js_result, jerry_string_sz("__ptr"), ptr);
    jerry_object_set(js_result, jerry_string_sz("__class"), cls);
    jerry_value_free(ptr);
    jerry_value_free(cls);

    return js_result;
}



/**
 * @brief Add a text to the content area of message box. Multiple texts will be created below each other. the created button
 */
static jerry_value_t js_lv_msgbox_add_text(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: text (const char*)

    char* arg_text_str = NULL;
    const char* arg_text = NULL;
    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1])) {
        if (!jerry_value_is_string(args[1])) {
            return throw_error("Argument 1 must be a string");
        }
        jerry_size_t arg_text_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
        arg_text_str = (char*)malloc(arg_text_len + 1);
        jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_text_str, arg_text_len);
        arg_text_str[arg_text_len] = '\0';
        arg_text = arg_text_str;
    }

    // 调用底层函数
    lv_obj_t* ret_value = lv_msgbox_add_text(arg_obj, arg_text);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = jerry_object();
    jerry_value_t ptr = jerry_number((double)(uintptr_t)ret_value);
    jerry_value_t cls = jerry_string_sz("lv_obj");
    jerry_object_set(js_result, jerry_string_sz("__ptr"), ptr);
    jerry_object_set(js_result, jerry_string_sz("__class"), cls);
    jerry_value_free(ptr);
    jerry_value_free(cls);

    // 释放临时字符串内存
    if (arg_text_str) free(arg_text_str);

    return js_result;
}



/**
 * @brief Add a button to the footer of to the message box. It also creates a footer. the created button
 */
static jerry_value_t js_lv_msgbox_add_footer_button(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: text (const char*)

    char* arg_text_str = NULL;
    const char* arg_text = NULL;
    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1])) {
        if (!jerry_value_is_string(args[1])) {
            return throw_error("Argument 1 must be a string");
        }
        jerry_size_t arg_text_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
        arg_text_str = (char*)malloc(arg_text_len + 1);
        jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_text_str, arg_text_len);
        arg_text_str[arg_text_len] = '\0';
        arg_text = arg_text_str;
    }

    // 调用底层函数
    lv_obj_t* ret_value = lv_msgbox_add_footer_button(arg_obj, arg_text);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = jerry_object();
    jerry_value_t ptr = jerry_number((double)(uintptr_t)ret_value);
    jerry_value_t cls = jerry_string_sz("lv_obj");
    jerry_object_set(js_result, jerry_string_sz("__ptr"), ptr);
    jerry_object_set(js_result, jerry_string_sz("__class"), cls);
    jerry_value_free(ptr);
    jerry_value_free(cls);

    // 释放临时字符串内存
    if (arg_text_str) free(arg_text_str);

    return js_result;
}



/**
 * @brief Add a close button to the message box. It also creates a header. the created close button
 */
static jerry_value_t js_lv_msgbox_add_close_button(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 调用底层函数
    lv_obj_t* ret_value = lv_msgbox_add_close_button(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = jerry_object();
    jerry_value_t ptr = jerry_number((double)(uintptr_t)ret_value);
    jerry_value_t cls = jerry_string_sz("lv_obj");
    jerry_object_set(js_result, jerry_string_sz("__ptr"), ptr);
    jerry_object_set(js_result, jerry_string_sz("__class"), cls);
    jerry_value_free(ptr);
    jerry_value_free(cls);

    return js_result;
}



/**
 * @brief Close a message box
 */
static jerry_value_t js_lv_msgbox_close(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: mbox (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_mbox = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_mbox = args[0];
        if (!jerry_value_is_object(js_arg_mbox)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_mbox_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_mbox_ptr_val = jerry_object_get(js_arg_mbox, arg_mbox_ptr_prop);
        jerry_value_free(arg_mbox_ptr_prop);
        
        if (!jerry_value_is_number(arg_mbox_ptr_val)) {
            jerry_value_free(arg_mbox_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_mbox_ptr = (uintptr_t)jerry_value_as_number(arg_mbox_ptr_val);
        jerry_value_free(arg_mbox_ptr_val);
        arg_mbox = (void*)arg_mbox_ptr;
    }
    
    // 调用底层函数
    lv_msgbox_close(arg_mbox);

    return jerry_undefined();
}



/**
 * @brief Create a roller object pointer to the created roller
 */
static jerry_value_t js_lv_roller_create(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: parent (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_parent = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_parent = args[0];
        if (!jerry_value_is_object(js_arg_parent)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_parent_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_parent_ptr_val = jerry_object_get(js_arg_parent, arg_parent_ptr_prop);
        jerry_value_free(arg_parent_ptr_prop);
        
        if (!jerry_value_is_number(arg_parent_ptr_val)) {
            jerry_value_free(arg_parent_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_parent_ptr = (uintptr_t)jerry_value_as_number(arg_parent_ptr_val);
        jerry_value_free(arg_parent_ptr_val);
        arg_parent = (void*)arg_parent_ptr;
    }
    
    // 调用底层函数
    lv_obj_t* ret_value = lv_roller_create(arg_parent);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = jerry_object();
    jerry_value_t ptr = jerry_number((double)(uintptr_t)ret_value);
    jerry_value_t cls = jerry_string_sz("lv_obj");
    jerry_object_set(js_result, jerry_string_sz("__ptr"), ptr);
    jerry_object_set(js_result, jerry_string_sz("__class"), cls);
    jerry_value_free(ptr);
    jerry_value_free(cls);

    return js_result;
}



/**
 * @brief Set the options on a roller
 */
static jerry_value_t js_lv_roller_set_options(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: options (const char*)

    char* arg_options_str = NULL;
    const char* arg_options = NULL;
    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1])) {
        if (!jerry_value_is_string(args[1])) {
            return throw_error("Argument 1 must be a string");
        }
        jerry_size_t arg_options_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
        arg_options_str = (char*)malloc(arg_options_len + 1);
        jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_options_str, arg_options_len);
        arg_options_str[arg_options_len] = '\0';
        arg_options = arg_options_str;
    }

    // 解析参数: mode (lv_roller_mode_t)
    jerry_value_t js_arg_mode = args[2];
    if (!jerry_value_is_number(js_arg_mode)) {
        return throw_error("Argument 2 must be a number");
    }
    
    int arg_mode = (int)jerry_value_as_number(js_arg_mode);
    
    // 调用底层函数
    lv_roller_set_options(arg_obj, arg_options, arg_mode);

    // 释放临时字符串内存
    if (arg_options_str) free(arg_options_str);

    return jerry_undefined();
}



/**
 * @brief Set the selected option
 */
static jerry_value_t js_lv_roller_set_selected(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: sel_opt (uint32_t)
    jerry_value_t js_arg_sel_opt = args[1];
    if (!jerry_value_is_number(js_arg_sel_opt)) {
        return throw_error("Argument 1 must be a number");
    }
    
    uint32_t arg_sel_opt = (uint32_t)jerry_value_as_number(js_arg_sel_opt);
    
    // 解析参数: anim (lv_anim_enable_t)
    jerry_value_t js_arg_anim = args[2];
    if (!jerry_value_is_number(js_arg_anim)) {
        return throw_error("Argument 2 must be a number");
    }
    
    int arg_anim = (int)jerry_value_as_number(js_arg_anim);
    
    // 调用底层函数
    lv_roller_set_selected(arg_obj, arg_sel_opt, arg_anim);

    return jerry_undefined();
}



/**
 * @brief Create a slider object pointer to the created slider
 */
static jerry_value_t js_lv_slider_create(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: parent (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_parent = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_parent = args[0];
        if (!jerry_value_is_object(js_arg_parent)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_parent_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_parent_ptr_val = jerry_object_get(js_arg_parent, arg_parent_ptr_prop);
        jerry_value_free(arg_parent_ptr_prop);
        
        if (!jerry_value_is_number(arg_parent_ptr_val)) {
            jerry_value_free(arg_parent_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_parent_ptr = (uintptr_t)jerry_value_as_number(arg_parent_ptr_val);
        jerry_value_free(arg_parent_ptr_val);
        arg_parent = (void*)arg_parent_ptr;
    }
    
    // 调用底层函数
    lv_obj_t* ret_value = lv_slider_create(arg_parent);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = jerry_object();
    jerry_value_t ptr = jerry_number((double)(uintptr_t)ret_value);
    jerry_value_t cls = jerry_string_sz("lv_obj");
    jerry_object_set(js_result, jerry_string_sz("__ptr"), ptr);
    jerry_object_set(js_result, jerry_string_sz("__class"), cls);
    jerry_value_free(ptr);
    jerry_value_free(cls);

    return js_result;
}



/**
 * @brief Set a new value on the slider
 */
static jerry_value_t js_lv_slider_set_value(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return throw_error("Argument 1 must be a number");
    }
    
    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);
    
    // 解析参数: anim (lv_anim_enable_t)
    jerry_value_t js_arg_anim = args[2];
    if (!jerry_value_is_number(js_arg_anim)) {
        return throw_error("Argument 2 must be a number");
    }
    
    int arg_anim = (int)jerry_value_as_number(js_arg_anim);
    
    // 调用底层函数
    lv_slider_set_value(arg_obj, arg_value, arg_anim);

    return jerry_undefined();
}



/**
 * @brief Set minimum and the maximum values of a bar
 */
static jerry_value_t js_lv_slider_set_range(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: min (int32_t)
    jerry_value_t js_arg_min = args[1];
    if (!jerry_value_is_number(js_arg_min)) {
        return throw_error("Argument 1 must be a number");
    }
    
    int32_t arg_min = (int32_t)jerry_value_as_number(js_arg_min);
    
    // 解析参数: max (int32_t)
    jerry_value_t js_arg_max = args[2];
    if (!jerry_value_is_number(js_arg_max)) {
        return throw_error("Argument 2 must be a number");
    }
    
    int32_t arg_max = (int32_t)jerry_value_as_number(js_arg_max);
    
    // 调用底层函数
    lv_slider_set_range(arg_obj, arg_min, arg_max);

    return jerry_undefined();
}



/**
 * @brief Get the value of the main knob of a slider the value of the main knob of the slider
 */
static jerry_value_t js_lv_slider_get_value(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 调用底层函数
    int32_t ret_value = lv_slider_get_value(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Create a text area object pointer to the created text area
 */
static jerry_value_t js_lv_textarea_create(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: parent (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_parent = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_parent = args[0];
        if (!jerry_value_is_object(js_arg_parent)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_parent_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_parent_ptr_val = jerry_object_get(js_arg_parent, arg_parent_ptr_prop);
        jerry_value_free(arg_parent_ptr_prop);
        
        if (!jerry_value_is_number(arg_parent_ptr_val)) {
            jerry_value_free(arg_parent_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_parent_ptr = (uintptr_t)jerry_value_as_number(arg_parent_ptr_val);
        jerry_value_free(arg_parent_ptr_val);
        arg_parent = (void*)arg_parent_ptr;
    }
    
    // 调用底层函数
    lv_obj_t* ret_value = lv_textarea_create(arg_parent);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = jerry_object();
    jerry_value_t ptr = jerry_number((double)(uintptr_t)ret_value);
    jerry_value_t cls = jerry_string_sz("lv_obj");
    jerry_object_set(js_result, jerry_string_sz("__ptr"), ptr);
    jerry_object_set(js_result, jerry_string_sz("__class"), cls);
    jerry_value_free(ptr);
    jerry_value_free(cls);

    return js_result;
}



/**
 * @brief Insert a text to the current cursor position
 */
static jerry_value_t js_lv_textarea_add_text(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: txt (const char*)

    char* arg_txt_str = NULL;
    const char* arg_txt = NULL;
    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1])) {
        if (!jerry_value_is_string(args[1])) {
            return throw_error("Argument 1 must be a string");
        }
        jerry_size_t arg_txt_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
        arg_txt_str = (char*)malloc(arg_txt_len + 1);
        jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_txt_str, arg_txt_len);
        arg_txt_str[arg_txt_len] = '\0';
        arg_txt = arg_txt_str;
    }

    // 调用底层函数
    lv_textarea_add_text(arg_obj, arg_txt);

    // 释放临时字符串内存
    if (arg_txt_str) free(arg_txt_str);

    return jerry_undefined();
}



/**
 * @brief Set the text of a text area
 */
static jerry_value_t js_lv_textarea_set_text(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: txt (const char*)

    char* arg_txt_str = NULL;
    const char* arg_txt = NULL;
    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1])) {
        if (!jerry_value_is_string(args[1])) {
            return throw_error("Argument 1 must be a string");
        }
        jerry_size_t arg_txt_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
        arg_txt_str = (char*)malloc(arg_txt_len + 1);
        jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_txt_str, arg_txt_len);
        arg_txt_str[arg_txt_len] = '\0';
        arg_txt = arg_txt_str;
    }

    // 调用底层函数
    lv_textarea_set_text(arg_obj, arg_txt);

    // 释放临时字符串内存
    if (arg_txt_str) free(arg_txt_str);

    return jerry_undefined();
}



/**
 * @brief Set the placeholder text of a text area
 */
static jerry_value_t js_lv_textarea_set_placeholder_text(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: txt (const char*)

    char* arg_txt_str = NULL;
    const char* arg_txt = NULL;
    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1])) {
        if (!jerry_value_is_string(args[1])) {
            return throw_error("Argument 1 must be a string");
        }
        jerry_size_t arg_txt_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
        arg_txt_str = (char*)malloc(arg_txt_len + 1);
        jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_txt_str, arg_txt_len);
        arg_txt_str[arg_txt_len] = '\0';
        arg_txt = arg_txt_str;
    }

    // 调用底层函数
    lv_textarea_set_placeholder_text(arg_obj, arg_txt);

    // 释放临时字符串内存
    if (arg_txt_str) free(arg_txt_str);

    return jerry_undefined();
}



/**
 * @brief Create a switch object pointer to the created switch
 */
static jerry_value_t js_lv_switch_create(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: parent (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_parent = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_parent = args[0];
        if (!jerry_value_is_object(js_arg_parent)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_parent_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_parent_ptr_val = jerry_object_get(js_arg_parent, arg_parent_ptr_prop);
        jerry_value_free(arg_parent_ptr_prop);
        
        if (!jerry_value_is_number(arg_parent_ptr_val)) {
            jerry_value_free(arg_parent_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_parent_ptr = (uintptr_t)jerry_value_as_number(arg_parent_ptr_val);
        jerry_value_free(arg_parent_ptr_val);
        arg_parent = (void*)arg_parent_ptr;
    }
    
    // 调用底层函数
    lv_obj_t* ret_value = lv_switch_create(arg_parent);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = jerry_object();
    jerry_value_t ptr = jerry_number((double)(uintptr_t)ret_value);
    jerry_value_t cls = jerry_string_sz("lv_obj");
    jerry_object_set(js_result, jerry_string_sz("__ptr"), ptr);
    jerry_object_set(js_result, jerry_string_sz("__class"), cls);
    jerry_value_free(ptr);
    jerry_value_free(cls);

    return js_result;
}



/**
 * @brief Create a table object pointer to the created table
 */
static jerry_value_t js_lv_table_create(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: parent (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_parent = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_parent = args[0];
        if (!jerry_value_is_object(js_arg_parent)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_parent_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_parent_ptr_val = jerry_object_get(js_arg_parent, arg_parent_ptr_prop);
        jerry_value_free(arg_parent_ptr_prop);
        
        if (!jerry_value_is_number(arg_parent_ptr_val)) {
            jerry_value_free(arg_parent_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_parent_ptr = (uintptr_t)jerry_value_as_number(arg_parent_ptr_val);
        jerry_value_free(arg_parent_ptr_val);
        arg_parent = (void*)arg_parent_ptr;
    }
    
    // 调用底层函数
    lv_obj_t* ret_value = lv_table_create(arg_parent);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = jerry_object();
    jerry_value_t ptr = jerry_number((double)(uintptr_t)ret_value);
    jerry_value_t cls = jerry_string_sz("lv_obj");
    jerry_object_set(js_result, jerry_string_sz("__ptr"), ptr);
    jerry_object_set(js_result, jerry_string_sz("__class"), cls);
    jerry_value_free(ptr);
    jerry_value_free(cls);

    return js_result;
}



/**
 * @brief Set the value of a cell. New roes/columns are added automatically if required
 */
static jerry_value_t js_lv_table_set_cell_value(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 4) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: row (uint32_t)
    jerry_value_t js_arg_row = args[1];
    if (!jerry_value_is_number(js_arg_row)) {
        return throw_error("Argument 1 must be a number");
    }
    
    uint32_t arg_row = (uint32_t)jerry_value_as_number(js_arg_row);
    
    // 解析参数: col (uint32_t)
    jerry_value_t js_arg_col = args[2];
    if (!jerry_value_is_number(js_arg_col)) {
        return throw_error("Argument 2 must be a number");
    }
    
    uint32_t arg_col = (uint32_t)jerry_value_as_number(js_arg_col);
    
    // 解析参数: txt (const char*)

    char* arg_txt_str = NULL;
    const char* arg_txt = NULL;
    if (!jerry_value_is_undefined(args[3]) && !jerry_value_is_null(args[3])) {
        if (!jerry_value_is_string(args[3])) {
            return throw_error("Argument 3 must be a string");
        }
        jerry_size_t arg_txt_len = jerry_string_size(args[3], JERRY_ENCODING_UTF8);
        arg_txt_str = (char*)malloc(arg_txt_len + 1);
        jerry_string_to_buffer(args[3], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_txt_str, arg_txt_len);
        arg_txt_str[arg_txt_len] = '\0';
        arg_txt = arg_txt_str;
    }

    // 调用底层函数
    lv_table_set_cell_value(arg_obj, arg_row, arg_col, arg_txt);

    // 释放临时字符串内存
    if (arg_txt_str) free(arg_txt_str);

    return jerry_undefined();
}



/**
 * @brief Delete an object and all of its children. Also remove the objects from their group and remove all animations (if any). Send LV_EVENT_DELETED to deleted objects.
 */
static jerry_value_t js_lv_obj_del(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 调用底层函数
    lv_obj_delete(arg_obj);

    return jerry_undefined();
}



/**
 * @brief Remove one or more flags
 */
static jerry_value_t js_lv_obj_clear_flag(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: f (lv_obj_flag_t)
    jerry_value_t js_arg_f = args[1];
    if (!jerry_value_is_number(js_arg_f)) {
        return throw_error("Argument 1 must be a number");
    }
    
    int arg_f = (int)jerry_value_as_number(js_arg_f);
    
    // 调用底层函数
    lv_obj_remove_flag(arg_obj, arg_f);

    return jerry_undefined();
}



/**
 * @brief Get the active screen of the default display pointer to the active screen
 */
static jerry_value_t js_lv_scr_act(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 调用底层函数
    lv_obj_t* ret_value = lv_screen_active();

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = jerry_object();
    jerry_value_t ptr = jerry_number((double)(uintptr_t)ret_value);
    jerry_value_t cls = jerry_string_sz("lv_obj");
    jerry_object_set(js_result, jerry_string_sz("__ptr"), ptr);
    jerry_object_set(js_result, jerry_string_sz("__class"), cls);
    jerry_value_free(ptr);
    jerry_value_free(cls);

    return js_result;
}



/**
 * @brief Return a pointer to the active screen on a display pointer to the active screen object (loaded by ' :ref:`lv_screen_load()` ')
 */
static jerry_value_t js_lv_disp_get_scr_act(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: disp (lv_display_t*)
    // 对象类型参数，支持null
    void* arg_disp = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_disp = args[0];
        if (!jerry_value_is_object(js_arg_disp)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_disp_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_disp_ptr_val = jerry_object_get(js_arg_disp, arg_disp_ptr_prop);
        jerry_value_free(arg_disp_ptr_prop);
        
        if (!jerry_value_is_number(arg_disp_ptr_val)) {
            jerry_value_free(arg_disp_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_disp_ptr = (uintptr_t)jerry_value_as_number(arg_disp_ptr_val);
        jerry_value_free(arg_disp_ptr_val);
        arg_disp = (void*)arg_disp_ptr;
    }
    
    // 调用底层函数
    lv_obj_t* ret_value = lv_display_get_screen_active(arg_disp);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = jerry_object();
    jerry_value_t ptr = jerry_number((double)(uintptr_t)ret_value);
    jerry_value_t cls = jerry_string_sz("lv_obj");
    jerry_object_set(js_result, jerry_string_sz("__ptr"), ptr);
    jerry_object_set(js_result, jerry_string_sz("__class"), cls);
    jerry_value_free(ptr);
    jerry_value_free(cls);

    return js_result;
}



/**
 * @brief Load a screen on the default display
 */
static jerry_value_t js_lv_scr_load(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: scr (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_scr = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_scr = args[0];
        if (!jerry_value_is_object(js_arg_scr)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_scr_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_scr_ptr_val = jerry_object_get(js_arg_scr, arg_scr_ptr_prop);
        jerry_value_free(arg_scr_ptr_prop);
        
        if (!jerry_value_is_number(arg_scr_ptr_val)) {
            jerry_value_free(arg_scr_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_scr_ptr = (uintptr_t)jerry_value_as_number(arg_scr_ptr_val);
        jerry_value_free(arg_scr_ptr_val);
        arg_scr = (void*)arg_scr_ptr;
    }
    
    // 调用底层函数
    lv_screen_load(arg_scr);

    return jerry_undefined();
}



/**
 * @brief Create an image object pointer to the created image
 */
static jerry_value_t js_lv_img_create(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: parent (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_parent = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_parent = args[0];
        if (!jerry_value_is_object(js_arg_parent)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_parent_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_parent_ptr_val = jerry_object_get(js_arg_parent, arg_parent_ptr_prop);
        jerry_value_free(arg_parent_ptr_prop);
        
        if (!jerry_value_is_number(arg_parent_ptr_val)) {
            jerry_value_free(arg_parent_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_parent_ptr = (uintptr_t)jerry_value_as_number(arg_parent_ptr_val);
        jerry_value_free(arg_parent_ptr_val);
        arg_parent = (void*)arg_parent_ptr;
    }
    
    // 调用底层函数
    lv_obj_t* ret_value = lv_image_create(arg_parent);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = jerry_object();
    jerry_value_t ptr = jerry_number((double)(uintptr_t)ret_value);
    jerry_value_t cls = jerry_string_sz("lv_obj");
    jerry_object_set(js_result, jerry_string_sz("__ptr"), ptr);
    jerry_object_set(js_result, jerry_string_sz("__class"), cls);
    jerry_value_free(ptr);
    jerry_value_free(cls);

    return js_result;
}



/**
 * @brief Set the rotation angle of the image. The image will be rotated around the set pivot set by :ref:`lv_image_set_pivot()` Note that indexed and alpha only images can't be transformed. if image_align is LV_IMAGE_ALIGN_STRETCH or LV_IMAGE_ALIGN_FIT rotation will be set to 0 automatically.
 */
static jerry_value_t js_lv_img_set_angle(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: angle (int32_t)
    jerry_value_t js_arg_angle = args[1];
    if (!jerry_value_is_number(js_arg_angle)) {
        return throw_error("Argument 1 must be a number");
    }
    
    int32_t arg_angle = (int32_t)jerry_value_as_number(js_arg_angle);
    
    // 调用底层函数
    lv_image_set_rotation(arg_obj, arg_angle);

    return jerry_undefined();
}



/**
 * @brief Set the zoom factor of the image. Note that indexed and alpha only images can't be transformed.
 */
static jerry_value_t js_lv_img_set_zoom(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: zoom (uint32_t)
    jerry_value_t js_arg_zoom = args[1];
    if (!jerry_value_is_number(js_arg_zoom)) {
        return throw_error("Argument 1 must be a number");
    }
    
    uint32_t arg_zoom = (uint32_t)jerry_value_as_number(js_arg_zoom);
    
    // 调用底层函数
    lv_image_set_scale(arg_obj, arg_zoom);

    return jerry_undefined();
}



/**
 * @brief Create a button object pointer to the created button
 */
static jerry_value_t js_lv_btn_create(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: parent (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_parent = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_parent = args[0];
        if (!jerry_value_is_object(js_arg_parent)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_parent_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_parent_ptr_val = jerry_object_get(js_arg_parent, arg_parent_ptr_prop);
        jerry_value_free(arg_parent_ptr_prop);
        
        if (!jerry_value_is_number(arg_parent_ptr_val)) {
            jerry_value_free(arg_parent_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_parent_ptr = (uintptr_t)jerry_value_as_number(arg_parent_ptr_val);
        jerry_value_free(arg_parent_ptr_val);
        arg_parent = (void*)arg_parent_ptr;
    }
    
    // 调用底层函数
    lv_obj_t* ret_value = lv_button_create(arg_parent);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = jerry_object();
    jerry_value_t ptr = jerry_number((double)(uintptr_t)ret_value);
    jerry_value_t cls = jerry_string_sz("lv_obj");
    jerry_object_set(js_result, jerry_string_sz("__ptr"), ptr);
    jerry_object_set(js_result, jerry_string_sz("__class"), cls);
    jerry_value_free(ptr);
    jerry_value_free(cls);

    return js_result;
}



/**
 * @brief Set the number of columns
 */
static jerry_value_t js_lv_table_set_col_cnt(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: col_cnt (uint32_t)
    jerry_value_t js_arg_col_cnt = args[1];
    if (!jerry_value_is_number(js_arg_col_cnt)) {
        return throw_error("Argument 1 must be a number");
    }
    
    uint32_t arg_col_cnt = (uint32_t)jerry_value_as_number(js_arg_col_cnt);
    
    // 调用底层函数
    lv_table_set_column_count(arg_obj, arg_col_cnt);

    return jerry_undefined();
}



/**
 * @brief Set the number of rows
 */
static jerry_value_t js_lv_table_set_row_cnt(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return throw_error("Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 对象类型参数，支持null
    void* arg_obj = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        jerry_value_t js_arg_obj = args[0];
        if (!jerry_value_is_object(js_arg_obj)) {
            return throw_error("Argument 0 must be an object or null");
        }
        
        jerry_value_t arg_obj_ptr_prop = jerry_string_sz("__ptr");
        jerry_value_t arg_obj_ptr_val = jerry_object_get(js_arg_obj, arg_obj_ptr_prop);
        jerry_value_free(arg_obj_ptr_prop);
        
        if (!jerry_value_is_number(arg_obj_ptr_val)) {
            jerry_value_free(arg_obj_ptr_val);
            return throw_error("Invalid __ptr property");
        }
        
        uintptr_t arg_obj_ptr = (uintptr_t)jerry_value_as_number(arg_obj_ptr_val);
        jerry_value_free(arg_obj_ptr_val);
        arg_obj = (void*)arg_obj_ptr;
    }
    
    // 解析参数: row_cnt (uint32_t)
    jerry_value_t js_arg_row_cnt = args[1];
    if (!jerry_value_is_number(js_arg_row_cnt)) {
        return throw_error("Argument 1 must be a number");
    }
    
    uint32_t arg_row_cnt = (uint32_t)jerry_value_as_number(js_arg_row_cnt);
    
    // 调用底层函数
    lv_table_set_row_count(arg_obj, arg_row_cnt);

    return jerry_undefined();
}



const LVBindingJerryscriptFuncEntry_t lvgl_binding_funcs[] = {
    { "lv_delay_ms", js_lv_delay_ms },
    { "lv_timer_handler", js_lv_timer_handler },
    { "lv_color_hex", js_lv_color_hex },
    { "lv_style_set_text_color", js_lv_style_set_text_color },
    { "lv_event_get_target", js_lv_event_get_target },
    { "lv_event_get_code", js_lv_event_get_code },
    { "lv_event_get_user_data", js_lv_event_get_user_data },
    { "lv_obj_clean", js_lv_obj_clean },
    { "lv_obj_set_pos", js_lv_obj_set_pos },
    { "lv_obj_set_size", js_lv_obj_set_size },
    { "lv_obj_set_width", js_lv_obj_set_width },
    { "lv_obj_align", js_lv_obj_align },
    { "lv_obj_center", js_lv_obj_center },
    { "lv_obj_add_style", js_lv_obj_add_style },
    { "lv_obj_set_style_pad_row", js_lv_obj_set_style_pad_row },
    { "lv_obj_set_style_pad_column", js_lv_obj_set_style_pad_column },
    { "lv_obj_set_style_bg_color", js_lv_obj_set_style_bg_color },
    { "lv_obj_set_style_border_color", js_lv_obj_set_style_border_color },
    { "lv_obj_set_style_border_width", js_lv_obj_set_style_border_width },
    { "lv_obj_set_style_text_color", js_lv_obj_set_style_text_color },
    { "lv_obj_set_style_text_font", js_lv_obj_set_style_text_font },
    { "lv_obj_set_style_radius", js_lv_obj_set_style_radius },
    { "lv_obj_set_style_pad_all", js_lv_obj_set_style_pad_all },
    { "lv_obj_create", js_lv_obj_create },
    { "lv_obj_add_flag", js_lv_obj_add_flag },
    { "lv_obj_add_state", js_lv_obj_add_state },
    { "lv_obj_remove_state", js_lv_obj_remove_state },
    { "lv_obj_set_state", js_lv_obj_set_state },
    { "lv_obj_has_state", js_lv_obj_has_state },
    { "lv_arc_create", js_lv_arc_create },
    { "lv_arc_set_bg_angles", js_lv_arc_set_bg_angles },
    { "lv_arc_set_value", js_lv_arc_set_value },
    { "lv_arc_set_range", js_lv_arc_set_range },
    { "lv_label_create", js_lv_label_create },
    { "lv_label_set_text", js_lv_label_set_text },
    { "lv_label_set_long_mode", js_lv_label_set_long_mode },
    { "lv_label_get_text", js_lv_label_get_text },
    { "lv_bar_create", js_lv_bar_create },
    { "lv_bar_set_value", js_lv_bar_set_value },
    { "lv_bar_set_range", js_lv_bar_set_range },
    { "lv_chart_create", js_lv_chart_create },
    { "lv_chart_set_type", js_lv_chart_set_type },
    { "lv_chart_set_point_count", js_lv_chart_set_point_count },
    { "lv_chart_set_range", js_lv_chart_set_range },
    { "lv_checkbox_create", js_lv_checkbox_create },
    { "lv_checkbox_set_text", js_lv_checkbox_set_text },
    { "lv_dropdown_create", js_lv_dropdown_create },
    { "lv_dropdown_set_options", js_lv_dropdown_set_options },
    { "lv_dropdown_set_selected", js_lv_dropdown_set_selected },
    { "lv_dropdown_get_selected", js_lv_dropdown_get_selected },
    { "lv_msgbox_create", js_lv_msgbox_create },
    { "lv_msgbox_add_title", js_lv_msgbox_add_title },
    { "lv_msgbox_add_header_button", js_lv_msgbox_add_header_button },
    { "lv_msgbox_add_text", js_lv_msgbox_add_text },
    { "lv_msgbox_add_footer_button", js_lv_msgbox_add_footer_button },
    { "lv_msgbox_add_close_button", js_lv_msgbox_add_close_button },
    { "lv_msgbox_close", js_lv_msgbox_close },
    { "lv_roller_create", js_lv_roller_create },
    { "lv_roller_set_options", js_lv_roller_set_options },
    { "lv_roller_set_selected", js_lv_roller_set_selected },
    { "lv_slider_create", js_lv_slider_create },
    { "lv_slider_set_value", js_lv_slider_set_value },
    { "lv_slider_set_range", js_lv_slider_set_range },
    { "lv_slider_get_value", js_lv_slider_get_value },
    { "lv_textarea_create", js_lv_textarea_create },
    { "lv_textarea_add_text", js_lv_textarea_add_text },
    { "lv_textarea_set_text", js_lv_textarea_set_text },
    { "lv_textarea_set_placeholder_text", js_lv_textarea_set_placeholder_text },
    { "lv_switch_create", js_lv_switch_create },
    { "lv_table_create", js_lv_table_create },
    { "lv_table_set_cell_value", js_lv_table_set_cell_value },
    { "lv_obj_del", js_lv_obj_del },
    { "lv_obj_clear_flag", js_lv_obj_clear_flag },
    { "lv_scr_act", js_lv_scr_act },
    { "lv_disp_get_scr_act", js_lv_disp_get_scr_act },
    { "lv_scr_load", js_lv_scr_load },
    { "lv_img_create", js_lv_img_create },
    { "lv_img_set_angle", js_lv_img_set_angle },
    { "lv_img_set_zoom", js_lv_img_set_zoom },
    { "lv_btn_create", js_lv_btn_create },
    { "lv_table_set_col_cnt", js_lv_table_set_col_cnt },
    { "lv_table_set_row_cnt", js_lv_table_set_row_cnt }
};

const unsigned int lvgl_binding_funcs_count = 84;

static void register_lvgl_enums(void) {
    jerry_value_t global = jerry_current_realm();
    lvgl_binding_set_enum(global, "LV_RESULT_INVALID", 0);
    lvgl_binding_set_enum(global, "LV_RESULT_OK", 1);
    lvgl_binding_set_enum(global, "LV_ANIM_OFF", 0);
    lvgl_binding_set_enum(global, "LV_ANIM_ON", 1);
    lvgl_binding_set_enum(global, "LV_RB_COLOR_RED", 0);
    lvgl_binding_set_enum(global, "LV_RB_COLOR_BLACK", 1);
    lvgl_binding_set_enum(global, "LV_ALIGN_DEFAULT", 0);
    lvgl_binding_set_enum(global, "LV_ALIGN_TOP_LEFT", 1);
    lvgl_binding_set_enum(global, "LV_ALIGN_TOP_MID", 2);
    lvgl_binding_set_enum(global, "LV_ALIGN_TOP_RIGHT", 3);
    lvgl_binding_set_enum(global, "LV_ALIGN_BOTTOM_LEFT", 4);
    lvgl_binding_set_enum(global, "LV_ALIGN_BOTTOM_MID", 5);
    lvgl_binding_set_enum(global, "LV_ALIGN_BOTTOM_RIGHT", 6);
    lvgl_binding_set_enum(global, "LV_ALIGN_LEFT_MID", 7);
    lvgl_binding_set_enum(global, "LV_ALIGN_RIGHT_MID", 8);
    lvgl_binding_set_enum(global, "LV_ALIGN_CENTER", 9);
    lvgl_binding_set_enum(global, "LV_ALIGN_OUT_TOP_LEFT", 10);
    lvgl_binding_set_enum(global, "LV_ALIGN_OUT_TOP_MID", 11);
    lvgl_binding_set_enum(global, "LV_ALIGN_OUT_TOP_RIGHT", 12);
    lvgl_binding_set_enum(global, "LV_ALIGN_OUT_BOTTOM_LEFT", 13);
    lvgl_binding_set_enum(global, "LV_ALIGN_OUT_BOTTOM_MID", 14);
    lvgl_binding_set_enum(global, "LV_ALIGN_OUT_BOTTOM_RIGHT", 15);
    lvgl_binding_set_enum(global, "LV_ALIGN_OUT_LEFT_TOP", 16);
    lvgl_binding_set_enum(global, "LV_ALIGN_OUT_LEFT_MID", 17);
    lvgl_binding_set_enum(global, "LV_ALIGN_OUT_LEFT_BOTTOM", 18);
    lvgl_binding_set_enum(global, "LV_ALIGN_OUT_RIGHT_TOP", 19);
    lvgl_binding_set_enum(global, "LV_ALIGN_OUT_RIGHT_MID", 20);
    lvgl_binding_set_enum(global, "LV_ALIGN_OUT_RIGHT_BOTTOM", 21);
    lvgl_binding_set_enum(global, "LV_DIR_NONE", 0);
    lvgl_binding_set_enum(global, "LV_DIR_LEFT", 1);
    lvgl_binding_set_enum(global, "LV_DIR_RIGHT", 2);
    lvgl_binding_set_enum(global, "LV_DIR_TOP", 4);
    lvgl_binding_set_enum(global, "LV_DIR_BOTTOM", 8);
    lvgl_binding_set_enum(global, "LV_DIR_HOR", 3);
    lvgl_binding_set_enum(global, "LV_DIR_VER", 12);
    lvgl_binding_set_enum(global, "LV_DIR_ALL", 15);
    lvgl_binding_set_enum(global, "LV_OPA_TRANSP", 0);
    lvgl_binding_set_enum(global, "LV_OPA_0", 0);
    lvgl_binding_set_enum(global, "LV_OPA_10", 25);
    lvgl_binding_set_enum(global, "LV_OPA_20", 51);
    lvgl_binding_set_enum(global, "LV_OPA_30", 76);
    lvgl_binding_set_enum(global, "LV_OPA_40", 102);
    lvgl_binding_set_enum(global, "LV_OPA_50", 127);
    lvgl_binding_set_enum(global, "LV_OPA_60", 153);
    lvgl_binding_set_enum(global, "LV_OPA_70", 178);
    lvgl_binding_set_enum(global, "LV_OPA_80", 204);
    lvgl_binding_set_enum(global, "LV_OPA_90", 229);
    lvgl_binding_set_enum(global, "LV_OPA_100", 255);
    lvgl_binding_set_enum(global, "LV_OPA_COVER", 255);
    lvgl_binding_set_enum(global, "LV_COLOR_FORMAT_UNKNOWN", 0);
    lvgl_binding_set_enum(global, "LV_COLOR_FORMAT_RAW", 1);
    lvgl_binding_set_enum(global, "LV_COLOR_FORMAT_RAW_ALPHA", 2);
    lvgl_binding_set_enum(global, "LV_COLOR_FORMAT_L8", 6);
    lvgl_binding_set_enum(global, "LV_COLOR_FORMAT_I1", 7);
    lvgl_binding_set_enum(global, "LV_COLOR_FORMAT_I2", 8);
    lvgl_binding_set_enum(global, "LV_COLOR_FORMAT_I4", 9);
    lvgl_binding_set_enum(global, "LV_COLOR_FORMAT_I8", 10);
    lvgl_binding_set_enum(global, "LV_COLOR_FORMAT_A8", 14);
    lvgl_binding_set_enum(global, "LV_COLOR_FORMAT_RGB565", 18);
    lvgl_binding_set_enum(global, "LV_COLOR_FORMAT_ARGB8565", 19);
    lvgl_binding_set_enum(global, "LV_COLOR_FORMAT_RGB565A8", 20);
    lvgl_binding_set_enum(global, "LV_COLOR_FORMAT_AL88", 21);
    lvgl_binding_set_enum(global, "LV_COLOR_FORMAT_RGB888", 15);
    lvgl_binding_set_enum(global, "LV_COLOR_FORMAT_ARGB8888", 16);
    lvgl_binding_set_enum(global, "LV_COLOR_FORMAT_XRGB8888", 17);
    lvgl_binding_set_enum(global, "LV_COLOR_FORMAT_A1", 11);
    lvgl_binding_set_enum(global, "LV_COLOR_FORMAT_A2", 12);
    lvgl_binding_set_enum(global, "LV_COLOR_FORMAT_A4", 13);
    lvgl_binding_set_enum(global, "LV_COLOR_FORMAT_YUV_START", 32);
    lvgl_binding_set_enum(global, "LV_COLOR_FORMAT_I420", 32);
    lvgl_binding_set_enum(global, "LV_COLOR_FORMAT_I422", 33);
    lvgl_binding_set_enum(global, "LV_COLOR_FORMAT_I444", 34);
    lvgl_binding_set_enum(global, "LV_COLOR_FORMAT_I400", 35);
    lvgl_binding_set_enum(global, "LV_COLOR_FORMAT_NV21", 36);
    lvgl_binding_set_enum(global, "LV_COLOR_FORMAT_NV12", 37);
    lvgl_binding_set_enum(global, "LV_COLOR_FORMAT_YUY2", 38);
    lvgl_binding_set_enum(global, "LV_COLOR_FORMAT_UYVY", 39);
    lvgl_binding_set_enum(global, "LV_COLOR_FORMAT_YUV_END", 39);
    lvgl_binding_set_enum(global, "LV_COLOR_FORMAT_NATIVE", 18);
    lvgl_binding_set_enum(global, "LV_COLOR_FORMAT_NATIVE_WITH_ALPHA", 20);
    lvgl_binding_set_enum(global, "LV_PALETTE_RED", 0);
    lvgl_binding_set_enum(global, "LV_PALETTE_PINK", 1);
    lvgl_binding_set_enum(global, "LV_PALETTE_PURPLE", 2);
    lvgl_binding_set_enum(global, "LV_PALETTE_DEEP_PURPLE", 3);
    lvgl_binding_set_enum(global, "LV_PALETTE_INDIGO", 4);
    lvgl_binding_set_enum(global, "LV_PALETTE_BLUE", 5);
    lvgl_binding_set_enum(global, "LV_PALETTE_LIGHT_BLUE", 6);
    lvgl_binding_set_enum(global, "LV_PALETTE_CYAN", 7);
    lvgl_binding_set_enum(global, "LV_PALETTE_TEAL", 8);
    lvgl_binding_set_enum(global, "LV_PALETTE_GREEN", 9);
    lvgl_binding_set_enum(global, "LV_PALETTE_LIGHT_GREEN", 10);
    lvgl_binding_set_enum(global, "LV_PALETTE_LIME", 11);
    lvgl_binding_set_enum(global, "LV_PALETTE_YELLOW", 12);
    lvgl_binding_set_enum(global, "LV_PALETTE_AMBER", 13);
    lvgl_binding_set_enum(global, "LV_PALETTE_ORANGE", 14);
    lvgl_binding_set_enum(global, "LV_PALETTE_DEEP_ORANGE", 15);
    lvgl_binding_set_enum(global, "LV_PALETTE_BROWN", 16);
    lvgl_binding_set_enum(global, "LV_PALETTE_BLUE_GREY", 17);
    lvgl_binding_set_enum(global, "LV_PALETTE_GREY", 18);
    lvgl_binding_set_enum(global, "LV_PALETTE_LAST", 19);
    lvgl_binding_set_enum(global, "LV_PALETTE_NONE", 255);
    lvgl_binding_set_enum(global, "LV_IMAGE_FLAGS_PREMULTIPLIED", 1);
    lvgl_binding_set_enum(global, "LV_IMAGE_FLAGS_COMPRESSED", 8);
    lvgl_binding_set_enum(global, "LV_IMAGE_FLAGS_ALLOCATED", 16);
    lvgl_binding_set_enum(global, "LV_IMAGE_FLAGS_MODIFIABLE", 32);
    lvgl_binding_set_enum(global, "LV_IMAGE_FLAGS_USER1", 256);
    lvgl_binding_set_enum(global, "LV_IMAGE_FLAGS_USER2", 512);
    lvgl_binding_set_enum(global, "LV_IMAGE_FLAGS_USER3", 1024);
    lvgl_binding_set_enum(global, "LV_IMAGE_FLAGS_USER4", 2048);
    lvgl_binding_set_enum(global, "LV_IMAGE_FLAGS_USER5", 4096);
    lvgl_binding_set_enum(global, "LV_IMAGE_FLAGS_USER6", 8192);
    lvgl_binding_set_enum(global, "LV_IMAGE_FLAGS_USER7", 16384);
    lvgl_binding_set_enum(global, "LV_IMAGE_FLAGS_USER8", 32768);
    lvgl_binding_set_enum(global, "LV_IMAGE_COMPRESS_NONE", 0);
    lvgl_binding_set_enum(global, "LV_IMAGE_COMPRESS_RLE", 1);
    lvgl_binding_set_enum(global, "LV_IMAGE_COMPRESS_LZ4", 2);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_BULLET", 0);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_AUDIO", 1);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_VIDEO", 2);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_LIST", 3);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_OK", 4);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_CLOSE", 5);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_POWER", 6);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_SETTINGS", 7);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_HOME", 8);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_DOWNLOAD", 9);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_DRIVE", 10);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_REFRESH", 11);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_MUTE", 12);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_VOLUME_MID", 13);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_VOLUME_MAX", 14);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_IMAGE", 15);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_TINT", 16);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_PREV", 17);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_PLAY", 18);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_PAUSE", 19);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_STOP", 20);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_NEXT", 21);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_EJECT", 22);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_LEFT", 23);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_RIGHT", 24);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_PLUS", 25);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_MINUS", 26);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_EYE_OPEN", 27);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_EYE_CLOSE", 28);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_WARNING", 29);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_SHUFFLE", 30);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_UP", 31);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_DOWN", 32);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_LOOP", 33);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_DIRECTORY", 34);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_UPLOAD", 35);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_CALL", 36);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_CUT", 37);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_COPY", 38);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_SAVE", 39);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_BARS", 40);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_ENVELOPE", 41);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_CHARGE", 42);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_PASTE", 43);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_BELL", 44);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_KEYBOARD", 45);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_GPS", 46);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_FILE", 47);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_WIFI", 48);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_BATTERY_FULL", 49);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_BATTERY_3", 50);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_BATTERY_2", 51);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_BATTERY_1", 52);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_BATTERY_EMPTY", 53);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_USB", 54);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_BLUETOOTH", 55);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_TRASH", 56);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_EDIT", 57);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_BACKSPACE", 58);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_SD_CARD", 59);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_NEW_LINE", 60);
    lvgl_binding_set_enum(global, "LV_STR_SYMBOL_DUMMY", 61);
    lvgl_binding_set_enum(global, "LV_THREAD_PRIO_LOWEST", 0);
    lvgl_binding_set_enum(global, "LV_THREAD_PRIO_LOW", 1);
    lvgl_binding_set_enum(global, "LV_THREAD_PRIO_MID", 2);
    lvgl_binding_set_enum(global, "LV_THREAD_PRIO_HIGH", 3);
    lvgl_binding_set_enum(global, "LV_THREAD_PRIO_HIGHEST", 4);
    lvgl_binding_set_enum(global, "LV_CACHE_RESERVE_COND_OK", 0);
    lvgl_binding_set_enum(global, "LV_CACHE_RESERVE_COND_TOO_LARGE", 1);
    lvgl_binding_set_enum(global, "LV_CACHE_RESERVE_COND_NEED_VICTIM", 2);
    lvgl_binding_set_enum(global, "LV_CACHE_RESERVE_COND_ERROR", 3);
    lvgl_binding_set_enum(global, "LV_FONT_GLYPH_FORMAT_NONE", 0);
    lvgl_binding_set_enum(global, "LV_FONT_GLYPH_FORMAT_A1", 1);
    lvgl_binding_set_enum(global, "LV_FONT_GLYPH_FORMAT_A2", 2);
    lvgl_binding_set_enum(global, "LV_FONT_GLYPH_FORMAT_A4", 4);
    lvgl_binding_set_enum(global, "LV_FONT_GLYPH_FORMAT_A8", 8);
    lvgl_binding_set_enum(global, "LV_FONT_GLYPH_FORMAT_IMAGE", 9);
    lvgl_binding_set_enum(global, "LV_FONT_GLYPH_FORMAT_VECTOR", 10);
    lvgl_binding_set_enum(global, "LV_FONT_GLYPH_FORMAT_SVG", 11);
    lvgl_binding_set_enum(global, "LV_FONT_GLYPH_FORMAT_CUSTOM", 255);
    lvgl_binding_set_enum(global, "LV_FONT_SUBPX_NONE", 0);
    lvgl_binding_set_enum(global, "LV_FONT_SUBPX_HOR", 1);
    lvgl_binding_set_enum(global, "LV_FONT_SUBPX_VER", 2);
    lvgl_binding_set_enum(global, "LV_FONT_SUBPX_BOTH", 3);
    lvgl_binding_set_enum(global, "LV_FONT_KERNING_NORMAL", 0);
    lvgl_binding_set_enum(global, "LV_FONT_KERNING_NONE", 1);
    lvgl_binding_set_enum(global, "LV_TEXT_FLAG_NONE", 0);
    lvgl_binding_set_enum(global, "LV_TEXT_FLAG_EXPAND", 1);
    lvgl_binding_set_enum(global, "LV_TEXT_FLAG_FIT", 2);
    lvgl_binding_set_enum(global, "LV_TEXT_FLAG_BREAK_ALL", 4);
    lvgl_binding_set_enum(global, "LV_TEXT_ALIGN_AUTO", 0);
    lvgl_binding_set_enum(global, "LV_TEXT_ALIGN_LEFT", 1);
    lvgl_binding_set_enum(global, "LV_TEXT_ALIGN_CENTER", 2);
    lvgl_binding_set_enum(global, "LV_TEXT_ALIGN_RIGHT", 3);
    lvgl_binding_set_enum(global, "LV_BASE_DIR_LTR", 0);
    lvgl_binding_set_enum(global, "LV_BASE_DIR_RTL", 1);
    lvgl_binding_set_enum(global, "LV_BASE_DIR_AUTO", 2);
    lvgl_binding_set_enum(global, "LV_BASE_DIR_NEUTRAL", 32);
    lvgl_binding_set_enum(global, "LV_BASE_DIR_WEAK", 33);
    lvgl_binding_set_enum(global, "LV_LAYOUT_NONE", 0);
    lvgl_binding_set_enum(global, "LV_LAYOUT_FLEX", 1);
    lvgl_binding_set_enum(global, "LV_LAYOUT_GRID", 2);
    lvgl_binding_set_enum(global, "LV_LAYOUT_LAST", 3);
    lvgl_binding_set_enum(global, "LV_FLEX_ALIGN_START", 0);
    lvgl_binding_set_enum(global, "LV_FLEX_ALIGN_END", 1);
    lvgl_binding_set_enum(global, "LV_FLEX_ALIGN_CENTER", 2);
    lvgl_binding_set_enum(global, "LV_FLEX_ALIGN_SPACE_EVENLY", 3);
    lvgl_binding_set_enum(global, "LV_FLEX_ALIGN_SPACE_AROUND", 4);
    lvgl_binding_set_enum(global, "LV_FLEX_ALIGN_SPACE_BETWEEN", 5);
    lvgl_binding_set_enum(global, "LV_FLEX_FLOW_ROW", 0);
    lvgl_binding_set_enum(global, "LV_FLEX_FLOW_COLUMN", 1);
    lvgl_binding_set_enum(global, "LV_FLEX_FLOW_ROW_WRAP", 4);
    lvgl_binding_set_enum(global, "LV_FLEX_FLOW_ROW_REVERSE", 8);
    lvgl_binding_set_enum(global, "LV_FLEX_FLOW_ROW_WRAP_REVERSE", 12);
    lvgl_binding_set_enum(global, "LV_FLEX_FLOW_COLUMN_WRAP", 5);
    lvgl_binding_set_enum(global, "LV_FLEX_FLOW_COLUMN_REVERSE", 9);
    lvgl_binding_set_enum(global, "LV_FLEX_FLOW_COLUMN_WRAP_REVERSE", 13);
    lvgl_binding_set_enum(global, "LV_GRID_ALIGN_START", 0);
    lvgl_binding_set_enum(global, "LV_GRID_ALIGN_CENTER", 1);
    lvgl_binding_set_enum(global, "LV_GRID_ALIGN_END", 2);
    lvgl_binding_set_enum(global, "LV_GRID_ALIGN_STRETCH", 3);
    lvgl_binding_set_enum(global, "LV_GRID_ALIGN_SPACE_EVENLY", 4);
    lvgl_binding_set_enum(global, "LV_GRID_ALIGN_SPACE_AROUND", 5);
    lvgl_binding_set_enum(global, "LV_GRID_ALIGN_SPACE_BETWEEN", 6);
    lvgl_binding_set_enum(global, "LV_BLEND_MODE_NORMAL", 0);
    lvgl_binding_set_enum(global, "LV_BLEND_MODE_ADDITIVE", 1);
    lvgl_binding_set_enum(global, "LV_BLEND_MODE_SUBTRACTIVE", 2);
    lvgl_binding_set_enum(global, "LV_BLEND_MODE_MULTIPLY", 3);
    lvgl_binding_set_enum(global, "LV_TEXT_DECOR_NONE", 0);
    lvgl_binding_set_enum(global, "LV_TEXT_DECOR_UNDERLINE", 1);
    lvgl_binding_set_enum(global, "LV_TEXT_DECOR_STRIKETHROUGH", 2);
    lvgl_binding_set_enum(global, "LV_BORDER_SIDE_NONE", 0);
    lvgl_binding_set_enum(global, "LV_BORDER_SIDE_BOTTOM", 1);
    lvgl_binding_set_enum(global, "LV_BORDER_SIDE_TOP", 2);
    lvgl_binding_set_enum(global, "LV_BORDER_SIDE_LEFT", 4);
    lvgl_binding_set_enum(global, "LV_BORDER_SIDE_RIGHT", 8);
    lvgl_binding_set_enum(global, "LV_BORDER_SIDE_FULL", 15);
    lvgl_binding_set_enum(global, "LV_BORDER_SIDE_INTERNAL", 16);
    lvgl_binding_set_enum(global, "LV_GRAD_DIR_NONE", 0);
    lvgl_binding_set_enum(global, "LV_GRAD_DIR_VER", 1);
    lvgl_binding_set_enum(global, "LV_GRAD_DIR_HOR", 2);
    lvgl_binding_set_enum(global, "LV_GRAD_DIR_LINEAR", 3);
    lvgl_binding_set_enum(global, "LV_GRAD_DIR_RADIAL", 4);
    lvgl_binding_set_enum(global, "LV_GRAD_DIR_CONICAL", 5);
    lvgl_binding_set_enum(global, "LV_GRAD_EXTEND_PAD", 0);
    lvgl_binding_set_enum(global, "LV_GRAD_EXTEND_REPEAT", 1);
    lvgl_binding_set_enum(global, "LV_GRAD_EXTEND_REFLECT", 2);
    lvgl_binding_set_enum(global, "LV_STYLE_PROP_INV", 0);
    lvgl_binding_set_enum(global, "LV_STYLE_WIDTH", 1);
    lvgl_binding_set_enum(global, "LV_STYLE_HEIGHT", 2);
    lvgl_binding_set_enum(global, "LV_STYLE_LENGTH", 3);
    lvgl_binding_set_enum(global, "LV_STYLE_MIN_WIDTH", 4);
    lvgl_binding_set_enum(global, "LV_STYLE_MAX_WIDTH", 5);
    lvgl_binding_set_enum(global, "LV_STYLE_MIN_HEIGHT", 6);
    lvgl_binding_set_enum(global, "LV_STYLE_MAX_HEIGHT", 7);
    lvgl_binding_set_enum(global, "LV_STYLE_X", 8);
    lvgl_binding_set_enum(global, "LV_STYLE_Y", 9);
    lvgl_binding_set_enum(global, "LV_STYLE_ALIGN", 10);
    lvgl_binding_set_enum(global, "LV_STYLE_RADIUS", 12);
    lvgl_binding_set_enum(global, "LV_STYLE_PAD_TOP", 16);
    lvgl_binding_set_enum(global, "LV_STYLE_PAD_BOTTOM", 17);
    lvgl_binding_set_enum(global, "LV_STYLE_PAD_LEFT", 18);
    lvgl_binding_set_enum(global, "LV_STYLE_PAD_RIGHT", 19);
    lvgl_binding_set_enum(global, "LV_STYLE_PAD_ROW", 20);
    lvgl_binding_set_enum(global, "LV_STYLE_PAD_COLUMN", 21);
    lvgl_binding_set_enum(global, "LV_STYLE_LAYOUT", 22);
    lvgl_binding_set_enum(global, "LV_STYLE_MARGIN_TOP", 24);
    lvgl_binding_set_enum(global, "LV_STYLE_MARGIN_BOTTOM", 25);
    lvgl_binding_set_enum(global, "LV_STYLE_MARGIN_LEFT", 26);
    lvgl_binding_set_enum(global, "LV_STYLE_MARGIN_RIGHT", 27);
    lvgl_binding_set_enum(global, "LV_STYLE_BG_COLOR", 28);
    lvgl_binding_set_enum(global, "LV_STYLE_BG_OPA", 29);
    lvgl_binding_set_enum(global, "LV_STYLE_BG_GRAD_DIR", 32);
    lvgl_binding_set_enum(global, "LV_STYLE_BG_MAIN_STOP", 33);
    lvgl_binding_set_enum(global, "LV_STYLE_BG_GRAD_STOP", 34);
    lvgl_binding_set_enum(global, "LV_STYLE_BG_GRAD_COLOR", 35);
    lvgl_binding_set_enum(global, "LV_STYLE_BG_MAIN_OPA", 36);
    lvgl_binding_set_enum(global, "LV_STYLE_BG_GRAD_OPA", 37);
    lvgl_binding_set_enum(global, "LV_STYLE_BG_GRAD", 38);
    lvgl_binding_set_enum(global, "LV_STYLE_BASE_DIR", 39);
    lvgl_binding_set_enum(global, "LV_STYLE_BG_IMAGE_SRC", 40);
    lvgl_binding_set_enum(global, "LV_STYLE_BG_IMAGE_OPA", 41);
    lvgl_binding_set_enum(global, "LV_STYLE_BG_IMAGE_RECOLOR", 42);
    lvgl_binding_set_enum(global, "LV_STYLE_BG_IMAGE_RECOLOR_OPA", 43);
    lvgl_binding_set_enum(global, "LV_STYLE_BG_IMAGE_TILED", 44);
    lvgl_binding_set_enum(global, "LV_STYLE_CLIP_CORNER", 45);
    lvgl_binding_set_enum(global, "LV_STYLE_BORDER_WIDTH", 48);
    lvgl_binding_set_enum(global, "LV_STYLE_BORDER_COLOR", 49);
    lvgl_binding_set_enum(global, "LV_STYLE_BORDER_OPA", 50);
    lvgl_binding_set_enum(global, "LV_STYLE_BORDER_SIDE", 52);
    lvgl_binding_set_enum(global, "LV_STYLE_BORDER_POST", 53);
    lvgl_binding_set_enum(global, "LV_STYLE_OUTLINE_WIDTH", 56);
    lvgl_binding_set_enum(global, "LV_STYLE_OUTLINE_COLOR", 57);
    lvgl_binding_set_enum(global, "LV_STYLE_OUTLINE_OPA", 58);
    lvgl_binding_set_enum(global, "LV_STYLE_OUTLINE_PAD", 59);
    lvgl_binding_set_enum(global, "LV_STYLE_SHADOW_WIDTH", 60);
    lvgl_binding_set_enum(global, "LV_STYLE_SHADOW_COLOR", 61);
    lvgl_binding_set_enum(global, "LV_STYLE_SHADOW_OPA", 62);
    lvgl_binding_set_enum(global, "LV_STYLE_SHADOW_OFFSET_X", 64);
    lvgl_binding_set_enum(global, "LV_STYLE_SHADOW_OFFSET_Y", 65);
    lvgl_binding_set_enum(global, "LV_STYLE_SHADOW_SPREAD", 66);
    lvgl_binding_set_enum(global, "LV_STYLE_IMAGE_OPA", 68);
    lvgl_binding_set_enum(global, "LV_STYLE_IMAGE_RECOLOR", 69);
    lvgl_binding_set_enum(global, "LV_STYLE_IMAGE_RECOLOR_OPA", 70);
    lvgl_binding_set_enum(global, "LV_STYLE_LINE_WIDTH", 72);
    lvgl_binding_set_enum(global, "LV_STYLE_LINE_DASH_WIDTH", 73);
    lvgl_binding_set_enum(global, "LV_STYLE_LINE_DASH_GAP", 74);
    lvgl_binding_set_enum(global, "LV_STYLE_LINE_ROUNDED", 75);
    lvgl_binding_set_enum(global, "LV_STYLE_LINE_COLOR", 76);
    lvgl_binding_set_enum(global, "LV_STYLE_LINE_OPA", 77);
    lvgl_binding_set_enum(global, "LV_STYLE_ARC_WIDTH", 80);
    lvgl_binding_set_enum(global, "LV_STYLE_ARC_ROUNDED", 81);
    lvgl_binding_set_enum(global, "LV_STYLE_ARC_COLOR", 82);
    lvgl_binding_set_enum(global, "LV_STYLE_ARC_OPA", 83);
    lvgl_binding_set_enum(global, "LV_STYLE_ARC_IMAGE_SRC", 84);
    lvgl_binding_set_enum(global, "LV_STYLE_TEXT_COLOR", 88);
    lvgl_binding_set_enum(global, "LV_STYLE_TEXT_OPA", 89);
    lvgl_binding_set_enum(global, "LV_STYLE_TEXT_FONT", 90);
    lvgl_binding_set_enum(global, "LV_STYLE_TEXT_LETTER_SPACE", 91);
    lvgl_binding_set_enum(global, "LV_STYLE_TEXT_LINE_SPACE", 92);
    lvgl_binding_set_enum(global, "LV_STYLE_TEXT_DECOR", 93);
    lvgl_binding_set_enum(global, "LV_STYLE_TEXT_ALIGN", 94);
    lvgl_binding_set_enum(global, "LV_STYLE_OPA", 95);
    lvgl_binding_set_enum(global, "LV_STYLE_OPA_LAYERED", 96);
    lvgl_binding_set_enum(global, "LV_STYLE_COLOR_FILTER_DSC", 97);
    lvgl_binding_set_enum(global, "LV_STYLE_COLOR_FILTER_OPA", 98);
    lvgl_binding_set_enum(global, "LV_STYLE_ANIM", 99);
    lvgl_binding_set_enum(global, "LV_STYLE_ANIM_DURATION", 100);
    lvgl_binding_set_enum(global, "LV_STYLE_TRANSITION", 102);
    lvgl_binding_set_enum(global, "LV_STYLE_BLEND_MODE", 103);
    lvgl_binding_set_enum(global, "LV_STYLE_TRANSFORM_WIDTH", 104);
    lvgl_binding_set_enum(global, "LV_STYLE_TRANSFORM_HEIGHT", 105);
    lvgl_binding_set_enum(global, "LV_STYLE_TRANSLATE_X", 106);
    lvgl_binding_set_enum(global, "LV_STYLE_TRANSLATE_Y", 107);
    lvgl_binding_set_enum(global, "LV_STYLE_TRANSFORM_SCALE_X", 108);
    lvgl_binding_set_enum(global, "LV_STYLE_TRANSFORM_SCALE_Y", 109);
    lvgl_binding_set_enum(global, "LV_STYLE_TRANSFORM_ROTATION", 110);
    lvgl_binding_set_enum(global, "LV_STYLE_TRANSFORM_PIVOT_X", 111);
    lvgl_binding_set_enum(global, "LV_STYLE_TRANSFORM_PIVOT_Y", 112);
    lvgl_binding_set_enum(global, "LV_STYLE_TRANSFORM_SKEW_X", 113);
    lvgl_binding_set_enum(global, "LV_STYLE_TRANSFORM_SKEW_Y", 114);
    lvgl_binding_set_enum(global, "LV_STYLE_BITMAP_MASK_SRC", 115);
    lvgl_binding_set_enum(global, "LV_STYLE_ROTARY_SENSITIVITY", 116);
    lvgl_binding_set_enum(global, "LV_STYLE_FLEX_FLOW", 125);
    lvgl_binding_set_enum(global, "LV_STYLE_FLEX_MAIN_PLACE", 126);
    lvgl_binding_set_enum(global, "LV_STYLE_FLEX_CROSS_PLACE", 127);
    lvgl_binding_set_enum(global, "LV_STYLE_FLEX_TRACK_PLACE", 128);
    lvgl_binding_set_enum(global, "LV_STYLE_FLEX_GROW", 129);
    lvgl_binding_set_enum(global, "LV_STYLE_GRID_COLUMN_ALIGN", 130);
    lvgl_binding_set_enum(global, "LV_STYLE_GRID_ROW_ALIGN", 131);
    lvgl_binding_set_enum(global, "LV_STYLE_GRID_ROW_DSC_ARRAY", 132);
    lvgl_binding_set_enum(global, "LV_STYLE_GRID_COLUMN_DSC_ARRAY", 133);
    lvgl_binding_set_enum(global, "LV_STYLE_GRID_CELL_COLUMN_POS", 134);
    lvgl_binding_set_enum(global, "LV_STYLE_GRID_CELL_COLUMN_SPAN", 135);
    lvgl_binding_set_enum(global, "LV_STYLE_GRID_CELL_X_ALIGN", 136);
    lvgl_binding_set_enum(global, "LV_STYLE_GRID_CELL_ROW_POS", 137);
    lvgl_binding_set_enum(global, "LV_STYLE_GRID_CELL_ROW_SPAN", 138);
    lvgl_binding_set_enum(global, "LV_STYLE_GRID_CELL_Y_ALIGN", 139);
    lvgl_binding_set_enum(global, "LV_STYLE_LAST_BUILT_IN_PROP", 140);
    lvgl_binding_set_enum(global, "LV_STYLE_NUM_BUILT_IN_PROPS", 141);
    lvgl_binding_set_enum(global, "LV_STYLE_PROP_ANY", 255);
    lvgl_binding_set_enum(global, "LV_STYLE_PROP_CONST", 255);
    lvgl_binding_set_enum(global, "LV_STYLE_RES_NOT_FOUND", 0);
    lvgl_binding_set_enum(global, "LV_STYLE_RES_FOUND", 1);
    lvgl_binding_set_enum(global, "LV_EVENT_ALL", 0);
    lvgl_binding_set_enum(global, "LV_EVENT_PRESSED", 1);
    lvgl_binding_set_enum(global, "LV_EVENT_PRESSING", 2);
    lvgl_binding_set_enum(global, "LV_EVENT_PRESS_LOST", 3);
    lvgl_binding_set_enum(global, "LV_EVENT_SHORT_CLICKED", 4);
    lvgl_binding_set_enum(global, "LV_EVENT_LONG_PRESSED", 5);
    lvgl_binding_set_enum(global, "LV_EVENT_LONG_PRESSED_REPEAT", 6);
    lvgl_binding_set_enum(global, "LV_EVENT_CLICKED", 7);
    lvgl_binding_set_enum(global, "LV_EVENT_RELEASED", 8);
    lvgl_binding_set_enum(global, "LV_EVENT_SCROLL_BEGIN", 9);
    lvgl_binding_set_enum(global, "LV_EVENT_SCROLL_THROW_BEGIN", 10);
    lvgl_binding_set_enum(global, "LV_EVENT_SCROLL_END", 11);
    lvgl_binding_set_enum(global, "LV_EVENT_SCROLL", 12);
    lvgl_binding_set_enum(global, "LV_EVENT_GESTURE", 13);
    lvgl_binding_set_enum(global, "LV_EVENT_KEY", 14);
    lvgl_binding_set_enum(global, "LV_EVENT_ROTARY", 15);
    lvgl_binding_set_enum(global, "LV_EVENT_FOCUSED", 16);
    lvgl_binding_set_enum(global, "LV_EVENT_DEFOCUSED", 17);
    lvgl_binding_set_enum(global, "LV_EVENT_LEAVE", 18);
    lvgl_binding_set_enum(global, "LV_EVENT_HIT_TEST", 19);
    lvgl_binding_set_enum(global, "LV_EVENT_INDEV_RESET", 20);
    lvgl_binding_set_enum(global, "LV_EVENT_HOVER_OVER", 21);
    lvgl_binding_set_enum(global, "LV_EVENT_HOVER_LEAVE", 22);
    lvgl_binding_set_enum(global, "LV_EVENT_COVER_CHECK", 23);
    lvgl_binding_set_enum(global, "LV_EVENT_REFR_EXT_DRAW_SIZE", 24);
    lvgl_binding_set_enum(global, "LV_EVENT_DRAW_MAIN_BEGIN", 25);
    lvgl_binding_set_enum(global, "LV_EVENT_DRAW_MAIN", 26);
    lvgl_binding_set_enum(global, "LV_EVENT_DRAW_MAIN_END", 27);
    lvgl_binding_set_enum(global, "LV_EVENT_DRAW_POST_BEGIN", 28);
    lvgl_binding_set_enum(global, "LV_EVENT_DRAW_POST", 29);
    lvgl_binding_set_enum(global, "LV_EVENT_DRAW_POST_END", 30);
    lvgl_binding_set_enum(global, "LV_EVENT_DRAW_TASK_ADDED", 31);
    lvgl_binding_set_enum(global, "LV_EVENT_VALUE_CHANGED", 32);
    lvgl_binding_set_enum(global, "LV_EVENT_INSERT", 33);
    lvgl_binding_set_enum(global, "LV_EVENT_REFRESH", 34);
    lvgl_binding_set_enum(global, "LV_EVENT_READY", 35);
    lvgl_binding_set_enum(global, "LV_EVENT_CANCEL", 36);
    lvgl_binding_set_enum(global, "LV_EVENT_CREATE", 37);
    lvgl_binding_set_enum(global, "LV_EVENT_DELETE", 38);
    lvgl_binding_set_enum(global, "LV_EVENT_CHILD_CHANGED", 39);
    lvgl_binding_set_enum(global, "LV_EVENT_CHILD_CREATED", 40);
    lvgl_binding_set_enum(global, "LV_EVENT_CHILD_DELETED", 41);
    lvgl_binding_set_enum(global, "LV_EVENT_SCREEN_UNLOAD_START", 42);
    lvgl_binding_set_enum(global, "LV_EVENT_SCREEN_LOAD_START", 43);
    lvgl_binding_set_enum(global, "LV_EVENT_SCREEN_LOADED", 44);
    lvgl_binding_set_enum(global, "LV_EVENT_SCREEN_UNLOADED", 45);
    lvgl_binding_set_enum(global, "LV_EVENT_SIZE_CHANGED", 46);
    lvgl_binding_set_enum(global, "LV_EVENT_STYLE_CHANGED", 47);
    lvgl_binding_set_enum(global, "LV_EVENT_LAYOUT_CHANGED", 48);
    lvgl_binding_set_enum(global, "LV_EVENT_GET_SELF_SIZE", 49);
    lvgl_binding_set_enum(global, "LV_EVENT_INVALIDATE_AREA", 50);
    lvgl_binding_set_enum(global, "LV_EVENT_RESOLUTION_CHANGED", 51);
    lvgl_binding_set_enum(global, "LV_EVENT_COLOR_FORMAT_CHANGED", 52);
    lvgl_binding_set_enum(global, "LV_EVENT_REFR_REQUEST", 53);
    lvgl_binding_set_enum(global, "LV_EVENT_REFR_START", 54);
    lvgl_binding_set_enum(global, "LV_EVENT_REFR_READY", 55);
    lvgl_binding_set_enum(global, "LV_EVENT_RENDER_START", 56);
    lvgl_binding_set_enum(global, "LV_EVENT_RENDER_READY", 57);
    lvgl_binding_set_enum(global, "LV_EVENT_FLUSH_START", 58);
    lvgl_binding_set_enum(global, "LV_EVENT_FLUSH_FINISH", 59);
    lvgl_binding_set_enum(global, "LV_EVENT_FLUSH_WAIT_START", 60);
    lvgl_binding_set_enum(global, "LV_EVENT_FLUSH_WAIT_FINISH", 61);
    lvgl_binding_set_enum(global, "LV_EVENT_VSYNC", 62);
    lvgl_binding_set_enum(global, "LV_EVENT_LAST", 63);
    lvgl_binding_set_enum(global, "LV_EVENT_PREPROCESS", 32768);
    lvgl_binding_set_enum(global, "LV_FS_RES_OK", 0);
    lvgl_binding_set_enum(global, "LV_FS_RES_HW_ERR", 1);
    lvgl_binding_set_enum(global, "LV_FS_RES_FS_ERR", 2);
    lvgl_binding_set_enum(global, "LV_FS_RES_NOT_EX", 3);
    lvgl_binding_set_enum(global, "LV_FS_RES_FULL", 4);
    lvgl_binding_set_enum(global, "LV_FS_RES_LOCKED", 5);
    lvgl_binding_set_enum(global, "LV_FS_RES_DENIED", 6);
    lvgl_binding_set_enum(global, "LV_FS_RES_BUSY", 7);
    lvgl_binding_set_enum(global, "LV_FS_RES_TOUT", 8);
    lvgl_binding_set_enum(global, "LV_FS_RES_NOT_IMP", 9);
    lvgl_binding_set_enum(global, "LV_FS_RES_OUT_OF_MEM", 10);
    lvgl_binding_set_enum(global, "LV_FS_RES_INV_PARAM", 11);
    lvgl_binding_set_enum(global, "LV_FS_RES_UNKNOWN", 12);
    lvgl_binding_set_enum(global, "LV_FS_MODE_WR", 1);
    lvgl_binding_set_enum(global, "LV_FS_MODE_RD", 2);
    lvgl_binding_set_enum(global, "LV_FS_SEEK_SET", 0);
    lvgl_binding_set_enum(global, "LV_FS_SEEK_CUR", 1);
    lvgl_binding_set_enum(global, "LV_FS_SEEK_END", 2);
    lvgl_binding_set_enum(global, "LV_IMAGE_SRC_VARIABLE", 0);
    lvgl_binding_set_enum(global, "LV_IMAGE_SRC_FILE", 1);
    lvgl_binding_set_enum(global, "LV_IMAGE_SRC_SYMBOL", 2);
    lvgl_binding_set_enum(global, "LV_IMAGE_SRC_UNKNOWN", 3);
    lvgl_binding_set_enum(global, "LV_DRAW_TASK_TYPE_NONE", 0);
    lvgl_binding_set_enum(global, "LV_DRAW_TASK_TYPE_FILL", 1);
    lvgl_binding_set_enum(global, "LV_DRAW_TASK_TYPE_BORDER", 2);
    lvgl_binding_set_enum(global, "LV_DRAW_TASK_TYPE_BOX_SHADOW", 3);
    lvgl_binding_set_enum(global, "LV_DRAW_TASK_TYPE_LABEL", 4);
    lvgl_binding_set_enum(global, "LV_DRAW_TASK_TYPE_IMAGE", 5);
    lvgl_binding_set_enum(global, "LV_DRAW_TASK_TYPE_LAYER", 6);
    lvgl_binding_set_enum(global, "LV_DRAW_TASK_TYPE_LINE", 7);
    lvgl_binding_set_enum(global, "LV_DRAW_TASK_TYPE_ARC", 8);
    lvgl_binding_set_enum(global, "LV_DRAW_TASK_TYPE_TRIANGLE", 9);
    lvgl_binding_set_enum(global, "LV_DRAW_TASK_TYPE_MASK_RECTANGLE", 10);
    lvgl_binding_set_enum(global, "LV_DRAW_TASK_TYPE_MASK_BITMAP", 11);
    lvgl_binding_set_enum(global, "LV_DRAW_TASK_TYPE_VECTOR", 12);
    lvgl_binding_set_enum(global, "LV_DRAW_TASK_STATE_WAITING", 0);
    lvgl_binding_set_enum(global, "LV_DRAW_TASK_STATE_QUEUED", 1);
    lvgl_binding_set_enum(global, "LV_DRAW_TASK_STATE_IN_PROGRESS", 2);
    lvgl_binding_set_enum(global, "LV_DRAW_TASK_STATE_READY", 3);
    lvgl_binding_set_enum(global, "LV_DISPLAY_ROTATION_0", 0);
    lvgl_binding_set_enum(global, "LV_DISPLAY_ROTATION_90", 1);
    lvgl_binding_set_enum(global, "LV_DISPLAY_ROTATION_180", 2);
    lvgl_binding_set_enum(global, "LV_DISPLAY_ROTATION_270", 3);
    lvgl_binding_set_enum(global, "LV_DISPLAY_RENDER_MODE_PARTIAL", 0);
    lvgl_binding_set_enum(global, "LV_DISPLAY_RENDER_MODE_DIRECT", 1);
    lvgl_binding_set_enum(global, "LV_DISPLAY_RENDER_MODE_FULL", 2);
    lvgl_binding_set_enum(global, "LV_SCR_LOAD_ANIM_NONE", 0);
    lvgl_binding_set_enum(global, "LV_SCR_LOAD_ANIM_OVER_LEFT", 1);
    lvgl_binding_set_enum(global, "LV_SCR_LOAD_ANIM_OVER_RIGHT", 2);
    lvgl_binding_set_enum(global, "LV_SCR_LOAD_ANIM_OVER_TOP", 3);
    lvgl_binding_set_enum(global, "LV_SCR_LOAD_ANIM_OVER_BOTTOM", 4);
    lvgl_binding_set_enum(global, "LV_SCR_LOAD_ANIM_MOVE_LEFT", 5);
    lvgl_binding_set_enum(global, "LV_SCR_LOAD_ANIM_MOVE_RIGHT", 6);
    lvgl_binding_set_enum(global, "LV_SCR_LOAD_ANIM_MOVE_TOP", 7);
    lvgl_binding_set_enum(global, "LV_SCR_LOAD_ANIM_MOVE_BOTTOM", 8);
    lvgl_binding_set_enum(global, "LV_SCR_LOAD_ANIM_FADE_IN", 9);
    lvgl_binding_set_enum(global, "LV_SCR_LOAD_ANIM_FADE_ON", 9);
    lvgl_binding_set_enum(global, "LV_SCR_LOAD_ANIM_FADE_OUT", 10);
    lvgl_binding_set_enum(global, "LV_SCR_LOAD_ANIM_OUT_LEFT", 11);
    lvgl_binding_set_enum(global, "LV_SCR_LOAD_ANIM_OUT_RIGHT", 12);
    lvgl_binding_set_enum(global, "LV_SCR_LOAD_ANIM_OUT_TOP", 13);
    lvgl_binding_set_enum(global, "LV_SCR_LOAD_ANIM_OUT_BOTTOM", 14);
    lvgl_binding_set_enum(global, "LV_OBJ_TREE_WALK_NEXT", 0);
    lvgl_binding_set_enum(global, "LV_OBJ_TREE_WALK_SKIP_CHILDREN", 1);
    lvgl_binding_set_enum(global, "LV_OBJ_TREE_WALK_END", 2);
    lvgl_binding_set_enum(global, "LV_OBJ_POINT_TRANSFORM_FLAG_NONE", 0);
    lvgl_binding_set_enum(global, "LV_OBJ_POINT_TRANSFORM_FLAG_RECURSIVE", 1);
    lvgl_binding_set_enum(global, "LV_OBJ_POINT_TRANSFORM_FLAG_INVERSE", 2);
    lvgl_binding_set_enum(global, "LV_OBJ_POINT_TRANSFORM_FLAG_INVERSE_RECURSIVE", 3);
    lvgl_binding_set_enum(global, "LV_SCROLLBAR_MODE_OFF", 0);
    lvgl_binding_set_enum(global, "LV_SCROLLBAR_MODE_ON", 1);
    lvgl_binding_set_enum(global, "LV_SCROLLBAR_MODE_ACTIVE", 2);
    lvgl_binding_set_enum(global, "LV_SCROLLBAR_MODE_AUTO", 3);
    lvgl_binding_set_enum(global, "LV_SCROLL_SNAP_NONE", 0);
    lvgl_binding_set_enum(global, "LV_SCROLL_SNAP_START", 1);
    lvgl_binding_set_enum(global, "LV_SCROLL_SNAP_END", 2);
    lvgl_binding_set_enum(global, "LV_SCROLL_SNAP_CENTER", 3);
    lvgl_binding_set_enum(global, "LV_STYLE_STATE_CMP_SAME", 0);
    lvgl_binding_set_enum(global, "LV_STYLE_STATE_CMP_DIFF_REDRAW", 1);
    lvgl_binding_set_enum(global, "LV_STYLE_STATE_CMP_DIFF_DRAW_PAD", 2);
    lvgl_binding_set_enum(global, "LV_STYLE_STATE_CMP_DIFF_LAYOUT", 3);
    lvgl_binding_set_enum(global, "LV_LAYER_TYPE_NONE", 0);
    lvgl_binding_set_enum(global, "LV_LAYER_TYPE_SIMPLE", 1);
    lvgl_binding_set_enum(global, "LV_LAYER_TYPE_TRANSFORM", 2);
    lvgl_binding_set_enum(global, "LV_OBJ_CLASS_EDITABLE_INHERIT", 0);
    lvgl_binding_set_enum(global, "LV_OBJ_CLASS_EDITABLE_TRUE", 1);
    lvgl_binding_set_enum(global, "LV_OBJ_CLASS_EDITABLE_FALSE", 2);
    lvgl_binding_set_enum(global, "LV_OBJ_CLASS_GROUP_DEF_INHERIT", 0);
    lvgl_binding_set_enum(global, "LV_OBJ_CLASS_GROUP_DEF_TRUE", 1);
    lvgl_binding_set_enum(global, "LV_OBJ_CLASS_GROUP_DEF_FALSE", 2);
    lvgl_binding_set_enum(global, "LV_OBJ_CLASS_THEME_INHERITABLE_FALSE", 0);
    lvgl_binding_set_enum(global, "LV_OBJ_CLASS_THEME_INHERITABLE_TRUE", 1);
    lvgl_binding_set_enum(global, "LV_KEY_UP", 17);
    lvgl_binding_set_enum(global, "LV_KEY_DOWN", 18);
    lvgl_binding_set_enum(global, "LV_KEY_RIGHT", 19);
    lvgl_binding_set_enum(global, "LV_KEY_LEFT", 20);
    lvgl_binding_set_enum(global, "LV_KEY_ESC", 27);
    lvgl_binding_set_enum(global, "LV_KEY_DEL", 127);
    lvgl_binding_set_enum(global, "LV_KEY_BACKSPACE", 8);
    lvgl_binding_set_enum(global, "LV_KEY_ENTER", 10);
    lvgl_binding_set_enum(global, "LV_KEY_NEXT", 9);
    lvgl_binding_set_enum(global, "LV_KEY_PREV", 11);
    lvgl_binding_set_enum(global, "LV_KEY_HOME", 2);
    lvgl_binding_set_enum(global, "LV_KEY_END", 3);
    lvgl_binding_set_enum(global, "LV_GROUP_REFOCUS_POLICY_NEXT", 0);
    lvgl_binding_set_enum(global, "LV_GROUP_REFOCUS_POLICY_PREV", 1);
    lvgl_binding_set_enum(global, "LV_INDEV_TYPE_NONE", 0);
    lvgl_binding_set_enum(global, "LV_INDEV_TYPE_POINTER", 1);
    lvgl_binding_set_enum(global, "LV_INDEV_TYPE_KEYPAD", 2);
    lvgl_binding_set_enum(global, "LV_INDEV_TYPE_BUTTON", 3);
    lvgl_binding_set_enum(global, "LV_INDEV_TYPE_ENCODER", 4);
    lvgl_binding_set_enum(global, "LV_INDEV_STATE_RELEASED", 0);
    lvgl_binding_set_enum(global, "LV_INDEV_STATE_PRESSED", 1);
    lvgl_binding_set_enum(global, "LV_INDEV_MODE_NONE", 0);
    lvgl_binding_set_enum(global, "LV_INDEV_MODE_TIMER", 1);
    lvgl_binding_set_enum(global, "LV_INDEV_MODE_EVENT", 2);
    lvgl_binding_set_enum(global, "LV_COVER_RES_COVER", 0);
    lvgl_binding_set_enum(global, "LV_COVER_RES_NOT_COVER", 1);
    lvgl_binding_set_enum(global, "LV_COVER_RES_MASKED", 2);
    lvgl_binding_set_enum(global, "LV_STATE_DEFAULT", 0);
    lvgl_binding_set_enum(global, "LV_STATE_CHECKED", 1);
    lvgl_binding_set_enum(global, "LV_STATE_FOCUSED", 2);
    lvgl_binding_set_enum(global, "LV_STATE_FOCUS_KEY", 4);
    lvgl_binding_set_enum(global, "LV_STATE_EDITED", 8);
    lvgl_binding_set_enum(global, "LV_STATE_HOVERED", 16);
    lvgl_binding_set_enum(global, "LV_STATE_PRESSED", 32);
    lvgl_binding_set_enum(global, "LV_STATE_SCROLLED", 64);
    lvgl_binding_set_enum(global, "LV_STATE_DISABLED", 128);
    lvgl_binding_set_enum(global, "LV_STATE_USER_1", 4096);
    lvgl_binding_set_enum(global, "LV_STATE_USER_2", 8192);
    lvgl_binding_set_enum(global, "LV_STATE_USER_3", 16384);
    lvgl_binding_set_enum(global, "LV_STATE_USER_4", 32768);
    lvgl_binding_set_enum(global, "LV_STATE_ANY", 65535);
    lvgl_binding_set_enum(global, "LV_PART_MAIN", 0);
    lvgl_binding_set_enum(global, "LV_PART_SCROLLBAR", 65536);
    lvgl_binding_set_enum(global, "LV_PART_INDICATOR", 131072);
    lvgl_binding_set_enum(global, "LV_PART_KNOB", 196608);
    lvgl_binding_set_enum(global, "LV_PART_SELECTED", 262144);
    lvgl_binding_set_enum(global, "LV_PART_ITEMS", 327680);
    lvgl_binding_set_enum(global, "LV_PART_CURSOR", 393216);
    lvgl_binding_set_enum(global, "LV_PART_CUSTOM_FIRST", 524288);
    lvgl_binding_set_enum(global, "LV_PART_ANY", 983040);
    lvgl_binding_set_enum(global, "LV_OBJ_FLAG_HIDDEN", 1);
    lvgl_binding_set_enum(global, "LV_OBJ_FLAG_CLICKABLE", 2);
    lvgl_binding_set_enum(global, "LV_OBJ_FLAG_CLICK_FOCUSABLE", 4);
    lvgl_binding_set_enum(global, "LV_OBJ_FLAG_CHECKABLE", 8);
    lvgl_binding_set_enum(global, "LV_OBJ_FLAG_SCROLLABLE", 16);
    lvgl_binding_set_enum(global, "LV_OBJ_FLAG_SCROLL_ELASTIC", 32);
    lvgl_binding_set_enum(global, "LV_OBJ_FLAG_SCROLL_MOMENTUM", 64);
    lvgl_binding_set_enum(global, "LV_OBJ_FLAG_SCROLL_ONE", 128);
    lvgl_binding_set_enum(global, "LV_OBJ_FLAG_SCROLL_CHAIN_HOR", 256);
    lvgl_binding_set_enum(global, "LV_OBJ_FLAG_SCROLL_CHAIN_VER", 512);
    lvgl_binding_set_enum(global, "LV_OBJ_FLAG_SCROLL_CHAIN", 768);
    lvgl_binding_set_enum(global, "LV_OBJ_FLAG_SCROLL_ON_FOCUS", 1024);
    lvgl_binding_set_enum(global, "LV_OBJ_FLAG_SCROLL_WITH_ARROW", 2048);
    lvgl_binding_set_enum(global, "LV_OBJ_FLAG_SNAPPABLE", 4096);
    lvgl_binding_set_enum(global, "LV_OBJ_FLAG_PRESS_LOCK", 8192);
    lvgl_binding_set_enum(global, "LV_OBJ_FLAG_EVENT_BUBBLE", 16384);
    lvgl_binding_set_enum(global, "LV_OBJ_FLAG_GESTURE_BUBBLE", 32768);
    lvgl_binding_set_enum(global, "LV_OBJ_FLAG_ADV_HITTEST", 65536);
    lvgl_binding_set_enum(global, "LV_OBJ_FLAG_IGNORE_LAYOUT", 131072);
    lvgl_binding_set_enum(global, "LV_OBJ_FLAG_FLOATING", 262144);
    lvgl_binding_set_enum(global, "LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS", 524288);
    lvgl_binding_set_enum(global, "LV_OBJ_FLAG_OVERFLOW_VISIBLE", 1048576);
    lvgl_binding_set_enum(global, "LV_OBJ_FLAG_FLEX_IN_NEW_TRACK", 2097152);
    lvgl_binding_set_enum(global, "LV_OBJ_FLAG_LAYOUT_1", 8388608);
    lvgl_binding_set_enum(global, "LV_OBJ_FLAG_LAYOUT_2", 16777216);
    lvgl_binding_set_enum(global, "LV_OBJ_FLAG_WIDGET_1", 33554432);
    lvgl_binding_set_enum(global, "LV_OBJ_FLAG_WIDGET_2", 67108864);
    lvgl_binding_set_enum(global, "LV_OBJ_FLAG_USER_1", 134217728);
    lvgl_binding_set_enum(global, "LV_OBJ_FLAG_USER_2", 268435456);
    lvgl_binding_set_enum(global, "LV_OBJ_FLAG_USER_3", 536870912);
    lvgl_binding_set_enum(global, "LV_OBJ_FLAG_USER_4", 1073741824);
    lvgl_binding_set_enum(global, "LV_FONT_FMT_TXT_CMAP_FORMAT0_FULL", 0);
    lvgl_binding_set_enum(global, "LV_FONT_FMT_TXT_CMAP_SPARSE_FULL", 1);
    lvgl_binding_set_enum(global, "LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY", 2);
    lvgl_binding_set_enum(global, "LV_FONT_FMT_TXT_CMAP_SPARSE_TINY", 3);
    lvgl_binding_set_enum(global, "LV_FONT_FMT_TXT_PLAIN", 0);
    lvgl_binding_set_enum(global, "LV_FONT_FMT_TXT_COMPRESSED", 1);
    lvgl_binding_set_enum(global, "LV_FONT_FMT_TXT_COMPRESSED_NO_PREFILTER", 1);
    lvgl_binding_set_enum(global, "LV_IMAGE_ALIGN_DEFAULT", 0);
    lvgl_binding_set_enum(global, "LV_IMAGE_ALIGN_TOP_LEFT", 1);
    lvgl_binding_set_enum(global, "LV_IMAGE_ALIGN_TOP_MID", 2);
    lvgl_binding_set_enum(global, "LV_IMAGE_ALIGN_TOP_RIGHT", 3);
    lvgl_binding_set_enum(global, "LV_IMAGE_ALIGN_BOTTOM_LEFT", 4);
    lvgl_binding_set_enum(global, "LV_IMAGE_ALIGN_BOTTOM_MID", 5);
    lvgl_binding_set_enum(global, "LV_IMAGE_ALIGN_BOTTOM_RIGHT", 6);
    lvgl_binding_set_enum(global, "LV_IMAGE_ALIGN_LEFT_MID", 7);
    lvgl_binding_set_enum(global, "LV_IMAGE_ALIGN_RIGHT_MID", 8);
    lvgl_binding_set_enum(global, "LV_IMAGE_ALIGN_CENTER", 9);
    lvgl_binding_set_enum(global, "LV_IMAGE_ALIGN_AUTO_TRANSFORM", 10);
    lvgl_binding_set_enum(global, "LV_IMAGE_ALIGN_STRETCH", 11);
    lvgl_binding_set_enum(global, "LV_IMAGE_ALIGN_TILE", 12);
    lvgl_binding_set_enum(global, "LV_ANIM_IMAGE_PART_MAIN", 0);
    lvgl_binding_set_enum(global, "LV_ARC_MODE_NORMAL", 0);
    lvgl_binding_set_enum(global, "LV_ARC_MODE_SYMMETRICAL", 1);
    lvgl_binding_set_enum(global, "LV_ARC_MODE_REVERSE", 2);
    lvgl_binding_set_enum(global, "LV_LABEL_LONG_WRAP", 0);
    lvgl_binding_set_enum(global, "LV_LABEL_LONG_DOT", 1);
    lvgl_binding_set_enum(global, "LV_LABEL_LONG_SCROLL", 2);
    lvgl_binding_set_enum(global, "LV_LABEL_LONG_SCROLL_CIRCULAR", 3);
    lvgl_binding_set_enum(global, "LV_LABEL_LONG_CLIP", 4);
    lvgl_binding_set_enum(global, "LV_BAR_MODE_NORMAL", 0);
    lvgl_binding_set_enum(global, "LV_BAR_MODE_SYMMETRICAL", 1);
    lvgl_binding_set_enum(global, "LV_BAR_MODE_RANGE", 2);
    lvgl_binding_set_enum(global, "LV_BAR_ORIENTATION_AUTO", 0);
    lvgl_binding_set_enum(global, "LV_BAR_ORIENTATION_HORIZONTAL", 1);
    lvgl_binding_set_enum(global, "LV_BAR_ORIENTATION_VERTICAL", 2);
    lvgl_binding_set_enum(global, "LV_BUTTONMATRIX_CTRL_HIDDEN", 16);
    lvgl_binding_set_enum(global, "LV_BUTTONMATRIX_CTRL_NO_REPEAT", 32);
    lvgl_binding_set_enum(global, "LV_BUTTONMATRIX_CTRL_DISABLED", 64);
    lvgl_binding_set_enum(global, "LV_BUTTONMATRIX_CTRL_CHECKABLE", 128);
    lvgl_binding_set_enum(global, "LV_BUTTONMATRIX_CTRL_CHECKED", 256);
    lvgl_binding_set_enum(global, "LV_BUTTONMATRIX_CTRL_CLICK_TRIG", 512);
    lvgl_binding_set_enum(global, "LV_BUTTONMATRIX_CTRL_POPOVER", 1024);
    lvgl_binding_set_enum(global, "LV_BUTTONMATRIX_CTRL_RESERVED_1", 2048);
    lvgl_binding_set_enum(global, "LV_BUTTONMATRIX_CTRL_RESERVED_2", 4096);
    lvgl_binding_set_enum(global, "LV_BUTTONMATRIX_CTRL_RESERVED_3", 8192);
    lvgl_binding_set_enum(global, "LV_BUTTONMATRIX_CTRL_CUSTOM_1", 16384);
    lvgl_binding_set_enum(global, "LV_BUTTONMATRIX_CTRL_CUSTOM_2", 32768);
    lvgl_binding_set_enum(global, "LV_CHART_TYPE_NONE", 0);
    lvgl_binding_set_enum(global, "LV_CHART_TYPE_LINE", 1);
    lvgl_binding_set_enum(global, "LV_CHART_TYPE_BAR", 2);
    lvgl_binding_set_enum(global, "LV_CHART_TYPE_SCATTER", 3);
    lvgl_binding_set_enum(global, "LV_CHART_UPDATE_MODE_SHIFT", 0);
    lvgl_binding_set_enum(global, "LV_CHART_UPDATE_MODE_CIRCULAR", 1);
    lvgl_binding_set_enum(global, "LV_CHART_AXIS_PRIMARY_Y", 0);
    lvgl_binding_set_enum(global, "LV_CHART_AXIS_SECONDARY_Y", 1);
    lvgl_binding_set_enum(global, "LV_CHART_AXIS_PRIMARY_X", 2);
    lvgl_binding_set_enum(global, "LV_CHART_AXIS_SECONDARY_X", 4);
    lvgl_binding_set_enum(global, "LV_CHART_AXIS_LAST", 5);
    lvgl_binding_set_enum(global, "LV_IMAGEBUTTON_STATE_RELEASED", 0);
    lvgl_binding_set_enum(global, "LV_IMAGEBUTTON_STATE_PRESSED", 1);
    lvgl_binding_set_enum(global, "LV_IMAGEBUTTON_STATE_DISABLED", 2);
    lvgl_binding_set_enum(global, "LV_IMAGEBUTTON_STATE_CHECKED_RELEASED", 3);
    lvgl_binding_set_enum(global, "LV_IMAGEBUTTON_STATE_CHECKED_PRESSED", 4);
    lvgl_binding_set_enum(global, "LV_IMAGEBUTTON_STATE_CHECKED_DISABLED", 5);
    lvgl_binding_set_enum(global, "LV_IMAGEBUTTON_STATE_NUM", 6);
    lvgl_binding_set_enum(global, "LV_KEYBOARD_MODE_TEXT_LOWER", 0);
    lvgl_binding_set_enum(global, "LV_KEYBOARD_MODE_TEXT_UPPER", 1);
    lvgl_binding_set_enum(global, "LV_KEYBOARD_MODE_SPECIAL", 2);
    lvgl_binding_set_enum(global, "LV_KEYBOARD_MODE_NUMBER", 3);
    lvgl_binding_set_enum(global, "LV_KEYBOARD_MODE_USER_1", 4);
    lvgl_binding_set_enum(global, "LV_KEYBOARD_MODE_USER_2", 5);
    lvgl_binding_set_enum(global, "LV_KEYBOARD_MODE_USER_3", 6);
    lvgl_binding_set_enum(global, "LV_KEYBOARD_MODE_USER_4", 7);
    lvgl_binding_set_enum(global, "LV_MENU_HEADER_TOP_FIXED", 0);
    lvgl_binding_set_enum(global, "LV_MENU_HEADER_TOP_UNFIXED", 1);
    lvgl_binding_set_enum(global, "LV_MENU_HEADER_BOTTOM_FIXED", 2);
    lvgl_binding_set_enum(global, "LV_MENU_ROOT_BACK_BUTTON_DISABLED", 0);
    lvgl_binding_set_enum(global, "LV_MENU_ROOT_BACK_BUTTON_ENABLED", 1);
    lvgl_binding_set_enum(global, "LV_ROLLER_MODE_NORMAL", 0);
    lvgl_binding_set_enum(global, "LV_ROLLER_MODE_INFINITE", 1);
    lvgl_binding_set_enum(global, "LV_SCALE_MODE_HORIZONTAL_TOP", 0);
    lvgl_binding_set_enum(global, "LV_SCALE_MODE_HORIZONTAL_BOTTOM", 1);
    lvgl_binding_set_enum(global, "LV_SCALE_MODE_VERTICAL_LEFT", 2);
    lvgl_binding_set_enum(global, "LV_SCALE_MODE_VERTICAL_RIGHT", 3);
    lvgl_binding_set_enum(global, "LV_SCALE_MODE_ROUND_INNER", 4);
    lvgl_binding_set_enum(global, "LV_SCALE_MODE_ROUND_OUTER", 5);
    lvgl_binding_set_enum(global, "LV_SCALE_MODE_LAST", 6);
    lvgl_binding_set_enum(global, "LV_SLIDER_MODE_NORMAL", 0);
    lvgl_binding_set_enum(global, "LV_SLIDER_MODE_SYMMETRICAL", 1);
    lvgl_binding_set_enum(global, "LV_SLIDER_MODE_RANGE", 2);
    lvgl_binding_set_enum(global, "LV_SPAN_OVERFLOW_CLIP", 0);
    lvgl_binding_set_enum(global, "LV_SPAN_OVERFLOW_ELLIPSIS", 1);
    lvgl_binding_set_enum(global, "LV_SPAN_OVERFLOW_LAST", 2);
    lvgl_binding_set_enum(global, "LV_SPAN_MODE_FIXED", 0);
    lvgl_binding_set_enum(global, "LV_SPAN_MODE_EXPAND", 1);
    lvgl_binding_set_enum(global, "LV_SPAN_MODE_BREAK", 2);
    lvgl_binding_set_enum(global, "LV_SPAN_MODE_LAST", 3);
    lvgl_binding_set_enum(global, "LV_PART_TEXTAREA_PLACEHOLDER", 524288);
    lvgl_binding_set_enum(global, "LV_TABLE_CELL_CTRL_MERGE_RIGHT", 1);
    lvgl_binding_set_enum(global, "LV_TABLE_CELL_CTRL_TEXT_CROP", 2);
    lvgl_binding_set_enum(global, "LV_TABLE_CELL_CTRL_CUSTOM_1", 16);
    lvgl_binding_set_enum(global, "LV_TABLE_CELL_CTRL_CUSTOM_2", 32);
    lvgl_binding_set_enum(global, "LV_TABLE_CELL_CTRL_CUSTOM_3", 64);
    lvgl_binding_set_enum(global, "LV_TABLE_CELL_CTRL_CUSTOM_4", 128);
    lvgl_binding_set_enum(global, "LV_SUBJECT_TYPE_INVALID", 0);
    lvgl_binding_set_enum(global, "LV_SUBJECT_TYPE_NONE", 1);
    lvgl_binding_set_enum(global, "LV_SUBJECT_TYPE_INT", 2);
    lvgl_binding_set_enum(global, "LV_SUBJECT_TYPE_POINTER", 3);
    lvgl_binding_set_enum(global, "LV_SUBJECT_TYPE_COLOR", 4);
    lvgl_binding_set_enum(global, "LV_SUBJECT_TYPE_GROUP", 5);
    lvgl_binding_set_enum(global, "LV_SUBJECT_TYPE_STRING", 6);
    lvgl_binding_set_enum(global, "LV_GRIDNAV_CTRL_NONE", 0);
    lvgl_binding_set_enum(global, "LV_GRIDNAV_CTRL_ROLLOVER", 1);
    lvgl_binding_set_enum(global, "LV_GRIDNAV_CTRL_SCROLL_FIRST", 2);
    lvgl_binding_set_enum(global, "LV_GRIDNAV_CTRL_HORIZONTAL_MOVE_ONLY", 4);
    lvgl_binding_set_enum(global, "LV_GRIDNAV_CTRL_VERTICAL_MOVE_ONLY", 8);
    lvgl_binding_set_enum(global, "LV_FREETYPE_FONT_STYLE_NORMAL", 0);
    lvgl_binding_set_enum(global, "LV_FREETYPE_FONT_STYLE_ITALIC", 1);
    lvgl_binding_set_enum(global, "LV_FREETYPE_FONT_STYLE_BOLD", 2);
    lvgl_binding_set_enum(global, "LV_FREETYPE_FONT_RENDER_MODE_BITMAP", 0);
    lvgl_binding_set_enum(global, "LV_FREETYPE_FONT_RENDER_MODE_OUTLINE", 1);
    lvgl_binding_set_enum(global, "LV_FREETYPE_OUTLINE_END", 0);
    lvgl_binding_set_enum(global, "LV_FREETYPE_OUTLINE_MOVE_TO", 1);
    lvgl_binding_set_enum(global, "LV_FREETYPE_OUTLINE_LINE_TO", 2);
    lvgl_binding_set_enum(global, "LV_FREETYPE_OUTLINE_CUBIC_TO", 3);
    lvgl_binding_set_enum(global, "LV_FREETYPE_OUTLINE_CONIC_TO", 4);
    lvgl_binding_set_enum(global, "LV_DRAW_SW_MASK_RES_TRANSP", 0);
    lvgl_binding_set_enum(global, "LV_DRAW_SW_MASK_RES_FULL_COVER", 1);
    lvgl_binding_set_enum(global, "LV_DRAW_SW_MASK_RES_CHANGED", 2);
    lvgl_binding_set_enum(global, "LV_DRAW_SW_MASK_RES_UNKNOWN", 3);
    lvgl_binding_set_enum(global, "LV_DRAW_SW_MASK_TYPE_LINE", 0);
    lvgl_binding_set_enum(global, "LV_DRAW_SW_MASK_TYPE_ANGLE", 1);
    lvgl_binding_set_enum(global, "LV_DRAW_SW_MASK_TYPE_RADIUS", 2);
    lvgl_binding_set_enum(global, "LV_DRAW_SW_MASK_TYPE_FADE", 3);
    lvgl_binding_set_enum(global, "LV_DRAW_SW_MASK_TYPE_MAP", 4);
    lvgl_binding_set_enum(global, "LV_DRAW_SW_MASK_LINE_SIDE_LEFT", 0);
    lvgl_binding_set_enum(global, "LV_DRAW_SW_MASK_LINE_SIDE_RIGHT", 1);
    lvgl_binding_set_enum(global, "LV_DRAW_SW_MASK_LINE_SIDE_TOP", 2);
    lvgl_binding_set_enum(global, "LV_DRAW_SW_MASK_LINE_SIDE_BOTTOM", 3);
#ifdef LV_SYMBOL_BULLET
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_BULLET"), jerry_string_sz(LV_SYMBOL_BULLET));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_BULLET is not defined")
#endif
#ifdef LV_SYMBOL_AUDIO
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_AUDIO"), jerry_string_sz(LV_SYMBOL_AUDIO));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_AUDIO is not defined")
#endif
#ifdef LV_SYMBOL_VIDEO
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_VIDEO"), jerry_string_sz(LV_SYMBOL_VIDEO));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_VIDEO is not defined")
#endif
#ifdef LV_SYMBOL_LIST
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_LIST"), jerry_string_sz(LV_SYMBOL_LIST));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_LIST is not defined")
#endif
#ifdef LV_SYMBOL_OK
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_OK"), jerry_string_sz(LV_SYMBOL_OK));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_OK is not defined")
#endif
#ifdef LV_SYMBOL_CLOSE
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_CLOSE"), jerry_string_sz(LV_SYMBOL_CLOSE));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_CLOSE is not defined")
#endif
#ifdef LV_SYMBOL_POWER
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_POWER"), jerry_string_sz(LV_SYMBOL_POWER));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_POWER is not defined")
#endif
#ifdef LV_SYMBOL_SETTINGS
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_SETTINGS"), jerry_string_sz(LV_SYMBOL_SETTINGS));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_SETTINGS is not defined")
#endif
#ifdef LV_SYMBOL_HOME
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_HOME"), jerry_string_sz(LV_SYMBOL_HOME));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_HOME is not defined")
#endif
#ifdef LV_SYMBOL_DOWNLOAD
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_DOWNLOAD"), jerry_string_sz(LV_SYMBOL_DOWNLOAD));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_DOWNLOAD is not defined")
#endif
#ifdef LV_SYMBOL_DRIVE
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_DRIVE"), jerry_string_sz(LV_SYMBOL_DRIVE));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_DRIVE is not defined")
#endif
#ifdef LV_SYMBOL_REFRESH
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_REFRESH"), jerry_string_sz(LV_SYMBOL_REFRESH));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_REFRESH is not defined")
#endif
#ifdef LV_SYMBOL_MUTE
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_MUTE"), jerry_string_sz(LV_SYMBOL_MUTE));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_MUTE is not defined")
#endif
#ifdef LV_SYMBOL_VOLUME_MID
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_VOLUME_MID"), jerry_string_sz(LV_SYMBOL_VOLUME_MID));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_VOLUME_MID is not defined")
#endif
#ifdef LV_SYMBOL_VOLUME_MAX
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_VOLUME_MAX"), jerry_string_sz(LV_SYMBOL_VOLUME_MAX));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_VOLUME_MAX is not defined")
#endif
#ifdef LV_SYMBOL_IMAGE
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_IMAGE"), jerry_string_sz(LV_SYMBOL_IMAGE));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_IMAGE is not defined")
#endif
#ifdef LV_SYMBOL_TINT
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_TINT"), jerry_string_sz(LV_SYMBOL_TINT));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_TINT is not defined")
#endif
#ifdef LV_SYMBOL_PREV
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_PREV"), jerry_string_sz(LV_SYMBOL_PREV));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_PREV is not defined")
#endif
#ifdef LV_SYMBOL_PLAY
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_PLAY"), jerry_string_sz(LV_SYMBOL_PLAY));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_PLAY is not defined")
#endif
#ifdef LV_SYMBOL_PAUSE
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_PAUSE"), jerry_string_sz(LV_SYMBOL_PAUSE));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_PAUSE is not defined")
#endif
#ifdef LV_SYMBOL_STOP
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_STOP"), jerry_string_sz(LV_SYMBOL_STOP));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_STOP is not defined")
#endif
#ifdef LV_SYMBOL_NEXT
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_NEXT"), jerry_string_sz(LV_SYMBOL_NEXT));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_NEXT is not defined")
#endif
#ifdef LV_SYMBOL_EJECT
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_EJECT"), jerry_string_sz(LV_SYMBOL_EJECT));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_EJECT is not defined")
#endif
#ifdef LV_SYMBOL_LEFT
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_LEFT"), jerry_string_sz(LV_SYMBOL_LEFT));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_LEFT is not defined")
#endif
#ifdef LV_SYMBOL_RIGHT
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_RIGHT"), jerry_string_sz(LV_SYMBOL_RIGHT));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_RIGHT is not defined")
#endif
#ifdef LV_SYMBOL_PLUS
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_PLUS"), jerry_string_sz(LV_SYMBOL_PLUS));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_PLUS is not defined")
#endif
#ifdef LV_SYMBOL_MINUS
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_MINUS"), jerry_string_sz(LV_SYMBOL_MINUS));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_MINUS is not defined")
#endif
#ifdef LV_SYMBOL_EYE_OPEN
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_EYE_OPEN"), jerry_string_sz(LV_SYMBOL_EYE_OPEN));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_EYE_OPEN is not defined")
#endif
#ifdef LV_SYMBOL_EYE_CLOSE
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_EYE_CLOSE"), jerry_string_sz(LV_SYMBOL_EYE_CLOSE));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_EYE_CLOSE is not defined")
#endif
#ifdef LV_SYMBOL_WARNING
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_WARNING"), jerry_string_sz(LV_SYMBOL_WARNING));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_WARNING is not defined")
#endif
#ifdef LV_SYMBOL_SHUFFLE
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_SHUFFLE"), jerry_string_sz(LV_SYMBOL_SHUFFLE));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_SHUFFLE is not defined")
#endif
#ifdef LV_SYMBOL_UP
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_UP"), jerry_string_sz(LV_SYMBOL_UP));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_UP is not defined")
#endif
#ifdef LV_SYMBOL_DOWN
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_DOWN"), jerry_string_sz(LV_SYMBOL_DOWN));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_DOWN is not defined")
#endif
#ifdef LV_SYMBOL_LOOP
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_LOOP"), jerry_string_sz(LV_SYMBOL_LOOP));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_LOOP is not defined")
#endif
#ifdef LV_SYMBOL_DIRECTORY
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_DIRECTORY"), jerry_string_sz(LV_SYMBOL_DIRECTORY));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_DIRECTORY is not defined")
#endif
#ifdef LV_SYMBOL_UPLOAD
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_UPLOAD"), jerry_string_sz(LV_SYMBOL_UPLOAD));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_UPLOAD is not defined")
#endif
#ifdef LV_SYMBOL_CALL
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_CALL"), jerry_string_sz(LV_SYMBOL_CALL));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_CALL is not defined")
#endif
#ifdef LV_SYMBOL_CUT
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_CUT"), jerry_string_sz(LV_SYMBOL_CUT));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_CUT is not defined")
#endif
#ifdef LV_SYMBOL_COPY
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_COPY"), jerry_string_sz(LV_SYMBOL_COPY));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_COPY is not defined")
#endif
#ifdef LV_SYMBOL_SAVE
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_SAVE"), jerry_string_sz(LV_SYMBOL_SAVE));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_SAVE is not defined")
#endif
#ifdef LV_SYMBOL_BARS
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_BARS"), jerry_string_sz(LV_SYMBOL_BARS));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_BARS is not defined")
#endif
#ifdef LV_SYMBOL_ENVELOPE
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_ENVELOPE"), jerry_string_sz(LV_SYMBOL_ENVELOPE));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_ENVELOPE is not defined")
#endif
#ifdef LV_SYMBOL_CHARGE
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_CHARGE"), jerry_string_sz(LV_SYMBOL_CHARGE));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_CHARGE is not defined")
#endif
#ifdef LV_SYMBOL_PASTE
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_PASTE"), jerry_string_sz(LV_SYMBOL_PASTE));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_PASTE is not defined")
#endif
#ifdef LV_SYMBOL_BELL
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_BELL"), jerry_string_sz(LV_SYMBOL_BELL));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_BELL is not defined")
#endif
#ifdef LV_SYMBOL_KEYBOARD
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_KEYBOARD"), jerry_string_sz(LV_SYMBOL_KEYBOARD));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_KEYBOARD is not defined")
#endif
#ifdef LV_SYMBOL_GPS
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_GPS"), jerry_string_sz(LV_SYMBOL_GPS));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_GPS is not defined")
#endif
#ifdef LV_SYMBOL_FILE
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_FILE"), jerry_string_sz(LV_SYMBOL_FILE));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_FILE is not defined")
#endif
#ifdef LV_SYMBOL_WIFI
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_WIFI"), jerry_string_sz(LV_SYMBOL_WIFI));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_WIFI is not defined")
#endif
#ifdef LV_SYMBOL_BATTERY_FULL
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_BATTERY_FULL"), jerry_string_sz(LV_SYMBOL_BATTERY_FULL));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_BATTERY_FULL is not defined")
#endif
#ifdef LV_SYMBOL_BATTERY_3
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_BATTERY_3"), jerry_string_sz(LV_SYMBOL_BATTERY_3));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_BATTERY_3 is not defined")
#endif
#ifdef LV_SYMBOL_BATTERY_2
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_BATTERY_2"), jerry_string_sz(LV_SYMBOL_BATTERY_2));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_BATTERY_2 is not defined")
#endif
#ifdef LV_SYMBOL_BATTERY_1
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_BATTERY_1"), jerry_string_sz(LV_SYMBOL_BATTERY_1));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_BATTERY_1 is not defined")
#endif
#ifdef LV_SYMBOL_BATTERY_EMPTY
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_BATTERY_EMPTY"), jerry_string_sz(LV_SYMBOL_BATTERY_EMPTY));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_BATTERY_EMPTY is not defined")
#endif
#ifdef LV_SYMBOL_USB
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_USB"), jerry_string_sz(LV_SYMBOL_USB));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_USB is not defined")
#endif
#ifdef LV_SYMBOL_BLUETOOTH
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_BLUETOOTH"), jerry_string_sz(LV_SYMBOL_BLUETOOTH));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_BLUETOOTH is not defined")
#endif
#ifdef LV_SYMBOL_TRASH
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_TRASH"), jerry_string_sz(LV_SYMBOL_TRASH));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_TRASH is not defined")
#endif
#ifdef LV_SYMBOL_EDIT
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_EDIT"), jerry_string_sz(LV_SYMBOL_EDIT));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_EDIT is not defined")
#endif
#ifdef LV_SYMBOL_BACKSPACE
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_BACKSPACE"), jerry_string_sz(LV_SYMBOL_BACKSPACE));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_BACKSPACE is not defined")
#endif
#ifdef LV_SYMBOL_SD_CARD
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_SD_CARD"), jerry_string_sz(LV_SYMBOL_SD_CARD));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_SD_CARD is not defined")
#endif
#ifdef LV_SYMBOL_NEW_LINE
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_NEW_LINE"), jerry_string_sz(LV_SYMBOL_NEW_LINE));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_NEW_LINE is not defined")
#endif
#ifdef LV_SYMBOL_DUMMY
    jerry_object_set(global, jerry_string_sz("LV_SYMBOL_DUMMY"), jerry_string_sz(LV_SYMBOL_DUMMY));
#else
    #pragma message("WARNING: Macro LV_SYMBOL_DUMMY is not defined")
#endif
#ifdef LV_LABEL_TEXT_SELECTION
    lvgl_binding_set_enum(global, "LV_LABEL_TEXT_SELECTION", LV_LABEL_TEXT_SELECTION);
#else
    #pragma message("WARNING: Macro LV_LABEL_TEXT_SELECTION is not defined")
#endif
#ifdef LV_LABEL_LONG_TXT_HINT
    lvgl_binding_set_enum(global, "LV_LABEL_LONG_TXT_HINT", LV_LABEL_LONG_TXT_HINT);
#else
    #pragma message("WARNING: Macro LV_LABEL_LONG_TXT_HINT is not defined")
#endif
#ifdef LV_LABEL_WAIT_CHAR_COUNT
    lvgl_binding_set_enum(global, "LV_LABEL_WAIT_CHAR_COUNT", LV_LABEL_WAIT_CHAR_COUNT);
#else
    #pragma message("WARNING: Macro LV_LABEL_WAIT_CHAR_COUNT is not defined")
#endif
#ifdef LV_ANIM_REPEAT_INFINITE
    lvgl_binding_set_enum(global, "LV_ANIM_REPEAT_INFINITE", LV_ANIM_REPEAT_INFINITE);
#else
    #pragma message("WARNING: Macro LV_ANIM_REPEAT_INFINITE is not defined")
#endif
#ifdef LV_ANIM_PLAYTIME_INFINITE
    lvgl_binding_set_enum(global, "LV_ANIM_PLAYTIME_INFINITE", LV_ANIM_PLAYTIME_INFINITE);
#else
    #pragma message("WARNING: Macro LV_ANIM_PLAYTIME_INFINITE is not defined")
#endif
#ifdef LV_LABEL_DOT_NUM
    lvgl_binding_set_enum(global, "LV_LABEL_DOT_NUM", LV_LABEL_DOT_NUM);
#else
    #pragma message("WARNING: Macro LV_LABEL_DOT_NUM is not defined")
#endif
#ifdef LV_LABEL_POS_LAST
    lvgl_binding_set_enum(global, "LV_LABEL_POS_LAST", LV_LABEL_POS_LAST);
#else
    #pragma message("WARNING: Macro LV_LABEL_POS_LAST is not defined")
#endif
#ifdef LV_LABEL_TEXT_SELECTION_OFF
    lvgl_binding_set_enum(global, "LV_LABEL_TEXT_SELECTION_OFF", LV_LABEL_TEXT_SELECTION_OFF);
#else
    #pragma message("WARNING: Macro LV_LABEL_TEXT_SELECTION_OFF is not defined")
#endif
    jerry_value_free(global);
}

/********************************** 初始化 LVGL 绑定系统 **********************************/
/**
 * @brief 初始化回调系统，注册 LVGL 对象删除事件处理函数，并注册 LVGL 函数
 */
void lv_binding_init() {
    lv_binding_jerryscript_register_functions(lvgl_binding_funcs, sizeof(lvgl_binding_funcs) / sizeof(LVBindingJerryscriptFuncEntry_t));
    lv_bindings_misc_init();
    register_lvgl_enums();
}
