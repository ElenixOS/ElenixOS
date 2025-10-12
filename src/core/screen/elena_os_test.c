#if 0

/**
 * @file elena_os_test.c
 * @brief 系统功能测试
 * @author Sab1e
 * @date 2025-08-20
 */

/**
 * TODO:
 * 新增上拉快捷控制台（先做设置App）
 */

#include "elena_os_test.h"

// Includes
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
#include "script_engine_nav.h"
#include "elena_os_misc.h"
#include "elena_os_watchface_list.h"
// Macros and Definitions
// #define TEST_USE_ZH_FONT
#ifdef TEST_USE_ZH_FONT
LV_FONT_DECLARE(eos_font_resource_han_rounded_30);
#endif
// Variables
static lv_obj_t *img = NULL;         // 全局图片对象
static lv_obj_t *ta = NULL;          // 全局文本输入框对象
extern script_pkg_t script_pkg; // 脚本包
// Function Implementations

void _create_new_scr()
{
    lv_obj_t *scr = eos_nav_scr_create();
    lv_screen_load(scr);
}

static void _test_msg_list_cb(lv_event_t *e)
{
    msg_list_t *msg_list = lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(msg_list);
    char *message = "Sab1e: No one's born being good at all things."
                    "You become good at things through hard work. "
                    "You're not a varsity athlete the first time "
                    "you play a new sport.";

    // 添加消息项
    msg_list_item_t *item = eos_msg_list_item_create(msg_list);
    // 设置内容
    eos_msg_list_item_set_title(item, "WeChat");
    eos_msg_list_item_set_msg(item, message);
    eos_msg_list_item_set_time(item, "12:30");

    eos_msg_list_item_icon_set_src(item, "/wx.bin");

    msg_list_item_t *item1 = eos_msg_list_item_create(msg_list);
    eos_msg_list_item_set_title(item1, "QQ");
    eos_msg_list_item_set_msg(item1, message);
    eos_msg_list_item_set_time(item1, "21:00");
}

static void _test_msg_list()
{
    _create_new_scr();
    msg_list_t *msg_list = eos_msg_list_create(lv_screen_active());
    EOS_CHECK_PTR_RETURN(msg_list);
    lv_obj_t *btn = lv_button_create(lv_screen_active());
    lv_obj_center(btn);
    lv_obj_t *btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, LV_SYMBOL_PLUS " Add new message");
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

    lv_obj_t *container = lv_obj_create(lv_screen_active());
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

    lv_obj_t *label = eos_lang_label_create(lv_screen_active(), STR_ID_TEST_LANG_STR);
    lv_obj_set_width(label, lv_pct(100));
#ifdef TEST_USE_ZH_FONT
    lv_obj_set_style_text_font(label, &eos_font_resource_han_rounded_30, LV_PART_MAIN);
#endif
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_t *btn = lv_button_create(lv_screen_active());
    lv_obj_t *btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, LV_SYMBOL_LOOP " Switch Language");
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
    _create_new_scr();
    // 创建图片对象但不设置源
    img = lv_image_create(lv_screen_active());
    lv_obj_center(img);
    lv_obj_move_background(img);

    // 创建文本输入框
    ta = lv_textarea_create(lv_screen_active());
    lv_obj_set_size(ta, LV_HOR_RES - 40, 80);
    lv_obj_align(ta, LV_ALIGN_TOP_MID, 0, 20);
    lv_textarea_set_placeholder_text(ta, "Input image path here.(e.g. /flower.bin)");
    lv_textarea_set_one_line(ta, true);

    // 添加键盘
    lv_obj_t *kb = lv_keyboard_create(lv_screen_active());
    lv_keyboard_set_textarea(kb, ta);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN); // 默认隐藏键盘

    // 添加事件回调
    lv_obj_add_event_cb(ta, _test_image_input_cb, LV_EVENT_ALL, kb);
}

static void _test_app_list()
{
    eos_app_list_create_async();
}

static void _test_watchface_list(){
    eos_watchface_list_create_async();
}

void eos_test_start(void)
{
#ifdef DEBUG_USE_ZH_FONT

    lv_theme_t *th = lv_theme_default_init(lv_display_get_default(), lv_palette_main(LV_PALETTE_BLUE),
                                           lv_palette_main(LV_PALETTE_RED),
                                           true, /* 深色模式 */
                                           &eos_font_resource_han_rounded_30);

#else
    lv_theme_t *th = lv_theme_default_init(lv_display_get_default(), lv_palette_main(LV_PALETTE_BLUE),
                                           lv_palette_main(LV_PALETTE_RED),
                                           true, /* 深色模式 */
                                           &lv_font_montserrat_24);
#endif
    lv_display_set_theme(NULL, th);

    lv_obj_t *scr = lv_screen_active();

    // lv_display_t *disp = lv_display_get_default();

    // lv_display_set_render_mode(disp, LV_DISPLAY_RENDER_MODE_PARTIAL);

    // lv_obj_t *img = lv_image_create(lv_screen_active());
    // eos_img_set_src(img, "/bg.bin");
    // lv_obj_center(img);
    // lv_obj_move_background(img);

    lv_obj_t *test_list = lv_list_create(scr);
    lv_obj_set_size(test_list, lv_pct(100), lv_pct(100));

    lv_obj_t *btn;
    lv_list_add_text(test_list, "[ElenaOS Test List]");
    // 测试导航功能
    btn = lv_list_add_button(test_list, LV_SYMBOL_HOME, "Navigation");
    lv_obj_add_event_cb(btn, _test_nav_cb_1, LV_EVENT_CLICKED, NULL);
    // 测试消息列表
    btn = lv_list_add_button(test_list, LV_SYMBOL_LIST, "Message List");
    lv_obj_add_event_cb(btn, _test_msg_list, LV_EVENT_CLICKED, NULL);
    // 测试字体
    btn = lv_list_add_button(test_list, LV_SYMBOL_FILE, "Font");
    lv_obj_add_event_cb(btn, _test_font, LV_EVENT_CLICKED, NULL);
    // 测试语言切换
    btn = lv_list_add_button(test_list, LV_SYMBOL_COPY, "Language");
    lv_obj_add_event_cb(btn, _test_lang, LV_EVENT_CLICKED, NULL);
    // 测试虚拟键盘
    btn = lv_list_add_button(test_list, LV_SYMBOL_KEYBOARD, "Virtual Keyboard");
    lv_obj_add_event_cb(btn, _test_vkb, LV_EVENT_CLICKED, NULL);
    // 测试图像显示
    btn = lv_list_add_button(test_list, LV_SYMBOL_IMAGE, "Image");
    lv_obj_add_event_cb(btn, _test_image, LV_EVENT_CLICKED, NULL);
    // 测试应用列表
    btn = lv_list_add_button(test_list, LV_SYMBOL_DRIVE, "App List");
    lv_obj_add_event_cb(btn, _test_app_list, LV_EVENT_CLICKED, NULL);
    // 测试应用列表
    btn = lv_list_add_button(test_list, LV_SYMBOL_LIST, "Watchface List");
    lv_obj_add_event_cb(btn, _test_watchface_list, LV_EVENT_CLICKED, NULL);

    while (1)
    {
        uint32_t d = lv_timer_handler();
        if (script_engine_get_state()==SCRIPT_STATE_READY)
        {
            script_engine_nav_init(scr);
            script_engine_result_t ret = script_engine_run(&script_pkg);
            eos_pkg_free(&script_pkg);
            if (ret != SE_OK)
            {
                EOS_LOG_E("Script encounter a fatal error");
                lv_obj_t *mbox = lv_msgbox_create(NULL);
                lv_obj_set_width(mbox,lv_pct(80));
                lv_msgbox_add_title(mbox, "Scrip Runtime");

                lv_msgbox_add_text(mbox, current_lang[STR_ID_SCRIPT_RUN_ERR]);
                lv_msgbox_add_close_button(mbox);
            }
            script_engine_nav_clean_up();
            EOS_LOG_D("Script OK");
        }
        eos_delay(d);
    }
}
jerry_value_t jph(const jerry_call_info_t *call_info_p,
                  const jerry_value_t args_p[],
                  const jerry_length_t args_count)
{
    (void)call_info_p; // 当前未用到 this/func_obj，可忽略

    for (jerry_length_t i = 0; i < args_count; i++)
    {
        jerry_value_t str_val;

        if (jerry_value_is_string(args_p[i]))
        {
            str_val = jerry_value_copy(args_p[i]);
        }
        else
        {
            str_val = jerry_value_to_string(args_p[i]); // 转为字符串
        }

        jerry_size_t size = jerry_string_size(str_val, JERRY_ENCODING_UTF8);
        jerry_char_t *buf = (jerry_char_t *)malloc(size + 1); // Explicitly cast void* to jerry_char_t*
        if (!buf)
        {
            jerry_value_free(str_val);
            continue;
        }

        jerry_string_to_buffer(str_val, JERRY_ENCODING_UTF8, buf, size);
        buf[size] = '\0';

        printf("%s", buf);
        if (i < args_count - 1)
        {
            printf(" ");
        }

        free(buf);
        jerry_value_free(str_val);
    }

    printf("\n");
    return jerry_undefined();
}


void print_current_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);  // 获取秒和微秒
    struct tm *tm_info = localtime(&tv.tv_sec);

    printf("%04d-%02d-%02d %02d:%02d:%02d.%03ld\n",
           tm_info->tm_year + 1900,
           tm_info->tm_mon + 1,
           tm_info->tm_mday,
           tm_info->tm_hour,
           tm_info->tm_min,
           tm_info->tm_sec,
           tv.tv_usec / 1000);  // 微秒转换为毫秒
}

static void _reg_print(jerry_value_t parent,int i)
{
    jerry_value_t fn = jerry_function_external(jph);
    char str[32]="";
    snprintf(str,32,"print%d",i);
    // printf("%s\n",str);
    jerry_value_t name = jerry_string_sz(str);
    jerry_value_free(jerry_object_set(parent, name, fn));
    jerry_value_free(name);
    jerry_value_free(fn);
    // jerry_value_free(parent);
}

jerry_value_t global_func;
static void _test_js(void)
{
    jerry_init(JERRY_INIT_MEM_STATS);
    jerry_value_t global = jerry_current_realm();

    printf("=== Initial memory ===\n");
    global_func = jerry_object();
    print_current_time_ms();
    for(int i=0;i<1000;i++){
        _reg_print(global_func,i);
    }
    _check_mem();

    for (int i = 0; i < 5; i++) {
        print_current_time_ms();
        printf("\n--- Realm iteration %d ---\n", i+1);

        // 创建新 realm
        jerry_value_t new_realm = jerry_realm();

        // 切换到新 realm
        jerry_value_t old_realm = jerry_set_realm(new_realm);

        // 挂到 global 上
        jerry_value_t key = jerry_string_sz("global");
        jerry_value_free(jerry_object_set(new_realm, key, global_func));
        jerry_value_free(key);

        // 执行 JS 测试
        const char *code = "var x = 100; global.print1(x);";
        jerry_value_free(jerry_eval(code, strlen(code), JERRY_PARSE_NO_OPTS));

        // 切换回原始 realm
        jerry_value_free(jerry_set_realm(old_realm));

        // 手动触发 GC
        jerry_heap_gc(JERRY_GC_PRESSURE_LOW);
        _check_mem();
    }

}

void benchmark_realm(int iterations, int print_count, const char *label) {
    struct timeval start, end;
    gettimeofday(&start, NULL);

    for (int i = 0; i < iterations; i++) {
        jerry_value_t new_realm = jerry_realm();
        jerry_value_t old_realm = jerry_set_realm(new_realm);

        // attach global_func
        jerry_value_t key = jerry_string_sz("global");
        jerry_value_free(jerry_object_set(new_realm, key, global_func));
        jerry_value_free(key);

        // create print_count prints
        for (int j = 0; j < print_count; j++) {
            _reg_print(global_func, j);
        }

        // eval JS
        const char *code = "var x = 100; global.print1(x);";
        jerry_value_free(jerry_eval(code, strlen(code), JERRY_PARSE_NO_OPTS));

        jerry_value_free(jerry_set_realm(old_realm));
        jerry_heap_gc(JERRY_GC_PRESSURE_LOW);
    }

    gettimeofday(&end, NULL);

    long elapsed_ms = (end.tv_sec - start.tv_sec) * 1000 +
                      (end.tv_usec - start.tv_usec) / 1000;

    printf("%s elapsed time: %ld ms\n", label, elapsed_ms);
}
long long current_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
void test_global_vs_per_realm() {
    long long start, end;
    jerry_value_t key;

    // Test 1: global_func reused
    jerry_init(JERRY_INIT_MEM_STATS);
    jerry_value_t global_func = jerry_object();
    for (int i = 0; i < 5000; i++) _reg_print(global_func, i);

    start = current_time_ms();
    for (int i = 0; i < 5; i++) {
        jerry_value_t new_realm = jerry_realm();
        jerry_value_t old_realm = jerry_set_realm(new_realm);

        key = jerry_string_sz("global");
        jerry_value_free(jerry_object_set(new_realm, key, global_func));
        jerry_value_free(key);

        const char *code = "var x=100; global.print();";
        jerry_value_free(jerry_eval(code, strlen(code), JERRY_PARSE_NO_OPTS));

        jerry_value_free(jerry_set_realm(old_realm));
        jerry_heap_gc(JERRY_GC_PRESSURE_LOW);
        _check_mem();
    }
    end = current_time_ms();
    long long test1_time = end - start;
    jerry_value_free(global_func);
    jerry_cleanup();

    // Test 2: per-realm function
    jerry_init(JERRY_INIT_MEM_STATS);
    start = current_time_ms();
    for (int i = 0; i < 5; i++) {
        jerry_value_t new_realm = jerry_realm();
        jerry_value_t old_realm = jerry_set_realm(new_realm);

        jerry_value_t realm_func = jerry_object();
        for (int j = 0; j < 5000; j++) _reg_print(realm_func, j);

        key = jerry_string_sz("global");
        jerry_value_free(jerry_object_set(new_realm, key, realm_func));
        jerry_value_free(key);

        const char *code = "var x=100; global.print();";
        jerry_value_free(jerry_eval(code, strlen(code), JERRY_PARSE_NO_OPTS));

        jerry_value_free(realm_func);
        jerry_value_free(jerry_set_realm(old_realm));
        jerry_heap_gc(JERRY_GC_PRESSURE_LOW);
        _check_mem();
    }
    end = current_time_ms();
    long long test2_time = end - start;
    jerry_cleanup();

    printf("Test1 total time: %lld ms\n", test1_time);
    printf("Test2 total time: %lld ms\n", test2_time);
}

#endif
