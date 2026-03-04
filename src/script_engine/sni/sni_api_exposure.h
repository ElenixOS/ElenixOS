/**
 * @file sni_api_exposure.h
 * @brief API 暴露层
 * @author Sab1e
 * @date 2026-03-05
 */

#ifndef SNI_API_EXPOSURE_H
#define SNI_API_EXPOSURE_H

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
 * @brief API 中的 entry 类型枚举
 */
typedef enum {
    SNI_ENTRY_FUNCTION,
    SNI_ENTRY_CONSTANT,
    SNI_ENTRY_NAMESPACE
} sni_entry_type_t;

/**
 * @brief API 中的 entry 结构体
 */
typedef struct sni_native_entry {
    const char *name;                                   /**< 条目名称 */
    sni_entry_type_t type;
    union {
        jerry_external_handler_t function;              /**< 函数 */
        int32_t constant;                               /**< 常量值 */
        const struct sni_native_entry *sub_entries;     /**< 子条目 */
    } value;
} sni_native_entry_t;

/* Public function prototypes --------------------------------*/

/**
 * @brief 根据描述表创建一个 API 对象
 * @param entries 描述表，最后一个条目的名称必须是 NULL 以表示命名空间结束
 * @return jerry_value_t API 对象
 */
jerry_value_t sni_api_build(const sni_native_entry_t *entries);

/**
 * @brief 将 API 对象挂载到指定的 realm 中
 * @param realm 目标 realm 对象
 * @param api_obj API 对象
 * @param name 挂载名称
 * @return bool 是否挂载成功
 */
bool sni_api_install(jerry_value_t realm, jerry_value_t api_obj, const char *name);

#ifdef __cplusplus
}
#endif

#endif /* SNI_API_EXPOSURE_H */
