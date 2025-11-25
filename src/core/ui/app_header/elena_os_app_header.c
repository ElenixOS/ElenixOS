/**
 * @file elena_os_app_header.c
 * @brief 应用的顶部导航头
 * @author Sab1e
 * @date 2025-11-25
 */

#include "elena_os_app_header.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "elena_os_core.h"
#include "elena_os_port.h"
#include "elena_os_lang.h"
#include "elena_os_img.h"
#include "elena_os_theme.h"
#include "elena_os_font.h"
#include "elena_os_basic_widgets.h"
#include "elena_os_misc.h"
/* Macros and Definitions -------------------------------------*/
#define _HEADER_HEIGHT 120
#define _HEADER_CLOCK_UPDATE_PERIOD_MS 60000 // 一分钟

#define _HEADER_MARGIN_RIGHT 30

#define _HEADER_TITLE_WIDTH 240

/**
 * @brief 应用内上方的头部结构体定义
 */
typedef struct{
    lv_obj_t *container;
    lv_obj_t *clock_label;
    lv_obj_t *title_label;
    lv_obj_t *back_btn;
    lv_timer_t *clock_timer;
}eos_app_header_t;

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

/* Variables --------------------------------------------------*/
static eos_app_header_t *app_header = NULL;
/* Function Implementations -----------------------------------*/


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
        eos_free(t->data.string);
    lv_obj_set_user_data(scr, NULL);
    eos_app_header_hide();
    eos_event_remove_cb(scr, LV_EVENT_REFRESH, _app_header_lang_changed_cb);
    eos_free(t);
}

void eos_app_header_set_title(lv_obj_t *scr, const char *title)
{
    eos_app_header_title_t *t = (eos_app_header_title_t *)lv_obj_get_user_data(scr);
    EOS_CHECK_PTR_RETURN(app_header && t);
    // 复制一份避免被删除
    t->type = APP_HEADER_TITLE_TYPE_STRING;
    if (t->data.string)
        eos_free(t->data.string);
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
        eos_free(t->data.string);
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

void eos_app_header_bind_screen(lv_obj_t *scr, const char *title)
{
    EOS_CHECK_PTR_RETURN(scr);
    eos_app_header_title_t *t = eos_malloc(sizeof(eos_app_header_title_t));
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

void eos_app_header_bind_screen_str_id(lv_obj_t *scr, lang_string_id_t id)
{
    EOS_CHECK_PTR_RETURN(scr);

    eos_app_header_title_t *t = eos_malloc(sizeof(eos_app_header_title_t));
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
    app_header = eos_malloc_zeroed(sizeof(eos_app_header_t));
    EOS_CHECK_PTR_RETURN_FREE(app_header, app_header);

    // 半透明容器
    app_header->container = lv_image_create(lv_layer_top());
    lv_obj_set_size(app_header->container, EOS_DISPLAY_WIDTH, _HEADER_HEIGHT);
    lv_obj_align(app_header->container, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_move_background(app_header->container);
    eos_img_set_src(app_header->container, EOS_IMG_APP_HEADER_BG);
    lv_obj_remove_flag(app_header->container, LV_OBJ_FLAG_SCROLLABLE);

    lv_coord_t header_h = _HEADER_HEIGHT;
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
    lv_obj_align(app_header->clock_label, LV_ALIGN_RIGHT_MID, -_HEADER_MARGIN_RIGHT, -20);
    app_header->clock_timer = lv_timer_create(_clock_update_cb, _HEADER_CLOCK_UPDATE_PERIOD_MS, app_header->clock_label);

    // 标题文字
    app_header->title_label = lv_label_create(app_header->container);
    lv_obj_add_style(app_header->title_label, eos_theme_get_label_style(), 0);
    lv_obj_set_width(app_header->title_label, _HEADER_TITLE_WIDTH);
    lv_label_set_text(app_header->title_label, "");
    eos_label_set_font_size(app_header->title_label, EOS_FONT_SIZE_LARGE);

    lv_label_set_long_mode(app_header->title_label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_text_align(app_header->title_label, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_set_style_text_color(app_header->title_label, EOS_THEME_PRIMARY_COLOR, 0);
    lv_obj_align(app_header->title_label, LV_ALIGN_RIGHT_MID, -_HEADER_MARGIN_RIGHT, 20);

    eos_event_add_cb(app_header->title_label, _app_header_lang_changed_cb, LV_EVENT_REFRESH, NULL);

    // 默认隐藏 app_header
    lv_obj_add_flag(app_header->container, LV_OBJ_FLAG_HIDDEN);
}
