/**
 * @file script_engine_lv.h
 * @brief lv库
 * @author Sab1e
 * @date 2025-12-25
 */

#ifndef SCRIPT_ENGINE_LV_H
#define SCRIPT_ENGINE_LV_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "jerryscript.h"

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

/**
 * @brief 获取 LV 库的根对象
 * @return jerry_value_t
 */
jerry_value_t script_engine_lv_get_root(void);
/**
 * @brief 将 LV 库的根对象挂载到指定 realm
 * @param realm
 */
void script_engine_lv_attach(jerry_value_t realm);
/**
 * @brief 初始化 LV 库（系统初始化时调用）
 */
void script_engine_lv_init(void);
#ifdef __cplusplus
}
#endif

#endif /* SCRIPT_ENGINE_LV_H */
