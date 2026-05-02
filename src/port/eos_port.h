/**
 * @file eos_port.h
 * @brief ElenixOS porting
 */

#ifndef EOS_PORT_H
#define EOS_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "eos_core.h"
#include "eos_sensor.h"
#include "eos_fs_port.h"
#include "eos_port_sensor.h"

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

typedef enum
{
    EOS_AUDIO_STATE_UNAVAILABLE = 0,
    EOS_AUDIO_STATE_READY = 1,
    EOS_AUDIO_STATE_BUSY = 2,
    EOS_AUDIO_STATE_ERROR = 3,
} eos_audio_state_t;
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
 * @brief Detect whether speaker hardware is available.
 * @return true if available
 */
bool eos_speaker_detect(void);

/**
 * @brief Detect whether microphone hardware is available.
 * @return true if available
 */
bool eos_microphone_detect(void);

/**
 * @brief Get platform audio playback state.
 */
eos_audio_state_t eos_audio_get_state(void);

/**
 * @brief Play audio file from path.
 * @param file_path path to audio file
 * @return 0 on success, negative value on failure
 */
int eos_audio_play_file(const char *file_path);

/**
 * @brief Stop current audio playback.
 * @return 0 on success, negative value on failure
 */
int eos_audio_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* EOS_PORT_H */
