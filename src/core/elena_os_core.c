/**
 * @file elena_os_core.c
 * @brief Elena OS 核心代码实现
 * @author Sab1e
 * @date 2025-08-10
 * @details
 * 系统首先初始化各种组件，
 * 然后加载表盘页面，
 * 接着开始无限调用`lv_timer_handler()`。
 *
 * 当侧键按下后：
 *  - 如果位于表盘页面，会从回调中加载应用列表页面。
 *  - 如果位于应用列表页面，会从回调中加载表盘页面。
 *  - 如果非上述两种情况，则会返回导航栈的上一页。
 *
 */

/**
 * TODO:
 * 编码器索引
 * Group问题
 * 编码器滚动时，列表跟随滚动-
 */

#include "elena_os_core.h"

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "lvgl.h"
#include "elena_os_img.h"
#include "elena_os_msg_list.h"
#include "elena_os_lang.h"
#include "elena_os_log.h"
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

// Macros and Definitions
#if EOS_FONT_TYPE == EOS_FONT_USE_C
LV_FONT_DECLARE(EOS_FONT_C_NAME);
#endif

// Variables
lv_group_t *encoder_group;

// Function Implementations

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

static void _side_btn_async_cb(void *user_data)
{
    eos_side_btn_state_t *state = (eos_side_btn_state_t *)user_data;
    EOS_CHECK_PTR_RETURN(state);
    switch (*state)
    {
    case EOS_SIDE_BTN_CLICKED:
        lv_obj_t *scr = lv_screen_active();
        if (scr == eos_watchface_get_screen())
        {
            EOS_LOG_D("Current screen: watchface");
            eos_watchface_delete();
            eos_app_list_create();
        }
        else if (scr == eos_app_list_get_screen())
        {
            EOS_LOG_D("Current screen: app_list");
            eos_watchface_create();
        }
        else
        {
            EOS_LOG_D("Side button: Nav Back");
            eos_nav_back_clean();
        }
    default:
        break;
    }
    free(state);
}

void eos_side_btn_handler(eos_side_btn_state_t state)
{
    eos_side_btn_state_t *side_btn_state = malloc(sizeof(eos_side_btn_state_t));
    EOS_CHECK_PTR_RETURN(side_btn_state);
    *side_btn_state = state;
    lv_async_call(_side_btn_async_cb, (void *)side_btn_state);
}

eos_result_t eos_run(void)
{
    /************************** 系统组件初始化 **************************/
    eos_sys_init();
    eos_event_init();
#if EOS_FONT_TYPE == EOS_FONT_USE_LVGL
    eos_theme_set(lv_palette_main(LV_PALETTE_BLUE),
                  lv_palette_main(LV_PALETTE_RED),
                  &EOS_FONT_LVGL);
#elif EOS_FONT_TYPE == EOS_FONT_USE_C
    eos_theme_set(lv_palette_main(LV_PALETTE_BLUE),
                  lv_palette_main(LV_PALETTE_RED),
                  &EOS_FONT_C_NAME);
#elif EOS_FONT_TYPE == EOS_FONT_USE_TTF
    static lv_font_t *font_ttf;
    font_ttf = lv_tiny_ttf_create_file(argv[1], 24); // 24px 大小
    if (font_ttf == NULL)
    {
        EOS_LOG_E("Failed to load TTF font!");
    }
    else
    {
        eos_theme_set(lv_palette_main(LV_PALETTE_BLUE),
                      lv_palette_main(LV_PALETTE_RED),
                      font_ttf);
    }
#endif /* EOS_FONT_TYPE */
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

    /************************** 服务启动 **************************/

    eos_battery_service_start();

    /************************** 系统启动 **************************/
    eos_watchface_create(); // 加载表盘
    script_engine_request_stop();
    // 开始绘制
    while (1)
    {
        uint32_t delay = lv_timer_handler();
        eos_delay(delay);
    }
}
