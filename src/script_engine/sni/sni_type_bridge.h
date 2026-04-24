/**
 * @file sni_type_bridge.h
 * @brief Type bridge
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
 * @brief Convert JerryScript string to C string
 * @param js_val JerryScript string value
 * @return const char* C string pointer
 * @note Caller must use `eos_free` to free the returned string memory
 */
const char *sni_tb_js2c_string(jerry_value_t js_val);

/**
 * @brief Convert JerryScript value to C value or value object
 * @param js_val JerryScript value
 * @param type Target type
 * @param out_obj Output value pointer (if output value is a pointer, should pass its address, i.e., double pointer)
 * @return bool Whether conversion was successful
 */
bool sni_tb_js2c(jerry_value_t js_val, sni_type_t type, void *out_obj);

/**
 * @brief Extract raw pointer and actual type from any Handle object
 * @param js_val JerryScript handle object
 * @param out_obj Output raw pointer
 * @param out_type Output handle actual type, can be NULL
 * @return bool Whether extraction was successful
 */
bool sni_tb_js2c_any_handle(jerry_value_t js_val, void *out_obj, sni_type_t *out_type);

/**
 * @brief Convert C value or value object to JerryScript value
 * @param c_val C value pointer (if input value is a pointer, should pass its address, i.e., double pointer)
 * @param type Source type
 * @return jerry_value_t JerryScript value
 */
jerry_value_t sni_tb_c2js(void *c_val, sni_type_t type);

/**
 * @brief Write C value object or handle to existing JerryScript object
 * @param c_val C value pointer (if input value is a pointer, should pass its address, i.e., double pointer)
 * @param type Source type
 * @param js_obj Target JerryScript object
 * @return bool Whether writing was successful
 */
bool sni_tb_c2js_set_object(void *c_val, sni_type_t type, jerry_value_t js_obj);

/**
 * @brief Register value object
 * @param val_obj Value object pointer
 */
void sni_tb_register_val_obj(const sni_val_obj_t *val_obj);

/**
 * @brief Register handle destroy callback (supports External/Realm lifecycle handles)
 * @param type Handle type (must be Handle type)
 * @param destroy_cb Destroy callback
 */
void sni_tb_register_handle_destroy_cb(sni_type_t type, sni_handle_destroy_cb_t destroy_cb);

void sni_tb_init(void);

#ifdef __cplusplus
}
#endif

#endif /* SNI_TYPE_BRIDGE_H */
