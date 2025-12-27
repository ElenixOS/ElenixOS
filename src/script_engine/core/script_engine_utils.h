/**
 * @file script_engine_utils.h
 * @brief 工具函数
 * @author Sab1e
 * @date 2025-12-27
 */

#ifndef SCRIPT_ENGINE_UTILS_H
#define SCRIPT_ENGINE_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "jerryscript.h"

/* Public macros ----------------------------------------------*/
#define JS_ARG_CHECK_OK (-1)

/* Public typedefs --------------------------------------------*/

typedef enum {
    ARG_TYPE_ANY,
    ARG_TYPE_STR,
    ARG_TYPE_BOOL,
    ARG_TYPE_NUMBER,
    ARG_TYPE_OBJ,
} js_arg_type_t;

typedef struct {
    bool ok;
    const char *error;
} arg_check_result_t;

/* Public function prototypes --------------------------------*/

/**
 * @brief 将 jerry_value_t 的字符串转换为 c 字符串
 * @param value
 * @return char* 字符串使用完成后必须使用`eos_free()`释放
 */
char *js_value_to_c_string(jerry_value_t value);
char *js_get_arg_type_str(const js_arg_type_t type);
/**
 * @brief 绑定层传入参数类型检查
 * @param args 参数数组
 * @param type_array 对应的类型数组
 * @param argc 参数数量
 * @return int32_t 返回值 >= 0 时，其值即为类型不匹配的参数的索引值；返回值 = JS_ARG_CHECK_OK 时，即为匹配成功
 */
int32_t js_args_check_type(const jerry_value_t args[],
                            const js_arg_type_t *type_array,
                            jerry_length_t argc);
#ifdef __cplusplus
}
#endif

#endif /* SCRIPT_ENGINE_UTILS_H */
