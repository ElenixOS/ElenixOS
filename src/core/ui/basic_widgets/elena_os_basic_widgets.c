/**
 * @file elena_os_basic_widgets.c
 * @brief 基本控件
 * @author Sab1e
 * @date 2025-08-17
 */

#include "elena_os_basic_widgets.h"

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lvgl.h"
#include "elena_os_core.h"
#include "elena_os_lang.h"
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

// Macros and Definitions
#define APP_HEADER_HEIGHT 120
#define APP_HEADER_CLOCK_UPDATE_PERIOD_MS 60000 // 一分钟

#define APP_HEADER_MARGIN_RIGHT 30

#define APP_HEADER_TITLE_WIDTH 240

typedef enum
{
    APP_HEADER_TITLE_TYPE_STRING,
    APP_HEADER_TITLE_TYPE_ID
} eos_app_header_title_string_type_t;

typedef union
{
    const char *string;
    language_id_t id;
} eos_app_header_title_data_t;

typedef struct
{
    eos_app_header_title_string_type_t type;
    eos_app_header_title_data_t data;
} eos_app_header_title_t;

// Variables
static eos_app_header_t *app_header = NULL;
// Function Implementations

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

lv_obj_t *eos_list_create(lv_obj_t *parent)
{
    EOS_CHECK_PTR_RETURN_VAL(parent, NULL);
    lv_obj_t *list = lv_list_create(parent);
    lv_obj_set_size(list, lv_pct(100), lv_pct(100));
    lv_obj_set_style_pad_ver(list, 0, 0);
    lv_obj_set_style_pad_hor(list, 20, 0);
    lv_obj_center(list);
    lv_obj_set_scrollbar_mode(list, LV_SCROLLBAR_MODE_OFF);

    // 占位符
    eos_list_add_placeholder(list, EOS_LIST_PLACEHOLDER_HEIGHT);
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
    }else{
        lv_label_set_text(btn_label, RI_ARROW_LEFT_S_LINE_LARGE);
    }
    lv_obj_set_style_text_color(btn_label, EOS_COLOR_WHITE, 0);
    lv_obj_align(btn_label, LV_ALIGN_CENTER, -2, 0);

    return btn;
}

/**
 * @brief 更新LVGL字符串，显示当前时间
 */
static inline void _app_header_update_clock_label(lv_obj_t *label)
{
    eos_datetime_t dt = eos_time_get();
    lv_label_set_text_fmt(label, "%02d:%02d", dt.hour, dt.min);
}

/**
 * @brief 时间刷新的回调，由 LVGL 的定时器触发
 */
static void _clock_update_cb(lv_timer_t *timer)
{
    lv_obj_t *label = lv_timer_get_user_data(timer);
    EOS_CHECK_PTR_RETURN(label);
    // 更新显示文字
    _app_header_update_clock_label(label);
}

static void _app_header_lang_changed_cb(lv_event_t *e)
{
    lv_obj_t *label = lv_event_get_target(e);

    // 从用户数据中获取str_id
    eos_app_header_title_t *t = (eos_app_header_title_t *)lv_obj_get_user_data(lv_screen_active());
    if (!t)
        return;
    if (t->type == APP_HEADER_TITLE_TYPE_ID)
    {
        lv_label_set_text(label, current_lang[t->data.id]);
    }
}

/**
 * @brief Header 的 screen 加载事件回调
 */
static void _screen_loaded_cb(lv_event_t *e)
{
    EOS_LOG_D("screen loaded");
    lv_obj_t *scr = lv_event_get_target(e);

    eos_app_header_title_t *t = (eos_app_header_title_t *)lv_obj_get_user_data(scr);

    if ((t->type == APP_HEADER_TITLE_TYPE_STRING) && t->data.string)
    {
        EOS_LOG_D("Set String title: %s", t->data.string);
        lv_label_set_text(app_header->title_label, t->data.string);
        eos_app_header_show();
    }
    else if (t->type == APP_HEADER_TITLE_TYPE_ID)
    {
        EOS_LOG_D("Set ID title: %s", current_lang[t->data.id]);
        lv_label_set_text(app_header->title_label, current_lang[t->data.id]);
        eos_app_header_show();
    }
    else
    {
        EOS_LOG_D("no title");
        eos_app_header_hide();
    }
}

/**
 * @brief Header 的 screen 删除事件回调
 */
static void _screen_delete_cb(lv_event_t *e)
{
    lv_obj_t *scr = lv_event_get_target(e);
    EOS_CHECK_PTR_RETURN(scr);
    EOS_LOG_D("screen deleted");
    eos_app_header_title_t *t = (eos_app_header_title_t *)lv_obj_get_user_data(scr);
    EOS_CHECK_PTR_RETURN(t);
    if ((t->type == APP_HEADER_TITLE_TYPE_STRING) && t->data.string)
        free(t->data.string);
    lv_obj_set_user_data(scr, NULL);
    eos_app_header_hide();
    eos_event_remove_cb(scr, LV_EVENT_REFRESH, _app_header_lang_changed_cb);
    free(t);
}

void eos_app_header_set_title(lv_obj_t *scr, const char *title)
{
    eos_app_header_title_t *t = (eos_app_header_title_t *)lv_obj_get_user_data(scr);
    EOS_CHECK_PTR_RETURN(app_header && t);
    // 复制一份避免被删除
    t->type = APP_HEADER_TITLE_TYPE_STRING;
    if (t->data.string)
        free(t->data.string);
    t->data.string = eos_strdup(title);
    // 更新字符串
    lv_label_set_text(app_header->title_label, t->data.string);
}

void eos_app_header_set_title_str_id(lv_obj_t *scr, language_id_t id)
{
    eos_app_header_title_t *t = (eos_app_header_title_t *)lv_obj_get_user_data(scr);
    EOS_CHECK_PTR_RETURN(app_header && t);
    // 复制一份避免被删除
    t->type = APP_HEADER_TITLE_TYPE_ID;
    if (t->data.string)
        free(t->data.string);
    t->data.id = id;
    // 更新字符串
    lv_label_set_text(app_header->title_label, current_lang[id]);
}

void eos_app_header_hide(void)
{
    EOS_CHECK_PTR_RETURN(app_header);
    lv_obj_add_flag(app_header->container, LV_OBJ_FLAG_HIDDEN);
}

void eos_app_header_show(void)
{
    EOS_CHECK_PTR_RETURN(app_header);
    lv_obj_remove_flag(app_header->container, LV_OBJ_FLAG_HIDDEN);
}

void eos_screen_bind_header(lv_obj_t *scr, const char *title)
{
    EOS_CHECK_PTR_RETURN(scr);
    eos_app_header_title_t *t = malloc(sizeof(eos_app_header_title_t));
    EOS_CHECK_PTR_RETURN(t);
    t->type = APP_HEADER_TITLE_TYPE_STRING;
    t->data.string = eos_strdup(title);
    lv_obj_set_user_data(scr, (void *)t);

    // LVGL 会在 screen 加载时触发 LV_EVENT_SCREEN_LOADED
    // 并在 screen 被删除时触发 LV_EVENT_DELETE
    _app_header_update_clock_label(app_header->clock_label); // 提前触发一次同步时钟
    lv_obj_add_event_cb(scr, _screen_loaded_cb, LV_EVENT_SCREEN_LOADED, NULL);
    lv_obj_add_event_cb(scr, _screen_delete_cb, LV_EVENT_DELETE, NULL);
}

void eos_screen_bind_header_str_id(lv_obj_t *scr, lang_string_id_t id)
{
    EOS_CHECK_PTR_RETURN(scr);

    eos_app_header_title_t *t = malloc(sizeof(eos_app_header_title_t));
    EOS_CHECK_PTR_RETURN(t);
    t->type = APP_HEADER_TITLE_TYPE_ID;
    t->data.id = id;
    lv_obj_set_user_data(scr, (void *)t);

    // LVGL 会在 screen 加载时触发 LV_EVENT_SCREEN_LOADED
    // 并在 screen 被删除时触发 LV_EVENT_DELETE
    _app_header_update_clock_label(app_header->clock_label); // 提前触发一次同步时钟
    lv_obj_add_event_cb(scr, _screen_loaded_cb, LV_EVENT_SCREEN_LOADED, NULL);
    lv_obj_add_event_cb(scr, _screen_delete_cb, LV_EVENT_DELETE, NULL);
}

void eos_app_header_init(void)
{
    EOS_LOG_D("Init eos_app_header");
    app_header = lv_malloc(sizeof(eos_app_header_t));
    EOS_CHECK_PTR_RETURN_FREE(app_header, app_header);
    memset(app_header, 0, sizeof(eos_app_header_t));

    // 半透明容器
    app_header->container = lv_image_create(lv_layer_top());
    lv_obj_set_size(app_header->container, EOS_DISPLAY_WIDTH, APP_HEADER_HEIGHT);
    lv_obj_align(app_header->container, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_move_background(app_header->container);
    eos_img_set_src(app_header->container, EOS_IMG_APP_HEADER_BG);

    lv_coord_t header_h = APP_HEADER_HEIGHT;
    lv_coord_t header_w = lv_obj_get_width(app_header->container);

    // 返回按钮
    app_header->back_btn = eos_back_btn_create(app_header->container, false);
    lv_obj_set_size(app_header->back_btn, 64, 64);
    lv_obj_set_style_radius(app_header->back_btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(app_header->back_btn, EOS_THEME_SECONDARY_COLOR, 0);
    lv_obj_align(app_header->back_btn, LV_ALIGN_LEFT_MID, 20, 0);

    // 时间文字
    app_header->clock_label = lv_label_create(app_header->container);
    lv_obj_add_style(app_header->clock_label, eos_theme_get_label_style(), 0);
    _app_header_update_clock_label(app_header->clock_label);
    lv_obj_align(app_header->clock_label, LV_ALIGN_RIGHT_MID, -APP_HEADER_MARGIN_RIGHT, -20);
    app_header->clock_timer = lv_timer_create(_clock_update_cb, APP_HEADER_CLOCK_UPDATE_PERIOD_MS, app_header->clock_label);

    // 标题文字
    app_header->title_label = lv_label_create(app_header->container);
    lv_obj_add_style(app_header->title_label, eos_theme_get_label_style(), 0);
    lv_obj_set_width(app_header->title_label, APP_HEADER_TITLE_WIDTH);
    lv_label_set_text(app_header->title_label, "");

    lv_label_set_long_mode(app_header->title_label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_text_align(app_header->title_label, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_set_style_text_color(app_header->title_label, EOS_THEME_PRIMARY_COLOR, 0);
    lv_obj_align(app_header->title_label, LV_ALIGN_RIGHT_MID, -APP_HEADER_MARGIN_RIGHT, 20);

    eos_event_add_cb(app_header->title_label, _app_header_lang_changed_cb, LV_EVENT_REFRESH, NULL);

    // 默认隐藏 app_header
    lv_obj_add_flag(app_header->container, LV_OBJ_FLAG_HIDDEN);
}

lv_obj_t *_list_btn_container_create(lv_obj_t *list)
{
    lv_obj_t *btn = lv_button_create(list);
    lv_obj_set_size(btn, lv_pct(100), EOS_LIST_CONTAINER_HEIGHT);
    lv_obj_remove_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scroll_dir(btn, LV_DIR_NONE); // 禁止滚动
    lv_obj_set_style_bg_color(btn, EOS_THEME_SECONDARY_COLOR, 0);
    lv_obj_set_style_border_width(btn, 0, 0);
    lv_obj_set_style_pad_all(btn, 18, 0);
    lv_obj_set_style_margin_bottom(btn, 20, 0);
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
    eos_label_set_text_id(label,id);
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
    eos_label_set_text_id(label,id);
    lv_obj_set_style_margin_left(label, 14, 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_flex_grow(label, 1);
    // 文字
    label = lv_label_create(btn);
    lv_obj_set_style_margin_right(label, 14, 0);
    lv_label_set_text(label, RI_ARROW_RIGHT_S_LINE);
    return btn;
}

lv_obj_t *eos_list_add_container(lv_obj_t *list)
{
    lv_obj_t *container = lv_obj_create(list);
    lv_obj_remove_flag(container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scroll_dir(container, LV_DIR_NONE); // 禁止滚动
    lv_obj_set_style_bg_color(container, EOS_THEME_SECONDARY_COLOR, 0);
    lv_obj_set_style_border_width(container, 0, 0);
    lv_obj_set_style_pad_all(container, 18, 0);
    lv_obj_set_style_margin_bottom(container, 20, 0);
    lv_obj_set_style_align(container, LV_ALIGN_CENTER, 0);
    lv_obj_set_style_radius(container, EOS_LIST_OBJ_RADIUS, 0);
    lv_obj_set_style_shadow_width(container, 0, 0);
    lv_obj_remove_flag(container, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    return container;
}

lv_obj_t *eos_list_add_switch(lv_obj_t *list, const char *txt)
{
    // 创建容器
    lv_obj_t *container = eos_list_add_container(list);
    lv_obj_set_size(container, lv_pct(100), EOS_LIST_CONTAINER_HEIGHT);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW); // 水平排布
    lv_obj_set_flex_align(container,
                          LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);

    // 文字
    lv_obj_t *label = lv_label_create(container);
    lv_obj_set_style_margin_left(label, 18, 0);
    lv_label_set_text(label, txt);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_flex_grow(label, 1);

    // 开关
    lv_obj_t *sw = lv_switch_create(container);
    lv_obj_set_style_margin_right(sw, 18, 0);
    lv_obj_set_height(sw, lv_pct(100));
    lv_obj_update_layout(sw);
    int32_t w = lv_obj_get_height(sw) * 2;
    lv_obj_set_width(sw, w);

    return sw;
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
    lv_free(list_slider);
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
    lv_obj_t *txt_label = lv_label_create(outer_container);
    lv_label_set_text(txt_label, title);
    lv_obj_set_style_text_align(txt_label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_align(txt_label, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_margin_bottom(txt_label, 10, 0);

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
    eos_list_slider_t *list_slider = (eos_list_slider_t *)lv_malloc(sizeof(eos_list_slider_t));
    EOS_CHECK_PTR_RETURN_VAL_FREE(list_slider, NULL, list_slider);
    memset(list_slider, 0, sizeof(eos_list_slider_t));

    list_slider->minus_label_scale = 255;
    list_slider->plus_label_scale = 255;

    // 创建外层透明容器
    lv_obj_t *inner_container = eos_list_add_title_container(list, txt);
    lv_obj_set_style_pad_all(inner_container, 0, 0);
    lv_obj_set_size(inner_container, lv_pct(100), EOS_LIST_CONTAINER_HEIGHT);
    lv_obj_add_event_cb(inner_container, _list_slider_delete_cb, LV_EVENT_DELETE, (void *)list_slider);

    const uint8_t margin = 25;
    const uint8_t button_radius = 20;

    const uint8_t pct_slider = 50;
    const uint8_t pct_btn = (100 - pct_slider) / 2;

    /************************** 左侧 **************************/
    list_slider->minus_btn = lv_obj_create(inner_container);
    lv_obj_set_size(list_slider->minus_btn, lv_pct(pct_btn), EOS_LIST_CONTAINER_HEIGHT);
    lv_obj_set_style_margin_all(list_slider->minus_btn, 0, 0);
    lv_obj_set_style_pad_all(list_slider->minus_btn, 0, 0);
    lv_obj_set_style_bg_opa(list_slider->minus_btn, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(list_slider->minus_btn, EOS_THEME_SECONDARY_COLOR, 0);
    lv_obj_set_style_radius(list_slider->minus_btn, button_radius, 0);
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
    lv_obj_set_style_radius(list_slider->plus_btn, button_radius, 0);
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
