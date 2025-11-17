/**
 * @file elena_os_toast.c
 * @brief 临时消息提示框
 * @author Sab1e
 * @date 2025-10-24
 */

#include "elena_os_toast.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "elena_os_theme.h"
#include "elena_os_img.h"
#include "elena_os_config.h"
#include "elena_os_anim.h"
#define EOS_LOG_DISABLE
#define EOS_LOG_TAG "Toast"
#include "elena_os_log.h"

/* Macros and Definitions -------------------------------------*/
#define _TOAST_PAD_ALL 12

#define _ICON_WIDTH 44
#define _ICON_HEIGHT _ICON_WIDTH

#define _LABEL_MARGIN_LEFT 12
#define _SCREEN_PAD_ALL 10
#define _LABEL_MAX_WIDTH     \
    EOS_DISPLAY_WIDTH -      \
        _ICON_WIDTH -        \
        _TOAST_PAD_ALL * 2 - \
        _LABEL_MARGIN_LEFT - \
        _SCREEN_PAD_ALL * 2

#define _TOAST_ANIM_DURATION 500
#define _TOAST_MARGIN_TOP 35
#define _TOAST_SHOW_SCROLL_SPEED 10         /**< 每像素滚动时间，单位毫秒 */
#define _TOAST_SHOW_DURATION 3000           /**< 基础显示时间，单位毫秒 */
#define _TOAST_SHOW_ANIM_BASE_DURATION 1000 /**< 此持续时间是用于滚动结束后持续的时间 */

#define _TOAST_LABEL_MAX_LENGTH 128
/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

static void _toast_timer_cb(lv_timer_t *t)
{
    lv_obj_t *toast = lv_timer_get_user_data(t);
    EOS_CHECK_PTR_RETURN(toast);
    eos_anim_move_start(toast, 0, _TOAST_MARGIN_TOP, 0, -lv_obj_get_height(toast), _TOAST_ANIM_DURATION, true);
    lv_timer_del(t);
}

static void _anim_move_end_cb(eos_anim_t *a)
{
    lv_obj_t *label = (lv_obj_t *)eos_anim_get_user_data(a);
    if (label)
    {
        lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    }
}

lv_obj_t *eos_toast_show(const char *icon_src, const char *message)
{
    // 创建 Toast 容器
    lv_obj_t *toast = lv_button_create(lv_layer_top());
    lv_obj_set_size(toast, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_color(toast, EOS_COLOR_DARK_GREY_2, 0);
    lv_obj_set_style_bg_opa(toast, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(toast, _TOAST_PAD_ALL, 0);
    lv_obj_set_style_radius(toast, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_flex_flow(toast, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(toast, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(toast, 8, 0);

    // 创建 icon
    lv_obj_t *icon = lv_image_create(toast);
    eos_img_set_src(icon, icon_src);
    eos_img_set_size(icon, _ICON_WIDTH, _ICON_HEIGHT);

    // 创建 mask
    lv_obj_t *mask = lv_obj_create(toast);
    lv_obj_remove_style_all(mask);
    lv_obj_set_style_clip_corner(mask, true, 0);
    lv_obj_set_size(mask, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(mask, LV_OPA_TRANSP, 0);
    lv_obj_set_style_translate_y(mask, 2, 0);
    lv_obj_set_style_margin_right(mask, 5, 0);

    // 创建 label
    lv_obj_t *label = lv_label_create(mask);
    lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_color(label, EOS_COLOR_WHITE, 0);

    // 测量文字
    lv_point_t size;
    lv_txt_get_size(&size, message, lv_obj_get_style_text_font(label, 0), 0, 0, LV_COORD_MAX, LV_TEXT_FLAG_NONE);
    lv_coord_t text_width = LV_MIN(size.x, _LABEL_MAX_WIDTH);

    lv_obj_add_flag(icon, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_flag(mask, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_flag(label, LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_obj_set_width(label, text_width);
    lv_label_set_text(label, message);

    // 设置滚动动画持续时间
    bool need_scroll = size.x > _LABEL_MAX_WIDTH;
    if (need_scroll)
    {
        uint32_t scroll_time = size.x * _TOAST_SHOW_SCROLL_SPEED;
        lv_obj_set_style_anim_duration(label, scroll_time, 0);
        lv_obj_set_style_radius(mask, LV_RADIUS_CIRCLE, 0);
    }

    // 顶部居中
    lv_obj_align(toast, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_update_layout(toast);

    // 动画移入屏幕
    eos_anim_t *a = eos_anim_move_create(toast, 0, -lv_obj_get_height(toast), 0, _TOAST_MARGIN_TOP, _TOAST_ANIM_DURATION, false);

    eos_anim_add_cb(a, _anim_move_end_cb, label);
    eos_anim_start(a);

    // 定时器时长计算
    uint32_t delay = 0;
    if (need_scroll)
    {
        // 获取 label 滚动动画的持续时间
        uint32_t anim_dur = lv_obj_get_style_anim_duration(label, 0);
        delay = _TOAST_SHOW_ANIM_BASE_DURATION + anim_dur; // 滚动完后再加上停留时间
    }
    else
    {
        delay = _TOAST_SHOW_DURATION;
    }
    EOS_LOG_D("Toast will show for %d ms", delay);
    lv_timer_create(_toast_timer_cb, delay, toast);
    return toast;
}

lv_obj_t *eos_toast_show_fmt(const char *icon_src, const char *fmt, ...)
{
    char buf[_TOAST_LABEL_MAX_LENGTH];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    return eos_toast_show(icon_src, buf);
}
