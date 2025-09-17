/**
 * @file elena_os_core.c
 * @brief Elena OS 核心代码实现
 * @author Sab1e
 * @date 2025-08-10
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
#include "script_engine_nav.h"
#include "elena_os_theme.h"
#include "elena_os_config.h"
// Macros and Definitions

// Variables
lv_group_t *encoder_group;
lv_obj_t *root_scr;
static eos_side_btn_state_t side_btn_state = SIDE_BTN_RELEASED;
// Function Implementations

static void _watchface_long_pressed_cb(lv_event_t *e)
{
    eos_watchface_list_create();
}

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
        if (scr == root_scr)
        {
            eos_app_list_create();
            side_btn_state = SIDE_BTN_RELEASED;
        }
        else
        {
            side_btn_state = SIDE_BTN_CLICKED;
        }
    default:
        break;
    }
    side_btn_processing = false;
}

static inline eos_result_t _draw_watch_face(void)
{
    // JSON中获取表盘id
    EOS_LOG_D("Loading wf_id");
    char *wf_id = eos_sys_cfg_get_string(EOS_SYS_CFG_KEY_WATCHFACE_ID, "cn.sab1e.clock");
    if (!wf_id)
    {
        EOS_LOG_E("NULL wf_id");
        return -EOS_FAILED;
    }
    // 直接通过表盘id 获取相关信息并存储到script_package
    EOS_LOG_D("Loading manifest");
    char manifest_path[PATH_MAX];
    snprintf(manifest_path, sizeof(manifest_path), EOS_WATCHFACE_INSTALLED_DIR "%s/" EOS_WATCHFACE_MANIFEST_FILE_NAME,
             wf_id);
    script_pkg_t *pkg = malloc(sizeof(script_pkg_t));
    memset((void *)pkg, 0, sizeof(script_pkg_t));
    pkg->type = SCRIPT_TYPE_WATCHFACE;
    EOS_LOG_D("script_engine_get_manifest");
    if (script_engine_get_manifest(manifest_path, pkg) != SE_OK)
    {
        EOS_LOG_E("Read manifest failed: %s", manifest_path);
    }
    EOS_LOG_D("App Info:\n"
              "id=%s | name=%s | version=%s |\n"
              "author:%s | description:%s",
              pkg->id, pkg->name, pkg->version,
              pkg->version, pkg->description);
    char script_path[PATH_MAX];
    snprintf(script_path, sizeof(script_path), EOS_WATCHFACE_INSTALLED_DIR "%s/" EOS_WATCHFACE_SCRIPT_ENTRY_FILE_NAME,
             wf_id);
    free((void *)wf_id);
    if (!eos_is_file(script_path))
    {
        EOS_LOG_E("Can't find script: %s", script_path);
        return -EOS_FAILED;
    }
    pkg->script_str = eos_read_file(script_path);

    // 设置下拉面板
    msg_list_t *msg_list = eos_msg_list_create(root_scr);
    if (!msg_list)
    {
        EOS_LOG_E("Create msg_list failed");
        return -SE_FAILED;
    }
    // 设置上拉面板

    // 设置长按回调 进入 watchface list 使用普通 nav 导航
    lv_obj_add_event_cb(root_scr, _watchface_long_pressed_cb, LV_EVENT_LONG_PRESSED, NULL);
    // 正式运行表盘脚本（脚本禁止阻塞线程）
    script_engine_result_t ret = script_engine_run(pkg);
    if (ret != SE_OK)
    {
        EOS_LOG_E("Script encounter a fatal error");
    }
}

eos_result_t eos_run(void)
{
    /************************** 变量初始化 **************************/
    root_scr = lv_screen_active();
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
    // 加载导航
    eos_nav_init(root_scr);
    eos_lang_set(LANG_EN);

    lv_indev_t *indev = _get_key_indev();
    if (indev)
    {
        encoder_group = lv_indev_get_group(indev);
        lv_group_add_obj(encoder_group, root_scr);
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
    _draw_watch_face();
    // 开始绘制
    while (1)
    {
        uint32_t delay = lv_timer_handler();
        eos_delay(delay);
    }

    // lv_obj_clean(root_scr); // 当且仅当表盘切换时或表盘更新时才清理

    // while (1)
    // {

    //     if (lv_screen_active() == root_scr)
    //     {
    //         // 判断有没有回到表盘页面，如果回到了，就退出刷新
    //         break;
    //     }
    //     if (side_btn_state == SIDE_BTN_CLICKED)
    //     {
    //         side_btn_state = SIDE_BTN_RELEASED;
    //         if (lv_screen_active() != root_scr)
    //         {
    //             eos_nav_back_clean();
    //         }
    //     }
    // }
}
