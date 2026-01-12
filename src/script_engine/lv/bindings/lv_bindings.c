
/**
 * @file lv_bindings.c
 * @brief 将 LVGL 绑定到 JerryScript 的实现文件，此文件使用脚本`gen_lvgl_binding.py`自动生成。
 * @author Sab1e
 * @date 2026-01-13
 */

/* Third party header files ---------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include "jerryscript.h"
#include "lvgl.h"
/* ElenaOS header files ---------------------------------------*/
#include "lv_bindings.h"
#include "lv_bindings_special.h"
#include "lv_bindings_anim.h"
#include "lv_bindings_timer.h"
#include "lv_bindings_event.h"
#include "script_engine_core.h"
#include "elena_os_mem.h"
#include "lvgl_js_bridge.h"
#include "script_engine_lv.h"

/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

/********************************** 宏定义处理辅助函数 **********************************/

static void lvgl_binding_set_enum(jerry_value_t parent, const char* key, int32_t val) {
    jerry_value_t jkey = jerry_string_sz(key);
    jerry_value_t jval = jerry_number(val);
    jerry_value_free(jerry_object_set(parent, jkey, jval));
    jerry_value_free(jkey);
    jerry_value_free(jval);
}

// 函数声明
static jerry_value_t js_lv_arc_align_obj_to_angle(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_arc_bind_value(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_arc_create(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_arc_get_angle_end(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_arc_get_angle_start(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_arc_get_bg_angle_end(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_arc_get_bg_angle_start(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_arc_get_knob_offset(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_arc_get_max_value(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_arc_get_min_value(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_arc_get_mode(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_arc_get_rotation(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_arc_get_value(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_arc_rotate_obj_to_angle(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_arc_set_angles(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_arc_set_bg_angles(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_arc_set_bg_end_angle(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_arc_set_bg_start_angle(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_arc_set_change_rate(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_arc_set_end_angle(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_arc_set_knob_offset(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_arc_set_mode(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_arc_set_range(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_arc_set_rotation(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_arc_set_start_angle(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_arc_set_value(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_bar_create(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_bar_get_max_value(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_bar_get_min_value(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_bar_get_mode(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_bar_get_orientation(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_bar_get_start_value(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_bar_get_value(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_bar_is_symmetrical(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_bar_set_mode(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_bar_set_orientation(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_bar_set_range(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_bar_set_start_value(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_bar_set_value(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_bin_decoder_close(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_bin_decoder_open(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_button_create(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_checkbox_create(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_checkbox_get_text(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_checkbox_set_text(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_checkbox_set_text_static(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_color_hex(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_delay_ms(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_display_get_screen_active(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_dropdown_add_option(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_dropdown_bind_value(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_dropdown_clear_options(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_dropdown_close(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_dropdown_create(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_dropdown_get_dir(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_dropdown_get_list(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_dropdown_get_option_count(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_dropdown_get_option_index(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_dropdown_get_options(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_dropdown_get_selected(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_dropdown_get_selected_highlight(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_dropdown_get_selected_str(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_dropdown_get_symbol(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_dropdown_get_text(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_dropdown_is_open(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_dropdown_open(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_dropdown_set_dir(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_dropdown_set_options(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_dropdown_set_options_static(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_dropdown_set_selected(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_dropdown_set_selected_highlight(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_dropdown_set_symbol(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_dropdown_set_text(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_buf_free(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_cache_drop(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_cache_init(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_cache_is_enabled(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_cache_resize(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_create(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_decoder_add_to_cache(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_decoder_close(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_decoder_create(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_decoder_delete(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_decoder_get_area(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_decoder_get_info(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_decoder_get_next(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_decoder_open(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_decoder_post_process(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_decoder_set_close_cb(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_decoder_set_get_area_cb(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_decoder_set_info_cb(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_decoder_set_open_cb(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_get_antialias(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_get_bitmap_map_src(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_get_blend_mode(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_get_inner_align(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_get_offset_x(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_get_offset_y(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_get_pivot(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_get_rotation(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_get_scale(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_get_scale_x(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_get_scale_y(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_get_src(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_header_cache_drop(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_header_cache_init(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_header_cache_is_enabled(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_header_cache_resize(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_set_antialias(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_set_bitmap_map_src(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_set_blend_mode(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_set_inner_align(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_set_offset_x(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_set_offset_y(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_set_pivot(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_set_rotation(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_set_scale(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_set_scale_x(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_set_scale_y(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_image_src_get_type(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_label_bind_text(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_label_create(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_label_cut_text(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_label_get_letter_on(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_label_get_letter_pos(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_label_get_long_mode(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_label_get_text(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_label_get_text_selection_end(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_label_get_text_selection_start(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_label_ins_text(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_label_is_char_under_pos(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_label_set_long_mode(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_label_set_text(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_label_set_text_selection_end(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_label_set_text_selection_start(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_label_set_text_static(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_list_add_button(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_list_add_text(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_list_create(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_list_get_button_text(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_list_set_button_text(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_add_flag(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_add_state(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_add_style(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_align(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_align_to(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_allocate_spec_attr(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_area_is_visible(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_bind_checked(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_bind_flag_if_eq(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_bind_flag_if_not_eq(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_bind_state_if_eq(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_bind_state_if_not_eq(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_calculate_ext_draw_size(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_calculate_style_text_align(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_center(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_check_type(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_class_create_obj(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_class_init_obj(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_clean(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_create(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_delete(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_delete_anim_completed_cb(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_delete_async(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_delete_delayed(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_dump_tree(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_enable_style_refresh(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_event_base(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_fade_in(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_fade_out(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_child(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_child_by_type(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_child_count(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_child_count_by_type(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_class(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_click_area(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_content_coords(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_content_height(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_content_width(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_coords(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_display(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_event_count(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_event_dsc(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_group(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_height(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_index(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_index_by_type(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_parent(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_screen(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_scroll_bottom(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_scroll_dir(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_scroll_end(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_scroll_left(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_scroll_right(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_scroll_snap_x(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_scroll_snap_y(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_scroll_top(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_scroll_x(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_scroll_y(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_scrollbar_area(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_scrollbar_mode(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_self_height(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_self_width(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_sibling(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_sibling_by_type(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_state(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_align(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_anim(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_anim_duration(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_arc_color(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_arc_color_filtered(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_arc_image_src(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_arc_opa(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_arc_rounded(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_arc_width(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_base_dir(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_bg_color(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_bg_color_filtered(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_bg_grad(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_bg_grad_color(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_bg_grad_color_filtered(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_bg_grad_dir(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_bg_grad_opa(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_bg_grad_stop(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_bg_image_opa(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_bg_image_recolor(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_bg_image_recolor_filtered(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_bg_image_recolor_opa(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_bg_image_src(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_bg_image_tiled(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_bg_main_opa(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_bg_main_stop(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_bg_opa(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_bitmap_mask_src(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_blend_mode(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_border_color(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_border_color_filtered(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_border_opa(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_border_post(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_border_side(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_border_width(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_clip_corner(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_color_filter_dsc(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_color_filter_opa(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_flex_cross_place(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_flex_flow(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_flex_grow(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_flex_main_place(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_flex_track_place(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_grid_cell_column_pos(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_grid_cell_column_span(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_grid_cell_row_pos(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_grid_cell_row_span(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_grid_cell_x_align(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_grid_cell_y_align(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_grid_column_align(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_grid_column_dsc_array(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_grid_row_align(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_grid_row_dsc_array(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_height(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_image_opa(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_image_recolor(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_image_recolor_filtered(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_image_recolor_opa(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_layout(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_length(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_line_color(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_line_color_filtered(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_line_dash_gap(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_line_dash_width(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_line_opa(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_line_rounded(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_line_width(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_margin_bottom(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_margin_left(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_margin_right(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_margin_top(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_max_height(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_max_width(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_min_height(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_min_width(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_opa(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_opa_layered(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_opa_recursive(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_outline_color(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_outline_color_filtered(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_outline_opa(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_outline_pad(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_outline_width(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_pad_bottom(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_pad_column(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_pad_left(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_pad_right(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_pad_row(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_pad_top(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_radius(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_rotary_sensitivity(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_shadow_color(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_shadow_color_filtered(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_shadow_offset_x(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_shadow_offset_y(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_shadow_opa(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_shadow_spread(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_shadow_width(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_space_bottom(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_space_left(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_space_right(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_space_top(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_text_align(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_text_color(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_text_color_filtered(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_text_decor(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_text_font(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_text_letter_space(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_text_line_space(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_text_opa(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_transform_height(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_transform_pivot_x(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_transform_pivot_y(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_transform_rotation(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_transform_scale_x(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_transform_scale_x_safe(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_transform_scale_y(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_transform_scale_y_safe(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_transform_skew_x(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_transform_skew_y(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_transform_width(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_transition(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_translate_x(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_translate_y(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_width(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_x(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_style_y(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_transformed_area(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_user_data(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_width(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_x(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_x2(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_x_aligned(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_y(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_y2(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_get_y_aligned(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_has_class(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_has_flag(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_has_flag_any(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_has_state(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_hit_test(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_init_draw_arc_dsc(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_init_draw_image_dsc(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_init_draw_label_dsc(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_init_draw_line_dsc(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_init_draw_rect_dsc(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_invalidate(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_invalidate_area(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_is_editable(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_is_group_def(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_is_layout_positioned(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_is_scrolling(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_is_valid(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_is_visible(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_mark_layout_as_dirty(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_move_background(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_move_children_by(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_move_foreground(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_move_to(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_move_to_index(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_null_on_delete(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_readjust_scroll(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_redraw(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_refr_pos(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_refr_size(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_refresh_ext_draw_size(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_refresh_self_size(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_refresh_style(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_remove_event(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_remove_event_cb(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_remove_event_cb_with_user_data(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_remove_event_dsc(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_remove_flag(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_remove_from_subject(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_remove_state(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_remove_style(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_remove_style_all(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_replace_style(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_report_style_change(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_scroll_by(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_scroll_by_bounded(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_scroll_to(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_scroll_to_view(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_scroll_to_view_recursive(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_scroll_to_x(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_scroll_to_y(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_scrollbar_invalidate(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_send_event(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_align(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_content_height(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_content_width(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_ext_click_area(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_flex_align(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_flex_flow(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_flex_grow(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_grid_align(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_grid_cell(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_height(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_layout(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_parent(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_pos(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_scroll_dir(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_scroll_snap_x(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_scroll_snap_y(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_scrollbar_mode(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_size(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_state(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_align(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_anim(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_anim_duration(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_arc_color(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_arc_image_src(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_arc_opa(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_arc_rounded(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_arc_width(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_base_dir(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_bg_color(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_bg_grad(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_bg_grad_color(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_bg_grad_dir(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_bg_grad_opa(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_bg_grad_stop(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_bg_image_opa(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_bg_image_recolor(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_bg_image_recolor_opa(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_bg_image_src(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_bg_image_tiled(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_bg_main_opa(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_bg_main_stop(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_bg_opa(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_bitmap_mask_src(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_blend_mode(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_border_color(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_border_opa(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_border_post(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_border_side(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_border_width(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_clip_corner(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_color_filter_dsc(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_color_filter_opa(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_flex_cross_place(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_flex_flow(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_flex_grow(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_flex_main_place(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_flex_track_place(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_grid_cell_column_pos(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_grid_cell_column_span(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_grid_cell_row_pos(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_grid_cell_row_span(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_grid_cell_x_align(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_grid_cell_y_align(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_grid_column_align(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_grid_column_dsc_array(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_grid_row_align(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_grid_row_dsc_array(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_height(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_image_opa(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_image_recolor(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_image_recolor_opa(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_layout(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_length(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_line_color(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_line_dash_gap(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_line_dash_width(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_line_opa(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_line_rounded(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_line_width(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_margin_all(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_margin_bottom(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_margin_hor(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_margin_left(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_margin_right(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_margin_top(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_margin_ver(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_max_height(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_max_width(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_min_height(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_min_width(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_opa(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_opa_layered(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_outline_color(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_outline_opa(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_outline_pad(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_outline_width(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_pad_all(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_pad_bottom(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_pad_column(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_pad_gap(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_pad_hor(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_pad_left(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_pad_right(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_pad_row(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_pad_top(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_pad_ver(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_radius(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_rotary_sensitivity(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_shadow_color(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_shadow_offset_x(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_shadow_offset_y(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_shadow_opa(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_shadow_spread(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_shadow_width(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_size(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_text_align(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_text_color(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_text_decor(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_text_font(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_text_letter_space(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_text_line_space(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_text_opa(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_transform_height(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_transform_pivot_x(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_transform_pivot_y(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_transform_rotation(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_transform_scale(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_transform_scale_x(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_transform_scale_y(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_transform_skew_x(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_transform_skew_y(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_transform_width(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_transition(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_translate_x(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_translate_y(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_width(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_x(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_style_y(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_user_data(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_width(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_x(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_set_y(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_style_get_selector_part(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_style_get_selector_state(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_swap(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_transform_point(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_tree_walk(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_update_flag(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_update_layout(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_obj_update_snap(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_palette_darken(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_palette_lighten(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_palette_main(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_screen_active(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_screen_load(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_screen_load_anim(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_slider_bind_value(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_slider_create(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_slider_get_left_value(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_slider_get_max_value(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_slider_get_min_value(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_slider_get_mode(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_slider_get_value(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_slider_is_dragged(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_slider_is_symmetrical(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_slider_set_left_value(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_slider_set_mode(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_slider_set_range(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_slider_set_value(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_switch_create(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_tileview_set_tile(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_timer_create(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_timer_create_basic(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_timer_delete(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_timer_get_paused(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_timer_pause(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_timer_ready(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_timer_reset(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_timer_resume(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_timer_set_auto_delete(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_timer_set_period(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_timer_set_repeat_count(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_trigo_cos(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);
static jerry_value_t js_lv_trigo_sin(const jerry_call_info_t*, const jerry_value_t*, jerry_length_t);

// 函数实现

/**
 * @brief Align an object to the current position of the arc (knob)
 */
static jerry_value_t js_lv_arc_align_obj_to_angle(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_arc_align_obj_to_angle: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: obj_to_align (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj_to_align = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_OBJ);
    // 解析参数: r_offset (int32_t)
    jerry_value_t js_arg_r_offset = args[2];
    if (!jerry_value_is_number(js_arg_r_offset)) {
        return script_engine_throw_error("lv_arc_align_obj_to_angle: Argument 2 must be a number");
    }

    int32_t arg_r_offset = (int32_t)jerry_value_as_number(js_arg_r_offset);

    // 调用底层函数
    lv_arc_align_obj_to_angle(arg_obj, arg_obj_to_align, arg_r_offset);

    return jerry_undefined();
}



/**
 * @brief Bind an integer subject to an arc's value pointer to the created observer
 */
static jerry_value_t js_lv_arc_bind_value(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_arc_bind_value: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: subject (lv_subject_t*)
    // 指针类型参数，支持null
    void* arg_subject = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 调用底层函数
    lv_observer_t* ret_value = lv_arc_bind_value(arg_obj, arg_subject);

    // 处理返回值
    jerry_value_t js_result;
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

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
        return script_engine_throw_error("lv_arc_create: Insufficient arguments");
    }

    // 解析参数: parent (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_parent = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_t* ret_value = lv_arc_create(arg_parent);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_OBJ);

    return js_result;
}



/**
 * @brief Get the end angle of an arc. the end angle [0..360] (if LV_USE_FLOAT is enabled it can be fractional too.)
 */
static jerry_value_t js_lv_arc_get_angle_end(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_arc_get_angle_end: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_value_precise_t ret_value = lv_arc_get_angle_end(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the start angle of an arc. the start angle [0..360] (if LV_USE_FLOAT is enabled it can be fractional too.)
 */
static jerry_value_t js_lv_arc_get_angle_start(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_arc_get_angle_start: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_value_precise_t ret_value = lv_arc_get_angle_start(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the end angle of an arc background. the end angle [0..360] (if LV_USE_FLOAT is enabled it can be fractional too.)
 */
static jerry_value_t js_lv_arc_get_bg_angle_end(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_arc_get_bg_angle_end: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_value_precise_t ret_value = lv_arc_get_bg_angle_end(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the start angle of an arc background. the start angle [0..360] (if LV_USE_FLOAT is enabled it can be fractional too.)
 */
static jerry_value_t js_lv_arc_get_bg_angle_start(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_arc_get_bg_angle_start: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_value_precise_t ret_value = lv_arc_get_bg_angle_start(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the current knob angle offset arc's current knob offset
 */
static jerry_value_t js_lv_arc_get_knob_offset(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_arc_get_knob_offset: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_arc_get_knob_offset(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the maximum value of an arc the maximum value of the arc
 */
static jerry_value_t js_lv_arc_get_max_value(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_arc_get_max_value: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_arc_get_max_value(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the minimum value of an arc the minimum value of the arc
 */
static jerry_value_t js_lv_arc_get_min_value(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_arc_get_min_value: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_arc_get_min_value(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get whether the arc is type or not. arc's mode
 */
static jerry_value_t js_lv_arc_get_mode(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_arc_get_mode: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_arc_mode_t ret_value = lv_arc_get_mode(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the rotation for the whole arc arc's current rotation
 */
static jerry_value_t js_lv_arc_get_rotation(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_arc_get_rotation: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_arc_get_rotation(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the value of an arc the value of the arc
 */
static jerry_value_t js_lv_arc_get_value(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_arc_get_value: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_arc_get_value(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Rotate an object to the current position of the arc (knob)
 */
static jerry_value_t js_lv_arc_rotate_obj_to_angle(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_arc_rotate_obj_to_angle: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: obj_to_rotate (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj_to_rotate = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_OBJ);
    // 解析参数: r_offset (int32_t)
    jerry_value_t js_arg_r_offset = args[2];
    if (!jerry_value_is_number(js_arg_r_offset)) {
        return script_engine_throw_error("lv_arc_rotate_obj_to_angle: Argument 2 must be a number");
    }

    int32_t arg_r_offset = (int32_t)jerry_value_as_number(js_arg_r_offset);

    // 调用底层函数
    lv_arc_rotate_obj_to_angle(arg_obj, arg_obj_to_rotate, arg_r_offset);

    return jerry_undefined();
}



/**
 * @brief Set the start and end angles
 */
static jerry_value_t js_lv_arc_set_angles(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_arc_set_angles: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: start (lv_value_precise_t)
    jerry_value_t js_arg_start = args[1];
    if (!jerry_value_is_number(js_arg_start)) {
        return script_engine_throw_error("lv_arc_set_angles: Argument 1 must be a number");
    }

    float arg_start = (float)jerry_value_as_number(js_arg_start);

    // 解析参数: end (lv_value_precise_t)
    jerry_value_t js_arg_end = args[2];
    if (!jerry_value_is_number(js_arg_end)) {
        return script_engine_throw_error("lv_arc_set_angles: Argument 2 must be a number");
    }

    float arg_end = (float)jerry_value_as_number(js_arg_end);

    // 调用底层函数
    lv_arc_set_angles(arg_obj, arg_start, arg_end);

    return jerry_undefined();
}



/**
 * @brief Set the start and end angles of the arc background
 */
static jerry_value_t js_lv_arc_set_bg_angles(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_arc_set_bg_angles: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: start (lv_value_precise_t)
    jerry_value_t js_arg_start = args[1];
    if (!jerry_value_is_number(js_arg_start)) {
        return script_engine_throw_error("lv_arc_set_bg_angles: Argument 1 must be a number");
    }

    float arg_start = (float)jerry_value_as_number(js_arg_start);

    // 解析参数: end (lv_value_precise_t)
    jerry_value_t js_arg_end = args[2];
    if (!jerry_value_is_number(js_arg_end)) {
        return script_engine_throw_error("lv_arc_set_bg_angles: Argument 2 must be a number");
    }

    float arg_end = (float)jerry_value_as_number(js_arg_end);

    // 调用底层函数
    lv_arc_set_bg_angles(arg_obj, arg_start, arg_end);

    return jerry_undefined();
}



/**
 * @brief Set the start angle of an arc background. 0 deg: right, 90 bottom etc.
 */
static jerry_value_t js_lv_arc_set_bg_end_angle(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_arc_set_bg_end_angle: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: end (lv_value_precise_t)
    jerry_value_t js_arg_end = args[1];
    if (!jerry_value_is_number(js_arg_end)) {
        return script_engine_throw_error("lv_arc_set_bg_end_angle: Argument 1 must be a number");
    }

    float arg_end = (float)jerry_value_as_number(js_arg_end);

    // 调用底层函数
    lv_arc_set_bg_end_angle(arg_obj, arg_end);

    return jerry_undefined();
}



/**
 * @brief Set the start angle of an arc background. 0 deg: right, 90 bottom, etc.
 */
static jerry_value_t js_lv_arc_set_bg_start_angle(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_arc_set_bg_start_angle: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: start (lv_value_precise_t)
    jerry_value_t js_arg_start = args[1];
    if (!jerry_value_is_number(js_arg_start)) {
        return script_engine_throw_error("lv_arc_set_bg_start_angle: Argument 1 must be a number");
    }

    float arg_start = (float)jerry_value_as_number(js_arg_start);

    // 调用底层函数
    lv_arc_set_bg_start_angle(arg_obj, arg_start);

    return jerry_undefined();
}



/**
 * @brief Set a change rate to limit the speed how fast the arc should reach the pressed point.
 */
static jerry_value_t js_lv_arc_set_change_rate(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_arc_set_change_rate: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: rate (uint32_t)
    jerry_value_t js_arg_rate = args[1];
    if (!jerry_value_is_number(js_arg_rate)) {
        return script_engine_throw_error("lv_arc_set_change_rate: Argument 1 must be a number");
    }

    uint32_t arg_rate = (uint32_t)jerry_value_as_number(js_arg_rate);

    // 调用底层函数
    lv_arc_set_change_rate(arg_obj, arg_rate);

    return jerry_undefined();
}



/**
 * @brief Set the end angle of an arc. 0 deg: right, 90 bottom, etc.
 */
static jerry_value_t js_lv_arc_set_end_angle(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_arc_set_end_angle: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: end (lv_value_precise_t)
    jerry_value_t js_arg_end = args[1];
    if (!jerry_value_is_number(js_arg_end)) {
        return script_engine_throw_error("lv_arc_set_end_angle: Argument 1 must be a number");
    }

    float arg_end = (float)jerry_value_as_number(js_arg_end);

    // 调用底层函数
    lv_arc_set_end_angle(arg_obj, arg_end);

    return jerry_undefined();
}



/**
 * @brief Set an offset angle for the knob
 */
static jerry_value_t js_lv_arc_set_knob_offset(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_arc_set_knob_offset: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: offset (int32_t)
    jerry_value_t js_arg_offset = args[1];
    if (!jerry_value_is_number(js_arg_offset)) {
        return script_engine_throw_error("lv_arc_set_knob_offset: Argument 1 must be a number");
    }

    int32_t arg_offset = (int32_t)jerry_value_as_number(js_arg_offset);

    // 调用底层函数
    lv_arc_set_knob_offset(arg_obj, arg_offset);

    return jerry_undefined();
}



/**
 * @brief Set the type of arc.
 */
static jerry_value_t js_lv_arc_set_mode(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_arc_set_mode: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: type (lv_arc_mode_t)
    jerry_value_t js_arg_type = args[1];
    if (!jerry_value_is_number(js_arg_type)) {
        return script_engine_throw_error("lv_arc_set_mode: Argument 1 must be a number");
    }

    int arg_type = (int)jerry_value_as_number(js_arg_type);

    // 调用底层函数
    lv_arc_set_mode(arg_obj, arg_type);

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
        return script_engine_throw_error("lv_arc_set_range: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: min (int32_t)
    jerry_value_t js_arg_min = args[1];
    if (!jerry_value_is_number(js_arg_min)) {
        return script_engine_throw_error("lv_arc_set_range: Argument 1 must be a number");
    }

    int32_t arg_min = (int32_t)jerry_value_as_number(js_arg_min);

    // 解析参数: max (int32_t)
    jerry_value_t js_arg_max = args[2];
    if (!jerry_value_is_number(js_arg_max)) {
        return script_engine_throw_error("lv_arc_set_range: Argument 2 must be a number");
    }

    int32_t arg_max = (int32_t)jerry_value_as_number(js_arg_max);

    // 调用底层函数
    lv_arc_set_range(arg_obj, arg_min, arg_max);

    return jerry_undefined();
}



/**
 * @brief Set the rotation for the whole arc
 */
static jerry_value_t js_lv_arc_set_rotation(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_arc_set_rotation: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: rotation (int32_t)
    jerry_value_t js_arg_rotation = args[1];
    if (!jerry_value_is_number(js_arg_rotation)) {
        return script_engine_throw_error("lv_arc_set_rotation: Argument 1 must be a number");
    }

    int32_t arg_rotation = (int32_t)jerry_value_as_number(js_arg_rotation);

    // 调用底层函数
    lv_arc_set_rotation(arg_obj, arg_rotation);

    return jerry_undefined();
}



/**
 * @brief Set the start angle of an arc. 0 deg: right, 90 bottom, etc.
 */
static jerry_value_t js_lv_arc_set_start_angle(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_arc_set_start_angle: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: start (lv_value_precise_t)
    jerry_value_t js_arg_start = args[1];
    if (!jerry_value_is_number(js_arg_start)) {
        return script_engine_throw_error("lv_arc_set_start_angle: Argument 1 must be a number");
    }

    float arg_start = (float)jerry_value_as_number(js_arg_start);

    // 调用底层函数
    lv_arc_set_start_angle(arg_obj, arg_start);

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
        return script_engine_throw_error("lv_arc_set_value: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_arc_set_value: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 调用底层函数
    lv_arc_set_value(arg_obj, arg_value);

    return jerry_undefined();
}



/**
 * @brief Create a bar object pointer to the created bar
 */
static jerry_value_t js_lv_bar_create(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_bar_create: Insufficient arguments");
    }

    // 解析参数: parent (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_parent = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_t* ret_value = lv_bar_create(arg_parent);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_OBJ);

    return js_result;
}



/**
 * @brief Get the maximum value of a bar the maximum value of the bar
 */
static jerry_value_t js_lv_bar_get_max_value(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_bar_get_max_value: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_bar_get_max_value(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the minimum value of a bar the minimum value of the bar
 */
static jerry_value_t js_lv_bar_get_min_value(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_bar_get_min_value: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_bar_get_min_value(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the type of bar. bar type from ::lv_bar_mode_t
 */
static jerry_value_t js_lv_bar_get_mode(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_bar_get_mode: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_bar_mode_t ret_value = lv_bar_get_mode(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the orientation of bar. bar orientation from ::lv_bar_orientation_t
 */
static jerry_value_t js_lv_bar_get_orientation(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_bar_get_orientation: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_bar_orientation_t ret_value = lv_bar_get_orientation(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the start value of a bar the start value of the bar
 */
static jerry_value_t js_lv_bar_get_start_value(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_bar_get_start_value: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_bar_get_start_value(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the value of a bar the value of the bar
 */
static jerry_value_t js_lv_bar_get_value(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_bar_get_value: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_bar_get_value(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Give the bar is in symmetrical mode or not true: in symmetrical mode false : not in
 */
static jerry_value_t js_lv_bar_is_symmetrical(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_bar_is_symmetrical: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    bool ret_value = lv_bar_is_symmetrical(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Set the type of bar.
 */
static jerry_value_t js_lv_bar_set_mode(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_bar_set_mode: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: mode (lv_bar_mode_t)
    jerry_value_t js_arg_mode = args[1];
    if (!jerry_value_is_number(js_arg_mode)) {
        return script_engine_throw_error("lv_bar_set_mode: Argument 1 must be a number");
    }

    int arg_mode = (int)jerry_value_as_number(js_arg_mode);

    // 调用底层函数
    lv_bar_set_mode(arg_obj, arg_mode);

    return jerry_undefined();
}



/**
 * @brief Set the orientation of bar.
 */
static jerry_value_t js_lv_bar_set_orientation(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_bar_set_orientation: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: orientation (lv_bar_orientation_t)
    jerry_value_t js_arg_orientation = args[1];
    if (!jerry_value_is_number(js_arg_orientation)) {
        return script_engine_throw_error("lv_bar_set_orientation: Argument 1 must be a number");
    }

    int arg_orientation = (int)jerry_value_as_number(js_arg_orientation);

    // 调用底层函数
    lv_bar_set_orientation(arg_obj, arg_orientation);

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
        return script_engine_throw_error("lv_bar_set_range: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: min (int32_t)
    jerry_value_t js_arg_min = args[1];
    if (!jerry_value_is_number(js_arg_min)) {
        return script_engine_throw_error("lv_bar_set_range: Argument 1 must be a number");
    }

    int32_t arg_min = (int32_t)jerry_value_as_number(js_arg_min);

    // 解析参数: max (int32_t)
    jerry_value_t js_arg_max = args[2];
    if (!jerry_value_is_number(js_arg_max)) {
        return script_engine_throw_error("lv_bar_set_range: Argument 2 must be a number");
    }

    int32_t arg_max = (int32_t)jerry_value_as_number(js_arg_max);

    // 调用底层函数
    lv_bar_set_range(arg_obj, arg_min, arg_max);

    return jerry_undefined();
}



/**
 * @brief Set a new start value on the bar
 */
static jerry_value_t js_lv_bar_set_start_value(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_bar_set_start_value: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: start_value (int32_t)
    jerry_value_t js_arg_start_value = args[1];
    if (!jerry_value_is_number(js_arg_start_value)) {
        return script_engine_throw_error("lv_bar_set_start_value: Argument 1 must be a number");
    }

    int32_t arg_start_value = (int32_t)jerry_value_as_number(js_arg_start_value);

    // 解析参数: anim (lv_anim_enable_t)
    jerry_value_t js_arg_anim = args[2];
    if (!jerry_value_is_number(js_arg_anim)) {
        return script_engine_throw_error("lv_bar_set_start_value: Argument 2 must be a number");
    }

    int arg_anim = (int)jerry_value_as_number(js_arg_anim);

    // 调用底层函数
    lv_bar_set_start_value(arg_obj, arg_start_value, arg_anim);

    return jerry_undefined();
}



/**
 * @brief Set a new value on the bar
 */
static jerry_value_t js_lv_bar_set_value(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_bar_set_value: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_bar_set_value: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: anim (lv_anim_enable_t)
    jerry_value_t js_arg_anim = args[2];
    if (!jerry_value_is_number(js_arg_anim)) {
        return script_engine_throw_error("lv_bar_set_value: Argument 2 must be a number");
    }

    int arg_anim = (int)jerry_value_as_number(js_arg_anim);

    // 调用底层函数
    lv_bar_set_value(arg_obj, arg_value, arg_anim);

    return jerry_undefined();
}



/**
 * @brief Close the pending decoding. Free resources etc.
 */
static jerry_value_t js_lv_bin_decoder_close(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_bin_decoder_close: Insufficient arguments");
    }

    // 解析参数: decoder (lv_image_decoder_t*)
    // 指针类型参数，支持null
    void* arg_decoder = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_ANY);
    // 解析参数: dsc (lv_image_decoder_dsc_t*)
    // 指针类型参数，支持null
    void* arg_dsc = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 调用底层函数
    lv_bin_decoder_close(arg_decoder, arg_dsc);

    return jerry_undefined();
}



/**
 * @brief Open a lvgl binary image LV_RESULT_OK: the info is successfully stored in header ; LV_RESULT_INVALID: unknown format or other error.
 */
static jerry_value_t js_lv_bin_decoder_open(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_bin_decoder_open: Insufficient arguments");
    }

    // 解析参数: decoder (lv_image_decoder_t*)
    // 指针类型参数，支持null
    void* arg_decoder = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_ANY);
    // 解析参数: dsc (lv_image_decoder_dsc_t*)
    // 指针类型参数，支持null
    void* arg_dsc = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 调用底层函数
    lv_result_t ret_value = lv_bin_decoder_open(arg_decoder, arg_dsc);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Create a button object pointer to the created button
 */
static jerry_value_t js_lv_button_create(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_button_create: Insufficient arguments");
    }

    // 解析参数: parent (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_parent = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_t* ret_value = lv_button_create(arg_parent);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_OBJ);

    return js_result;
}



/**
 * @brief Create a check box object pointer to the created check box
 */
static jerry_value_t js_lv_checkbox_create(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_checkbox_create: Insufficient arguments");
    }

    // 解析参数: parent (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_parent = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_t* ret_value = lv_checkbox_create(arg_parent);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_OBJ);

    return js_result;
}



/**
 * @brief Get the text of a check box pointer to the text of the check box
 */
static jerry_value_t js_lv_checkbox_get_text(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_checkbox_get_text: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    const char* ret_value = lv_checkbox_get_text(arg_obj);

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
 * @brief Set the text of a check box. txt will be copied and may be deallocated after this function returns.
 */
static jerry_value_t js_lv_checkbox_set_text(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_checkbox_set_text: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: txt (const char*)

    char* arg_txt_str = NULL;
    const char* arg_txt = NULL;
    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1])) {
        if (!jerry_value_is_string(args[1])) {
            return script_engine_throw_error("lv_checkbox_set_text: Argument 1 must be a string");
        }
        jerry_size_t arg_txt_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
        arg_txt_str = (char*)eos_malloc(arg_txt_len + 1);
        jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_txt_str, arg_txt_len);
        arg_txt_str[arg_txt_len] = '\0';
        arg_txt = arg_txt_str;
    }

    // 调用底层函数
    lv_checkbox_set_text(arg_obj, arg_txt);

    // 释放临时内存
    if (arg_txt_str) eos_free(arg_txt_str);

    return jerry_undefined();
}



/**
 * @brief Set the text of a check box. txt must not be deallocated during the life of this checkbox.
 */
static jerry_value_t js_lv_checkbox_set_text_static(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_checkbox_set_text_static: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: txt (const char*)

    char* arg_txt_str = NULL;
    const char* arg_txt = NULL;
    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1])) {
        if (!jerry_value_is_string(args[1])) {
            return script_engine_throw_error("lv_checkbox_set_text_static: Argument 1 must be a string");
        }
        jerry_size_t arg_txt_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
        arg_txt_str = (char*)eos_malloc(arg_txt_len + 1);
        jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_txt_str, arg_txt_len);
        arg_txt_str[arg_txt_len] = '\0';
        arg_txt = arg_txt_str;
    }

    // 调用底层函数
    lv_checkbox_set_text_static(arg_obj, arg_txt);

    // 释放临时内存
    if (arg_txt_str) eos_free(arg_txt_str);

    return jerry_undefined();
}



/**
 * @brief Create a color from 0x000000..0xffffff input the color
 */
static jerry_value_t js_lv_color_hex(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_color_hex: Insufficient arguments");
    }

    // 解析参数: c (uint32_t)
    jerry_value_t js_arg_c = args[0];
    if (!jerry_value_is_number(js_arg_c)) {
        return script_engine_throw_error("lv_color_hex: Argument 0 must be a number");
    }

    uint32_t arg_c = (uint32_t)jerry_value_as_number(js_arg_c);

    // 调用底层函数
    lv_color_t ret_value = lv_color_hex(arg_c);

    // 处理返回值
    jerry_value_t js_result;
    // 转换为JS颜色对象
    js_result = lv_js_bridge_color_2_obj(&ret_value);

    return js_result;
}



/**
 * @brief Delay for the given milliseconds. By default it's a blocking delay, but with :ref:`lv_delay_set_cb()` a custom delay function can be set too
 */
static jerry_value_t js_lv_delay_ms(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_delay_ms: Insufficient arguments");
    }

    // 解析参数: ms (uint32_t)
    jerry_value_t js_arg_ms = args[0];
    if (!jerry_value_is_number(js_arg_ms)) {
        return script_engine_throw_error("lv_delay_ms: Argument 0 must be a number");
    }

    uint32_t arg_ms = (uint32_t)jerry_value_as_number(js_arg_ms);

    // 调用底层函数
    lv_delay_ms(arg_ms);

    return jerry_undefined();
}



/**
 * @brief Return a pointer to the active screen on a display pointer to the active screen object (loaded by ' :ref:`lv_screen_load()` ')
 */
static jerry_value_t js_lv_display_get_screen_active(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_display_get_screen_active: Insufficient arguments");
    }

    // 解析参数: disp (lv_display_t*)
    // 指针类型参数，支持null
    void* arg_disp = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_ANY);
    // 调用底层函数
    lv_obj_t* ret_value = lv_display_get_screen_active(arg_disp);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_OBJ);

    return js_result;
}



/**
 * @brief Add an options to a drop-down list from a string. Only works for non-static options.
 */
static jerry_value_t js_lv_dropdown_add_option(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_dropdown_add_option: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: option (const char*)

    char* arg_option_str = NULL;
    const char* arg_option = NULL;
    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1])) {
        if (!jerry_value_is_string(args[1])) {
            return script_engine_throw_error("lv_dropdown_add_option: Argument 1 must be a string");
        }
        jerry_size_t arg_option_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
        arg_option_str = (char*)eos_malloc(arg_option_len + 1);
        jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_option_str, arg_option_len);
        arg_option_str[arg_option_len] = '\0';
        arg_option = arg_option_str;
    }

    // 解析参数: pos (uint32_t)
    jerry_value_t js_arg_pos = args[2];
    if (!jerry_value_is_number(js_arg_pos)) {
        return script_engine_throw_error("lv_dropdown_add_option: Argument 2 must be a number");
    }

    uint32_t arg_pos = (uint32_t)jerry_value_as_number(js_arg_pos);

    // 调用底层函数
    lv_dropdown_add_option(arg_obj, arg_option, arg_pos);

    // 释放临时内存
    if (arg_option_str) eos_free(arg_option_str);

    return jerry_undefined();
}



/**
 * @brief Bind an integer subject to a dropdown's value pointer to the created observer
 */
static jerry_value_t js_lv_dropdown_bind_value(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_dropdown_bind_value: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: subject (lv_subject_t*)
    // 指针类型参数，支持null
    void* arg_subject = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 调用底层函数
    lv_observer_t* ret_value = lv_dropdown_bind_value(arg_obj, arg_subject);

    // 处理返回值
    jerry_value_t js_result;
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

    return js_result;
}



/**
 * @brief Clear all options in a drop-down list. Works with both static and dynamic options.
 */
static jerry_value_t js_lv_dropdown_clear_options(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_dropdown_clear_options: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_dropdown_clear_options(arg_obj);

    return jerry_undefined();
}



/**
 * @brief Close (Collapse) the drop-down list
 */
static jerry_value_t js_lv_dropdown_close(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_dropdown_close: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_dropdown_close(arg_obj);

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
        return script_engine_throw_error("lv_dropdown_create: Insufficient arguments");
    }

    // 解析参数: parent (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_parent = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_t* ret_value = lv_dropdown_create(arg_parent);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_OBJ);

    return js_result;
}



/**
 * @brief Get the direction of the drop-down list LV_DIR_LEF/RIGHT/TOP/BOTTOM
 */
static jerry_value_t js_lv_dropdown_get_dir(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_dropdown_get_dir: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_dir_t ret_value = lv_dropdown_get_dir(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the list of a drop-down to allow styling or other modifications pointer to the list of the drop-down
 */
static jerry_value_t js_lv_dropdown_get_list(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_dropdown_get_list: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_t* ret_value = lv_dropdown_get_list(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_OBJ);

    return js_result;
}



/**
 * @brief Get the total number of options the total number of options in the list
 */
static jerry_value_t js_lv_dropdown_get_option_count(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_dropdown_get_option_count: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    uint32_t ret_value = lv_dropdown_get_option_count(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the index of an option. index of option in the list of all options. -1 if not found.
 */
static jerry_value_t js_lv_dropdown_get_option_index(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_dropdown_get_option_index: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: option (const char*)

    char* arg_option_str = NULL;
    const char* arg_option = NULL;
    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1])) {
        if (!jerry_value_is_string(args[1])) {
            return script_engine_throw_error("lv_dropdown_get_option_index: Argument 1 must be a string");
        }
        jerry_size_t arg_option_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
        arg_option_str = (char*)eos_malloc(arg_option_len + 1);
        jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_option_str, arg_option_len);
        arg_option_str[arg_option_len] = '\0';
        arg_option = arg_option_str;
    }

    // 调用底层函数
    int32_t ret_value = lv_dropdown_get_option_index(arg_obj, arg_option);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    // 释放临时内存
    if (arg_option_str) eos_free(arg_option_str);

    return js_result;
}



/**
 * @brief Get the options of a drop-down list the options separated by ' '-s (E.g. "Option1\nOption2\nOption3")
 */
static jerry_value_t js_lv_dropdown_get_options(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_dropdown_get_options: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    const char* ret_value = lv_dropdown_get_options(arg_obj);

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
 * @brief Get the index of the selected option index of the selected option (0 ... number of option - 1);
 */
static jerry_value_t js_lv_dropdown_get_selected(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_dropdown_get_selected: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    uint32_t ret_value = lv_dropdown_get_selected(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get whether the selected option in the list should be highlighted or not true: highlight enabled; false: disabled
 */
static jerry_value_t js_lv_dropdown_get_selected_highlight(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_dropdown_get_selected_highlight: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    bool ret_value = lv_dropdown_get_selected_highlight(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the current selected option as a string
 */
static jerry_value_t js_lv_dropdown_get_selected_str(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_dropdown_get_selected_str: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: buf (char*)

    char* arg_buf_str = NULL;
    const char* arg_buf = NULL;
    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1])) {
        if (!jerry_value_is_string(args[1])) {
            return script_engine_throw_error("lv_dropdown_get_selected_str: Argument 1 must be a string");
        }
        jerry_size_t arg_buf_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
        arg_buf_str = (char*)eos_malloc(arg_buf_len + 1);
        jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_buf_str, arg_buf_len);
        arg_buf_str[arg_buf_len] = '\0';
        arg_buf = arg_buf_str;
    }

    // 解析参数: buf_size (uint32_t)
    jerry_value_t js_arg_buf_size = args[2];
    if (!jerry_value_is_number(js_arg_buf_size)) {
        return script_engine_throw_error("lv_dropdown_get_selected_str: Argument 2 must be a number");
    }

    uint32_t arg_buf_size = (uint32_t)jerry_value_as_number(js_arg_buf_size);

    // 调用底层函数
    lv_dropdown_get_selected_str(arg_obj, arg_buf, arg_buf_size);

    // 释放临时内存
    if (arg_buf_str) eos_free(arg_buf_str);

    return jerry_undefined();
}



/**
 * @brief Get the symbol on the drop-down list. Typically a down caret or arrow. the symbol or NULL if not enabled
 */
static jerry_value_t js_lv_dropdown_get_symbol(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_dropdown_get_symbol: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    const char* ret_value = lv_dropdown_get_symbol(arg_obj);

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
 * @brief Get text of the drop-down list's button. the text as string, NULL if no text
 */
static jerry_value_t js_lv_dropdown_get_text(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_dropdown_get_text: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    const char* ret_value = lv_dropdown_get_text(arg_obj);

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
 * @brief Tells whether the list is opened or not true if the list os opened
 */
static jerry_value_t js_lv_dropdown_is_open(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_dropdown_is_open: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    bool ret_value = lv_dropdown_is_open(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Open the drop.down list
 */
static jerry_value_t js_lv_dropdown_open(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_dropdown_open: Insufficient arguments");
    }

    // 解析参数: dropdown_obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_dropdown_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_dropdown_open(arg_dropdown_obj);

    return jerry_undefined();
}



/**
 * @brief Set the direction of the a drop-down list
 */
static jerry_value_t js_lv_dropdown_set_dir(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_dropdown_set_dir: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: dir (lv_dir_t)
    jerry_value_t js_arg_dir = args[1];
    if (!jerry_value_is_number(js_arg_dir)) {
        return script_engine_throw_error("lv_dropdown_set_dir: Argument 1 must be a number");
    }

    int arg_dir = (int)jerry_value_as_number(js_arg_dir);

    // 调用底层函数
    lv_dropdown_set_dir(arg_obj, arg_dir);

    return jerry_undefined();
}



/**
 * @brief Set the options in a drop-down list from a string. The options will be copied and saved in the object so the options can be destroyed after calling this function
 */
static jerry_value_t js_lv_dropdown_set_options(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_dropdown_set_options: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: options (const char*)

    char* arg_options_str = NULL;
    const char* arg_options = NULL;
    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1])) {
        if (!jerry_value_is_string(args[1])) {
            return script_engine_throw_error("lv_dropdown_set_options: Argument 1 must be a string");
        }
        jerry_size_t arg_options_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
        arg_options_str = (char*)eos_malloc(arg_options_len + 1);
        jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_options_str, arg_options_len);
        arg_options_str[arg_options_len] = '\0';
        arg_options = arg_options_str;
    }

    // 调用底层函数
    lv_dropdown_set_options(arg_obj, arg_options);

    // 释放临时内存
    if (arg_options_str) eos_free(arg_options_str);

    return jerry_undefined();
}



/**
 * @brief Set the options in a drop-down list from a static string (global, static or dynamically allocated). Only the pointer of the option string will be saved.
 */
static jerry_value_t js_lv_dropdown_set_options_static(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_dropdown_set_options_static: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: options (const char*)

    char* arg_options_str = NULL;
    const char* arg_options = NULL;
    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1])) {
        if (!jerry_value_is_string(args[1])) {
            return script_engine_throw_error("lv_dropdown_set_options_static: Argument 1 must be a string");
        }
        jerry_size_t arg_options_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
        arg_options_str = (char*)eos_malloc(arg_options_len + 1);
        jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_options_str, arg_options_len);
        arg_options_str[arg_options_len] = '\0';
        arg_options = arg_options_str;
    }

    // 调用底层函数
    lv_dropdown_set_options_static(arg_obj, arg_options);

    // 释放临时内存
    if (arg_options_str) eos_free(arg_options_str);

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
        return script_engine_throw_error("lv_dropdown_set_selected: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: sel_opt (uint32_t)
    jerry_value_t js_arg_sel_opt = args[1];
    if (!jerry_value_is_number(js_arg_sel_opt)) {
        return script_engine_throw_error("lv_dropdown_set_selected: Argument 1 must be a number");
    }

    uint32_t arg_sel_opt = (uint32_t)jerry_value_as_number(js_arg_sel_opt);

    // 调用底层函数
    lv_dropdown_set_selected(arg_obj, arg_sel_opt);

    return jerry_undefined();
}



/**
 * @brief Set whether the selected option in the list should be highlighted or not
 */
static jerry_value_t js_lv_dropdown_set_selected_highlight(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_dropdown_set_selected_highlight: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: en (bool)
    // 布尔类型参数: en
    bool arg_en = false;
    if (!jerry_value_is_undefined(args[1])) {
        if (jerry_value_is_boolean(args[1])) {
            arg_en = jerry_value_to_boolean(args[1]);
        }
        else if (jerry_value_is_number(args[1])) {
            arg_en = (jerry_value_as_number(args[1]) != 0);
        }
        else {
            return script_engine_throw_error("lv_dropdown_set_selected_highlight: Argument 1 must be boolean or number for bool");
        }
    }

    // 调用底层函数
    lv_dropdown_set_selected_highlight(arg_obj, arg_en);

    return jerry_undefined();
}



/**
 * @brief Set an arrow or other symbol to display when on drop-down list's button. Typically a down caret or arrow. angle and zoom transformation can be applied if the symbol is an image. E.g. when drop down is checked (opened) rotate the symbol by 180 degree
 */
static jerry_value_t js_lv_dropdown_set_symbol(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_dropdown_set_symbol: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: symbol (const void*)
    // void*/字符串 类型参数，支持null
    void* arg_symbol = NULL;
    char* arg_symbol_str = NULL;  // 用于字符串参数的临时存储

    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1])) {
        if (jerry_value_is_string(args[1])) {
            // 处理字符串类型的符号（如LV_SYMBOL_MINUS）
            jerry_size_t arg_symbol_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
            arg_symbol_str = (char*)eos_malloc(arg_symbol_len + 1);
            if (!arg_symbol_str) {
                return script_engine_throw_error("lv_dropdown_set_symbol: Failed to allocate memory for string argument");
            }
            jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_symbol_str, arg_symbol_len);
            arg_symbol_str[arg_symbol_len] = '\0';
            arg_symbol = (void*)arg_symbol_str;
        }
        else if (jerry_value_is_object(args[1])) {
            // 尝试从对象获取指针
            arg_symbol = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
        }
        else if (jerry_value_is_number(args[1])) {
            // 直接传递指针数值
            uintptr_t ptr_num = (uintptr_t)jerry_value_as_number(args[1]);
            arg_symbol = (void*)ptr_num;
        }
        else {
            return script_engine_throw_error("lv_dropdown_set_symbol: Argument 1 must be string, object or number");
        }
    }
    // 调用底层函数
    lv_dropdown_set_symbol(arg_obj, arg_symbol);

    // 释放临时内存
    if (arg_symbol_str) eos_free(arg_symbol_str);

    return jerry_undefined();
}



/**
 * @brief Set text of the drop-down list's button. If set to NULL the selected option's text will be displayed on the button. If set to a specific text then that text will be shown regardless of the selected option.
 */
static jerry_value_t js_lv_dropdown_set_text(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_dropdown_set_text: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: txt (const char*)

    char* arg_txt_str = NULL;
    const char* arg_txt = NULL;
    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1])) {
        if (!jerry_value_is_string(args[1])) {
            return script_engine_throw_error("lv_dropdown_set_text: Argument 1 must be a string");
        }
        jerry_size_t arg_txt_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
        arg_txt_str = (char*)eos_malloc(arg_txt_len + 1);
        jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_txt_str, arg_txt_len);
        arg_txt_str[arg_txt_len] = '\0';
        arg_txt = arg_txt_str;
    }

    // 调用底层函数
    lv_dropdown_set_text(arg_obj, arg_txt);

    // 释放临时内存
    if (arg_txt_str) eos_free(arg_txt_str);

    return jerry_undefined();
}



/**
 * @brief Deprecated Use lv_draw_buffer_create/destroy instead. Free the data pointer and dsc struct of an image.
 */
static jerry_value_t js_lv_image_buf_free(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_image_buf_free: Insufficient arguments");
    }

    // 解析参数: dsc (lv_image_dsc_t*)
    // 指针类型参数，支持null
    void* arg_dsc = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_ANY);
    // 调用底层函数
    lv_image_buf_free(arg_dsc);

    return jerry_undefined();
}



/**
 * @brief Invalidate image cache. Use NULL to invalidate all images.
 */
static jerry_value_t js_lv_image_cache_drop(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_image_cache_drop: Insufficient arguments");
    }

    // 解析参数: src (const void*)
    // void*/字符串 类型参数，支持null
    void* arg_src = NULL;
    char* arg_src_str = NULL;  // 用于字符串参数的临时存储

    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        if (jerry_value_is_string(args[0])) {
            // 处理字符串类型的符号（如LV_SYMBOL_MINUS）
            jerry_size_t arg_src_len = jerry_string_size(args[0], JERRY_ENCODING_UTF8);
            arg_src_str = (char*)eos_malloc(arg_src_len + 1);
            if (!arg_src_str) {
                return script_engine_throw_error("lv_image_cache_drop: Failed to allocate memory for string argument");
            }
            jerry_string_to_buffer(args[0], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_src_str, arg_src_len);
            arg_src_str[arg_src_len] = '\0';
            arg_src = (void*)arg_src_str;
        }
        else if (jerry_value_is_object(args[0])) {
            // 尝试从对象获取指针
            arg_src = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_ANY);
        }
        else if (jerry_value_is_number(args[0])) {
            // 直接传递指针数值
            uintptr_t ptr_num = (uintptr_t)jerry_value_as_number(args[0]);
            arg_src = (void*)ptr_num;
        }
        else {
            return script_engine_throw_error("lv_image_cache_drop: Argument 0 must be string, object or number");
        }
    }
    // 调用底层函数
    lv_image_cache_drop(arg_src);

    // 释放临时内存
    if (arg_src_str) eos_free(arg_src_str);

    return jerry_undefined();
}



/**
 * @brief Initialize image cache. LV_RESULT_OK: initialization succeeded, LV_RESULT_INVALID: failed.
 */
static jerry_value_t js_lv_image_cache_init(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_image_cache_init: Insufficient arguments");
    }

    // 解析参数: size (uint32_t)
    jerry_value_t js_arg_size = args[0];
    if (!jerry_value_is_number(js_arg_size)) {
        return script_engine_throw_error("lv_image_cache_init: Argument 0 must be a number");
    }

    uint32_t arg_size = (uint32_t)jerry_value_as_number(js_arg_size);

    // 调用底层函数
    lv_result_t ret_value = lv_image_cache_init(arg_size);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Return true if the image cache is enabled. true: enabled, false: disabled.
 */
static jerry_value_t js_lv_image_cache_is_enabled(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 调用底层函数
    bool ret_value = lv_image_cache_is_enabled();

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Resize image cache. If set to 0, the cache will be disabled.
 */
static jerry_value_t js_lv_image_cache_resize(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_image_cache_resize: Insufficient arguments");
    }

    // 解析参数: new_size (uint32_t)
    jerry_value_t js_arg_new_size = args[0];
    if (!jerry_value_is_number(js_arg_new_size)) {
        return script_engine_throw_error("lv_image_cache_resize: Argument 0 must be a number");
    }

    uint32_t arg_new_size = (uint32_t)jerry_value_as_number(js_arg_new_size);

    // 解析参数: evict_now (bool)
    // 布尔类型参数: evict_now
    bool arg_evict_now = false;
    if (!jerry_value_is_undefined(args[1])) {
        if (jerry_value_is_boolean(args[1])) {
            arg_evict_now = jerry_value_to_boolean(args[1]);
        }
        else if (jerry_value_is_number(args[1])) {
            arg_evict_now = (jerry_value_as_number(args[1]) != 0);
        }
        else {
            return script_engine_throw_error("lv_image_cache_resize: Argument 1 must be boolean or number for bool");
        }
    }

    // 调用底层函数
    lv_image_cache_resize(arg_new_size, arg_evict_now);

    return jerry_undefined();
}



/**
 * @brief Create an image object pointer to the created image
 */
static jerry_value_t js_lv_image_create(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_image_create: Insufficient arguments");
    }

    // 解析参数: parent (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_parent = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_t* ret_value = lv_image_create(arg_parent);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_OBJ);

    return js_result;
}



/**
 * @brief lv_image_decoder_add_to_cache function
 */
static jerry_value_t js_lv_image_decoder_add_to_cache(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 4) {
        return script_engine_throw_error("lv_image_decoder_add_to_cache: Insufficient arguments");
    }

    // 解析参数: decoder (lv_image_decoder_t*)
    // 指针类型参数，支持null
    void* arg_decoder = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_ANY);
    // 解析参数: search_key (lv_image_cache_data_t*)
    // 指针类型参数，支持null
    void* arg_search_key = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 解析参数: decoded (const lv_draw_buf_t*)
    // 指针类型参数，支持null
    void* arg_decoded = lv_js_bridge_obj_2_ptr(args[2],LV_TYPE_ANY);
    // 解析参数: user_data (void*)
    // void*/字符串 类型参数，支持null
    void* arg_user_data = NULL;
    char* arg_user_data_str = NULL;  // 用于字符串参数的临时存储

    if (!jerry_value_is_undefined(args[3]) && !jerry_value_is_null(args[3])) {
        if (jerry_value_is_string(args[3])) {
            // 处理字符串类型的符号（如LV_SYMBOL_MINUS）
            jerry_size_t arg_user_data_len = jerry_string_size(args[3], JERRY_ENCODING_UTF8);
            arg_user_data_str = (char*)eos_malloc(arg_user_data_len + 1);
            if (!arg_user_data_str) {
                return script_engine_throw_error("lv_image_decoder_add_to_cache: Failed to allocate memory for string argument");
            }
            jerry_string_to_buffer(args[3], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_user_data_str, arg_user_data_len);
            arg_user_data_str[arg_user_data_len] = '\0';
            arg_user_data = (void*)arg_user_data_str;
        }
        else if (jerry_value_is_object(args[3])) {
            // 尝试从对象获取指针
            arg_user_data = lv_js_bridge_obj_2_ptr(args[3],LV_TYPE_ANY);
        }
        else if (jerry_value_is_number(args[3])) {
            // 直接传递指针数值
            uintptr_t ptr_num = (uintptr_t)jerry_value_as_number(args[3]);
            arg_user_data = (void*)ptr_num;
        }
        else {
            return script_engine_throw_error("lv_image_decoder_add_to_cache: Argument 3 must be string, object or number");
        }
    }
    // 调用底层函数
    lv_cache_entry_t* ret_value = lv_image_decoder_add_to_cache(arg_decoder, arg_search_key, arg_decoded, arg_user_data);

    // 处理返回值
    jerry_value_t js_result;
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

    // 释放临时内存
    if (arg_user_data_str) eos_free(arg_user_data_str);

    return js_result;
}



/**
 * @brief Close a decoding session
 */
static jerry_value_t js_lv_image_decoder_close(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_image_decoder_close: Insufficient arguments");
    }

    // 解析参数: dsc (lv_image_decoder_dsc_t*)
    // 指针类型参数，支持null
    void* arg_dsc = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_ANY);
    // 调用底层函数
    lv_image_decoder_close(arg_dsc);

    return jerry_undefined();
}



/**
 * @brief Create a new image decoder pointer to the new image decoder
 */
static jerry_value_t js_lv_image_decoder_create(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 调用底层函数
    lv_image_decoder_t* ret_value = lv_image_decoder_create();

    // 处理返回值
    jerry_value_t js_result;
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

    return js_result;
}



/**
 * @brief Delete an image decoder
 */
static jerry_value_t js_lv_image_decoder_delete(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_image_decoder_delete: Insufficient arguments");
    }

    // 解析参数: decoder (lv_image_decoder_t*)
    // 指针类型参数，支持null
    void* arg_decoder = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_ANY);
    // 调用底层函数
    lv_image_decoder_delete(arg_decoder);

    return jerry_undefined();
}



/**
 * @brief Decode full_area pixels incrementally by calling in a loop. Set decoded_area to LV_COORD_MIN on first call. LV_RESULT_OK: success; LV_RESULT_INVALID: an error occurred or there is nothing left to decode
 */
static jerry_value_t js_lv_image_decoder_get_area(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_image_decoder_get_area: Insufficient arguments");
    }

    // 解析参数: dsc (lv_image_decoder_dsc_t*)
    // 指针类型参数，支持null
    void* arg_dsc = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_ANY);
    // 解析参数: full_area (const lv_area_t*)
    // 指针类型参数，支持null
    void* arg_full_area = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 解析参数: decoded_area (lv_area_t*)
    // 指针类型参数，支持null
    void* arg_decoded_area = lv_js_bridge_obj_2_ptr(args[2],LV_TYPE_ANY);
    // 调用底层函数
    lv_result_t ret_value = lv_image_decoder_get_area(arg_dsc, arg_full_area, arg_decoded_area);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get information about an image. Try the created image decoder one by one. Once one is able to get info that info will be used. LV_RESULT_OK: success; LV_RESULT_INVALID: wasn't able to get info about the image
 */
static jerry_value_t js_lv_image_decoder_get_info(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_image_decoder_get_info: Insufficient arguments");
    }

    // 解析参数: src (const void*)
    // void*/字符串 类型参数，支持null
    void* arg_src = NULL;
    char* arg_src_str = NULL;  // 用于字符串参数的临时存储

    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        if (jerry_value_is_string(args[0])) {
            // 处理字符串类型的符号（如LV_SYMBOL_MINUS）
            jerry_size_t arg_src_len = jerry_string_size(args[0], JERRY_ENCODING_UTF8);
            arg_src_str = (char*)eos_malloc(arg_src_len + 1);
            if (!arg_src_str) {
                return script_engine_throw_error("lv_image_decoder_get_info: Failed to allocate memory for string argument");
            }
            jerry_string_to_buffer(args[0], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_src_str, arg_src_len);
            arg_src_str[arg_src_len] = '\0';
            arg_src = (void*)arg_src_str;
        }
        else if (jerry_value_is_object(args[0])) {
            // 尝试从对象获取指针
            arg_src = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_ANY);
        }
        else if (jerry_value_is_number(args[0])) {
            // 直接传递指针数值
            uintptr_t ptr_num = (uintptr_t)jerry_value_as_number(args[0]);
            arg_src = (void*)ptr_num;
        }
        else {
            return script_engine_throw_error("lv_image_decoder_get_info: Argument 0 must be string, object or number");
        }
    }
    // 解析参数: header (lv_image_header_t*)
    // 指针类型参数，支持null
    void* arg_header = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 调用底层函数
    lv_result_t ret_value = lv_image_decoder_get_info(arg_src, arg_header);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    // 释放临时内存
    if (arg_src_str) eos_free(arg_src_str);

    return js_result;
}



/**
 * @brief Get the next image decoder in the linked list of image decoders the next image decoder or NULL if no more image decoder exists
 */
static jerry_value_t js_lv_image_decoder_get_next(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_image_decoder_get_next: Insufficient arguments");
    }

    // 解析参数: decoder (lv_image_decoder_t*)
    // 指针类型参数，支持null
    void* arg_decoder = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_ANY);
    // 调用底层函数
    lv_image_decoder_t* ret_value = lv_image_decoder_get_next(arg_decoder);

    // 处理返回值
    jerry_value_t js_result;
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

    return js_result;
}



/**
 * @brief Open an image. Try the created image decoders one by one. Once one is able to open the image that decoder is saved in dsc  LV_RESULT_OK: opened the image. dsc->decoded and dsc->header are set. LV_RESULT_INVALID: none of the registered image decoders were able to open the image.
 */
static jerry_value_t js_lv_image_decoder_open(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_image_decoder_open: Insufficient arguments");
    }

    // 解析参数: dsc (lv_image_decoder_dsc_t*)
    // 指针类型参数，支持null
    void* arg_dsc = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_ANY);
    // 解析参数: src (const void*)
    // void*/字符串 类型参数，支持null
    void* arg_src = NULL;
    char* arg_src_str = NULL;  // 用于字符串参数的临时存储

    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1])) {
        if (jerry_value_is_string(args[1])) {
            // 处理字符串类型的符号（如LV_SYMBOL_MINUS）
            jerry_size_t arg_src_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
            arg_src_str = (char*)eos_malloc(arg_src_len + 1);
            if (!arg_src_str) {
                return script_engine_throw_error("lv_image_decoder_open: Failed to allocate memory for string argument");
            }
            jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_src_str, arg_src_len);
            arg_src_str[arg_src_len] = '\0';
            arg_src = (void*)arg_src_str;
        }
        else if (jerry_value_is_object(args[1])) {
            // 尝试从对象获取指针
            arg_src = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
        }
        else if (jerry_value_is_number(args[1])) {
            // 直接传递指针数值
            uintptr_t ptr_num = (uintptr_t)jerry_value_as_number(args[1]);
            arg_src = (void*)ptr_num;
        }
        else {
            return script_engine_throw_error("lv_image_decoder_open: Argument 1 must be string, object or number");
        }
    }
    // 解析参数: args (const lv_image_decoder_args_t*)
    // 指针类型参数，支持null
    void* arg_args = lv_js_bridge_obj_2_ptr(args[2],LV_TYPE_ANY);
    // 调用底层函数
    lv_result_t ret_value = lv_image_decoder_open(arg_dsc, arg_src, arg_args);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    // 释放临时内存
    if (arg_src_str) eos_free(arg_src_str);

    return js_result;
}



/**
 * @brief Check the decoded image, make any modification if decoder args requires. A new draw buf will be allocated if provided decoded is not modifiable or stride mismatch etc.  post processed draw buffer, when it differs with decoded , it's newly allocated.
 */
static jerry_value_t js_lv_image_decoder_post_process(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_image_decoder_post_process: Insufficient arguments");
    }

    // 解析参数: dsc (lv_image_decoder_dsc_t*)
    // 指针类型参数，支持null
    void* arg_dsc = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_ANY);
    // 解析参数: decoded (lv_draw_buf_t*)
    // 指针类型参数，支持null
    void* arg_decoded = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 调用底层函数
    lv_draw_buf_t* ret_value = lv_image_decoder_post_process(arg_dsc, arg_decoded);

    // 处理返回值
    jerry_value_t js_result;
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

    return js_result;
}



/**
 * @brief Set a callback to close a decoding session. E.g. close files and free other resources.
 */
static jerry_value_t js_lv_image_decoder_set_close_cb(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_image_decoder_set_close_cb: Insufficient arguments");
    }

    // 解析参数: decoder (lv_image_decoder_t*)
    // 指针类型参数，支持null
    void* arg_decoder = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_ANY);
    // 解析参数: close_cb (lv_image_decoder_close_f_t)
    // 通用指针类型: lv_image_decoder_close_f_t，支持null
    lv_image_decoder_close_f_t arg_close_cb = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);

    // 调用底层函数
    lv_image_decoder_set_close_cb(arg_decoder, arg_close_cb);

    return jerry_undefined();
}



/**
 * @brief Set a callback to a decoded line of an image
 */
static jerry_value_t js_lv_image_decoder_set_get_area_cb(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_image_decoder_set_get_area_cb: Insufficient arguments");
    }

    // 解析参数: decoder (lv_image_decoder_t*)
    // 指针类型参数，支持null
    void* arg_decoder = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_ANY);
    // 解析参数: read_line_cb (lv_image_decoder_get_area_cb_t)
    // 通用指针类型: lv_image_decoder_get_area_cb_t，支持null
    lv_image_decoder_get_area_cb_t arg_read_line_cb = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);

    // 调用底层函数
    lv_image_decoder_set_get_area_cb(arg_decoder, arg_read_line_cb);

    return jerry_undefined();
}



/**
 * @brief Set a callback to get information about the image
 */
static jerry_value_t js_lv_image_decoder_set_info_cb(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_image_decoder_set_info_cb: Insufficient arguments");
    }

    // 解析参数: decoder (lv_image_decoder_t*)
    // 指针类型参数，支持null
    void* arg_decoder = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_ANY);
    // 解析参数: info_cb (lv_image_decoder_info_f_t)
    // 通用指针类型: lv_image_decoder_info_f_t，支持null
    lv_image_decoder_info_f_t arg_info_cb = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);

    // 调用底层函数
    lv_image_decoder_set_info_cb(arg_decoder, arg_info_cb);

    return jerry_undefined();
}



/**
 * @brief Set a callback to open an image
 */
static jerry_value_t js_lv_image_decoder_set_open_cb(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_image_decoder_set_open_cb: Insufficient arguments");
    }

    // 解析参数: decoder (lv_image_decoder_t*)
    // 指针类型参数，支持null
    void* arg_decoder = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_ANY);
    // 解析参数: open_cb (lv_image_decoder_open_f_t)
    // 通用指针类型: lv_image_decoder_open_f_t，支持null
    lv_image_decoder_open_f_t arg_open_cb = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);

    // 调用底层函数
    lv_image_decoder_set_open_cb(arg_decoder, arg_open_cb);

    return jerry_undefined();
}



/**
 * @brief Get whether the transformations (rotate, zoom) are anti-aliased or not true: anti-aliased; false: not anti-aliased
 */
static jerry_value_t js_lv_image_get_antialias(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_image_get_antialias: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    bool ret_value = lv_image_get_antialias(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the bitmap mask source. an :ref:`lv_image_dsc_t` bitmap mask source.
 */
static jerry_value_t js_lv_image_get_bitmap_map_src(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_image_get_bitmap_map_src: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    const lv_image_dsc_t* ret_value = lv_image_get_bitmap_map_src(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

    return js_result;
}



/**
 * @brief Get the current blend mode of the image the current blend mode
 */
static jerry_value_t js_lv_image_get_blend_mode(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_image_get_blend_mode: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_blend_mode_t ret_value = lv_image_get_blend_mode(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the size mode of the image element of :ref:`lv_image_align_t`
 */
static jerry_value_t js_lv_image_get_inner_align(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_image_get_inner_align: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_image_align_t ret_value = lv_image_get_inner_align(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the offset's x attribute of the image object. offset X value.
 */
static jerry_value_t js_lv_image_get_offset_x(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_image_get_offset_x: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_image_get_offset_x(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the offset's y attribute of the image object. offset Y value.
 */
static jerry_value_t js_lv_image_get_offset_y(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_image_get_offset_y: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_image_get_offset_y(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the pivot (rotation center) of the image. If pivot is set with LV_PCT, convert it to px before return.
 */
static jerry_value_t js_lv_image_get_pivot(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_image_get_pivot: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: pivot (lv_point_t*)
    // 指针类型参数，支持null
    void* arg_pivot = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 调用底层函数
    lv_image_get_pivot(arg_obj, arg_pivot);

    return jerry_undefined();
}



/**
 * @brief Get the rotation of the image. rotation in 0.1 degrees (0..3600)  if image_align is LV_IMAGE_ALIGN_STRETCH or LV_IMAGE_ALIGN_FIT rotation will be set to 0 automatically.
 */
static jerry_value_t js_lv_image_get_rotation(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_image_get_rotation: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_image_get_rotation(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the zoom factor of the image. zoom factor (256: no zoom)
 */
static jerry_value_t js_lv_image_get_scale(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_image_get_scale: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_image_get_scale(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the horizontal zoom factor of the image. zoom factor (256: no zoom)
 */
static jerry_value_t js_lv_image_get_scale_x(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_image_get_scale_x: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_image_get_scale_x(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the vertical zoom factor of the image. zoom factor (256: no zoom)
 */
static jerry_value_t js_lv_image_get_scale_y(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_image_get_scale_y: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_image_get_scale_y(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the source of the image the image source (symbol, file name or ::lv-img_dsc_t for C arrays)
 */
static jerry_value_t js_lv_image_get_src(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_image_get_src: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    const void* ret_value = lv_image_get_src(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为通用指针对象
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

    return js_result;
}



/**
 * @brief Invalidate image header cache. Use NULL to invalidate all image headers. It's also automatically called when an image is invalidated.
 */
static jerry_value_t js_lv_image_header_cache_drop(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_image_header_cache_drop: Insufficient arguments");
    }

    // 解析参数: src (const void*)
    // void*/字符串 类型参数，支持null
    void* arg_src = NULL;
    char* arg_src_str = NULL;  // 用于字符串参数的临时存储

    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        if (jerry_value_is_string(args[0])) {
            // 处理字符串类型的符号（如LV_SYMBOL_MINUS）
            jerry_size_t arg_src_len = jerry_string_size(args[0], JERRY_ENCODING_UTF8);
            arg_src_str = (char*)eos_malloc(arg_src_len + 1);
            if (!arg_src_str) {
                return script_engine_throw_error("lv_image_header_cache_drop: Failed to allocate memory for string argument");
            }
            jerry_string_to_buffer(args[0], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_src_str, arg_src_len);
            arg_src_str[arg_src_len] = '\0';
            arg_src = (void*)arg_src_str;
        }
        else if (jerry_value_is_object(args[0])) {
            // 尝试从对象获取指针
            arg_src = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_ANY);
        }
        else if (jerry_value_is_number(args[0])) {
            // 直接传递指针数值
            uintptr_t ptr_num = (uintptr_t)jerry_value_as_number(args[0]);
            arg_src = (void*)ptr_num;
        }
        else {
            return script_engine_throw_error("lv_image_header_cache_drop: Argument 0 must be string, object or number");
        }
    }
    // 调用底层函数
    lv_image_header_cache_drop(arg_src);

    // 释放临时内存
    if (arg_src_str) eos_free(arg_src_str);

    return jerry_undefined();
}



/**
 * @brief Initialize image header cache. LV_RESULT_OK: initialization succeeded, LV_RESULT_INVALID: failed.
 */
static jerry_value_t js_lv_image_header_cache_init(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_image_header_cache_init: Insufficient arguments");
    }

    // 解析参数: count (uint32_t)
    jerry_value_t js_arg_count = args[0];
    if (!jerry_value_is_number(js_arg_count)) {
        return script_engine_throw_error("lv_image_header_cache_init: Argument 0 must be a number");
    }

    uint32_t arg_count = (uint32_t)jerry_value_as_number(js_arg_count);

    // 调用底层函数
    lv_result_t ret_value = lv_image_header_cache_init(arg_count);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Return true if the image header cache is enabled. true: enabled, false: disabled.
 */
static jerry_value_t js_lv_image_header_cache_is_enabled(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 调用底层函数
    bool ret_value = lv_image_header_cache_is_enabled();

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Resize image header cache. If set to 0, the cache is disabled.
 */
static jerry_value_t js_lv_image_header_cache_resize(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_image_header_cache_resize: Insufficient arguments");
    }

    // 解析参数: count (uint32_t)
    jerry_value_t js_arg_count = args[0];
    if (!jerry_value_is_number(js_arg_count)) {
        return script_engine_throw_error("lv_image_header_cache_resize: Argument 0 must be a number");
    }

    uint32_t arg_count = (uint32_t)jerry_value_as_number(js_arg_count);

    // 解析参数: evict_now (bool)
    // 布尔类型参数: evict_now
    bool arg_evict_now = false;
    if (!jerry_value_is_undefined(args[1])) {
        if (jerry_value_is_boolean(args[1])) {
            arg_evict_now = jerry_value_to_boolean(args[1]);
        }
        else if (jerry_value_is_number(args[1])) {
            arg_evict_now = (jerry_value_as_number(args[1]) != 0);
        }
        else {
            return script_engine_throw_error("lv_image_header_cache_resize: Argument 1 must be boolean or number for bool");
        }
    }

    // 调用底层函数
    lv_image_header_cache_resize(arg_count, arg_evict_now);

    return jerry_undefined();
}



/**
 * @brief Enable/disable anti-aliasing for the transformations (rotate, zoom) or not. The quality is better with anti-aliasing looks better but slower.
 */
static jerry_value_t js_lv_image_set_antialias(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_image_set_antialias: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: antialias (bool)
    // 布尔类型参数: antialias
    bool arg_antialias = false;
    if (!jerry_value_is_undefined(args[1])) {
        if (jerry_value_is_boolean(args[1])) {
            arg_antialias = jerry_value_to_boolean(args[1]);
        }
        else if (jerry_value_is_number(args[1])) {
            arg_antialias = (jerry_value_as_number(args[1]) != 0);
        }
        else {
            return script_engine_throw_error("lv_image_set_antialias: Argument 1 must be boolean or number for bool");
        }
    }

    // 调用底层函数
    lv_image_set_antialias(arg_obj, arg_antialias);

    return jerry_undefined();
}



/**
 * @brief Set an A8 bitmap mask for the image.
 */
static jerry_value_t js_lv_image_set_bitmap_map_src(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_image_set_bitmap_map_src: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: src (const lv_image_dsc_t*)
    // 指针类型参数，支持null
    void* arg_src = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 调用底层函数
    lv_image_set_bitmap_map_src(arg_obj, arg_src);

    return jerry_undefined();
}



/**
 * @brief Set the blend mode of an image.
 */
static jerry_value_t js_lv_image_set_blend_mode(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_image_set_blend_mode: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: blend_mode (lv_blend_mode_t)
    jerry_value_t js_arg_blend_mode = args[1];
    if (!jerry_value_is_number(js_arg_blend_mode)) {
        return script_engine_throw_error("lv_image_set_blend_mode: Argument 1 must be a number");
    }

    int arg_blend_mode = (int)jerry_value_as_number(js_arg_blend_mode);

    // 调用底层函数
    lv_image_set_blend_mode(arg_obj, arg_blend_mode);

    return jerry_undefined();
}



/**
 * @brief Set the image object size mode. if image_align is LV_IMAGE_ALIGN_STRETCH or LV_IMAGE_ALIGN_FIT rotation, scale and pivot will be overwritten and controlled internally.
 */
static jerry_value_t js_lv_image_set_inner_align(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_image_set_inner_align: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: align (lv_image_align_t)
    jerry_value_t js_arg_align = args[1];
    if (!jerry_value_is_number(js_arg_align)) {
        return script_engine_throw_error("lv_image_set_inner_align: Argument 1 must be a number");
    }

    int arg_align = (int)jerry_value_as_number(js_arg_align);

    // 调用底层函数
    lv_image_set_inner_align(arg_obj, arg_align);

    return jerry_undefined();
}



/**
 * @brief Set an offset for the source of an image so the image will be displayed from the new origin.
 */
static jerry_value_t js_lv_image_set_offset_x(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_image_set_offset_x: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: x (int32_t)
    jerry_value_t js_arg_x = args[1];
    if (!jerry_value_is_number(js_arg_x)) {
        return script_engine_throw_error("lv_image_set_offset_x: Argument 1 must be a number");
    }

    int32_t arg_x = (int32_t)jerry_value_as_number(js_arg_x);

    // 调用底层函数
    lv_image_set_offset_x(arg_obj, arg_x);

    return jerry_undefined();
}



/**
 * @brief Set an offset for the source of an image. so the image will be displayed from the new origin.
 */
static jerry_value_t js_lv_image_set_offset_y(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_image_set_offset_y: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: y (int32_t)
    jerry_value_t js_arg_y = args[1];
    if (!jerry_value_is_number(js_arg_y)) {
        return script_engine_throw_error("lv_image_set_offset_y: Argument 1 must be a number");
    }

    int32_t arg_y = (int32_t)jerry_value_as_number(js_arg_y);

    // 调用底层函数
    lv_image_set_offset_y(arg_obj, arg_y);

    return jerry_undefined();
}



/**
 * @brief Set the rotation center of the image. The image will be rotated around this point. x, y can be set with value of LV_PCT, lv_image_get_pivot will return the true pixel coordinate of pivot in this case.
 */
static jerry_value_t js_lv_image_set_pivot(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_image_set_pivot: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: x (int32_t)
    jerry_value_t js_arg_x = args[1];
    if (!jerry_value_is_number(js_arg_x)) {
        return script_engine_throw_error("lv_image_set_pivot: Argument 1 must be a number");
    }

    int32_t arg_x = (int32_t)jerry_value_as_number(js_arg_x);

    // 解析参数: y (int32_t)
    jerry_value_t js_arg_y = args[2];
    if (!jerry_value_is_number(js_arg_y)) {
        return script_engine_throw_error("lv_image_set_pivot: Argument 2 must be a number");
    }

    int32_t arg_y = (int32_t)jerry_value_as_number(js_arg_y);

    // 调用底层函数
    lv_image_set_pivot(arg_obj, arg_x, arg_y);

    return jerry_undefined();
}



/**
 * @brief Set the rotation angle of the image. The image will be rotated around the set pivot set by :ref:`lv_image_set_pivot()` Note that indexed and alpha only images can't be transformed. if image_align is LV_IMAGE_ALIGN_STRETCH or LV_IMAGE_ALIGN_FIT rotation will be set to 0 automatically.
 */
static jerry_value_t js_lv_image_set_rotation(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_image_set_rotation: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: angle (int32_t)
    jerry_value_t js_arg_angle = args[1];
    if (!jerry_value_is_number(js_arg_angle)) {
        return script_engine_throw_error("lv_image_set_rotation: Argument 1 must be a number");
    }

    int32_t arg_angle = (int32_t)jerry_value_as_number(js_arg_angle);

    // 调用底层函数
    lv_image_set_rotation(arg_obj, arg_angle);

    return jerry_undefined();
}



/**
 * @brief Set the zoom factor of the image. Note that indexed and alpha only images can't be transformed.
 */
static jerry_value_t js_lv_image_set_scale(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_image_set_scale: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: zoom (uint32_t)
    jerry_value_t js_arg_zoom = args[1];
    if (!jerry_value_is_number(js_arg_zoom)) {
        return script_engine_throw_error("lv_image_set_scale: Argument 1 must be a number");
    }

    uint32_t arg_zoom = (uint32_t)jerry_value_as_number(js_arg_zoom);

    // 调用底层函数
    lv_image_set_scale(arg_obj, arg_zoom);

    return jerry_undefined();
}



/**
 * @brief Set the horizontal zoom factor of the image. Note that indexed and alpha only images can't be transformed.
 */
static jerry_value_t js_lv_image_set_scale_x(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_image_set_scale_x: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: zoom (uint32_t)
    jerry_value_t js_arg_zoom = args[1];
    if (!jerry_value_is_number(js_arg_zoom)) {
        return script_engine_throw_error("lv_image_set_scale_x: Argument 1 must be a number");
    }

    uint32_t arg_zoom = (uint32_t)jerry_value_as_number(js_arg_zoom);

    // 调用底层函数
    lv_image_set_scale_x(arg_obj, arg_zoom);

    return jerry_undefined();
}



/**
 * @brief Set the vertical zoom factor of the image. Note that indexed and alpha only images can't be transformed.
 */
static jerry_value_t js_lv_image_set_scale_y(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_image_set_scale_y: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: zoom (uint32_t)
    jerry_value_t js_arg_zoom = args[1];
    if (!jerry_value_is_number(js_arg_zoom)) {
        return script_engine_throw_error("lv_image_set_scale_y: Argument 1 must be a number");
    }

    uint32_t arg_zoom = (uint32_t)jerry_value_as_number(js_arg_zoom);

    // 调用底层函数
    lv_image_set_scale_y(arg_obj, arg_zoom);

    return jerry_undefined();
}



/**
 * @brief Get the type of an image source type of the image source LV_IMAGE_SRC_VARIABLE/FILE/SYMBOL/UNKNOWN
 */
static jerry_value_t js_lv_image_src_get_type(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_image_src_get_type: Insufficient arguments");
    }

    // 解析参数: src (const void*)
    // void*/字符串 类型参数，支持null
    void* arg_src = NULL;
    char* arg_src_str = NULL;  // 用于字符串参数的临时存储

    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0])) {
        if (jerry_value_is_string(args[0])) {
            // 处理字符串类型的符号（如LV_SYMBOL_MINUS）
            jerry_size_t arg_src_len = jerry_string_size(args[0], JERRY_ENCODING_UTF8);
            arg_src_str = (char*)eos_malloc(arg_src_len + 1);
            if (!arg_src_str) {
                return script_engine_throw_error("lv_image_src_get_type: Failed to allocate memory for string argument");
            }
            jerry_string_to_buffer(args[0], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_src_str, arg_src_len);
            arg_src_str[arg_src_len] = '\0';
            arg_src = (void*)arg_src_str;
        }
        else if (jerry_value_is_object(args[0])) {
            // 尝试从对象获取指针
            arg_src = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_ANY);
        }
        else if (jerry_value_is_number(args[0])) {
            // 直接传递指针数值
            uintptr_t ptr_num = (uintptr_t)jerry_value_as_number(args[0]);
            arg_src = (void*)ptr_num;
        }
        else {
            return script_engine_throw_error("lv_image_src_get_type: Argument 0 must be string, object or number");
        }
    }
    // 调用底层函数
    lv_image_src_t ret_value = lv_image_src_get_type(arg_src);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    // 释放临时内存
    if (arg_src_str) eos_free(arg_src_str);

    return js_result;
}



/**
 * @brief Bind an integer, string, or pointer subject to a label. pointer to the created observer  fmt == NULL can be used only with string and pointer subjects.  if the subject is a pointer must point to a \0 terminated string.
 */
static jerry_value_t js_lv_label_bind_text(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_label_bind_text: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: subject (lv_subject_t*)
    // 指针类型参数，支持null
    void* arg_subject = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 解析参数: fmt (const char*)

    char* arg_fmt_str = NULL;
    const char* arg_fmt = NULL;
    if (!jerry_value_is_undefined(args[2]) && !jerry_value_is_null(args[2])) {
        if (!jerry_value_is_string(args[2])) {
            return script_engine_throw_error("lv_label_bind_text: Argument 2 must be a string");
        }
        jerry_size_t arg_fmt_len = jerry_string_size(args[2], JERRY_ENCODING_UTF8);
        arg_fmt_str = (char*)eos_malloc(arg_fmt_len + 1);
        jerry_string_to_buffer(args[2], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_fmt_str, arg_fmt_len);
        arg_fmt_str[arg_fmt_len] = '\0';
        arg_fmt = arg_fmt_str;
    }

    // 调用底层函数
    lv_observer_t* ret_value = lv_label_bind_text(arg_obj, arg_subject, arg_fmt);

    // 处理返回值
    jerry_value_t js_result;
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

    // 释放临时内存
    if (arg_fmt_str) eos_free(arg_fmt_str);

    return js_result;
}



/**
 * @brief Create a label object pointer to the created button
 */
static jerry_value_t js_lv_label_create(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_label_create: Insufficient arguments");
    }

    // 解析参数: parent (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_parent = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_t* ret_value = lv_label_create(arg_parent);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_OBJ);

    return js_result;
}



/**
 * @brief Delete characters from a label. The label text cannot be static.
 */
static jerry_value_t js_lv_label_cut_text(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_label_cut_text: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: pos (uint32_t)
    jerry_value_t js_arg_pos = args[1];
    if (!jerry_value_is_number(js_arg_pos)) {
        return script_engine_throw_error("lv_label_cut_text: Argument 1 must be a number");
    }

    uint32_t arg_pos = (uint32_t)jerry_value_as_number(js_arg_pos);

    // 解析参数: cnt (uint32_t)
    jerry_value_t js_arg_cnt = args[2];
    if (!jerry_value_is_number(js_arg_cnt)) {
        return script_engine_throw_error("lv_label_cut_text: Argument 2 must be a number");
    }

    uint32_t arg_cnt = (uint32_t)jerry_value_as_number(js_arg_cnt);

    // 调用底层函数
    lv_label_cut_text(arg_obj, arg_pos, arg_cnt);

    return jerry_undefined();
}



/**
 * @brief Get the index of letter on a relative point of a label. The index of the letter on the 'pos_p' point (E.g. on 0;0 is the 0. letter if aligned to the left) Expressed in character index and not byte index (different in UTF-8)
 */
static jerry_value_t js_lv_label_get_letter_on(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_label_get_letter_on: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: pos_in (lv_point_t*)
    // 指针类型参数，支持null
    void* arg_pos_in = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 解析参数: bidi (bool)
    // 布尔类型参数: bidi
    bool arg_bidi = false;
    if (!jerry_value_is_undefined(args[2])) {
        if (jerry_value_is_boolean(args[2])) {
            arg_bidi = jerry_value_to_boolean(args[2]);
        }
        else if (jerry_value_is_number(args[2])) {
            arg_bidi = (jerry_value_as_number(args[2]) != 0);
        }
        else {
            return script_engine_throw_error("lv_label_get_letter_on: Argument 2 must be boolean or number for bool");
        }
    }

    // 调用底层函数
    uint32_t ret_value = lv_label_get_letter_on(arg_obj, arg_pos_in, arg_bidi);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the relative x and y coordinates of a letter
 */
static jerry_value_t js_lv_label_get_letter_pos(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_label_get_letter_pos: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: char_id (uint32_t)
    jerry_value_t js_arg_char_id = args[1];
    if (!jerry_value_is_number(js_arg_char_id)) {
        return script_engine_throw_error("lv_label_get_letter_pos: Argument 1 must be a number");
    }

    uint32_t arg_char_id = (uint32_t)jerry_value_as_number(js_arg_char_id);

    // 解析参数: pos (lv_point_t*)
    // 指针类型参数，支持null
    void* arg_pos = lv_js_bridge_obj_2_ptr(args[2],LV_TYPE_ANY);
    // 调用底层函数
    lv_label_get_letter_pos(arg_obj, arg_char_id, arg_pos);

    return jerry_undefined();
}



/**
 * @brief Get the long mode of a label the current long mode
 */
static jerry_value_t js_lv_label_get_long_mode(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_label_get_long_mode: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_label_long_mode_t ret_value = lv_label_get_long_mode(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the text of a label the text of the label
 */
static jerry_value_t js_lv_label_get_text(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_label_get_text: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
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
 * @brief selection end index. LV_LABEL_TXT_SEL_OFF if nothing is selected.
 */
static jerry_value_t js_lv_label_get_text_selection_end(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_label_get_text_selection_end: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    uint32_t ret_value = lv_label_get_text_selection_end(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief selection start index. LV_LABEL_TEXT_SELECTION_OFF if nothing is selected.
 */
static jerry_value_t js_lv_label_get_text_selection_start(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_label_get_text_selection_start: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    uint32_t ret_value = lv_label_get_text_selection_start(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Insert a text to a label. The label text cannot be static.
 */
static jerry_value_t js_lv_label_ins_text(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_label_ins_text: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: pos (uint32_t)
    jerry_value_t js_arg_pos = args[1];
    if (!jerry_value_is_number(js_arg_pos)) {
        return script_engine_throw_error("lv_label_ins_text: Argument 1 must be a number");
    }

    uint32_t arg_pos = (uint32_t)jerry_value_as_number(js_arg_pos);

    // 解析参数: txt (const char*)

    char* arg_txt_str = NULL;
    const char* arg_txt = NULL;
    if (!jerry_value_is_undefined(args[2]) && !jerry_value_is_null(args[2])) {
        if (!jerry_value_is_string(args[2])) {
            return script_engine_throw_error("lv_label_ins_text: Argument 2 must be a string");
        }
        jerry_size_t arg_txt_len = jerry_string_size(args[2], JERRY_ENCODING_UTF8);
        arg_txt_str = (char*)eos_malloc(arg_txt_len + 1);
        jerry_string_to_buffer(args[2], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_txt_str, arg_txt_len);
        arg_txt_str[arg_txt_len] = '\0';
        arg_txt = arg_txt_str;
    }

    // 调用底层函数
    lv_label_ins_text(arg_obj, arg_pos, arg_txt);

    // 释放临时内存
    if (arg_txt_str) eos_free(arg_txt_str);

    return jerry_undefined();
}



/**
 * @brief Check if a character is drawn under a point. whether a character is drawn under the point
 */
static jerry_value_t js_lv_label_is_char_under_pos(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_label_is_char_under_pos: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: pos (lv_point_t*)
    // 指针类型参数，支持null
    void* arg_pos = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 调用底层函数
    bool ret_value = lv_label_is_char_under_pos(arg_obj, arg_pos);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Set the behavior of the label with text longer than the object size
 */
static jerry_value_t js_lv_label_set_long_mode(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_label_set_long_mode: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: long_mode (lv_label_long_mode_t)
    jerry_value_t js_arg_long_mode = args[1];
    if (!jerry_value_is_number(js_arg_long_mode)) {
        return script_engine_throw_error("lv_label_set_long_mode: Argument 1 must be a number");
    }

    lv_label_long_mode_t arg_long_mode = (lv_label_long_mode_t)jerry_value_as_number(js_arg_long_mode);

    // 调用底层函数
    lv_label_set_long_mode(arg_obj, arg_long_mode);

    return jerry_undefined();
}



/**
 * @brief Set a new text for a label. Memory will be allocated to store the text by the label.
 */
static jerry_value_t js_lv_label_set_text(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_label_set_text: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: text (const char*)

    char* arg_text_str = NULL;
    const char* arg_text = NULL;
    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1])) {
        if (!jerry_value_is_string(args[1])) {
            return script_engine_throw_error("lv_label_set_text: Argument 1 must be a string");
        }
        jerry_size_t arg_text_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
        arg_text_str = (char*)eos_malloc(arg_text_len + 1);
        jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_text_str, arg_text_len);
        arg_text_str[arg_text_len] = '\0';
        arg_text = arg_text_str;
    }

    // 调用底层函数
    lv_label_set_text(arg_obj, arg_text);

    // 释放临时内存
    if (arg_text_str) eos_free(arg_text_str);

    return jerry_undefined();
}



/**
 * @brief Set where text selection should end
 */
static jerry_value_t js_lv_label_set_text_selection_end(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_label_set_text_selection_end: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: index (uint32_t)
    jerry_value_t js_arg_index = args[1];
    if (!jerry_value_is_number(js_arg_index)) {
        return script_engine_throw_error("lv_label_set_text_selection_end: Argument 1 must be a number");
    }

    uint32_t arg_index = (uint32_t)jerry_value_as_number(js_arg_index);

    // 调用底层函数
    lv_label_set_text_selection_end(arg_obj, arg_index);

    return jerry_undefined();
}



/**
 * @brief Set where text selection should start
 */
static jerry_value_t js_lv_label_set_text_selection_start(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_label_set_text_selection_start: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: index (uint32_t)
    jerry_value_t js_arg_index = args[1];
    if (!jerry_value_is_number(js_arg_index)) {
        return script_engine_throw_error("lv_label_set_text_selection_start: Argument 1 must be a number");
    }

    uint32_t arg_index = (uint32_t)jerry_value_as_number(js_arg_index);

    // 调用底层函数
    lv_label_set_text_selection_start(arg_obj, arg_index);

    return jerry_undefined();
}



/**
 * @brief Set a static text. It will not be saved by the label so the 'text' variable has to be 'alive' while the label exists.
 */
static jerry_value_t js_lv_label_set_text_static(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_label_set_text_static: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: text (const char*)

    char* arg_text_str = NULL;
    const char* arg_text = NULL;
    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1])) {
        if (!jerry_value_is_string(args[1])) {
            return script_engine_throw_error("lv_label_set_text_static: Argument 1 must be a string");
        }
        jerry_size_t arg_text_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
        arg_text_str = (char*)eos_malloc(arg_text_len + 1);
        jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_text_str, arg_text_len);
        arg_text_str[arg_text_len] = '\0';
        arg_text = arg_text_str;
    }

    // 调用底层函数
    lv_label_set_text_static(arg_obj, arg_text);

    // 释放临时内存
    if (arg_text_str) eos_free(arg_text_str);

    return jerry_undefined();
}



/**
 * @brief Add button to a list pointer to the created button
 */
static jerry_value_t js_lv_list_add_button(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_list_add_button: Insufficient arguments");
    }

    // 解析参数: list (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_list = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: icon (const void*)
    // void*/字符串 类型参数，支持null
    void* arg_icon = NULL;
    char* arg_icon_str = NULL;  // 用于字符串参数的临时存储

    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1])) {
        if (jerry_value_is_string(args[1])) {
            // 处理字符串类型的符号（如LV_SYMBOL_MINUS）
            jerry_size_t arg_icon_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
            arg_icon_str = (char*)eos_malloc(arg_icon_len + 1);
            if (!arg_icon_str) {
                return script_engine_throw_error("lv_list_add_button: Failed to allocate memory for string argument");
            }
            jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_icon_str, arg_icon_len);
            arg_icon_str[arg_icon_len] = '\0';
            arg_icon = (void*)arg_icon_str;
        }
        else if (jerry_value_is_object(args[1])) {
            // 尝试从对象获取指针
            arg_icon = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
        }
        else if (jerry_value_is_number(args[1])) {
            // 直接传递指针数值
            uintptr_t ptr_num = (uintptr_t)jerry_value_as_number(args[1]);
            arg_icon = (void*)ptr_num;
        }
        else {
            return script_engine_throw_error("lv_list_add_button: Argument 1 must be string, object or number");
        }
    }
    // 解析参数: txt (const char*)

    char* arg_txt_str = NULL;
    const char* arg_txt = NULL;
    if (!jerry_value_is_undefined(args[2]) && !jerry_value_is_null(args[2])) {
        if (!jerry_value_is_string(args[2])) {
            return script_engine_throw_error("lv_list_add_button: Argument 2 must be a string");
        }
        jerry_size_t arg_txt_len = jerry_string_size(args[2], JERRY_ENCODING_UTF8);
        arg_txt_str = (char*)eos_malloc(arg_txt_len + 1);
        jerry_string_to_buffer(args[2], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_txt_str, arg_txt_len);
        arg_txt_str[arg_txt_len] = '\0';
        arg_txt = arg_txt_str;
    }

    // 调用底层函数
    lv_obj_t* ret_value = lv_list_add_button(arg_list, arg_icon, arg_txt);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_OBJ);

    // 释放临时内存
    if (arg_txt_str) eos_free(arg_txt_str);
    if (arg_icon_str) eos_free(arg_icon_str);

    return js_result;
}



/**
 * @brief Add text to a list pointer to the created label
 */
static jerry_value_t js_lv_list_add_text(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_list_add_text: Insufficient arguments");
    }

    // 解析参数: list (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_list = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: txt (const char*)

    char* arg_txt_str = NULL;
    const char* arg_txt = NULL;
    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1])) {
        if (!jerry_value_is_string(args[1])) {
            return script_engine_throw_error("lv_list_add_text: Argument 1 must be a string");
        }
        jerry_size_t arg_txt_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
        arg_txt_str = (char*)eos_malloc(arg_txt_len + 1);
        jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_txt_str, arg_txt_len);
        arg_txt_str[arg_txt_len] = '\0';
        arg_txt = arg_txt_str;
    }

    // 调用底层函数
    lv_obj_t* ret_value = lv_list_add_text(arg_list, arg_txt);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_OBJ);

    // 释放临时内存
    if (arg_txt_str) eos_free(arg_txt_str);

    return js_result;
}



/**
 * @brief Create a list object pointer to the created list
 */
static jerry_value_t js_lv_list_create(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_list_create: Insufficient arguments");
    }

    // 解析参数: parent (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_parent = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_t* ret_value = lv_list_create(arg_parent);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_OBJ);

    return js_result;
}



/**
 * @brief Get text of a given list button text of btn, if btn doesn't have text "" will be returned
 */
static jerry_value_t js_lv_list_get_button_text(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_list_get_button_text: Insufficient arguments");
    }

    // 解析参数: list (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_list = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: btn (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_btn = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_OBJ);
    // 调用底层函数
    const char* ret_value = lv_list_get_button_text(arg_list, arg_btn);

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
 * @brief Set text of a given list button
 */
static jerry_value_t js_lv_list_set_button_text(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_list_set_button_text: Insufficient arguments");
    }

    // 解析参数: list (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_list = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: btn (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_btn = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_OBJ);
    // 解析参数: txt (const char*)

    char* arg_txt_str = NULL;
    const char* arg_txt = NULL;
    if (!jerry_value_is_undefined(args[2]) && !jerry_value_is_null(args[2])) {
        if (!jerry_value_is_string(args[2])) {
            return script_engine_throw_error("lv_list_set_button_text: Argument 2 must be a string");
        }
        jerry_size_t arg_txt_len = jerry_string_size(args[2], JERRY_ENCODING_UTF8);
        arg_txt_str = (char*)eos_malloc(arg_txt_len + 1);
        jerry_string_to_buffer(args[2], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_txt_str, arg_txt_len);
        arg_txt_str[arg_txt_len] = '\0';
        arg_txt = arg_txt_str;
    }

    // 调用底层函数
    lv_list_set_button_text(arg_list, arg_btn, arg_txt);

    // 释放临时内存
    if (arg_txt_str) eos_free(arg_txt_str);

    return jerry_undefined();
}



/**
 * @brief Set one or more flags
 */
static jerry_value_t js_lv_obj_add_flag(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_add_flag: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: f (lv_obj_flag_t)
    jerry_value_t js_arg_f = args[1];
    if (!jerry_value_is_number(js_arg_f)) {
        return script_engine_throw_error("lv_obj_add_flag: Argument 1 must be a number");
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
        return script_engine_throw_error("lv_obj_add_state: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: state (lv_state_t)
    jerry_value_t js_arg_state = args[1];
    if (!jerry_value_is_number(js_arg_state)) {
        return script_engine_throw_error("lv_obj_add_state: Argument 1 must be a number");
    }

    uint16_t arg_state = (uint16_t)jerry_value_as_number(js_arg_state);

    // 调用底层函数
    lv_obj_add_state(arg_obj, arg_state);

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
        return script_engine_throw_error("lv_obj_add_style: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: style (const lv_style_t*)
    // 指针类型参数，支持null
    void* arg_style = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_add_style: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_add_style(arg_obj, arg_style, arg_selector);

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
        return script_engine_throw_error("lv_obj_align: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: align (lv_align_t)
    jerry_value_t js_arg_align = args[1];
    if (!jerry_value_is_number(js_arg_align)) {
        return script_engine_throw_error("lv_obj_align: Argument 1 must be a number");
    }

    int arg_align = (int)jerry_value_as_number(js_arg_align);

    // 解析参数: x_ofs (int32_t)
    jerry_value_t js_arg_x_ofs = args[2];
    if (!jerry_value_is_number(js_arg_x_ofs)) {
        return script_engine_throw_error("lv_obj_align: Argument 2 must be a number");
    }

    int32_t arg_x_ofs = (int32_t)jerry_value_as_number(js_arg_x_ofs);

    // 解析参数: y_ofs (int32_t)
    jerry_value_t js_arg_y_ofs = args[3];
    if (!jerry_value_is_number(js_arg_y_ofs)) {
        return script_engine_throw_error("lv_obj_align: Argument 3 must be a number");
    }

    int32_t arg_y_ofs = (int32_t)jerry_value_as_number(js_arg_y_ofs);

    // 调用底层函数
    lv_obj_align(arg_obj, arg_align, arg_x_ofs, arg_y_ofs);

    return jerry_undefined();
}



/**
 * @brief Align an object to another object. if the position or size of base changes obj needs to be aligned manually again
 */
static jerry_value_t js_lv_obj_align_to(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 5) {
        return script_engine_throw_error("lv_obj_align_to: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: base (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_base = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_OBJ);
    // 解析参数: align (lv_align_t)
    jerry_value_t js_arg_align = args[2];
    if (!jerry_value_is_number(js_arg_align)) {
        return script_engine_throw_error("lv_obj_align_to: Argument 2 must be a number");
    }

    int arg_align = (int)jerry_value_as_number(js_arg_align);

    // 解析参数: x_ofs (int32_t)
    jerry_value_t js_arg_x_ofs = args[3];
    if (!jerry_value_is_number(js_arg_x_ofs)) {
        return script_engine_throw_error("lv_obj_align_to: Argument 3 must be a number");
    }

    int32_t arg_x_ofs = (int32_t)jerry_value_as_number(js_arg_x_ofs);

    // 解析参数: y_ofs (int32_t)
    jerry_value_t js_arg_y_ofs = args[4];
    if (!jerry_value_is_number(js_arg_y_ofs)) {
        return script_engine_throw_error("lv_obj_align_to: Argument 4 must be a number");
    }

    int32_t arg_y_ofs = (int32_t)jerry_value_as_number(js_arg_y_ofs);

    // 调用底层函数
    lv_obj_align_to(arg_obj, arg_base, arg_align, arg_x_ofs, arg_y_ofs);

    return jerry_undefined();
}



/**
 * @brief Allocate special data for an object if not allocated yet.
 */
static jerry_value_t js_lv_obj_allocate_spec_attr(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_allocate_spec_attr: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_allocate_spec_attr(arg_obj);

    return jerry_undefined();
}



/**
 * @brief Tell whether an area of an object is visible (even partially) now or not true visible; false not visible (hidden, out of parent, on other screen, etc)
 */
static jerry_value_t js_lv_obj_area_is_visible(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_area_is_visible: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: area (lv_area_t*)
    // 指针类型参数，支持null
    void* arg_area = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 调用底层函数
    bool ret_value = lv_obj_area_is_visible(arg_obj, arg_area);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Set an integer subject to 1 when an object is checked and set it 0 when unchecked. pointer to the created observer  Ensure the object's LV_OBJ_FLAG_CHECKABLE flag is set
 */
static jerry_value_t js_lv_obj_bind_checked(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_bind_checked: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: subject (lv_subject_t*)
    // 指针类型参数，支持null
    void* arg_subject = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 调用底层函数
    lv_observer_t* ret_value = lv_obj_bind_checked(arg_obj, arg_subject);

    // 处理返回值
    jerry_value_t js_result;
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

    return js_result;
}



/**
 * @brief Set an object flag if an integer subject's value is equal to a reference value, clear the flag otherwise pointer to the created observer
 */
static jerry_value_t js_lv_obj_bind_flag_if_eq(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 4) {
        return script_engine_throw_error("lv_obj_bind_flag_if_eq: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: subject (lv_subject_t*)
    // 指针类型参数，支持null
    void* arg_subject = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 解析参数: flag (lv_obj_flag_t)
    jerry_value_t js_arg_flag = args[2];
    if (!jerry_value_is_number(js_arg_flag)) {
        return script_engine_throw_error("lv_obj_bind_flag_if_eq: Argument 2 must be a number");
    }

    int arg_flag = (int)jerry_value_as_number(js_arg_flag);

    // 解析参数: ref_value (int32_t)
    jerry_value_t js_arg_ref_value = args[3];
    if (!jerry_value_is_number(js_arg_ref_value)) {
        return script_engine_throw_error("lv_obj_bind_flag_if_eq: Argument 3 must be a number");
    }

    int32_t arg_ref_value = (int32_t)jerry_value_as_number(js_arg_ref_value);

    // 调用底层函数
    lv_observer_t* ret_value = lv_obj_bind_flag_if_eq(arg_obj, arg_subject, arg_flag, arg_ref_value);

    // 处理返回值
    jerry_value_t js_result;
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

    return js_result;
}



/**
 * @brief Set an object flag if an integer subject's value is not equal to a reference value, clear the flag otherwise pointer to the created observer
 */
static jerry_value_t js_lv_obj_bind_flag_if_not_eq(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 4) {
        return script_engine_throw_error("lv_obj_bind_flag_if_not_eq: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: subject (lv_subject_t*)
    // 指针类型参数，支持null
    void* arg_subject = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 解析参数: flag (lv_obj_flag_t)
    jerry_value_t js_arg_flag = args[2];
    if (!jerry_value_is_number(js_arg_flag)) {
        return script_engine_throw_error("lv_obj_bind_flag_if_not_eq: Argument 2 must be a number");
    }

    int arg_flag = (int)jerry_value_as_number(js_arg_flag);

    // 解析参数: ref_value (int32_t)
    jerry_value_t js_arg_ref_value = args[3];
    if (!jerry_value_is_number(js_arg_ref_value)) {
        return script_engine_throw_error("lv_obj_bind_flag_if_not_eq: Argument 3 must be a number");
    }

    int32_t arg_ref_value = (int32_t)jerry_value_as_number(js_arg_ref_value);

    // 调用底层函数
    lv_observer_t* ret_value = lv_obj_bind_flag_if_not_eq(arg_obj, arg_subject, arg_flag, arg_ref_value);

    // 处理返回值
    jerry_value_t js_result;
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

    return js_result;
}



/**
 * @brief Set an object state if an integer subject's value is equal to a reference value, clear the flag otherwise pointer to the created observer
 */
static jerry_value_t js_lv_obj_bind_state_if_eq(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 4) {
        return script_engine_throw_error("lv_obj_bind_state_if_eq: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: subject (lv_subject_t*)
    // 指针类型参数，支持null
    void* arg_subject = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 解析参数: state (lv_state_t)
    jerry_value_t js_arg_state = args[2];
    if (!jerry_value_is_number(js_arg_state)) {
        return script_engine_throw_error("lv_obj_bind_state_if_eq: Argument 2 must be a number");
    }

    uint16_t arg_state = (uint16_t)jerry_value_as_number(js_arg_state);

    // 解析参数: ref_value (int32_t)
    jerry_value_t js_arg_ref_value = args[3];
    if (!jerry_value_is_number(js_arg_ref_value)) {
        return script_engine_throw_error("lv_obj_bind_state_if_eq: Argument 3 must be a number");
    }

    int32_t arg_ref_value = (int32_t)jerry_value_as_number(js_arg_ref_value);

    // 调用底层函数
    lv_observer_t* ret_value = lv_obj_bind_state_if_eq(arg_obj, arg_subject, arg_state, arg_ref_value);

    // 处理返回值
    jerry_value_t js_result;
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

    return js_result;
}



/**
 * @brief Set an object state if an integer subject's value is not equal to a reference value, clear the flag otherwise pointer to the created observer
 */
static jerry_value_t js_lv_obj_bind_state_if_not_eq(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 4) {
        return script_engine_throw_error("lv_obj_bind_state_if_not_eq: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: subject (lv_subject_t*)
    // 指针类型参数，支持null
    void* arg_subject = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 解析参数: state (lv_state_t)
    jerry_value_t js_arg_state = args[2];
    if (!jerry_value_is_number(js_arg_state)) {
        return script_engine_throw_error("lv_obj_bind_state_if_not_eq: Argument 2 must be a number");
    }

    uint16_t arg_state = (uint16_t)jerry_value_as_number(js_arg_state);

    // 解析参数: ref_value (int32_t)
    jerry_value_t js_arg_ref_value = args[3];
    if (!jerry_value_is_number(js_arg_ref_value)) {
        return script_engine_throw_error("lv_obj_bind_state_if_not_eq: Argument 3 must be a number");
    }

    int32_t arg_ref_value = (int32_t)jerry_value_as_number(js_arg_ref_value);

    // 调用底层函数
    lv_observer_t* ret_value = lv_obj_bind_state_if_not_eq(arg_obj, arg_subject, arg_state, arg_ref_value);

    // 处理返回值
    jerry_value_t js_result;
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

    return js_result;
}



/**
 * @brief Get the required extra size (around the object's part) to draw shadow, outline, value etc. the extra size required around the object
 */
static jerry_value_t js_lv_obj_calculate_ext_draw_size(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_calculate_ext_draw_size: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_calculate_ext_draw_size: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_calculate_ext_draw_size(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_calculate_style_text_align function
 */
static jerry_value_t js_lv_obj_calculate_style_text_align(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_calculate_style_text_align: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_calculate_style_text_align: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 解析参数: txt (const char*)

    char* arg_txt_str = NULL;
    const char* arg_txt = NULL;
    if (!jerry_value_is_undefined(args[2]) && !jerry_value_is_null(args[2])) {
        if (!jerry_value_is_string(args[2])) {
            return script_engine_throw_error("lv_obj_calculate_style_text_align: Argument 2 must be a string");
        }
        jerry_size_t arg_txt_len = jerry_string_size(args[2], JERRY_ENCODING_UTF8);
        arg_txt_str = (char*)eos_malloc(arg_txt_len + 1);
        jerry_string_to_buffer(args[2], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_txt_str, arg_txt_len);
        arg_txt_str[arg_txt_len] = '\0';
        arg_txt = arg_txt_str;
    }

    // 调用底层函数
    lv_text_align_t ret_value = lv_obj_calculate_style_text_align(arg_obj, arg_part, arg_txt);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    // 释放临时内存
    if (arg_txt_str) eos_free(arg_txt_str);

    return js_result;
}



/**
 * @brief Align an object to the center on its parent. if the parent size changes obj needs to be aligned manually again
 */
static jerry_value_t js_lv_obj_center(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_center: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_center(arg_obj);

    return jerry_undefined();
}



/**
 * @brief Check the type of obj. true: class_p is the obj class.
 */
static jerry_value_t js_lv_obj_check_type(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_check_type: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: class_p (const lv_obj_class_t*)
    // 指针类型参数，支持null
    void* arg_class_p = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 调用底层函数
    bool ret_value = lv_obj_check_type(arg_obj, arg_class_p);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Create an object form a class descriptor pointer to the created object
 */
static jerry_value_t js_lv_obj_class_create_obj(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_class_create_obj: Insufficient arguments");
    }

    // 解析参数: class_p (const lv_obj_class_t*)
    // 指针类型参数，支持null
    void* arg_class_p = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_ANY);
    // 解析参数: parent (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_parent = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_t* ret_value = lv_obj_class_create_obj(arg_class_p, arg_parent);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_OBJ);

    return js_result;
}



/**
 * @brief lv_obj_class_init_obj function
 */
static jerry_value_t js_lv_obj_class_init_obj(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_class_init_obj: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_class_init_obj(arg_obj);

    return jerry_undefined();
}



/**
 * @brief Delete all children of an object. Also remove the objects from their group and remove all animations (if any). Send LV_EVENT_DELETED to deleted objects.
 */
static jerry_value_t js_lv_obj_clean(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_clean: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_clean(arg_obj);

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
        return script_engine_throw_error("lv_obj_create: Insufficient arguments");
    }

    // 解析参数: parent (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_parent = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_t* ret_value = lv_obj_create(arg_parent);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_OBJ);

    return js_result;
}



/**
 * @brief Delete an object and all of its children. Also remove the objects from their group and remove all animations (if any). Send LV_EVENT_DELETED to deleted objects.
 */
static jerry_value_t js_lv_obj_delete(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_delete: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_delete(arg_obj);

    return jerry_undefined();
}



/**
 * @brief A function to be easily used in animation ready callback to delete an object when the animation is ready
 */
static jerry_value_t js_lv_obj_delete_anim_completed_cb(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_delete_anim_completed_cb: Insufficient arguments");
    }

    // 解析参数: a ( lv_anim_t*)
    // lv_anim_t* 类型参数处理
    lv_anim_t* arg_a = lv_js_bridge_obj_2_ptr(args[0], LV_TYPE_ANY);
    // 调用底层函数
    lv_obj_delete_anim_completed_cb(arg_a);

    return jerry_undefined();
}



/**
 * @brief Helper function for asynchronously deleting objects. Useful for cases where you can't delete an object directly in an LV_EVENT_DELETE handler (i.e. parent). :ref:`lv_async_call`
 */
static jerry_value_t js_lv_obj_delete_async(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_delete_async: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_delete_async(arg_obj);

    return jerry_undefined();
}



/**
 * @brief Delete an object after some delay
 */
static jerry_value_t js_lv_obj_delete_delayed(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_delete_delayed: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: delay_ms (uint32_t)
    jerry_value_t js_arg_delay_ms = args[1];
    if (!jerry_value_is_number(js_arg_delay_ms)) {
        return script_engine_throw_error("lv_obj_delete_delayed: Argument 1 must be a number");
    }

    uint32_t arg_delay_ms = (uint32_t)jerry_value_as_number(js_arg_delay_ms);

    // 调用底层函数
    lv_obj_delete_delayed(arg_obj, arg_delay_ms);

    return jerry_undefined();
}



/**
 * @brief Iterate through all children of any object and print their ID.
 */
static jerry_value_t js_lv_obj_dump_tree(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_dump_tree: Insufficient arguments");
    }

    // 解析参数: start_obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_start_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_dump_tree(arg_start_obj);

    return jerry_undefined();
}



/**
 * @brief Enable or disable automatic style refreshing when a new style is added/removed to/from an object or any other style change happens.
 */
static jerry_value_t js_lv_obj_enable_style_refresh(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_enable_style_refresh: Insufficient arguments");
    }

    // 解析参数: en (bool)
    // 布尔类型参数: en
    bool arg_en = false;
    if (!jerry_value_is_undefined(args[0])) {
        if (jerry_value_is_boolean(args[0])) {
            arg_en = jerry_value_to_boolean(args[0]);
        }
        else if (jerry_value_is_number(args[0])) {
            arg_en = (jerry_value_as_number(args[0]) != 0);
        }
        else {
            return script_engine_throw_error("lv_obj_enable_style_refresh: Argument 0 must be boolean or number for bool");
        }
    }

    // 调用底层函数
    lv_obj_enable_style_refresh(arg_en);

    return jerry_undefined();
}



/**
 * @brief Used by the widgets internally to call the ancestor widget types's event handler LV_RESULT_OK: the target object was not deleted in the event; LV_RESULT_INVALID: it was deleted in the event_code
 */
static jerry_value_t js_lv_obj_event_base(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_event_base: Insufficient arguments");
    }

    // 解析参数: class_p (const lv_obj_class_t*)
    // 指针类型参数，支持null
    void* arg_class_p = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_ANY);
    // 解析参数: e (lv_event_t*)
    // 指针类型参数，支持null
    void* arg_e = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_EVENT);
    // 调用底层函数
    lv_result_t ret_value = lv_obj_event_base(arg_class_p, arg_e);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Fade in an an object and all its children.
 */
static jerry_value_t js_lv_obj_fade_in(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_fade_in: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: time (uint32_t)
    jerry_value_t js_arg_time = args[1];
    if (!jerry_value_is_number(js_arg_time)) {
        return script_engine_throw_error("lv_obj_fade_in: Argument 1 must be a number");
    }

    uint32_t arg_time = (uint32_t)jerry_value_as_number(js_arg_time);

    // 解析参数: delay (uint32_t)
    jerry_value_t js_arg_delay = args[2];
    if (!jerry_value_is_number(js_arg_delay)) {
        return script_engine_throw_error("lv_obj_fade_in: Argument 2 must be a number");
    }

    uint32_t arg_delay = (uint32_t)jerry_value_as_number(js_arg_delay);

    // 调用底层函数
    lv_obj_fade_in(arg_obj, arg_time, arg_delay);

    return jerry_undefined();
}



/**
 * @brief Fade out an an object and all its children.
 */
static jerry_value_t js_lv_obj_fade_out(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_fade_out: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: time (uint32_t)
    jerry_value_t js_arg_time = args[1];
    if (!jerry_value_is_number(js_arg_time)) {
        return script_engine_throw_error("lv_obj_fade_out: Argument 1 must be a number");
    }

    uint32_t arg_time = (uint32_t)jerry_value_as_number(js_arg_time);

    // 解析参数: delay (uint32_t)
    jerry_value_t js_arg_delay = args[2];
    if (!jerry_value_is_number(js_arg_delay)) {
        return script_engine_throw_error("lv_obj_fade_out: Argument 2 must be a number");
    }

    uint32_t arg_delay = (uint32_t)jerry_value_as_number(js_arg_delay);

    // 调用底层函数
    lv_obj_fade_out(arg_obj, arg_time, arg_delay);

    return jerry_undefined();
}



/**
 * @brief Get the child of an object by the child's index. pointer to the child or NULL if the index was invalid
 */
static jerry_value_t js_lv_obj_get_child(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_child: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: idx (int32_t)
    jerry_value_t js_arg_idx = args[1];
    if (!jerry_value_is_number(js_arg_idx)) {
        return script_engine_throw_error("lv_obj_get_child: Argument 1 must be a number");
    }

    int32_t arg_idx = (int32_t)jerry_value_as_number(js_arg_idx);

    // 调用底层函数
    lv_obj_t* ret_value = lv_obj_get_child(arg_obj, arg_idx);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_OBJ);

    return js_result;
}



/**
 * @brief Get the child of an object by the child's index. Consider the children only with a given type. pointer to the child or NULL if the index was invalid
 */
static jerry_value_t js_lv_obj_get_child_by_type(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_get_child_by_type: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: idx (int32_t)
    jerry_value_t js_arg_idx = args[1];
    if (!jerry_value_is_number(js_arg_idx)) {
        return script_engine_throw_error("lv_obj_get_child_by_type: Argument 1 must be a number");
    }

    int32_t arg_idx = (int32_t)jerry_value_as_number(js_arg_idx);

    // 解析参数: class_p (const lv_obj_class_t*)
    // 指针类型参数，支持null
    void* arg_class_p = lv_js_bridge_obj_2_ptr(args[2],LV_TYPE_ANY);
    // 调用底层函数
    lv_obj_t* ret_value = lv_obj_get_child_by_type(arg_obj, arg_idx, arg_class_p);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_OBJ);

    return js_result;
}



/**
 * @brief Get the number of children the number of children
 */
static jerry_value_t js_lv_obj_get_child_count(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_child_count: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    uint32_t ret_value = lv_obj_get_child_count(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the number of children having a given type. the number of children
 */
static jerry_value_t js_lv_obj_get_child_count_by_type(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_child_count_by_type: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: class_p (const lv_obj_class_t*)
    // 指针类型参数，支持null
    void* arg_class_p = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 调用底层函数
    uint32_t ret_value = lv_obj_get_child_count_by_type(arg_obj, arg_class_p);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the class (type) of the object the class (type) of the object
 */
static jerry_value_t js_lv_obj_get_class(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_class: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    const lv_obj_class_t* ret_value = lv_obj_get_class(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

    return js_result;
}



/**
 * @brief Get the an area where to object can be clicked. It's the object's normal area plus the extended click area.
 */
static jerry_value_t js_lv_obj_get_click_area(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_click_area: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: area (lv_area_t*)
    // 指针类型参数，支持null
    void* arg_area = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 调用底层函数
    lv_obj_get_click_area(arg_obj, arg_area);

    return jerry_undefined();
}



/**
 * @brief Get the area reduced by the paddings and the border width. The position of the object is recalculated only on the next redraw. To force coordinate recalculation call lv_obj_update_layout(obj) .
 */
static jerry_value_t js_lv_obj_get_content_coords(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_content_coords: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: area (lv_area_t*)
    // 指针类型参数，支持null
    void* arg_area = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 调用底层函数
    lv_obj_get_content_coords(arg_obj, arg_area);

    return jerry_undefined();
}



/**
 * @brief Get the height reduced by the top and bottom padding and the border width. The position of the object is recalculated only on the next redraw. To force coordinate recalculation call lv_obj_update_layout(obj) .  the height which still fits into the parent without causing overflow (making the parent scrollable)
 */
static jerry_value_t js_lv_obj_get_content_height(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_content_height: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_obj_get_content_height(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the width reduced by the left and right padding and the border width. The position of the object is recalculated only on the next redraw. To force coordinate recalculation call lv_obj_update_layout(obj) .  the width which still fits into its parent without causing overflow (making the parent scrollable)
 */
static jerry_value_t js_lv_obj_get_content_width(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_content_width: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_obj_get_content_width(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Copy the coordinates of an object to an area
 */
static jerry_value_t js_lv_obj_get_coords(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_coords: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: coords (lv_area_t*)
    // 指针类型参数，支持null
    void* arg_coords = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 调用底层函数
    lv_obj_get_coords(arg_obj, arg_coords);

    return jerry_undefined();
}



/**
 * @brief Get the display of the object pointer to the object's display
 */
static jerry_value_t js_lv_obj_get_display(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_display: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_display_t* ret_value = lv_obj_get_display(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

    return js_result;
}



/**
 * @brief lv_obj_get_event_count function
 */
static jerry_value_t js_lv_obj_get_event_count(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_event_count: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    uint32_t ret_value = lv_obj_get_event_count(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_event_dsc function
 */
static jerry_value_t js_lv_obj_get_event_dsc(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_event_dsc: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: index (uint32_t)
    jerry_value_t js_arg_index = args[1];
    if (!jerry_value_is_number(js_arg_index)) {
        return script_engine_throw_error("lv_obj_get_event_dsc: Argument 1 must be a number");
    }

    uint32_t arg_index = (uint32_t)jerry_value_as_number(js_arg_index);

    // 调用底层函数
    lv_event_dsc_t* ret_value = lv_obj_get_event_dsc(arg_obj, arg_index);

    // 处理返回值
    jerry_value_t js_result;
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

    return js_result;
}



/**
 * @brief Get the group of the object the pointer to group of the object
 */
static jerry_value_t js_lv_obj_get_group(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_group: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_group_t* ret_value = lv_obj_get_group(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

    return js_result;
}



/**
 * @brief Get the height of an object The position of the object is recalculated only on the next redraw. To force coordinate recalculation call lv_obj_update_layout(obj) .  the height in pixels
 */
static jerry_value_t js_lv_obj_get_height(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_height: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_obj_get_height(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the index of a child. the child index of the object. E.g. 0: the oldest (firstly created child). (-1 if child could not be found or no parent exists)
 */
static jerry_value_t js_lv_obj_get_index(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_index: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_obj_get_index(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the index of a child. Consider the children only with a given type. the child index of the object. E.g. 0: the oldest (firstly created child with the given class). (-1 if child could not be found or no parent exists)
 */
static jerry_value_t js_lv_obj_get_index_by_type(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_index_by_type: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: class_p (const lv_obj_class_t*)
    // 指针类型参数，支持null
    void* arg_class_p = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 调用底层函数
    int32_t ret_value = lv_obj_get_index_by_type(arg_obj, arg_class_p);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the parent of an object the parent of the object. (NULL if obj was a screen)
 */
static jerry_value_t js_lv_obj_get_parent(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_parent: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_t* ret_value = lv_obj_get_parent(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_OBJ);

    return js_result;
}



/**
 * @brief Get the screen of an object pointer to the object's screen
 */
static jerry_value_t js_lv_obj_get_screen(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_screen: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_t* ret_value = lv_obj_get_screen(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_OBJ);

    return js_result;
}



/**
 * @brief Return the height of the area below the object. That is the number of pixels the object can be scrolled down. Normally positive but can be negative when scrolled inside. the scrollable area below the object in pixels
 */
static jerry_value_t js_lv_obj_get_scroll_bottom(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_scroll_bottom: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_obj_get_scroll_bottom(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the object in which directions can be scrolled
 */
static jerry_value_t js_lv_obj_get_scroll_dir(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_scroll_dir: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_dir_t ret_value = lv_obj_get_scroll_dir(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the X and Y coordinates where the scrolling will end for this object if a scrolling animation is in progress. If no scrolling animation, give the current x or y scroll position.
 */
static jerry_value_t js_lv_obj_get_scroll_end(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_scroll_end: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: end (lv_point_t*)
    // 指针类型参数，支持null
    void* arg_end = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 调用底层函数
    lv_obj_get_scroll_end(arg_obj, arg_end);

    return jerry_undefined();
}



/**
 * @brief Return the width of the area on the left the object. That is the number of pixels the object can be scrolled down. Normally positive but can be negative when scrolled inside. the scrollable area on the left the object in pixels
 */
static jerry_value_t js_lv_obj_get_scroll_left(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_scroll_left: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_obj_get_scroll_left(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Return the width of the area on the right the object. That is the number of pixels the object can be scrolled down. Normally positive but can be negative when scrolled inside. the scrollable area on the right the object in pixels
 */
static jerry_value_t js_lv_obj_get_scroll_right(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_scroll_right: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_obj_get_scroll_right(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get where to snap the children when scrolling ends horizontally the current snap align from :ref:`lv_scroll_snap_t`
 */
static jerry_value_t js_lv_obj_get_scroll_snap_x(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_scroll_snap_x: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_scroll_snap_t ret_value = lv_obj_get_scroll_snap_x(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get where to snap the children when scrolling ends vertically the current snap align from :ref:`lv_scroll_snap_t`
 */
static jerry_value_t js_lv_obj_get_scroll_snap_y(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_scroll_snap_y: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_scroll_snap_t ret_value = lv_obj_get_scroll_snap_y(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Return the height of the area above the object. That is the number of pixels the object can be scrolled down. Normally positive but can be negative when scrolled inside. the scrollable area above the object in pixels
 */
static jerry_value_t js_lv_obj_get_scroll_top(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_scroll_top: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_obj_get_scroll_top(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get current X scroll position. the current scroll position from the left edge. If the object is not scrolled return 0 If scrolled return > 0 If scrolled in (elastic scroll) return < 0
 */
static jerry_value_t js_lv_obj_get_scroll_x(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_scroll_x: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_obj_get_scroll_x(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get current Y scroll position. the current scroll position from the top edge. If the object is not scrolled return 0 If scrolled return > 0 If scrolled inside return < 0
 */
static jerry_value_t js_lv_obj_get_scroll_y(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_scroll_y: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_obj_get_scroll_y(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the area of the scrollbars
 */
static jerry_value_t js_lv_obj_get_scrollbar_area(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_get_scrollbar_area: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: hor (lv_area_t*)
    // 指针类型参数，支持null
    void* arg_hor = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 解析参数: ver (lv_area_t*)
    // 指针类型参数，支持null
    void* arg_ver = lv_js_bridge_obj_2_ptr(args[2],LV_TYPE_ANY);
    // 调用底层函数
    lv_obj_get_scrollbar_area(arg_obj, arg_hor, arg_ver);

    return jerry_undefined();
}



/**
 * @brief Get the current scroll mode (when to hide the scrollbars) the current scroll mode from :ref:`lv_scrollbar_mode_t`
 */
static jerry_value_t js_lv_obj_get_scrollbar_mode(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_scrollbar_mode: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_scrollbar_mode_t ret_value = lv_obj_get_scrollbar_mode(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the height occupied by the "parts" of the widget. E.g. the height of all rows of a table. the width of the virtually drawn content  This size independent from the real size of the widget. It just tells how large the internal ("virtual") content is.
 */
static jerry_value_t js_lv_obj_get_self_height(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_self_height: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_obj_get_self_height(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the width occupied by the "parts" of the widget. E.g. the width of all columns of a table. the width of the virtually drawn content  This size independent from the real size of the widget. It just tells how large the internal ("virtual") content is.
 */
static jerry_value_t js_lv_obj_get_self_width(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_self_width: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_obj_get_self_width(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Return a sibling of an object pointer to the requested sibling or NULL if there is no such sibling
 */
static jerry_value_t js_lv_obj_get_sibling(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_sibling: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: idx (int32_t)
    jerry_value_t js_arg_idx = args[1];
    if (!jerry_value_is_number(js_arg_idx)) {
        return script_engine_throw_error("lv_obj_get_sibling: Argument 1 must be a number");
    }

    int32_t arg_idx = (int32_t)jerry_value_as_number(js_arg_idx);

    // 调用底层函数
    lv_obj_t* ret_value = lv_obj_get_sibling(arg_obj, arg_idx);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_OBJ);

    return js_result;
}



/**
 * @brief Return a sibling of an object. Consider the siblings only with a given type. pointer to the requested sibling or NULL if there is no such sibling
 */
static jerry_value_t js_lv_obj_get_sibling_by_type(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_get_sibling_by_type: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: idx (int32_t)
    jerry_value_t js_arg_idx = args[1];
    if (!jerry_value_is_number(js_arg_idx)) {
        return script_engine_throw_error("lv_obj_get_sibling_by_type: Argument 1 must be a number");
    }

    int32_t arg_idx = (int32_t)jerry_value_as_number(js_arg_idx);

    // 解析参数: class_p (const lv_obj_class_t*)
    // 指针类型参数，支持null
    void* arg_class_p = lv_js_bridge_obj_2_ptr(args[2],LV_TYPE_ANY);
    // 调用底层函数
    lv_obj_t* ret_value = lv_obj_get_sibling_by_type(arg_obj, arg_idx, arg_class_p);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_OBJ);

    return js_result;
}



/**
 * @brief Get the state of an object the state (OR-ed values from lv_state_t )
 */
static jerry_value_t js_lv_obj_get_state(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_state: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_state_t ret_value = lv_obj_get_state(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_align function
 */
static jerry_value_t js_lv_obj_get_style_align(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_align: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_align: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_align_t ret_value = lv_obj_get_style_align(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_anim function
 */
static jerry_value_t js_lv_obj_get_style_anim(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_anim: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_anim: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
     lv_anim_t* ret_value = lv_obj_get_style_anim(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

    return js_result;
}



/**
 * @brief lv_obj_get_style_anim_duration function
 */
static jerry_value_t js_lv_obj_get_style_anim_duration(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_anim_duration: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_anim_duration: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    uint32_t ret_value = lv_obj_get_style_anim_duration(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_arc_color function
 */
static jerry_value_t js_lv_obj_get_style_arc_color(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_arc_color: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_arc_color: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_color_t ret_value = lv_obj_get_style_arc_color(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    // 转换为JS颜色对象
    js_result = lv_js_bridge_color_2_obj(&ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_arc_color_filtered function
 */
static jerry_value_t js_lv_obj_get_style_arc_color_filtered(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_arc_color_filtered: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_arc_color_filtered: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_color_t ret_value = lv_obj_get_style_arc_color_filtered(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    // 转换为JS颜色对象
    js_result = lv_js_bridge_color_2_obj(&ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_arc_image_src function
 */
static jerry_value_t js_lv_obj_get_style_arc_image_src(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_arc_image_src: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_arc_image_src: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    const void* ret_value = lv_obj_get_style_arc_image_src(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为通用指针对象
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

    return js_result;
}



/**
 * @brief lv_obj_get_style_arc_opa function
 */
static jerry_value_t js_lv_obj_get_style_arc_opa(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_arc_opa: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_arc_opa: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_opa_t ret_value = lv_obj_get_style_arc_opa(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_arc_rounded function
 */
static jerry_value_t js_lv_obj_get_style_arc_rounded(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_arc_rounded: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_arc_rounded: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    bool ret_value = lv_obj_get_style_arc_rounded(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_arc_width function
 */
static jerry_value_t js_lv_obj_get_style_arc_width(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_arc_width: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_arc_width: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_arc_width(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_base_dir function
 */
static jerry_value_t js_lv_obj_get_style_base_dir(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_base_dir: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_base_dir: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_base_dir_t ret_value = lv_obj_get_style_base_dir(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_bg_color function
 */
static jerry_value_t js_lv_obj_get_style_bg_color(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_bg_color: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_bg_color: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_color_t ret_value = lv_obj_get_style_bg_color(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    // 转换为JS颜色对象
    js_result = lv_js_bridge_color_2_obj(&ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_bg_color_filtered function
 */
static jerry_value_t js_lv_obj_get_style_bg_color_filtered(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_bg_color_filtered: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_bg_color_filtered: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_color_t ret_value = lv_obj_get_style_bg_color_filtered(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    // 转换为JS颜色对象
    js_result = lv_js_bridge_color_2_obj(&ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_bg_grad function
 */
static jerry_value_t js_lv_obj_get_style_bg_grad(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_bg_grad: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_bg_grad: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    const lv_grad_dsc_t* ret_value = lv_obj_get_style_bg_grad(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

    return js_result;
}



/**
 * @brief lv_obj_get_style_bg_grad_color function
 */
static jerry_value_t js_lv_obj_get_style_bg_grad_color(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_bg_grad_color: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_bg_grad_color: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_color_t ret_value = lv_obj_get_style_bg_grad_color(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    // 转换为JS颜色对象
    js_result = lv_js_bridge_color_2_obj(&ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_bg_grad_color_filtered function
 */
static jerry_value_t js_lv_obj_get_style_bg_grad_color_filtered(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_bg_grad_color_filtered: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_bg_grad_color_filtered: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_color_t ret_value = lv_obj_get_style_bg_grad_color_filtered(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    // 转换为JS颜色对象
    js_result = lv_js_bridge_color_2_obj(&ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_bg_grad_dir function
 */
static jerry_value_t js_lv_obj_get_style_bg_grad_dir(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_bg_grad_dir: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_bg_grad_dir: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_grad_dir_t ret_value = lv_obj_get_style_bg_grad_dir(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_bg_grad_opa function
 */
static jerry_value_t js_lv_obj_get_style_bg_grad_opa(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_bg_grad_opa: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_bg_grad_opa: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_opa_t ret_value = lv_obj_get_style_bg_grad_opa(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_bg_grad_stop function
 */
static jerry_value_t js_lv_obj_get_style_bg_grad_stop(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_bg_grad_stop: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_bg_grad_stop: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_bg_grad_stop(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_bg_image_opa function
 */
static jerry_value_t js_lv_obj_get_style_bg_image_opa(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_bg_image_opa: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_bg_image_opa: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_opa_t ret_value = lv_obj_get_style_bg_image_opa(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_bg_image_recolor function
 */
static jerry_value_t js_lv_obj_get_style_bg_image_recolor(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_bg_image_recolor: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_bg_image_recolor: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_color_t ret_value = lv_obj_get_style_bg_image_recolor(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    // 转换为JS颜色对象
    js_result = lv_js_bridge_color_2_obj(&ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_bg_image_recolor_filtered function
 */
static jerry_value_t js_lv_obj_get_style_bg_image_recolor_filtered(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_bg_image_recolor_filtered: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_bg_image_recolor_filtered: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_color_t ret_value = lv_obj_get_style_bg_image_recolor_filtered(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    // 转换为JS颜色对象
    js_result = lv_js_bridge_color_2_obj(&ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_bg_image_recolor_opa function
 */
static jerry_value_t js_lv_obj_get_style_bg_image_recolor_opa(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_bg_image_recolor_opa: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_bg_image_recolor_opa: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_opa_t ret_value = lv_obj_get_style_bg_image_recolor_opa(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_bg_image_src function
 */
static jerry_value_t js_lv_obj_get_style_bg_image_src(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_bg_image_src: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_bg_image_src: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    const void* ret_value = lv_obj_get_style_bg_image_src(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为通用指针对象
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

    return js_result;
}



/**
 * @brief lv_obj_get_style_bg_image_tiled function
 */
static jerry_value_t js_lv_obj_get_style_bg_image_tiled(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_bg_image_tiled: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_bg_image_tiled: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    bool ret_value = lv_obj_get_style_bg_image_tiled(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_bg_main_opa function
 */
static jerry_value_t js_lv_obj_get_style_bg_main_opa(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_bg_main_opa: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_bg_main_opa: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_opa_t ret_value = lv_obj_get_style_bg_main_opa(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_bg_main_stop function
 */
static jerry_value_t js_lv_obj_get_style_bg_main_stop(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_bg_main_stop: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_bg_main_stop: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_bg_main_stop(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_bg_opa function
 */
static jerry_value_t js_lv_obj_get_style_bg_opa(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_bg_opa: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_bg_opa: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_opa_t ret_value = lv_obj_get_style_bg_opa(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_bitmap_mask_src function
 */
static jerry_value_t js_lv_obj_get_style_bitmap_mask_src(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_bitmap_mask_src: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_bitmap_mask_src: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    const void* ret_value = lv_obj_get_style_bitmap_mask_src(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为通用指针对象
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

    return js_result;
}



/**
 * @brief lv_obj_get_style_blend_mode function
 */
static jerry_value_t js_lv_obj_get_style_blend_mode(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_blend_mode: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_blend_mode: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_blend_mode_t ret_value = lv_obj_get_style_blend_mode(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_border_color function
 */
static jerry_value_t js_lv_obj_get_style_border_color(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_border_color: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_border_color: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_color_t ret_value = lv_obj_get_style_border_color(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    // 转换为JS颜色对象
    js_result = lv_js_bridge_color_2_obj(&ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_border_color_filtered function
 */
static jerry_value_t js_lv_obj_get_style_border_color_filtered(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_border_color_filtered: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_border_color_filtered: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_color_t ret_value = lv_obj_get_style_border_color_filtered(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    // 转换为JS颜色对象
    js_result = lv_js_bridge_color_2_obj(&ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_border_opa function
 */
static jerry_value_t js_lv_obj_get_style_border_opa(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_border_opa: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_border_opa: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_opa_t ret_value = lv_obj_get_style_border_opa(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_border_post function
 */
static jerry_value_t js_lv_obj_get_style_border_post(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_border_post: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_border_post: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    bool ret_value = lv_obj_get_style_border_post(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_border_side function
 */
static jerry_value_t js_lv_obj_get_style_border_side(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_border_side: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_border_side: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_border_side_t ret_value = lv_obj_get_style_border_side(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_border_width function
 */
static jerry_value_t js_lv_obj_get_style_border_width(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_border_width: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_border_width: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_border_width(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_clip_corner function
 */
static jerry_value_t js_lv_obj_get_style_clip_corner(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_clip_corner: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_clip_corner: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    bool ret_value = lv_obj_get_style_clip_corner(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_color_filter_dsc function
 */
static jerry_value_t js_lv_obj_get_style_color_filter_dsc(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_color_filter_dsc: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_color_filter_dsc: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    const lv_color_filter_dsc_t* ret_value = lv_obj_get_style_color_filter_dsc(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

    return js_result;
}



/**
 * @brief lv_obj_get_style_color_filter_opa function
 */
static jerry_value_t js_lv_obj_get_style_color_filter_opa(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_color_filter_opa: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_color_filter_opa: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_opa_t ret_value = lv_obj_get_style_color_filter_opa(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_flex_cross_place function
 */
static jerry_value_t js_lv_obj_get_style_flex_cross_place(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_flex_cross_place: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_flex_cross_place: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_flex_align_t ret_value = lv_obj_get_style_flex_cross_place(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_flex_flow function
 */
static jerry_value_t js_lv_obj_get_style_flex_flow(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_flex_flow: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_flex_flow: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_flex_flow_t ret_value = lv_obj_get_style_flex_flow(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_flex_grow function
 */
static jerry_value_t js_lv_obj_get_style_flex_grow(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_flex_grow: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_flex_grow: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    uint8_t ret_value = lv_obj_get_style_flex_grow(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_flex_main_place function
 */
static jerry_value_t js_lv_obj_get_style_flex_main_place(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_flex_main_place: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_flex_main_place: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_flex_align_t ret_value = lv_obj_get_style_flex_main_place(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_flex_track_place function
 */
static jerry_value_t js_lv_obj_get_style_flex_track_place(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_flex_track_place: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_flex_track_place: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_flex_align_t ret_value = lv_obj_get_style_flex_track_place(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_grid_cell_column_pos function
 */
static jerry_value_t js_lv_obj_get_style_grid_cell_column_pos(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_grid_cell_column_pos: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_grid_cell_column_pos: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_grid_cell_column_pos(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_grid_cell_column_span function
 */
static jerry_value_t js_lv_obj_get_style_grid_cell_column_span(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_grid_cell_column_span: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_grid_cell_column_span: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_grid_cell_column_span(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_grid_cell_row_pos function
 */
static jerry_value_t js_lv_obj_get_style_grid_cell_row_pos(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_grid_cell_row_pos: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_grid_cell_row_pos: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_grid_cell_row_pos(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_grid_cell_row_span function
 */
static jerry_value_t js_lv_obj_get_style_grid_cell_row_span(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_grid_cell_row_span: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_grid_cell_row_span: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_grid_cell_row_span(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_grid_cell_x_align function
 */
static jerry_value_t js_lv_obj_get_style_grid_cell_x_align(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_grid_cell_x_align: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_grid_cell_x_align: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_grid_align_t ret_value = lv_obj_get_style_grid_cell_x_align(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_grid_cell_y_align function
 */
static jerry_value_t js_lv_obj_get_style_grid_cell_y_align(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_grid_cell_y_align: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_grid_cell_y_align: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_grid_align_t ret_value = lv_obj_get_style_grid_cell_y_align(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_grid_column_align function
 */
static jerry_value_t js_lv_obj_get_style_grid_column_align(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_grid_column_align: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_grid_column_align: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_grid_align_t ret_value = lv_obj_get_style_grid_column_align(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_grid_column_dsc_array function
 */
static jerry_value_t js_lv_obj_get_style_grid_column_dsc_array(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_grid_column_dsc_array: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_grid_column_dsc_array: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    const int32_t* ret_value = lv_obj_get_style_grid_column_dsc_array(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

    return js_result;
}



/**
 * @brief lv_obj_get_style_grid_row_align function
 */
static jerry_value_t js_lv_obj_get_style_grid_row_align(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_grid_row_align: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_grid_row_align: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_grid_align_t ret_value = lv_obj_get_style_grid_row_align(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_grid_row_dsc_array function
 */
static jerry_value_t js_lv_obj_get_style_grid_row_dsc_array(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_grid_row_dsc_array: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_grid_row_dsc_array: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    const int32_t* ret_value = lv_obj_get_style_grid_row_dsc_array(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

    return js_result;
}



/**
 * @brief lv_obj_get_style_height function
 */
static jerry_value_t js_lv_obj_get_style_height(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_height: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_height: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_height(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_image_opa function
 */
static jerry_value_t js_lv_obj_get_style_image_opa(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_image_opa: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_image_opa: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_opa_t ret_value = lv_obj_get_style_image_opa(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_image_recolor function
 */
static jerry_value_t js_lv_obj_get_style_image_recolor(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_image_recolor: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_image_recolor: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_color_t ret_value = lv_obj_get_style_image_recolor(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    // 转换为JS颜色对象
    js_result = lv_js_bridge_color_2_obj(&ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_image_recolor_filtered function
 */
static jerry_value_t js_lv_obj_get_style_image_recolor_filtered(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_image_recolor_filtered: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_image_recolor_filtered: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_color_t ret_value = lv_obj_get_style_image_recolor_filtered(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    // 转换为JS颜色对象
    js_result = lv_js_bridge_color_2_obj(&ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_image_recolor_opa function
 */
static jerry_value_t js_lv_obj_get_style_image_recolor_opa(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_image_recolor_opa: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_image_recolor_opa: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_opa_t ret_value = lv_obj_get_style_image_recolor_opa(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_layout function
 */
static jerry_value_t js_lv_obj_get_style_layout(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_layout: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_layout: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    uint16_t ret_value = lv_obj_get_style_layout(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_length function
 */
static jerry_value_t js_lv_obj_get_style_length(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_length: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_length: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_length(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_line_color function
 */
static jerry_value_t js_lv_obj_get_style_line_color(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_line_color: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_line_color: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_color_t ret_value = lv_obj_get_style_line_color(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    // 转换为JS颜色对象
    js_result = lv_js_bridge_color_2_obj(&ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_line_color_filtered function
 */
static jerry_value_t js_lv_obj_get_style_line_color_filtered(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_line_color_filtered: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_line_color_filtered: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_color_t ret_value = lv_obj_get_style_line_color_filtered(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    // 转换为JS颜色对象
    js_result = lv_js_bridge_color_2_obj(&ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_line_dash_gap function
 */
static jerry_value_t js_lv_obj_get_style_line_dash_gap(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_line_dash_gap: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_line_dash_gap: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_line_dash_gap(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_line_dash_width function
 */
static jerry_value_t js_lv_obj_get_style_line_dash_width(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_line_dash_width: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_line_dash_width: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_line_dash_width(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_line_opa function
 */
static jerry_value_t js_lv_obj_get_style_line_opa(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_line_opa: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_line_opa: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_opa_t ret_value = lv_obj_get_style_line_opa(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_line_rounded function
 */
static jerry_value_t js_lv_obj_get_style_line_rounded(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_line_rounded: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_line_rounded: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    bool ret_value = lv_obj_get_style_line_rounded(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_line_width function
 */
static jerry_value_t js_lv_obj_get_style_line_width(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_line_width: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_line_width: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_line_width(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_margin_bottom function
 */
static jerry_value_t js_lv_obj_get_style_margin_bottom(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_margin_bottom: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_margin_bottom: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_margin_bottom(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_margin_left function
 */
static jerry_value_t js_lv_obj_get_style_margin_left(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_margin_left: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_margin_left: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_margin_left(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_margin_right function
 */
static jerry_value_t js_lv_obj_get_style_margin_right(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_margin_right: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_margin_right: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_margin_right(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_margin_top function
 */
static jerry_value_t js_lv_obj_get_style_margin_top(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_margin_top: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_margin_top: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_margin_top(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_max_height function
 */
static jerry_value_t js_lv_obj_get_style_max_height(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_max_height: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_max_height: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_max_height(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_max_width function
 */
static jerry_value_t js_lv_obj_get_style_max_width(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_max_width: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_max_width: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_max_width(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_min_height function
 */
static jerry_value_t js_lv_obj_get_style_min_height(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_min_height: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_min_height: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_min_height(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_min_width function
 */
static jerry_value_t js_lv_obj_get_style_min_width(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_min_width: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_min_width: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_min_width(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_opa function
 */
static jerry_value_t js_lv_obj_get_style_opa(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_opa: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_opa: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_opa_t ret_value = lv_obj_get_style_opa(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_opa_layered function
 */
static jerry_value_t js_lv_obj_get_style_opa_layered(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_opa_layered: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_opa_layered: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_opa_t ret_value = lv_obj_get_style_opa_layered(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the opa style property from all parents and multiply and >> 8 them. the final opacity considering the parents' opacity too
 */
static jerry_value_t js_lv_obj_get_style_opa_recursive(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_opa_recursive: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_opa_recursive: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_opa_t ret_value = lv_obj_get_style_opa_recursive(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_outline_color function
 */
static jerry_value_t js_lv_obj_get_style_outline_color(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_outline_color: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_outline_color: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_color_t ret_value = lv_obj_get_style_outline_color(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    // 转换为JS颜色对象
    js_result = lv_js_bridge_color_2_obj(&ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_outline_color_filtered function
 */
static jerry_value_t js_lv_obj_get_style_outline_color_filtered(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_outline_color_filtered: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_outline_color_filtered: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_color_t ret_value = lv_obj_get_style_outline_color_filtered(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    // 转换为JS颜色对象
    js_result = lv_js_bridge_color_2_obj(&ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_outline_opa function
 */
static jerry_value_t js_lv_obj_get_style_outline_opa(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_outline_opa: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_outline_opa: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_opa_t ret_value = lv_obj_get_style_outline_opa(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_outline_pad function
 */
static jerry_value_t js_lv_obj_get_style_outline_pad(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_outline_pad: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_outline_pad: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_outline_pad(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_outline_width function
 */
static jerry_value_t js_lv_obj_get_style_outline_width(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_outline_width: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_outline_width: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_outline_width(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_pad_bottom function
 */
static jerry_value_t js_lv_obj_get_style_pad_bottom(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_pad_bottom: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_pad_bottom: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_pad_bottom(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_pad_column function
 */
static jerry_value_t js_lv_obj_get_style_pad_column(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_pad_column: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_pad_column: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_pad_column(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_pad_left function
 */
static jerry_value_t js_lv_obj_get_style_pad_left(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_pad_left: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_pad_left: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_pad_left(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_pad_right function
 */
static jerry_value_t js_lv_obj_get_style_pad_right(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_pad_right: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_pad_right: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_pad_right(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_pad_row function
 */
static jerry_value_t js_lv_obj_get_style_pad_row(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_pad_row: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_pad_row: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_pad_row(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_pad_top function
 */
static jerry_value_t js_lv_obj_get_style_pad_top(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_pad_top: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_pad_top: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_pad_top(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_radius function
 */
static jerry_value_t js_lv_obj_get_style_radius(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_radius: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_radius: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_radius(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_rotary_sensitivity function
 */
static jerry_value_t js_lv_obj_get_style_rotary_sensitivity(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_rotary_sensitivity: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_rotary_sensitivity: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    uint32_t ret_value = lv_obj_get_style_rotary_sensitivity(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_shadow_color function
 */
static jerry_value_t js_lv_obj_get_style_shadow_color(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_shadow_color: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_shadow_color: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_color_t ret_value = lv_obj_get_style_shadow_color(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    // 转换为JS颜色对象
    js_result = lv_js_bridge_color_2_obj(&ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_shadow_color_filtered function
 */
static jerry_value_t js_lv_obj_get_style_shadow_color_filtered(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_shadow_color_filtered: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_shadow_color_filtered: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_color_t ret_value = lv_obj_get_style_shadow_color_filtered(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    // 转换为JS颜色对象
    js_result = lv_js_bridge_color_2_obj(&ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_shadow_offset_x function
 */
static jerry_value_t js_lv_obj_get_style_shadow_offset_x(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_shadow_offset_x: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_shadow_offset_x: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_shadow_offset_x(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_shadow_offset_y function
 */
static jerry_value_t js_lv_obj_get_style_shadow_offset_y(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_shadow_offset_y: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_shadow_offset_y: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_shadow_offset_y(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_shadow_opa function
 */
static jerry_value_t js_lv_obj_get_style_shadow_opa(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_shadow_opa: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_shadow_opa: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_opa_t ret_value = lv_obj_get_style_shadow_opa(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_shadow_spread function
 */
static jerry_value_t js_lv_obj_get_style_shadow_spread(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_shadow_spread: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_shadow_spread: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_shadow_spread(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_shadow_width function
 */
static jerry_value_t js_lv_obj_get_style_shadow_width(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_shadow_width: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_shadow_width: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_shadow_width(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_space_bottom function
 */
static jerry_value_t js_lv_obj_get_style_space_bottom(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_space_bottom: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_space_bottom: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_space_bottom(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_space_left function
 */
static jerry_value_t js_lv_obj_get_style_space_left(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_space_left: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_space_left: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_space_left(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_space_right function
 */
static jerry_value_t js_lv_obj_get_style_space_right(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_space_right: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_space_right: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_space_right(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_space_top function
 */
static jerry_value_t js_lv_obj_get_style_space_top(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_space_top: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_space_top: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_space_top(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_text_align function
 */
static jerry_value_t js_lv_obj_get_style_text_align(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_text_align: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_text_align: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_text_align_t ret_value = lv_obj_get_style_text_align(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_text_color function
 */
static jerry_value_t js_lv_obj_get_style_text_color(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_text_color: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_text_color: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_color_t ret_value = lv_obj_get_style_text_color(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    // 转换为JS颜色对象
    js_result = lv_js_bridge_color_2_obj(&ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_text_color_filtered function
 */
static jerry_value_t js_lv_obj_get_style_text_color_filtered(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_text_color_filtered: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_text_color_filtered: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_color_t ret_value = lv_obj_get_style_text_color_filtered(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    // 转换为JS颜色对象
    js_result = lv_js_bridge_color_2_obj(&ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_text_decor function
 */
static jerry_value_t js_lv_obj_get_style_text_decor(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_text_decor: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_text_decor: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_text_decor_t ret_value = lv_obj_get_style_text_decor(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_text_font function
 */
static jerry_value_t js_lv_obj_get_style_text_font(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_text_font: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_text_font: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
     lv_font_t* ret_value = lv_obj_get_style_text_font(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

    return js_result;
}



/**
 * @brief lv_obj_get_style_text_letter_space function
 */
static jerry_value_t js_lv_obj_get_style_text_letter_space(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_text_letter_space: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_text_letter_space: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_text_letter_space(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_text_line_space function
 */
static jerry_value_t js_lv_obj_get_style_text_line_space(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_text_line_space: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_text_line_space: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_text_line_space(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_text_opa function
 */
static jerry_value_t js_lv_obj_get_style_text_opa(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_text_opa: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_text_opa: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    lv_opa_t ret_value = lv_obj_get_style_text_opa(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_transform_height function
 */
static jerry_value_t js_lv_obj_get_style_transform_height(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_transform_height: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_transform_height: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_transform_height(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_transform_pivot_x function
 */
static jerry_value_t js_lv_obj_get_style_transform_pivot_x(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_transform_pivot_x: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_transform_pivot_x: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_transform_pivot_x(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_transform_pivot_y function
 */
static jerry_value_t js_lv_obj_get_style_transform_pivot_y(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_transform_pivot_y: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_transform_pivot_y: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_transform_pivot_y(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_transform_rotation function
 */
static jerry_value_t js_lv_obj_get_style_transform_rotation(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_transform_rotation: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_transform_rotation: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_transform_rotation(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_transform_scale_x function
 */
static jerry_value_t js_lv_obj_get_style_transform_scale_x(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_transform_scale_x: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_transform_scale_x: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_transform_scale_x(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_transform_scale_x_safe function
 */
static jerry_value_t js_lv_obj_get_style_transform_scale_x_safe(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_transform_scale_x_safe: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_transform_scale_x_safe: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_transform_scale_x_safe(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_transform_scale_y function
 */
static jerry_value_t js_lv_obj_get_style_transform_scale_y(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_transform_scale_y: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_transform_scale_y: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_transform_scale_y(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_transform_scale_y_safe function
 */
static jerry_value_t js_lv_obj_get_style_transform_scale_y_safe(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_transform_scale_y_safe: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_transform_scale_y_safe: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_transform_scale_y_safe(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_transform_skew_x function
 */
static jerry_value_t js_lv_obj_get_style_transform_skew_x(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_transform_skew_x: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_transform_skew_x: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_transform_skew_x(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_transform_skew_y function
 */
static jerry_value_t js_lv_obj_get_style_transform_skew_y(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_transform_skew_y: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_transform_skew_y: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_transform_skew_y(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_transform_width function
 */
static jerry_value_t js_lv_obj_get_style_transform_width(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_transform_width: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_transform_width: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_transform_width(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_transition function
 */
static jerry_value_t js_lv_obj_get_style_transition(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_transition: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_transition: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    const lv_style_transition_dsc_t* ret_value = lv_obj_get_style_transition(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

    return js_result;
}



/**
 * @brief lv_obj_get_style_translate_x function
 */
static jerry_value_t js_lv_obj_get_style_translate_x(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_translate_x: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_translate_x: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_translate_x(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_translate_y function
 */
static jerry_value_t js_lv_obj_get_style_translate_y(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_translate_y: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_translate_y: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_translate_y(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_width function
 */
static jerry_value_t js_lv_obj_get_style_width(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_width: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_width: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_width(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_x function
 */
static jerry_value_t js_lv_obj_get_style_x(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_x: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_x: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_x(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_get_style_y function
 */
static jerry_value_t js_lv_obj_get_style_y(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_get_style_y: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_get_style_y: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 调用底层函数
    int32_t ret_value = lv_obj_get_style_y(arg_obj, arg_part);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Transform an area using the angle and zoom style properties of an object
 */
static jerry_value_t js_lv_obj_get_transformed_area(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_get_transformed_area: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: area (lv_area_t*)
    // 指针类型参数，支持null
    void* arg_area = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 解析参数: flags (lv_obj_point_transform_flag_t)
    jerry_value_t js_arg_flags = args[2];
    if (!jerry_value_is_number(js_arg_flags)) {
        return script_engine_throw_error("lv_obj_get_transformed_area: Argument 2 must be a number");
    }

    lv_obj_point_transform_flag_t arg_flags = (lv_obj_point_transform_flag_t)jerry_value_as_number(js_arg_flags);

    // 调用底层函数
    lv_obj_get_transformed_area(arg_obj, arg_area, arg_flags);

    return jerry_undefined();
}



/**
 * @brief Get the user_data field of the object the pointer to the user_data of the object
 */
static jerry_value_t js_lv_obj_get_user_data(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_user_data: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    void* ret_value = lv_obj_get_user_data(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为通用指针对象
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

    return js_result;
}



/**
 * @brief Get the width of an object The position of the object is recalculated only on the next redraw. To force coordinate recalculation call lv_obj_update_layout(obj) .  the width in pixels
 */
static jerry_value_t js_lv_obj_get_width(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_width: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_obj_get_width(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the x coordinate of object. distance of obj from the left side of its parent plus the parent's left padding  The position of the object is recalculated only on the next redraw. To force coordinate recalculation call lv_obj_update_layout(obj) .  Zero return value means the object is on the left padding of the parent, and not on the left edge.  Scrolling of the parent doesn't change the returned value.  The returned value is always the distance from the parent even if obj is positioned by a layout.
 */
static jerry_value_t js_lv_obj_get_x(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_x: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_obj_get_x(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the x2 coordinate of object. distance of obj from the right side of its parent plus the parent's right padding  The position of the object is recalculated only on the next redraw. To force coordinate recalculation call lv_obj_update_layout(obj) .  Zero return value means the object is on the right padding of the parent, and not on the right edge.  Scrolling of the parent doesn't change the returned value.  The returned value is always the distance from the parent even if obj is positioned by a layout.
 */
static jerry_value_t js_lv_obj_get_x2(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_x2: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_obj_get_x2(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the actually set x coordinate of object, i.e. the offset from the set alignment the set x coordinate
 */
static jerry_value_t js_lv_obj_get_x_aligned(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_x_aligned: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_obj_get_x_aligned(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the y coordinate of object. distance of obj from the top side of its parent plus the parent's top padding  The position of the object is recalculated only on the next redraw. To force coordinate recalculation call lv_obj_update_layout(obj) .  Zero return value means the object is on the top padding of the parent, and not on the top edge.  Scrolling of the parent doesn't change the returned value.  The returned value is always the distance from the parent even if obj is positioned by a layout.
 */
static jerry_value_t js_lv_obj_get_y(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_y: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_obj_get_y(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the y2 coordinate of object. distance of obj from the bottom side of its parent plus the parent's bottom padding  The position of the object is recalculated only on the next redraw. To force coordinate recalculation call lv_obj_update_layout(obj) .  Zero return value means the object is on the bottom padding of the parent, and not on the bottom edge.  Scrolling of the parent doesn't change the returned value.  The returned value is always the distance from the parent even if obj is positioned by a layout.
 */
static jerry_value_t js_lv_obj_get_y2(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_y2: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_obj_get_y2(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the actually set y coordinate of object, i.e. the offset from the set alignment the set y coordinate
 */
static jerry_value_t js_lv_obj_get_y_aligned(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_get_y_aligned: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_obj_get_y_aligned(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Check if any object has a given class (type). It checks the ancestor classes too. true: obj has the given class
 */
static jerry_value_t js_lv_obj_has_class(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_has_class: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: class_p (const lv_obj_class_t*)
    // 指针类型参数，支持null
    void* arg_class_p = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 调用底层函数
    bool ret_value = lv_obj_has_class(arg_obj, arg_class_p);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Check if a given flag or all the given flags are set on an object. true: all flags are set; false: not all flags are set
 */
static jerry_value_t js_lv_obj_has_flag(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_has_flag: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: f (lv_obj_flag_t)
    jerry_value_t js_arg_f = args[1];
    if (!jerry_value_is_number(js_arg_f)) {
        return script_engine_throw_error("lv_obj_has_flag: Argument 1 must be a number");
    }

    int arg_f = (int)jerry_value_as_number(js_arg_f);

    // 调用底层函数
    bool ret_value = lv_obj_has_flag(arg_obj, arg_f);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Check if a given flag or any of the flags are set on an object. true: at least one flag is set; false: none of the flags are set
 */
static jerry_value_t js_lv_obj_has_flag_any(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_has_flag_any: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: f (lv_obj_flag_t)
    jerry_value_t js_arg_f = args[1];
    if (!jerry_value_is_number(js_arg_f)) {
        return script_engine_throw_error("lv_obj_has_flag_any: Argument 1 must be a number");
    }

    int arg_f = (int)jerry_value_as_number(js_arg_f);

    // 调用底层函数
    bool ret_value = lv_obj_has_flag_any(arg_obj, arg_f);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Check if the object is in a given state or not. true: obj is in state ; false: obj is not in state
 */
static jerry_value_t js_lv_obj_has_state(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_has_state: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: state (lv_state_t)
    jerry_value_t js_arg_state = args[1];
    if (!jerry_value_is_number(js_arg_state)) {
        return script_engine_throw_error("lv_obj_has_state: Argument 1 must be a number");
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
 * @brief Hit-test an object given a particular point in screen space. true: if the object is considered under the point
 */
static jerry_value_t js_lv_obj_hit_test(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_hit_test: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: point (const lv_point_t*)
    // 指针类型参数，支持null
    void* arg_point = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 调用底层函数
    bool ret_value = lv_obj_hit_test(arg_obj, arg_point);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Initialize an arc draw descriptor from an object's styles in its current state
 */
static jerry_value_t js_lv_obj_init_draw_arc_dsc(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_init_draw_arc_dsc: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_init_draw_arc_dsc: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 解析参数: draw_dsc (lv_draw_arc_dsc_t*)
    // 指针类型参数，支持null
    void* arg_draw_dsc = lv_js_bridge_obj_2_ptr(args[2],LV_TYPE_ANY);
    // 调用底层函数
    lv_obj_init_draw_arc_dsc(arg_obj, arg_part, arg_draw_dsc);

    return jerry_undefined();
}



/**
 * @brief Initialize an image draw descriptor from an object's styles in its current state
 */
static jerry_value_t js_lv_obj_init_draw_image_dsc(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_init_draw_image_dsc: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_init_draw_image_dsc: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 解析参数: draw_dsc (lv_draw_image_dsc_t*)
    // 指针类型参数，支持null
    void* arg_draw_dsc = lv_js_bridge_obj_2_ptr(args[2],LV_TYPE_ANY);
    // 调用底层函数
    lv_obj_init_draw_image_dsc(arg_obj, arg_part, arg_draw_dsc);

    return jerry_undefined();
}



/**
 * @brief Initialize a label draw descriptor from an object's styles in its current state
 */
static jerry_value_t js_lv_obj_init_draw_label_dsc(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_init_draw_label_dsc: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_init_draw_label_dsc: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 解析参数: draw_dsc (lv_draw_label_dsc_t*)
    // 指针类型参数，支持null
    void* arg_draw_dsc = lv_js_bridge_obj_2_ptr(args[2],LV_TYPE_ANY);
    // 调用底层函数
    lv_obj_init_draw_label_dsc(arg_obj, arg_part, arg_draw_dsc);

    return jerry_undefined();
}



/**
 * @brief Initialize a line draw descriptor from an object's styles in its current state
 */
static jerry_value_t js_lv_obj_init_draw_line_dsc(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_init_draw_line_dsc: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_init_draw_line_dsc: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 解析参数: draw_dsc (lv_draw_line_dsc_t*)
    // 指针类型参数，支持null
    void* arg_draw_dsc = lv_js_bridge_obj_2_ptr(args[2],LV_TYPE_ANY);
    // 调用底层函数
    lv_obj_init_draw_line_dsc(arg_obj, arg_part, arg_draw_dsc);

    return jerry_undefined();
}



/**
 * @brief Initialize a rectangle draw descriptor from an object's styles in its current state Only the relevant fields will be set. E.g. if border width == 0 the other border properties won't be evaluated.
 */
static jerry_value_t js_lv_obj_init_draw_rect_dsc(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_init_draw_rect_dsc: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_init_draw_rect_dsc: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 解析参数: draw_dsc (lv_draw_rect_dsc_t*)
    // 指针类型参数，支持null
    void* arg_draw_dsc = lv_js_bridge_obj_2_ptr(args[2],LV_TYPE_ANY);
    // 调用底层函数
    lv_obj_init_draw_rect_dsc(arg_obj, arg_part, arg_draw_dsc);

    return jerry_undefined();
}



/**
 * @brief Mark the object as invalid to redrawn its area
 */
static jerry_value_t js_lv_obj_invalidate(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_invalidate: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_invalidate(arg_obj);

    return jerry_undefined();
}



/**
 * @brief Mark an area of an object as invalid. The area will be truncated to the object's area and marked for redraw.
 */
static jerry_value_t js_lv_obj_invalidate_area(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_invalidate_area: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: area (const lv_area_t*)
    // 指针类型参数，支持null
    void* arg_area = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 调用底层函数
    lv_obj_invalidate_area(arg_obj, arg_area);

    return jerry_undefined();
}



/**
 * @brief lv_obj_is_editable function
 */
static jerry_value_t js_lv_obj_is_editable(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_is_editable: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    bool ret_value = lv_obj_is_editable(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_is_group_def function
 */
static jerry_value_t js_lv_obj_is_group_def(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_is_group_def: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    bool ret_value = lv_obj_is_group_def(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Test whether the and object is positioned by a layout or not true: positioned by a layout; false: not positioned by a layout
 */
static jerry_value_t js_lv_obj_is_layout_positioned(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_is_layout_positioned: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    bool ret_value = lv_obj_is_layout_positioned(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Tell whether an object is being scrolled or not at this moment true: obj is being scrolled
 */
static jerry_value_t js_lv_obj_is_scrolling(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_is_scrolling: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    bool ret_value = lv_obj_is_scrolling(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Check if any object is still "alive". true: valid
 */
static jerry_value_t js_lv_obj_is_valid(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_is_valid: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    bool ret_value = lv_obj_is_valid(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Tell whether an object is visible (even partially) now or not true: visible; false not visible (hidden, out of parent, on other screen, etc)
 */
static jerry_value_t js_lv_obj_is_visible(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_is_visible: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    bool ret_value = lv_obj_is_visible(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Mark the object for layout update.
 */
static jerry_value_t js_lv_obj_mark_layout_as_dirty(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_mark_layout_as_dirty: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_mark_layout_as_dirty(arg_obj);

    return jerry_undefined();
}



/**
 * @brief Move the object to the background. It will look like if it was created as the first child of its parent. It also means any of the siblings can cover the object.
 */
static jerry_value_t js_lv_obj_move_background(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_move_background: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_move_background(arg_obj);

    return jerry_undefined();
}



/**
 * @brief lv_obj_move_children_by function
 */
static jerry_value_t js_lv_obj_move_children_by(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 4) {
        return script_engine_throw_error("lv_obj_move_children_by: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: x_diff (int32_t)
    jerry_value_t js_arg_x_diff = args[1];
    if (!jerry_value_is_number(js_arg_x_diff)) {
        return script_engine_throw_error("lv_obj_move_children_by: Argument 1 must be a number");
    }

    int32_t arg_x_diff = (int32_t)jerry_value_as_number(js_arg_x_diff);

    // 解析参数: y_diff (int32_t)
    jerry_value_t js_arg_y_diff = args[2];
    if (!jerry_value_is_number(js_arg_y_diff)) {
        return script_engine_throw_error("lv_obj_move_children_by: Argument 2 must be a number");
    }

    int32_t arg_y_diff = (int32_t)jerry_value_as_number(js_arg_y_diff);

    // 解析参数: ignore_floating (bool)
    // 布尔类型参数: ignore_floating
    bool arg_ignore_floating = false;
    if (!jerry_value_is_undefined(args[3])) {
        if (jerry_value_is_boolean(args[3])) {
            arg_ignore_floating = jerry_value_to_boolean(args[3]);
        }
        else if (jerry_value_is_number(args[3])) {
            arg_ignore_floating = (jerry_value_as_number(args[3]) != 0);
        }
        else {
            return script_engine_throw_error("lv_obj_move_children_by: Argument 3 must be boolean or number for bool");
        }
    }

    // 调用底层函数
    lv_obj_move_children_by(arg_obj, arg_x_diff, arg_y_diff, arg_ignore_floating);

    return jerry_undefined();
}



/**
 * @brief Move the object to the foreground. It will look like if it was created as the last child of its parent. It also means it can cover any of the siblings.
 */
static jerry_value_t js_lv_obj_move_foreground(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_move_foreground: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_move_foreground(arg_obj);

    return jerry_undefined();
}



/**
 * @brief lv_obj_move_to function
 */
static jerry_value_t js_lv_obj_move_to(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_move_to: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: x (int32_t)
    jerry_value_t js_arg_x = args[1];
    if (!jerry_value_is_number(js_arg_x)) {
        return script_engine_throw_error("lv_obj_move_to: Argument 1 must be a number");
    }

    int32_t arg_x = (int32_t)jerry_value_as_number(js_arg_x);

    // 解析参数: y (int32_t)
    jerry_value_t js_arg_y = args[2];
    if (!jerry_value_is_number(js_arg_y)) {
        return script_engine_throw_error("lv_obj_move_to: Argument 2 must be a number");
    }

    int32_t arg_y = (int32_t)jerry_value_as_number(js_arg_y);

    // 调用底层函数
    lv_obj_move_to(arg_obj, arg_x, arg_y);

    return jerry_undefined();
}



/**
 * @brief moves the object to the given index in its parent. When used in listboxes, it can be used to sort the listbox items. to move to the background: lv_obj_move_to_index(obj, 0)  to move forward (up): lv_obj_move_to_index(obj, lv_obj_get_index(obj) - 1)
 */
static jerry_value_t js_lv_obj_move_to_index(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_move_to_index: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: index (int32_t)
    jerry_value_t js_arg_index = args[1];
    if (!jerry_value_is_number(js_arg_index)) {
        return script_engine_throw_error("lv_obj_move_to_index: Argument 1 must be a number");
    }

    int32_t arg_index = (int32_t)jerry_value_as_number(js_arg_index);

    // 调用底层函数
    lv_obj_move_to_index(arg_obj, arg_index);

    return jerry_undefined();
}



/**
 * @brief Utility to set an object reference to NULL when it gets deleted. The reference should be in a location that will not become invalid during the object's lifetime, i.e. static or allocated.
 */
static jerry_value_t js_lv_obj_null_on_delete(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_null_on_delete: Insufficient arguments");
    }

    // 解析参数: obj_ptr (lv_obj_t**)
    // 指针类型参数，支持null
    void* arg_obj_ptr = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_ANY);
    // 调用底层函数
    lv_obj_null_on_delete(arg_obj_ptr);

    return jerry_undefined();
}



/**
 * @brief Checks if the content is scrolled "in" and adjusts it to a normal position.
 */
static jerry_value_t js_lv_obj_readjust_scroll(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_readjust_scroll: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: anim_en (lv_anim_enable_t)
    jerry_value_t js_arg_anim_en = args[1];
    if (!jerry_value_is_number(js_arg_anim_en)) {
        return script_engine_throw_error("lv_obj_readjust_scroll: Argument 1 must be a number");
    }

    int arg_anim_en = (int)jerry_value_as_number(js_arg_anim_en);

    // 调用底层函数
    lv_obj_readjust_scroll(arg_obj, arg_anim_en);

    return jerry_undefined();
}



/**
 * @brief Redrawn on object and all its children using the passed draw context
 */
static jerry_value_t js_lv_obj_redraw(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_redraw: Insufficient arguments");
    }

    // 解析参数: layer (lv_layer_t*)
    // 指针类型参数，支持null
    void* arg_layer = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_ANY);
    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_redraw(arg_layer, arg_obj);

    return jerry_undefined();
}



/**
 * @brief lv_obj_refr_pos function
 */
static jerry_value_t js_lv_obj_refr_pos(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_refr_pos: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_refr_pos(arg_obj);

    return jerry_undefined();
}



/**
 * @brief Recalculate the size of the object true: the size has been changed
 */
static jerry_value_t js_lv_obj_refr_size(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_refr_size: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    bool ret_value = lv_obj_refr_size(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Send a 'LV_EVENT_REFR_EXT_DRAW_SIZE' Call the ancestor's event handler to the object to refresh the value of the extended draw size. The result will be saved in obj .
 */
static jerry_value_t js_lv_obj_refresh_ext_draw_size(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_refresh_ext_draw_size: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_refresh_ext_draw_size(arg_obj);

    return jerry_undefined();
}



/**
 * @brief Handle if the size of the internal ("virtual") content of an object has changed. false: nothing happened; true: refresh happened
 */
static jerry_value_t js_lv_obj_refresh_self_size(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_refresh_self_size: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    bool ret_value = lv_obj_refresh_self_size(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Notify an object and its children about its style is modified.
 */
static jerry_value_t js_lv_obj_refresh_style(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_refresh_style: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: part (lv_part_t)
    jerry_value_t js_arg_part = args[1];
    if (!jerry_value_is_number(js_arg_part)) {
        return script_engine_throw_error("lv_obj_refresh_style: Argument 1 must be a number");
    }

    uint32_t arg_part = (uint32_t)jerry_value_as_number(js_arg_part);

    // 解析参数: prop (lv_style_prop_t)
    jerry_value_t js_arg_prop = args[2];
    if (!jerry_value_is_number(js_arg_prop)) {
        return script_engine_throw_error("lv_obj_refresh_style: Argument 2 must be a number");
    }

    uint8_t arg_prop = (uint8_t)jerry_value_as_number(js_arg_prop);

    // 调用底层函数
    lv_obj_refresh_style(arg_obj, arg_part, arg_prop);

    return jerry_undefined();
}



/**
 * @brief lv_obj_remove_event function
 */
static jerry_value_t js_lv_obj_remove_event(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_remove_event: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: index (uint32_t)
    jerry_value_t js_arg_index = args[1];
    if (!jerry_value_is_number(js_arg_index)) {
        return script_engine_throw_error("lv_obj_remove_event: Argument 1 must be a number");
    }

    uint32_t arg_index = (uint32_t)jerry_value_as_number(js_arg_index);

    // 调用底层函数
    bool ret_value = lv_obj_remove_event(arg_obj, arg_index);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_remove_event_cb function
 */
static jerry_value_t js_lv_obj_remove_event_cb(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_remove_event_cb: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: event_cb (lv_event_cb_t)
    // 通用指针类型: lv_event_cb_t，支持null
    lv_event_cb_t arg_event_cb = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);

    // 调用底层函数
    bool ret_value = lv_obj_remove_event_cb(arg_obj, arg_event_cb);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Remove an event_cb with user_data the count of the event removed
 */
static jerry_value_t js_lv_obj_remove_event_cb_with_user_data(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_remove_event_cb_with_user_data: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: event_cb (lv_event_cb_t)
    // 通用指针类型: lv_event_cb_t，支持null
    lv_event_cb_t arg_event_cb = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);

    // 解析参数: user_data (void*)
    // void*/字符串 类型参数，支持null
    void* arg_user_data = NULL;
    char* arg_user_data_str = NULL;  // 用于字符串参数的临时存储

    if (!jerry_value_is_undefined(args[2]) && !jerry_value_is_null(args[2])) {
        if (jerry_value_is_string(args[2])) {
            // 处理字符串类型的符号（如LV_SYMBOL_MINUS）
            jerry_size_t arg_user_data_len = jerry_string_size(args[2], JERRY_ENCODING_UTF8);
            arg_user_data_str = (char*)eos_malloc(arg_user_data_len + 1);
            if (!arg_user_data_str) {
                return script_engine_throw_error("lv_obj_remove_event_cb_with_user_data: Failed to allocate memory for string argument");
            }
            jerry_string_to_buffer(args[2], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_user_data_str, arg_user_data_len);
            arg_user_data_str[arg_user_data_len] = '\0';
            arg_user_data = (void*)arg_user_data_str;
        }
        else if (jerry_value_is_object(args[2])) {
            // 尝试从对象获取指针
            arg_user_data = lv_js_bridge_obj_2_ptr(args[2],LV_TYPE_ANY);
        }
        else if (jerry_value_is_number(args[2])) {
            // 直接传递指针数值
            uintptr_t ptr_num = (uintptr_t)jerry_value_as_number(args[2]);
            arg_user_data = (void*)ptr_num;
        }
        else {
            return script_engine_throw_error("lv_obj_remove_event_cb_with_user_data: Argument 2 must be string, object or number");
        }
    }
    // 调用底层函数
    uint32_t ret_value = lv_obj_remove_event_cb_with_user_data(arg_obj, arg_event_cb, arg_user_data);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    // 释放临时内存
    if (arg_user_data_str) eos_free(arg_user_data_str);

    return js_result;
}



/**
 * @brief lv_obj_remove_event_dsc function
 */
static jerry_value_t js_lv_obj_remove_event_dsc(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_remove_event_dsc: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: dsc (lv_event_dsc_t*)
    // 指针类型参数，支持null
    void* arg_dsc = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 调用底层函数
    bool ret_value = lv_obj_remove_event_dsc(arg_obj, arg_dsc);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Remove one or more flags
 */
static jerry_value_t js_lv_obj_remove_flag(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_remove_flag: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: f (lv_obj_flag_t)
    jerry_value_t js_arg_f = args[1];
    if (!jerry_value_is_number(js_arg_f)) {
        return script_engine_throw_error("lv_obj_remove_flag: Argument 1 must be a number");
    }

    int arg_f = (int)jerry_value_as_number(js_arg_f);

    // 调用底层函数
    lv_obj_remove_flag(arg_obj, arg_f);

    return jerry_undefined();
}



/**
 * @brief Remove the observers of an object from a subject or all subjects This function can be used e.g. when an object's subject(s) needs to be replaced by other subject(s)
 */
static jerry_value_t js_lv_obj_remove_from_subject(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_remove_from_subject: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: subject (lv_subject_t*)
    // 指针类型参数，支持null
    void* arg_subject = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 调用底层函数
    lv_obj_remove_from_subject(arg_obj, arg_subject);

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
        return script_engine_throw_error("lv_obj_remove_state: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: state (lv_state_t)
    jerry_value_t js_arg_state = args[1];
    if (!jerry_value_is_number(js_arg_state)) {
        return script_engine_throw_error("lv_obj_remove_state: Argument 1 must be a number");
    }

    uint16_t arg_state = (uint16_t)jerry_value_as_number(js_arg_state);

    // 调用底层函数
    lv_obj_remove_state(arg_obj, arg_state);

    return jerry_undefined();
}



/**
 * @brief Remove a style from an object. lv_obj_remove_style(obj, &style, LV_PART_ANY | LV_STATE_ANY); //Remove a specific style lv_obj_remove_style(obj, NULL, LV_PART_MAIN | LV_STATE_ANY); //Remove all styles from the main part  lv_obj_remove_style(obj, NULL, LV_PART_ANY | LV_STATE_ANY); //Remove all styles
 */
static jerry_value_t js_lv_obj_remove_style(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_remove_style: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: style (const lv_style_t*)
    // 指针类型参数，支持null
    void* arg_style = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_remove_style: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_remove_style(arg_obj, arg_style, arg_selector);

    return jerry_undefined();
}



/**
 * @brief Remove all styles from an object
 */
static jerry_value_t js_lv_obj_remove_style_all(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_remove_style_all: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_remove_style_all(arg_obj);

    return jerry_undefined();
}



/**
 * @brief Replaces a style of an object, preserving the order of the style stack (local styles and transitions are ignored). lv_obj_replace_style(obj, &yellow_style, &blue_style, LV_PART_ANY | LV_STATE_ANY); //Replace a specific style lv_obj_replace_style(obj, &yellow_style, &blue_style, LV_PART_MAIN | LV_STATE_PRESSED); //Replace a specific style assigned to the main part when it is pressed
 */
static jerry_value_t js_lv_obj_replace_style(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 4) {
        return script_engine_throw_error("lv_obj_replace_style: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: old_style (const lv_style_t*)
    // 指针类型参数，支持null
    void* arg_old_style = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 解析参数: new_style (const lv_style_t*)
    // 指针类型参数，支持null
    void* arg_new_style = lv_js_bridge_obj_2_ptr(args[2],LV_TYPE_ANY);
    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[3];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_replace_style: Argument 3 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    bool ret_value = lv_obj_replace_style(arg_obj, arg_old_style, arg_new_style, arg_selector);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Notify all object if a style is modified
 */
static jerry_value_t js_lv_obj_report_style_change(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_report_style_change: Insufficient arguments");
    }

    // 解析参数: style (lv_style_t*)
    // 指针类型参数，支持null
    void* arg_style = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_ANY);
    // 调用底层函数
    lv_obj_report_style_change(arg_style);

    return jerry_undefined();
}



/**
 * @brief Scroll by a given amount of pixels > 0 value means scroll right/bottom (show the more content on the right/bottom)  e.g. dy = -20 means scroll down 20 px
 */
static jerry_value_t js_lv_obj_scroll_by(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 4) {
        return script_engine_throw_error("lv_obj_scroll_by: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: x (int32_t)
    jerry_value_t js_arg_x = args[1];
    if (!jerry_value_is_number(js_arg_x)) {
        return script_engine_throw_error("lv_obj_scroll_by: Argument 1 must be a number");
    }

    int32_t arg_x = (int32_t)jerry_value_as_number(js_arg_x);

    // 解析参数: y (int32_t)
    jerry_value_t js_arg_y = args[2];
    if (!jerry_value_is_number(js_arg_y)) {
        return script_engine_throw_error("lv_obj_scroll_by: Argument 2 must be a number");
    }

    int32_t arg_y = (int32_t)jerry_value_as_number(js_arg_y);

    // 解析参数: anim_en (lv_anim_enable_t)
    jerry_value_t js_arg_anim_en = args[3];
    if (!jerry_value_is_number(js_arg_anim_en)) {
        return script_engine_throw_error("lv_obj_scroll_by: Argument 3 must be a number");
    }

    int arg_anim_en = (int)jerry_value_as_number(js_arg_anim_en);

    // 调用底层函数
    lv_obj_scroll_by(arg_obj, arg_x, arg_y, arg_anim_en);

    return jerry_undefined();
}



/**
 * @brief Scroll by a given amount of pixels. dx and dy will be limited internally to allow scrolling only on the content area. e.g. dy = -20 means scroll down 20 px
 */
static jerry_value_t js_lv_obj_scroll_by_bounded(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 4) {
        return script_engine_throw_error("lv_obj_scroll_by_bounded: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: dx (int32_t)
    jerry_value_t js_arg_dx = args[1];
    if (!jerry_value_is_number(js_arg_dx)) {
        return script_engine_throw_error("lv_obj_scroll_by_bounded: Argument 1 must be a number");
    }

    int32_t arg_dx = (int32_t)jerry_value_as_number(js_arg_dx);

    // 解析参数: dy (int32_t)
    jerry_value_t js_arg_dy = args[2];
    if (!jerry_value_is_number(js_arg_dy)) {
        return script_engine_throw_error("lv_obj_scroll_by_bounded: Argument 2 must be a number");
    }

    int32_t arg_dy = (int32_t)jerry_value_as_number(js_arg_dy);

    // 解析参数: anim_en (lv_anim_enable_t)
    jerry_value_t js_arg_anim_en = args[3];
    if (!jerry_value_is_number(js_arg_anim_en)) {
        return script_engine_throw_error("lv_obj_scroll_by_bounded: Argument 3 must be a number");
    }

    int arg_anim_en = (int)jerry_value_as_number(js_arg_anim_en);

    // 调用底层函数
    lv_obj_scroll_by_bounded(arg_obj, arg_dx, arg_dy, arg_anim_en);

    return jerry_undefined();
}



/**
 * @brief Scroll to a given coordinate on an object. x and y will be limited internally to allow scrolling only on the content area.
 */
static jerry_value_t js_lv_obj_scroll_to(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 4) {
        return script_engine_throw_error("lv_obj_scroll_to: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: x (int32_t)
    jerry_value_t js_arg_x = args[1];
    if (!jerry_value_is_number(js_arg_x)) {
        return script_engine_throw_error("lv_obj_scroll_to: Argument 1 must be a number");
    }

    int32_t arg_x = (int32_t)jerry_value_as_number(js_arg_x);

    // 解析参数: y (int32_t)
    jerry_value_t js_arg_y = args[2];
    if (!jerry_value_is_number(js_arg_y)) {
        return script_engine_throw_error("lv_obj_scroll_to: Argument 2 must be a number");
    }

    int32_t arg_y = (int32_t)jerry_value_as_number(js_arg_y);

    // 解析参数: anim_en (lv_anim_enable_t)
    jerry_value_t js_arg_anim_en = args[3];
    if (!jerry_value_is_number(js_arg_anim_en)) {
        return script_engine_throw_error("lv_obj_scroll_to: Argument 3 must be a number");
    }

    int arg_anim_en = (int)jerry_value_as_number(js_arg_anim_en);

    // 调用底层函数
    lv_obj_scroll_to(arg_obj, arg_x, arg_y, arg_anim_en);

    return jerry_undefined();
}



/**
 * @brief Scroll to an object until it becomes visible on its parent
 */
static jerry_value_t js_lv_obj_scroll_to_view(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_scroll_to_view: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: anim_en (lv_anim_enable_t)
    jerry_value_t js_arg_anim_en = args[1];
    if (!jerry_value_is_number(js_arg_anim_en)) {
        return script_engine_throw_error("lv_obj_scroll_to_view: Argument 1 must be a number");
    }

    int arg_anim_en = (int)jerry_value_as_number(js_arg_anim_en);

    // 调用底层函数
    lv_obj_scroll_to_view(arg_obj, arg_anim_en);

    return jerry_undefined();
}



/**
 * @brief Scroll to an object until it becomes visible on its parent. Do the same on the parent's parent, and so on. Therefore the object will be scrolled into view even it has nested scrollable parents
 */
static jerry_value_t js_lv_obj_scroll_to_view_recursive(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_scroll_to_view_recursive: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: anim_en (lv_anim_enable_t)
    jerry_value_t js_arg_anim_en = args[1];
    if (!jerry_value_is_number(js_arg_anim_en)) {
        return script_engine_throw_error("lv_obj_scroll_to_view_recursive: Argument 1 must be a number");
    }

    int arg_anim_en = (int)jerry_value_as_number(js_arg_anim_en);

    // 调用底层函数
    lv_obj_scroll_to_view_recursive(arg_obj, arg_anim_en);

    return jerry_undefined();
}



/**
 * @brief Scroll to a given X coordinate on an object. x will be limited internally to allow scrolling only on the content area.
 */
static jerry_value_t js_lv_obj_scroll_to_x(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_scroll_to_x: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: x (int32_t)
    jerry_value_t js_arg_x = args[1];
    if (!jerry_value_is_number(js_arg_x)) {
        return script_engine_throw_error("lv_obj_scroll_to_x: Argument 1 must be a number");
    }

    int32_t arg_x = (int32_t)jerry_value_as_number(js_arg_x);

    // 解析参数: anim_en (lv_anim_enable_t)
    jerry_value_t js_arg_anim_en = args[2];
    if (!jerry_value_is_number(js_arg_anim_en)) {
        return script_engine_throw_error("lv_obj_scroll_to_x: Argument 2 must be a number");
    }

    int arg_anim_en = (int)jerry_value_as_number(js_arg_anim_en);

    // 调用底层函数
    lv_obj_scroll_to_x(arg_obj, arg_x, arg_anim_en);

    return jerry_undefined();
}



/**
 * @brief Scroll to a given Y coordinate on an object y will be limited internally to allow scrolling only on the content area.
 */
static jerry_value_t js_lv_obj_scroll_to_y(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_scroll_to_y: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: y (int32_t)
    jerry_value_t js_arg_y = args[1];
    if (!jerry_value_is_number(js_arg_y)) {
        return script_engine_throw_error("lv_obj_scroll_to_y: Argument 1 must be a number");
    }

    int32_t arg_y = (int32_t)jerry_value_as_number(js_arg_y);

    // 解析参数: anim_en (lv_anim_enable_t)
    jerry_value_t js_arg_anim_en = args[2];
    if (!jerry_value_is_number(js_arg_anim_en)) {
        return script_engine_throw_error("lv_obj_scroll_to_y: Argument 2 must be a number");
    }

    int arg_anim_en = (int)jerry_value_as_number(js_arg_anim_en);

    // 调用底层函数
    lv_obj_scroll_to_y(arg_obj, arg_y, arg_anim_en);

    return jerry_undefined();
}



/**
 * @brief Invalidate the area of the scrollbars
 */
static jerry_value_t js_lv_obj_scrollbar_invalidate(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_scrollbar_invalidate: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_scrollbar_invalidate(arg_obj);

    return jerry_undefined();
}



/**
 * @brief Send an event to the object LV_RESULT_OK: obj was not deleted in the event; LV_RESULT_INVALID: obj was deleted in the event_code
 */
static jerry_value_t js_lv_obj_send_event(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_send_event: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: event_code (lv_event_code_t)
    jerry_value_t js_arg_event_code = args[1];
    if (!jerry_value_is_number(js_arg_event_code)) {
        return script_engine_throw_error("lv_obj_send_event: Argument 1 must be a number");
    }

    int arg_event_code = (int)jerry_value_as_number(js_arg_event_code);

    // 解析参数: param (void*)
    // void*/字符串 类型参数，支持null
    void* arg_param = NULL;
    char* arg_param_str = NULL;  // 用于字符串参数的临时存储

    if (!jerry_value_is_undefined(args[2]) && !jerry_value_is_null(args[2])) {
        if (jerry_value_is_string(args[2])) {
            // 处理字符串类型的符号（如LV_SYMBOL_MINUS）
            jerry_size_t arg_param_len = jerry_string_size(args[2], JERRY_ENCODING_UTF8);
            arg_param_str = (char*)eos_malloc(arg_param_len + 1);
            if (!arg_param_str) {
                return script_engine_throw_error("lv_obj_send_event: Failed to allocate memory for string argument");
            }
            jerry_string_to_buffer(args[2], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_param_str, arg_param_len);
            arg_param_str[arg_param_len] = '\0';
            arg_param = (void*)arg_param_str;
        }
        else if (jerry_value_is_object(args[2])) {
            // 尝试从对象获取指针
            arg_param = lv_js_bridge_obj_2_ptr(args[2],LV_TYPE_ANY);
        }
        else if (jerry_value_is_number(args[2])) {
            // 直接传递指针数值
            uintptr_t ptr_num = (uintptr_t)jerry_value_as_number(args[2]);
            arg_param = (void*)ptr_num;
        }
        else {
            return script_engine_throw_error("lv_obj_send_event: Argument 2 must be string, object or number");
        }
    }
    // 调用底层函数
    lv_result_t ret_value = lv_obj_send_event(arg_obj, arg_event_code, arg_param);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    // 释放临时内存
    if (arg_param_str) eos_free(arg_param_str);

    return js_result;
}



/**
 * @brief Change the alignment of an object.
 */
static jerry_value_t js_lv_obj_set_align(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_set_align: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: align (lv_align_t)
    jerry_value_t js_arg_align = args[1];
    if (!jerry_value_is_number(js_arg_align)) {
        return script_engine_throw_error("lv_obj_set_align: Argument 1 must be a number");
    }

    int arg_align = (int)jerry_value_as_number(js_arg_align);

    // 调用底层函数
    lv_obj_set_align(arg_obj, arg_align);

    return jerry_undefined();
}



/**
 * @brief Set the height reduced by the top and bottom padding and the border width.
 */
static jerry_value_t js_lv_obj_set_content_height(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_set_content_height: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: h (int32_t)
    jerry_value_t js_arg_h = args[1];
    if (!jerry_value_is_number(js_arg_h)) {
        return script_engine_throw_error("lv_obj_set_content_height: Argument 1 must be a number");
    }

    int32_t arg_h = (int32_t)jerry_value_as_number(js_arg_h);

    // 调用底层函数
    lv_obj_set_content_height(arg_obj, arg_h);

    return jerry_undefined();
}



/**
 * @brief Set the width reduced by the left and right padding and the border width.
 */
static jerry_value_t js_lv_obj_set_content_width(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_set_content_width: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: w (int32_t)
    jerry_value_t js_arg_w = args[1];
    if (!jerry_value_is_number(js_arg_w)) {
        return script_engine_throw_error("lv_obj_set_content_width: Argument 1 must be a number");
    }

    int32_t arg_w = (int32_t)jerry_value_as_number(js_arg_w);

    // 调用底层函数
    lv_obj_set_content_width(arg_obj, arg_w);

    return jerry_undefined();
}



/**
 * @brief Set the size of an extended clickable area
 */
static jerry_value_t js_lv_obj_set_ext_click_area(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_set_ext_click_area: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: size (int32_t)
    jerry_value_t js_arg_size = args[1];
    if (!jerry_value_is_number(js_arg_size)) {
        return script_engine_throw_error("lv_obj_set_ext_click_area: Argument 1 must be a number");
    }

    int32_t arg_size = (int32_t)jerry_value_as_number(js_arg_size);

    // 调用底层函数
    lv_obj_set_ext_click_area(arg_obj, arg_size);

    return jerry_undefined();
}



/**
 * @brief Set how to place (where to align) the items and tracks
 */
static jerry_value_t js_lv_obj_set_flex_align(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 4) {
        return script_engine_throw_error("lv_obj_set_flex_align: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: main_place (lv_flex_align_t)
    jerry_value_t js_arg_main_place = args[1];
    if (!jerry_value_is_number(js_arg_main_place)) {
        return script_engine_throw_error("lv_obj_set_flex_align: Argument 1 must be a number");
    }

    int arg_main_place = (int)jerry_value_as_number(js_arg_main_place);

    // 解析参数: cross_place (lv_flex_align_t)
    jerry_value_t js_arg_cross_place = args[2];
    if (!jerry_value_is_number(js_arg_cross_place)) {
        return script_engine_throw_error("lv_obj_set_flex_align: Argument 2 must be a number");
    }

    int arg_cross_place = (int)jerry_value_as_number(js_arg_cross_place);

    // 解析参数: track_cross_place (lv_flex_align_t)
    jerry_value_t js_arg_track_cross_place = args[3];
    if (!jerry_value_is_number(js_arg_track_cross_place)) {
        return script_engine_throw_error("lv_obj_set_flex_align: Argument 3 must be a number");
    }

    int arg_track_cross_place = (int)jerry_value_as_number(js_arg_track_cross_place);

    // 调用底层函数
    lv_obj_set_flex_align(arg_obj, arg_main_place, arg_cross_place, arg_track_cross_place);

    return jerry_undefined();
}



/**
 * @brief Set how the item should flow
 */
static jerry_value_t js_lv_obj_set_flex_flow(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_set_flex_flow: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: flow (lv_flex_flow_t)
    jerry_value_t js_arg_flow = args[1];
    if (!jerry_value_is_number(js_arg_flow)) {
        return script_engine_throw_error("lv_obj_set_flex_flow: Argument 1 must be a number");
    }

    int arg_flow = (int)jerry_value_as_number(js_arg_flow);

    // 调用底层函数
    lv_obj_set_flex_flow(arg_obj, arg_flow);

    return jerry_undefined();
}



/**
 * @brief Sets the width or height (on main axis) to grow the object in order fill the free space
 */
static jerry_value_t js_lv_obj_set_flex_grow(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_set_flex_grow: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: grow (uint8_t)
    jerry_value_t js_arg_grow = args[1];
    if (!jerry_value_is_number(js_arg_grow)) {
        return script_engine_throw_error("lv_obj_set_flex_grow: Argument 1 must be a number");
    }

    uint8_t arg_grow = (uint8_t)jerry_value_as_number(js_arg_grow);

    // 调用底层函数
    lv_obj_set_flex_grow(arg_obj, arg_grow);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_grid_align function
 */
static jerry_value_t js_lv_obj_set_grid_align(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_grid_align: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: column_align (lv_grid_align_t)
    jerry_value_t js_arg_column_align = args[1];
    if (!jerry_value_is_number(js_arg_column_align)) {
        return script_engine_throw_error("lv_obj_set_grid_align: Argument 1 must be a number");
    }

    int arg_column_align = (int)jerry_value_as_number(js_arg_column_align);

    // 解析参数: row_align (lv_grid_align_t)
    jerry_value_t js_arg_row_align = args[2];
    if (!jerry_value_is_number(js_arg_row_align)) {
        return script_engine_throw_error("lv_obj_set_grid_align: Argument 2 must be a number");
    }

    int arg_row_align = (int)jerry_value_as_number(js_arg_row_align);

    // 调用底层函数
    lv_obj_set_grid_align(arg_obj, arg_column_align, arg_row_align);

    return jerry_undefined();
}



/**
 * @brief Set the cell of an object. The object's parent needs to have grid layout, else nothing will happen
 */
static jerry_value_t js_lv_obj_set_grid_cell(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 7) {
        return script_engine_throw_error("lv_obj_set_grid_cell: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: column_align (lv_grid_align_t)
    jerry_value_t js_arg_column_align = args[1];
    if (!jerry_value_is_number(js_arg_column_align)) {
        return script_engine_throw_error("lv_obj_set_grid_cell: Argument 1 must be a number");
    }

    int arg_column_align = (int)jerry_value_as_number(js_arg_column_align);

    // 解析参数: col_pos (int32_t)
    jerry_value_t js_arg_col_pos = args[2];
    if (!jerry_value_is_number(js_arg_col_pos)) {
        return script_engine_throw_error("lv_obj_set_grid_cell: Argument 2 must be a number");
    }

    int32_t arg_col_pos = (int32_t)jerry_value_as_number(js_arg_col_pos);

    // 解析参数: col_span (int32_t)
    jerry_value_t js_arg_col_span = args[3];
    if (!jerry_value_is_number(js_arg_col_span)) {
        return script_engine_throw_error("lv_obj_set_grid_cell: Argument 3 must be a number");
    }

    int32_t arg_col_span = (int32_t)jerry_value_as_number(js_arg_col_span);

    // 解析参数: row_align (lv_grid_align_t)
    jerry_value_t js_arg_row_align = args[4];
    if (!jerry_value_is_number(js_arg_row_align)) {
        return script_engine_throw_error("lv_obj_set_grid_cell: Argument 4 must be a number");
    }

    int arg_row_align = (int)jerry_value_as_number(js_arg_row_align);

    // 解析参数: row_pos (int32_t)
    jerry_value_t js_arg_row_pos = args[5];
    if (!jerry_value_is_number(js_arg_row_pos)) {
        return script_engine_throw_error("lv_obj_set_grid_cell: Argument 5 must be a number");
    }

    int32_t arg_row_pos = (int32_t)jerry_value_as_number(js_arg_row_pos);

    // 解析参数: row_span (int32_t)
    jerry_value_t js_arg_row_span = args[6];
    if (!jerry_value_is_number(js_arg_row_span)) {
        return script_engine_throw_error("lv_obj_set_grid_cell: Argument 6 must be a number");
    }

    int32_t arg_row_span = (int32_t)jerry_value_as_number(js_arg_row_span);

    // 调用底层函数
    lv_obj_set_grid_cell(arg_obj, arg_column_align, arg_col_pos, arg_col_span, arg_row_align, arg_row_pos, arg_row_span);

    return jerry_undefined();
}



/**
 * @brief Set the height of an object possible values are: pixel simple set the size accordingly LV_SIZE_CONTENT set the size to involve all children in the given direction lv_pct(x) to set size in percentage of the parent's content area size (the size without paddings). x should be in [0..1000]% range
 */
static jerry_value_t js_lv_obj_set_height(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_set_height: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: h (int32_t)
    jerry_value_t js_arg_h = args[1];
    if (!jerry_value_is_number(js_arg_h)) {
        return script_engine_throw_error("lv_obj_set_height: Argument 1 must be a number");
    }

    int32_t arg_h = (int32_t)jerry_value_as_number(js_arg_h);

    // 调用底层函数
    lv_obj_set_height(arg_obj, arg_h);

    return jerry_undefined();
}



/**
 * @brief Set a layout for an object
 */
static jerry_value_t js_lv_obj_set_layout(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_set_layout: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: layout (uint32_t)
    jerry_value_t js_arg_layout = args[1];
    if (!jerry_value_is_number(js_arg_layout)) {
        return script_engine_throw_error("lv_obj_set_layout: Argument 1 must be a number");
    }

    uint32_t arg_layout = (uint32_t)jerry_value_as_number(js_arg_layout);

    // 调用底层函数
    lv_obj_set_layout(arg_obj, arg_layout);

    return jerry_undefined();
}



/**
 * @brief Move the parent of an object. The relative coordinates will be kept.
 */
static jerry_value_t js_lv_obj_set_parent(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_set_parent: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: parent (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_parent = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_set_parent(arg_obj, arg_parent);

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
        return script_engine_throw_error("lv_obj_set_pos: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: x (int32_t)
    jerry_value_t js_arg_x = args[1];
    if (!jerry_value_is_number(js_arg_x)) {
        return script_engine_throw_error("lv_obj_set_pos: Argument 1 must be a number");
    }

    int32_t arg_x = (int32_t)jerry_value_as_number(js_arg_x);

    // 解析参数: y (int32_t)
    jerry_value_t js_arg_y = args[2];
    if (!jerry_value_is_number(js_arg_y)) {
        return script_engine_throw_error("lv_obj_set_pos: Argument 2 must be a number");
    }

    int32_t arg_y = (int32_t)jerry_value_as_number(js_arg_y);

    // 调用底层函数
    lv_obj_set_pos(arg_obj, arg_x, arg_y);

    return jerry_undefined();
}



/**
 * @brief Set the object in which directions can be scrolled
 */
static jerry_value_t js_lv_obj_set_scroll_dir(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_set_scroll_dir: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: dir (lv_dir_t)
    jerry_value_t js_arg_dir = args[1];
    if (!jerry_value_is_number(js_arg_dir)) {
        return script_engine_throw_error("lv_obj_set_scroll_dir: Argument 1 must be a number");
    }

    int arg_dir = (int)jerry_value_as_number(js_arg_dir);

    // 调用底层函数
    lv_obj_set_scroll_dir(arg_obj, arg_dir);

    return jerry_undefined();
}



/**
 * @brief Set where to snap the children when scrolling ends horizontally
 */
static jerry_value_t js_lv_obj_set_scroll_snap_x(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_set_scroll_snap_x: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: align (lv_scroll_snap_t)
    jerry_value_t js_arg_align = args[1];
    if (!jerry_value_is_number(js_arg_align)) {
        return script_engine_throw_error("lv_obj_set_scroll_snap_x: Argument 1 must be a number");
    }

    int arg_align = (int)jerry_value_as_number(js_arg_align);

    // 调用底层函数
    lv_obj_set_scroll_snap_x(arg_obj, arg_align);

    return jerry_undefined();
}



/**
 * @brief Set where to snap the children when scrolling ends vertically
 */
static jerry_value_t js_lv_obj_set_scroll_snap_y(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_set_scroll_snap_y: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: align (lv_scroll_snap_t)
    jerry_value_t js_arg_align = args[1];
    if (!jerry_value_is_number(js_arg_align)) {
        return script_engine_throw_error("lv_obj_set_scroll_snap_y: Argument 1 must be a number");
    }

    int arg_align = (int)jerry_value_as_number(js_arg_align);

    // 调用底层函数
    lv_obj_set_scroll_snap_y(arg_obj, arg_align);

    return jerry_undefined();
}



/**
 * @brief Set how the scrollbars should behave.
 */
static jerry_value_t js_lv_obj_set_scrollbar_mode(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_set_scrollbar_mode: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: mode (lv_scrollbar_mode_t)
    jerry_value_t js_arg_mode = args[1];
    if (!jerry_value_is_number(js_arg_mode)) {
        return script_engine_throw_error("lv_obj_set_scrollbar_mode: Argument 1 must be a number");
    }

    int arg_mode = (int)jerry_value_as_number(js_arg_mode);

    // 调用底层函数
    lv_obj_set_scrollbar_mode(arg_obj, arg_mode);

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
        return script_engine_throw_error("lv_obj_set_size: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: w (int32_t)
    jerry_value_t js_arg_w = args[1];
    if (!jerry_value_is_number(js_arg_w)) {
        return script_engine_throw_error("lv_obj_set_size: Argument 1 must be a number");
    }

    int32_t arg_w = (int32_t)jerry_value_as_number(js_arg_w);

    // 解析参数: h (int32_t)
    jerry_value_t js_arg_h = args[2];
    if (!jerry_value_is_number(js_arg_h)) {
        return script_engine_throw_error("lv_obj_set_size: Argument 2 must be a number");
    }

    int32_t arg_h = (int32_t)jerry_value_as_number(js_arg_h);

    // 调用底层函数
    lv_obj_set_size(arg_obj, arg_w, arg_h);

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
        return script_engine_throw_error("lv_obj_set_state: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: state (lv_state_t)
    jerry_value_t js_arg_state = args[1];
    if (!jerry_value_is_number(js_arg_state)) {
        return script_engine_throw_error("lv_obj_set_state: Argument 1 must be a number");
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
            return script_engine_throw_error("lv_obj_set_state: Argument 2 must be boolean or number for bool");
        }
    }

    // 调用底层函数
    lv_obj_set_state(arg_obj, arg_state, arg_v);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_align function
 */
static jerry_value_t js_lv_obj_set_style_align(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_align: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_align_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_align: Argument 1 must be a number");
    }

    int arg_value = (int)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_align: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_align(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_anim function
 */
static jerry_value_t js_lv_obj_set_style_anim(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_anim: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value ( lv_anim_t*)
    // lv_anim_t* 类型参数处理
    lv_anim_t* arg_value = lv_js_bridge_obj_2_ptr(args[1], LV_TYPE_ANY);
    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_anim: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_anim(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_anim_duration function
 */
static jerry_value_t js_lv_obj_set_style_anim_duration(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_anim_duration: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (uint32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_anim_duration: Argument 1 must be a number");
    }

    uint32_t arg_value = (uint32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_anim_duration: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_anim_duration(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_arc_color function
 */
static jerry_value_t js_lv_obj_set_style_arc_color(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_arc_color: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_color_t)
    lv_color_t arg_value = lv_js_bridge_obj_2_color(args[1]);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_arc_color: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_arc_color(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_arc_image_src function
 */
static jerry_value_t js_lv_obj_set_style_arc_image_src(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_arc_image_src: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (const void*)
    // void*/字符串 类型参数，支持null
    void* arg_value = NULL;
    char* arg_value_str = NULL;  // 用于字符串参数的临时存储

    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1])) {
        if (jerry_value_is_string(args[1])) {
            // 处理字符串类型的符号（如LV_SYMBOL_MINUS）
            jerry_size_t arg_value_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
            arg_value_str = (char*)eos_malloc(arg_value_len + 1);
            if (!arg_value_str) {
                return script_engine_throw_error("lv_obj_set_style_arc_image_src: Failed to allocate memory for string argument");
            }
            jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_value_str, arg_value_len);
            arg_value_str[arg_value_len] = '\0';
            arg_value = (void*)arg_value_str;
        }
        else if (jerry_value_is_object(args[1])) {
            // 尝试从对象获取指针
            arg_value = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
        }
        else if (jerry_value_is_number(args[1])) {
            // 直接传递指针数值
            uintptr_t ptr_num = (uintptr_t)jerry_value_as_number(args[1]);
            arg_value = (void*)ptr_num;
        }
        else {
            return script_engine_throw_error("lv_obj_set_style_arc_image_src: Argument 1 must be string, object or number");
        }
    }
    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_arc_image_src: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_arc_image_src(arg_obj, arg_value, arg_selector);

    // 释放临时内存
    if (arg_value_str) eos_free(arg_value_str);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_arc_opa function
 */
static jerry_value_t js_lv_obj_set_style_arc_opa(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_arc_opa: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_opa_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_arc_opa: Argument 1 must be a number");
    }

    uint8_t arg_value = (uint8_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_arc_opa: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_arc_opa(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_arc_rounded function
 */
static jerry_value_t js_lv_obj_set_style_arc_rounded(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_arc_rounded: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (bool)
    // 布尔类型参数: value
    bool arg_value = false;
    if (!jerry_value_is_undefined(args[1])) {
        if (jerry_value_is_boolean(args[1])) {
            arg_value = jerry_value_to_boolean(args[1]);
        }
        else if (jerry_value_is_number(args[1])) {
            arg_value = (jerry_value_as_number(args[1]) != 0);
        }
        else {
            return script_engine_throw_error("lv_obj_set_style_arc_rounded: Argument 1 must be boolean or number for bool");
        }
    }

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_arc_rounded: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_arc_rounded(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_arc_width function
 */
static jerry_value_t js_lv_obj_set_style_arc_width(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_arc_width: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_arc_width: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_arc_width: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_arc_width(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_base_dir function
 */
static jerry_value_t js_lv_obj_set_style_base_dir(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_base_dir: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_base_dir_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_base_dir: Argument 1 must be a number");
    }

    int arg_value = (int)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_base_dir: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_base_dir(arg_obj, arg_value, arg_selector);

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
        return script_engine_throw_error("lv_obj_set_style_bg_color: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_color_t)
    lv_color_t arg_value = lv_js_bridge_obj_2_color(args[1]);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_bg_color: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_bg_color(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_bg_grad function
 */
static jerry_value_t js_lv_obj_set_style_bg_grad(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_bg_grad: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (const lv_grad_dsc_t*)
    // 指针类型参数，支持null
    void* arg_value = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_bg_grad: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_bg_grad(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_bg_grad_color function
 */
static jerry_value_t js_lv_obj_set_style_bg_grad_color(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_bg_grad_color: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_color_t)
    lv_color_t arg_value = lv_js_bridge_obj_2_color(args[1]);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_bg_grad_color: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_bg_grad_color(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_bg_grad_dir function
 */
static jerry_value_t js_lv_obj_set_style_bg_grad_dir(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_bg_grad_dir: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_grad_dir_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_bg_grad_dir: Argument 1 must be a number");
    }

    int arg_value = (int)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_bg_grad_dir: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_bg_grad_dir(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_bg_grad_opa function
 */
static jerry_value_t js_lv_obj_set_style_bg_grad_opa(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_bg_grad_opa: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_opa_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_bg_grad_opa: Argument 1 must be a number");
    }

    uint8_t arg_value = (uint8_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_bg_grad_opa: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_bg_grad_opa(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_bg_grad_stop function
 */
static jerry_value_t js_lv_obj_set_style_bg_grad_stop(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_bg_grad_stop: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_bg_grad_stop: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_bg_grad_stop: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_bg_grad_stop(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_bg_image_opa function
 */
static jerry_value_t js_lv_obj_set_style_bg_image_opa(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_bg_image_opa: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_opa_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_bg_image_opa: Argument 1 must be a number");
    }

    uint8_t arg_value = (uint8_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_bg_image_opa: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_bg_image_opa(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_bg_image_recolor function
 */
static jerry_value_t js_lv_obj_set_style_bg_image_recolor(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_bg_image_recolor: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_color_t)
    lv_color_t arg_value = lv_js_bridge_obj_2_color(args[1]);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_bg_image_recolor: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_bg_image_recolor(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_bg_image_recolor_opa function
 */
static jerry_value_t js_lv_obj_set_style_bg_image_recolor_opa(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_bg_image_recolor_opa: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_opa_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_bg_image_recolor_opa: Argument 1 must be a number");
    }

    uint8_t arg_value = (uint8_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_bg_image_recolor_opa: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_bg_image_recolor_opa(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_bg_image_src function
 */
static jerry_value_t js_lv_obj_set_style_bg_image_src(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_bg_image_src: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (const void*)
    // void*/字符串 类型参数，支持null
    void* arg_value = NULL;
    char* arg_value_str = NULL;  // 用于字符串参数的临时存储

    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1])) {
        if (jerry_value_is_string(args[1])) {
            // 处理字符串类型的符号（如LV_SYMBOL_MINUS）
            jerry_size_t arg_value_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
            arg_value_str = (char*)eos_malloc(arg_value_len + 1);
            if (!arg_value_str) {
                return script_engine_throw_error("lv_obj_set_style_bg_image_src: Failed to allocate memory for string argument");
            }
            jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_value_str, arg_value_len);
            arg_value_str[arg_value_len] = '\0';
            arg_value = (void*)arg_value_str;
        }
        else if (jerry_value_is_object(args[1])) {
            // 尝试从对象获取指针
            arg_value = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
        }
        else if (jerry_value_is_number(args[1])) {
            // 直接传递指针数值
            uintptr_t ptr_num = (uintptr_t)jerry_value_as_number(args[1]);
            arg_value = (void*)ptr_num;
        }
        else {
            return script_engine_throw_error("lv_obj_set_style_bg_image_src: Argument 1 must be string, object or number");
        }
    }
    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_bg_image_src: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_bg_image_src(arg_obj, arg_value, arg_selector);

    // 释放临时内存
    if (arg_value_str) eos_free(arg_value_str);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_bg_image_tiled function
 */
static jerry_value_t js_lv_obj_set_style_bg_image_tiled(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_bg_image_tiled: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (bool)
    // 布尔类型参数: value
    bool arg_value = false;
    if (!jerry_value_is_undefined(args[1])) {
        if (jerry_value_is_boolean(args[1])) {
            arg_value = jerry_value_to_boolean(args[1]);
        }
        else if (jerry_value_is_number(args[1])) {
            arg_value = (jerry_value_as_number(args[1]) != 0);
        }
        else {
            return script_engine_throw_error("lv_obj_set_style_bg_image_tiled: Argument 1 must be boolean or number for bool");
        }
    }

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_bg_image_tiled: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_bg_image_tiled(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_bg_main_opa function
 */
static jerry_value_t js_lv_obj_set_style_bg_main_opa(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_bg_main_opa: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_opa_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_bg_main_opa: Argument 1 must be a number");
    }

    uint8_t arg_value = (uint8_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_bg_main_opa: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_bg_main_opa(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_bg_main_stop function
 */
static jerry_value_t js_lv_obj_set_style_bg_main_stop(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_bg_main_stop: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_bg_main_stop: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_bg_main_stop: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_bg_main_stop(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_bg_opa function
 */
static jerry_value_t js_lv_obj_set_style_bg_opa(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_bg_opa: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_opa_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_bg_opa: Argument 1 must be a number");
    }

    uint8_t arg_value = (uint8_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_bg_opa: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_bg_opa(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_bitmap_mask_src function
 */
static jerry_value_t js_lv_obj_set_style_bitmap_mask_src(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_bitmap_mask_src: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (const void*)
    // void*/字符串 类型参数，支持null
    void* arg_value = NULL;
    char* arg_value_str = NULL;  // 用于字符串参数的临时存储

    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1])) {
        if (jerry_value_is_string(args[1])) {
            // 处理字符串类型的符号（如LV_SYMBOL_MINUS）
            jerry_size_t arg_value_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
            arg_value_str = (char*)eos_malloc(arg_value_len + 1);
            if (!arg_value_str) {
                return script_engine_throw_error("lv_obj_set_style_bitmap_mask_src: Failed to allocate memory for string argument");
            }
            jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_value_str, arg_value_len);
            arg_value_str[arg_value_len] = '\0';
            arg_value = (void*)arg_value_str;
        }
        else if (jerry_value_is_object(args[1])) {
            // 尝试从对象获取指针
            arg_value = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
        }
        else if (jerry_value_is_number(args[1])) {
            // 直接传递指针数值
            uintptr_t ptr_num = (uintptr_t)jerry_value_as_number(args[1]);
            arg_value = (void*)ptr_num;
        }
        else {
            return script_engine_throw_error("lv_obj_set_style_bitmap_mask_src: Argument 1 must be string, object or number");
        }
    }
    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_bitmap_mask_src: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_bitmap_mask_src(arg_obj, arg_value, arg_selector);

    // 释放临时内存
    if (arg_value_str) eos_free(arg_value_str);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_blend_mode function
 */
static jerry_value_t js_lv_obj_set_style_blend_mode(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_blend_mode: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_blend_mode_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_blend_mode: Argument 1 must be a number");
    }

    int arg_value = (int)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_blend_mode: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_blend_mode(arg_obj, arg_value, arg_selector);

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
        return script_engine_throw_error("lv_obj_set_style_border_color: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_color_t)
    lv_color_t arg_value = lv_js_bridge_obj_2_color(args[1]);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_border_color: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_border_color(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_border_opa function
 */
static jerry_value_t js_lv_obj_set_style_border_opa(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_border_opa: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_opa_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_border_opa: Argument 1 must be a number");
    }

    uint8_t arg_value = (uint8_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_border_opa: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_border_opa(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_border_post function
 */
static jerry_value_t js_lv_obj_set_style_border_post(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_border_post: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (bool)
    // 布尔类型参数: value
    bool arg_value = false;
    if (!jerry_value_is_undefined(args[1])) {
        if (jerry_value_is_boolean(args[1])) {
            arg_value = jerry_value_to_boolean(args[1]);
        }
        else if (jerry_value_is_number(args[1])) {
            arg_value = (jerry_value_as_number(args[1]) != 0);
        }
        else {
            return script_engine_throw_error("lv_obj_set_style_border_post: Argument 1 must be boolean or number for bool");
        }
    }

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_border_post: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_border_post(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_border_side function
 */
static jerry_value_t js_lv_obj_set_style_border_side(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_border_side: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_border_side_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_border_side: Argument 1 must be a number");
    }

    int arg_value = (int)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_border_side: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_border_side(arg_obj, arg_value, arg_selector);

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
        return script_engine_throw_error("lv_obj_set_style_border_width: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_border_width: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_border_width: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_border_width(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_clip_corner function
 */
static jerry_value_t js_lv_obj_set_style_clip_corner(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_clip_corner: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (bool)
    // 布尔类型参数: value
    bool arg_value = false;
    if (!jerry_value_is_undefined(args[1])) {
        if (jerry_value_is_boolean(args[1])) {
            arg_value = jerry_value_to_boolean(args[1]);
        }
        else if (jerry_value_is_number(args[1])) {
            arg_value = (jerry_value_as_number(args[1]) != 0);
        }
        else {
            return script_engine_throw_error("lv_obj_set_style_clip_corner: Argument 1 must be boolean or number for bool");
        }
    }

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_clip_corner: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_clip_corner(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_color_filter_dsc function
 */
static jerry_value_t js_lv_obj_set_style_color_filter_dsc(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_color_filter_dsc: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (const lv_color_filter_dsc_t*)
    // 指针类型参数，支持null
    void* arg_value = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_color_filter_dsc: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_color_filter_dsc(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_color_filter_opa function
 */
static jerry_value_t js_lv_obj_set_style_color_filter_opa(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_color_filter_opa: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_opa_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_color_filter_opa: Argument 1 must be a number");
    }

    uint8_t arg_value = (uint8_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_color_filter_opa: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_color_filter_opa(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_flex_cross_place function
 */
static jerry_value_t js_lv_obj_set_style_flex_cross_place(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_flex_cross_place: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_flex_align_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_flex_cross_place: Argument 1 must be a number");
    }

    int arg_value = (int)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_flex_cross_place: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_flex_cross_place(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_flex_flow function
 */
static jerry_value_t js_lv_obj_set_style_flex_flow(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_flex_flow: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_flex_flow_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_flex_flow: Argument 1 must be a number");
    }

    int arg_value = (int)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_flex_flow: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_flex_flow(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_flex_grow function
 */
static jerry_value_t js_lv_obj_set_style_flex_grow(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_flex_grow: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (uint8_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_flex_grow: Argument 1 must be a number");
    }

    uint8_t arg_value = (uint8_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_flex_grow: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_flex_grow(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_flex_main_place function
 */
static jerry_value_t js_lv_obj_set_style_flex_main_place(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_flex_main_place: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_flex_align_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_flex_main_place: Argument 1 must be a number");
    }

    int arg_value = (int)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_flex_main_place: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_flex_main_place(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_flex_track_place function
 */
static jerry_value_t js_lv_obj_set_style_flex_track_place(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_flex_track_place: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_flex_align_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_flex_track_place: Argument 1 must be a number");
    }

    int arg_value = (int)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_flex_track_place: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_flex_track_place(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_grid_cell_column_pos function
 */
static jerry_value_t js_lv_obj_set_style_grid_cell_column_pos(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_grid_cell_column_pos: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_grid_cell_column_pos: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_grid_cell_column_pos: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_grid_cell_column_pos(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_grid_cell_column_span function
 */
static jerry_value_t js_lv_obj_set_style_grid_cell_column_span(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_grid_cell_column_span: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_grid_cell_column_span: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_grid_cell_column_span: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_grid_cell_column_span(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_grid_cell_row_pos function
 */
static jerry_value_t js_lv_obj_set_style_grid_cell_row_pos(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_grid_cell_row_pos: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_grid_cell_row_pos: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_grid_cell_row_pos: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_grid_cell_row_pos(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_grid_cell_row_span function
 */
static jerry_value_t js_lv_obj_set_style_grid_cell_row_span(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_grid_cell_row_span: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_grid_cell_row_span: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_grid_cell_row_span: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_grid_cell_row_span(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_grid_cell_x_align function
 */
static jerry_value_t js_lv_obj_set_style_grid_cell_x_align(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_grid_cell_x_align: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_grid_align_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_grid_cell_x_align: Argument 1 must be a number");
    }

    int arg_value = (int)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_grid_cell_x_align: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_grid_cell_x_align(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_grid_cell_y_align function
 */
static jerry_value_t js_lv_obj_set_style_grid_cell_y_align(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_grid_cell_y_align: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_grid_align_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_grid_cell_y_align: Argument 1 must be a number");
    }

    int arg_value = (int)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_grid_cell_y_align: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_grid_cell_y_align(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_grid_column_align function
 */
static jerry_value_t js_lv_obj_set_style_grid_column_align(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_grid_column_align: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_grid_align_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_grid_column_align: Argument 1 must be a number");
    }

    int arg_value = (int)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_grid_column_align: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_grid_column_align(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_grid_column_dsc_array function
 */
static jerry_value_t js_lv_obj_set_style_grid_column_dsc_array(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_grid_column_dsc_array: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (const int32_t*)
    // 通用指针类型: const int32_t*，支持null
    const int32_t* arg_value = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_grid_column_dsc_array: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_grid_column_dsc_array(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_grid_row_align function
 */
static jerry_value_t js_lv_obj_set_style_grid_row_align(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_grid_row_align: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_grid_align_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_grid_row_align: Argument 1 must be a number");
    }

    int arg_value = (int)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_grid_row_align: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_grid_row_align(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_grid_row_dsc_array function
 */
static jerry_value_t js_lv_obj_set_style_grid_row_dsc_array(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_grid_row_dsc_array: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (const int32_t*)
    // 通用指针类型: const int32_t*，支持null
    const int32_t* arg_value = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_grid_row_dsc_array: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_grid_row_dsc_array(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_height function
 */
static jerry_value_t js_lv_obj_set_style_height(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_height: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_height: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_height: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_height(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_image_opa function
 */
static jerry_value_t js_lv_obj_set_style_image_opa(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_image_opa: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_opa_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_image_opa: Argument 1 must be a number");
    }

    uint8_t arg_value = (uint8_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_image_opa: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_image_opa(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_image_recolor function
 */
static jerry_value_t js_lv_obj_set_style_image_recolor(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_image_recolor: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_color_t)
    lv_color_t arg_value = lv_js_bridge_obj_2_color(args[1]);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_image_recolor: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_image_recolor(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_image_recolor_opa function
 */
static jerry_value_t js_lv_obj_set_style_image_recolor_opa(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_image_recolor_opa: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_opa_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_image_recolor_opa: Argument 1 must be a number");
    }

    uint8_t arg_value = (uint8_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_image_recolor_opa: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_image_recolor_opa(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_layout function
 */
static jerry_value_t js_lv_obj_set_style_layout(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_layout: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (uint16_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_layout: Argument 1 must be a number");
    }

    uint16_t arg_value = (uint16_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_layout: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_layout(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_length function
 */
static jerry_value_t js_lv_obj_set_style_length(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_length: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_length: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_length: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_length(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_line_color function
 */
static jerry_value_t js_lv_obj_set_style_line_color(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_line_color: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_color_t)
    lv_color_t arg_value = lv_js_bridge_obj_2_color(args[1]);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_line_color: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_line_color(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_line_dash_gap function
 */
static jerry_value_t js_lv_obj_set_style_line_dash_gap(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_line_dash_gap: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_line_dash_gap: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_line_dash_gap: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_line_dash_gap(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_line_dash_width function
 */
static jerry_value_t js_lv_obj_set_style_line_dash_width(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_line_dash_width: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_line_dash_width: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_line_dash_width: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_line_dash_width(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_line_opa function
 */
static jerry_value_t js_lv_obj_set_style_line_opa(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_line_opa: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_opa_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_line_opa: Argument 1 must be a number");
    }

    uint8_t arg_value = (uint8_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_line_opa: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_line_opa(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_line_rounded function
 */
static jerry_value_t js_lv_obj_set_style_line_rounded(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_line_rounded: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (bool)
    // 布尔类型参数: value
    bool arg_value = false;
    if (!jerry_value_is_undefined(args[1])) {
        if (jerry_value_is_boolean(args[1])) {
            arg_value = jerry_value_to_boolean(args[1]);
        }
        else if (jerry_value_is_number(args[1])) {
            arg_value = (jerry_value_as_number(args[1]) != 0);
        }
        else {
            return script_engine_throw_error("lv_obj_set_style_line_rounded: Argument 1 must be boolean or number for bool");
        }
    }

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_line_rounded: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_line_rounded(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_line_width function
 */
static jerry_value_t js_lv_obj_set_style_line_width(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_line_width: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_line_width: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_line_width: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_line_width(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_margin_all function
 */
static jerry_value_t js_lv_obj_set_style_margin_all(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_margin_all: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_margin_all: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_margin_all: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_margin_all(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_margin_bottom function
 */
static jerry_value_t js_lv_obj_set_style_margin_bottom(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_margin_bottom: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_margin_bottom: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_margin_bottom: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_margin_bottom(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_margin_hor function
 */
static jerry_value_t js_lv_obj_set_style_margin_hor(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_margin_hor: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_margin_hor: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_margin_hor: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_margin_hor(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_margin_left function
 */
static jerry_value_t js_lv_obj_set_style_margin_left(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_margin_left: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_margin_left: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_margin_left: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_margin_left(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_margin_right function
 */
static jerry_value_t js_lv_obj_set_style_margin_right(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_margin_right: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_margin_right: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_margin_right: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_margin_right(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_margin_top function
 */
static jerry_value_t js_lv_obj_set_style_margin_top(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_margin_top: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_margin_top: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_margin_top: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_margin_top(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_margin_ver function
 */
static jerry_value_t js_lv_obj_set_style_margin_ver(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_margin_ver: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_margin_ver: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_margin_ver: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_margin_ver(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_max_height function
 */
static jerry_value_t js_lv_obj_set_style_max_height(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_max_height: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_max_height: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_max_height: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_max_height(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_max_width function
 */
static jerry_value_t js_lv_obj_set_style_max_width(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_max_width: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_max_width: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_max_width: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_max_width(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_min_height function
 */
static jerry_value_t js_lv_obj_set_style_min_height(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_min_height: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_min_height: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_min_height: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_min_height(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_min_width function
 */
static jerry_value_t js_lv_obj_set_style_min_width(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_min_width: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_min_width: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_min_width: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_min_width(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_opa function
 */
static jerry_value_t js_lv_obj_set_style_opa(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_opa: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_opa_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_opa: Argument 1 must be a number");
    }

    uint8_t arg_value = (uint8_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_opa: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_opa(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_opa_layered function
 */
static jerry_value_t js_lv_obj_set_style_opa_layered(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_opa_layered: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_opa_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_opa_layered: Argument 1 must be a number");
    }

    uint8_t arg_value = (uint8_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_opa_layered: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_opa_layered(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_outline_color function
 */
static jerry_value_t js_lv_obj_set_style_outline_color(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_outline_color: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_color_t)
    lv_color_t arg_value = lv_js_bridge_obj_2_color(args[1]);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_outline_color: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_outline_color(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_outline_opa function
 */
static jerry_value_t js_lv_obj_set_style_outline_opa(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_outline_opa: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_opa_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_outline_opa: Argument 1 must be a number");
    }

    uint8_t arg_value = (uint8_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_outline_opa: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_outline_opa(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_outline_pad function
 */
static jerry_value_t js_lv_obj_set_style_outline_pad(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_outline_pad: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_outline_pad: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_outline_pad: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_outline_pad(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_outline_width function
 */
static jerry_value_t js_lv_obj_set_style_outline_width(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_outline_width: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_outline_width: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_outline_width: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_outline_width(arg_obj, arg_value, arg_selector);

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
        return script_engine_throw_error("lv_obj_set_style_pad_all: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_pad_all: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_pad_all: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_pad_all(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_pad_bottom function
 */
static jerry_value_t js_lv_obj_set_style_pad_bottom(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_pad_bottom: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_pad_bottom: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_pad_bottom: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_pad_bottom(arg_obj, arg_value, arg_selector);

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
        return script_engine_throw_error("lv_obj_set_style_pad_column: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_pad_column: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_pad_column: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_pad_column(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_pad_gap function
 */
static jerry_value_t js_lv_obj_set_style_pad_gap(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_pad_gap: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_pad_gap: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_pad_gap: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_pad_gap(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_pad_hor function
 */
static jerry_value_t js_lv_obj_set_style_pad_hor(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_pad_hor: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_pad_hor: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_pad_hor: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_pad_hor(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_pad_left function
 */
static jerry_value_t js_lv_obj_set_style_pad_left(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_pad_left: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_pad_left: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_pad_left: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_pad_left(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_pad_right function
 */
static jerry_value_t js_lv_obj_set_style_pad_right(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_pad_right: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_pad_right: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_pad_right: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_pad_right(arg_obj, arg_value, arg_selector);

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
        return script_engine_throw_error("lv_obj_set_style_pad_row: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_pad_row: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_pad_row: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_pad_row(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_pad_top function
 */
static jerry_value_t js_lv_obj_set_style_pad_top(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_pad_top: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_pad_top: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_pad_top: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_pad_top(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_pad_ver function
 */
static jerry_value_t js_lv_obj_set_style_pad_ver(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_pad_ver: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_pad_ver: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_pad_ver: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_pad_ver(arg_obj, arg_value, arg_selector);

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
        return script_engine_throw_error("lv_obj_set_style_radius: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_radius: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_radius: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_radius(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_rotary_sensitivity function
 */
static jerry_value_t js_lv_obj_set_style_rotary_sensitivity(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_rotary_sensitivity: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (uint32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_rotary_sensitivity: Argument 1 must be a number");
    }

    uint32_t arg_value = (uint32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_rotary_sensitivity: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_rotary_sensitivity(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_shadow_color function
 */
static jerry_value_t js_lv_obj_set_style_shadow_color(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_shadow_color: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_color_t)
    lv_color_t arg_value = lv_js_bridge_obj_2_color(args[1]);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_shadow_color: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_shadow_color(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_shadow_offset_x function
 */
static jerry_value_t js_lv_obj_set_style_shadow_offset_x(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_shadow_offset_x: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_shadow_offset_x: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_shadow_offset_x: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_shadow_offset_x(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_shadow_offset_y function
 */
static jerry_value_t js_lv_obj_set_style_shadow_offset_y(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_shadow_offset_y: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_shadow_offset_y: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_shadow_offset_y: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_shadow_offset_y(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_shadow_opa function
 */
static jerry_value_t js_lv_obj_set_style_shadow_opa(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_shadow_opa: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_opa_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_shadow_opa: Argument 1 must be a number");
    }

    uint8_t arg_value = (uint8_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_shadow_opa: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_shadow_opa(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_shadow_spread function
 */
static jerry_value_t js_lv_obj_set_style_shadow_spread(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_shadow_spread: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_shadow_spread: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_shadow_spread: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_shadow_spread(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_shadow_width function
 */
static jerry_value_t js_lv_obj_set_style_shadow_width(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_shadow_width: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_shadow_width: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_shadow_width: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_shadow_width(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_size function
 */
static jerry_value_t js_lv_obj_set_style_size(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 4) {
        return script_engine_throw_error("lv_obj_set_style_size: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: width (int32_t)
    jerry_value_t js_arg_width = args[1];
    if (!jerry_value_is_number(js_arg_width)) {
        return script_engine_throw_error("lv_obj_set_style_size: Argument 1 must be a number");
    }

    int32_t arg_width = (int32_t)jerry_value_as_number(js_arg_width);

    // 解析参数: height (int32_t)
    jerry_value_t js_arg_height = args[2];
    if (!jerry_value_is_number(js_arg_height)) {
        return script_engine_throw_error("lv_obj_set_style_size: Argument 2 must be a number");
    }

    int32_t arg_height = (int32_t)jerry_value_as_number(js_arg_height);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[3];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_size: Argument 3 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_size(arg_obj, arg_width, arg_height, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_text_align function
 */
static jerry_value_t js_lv_obj_set_style_text_align(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_text_align: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_text_align_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_text_align: Argument 1 must be a number");
    }

    int arg_value = (int)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_text_align: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_text_align(arg_obj, arg_value, arg_selector);

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
        return script_engine_throw_error("lv_obj_set_style_text_color: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_color_t)
    lv_color_t arg_value = lv_js_bridge_obj_2_color(args[1]);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_text_color: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_text_color(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_text_decor function
 */
static jerry_value_t js_lv_obj_set_style_text_decor(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_text_decor: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_text_decor_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_text_decor: Argument 1 must be a number");
    }

    int arg_value = (int)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_text_decor: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_text_decor(arg_obj, arg_value, arg_selector);

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
        return script_engine_throw_error("lv_obj_set_style_text_font: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value ( lv_font_t*)
    // lv_font_t* 类型参数处理
    const lv_font_t* arg_value = lv_js_bridge_obj_2_ptr(args[1], LV_TYPE_ANY);
    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_text_font: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_text_font(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_text_letter_space function
 */
static jerry_value_t js_lv_obj_set_style_text_letter_space(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_text_letter_space: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_text_letter_space: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_text_letter_space: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_text_letter_space(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_text_line_space function
 */
static jerry_value_t js_lv_obj_set_style_text_line_space(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_text_line_space: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_text_line_space: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_text_line_space: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_text_line_space(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_text_opa function
 */
static jerry_value_t js_lv_obj_set_style_text_opa(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_text_opa: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (lv_opa_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_text_opa: Argument 1 must be a number");
    }

    uint8_t arg_value = (uint8_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_text_opa: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_text_opa(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_transform_height function
 */
static jerry_value_t js_lv_obj_set_style_transform_height(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_transform_height: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_transform_height: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_transform_height: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_transform_height(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_transform_pivot_x function
 */
static jerry_value_t js_lv_obj_set_style_transform_pivot_x(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_transform_pivot_x: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_transform_pivot_x: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_transform_pivot_x: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_transform_pivot_x(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_transform_pivot_y function
 */
static jerry_value_t js_lv_obj_set_style_transform_pivot_y(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_transform_pivot_y: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_transform_pivot_y: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_transform_pivot_y: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_transform_pivot_y(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_transform_rotation function
 */
static jerry_value_t js_lv_obj_set_style_transform_rotation(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_transform_rotation: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_transform_rotation: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_transform_rotation: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_transform_rotation(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_transform_scale function
 */
static jerry_value_t js_lv_obj_set_style_transform_scale(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_transform_scale: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_transform_scale: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_transform_scale: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_transform_scale(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_transform_scale_x function
 */
static jerry_value_t js_lv_obj_set_style_transform_scale_x(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_transform_scale_x: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_transform_scale_x: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_transform_scale_x: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_transform_scale_x(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_transform_scale_y function
 */
static jerry_value_t js_lv_obj_set_style_transform_scale_y(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_transform_scale_y: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_transform_scale_y: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_transform_scale_y: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_transform_scale_y(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_transform_skew_x function
 */
static jerry_value_t js_lv_obj_set_style_transform_skew_x(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_transform_skew_x: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_transform_skew_x: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_transform_skew_x: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_transform_skew_x(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_transform_skew_y function
 */
static jerry_value_t js_lv_obj_set_style_transform_skew_y(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_transform_skew_y: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_transform_skew_y: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_transform_skew_y: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_transform_skew_y(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_transform_width function
 */
static jerry_value_t js_lv_obj_set_style_transform_width(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_transform_width: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_transform_width: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_transform_width: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_transform_width(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_transition function
 */
static jerry_value_t js_lv_obj_set_style_transition(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_transition: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (const lv_style_transition_dsc_t*)
    // 指针类型参数，支持null
    void* arg_value = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_transition: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_transition(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_translate_x function
 */
static jerry_value_t js_lv_obj_set_style_translate_x(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_translate_x: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_translate_x: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_translate_x: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_translate_x(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_translate_y function
 */
static jerry_value_t js_lv_obj_set_style_translate_y(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_translate_y: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_translate_y: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_translate_y: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_translate_y(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_width function
 */
static jerry_value_t js_lv_obj_set_style_width(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_width: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_width: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_width: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_width(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_x function
 */
static jerry_value_t js_lv_obj_set_style_x(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_x: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_x: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_x: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_x(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief lv_obj_set_style_y function
 */
static jerry_value_t js_lv_obj_set_style_y(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_set_style_y: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_obj_set_style_y: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[2];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_set_style_y: Argument 2 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_obj_set_style_y(arg_obj, arg_value, arg_selector);

    return jerry_undefined();
}



/**
 * @brief Set the user_data field of the object
 */
static jerry_value_t js_lv_obj_set_user_data(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_set_user_data: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: user_data (void*)
    // void*/字符串 类型参数，支持null
    void* arg_user_data = NULL;
    char* arg_user_data_str = NULL;  // 用于字符串参数的临时存储

    if (!jerry_value_is_undefined(args[1]) && !jerry_value_is_null(args[1])) {
        if (jerry_value_is_string(args[1])) {
            // 处理字符串类型的符号（如LV_SYMBOL_MINUS）
            jerry_size_t arg_user_data_len = jerry_string_size(args[1], JERRY_ENCODING_UTF8);
            arg_user_data_str = (char*)eos_malloc(arg_user_data_len + 1);
            if (!arg_user_data_str) {
                return script_engine_throw_error("lv_obj_set_user_data: Failed to allocate memory for string argument");
            }
            jerry_string_to_buffer(args[1], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_user_data_str, arg_user_data_len);
            arg_user_data_str[arg_user_data_len] = '\0';
            arg_user_data = (void*)arg_user_data_str;
        }
        else if (jerry_value_is_object(args[1])) {
            // 尝试从对象获取指针
            arg_user_data = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
        }
        else if (jerry_value_is_number(args[1])) {
            // 直接传递指针数值
            uintptr_t ptr_num = (uintptr_t)jerry_value_as_number(args[1]);
            arg_user_data = (void*)ptr_num;
        }
        else {
            return script_engine_throw_error("lv_obj_set_user_data: Argument 1 must be string, object or number");
        }
    }
    // 调用底层函数
    lv_obj_set_user_data(arg_obj, arg_user_data);

    // 释放临时内存
    if (arg_user_data_str) eos_free(arg_user_data_str);

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
        return script_engine_throw_error("lv_obj_set_width: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: w (int32_t)
    jerry_value_t js_arg_w = args[1];
    if (!jerry_value_is_number(js_arg_w)) {
        return script_engine_throw_error("lv_obj_set_width: Argument 1 must be a number");
    }

    int32_t arg_w = (int32_t)jerry_value_as_number(js_arg_w);

    // 调用底层函数
    lv_obj_set_width(arg_obj, arg_w);

    return jerry_undefined();
}



/**
 * @brief Set the x coordinate of an object With default alignment it's the distance from the top left corner  E.g. LV_ALIGN_CENTER alignment it's the offset from the center of the parent  The position is interpreted on the content area of the parent  The values can be set in pixel or in percentage of parent size with lv_pct(v)
 */
static jerry_value_t js_lv_obj_set_x(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_set_x: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: x (int32_t)
    jerry_value_t js_arg_x = args[1];
    if (!jerry_value_is_number(js_arg_x)) {
        return script_engine_throw_error("lv_obj_set_x: Argument 1 must be a number");
    }

    int32_t arg_x = (int32_t)jerry_value_as_number(js_arg_x);

    // 调用底层函数
    lv_obj_set_x(arg_obj, arg_x);

    return jerry_undefined();
}



/**
 * @brief Set the y coordinate of an object With default alignment it's the distance from the top left corner  E.g. LV_ALIGN_CENTER alignment it's the offset from the center of the parent  The position is interpreted on the content area of the parent  The values can be set in pixel or in percentage of parent size with lv_pct(v)
 */
static jerry_value_t js_lv_obj_set_y(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_set_y: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: y (int32_t)
    jerry_value_t js_arg_y = args[1];
    if (!jerry_value_is_number(js_arg_y)) {
        return script_engine_throw_error("lv_obj_set_y: Argument 1 must be a number");
    }

    int32_t arg_y = (int32_t)jerry_value_as_number(js_arg_y);

    // 调用底层函数
    lv_obj_set_y(arg_obj, arg_y);

    return jerry_undefined();
}



/**
 * @brief lv_obj_style_get_selector_part function
 */
static jerry_value_t js_lv_obj_style_get_selector_part(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_style_get_selector_part: Insufficient arguments");
    }

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[0];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_style_get_selector_part: Argument 0 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_part_t ret_value = lv_obj_style_get_selector_part(arg_selector);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_obj_style_get_selector_state function
 */
static jerry_value_t js_lv_obj_style_get_selector_state(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_style_get_selector_state: Insufficient arguments");
    }

    // 解析参数: selector (lv_style_selector_t)
    jerry_value_t js_arg_selector = args[0];
    if (!jerry_value_is_number(js_arg_selector)) {
        return script_engine_throw_error("lv_obj_style_get_selector_state: Argument 0 must be a number");
    }

    uint32_t arg_selector = (uint32_t)jerry_value_as_number(js_arg_selector);

    // 调用底层函数
    lv_state_t ret_value = lv_obj_style_get_selector_state(arg_selector);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Swap the positions of two objects. When used in listboxes, it can be used to sort the listbox items.
 */
static jerry_value_t js_lv_obj_swap(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_swap: Insufficient arguments");
    }

    // 解析参数: obj1 (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj1 = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: obj2 (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj2 = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_swap(arg_obj1, arg_obj2);

    return jerry_undefined();
}



/**
 * @brief Transform a point using the angle and zoom style properties of an object
 */
static jerry_value_t js_lv_obj_transform_point(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_transform_point: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: p (lv_point_t*)
    // 指针类型参数，支持null
    void* arg_p = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 解析参数: flags (lv_obj_point_transform_flag_t)
    jerry_value_t js_arg_flags = args[2];
    if (!jerry_value_is_number(js_arg_flags)) {
        return script_engine_throw_error("lv_obj_transform_point: Argument 2 must be a number");
    }

    lv_obj_point_transform_flag_t arg_flags = (lv_obj_point_transform_flag_t)jerry_value_as_number(js_arg_flags);

    // 调用底层函数
    lv_obj_transform_point(arg_obj, arg_p, arg_flags);

    return jerry_undefined();
}



/**
 * @brief Iterate through all children of any object.
 */
static jerry_value_t js_lv_obj_tree_walk(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_tree_walk: Insufficient arguments");
    }

    // 解析参数: start_obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_start_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: cb (lv_obj_tree_walk_cb_t)
    // 通用指针类型: lv_obj_tree_walk_cb_t，支持null
    lv_obj_tree_walk_cb_t arg_cb = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);

    // 解析参数: user_data (void*)
    // void*/字符串 类型参数，支持null
    void* arg_user_data = NULL;
    char* arg_user_data_str = NULL;  // 用于字符串参数的临时存储

    if (!jerry_value_is_undefined(args[2]) && !jerry_value_is_null(args[2])) {
        if (jerry_value_is_string(args[2])) {
            // 处理字符串类型的符号（如LV_SYMBOL_MINUS）
            jerry_size_t arg_user_data_len = jerry_string_size(args[2], JERRY_ENCODING_UTF8);
            arg_user_data_str = (char*)eos_malloc(arg_user_data_len + 1);
            if (!arg_user_data_str) {
                return script_engine_throw_error("lv_obj_tree_walk: Failed to allocate memory for string argument");
            }
            jerry_string_to_buffer(args[2], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_user_data_str, arg_user_data_len);
            arg_user_data_str[arg_user_data_len] = '\0';
            arg_user_data = (void*)arg_user_data_str;
        }
        else if (jerry_value_is_object(args[2])) {
            // 尝试从对象获取指针
            arg_user_data = lv_js_bridge_obj_2_ptr(args[2],LV_TYPE_ANY);
        }
        else if (jerry_value_is_number(args[2])) {
            // 直接传递指针数值
            uintptr_t ptr_num = (uintptr_t)jerry_value_as_number(args[2]);
            arg_user_data = (void*)ptr_num;
        }
        else {
            return script_engine_throw_error("lv_obj_tree_walk: Argument 2 must be string, object or number");
        }
    }
    // 调用底层函数
    lv_obj_tree_walk(arg_start_obj, arg_cb, arg_user_data);

    // 释放临时内存
    if (arg_user_data_str) eos_free(arg_user_data_str);

    return jerry_undefined();
}



/**
 * @brief Set add or remove one or more flags.
 */
static jerry_value_t js_lv_obj_update_flag(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_obj_update_flag: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: f (lv_obj_flag_t)
    jerry_value_t js_arg_f = args[1];
    if (!jerry_value_is_number(js_arg_f)) {
        return script_engine_throw_error("lv_obj_update_flag: Argument 1 must be a number");
    }

    int arg_f = (int)jerry_value_as_number(js_arg_f);

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
            return script_engine_throw_error("lv_obj_update_flag: Argument 2 must be boolean or number for bool");
        }
    }

    // 调用底层函数
    lv_obj_update_flag(arg_obj, arg_f, arg_v);

    return jerry_undefined();
}



/**
 * @brief Update the layout of an object.
 */
static jerry_value_t js_lv_obj_update_layout(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_obj_update_layout: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_update_layout(arg_obj);

    return jerry_undefined();
}



/**
 * @brief Check the children of obj and scroll obj to fulfill the scroll_snap settings
 */
static jerry_value_t js_lv_obj_update_snap(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_obj_update_snap: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: anim_en (lv_anim_enable_t)
    jerry_value_t js_arg_anim_en = args[1];
    if (!jerry_value_is_number(js_arg_anim_en)) {
        return script_engine_throw_error("lv_obj_update_snap: Argument 1 must be a number");
    }

    int arg_anim_en = (int)jerry_value_as_number(js_arg_anim_en);

    // 调用底层函数
    lv_obj_update_snap(arg_obj, arg_anim_en);

    return jerry_undefined();
}



/**
 * @brief lv_palette_darken function
 */
static jerry_value_t js_lv_palette_darken(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_palette_darken: Insufficient arguments");
    }

    // 解析参数: p (lv_palette_t)
    jerry_value_t js_arg_p = args[0];
    if (!jerry_value_is_number(js_arg_p)) {
        return script_engine_throw_error("lv_palette_darken: Argument 0 must be a number");
    }

    int arg_p = (int)jerry_value_as_number(js_arg_p);

    // 解析参数: lvl (uint8_t)
    jerry_value_t js_arg_lvl = args[1];
    if (!jerry_value_is_number(js_arg_lvl)) {
        return script_engine_throw_error("lv_palette_darken: Argument 1 must be a number");
    }

    uint8_t arg_lvl = (uint8_t)jerry_value_as_number(js_arg_lvl);

    // 调用底层函数
    lv_color_t ret_value = lv_palette_darken(arg_p, arg_lvl);

    // 处理返回值
    jerry_value_t js_result;
    // 转换为JS颜色对象
    js_result = lv_js_bridge_color_2_obj(&ret_value);

    return js_result;
}



/**
 * @brief lv_palette_lighten function
 */
static jerry_value_t js_lv_palette_lighten(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_palette_lighten: Insufficient arguments");
    }

    // 解析参数: p (lv_palette_t)
    jerry_value_t js_arg_p = args[0];
    if (!jerry_value_is_number(js_arg_p)) {
        return script_engine_throw_error("lv_palette_lighten: Argument 0 must be a number");
    }

    int arg_p = (int)jerry_value_as_number(js_arg_p);

    // 解析参数: lvl (uint8_t)
    jerry_value_t js_arg_lvl = args[1];
    if (!jerry_value_is_number(js_arg_lvl)) {
        return script_engine_throw_error("lv_palette_lighten: Argument 1 must be a number");
    }

    uint8_t arg_lvl = (uint8_t)jerry_value_as_number(js_arg_lvl);

    // 调用底层函数
    lv_color_t ret_value = lv_palette_lighten(arg_p, arg_lvl);

    // 处理返回值
    jerry_value_t js_result;
    // 转换为JS颜色对象
    js_result = lv_js_bridge_color_2_obj(&ret_value);

    return js_result;
}



/**
 * @brief lv_palette_main function
 */
static jerry_value_t js_lv_palette_main(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_palette_main: Insufficient arguments");
    }

    // 解析参数: p (lv_palette_t)
    jerry_value_t js_arg_p = args[0];
    if (!jerry_value_is_number(js_arg_p)) {
        return script_engine_throw_error("lv_palette_main: Argument 0 must be a number");
    }

    int arg_p = (int)jerry_value_as_number(js_arg_p);

    // 调用底层函数
    lv_color_t ret_value = lv_palette_main(arg_p);

    // 处理返回值
    jerry_value_t js_result;
    // 转换为JS颜色对象
    js_result = lv_js_bridge_color_2_obj(&ret_value);

    return js_result;
}



/**
 * @brief Get the active screen of the default display pointer to the active screen
 */
static jerry_value_t js_lv_screen_active(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 调用底层函数
    lv_obj_t* ret_value = lv_screen_active();

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_OBJ);

    return js_result;
}



/**
 * @brief Load a screen on the default display
 */
static jerry_value_t js_lv_screen_load(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_screen_load: Insufficient arguments");
    }

    // 解析参数: scr (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_scr = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_screen_load(arg_scr);

    return jerry_undefined();
}



/**
 * @brief Switch screen with animation
 */
static jerry_value_t js_lv_screen_load_anim(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 5) {
        return script_engine_throw_error("lv_screen_load_anim: Insufficient arguments");
    }

    // 解析参数: scr (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_scr = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: anim_type (lv_screen_load_anim_t)
    jerry_value_t js_arg_anim_type = args[1];
    if (!jerry_value_is_number(js_arg_anim_type)) {
        return script_engine_throw_error("lv_screen_load_anim: Argument 1 must be a number");
    }

    int arg_anim_type = (int)jerry_value_as_number(js_arg_anim_type);

    // 解析参数: time (uint32_t)
    jerry_value_t js_arg_time = args[2];
    if (!jerry_value_is_number(js_arg_time)) {
        return script_engine_throw_error("lv_screen_load_anim: Argument 2 must be a number");
    }

    uint32_t arg_time = (uint32_t)jerry_value_as_number(js_arg_time);

    // 解析参数: delay (uint32_t)
    jerry_value_t js_arg_delay = args[3];
    if (!jerry_value_is_number(js_arg_delay)) {
        return script_engine_throw_error("lv_screen_load_anim: Argument 3 must be a number");
    }

    uint32_t arg_delay = (uint32_t)jerry_value_as_number(js_arg_delay);

    // 解析参数: auto_del (bool)
    // 布尔类型参数: auto_del
    bool arg_auto_del = false;
    if (!jerry_value_is_undefined(args[4])) {
        if (jerry_value_is_boolean(args[4])) {
            arg_auto_del = jerry_value_to_boolean(args[4]);
        }
        else if (jerry_value_is_number(args[4])) {
            arg_auto_del = (jerry_value_as_number(args[4]) != 0);
        }
        else {
            return script_engine_throw_error("lv_screen_load_anim: Argument 4 must be boolean or number for bool");
        }
    }

    // 调用底层函数
    lv_screen_load_anim(arg_scr, arg_anim_type, arg_time, arg_delay, arg_auto_del);

    return jerry_undefined();
}



/**
 * @brief Bind an integer subject to a slider's value pointer to the created observer
 */
static jerry_value_t js_lv_slider_bind_value(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_slider_bind_value: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: subject (lv_subject_t*)
    // 指针类型参数，支持null
    void* arg_subject = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_ANY);
    // 调用底层函数
    lv_observer_t* ret_value = lv_slider_bind_value(arg_obj, arg_subject);

    // 处理返回值
    jerry_value_t js_result;
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

    return js_result;
}



/**
 * @brief Create a slider object pointer to the created slider
 */
static jerry_value_t js_lv_slider_create(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_slider_create: Insufficient arguments");
    }

    // 解析参数: parent (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_parent = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_t* ret_value = lv_slider_create(arg_parent);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_OBJ);

    return js_result;
}



/**
 * @brief Get the value of the left knob of a slider the value of the left knob of the slider
 */
static jerry_value_t js_lv_slider_get_left_value(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_slider_get_left_value: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_slider_get_left_value(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the maximum value of a slider the maximum value of the slider
 */
static jerry_value_t js_lv_slider_get_max_value(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_slider_get_max_value: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_slider_get_max_value(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the minimum value of a slider the minimum value of the slider
 */
static jerry_value_t js_lv_slider_get_min_value(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_slider_get_min_value: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_slider_get_min_value(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the mode of the slider. see ::lv_slider_mode_t
 */
static jerry_value_t js_lv_slider_get_mode(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_slider_get_mode: Insufficient arguments");
    }

    // 解析参数: slider (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_slider = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_slider_mode_t ret_value = lv_slider_get_mode(arg_slider);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Get the value of the main knob of a slider the value of the main knob of the slider
 */
static jerry_value_t js_lv_slider_get_value(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_slider_get_value: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    int32_t ret_value = lv_slider_get_value(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Give the slider is being dragged or not true: drag in progress false: not dragged
 */
static jerry_value_t js_lv_slider_is_dragged(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_slider_is_dragged: Insufficient arguments");
    }

    // 解析参数: obj (const lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    bool ret_value = lv_slider_is_dragged(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Give the slider is in symmetrical mode or not true: in symmetrical mode false : not in
 */
static jerry_value_t js_lv_slider_is_symmetrical(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_slider_is_symmetrical: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    bool ret_value = lv_slider_is_symmetrical(arg_obj);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Set a new value for the left knob of a slider
 */
static jerry_value_t js_lv_slider_set_left_value(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_slider_set_left_value: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_slider_set_left_value: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: anim (lv_anim_enable_t)
    jerry_value_t js_arg_anim = args[2];
    if (!jerry_value_is_number(js_arg_anim)) {
        return script_engine_throw_error("lv_slider_set_left_value: Argument 2 must be a number");
    }

    int arg_anim = (int)jerry_value_as_number(js_arg_anim);

    // 调用底层函数
    lv_slider_set_left_value(arg_obj, arg_value, arg_anim);

    return jerry_undefined();
}



/**
 * @brief Set the mode of slider.
 */
static jerry_value_t js_lv_slider_set_mode(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_slider_set_mode: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: mode (lv_slider_mode_t)
    jerry_value_t js_arg_mode = args[1];
    if (!jerry_value_is_number(js_arg_mode)) {
        return script_engine_throw_error("lv_slider_set_mode: Argument 1 must be a number");
    }

    int arg_mode = (int)jerry_value_as_number(js_arg_mode);

    // 调用底层函数
    lv_slider_set_mode(arg_obj, arg_mode);

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
        return script_engine_throw_error("lv_slider_set_range: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: min (int32_t)
    jerry_value_t js_arg_min = args[1];
    if (!jerry_value_is_number(js_arg_min)) {
        return script_engine_throw_error("lv_slider_set_range: Argument 1 must be a number");
    }

    int32_t arg_min = (int32_t)jerry_value_as_number(js_arg_min);

    // 解析参数: max (int32_t)
    jerry_value_t js_arg_max = args[2];
    if (!jerry_value_is_number(js_arg_max)) {
        return script_engine_throw_error("lv_slider_set_range: Argument 2 must be a number");
    }

    int32_t arg_max = (int32_t)jerry_value_as_number(js_arg_max);

    // 调用底层函数
    lv_slider_set_range(arg_obj, arg_min, arg_max);

    return jerry_undefined();
}



/**
 * @brief Set a new value on the slider
 */
static jerry_value_t js_lv_slider_set_value(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_slider_set_value: Insufficient arguments");
    }

    // 解析参数: obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_obj = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: value (int32_t)
    jerry_value_t js_arg_value = args[1];
    if (!jerry_value_is_number(js_arg_value)) {
        return script_engine_throw_error("lv_slider_set_value: Argument 1 must be a number");
    }

    int32_t arg_value = (int32_t)jerry_value_as_number(js_arg_value);

    // 解析参数: anim (lv_anim_enable_t)
    jerry_value_t js_arg_anim = args[2];
    if (!jerry_value_is_number(js_arg_anim)) {
        return script_engine_throw_error("lv_slider_set_value: Argument 2 must be a number");
    }

    int arg_anim = (int)jerry_value_as_number(js_arg_anim);

    // 调用底层函数
    lv_slider_set_value(arg_obj, arg_value, arg_anim);

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
        return script_engine_throw_error("lv_switch_create: Insufficient arguments");
    }

    // 解析参数: parent (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_parent = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 调用底层函数
    lv_obj_t* ret_value = lv_switch_create(arg_parent);

    // 处理返回值
    jerry_value_t js_result;
    // 包装为LVGL对象
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_OBJ);

    return js_result;
}



/**
 * @brief lv_tileview_set_tile function
 */
static jerry_value_t js_lv_tileview_set_tile(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_tileview_set_tile: Insufficient arguments");
    }

    // 解析参数: tv (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_tv = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_OBJ);
    // 解析参数: tile_obj (lv_obj_t*)
    // 指针类型参数，支持null
    void* arg_tile_obj = lv_js_bridge_obj_2_ptr(args[1],LV_TYPE_OBJ);
    // 解析参数: anim_en (lv_anim_enable_t)
    jerry_value_t js_arg_anim_en = args[2];
    if (!jerry_value_is_number(js_arg_anim_en)) {
        return script_engine_throw_error("lv_tileview_set_tile: Argument 2 must be a number");
    }

    int arg_anim_en = (int)jerry_value_as_number(js_arg_anim_en);

    // 调用底层函数
    lv_tileview_set_tile(arg_tv, arg_tile_obj, arg_anim_en);

    return jerry_undefined();
}



/**
 * @brief Create a new lv_timer pointer to the new timer
 */
static jerry_value_t js_lv_timer_create(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 3) {
        return script_engine_throw_error("lv_timer_create: Insufficient arguments");
    }

    // 解析参数: timer_xcb (lv_timer_cb_t)
    // 通用指针类型: lv_timer_cb_t，支持null
    lv_timer_cb_t arg_timer_xcb = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_ANY);

    // 解析参数: period (uint32_t)
    jerry_value_t js_arg_period = args[1];
    if (!jerry_value_is_number(js_arg_period)) {
        return script_engine_throw_error("lv_timer_create: Argument 1 must be a number");
    }

    uint32_t arg_period = (uint32_t)jerry_value_as_number(js_arg_period);

    // 解析参数: user_data (void*)
    // void*/字符串 类型参数，支持null
    void* arg_user_data = NULL;
    char* arg_user_data_str = NULL;  // 用于字符串参数的临时存储

    if (!jerry_value_is_undefined(args[2]) && !jerry_value_is_null(args[2])) {
        if (jerry_value_is_string(args[2])) {
            // 处理字符串类型的符号（如LV_SYMBOL_MINUS）
            jerry_size_t arg_user_data_len = jerry_string_size(args[2], JERRY_ENCODING_UTF8);
            arg_user_data_str = (char*)eos_malloc(arg_user_data_len + 1);
            if (!arg_user_data_str) {
                return script_engine_throw_error("lv_timer_create: Failed to allocate memory for string argument");
            }
            jerry_string_to_buffer(args[2], JERRY_ENCODING_UTF8, (jerry_char_t*)arg_user_data_str, arg_user_data_len);
            arg_user_data_str[arg_user_data_len] = '\0';
            arg_user_data = (void*)arg_user_data_str;
        }
        else if (jerry_value_is_object(args[2])) {
            // 尝试从对象获取指针
            arg_user_data = lv_js_bridge_obj_2_ptr(args[2],LV_TYPE_ANY);
        }
        else if (jerry_value_is_number(args[2])) {
            // 直接传递指针数值
            uintptr_t ptr_num = (uintptr_t)jerry_value_as_number(args[2]);
            arg_user_data = (void*)ptr_num;
        }
        else {
            return script_engine_throw_error("lv_timer_create: Argument 2 must be string, object or number");
        }
    }
    // 调用底层函数
    lv_timer_t* ret_value = lv_timer_create(arg_timer_xcb, arg_period, arg_user_data);

    // 处理返回值
    jerry_value_t js_result;
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

    // 释放临时内存
    if (arg_user_data_str) eos_free(arg_user_data_str);

    return js_result;
}



/**
 * @brief Create an "empty" timer. It needs to be initialized with at least lv_timer_set_cb and lv_timer_set_period  pointer to the created timer
 */
static jerry_value_t js_lv_timer_create_basic(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 调用底层函数
    lv_timer_t* ret_value = lv_timer_create_basic();

    // 处理返回值
    jerry_value_t js_result;
    js_result = lv_js_bridge_ptr_2_obj(ret_value, LV_TYPE_ANY);

    return js_result;
}



/**
 * @brief Delete a lv_timer
 */
static jerry_value_t js_lv_timer_delete(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_timer_delete: Insufficient arguments");
    }

    // 解析参数: timer (lv_timer_t*)
    // 指针类型参数，支持null
    void* arg_timer = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_TIMER);
    // 调用底层函数
    lv_timer_delete(arg_timer);

    return jerry_undefined();
}



/**
 * @brief Get the pause state of a timer true: timer is paused; false: timer is running
 */
static jerry_value_t js_lv_timer_get_paused(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_timer_get_paused: Insufficient arguments");
    }

    // 解析参数: timer (lv_timer_t*)
    // 指针类型参数，支持null
    void* arg_timer = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_TIMER);
    // 调用底层函数
    bool ret_value = lv_timer_get_paused(arg_timer);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief Pause a timer.
 */
static jerry_value_t js_lv_timer_pause(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_timer_pause: Insufficient arguments");
    }

    // 解析参数: timer (lv_timer_t*)
    // 指针类型参数，支持null
    void* arg_timer = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_TIMER);
    // 调用底层函数
    lv_timer_pause(arg_timer);

    return jerry_undefined();
}



/**
 * @brief Make a lv_timer ready. It will not wait its period.
 */
static jerry_value_t js_lv_timer_ready(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_timer_ready: Insufficient arguments");
    }

    // 解析参数: timer (lv_timer_t*)
    // 指针类型参数，支持null
    void* arg_timer = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_TIMER);
    // 调用底层函数
    lv_timer_ready(arg_timer);

    return jerry_undefined();
}



/**
 * @brief Reset a lv_timer. It will be called the previously set period milliseconds later.
 */
static jerry_value_t js_lv_timer_reset(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_timer_reset: Insufficient arguments");
    }

    // 解析参数: timer (lv_timer_t*)
    // 指针类型参数，支持null
    void* arg_timer = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_TIMER);
    // 调用底层函数
    lv_timer_reset(arg_timer);

    return jerry_undefined();
}



/**
 * @brief Resume a timer.
 */
static jerry_value_t js_lv_timer_resume(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_timer_resume: Insufficient arguments");
    }

    // 解析参数: timer (lv_timer_t*)
    // 指针类型参数，支持null
    void* arg_timer = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_TIMER);
    // 调用底层函数
    lv_timer_resume(arg_timer);

    return jerry_undefined();
}



/**
 * @brief Set whether a lv_timer will be deleted automatically when it is called repeat_count times.
 */
static jerry_value_t js_lv_timer_set_auto_delete(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_timer_set_auto_delete: Insufficient arguments");
    }

    // 解析参数: timer (lv_timer_t*)
    // 指针类型参数，支持null
    void* arg_timer = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_TIMER);
    // 解析参数: auto_delete (bool)
    // 布尔类型参数: auto_delete
    bool arg_auto_delete = false;
    if (!jerry_value_is_undefined(args[1])) {
        if (jerry_value_is_boolean(args[1])) {
            arg_auto_delete = jerry_value_to_boolean(args[1]);
        }
        else if (jerry_value_is_number(args[1])) {
            arg_auto_delete = (jerry_value_as_number(args[1]) != 0);
        }
        else {
            return script_engine_throw_error("lv_timer_set_auto_delete: Argument 1 must be boolean or number for bool");
        }
    }

    // 调用底层函数
    lv_timer_set_auto_delete(arg_timer, arg_auto_delete);

    return jerry_undefined();
}



/**
 * @brief Set new period for a lv_timer
 */
static jerry_value_t js_lv_timer_set_period(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_timer_set_period: Insufficient arguments");
    }

    // 解析参数: timer (lv_timer_t*)
    // 指针类型参数，支持null
    void* arg_timer = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_TIMER);
    // 解析参数: period (uint32_t)
    jerry_value_t js_arg_period = args[1];
    if (!jerry_value_is_number(js_arg_period)) {
        return script_engine_throw_error("lv_timer_set_period: Argument 1 must be a number");
    }

    uint32_t arg_period = (uint32_t)jerry_value_as_number(js_arg_period);

    // 调用底层函数
    lv_timer_set_period(arg_timer, arg_period);

    return jerry_undefined();
}



/**
 * @brief Set the number of times a timer will repeat.
 */
static jerry_value_t js_lv_timer_set_repeat_count(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 2) {
        return script_engine_throw_error("lv_timer_set_repeat_count: Insufficient arguments");
    }

    // 解析参数: timer (lv_timer_t*)
    // 指针类型参数，支持null
    void* arg_timer = lv_js_bridge_obj_2_ptr(args[0],LV_TYPE_TIMER);
    // 解析参数: repeat_count (int32_t)
    jerry_value_t js_arg_repeat_count = args[1];
    if (!jerry_value_is_number(js_arg_repeat_count)) {
        return script_engine_throw_error("lv_timer_set_repeat_count: Argument 1 must be a number");
    }

    int32_t arg_repeat_count = (int32_t)jerry_value_as_number(js_arg_repeat_count);

    // 调用底层函数
    lv_timer_set_repeat_count(arg_timer, arg_repeat_count);

    return jerry_undefined();
}



/**
 * @brief lv_trigo_cos function
 */
static jerry_value_t js_lv_trigo_cos(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_trigo_cos: Insufficient arguments");
    }

    // 解析参数: angle (int16_t)
    jerry_value_t js_arg_angle = args[0];
    if (!jerry_value_is_number(js_arg_angle)) {
        return script_engine_throw_error("lv_trigo_cos: Argument 0 must be a number");
    }

    int16_t arg_angle = (int16_t)jerry_value_as_number(js_arg_angle);

    // 调用底层函数
    int32_t ret_value = lv_trigo_cos(arg_angle);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



/**
 * @brief lv_trigo_sin function
 */
static jerry_value_t js_lv_trigo_sin(const jerry_call_info_t* call_info_p,
    const jerry_value_t args[],
    const jerry_length_t argc) {
    // 参数数量检查
    if (argc < 1) {
        return script_engine_throw_error("lv_trigo_sin: Insufficient arguments");
    }

    // 解析参数: angle (int16_t)
    jerry_value_t js_arg_angle = args[0];
    if (!jerry_value_is_number(js_arg_angle)) {
        return script_engine_throw_error("lv_trigo_sin: Argument 0 must be a number");
    }

    int16_t arg_angle = (int16_t)jerry_value_as_number(js_arg_angle);

    // 调用底层函数
    int32_t ret_value = lv_trigo_sin(arg_angle);

    // 处理返回值
    jerry_value_t js_result;
    js_result = jerry_number(ret_value);

    return js_result;
}



const script_engine_func_entry_t lvgl_binding_funcs[] = {
    { "delay", "ms", js_lv_delay_ms },
    { "timer", "createBasic", js_lv_timer_create_basic },
    { "timer", "create", js_lv_timer_create },
    { "timer", "delete", js_lv_timer_delete },
    { "timer", "pause", js_lv_timer_pause },
    { "timer", "resume", js_lv_timer_resume },
    { "timer", "setPeriod", js_lv_timer_set_period },
    { "timer", "ready", js_lv_timer_ready },
    { "timer", "setRepeatCount", js_lv_timer_set_repeat_count },
    { "timer", "setAutoDelete", js_lv_timer_set_auto_delete },
    { "timer", "reset", js_lv_timer_reset },
    { "timer", "getPaused", js_lv_timer_get_paused },
    { "trigo", "sin", js_lv_trigo_sin },
    { "trigo", "cos", js_lv_trigo_cos },
    { "color", "hex", js_lv_color_hex },
    { "palette", "main", js_lv_palette_main },
    { "palette", "lighten", js_lv_palette_lighten },
    { "palette", "darken", js_lv_palette_darken },
    { "image", "bufFree", js_lv_image_buf_free },
    { "image", "cacheInit", js_lv_image_cache_init },
    { "image", "cacheResize", js_lv_image_cache_resize },
    { "image", "cacheDrop", js_lv_image_cache_drop },
    { "image", "cacheIsEnabled", js_lv_image_cache_is_enabled },
    { "image", "headerCacheInit", js_lv_image_header_cache_init },
    { "image", "headerCacheResize", js_lv_image_header_cache_resize },
    { "image", "headerCacheDrop", js_lv_image_header_cache_drop },
    { "image", "headerCacheIsEnabled", js_lv_image_header_cache_is_enabled },
    { "obj", "setFlexFlow", js_lv_obj_set_flex_flow },
    { "obj", "setFlexAlign", js_lv_obj_set_flex_align },
    { "obj", "setFlexGrow", js_lv_obj_set_flex_grow },
    { "obj", "setGridAlign", js_lv_obj_set_grid_align },
    { "obj", "setGridCell", js_lv_obj_set_grid_cell },
    { "image", "decoderGetInfo", js_lv_image_decoder_get_info },
    { "image", "decoderOpen", js_lv_image_decoder_open },
    { "image", "decoderGetArea", js_lv_image_decoder_get_area },
    { "image", "decoderClose", js_lv_image_decoder_close },
    { "image", "decoderCreate", js_lv_image_decoder_create },
    { "image", "decoderDelete", js_lv_image_decoder_delete },
    { "image", "decoderGetNext", js_lv_image_decoder_get_next },
    { "image", "decoderSetInfoCb", js_lv_image_decoder_set_info_cb },
    { "image", "decoderSetOpenCb", js_lv_image_decoder_set_open_cb },
    { "image", "decoderSetGetAreaCb", js_lv_image_decoder_set_get_area_cb },
    { "image", "decoderSetCloseCb", js_lv_image_decoder_set_close_cb },
    { "image", "decoderAddToCache", js_lv_image_decoder_add_to_cache },
    { "image", "decoderPostProcess", js_lv_image_decoder_post_process },
    { "screen", "load", js_lv_screen_load },
    { "screen", "loadAnim", js_lv_screen_load_anim },
    { "screen", "active", js_lv_screen_active },
    { "obj", "delete", js_lv_obj_delete },
    { "obj", "clean", js_lv_obj_clean },
    { "obj", "deleteDelayed", js_lv_obj_delete_delayed },
    { "obj", "deleteAnimCompletedCb", js_lv_obj_delete_anim_completed_cb },
    { "obj", "deleteAsync", js_lv_obj_delete_async },
    { "obj", "setParent", js_lv_obj_set_parent },
    { "obj", "swap", js_lv_obj_swap },
    { "obj", "moveToIndex", js_lv_obj_move_to_index },
    { "obj", "getScreen", js_lv_obj_get_screen },
    { "obj", "getDisplay", js_lv_obj_get_display },
    { "obj", "getParent", js_lv_obj_get_parent },
    { "obj", "getChild", js_lv_obj_get_child },
    { "obj", "getChildByType", js_lv_obj_get_child_by_type },
    { "obj", "getSibling", js_lv_obj_get_sibling },
    { "obj", "getSiblingByType", js_lv_obj_get_sibling_by_type },
    { "obj", "getChildCount", js_lv_obj_get_child_count },
    { "obj", "getChildCountByType", js_lv_obj_get_child_count_by_type },
    { "obj", "getIndex", js_lv_obj_get_index },
    { "obj", "getIndexByType", js_lv_obj_get_index_by_type },
    { "obj", "treeWalk", js_lv_obj_tree_walk },
    { "obj", "dumpTree", js_lv_obj_dump_tree },
    { "obj", "setPos", js_lv_obj_set_pos },
    { "obj", "setX", js_lv_obj_set_x },
    { "obj", "setY", js_lv_obj_set_y },
    { "obj", "setSize", js_lv_obj_set_size },
    { "obj", "refrSize", js_lv_obj_refr_size },
    { "obj", "setWidth", js_lv_obj_set_width },
    { "obj", "setHeight", js_lv_obj_set_height },
    { "obj", "setContentWidth", js_lv_obj_set_content_width },
    { "obj", "setContentHeight", js_lv_obj_set_content_height },
    { "obj", "setLayout", js_lv_obj_set_layout },
    { "obj", "isLayoutPositioned", js_lv_obj_is_layout_positioned },
    { "obj", "markLayoutAsDirty", js_lv_obj_mark_layout_as_dirty },
    { "obj", "updateLayout", js_lv_obj_update_layout },
    { "obj", "setAlign", js_lv_obj_set_align },
    { "obj", "align", js_lv_obj_align },
    { "obj", "alignTo", js_lv_obj_align_to },
    { "obj", "center", js_lv_obj_center },
    { "obj", "getCoords", js_lv_obj_get_coords },
    { "obj", "getX", js_lv_obj_get_x },
    { "obj", "getX2", js_lv_obj_get_x2 },
    { "obj", "getY", js_lv_obj_get_y },
    { "obj", "getY2", js_lv_obj_get_y2 },
    { "obj", "getXAligned", js_lv_obj_get_x_aligned },
    { "obj", "getYAligned", js_lv_obj_get_y_aligned },
    { "obj", "getWidth", js_lv_obj_get_width },
    { "obj", "getHeight", js_lv_obj_get_height },
    { "obj", "getContentWidth", js_lv_obj_get_content_width },
    { "obj", "getContentHeight", js_lv_obj_get_content_height },
    { "obj", "getContentCoords", js_lv_obj_get_content_coords },
    { "obj", "getSelfWidth", js_lv_obj_get_self_width },
    { "obj", "getSelfHeight", js_lv_obj_get_self_height },
    { "obj", "refreshSelfSize", js_lv_obj_refresh_self_size },
    { "obj", "refrPos", js_lv_obj_refr_pos },
    { "obj", "moveTo", js_lv_obj_move_to },
    { "obj", "moveChildrenBy", js_lv_obj_move_children_by },
    { "obj", "transformPoint", js_lv_obj_transform_point },
    { "obj", "getTransformedArea", js_lv_obj_get_transformed_area },
    { "obj", "invalidateArea", js_lv_obj_invalidate_area },
    { "obj", "invalidate", js_lv_obj_invalidate },
    { "obj", "areaIsVisible", js_lv_obj_area_is_visible },
    { "obj", "isVisible", js_lv_obj_is_visible },
    { "obj", "setExtClickArea", js_lv_obj_set_ext_click_area },
    { "obj", "getClickArea", js_lv_obj_get_click_area },
    { "obj", "hitTest", js_lv_obj_hit_test },
    { "obj", "setScrollbarMode", js_lv_obj_set_scrollbar_mode },
    { "obj", "setScrollDir", js_lv_obj_set_scroll_dir },
    { "obj", "setScrollSnapX", js_lv_obj_set_scroll_snap_x },
    { "obj", "setScrollSnapY", js_lv_obj_set_scroll_snap_y },
    { "obj", "getScrollbarMode", js_lv_obj_get_scrollbar_mode },
    { "obj", "getScrollDir", js_lv_obj_get_scroll_dir },
    { "obj", "getScrollSnapX", js_lv_obj_get_scroll_snap_x },
    { "obj", "getScrollSnapY", js_lv_obj_get_scroll_snap_y },
    { "obj", "getScrollX", js_lv_obj_get_scroll_x },
    { "obj", "getScrollY", js_lv_obj_get_scroll_y },
    { "obj", "getScrollTop", js_lv_obj_get_scroll_top },
    { "obj", "getScrollBottom", js_lv_obj_get_scroll_bottom },
    { "obj", "getScrollLeft", js_lv_obj_get_scroll_left },
    { "obj", "getScrollRight", js_lv_obj_get_scroll_right },
    { "obj", "getScrollEnd", js_lv_obj_get_scroll_end },
    { "obj", "scrollBy", js_lv_obj_scroll_by },
    { "obj", "scrollByBounded", js_lv_obj_scroll_by_bounded },
    { "obj", "scrollTo", js_lv_obj_scroll_to },
    { "obj", "scrollToX", js_lv_obj_scroll_to_x },
    { "obj", "scrollToY", js_lv_obj_scroll_to_y },
    { "obj", "scrollToView", js_lv_obj_scroll_to_view },
    { "obj", "scrollToViewRecursive", js_lv_obj_scroll_to_view_recursive },
    { "obj", "isScrolling", js_lv_obj_is_scrolling },
    { "obj", "updateSnap", js_lv_obj_update_snap },
    { "obj", "getScrollbarArea", js_lv_obj_get_scrollbar_area },
    { "obj", "scrollbarInvalidate", js_lv_obj_scrollbar_invalidate },
    { "obj", "readjustScroll", js_lv_obj_readjust_scroll },
    { "obj", "addStyle", js_lv_obj_add_style },
    { "obj", "replaceStyle", js_lv_obj_replace_style },
    { "obj", "removeStyle", js_lv_obj_remove_style },
    { "obj", "removeStyleAll", js_lv_obj_remove_style_all },
    { "obj", "reportStyleChange", js_lv_obj_report_style_change },
    { "obj", "refreshStyle", js_lv_obj_refresh_style },
    { "obj", "enableStyleRefresh", js_lv_obj_enable_style_refresh },
    { "obj", "fadeIn", js_lv_obj_fade_in },
    { "obj", "fadeOut", js_lv_obj_fade_out },
    { "obj", "styleGetSelectorState", js_lv_obj_style_get_selector_state },
    { "obj", "styleGetSelectorPart", js_lv_obj_style_get_selector_part },
    { "obj", "getStyleWidth", js_lv_obj_get_style_width },
    { "obj", "getStyleMinWidth", js_lv_obj_get_style_min_width },
    { "obj", "getStyleMaxWidth", js_lv_obj_get_style_max_width },
    { "obj", "getStyleHeight", js_lv_obj_get_style_height },
    { "obj", "getStyleMinHeight", js_lv_obj_get_style_min_height },
    { "obj", "getStyleMaxHeight", js_lv_obj_get_style_max_height },
    { "obj", "getStyleLength", js_lv_obj_get_style_length },
    { "obj", "getStyleX", js_lv_obj_get_style_x },
    { "obj", "getStyleY", js_lv_obj_get_style_y },
    { "obj", "getStyleAlign", js_lv_obj_get_style_align },
    { "obj", "getStyleTransformWidth", js_lv_obj_get_style_transform_width },
    { "obj", "getStyleTransformHeight", js_lv_obj_get_style_transform_height },
    { "obj", "getStyleTranslateX", js_lv_obj_get_style_translate_x },
    { "obj", "getStyleTranslateY", js_lv_obj_get_style_translate_y },
    { "obj", "getStyleTransformScaleX", js_lv_obj_get_style_transform_scale_x },
    { "obj", "getStyleTransformScaleY", js_lv_obj_get_style_transform_scale_y },
    { "obj", "getStyleTransformRotation", js_lv_obj_get_style_transform_rotation },
    { "obj", "getStyleTransformPivotX", js_lv_obj_get_style_transform_pivot_x },
    { "obj", "getStyleTransformPivotY", js_lv_obj_get_style_transform_pivot_y },
    { "obj", "getStyleTransformSkewX", js_lv_obj_get_style_transform_skew_x },
    { "obj", "getStyleTransformSkewY", js_lv_obj_get_style_transform_skew_y },
    { "obj", "getStylePadTop", js_lv_obj_get_style_pad_top },
    { "obj", "getStylePadBottom", js_lv_obj_get_style_pad_bottom },
    { "obj", "getStylePadLeft", js_lv_obj_get_style_pad_left },
    { "obj", "getStylePadRight", js_lv_obj_get_style_pad_right },
    { "obj", "getStylePadRow", js_lv_obj_get_style_pad_row },
    { "obj", "getStylePadColumn", js_lv_obj_get_style_pad_column },
    { "obj", "getStyleMarginTop", js_lv_obj_get_style_margin_top },
    { "obj", "getStyleMarginBottom", js_lv_obj_get_style_margin_bottom },
    { "obj", "getStyleMarginLeft", js_lv_obj_get_style_margin_left },
    { "obj", "getStyleMarginRight", js_lv_obj_get_style_margin_right },
    { "obj", "getStyleBgColor", js_lv_obj_get_style_bg_color },
    { "obj", "getStyleBgColorFiltered", js_lv_obj_get_style_bg_color_filtered },
    { "obj", "getStyleBgOpa", js_lv_obj_get_style_bg_opa },
    { "obj", "getStyleBgGradColor", js_lv_obj_get_style_bg_grad_color },
    { "obj", "getStyleBgGradColorFiltered", js_lv_obj_get_style_bg_grad_color_filtered },
    { "obj", "getStyleBgGradDir", js_lv_obj_get_style_bg_grad_dir },
    { "obj", "getStyleBgMainStop", js_lv_obj_get_style_bg_main_stop },
    { "obj", "getStyleBgGradStop", js_lv_obj_get_style_bg_grad_stop },
    { "obj", "getStyleBgMainOpa", js_lv_obj_get_style_bg_main_opa },
    { "obj", "getStyleBgGradOpa", js_lv_obj_get_style_bg_grad_opa },
    { "obj", "getStyleBgGrad", js_lv_obj_get_style_bg_grad },
    { "obj", "getStyleBgImageSrc", js_lv_obj_get_style_bg_image_src },
    { "obj", "getStyleBgImageOpa", js_lv_obj_get_style_bg_image_opa },
    { "obj", "getStyleBgImageRecolor", js_lv_obj_get_style_bg_image_recolor },
    { "obj", "getStyleBgImageRecolorFiltered", js_lv_obj_get_style_bg_image_recolor_filtered },
    { "obj", "getStyleBgImageRecolorOpa", js_lv_obj_get_style_bg_image_recolor_opa },
    { "obj", "getStyleBgImageTiled", js_lv_obj_get_style_bg_image_tiled },
    { "obj", "getStyleBorderColor", js_lv_obj_get_style_border_color },
    { "obj", "getStyleBorderColorFiltered", js_lv_obj_get_style_border_color_filtered },
    { "obj", "getStyleBorderOpa", js_lv_obj_get_style_border_opa },
    { "obj", "getStyleBorderWidth", js_lv_obj_get_style_border_width },
    { "obj", "getStyleBorderSide", js_lv_obj_get_style_border_side },
    { "obj", "getStyleBorderPost", js_lv_obj_get_style_border_post },
    { "obj", "getStyleOutlineWidth", js_lv_obj_get_style_outline_width },
    { "obj", "getStyleOutlineColor", js_lv_obj_get_style_outline_color },
    { "obj", "getStyleOutlineColorFiltered", js_lv_obj_get_style_outline_color_filtered },
    { "obj", "getStyleOutlineOpa", js_lv_obj_get_style_outline_opa },
    { "obj", "getStyleOutlinePad", js_lv_obj_get_style_outline_pad },
    { "obj", "getStyleShadowWidth", js_lv_obj_get_style_shadow_width },
    { "obj", "getStyleShadowOffsetX", js_lv_obj_get_style_shadow_offset_x },
    { "obj", "getStyleShadowOffsetY", js_lv_obj_get_style_shadow_offset_y },
    { "obj", "getStyleShadowSpread", js_lv_obj_get_style_shadow_spread },
    { "obj", "getStyleShadowColor", js_lv_obj_get_style_shadow_color },
    { "obj", "getStyleShadowColorFiltered", js_lv_obj_get_style_shadow_color_filtered },
    { "obj", "getStyleShadowOpa", js_lv_obj_get_style_shadow_opa },
    { "obj", "getStyleImageOpa", js_lv_obj_get_style_image_opa },
    { "obj", "getStyleImageRecolor", js_lv_obj_get_style_image_recolor },
    { "obj", "getStyleImageRecolorFiltered", js_lv_obj_get_style_image_recolor_filtered },
    { "obj", "getStyleImageRecolorOpa", js_lv_obj_get_style_image_recolor_opa },
    { "obj", "getStyleLineWidth", js_lv_obj_get_style_line_width },
    { "obj", "getStyleLineDashWidth", js_lv_obj_get_style_line_dash_width },
    { "obj", "getStyleLineDashGap", js_lv_obj_get_style_line_dash_gap },
    { "obj", "getStyleLineRounded", js_lv_obj_get_style_line_rounded },
    { "obj", "getStyleLineColor", js_lv_obj_get_style_line_color },
    { "obj", "getStyleLineColorFiltered", js_lv_obj_get_style_line_color_filtered },
    { "obj", "getStyleLineOpa", js_lv_obj_get_style_line_opa },
    { "obj", "getStyleArcWidth", js_lv_obj_get_style_arc_width },
    { "obj", "getStyleArcRounded", js_lv_obj_get_style_arc_rounded },
    { "obj", "getStyleArcColor", js_lv_obj_get_style_arc_color },
    { "obj", "getStyleArcColorFiltered", js_lv_obj_get_style_arc_color_filtered },
    { "obj", "getStyleArcOpa", js_lv_obj_get_style_arc_opa },
    { "obj", "getStyleArcImageSrc", js_lv_obj_get_style_arc_image_src },
    { "obj", "getStyleTextColor", js_lv_obj_get_style_text_color },
    { "obj", "getStyleTextColorFiltered", js_lv_obj_get_style_text_color_filtered },
    { "obj", "getStyleTextOpa", js_lv_obj_get_style_text_opa },
    { "obj", "getStyleTextFont", js_lv_obj_get_style_text_font },
    { "obj", "getStyleTextLetterSpace", js_lv_obj_get_style_text_letter_space },
    { "obj", "getStyleTextLineSpace", js_lv_obj_get_style_text_line_space },
    { "obj", "getStyleTextDecor", js_lv_obj_get_style_text_decor },
    { "obj", "getStyleTextAlign", js_lv_obj_get_style_text_align },
    { "obj", "getStyleRadius", js_lv_obj_get_style_radius },
    { "obj", "getStyleClipCorner", js_lv_obj_get_style_clip_corner },
    { "obj", "getStyleOpa", js_lv_obj_get_style_opa },
    { "obj", "getStyleOpaLayered", js_lv_obj_get_style_opa_layered },
    { "obj", "getStyleColorFilterDsc", js_lv_obj_get_style_color_filter_dsc },
    { "obj", "getStyleColorFilterOpa", js_lv_obj_get_style_color_filter_opa },
    { "obj", "getStyleAnim", js_lv_obj_get_style_anim },
    { "obj", "getStyleAnimDuration", js_lv_obj_get_style_anim_duration },
    { "obj", "getStyleTransition", js_lv_obj_get_style_transition },
    { "obj", "getStyleBlendMode", js_lv_obj_get_style_blend_mode },
    { "obj", "getStyleLayout", js_lv_obj_get_style_layout },
    { "obj", "getStyleBaseDir", js_lv_obj_get_style_base_dir },
    { "obj", "getStyleBitmapMaskSrc", js_lv_obj_get_style_bitmap_mask_src },
    { "obj", "getStyleRotarySensitivity", js_lv_obj_get_style_rotary_sensitivity },
    { "obj", "getStyleFlexFlow", js_lv_obj_get_style_flex_flow },
    { "obj", "getStyleFlexMainPlace", js_lv_obj_get_style_flex_main_place },
    { "obj", "getStyleFlexCrossPlace", js_lv_obj_get_style_flex_cross_place },
    { "obj", "getStyleFlexTrackPlace", js_lv_obj_get_style_flex_track_place },
    { "obj", "getStyleFlexGrow", js_lv_obj_get_style_flex_grow },
    { "obj", "getStyleGridColumnDscArray", js_lv_obj_get_style_grid_column_dsc_array },
    { "obj", "getStyleGridColumnAlign", js_lv_obj_get_style_grid_column_align },
    { "obj", "getStyleGridRowDscArray", js_lv_obj_get_style_grid_row_dsc_array },
    { "obj", "getStyleGridRowAlign", js_lv_obj_get_style_grid_row_align },
    { "obj", "getStyleGridCellColumnPos", js_lv_obj_get_style_grid_cell_column_pos },
    { "obj", "getStyleGridCellXAlign", js_lv_obj_get_style_grid_cell_x_align },
    { "obj", "getStyleGridCellColumnSpan", js_lv_obj_get_style_grid_cell_column_span },
    { "obj", "getStyleGridCellRowPos", js_lv_obj_get_style_grid_cell_row_pos },
    { "obj", "getStyleGridCellYAlign", js_lv_obj_get_style_grid_cell_y_align },
    { "obj", "getStyleGridCellRowSpan", js_lv_obj_get_style_grid_cell_row_span },
    { "obj", "setStyleWidth", js_lv_obj_set_style_width },
    { "obj", "setStyleMinWidth", js_lv_obj_set_style_min_width },
    { "obj", "setStyleMaxWidth", js_lv_obj_set_style_max_width },
    { "obj", "setStyleHeight", js_lv_obj_set_style_height },
    { "obj", "setStyleMinHeight", js_lv_obj_set_style_min_height },
    { "obj", "setStyleMaxHeight", js_lv_obj_set_style_max_height },
    { "obj", "setStyleLength", js_lv_obj_set_style_length },
    { "obj", "setStyleX", js_lv_obj_set_style_x },
    { "obj", "setStyleY", js_lv_obj_set_style_y },
    { "obj", "setStyleAlign", js_lv_obj_set_style_align },
    { "obj", "setStyleTransformWidth", js_lv_obj_set_style_transform_width },
    { "obj", "setStyleTransformHeight", js_lv_obj_set_style_transform_height },
    { "obj", "setStyleTranslateX", js_lv_obj_set_style_translate_x },
    { "obj", "setStyleTranslateY", js_lv_obj_set_style_translate_y },
    { "obj", "setStyleTransformScaleX", js_lv_obj_set_style_transform_scale_x },
    { "obj", "setStyleTransformScaleY", js_lv_obj_set_style_transform_scale_y },
    { "obj", "setStyleTransformRotation", js_lv_obj_set_style_transform_rotation },
    { "obj", "setStyleTransformPivotX", js_lv_obj_set_style_transform_pivot_x },
    { "obj", "setStyleTransformPivotY", js_lv_obj_set_style_transform_pivot_y },
    { "obj", "setStyleTransformSkewX", js_lv_obj_set_style_transform_skew_x },
    { "obj", "setStyleTransformSkewY", js_lv_obj_set_style_transform_skew_y },
    { "obj", "setStylePadTop", js_lv_obj_set_style_pad_top },
    { "obj", "setStylePadBottom", js_lv_obj_set_style_pad_bottom },
    { "obj", "setStylePadLeft", js_lv_obj_set_style_pad_left },
    { "obj", "setStylePadRight", js_lv_obj_set_style_pad_right },
    { "obj", "setStylePadRow", js_lv_obj_set_style_pad_row },
    { "obj", "setStylePadColumn", js_lv_obj_set_style_pad_column },
    { "obj", "setStyleMarginTop", js_lv_obj_set_style_margin_top },
    { "obj", "setStyleMarginBottom", js_lv_obj_set_style_margin_bottom },
    { "obj", "setStyleMarginLeft", js_lv_obj_set_style_margin_left },
    { "obj", "setStyleMarginRight", js_lv_obj_set_style_margin_right },
    { "obj", "setStyleBgColor", js_lv_obj_set_style_bg_color },
    { "obj", "setStyleBgOpa", js_lv_obj_set_style_bg_opa },
    { "obj", "setStyleBgGradColor", js_lv_obj_set_style_bg_grad_color },
    { "obj", "setStyleBgGradDir", js_lv_obj_set_style_bg_grad_dir },
    { "obj", "setStyleBgMainStop", js_lv_obj_set_style_bg_main_stop },
    { "obj", "setStyleBgGradStop", js_lv_obj_set_style_bg_grad_stop },
    { "obj", "setStyleBgMainOpa", js_lv_obj_set_style_bg_main_opa },
    { "obj", "setStyleBgGradOpa", js_lv_obj_set_style_bg_grad_opa },
    { "obj", "setStyleBgGrad", js_lv_obj_set_style_bg_grad },
    { "obj", "setStyleBgImageSrc", js_lv_obj_set_style_bg_image_src },
    { "obj", "setStyleBgImageOpa", js_lv_obj_set_style_bg_image_opa },
    { "obj", "setStyleBgImageRecolor", js_lv_obj_set_style_bg_image_recolor },
    { "obj", "setStyleBgImageRecolorOpa", js_lv_obj_set_style_bg_image_recolor_opa },
    { "obj", "setStyleBgImageTiled", js_lv_obj_set_style_bg_image_tiled },
    { "obj", "setStyleBorderColor", js_lv_obj_set_style_border_color },
    { "obj", "setStyleBorderOpa", js_lv_obj_set_style_border_opa },
    { "obj", "setStyleBorderWidth", js_lv_obj_set_style_border_width },
    { "obj", "setStyleBorderSide", js_lv_obj_set_style_border_side },
    { "obj", "setStyleBorderPost", js_lv_obj_set_style_border_post },
    { "obj", "setStyleOutlineWidth", js_lv_obj_set_style_outline_width },
    { "obj", "setStyleOutlineColor", js_lv_obj_set_style_outline_color },
    { "obj", "setStyleOutlineOpa", js_lv_obj_set_style_outline_opa },
    { "obj", "setStyleOutlinePad", js_lv_obj_set_style_outline_pad },
    { "obj", "setStyleShadowWidth", js_lv_obj_set_style_shadow_width },
    { "obj", "setStyleShadowOffsetX", js_lv_obj_set_style_shadow_offset_x },
    { "obj", "setStyleShadowOffsetY", js_lv_obj_set_style_shadow_offset_y },
    { "obj", "setStyleShadowSpread", js_lv_obj_set_style_shadow_spread },
    { "obj", "setStyleShadowColor", js_lv_obj_set_style_shadow_color },
    { "obj", "setStyleShadowOpa", js_lv_obj_set_style_shadow_opa },
    { "obj", "setStyleImageOpa", js_lv_obj_set_style_image_opa },
    { "obj", "setStyleImageRecolor", js_lv_obj_set_style_image_recolor },
    { "obj", "setStyleImageRecolorOpa", js_lv_obj_set_style_image_recolor_opa },
    { "obj", "setStyleLineWidth", js_lv_obj_set_style_line_width },
    { "obj", "setStyleLineDashWidth", js_lv_obj_set_style_line_dash_width },
    { "obj", "setStyleLineDashGap", js_lv_obj_set_style_line_dash_gap },
    { "obj", "setStyleLineRounded", js_lv_obj_set_style_line_rounded },
    { "obj", "setStyleLineColor", js_lv_obj_set_style_line_color },
    { "obj", "setStyleLineOpa", js_lv_obj_set_style_line_opa },
    { "obj", "setStyleArcWidth", js_lv_obj_set_style_arc_width },
    { "obj", "setStyleArcRounded", js_lv_obj_set_style_arc_rounded },
    { "obj", "setStyleArcColor", js_lv_obj_set_style_arc_color },
    { "obj", "setStyleArcOpa", js_lv_obj_set_style_arc_opa },
    { "obj", "setStyleArcImageSrc", js_lv_obj_set_style_arc_image_src },
    { "obj", "setStyleTextColor", js_lv_obj_set_style_text_color },
    { "obj", "setStyleTextOpa", js_lv_obj_set_style_text_opa },
    { "obj", "setStyleTextFont", js_lv_obj_set_style_text_font },
    { "obj", "setStyleTextLetterSpace", js_lv_obj_set_style_text_letter_space },
    { "obj", "setStyleTextLineSpace", js_lv_obj_set_style_text_line_space },
    { "obj", "setStyleTextDecor", js_lv_obj_set_style_text_decor },
    { "obj", "setStyleTextAlign", js_lv_obj_set_style_text_align },
    { "obj", "setStyleRadius", js_lv_obj_set_style_radius },
    { "obj", "setStyleClipCorner", js_lv_obj_set_style_clip_corner },
    { "obj", "setStyleOpa", js_lv_obj_set_style_opa },
    { "obj", "setStyleOpaLayered", js_lv_obj_set_style_opa_layered },
    { "obj", "setStyleColorFilterDsc", js_lv_obj_set_style_color_filter_dsc },
    { "obj", "setStyleColorFilterOpa", js_lv_obj_set_style_color_filter_opa },
    { "obj", "setStyleAnim", js_lv_obj_set_style_anim },
    { "obj", "setStyleAnimDuration", js_lv_obj_set_style_anim_duration },
    { "obj", "setStyleTransition", js_lv_obj_set_style_transition },
    { "obj", "setStyleBlendMode", js_lv_obj_set_style_blend_mode },
    { "obj", "setStyleLayout", js_lv_obj_set_style_layout },
    { "obj", "setStyleBaseDir", js_lv_obj_set_style_base_dir },
    { "obj", "setStyleBitmapMaskSrc", js_lv_obj_set_style_bitmap_mask_src },
    { "obj", "setStyleRotarySensitivity", js_lv_obj_set_style_rotary_sensitivity },
    { "obj", "setStyleFlexFlow", js_lv_obj_set_style_flex_flow },
    { "obj", "setStyleFlexMainPlace", js_lv_obj_set_style_flex_main_place },
    { "obj", "setStyleFlexCrossPlace", js_lv_obj_set_style_flex_cross_place },
    { "obj", "setStyleFlexTrackPlace", js_lv_obj_set_style_flex_track_place },
    { "obj", "setStyleFlexGrow", js_lv_obj_set_style_flex_grow },
    { "obj", "setStyleGridColumnDscArray", js_lv_obj_set_style_grid_column_dsc_array },
    { "obj", "setStyleGridColumnAlign", js_lv_obj_set_style_grid_column_align },
    { "obj", "setStyleGridRowDscArray", js_lv_obj_set_style_grid_row_dsc_array },
    { "obj", "setStyleGridRowAlign", js_lv_obj_set_style_grid_row_align },
    { "obj", "setStyleGridCellColumnPos", js_lv_obj_set_style_grid_cell_column_pos },
    { "obj", "setStyleGridCellXAlign", js_lv_obj_set_style_grid_cell_x_align },
    { "obj", "setStyleGridCellColumnSpan", js_lv_obj_set_style_grid_cell_column_span },
    { "obj", "setStyleGridCellRowPos", js_lv_obj_set_style_grid_cell_row_pos },
    { "obj", "setStyleGridCellYAlign", js_lv_obj_set_style_grid_cell_y_align },
    { "obj", "setStyleGridCellRowSpan", js_lv_obj_set_style_grid_cell_row_span },
    { "obj", "setStylePadAll", js_lv_obj_set_style_pad_all },
    { "obj", "setStylePadHor", js_lv_obj_set_style_pad_hor },
    { "obj", "setStylePadVer", js_lv_obj_set_style_pad_ver },
    { "obj", "setStyleMarginAll", js_lv_obj_set_style_margin_all },
    { "obj", "setStyleMarginHor", js_lv_obj_set_style_margin_hor },
    { "obj", "setStyleMarginVer", js_lv_obj_set_style_margin_ver },
    { "obj", "setStylePadGap", js_lv_obj_set_style_pad_gap },
    { "obj", "setStyleSize", js_lv_obj_set_style_size },
    { "obj", "setStyleTransformScale", js_lv_obj_set_style_transform_scale },
    { "obj", "getStyleSpaceLeft", js_lv_obj_get_style_space_left },
    { "obj", "getStyleSpaceRight", js_lv_obj_get_style_space_right },
    { "obj", "getStyleSpaceTop", js_lv_obj_get_style_space_top },
    { "obj", "getStyleSpaceBottom", js_lv_obj_get_style_space_bottom },
    { "obj", "calculateStyleTextAlign", js_lv_obj_calculate_style_text_align },
    { "obj", "getStyleTransformScaleXSafe", js_lv_obj_get_style_transform_scale_x_safe },
    { "obj", "getStyleTransformScaleYSafe", js_lv_obj_get_style_transform_scale_y_safe },
    { "obj", "getStyleOpaRecursive", js_lv_obj_get_style_opa_recursive },
    { "image", "srcGetType", js_lv_image_src_get_type },
    { "obj", "initDrawRectDsc", js_lv_obj_init_draw_rect_dsc },
    { "obj", "initDrawLabelDsc", js_lv_obj_init_draw_label_dsc },
    { "obj", "initDrawImageDsc", js_lv_obj_init_draw_image_dsc },
    { "obj", "initDrawLineDsc", js_lv_obj_init_draw_line_dsc },
    { "obj", "initDrawArcDsc", js_lv_obj_init_draw_arc_dsc },
    { "obj", "calculateExtDrawSize", js_lv_obj_calculate_ext_draw_size },
    { "obj", "refreshExtDrawSize", js_lv_obj_refresh_ext_draw_size },
    { "obj", "classCreateObj", js_lv_obj_class_create_obj },
    { "obj", "classInitObj", js_lv_obj_class_init_obj },
    { "obj", "isEditable", js_lv_obj_is_editable },
    { "obj", "isGroupDef", js_lv_obj_is_group_def },
    { "obj", "sendEvent", js_lv_obj_send_event },
    { "obj", "eventBase", js_lv_obj_event_base },
    { "obj", "getEventCount", js_lv_obj_get_event_count },
    { "obj", "getEventDsc", js_lv_obj_get_event_dsc },
    { "obj", "removeEvent", js_lv_obj_remove_event },
    { "obj", "removeEventCb", js_lv_obj_remove_event_cb },
    { "obj", "removeEventDsc", js_lv_obj_remove_event_dsc },
    { "obj", "removeEventCbWithUserData", js_lv_obj_remove_event_cb_with_user_data },
    { "obj", "create", js_lv_obj_create },
    { "obj", "addFlag", js_lv_obj_add_flag },
    { "obj", "removeFlag", js_lv_obj_remove_flag },
    { "obj", "updateFlag", js_lv_obj_update_flag },
    { "obj", "addState", js_lv_obj_add_state },
    { "obj", "removeState", js_lv_obj_remove_state },
    { "obj", "setState", js_lv_obj_set_state },
    { "obj", "setUserData", js_lv_obj_set_user_data },
    { "obj", "hasFlag", js_lv_obj_has_flag },
    { "obj", "hasFlagAny", js_lv_obj_has_flag_any },
    { "obj", "getState", js_lv_obj_get_state },
    { "obj", "hasState", js_lv_obj_has_state },
    { "obj", "getGroup", js_lv_obj_get_group },
    { "obj", "getUserData", js_lv_obj_get_user_data },
    { "obj", "allocateSpecAttr", js_lv_obj_allocate_spec_attr },
    { "obj", "checkType", js_lv_obj_check_type },
    { "obj", "hasClass", js_lv_obj_has_class },
    { "obj", "getClass", js_lv_obj_get_class },
    { "obj", "isValid", js_lv_obj_is_valid },
    { "obj", "nullOnDelete", js_lv_obj_null_on_delete },
    { "obj", "redraw", js_lv_obj_redraw },
    { "image", "create", js_lv_image_create },
    { "image", "setOffsetX", js_lv_image_set_offset_x },
    { "image", "setOffsetY", js_lv_image_set_offset_y },
    { "image", "setRotation", js_lv_image_set_rotation },
    { "image", "setPivot", js_lv_image_set_pivot },
    { "image", "setScale", js_lv_image_set_scale },
    { "image", "setScaleX", js_lv_image_set_scale_x },
    { "image", "setScaleY", js_lv_image_set_scale_y },
    { "image", "setBlendMode", js_lv_image_set_blend_mode },
    { "image", "setAntialias", js_lv_image_set_antialias },
    { "image", "setInnerAlign", js_lv_image_set_inner_align },
    { "image", "setBitmapMapSrc", js_lv_image_set_bitmap_map_src },
    { "image", "getSrc", js_lv_image_get_src },
    { "image", "getOffsetX", js_lv_image_get_offset_x },
    { "image", "getOffsetY", js_lv_image_get_offset_y },
    { "image", "getRotation", js_lv_image_get_rotation },
    { "image", "getPivot", js_lv_image_get_pivot },
    { "image", "getScale", js_lv_image_get_scale },
    { "image", "getScaleX", js_lv_image_get_scale_x },
    { "image", "getScaleY", js_lv_image_get_scale_y },
    { "image", "getBlendMode", js_lv_image_get_blend_mode },
    { "image", "getAntialias", js_lv_image_get_antialias },
    { "image", "getInnerAlign", js_lv_image_get_inner_align },
    { "image", "getBitmapMapSrc", js_lv_image_get_bitmap_map_src },
    { "arc", "create", js_lv_arc_create },
    { "arc", "setStartAngle", js_lv_arc_set_start_angle },
    { "arc", "setEndAngle", js_lv_arc_set_end_angle },
    { "arc", "setAngles", js_lv_arc_set_angles },
    { "arc", "setBgStartAngle", js_lv_arc_set_bg_start_angle },
    { "arc", "setBgEndAngle", js_lv_arc_set_bg_end_angle },
    { "arc", "setBgAngles", js_lv_arc_set_bg_angles },
    { "arc", "setRotation", js_lv_arc_set_rotation },
    { "arc", "setMode", js_lv_arc_set_mode },
    { "arc", "setValue", js_lv_arc_set_value },
    { "arc", "setRange", js_lv_arc_set_range },
    { "arc", "setChangeRate", js_lv_arc_set_change_rate },
    { "arc", "setKnobOffset", js_lv_arc_set_knob_offset },
    { "arc", "getAngleStart", js_lv_arc_get_angle_start },
    { "arc", "getAngleEnd", js_lv_arc_get_angle_end },
    { "arc", "getBgAngleStart", js_lv_arc_get_bg_angle_start },
    { "arc", "getBgAngleEnd", js_lv_arc_get_bg_angle_end },
    { "arc", "getValue", js_lv_arc_get_value },
    { "arc", "getMinValue", js_lv_arc_get_min_value },
    { "arc", "getMaxValue", js_lv_arc_get_max_value },
    { "arc", "getMode", js_lv_arc_get_mode },
    { "arc", "getRotation", js_lv_arc_get_rotation },
    { "arc", "getKnobOffset", js_lv_arc_get_knob_offset },
    { "arc", "alignObjToAngle", js_lv_arc_align_obj_to_angle },
    { "arc", "rotateObjToAngle", js_lv_arc_rotate_obj_to_angle },
    { "label", "create", js_lv_label_create },
    { "label", "setText", js_lv_label_set_text },
    { "label", "setTextStatic", js_lv_label_set_text_static },
    { "label", "setLongMode", js_lv_label_set_long_mode },
    { "label", "setTextSelectionStart", js_lv_label_set_text_selection_start },
    { "label", "setTextSelectionEnd", js_lv_label_set_text_selection_end },
    { "label", "getText", js_lv_label_get_text },
    { "label", "getLongMode", js_lv_label_get_long_mode },
    { "label", "getLetterPos", js_lv_label_get_letter_pos },
    { "label", "getLetterOn", js_lv_label_get_letter_on },
    { "label", "isCharUnderPos", js_lv_label_is_char_under_pos },
    { "label", "getTextSelectionStart", js_lv_label_get_text_selection_start },
    { "label", "getTextSelectionEnd", js_lv_label_get_text_selection_end },
    { "label", "insText", js_lv_label_ins_text },
    { "label", "cutText", js_lv_label_cut_text },
    { "bar", "create", js_lv_bar_create },
    { "bar", "setValue", js_lv_bar_set_value },
    { "bar", "setStartValue", js_lv_bar_set_start_value },
    { "bar", "setRange", js_lv_bar_set_range },
    { "bar", "setMode", js_lv_bar_set_mode },
    { "bar", "setOrientation", js_lv_bar_set_orientation },
    { "bar", "getValue", js_lv_bar_get_value },
    { "bar", "getStartValue", js_lv_bar_get_start_value },
    { "bar", "getMinValue", js_lv_bar_get_min_value },
    { "bar", "getMaxValue", js_lv_bar_get_max_value },
    { "bar", "getMode", js_lv_bar_get_mode },
    { "bar", "getOrientation", js_lv_bar_get_orientation },
    { "bar", "isSymmetrical", js_lv_bar_is_symmetrical },
    { "button", "create", js_lv_button_create },
    { "checkbox", "create", js_lv_checkbox_create },
    { "checkbox", "setText", js_lv_checkbox_set_text },
    { "checkbox", "setTextStatic", js_lv_checkbox_set_text_static },
    { "checkbox", "getText", js_lv_checkbox_get_text },
    { "dropdown", "create", js_lv_dropdown_create },
    { "dropdown", "setText", js_lv_dropdown_set_text },
    { "dropdown", "setOptions", js_lv_dropdown_set_options },
    { "dropdown", "setOptionsStatic", js_lv_dropdown_set_options_static },
    { "dropdown", "addOption", js_lv_dropdown_add_option },
    { "dropdown", "clearOptions", js_lv_dropdown_clear_options },
    { "dropdown", "setSelected", js_lv_dropdown_set_selected },
    { "dropdown", "setDir", js_lv_dropdown_set_dir },
    { "dropdown", "setSymbol", js_lv_dropdown_set_symbol },
    { "dropdown", "setSelectedHighlight", js_lv_dropdown_set_selected_highlight },
    { "dropdown", "getList", js_lv_dropdown_get_list },
    { "dropdown", "getText", js_lv_dropdown_get_text },
    { "dropdown", "getOptions", js_lv_dropdown_get_options },
    { "dropdown", "getSelected", js_lv_dropdown_get_selected },
    { "dropdown", "getOptionCount", js_lv_dropdown_get_option_count },
    { "dropdown", "getSelectedStr", js_lv_dropdown_get_selected_str },
    { "dropdown", "getOptionIndex", js_lv_dropdown_get_option_index },
    { "dropdown", "getSymbol", js_lv_dropdown_get_symbol },
    { "dropdown", "getSelectedHighlight", js_lv_dropdown_get_selected_highlight },
    { "dropdown", "getDir", js_lv_dropdown_get_dir },
    { "dropdown", "open", js_lv_dropdown_open },
    { "dropdown", "close", js_lv_dropdown_close },
    { "dropdown", "isOpen", js_lv_dropdown_is_open },
    { "list", "create", js_lv_list_create },
    { "list", "addText", js_lv_list_add_text },
    { "list", "addButton", js_lv_list_add_button },
    { "list", "getButtonText", js_lv_list_get_button_text },
    { "list", "setButtonText", js_lv_list_set_button_text },
    { "slider", "create", js_lv_slider_create },
    { "slider", "setValue", js_lv_slider_set_value },
    { "slider", "setLeftValue", js_lv_slider_set_left_value },
    { "slider", "setRange", js_lv_slider_set_range },
    { "slider", "setMode", js_lv_slider_set_mode },
    { "slider", "getValue", js_lv_slider_get_value },
    { "slider", "getLeftValue", js_lv_slider_get_left_value },
    { "slider", "getMinValue", js_lv_slider_get_min_value },
    { "slider", "getMaxValue", js_lv_slider_get_max_value },
    { "slider", "isDragged", js_lv_slider_is_dragged },
    { "slider", "getMode", js_lv_slider_get_mode },
    { "slider", "isSymmetrical", js_lv_slider_is_symmetrical },
    { "switch", "create", js_lv_switch_create },
    { "obj", "removeFromSubject", js_lv_obj_remove_from_subject },
    { "obj", "bindFlagIfEq", js_lv_obj_bind_flag_if_eq },
    { "obj", "bindFlagIfNotEq", js_lv_obj_bind_flag_if_not_eq },
    { "obj", "bindStateIfEq", js_lv_obj_bind_state_if_eq },
    { "obj", "bindStateIfNotEq", js_lv_obj_bind_state_if_not_eq },
    { "obj", "bindChecked", js_lv_obj_bind_checked },
    { "label", "bindText", js_lv_label_bind_text },
    { "arc", "bindValue", js_lv_arc_bind_value },
    { "slider", "bindValue", js_lv_slider_bind_value },
    { "dropdown", "bindValue", js_lv_dropdown_bind_value },
    { "obj", "moveForeground", js_lv_obj_move_foreground },
    { "obj", "moveBackground", js_lv_obj_move_background },
    { "obj", "del", js_lv_obj_delete },
    { "obj", "delAsync", js_lv_obj_delete_async },
    { "obj", "clearFlag", js_lv_obj_remove_flag },
    { "obj", "clearState", js_lv_obj_remove_state },
    { "disp", "getScrAct", js_lv_display_get_screen_active },
    { "display", "getScreenActive", js_lv_display_get_screen_active },
    { "timer", "del", js_lv_timer_delete },
    { "list", "setBtnText", js_lv_list_set_button_text },
    { "list", "getBtnText", js_lv_list_get_button_text },
    { "list", "addBtn", js_lv_list_add_button },
    { "obj", "setTile", js_lv_tileview_set_tile },
    { "tileview", "setTile", js_lv_tileview_set_tile },
    { "dropdown", "getOptionCnt", js_lv_dropdown_get_option_count },
    { "obj", "getChildCnt", js_lv_obj_get_child_count },
    { "obj", "getDisp", js_lv_obj_get_display },
    { "obj", "deleteAnimReadyCb", js_lv_obj_delete_anim_completed_cb },
    { "obj", "getStyleAnimTime", js_lv_obj_get_style_anim_duration },
    { "obj", "getStyleImgOpa", js_lv_obj_get_style_image_opa },
    { "obj", "getStyleImgRecolor", js_lv_obj_get_style_image_recolor },
    { "obj", "getStyleImgRecolorFiltered", js_lv_obj_get_style_image_recolor_filtered },
    { "obj", "getStyleImgRecolorOpa", js_lv_obj_get_style_image_recolor_opa },
    { "obj", "getStyleShadowOfsX", js_lv_obj_get_style_shadow_offset_x },
    { "obj", "getStyleShadowOfsY", js_lv_obj_get_style_shadow_offset_y },
    { "obj", "getStyleTransformAngle", js_lv_obj_get_style_transform_rotation },
    { "obj", "getStyleBgImgSrc", js_lv_obj_get_style_bg_image_src },
    { "obj", "getStyleBgImgRecolor", js_lv_obj_get_style_bg_image_recolor },
    { "obj", "getStyleBgImgRecolorOpa", js_lv_obj_get_style_bg_image_recolor_opa },
    { "obj", "setStyleAnimTime", js_lv_obj_set_style_anim_duration },
    { "obj", "setStyleImgOpa", js_lv_obj_set_style_image_opa },
    { "obj", "setStyleImgRecolor", js_lv_obj_set_style_image_recolor },
    { "obj", "setStyleImgRecolorOpa", js_lv_obj_set_style_image_recolor_opa },
    { "obj", "setStyleShadowOfsX", js_lv_obj_set_style_shadow_offset_x },
    { "obj", "setStyleShadowOfsY", js_lv_obj_set_style_shadow_offset_y },
    { "obj", "setStyleTransformZoom", js_lv_obj_set_style_transform_scale },
    { "obj", "setStyleTransformAngle", js_lv_obj_set_style_transform_rotation },
    { "obj", "setStyleBgImgSrc", js_lv_obj_set_style_bg_image_src },
    { "obj", "setStyleBgImgRecolor", js_lv_obj_set_style_bg_image_recolor },
    { "obj", "setStyleBgImgRecolorOpa", js_lv_obj_set_style_bg_image_recolor_opa },
    { "image", "decoderBuiltInOpen", js_lv_bin_decoder_open },
    { "bin", "decoderOpen", js_lv_bin_decoder_open },
    { "image", "decoderBuiltInClose", js_lv_bin_decoder_close },
    { "bin", "decoderClose", js_lv_bin_decoder_close },
    { "image", "setAlign", js_lv_image_set_inner_align },
    { "image", "getAlign", js_lv_image_get_inner_align },
    { "button", "bindChecked", js_lv_obj_bind_checked }
};

const unsigned int lvgl_binding_funcs_count = 619;

static void register_lvgl_enums(jerry_value_t parent) {

    typedef struct { const char* name; int value; } lvgl_enum_entry_t;

    static const lvgl_enum_entry_t enum_entries[] = {
        { "RESULT_INVALID", 0 },
        { "RESULT_OK", 1 },
        { "ANIM_OFF", 0 },
        { "ANIM_ON", 1 },
        { "RB_COLOR_RED", 0 },
        { "RB_COLOR_BLACK", 1 },
        { "ALIGN_DEFAULT", 0 },
        { "ALIGN_TOP_LEFT", 1 },
        { "ALIGN_TOP_MID", 2 },
        { "ALIGN_TOP_RIGHT", 3 },
        { "ALIGN_BOTTOM_LEFT", 4 },
        { "ALIGN_BOTTOM_MID", 5 },
        { "ALIGN_BOTTOM_RIGHT", 6 },
        { "ALIGN_LEFT_MID", 7 },
        { "ALIGN_RIGHT_MID", 8 },
        { "ALIGN_CENTER", 9 },
        { "ALIGN_OUT_TOP_LEFT", 10 },
        { "ALIGN_OUT_TOP_MID", 11 },
        { "ALIGN_OUT_TOP_RIGHT", 12 },
        { "ALIGN_OUT_BOTTOM_LEFT", 13 },
        { "ALIGN_OUT_BOTTOM_MID", 14 },
        { "ALIGN_OUT_BOTTOM_RIGHT", 15 },
        { "ALIGN_OUT_LEFT_TOP", 16 },
        { "ALIGN_OUT_LEFT_MID", 17 },
        { "ALIGN_OUT_LEFT_BOTTOM", 18 },
        { "ALIGN_OUT_RIGHT_TOP", 19 },
        { "ALIGN_OUT_RIGHT_MID", 20 },
        { "ALIGN_OUT_RIGHT_BOTTOM", 21 },
        { "DIR_NONE", 0 },
        { "DIR_LEFT", 1 },
        { "DIR_RIGHT", 2 },
        { "DIR_TOP", 4 },
        { "DIR_BOTTOM", 8 },
        { "DIR_HOR", 3 },
        { "DIR_VER", 12 },
        { "DIR_ALL", 15 },
        { "OPA_TRANSP", 0 },
        { "OPA_0", 0 },
        { "OPA_10", 25 },
        { "OPA_20", 51 },
        { "OPA_30", 76 },
        { "OPA_40", 102 },
        { "OPA_50", 127 },
        { "OPA_60", 153 },
        { "OPA_70", 178 },
        { "OPA_80", 204 },
        { "OPA_90", 229 },
        { "OPA_100", 255 },
        { "OPA_COVER", 255 },
        { "COLOR_FORMAT_UNKNOWN", 0 },
        { "COLOR_FORMAT_RAW", 1 },
        { "COLOR_FORMAT_RAW_ALPHA", 2 },
        { "COLOR_FORMAT_L8", 6 },
        { "COLOR_FORMAT_I1", 7 },
        { "COLOR_FORMAT_I2", 8 },
        { "COLOR_FORMAT_I4", 9 },
        { "COLOR_FORMAT_I8", 10 },
        { "COLOR_FORMAT_A8", 14 },
        { "COLOR_FORMAT_RGB565", 18 },
        { "COLOR_FORMAT_ARGB8565", 19 },
        { "COLOR_FORMAT_RGB565A8", 20 },
        { "COLOR_FORMAT_AL88", 21 },
        { "COLOR_FORMAT_RGB888", 15 },
        { "COLOR_FORMAT_ARGB8888", 16 },
        { "COLOR_FORMAT_XRGB8888", 17 },
        { "COLOR_FORMAT_A1", 11 },
        { "COLOR_FORMAT_A2", 12 },
        { "COLOR_FORMAT_A4", 13 },
        { "COLOR_FORMAT_YUV_START", 32 },
        { "COLOR_FORMAT_I420", 32 },
        { "COLOR_FORMAT_I422", 33 },
        { "COLOR_FORMAT_I444", 34 },
        { "COLOR_FORMAT_I400", 35 },
        { "COLOR_FORMAT_NV21", 36 },
        { "COLOR_FORMAT_NV12", 37 },
        { "COLOR_FORMAT_YUY2", 38 },
        { "COLOR_FORMAT_UYVY", 39 },
        { "COLOR_FORMAT_YUV_END", 39 },
        { "COLOR_FORMAT_NATIVE", 18 },
        { "COLOR_FORMAT_NATIVE_WITH_ALPHA", 20 },
        { "PALETTE_RED", 0 },
        { "PALETTE_PINK", 1 },
        { "PALETTE_PURPLE", 2 },
        { "PALETTE_DEEP_PURPLE", 3 },
        { "PALETTE_INDIGO", 4 },
        { "PALETTE_BLUE", 5 },
        { "PALETTE_LIGHT_BLUE", 6 },
        { "PALETTE_CYAN", 7 },
        { "PALETTE_TEAL", 8 },
        { "PALETTE_GREEN", 9 },
        { "PALETTE_LIGHT_GREEN", 10 },
        { "PALETTE_LIME", 11 },
        { "PALETTE_YELLOW", 12 },
        { "PALETTE_AMBER", 13 },
        { "PALETTE_ORANGE", 14 },
        { "PALETTE_DEEP_ORANGE", 15 },
        { "PALETTE_BROWN", 16 },
        { "PALETTE_BLUE_GREY", 17 },
        { "PALETTE_GREY", 18 },
        { "PALETTE_LAST", 19 },
        { "PALETTE_NONE", 255 },
        { "IMAGE_FLAGS_PREMULTIPLIED", 1 },
        { "IMAGE_FLAGS_COMPRESSED", 8 },
        { "IMAGE_FLAGS_ALLOCATED", 16 },
        { "IMAGE_FLAGS_MODIFIABLE", 32 },
        { "IMAGE_FLAGS_USER1", 256 },
        { "IMAGE_FLAGS_USER2", 512 },
        { "IMAGE_FLAGS_USER3", 1024 },
        { "IMAGE_FLAGS_USER4", 2048 },
        { "IMAGE_FLAGS_USER5", 4096 },
        { "IMAGE_FLAGS_USER6", 8192 },
        { "IMAGE_FLAGS_USER7", 16384 },
        { "IMAGE_FLAGS_USER8", 32768 },
        { "IMAGE_COMPRESS_NONE", 0 },
        { "IMAGE_COMPRESS_RLE", 1 },
        { "IMAGE_COMPRESS_LZ4", 2 },
        { "STR_SYMBOL_BULLET", 0 },
        { "STR_SYMBOL_AUDIO", 1 },
        { "STR_SYMBOL_VIDEO", 2 },
        { "STR_SYMBOL_LIST", 3 },
        { "STR_SYMBOL_OK", 4 },
        { "STR_SYMBOL_CLOSE", 5 },
        { "STR_SYMBOL_POWER", 6 },
        { "STR_SYMBOL_SETTINGS", 7 },
        { "STR_SYMBOL_HOME", 8 },
        { "STR_SYMBOL_DOWNLOAD", 9 },
        { "STR_SYMBOL_DRIVE", 10 },
        { "STR_SYMBOL_REFRESH", 11 },
        { "STR_SYMBOL_MUTE", 12 },
        { "STR_SYMBOL_VOLUME_MID", 13 },
        { "STR_SYMBOL_VOLUME_MAX", 14 },
        { "STR_SYMBOL_IMAGE", 15 },
        { "STR_SYMBOL_TINT", 16 },
        { "STR_SYMBOL_PREV", 17 },
        { "STR_SYMBOL_PLAY", 18 },
        { "STR_SYMBOL_PAUSE", 19 },
        { "STR_SYMBOL_STOP", 20 },
        { "STR_SYMBOL_NEXT", 21 },
        { "STR_SYMBOL_EJECT", 22 },
        { "STR_SYMBOL_LEFT", 23 },
        { "STR_SYMBOL_RIGHT", 24 },
        { "STR_SYMBOL_PLUS", 25 },
        { "STR_SYMBOL_MINUS", 26 },
        { "STR_SYMBOL_EYE_OPEN", 27 },
        { "STR_SYMBOL_EYE_CLOSE", 28 },
        { "STR_SYMBOL_WARNING", 29 },
        { "STR_SYMBOL_SHUFFLE", 30 },
        { "STR_SYMBOL_UP", 31 },
        { "STR_SYMBOL_DOWN", 32 },
        { "STR_SYMBOL_LOOP", 33 },
        { "STR_SYMBOL_DIRECTORY", 34 },
        { "STR_SYMBOL_UPLOAD", 35 },
        { "STR_SYMBOL_CALL", 36 },
        { "STR_SYMBOL_CUT", 37 },
        { "STR_SYMBOL_COPY", 38 },
        { "STR_SYMBOL_SAVE", 39 },
        { "STR_SYMBOL_BARS", 40 },
        { "STR_SYMBOL_ENVELOPE", 41 },
        { "STR_SYMBOL_CHARGE", 42 },
        { "STR_SYMBOL_PASTE", 43 },
        { "STR_SYMBOL_BELL", 44 },
        { "STR_SYMBOL_KEYBOARD", 45 },
        { "STR_SYMBOL_GPS", 46 },
        { "STR_SYMBOL_FILE", 47 },
        { "STR_SYMBOL_WIFI", 48 },
        { "STR_SYMBOL_BATTERY_FULL", 49 },
        { "STR_SYMBOL_BATTERY_3", 50 },
        { "STR_SYMBOL_BATTERY_2", 51 },
        { "STR_SYMBOL_BATTERY_1", 52 },
        { "STR_SYMBOL_BATTERY_EMPTY", 53 },
        { "STR_SYMBOL_USB", 54 },
        { "STR_SYMBOL_BLUETOOTH", 55 },
        { "STR_SYMBOL_TRASH", 56 },
        { "STR_SYMBOL_EDIT", 57 },
        { "STR_SYMBOL_BACKSPACE", 58 },
        { "STR_SYMBOL_SD_CARD", 59 },
        { "STR_SYMBOL_NEW_LINE", 60 },
        { "STR_SYMBOL_DUMMY", 61 },
        { "THREAD_PRIO_LOWEST", 0 },
        { "THREAD_PRIO_LOW", 1 },
        { "THREAD_PRIO_MID", 2 },
        { "THREAD_PRIO_HIGH", 3 },
        { "THREAD_PRIO_HIGHEST", 4 },
        { "CACHE_RESERVE_COND_OK", 0 },
        { "CACHE_RESERVE_COND_TOO_LARGE", 1 },
        { "CACHE_RESERVE_COND_NEED_VICTIM", 2 },
        { "CACHE_RESERVE_COND_ERROR", 3 },
        { "FONT_GLYPH_FORMAT_NONE", 0 },
        { "FONT_GLYPH_FORMAT_A1", 1 },
        { "FONT_GLYPH_FORMAT_A2", 2 },
        { "FONT_GLYPH_FORMAT_A4", 4 },
        { "FONT_GLYPH_FORMAT_A8", 8 },
        { "FONT_GLYPH_FORMAT_IMAGE", 9 },
        { "FONT_GLYPH_FORMAT_VECTOR", 10 },
        { "FONT_GLYPH_FORMAT_SVG", 11 },
        { "FONT_GLYPH_FORMAT_CUSTOM", 255 },
        { "FONT_SUBPX_NONE", 0 },
        { "FONT_SUBPX_HOR", 1 },
        { "FONT_SUBPX_VER", 2 },
        { "FONT_SUBPX_BOTH", 3 },
        { "FONT_KERNING_NORMAL", 0 },
        { "FONT_KERNING_NONE", 1 },
        { "TEXT_FLAG_NONE", 0 },
        { "TEXT_FLAG_EXPAND", 1 },
        { "TEXT_FLAG_FIT", 2 },
        { "TEXT_FLAG_BREAK_ALL", 4 },
        { "TEXT_ALIGN_AUTO", 0 },
        { "TEXT_ALIGN_LEFT", 1 },
        { "TEXT_ALIGN_CENTER", 2 },
        { "TEXT_ALIGN_RIGHT", 3 },
        { "BASE_DIR_LTR", 0 },
        { "BASE_DIR_RTL", 1 },
        { "BASE_DIR_AUTO", 2 },
        { "BASE_DIR_NEUTRAL", 32 },
        { "BASE_DIR_WEAK", 33 },
        { "LAYOUT_NONE", 0 },
        { "LAYOUT_FLEX", 1 },
        { "LAYOUT_GRID", 2 },
        { "LAYOUT_LAST", 3 },
        { "FLEX_ALIGN_START", 0 },
        { "FLEX_ALIGN_END", 1 },
        { "FLEX_ALIGN_CENTER", 2 },
        { "FLEX_ALIGN_SPACE_EVENLY", 3 },
        { "FLEX_ALIGN_SPACE_AROUND", 4 },
        { "FLEX_ALIGN_SPACE_BETWEEN", 5 },
        { "FLEX_FLOW_ROW", 0 },
        { "FLEX_FLOW_COLUMN", 1 },
        { "FLEX_FLOW_ROW_WRAP", 4 },
        { "FLEX_FLOW_ROW_REVERSE", 8 },
        { "FLEX_FLOW_ROW_WRAP_REVERSE", 12 },
        { "FLEX_FLOW_COLUMN_WRAP", 5 },
        { "FLEX_FLOW_COLUMN_REVERSE", 9 },
        { "FLEX_FLOW_COLUMN_WRAP_REVERSE", 13 },
        { "GRID_ALIGN_START", 0 },
        { "GRID_ALIGN_CENTER", 1 },
        { "GRID_ALIGN_END", 2 },
        { "GRID_ALIGN_STRETCH", 3 },
        { "GRID_ALIGN_SPACE_EVENLY", 4 },
        { "GRID_ALIGN_SPACE_AROUND", 5 },
        { "GRID_ALIGN_SPACE_BETWEEN", 6 },
        { "BLEND_MODE_NORMAL", 0 },
        { "BLEND_MODE_ADDITIVE", 1 },
        { "BLEND_MODE_SUBTRACTIVE", 2 },
        { "BLEND_MODE_MULTIPLY", 3 },
        { "TEXT_DECOR_NONE", 0 },
        { "TEXT_DECOR_UNDERLINE", 1 },
        { "TEXT_DECOR_STRIKETHROUGH", 2 },
        { "BORDER_SIDE_NONE", 0 },
        { "BORDER_SIDE_BOTTOM", 1 },
        { "BORDER_SIDE_TOP", 2 },
        { "BORDER_SIDE_LEFT", 4 },
        { "BORDER_SIDE_RIGHT", 8 },
        { "BORDER_SIDE_FULL", 15 },
        { "BORDER_SIDE_INTERNAL", 16 },
        { "GRAD_DIR_NONE", 0 },
        { "GRAD_DIR_VER", 1 },
        { "GRAD_DIR_HOR", 2 },
        { "GRAD_DIR_LINEAR", 3 },
        { "GRAD_DIR_RADIAL", 4 },
        { "GRAD_DIR_CONICAL", 5 },
        { "GRAD_EXTEND_PAD", 0 },
        { "GRAD_EXTEND_REPEAT", 1 },
        { "GRAD_EXTEND_REFLECT", 2 },
        { "STYLE_PROP_INV", 0 },
        { "STYLE_WIDTH", 1 },
        { "STYLE_HEIGHT", 2 },
        { "STYLE_LENGTH", 3 },
        { "STYLE_MIN_WIDTH", 4 },
        { "STYLE_MAX_WIDTH", 5 },
        { "STYLE_MIN_HEIGHT", 6 },
        { "STYLE_MAX_HEIGHT", 7 },
        { "STYLE_X", 8 },
        { "STYLE_Y", 9 },
        { "STYLE_ALIGN", 10 },
        { "STYLE_RADIUS", 12 },
        { "STYLE_PAD_TOP", 16 },
        { "STYLE_PAD_BOTTOM", 17 },
        { "STYLE_PAD_LEFT", 18 },
        { "STYLE_PAD_RIGHT", 19 },
        { "STYLE_PAD_ROW", 20 },
        { "STYLE_PAD_COLUMN", 21 },
        { "STYLE_LAYOUT", 22 },
        { "STYLE_MARGIN_TOP", 24 },
        { "STYLE_MARGIN_BOTTOM", 25 },
        { "STYLE_MARGIN_LEFT", 26 },
        { "STYLE_MARGIN_RIGHT", 27 },
        { "STYLE_BG_COLOR", 28 },
        { "STYLE_BG_OPA", 29 },
        { "STYLE_BG_GRAD_DIR", 32 },
        { "STYLE_BG_MAIN_STOP", 33 },
        { "STYLE_BG_GRAD_STOP", 34 },
        { "STYLE_BG_GRAD_COLOR", 35 },
        { "STYLE_BG_MAIN_OPA", 36 },
        { "STYLE_BG_GRAD_OPA", 37 },
        { "STYLE_BG_GRAD", 38 },
        { "STYLE_BASE_DIR", 39 },
        { "STYLE_BG_IMAGE_SRC", 40 },
        { "STYLE_BG_IMAGE_OPA", 41 },
        { "STYLE_BG_IMAGE_RECOLOR", 42 },
        { "STYLE_BG_IMAGE_RECOLOR_OPA", 43 },
        { "STYLE_BG_IMAGE_TILED", 44 },
        { "STYLE_CLIP_CORNER", 45 },
        { "STYLE_BORDER_WIDTH", 48 },
        { "STYLE_BORDER_COLOR", 49 },
        { "STYLE_BORDER_OPA", 50 },
        { "STYLE_BORDER_SIDE", 52 },
        { "STYLE_BORDER_POST", 53 },
        { "STYLE_OUTLINE_WIDTH", 56 },
        { "STYLE_OUTLINE_COLOR", 57 },
        { "STYLE_OUTLINE_OPA", 58 },
        { "STYLE_OUTLINE_PAD", 59 },
        { "STYLE_SHADOW_WIDTH", 60 },
        { "STYLE_SHADOW_COLOR", 61 },
        { "STYLE_SHADOW_OPA", 62 },
        { "STYLE_SHADOW_OFFSET_X", 64 },
        { "STYLE_SHADOW_OFFSET_Y", 65 },
        { "STYLE_SHADOW_SPREAD", 66 },
        { "STYLE_IMAGE_OPA", 68 },
        { "STYLE_IMAGE_RECOLOR", 69 },
        { "STYLE_IMAGE_RECOLOR_OPA", 70 },
        { "STYLE_LINE_WIDTH", 72 },
        { "STYLE_LINE_DASH_WIDTH", 73 },
        { "STYLE_LINE_DASH_GAP", 74 },
        { "STYLE_LINE_ROUNDED", 75 },
        { "STYLE_LINE_COLOR", 76 },
        { "STYLE_LINE_OPA", 77 },
        { "STYLE_ARC_WIDTH", 80 },
        { "STYLE_ARC_ROUNDED", 81 },
        { "STYLE_ARC_COLOR", 82 },
        { "STYLE_ARC_OPA", 83 },
        { "STYLE_ARC_IMAGE_SRC", 84 },
        { "STYLE_TEXT_COLOR", 88 },
        { "STYLE_TEXT_OPA", 89 },
        { "STYLE_TEXT_FONT", 90 },
        { "STYLE_TEXT_LETTER_SPACE", 91 },
        { "STYLE_TEXT_LINE_SPACE", 92 },
        { "STYLE_TEXT_DECOR", 93 },
        { "STYLE_TEXT_ALIGN", 94 },
        { "STYLE_OPA", 95 },
        { "STYLE_OPA_LAYERED", 96 },
        { "STYLE_COLOR_FILTER_DSC", 97 },
        { "STYLE_COLOR_FILTER_OPA", 98 },
        { "STYLE_ANIM", 99 },
        { "STYLE_ANIM_DURATION", 100 },
        { "STYLE_TRANSITION", 102 },
        { "STYLE_BLEND_MODE", 103 },
        { "STYLE_TRANSFORM_WIDTH", 104 },
        { "STYLE_TRANSFORM_HEIGHT", 105 },
        { "STYLE_TRANSLATE_X", 106 },
        { "STYLE_TRANSLATE_Y", 107 },
        { "STYLE_TRANSFORM_SCALE_X", 108 },
        { "STYLE_TRANSFORM_SCALE_Y", 109 },
        { "STYLE_TRANSFORM_ROTATION", 110 },
        { "STYLE_TRANSFORM_PIVOT_X", 111 },
        { "STYLE_TRANSFORM_PIVOT_Y", 112 },
        { "STYLE_TRANSFORM_SKEW_X", 113 },
        { "STYLE_TRANSFORM_SKEW_Y", 114 },
        { "STYLE_BITMAP_MASK_SRC", 115 },
        { "STYLE_ROTARY_SENSITIVITY", 116 },
        { "STYLE_FLEX_FLOW", 125 },
        { "STYLE_FLEX_MAIN_PLACE", 126 },
        { "STYLE_FLEX_CROSS_PLACE", 127 },
        { "STYLE_FLEX_TRACK_PLACE", 128 },
        { "STYLE_FLEX_GROW", 129 },
        { "STYLE_GRID_COLUMN_ALIGN", 130 },
        { "STYLE_GRID_ROW_ALIGN", 131 },
        { "STYLE_GRID_ROW_DSC_ARRAY", 132 },
        { "STYLE_GRID_COLUMN_DSC_ARRAY", 133 },
        { "STYLE_GRID_CELL_COLUMN_POS", 134 },
        { "STYLE_GRID_CELL_COLUMN_SPAN", 135 },
        { "STYLE_GRID_CELL_X_ALIGN", 136 },
        { "STYLE_GRID_CELL_ROW_POS", 137 },
        { "STYLE_GRID_CELL_ROW_SPAN", 138 },
        { "STYLE_GRID_CELL_Y_ALIGN", 139 },
        { "STYLE_LAST_BUILT_IN_PROP", 140 },
        { "STYLE_NUM_BUILT_IN_PROPS", 141 },
        { "STYLE_PROP_ANY", 255 },
        { "STYLE_PROP_CONST", 255 },
        { "STYLE_RES_NOT_FOUND", 0 },
        { "STYLE_RES_FOUND", 1 },
        { "EVENT_ALL", 0 },
        { "EVENT_PRESSED", 1 },
        { "EVENT_PRESSING", 2 },
        { "EVENT_PRESS_LOST", 3 },
        { "EVENT_SHORT_CLICKED", 4 },
        { "EVENT_LONG_PRESSED", 5 },
        { "EVENT_LONG_PRESSED_REPEAT", 6 },
        { "EVENT_CLICKED", 7 },
        { "EVENT_RELEASED", 8 },
        { "EVENT_SCROLL_BEGIN", 9 },
        { "EVENT_SCROLL_THROW_BEGIN", 10 },
        { "EVENT_SCROLL_END", 11 },
        { "EVENT_SCROLL", 12 },
        { "EVENT_GESTURE", 13 },
        { "EVENT_KEY", 14 },
        { "EVENT_ROTARY", 15 },
        { "EVENT_FOCUSED", 16 },
        { "EVENT_DEFOCUSED", 17 },
        { "EVENT_LEAVE", 18 },
        { "EVENT_HIT_TEST", 19 },
        { "EVENT_INDEV_RESET", 20 },
        { "EVENT_HOVER_OVER", 21 },
        { "EVENT_HOVER_LEAVE", 22 },
        { "EVENT_COVER_CHECK", 23 },
        { "EVENT_REFR_EXT_DRAW_SIZE", 24 },
        { "EVENT_DRAW_MAIN_BEGIN", 25 },
        { "EVENT_DRAW_MAIN", 26 },
        { "EVENT_DRAW_MAIN_END", 27 },
        { "EVENT_DRAW_POST_BEGIN", 28 },
        { "EVENT_DRAW_POST", 29 },
        { "EVENT_DRAW_POST_END", 30 },
        { "EVENT_DRAW_TASK_ADDED", 31 },
        { "EVENT_VALUE_CHANGED", 32 },
        { "EVENT_INSERT", 33 },
        { "EVENT_REFRESH", 34 },
        { "EVENT_READY", 35 },
        { "EVENT_CANCEL", 36 },
        { "EVENT_CREATE", 37 },
        { "EVENT_DELETE", 38 },
        { "EVENT_CHILD_CHANGED", 39 },
        { "EVENT_CHILD_CREATED", 40 },
        { "EVENT_CHILD_DELETED", 41 },
        { "EVENT_SCREEN_UNLOAD_START", 42 },
        { "EVENT_SCREEN_LOAD_START", 43 },
        { "EVENT_SCREEN_LOADED", 44 },
        { "EVENT_SCREEN_UNLOADED", 45 },
        { "EVENT_SIZE_CHANGED", 46 },
        { "EVENT_STYLE_CHANGED", 47 },
        { "EVENT_LAYOUT_CHANGED", 48 },
        { "EVENT_GET_SELF_SIZE", 49 },
        { "EVENT_INVALIDATE_AREA", 50 },
        { "EVENT_RESOLUTION_CHANGED", 51 },
        { "EVENT_COLOR_FORMAT_CHANGED", 52 },
        { "EVENT_REFR_REQUEST", 53 },
        { "EVENT_REFR_START", 54 },
        { "EVENT_REFR_READY", 55 },
        { "EVENT_RENDER_START", 56 },
        { "EVENT_RENDER_READY", 57 },
        { "EVENT_FLUSH_START", 58 },
        { "EVENT_FLUSH_FINISH", 59 },
        { "EVENT_FLUSH_WAIT_START", 60 },
        { "EVENT_FLUSH_WAIT_FINISH", 61 },
        { "EVENT_VSYNC", 62 },
        { "EVENT_LAST", 63 },
        { "EVENT_PREPROCESS", 32768 },
        { "FS_RES_OK", 0 },
        { "FS_RES_HW_ERR", 1 },
        { "FS_RES_FS_ERR", 2 },
        { "FS_RES_NOT_EX", 3 },
        { "FS_RES_FULL", 4 },
        { "FS_RES_LOCKED", 5 },
        { "FS_RES_DENIED", 6 },
        { "FS_RES_BUSY", 7 },
        { "FS_RES_TOUT", 8 },
        { "FS_RES_NOT_IMP", 9 },
        { "FS_RES_OUT_OF_MEM", 10 },
        { "FS_RES_INV_PARAM", 11 },
        { "FS_RES_UNKNOWN", 12 },
        { "FS_MODE_WR", 1 },
        { "FS_MODE_RD", 2 },
        { "FS_SEEK_SET", 0 },
        { "FS_SEEK_CUR", 1 },
        { "FS_SEEK_END", 2 },
        { "IMAGE_SRC_VARIABLE", 0 },
        { "IMAGE_SRC_FILE", 1 },
        { "IMAGE_SRC_SYMBOL", 2 },
        { "IMAGE_SRC_UNKNOWN", 3 },
        { "DRAW_TASK_TYPE_NONE", 0 },
        { "DRAW_TASK_TYPE_FILL", 1 },
        { "DRAW_TASK_TYPE_BORDER", 2 },
        { "DRAW_TASK_TYPE_BOX_SHADOW", 3 },
        { "DRAW_TASK_TYPE_LABEL", 4 },
        { "DRAW_TASK_TYPE_IMAGE", 5 },
        { "DRAW_TASK_TYPE_LAYER", 6 },
        { "DRAW_TASK_TYPE_LINE", 7 },
        { "DRAW_TASK_TYPE_ARC", 8 },
        { "DRAW_TASK_TYPE_TRIANGLE", 9 },
        { "DRAW_TASK_TYPE_MASK_RECTANGLE", 10 },
        { "DRAW_TASK_TYPE_MASK_BITMAP", 11 },
        { "DRAW_TASK_TYPE_VECTOR", 12 },
        { "DRAW_TASK_STATE_WAITING", 0 },
        { "DRAW_TASK_STATE_QUEUED", 1 },
        { "DRAW_TASK_STATE_IN_PROGRESS", 2 },
        { "DRAW_TASK_STATE_READY", 3 },
        { "DISPLAY_ROTATION_0", 0 },
        { "DISPLAY_ROTATION_90", 1 },
        { "DISPLAY_ROTATION_180", 2 },
        { "DISPLAY_ROTATION_270", 3 },
        { "DISPLAY_RENDER_MODE_PARTIAL", 0 },
        { "DISPLAY_RENDER_MODE_DIRECT", 1 },
        { "DISPLAY_RENDER_MODE_FULL", 2 },
        { "SCR_LOAD_ANIM_NONE", 0 },
        { "SCR_LOAD_ANIM_OVER_LEFT", 1 },
        { "SCR_LOAD_ANIM_OVER_RIGHT", 2 },
        { "SCR_LOAD_ANIM_OVER_TOP", 3 },
        { "SCR_LOAD_ANIM_OVER_BOTTOM", 4 },
        { "SCR_LOAD_ANIM_MOVE_LEFT", 5 },
        { "SCR_LOAD_ANIM_MOVE_RIGHT", 6 },
        { "SCR_LOAD_ANIM_MOVE_TOP", 7 },
        { "SCR_LOAD_ANIM_MOVE_BOTTOM", 8 },
        { "SCR_LOAD_ANIM_FADE_IN", 9 },
        { "SCR_LOAD_ANIM_FADE_ON", 9 },
        { "SCR_LOAD_ANIM_FADE_OUT", 10 },
        { "SCR_LOAD_ANIM_OUT_LEFT", 11 },
        { "SCR_LOAD_ANIM_OUT_RIGHT", 12 },
        { "SCR_LOAD_ANIM_OUT_TOP", 13 },
        { "SCR_LOAD_ANIM_OUT_BOTTOM", 14 },
        { "OBJ_TREE_WALK_NEXT", 0 },
        { "OBJ_TREE_WALK_SKIP_CHILDREN", 1 },
        { "OBJ_TREE_WALK_END", 2 },
        { "OBJ_POINT_TRANSFORM_FLAG_NONE", 0 },
        { "OBJ_POINT_TRANSFORM_FLAG_RECURSIVE", 1 },
        { "OBJ_POINT_TRANSFORM_FLAG_INVERSE", 2 },
        { "OBJ_POINT_TRANSFORM_FLAG_INVERSE_RECURSIVE", 3 },
        { "SCROLLBAR_MODE_OFF", 0 },
        { "SCROLLBAR_MODE_ON", 1 },
        { "SCROLLBAR_MODE_ACTIVE", 2 },
        { "SCROLLBAR_MODE_AUTO", 3 },
        { "SCROLL_SNAP_NONE", 0 },
        { "SCROLL_SNAP_START", 1 },
        { "SCROLL_SNAP_END", 2 },
        { "SCROLL_SNAP_CENTER", 3 },
        { "STYLE_STATE_CMP_SAME", 0 },
        { "STYLE_STATE_CMP_DIFF_REDRAW", 1 },
        { "STYLE_STATE_CMP_DIFF_DRAW_PAD", 2 },
        { "STYLE_STATE_CMP_DIFF_LAYOUT", 3 },
        { "LAYER_TYPE_NONE", 0 },
        { "LAYER_TYPE_SIMPLE", 1 },
        { "LAYER_TYPE_TRANSFORM", 2 },
        { "OBJ_CLASS_EDITABLE_INHERIT", 0 },
        { "OBJ_CLASS_EDITABLE_TRUE", 1 },
        { "OBJ_CLASS_EDITABLE_FALSE", 2 },
        { "OBJ_CLASS_GROUP_DEF_INHERIT", 0 },
        { "OBJ_CLASS_GROUP_DEF_TRUE", 1 },
        { "OBJ_CLASS_GROUP_DEF_FALSE", 2 },
        { "OBJ_CLASS_THEME_INHERITABLE_FALSE", 0 },
        { "OBJ_CLASS_THEME_INHERITABLE_TRUE", 1 },
        { "KEY_UP", 17 },
        { "KEY_DOWN", 18 },
        { "KEY_RIGHT", 19 },
        { "KEY_LEFT", 20 },
        { "KEY_ESC", 27 },
        { "KEY_DEL", 127 },
        { "KEY_BACKSPACE", 8 },
        { "KEY_ENTER", 10 },
        { "KEY_NEXT", 9 },
        { "KEY_PREV", 11 },
        { "KEY_HOME", 2 },
        { "KEY_END", 3 },
        { "GROUP_REFOCUS_POLICY_NEXT", 0 },
        { "GROUP_REFOCUS_POLICY_PREV", 1 },
        { "INDEV_TYPE_NONE", 0 },
        { "INDEV_TYPE_POINTER", 1 },
        { "INDEV_TYPE_KEYPAD", 2 },
        { "INDEV_TYPE_BUTTON", 3 },
        { "INDEV_TYPE_ENCODER", 4 },
        { "INDEV_STATE_RELEASED", 0 },
        { "INDEV_STATE_PRESSED", 1 },
        { "INDEV_MODE_NONE", 0 },
        { "INDEV_MODE_TIMER", 1 },
        { "INDEV_MODE_EVENT", 2 },
        { "COVER_RES_COVER", 0 },
        { "COVER_RES_NOT_COVER", 1 },
        { "COVER_RES_MASKED", 2 },
        { "STATE_DEFAULT", 0 },
        { "STATE_CHECKED", 1 },
        { "STATE_FOCUSED", 2 },
        { "STATE_FOCUS_KEY", 4 },
        { "STATE_EDITED", 8 },
        { "STATE_HOVERED", 16 },
        { "STATE_PRESSED", 32 },
        { "STATE_SCROLLED", 64 },
        { "STATE_DISABLED", 128 },
        { "STATE_USER_1", 4096 },
        { "STATE_USER_2", 8192 },
        { "STATE_USER_3", 16384 },
        { "STATE_USER_4", 32768 },
        { "STATE_ANY", 65535 },
        { "PART_MAIN", 0 },
        { "PART_SCROLLBAR", 65536 },
        { "PART_INDICATOR", 131072 },
        { "PART_KNOB", 196608 },
        { "PART_SELECTED", 262144 },
        { "PART_ITEMS", 327680 },
        { "PART_CURSOR", 393216 },
        { "PART_CUSTOM_FIRST", 524288 },
        { "PART_ANY", 983040 },
        { "OBJ_FLAG_HIDDEN", 1 },
        { "OBJ_FLAG_CLICKABLE", 2 },
        { "OBJ_FLAG_CLICK_FOCUSABLE", 4 },
        { "OBJ_FLAG_CHECKABLE", 8 },
        { "OBJ_FLAG_SCROLLABLE", 16 },
        { "OBJ_FLAG_SCROLL_ELASTIC", 32 },
        { "OBJ_FLAG_SCROLL_MOMENTUM", 64 },
        { "OBJ_FLAG_SCROLL_ONE", 128 },
        { "OBJ_FLAG_SCROLL_CHAIN_HOR", 256 },
        { "OBJ_FLAG_SCROLL_CHAIN_VER", 512 },
        { "OBJ_FLAG_SCROLL_CHAIN", 768 },
        { "OBJ_FLAG_SCROLL_ON_FOCUS", 1024 },
        { "OBJ_FLAG_SCROLL_WITH_ARROW", 2048 },
        { "OBJ_FLAG_SNAPPABLE", 4096 },
        { "OBJ_FLAG_PRESS_LOCK", 8192 },
        { "OBJ_FLAG_EVENT_BUBBLE", 16384 },
        { "OBJ_FLAG_GESTURE_BUBBLE", 32768 },
        { "OBJ_FLAG_ADV_HITTEST", 65536 },
        { "OBJ_FLAG_IGNORE_LAYOUT", 131072 },
        { "OBJ_FLAG_FLOATING", 262144 },
        { "OBJ_FLAG_SEND_DRAW_TASK_EVENTS", 524288 },
        { "OBJ_FLAG_OVERFLOW_VISIBLE", 1048576 },
        { "OBJ_FLAG_FLEX_IN_NEW_TRACK", 2097152 },
        { "OBJ_FLAG_LAYOUT_1", 8388608 },
        { "OBJ_FLAG_LAYOUT_2", 16777216 },
        { "OBJ_FLAG_WIDGET_1", 33554432 },
        { "OBJ_FLAG_WIDGET_2", 67108864 },
        { "OBJ_FLAG_USER_1", 134217728 },
        { "OBJ_FLAG_USER_2", 268435456 },
        { "OBJ_FLAG_USER_3", 536870912 },
        { "OBJ_FLAG_USER_4", 1073741824 },
        { "FONT_FMT_TXT_CMAP_FORMAT0_FULL", 0 },
        { "FONT_FMT_TXT_CMAP_SPARSE_FULL", 1 },
        { "FONT_FMT_TXT_CMAP_FORMAT0_TINY", 2 },
        { "FONT_FMT_TXT_CMAP_SPARSE_TINY", 3 },
        { "FONT_FMT_TXT_PLAIN", 0 },
        { "FONT_FMT_TXT_COMPRESSED", 1 },
        { "FONT_FMT_TXT_COMPRESSED_NO_PREFILTER", 1 },
        { "IMAGE_ALIGN_DEFAULT", 0 },
        { "IMAGE_ALIGN_TOP_LEFT", 1 },
        { "IMAGE_ALIGN_TOP_MID", 2 },
        { "IMAGE_ALIGN_TOP_RIGHT", 3 },
        { "IMAGE_ALIGN_BOTTOM_LEFT", 4 },
        { "IMAGE_ALIGN_BOTTOM_MID", 5 },
        { "IMAGE_ALIGN_BOTTOM_RIGHT", 6 },
        { "IMAGE_ALIGN_LEFT_MID", 7 },
        { "IMAGE_ALIGN_RIGHT_MID", 8 },
        { "IMAGE_ALIGN_CENTER", 9 },
        { "IMAGE_ALIGN_AUTO_TRANSFORM", 10 },
        { "IMAGE_ALIGN_STRETCH", 11 },
        { "IMAGE_ALIGN_TILE", 12 },
        { "ANIM_IMAGE_PART_MAIN", 0 },
        { "ARC_MODE_NORMAL", 0 },
        { "ARC_MODE_SYMMETRICAL", 1 },
        { "ARC_MODE_REVERSE", 2 },
        { "LABEL_LONG_WRAP", 0 },
        { "LABEL_LONG_DOT", 1 },
        { "LABEL_LONG_SCROLL", 2 },
        { "LABEL_LONG_SCROLL_CIRCULAR", 3 },
        { "LABEL_LONG_CLIP", 4 },
        { "BAR_MODE_NORMAL", 0 },
        { "BAR_MODE_SYMMETRICAL", 1 },
        { "BAR_MODE_RANGE", 2 },
        { "BAR_ORIENTATION_AUTO", 0 },
        { "BAR_ORIENTATION_HORIZONTAL", 1 },
        { "BAR_ORIENTATION_VERTICAL", 2 },
        { "BUTTONMATRIX_CTRL_HIDDEN", 16 },
        { "BUTTONMATRIX_CTRL_NO_REPEAT", 32 },
        { "BUTTONMATRIX_CTRL_DISABLED", 64 },
        { "BUTTONMATRIX_CTRL_CHECKABLE", 128 },
        { "BUTTONMATRIX_CTRL_CHECKED", 256 },
        { "BUTTONMATRIX_CTRL_CLICK_TRIG", 512 },
        { "BUTTONMATRIX_CTRL_POPOVER", 1024 },
        { "BUTTONMATRIX_CTRL_RESERVED_1", 2048 },
        { "BUTTONMATRIX_CTRL_RESERVED_2", 4096 },
        { "BUTTONMATRIX_CTRL_RESERVED_3", 8192 },
        { "BUTTONMATRIX_CTRL_CUSTOM_1", 16384 },
        { "BUTTONMATRIX_CTRL_CUSTOM_2", 32768 },
        { "CHART_TYPE_NONE", 0 },
        { "CHART_TYPE_LINE", 1 },
        { "CHART_TYPE_BAR", 2 },
        { "CHART_TYPE_SCATTER", 3 },
        { "CHART_UPDATE_MODE_SHIFT", 0 },
        { "CHART_UPDATE_MODE_CIRCULAR", 1 },
        { "CHART_AXIS_PRIMARY_Y", 0 },
        { "CHART_AXIS_SECONDARY_Y", 1 },
        { "CHART_AXIS_PRIMARY_X", 2 },
        { "CHART_AXIS_SECONDARY_X", 4 },
        { "CHART_AXIS_LAST", 5 },
        { "IMAGEBUTTON_STATE_RELEASED", 0 },
        { "IMAGEBUTTON_STATE_PRESSED", 1 },
        { "IMAGEBUTTON_STATE_DISABLED", 2 },
        { "IMAGEBUTTON_STATE_CHECKED_RELEASED", 3 },
        { "IMAGEBUTTON_STATE_CHECKED_PRESSED", 4 },
        { "IMAGEBUTTON_STATE_CHECKED_DISABLED", 5 },
        { "IMAGEBUTTON_STATE_NUM", 6 },
        { "KEYBOARD_MODE_TEXT_LOWER", 0 },
        { "KEYBOARD_MODE_TEXT_UPPER", 1 },
        { "KEYBOARD_MODE_SPECIAL", 2 },
        { "KEYBOARD_MODE_NUMBER", 3 },
        { "KEYBOARD_MODE_USER_1", 4 },
        { "KEYBOARD_MODE_USER_2", 5 },
        { "KEYBOARD_MODE_USER_3", 6 },
        { "KEYBOARD_MODE_USER_4", 7 },
        { "MENU_HEADER_TOP_FIXED", 0 },
        { "MENU_HEADER_TOP_UNFIXED", 1 },
        { "MENU_HEADER_BOTTOM_FIXED", 2 },
        { "MENU_ROOT_BACK_BUTTON_DISABLED", 0 },
        { "MENU_ROOT_BACK_BUTTON_ENABLED", 1 },
        { "ROLLER_MODE_NORMAL", 0 },
        { "ROLLER_MODE_INFINITE", 1 },
        { "SCALE_MODE_HORIZONTAL_TOP", 0 },
        { "SCALE_MODE_HORIZONTAL_BOTTOM", 1 },
        { "SCALE_MODE_VERTICAL_LEFT", 2 },
        { "SCALE_MODE_VERTICAL_RIGHT", 3 },
        { "SCALE_MODE_ROUND_INNER", 4 },
        { "SCALE_MODE_ROUND_OUTER", 5 },
        { "SCALE_MODE_LAST", 6 },
        { "SLIDER_MODE_NORMAL", 0 },
        { "SLIDER_MODE_SYMMETRICAL", 1 },
        { "SLIDER_MODE_RANGE", 2 },
        { "SPAN_OVERFLOW_CLIP", 0 },
        { "SPAN_OVERFLOW_ELLIPSIS", 1 },
        { "SPAN_OVERFLOW_LAST", 2 },
        { "SPAN_MODE_FIXED", 0 },
        { "SPAN_MODE_EXPAND", 1 },
        { "SPAN_MODE_BREAK", 2 },
        { "SPAN_MODE_LAST", 3 },
        { "PART_TEXTAREA_PLACEHOLDER", 524288 },
        { "TABLE_CELL_CTRL_MERGE_RIGHT", 1 },
        { "TABLE_CELL_CTRL_TEXT_CROP", 2 },
        { "TABLE_CELL_CTRL_CUSTOM_1", 16 },
        { "TABLE_CELL_CTRL_CUSTOM_2", 32 },
        { "TABLE_CELL_CTRL_CUSTOM_3", 64 },
        { "TABLE_CELL_CTRL_CUSTOM_4", 128 },
        { "SUBJECT_TYPE_INVALID", 0 },
        { "SUBJECT_TYPE_NONE", 1 },
        { "SUBJECT_TYPE_INT", 2 },
        { "SUBJECT_TYPE_POINTER", 3 },
        { "SUBJECT_TYPE_COLOR", 4 },
        { "SUBJECT_TYPE_GROUP", 5 },
        { "SUBJECT_TYPE_STRING", 6 },
        { "GRIDNAV_CTRL_NONE", 0 },
        { "GRIDNAV_CTRL_ROLLOVER", 1 },
        { "GRIDNAV_CTRL_SCROLL_FIRST", 2 },
        { "GRIDNAV_CTRL_HORIZONTAL_MOVE_ONLY", 4 },
        { "GRIDNAV_CTRL_VERTICAL_MOVE_ONLY", 8 },
        { "FREETYPE_FONT_STYLE_NORMAL", 0 },
        { "FREETYPE_FONT_STYLE_ITALIC", 1 },
        { "FREETYPE_FONT_STYLE_BOLD", 2 },
        { "FREETYPE_FONT_RENDER_MODE_BITMAP", 0 },
        { "FREETYPE_FONT_RENDER_MODE_OUTLINE", 1 },
        { "FREETYPE_OUTLINE_END", 0 },
        { "FREETYPE_OUTLINE_MOVE_TO", 1 },
        { "FREETYPE_OUTLINE_LINE_TO", 2 },
        { "FREETYPE_OUTLINE_CUBIC_TO", 3 },
        { "FREETYPE_OUTLINE_CONIC_TO", 4 },
        { "DRAW_SW_MASK_RES_TRANSP", 0 },
        { "DRAW_SW_MASK_RES_FULL_COVER", 1 },
        { "DRAW_SW_MASK_RES_CHANGED", 2 },
        { "DRAW_SW_MASK_RES_UNKNOWN", 3 },
        { "DRAW_SW_MASK_TYPE_LINE", 0 },
        { "DRAW_SW_MASK_TYPE_ANGLE", 1 },
        { "DRAW_SW_MASK_TYPE_RADIUS", 2 },
        { "DRAW_SW_MASK_TYPE_FADE", 3 },
        { "DRAW_SW_MASK_TYPE_MAP", 4 },
        { "DRAW_SW_MASK_LINE_SIDE_LEFT", 0 },
        { "DRAW_SW_MASK_LINE_SIDE_RIGHT", 1 },
        { "DRAW_SW_MASK_LINE_SIDE_TOP", 2 },
        { "DRAW_SW_MASK_LINE_SIDE_BOTTOM", 3 },
        { NULL, 0 }
    };

    /* 宏（数值） */
    static const lvgl_enum_entry_t macro_entries[] = {
#ifdef LV_LABEL_TEXT_SELECTION
        { "LABEL_TEXT_SELECTION", LV_LABEL_TEXT_SELECTION },
#endif
#ifdef LV_LABEL_LONG_TXT_HINT
        { "LABEL_LONG_TXT_HINT", LV_LABEL_LONG_TXT_HINT },
#endif
#ifdef LV_LABEL_WAIT_CHAR_COUNT
        { "LABEL_WAIT_CHAR_COUNT", LV_LABEL_WAIT_CHAR_COUNT },
#endif
#ifdef LV_ANIM_REPEAT_INFINITE
        { "ANIM_REPEAT_INFINITE", LV_ANIM_REPEAT_INFINITE },
#endif
#ifdef LV_ANIM_PLAYTIME_INFINITE
        { "ANIM_PLAYTIME_INFINITE", LV_ANIM_PLAYTIME_INFINITE },
#endif
#ifdef LV_SIZE_CONTENT
        { "SIZE_CONTENT", LV_SIZE_CONTENT },
#endif
#ifdef LV_TRIGO_SIN_MAX
        { "TRIGO_SIN_MAX", LV_TRIGO_SIN_MAX },
#endif
#ifdef LV_TRIGO_SHIFT
        { "TRIGO_SHIFT", LV_TRIGO_SHIFT },
#endif
#ifdef LV_BEZIER_VAL_SHIFT
        { "BEZIER_VAL_SHIFT", LV_BEZIER_VAL_SHIFT },
#endif
#ifdef LV_BEZIER_VAL_MAX
        { "BEZIER_VAL_MAX", LV_BEZIER_VAL_MAX },
#endif
#ifdef LV_LABEL_DOT_NUM
        { "LABEL_DOT_NUM", LV_LABEL_DOT_NUM },
#endif
#ifdef LV_LABEL_POS_LAST
        { "LABEL_POS_LAST", LV_LABEL_POS_LAST },
#endif
#ifdef LV_LABEL_TEXT_SELECTION_OFF
        { "LABEL_TEXT_SELECTION_OFF", LV_LABEL_TEXT_SELECTION_OFF },
#endif
        { NULL, 0 }
    };

    typedef struct { const char* name; const char* val; } lvgl_symbol_entry_t;
    static const lvgl_symbol_entry_t symbol_entries[] = {
#ifdef LV_SYMBOL_BULLET
        { "SYMBOL_BULLET", LV_SYMBOL_BULLET },
#endif
#ifdef LV_SYMBOL_AUDIO
        { "SYMBOL_AUDIO", LV_SYMBOL_AUDIO },
#endif
#ifdef LV_SYMBOL_VIDEO
        { "SYMBOL_VIDEO", LV_SYMBOL_VIDEO },
#endif
#ifdef LV_SYMBOL_LIST
        { "SYMBOL_LIST", LV_SYMBOL_LIST },
#endif
#ifdef LV_SYMBOL_OK
        { "SYMBOL_OK", LV_SYMBOL_OK },
#endif
#ifdef LV_SYMBOL_CLOSE
        { "SYMBOL_CLOSE", LV_SYMBOL_CLOSE },
#endif
#ifdef LV_SYMBOL_POWER
        { "SYMBOL_POWER", LV_SYMBOL_POWER },
#endif
#ifdef LV_SYMBOL_SETTINGS
        { "SYMBOL_SETTINGS", LV_SYMBOL_SETTINGS },
#endif
#ifdef LV_SYMBOL_HOME
        { "SYMBOL_HOME", LV_SYMBOL_HOME },
#endif
#ifdef LV_SYMBOL_DOWNLOAD
        { "SYMBOL_DOWNLOAD", LV_SYMBOL_DOWNLOAD },
#endif
#ifdef LV_SYMBOL_DRIVE
        { "SYMBOL_DRIVE", LV_SYMBOL_DRIVE },
#endif
#ifdef LV_SYMBOL_REFRESH
        { "SYMBOL_REFRESH", LV_SYMBOL_REFRESH },
#endif
#ifdef LV_SYMBOL_MUTE
        { "SYMBOL_MUTE", LV_SYMBOL_MUTE },
#endif
#ifdef LV_SYMBOL_VOLUME_MID
        { "SYMBOL_VOLUME_MID", LV_SYMBOL_VOLUME_MID },
#endif
#ifdef LV_SYMBOL_VOLUME_MAX
        { "SYMBOL_VOLUME_MAX", LV_SYMBOL_VOLUME_MAX },
#endif
#ifdef LV_SYMBOL_IMAGE
        { "SYMBOL_IMAGE", LV_SYMBOL_IMAGE },
#endif
#ifdef LV_SYMBOL_TINT
        { "SYMBOL_TINT", LV_SYMBOL_TINT },
#endif
#ifdef LV_SYMBOL_PREV
        { "SYMBOL_PREV", LV_SYMBOL_PREV },
#endif
#ifdef LV_SYMBOL_PLAY
        { "SYMBOL_PLAY", LV_SYMBOL_PLAY },
#endif
#ifdef LV_SYMBOL_PAUSE
        { "SYMBOL_PAUSE", LV_SYMBOL_PAUSE },
#endif
#ifdef LV_SYMBOL_STOP
        { "SYMBOL_STOP", LV_SYMBOL_STOP },
#endif
#ifdef LV_SYMBOL_NEXT
        { "SYMBOL_NEXT", LV_SYMBOL_NEXT },
#endif
#ifdef LV_SYMBOL_EJECT
        { "SYMBOL_EJECT", LV_SYMBOL_EJECT },
#endif
#ifdef LV_SYMBOL_LEFT
        { "SYMBOL_LEFT", LV_SYMBOL_LEFT },
#endif
#ifdef LV_SYMBOL_RIGHT
        { "SYMBOL_RIGHT", LV_SYMBOL_RIGHT },
#endif
#ifdef LV_SYMBOL_PLUS
        { "SYMBOL_PLUS", LV_SYMBOL_PLUS },
#endif
#ifdef LV_SYMBOL_MINUS
        { "SYMBOL_MINUS", LV_SYMBOL_MINUS },
#endif
#ifdef LV_SYMBOL_EYE_OPEN
        { "SYMBOL_EYE_OPEN", LV_SYMBOL_EYE_OPEN },
#endif
#ifdef LV_SYMBOL_EYE_CLOSE
        { "SYMBOL_EYE_CLOSE", LV_SYMBOL_EYE_CLOSE },
#endif
#ifdef LV_SYMBOL_WARNING
        { "SYMBOL_WARNING", LV_SYMBOL_WARNING },
#endif
#ifdef LV_SYMBOL_SHUFFLE
        { "SYMBOL_SHUFFLE", LV_SYMBOL_SHUFFLE },
#endif
#ifdef LV_SYMBOL_UP
        { "SYMBOL_UP", LV_SYMBOL_UP },
#endif
#ifdef LV_SYMBOL_DOWN
        { "SYMBOL_DOWN", LV_SYMBOL_DOWN },
#endif
#ifdef LV_SYMBOL_LOOP
        { "SYMBOL_LOOP", LV_SYMBOL_LOOP },
#endif
#ifdef LV_SYMBOL_DIRECTORY
        { "SYMBOL_DIRECTORY", LV_SYMBOL_DIRECTORY },
#endif
#ifdef LV_SYMBOL_UPLOAD
        { "SYMBOL_UPLOAD", LV_SYMBOL_UPLOAD },
#endif
#ifdef LV_SYMBOL_CALL
        { "SYMBOL_CALL", LV_SYMBOL_CALL },
#endif
#ifdef LV_SYMBOL_CUT
        { "SYMBOL_CUT", LV_SYMBOL_CUT },
#endif
#ifdef LV_SYMBOL_COPY
        { "SYMBOL_COPY", LV_SYMBOL_COPY },
#endif
#ifdef LV_SYMBOL_SAVE
        { "SYMBOL_SAVE", LV_SYMBOL_SAVE },
#endif
#ifdef LV_SYMBOL_BARS
        { "SYMBOL_BARS", LV_SYMBOL_BARS },
#endif
#ifdef LV_SYMBOL_ENVELOPE
        { "SYMBOL_ENVELOPE", LV_SYMBOL_ENVELOPE },
#endif
#ifdef LV_SYMBOL_CHARGE
        { "SYMBOL_CHARGE", LV_SYMBOL_CHARGE },
#endif
#ifdef LV_SYMBOL_PASTE
        { "SYMBOL_PASTE", LV_SYMBOL_PASTE },
#endif
#ifdef LV_SYMBOL_BELL
        { "SYMBOL_BELL", LV_SYMBOL_BELL },
#endif
#ifdef LV_SYMBOL_KEYBOARD
        { "SYMBOL_KEYBOARD", LV_SYMBOL_KEYBOARD },
#endif
#ifdef LV_SYMBOL_GPS
        { "SYMBOL_GPS", LV_SYMBOL_GPS },
#endif
#ifdef LV_SYMBOL_FILE
        { "SYMBOL_FILE", LV_SYMBOL_FILE },
#endif
#ifdef LV_SYMBOL_WIFI
        { "SYMBOL_WIFI", LV_SYMBOL_WIFI },
#endif
#ifdef LV_SYMBOL_BATTERY_FULL
        { "SYMBOL_BATTERY_FULL", LV_SYMBOL_BATTERY_FULL },
#endif
#ifdef LV_SYMBOL_BATTERY_3
        { "SYMBOL_BATTERY_3", LV_SYMBOL_BATTERY_3 },
#endif
#ifdef LV_SYMBOL_BATTERY_2
        { "SYMBOL_BATTERY_2", LV_SYMBOL_BATTERY_2 },
#endif
#ifdef LV_SYMBOL_BATTERY_1
        { "SYMBOL_BATTERY_1", LV_SYMBOL_BATTERY_1 },
#endif
#ifdef LV_SYMBOL_BATTERY_EMPTY
        { "SYMBOL_BATTERY_EMPTY", LV_SYMBOL_BATTERY_EMPTY },
#endif
#ifdef LV_SYMBOL_USB
        { "SYMBOL_USB", LV_SYMBOL_USB },
#endif
#ifdef LV_SYMBOL_BLUETOOTH
        { "SYMBOL_BLUETOOTH", LV_SYMBOL_BLUETOOTH },
#endif
#ifdef LV_SYMBOL_TRASH
        { "SYMBOL_TRASH", LV_SYMBOL_TRASH },
#endif
#ifdef LV_SYMBOL_EDIT
        { "SYMBOL_EDIT", LV_SYMBOL_EDIT },
#endif
#ifdef LV_SYMBOL_BACKSPACE
        { "SYMBOL_BACKSPACE", LV_SYMBOL_BACKSPACE },
#endif
#ifdef LV_SYMBOL_SD_CARD
        { "SYMBOL_SD_CARD", LV_SYMBOL_SD_CARD },
#endif
#ifdef LV_SYMBOL_NEW_LINE
        { "SYMBOL_NEW_LINE", LV_SYMBOL_NEW_LINE },
#endif
#ifdef LV_SYMBOL_DUMMY
        { "SYMBOL_DUMMY", LV_SYMBOL_DUMMY },
#endif
        { NULL, NULL }
    };

    /* 注册枚举条目 */
    for (size_t i = 0; enum_entries[i].name != NULL; ++i) {
        lvgl_binding_set_enum(parent, enum_entries[i].name, enum_entries[i].value);
    }

    /* 注册数值宏条目 */
    for (size_t i = 0; macro_entries[i].name != NULL; ++i) {
        lvgl_binding_set_enum(parent, macro_entries[i].name, macro_entries[i].value);
    }

    /* 注册 LV_SYMBOL_ 字符串宏（作为全局字符串） */
    for (size_t i = 0; symbol_entries[i].name != NULL; ++i) {
        jerry_value_t key = jerry_string_sz(symbol_entries[i].name);
        jerry_value_t val = jerry_string_sz(symbol_entries[i].val);
        jerry_value_free(jerry_object_set(parent, key, val));
        jerry_value_free(key);
        jerry_value_free(val);
    }

}

/********************************** 初始化 LVGL 绑定系统 **********************************/

/**
 * @brief 初始化回调系统，注册 LVGL 对象删除事件处理函数，并注册 LVGL 函数
 */
void lv_binding_init(jerry_value_t parent) {
    script_engine_register_functions(parent,
                                     lvgl_binding_funcs,
                                     sizeof(lvgl_binding_funcs) / sizeof(script_engine_func_entry_t));
    lv_bindings_special_init(parent);
    lv_bindings_anim_init(parent);
    lv_bindings_event_init(parent);
    lv_bindings_timer_init(parent);
    register_lvgl_enums(parent);
}
