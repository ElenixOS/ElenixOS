/**
 * @file sni_type_bridge.h
 * @brief 类型桥
 * @author Sab1e
 * @date 2026-02-09
 */

#ifndef SNI_TYPE_BRIDGE_H
#define SNI_TYPE_BRIDGE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "jerryscript.h"
#include "sni_types.h"
/* Public macros ----------------------------------------------*/

#define sni_tb_c2js_number jerry_number
#define sni_tb_js2c_number jerry_value_as_number
#define sni_tb_js2c_integer jerry_value_as_integer
#define sni_tb_js2c_int32 jerry_value_as_int32
#define sni_tb_js2c_uint32 jerry_value_as_uint32

#define sni_tb_c2js_boolean jerry_boolean
#define sni_tb_js2c_boolean jerry_value_to_boolean

#define sni_tb_c2js_string jerry_string_sz

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

/**
 * @brief 将 JerryScript 字符串转换为 C 字符串
 * @param js_val JerryScript 字符串值
 * @return const char* C 字符串指针
 * @note 调用者必须使用`eos_free`释放返回的字符串内存
 */
const char *sni_tb_js2c_string(jerry_value_t js_val);

/**
 * @brief 将 JerryScript 值转换为 C 值或值对象
 * @param js_val JerryScript 值
 * @param type 目标类型
 * @param out_obj 输出值指针（如果输出值是指针，则应该传入它的地址，即二级指针）
 * @return bool 转换是否成功
 */
bool sni_tb_js2c(jerry_value_t js_val, sni_type_t type, void *out_obj);

/**
 * @brief 将 C 值或值对象转换为 JerryScript 值
 * @param c_val C 值指针（如果输入值是指针，则应该传入它的地址，即二级指针）
 * @param type 源类型
 * @return jerry_value_t JerryScript 值
 */
jerry_value_t sni_tb_c2js(void *c_val, sni_type_t type);

/**
 * @brief 将 C 值对象或句柄写入现有 JerryScript 对象
 * @param c_val C 值指针（如果输入值是指针，则应该传入它的地址，即二级指针）
 * @param type 源类型
 * @param js_obj 目标 JerryScript 对象
 * @return bool 写入是否成功
 */
bool sni_tb_c2js_set_object(void *c_val, sni_type_t type, jerry_value_t js_obj);

/**
 * @brief 注册值对象
 * @param val_obj 值对象指针
 */
void sni_tb_register_val_obj(const sni_val_obj_t *val_obj);

/**
 * @brief 注册 Realm 生命周期句柄的销毁回调
 * @param type 句柄类型（必须为 Realm 生命周期句柄）
 * @param destroy_cb 销毁回调
 */
void sni_tb_register_handle_destroy_cb(sni_type_t type, sni_handle_destroy_cb_t destroy_cb);

void sni_tb_init(void);

#ifdef __cplusplus
}
#endif

#endif /* SNI_TYPE_BRIDGE_H */
