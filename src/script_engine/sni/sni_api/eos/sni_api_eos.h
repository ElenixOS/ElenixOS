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
 * @brief Initialize ElenaOS API
 */
void sni_api_eos_init(void);

/**
 * @brief Mount ElenaOS API to specified Realm
 * @param realm Target Realm value
 */
void sni_api_eos_mount(jerry_value_t realm);

#ifdef __cplusplus
}
#endif

#endif /* SNI_API_EOS_H */
