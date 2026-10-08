/**
 * @file eos_app_header.c
 * @brief Application top navigation header
 */

#include "eos_app_header.h"
#include "eos_config.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#define EOS_LOG_TAG "AppHeader"
#include "eos_log.h"
#include "eos_core.h"
#include "eos_port.h"
#include "eos_lang.h"
#include "eos_image.h"
#include "eos_theme.h"
#include "eos_font.h"
#include "eos_basic_widgets.h"
#include "eos_mem.h"
#include "eos_activity.h"
#include "eos_service_time.h"
#include "eos_event.h"
#include "eos_overlay_layer.h"
#include "eos_service_cache.h"
#include "eos_anim.h"

/* Macros and Definitions -------------------------------------*/
#define _HEADER_HEIGHT EOS_APP_HEADER_STANDARD_HEIGHT
#define _MINI_HEADER_HEIGHT EOS_APP_HEADER_MINI_HEIGHT
#define _MINI_HEADER_GRADIENT_HEIGHT _MINI_HEADER_HEIGHT
#define _HEADER_CLOCK_UPDATE_PERIOD_MINUTES 1 /**< Clock label text update interval in minutes */

#define _HEADER_MARGIN_RIGHT 30

#define _HEADER_TITLE_WIDTH 240

#define _TITLE_LABEL_Y_OFFSET 20
#define _TITLE_LABEL_X_OFFSET -_HEADER_MARGIN_RIGHT
#define _ANIM_DURATION EOS_VIEW_SWITCH_DURATION

#define _BACK_BTN_MARGIN_LEFT 20
#define _MINI_BACK_BTN_SIZE 44
#define _MINI_TITLE_GAP 4
#define _MINI_CLOCK_WIDTH 66
#define _MINI_CLOCK_GAP 8
#define _MINI_EDGE_INSET 36

#define _ANIM_TITLE_MOVE_DISTANCE 50
#define _ANIM_BACK_BTN_MOVE_DISTANCE _ANIM_TITLE_MOVE_DISTANCE

/**
 * @brief Application top header structure definition
 */
typedef struct
{
    lv_obj_t *container;
    lv_obj_t *clock_label;
    lv_obj_t *title_label;
    lv_obj_t *back_btn;
    lv_timer_t *clock_timer;
    lv_obj_t *original_parent; // Original parent object for restoration
    lv_obj_t *old_fading_title; // Old title label pending cleanup
    lv_obj_t *old_fading_back_btn; // Old back button pending cleanup
    bool is_anim_entering; // Animation direction
    bool attached_to_view; // Whether attached to View
    const lv_font_t *clock_font;
    lv_image_dsc_t *grad_bg_img; // Pre-rendered gradient background (ARGB8888 full-size)
    lv_image_dsc_t *mini_grad_bg_img; // Pre-rendered gradient background (ARGB8888 Mini row)
} eos_app_header_t;

/* Variables --------------------------------------------------*/
static eos_app_header_t *app_header = NULL;
/* Function Implementations -----------------------------------*/
static void _clock_update_cb(lv_timer_t *timer);

static void _app_header_fade_out_ready_cb(eos_anim_t *a)
{
    lv_obj_t *obj = a->tar_obj;
    if (!obj || !lv_obj_is_valid(obj))
        return;

    lv_obj_set_hidden(obj, true);
    lv_obj_set_style_opa(obj, LV_OPA_COVER, 0);
}

static eos_app_header_style_t _app_header_effective_style(eos_activity_t *activity)
{
    if (!activity || eos_activity_is_app_header_time_only(activity))
        return EOS_APP_HEADER_STYLE_STANDARD;

    return eos_activity_get_app_header_style(activity);
}

static bool _app_header_back_button_visible(eos_activity_t *activity)
{
    if (!activity)
        return true;

    return !eos_activity_is_app_header_time_only(activity) && eos_activity_is_app_header_back_button_visible(activity);
}

static void _app_header_layout_title(lv_obj_t *label, eos_app_header_style_t style, bool back_button_visible)
{
    if (!label || !lv_obj_is_valid(label))
        return;

    if (style == EOS_APP_HEADER_STYLE_STANDARD)
    {
        lv_obj_set_width(label, _HEADER_TITLE_WIDTH);
        eos_label_set_font_size(label, EOS_FONT_SIZE_LARGE);
        lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_align(label, LV_ALIGN_RIGHT_MID, _TITLE_LABEL_X_OFFSET, _TITLE_LABEL_Y_OFFSET);
        return;
    }

    lv_coord_t container_width = lv_obj_get_width(app_header->container);
    lv_coord_t left = _MINI_EDGE_INSET;
    if (back_button_visible)
        left += _MINI_BACK_BTN_SIZE + _MINI_TITLE_GAP;

    lv_coord_t right = _MINI_EDGE_INSET + _MINI_CLOCK_WIDTH + _MINI_CLOCK_GAP;
    lv_coord_t width = container_width - left - right;
    if (width < 0)
        width = 0;

    lv_obj_set_width(label, width);
    eos_label_set_font_size(label, EOS_FONT_SIZE_SMALL);
    lv_label_set_long_mode(label, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
    const lv_font_t *font = lv_obj_get_style_text_font(label, LV_PART_MAIN);
    lv_coord_t line_height = font ? lv_font_get_line_height(font) : _MINI_HEADER_HEIGHT;
    lv_coord_t top = (_MINI_HEADER_HEIGHT - line_height) / 2;
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, left, top);
}

static void _app_header_layout_clock(eos_app_header_style_t style)
{
    if (!app_header->clock_label || !lv_obj_is_valid(app_header->clock_label))
        return;

    if (style == EOS_APP_HEADER_STYLE_STANDARD)
    {
        if (app_header->clock_font)
            lv_obj_set_style_text_font(app_header->clock_label, app_header->clock_font, 0);
        lv_obj_set_width(app_header->clock_label, LV_SIZE_CONTENT);
        lv_obj_align(app_header->clock_label, LV_ALIGN_RIGHT_MID, -_HEADER_MARGIN_RIGHT, -20);
        return;
    }

    eos_label_set_font_size(app_header->clock_label, EOS_FONT_SIZE_SMALL);
    lv_obj_set_width(app_header->clock_label, _MINI_CLOCK_WIDTH);
    lv_obj_set_style_text_align(app_header->clock_label, LV_TEXT_ALIGN_RIGHT, 0);
    const lv_font_t *font = lv_obj_get_style_text_font(app_header->clock_label, LV_PART_MAIN);
    lv_coord_t line_height = font ? lv_font_get_line_height(font) : _MINI_HEADER_HEIGHT;
    lv_coord_t top = (_MINI_HEADER_HEIGHT - line_height) / 2;
    lv_obj_align(app_header->clock_label, LV_ALIGN_TOP_RIGHT, -_MINI_EDGE_INSET, top);
}

static void _app_header_layout_back_button(lv_obj_t *button, eos_app_header_style_t style)
{
    if (!button || !lv_obj_is_valid(button))
        return;

    if (style == EOS_APP_HEADER_STYLE_STANDARD)
    {
        lv_obj_set_size(button, 64, 64);
        lv_obj_set_style_radius(button, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(button, EOS_THEME_SECONDARY_COLOR, 0);
        lv_obj_set_style_bg_opa(button, LV_OPA_COVER, 0);
        lv_obj_align(button, LV_ALIGN_LEFT_MID, _BACK_BTN_MARGIN_LEFT, 0);
        return;
    }

    lv_obj_set_size(button, _MINI_BACK_BTN_SIZE, _MINI_BACK_BTN_SIZE);
    lv_obj_set_style_radius(button, 0, 0);
    lv_obj_set_style_bg_opa(button, LV_OPA_TRANSP, 0);
    lv_obj_set_style_bg_opa(button, LV_OPA_TRANSP, LV_STATE_PRESSED);
    lv_obj_align(button, LV_ALIGN_TOP_LEFT, _MINI_EDGE_INSET, (_MINI_HEADER_HEIGHT - _MINI_BACK_BTN_SIZE) / 2);
}

static void _app_header_apply_activity_mode(eos_activity_t *activity)
{
    EOS_CHECK_PTR_RETURN(app_header);

    bool time_only = false;
    lv_color_t clock_color = EOS_COLOR_WHITE;
    if (activity)
    {
        time_only = eos_activity_is_app_header_time_only(activity);
        if (time_only)
        {
            clock_color = eos_activity_get_app_header_time_only_text_color(activity);
        }
    }

    eos_app_header_style_t style = _app_header_effective_style(activity);
    bool back_button_visible = _app_header_back_button_visible(activity);

    if (app_header->container && lv_obj_is_valid(app_header->container))
    {
        lv_coord_t height = style == EOS_APP_HEADER_STYLE_MINI ? _MINI_HEADER_GRADIENT_HEIGHT : _HEADER_HEIGHT;
        if (lv_obj_get_height(app_header->container) != height)
            lv_obj_set_height(app_header->container, height);

        if (style == EOS_APP_HEADER_STYLE_MINI)
        {
            lv_obj_set_style_bg_grad(app_header->container, NULL, 0);
            lv_obj_set_style_bg_image_src(app_header->container, app_header->mini_grad_bg_img, 0);
            lv_obj_set_style_bg_image_opa(app_header->container, LV_OPA_COVER, 0);
            lv_obj_set_style_bg_opa(app_header->container, LV_OPA_TRANSP, 0);
        }
        else
        {
            lv_obj_set_style_bg_grad(app_header->container, NULL, 0);
            lv_obj_set_style_bg_image_src(app_header->container, app_header->grad_bg_img, 0);
            lv_obj_set_style_bg_image_opa(app_header->container, time_only ? LV_OPA_TRANSP : LV_OPA_COVER, 0);
            lv_obj_set_style_bg_opa(app_header->container, LV_OPA_TRANSP, 0);
        }
    }

    _app_header_layout_clock(style);
    _app_header_layout_title(app_header->title_label, style, back_button_visible);
    _app_header_layout_back_button(app_header->back_btn, style);

    if (app_header->clock_label && lv_obj_is_valid(app_header->clock_label))
    {
        lv_obj_set_style_text_color(app_header->clock_label, clock_color, 0);
        lv_obj_set_hidden(app_header->clock_label, false);
    }

    if (app_header->title_label && lv_obj_is_valid(app_header->title_label))
    {
        if (time_only)
            lv_obj_set_hidden(app_header->title_label, true);
        else
            lv_obj_set_hidden(app_header->title_label, false);
    }

    if (app_header->back_btn && lv_obj_is_valid(app_header->back_btn))
    {
        if (!back_button_visible)
            lv_obj_set_hidden(app_header->back_btn, true);
        else
            lv_obj_set_hidden(app_header->back_btn, false);
    }
}

static bool _app_header_can_reparent(lv_obj_t *new_parent)
{
    if (!app_header)
    {
        return false;
    }

    if (!app_header->container || !lv_obj_is_valid(app_header->container))
    {
        app_header->attached_to_view = false;
        EOS_LOG_E("AppHeader container is invalid");
        return false;
    }

    if (!new_parent || !lv_obj_is_valid(new_parent))
    {
        EOS_LOG_E("AppHeader target parent is invalid");
        return false;
    }

    return true;
}

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
    _app_header_layout_back_button(btn, EOS_APP_HEADER_STYLE_STANDARD);
}

void _play_title_changed_anim(eos_activity_t *from,
                              eos_activity_t *to,
                              bool need_anim,
                              bool reverse_anim,
                              eos_anim_group_t *group)
{
    EOS_CHECK_PTR_RETURN(app_header);
    if (!(lv_obj_is_valid(app_header->title_label) && lv_obj_has_class(app_header->title_label, &lv_label_class)))
        return;

    bool from_time_only = from ? eos_activity_is_app_header_time_only(from) : false;
    bool to_time_only = to ? eos_activity_is_app_header_time_only(to) : false;
    eos_app_header_style_t from_style = _app_header_effective_style(from);
    eos_app_header_style_t to_style = _app_header_effective_style(to);
    bool from_back_visible = _app_header_back_button_visible(from);
    bool to_back_visible = _app_header_back_button_visible(to);

    if (from_time_only || to_time_only || from_style != to_style || from_back_visible != to_back_visible)
    {
        need_anim = false;
    }

    if (!need_anim || !group)
    {
        const char *new_title = eos_activity_get_title(to);
        EOS_LOG_D("New title: %s", new_title);
        lv_label_set_text(app_header->title_label, new_title ? new_title : "");

        lv_color_t color = eos_activity_get_title_color(to);
        lv_obj_set_style_text_color(app_header->title_label, color, 0);
        _app_header_apply_activity_mode(to);
        return;
    }

    if (app_header->old_fading_title && lv_obj_is_valid(app_header->old_fading_title))
    {
        lv_obj_delete(app_header->old_fading_title);
    }
    if (app_header->old_fading_back_btn && lv_obj_is_valid(app_header->old_fading_back_btn))
    {
        lv_obj_delete(app_header->old_fading_back_btn);
    }
    app_header->old_fading_title = NULL;
    app_header->old_fading_back_btn = NULL;

    if (reverse_anim)
    {
        app_header->is_anim_entering = false;
    }
    else
    {
        app_header->is_anim_entering = true;
    }

    lv_obj_t *l = app_header->title_label;
    lv_obj_t *back_btn = app_header->back_btn;
    lv_obj_t *parent = lv_obj_get_parent(l);

    int32_t title_start_x = 0;
    int32_t title_end_x;
    int32_t back_btn_start_x = 0;
    int32_t back_btn_end_x;

    if (app_header->is_anim_entering)
    {
        title_end_x = title_start_x - _ANIM_TITLE_MOVE_DISTANCE;
        back_btn_end_x = back_btn_start_x - _ANIM_BACK_BTN_MOVE_DISTANCE;
    }
    else
    {
        title_end_x = title_start_x + _ANIM_TITLE_MOVE_DISTANCE;
        back_btn_end_x = back_btn_start_x + _ANIM_BACK_BTN_MOVE_DISTANCE;
    }

    // Old title slide out + fade out
    eos_anim_t *anim = eos_anim_move_create(l, title_start_x, 0, title_end_x, 0, _ANIM_DURATION, false);
    if (anim)
    {
        eos_anim_set_path(anim, lv_anim_path_ease_in_out);
        eos_anim_group_attach(anim, group);
        eos_anim_start(anim);
    }

    anim = eos_anim_fade_create(l, LV_OPA_COVER, LV_OPA_TRANSP, _ANIM_DURATION + 1, false);
    if (anim)
    {
        eos_anim_fade_set_layered(anim, true);
        eos_anim_group_attach(anim, group);
        eos_anim_start(anim);
    }

    // Old back btn slide out + fade out
    anim = eos_anim_move_create(back_btn, back_btn_start_x, 0, back_btn_end_x, 0, _ANIM_DURATION, false);
    if (anim)
    {
        eos_anim_set_path(anim, lv_anim_path_ease_in_out);
        eos_anim_group_attach(anim, group);
        eos_anim_start(anim);
    }

    anim = eos_anim_fade_create(back_btn, LV_OPA_COVER, LV_OPA_TRANSP, _ANIM_DURATION + 1, false);
    if (anim)
    {
        eos_anim_fade_set_layered(anim, true);
        eos_anim_group_attach(anim, group);
        eos_anim_start(anim);
    }

    app_header->old_fading_title = l;
    app_header->old_fading_back_btn = back_btn;

    lv_obj_t *new_l = lv_label_create(parent);
    _set_title_style(new_l);
    _app_header_layout_title(new_l, to_style, to_back_visible);

    const char *new_title = eos_activity_get_title(to);
    EOS_LOG_D("New title: %s", new_title);
    lv_label_set_text(new_l, new_title ? new_title : "");

    lv_color_t color = eos_activity_get_title_color(to);
    lv_obj_set_style_text_color(new_l, color, 0);

    lv_obj_t *new_back_btn = eos_back_btn_create(parent, false);
    _app_header_layout_back_button(new_back_btn, to_style);
    if (!to_back_visible)
        lv_obj_set_hidden(new_back_btn, true);

    int32_t new_title_start_x, new_title_end_x = 0;
    int32_t new_back_btn_start_x, new_back_btn_end_x = 0;

    if (app_header->is_anim_entering)
    {
        new_title_start_x = new_title_end_x + _ANIM_TITLE_MOVE_DISTANCE;
        new_back_btn_start_x = new_back_btn_end_x + _ANIM_BACK_BTN_MOVE_DISTANCE;
    }
    else
    {
        new_title_start_x = new_title_end_x - _ANIM_TITLE_MOVE_DISTANCE;
        new_back_btn_start_x = new_back_btn_end_x - _ANIM_BACK_BTN_MOVE_DISTANCE;
    }

    lv_obj_set_style_translate_x(new_l, new_title_start_x, 0);
    lv_obj_set_style_opa_layered(new_l, LV_OPA_TRANSP, 0);
    lv_obj_set_style_translate_x(new_back_btn, new_back_btn_start_x, 0);
    lv_obj_set_style_opa_layered(new_back_btn, LV_OPA_TRANSP, 0);

    // New title slide in + fade in
    anim = eos_anim_move_create(new_l, new_title_start_x, 0, new_title_end_x, 0, _ANIM_DURATION, false);
    if (anim)
    {
        eos_anim_set_path(anim, lv_anim_path_ease_in_out);
        eos_anim_group_attach(anim, group);
        eos_anim_start(anim);
    }

    anim = eos_anim_fade_create(new_l, LV_OPA_TRANSP, LV_OPA_COVER, _ANIM_DURATION, false);
    if (anim)
    {
        eos_anim_fade_set_layered(anim, true);
        eos_anim_group_attach(anim, group);
        eos_anim_start(anim);
    }

    // New back btn slide in + fade in
    anim = eos_anim_move_create(new_back_btn, new_back_btn_start_x, 0, new_back_btn_end_x, 0, _ANIM_DURATION, false);
    if (anim)
    {
        eos_anim_set_path(anim, lv_anim_path_ease_in_out);
        eos_anim_group_attach(anim, group);
        eos_anim_start(anim);
    }

    anim = eos_anim_fade_create(new_back_btn, LV_OPA_TRANSP, LV_OPA_COVER, _ANIM_DURATION, false);
    if (anim)
    {
        eos_anim_fade_set_layered(anim, true);
        eos_anim_group_attach(anim, group);
        eos_anim_start(anim);
    }

    app_header->title_label = new_l;
    app_header->back_btn = new_back_btn;
    _app_header_apply_activity_mode(to);
}

/**
 * @brief Update LVGL string to display current time
 */
static inline void _app_header_update_clock_label(lv_obj_t *label)
{
    eos_datetime_t dt = eos_time_get();
    uint32_t next_ms = (uint32_t)((_HEADER_CLOCK_UPDATE_PERIOD_MINUTES * 60) - dt.sec) * 1000;
    lv_timer_set_period(app_header->clock_timer, next_ms);
    lv_label_set_text_fmt(label, "%02d:%02d", dt.hour, dt.min);
}

/**
 * @brief Time refresh callback, triggered by LVGL timer
 */
static void _clock_update_cb(lv_timer_t *timer)
{
    lv_obj_t *label = lv_timer_get_user_data(timer);
    EOS_CHECK_PTR_RETURN(app_header && label);
    if (lv_obj_is_hidden(app_header->container))
    {
        return;
    }
    // Update display text
    _app_header_update_clock_label(label);
}

void eos_app_header_hide(void)
{
    EOS_CHECK_PTR_RETURN(app_header);
    EOS_LOG_D("Hide app header");
    // If attached to a View, restore the parent object first
    if (app_header->attached_to_view)
    {
        lv_obj_t *restore_parent = app_header->original_parent;
        if (!restore_parent || !lv_obj_is_valid(restore_parent))
        {
            restore_parent = eos_overlay_layer_get(EOS_TOP_LAYER_APP_HEADER);
            app_header->original_parent = restore_parent;
        }

        if (_app_header_can_reparent(restore_parent))
        {
            lv_obj_set_parent(app_header->container, restore_parent);
        }
        app_header->attached_to_view = false;
    }

    if (app_header->container && lv_obj_is_valid(app_header->container))
    {
        lv_obj_set_hidden(app_header->container, true);
    }
}

void eos_app_header_show(eos_activity_t *a)
{
    EOS_CHECK_PTR_RETURN(app_header);

    // Check if it is a Watchface Activity; if so, hide AppHeader
    eos_activity_t *target_activity = a;
    if (!target_activity)
        target_activity = eos_activity_get_current();

    if (target_activity && target_activity == eos_activity_get_watchface())
    {
        EOS_LOG_D("Skip showing app header for watchface activity");
        return;
    }

    EOS_LOG_D("Show app header");
    if (!(app_header->container && lv_obj_is_valid(app_header->container)))
    {
        app_header->attached_to_view = false;
        EOS_LOG_E("AppHeader container is invalid");
        return;
    }

    if (app_header->attached_to_view)
    {
        lv_obj_t *restore_parent = app_header->original_parent;
        if (!restore_parent || !lv_obj_is_valid(restore_parent))
        {
            restore_parent = eos_overlay_layer_get(EOS_TOP_LAYER_APP_HEADER);
            app_header->original_parent = restore_parent;
        }

        if (_app_header_can_reparent(restore_parent))
        {
            lv_obj_set_parent(app_header->container, restore_parent);
        }
        app_header->attached_to_view = false;
    }
    // Get title text from current Activity
    const char *title = NULL;
    if (a)
        title = eos_activity_get_title(a);
    else
        title = eos_activity_get_title(eos_activity_get_current());
    if (title)
        lv_label_set_text(app_header->title_label, title);
    else
        lv_label_set_text(app_header->title_label, "");
    // Get title color from current Activity
    lv_color_t color = EOS_COLOR_WHITE;
    if (a)
        color = eos_activity_get_title_color(a);
    else
        color = eos_activity_get_title_color(eos_activity_get_current());
    if (lv_obj_is_valid(app_header->title_label))
        lv_obj_set_style_text_color(app_header->title_label, color, 0);

    _app_header_apply_activity_mode(target_activity);
    _app_header_update_clock_label(app_header->clock_label);
    lv_obj_set_hidden(app_header->container, false);
}

void eos_app_header_set_visible_animated(eos_activity_t *a, bool visible, uint32_t duration_ms)
{
    EOS_CHECK_PTR_RETURN(app_header);

    if (!(app_header->container && lv_obj_is_valid(app_header->container)))
        return;

    if (duration_ms == 0)
    {
        if (visible)
            eos_app_header_show(a);
        else
            eos_app_header_hide();
        return;
    }

    lv_obj_t *container = app_header->container;
    eos_anim_t *anim = eos_anim_fade_create(container,
                                            visible ? LV_OPA_TRANSP : LV_OPA_COVER,
                                            visible ? LV_OPA_COVER : LV_OPA_TRANSP,
                                            duration_ms,
                                            false);
    if (!anim)
        return;

    eos_anim_fade_set_main_opa(anim, true);

    if (visible)
    {
        eos_app_header_show(a);
        lv_obj_set_style_opa(container, LV_OPA_TRANSP, 0);
        eos_anim_start(anim);
    }
    else
    {
        if (lv_obj_is_hidden(container))
        {
            eos_anim_del(anim);
            return;
        }
        lv_obj_set_style_opa(container, LV_OPA_COVER, 0);
        eos_anim_add_cb(anim, _app_header_fade_out_ready_cb, NULL);
        eos_anim_start(anim);
    }
}

static void _app_header_slide_hide_ready_cb(eos_anim_t *a)
{
    lv_obj_t *container = a->tar_obj;
    if (!container || !lv_obj_is_valid(container))
    {
        return;
    }

    lv_obj_set_hidden(container, true);
    lv_obj_set_style_translate_y(container, 0, 0);
}

void eos_app_header_slide_visible_animated(eos_activity_t *a, bool visible, uint32_t duration_ms)
{
    EOS_CHECK_PTR_RETURN(app_header);

    if (!(app_header->container && lv_obj_is_valid(app_header->container)))
    {
        return;
    }

    if (duration_ms == 0)
    {
        if (visible)
        {
            eos_app_header_show(a);
        }
        else
        {
            eos_app_header_hide();
        }
        return;
    }

    lv_obj_t *container = app_header->container;

    if (visible)
        eos_app_header_show(a);

    int32_t header_height = lv_obj_get_height(container);
    if (header_height <= 0)
    {
        header_height = _HEADER_HEIGHT;
    }

    int32_t container_x = lv_obj_get_x(container);
    eos_anim_t *anim = eos_anim_move_create(container,
                                            container_x,
                                            visible ? -header_height : 0,
                                            container_x,
                                            visible ? 0 : -header_height,
                                            duration_ms,
                                            false);
    if (!anim)
        return;

    if (visible)
    {
        lv_obj_set_hidden(container, false);
        lv_obj_set_style_translate_y(container, -header_height, 0);
        eos_anim_start(anim);
    }
    else
    {
        if (lv_obj_is_hidden(container))
        {
            eos_anim_del(anim);
            return;
        }
        lv_obj_set_style_translate_y(container, 0, 0);
        eos_anim_add_cb(anim, _app_header_slide_hide_ready_cb, NULL);
        eos_anim_start(anim);
    }
}
/**
 * @brief Attach app header to specified View
 * @param view View to attach to
 */
void eos_app_header_attach_to_view(lv_obj_t *view)
{
    EOS_CHECK_PTR_RETURN(app_header && view);

    if (!_app_header_can_reparent(view))
    {
        app_header->attached_to_view = false;
        return;
    }

    if (app_header->attached_to_view)
    {
        lv_obj_t *cur_parent = lv_obj_get_parent(app_header->container);
        if (cur_parent == view)
        {
            return;
        }
    }

    if (!app_header->original_parent || !lv_obj_is_valid(app_header->original_parent))
    {
        app_header->original_parent = eos_overlay_layer_get(EOS_TOP_LAYER_APP_HEADER);
    }

    lv_obj_set_parent(app_header->container, view);
    app_header->attached_to_view = true;
}

/**
 * @brief Detach app header from View, restore to original parent object
 */
void eos_app_header_detach_from_view(void)
{
    EOS_CHECK_PTR_RETURN(app_header);
    if (!app_header->attached_to_view)
    {
        return;
    }

    lv_obj_t *restore_parent = app_header->original_parent;
    if (!restore_parent || !lv_obj_is_valid(restore_parent))
    {
        restore_parent = eos_overlay_layer_get(EOS_TOP_LAYER_APP_HEADER);
        app_header->original_parent = restore_parent;
    }

    if (_app_header_can_reparent(restore_parent))
    {
        lv_obj_set_parent(app_header->container, restore_parent);
    }
    app_header->attached_to_view = false;
}

bool eos_app_header_is_attached_to_view(void)
{
    EOS_CHECK_PTR_RETURN_VAL(app_header, false);
    return app_header->attached_to_view;
}

bool eos_app_header_is_visible(void)
{
    EOS_CHECK_PTR_RETURN_VAL(app_header, false);
    return !lv_obj_is_hidden(app_header->container);
}

static void _update_title_label(lv_event_t *e)
{
    const char *title = eos_activity_get_title(eos_activity_get_current());
    if (title)
        lv_label_set_text(app_header->title_label, title);
    else
        lv_label_set_text(app_header->title_label, "");
}

static lv_image_dsc_t *_create_gradient_bg(lv_coord_t img_h, bool fade_from_top)
{
    const lv_coord_t img_w = EOS_DISPLAY_WIDTH;
    const uint32_t cf_bytes = 4;
    const uint32_t buf_size = (uint32_t)img_w * img_h * cf_bytes;

    uint8_t *buf = eos_cache_buf_alloc(buf_size);
    if (!buf)
    {
        EOS_LOG_E("Failed to allocate gradient bg buffer");
        return NULL;
    }

    for (lv_coord_t y = 0; y < img_h; y++)
    {
        int frac = y * 255 / (img_h - 1);
        uint8_t opa;
        if (fade_from_top)
            opa = LV_OPA_90 - (uint8_t)((uint32_t)LV_OPA_90 * frac / 255);
        else if (frac <= 125)
            opa = LV_OPA_90;
        else
            opa = LV_OPA_90 - (uint8_t)((uint32_t)LV_OPA_90 * (frac - 125) / (255 - 125));
        for (lv_coord_t x = 0; x < img_w; x++)
        {
            uint8_t *p = &buf[(y * img_w + x) * cf_bytes];
            p[0] = 0x00; // B
            p[1] = 0x00; // G
            p[2] = 0x00; // R
            p[3] = opa; // A
        }
    }

    lv_image_dsc_t *dsc = eos_malloc_zeroed(sizeof(lv_image_dsc_t));
    if (!dsc)
    {
        eos_cache_buf_free(buf);
        return NULL;
    }

    dsc->header.magic = LV_IMAGE_HEADER_MAGIC;
    dsc->header.cf = LV_COLOR_FORMAT_ARGB8888;
    dsc->header.w = img_w;
    dsc->header.h = img_h;
    dsc->header.stride = img_w * cf_bytes;
    dsc->data = buf;
    dsc->data_size = buf_size;

    return dsc;
}

static void _free_gradient_bg(lv_image_dsc_t *dsc)
{
    if (!dsc)
        return;

    if (dsc->data)
        eos_cache_buf_free((void *)dsc->data);
    eos_free(dsc);
}

static void _grad_bg_img_delete_cb(lv_event_t *e)
{
    lv_image_dsc_t *dsc = lv_event_get_user_data(e);
    _free_gradient_bg(dsc);
}

void eos_app_header_init(void)
{
    EOS_LOG_D("Init eos_app_header");
    app_header = eos_malloc_zeroed(sizeof(eos_app_header_t));
    EOS_CHECK_PTR_RETURN_FREE(app_header, app_header);

    app_header->grad_bg_img = _create_gradient_bg(_HEADER_HEIGHT, false);
    if (!app_header->grad_bg_img)
    {
        eos_free(app_header);
        app_header = NULL;
        return;
    }

    app_header->mini_grad_bg_img = _create_gradient_bg(_MINI_HEADER_HEIGHT, true);
    if (!app_header->mini_grad_bg_img)
    {
        _free_gradient_bg(app_header->grad_bg_img);
        eos_free(app_header);
        app_header = NULL;
        return;
    }

    // Semi-transparent container
    app_header->container = lv_obj_create(eos_overlay_layer_get(EOS_TOP_LAYER_APP_HEADER));
    app_header->original_parent = lv_obj_get_parent(app_header->container); // Save original parent object
    lv_obj_remove_style_all(app_header->container);
    lv_obj_set_size(app_header->container, EOS_DISPLAY_WIDTH, _HEADER_HEIGHT);
    lv_obj_align(app_header->container, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_image_src(app_header->container, app_header->grad_bg_img, 0);
    lv_obj_set_style_bg_image_opa(app_header->container, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_opa(app_header->container, LV_OPA_TRANSP, 0);
    lv_obj_set_scrollable(app_header->container, false);
    lv_obj_set_clickable(app_header->container, false);

    lv_obj_add_event_cb(app_header->container, _grad_bg_img_delete_cb, LV_EVENT_DELETE, app_header->grad_bg_img);
    lv_obj_add_event_cb(app_header->container, _grad_bg_img_delete_cb, LV_EVENT_DELETE, app_header->mini_grad_bg_img);

    // Back button
    app_header->back_btn = eos_back_btn_create(app_header->container, false);
    _set_back_btn_style(app_header->back_btn);

    // Clock label
    app_header->clock_label = lv_label_create(app_header->container);
    lv_obj_add_style(app_header->clock_label, eos_theme_get_label_style(), 0);
    app_header->clock_font = lv_obj_get_style_text_font(app_header->clock_label, LV_PART_MAIN);
    app_header->clock_timer =
        lv_timer_create(_clock_update_cb, _HEADER_CLOCK_UPDATE_PERIOD_MINUTES * 60 * 1000, app_header->clock_label);
    lv_timer_set_repeat_count(app_header->clock_timer, -1);
    _app_header_update_clock_label(app_header->clock_label);
    lv_obj_align(app_header->clock_label, LV_ALIGN_RIGHT_MID, -_HEADER_MARGIN_RIGHT, -20);

    // Title label
    app_header->title_label = lv_label_create(app_header->container);
    _set_title_style(app_header->title_label);
    lv_obj_add_event_cb(app_header->title_label, _update_title_label, LV_EVENT_REFRESH, NULL);

    // Hide app_header by default
    lv_obj_set_hidden(app_header->container, true);

    app_header->is_anim_entering = false;
    app_header->attached_to_view = false;
}
