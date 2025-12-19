/**
 * @file elena_os_port_vibrator.h
 * @brief 震动器
 * @author Sab1e
 * @date 2025-12-10
 */

#ifndef ELENA_OS_PORT_VIBRATOR_H
#define ELENA_OS_PORT_VIBRATOR_H

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
 * @brief 震动器开启
 * @param strength 震动强度 [0,255]
 */
void eos_vibrator_on(uint8_t strength);
/**
 * @brief 震动器关闭
 */
void eos_vibrator_off(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_PORT_VIBRATOR_H */
