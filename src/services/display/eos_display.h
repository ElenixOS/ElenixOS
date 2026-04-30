/**
 * @file eos_display.h
 * @brief Display settings
 */

#ifndef ELENIX_OS_DISPLAY_H
#define ELENIX_OS_DISPLAY_H

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
 * @brief Set brightness (smooth transition)
 * @param start_brightness Start brightness
 * @param end_brightness End brightness
 * @param duration_ms Animation duration (milliseconds)
 */
void eos_display_set_brightness_smooth(uint8_t start_brightness, uint8_t end_brightness, uint32_t duration_ms);
/**
 * @brief Temporarily adjust brightness
 * @param target_brightness Target brightness
 */
void eos_display_tmp_set_brightness_smooth(uint8_t target_brightness);
/**
 * @brief Restore temporarily adjusted brightness
 */
void eos_display_tmp_restore_brightness_smooth(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENIX_OS_DISPLAY_H */