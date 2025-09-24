/**
 * @file elena_os_control_center.c
 * @brief 上拉控制台
 * @author Sab1e
 * @date 2025-09-22
 */

#include "elena_os_control_center.h"

// Includes
#include <stdio.h>
#include <stdlib.h>
#include "elena_os_swipe_panel.h"
#include "elena_os_log.h"
#include "elena_os_theme.h"
#include "elena_os_sys.h"
#include "elena_os_port.h"

// Macros and Definitions
#define CONTROL_CENTER_BTN_DEFAULT_COLOR lv_color_hex(0x232323)
// Variables

// Function Implementations

static void _control_center_bluetooth_switch_btn_cb(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e);
    if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED)
    {
        if (lv_obj_has_state(btn, LV_STATE_CHECKED))
        {
            EOS_LOG_D("CHECKED");
            eos_bluetooth_enable();
            eos_sys_cfg_set_bool(EOS_SYS_CFG_KEY_BLUETOOTH, true);
        }
        else
        {
            EOS_LOG_D("UNCHECKED");
            eos_bluetooth_disable();
            eos_sys_cfg_set_bool(EOS_SYS_CFG_KEY_BLUETOOTH, false);
        }
    }
}

lv_obj_t *_control_center_create_switch_btn(lv_obj_t *parent, const char *symbol)
{
    lv_obj_t *btn = lv_button_create(parent);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_size(btn, 150, 100);
    lv_obj_set_style_radius(btn, 50, 0);
    lv_obj_set_style_bg_color(btn, CONTROL_CENTER_BTN_DEFAULT_COLOR, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(btn, EOS_THEME_PRIMARY_COLOR, LV_PART_MAIN | LV_STATE_CHECKED);

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, symbol);
    lv_obj_center(label);
    return btn;
}

static void _control_crenter_slider_btn_event_cb(lv_event_t *e)
{
}

lv_obj_t *_control_center_create_slider_btn(lv_obj_t *parent)
{
    // 隐藏 control_center 然后将 Slider 放在顶层
    lv_obj_t *btn = lv_button_create(parent);

    lv_obj_set_size(btn, 150, 100);
    lv_obj_set_style_radius(btn, 50, 0);
    lv_obj_set_style_bg_color(btn, CONTROL_CENTER_BTN_DEFAULT_COLOR, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(btn, EOS_THEME_PRIMARY_COLOR, LV_PART_MAIN | LV_STATE_CHECKED);
    // lv_obj_add_event_cb(btn, _control_center_switch_btn_event_cb, LV_EVENT_ALL, NULL);

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, LV_SYMBOL_BLUETOOTH);
    lv_obj_center(label);
    return btn;
}

lv_obj_t *_control_center_create_btn(lv_obj_t *parent)
{
    lv_obj_t *btn = lv_button_create(parent);
    lv_obj_set_size(btn, 150, 100);
    lv_obj_set_style_radius(btn, 50, 0);
    lv_obj_set_style_bg_color(btn, CONTROL_CENTER_BTN_DEFAULT_COLOR, 0);
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, LV_SYMBOL_GPS);
    lv_obj_center(label);
    return btn;
}

void eos_control_center_create(lv_obj_t *parent)
{
    EOS_LOG_D("Create Control Center");
    swipe_panel_t *swipe_panel = eos_swipe_panel_create(parent);
    eos_swipe_panel_set_dir(swipe_panel, SWIPE_DIR_UP);
    eos_swipe_panel_show_handle_bar(swipe_panel);

    lv_obj_t *container = lv_list_create(swipe_panel->swipe_obj);
    lv_obj_set_size(container, LV_PCT(100), LV_PCT(88));
    lv_obj_set_style_bg_opa(container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(container, 0, 0);
    lv_obj_set_style_pad_all(container, 30, 0);
    lv_obj_align(container, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_scroll_dir(container, LV_DIR_VER);
    lv_obj_set_style_pad_column(container, 20, 0); // 列间距
    lv_obj_set_style_pad_row(container, 20, 0);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_scrollbar_mode(container, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_flex_align(container,
                          LV_FLEX_ALIGN_CENTER,        // 主轴（水平方向）从头开始（靠左）
                          LV_FLEX_ALIGN_CENTER,        // 交叉轴（垂直方向）居中
                          LV_FLEX_ALIGN_SPACE_EVENLY); // 行与行之间均匀分布

    lv_obj_t *btn;
    btn = _control_center_create_switch_btn(container, LV_SYMBOL_BLUETOOTH);
    lv_obj_add_event_cb(btn, _control_center_bluetooth_switch_btn_cb, LV_EVENT_ALL, NULL);
    if (eos_sys_cfg_get_bool(EOS_SYS_CFG_KEY_BLUETOOTH, false))
    {
        lv_obj_add_state(btn, LV_STATE_CHECKED);
    }
    else
    {
        lv_obj_remove_state(btn, LV_STATE_CHECKED);
    }

    _control_center_create_btn(container);
    _control_center_create_btn(container);
    _control_center_create_btn(container);
    _control_center_create_btn(container);
    _control_center_create_btn(container);
}