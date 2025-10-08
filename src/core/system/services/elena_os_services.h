/**
 * @file elena_os_services.h
 * @brief 服务
 * @author Sab1e
 * @date 2025-10-07
 */

#ifndef ELENA_OS_SERVICES_H
#define ELENA_OS_SERVICES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "elena_os_core.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

uint8_t eos_battery_service_get_level(void);

/**
 * @brief 启动电池检测服务
 * @return eos_result_t
 */
eos_result_t eos_battery_service_start(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_SERVICES_H */
