/**
 * @file eos_radio_list.c
 * @brief Radio list page
 */

#include "eos_radio_list.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#define EOS_LOG_TAG "RadioList"
#include "eos_log.h"
#include "eos_port.h"
#include "eos_basic_widgets.h"
#include "eos_app_header.h"
#include "eos_theme.h"
#include "eos_icon.h"
#include "eos_mem.h"
#include "eos_activity.h"

/* Macros and Definitions -------------------------------------*/
#define _RADIO_ITEM_HEIGHT 100
#define _RADIO_ITEM_PRESSED_SCALE 240

#define _RADIO_ITEM_TITLE_LABEL_INDEX 0
#define _RADIO_ITEM_CHECK_LABEL_INDEX 1
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
    lv_obj_t *label = lv_obj_get_child(obj, _RADIO_ITEM_CHECK_LABEL_INDEX);
    EOS_CHECK_PTR_RETURN(label);
    _radio_item_check(rl, label);
    lv_obj_send_event(rl->radio_item_container, LV_EVENT_VALUE_CHANGED, (void *)(intptr_t)index);
}

void eos_radio_list_check(eos_radio_list_t *rl, uint32_t index)
{
    EOS_CHECK_PTR_RETURN(rl);
    lv_obj_t *item = lv_obj_get_child(rl->radio_item_container, index);
    EOS_CHECK_PTR_RETURN(item);
    lv_obj_t *check_label = lv_obj_get_child(item, _RADIO_ITEM_CHECK_LABEL_INDEX);
    EOS_CHECK_PTR_RETURN(check_label);
    _radio_item_check(rl, check_label);
}

uint32_t eos_radio_list_add_item(eos_radio_list_t *rl, const char *txt)
{
    EOS_CHECK_PTR_RETURN_VAL(rl && txt && rl->radio_item_container, EOS_INVALID_RADIO_INDEX);
    lv_obj_t *item = lv_button_create(rl->radio_item_container);
    lv_obj_set_size(item, lv_pct(100), _RADIO_ITEM_HEIGHT);
    lv_obj_set_style_bg_color(item, EOS_COLOR_DARK_GREY_2, 0);
    lv_obj_set_style_bg_opa(item, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(item, 0, 0);
    lv_obj_set_style_pad_hor(item, EOS_LIST_CONTAINER_PAD_ALL, 0);
    lv_obj_set_style_margin_all(item, 0, 0);

    lv_obj_update_layout(item);
    lv_obj_set_style_transform_pivot_x(item, lv_obj_get_width(item) / 2, 0);
    lv_obj_set_style_transform_pivot_y(item, lv_obj_get_height(item) / 2, 0);
    lv_obj_set_style_transform_scale(item, LV_SCALE_NONE, LV_STATE_DEFAULT);
    lv_obj_set_style_transform_scale(item, _RADIO_ITEM_PRESSED_SCALE, LV_STATE_PRESSED);

    lv_obj_t *title_label = lv_label_create(item);
    lv_label_set_text(title_label, txt);
    lv_obj_align(title_label, LV_ALIGN_LEFT_MID, 0, 0);

    // Note: If you want to add or remove objects within the item, you need to modify the label index in `_radio_item_clicked_cb`
    lv_obj_t *check_label = lv_label_create(item);
    lv_label_set_text(check_label, RI_CHECK_FILL);
    lv_obj_set_style_text_color(check_label, EOS_COLOR_GREEN, 0);
    lv_obj_align(check_label, LV_ALIGN_RIGHT_MID, 0, 0);
    if (rl->item_number == 0)
    {
        _radio_item_check(rl, check_label);
        eos_obj_set_corner_radius_bg(item,
                                     EOS_ROUND_TOP_LEFT | EOS_ROUND_TOP_RIGHT,
                                     EOS_ITEM_RADIUS, EOS_COLOR_DARK_GREY_2);
    }
    else
    {
        if (rl->item_number > 1)
        {
            // 已经有两个对象，需要删除第二个对象的圆角
            eos_obj_remove_corner_radius_bg(rl->last_item);
        }
        eos_obj_set_corner_radius_bg(item,
                                     EOS_ROUND_BOTTOM_LEFT | EOS_ROUND_BOTTOM_RIGHT,
                                     EOS_ITEM_RADIUS, EOS_COLOR_DARK_GREY_2);
        lv_obj_add_flag(check_label, LV_OBJ_FLAG_HIDDEN);
    }

    // 设置label顺序以便能够找到check_label
    lv_obj_move_to_index(title_label, _RADIO_ITEM_TITLE_LABEL_INDEX);
    lv_obj_move_to_index(check_label, _RADIO_ITEM_CHECK_LABEL_INDEX);

    lv_obj_set_user_data(item, (void *)(intptr_t)rl->item_number);
    lv_obj_add_event_cb(item, _radio_item_clicked_cb, LV_EVENT_CLICKED, rl);

    rl->last_item = item;
    uint32_t item_index = rl->item_number;
    rl->item_number++;

    return item_index;
}

void eos_radio_list_set_subtitle(eos_radio_list_t *rl, const char *subtitle)
{
    EOS_CHECK_PTR_RETURN(rl && subtitle && rl->subtitle_label);
    lv_label_set_text(rl->subtitle_label, subtitle);
}

void eos_radio_list_set_comment(eos_radio_list_t *rl, const char *comment)
{
    EOS_CHECK_PTR_RETURN(rl && comment && rl->comment_label);
    lv_label_set_text(rl->comment_label, comment);
}

void eos_radio_list_add_event_cb(eos_radio_list_t *rl, lv_event_cb_t event_cb, void *user_data)
{
    EOS_CHECK_PTR_RETURN(rl && rl->radio_item_container);
    lv_obj_add_event_cb(rl->radio_item_container, event_cb, LV_EVENT_VALUE_CHANGED, user_data);
}

static const eos_activity_lifecycle_t radio_list_lifecycle = {
    .on_enter = NULL,
    .on_destroy = NULL,
    .on_pause = NULL,
    .on_resume = NULL,
};

eos_radio_list_t *eos_radio_list_enter(const char *title)
{
    eos_radio_list_t *rl = eos_malloc_zeroed(sizeof(eos_radio_list_t));
    EOS_CHECK_PTR_RETURN_VAL(rl, NULL);

    rl->item_number = 0;
    rl->selected_index = 0;

    eos_activity_t *a = eos_activity_create(&radio_list_lifecycle);
    EOS_CHECK_PTR_RETURN_VAL(a, NULL);
    eos_activity_set_title(a, title);
    eos_activity_set_app_header_visible(a, true);
    lv_obj_t *view = eos_activity_get_view(a);

    lv_obj_t *list = eos_list_create(view);

    rl->subtitle_label = eos_list_add_title(list, title);

    lv_obj_t *con = lv_obj_create(list);
    lv_obj_remove_style_all(con);
    lv_obj_set_size(con, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(con, LV_OPA_TRANSP, 0);
    lv_obj_set_style_radius(con, 0, 0);
    lv_obj_set_style_pad_all(con, 0, 0);
    lv_obj_set_style_clip_corner(con, true, 0);
    lv_obj_set_flex_flow(con, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(con, 1, 0);
    lv_obj_set_style_pad_top(con, 0, 0);
    lv_obj_set_style_pad_bottom(con, 0, 0);

    rl->radio_item_container = con;
    rl->comment_label = eos_list_add_comment(list, "");

    eos_activity_set_user_data(a, rl);
    eos_activity_enter(a);
    return rl;
}
