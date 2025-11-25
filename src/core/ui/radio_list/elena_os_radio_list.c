/**
 * @file elena_os_radio_list.c
 * @brief 单选列表页面
 * @author Sab1e
 * @date 2025-11-25
 */

#include "elena_os_radio_list.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#define EOS_LOG_TAG "RadioList"
#include "elena_os_log.h"
#include "elena_os_port.h"
#include "elena_os_nav.h"
#include "elena_os_basic_widgets.h"
#include "elena_os_app_header.h"
#include "elena_os_theme.h"
#include "elena_os_icon.h"

/* Macros and Definitions -------------------------------------*/
#define _RADIO_ITEM_HEIGHT 100
#define _RADIO_ITEM_PRESSED_SCALE 240
struct eos_radio_list_t
{
    lv_obj_t *screen;
    lv_obj_t *subtitle_label;
    lv_obj_t *radio_item_container;
    lv_obj_t *comment_label;
    lv_obj_t *last_right_label;
    lv_obj_t *last_item;
    uint32_t selected_index;
    uint32_t item_number;
};
/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

static void _radio_item_check(eos_radio_list_t *rl, lv_obj_t *right_label)
{
    if (rl->last_right_label == right_label)
        return;
    lv_obj_remove_flag(right_label, LV_OBJ_FLAG_HIDDEN);
    if (rl->last_right_label)
    {
        lv_obj_add_flag(rl->last_right_label, LV_OBJ_FLAG_HIDDEN);
    }
    rl->last_right_label = right_label;
}

static void _radio_item_clicked_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    uint32_t index = (uint32_t)lv_obj_get_user_data(obj);
    eos_radio_list_t *rl = lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(rl);
    lv_obj_t *label = lv_obj_get_child(obj, 1);
    EOS_CHECK_PTR_RETURN(label);
    _radio_item_check(rl, label);
    lv_obj_send_event(rl->radio_item_container, LV_EVENT_VALUE_CHANGED, (void *)(intptr_t)index);
}

void eos_radio_list_add_item(eos_radio_list_t *rl, const char *txt)
{
    EOS_CHECK_PTR_RETURN(rl && txt && rl->radio_item_container);
    lv_obj_t *item = lv_button_create(rl->radio_item_container);
    lv_obj_set_size(item, lv_pct(100), _RADIO_ITEM_HEIGHT);
    lv_obj_set_style_bg_color(item, EOS_COLOR_DARK_GREY_2, 0);
    lv_obj_set_style_bg_opa(item, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(item, 0, 0);
    lv_obj_set_style_pad_hor(item, EOS_LIST_CONTAINER_PAD_ALL, 0);

    // TODO: 按下时圆角问题

    lv_obj_update_layout(item);
    lv_obj_set_style_transform_pivot_x(item, lv_obj_get_width(item) / 2, 0);
    lv_obj_set_style_transform_pivot_y(item, lv_obj_get_height(item) / 2, 0);
    lv_obj_set_style_transform_scale(item, LV_SCALE_NONE, LV_STATE_DEFAULT);
    lv_obj_set_style_transform_scale(item, _RADIO_ITEM_PRESSED_SCALE, LV_STATE_PRESSED);

    lv_obj_t *label = lv_label_create(item);
    lv_label_set_text(label, txt);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 0, 0);

    // 注意：如果要添加或删除item内的对象，需要修改`_radio_item_clicked_cb`中label的索引值
    label = lv_label_create(item);
    lv_label_set_text(label, RI_CHECK_FILL);
    lv_obj_set_style_text_color(label, EOS_COLOR_GREEN, 0);
    lv_obj_align(label, LV_ALIGN_RIGHT_MID, 0, 0);
    if (rl->item_number == 0)
    {
        _radio_item_check(rl, label);
    }
    else
    {
        lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
    }

    lv_obj_set_user_data(item, (void *)(intptr_t)rl->item_number);
    lv_obj_add_event_cb(item, _radio_item_clicked_cb, LV_EVENT_CLICKED, rl);

    rl->item_number++;
}

void eos_radio_list_set_subtitle(eos_radio_list_t *rl, const char *subtitle)
{
    EOS_CHECK_PTR_RETURN(rl && subtitle && rl->subtitle_label);
    lv_label_set_text(rl->comment_label, subtitle);
}

void eos_radio_list_set_comment(eos_radio_list_t *rl, const char *comment)
{
    EOS_CHECK_PTR_RETURN(rl && comment && rl->comment_label);
    lv_label_set_text(rl->comment_label, comment);
}

static void _screen_deleted_cb(lv_event_t *e)
{
    eos_radio_list_t *rl = lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(rl);
    eos_free(rl);
}

void eos_radio_list_add_event_cb(eos_radio_list_t *rl, lv_event_cb_t event_cb, void *user_data)
{
    EOS_CHECK_PTR_RETURN(rl && rl->radio_item_container);
    lv_obj_add_event_cb(rl->radio_item_container, event_cb, LV_EVENT_VALUE_CHANGED, user_data);
}

eos_radio_list_t *eos_radio_list_create(const char *title)
{
    eos_radio_list_t *rl = eos_malloc_zeroed(sizeof(eos_radio_list_t));
    EOS_CHECK_PTR_RETURN_VAL(rl, NULL);

    rl->item_number = 0;
    rl->selected_index = 0;

    if (eos_nav_get_initialized())
    {
        rl->screen = eos_nav_scr_create();
    }
    EOS_CHECK_PTR_RETURN_VAL(rl->screen, NULL);
    eos_app_header_bind_screen(rl->screen, title);
    lv_screen_load(rl->screen);

    lv_obj_t *list = eos_list_create(rl->screen);

    rl->subtitle_label = eos_list_add_title(list, title);

    lv_obj_t *con = lv_obj_create(list);
    lv_obj_remove_style_all(con);
    lv_obj_set_size(con, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(con, LV_OPA_TRANSP, 0);
    lv_obj_set_style_radius(con, EOS_ITEM_RADIUS, 0);
    lv_obj_set_style_pad_all(con, 0, 0);
    lv_obj_set_style_clip_corner(con, true, 0);
    lv_obj_set_flex_flow(con, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(con, 1, 0);
    lv_obj_set_style_pad_top(con, 0, 0);
    lv_obj_set_style_pad_bottom(con, 0, 0);

    rl->radio_item_container = con;
    rl->comment_label = eos_list_add_comment(list, "");
    lv_obj_add_event_cb(rl->screen, _screen_deleted_cb, LV_EVENT_DELETE, rl);
    return rl;
}
