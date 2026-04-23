/**
 * @file elena_os_test.c
 * @brief System function test
 * @author Sab1e
 * @date 2025-08-20
 */

#include "elena_os_config.h"
#if EOS_SYSTEM_MODE == TEST_MODE
#include "elena_os_test.h"

/* Includes ---------------------------------------------------*/
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elena_os_swipe_panel.h"
#include "lvgl.h"
#include "elena_os_img.h"
#include "elena_os_msg_list.h"
#include "elena_os_lang.h"
#define EOS_LOG_TAG "Test"
#include "elena_os_log.h"
#include "elena_os_basic_widgets.h"
#include "elena_os_std_widgets.h"
#include "elena_os_event.h"
#include "elena_os_port.h"
#include "elena_os_app.h"
#include "elena_os_app_list.h"
#include "elena_os_core.h"
#include "script_engine_core.h"
#include "elena_os_misc.h"
#include "elena_os_watchface_list.h"
#include "elena_os_icon.h"
#include "elena_os_toast.h"
#include "elena_os_slide_widget.h"
#include "elena_os_font.h"
#include "elena_os_sensor.h"
#include "elena_os_crown.h"
#include "elena_os_app_header.h"
#include "elena_os_fs.h"
#include "elena_os_mem.h"
#include "elena_os_theme.h"

/* Macros and Definitions -------------------------------------*/
// #define TEST_USE_ZH_FONT
#ifdef TEST_USE_ZH_FONT
LV_FONT_DECLARE(eos_font_resource_han_rounded_30);
#endif
#define LV_KB_BTN(width) LV_BUTTONMATRIX_CTRL_POPOVER | width

/* Variables --------------------------------------------------*/
static lv_obj_t *img = NULL;    // Global image object
static lv_obj_t *ta = NULL;     // Global text input object
extern script_pkg_t script_pkg; // Script package
/* Function Implementations -----------------------------------*/

typedef struct
{
    const char *symbol;
    uint32_t codepoint;
} symbol_t;

typedef struct
{
    lv_obj_t *launcher_screen;
    lv_obj_t *list_screen;
    lv_obj_t *debug_bar;
    char *current_app_id;
    bool debug_active;
    bool global_cb_registered;
} test_app_debug_ctx_t;

// All LVGL built-in symbols
static const symbol_t lv_symbols[] = {
    {LV_SYMBOL_AUDIO, 0xF001},
    {LV_SYMBOL_VIDEO, 0xF008},
    {LV_SYMBOL_LIST, 0xF00B},
    {LV_SYMBOL_OK, 0xF00C},
    {LV_SYMBOL_CLOSE, 0xF00D},
    {LV_SYMBOL_POWER, 0xF011},
    {LV_SYMBOL_SETTINGS, 0xF013},
    {LV_SYMBOL_HOME, 0xF015},
    {LV_SYMBOL_DOWNLOAD, 0xF019},
    {LV_SYMBOL_DRIVE, 0xF01C},
    {LV_SYMBOL_REFRESH, 0xF021},
    {LV_SYMBOL_MUTE, 0xF026},
    {LV_SYMBOL_VOLUME_MID, 0xF027},
    {LV_SYMBOL_VOLUME_MAX, 0xF028},
    {LV_SYMBOL_IMAGE, 0xF03E},
    {LV_SYMBOL_TINT, 0xF043},
    {LV_SYMBOL_PREV, 0xF048},
    {LV_SYMBOL_PLAY, 0xF04B},
    {LV_SYMBOL_PAUSE, 0xF04C},
    {LV_SYMBOL_STOP, 0xF04D},
    {LV_SYMBOL_NEXT, 0xF051},
    {LV_SYMBOL_EJECT, 0xF052},
    {LV_SYMBOL_LEFT, 0xF053},
    {LV_SYMBOL_RIGHT, 0xF054},
    {LV_SYMBOL_PLUS, 0xF067},
    {LV_SYMBOL_MINUS, 0xF068},
    {LV_SYMBOL_EYE_OPEN, 0xF06E},
    {LV_SYMBOL_EYE_CLOSE, 0xF070},
    {LV_SYMBOL_WARNING, 0xF071},
    {LV_SYMBOL_SHUFFLE, 0xF074},
    {LV_SYMBOL_UP, 0xF077},
    {LV_SYMBOL_DOWN, 0xF078},
    {LV_SYMBOL_LOOP, 0xF079},
    {LV_SYMBOL_DIRECTORY, 0xF07B},
    {LV_SYMBOL_UPLOAD, 0xF093},
    {LV_SYMBOL_CALL, 0xF095},
    {LV_SYMBOL_CUT, 0xF0C4},
    {LV_SYMBOL_COPY, 0xF0C5},
    {LV_SYMBOL_SAVE, 0xF0C7},
    {LV_SYMBOL_BARS, 0xF0C9},
    {LV_SYMBOL_ENVELOPE, 0xF0E0},
    {LV_SYMBOL_CHARGE, 0xF0E7},
    {LV_SYMBOL_PASTE, 0xF0EA},
    {LV_SYMBOL_BELL, 0xF0F3},
    {LV_SYMBOL_KEYBOARD, 0xF11C},
    {LV_SYMBOL_GPS, 0xF124},
    {LV_SYMBOL_FILE, 0xF158},
    {LV_SYMBOL_WIFI, 0xF1EB},
    {LV_SYMBOL_BATTERY_FULL, 0xF240},
    {LV_SYMBOL_BATTERY_3, 0xF241},
    {LV_SYMBOL_BATTERY_2, 0xF242},
    {LV_SYMBOL_BATTERY_1, 0xF243},
    {LV_SYMBOL_BATTERY_EMPTY, 0xF244},
    {LV_SYMBOL_USB, 0xF287},
    {LV_SYMBOL_BLUETOOTH, 0xF293},
    {LV_SYMBOL_TRASH, 0xF2ED},
    {LV_SYMBOL_EDIT, 0xF304},
    {LV_SYMBOL_BACKSPACE, 0xF55A},
    {LV_SYMBOL_SD_CARD, 0xF7C2},
    {LV_SYMBOL_NEW_LINE, 0xF8A2},
    {LV_SYMBOL_DUMMY, 0xFFFF},
};

static test_app_debug_ctx_t s_test_app_debug = {0};
static lv_coord_t s_debug_bar_global_x = 0;
static lv_coord_t s_debug_bar_global_y = 0;
static bool s_debug_bar_global_pos_valid = false;

#define TEST_APP_DEBUG_BAR_W 220
#define TEST_APP_DEBUG_BAR_H 64

static void _test_app_debug_clamp_bar_pos(int32_t *x, int32_t *y, int32_t w, int32_t h)
{
    if (!(x && y))
        return;

    if (*x < 0) *x = 0;
    if (*y < 0) *y = 0;
    if (*x + w > EOS_DISPLAY_WIDTH) *x = EOS_DISPLAY_WIDTH - w;
    if (*y + h > EOS_DISPLAY_HEIGHT) *y = EOS_DISPLAY_HEIGHT - h;
    if (*x < 0) *x = 0;
    if (*y < 0) *y = 0;
}

static void _test_app_debug_set_global_bar_pos(int32_t x, int32_t y, int32_t w, int32_t h)
{
    _test_app_debug_clamp_bar_pos(&x, &y, w, h);
    s_debug_bar_global_x = x;
    s_debug_bar_global_y = y;
    s_debug_bar_global_pos_valid = true;
}

static void _test_app_debug_save_bar_pos(lv_obj_t *bar)
{
    if (!(bar && lv_obj_is_valid(bar)))
        return;

    int32_t x = lv_obj_get_x(bar);
    int32_t y = lv_obj_get_y(bar);
    int32_t w = lv_obj_get_width(bar);
    int32_t h = lv_obj_get_height(bar);
    if (w <= 0) w = TEST_APP_DEBUG_BAR_W;
    if (h <= 0) h = TEST_APP_DEBUG_BAR_H;

    _test_app_debug_set_global_bar_pos(x, y, w, h);
}

static void _test_app_debug_sync_bar_pos(void)
{
    lv_obj_t *bar = s_test_app_debug.debug_bar;
    if (!(bar && lv_obj_is_valid(bar) && s_debug_bar_global_pos_valid))
        return;

    int32_t x = s_debug_bar_global_x;
    int32_t y = s_debug_bar_global_y;
    _test_app_debug_clamp_bar_pos(&x, &y, lv_obj_get_width(bar), lv_obj_get_height(bar));

    lv_obj_set_pos(bar, x, y);
    s_debug_bar_global_x = x;
    s_debug_bar_global_y = y;
}

static void _test_app_debug_clear_current_app_id(void)
{
    if (s_test_app_debug.current_app_id)
    {
        eos_free(s_test_app_debug.current_app_id);
        s_test_app_debug.current_app_id = NULL;
    }
}

static void _test_app_debug_destroy_bar_internal(bool save_pos)
{
    if (s_test_app_debug.debug_bar && lv_obj_is_valid(s_test_app_debug.debug_bar))
    {
        if (save_pos)
        {
            _test_app_debug_save_bar_pos(s_test_app_debug.debug_bar);
        }
        lv_obj_delete_async(s_test_app_debug.debug_bar);
    }
    s_test_app_debug.debug_bar = NULL;
}

static void _test_app_debug_destroy_bar(void)
{
    _test_app_debug_destroy_bar_internal(true);
}

static void _test_app_debug_create_bar(void);

static void _test_app_debug_script_exited_cb(lv_event_t *e)
{
    LV_UNUSED(e);

    if (!s_test_app_debug.debug_active)
        return;

    /* Do not overwrite persisted position in script-exit timing path. */
    _test_app_debug_destroy_bar_internal(false);
    s_test_app_debug.debug_active = false;
    _test_app_debug_clear_current_app_id();
}

static void _test_app_debug_global_screen_loaded_cb(lv_event_t *e)
{
    lv_obj_t *scr = lv_event_get_param(e);
    if (!s_test_app_debug.debug_active)
        return;
    if (!(scr && lv_obj_is_valid(scr) && lv_obj_has_class(scr, &lv_obj_class)))
        return;

    eos_app_header_hide();
    if (s_test_app_debug.debug_bar && lv_obj_is_valid(s_test_app_debug.debug_bar))
    {
        _test_app_debug_sync_bar_pos();
        lv_obj_move_foreground(s_test_app_debug.debug_bar);
    }
}

static void _test_app_debug_register_global_cb(void)
{
    if (s_test_app_debug.global_cb_registered)
        return;

    // eos_event_add_global_cb(_test_app_debug_global_screen_loaded_cb,
    //                         EOS_EVENT_GLOBAL_SCREEN_LOADED,
    //                         NULL);
    eos_event_add_global_cb(_test_app_debug_script_exited_cb,
                            EOS_EVENT_SCRIPT_EXITED,
                            NULL);
    s_test_app_debug.global_cb_registered = true;
}

static void _test_app_debug_unregister_global_cb(void)
{
    if (!s_test_app_debug.global_cb_registered)
        return;

    eos_event_remove_all_global_cbs(_test_app_debug_global_screen_loaded_cb);
    eos_event_remove_all_global_cbs(_test_app_debug_script_exited_cb);
    eos_event_cleanup_now();
    s_test_app_debug.global_cb_registered = false;
}

static script_engine_result_t _test_app_debug_create_pkg(const char *app_id, script_pkg_t **out_pkg)
{
    if (!(app_id && out_pkg))
        return -SE_ERR_NULL_PACKAGE;

    char manifest_path[PATH_MAX];
    snprintf(manifest_path, sizeof(manifest_path), EOS_APP_INSTALLED_DIR "%s/" EOS_APP_MANIFEST_FILE_NAME,
             app_id);

    script_pkg_t *pkg = eos_malloc_zeroed(sizeof(script_pkg_t));
    if (!pkg)
        return -SE_ERR_MALLOC;

    pkg->type = SCRIPT_TYPE_APPLICATION;
    if (script_engine_get_manifest(manifest_path, pkg) != SE_OK)
    {
        EOS_LOG_E("Read manifest failed: %s", manifest_path);
        eos_free(pkg);
        return -SE_FAILED;
    }

    char script_path[PATH_MAX];
    snprintf(script_path, sizeof(script_path), EOS_APP_INSTALLED_DIR "%s/" EOS_APP_SCRIPT_ENTRY_FILE_NAME,
             app_id);

    char base_path[PATH_MAX];
    snprintf(base_path, sizeof(base_path), EOS_APP_INSTALLED_DIR "%s/", app_id);
    pkg->base_path = eos_strdup(base_path);

    if (!eos_is_file(script_path))
    {
        EOS_LOG_E("Can't find script: %s", script_path);
        eos_pkg_free(pkg);
        eos_free(pkg);
        return -SE_FAILED;
    }

    pkg->script_str = eos_fs_read_file(script_path);
    if (!pkg->script_str)
    {
        eos_pkg_free(pkg);
        eos_free(pkg);
        return -SE_FAILED;
    }

    *out_pkg = pkg;
    return SE_OK;
}

static void _test_app_debug_show_error(lv_obj_t *scr, const char *app_id, script_engine_result_t ret)
{
    lv_obj_clean(scr);
    lv_obj_remove_style_all(scr);
    // lv_obj_add_style(scr, eos_theme_get_view_style(), 0);

    lv_obj_t *list = eos_std_info_create(
        scr,
        EOS_THEME_DANGEROS_COLOR,
        RI_BUG_LINE,
        eos_lang_get_text(STR_ID_APP_RUN_ERR_TITLE),
        eos_lang_get_text(STR_ID_APP_RUN_ERR));

    char info_str[1024];
    snprintf(info_str, sizeof(info_str), "Code: %d\nAppID: %s\nError: %s",
             ret,
             app_id,
             script_engine_get_error_info());
    eos_list_add_comment(list, info_str);
}

static void _test_app_debug_restore_after_error(const char *app_id)
{
    if (!app_id)
        return;

    s_test_app_debug.debug_active = true;
    _test_app_debug_clear_current_app_id();
    s_test_app_debug.current_app_id = (char *)eos_strdup(app_id);
    _test_app_debug_create_bar();
    _test_app_debug_sync_bar_pos();
}

static script_engine_result_t _test_app_debug_start_internal(const char *app_id)
{
    // if (!(app_id && s_test_app_debug.list_screen && lv_obj_is_valid(s_test_app_debug.list_screen)))
    //     return -SE_ERR_NULL_PACKAGE;

    // if (script_engine_get_state() != SCRIPT_STATE_STOPPED &&
    //     script_engine_get_state() != SCRIPT_STATE_ERROR)
    // {
    //     script_engine_request_stop();
    // }

    // _test_app_debug_register_global_cb();
    // _test_app_debug_clear_current_app_id();
    // s_test_app_debug.current_app_id = (char *)eos_strdup(app_id);
    // s_test_app_debug.debug_active = true;

    // lv_obj_t *scr = eos_nav_init(s_test_app_debug.list_screen);
    // if (!scr)
    // {
    //     s_test_app_debug.debug_active = false;
    //     return -SE_FAILED;
    // }

    // script_pkg_t *pkg = NULL;
    // script_engine_result_t ret = _test_app_debug_create_pkg(app_id, &pkg);
    // if (ret != SE_OK)
    // {
    //     s_test_app_debug.debug_active = false;
    //     return ret;
    // }

    // eos_screen_load_without_anim(scr);
    // ret = script_engine_run(pkg);
    // if (ret != SE_OK)
    // {
    //     _test_app_debug_show_error(scr, app_id, ret);
    //     _test_app_debug_restore_after_error(app_id);
    // }

    // return ret;
}

static void _test_app_debug_safe_nav_cleanup(void)
{
    if (!eos_nav_get_initialized())
        return;

    lv_obj_t *launcher = s_test_app_debug.list_screen;
    if (launcher && lv_obj_is_valid(launcher) && lv_obj_has_class(launcher, &lv_obj_class))
    {
        // First return to debug list without animation to avoid brief blank screen during stack cleanup.
        eos_screen_load_without_anim(launcher);
    }

    eos_nav_clean_up();
}

static void _test_app_debug_exit_current_app(void)
{
    if (script_engine_get_state() == SCRIPT_STATE_RUNNING ||
        script_engine_get_state() == SCRIPT_STATE_SUSPEND ||
        script_engine_get_state() == SCRIPT_STATE_ERROR)
    {
        script_engine_request_stop();
    }

    s_test_app_debug.debug_active = false;
    _test_app_debug_destroy_bar();
    _test_app_debug_clear_current_app_id();
    _test_app_debug_unregister_global_cb();

    _test_app_debug_safe_nav_cleanup();
}

static void _test_app_debug_restart_current_app(void)
{
    if (!s_test_app_debug.current_app_id)
        return;

    char *app_id = (char *)eos_strdup(s_test_app_debug.current_app_id);
    if (!app_id)
        return;

    if (script_engine_get_state() == SCRIPT_STATE_RUNNING ||
        script_engine_get_state() == SCRIPT_STATE_SUSPEND ||
        script_engine_get_state() == SCRIPT_STATE_ERROR)
    {
        script_engine_request_stop();
    }

    _test_app_debug_safe_nav_cleanup();

    _test_app_debug_create_bar();
    _test_app_debug_start_internal(app_id);
    eos_free(app_id);
}

static void _test_app_debug_restart_btn_cb(lv_event_t *e)
{
    _test_app_debug_restart_current_app();
}

static void _test_app_debug_exit_btn_cb(lv_event_t *e)
{
    _test_app_debug_exit_current_app();
}

/* Drag handle callback: PRESSED=highlight, PRESSING=move bar, RELEASED=restore */
static void _test_app_debug_drag_handle_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *handle = lv_event_get_target(e);
    lv_obj_t *bar    = lv_obj_get_parent(handle);

    if (code == LV_EVENT_PRESSING) {
        lv_indev_t *indev = lv_indev_active();
        if (!indev) return;
        lv_point_t vect;
        lv_indev_get_vect(indev, &vect);
        if (vect.x == 0 && vect.y == 0) return;

        int32_t new_x  = lv_obj_get_x(bar) + vect.x;
        int32_t new_y  = lv_obj_get_y(bar) + vect.y;
        int32_t bar_w  = lv_obj_get_width(bar);
        int32_t bar_h  = lv_obj_get_height(bar);

        if (new_x < 0) new_x = 0;
        if (new_y < 0) new_y = 0;
        if (new_x + bar_w > EOS_DISPLAY_WIDTH)  new_x = EOS_DISPLAY_WIDTH  - bar_w;
        if (new_y + bar_h > EOS_DISPLAY_HEIGHT) new_y = EOS_DISPLAY_HEIGHT - bar_h;

        lv_obj_set_pos(bar, new_x, new_y);
        _test_app_debug_set_global_bar_pos(new_x, new_y, bar_w, bar_h);
    } else if (code == LV_EVENT_PRESSED) {
        lv_obj_set_style_bg_color(handle, lv_color_hex(0x3A4550), 0);
        lv_obj_set_style_bg_opa(handle, LV_OPA_COVER, 0);
        lv_obj_set_style_text_color(handle, lv_color_hex(0xFFFFFF), 0);
    } else if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
        _test_app_debug_save_bar_pos(bar);
        lv_obj_set_style_bg_opa(handle, LV_OPA_TRANSP, 0);
        lv_obj_set_style_text_color(handle, lv_color_hex(0x8090A0), 0);
    }
}

static void _test_app_debug_create_bar(void)
{
    _test_app_debug_destroy_bar();

    lv_obj_t *bar = lv_obj_create(lv_layer_top());
    s_test_app_debug.debug_bar = bar;
    lv_obj_remove_style_all(bar);

    const int32_t bar_w = TEST_APP_DEBUG_BAR_W;
    const int32_t bar_h = TEST_APP_DEBUG_BAR_H;
    int32_t bar_x = (EOS_DISPLAY_WIDTH - bar_w) / 2;
    int32_t bar_y = 18;

    if (s_debug_bar_global_pos_valid)
    {
        bar_x = s_debug_bar_global_x;
        bar_y = s_debug_bar_global_y;
    }

    _test_app_debug_clamp_bar_pos(&bar_x, &bar_y, bar_w, bar_h);

    lv_obj_set_size(bar, bar_w, bar_h);
    lv_obj_set_pos(bar, bar_x, bar_y);
    lv_obj_set_style_radius(bar, 20, 0);
    lv_obj_set_style_bg_color(bar, lv_color_hex(0x101418), 0);
    lv_obj_set_style_bg_opa(bar, LV_OPA_90, 0);
    lv_obj_set_style_border_width(bar, 0, 0);
    lv_obj_set_style_pad_all(bar, 8, 0);
    lv_obj_set_style_pad_column(bar, 8, 0);
    lv_obj_set_layout(bar, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(bar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(bar,
                          LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);

    _test_app_debug_set_global_bar_pos(bar_x, bar_y, bar_w, bar_h);

    /* Drag handle (left side) */
    lv_obj_t *drag_handle = lv_obj_create(bar);
    lv_obj_remove_style_all(drag_handle);
    lv_obj_set_size(drag_handle, 44, lv_pct(100));
    lv_obj_set_style_radius(drag_handle, 12, 0);
    lv_obj_set_style_bg_opa(drag_handle, LV_OPA_TRANSP, 0);
    lv_obj_set_style_text_color(drag_handle, lv_color_hex(0x8090A0), 0);
    lv_obj_add_flag(drag_handle, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(drag_handle, _test_app_debug_drag_handle_cb, LV_EVENT_ALL, NULL);
    lv_obj_t *drag_icon = lv_label_create(drag_handle);
    lv_label_set_text(drag_icon, RI_DRAG_MOVE_LINE);
    lv_obj_center(drag_icon);

    /* Restart button (icon only) */
    lv_obj_t *restart_btn = lv_button_create(bar);
    lv_obj_set_size(restart_btn, 72, lv_pct(100));
    lv_obj_add_event_cb(restart_btn, _test_app_debug_restart_btn_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *restart_icon = lv_label_create(restart_btn);
    lv_label_set_text(restart_icon, RI_RESTART_LINE);
    lv_obj_center(restart_icon);

    /* Exit button (icon only) */
    lv_obj_t *exit_btn = lv_button_create(bar);
    lv_obj_set_size(exit_btn, 72, lv_pct(100));
    lv_obj_set_style_bg_color(exit_btn, EOS_THEME_DANGEROS_COLOR, 0);
    lv_obj_add_event_cb(exit_btn, _test_app_debug_exit_btn_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *exit_icon = lv_label_create(exit_btn);
    lv_label_set_text(exit_icon, RI_CLOSE_FILL);
    lv_obj_center(exit_icon);
}

static void _test_app_debug_app_btn_cb(lv_event_t *e)
{
    const char *app_id = (const char *)lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(app_id);

    _test_app_debug_create_bar();
    _test_app_debug_start_internal(app_id);
}

static void _test_app_debug_app_btn_create(lv_obj_t *parent, const char *app_id)
{
    char icon_path[PATH_MAX];
    snprintf(icon_path, sizeof(icon_path), EOS_APP_INSTALLED_DIR "%s/" EOS_APP_ICON_FILE_NAME,
             app_id);
    if (!eos_is_file(icon_path))
    {
        memcpy(icon_path, EOS_IMG_APP, sizeof(EOS_IMG_APP));
    }

    char manifest_path[PATH_MAX];
    snprintf(manifest_path, sizeof(manifest_path), EOS_APP_INSTALLED_DIR "%s/" EOS_APP_MANIFEST_FILE_NAME,
             app_id);

    script_pkg_t pkg = {0};
    if (script_engine_get_manifest(manifest_path, &pkg) != SE_OK)
    {
        EOS_LOG_E("Read manifest failed: %s", manifest_path);
        return;
    }

    lv_obj_t *btn = eos_list_add_button(parent, icon_path, pkg.name);
    lv_obj_add_event_cb(btn, _test_app_debug_app_btn_cb, LV_EVENT_CLICKED, (void *)app_id);
    eos_app_obj_auto_delete(btn, app_id);
    eos_pkg_free(&pkg);
}

static void _test_app_debug_app_installed_cb(lv_event_t *e)
{
    lv_obj_t *parent = lv_event_get_target(e);
    const char *installed_app_id = lv_event_get_param(e);
    EOS_CHECK_PTR_RETURN(parent && installed_app_id);

    _test_app_debug_app_btn_create(parent, installed_app_id);
}

static void _test_app_debug_list_delete_cb(lv_event_t *e)
{
    lv_obj_t *scr = lv_event_get_target(e);
    if (scr == s_test_app_debug.list_screen)
    {
        s_test_app_debug.list_screen = NULL;
        s_test_app_debug.launcher_screen = NULL;
    }
    _test_app_debug_destroy_bar();
    _test_app_debug_clear_current_app_id();
    _test_app_debug_unregister_global_cb();
    s_test_app_debug.debug_active = false;
}

static void _test_app_debug_back_to_test_cb(lv_event_t *e)
{
    lv_obj_t *list_screen = s_test_app_debug.list_screen;
    lv_obj_t *launcher_screen = s_test_app_debug.launcher_screen;

    _test_app_debug_destroy_bar();
    _test_app_debug_clear_current_app_id();
    _test_app_debug_unregister_global_cb();
    s_test_app_debug.debug_active = false;

    if (launcher_screen && lv_obj_is_valid(launcher_screen))
    {
        eos_screen_load_without_anim(launcher_screen);
    }

    if (list_screen && lv_obj_is_valid(list_screen))
    {
        lv_obj_delete_async(list_screen);
    }
}

static void _test_app_debugger(void)
{
    if (s_test_app_debug.list_screen && lv_obj_is_valid(s_test_app_debug.list_screen))
    {
        lv_obj_delete_async(s_test_app_debug.list_screen);
        s_test_app_debug.list_screen = NULL;
    }

    s_test_app_debug.launcher_screen = eos_screen_active();
    s_test_app_debug.list_screen = eos_screen_create();

    lv_obj_t *scr = s_test_app_debug.list_screen;
    lv_obj_add_event_cb(scr, _test_app_debug_list_delete_cb, LV_EVENT_DELETE, NULL);
    lv_obj_set_layout(scr, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(scr,
                          LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_all(scr, 18, 0);
    lv_obj_set_style_pad_row(scr, 10, 0);

    lv_obj_t *toolbar = lv_obj_create(scr);
    lv_obj_set_size(toolbar, lv_pct(100), 96);
    lv_obj_set_style_border_width(toolbar, 0, 0);
    lv_obj_set_style_bg_opa(toolbar, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(toolbar, 0, 0);

    lv_obj_t *back_btn = lv_button_create(toolbar);
    lv_obj_set_size(back_btn, 68, 68);
    lv_obj_align(back_btn, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_add_event_cb(back_btn, _test_app_debug_back_to_test_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, LV_SYMBOL_LEFT);
    lv_obj_center(back_label);

    lv_obj_t *title = lv_label_create(toolbar);
    lv_label_set_text(title, "App Debugger");
    eos_label_set_font_size(title, EOS_FONT_SIZE_LARGE);
    lv_obj_align(title, LV_ALIGN_RIGHT_MID, 0, 0);

    lv_obj_t *app_list = lv_list_create(scr);
    lv_obj_set_width(app_list, lv_pct(100));
    lv_obj_set_flex_grow(app_list, 1);
    eos_event_add_cb(app_list, _test_app_debug_app_installed_cb, EOS_EVENT_APP_INSTALLED, NULL);

    size_t app_list_size = eos_app_get_installed();
    for (size_t i = 0; i < app_list_size; i++)
    {
        _test_app_debug_app_btn_create(app_list, eos_app_list_get_id(i));
    }

    eos_crown_encoder_set_target_obj(app_list);
    eos_screen_load(scr);
}

lv_obj_t *_create_new_scr()
{
    lv_obj_t *scr = eos_nav_init(eos_screen_active());
    eos_app_header_bind_screen(scr, "ElenaOS Test");
    eos_screen_load(scr);
    return scr;
}

static void _test_msg_list_cb(lv_event_t *e)
{
    eos_msg_list_t *msg_list = lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(msg_list);
    char *message = "Sab1e: No one's born being good at all things."
                    "You become good at things through hard work. "
                    "You're not a varsity athlete the first time "
                    "you play a new sport.";

    // 添加消息项
    eos_msg_list_item_t *item = eos_msg_list_item_create(msg_list);
    // 设置内容
    eos_msg_list_item_set_title(item, "Settings");
    eos_msg_list_item_set_msg(item, message);
    eos_msg_list_item_set_time(item, "12:30");

    eos_msg_list_item_icon_set_src(item, EOS_IMG_SETTINGS);

    eos_msg_list_item_t *item1 = eos_msg_list_item_create(msg_list);
    eos_msg_list_item_set_title(item1, "QQ");
    eos_msg_list_item_set_msg(item1, message);
    eos_msg_list_item_set_time(item1, "21:00");
}

static void _test_msg_list()
{
    _create_new_scr();
    eos_msg_list_t *msg_list = eos_msg_list_get_instance();
    EOS_CHECK_PTR_RETURN(msg_list);
    lv_obj_t *btn = lv_button_create(eos_screen_active());
    lv_obj_center(btn);
    lv_obj_t *btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, RI_CHAT_FOLLOW_UP_FILL " Add new message");
    lv_obj_add_event_cb(btn, _test_msg_list_cb, LV_EVENT_CLICKED, msg_list);
}

static void _nav_init_global_cb(lv_event_t *e)
{
    (*(int32_t *)lv_event_get_user_data(e)) = 0;
}

static void _back_prev_global_cb(lv_event_t *e)
{
    (*(int32_t *)lv_event_get_user_data(e))--;
}

static void _test_nav_cb_1(lv_event_t *e)
{
    static int32_t nav_counter = 0;
    static bool cb_reg = false;
    if (!cb_reg)
    {
        eos_event_add_global_cb(_nav_init_global_cb, EOS_EVENT_NAVIGATION_INIT, &nav_counter);
        eos_event_add_global_cb(_back_prev_global_cb, EOS_EVENT_NAVIGATION_BACK_PREV, &nav_counter);
        cb_reg = true;
    }

    lv_obj_t *scr;
    if (!eos_nav_get_initialized())
    {
        scr = eos_nav_init(eos_screen_active());
    }
    else
    {
        scr = eos_nav_scr_create();
    }
    char title_str[64];
    snprintf(title_str, sizeof(title_str), "Screen %d", nav_counter);
    EOS_LOG_D("%s", title_str);
    eos_app_header_bind_screen(scr, title_str);
    eos_screen_load(scr);

    lv_obj_t *new_scr_btn = lv_button_create(scr);
    lv_obj_t *label = lv_label_create(new_scr_btn);
    lv_label_set_text(label, "Create new scr");
    lv_obj_center(label);
    lv_obj_add_event_cb(new_scr_btn, _test_nav_cb_1, LV_EVENT_CLICKED, NULL);

#if 0
    uint32_t w = 250, h = 80;
    lv_obj_set_size(new_scr_btn, w, h);
    int32_t x = rand() % (EOS_DISPLAY_WIDTH - w);
    int32_t y = rand() % (EOS_DISPLAY_HEIGHT - h);
    if (y < 130)
        y += 130;
    lv_obj_set_pos(new_scr_btn, x, y);
#else
    lv_obj_center(new_scr_btn);
#endif

    nav_counter++;
}

static void _test_font()
{
    _create_new_scr();

    const char *test_str = /* 中文符号测试 */ "，。、：；？！“”‘’（）【】《》〈〉——……·＋－×÷＝≠＞＜≥≤≈±￥％‰℃°＠＃＆☆★●○■□▲△▼▽"
                                              /* 英文符号测试 */ "~!@#$%^&*()-_=+[]{}\\|;:'\",./<>?`©®™"
                                              /* 希腊字母测试 */ "ΑαΒβΓγΔδΕεΖζΗηΘθΙιΚκΛλΜμΝνΞξΟοΠπΡρΣσΤτΥυΦφΧχΨψΩω"
                                              /* 英文数字测试 */ "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890"
                                              /* 常用汉字测试 */ "在夏末的午后，风把阳台上的风铃吹得叮当作响，像是某种不经意的暗号。"
                                              /* 罕见汉字测试 */ "霡霂淅沥，薜荔葳蕤。彳亍踟蹰，睥睨娉婷。觊觎饕餮，倥偬倜傥。菡萏猗傩，蘼芜菁菁。";

    lv_obj_t *container = eos_list_create(eos_screen_active());
    lv_obj_set_size(container, lv_pct(100), lv_pct(100));
    lv_obj_t *font_label = lv_label_create(container);
    lv_label_set_text(font_label, test_str);
    lv_obj_set_width(font_label, lv_pct(100));
    lv_label_set_long_mode(font_label, LV_LABEL_LONG_WRAP);
#ifdef TEST_USE_ZH_FONT
    lv_obj_set_style_text_font(font_label, &eos_font_resource_han_rounded_30, LV_PART_MAIN);
#endif
}

static void _test_lang_cb(lv_event_t *e)
{
    if (eos_lang_get_current_id() == LANG_ZH)
    {
        eos_lang_set_current_id(LANG_EN);
    }
    else
    {
        eos_lang_set_current_id(LANG_ZH);
    }
}

static void _test_lang(lv_event_t *e)
{
    _create_new_scr();

    lv_obj_t *label = lv_label_create(eos_screen_active());
    lv_obj_set_width(label, lv_pct(100));
    lv_obj_center(label);
#ifdef TEST_USE_ZH_FONT
    lv_obj_set_style_text_font(label, &eos_font_resource_han_rounded_30, LV_PART_MAIN);
#endif
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_t *btn = lv_button_create(eos_screen_active());
    lv_obj_t *btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, RI_REPEAT_2_FILL " Switch Language");
    lv_obj_add_event_cb(btn, _test_lang_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -20);
}

static void _test_vkb_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *ta = lv_event_get_target(e);
    lv_obj_t *kb = lv_event_get_user_data(e);

    if (code == LV_EVENT_FOCUSED)
    {
        if (lv_indev_get_type(lv_indev_active()) != LV_INDEV_TYPE_KEYPAD)
        {
            lv_keyboard_set_textarea(kb, ta);
            lv_obj_remove_flag(kb, LV_OBJ_FLAG_HIDDEN);
        }
    }
    else if (code == LV_EVENT_CANCEL)
    {
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_state(ta, LV_STATE_FOCUSED);
        lv_indev_reset(NULL, ta); /*To forget the last clicked object to make it focusable again*/
    }
}

static void _test_vkb()
{
    _create_new_scr();
    lv_obj_t *pinyin_ime = lv_ime_pinyin_create(eos_screen_active());
#ifdef TEST_USE_ZH_FONT
    lv_obj_set_style_text_font(pinyin_ime, &eos_font_resource_han_rounded_30, 0);
#endif
    // lv_ime_pinyin_set_dict(pinyin_ime, your_dict); // Use a custom dictionary. If it is not set, the built-in dictionary will be used.

    /* ta1 */
    lv_obj_t *ta1 = lv_textarea_create(eos_screen_active());
    lv_textarea_set_one_line(ta1, true);
#ifdef TEST_USE_ZH_FONT
    lv_obj_set_style_text_font(ta1, &eos_font_resource_han_rounded_30, 0);
#endif
    lv_obj_align(ta1, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_width(ta1, lv_pct(100));

    /*Create a keyboard and add it to ime_pinyin*/
    lv_obj_t *kb = lv_keyboard_create(eos_screen_active());

    lv_ime_pinyin_set_keyboard(pinyin_ime, kb);
    lv_keyboard_set_textarea(kb, ta1);

    lv_obj_add_event_cb(ta1, _test_vkb_event_cb, LV_EVENT_ALL, kb);

    /*Get the cand_panel, and adjust its size and position*/
    lv_obj_t *cand_panel = lv_ime_pinyin_get_cand_panel(pinyin_ime);
    lv_obj_set_size(cand_panel, LV_PCT(100), LV_PCT(10));
    lv_obj_align_to(cand_panel, kb, LV_ALIGN_OUT_TOP_MID, 0, 0);
}

static void _test_image_input_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *kb = lv_event_get_user_data(e);

    if (code == LV_EVENT_CLICKED)
    {
        // 点击文本框时显示键盘
        lv_obj_remove_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
    else if (code == LV_EVENT_READY || code == LV_EVENT_DEFOCUSED)
    {
        // 按下确认键或失去焦点时处理
        const char *path = lv_textarea_get_text(ta);

        if (strlen(path) > 0)
        {
            // 设置图片源
            eos_img_set_src(img, path);

            // 清除文本框内容
            lv_textarea_set_text(ta, "");
        }

        // 隐藏键盘
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
}

static void _test_image()
{
    lv_obj_t *scr = _create_new_scr();
    // 创建图片对象但不设置源
    img = lv_image_create(scr);
    lv_obj_center(img);
    lv_obj_move_background(img);

    // 创建文本输入框
    ta = lv_textarea_create(scr);
    lv_obj_set_size(ta, LV_HOR_RES - 40, 80);
    lv_obj_align(ta, LV_ALIGN_TOP_MID, 0, 20);
    lv_textarea_set_placeholder_text(ta, "Input image path here.(e.g. /flower.bin)");
    lv_textarea_set_one_line(ta, true);

    // 添加键盘
    lv_obj_t *kb = lv_keyboard_create(scr);
    lv_keyboard_set_textarea(kb, ta);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN); // 默认隐藏键盘

    // 添加事件回调
    lv_obj_add_event_cb(ta, _test_image_input_cb, LV_EVENT_ALL, kb);
}

static void _test_app_list()
{
    eos_scene_switch(EOS_SCENE_APP_LIST);
}

static void _test_app_debug_page()
{
    _test_app_debugger();
}

static void _test_watchface_list()
{
    eos_scene_switch(EOS_SCENE_WATCHFACE_LIST);
}

static void _toast_clicked_cb(lv_event_t *e)
{
    eos_scene_switch(EOS_SCENE_APP_LIST);
}

static void _test_toast()
{
    lv_obj_t *toast = eos_toast_show(NULL, "Click me to open App List!");
    lv_obj_add_event_cb(toast, _toast_clicked_cb, LV_EVENT_CLICKED, NULL);
}

static void _test_show_all_lv_symbols_list()
{
    lv_obj_t *scr = _create_new_scr();

    // 创建列表
    lv_obj_t *list = lv_list_create(scr);
    lv_obj_set_size(list, lv_pct(100), lv_pct(100));
    lv_obj_set_style_pad_all(list, 5, 0);

    for (size_t i = 0; i < sizeof(lv_symbols) / sizeof(lv_symbols[0]); i++)
    {
        char buf[64];
        snprintf(buf, sizeof(buf), "%s  U+%04X", lv_symbols[i].symbol, lv_symbols[i].codepoint);

        // 在列表中添加条目
        lv_obj_t *label = lv_list_add_text(list, buf);
        // lv_obj_set_style_text_font(label, &lv_font_montserrat_30, 0); // 设置字体
    }
}

static void _slide_widget_reached_threshold_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    eos_slide_widget_t *sw = (eos_slide_widget_t *)lv_event_get_user_data(e);
    eos_slide_widget_delete(sw);
}

static void _slide_widget_moving_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    lv_obj_t *label = (lv_obj_t *)lv_event_get_user_data(e);
    lv_label_set_text_fmt(label, "(%d,%d)", lv_obj_get_x(obj), lv_obj_get_y(obj));
}

static void _slide_widget_reset_btn_clicked_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    lv_obj_t *target = (lv_obj_t *)lv_event_get_user_data(e);
    lv_obj_set_pos(target, 0, 160);
}

static lv_obj_t *_add_slide_wdiget(lv_obj_t *parent)
{
    lv_obj_t *obj = lv_button_create(parent);

    lv_obj_set_size(obj, EOS_DISPLAY_WIDTH - 100, 100);
    lv_obj_set_style_margin_ver(obj, 10, 0);
    lv_obj_update_layout(obj);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    eos_slide_widget_t *sw = eos_slide_widget_create_with_touch(obj, obj, EOS_SLIDE_DIR_HOR, EOS_DISPLAY_WIDTH, EOS_THRESHOLD_30);
    eos_slide_widget_set_bidirectional(sw, true);
    lv_obj_add_event_cb(sw->touch_obj, _slide_widget_reached_threshold_cb, EOS_EVENT_SLIDE_WIDGET_REACHED_THRESHOLD, sw);

    return obj;
}

static void _test_slide_widget()
{
    lv_obj_t *scr = _create_new_scr();
    lv_obj_remove_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *list = lv_list_create(scr);
    lv_obj_set_size(list, LV_PCT(100), LV_PCT(88));
    lv_obj_center(list);
    lv_obj_set_style_bg_opa(list, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(list, 0, 0);
    lv_obj_set_style_pad_all(list, 30, 0);
    lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_user_data(list, list);
    lv_obj_set_scroll_dir(list, LV_DIR_VER);
    lv_obj_add_flag(list, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *obj = _add_slide_wdiget(list);
    obj = _add_slide_wdiget(list);
    obj = _add_slide_wdiget(list);
    obj = _add_slide_wdiget(list);
    // lv_obj_t *reset_btn = lv_button_create(scr);
    // lv_obj_t *label = lv_label_create(reset_btn);
    // lv_obj_align(reset_btn, LV_ALIGN_BOTTOM_MID, 0, -40);
    // lv_obj_add_event_cb(reset_btn, _slide_widget_reset_btn_clicked_cb, LV_EVENT_CLICKED, obj);
    // lv_obj_add_event_cb(sw->touch_obj, _slide_widget_moving_cb, EOS_EVENT_SLIDE_WIDGET_MOVING, label);
}

static void _test_font_size()
{
    lv_obj_t *scr = _create_new_scr();
    lv_obj_remove_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *list = eos_list_create(scr);
    lv_obj_set_size(list, lv_pct(100), lv_pct(100));

    static const char *test_text = "AaBbCc 你好中国 1234567890";

    lv_obj_t *label = lv_label_create(list);
    lv_label_set_text(label, test_text);
    eos_label_set_font_size(label, EOS_FONT_SIZE_SMALL);

    label = lv_label_create(list);
    lv_label_set_text(label, test_text);
    eos_label_set_font_size(label, EOS_FONT_SIZE_MEDIUM);

    label = lv_label_create(list);
    lv_label_set_text(label, test_text);
    eos_label_set_font_size(label, EOS_FONT_SIZE_LARGE);
}

static void _sensor_meas_cb(lv_event_t *e)
{
    lv_obj_t *tb = lv_event_get_target(e);
    eos_sensor_t *s = lv_event_get_param(e);
    EOS_CHECK_PTR_RETURN(s && tb);
    int32_t i = eos_sensor_get_type_by_event_code(lv_event_get_code(e));

    const uint8_t _SENSOR_VAL_COL = 1;
    switch (i)
    {
    case EOS_SENSOR_TYPE_ACCE: /**< 加速度传感器 */
        lv_table_set_cell_value_fmt(tb, i, _SENSOR_VAL_COL,
                                    "X=%d\nY=%d\nZ=%d",
                                    s->data.acce.x,
                                    s->data.acce.y,
                                    s->data.acce.z);
        break;

    case EOS_SENSOR_TYPE_GYRO: /**< 陀螺仪传感器 */
        lv_table_set_cell_value_fmt(tb, i, _SENSOR_VAL_COL,
                                    "X=%d\nY=%d\nZ=%d",
                                    s->data.gyro.x,
                                    s->data.gyro.y,
                                    s->data.gyro.z);
        break;

    case EOS_SENSOR_TYPE_MAG: /**< 磁传感器 */
        lv_table_set_cell_value_fmt(tb, i, _SENSOR_VAL_COL,
                                    "X=%d\nY=%d\nZ=%d",
                                    s->data.mag.x,
                                    s->data.mag.y,
                                    s->data.mag.z);
        break;

    case EOS_SENSOR_TYPE_TEMP: /**< 温度传感器 */
        lv_table_set_cell_value_fmt(tb, i, _SENSOR_VAL_COL,
                                    "%.2f °C",
                                    s->data.temp.temp / 100.0f);
        break;

    case EOS_SENSOR_TYPE_HUMI: /**< 相对湿度传感器 */
        lv_table_set_cell_value_fmt(tb, i, _SENSOR_VAL_COL,
                                    "%.2f %%RH",
                                    s->data.humi.humidity / 100.0f);
        break;

    case EOS_SENSOR_TYPE_BARO: /**< 气压传感器 */
        lv_table_set_cell_value_fmt(tb, i, _SENSOR_VAL_COL,
                                    "%.2f hPa",
                                    s->data.baro.pressure / 100.0f);
        break;

    case EOS_SENSOR_TYPE_LIGHT: /**< 环境光传感器 */
        lv_table_set_cell_value_fmt(tb, i, _SENSOR_VAL_COL,
                                    "%u lx",
                                    s->data.light.lux);
        break;

    case EOS_SENSOR_TYPE_PROXIMITY: /**< 距离传感器 */
        lv_table_set_cell_value_fmt(tb, i, _SENSOR_VAL_COL,
                                    "%u mm",
                                    s->data.proximity.distance_mm);
        break;

    case EOS_SENSOR_TYPE_HR: /**< 心率传感器 */
        lv_table_set_cell_value_fmt(tb, i, _SENSOR_VAL_COL,
                                    "HR=%u bpm\nSpO₂=%.2f%%",
                                    s->data.hr.heart_rate,
                                    s->data.hr.spo2 / 100.0f);
        break;

    case EOS_SENSOR_TYPE_TVOC: /**< TVOC传感器 */
        lv_table_set_cell_value_fmt(tb, i, _SENSOR_VAL_COL,
                                    "%u ppb",
                                    s->data.tvoc.tvoc);
        break;

    case EOS_SENSOR_TYPE_NOISE: /**< 噪声传感器 */
        lv_table_set_cell_value_fmt(tb, i, _SENSOR_VAL_COL,
                                    "%.2f dB",
                                    s->data.noise.noise_db / 100.0f);
        break;

    case EOS_SENSOR_TYPE_STEP: /**< 计步传感器 */
        lv_table_set_cell_value_fmt(tb, i, _SENSOR_VAL_COL,
                                    "%lu 步",
                                    (unsigned long)s->data.step.steps);
        break;

    case EOS_SENSOR_TYPE_FORCE: /**< 力传感器 */
        lv_table_set_cell_value_fmt(tb, i, _SENSOR_VAL_COL,
                                    "%.2f N",
                                    s->data.force.force / 100.0f);
        break;

    case EOS_SENSOR_TYPE_BAT: /**< 电池电量传感器 */
        lv_table_set_cell_value_fmt(tb, i, _SENSOR_VAL_COL,
                                    "%d%% %s",
                                    s->data.bat.level,
                                    s->data.bat.charging ? "(充电中)" : "(未充电)");
        break;

    case EOS_SENSOR_TYPE_UNKNOWN:
    default:
        lv_table_set_cell_value_fmt(tb, i, _SENSOR_VAL_COL,
                                    "未知传感器类型 (%d)", i);
        break;
    }
}

static void _timer_cb(lv_timer_t *t)
{
}

static void _test_sensor()
{
    lv_obj_t *scr = _create_new_scr();

    lv_obj_t *list = eos_list_create(scr);

    lv_obj_t *tb = lv_table_create(list);
    lv_table_set_row_count(tb, EOS_SENSOR_NUMBER + 1);
    lv_table_set_column_count(tb, 2);
    lv_obj_set_width(tb, lv_pct(100));
    lv_table_set_cell_value(tb, 0, 0, "Sensor");
    lv_table_set_cell_value(tb, 0, 1, "Value");
    lv_table_set_column_width(tb, 0, 180);
    lv_table_set_column_width(tb, 1, 180);

    for (int i = 0; i < EOS_SENSOR_NUMBER; i++)
    {
        lv_table_set_cell_value(tb, i + 1, 0, eos_lang_get_text(STR_ID_SENSOR_START + 1 + i));
    }

    for (int i = 0; i < EOS_SENSOR_NUMBER; i++)
    {
        lv_table_set_cell_value(tb, i + 1, 1, "NaN");
    }

    for (int i = 1; i <= EOS_SENSOR_NUMBER; i++)
    {
        eos_event_add_cb(tb, _sensor_meas_cb, eos_sensor_get_event_code(i), NULL);
        EOS_LOG_D("Sensor event add: %d", eos_sensor_get_event_code(i));
    }
    eos_sensor_read_async((eos_sensor_type_t)EOS_SENSOR_TYPE_ACCE);
}

void eos_test_start(void)
{
    lv_obj_t *scr = eos_screen_active();

    lv_obj_t *test_list = lv_list_create(scr);
    lv_obj_set_size(test_list, lv_pct(100), lv_pct(100));
    eos_crown_encoder_set_target_obj(test_list);

    lv_obj_t *btn;
    lv_obj_t *label = lv_list_add_text(test_list, RI_ELENA_WATCH " ElenaOS Test List");
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    // 测试应用调试器
    btn = lv_list_add_button(test_list, RI_BUG_LINE, "App Debugger");
    lv_obj_add_event_cb(btn, _test_app_debug_page, LV_EVENT_CLICKED, NULL);
    // 传感器测试
    btn = lv_list_add_button(test_list, RI_SENSOR_LINE, "SensorTester");
    lv_obj_add_event_cb(btn, _test_sensor, LV_EVENT_CLICKED, NULL);
    // 测试滑动组件
    btn = lv_list_add_button(test_list, RI_CAROUSEL_VIEW, "Slide Widget");
    lv_obj_add_event_cb(btn, _test_slide_widget, LV_EVENT_CLICKED, NULL);
    // 测试导航功能
    btn = lv_list_add_button(test_list, RI_MENU_UNFOLD_FILL, "Navigation");
    lv_obj_add_event_cb(btn, _test_nav_cb_1, LV_EVENT_CLICKED, NULL);
    // 测试消息列表
    btn = lv_list_add_button(test_list, RI_CHAT_SMILE_FILL, "Message List");
    lv_obj_add_event_cb(btn, _test_msg_list, LV_EVENT_CLICKED, NULL);
    // 测试字体
    btn = lv_list_add_button(test_list, RI_FONT_SANS_SERIF, "Font");
    lv_obj_add_event_cb(btn, _test_font, LV_EVENT_CLICKED, NULL);
    // 测试字号
    btn = lv_list_add_button(test_list, RI_FONT_SIZE, "Font Size");
    lv_obj_add_event_cb(btn, _test_font_size, LV_EVENT_CLICKED, NULL);
    // 测试语言切换
    btn = lv_list_add_button(test_list, RI_TRANSLATE, "Language");
    lv_obj_add_event_cb(btn, _test_lang, LV_EVENT_CLICKED, NULL);
    // 测试虚拟键盘
    btn = lv_list_add_button(test_list, RI_KEYBOARD_BOX_FILL, "Virtual Keyboard");
    lv_obj_add_event_cb(btn, _test_vkb, LV_EVENT_CLICKED, NULL);
    // 测试图像显示
    btn = lv_list_add_button(test_list, RI_IMAGE_FILL, "Image");
    lv_obj_add_event_cb(btn, _test_image, LV_EVENT_CLICKED, NULL);
    // 测试应用列表
    btn = lv_list_add_button(test_list, RI_APPS_FILL, "App List");
    lv_obj_add_event_cb(btn, _test_app_list, LV_EVENT_CLICKED, NULL);
    // 测试表盘列表
    btn = lv_list_add_button(test_list, RI_APPS_FILL, "Watchface List");
    lv_obj_add_event_cb(btn, _test_watchface_list, LV_EVENT_CLICKED, NULL);
    // 测试 Toast
    btn = lv_list_add_button(test_list, RI_MESSAGE_2_FILL, "Toast");
    lv_obj_add_event_cb(btn, _test_toast, LV_EVENT_CLICKED, NULL);
    // 测试 LVGL 默认符号
    btn = lv_list_add_button(test_list, RI_OMEGA, "LVGL Symbols");
    lv_obj_add_event_cb(btn, _test_show_all_lv_symbols_list, LV_EVENT_CLICKED, NULL);
}

#endif /* EOS_SYSTEM_MODE == TEST_MODE */
