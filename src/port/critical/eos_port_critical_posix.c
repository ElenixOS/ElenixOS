/**
 * @file eos_port_critical_posix.c
 * @brief POSIX critical section implementation
 *
 * Since POSIX does not have interrupt disable/enable, we simulate
 * a critical section using a global pthread_mutex_t.
 *
 * NOTE: This only serializes access between POSIX threads and does
 * NOT provide real interrupt masking. It is suitable for user-space
 * multi-threaded simulation environments.
 */

#include "eos_config.h"

#if EOS_RTOS_TYPE == EOS_RTOS_POSIX

#include "eos_port_critical.h"

/* Includes ---------------------------------------------------*/
#include <pthread.h>
#include "eos_port.h"

/* Variables --------------------------------------------------*/
static pthread_mutex_t s_critical_mutex = PTHREAD_MUTEX_INITIALIZER;

eos_critical_ctx_t eos_critical_enter(void)
{
    pthread_mutex_lock(&s_critical_mutex);
    return 1;
}

void eos_critical_leave(eos_critical_ctx_t ctx)
{
    (void)ctx;
    pthread_mutex_unlock(&s_critical_mutex);
}

#endif /* EOS_RTOS_TYPE == EOS_RTOS_POSIX */
