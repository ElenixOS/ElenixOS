/**
 * @file eos_services.h
 * @brief System services
 */

#ifndef ELENIX_OS_SERVICES_H
#define ELENIX_OS_SERVICES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "eos_core.h"
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

#endif /* ELENIX_OS_SERVICES_H */