/**
 * @file eos_vibrator.h
 * @brief Vibrator driver
 */

#ifndef ELENIX_OS_VIBRATOR_H
#define ELENIX_OS_VIBRATOR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "eos_port_vibrator.h"
#include "lvgl.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/
typedef enum{
    EOS_VIBRATOR_STRENGTH_OFF = 0,
    EOS_VIBRATOR_STRENGTH_NORMAL = 128,
    EOS_VIBRATOR_STRENGTH_INTENSE = 255,
} eos_vibrator_strength_t;
/* Public function prototypes --------------------------------*/

/**
 * @brief Short and light tick vibration
 */
void eos_vibrator_tick(void);
/**
 * @brief Slightly stronger and longer vibration
 */
void eos_vibrator_buzz(void);
/**
 * @brief Strongest and longest vibration
 */
void eos_vibrator_vibrate_long(void);
/**
 * @brief Vibrator initialization
 */
void eos_vibrator_init(void);
/**
 * @brief Set global vibration strength
 */
void eos_vibrator_set_strength(eos_vibrator_strength_t s);
#ifdef __cplusplus
}
#endif

#endif /* ELENIX_OS_VIBRATOR_H */