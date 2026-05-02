/**
 * @file eos_port_mutex.h
 * @brief Mutex porting layer
 */

#ifndef EOS_PORT_MUTEX_H
#define EOS_PORT_MUTEX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/
typedef struct eos_mutex_t eos_mutex_t;

/* Public function prototypes --------------------------------*/

/**
 * @brief Create a mutex
 * @return Pointer to mutex object, returns NULL if failed
 */
eos_mutex_t *eos_mutex_create(void);

/**
 * @brief Destroy mutex
 * @param mutex Mutex pointer
 */
void eos_mutex_destroy(eos_mutex_t *mutex);

/**
 * @brief Lock mutex
 * @param mutex Mutex pointer
 * @param timeout_ms Timeout time (milliseconds), 0 for non-blocking, EOS_TIMEOUT_INFINITE for infinite wait
 * @return true Successfully locked, false Timeout or failed
 */
bool eos_mutex_lock(eos_mutex_t *mutex, uint32_t timeout_ms);

/**
 * @brief Unlock mutex
 * @param mutex Mutex pointer
 * @return true Successfully unlocked, false Failed
 */
bool eos_mutex_unlock(eos_mutex_t *mutex);

#ifdef __cplusplus
}
#endif

#endif /* EOS_PORT_MUTEX_H */
