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
// #define EOS_LOG_DISABLE
#define EOS_LOG_TAG "AppHeader"
#include "elena_os_log.h"
#include "elena_os_core.h"
#include "elena_os_port.h"
#include "elena_os_lang.h"
#include "elena_os_img.h"
#include "elena_os_theme.h"
#include "elena_os_font.h"
#include "elena_os_basic_widgets.h"
#include "elena_os_misc.h"
#include "elena_os_anim.h"
#include "elena_os_nav.h"
#include "elena_os_screen_mgr.h"
#include "elena_os_mem.h"

/* Macros and Definitions -------------------------------------*/
#define _HEADER_HEIGHT 120
#define _HEADER_CLOCK_UPDATE_PERIOD_MINUTES 1 /**< 时钟标签文本更新间隔，单位：分钟 */

#define _HEADER_MARGIN_RIGHT 30

#define _HEADER_TITLE_WIDTH 240

#define _TITLE_LABEL_Y_OFFSET 20
#define _TITLE_LABEL_X_OFFSET -_HEADER_MARGIN_RIGHT
#define _ANIM_DURATION 350

#define _BACK_BTN_MARGIN_LEFT 20

#define _ANIM_TITLE_MOVE_DISTANCE 50
#define _ANIM_BACK_BTN_MOVE_DISTANCE _ANIM_TITLE_MOVE_DISTANCE

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

/**
 * @brief 应用内上方的头部结构体定义
 */
typedef struct
{
    lv_obj_t *container;
    lv_obj_t *clock_label;
    lv_obj_t *title_label;
    lv_obj_t *back_btn;
    lv_timer_t *clock_timer;
    lv_obj_t *current_scr;
    lv_obj_t *next_scr;
    bool is_anim_entering;
    bool is_title_label_initialized;
} eos_app_header_t;

/* Variables --------------------------------------------------*/
static eos_app_header_t *app_header = NULL;
/* Function Implementations -----------------------------------*/
static void _clock_update_cb(lv_timer_t *timer);

static char *_get_title_str(eos_app_header_title_t *t)
{
    return (t->type == APP_HEADER_TITLE_TYPE_ID) ? current_lang[t->data.id] : t->data.string;
}

static void _free_header_title_data(eos_app_header_title_t *t)
{
    if (!t)
        return;
    if (t->type == APP_HEADER_TITLE_TYPE_STRING && t->data.string)
    {
        eos_free((void *)t->data.string);
        t->data.string = NULL;
    }
}

static void _app_header_lang_changed_cb(lv_event_t *e);

static void _set_title_style(lv_obj_t *label)
{
    lv_obj_add_style(label, eos_theme_get_label_style(), 0);
    lv_obj_set_width(label, _HEADER_TITLE_WIDTH);
    lv_label_set_text(label, "");
    eos_label_set_font_size(label, EOS_FONT_SIZE_LARGE);

    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_set_style_text_color(label, EOS_THEME_PRIMARY_COLOR, 0);
    lv_obj_align(label, LV_ALIGN_RIGHT_MID, _TITLE_LABEL_X_OFFSET, _TITLE_LABEL_Y_OFFSET);
}

static void _set_back_btn_style(lv_obj_t *btn)
{
    lv_obj_set_size(btn, 64, 64);
    lv_obj_set_style_radius(btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(btn, EOS_THEME_SECONDARY_COLOR, 0);
    lv_obj_align(btn, LV_ALIGN_LEFT_MID, _BACK_BTN_MARGIN_LEFT, 0);
}

void eos_app_header_set_title_anim(lv_obj_t *current_scr, lv_obj_t *next_scr, bool is_anim_entering)
{
    EOS_CHECK_PTR_RETURN(current_scr && next_scr);
    if (
        lv_obj_is_valid(current_scr) &&
        lv_obj_has_class(current_scr, &lv_obj_class) &&
        lv_obj_is_valid(next_scr) &&
        lv_obj_has_class(next_scr, &lv_obj_class))
    {
        app_header->current_scr = current_scr;
        app_header->next_scr = next_scr;
        app_header->is_anim_entering = is_anim_entering;
    }
}

void _play_title_changed_anim(void)
{
    EOS_CHECK_PTR_RETURN(app_header && app_header->current_scr && app_header->next_scr);
    if (!(lv_obj_is_valid(app_header->title_label) &&
          lv_obj_has_class(app_header->title_label, &lv_label_class)))
        return;

    lv_obj_t *l = app_header->title_label;
    lv_obj_t *back_btn = app_header->back_btn;
    lv_obj_t *parent = lv_obj_get_parent(l);

    int32_t title_start_x = 0;
    int32_t title_end_x;
    int32_t back_btn_start_x = 0;
    int32_t back_btn_end_x;

    // 确定移动方向
    if (app_header->is_anim_entering)
    {
        // 进入：从右往左滑入
        title_end_x = title_start_x - _ANIM_TITLE_MOVE_DISTANCE; // 向左移动
        back_btn_end_x = back_btn_start_x - _ANIM_BACK_BTN_MOVE_DISTANCE;
    }
    else
    {
        // 退出：从左往右滑入
        title_end_x = title_start_x + _ANIM_TITLE_MOVE_DISTANCE; // 向右移动
        back_btn_end_x = back_btn_start_x + _ANIM_BACK_BTN_MOVE_DISTANCE;
    }

    // 原始按钮从默认位置移动到目标位置
    eos_anim_move_start(l, title_start_x, 0, title_end_x, 0, _ANIM_DURATION, false);
    eos_anim_fade_start(l, LV_OPA_COVER, LV_OPA_TRANSP, _ANIM_DURATION + 1, true);

    // 原始 back_btn 从默认位置移动到目标位置
    eos_anim_move_start(back_btn, back_btn_start_x, 0, back_btn_end_x, 0, _ANIM_DURATION, false);
    eos_anim_fade_start(back_btn, LV_OPA_COVER, LV_OPA_TRANSP, _ANIM_DURATION + 1, true); // 淡出后自动删除

    // 创建新的 title_label 和 back_btn
    lv_obj_t *new_l = lv_label_create(parent);
    _set_title_style(new_l);

    const char *new_title = _get_title_str(lv_obj_get_user_data(app_header->next_scr));
    EOS_LOG_D("New title: %s", new_title);
    lv_label_set_text(new_l, new_title);

    // 创建新的返回按钮
    lv_obj_t *new_back_btn = eos_back_btn_create(parent, false);
    _set_back_btn_style(new_back_btn);

    // 新按钮的起始和结束位置
    int32_t new_title_start_x, new_title_end_x = 0;
    int32_t new_back_btn_start_x, new_back_btn_end_x = 0;

    if (app_header->is_anim_entering)
    {
        // 新按钮从右侧进入
        new_title_start_x = new_title_end_x + _ANIM_TITLE_MOVE_DISTANCE;
        new_back_btn_start_x = new_back_btn_end_x + _ANIM_BACK_BTN_MOVE_DISTANCE;
    }
    else
    {
        // 新按钮从左侧进入
        new_title_start_x = new_title_end_x - _ANIM_TITLE_MOVE_DISTANCE;
        new_back_btn_start_x = new_back_btn_end_x - _ANIM_BACK_BTN_MOVE_DISTANCE;
    }

    // 从起始位置移动到默认位置
    eos_anim_move_start(new_l, new_title_start_x, 0, new_title_end_x, 0, _ANIM_DURATION, false);
    eos_anim_fade_start(new_l, LV_OPA_TRANSP, LV_OPA_COVER, _ANIM_DURATION, false);

    // 从起始位置移动到默认位置
    eos_anim_move_start(new_back_btn, new_back_btn_start_x, 0, new_back_btn_end_x, 0, _ANIM_DURATION, false);
    eos_anim_fade_start(new_back_btn, LV_OPA_TRANSP, LV_OPA_COVER, _ANIM_DURATION, false);

    // 更新指针
    app_header->title_label = new_l;
    app_header->back_btn = new_back_btn;

    app_header->current_scr = NULL;
    app_header->next_scr = NULL;
}

/**
 * @brief 更新LVGL字符串，显示当前时间
 */
static inline void _app_header_update_clock_label(lv_obj_t *label)
{
    eos_datetime_t dt = eos_time_get();
    uint32_t next_ms = (uint32_t)((_HEADER_CLOCK_UPDATE_PERIOD_MINUTES * 60) - dt.sec) * 1000;
    lv_timer_set_period(app_header->clock_timer, next_ms);
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
    eos_app_header_title_t *t = (eos_app_header_title_t *)lv_obj_get_user_data(eos_screen_active());
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
    if (!app_header->is_title_label_initialized)
    {
        lv_label_set_text(app_header->title_label, _get_title_str(t));
        app_header->is_title_label_initialized = true;
    }
    if ((t->type == APP_HEADER_TITLE_TYPE_STRING) && t->data.string)
    {
        EOS_LOG_D("Set String title: %s", t->data.string);
        _play_title_changed_anim();
    }
    else if (t->type == APP_HEADER_TITLE_TYPE_ID && t->data.id < STR_ID_MAX_NUMBER)
    {
        EOS_LOG_D("Set ID title: %s", current_lang[t->data.id]);
        _play_title_changed_anim();
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
    lv_obj_set_style_text_color(app_header->title_label, EOS_THEME_PRIMARY_COLOR, 0);
    eos_event_remove_cb(scr, LV_EVENT_REFRESH, _app_header_lang_changed_cb);
    EOS_LOG_D("Freed t: [%p]", t);
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
    EOS_LOG_D("Hide app header");
    lv_obj_add_flag(app_header->container, LV_OBJ_FLAG_HIDDEN);
}

void eos_app_header_show(void)
{
    EOS_CHECK_PTR_RETURN(app_header);
    EOS_LOG_D("Show app header");
    lv_obj_remove_flag(app_header->container, LV_OBJ_FLAG_HIDDEN);
}

void eos_app_header_bind_screen(lv_obj_t *scr, const char *title)
{
    EOS_CHECK_PTR_RETURN(scr);

    eos_app_header_title_t *old_t = lv_obj_get_user_data(scr);
    if (old_t)
    {
        _free_header_title_data(old_t); // 释放内部字符串
        eos_free(old_t);                // 释放结构体
        lv_obj_set_user_data(scr, NULL);
    }
    eos_app_header_title_t *t = eos_malloc(sizeof(eos_app_header_title_t));
    EOS_CHECK_PTR_RETURN(t);
    EOS_LOG_D("Created t: [%p]", t);
    t->type = APP_HEADER_TITLE_TYPE_STRING;
    t->data.string = eos_strdup(title);
    lv_obj_set_user_data(scr, (void *)t);
    lv_label_set_text(app_header->title_label, title);
    lv_obj_set_style_text_color(app_header->title_label, EOS_THEME_PRIMARY_COLOR, 0);
    // LVGL 会在 screen 加载时触发 LV_EVENT_SCREEN_LOADED
    // 并在 screen 被删除时触发 LV_EVENT_DELETE
    lv_obj_add_event_cb(scr, _screen_loaded_cb, LV_EVENT_SCREEN_LOAD_START, NULL);
    lv_obj_add_event_cb(scr, _screen_delete_cb, LV_EVENT_DELETE, NULL);
}

void eos_app_header_bind_screen_str_id(lv_obj_t *scr, lang_string_id_t id)
{
    EOS_CHECK_PTR_RETURN(scr);

    eos_app_header_title_t *old_t = lv_obj_get_user_data(scr);
    if (old_t)
    {
        _free_header_title_data(old_t); // 释放内部字符串
        eos_free(old_t);                // 释放结构体
        lv_obj_set_user_data(scr, NULL);
    }
    eos_app_header_title_t *t = eos_malloc(sizeof(eos_app_header_title_t));
    EOS_CHECK_PTR_RETURN(t);
    EOS_LOG_D("Created t: [%p]", t);
    t->type = APP_HEADER_TITLE_TYPE_ID;
    t->data.id = id;
    lv_obj_set_user_data(scr, (void *)t);
    lv_label_set_text(app_header->title_label, current_lang[id]);
    lv_obj_set_style_text_color(app_header->title_label, EOS_THEME_PRIMARY_COLOR, 0);
    // LVGL 会在 screen 加载时触发 LV_EVENT_SCREEN_LOADED
    // 并在 screen 被删除时触发 LV_EVENT_DELETE
    lv_obj_add_event_cb(scr, _screen_loaded_cb, LV_EVENT_SCREEN_LOAD_START, NULL);
    lv_obj_add_event_cb(scr, _screen_delete_cb, LV_EVENT_DELETE, NULL);
}

bool eos_app_header_is_visible(void)
{
    EOS_CHECK_PTR_RETURN_VAL(app_header, false);
    return !lv_obj_has_flag(app_header->container, LV_OBJ_FLAG_HIDDEN);
}

void eos_app_header_set_parent(lv_obj_t *parent)
{
    EOS_CHECK_PTR_RETURN(app_header && parent);
    lv_obj_set_parent(app_header->container, parent);
    lv_obj_move_foreground(app_header->container);
}

void eos_app_header_parent_reset(void)
{
    EOS_CHECK_PTR_RETURN(app_header);
    lv_obj_set_parent(app_header->container, lv_layer_sys());
    lv_obj_move_foreground(app_header->container);
}


static void _nav_clean_up_reset_label_cb(lv_event_t *e)
{
    lv_label_set_text(app_header->title_label, "");
    app_header->is_title_label_initialized = false;
}

void eos_app_header_set_title_color_once(lv_color_t title_text_color)
{
    EOS_CHECK_PTR_RETURN(app_header && app_header->title_label);
    if (lv_obj_is_valid(app_header->title_label))
        lv_obj_set_style_text_color(app_header->title_label, title_text_color, 0);
}

void eos_app_header_init(void)
{
    EOS_LOG_D("Init eos_app_header");
    app_header = eos_malloc_zeroed(sizeof(eos_app_header_t));
    EOS_CHECK_PTR_RETURN_FREE(app_header, app_header);
    app_header->is_title_label_initialized = false;

    static lv_grad_dsc_t grad;
    grad.dir = LV_GRAD_DIR_VER;
    grad.stops_count = 2;
    grad.stops[0].color = lv_color_black();
    grad.stops[0].opa = LV_OPA_90;
    grad.stops[0].frac = 125;

    grad.stops[1].color = lv_color_black();
    grad.stops[1].opa = LV_OPA_TRANSP;
    grad.stops[1].frac = 255;

    // 半透明容器
    app_header->container = lv_obj_create(lv_layer_sys());
    lv_obj_remove_style_all(app_header->container);
    lv_obj_set_size(app_header->container, EOS_DISPLAY_WIDTH, _HEADER_HEIGHT);
    lv_obj_align(app_header->container, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_move_background(app_header->container);
    // eos_img_set_src(app_header->container, EOS_IMG_APP_HEADER_BG);
    lv_obj_set_style_bg_grad(app_header->container, &grad, 0);
    lv_obj_set_style_bg_opa(app_header->container, LV_OPA_COVER, 0);
    lv_obj_remove_flag(app_header->container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(app_header->container, LV_OBJ_FLAG_CLICKABLE);

    lv_coord_t header_h = _HEADER_HEIGHT;
    lv_coord_t header_w = lv_obj_get_width(app_header->container);

    // 返回按钮
    app_header->back_btn = eos_back_btn_create(app_header->container, false);
    _set_back_btn_style(app_header->back_btn);

    // 时间文字
    app_header->clock_label = lv_label_create(app_header->container);
    lv_obj_add_style(app_header->clock_label, eos_theme_get_label_style(), 0);
    app_header->clock_timer = lv_timer_create(_clock_update_cb, _HEADER_CLOCK_UPDATE_PERIOD_MINUTES * 60 * 1000, app_header->clock_label);
    lv_timer_set_repeat_count(app_header->clock_timer, -1);
    _app_header_update_clock_label(app_header->clock_label);
    lv_obj_align(app_header->clock_label, LV_ALIGN_RIGHT_MID, -_HEADER_MARGIN_RIGHT, -20);

    // 标题文字
    app_header->title_label = lv_label_create(app_header->container);
    _set_title_style(app_header->title_label);
    eos_event_add_cb(app_header->title_label, _app_header_lang_changed_cb, LV_EVENT_REFRESH, NULL);

    // 默认隐藏 app_header
    lv_obj_add_flag(app_header->container, LV_OBJ_FLAG_HIDDEN);

    eos_event_add_global_cb(_nav_clean_up_reset_label_cb, EOS_EVENT_NAVIGATION_CLEAN_UP, NULL);
}
