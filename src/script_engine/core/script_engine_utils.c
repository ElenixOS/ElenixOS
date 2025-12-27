/**
 * @file script_engine_utils.c
 * @brief 工具函数
 * @author Sab1e
 * @date 2025-12-27
 */

#include "script_engine_utils.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#define EOS_LOG_TAG "ScriptEngineUtils"
#include "elena_os_log.h"
#include "script_engine_core.h"
#include "elena_os_mem.h"
/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

char *js_value_to_c_string(jerry_value_t value)
{
    if (!jerry_value_is_string(value))
    {
        script_engine_throw_error("Expected a string");
        return NULL;
    }

    // 获取 UTF-8 字符串长度（不包含 '\0'）
    jerry_size_t size =
        jerry_string_size(value, JERRY_ENCODING_UTF8);

    // 分配 C 字符串（+1 给 '\0'）
    char *buf = eos_malloc(size + 1);
    if (!buf)
    {
        jerry_value_t err =
            jerry_error_sz(JERRY_ERROR_COMMON,
                           (const jerry_char_t *)"Out of memory");
        jerry_throw_value(err, true);
        return NULL;
    }

    // 拷贝字符串
    jerry_string_to_buffer(
        value,
        JERRY_ENCODING_UTF8,
        (jerry_char_t *)buf,
        size);

    buf[size] = '\0';

    return buf;
}

char *js_get_arg_type_str(const js_arg_type_t type)
{
    switch (type)
    {
    case ARG_TYPE_ANY:
        return "ANY";
    case ARG_TYPE_BOOL:
        return "BOOL";
    case ARG_TYPE_NUMBER:
        return "NUMBER";
    case ARG_TYPE_OBJ:
        return "OBJ";
    case ARG_TYPE_STR:
        return "STR";
    default:
        return "UNKNOWN";
    }
}

int32_t js_args_check_type(const jerry_value_t args[],
                           const js_arg_type_t *type_array,
                           jerry_length_t argc)
{
    for (jerry_length_t i = 0; i < argc; i++)
    {
        switch (type_array[i])
        {
        case ARG_TYPE_STR:
            if (!jerry_value_is_string(args[i]))
                return i;
            break;

        case ARG_TYPE_BOOL:
            if (!jerry_value_is_boolean(args[i]))
                return i;
            break;

        case ARG_TYPE_NUMBER:
            if (!jerry_value_is_number(args[i]))
                return i;
            break;

        case ARG_TYPE_OBJ:
            if (!jerry_value_is_object(args[i]))
                return i;
            break;

        case ARG_TYPE_ANY:
            // 不检查
            break;

        default:
            EOS_LOG_W("Unknown type");
            return i;
        }
    }

    return JS_ARG_CHECK_OK;
}
