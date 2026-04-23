/**
 * @file elena_os_services.h
 * @brief System services
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

/**
 * @brief Get battery level
 * @return uint8_t Returns battery level, range: 0~100
 */
uint8_t eos_battery_service_get_level(void);

/**
 * @brief Check if battery is charging
 * @return true
 * @return false
 */
bool eos_battery_service_get_charging(void);

/**
 * @brief Start services
 */
void eos_services_start(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_SERVICES_H */