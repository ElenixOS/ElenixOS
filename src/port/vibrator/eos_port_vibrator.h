/**
 * @file eos_port_vibrator.h
 * @brief Vibrator
 */

#ifndef ELENIX_OS_PORT_VIBRATOR_H
#define ELENIX_OS_PORT_VIBRATOR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

/**
 * @brief Turn on vibrator
 * @param strength Vibration strength [0,255]
 */
void eos_vibrator_on(uint8_t strength);
/**
 * @brief Turn off vibrator
 */
void eos_vibrator_off(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENIX_OS_PORT_VIBRATOR_H */
