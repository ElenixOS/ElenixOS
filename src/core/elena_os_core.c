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
 *  - 如果位于表盘页面，会从回调中异步加载应用列表页面。
 *  - 如果位于应用列表页面，会从回调中异步加载表盘页面。
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
// Macros and Definitions

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

void eos_side_btn_handler(eos_side_btn_state_t state)
{
    static bool side_btn_processing = false;
    if (side_btn_processing)
        return;
    side_btn_processing = true;
    switch (state)
    {
    case SIDE_BTN_CLICKED:
        lv_obj_t *scr = lv_screen_active();
        if (scr == eos_watchface_get_screen())
        {
            eos_watchface_delete();
            eos_app_list_create_async();
        }
        else if (scr == eos_app_list_get_screen())
        {
            eos_watchface_create_async();
        }
        else
        {
            EOS_LOG_D("Nav Back");
            eos_nav_back_clean();
        }
    default:
        break;
    }
    side_btn_processing = false;
}

eos_result_t eos_run(void)
{
    /************************** 系统组件初始化 **************************/
    eos_event_init();
#ifdef EOS_USE_FONT_TTF
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
                      &EOS_FONT_C_NAME);
    }
#elif EOS_USE_FONT_C
    eos_theme_set(lv_palette_main(LV_PALETTE_BLUE),
                  lv_palette_main(LV_PALETTE_RED),
                  &EOS_FONT_C_NAME);
#else
    eos_theme_set(lv_palette_main(LV_PALETTE_BLUE),
                  lv_palette_main(LV_PALETTE_RED),
                  &lv_font_montserrat_30);
#endif /* EOS_USE_FONT_TTF */
    eos_app_init();
    eos_watchface_init();
    eos_sys_init();
    eos_lang_init();
    eos_lang_set(LANG_EN);

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
    /************************** 基础部件初始化 **************************/
    eos_app_header_init();

    /************************** 系统启动 **************************/
    eos_watchface_create(); // 加载表盘
    // 开始绘制
    while (1)
    {
        uint32_t delay = lv_timer_handler();
        eos_delay(delay);
    }

    // lv_obj_clean(eos_watchface_get_screen()); // 当且仅当表盘切换时或表盘更新时才清理

    // while (1)
    // {

    //     if (lv_screen_active() == eos_watchface_get_screen())
    //     {
    //         // 判断有没有回到表盘页面，如果回到了，就退出刷新
    //         break;
    //     }
    //     if (side_btn_state == SIDE_BTN_CLICKED)
    //     {
    //         side_btn_state = SIDE_BTN_RELEASED;
    //         if (lv_screen_active() != eos_watchface_get_screen())
    //         {
    //             eos_nav_back_clean();
    //         }
    //     }
    // }
}
