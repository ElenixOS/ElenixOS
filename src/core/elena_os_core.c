/**
 * @file elena_os_core.c
 * @brief Elena OS 核心代码实现
 * @author Sab1e
 * @date 2025-08-10
 */

#include "elena_os_core.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "lvgl.h"
#include "elena_os_img.h"
#include "elena_os_msg_list.h"
#include "elena_os_lang.h"
#include "elena_os_nav.h"
#include "elena_os_basic_widgets.h"
#include "elena_os_event.h"
#include "elena_os_test.h"
#include "elena_os_version.h"
#include "elena_os_port.h"
#include "elena_os_swipe_panel.h"
#include "elena_os_sys.h"
#include "elena_os_app.h"
#include "script_engine_core.h"
#include "elena_os_watchface.h"
#include "elena_os_misc.h"
#include "elena_os_watchface_list.h"
#include "elena_os_app_list.h"
#include "elena_os_theme.h"
#include "elena_os_config.h"
#include "elena_os_config_internal.h"
#include "elena_os_services.h"
#include "jerryscript.h"
#include "elena_os_font.h"
#define EOS_LOG_TAG "Core"
#include "elena_os_log.h"
#include "elena_os_sensor.h"
#include "elena_os_dispatcher.h"
#include "elena_os_scene.h"
#include "elena_os_anim.h"
#include "elena_os_control_center.h"
#include "elena_os_fs.h"
#include "elena_os_afw.h"
/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/
lv_group_t *encoder_group;
static bool is_logo_played = false;
static lv_obj_t *logo = NULL;
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

void eos_logo_play(bool anim)
{
    if (is_logo_played)
        return;

    eos_display_set_brightness(EOS_DISPLAY_BRIGHTNESS_MAX);

    // 创建全屏容器
    logo = lv_obj_create(lv_screen_active());
    lv_obj_set_style_bg_color(logo, EOS_COLOR_BLACK, 0);
    lv_obj_set_size(logo, lv_pct(100), lv_pct(100));
    lv_obj_set_style_border_width(logo, 0, 0);
    lv_obj_move_foreground(logo);

    // 创建LOGO图片对象
    lv_obj_t *logo_img = lv_image_create(logo);
    eos_img_set_src(logo_img, EOS_IMG_LOGO);
    lv_obj_center(logo_img);

    if (anim)
    {
        const uint16_t duration_ms = 800;
        const uint8_t timer_handler_delay = 20;
        eos_anim_fade_start(logo_img, LV_OPA_TRANSP, LV_OPA_COVER, duration_ms, false);

        uint16_t count = 0;
        while (1)
        {
            lv_timer_handler();
            eos_delay(timer_handler_delay);
            count += timer_handler_delay;
            if (count >= duration_ms + 100)
                break;
        }
    }
    else
    {
        lv_timer_handler();
    }

    is_logo_played = true;
}

eos_result_t eos_run(void)
{
    eos_logo_play(true);
    /************************** 系统组件初始化 **************************/
    eos_afw_init();
    eos_dispatcher_init();
    script_engine_init();
    eos_sys_init();
    lv_font_t *default_font = eos_font_init();
    EOS_ASSERT(default_font != NULL);
    eos_theme_set(lv_palette_main(LV_PALETTE_BLUE),
                  lv_palette_main(LV_PALETTE_RED),
                  default_font);
    eos_app_init();
    eos_watchface_init();
    eos_lang_init();

    /*
    lv_indev_t *indev = _get_key_indev();
    if (indev)
    {
        encoder_group = lv_indev_get_group(indev);
        lv_group_add_obj(encoder_group, eos_watchface_get_screen());
    }
    else
    {
        EOS_LOG_W("Input device not found");
    }
    */
    if (eos_watchface_list_size() == 0)
    {
        EOS_LOG_E("Watchface not found");
        while (1)
        {
            if (eos_watchface_list_size() > 0)
                break;
            eos_delay(5000);
        }
    }
    eos_app_header_init();
    eos_services_start();
    eos_msg_list_init();
    eos_control_center_init();
    if (logo)
        lv_obj_delete(logo);
    /************************** 系统启动 **************************/
    eos_scene_init(eos_watchface_create, eos_watchface_delete, eos_app_list_create, eos_watchface_list_create);
    // 开始绘制
    while (1)
    {
        eos_dispatch_tick();
        eos_afw_handler();
        uint32_t delay = lv_timer_handler();
        eos_delay(delay);
    }
}
