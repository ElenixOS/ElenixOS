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

/**
 * @brief API 描述表中条目类型枚举
 */
typedef enum
{
    SNI_ENTRY_FUNCTION,
    SNI_ENTRY_CONSTANT_INT,
    SNI_ENTRY_CONSTANT_FLOAT,
    SNI_ENTRY_CONSTANT_STRING,
    SNI_ENTRY_NAMESPACE
} sni_entry_type_t;

/**
 * @brief API 描述表中的条目结构体
 */
typedef struct sni_api_entry_t
{
    const char *name;                                   /**< 条目名称 */
    sni_entry_type_t type;
    union {
        jerry_external_handler_t function;              /**< 函数 */
        union {
            int32_t i;                                  /**< 整数常量 */
            double f;                                   /**< 浮点数常量 */
            const char *s;                              /**< 字符串常量 */
        } constant;
        const struct sni_api_entry_t *sub_entries;      /**< 子条目 */
    } value;
} sni_api_entry_t;

/* Public function prototypes --------------------------------*/

/**
 * @brief 根据描述表创建一个 API 对象
 * @param entries 描述表，最后一个条目的名称必须是 NULL 以表示命名空间结束
 * @return jerry_value_t API 对象
 */
jerry_value_t sni_api_build(const sni_api_entry_t *entries);

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
