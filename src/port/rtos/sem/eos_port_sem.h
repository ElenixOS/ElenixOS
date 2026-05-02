/**
 * @file eos_port_sem.h
 * @brief Semaphore porting layer
 */

#ifndef EOS_PORT_SEM_H
#define EOS_PORT_SEM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* Public macros ----------------------------------------------*/
#define EOS_TIMEOUT_INFINITE UINT32_MAX

/* Public typedefs --------------------------------------------*/
typedef struct eos_sem_t eos_sem_t;

/* Public function prototypes --------------------------------*/

/**
 * @brief Create a semaphore
 * @param initial_count Initial count
 * @param max_count Maximum count
 * @return Pointer to semaphore object, returns NULL if failed
 */
eos_sem_t *eos_sem_create(uint32_t initial_count, uint32_t max_count);

/**
 * @brief Destroy semaphore
 * @param sem Semaphore pointer
 */
void eos_sem_destroy(eos_sem_t *sem);

/**
 * @brief Wait for semaphore
 * @param sem Semaphore pointer
 * @param timeout_ms Timeout time (milliseconds), 0 for non-blocking, EOS_TIMEOUT_INFINITE for infinite wait
 * @return true Successfully obtained, false Timeout or failed
 */
bool eos_sem_take(eos_sem_t *sem, uint32_t timeout_ms);

/**
 * @brief Release semaphore
 * @param sem Semaphore pointer
 */
void eos_sem_give(eos_sem_t *sem);

#ifdef __cplusplus
}
#endif

#endif /* EOS_PORT_SEM_H */
