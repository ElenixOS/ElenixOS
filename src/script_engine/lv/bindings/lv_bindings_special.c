/**
 * @file lv_bindings_special.c
 * @brief 特殊绑定层
 * @author Sab1e
 * @date 2025-12-25
 */

#include "lv_bindings_special.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define EOS_LOG_DISABLE
#define EOS_LOG_TAG "LV Bindings Special"
#include "elena_os_log.h"
#include "lv_bindings.h"
#include "elena_os_nav.h"
#include "elena_os_watchface.h"
#include "elena_os_port.h"
#include "elena_os_mem.h"
#include "lvgl_js_bridge.h"
#include "script_engine_lv.h"
#include "elena_os_app.h"
#include "elena_os_fs.h"

/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

/********************************** 特殊 LVGL 函数 **********************************/
/**
 * @brief 样式初始化
 */
static jerry_value_t js_lv_style_init(const jerry_call_info_t *call_info_p,
                                      const jerry_value_t args[],
                                      const jerry_length_t argc)
{
    // 参数数量检查
    if (argc < 1)
    {
        return script_engine_throw_error("Insufficient arguments");
    }

    // 检查参数是否为对象
    if (!jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Argument must be a style object");
    }

    // 检查对象是否已经分配了内存
    jerry_value_t ptr_prop = jerry_string_sz("__ptr");
    jerry_value_t ptr_val = jerry_object_get(args[0], ptr_prop);
    jerry_value_free(ptr_prop);

    lv_style_t *style = NULL;

    if (jerry_value_is_number(ptr_val))
    {
        // 已有指针的情况
        uintptr_t ptr = (uintptr_t)jerry_value_as_number(ptr_val);
        style = (lv_style_t *)ptr;
    }
    else
    {
        // 没有指针的情况，分配新内存
        style = (lv_style_t *)eos_malloc(sizeof(lv_style_t));
        if (!style)
        {
            jerry_value_free(ptr_val);
            return script_engine_throw_error("Failed to allocate memory for style");
        }

        // 将指针保存回JS对象
        ptr_val = jerry_number((uintptr_t)style);
        ptr_prop = jerry_string_sz("__ptr");
        jerry_value_free(jerry_object_set(args[0], ptr_prop, ptr_val));
        jerry_value_free(ptr_prop);
        jerry_value_free(ptr_val);

        // 添加类型标记
        jerry_value_t type_prop = jerry_string_sz("__type");
        jerry_value_t type_val = jerry_string_sz("lv_style");
        jerry_value_free(jerry_object_set(args[0], type_prop, type_val));
        jerry_value_free(type_prop);
        jerry_value_free(type_val);
    }

    // 调用初始化函数
    lv_style_init(style);

    // 返回JS对象本身，支持链式调用
    return jerry_value_copy(args[0]);
}
/**
 * @brief 样式清除
 */
static jerry_value_t js_lv_style_delete(const jerry_call_info_t *call_info_p,
                                        const jerry_value_t args[],
                                        const jerry_length_t argc)
{
    if (argc < 1 || !jerry_value_is_object(args[0]))
    {
        return script_engine_throw_error("Invalid arguments");
    }

    jerry_value_t ptr_prop = jerry_string_sz("__ptr");
    jerry_value_t ptr_val = jerry_object_get(args[0], ptr_prop);
    jerry_value_free(ptr_prop);

    if (jerry_value_is_number(ptr_val))
    {
        lv_style_t *style = (lv_style_t *)(uintptr_t)jerry_value_as_number(ptr_val);
        eos_free(style);

        // 清除指针引用
        ptr_prop = jerry_string_sz("__ptr");
        jerry_value_free(jerry_object_set(args[0], ptr_prop, jerry_number(0)));
        jerry_value_free(ptr_prop);
    }

    jerry_value_free(ptr_val);
    return jerry_undefined();
}

/********************************** 字体系统 **********************************/
static void register_lvgl_fonts(jerry_value_t parent)
{
    // 创建字体对象容器
    jerry_value_t fonts = jerry_object();

    // 注册Montserrat字体集
#define REGISTER_FONT(name)                                               \
    do                                                                    \
    {                                                                     \
        jerry_value_t font_obj = jerry_object();                          \
                                                                          \
        jerry_value_t key_ptr = jerry_string_sz("__ptr");                 \
        jerry_value_t key_type = jerry_string_sz("__type");               \
        jerry_value_t key_name = jerry_string_sz(#name);                  \
                                                                          \
        jerry_value_t val_ptr = jerry_number((uintptr_t)&name);           \
        jerry_value_t val_type = jerry_string_sz("lv_font");              \
                                                                          \
        jerry_value_free(jerry_object_set(font_obj, key_ptr, val_ptr));   \
        jerry_value_free(jerry_object_set(font_obj, key_type, val_type)); \
        jerry_value_free(jerry_object_set(fonts, key_name, font_obj));    \
                                                                          \
        jerry_value_free(key_ptr);                                        \
        jerry_value_free(key_type);                                       \
        jerry_value_free(key_name);                                       \
        jerry_value_free(val_ptr);                                        \
        jerry_value_free(val_type);                                       \
        jerry_value_free(font_obj);                                       \
    } while (0)
#if LV_FONT_MONTSERRAT_8
    REGISTER_FONT(lv_font_montserrat_8);
#endif
#if LV_FONT_MONTSERRAT_10
    REGISTER_FONT(lv_font_montserrat_10);
#endif
#if LV_FONT_MONTSERRAT_12
    REGISTER_FONT(lv_font_montserrat_12);
#endif
#if LV_FONT_MONTSERRAT_14
    REGISTER_FONT(lv_font_montserrat_14);
#endif
#if LV_FONT_MONTSERRAT_16
    REGISTER_FONT(lv_font_montserrat_16);
#endif
#if LV_FONT_MONTSERRAT_18
    REGISTER_FONT(lv_font_montserrat_18);
#endif
#if LV_FONT_MONTSERRAT_20
    REGISTER_FONT(lv_font_montserrat_20);
#endif
#if LV_FONT_MONTSERRAT_22
    REGISTER_FONT(lv_font_montserrat_22);
#endif
#if LV_FONT_MONTSERRAT_24
    REGISTER_FONT(lv_font_montserrat_24);
#endif
#if LV_FONT_MONTSERRAT_26
    REGISTER_FONT(lv_font_montserrat_26);
#endif
#if LV_FONT_MONTSERRAT_28
    REGISTER_FONT(lv_font_montserrat_28);
#endif
#if LV_FONT_MONTSERRAT_30
    REGISTER_FONT(lv_font_montserrat_30);
#endif
#if LV_FONT_MONTSERRAT_32
    REGISTER_FONT(lv_font_montserrat_32);
#endif
#if LV_FONT_MONTSERRAT_34
    REGISTER_FONT(lv_font_montserrat_34);
#endif
#if LV_FONT_MONTSERRAT_36
    REGISTER_FONT(lv_font_montserrat_36);
#endif
#if LV_FONT_MONTSERRAT_38
    REGISTER_FONT(lv_font_montserrat_38);
#endif
#if LV_FONT_MONTSERRAT_40
    REGISTER_FONT(lv_font_montserrat_40);
#endif
#if LV_FONT_MONTSERRAT_42
    REGISTER_FONT(lv_font_montserrat_42);
#endif
#if LV_FONT_MONTSERRAT_44
    REGISTER_FONT(lv_font_montserrat_44);
#endif
#if LV_FONT_MONTSERRAT_46
    REGISTER_FONT(lv_font_montserrat_46);
#endif
#if LV_FONT_MONTSERRAT_48
    REGISTER_FONT(lv_font_montserrat_48);
#endif

#undef REGISTER_FONT

    // 将字体容器挂载到全局对象
    jerry_value_t font_key = jerry_string_sz("lv_font");
    jerry_value_free(jerry_object_set(parent, font_key, fonts));
    jerry_value_free(font_key);
    jerry_value_free(fonts);
}

static jerry_value_t js_lv_tiny_ttf_create_file(const jerry_call_info_t *call_info_p,
                                                const jerry_value_t args[],
                                                const jerry_length_t argc)
{
#if LV_TINY_TTF_FILE_SUPPORT
    // 参数数量检查
    if (argc < 2)
    {
        return script_engine_throw_error("Insufficient arguments");
    }

    // 解析参数: src (const char*)
    char *arg_src_str = NULL;
    const char *arg_src = NULL;
    if (!jerry_value_is_undefined(args[0]) && !jerry_value_is_null(args[0]))
    {
        if (!jerry_value_is_string(args[0]))
        {
            return script_engine_throw_error("Argument 0 must be a string");
        }
        jerry_size_t arg_src_len = jerry_string_size(args[0], JERRY_ENCODING_UTF8);
        arg_src_str = (char *)eos_malloc(arg_src_len + 0);
        jerry_string_to_buffer(args[0], JERRY_ENCODING_UTF8, (jerry_char_t *)arg_src_str, arg_src_len);
        arg_src_str[arg_src_len] = '\0';
        arg_src = arg_src_str;
    }

    // 解析参数: font_size (uint32_t)
    if (!jerry_value_is_number(args[1]))
    {
        if (arg_src_str)
            eos_free(arg_src_str);
        return script_engine_throw_error("Argument 1 must be a number");
    }
    uint32_t font_size = (uint32_t)jerry_value_as_number(args[1]);

    char font_path[PATH_MAX];
    if (script_engine_get_current_script_id() == NULL)
    {
        if (arg_src_str)
            eos_free(arg_src_str);
        return script_engine_throw_error("Script package info is NULL");
    }

    if (script_engine_get_current_script_type() == SCRIPT_TYPE_APPLICATION)
    {
        snprintf(font_path, sizeof(font_path), EOS_APP_INSTALLED_DIR "%s/assets/%s",
                 script_engine_get_current_script_id(), arg_src);
    }
    else if (script_engine_get_current_script_type() == SCRIPT_TYPE_WATCHFACE)
    {
        snprintf(font_path, sizeof(font_path), EOS_WATCHFACE_INSTALLED_DIR "%s/assets/%s",
                 script_engine_get_current_script_id(), arg_src);
    }
    else
    {
        if (arg_src_str)
            eos_free(arg_src_str);
        return script_engine_throw_error("Unknown script type");
    }

    if (!eos_is_file(font_path))
    {
        if (arg_src_str)
            eos_free(arg_src_str);
        return script_engine_throw_error("Font file not found");
    }

    EOS_LOG_D("Font Path: %s", font_path);

    // 调用底层函数创建字体
    lv_font_t *font = lv_tiny_ttf_create_file(font_path, font_size);

    // 释放临时字符串内存
    if (arg_src_str)
        eos_free(arg_src_str);

    if (!font)
    {
        return script_engine_throw_error("Failed to create font");
    }

    // 包装为LVGL字体对象返回
    jerry_value_t js_result = lv_js_bridge_ptr_2_obj(font, LV_TYPE_FONT);

    return js_result;
#else
    return script_engine_throw_error("LV_TINY_TTF_FILE_SUPPORT is not enabled");
#endif
}

/********************************** 绑定注册 **********************************/

const script_engine_func_entry_t lvgl_binding_special_funcs[] = {
    {"style", "init", js_lv_style_init},
    {"style", "delete", js_lv_style_delete},
    {"tinyTTF", "createFile", js_lv_tiny_ttf_create_file}};

/********************************** 初始化 **********************************/

void lv_bindings_special_init(jerry_value_t parent)
{
    // 初始化函数
    script_engine_register_functions(parent, lvgl_binding_special_funcs, sizeof(lvgl_binding_special_funcs) / sizeof(script_engine_func_entry_t));
    register_lvgl_fonts(parent);
}
