/**
 * @file elena_os_swipe_panel.c
 * @brief 滑动面板实现
 * @author Sab1e
 * @date 2025-08-10
 */

#include "elena_os_swipe_panel.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#define EOS_LOG_DISABLE
#define EOS_LOG_TAG "SwipePanel"
#include "elena_os_log.h"
#include "elena_os_event.h"
#include "elena_os_config.h"
#include "elena_os_theme.h"
#include "elena_os_port.h"
#include "elena_os_basic_widgets.h"

/* Macros and Definitions -------------------------------------*/
#define GESTURE_AREA_HEIGHT 50
#define TOUCH_BAR_MARGIN 20
#define HANDLE_BAR_WIDTH 80
#define HANDLE_BAR_HEIGHT 10
#define SWIPE_ANIM_DURATION 200

#define DIR_UP_HIDE_TARGET_COORD EOS_DISPLAY_HEIGHT
#define DIR_UP_SHOW_TARGET_COORD 0

#define DIR_DOWN_HIDE_TARGET_COORD -EOS_DISPLAY_HEIGHT
#define DIR_DOWN_SHOW_TARGET_COORD 0

#define DIR_LEFT_HIDE_TARGET_COORD EOS_DISPLAY_WIDTH
#define DIR_LEFT_SHOW_TARGET_COORD 0

#define DIR_RIGHT_HIDE_TARGET_COORD -EOS_DISPLAY_WIDTH
#define DIR_RIGHT_SHOW_TARGET_COORD 0
/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

static void _update_handle_bar_position(eos_swipe_panel_t *sp, eos_swipe_dir_t dir)
{
    EOS_CHECK_PTR_RETURN(sp && sp->handle_bar);
    switch (dir)
    {
    case EOS_SWIPE_DIR_DOWN:
        lv_obj_align(sp->handle_bar, LV_ALIGN_BOTTOM_MID, 0, -TOUCH_BAR_MARGIN); // 底部上方10px
        break;
    case EOS_SWIPE_DIR_UP:
        lv_obj_align(sp->handle_bar, LV_ALIGN_TOP_MID, 0, TOUCH_BAR_MARGIN); // 顶部下方10px
        break;
    case EOS_SWIPE_DIR_LEFT:
        lv_obj_align(sp->handle_bar, LV_ALIGN_LEFT_MID, TOUCH_BAR_MARGIN, 0); // 右侧10px
        break;
    case EOS_SWIPE_DIR_RIGHT:
        lv_obj_align(sp->handle_bar, LV_ALIGN_RIGHT_MID, -TOUCH_BAR_MARGIN, 0); // 左侧10px
        break;
    }

    switch (dir)
    {
    case EOS_SWIPE_DIR_UP:
    case EOS_SWIPE_DIR_DOWN:
        lv_obj_set_size(sp->handle_bar, HANDLE_BAR_WIDTH, HANDLE_BAR_HEIGHT);
        break;
    case EOS_SWIPE_DIR_LEFT:
    case EOS_SWIPE_DIR_RIGHT:
        lv_obj_set_size(sp->handle_bar, HANDLE_BAR_HEIGHT, HANDLE_BAR_WIDTH);
        break;

    default:
        break;
    }
}

void eos_swipe_panel_move(eos_swipe_panel_t *sp, int32_t target, bool anim)
{
    EOS_CHECK_PTR_RETURN(sp);

    lv_coord_t cur;
    if (sp->sw->dir == EOS_SLIDE_DIR_VER)
    {
        cur = lv_obj_get_y(sp->swipe_obj);
    }
    else
    {
        cur = lv_obj_get_x(sp->swipe_obj);
    }

    eos_slide_widget_move(sp->sw, cur, target, anim ? 120 : 0);
}

void eos_swipe_panel_pull_back(eos_swipe_panel_t *sp)
{
    EOS_CHECK_PTR_RETURN(sp);
    int32_t target = 0;
    switch (sp->dir)
    {
    case EOS_SWIPE_DIR_UP:
        target = EOS_DISPLAY_HEIGHT; // 向上拉回时完全隐藏
        break;
    case EOS_SWIPE_DIR_DOWN:
        target = -EOS_DISPLAY_HEIGHT; // 向下拉回时完全隐藏
        break;
    case EOS_SWIPE_DIR_LEFT:
        target = EOS_DISPLAY_WIDTH; // 向左拉回时完全隐藏
        break;
    case EOS_SWIPE_DIR_RIGHT:
        target = -EOS_DISPLAY_WIDTH; // 向右拉回时完全隐藏
        break;
    }
    eos_swipe_panel_move(sp, target, true);
    eos_slide_widget_reverse(sp->sw);
    if (sp->sw->dir == EOS_SLIDE_DIR_VER)
    {
        lv_obj_set_y(sp->sw->touch_obj, sp->sw->touch_obj_base);
    }
    else
    {
        lv_obj_set_x(sp->sw->touch_obj, sp->sw->touch_obj_base);
    }
}

void eos_swipe_panel_hide_handle_bar(eos_swipe_panel_t *sp)
{
    if (!sp || !sp->handle_bar)
        return;

    lv_obj_add_flag(sp->handle_bar, LV_OBJ_FLAG_HIDDEN);
}

void eos_swipe_panel_show_handle_bar(eos_swipe_panel_t *sp)
{
    if (!sp || !sp->handle_bar)
        return;

    lv_obj_remove_flag(sp->handle_bar, LV_OBJ_FLAG_HIDDEN);
}

void eos_swipe_panel_set_dir(eos_swipe_panel_t *sp, const eos_swipe_dir_t dir)
{
    EOS_CHECK_PTR_RETURN(sp);

    sp->dir = dir;

    // 根据方向调整手势区域尺寸
    switch (sp->sw->dir)
    {
    case EOS_SLIDE_DIR_VER:
        lv_obj_set_size(sp->sw->touch_obj, EOS_DISPLAY_WIDTH, GESTURE_AREA_HEIGHT);
        break;
    case EOS_SLIDE_DIR_HOR:
        lv_obj_set_size(sp->sw->touch_obj, GESTURE_AREA_HEIGHT, EOS_DISPLAY_HEIGHT);
        break;
    }

    // 更新主对象位置
    switch (dir)
    {
    case EOS_SWIPE_DIR_UP:
        lv_obj_set_pos(sp->swipe_obj, 0, EOS_DISPLAY_HEIGHT);
        lv_obj_set_pos(sp->sw->touch_obj, 0, EOS_DISPLAY_HEIGHT - GESTURE_AREA_HEIGHT);
        sp->sw->base = DIR_UP_HIDE_TARGET_COORD;
        sp->sw->target = DIR_UP_SHOW_TARGET_COORD;
        sp->sw->dir = EOS_SLIDE_DIR_VER;
        sp->sw->touch_obj_base = EOS_DISPLAY_HEIGHT - GESTURE_AREA_HEIGHT;
        sp->sw->touch_obj_target = 0;
        break;
    case EOS_SWIPE_DIR_DOWN:
        lv_obj_set_pos(sp->swipe_obj, 0, -EOS_DISPLAY_HEIGHT);
        lv_obj_set_pos(sp->sw->touch_obj, 0, 0);
        sp->sw->base = DIR_DOWN_HIDE_TARGET_COORD;
        sp->sw->target = DIR_DOWN_SHOW_TARGET_COORD;
        sp->sw->dir = EOS_SLIDE_DIR_VER;
        sp->sw->touch_obj_base = 0;
        sp->sw->touch_obj_target = EOS_DISPLAY_HEIGHT - GESTURE_AREA_HEIGHT;
        break;
    case EOS_SWIPE_DIR_LEFT:
        lv_obj_set_pos(sp->swipe_obj, EOS_DISPLAY_WIDTH, 0);
        lv_obj_set_pos(sp->sw->touch_obj, EOS_DISPLAY_WIDTH - GESTURE_AREA_HEIGHT, 0);
        sp->sw->base = DIR_LEFT_HIDE_TARGET_COORD;
        sp->sw->target = DIR_LEFT_SHOW_TARGET_COORD;
        sp->sw->dir = EOS_SLIDE_DIR_HOR;
        sp->sw->touch_obj_base = EOS_DISPLAY_WIDTH - GESTURE_AREA_HEIGHT;
        sp->sw->touch_obj_target = 0;
        break;
    case EOS_SWIPE_DIR_RIGHT:
        lv_obj_set_pos(sp->swipe_obj, -EOS_DISPLAY_WIDTH, 0);
        lv_obj_set_pos(sp->sw->touch_obj, 0, 0);
        sp->sw->base = DIR_RIGHT_HIDE_TARGET_COORD;
        sp->sw->target = DIR_RIGHT_SHOW_TARGET_COORD;
        sp->sw->dir = EOS_SLIDE_DIR_HOR;
        sp->sw->touch_obj_base = 0;
        sp->sw->touch_obj_target = EOS_DISPLAY_WIDTH - GESTURE_AREA_HEIGHT;
        break;
    }

    // 更新handle_bar位置
    _update_handle_bar_position(sp, dir);
}

void eos_swipe_panel_delete(eos_swipe_panel_t *sp)
{
    EOS_CHECK_PTR_RETURN(sp);
    lv_obj_delete(sp->swipe_obj);
    eos_free(sp);
}

static void _slide_widget_move_done_cb(lv_event_t *e)
{
    eos_slide_widget_t *sw = (eos_slide_widget_t *)lv_event_get_user_data(e);
    if (sw->state == EOS_SLIDE_WIDGET_STATE_THRESHOLD)
    {
        EOS_LOG_I("Move cb threshold");
        if (sw->dir == EOS_SLIDE_DIR_VER)
        {
            lv_obj_set_y(sw->touch_obj, sw->touch_obj_target);
        }
        else
        {
            lv_obj_set_x(sw->touch_obj, sw->touch_obj_target);
        }

        eos_slide_widget_reverse(sw);
    }
    else if (sw->state == EOS_SLIDE_WIDGET_STATE_REVERTING)
    {
        EOS_LOG_I("Move cb reverting");
        if (sw->dir == EOS_SLIDE_DIR_VER)
        {
            lv_obj_set_y(sw->touch_obj, sw->touch_obj_base);
        }
        else
        {
            lv_obj_set_x(sw->touch_obj, sw->touch_obj_base);
        }
    }
}

void eos_swipe_panel_slide_down(eos_swipe_panel_t *sp)
{
    EOS_CHECK_PTR_RETURN(sp);
    if (sp->sw->reversed)
    {
        eos_slide_widget_reverse(sp->sw);
    }
    lv_coord_t base, target;
    switch (sp->dir)
    {
    case EOS_SWIPE_DIR_UP:
        base = DIR_UP_HIDE_TARGET_COORD;
        target = DIR_UP_SHOW_TARGET_COORD;
        break;
    case EOS_SWIPE_DIR_DOWN:
        base = DIR_DOWN_HIDE_TARGET_COORD;
        target = DIR_DOWN_SHOW_TARGET_COORD;
        break;
    case EOS_SWIPE_DIR_LEFT:
        base = DIR_LEFT_HIDE_TARGET_COORD;
        target = DIR_LEFT_SHOW_TARGET_COORD;
        break;
    case EOS_SWIPE_DIR_RIGHT:
        base = DIR_RIGHT_HIDE_TARGET_COORD;
        target = DIR_RIGHT_SHOW_TARGET_COORD;
        break;
    }

    eos_slide_widget_move(sp->sw, base, target, SWIPE_ANIM_DURATION);

    if (sp->sw->dir == EOS_SLIDE_DIR_VER)
    {
        lv_obj_set_y(sp->sw->touch_obj, sp->sw->touch_obj_target);
    }
    else
    {
        lv_obj_set_x(sp->sw->touch_obj, sp->sw->touch_obj_target);
    }
}

void eos_swipe_panel_slide_up(eos_swipe_panel_t *sp)
{
    EOS_CHECK_PTR_RETURN(sp);
    if (sp->sw->reversed)
    {
        eos_slide_widget_reverse(sp->sw);
    }
    lv_coord_t base, target;
    switch (sp->dir)
    {
    case EOS_SWIPE_DIR_UP:
        base = DIR_UP_HIDE_TARGET_COORD;
        target = DIR_UP_SHOW_TARGET_COORD;
        break;
    case EOS_SWIPE_DIR_DOWN:
        base = DIR_DOWN_HIDE_TARGET_COORD;
        target = DIR_DOWN_SHOW_TARGET_COORD;
        break;
    case EOS_SWIPE_DIR_LEFT:
        base = DIR_LEFT_HIDE_TARGET_COORD;
        target = DIR_LEFT_SHOW_TARGET_COORD;
        break;
    case EOS_SWIPE_DIR_RIGHT:
        base = DIR_RIGHT_HIDE_TARGET_COORD;
        target = DIR_RIGHT_SHOW_TARGET_COORD;
        break;
    }

    eos_slide_widget_move(sp->sw, target, base, SWIPE_ANIM_DURATION);

    if (sp->sw->dir == EOS_SLIDE_DIR_VER)
    {
        lv_obj_set_y(sp->sw->touch_obj, sp->sw->touch_obj_base);
    }
    else
    {
        lv_obj_set_x(sp->sw->touch_obj, sp->sw->touch_obj_base);
    }
}

void eos_swipe_panel_hide(eos_swipe_panel_t *sp)
{
    EOS_CHECK_PTR_RETURN(sp);
    lv_obj_add_flag(sp->swipe_obj, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(sp->sw->touch_obj, LV_OBJ_FLAG_HIDDEN);
}

void eos_swipe_panel_show(eos_swipe_panel_t *sp)
{
    EOS_CHECK_PTR_RETURN(sp);
    lv_obj_remove_flag(sp->swipe_obj, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(sp->sw->touch_obj, LV_OBJ_FLAG_HIDDEN);
}

eos_swipe_panel_t *eos_swipe_panel_create(lv_obj_t *parent)
{
    eos_swipe_panel_t *sp = eos_malloc(sizeof(eos_swipe_panel_t));
    EOS_CHECK_PTR_RETURN_VAL(sp && parent, NULL);

    // 初始化 swipe_obj
    sp->swipe_obj = lv_obj_create(parent);
    lv_obj_set_size(sp->swipe_obj, EOS_DISPLAY_WIDTH, EOS_DISPLAY_HEIGHT);
    lv_obj_set_style_bg_color(sp->swipe_obj, EOS_COLOR_BLACK, 0);
    lv_obj_set_style_border_width(sp->swipe_obj, 0, 0);
    lv_obj_set_style_shadow_width(sp->swipe_obj, 0, 0);
    lv_obj_set_style_radius(sp->swipe_obj, 0, 0);
    lv_obj_set_style_pad_all(sp->swipe_obj, 0, 0);
    // 默认是下拉栏
    lv_obj_set_y(sp->swipe_obj, -EOS_DISPLAY_HEIGHT);
    lv_obj_move_foreground(sp->swipe_obj);
    // 小白条
    sp->handle_bar = lv_obj_create(sp->swipe_obj);
    lv_obj_set_size(sp->handle_bar, HANDLE_BAR_WIDTH, HANDLE_BAR_HEIGHT);
    lv_obj_set_style_radius(sp->handle_bar, 5, 0);
    lv_obj_set_style_bg_color(sp->handle_bar, EOS_COLOR_GREY, 0);
    lv_obj_set_style_border_width(sp->handle_bar, 0, 0);
    lv_obj_remove_flag(sp->handle_bar, LV_OBJ_FLAG_SCROLLABLE);

    // 默认设置为下拉模式（位于底部）
    _update_handle_bar_position(sp, EOS_SWIPE_DIR_DOWN);

    // 初始化 touch_area
    sp->sw = eos_slide_widget_create(parent, sp->swipe_obj, EOS_SLIDE_DIR_VER, DIR_DOWN_SHOW_TARGET_COORD, EOS_THRESHOLD_40);
    sp->sw->base = DIR_DOWN_HIDE_TARGET_COORD;
    sp->sw->target = DIR_DOWN_SHOW_TARGET_COORD;
    eos_swipe_panel_set_dir(sp, EOS_SWIPE_DIR_DOWN);

    lv_obj_add_event_cb(sp->sw->touch_obj, _slide_widget_move_done_cb, EOS_EVENT_SLIDE_WIDGET_DONE, sp->sw);

    lv_obj_remove_flag(parent, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(sp->swipe_obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(sp->sw->touch_obj, LV_OBJ_FLAG_SCROLLABLE);

    return sp;
}
