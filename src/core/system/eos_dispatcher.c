/**
 * @file eos_dispatcher.c
 * @brief Task dispatcher
 */

#include "eos_dispatcher.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "eos_port.h"
#include "eos_config.h"
#include "eos_mem.h"
/* Macros and Definitions -------------------------------------*/
typedef struct
{
    eos_dispatcher_cb_t cb;
    void *user_data;
} eos_dispatcher_item_t;
/* Variables --------------------------------------------------*/
static eos_dispatcher_item_t *s_queue = NULL;
static volatile int s_head = 0;
static volatile int s_tail = 0;
static int s_capacity = 0;
static eos_sem_t *sem;
#if EOS_COMPILE_MODE == DEUBG
static bool async_initialized = false;
#endif /* EOS_COMPILE_MODE */
/* Function Implementations -----------------------------------*/

void eos_dispatcher_init(void)
{
#if EOS_COMPILE_MODE == DEUBG
    if (async_initialized)
        return;
#endif /* EOS_COMPILE_MODE */
    sem = eos_sem_create(1, 1);

    s_capacity = 8;
    s_queue = eos_malloc(sizeof(eos_dispatcher_item_t) * s_capacity);
    if (!s_queue)
    {
        s_capacity = 0;
    }
    s_head = 0;
    s_tail = 0;
#if EOS_COMPILE_MODE == DEUBG
    async_initialized = true;
#endif /* EOS_COMPILE_MODE */
}

static int _queue_expand(void)
{
    int new_capacity = s_capacity * 2;
    eos_dispatcher_item_t *new_queue = eos_malloc(sizeof(eos_dispatcher_item_t) * new_capacity);
    if (!new_queue)
        return -1;

    int i = 0;
    for (int idx = s_head; idx != s_tail; idx = (idx + 1) % s_capacity)
    {
        new_queue[i++] = s_queue[idx];
    }

    eos_free(s_queue);
    s_queue = new_queue;
    s_head = 0;
    s_tail = i;
    s_capacity = new_capacity;
    return 0;
}

void eos_dispatcher_call(eos_dispatcher_cb_t cb, void *user_data)
{
#if EOS_COMPILE_MODE == DEUBG
    if(!async_initialized)return;
#endif /* EOS_COMPILE_MODE */
    if (!cb)
        return;

    eos_sem_take(sem, EOS_ASYNC_SEM_TIMEOUT);

    int next = (s_tail + 1) % s_capacity;
    if (next == s_head)
    {
        if (_queue_expand() < 0)
        {
            eos_sem_give(sem);
            return;
        }
        next = (s_tail + 1) % s_capacity;
    }

    s_queue[s_tail].cb = cb;
    s_queue[s_tail].user_data = user_data;
    s_tail = next;

    eos_sem_give(sem);
}

void eos_dispatch_tick(void)
{
#if EOS_COMPILE_MODE == DEUBG
    if(!async_initialized)return;
#endif /* EOS_COMPILE_MODE */
    eos_sem_take(sem, EOS_ASYNC_SEM_TIMEOUT);

    while (s_head != s_tail)
    {
        eos_dispatcher_item_t item = s_queue[s_head];
        s_head = (s_head + 1) % s_capacity;

        eos_sem_give(sem);
        item.cb(item.user_data);
        eos_sem_take(sem, EOS_ASYNC_SEM_TIMEOUT);
    }

    eos_sem_give(sem);
}