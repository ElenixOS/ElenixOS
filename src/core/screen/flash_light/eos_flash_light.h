/**
 * @file eos_flash_light.h
 * @brief Flashlight
 */

#ifndef ELENIX_OS_FLASH_LIGHT_H
#define ELENIX_OS_FLASH_LIGHT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

/**
 * @brief Quick display flashlight, mainly used for `control_center`.
 */
void eos_flash_light_show(void);
/**
 * @brief Enter flashlight page, used for app list.
 */
void eos_flash_light_enter(void);

#ifdef __cplusplus
}
#endif

#endif /* ELENIX_OS_FLASH_LIGHT_H */
