/**
 * @file elena_os_port.h
 * @brief ElenaOS porting
 * @author Sab1e
 * @date 2025-08-21
 */

#ifndef ELENA_OS_PORT_H
#define ELENA_OS_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "elena_os_core.h"
#include "elena_os_sensor.h"
#include "elena_os_fs_port.h"
#include "elena_os_port_sensor.h"
#include "elena_os_time.h"
/* Public macros ----------------------------------------------*/
/**
 * @brief Function weak definition macro
 */
#ifdef __CC_ARM /* ARM Compiler */
    #define EOS_WEAK __weak
#elif defined(__IAR_SYSTEMS_ICC__) /* for IAR Compiler */
    #define EOS_WEAK __weak
#elif defined(__GNUC__) /* GNU GCC Compiler */
    #define EOS_WEAK __attribute__((weak))
#elif defined(__ADSPBLACKFIN__) /* for VisualDSP++ Compiler */
    #define EOS_WEAK __attribute__((weak))
#elif defined(_MSC_VER)
    #define EOS_WEAK
#elif defined(__TI_COMPILER_VERSION__)
    #define EOS_WEAK
#else
    #error not supported tool chain
#endif

#define EOS_TIMEOUT_INFINITE UINT32_MAX

/* Public typedefs --------------------------------------------*/
typedef struct eos_sem_t eos_sem_t;   /**< Semaphore */
/* Public function prototypes --------------------------------*/

/**
 * @brief Create a semaphore
 * @param initial_count Initial count
 * @param max_count Maximum count
 * @return Pointer to semaphore object, returns NULL if failed
 */
eos_sem_t* eos_sem_create(uint32_t initial_count, uint32_t max_count);
/**
 * @brief Destroy semaphore
 */
void eos_sem_destroy(eos_sem_t* sem);
/**
 * @brief Wait for semaphore
 * @param timeout_ms Timeout time (milliseconds), 0 for non-blocking, EOS_TIMEOUT_INFINITE for infinite wait
 * @return true Successfully obtained
 * @return false Timeout or failed
 */
bool eos_sem_take(eos_sem_t* sem, uint32_t timeout_ms);
/**
 * @brief Release semaphore
 */
void eos_sem_give(eos_sem_t* sem);
/**
 * @brief Delay for specified time (non-blocking)
 * @param ms Milliseconds
 */
void eos_delay(uint32_t ms);
/**
 * @brief System reset
 */
void eos_cpu_reset();
/**
 * @brief Enable Bluetooth
 * @warning To avoid blocking UI thread, do not initialize Bluetooth protocol stack here, only send Bluetooth start message to other threads.
 * @note Creating threads also belongs to operations that easily block threads
 */
void eos_bluetooth_enable(void);
/**
 * @brief Disable Bluetooth
 */
void eos_bluetooth_disable(void);
/**
 * @brief Get current time structure
 * @return eos_datetime_t Time structure
 * @note Recommended to use RTC to get time
 * @warning Please sync time by yourself to ensure accurate time
 */
eos_datetime_t eos_time_get_core(void);
/**
 * @brief Set screen brightness
 * @param brightness Brightness value (0~100)
 */
void eos_display_set_brightness(uint8_t brightness);
/**
 * @brief Locate phone
 *
 * Make phone ring via Bluetooth or other methods to locate phone.
 */
void eos_locate_phone(void);
/**
 * @brief Set speaker volume
 * @param volume Volume
 */
void eos_speaker_set_volume(uint8_t volume);
/**
 * @brief System enters sleep mode (low power state)
 *
 * This function needs to complete:
 *
 * - Stop touch
 *
 * - Turn off screen
 *
 * - Turn off other peripherals
 *
 * - CPU frequency reduction, enter low power
 *
 */
void eos_sys_sleep(void);
/**
 * @brief System enters always-on display mode
 *
 * This function needs to complete:
 *
 * - Stop touch
 *
 * - Turn off other peripherals
 *
 * - CPU frequency reduction, enter low power
 *
 */
void eos_sys_aod(void);
/**
 * @brief System exits sleep mode
 *
 * This function needs to complete:
 *
 * - Wake up peripherals turned off by `eos_sys_sleep()`, turn off low power mode
 *
 */
void eos_sys_wake(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_PORT_H */
