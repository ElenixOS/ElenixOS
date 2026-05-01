/**
 * @file eos_core.c
 * @brief Elenix OS core code implementation
 */

#include "eos_core.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "lvgl.h"
#include "eos_img.h"
#include "eos_msg_list.h"
#include "eos_lang.h"
#include "eos_basic_widgets.h"
#include "eos_event.h"
#include "eos_test.h"
#include "eos_version.h"
#include "eos_port.h"
#include "eos_swipe_panel.h"
#include "eos_service_config.h"
#include "eos_app.h"
#include "script_engine_core.h"
#include "eos_watchface.h"
#include "eos_misc.h"
#include "eos_watchface_list.h"
#include "eos_app_list.h"
#include "eos_theme.h"
#include "eos_config.h"
#include "eos_config_internal.h"
#include "jerryscript.h"
#include "eos_font.h"
#define EOS_LOG_TAG "Core"
#include "eos_log.h"
#include "eos_sensor.h"
#include "eos_dispatcher.h"
#include "eos_anim.h"
#include "eos_control_center.h"
#include "eos_service_storage.h"
#include "eos_pm.h"
#include "eos_dfw.h"
#include "eos_app_header.h"
#include "eos_toast.h"
#include "eos_vibrator.h"
#include "eos_crown.h"
#include "eos_icon.h"
#include "eos_activity.h"
#include "eos_std_widgets.h"
/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/
static bool g_is_inited = false;
/* Function Implementations -----------------------------------*/

static lv_indev_t *_get_key_indev()
{
    lv_indev_t *indev = lv_indev_get_next(NULL);
    while (indev)
    {
        if (lv_indev_get_type(indev) == LV_INDEV_TYPE_KEYPAD)
        {
            return indev;
        }
        indev = lv_indev_get_next(indev);
    }
    EOS_LOG_W("Not found input device: key");
}

void _sys_init_err_handler(const char *err_msg)
{
    EOS_LOG_E("System initialization failed: %s", err_msg);
    lv_obj_t *list = eos_std_info_create(lv_layer_sys(),
                                         EOS_COLOR_RED, RI_BUG_LINE,
                                         eos_lang_get_text(STR_ID_SYS_INIT_FAILED),
                                         eos_lang_get_text(STR_ID_SYS_INIT_FAILED_CONTENT));
    lv_obj_set_style_pad_top(list, 80, 0);
    char info_str[1024];
    snprintf(info_str, sizeof(info_str),
             "Error: %s", err_msg);
    lv_obj_t *err_label = eos_list_add_comment(list, info_str);
    (void)err_label;

    for (uint8_t i = 0; i < 3; ++i)
    {
        uint32_t delay = lv_timer_handler();
        eos_delay(delay);
    }
}

void eos_logo_play(bool anim)
{
    eos_display_set_brightness(EOS_DISPLAY_BRIGHTNESS_MAX);

    lv_obj_t *scr = lv_screen_active();

    if (!scr)
    {
        EOS_LOG_W("Active screen not found, creating a new screen for logo");
        lv_obj_t *scr = lv_obj_create(NULL);
        lv_screen_load(scr);
    }

    // Create full screen container
    lv_obj_t *logo_container = lv_obj_create(scr);
    lv_obj_set_style_bg_color(logo_container, EOS_COLOR_BLACK, 0);
    lv_obj_set_size(logo_container, lv_pct(100), lv_pct(100));
    lv_obj_set_style_border_width(logo_container, 0, 0);
    lv_obj_move_foreground(logo_container);

    // Create LOGO image object
    lv_obj_t *logo_img = lv_image_create(logo_container);
    eos_img_set_src(logo_img, EOS_IMG_LOGO);
    lv_obj_center(logo_img);

    lv_timer_handler();
}

void eos_init(void)
{

    eos_logo_play(true);
    /************************** System components initialization **************************/
#if EOS_DFW_ENABLE
    eos_dfw_init();
#endif /* EOS_DFW_ENABLE */
    eos_lang_init();
    eos_dispatcher_init();
    eos_toast_init();
    eos_vibrator_init();
    eos_crown_init();
    script_engine_init();
    eos_service_config_init();
    lv_font_t *default_font = eos_font_init();
    if (!default_font)
        _sys_init_err_handler("Failed to initialize default font");
    eos_theme_set(lv_palette_main(LV_PALETTE_BLUE),
                  lv_palette_main(LV_PALETTE_RED),
                  default_font);
    eos_app_init();
    eos_watchface_init();
    eos_app_header_init();
    eos_msg_list_init();
    eos_control_center_init();
    eos_pm_init();

    eos_activity_t *watchface_activity = eos_watchface_get_activity();
    if (!watchface_activity)
        _sys_init_err_handler("Failed to get watchface activity");

    // Activity controller will automatically delete Logo Screen
    if (eos_activity_controller_init(watchface_activity) != EOS_OK)
        _sys_init_err_handler("Failed to initialize activity controller");
    g_is_inited = true;
}

bool eos_is_initialized(void)
{
    return g_is_inited;
}

uint32_t eos_main_loop(void)
{
    if (!g_is_inited)
    {
        EOS_LOG_E("System not initialized. Please call eos_init() before eos_main_loop().");
        return 0;
    }
    eos_dispatch_tick();
    return lv_timer_handler();
}
