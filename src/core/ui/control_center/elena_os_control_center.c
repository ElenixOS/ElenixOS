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
#include "elena_os_watchface.h"
#include "elena_os_port.h"
#include "elena_os_config.h"
#include "elena_os_services.h"
#include "elena_os_event.h"
#include "elena_os_icon.h"
#include "elena_os_flash_light.h"

// Macros and Definitions
#define _BTN_DEFAULT_COLOR EOS_THEME_SECONDARY_COLOR
#define _SLIDER_DEFAULT_WIDTH 150
#define _SLIDER_DEFAULT_HEIGHT 360
#define _SLIDER_DEFAULT_RADIUS 50

// Variables
static int32_t _slider_symbol_label_center_x = 0;
static int32_t _slider_symbol_label_center_y = 0;

// Function Implementations

/************************** 基础组件 **************************/
static lv_obj_t *_control_center_create_switch_btn(lv_obj_t *parent, const char *symbol)
{
    lv_obj_t *btn = lv_button_create(parent);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_size(btn, 150, 100);
    lv_obj_set_style_radius(btn, 50, 0);
    lv_obj_set_style_bg_color(btn, _BTN_DEFAULT_COLOR, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(btn, EOS_THEME_PRIMARY_COLOR, LV_PART_MAIN | LV_STATE_CHECKED);

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, symbol);
    lv_obj_center(label);
    return btn;
}

static void _control_center_slider_page_clicked_cb(lv_event_t *e)
{
    lv_obj_t *page = lv_event_get_target(e);
    lv_obj_delete(page);
}

static lv_obj_t *_control_center_slider_create(const char *symbol)
{
    lv_obj_t *slider_page = lv_obj_create(eos_watchface_get_screen());
    lv_obj_remove_style_all(slider_page);
    lv_obj_set_size(slider_page, lv_pct(100), lv_pct(100));
    lv_obj_move_foreground(slider_page);
    lv_obj_set_style_bg_opa(slider_page, LV_OPA_50, 0);
    lv_obj_set_style_bg_color(slider_page, EOS_COLOR_BLACK, 0);
    lv_obj_add_event_cb(slider_page, _control_center_slider_page_clicked_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *slider_mask = lv_obj_create(slider_page);
    lv_obj_remove_style_all(slider_mask);
    lv_obj_set_size(slider_mask, _SLIDER_DEFAULT_WIDTH, _SLIDER_DEFAULT_HEIGHT);
    lv_obj_center(slider_mask);
    lv_obj_set_style_clip_corner(slider_mask, true, 0);
    lv_obj_set_style_radius(slider_mask, 50, 0);

    lv_obj_t *slider = lv_slider_create(slider_mask);
    lv_bar_set_orientation(slider, LV_BAR_ORIENTATION_VERTICAL);
    lv_obj_set_size(slider, _SLIDER_DEFAULT_WIDTH, _SLIDER_DEFAULT_HEIGHT);
    lv_obj_center(slider);

    lv_obj_set_style_clip_corner(slider, true, LV_PART_MAIN);

    lv_obj_set_style_bg_opa(slider, LV_OPA_TRANSP, LV_PART_KNOB);
    lv_obj_set_style_border_opa(slider, LV_OPA_TRANSP, LV_PART_KNOB);
    lv_obj_set_style_shadow_opa(slider, LV_OPA_TRANSP, LV_PART_KNOB);

    lv_obj_set_style_width(slider, _SLIDER_DEFAULT_WIDTH, LV_PART_MAIN);
    lv_obj_set_style_height(slider, _SLIDER_DEFAULT_HEIGHT, LV_PART_MAIN);

    lv_obj_set_style_bg_color(slider, EOS_COLOR_WHITE, LV_PART_INDICATOR);
    lv_obj_set_style_radius(slider, 50, LV_PART_INDICATOR);

    lv_obj_set_style_bg_color(slider, EOS_COLOR_DARK_GREY_1, LV_PART_MAIN);
    lv_obj_set_style_radius(slider, 50, LV_PART_MAIN);

    lv_obj_set_style_bg_color(slider, EOS_COLOR_WHITE, LV_PART_INDICATOR | LV_STATE_PRESSED);

    lv_obj_t *label = lv_label_create(slider_page);
    lv_label_set_text(label, symbol);
    lv_obj_set_user_data(slider, (void *)label);
    lv_obj_set_style_text_color(label, EOS_COLOR_BLACK, 0);
    lv_obj_move_foreground(label);
    lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, -100);

    lv_obj_update_layout(label);
    lv_obj_set_style_transform_pivot_x(label, lv_obj_get_width(label) / 2, 0);
    lv_obj_set_style_transform_pivot_y(label, lv_obj_get_height(label) / 2, 0);

    return slider;
}

static lv_obj_t *_control_center_create_btn(lv_obj_t *parent, const char *symbol)
{
    lv_obj_t *btn = lv_button_create(parent);
    lv_obj_set_size(btn, 150, 100);
    lv_obj_set_style_radius(btn, 50, 0);
    lv_obj_set_style_bg_color(btn, _BTN_DEFAULT_COLOR, 0);

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, symbol);
    lv_obj_center(label);
    return btn;
}

/************************** 功能组件 **************************/

static void _control_center_bluetooth_switch_btn_cb(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e);

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

static void _control_center_brightness_value_changed_cb(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    lv_obj_t *label = (lv_obj_t *)lv_obj_get_user_data(slider);
    EOS_CHECK_PTR_RETURN(label);

    // 获取 Slider 当前值
    int16_t value = lv_slider_get_value(slider);
    eos_display_set_brightness(value);

    // 将 Slider 值映射为角度
    int32_t angle = (int32_t)value * 18;

    // 设置 Label 旋转
    lv_obj_set_style_transform_rotation(label, angle, 0);
}

static void _control_center_brightness_slider_delete_cb(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    eos_sys_cfg_set_number(EOS_SYS_CFG_KEY_DISPLAY_BRIGHTNESS, lv_slider_get_value(slider));
}

static void _control_center_brightness_btn_clicked_cb(lv_event_t *e)
{
    lv_obj_t *slider = _control_center_slider_create(RI_SUN_LINE);
    lv_slider_set_range(slider, EOS_DISPLAY_BRIGHTNESS_MIN, EOS_DISPLAY_BRIGHTNESS_MAX);
    lv_slider_set_value(slider,
                        eos_sys_cfg_get_number(EOS_SYS_CFG_KEY_DISPLAY_BRIGHTNESS, 50),
                        LV_ANIM_ON);
    lv_obj_add_event_cb(slider,
                        _control_center_brightness_value_changed_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(slider, _control_center_brightness_slider_delete_cb, LV_EVENT_DELETE, NULL);
}

static void _control_center_battery_level_update_cb(lv_event_t *e)
{
    eos_sensor_t *sensor = lv_event_get_param(e);
    lv_obj_t *label = lv_event_get_target(e);
    lv_obj_t *btn = lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(sensor && label && btn);
    if (sensor->data.bat.charging)
    {
        char str[16];
        snprintf(str, sizeof(str), RI_FLASHLIGHT_FILL " %d%%", sensor->data.bat.level);
        lv_label_set_text(label, str);
    }
    else
    {
        char str[16];
        snprintf(str, sizeof(str), "%d%%", sensor->data.bat.level);
        lv_label_set_text(label, str);
    }
}

static lv_obj_t *_control_center_create_battery(lv_obj_t *parent)
{
    lv_obj_t *btn = lv_button_create(parent);
    lv_obj_set_size(btn, 150, 100);
    lv_obj_set_style_radius(btn, 50, 0);
    lv_obj_set_style_bg_color(btn, _BTN_DEFAULT_COLOR, 0);

    lv_obj_t *label = lv_label_create(btn);
    char str[16];
    snprintf(str, sizeof(str), "%d%%", eos_battery_service_get_level());
    lv_label_set_text(label, str);
    lv_obj_center(label);

    eos_event_add_cb(
        label,
        _control_center_battery_level_update_cb,
        eos_event_get_code(EOS_EVENT_SENSOR_REPORT_BAT),
        (void *)btn);
    return btn;
}

static void _control_center_phone_find_cb(lv_event_t *e)
{
    eos_locate_phone();
}

static char *_control_center_volume_get_icon_by_value(uint8_t value)
{
    if (value > 50)
    {
        return RI_VOLUME_UP_FILL;
    }
    else if (value > 0)
    {
        return RI_VOLUME_DOWN_FILL;
    }
    else
    {
        return RI_VOLUME_MUTE_FILL;
    }
}

static void _control_center_volume_value_changed_cb(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    lv_obj_t *label = (lv_obj_t *)lv_obj_get_user_data(slider);
    EOS_CHECK_PTR_RETURN(label);
    // 获取 Slider 当前值
    int16_t value = lv_slider_get_value(slider);
    eos_speaker_set_volume(value);
    lv_label_set_text(label, _control_center_volume_get_icon_by_value(value));
}

static void _control_center_volume_slider_delete_cb(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    eos_sys_cfg_set_number(EOS_SYS_CFG_KEY_SPEAKER_VOLUME, lv_slider_get_value(slider));
}

static void _control_center_volume_btn_clicked_cb(lv_event_t *e)
{
    uint8_t volume = eos_sys_cfg_get_number(EOS_SYS_CFG_KEY_SPEAKER_VOLUME, 50);

    lv_obj_t *slider = _control_center_slider_create(_control_center_volume_get_icon_by_value(volume));
    lv_slider_set_range(slider, EOS_SPEAKER_VOLUME_MIN, EOS_SPEAKER_VOLUME_MAX);

    lv_slider_set_value(slider,
                        volume,
                        LV_ANIM_ON);
    lv_obj_add_event_cb(slider,
                        _control_center_volume_value_changed_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(slider, _control_center_volume_slider_delete_cb, LV_EVENT_DELETE, NULL);
}

static void _control_center_flash_light_btn_clicked_cb(lv_event_t *e)
{
    eos_flash_light_show();
}

/************************** 创建控制中心 **************************/
void eos_control_center_create(lv_obj_t *parent)
{
    swipe_panel_t *swipe_panel = eos_swipe_panel_create(parent);
    eos_swipe_panel_set_dir(swipe_panel, EOS_SWIPE_DIR_UP);
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
    /************************** 蓝牙开关 **************************/
    btn = _control_center_create_switch_btn(container, RI_BLUETOOTH_FILL);
    lv_obj_add_event_cb(btn, _control_center_bluetooth_switch_btn_cb, LV_EVENT_VALUE_CHANGED, NULL);
    if (eos_sys_cfg_get_bool(EOS_SYS_CFG_KEY_BLUETOOTH, false))
    {
        lv_obj_add_state(btn, LV_STATE_CHECKED);
    }
    else
    {
        lv_obj_remove_state(btn, LV_STATE_CHECKED);
    }
    /************************** 亮度调整滚动条 **************************/
    btn = _control_center_create_btn(container, RI_SUN_FILL);
    lv_obj_add_event_cb(btn, _control_center_brightness_btn_clicked_cb, LV_EVENT_CLICKED, 0);
    /************************** 电量显示 **************************/
    btn = _control_center_create_battery(container);
    /************************** 查找手机 **************************/
    btn = _control_center_create_btn(container, RI_PHONE_FIND_FILL);
    lv_obj_add_event_cb(btn, _control_center_phone_find_cb, LV_EVENT_CLICKED, 0);
    /************************** 音量调整滚动条 **************************/
    btn = _control_center_create_btn(container, RI_VOLUME_UP_FILL);
    lv_obj_add_event_cb(btn, _control_center_volume_btn_clicked_cb, LV_EVENT_CLICKED, 0);
    /************************** 手电筒 **************************/
    btn = _control_center_create_btn(container, RI_FLASH_LIGHT);
    lv_obj_add_event_cb(btn, _control_center_flash_light_btn_clicked_cb, LV_EVENT_CLICKED, 0);
}
