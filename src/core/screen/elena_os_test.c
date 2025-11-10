/**
 * @file elena_os_test.c
 * @brief 系统功能测试
 * @author Sab1e
 * @date 2025-08-20
 */

#include "elena_os_test.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elena_os_swipe_panel.h"
#include "lvgl.h"
#include "elena_os_img.h"
#include "elena_os_msg_list.h"
#include "elena_os_lang.h"
#include "elena_os_log.h"
#include "elena_os_nav.h"
#include "elena_os_basic_widgets.h"
#include "elena_os_event.h"
#include "elena_os_port.h"
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

/* Macros and Definitions -------------------------------------*/
// #define TEST_USE_ZH_FONT
#ifdef TEST_USE_ZH_FONT
LV_FONT_DECLARE(eos_font_resource_han_rounded_30);
#endif
#define LV_KB_BTN(width) LV_BUTTONMATRIX_CTRL_POPOVER | width

/* Variables --------------------------------------------------*/
static lv_obj_t *img = NULL;    // 全局图片对象
static lv_obj_t *ta = NULL;     // 全局文本输入框对象
extern script_pkg_t script_pkg; // 脚本包
/* Function Implementations -----------------------------------*/

typedef struct
{
    const char *symbol;
    uint32_t codepoint;
} symbol_t;

// 所有 LVGL 内置符号
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

lv_obj_t *_create_new_scr()
{
    lv_obj_t *scr = eos_nav_init(lv_screen_active());
    eos_screen_bind_header(scr, "ElenaOS Test");
    lv_screen_load(scr);
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
    eos_msg_list_t *msg_list = eos_msg_list_create(lv_screen_active());
    EOS_CHECK_PTR_RETURN(msg_list);
    lv_obj_t *btn = lv_button_create(lv_screen_active());
    lv_obj_center(btn);
    lv_obj_t *btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, RI_CHAT_FOLLOW_UP_FILL " Add new message");
    lv_obj_add_event_cb(btn, _test_msg_list_cb, LV_EVENT_CLICKED, msg_list);
}

static void _test_nav_cb_1(lv_event_t *e)
{
    _create_new_scr();

    lv_obj_t *back_btn = eos_back_btn_create(lv_screen_active(), true);
    lv_obj_center(back_btn);
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

    lv_obj_t *container = eos_list_create(lv_screen_active());
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
    if (eos_lang_get() == LANG_ZH)
    {
        eos_lang_set(LANG_EN);
    }
    else
    {
        eos_lang_set(LANG_ZH);
    }
}

static void _test_lang(lv_event_t *e)
{
    _create_new_scr();

    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_obj_set_width(label, lv_pct(100));
    lv_obj_center(label);
#ifdef TEST_USE_ZH_FONT
    lv_obj_set_style_text_font(label, &eos_font_resource_han_rounded_30, LV_PART_MAIN);
#endif
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_t *btn = lv_button_create(lv_screen_active());
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
    lv_obj_t *pinyin_ime = lv_ime_pinyin_create(lv_screen_active());
#ifdef TEST_USE_ZH_FONT
    lv_obj_set_style_text_font(pinyin_ime, &eos_font_resource_han_rounded_30, 0);
#endif
    // lv_ime_pinyin_set_dict(pinyin_ime, your_dict); // Use a custom dictionary. If it is not set, the built-in dictionary will be used.

    /* ta1 */
    lv_obj_t *ta1 = lv_textarea_create(lv_screen_active());
    lv_textarea_set_one_line(ta1, true);
#ifdef TEST_USE_ZH_FONT
    lv_obj_set_style_text_font(ta1, &eos_font_resource_han_rounded_30, 0);
#endif
    lv_obj_align(ta1, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_width(ta1, lv_pct(100));

    /*Create a keyboard and add it to ime_pinyin*/
    lv_obj_t *kb = lv_keyboard_create(lv_screen_active());

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
    eos_app_list_create();
}

static void _test_watchface_list()
{
    eos_watchface_list_create();
}

static void _toast_clicked_cb(lv_event_t *e)
{
    eos_app_list_create();
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
        lv_table_set_cell_value(tb, i + 1, 0, current_lang[STR_ID_SENSOR_START + 1 + i]);
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
    lv_obj_t *scr = lv_screen_active();

    lv_obj_t *test_list = lv_list_create(scr);
    lv_obj_set_size(test_list, lv_pct(100), lv_pct(100));

    lv_obj_t *btn;
    lv_obj_t *label = lv_list_add_text(test_list, RI_ELENA_WATCH " ElenaOS Test List");
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
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
