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

#define SNI_TYPE_IS_NUMBER(type) ((type) >= __SNI_TYPE_NUMBER_START && (type) <= __SNI_TYPE_NUMBER_END)
#define SNI_TYPE_IS_HANDLE(type) ((type) >= __SNI_HANDLE_START && (type) <= __SNI_HANDLE_END)
#define SNI_TYPE_IS_VALUE(type) ((type) >= __SNI_VALUE_START && (type) <= __SNI_VALUE_END)
#define SNI_TYPE_IS_HANDLE_LC_EXTERNAL(type) ((type) >= __SNI_HANDLE_LC_EXTERNAL_START && (type) <= __SNI_HANDLE_LC_EXTERNAL_END)
#define SNI_TYPE_IS_HANDLE_LC_REALM(type) ((type) >= __SNI_HANDLE_LC_REALM_START && (type) <= __SNI_HANDLE_LC_REALM_END)

#define SNI_HANDLE_LC_EXTERNAL_COUNT \
    (__SNI_HANDLE_LC_EXTERNAL_END - __SNI_HANDLE_LC_EXTERNAL_START - 1)

#define SNI_HANDLE_LC_REALM_COUNT \
    (__SNI_HANDLE_LC_REALM_END - __SNI_HANDLE_LC_REALM_START - 1)

#define SNI_HANDLE_COUNT \
    (SNI_HANDLE_LC_EXTERNAL_COUNT + SNI_HANDLE_LC_REALM_COUNT)


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

	/************************** 外部生命周期的 Handle 对象 **************************/
	__SNI_HANDLE_LC_EXTERNAL_START,

	SNI_H_LV_TIMER,
	SNI_H_LV_STYLE,

	__SNI_HANDLE_LC_EXTERNAL_END,

	/************************** Realm 生命周期的 Handle 对象 **************************/

	__SNI_HANDLE_LC_REALM_START,

	SNI_H_LV_ANIM,
	SNI_H_INT32,
	SNI_H_LV_COLOR_FILTER_DSC,
	SNI_H_LV_DISPLAY,
	SNI_H_LV_DRAW_ARC_DSC,
	SNI_H_LV_DRAW_IMAGE_DSC,
	SNI_H_LV_DRAW_LABEL_DSC,
	SNI_H_LV_DRAW_LINE_DSC,
	SNI_H_LV_DRAW_RECT_DSC,
	SNI_H_LV_EVENT,
	SNI_H_LV_EVENT_CB,
	SNI_H_LV_EVENT_DSC,
	SNI_H_LV_FONT,
	SNI_H_LV_GRAD_DSC,
	SNI_H_LV_GROUP,
	SNI_H_LV_LAYER,
	SNI_H_LV_OBJ,
	SNI_H_LV_OBJ_CLASS,
	SNI_H_LV_OBJ_TREE_WALK_CB,
	SNI_H_LV_OBSERVER,
	SNI_H_LV_STYLE_TRANSITION_DSC,
	SNI_H_LV_STYLE_VALUE,
	SNI_H_LV_SUBJECT,

	__SNI_HANDLE_LC_REALM_END,

	__SNI_HANDLE_END,

	__SNI_VALUE_START,
	SNI_V_LV_POINT,
    SNI_V_LV_COLOR16,
	SNI_V_LV_AREA,
	SNI_V_LV_COLOR32,
	SNI_V_LV_SQRT_RES,
	SNI_V_LV_ANIM_BEZIER3_PARA,
	SNI_V_LV_POINT_PRECISE,
	SNI_V_LV_GRAD_COLOR,
	SNI_V_LV_COLOR_HSV,
	SNI_V_LV_COLOR16A,
	SNI_V_LV_GRADIENT_STOP,
	SNI_V_LV_COLOR,
	SNI_V_LV_OPA,
	SNI_V_LV_PART,
	SNI_V_LV_STATE,
	SNI_V_LV_STYLE_PROP,
	SNI_V_LV_STYLE_SELECTOR,
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
    uint8_t bit_width;  /**< 位域成员的位宽（非位域成员为0） */
} sni_val_prop_t;


typedef struct
{
    sni_type_t type;
    uint16_t prop_count;    /**< 属性数量 */
    const sni_val_prop_t *props;  /**< 属性数组指针 */
} sni_val_obj_t;

typedef void (*sni_handle_destroy_cb_t)(void *native_ptr);

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
