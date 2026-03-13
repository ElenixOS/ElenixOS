/**
 * @file sni_api_eos.h
 * @brief ElenaOS API
 * @author Sab1e
 * @date 2026-03-14
 */

#ifndef SNI_API_EOS_H
#define SNI_API_EOS_H

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
 * @brief 初始化 ElenaOS API
 */
void sni_api_eos_init(void);

/**
 * @brief 挂载 ElenaOS API 到指定的 Realm
 * @param realm 目标 Realm 值
 */
void sni_api_eos_mount(jerry_value_t realm);

#ifdef __cplusplus
}
#endif

#endif /* SNI_API_EOS_H */
