/**
 * @file elena_os_basic_widgets.c
 * @brief 基本控件
 * @author Sab1e
 * @date 2025-08-17
 */

#include "elena_os_basic_widgets.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lvgl.h"
#include "elena_os_core.h"
#include "elena_os_lang.h"
#define EOS_LOG_DISABLE
#define EOS_LOG_TAG "BasicWidgets"
#include "elena_os_log.h"
#include "elena_os_nav.h"
#include "elena_os_img.h"
#include "elena_os_event.h"
#include "script_engine_core.h"
#include "elena_os_theme.h"
#include "elena_os_port.h"
#include "elena_os_misc.h"
#include "elena_os_icon.h"
#include "elena_os_config.h"
#include "elena_os_anim.h"
#include "elena_os_font.h"
#include "elena_os_crown.h"

/* Macros and Definitions -------------------------------------*/

#define _LIST_SWITCH_WIDTH 38
#define _LIST_SWITCH_HEIGHT 65

#define _LIST_LABEL_MARGIN_VER 8

#define _LIST_HEAD_PLACEHOLDER_HEIGHT 110
#define _LIST_TAIL_PLACEHOLDER_HEIGHT 60

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

void eos_screen_load(lv_obj_t *scr)
{
    eos_event_broadcast(EOS_EVENT_GLOBAL_SCREEN_LOAD_START, scr);
    lv_screen_load(scr);
    eos_event_broadcast(EOS_EVENT_GLOBAL_SCREEN_LOADED, scr);
}

lv_obj_t *eos_screen_create(void)
{
    lv_obj_t *scr = lv_obj_create(NULL);
    lv_obj_add_style(scr, eos_theme_get_screen_style(), 0);
    return scr;
}

/**
 * @brief 返回按钮的回调
 */
static void _back_btn_cb(lv_event_t *e)
{
    EOS_LOG_D("NAV back");
    if (eos_nav_back_clean() != EOS_OK)
    {
        EOS_LOG_E("BACK ERR");
    }
}

static void _list_child_created_cb(lv_event_t *e)
{
    lv_obj_t *list = lv_event_get_target(e);
    lv_obj_t *bottom_ph = (lv_obj_t *)lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(bottom_ph && list);
    lv_obj_move_to_index(bottom_ph, -1);
}

void eos_switch_set_state(lv_obj_t *sw, bool checked)
{
    if (checked)
    {
        lv_obj_set_state(sw, LV_STATE_CHECKED, checked);
        lv_obj_send_event(sw, LV_EVENT_VALUE_CHANGED, NULL);
    }
}

lv_obj_t *eos_list_create(lv_obj_t *parent)
{
    EOS_CHECK_PTR_RETURN_VAL(parent, NULL);
    lv_obj_t *list = lv_list_create(parent);
    lv_obj_set_size(list, lv_pct(100), lv_pct(100));
    lv_obj_set_style_pad_ver(list, 0, 0);
    lv_obj_set_style_pad_hor(list, 10, 0);
    lv_obj_center(list);
    lv_obj_set_scrollbar_mode(list, LV_SCROLLBAR_MODE_OFF);

    // 占位符
    eos_list_add_placeholder(list, _LIST_HEAD_PLACEHOLDER_HEIGHT);
    lv_obj_t *bottom_ph = eos_list_add_placeholder(list, _LIST_TAIL_PLACEHOLDER_HEIGHT);
    lv_obj_add_event_cb(list, _list_child_created_cb, LV_EVENT_CHILD_CREATED, bottom_ph);
    return list;
}

lv_obj_t *eos_back_btn_create(lv_obj_t *parent, bool show_text)
{
    lv_obj_t *btn = lv_button_create(parent);
    lv_obj_add_event_cb(btn, _back_btn_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_border_width(btn, 0, 0);
    lv_obj_set_style_shadow_width(btn, 0, 0);

    lv_obj_t *btn_label = lv_label_create(btn);
    if (show_text)
    {
        lv_label_set_text_fmt(btn_label, RI_ARROW_LEFT_S_LINE_LARGE "%s", current_lang[STR_ID_BASE_ITEM_BACK]);
    }
    else
    {
        lv_label_set_text(btn_label, RI_ARROW_LEFT_S_LINE_LARGE);
    }
    lv_obj_set_style_text_color(btn_label, EOS_COLOR_WHITE, 0);
    lv_obj_align(btn_label, LV_ALIGN_CENTER, -2, 0);

    return btn;
}

lv_obj_t *_list_btn_container_create(lv_obj_t *list)
{
    lv_obj_t *btn = lv_button_create(list);
    lv_obj_set_size(btn, lv_pct(100), EOS_LIST_CONTAINER_HEIGHT);
    lv_obj_remove_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scroll_dir(btn, LV_DIR_NONE); // 禁止滚动
    lv_obj_set_style_bg_color(btn, EOS_THEME_SECONDARY_COLOR, 0);
    lv_obj_set_style_border_width(btn, 0, 0);
    lv_obj_set_style_pad_all(btn, EOS_LIST_CONTAINER_PAD_ALL, 0);
    lv_obj_set_style_margin_bottom(btn, 10, 0);
    lv_obj_set_style_align(btn, LV_ALIGN_CENTER, 0);
    lv_obj_set_style_radius(btn, EOS_LIST_OBJ_RADIUS, 0);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_remove_flag(btn, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_set_flex_flow(btn, LV_FLEX_FLOW_ROW); // 水平排布
    lv_obj_set_flex_align(btn,
                          LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    return btn;
}

lv_obj_t *eos_list_add_button(lv_obj_t *list, const void *icon, const char *txt)
{
    lv_obj_t *obj = _list_btn_container_create(list);

    if (icon)
    {
        lv_obj_t *img = lv_image_create(obj);
        eos_img_set_src(img, icon);
        eos_img_set_size(img, 64, 64);
    }

    if (txt)
    {
        lv_obj_t *label = lv_label_create(obj);
        lv_label_set_text(label, txt);
        lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_obj_set_flex_grow(label, 1);
    }

    return obj;
}

lv_obj_t *eos_list_add_placeholder(lv_obj_t *list, uint32_t height)
{
    lv_obj_t *ph = lv_obj_create(list);
    lv_obj_remove_style_all(ph);
    lv_obj_set_size(ph, lv_pct(100), height);
    return ph;
}

lv_obj_t *eos_list_add_circle_icon_button(lv_obj_t *list, lv_color_t circle_color, const void *icon_src, const char *txt)
{
    // 创建按钮
    lv_obj_t *btn = _list_btn_container_create(list);
    // 绘制圆形背景
    lv_obj_t *circle = lv_obj_create(btn);
    lv_obj_set_style_border_width(circle, 0, 0);
    lv_obj_remove_flag(circle, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(circle, 50, 50);
    lv_obj_set_style_pad_all(circle, 0, 0);
    lv_obj_set_style_radius(circle, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(circle, circle_color, 0);
    // 绘制图像
    lv_obj_t *icon = lv_image_create(circle);
    lv_image_set_src(icon, icon_src);
    lv_obj_center(icon);
    // 文字
    lv_obj_t *label = lv_label_create(btn);
    lv_obj_set_style_margin_left(label, 14, 0);
    lv_label_set_text(label, txt);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_flex_grow(label, 1);
    lv_obj_set_style_margin_right(label, 36, 0);
    return btn;
}

lv_obj_t *eos_list_add_circle_icon_button_str_id(lv_obj_t *list, lv_color_t circle_color, const void *icon_src, lang_string_id_t id)
{
    // 创建按钮
    lv_obj_t *btn = _list_btn_container_create(list);
    // 绘制圆形背景
    lv_obj_t *circle = lv_obj_create(btn);
    lv_obj_set_style_border_width(circle, 0, 0);
    lv_obj_remove_flag(circle, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(circle, 45, 45);
    lv_obj_set_style_pad_all(circle, 0, 0);
    lv_obj_set_style_radius(circle, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(circle, circle_color, 0);
    // 绘制图像
    lv_obj_t *icon = lv_image_create(circle);
    lv_image_set_src(icon, icon_src);
    lv_obj_center(icon);
    // 文字
    lv_obj_t *label = lv_label_create(btn);
    eos_label_set_text_id(label, id);
    lv_obj_set_style_margin_left(label, 14, 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_flex_grow(label, 1);
    lv_obj_set_style_margin_right(label, 36, 0);
    return btn;
}

lv_obj_t *eos_list_add_entry_button(lv_obj_t *list, const char *txt)
{
    // 创建按钮
    lv_obj_t *btn = _list_btn_container_create(list);
    // 文字
    lv_obj_t *label = lv_label_create(btn);
    lv_obj_set_style_margin_left(label, 14, 0);
    lv_label_set_text(label, txt);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_flex_grow(label, 1);
    // 文字
    label = lv_label_create(btn);
    lv_obj_set_style_margin_right(label, 14, 0);
    lv_label_set_text(label, RI_ARROW_RIGHT_S_LINE);
    return btn;
}

lv_obj_t *eos_list_add_entry_button_str_id(lv_obj_t *list, language_id_t id)
{
    // 创建按钮
    lv_obj_t *btn = _list_btn_container_create(list);
    // 文字
    lv_obj_t *label = lv_label_create(btn);
    eos_label_set_text_id(label, id);
    lv_obj_set_style_margin_left(label, 14, 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_flex_grow(label, 1);
    // 文字
    label = lv_label_create(btn);
    lv_obj_set_style_margin_right(label, 14, 0);
    lv_label_set_text(label, RI_ARROW_RIGHT_S_LINE);
    return btn;
}

static void _list_container_common_style(lv_obj_t *container)
{
    lv_obj_remove_flag(container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scroll_dir(container, LV_DIR_NONE); // 禁止滚动
    lv_obj_set_style_bg_color(container, EOS_THEME_SECONDARY_COLOR, 0);
    lv_obj_set_style_border_width(container, 0, 0);
    lv_obj_set_style_pad_all(container, EOS_LIST_CONTAINER_PAD_ALL, 0);
    lv_obj_set_style_margin_bottom(container, 20, 0);
    lv_obj_set_style_align(container, LV_ALIGN_CENTER, 0);
    lv_obj_set_style_radius(container, EOS_LIST_OBJ_RADIUS, 0);
    lv_obj_set_style_shadow_width(container, 0, 0);
    lv_obj_remove_flag(container, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
}

lv_obj_t *eos_list_add_container(lv_obj_t *list)
{
    lv_obj_t *container = lv_obj_create(list);
    _list_container_common_style(container);
    return container;
}

lv_obj_t *eos_list_add_button_container(lv_obj_t *list)
{
    lv_obj_t *container = lv_button_create(list);
    _list_container_common_style(container);
    return container;
}

static void _list_switch_container_clicked_cb(lv_event_t *e)
{
    lv_obj_t *sw = lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(sw);

    if (lv_obj_has_state(sw, LV_STATE_CHECKED))
    {
        lv_obj_remove_state(sw, LV_STATE_CHECKED);
    }
    else
    {
        lv_obj_add_state(sw, LV_STATE_CHECKED);
    }
    lv_obj_send_event(sw, LV_EVENT_VALUE_CHANGED, NULL);
}

lv_obj_t *eos_list_add_switch(lv_obj_t *list, const char *txt)
{
    // 创建容器
    lv_obj_t *container = eos_list_add_button_container(list);
    lv_obj_set_size(container, lv_pct(100), EOS_LIST_CONTAINER_HEIGHT);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW); // 水平排布
    lv_obj_set_flex_align(container,
                          LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);

    // 文字
    lv_obj_t *label = lv_label_create(container);
    lv_label_set_text(label, txt);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_flex_grow(label, 1);

    // 开关
    lv_obj_t *sw = lv_switch_create(container);
    lv_obj_set_height(sw, _LIST_SWITCH_WIDTH);
    lv_obj_update_layout(sw);
    lv_obj_set_width(sw, _LIST_SWITCH_HEIGHT);
    lv_obj_add_event_cb(container, _list_switch_container_clicked_cb, LV_EVENT_CLICKED, sw);

    return sw;
}

static void _list_label_common_style(lv_obj_t *label)
{
    eos_label_set_font_size(label, EOS_FONT_SIZE_SMALL);
    lv_obj_set_align(label, LV_ALIGN_LEFT_MID);
    lv_obj_set_size(label, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_style_margin_hor(label, EOS_LIST_CONTAINER_PAD_ALL, 0);
    lv_obj_set_style_margin_ver(label, _LIST_LABEL_MARGIN_VER, 0);
}

lv_obj_t *eos_list_add_title(lv_obj_t *list, const char *txt)
{
    lv_obj_t *label = lv_label_create(list);
    _list_label_common_style(label);
    lv_label_set_text(label, txt);
    lv_obj_set_style_text_color(label, EOS_COLOR_WHITE, 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    return label;
}

lv_obj_t *eos_list_add_comment(lv_obj_t *list, const char *txt)
{
    lv_obj_t *label = lv_label_create(list);
    _list_label_common_style(label);
    lv_label_set_text(label, txt);
    lv_obj_set_style_text_color(label, EOS_COLOR_GREY_1, 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    return label;
}

lv_obj_t *_split_line_create(lv_obj_t *parent)
{
    lv_obj_t *sl = lv_obj_create(parent);
    lv_obj_remove_style_all(sl);
    lv_obj_set_size(sl, lv_pct(90), 2);
    lv_obj_set_style_bg_color(sl, EOS_COLOR_DARK_GREY_2, 0);
    return sl;
}

static void _list_slider_delete_cb(lv_event_t *e)
{
    eos_list_slider_t *list_slider = lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(list_slider);
    eos_free(list_slider);
}

lv_obj_t *eos_list_add_title_container(lv_obj_t *list, const char *title)
{
    // 创建外层透明容器
    lv_obj_t *outer_container = lv_obj_create(list);
    lv_obj_remove_style_all(outer_container);                       // 移除默认样式
    lv_obj_set_size(outer_container, lv_pct(100), LV_SIZE_CONTENT); // 高度自适应
    lv_obj_set_style_bg_opa(outer_container, LV_OPA_TRANSP, 0);     // 设置透明背景
    lv_obj_set_flex_flow(outer_container, LV_FLEX_FLOW_COLUMN);     // 垂直布局

    // 添加左上角标签
    lv_obj_t *txt_label = eos_list_add_title(outer_container, title);

    // 创建内层容器（水平居中）
    lv_obj_t *inner_container = eos_list_add_container(outer_container);
    lv_obj_set_style_align(inner_container, LV_ALIGN_CENTER, 0); // 水平居中
    lv_obj_set_style_margin_hor(inner_container, 0, 0);

    return inner_container;
}

static void _slider_button_clicked_cb(lv_event_t *e)
{
    const uint8_t scale_diff = 50;
    const uint8_t duration = 120;
    lv_obj_t *target = lv_event_get_target(e);
    eos_list_slider_t *ls = (eos_list_slider_t *)lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(ls);
    if (ls->minus_btn == target)
    {
        eos_anim_transform_scale_start_ex(
            ls->minus_label,
            ls->minus_label_scale,
            ls->minus_label_scale - scale_diff,
            duration, duration, 1, false);
    }
    else if (ls->plus_btn == target)
    {
        eos_anim_transform_scale_start_ex(
            ls->plus_label,
            ls->plus_label_scale,
            ls->plus_label_scale - scale_diff,
            duration, duration, 1, false);
    }
}

void eos_list_slider_set_minus_label_scale(eos_list_slider_t *ls, uint16_t scale)
{
    EOS_CHECK_PTR_RETURN(ls);
    ls->minus_label_scale = scale;
    lv_obj_set_style_transform_scale(ls->minus_label, scale, 0);
}

void eos_list_slider_set_plus_label_scale(eos_list_slider_t *ls, uint16_t scale)
{
    EOS_CHECK_PTR_RETURN(ls);
    ls->plus_label_scale = scale;
    lv_obj_set_style_transform_scale(ls->plus_label, scale, 0);
}

eos_list_slider_t *eos_list_add_slider(lv_obj_t *list, const char *txt)
{
    eos_list_slider_t *list_slider = eos_malloc_zeroed(sizeof(eos_list_slider_t));
    EOS_CHECK_PTR_RETURN_VAL_FREE(list_slider, NULL, list_slider);

    list_slider->minus_label_scale = 255;
    list_slider->plus_label_scale = 255;

    // 创建外层透明容器
    lv_obj_t *inner_container = eos_list_add_title_container(list, txt);
    lv_obj_set_style_pad_all(inner_container, 0, 0);
    lv_obj_set_size(inner_container, lv_pct(100), EOS_LIST_CONTAINER_HEIGHT);
    lv_obj_add_event_cb(inner_container, _list_slider_delete_cb, LV_EVENT_DELETE, (void *)list_slider);

    const uint8_t margin = 25;

    const uint8_t pct_slider = 50;
    const uint8_t pct_btn = (100 - pct_slider) / 2;

    /************************** 左侧 **************************/
    list_slider->minus_btn = lv_obj_create(inner_container);
    lv_obj_set_size(list_slider->minus_btn, lv_pct(pct_btn), EOS_LIST_CONTAINER_HEIGHT);
    lv_obj_set_style_margin_all(list_slider->minus_btn, 0, 0);
    lv_obj_set_style_pad_all(list_slider->minus_btn, 0, 0);
    lv_obj_set_style_bg_opa(list_slider->minus_btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(list_slider->minus_btn, EOS_THEME_SECONDARY_COLOR, 0);
    lv_obj_set_style_radius(list_slider->minus_btn, EOS_ITEM_RADIUS, 0);
    lv_obj_set_style_border_width(list_slider->minus_btn, 0, 0);
    lv_obj_align_to(list_slider->minus_btn, inner_container, LV_ALIGN_LEFT_MID, 0, 0);

    list_slider->minus_label = lv_label_create(list_slider->minus_btn);
    lv_label_set_text(list_slider->minus_label, RI_SUBTRACT_FILL);
    lv_obj_center(list_slider->minus_label);
    lv_obj_add_event_cb(list_slider->minus_btn, _slider_button_clicked_cb, LV_EVENT_CLICKED, list_slider);
    lv_obj_update_layout(list_slider->minus_label);
    lv_obj_set_style_transform_pivot_x(list_slider->minus_label, lv_obj_get_width(list_slider->minus_label) / 2, 0);
    lv_obj_set_style_transform_pivot_y(list_slider->minus_label, lv_obj_get_height(list_slider->minus_label) / 2, 0);

    /************************** 右侧 **************************/
    list_slider->plus_btn = lv_obj_create(inner_container);
    lv_obj_set_size(list_slider->plus_btn, lv_pct(pct_btn), EOS_LIST_CONTAINER_HEIGHT);
    lv_obj_set_style_margin_all(list_slider->plus_btn, 0, 0);
    lv_obj_set_style_pad_all(list_slider->plus_btn, 0, 0);
    lv_obj_set_style_bg_opa(list_slider->plus_btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(list_slider->plus_btn, EOS_THEME_SECONDARY_COLOR, 0);
    lv_obj_set_style_border_width(list_slider->plus_btn, 0, 0);
    lv_obj_set_style_radius(list_slider->plus_btn, EOS_ITEM_RADIUS, 0);
    lv_obj_align_to(list_slider->plus_btn, inner_container, LV_ALIGN_RIGHT_MID, 0, 0);

    list_slider->plus_label = lv_label_create(list_slider->plus_btn);
    lv_label_set_text(list_slider->plus_label, RI_ADD_FILL);
    lv_obj_center(list_slider->plus_label);
    lv_obj_add_event_cb(list_slider->plus_btn, _slider_button_clicked_cb, LV_EVENT_CLICKED, list_slider);
    lv_obj_update_layout(list_slider->plus_label);
    lv_obj_set_style_transform_pivot_x(list_slider->plus_label, lv_obj_get_width(list_slider->plus_label) / 2, 0);
    lv_obj_set_style_transform_pivot_y(list_slider->plus_label, lv_obj_get_height(list_slider->plus_label) / 2, 0);

    const uint8_t split_width = 2;
    lv_obj_t *obj = lv_obj_create(inner_container);
    lv_obj_set_style_bg_color(obj, EOS_THEME_SECONDARY_COLOR, 0);
    lv_obj_set_style_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_size(obj, lv_pct(pct_slider), EOS_LIST_CONTAINER_HEIGHT + split_width * 2);
    lv_obj_set_style_border_width(obj, split_width, 0);
    lv_obj_set_style_border_color(obj, EOS_COLOR_BLACK, 0);
    lv_obj_set_style_radius(obj, 0, 0);
    lv_obj_center(obj);

    // 滑块
    const uint8_t slider_height = 5;
    const uint8_t slider_main_bg_darken_lvl = 180;
    list_slider->slider = lv_slider_create(obj);
    lv_obj_set_style_margin_left(list_slider->slider, 18, 0);
    lv_obj_set_size(list_slider->slider, lv_pct(100), slider_height);
    lv_obj_set_style_margin_top(list_slider->slider, 12, 0);
    lv_obj_center(list_slider->slider);
    lv_obj_set_style_bg_opa(list_slider->slider, LV_OPA_TRANSP, LV_PART_KNOB);
    lv_obj_set_style_border_opa(list_slider->slider, LV_OPA_TRANSP, LV_PART_KNOB);
    lv_obj_set_style_bg_color(list_slider->slider, lv_color_darken(EOS_COLOR_GREEN, slider_main_bg_darken_lvl), LV_PART_MAIN);

    return list_slider;
}

lv_obj_t *eos_row_create(lv_obj_t *parent,
                         const char *left_text,
                         const char *right_text,
                         const char *left_img_path,
                         int icon_w, int icon_h)
{
    lv_obj_t *row = lv_obj_create(parent);
    lv_obj_remove_style_all(row);
    lv_obj_set_size(row, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row,
                          LV_FLEX_ALIGN_SPACE_BETWEEN, // 主轴两端对齐
                          LV_FLEX_ALIGN_CENTER,        // 交叉轴居中
                          LV_FLEX_ALIGN_CENTER);

    // 左边图像
    if (left_img_path)
    {
        lv_obj_t *icon = lv_image_create(row);
        eos_img_set_src(icon, left_img_path);
        eos_img_set_size(icon, icon_w, icon_h);
    }

    // 左边文本
    if (left_text)
    {
        lv_obj_t *left_label = lv_label_create(row);
        lv_label_set_text(left_label, left_text);
    }

    // 右边文本
    if (right_text)
    {
        lv_obj_t *right_label = lv_label_create(row);
        lv_label_set_text(right_label, right_text);
        lv_obj_set_flex_grow(right_label, 1); // 吃掉中间空间
        lv_obj_set_style_text_align(right_label, LV_TEXT_ALIGN_RIGHT, 0);
        lv_label_set_long_mode(right_label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    }

    return row;
}
