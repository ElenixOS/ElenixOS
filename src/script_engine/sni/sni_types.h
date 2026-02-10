/**
 * @file sni_types.h
 * @brief 脚本原生接口
 * @author Sab1e
 * @date 2026-02-09
 */

#ifndef SNI_TYPES_H
#define SNI_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "jerryscript.h"
#include "uthash.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

/**
 * @brief SNI 类型枚举
 */
typedef enum
{
    __SNI_TYPE_START = 0,
    SNI_T_UNKNOWN = 0,

	__SNI_TYPE_NUMBER_START,
	SNI_T_UINT32,
	SNI_T_INT32,
	SNI_T_DOUBLE,
	__SNI_TYPE_NUMBER_END,

	SNI_T_BOOL,
	SNI_T_STRING,
	SNI_T_PTR,

	__SNI_HANDLE_START,
	SNI_H_LV_OBJ,
	__SNI_HANDLE_END,

	__SNI_VALUE_START,
	SNI_V_LV_POINT,
    SNI_V_LV_COLOR16,
	SNI_V_LV_AREA,
	SNI_V_LV_COLOR32,
	__SNI_VALUE_END,

    __SNI_TYPE_MAX
} sni_type_t;

/**
 * @brief 属性结构体
 *
 * 值对象使用属性结构体数组来定义
 *
 * 示例：
 * ```c
 * const sni_val_prop_t lv_point_props[] = {
 *     {"x", SNI_T_INT32, offsetof(lv_point_t, x)},
 *     {"y", SNI_T_INT32, offsetof(lv_point_t, y)},
 * };
 * ```
 */
typedef struct
{
	const char *name;   /**< 属性名 */
	sni_type_t type;    /**< 属性类型 */
    size_t offset;      /**< 属性在值对象结构体中的偏移量 */
} sni_val_prop_t;


typedef struct
{
    sni_type_t type;
    uint16_t prop_count;    /**< 属性数量 */
    sni_val_prop_t *props;  /**< 属性数组指针 */
} sni_val_obj_t;

/**
 * @brief 句柄对象结构体
 */
typedef struct
{
    void *ptr;                  /**< 句柄指向的对象指针 */
    jerry_value_t js_obj;       /**< 句柄对应的 JavaScript 对象 */
    sni_type_t type;            /**< 句柄类型 */
    bool is_alive;              /**< 句柄是否存活 */
    UT_hash_handle hh;          /**< 哈希句柄，用于哈希表操作 */
} sni_handle_t;

#ifdef __cplusplus
}
#endif

#endif /* SNI_TYPES_H */
