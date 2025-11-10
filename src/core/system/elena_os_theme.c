/**
 * @file elena_os_theme.c
 * @brief 主题色
 * @author Sab1e
 * @date 2025-08-27
 */

#include "elena_os_theme.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "elena_os_log.h"
#include "lvgl_private.h"
#include "elena_os_font.h"
/* Macros and Definitions -------------------------------------*/
/************************** Text **************************/
#define TEXT_COLOR EOS_COLOR_WHITE
/************************** Screen **************************/
#define SCREEN_BG_COLOR EOS_COLOR_BLACK
/************************** List **************************/
#define LIST_BG_COLOR EOS_COLOR_BLACK
/************************** Switch **************************/
#define SWITCH_BG_COLOR EOS_COLOR_GREEN
/************************** Slider **************************/
#define SLIDER_MAIN_COLOR lv_color_hex(0x34C759)
#define SLIDER_BG_COLOR lv_color_hex(0x262737)

/* Variables --------------------------------------------------*/
static lv_style_t style_screen;
static lv_style_t style_label;
static lv_style_t style_list;

static lv_style_t style_switch_main;
static lv_style_t style_switch_indicator;

static lv_style_t style_roller_main;
static lv_style_t style_roller_selected;

static lv_style_t style_slider_main;
static lv_style_t style_slider_indicator;
static lv_style_t style_slider_knob;
static lv_style_t style_slider_pressed_color;

static lv_font_t *global_font = NULL;
/* Function Implementations -----------------------------------*/

void _init_style_screen(void)
{
    lv_style_init(&style_screen);
    lv_style_set_bg_color(&style_screen, SCREEN_BG_COLOR);
}

void _init_style_label(void)
{
    lv_style_init(&style_label);
    lv_style_set_text_color(&style_label, TEXT_COLOR);
    lv_style_set_text_font(&style_label, global_font);
}

void _init_style_switch(void)
{
    lv_style_init(&style_switch_main);
    lv_style_set_bg_color(&style_switch_main, EOS_COLOR_GREY);

    lv_style_init(&style_switch_indicator);
    lv_style_set_bg_color(&style_switch_indicator, SWITCH_BG_COLOR);
}

void _init_style_list(void)
{
    lv_style_init(&style_list);
    lv_style_set_bg_color(&style_list, LIST_BG_COLOR);
    lv_style_set_border_width(&style_list, 0);
}

void _init_style_slider(void)
{
    static const lv_style_prop_t props[] = {LV_STYLE_BG_COLOR, 0};
    static lv_style_transition_dsc_t transition_dsc;
    lv_style_transition_dsc_init(&transition_dsc, props, lv_anim_path_linear, 300, 0, NULL);

    lv_style_init(&style_slider_main);
    lv_style_set_bg_opa(&style_slider_main, LV_OPA_COVER);
    lv_style_set_bg_color(&style_slider_main, SLIDER_BG_COLOR);
    lv_style_set_radius(&style_slider_main, LV_RADIUS_CIRCLE);
    lv_style_set_pad_ver(&style_slider_main, -2); /*Makes the indicator larger*/

    lv_style_init(&style_slider_indicator);
    lv_style_set_bg_opa(&style_slider_indicator, LV_OPA_COVER);
    lv_style_set_bg_color(&style_slider_indicator, SLIDER_MAIN_COLOR);
    lv_style_set_radius(&style_slider_indicator, LV_RADIUS_CIRCLE);
    lv_style_set_transition(&style_slider_indicator, &transition_dsc);

    lv_style_init(&style_slider_knob);
    lv_style_set_bg_opa(&style_slider_knob, LV_OPA_COVER);
    lv_style_set_bg_color(&style_slider_knob, SLIDER_MAIN_COLOR);
    lv_style_set_border_color(&style_slider_knob, EOS_COLOR_WHITE);
    lv_style_set_border_width(&style_slider_knob, 4);
    lv_style_set_radius(&style_slider_knob, LV_RADIUS_CIRCLE);
    lv_style_set_pad_all(&style_slider_knob, 6); /*Makes the knob larger*/
    lv_style_set_transition(&style_slider_knob, &transition_dsc);

    lv_style_init(&style_slider_pressed_color);
    lv_style_set_bg_color(&style_slider_pressed_color, lv_color_darken(SLIDER_MAIN_COLOR, 2));
}

void _init_style_roller(void)
{
    lv_style_init(&style_roller_main);
    lv_style_set_bg_color(&style_roller_main, EOS_COLOR_BLACK);
    lv_style_set_border_color(&style_roller_main, EOS_COLOR_DARK_GREY_1);
    lv_style_set_radius(&style_roller_main, 20);
    lv_style_set_text_color(&style_roller_main, EOS_COLOR_DARK_GREY_2);

    lv_style_init(&style_roller_selected);
    lv_style_set_bg_opa(&style_roller_selected, LV_OPA_TRANSP);
}

static void _theme_apply_cb(lv_theme_t *th, lv_obj_t *obj)
{
    LV_UNUSED(th);
    if (lv_obj_check_type(obj, &lv_button_class))
    {
        // lv_obj_add_style(obj, &style_btn, 0);
    }
    else if (lv_obj_check_type(obj, &lv_label_class))
    {
        lv_obj_add_style(obj, &style_label, 0);
#if EOS_FONT_TYPE == EOS_FONT_C_SCALE
        eos_label_add_size_changed_cb(obj);
        eos_label_set_font_size(obj, EOS_FONT_SIZE_MEDIUM);
#endif
    }
    else if (lv_obj_check_type(obj, &lv_list_class))
    {
        lv_obj_add_style(obj, &style_list, 0);
    }
    else if (lv_obj_check_type(obj, &lv_switch_class))
    {
        // 开关开启的状态
        lv_obj_add_style(obj, &style_switch_main, LV_PART_MAIN);
        lv_obj_add_style(obj, &style_switch_indicator, LV_PART_INDICATOR | LV_STATE_CHECKED);
    }
    else if (lv_obj_check_type(obj, &lv_slider_class))
    {
        lv_obj_remove_style_all(obj);
        lv_obj_add_style(obj, &style_slider_main, LV_PART_MAIN);
        lv_obj_add_style(obj, &style_slider_indicator, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &style_slider_pressed_color, LV_PART_INDICATOR | LV_STATE_PRESSED);
        lv_obj_add_style(obj, &style_slider_knob, LV_PART_KNOB);
    }
    else if (lv_obj_check_type(obj, &lv_roller_class))
    {
        lv_obj_add_style(obj, &style_roller_main, LV_PART_MAIN);
        lv_obj_add_style(obj, &style_roller_selected, LV_PART_SELECTED);
    }
}

lv_style_t *eos_theme_get_screen_style(void)
{
    return &style_screen;
}

lv_style_t *eos_theme_get_label_style(void)
{
    return &style_label;
}

void eos_theme_set(lv_color_t primary_color, lv_color_t secondary_color, lv_font_t *font)
{
    global_font = font;

    _init_style_screen();
    _init_style_label();
    _init_style_list();
    _init_style_switch();
    _init_style_slider();
    _init_style_roller();

    lv_theme_t *th_act = lv_theme_default_init(lv_display_get_default(),
                                               primary_color,
                                               secondary_color,
                                               true,
                                               font);

    static lv_theme_t th_new;
    th_new = *th_act;

    lv_theme_set_parent(&th_new, th_act);
    lv_theme_set_apply_cb(&th_new, _theme_apply_cb);

    lv_display_set_theme(lv_display_get_default(), &th_new);
}
