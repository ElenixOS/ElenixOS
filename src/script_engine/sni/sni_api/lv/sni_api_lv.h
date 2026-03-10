/**
 * @file sni_api_lv.h
 * @brief LVGL API 导出
 * @author Sab1e
 * @date 2026-03-06
 */

#ifndef SNI_API_LV_H
#define SNI_API_LV_H

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
 * @brief 初始化 LVGL API
 */
void sni_api_lv_init(void);

/**
 * @brief 挂载 LVGL API 到指定的 Realm
 * @param realm 目标 Realm 值
 */
void sni_api_lv_mount(jerry_value_t realm);

#ifdef __cplusplus
}
#endif

#endif /* SNI_API_LV_H */
