/**
 * @file sni_api_export.h
 * @brief API 导出层
 * @author Sab1e
 * @date 2026-03-06
 */

#ifndef SNI_API_EXPORT_H
#define SNI_API_EXPORT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "jerryscript.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

typedef enum
{
    SNI_CONST_INT,
    SNI_CONST_FLOAT,
    SNI_CONST_STRING,
} sni_constant_type_t;

typedef struct
{
    const char *name;
    jerry_external_handler_t handler;
} sni_method_desc_t;

typedef struct
{
    const char *name;
    jerry_external_handler_t getter;
    jerry_external_handler_t setter;
} sni_property_desc_t;

typedef struct
{
    const char *name;
    sni_constant_type_t type;
    union {
        int32_t i;
        double f;
        const char *s;
    } value;
} sni_constant_desc_t;

typedef struct sni_class_desc_t sni_class_desc_t;

struct sni_class_desc_t
{
    const char *name;

    jerry_external_handler_t constructor;   /**< 构造函数；为 NULL 时该类按静态类导出 */

    const sni_class_desc_t *base_class;

    const sni_method_desc_t *methods;       /**< 实例方法 */
    const sni_property_desc_t *properties;  /**< 属性 */

    const sni_method_desc_t *static_methods;
    const sni_constant_desc_t *constants;
};

/* Public function prototypes --------------------------------*/

/**
 * @brief 根据类描述表创建一个 API 对象
 * @param classes 类描述表指针数组，最后一个元素必须为 NULL
 * @return jerry_value_t API 对象
 */
jerry_value_t sni_api_build(const sni_class_desc_t *const classes[]);

/**
 * @brief 将 API 对象挂载到指定的 realm 中
 * @param realm 目标 realm 对象
 * @param api_obj API 对象
 * @param name 挂载名称
 * @return bool 是否挂载成功
 */
bool sni_api_mount(jerry_value_t realm, jerry_value_t api_obj, const char *name);

/**
 * @brief 抛出一个错误异常
 * @param message 错误信息
 * @return jerry_value_t 错误异常对象
 */
jerry_value_t sni_api_throw_error(const char *message);

#ifdef __cplusplus
}
#endif

#endif /* SNI_API_EXPORT_H */
