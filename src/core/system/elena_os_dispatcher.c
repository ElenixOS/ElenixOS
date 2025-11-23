/**
 * @file elena_os_dispatcher.c
 * @brief 任务调度器
 * @author Sab1e
 * @date 2025-11-10
 * @details
 *
 * # Async
 *
 * ## 简述
 *
 * ElenaOS 任务调度器提供一个轻量级的任务队列，用于在非实时上下文中延迟执行回调函数。
 *
 * 特性：
 * - 调用（`eos_dispatcher_call`）将回调推入队列；
 * - 队列处理（`eos_dispatch_tick`）依次执行所有待处理回调；
 * - 内部使用信号量保护队列，保证线程安全；
 * - 可配置动态队列大小，支持扩容；
 *
 * ## 使用场景
 *
 * - 在中断或任务中收集事件，但事件处理需要在主循环中执行；
 * - 避免在中断中执行耗时操作；
 * - 提供一种轻量的异步执行机制，不依赖完整的 RTOS 调度。
 *
 * ## 注意
 *
 * `eos_dispatcher_call()`与`lv_call_async()`不同，`lv_call_async()`只支持 GUI 线程调用，
 * 而`eos_dispatcher_call()`支持任意线程调用，切勿在非 GUI 线程调用任何 LVGL 函数。
 *
 */

#include "elena_os_dispatcher.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "elena_os_port.h"
#include "elena_os_config.h"
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

    s_capacity = 8; // 初始容量
    s_queue = (eos_dispatcher_item_t *)malloc(sizeof(eos_dispatcher_item_t) * s_capacity);
    if (!s_queue)
    {
        // 内存分配失败
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
    eos_dispatcher_item_t *new_queue = (eos_dispatcher_item_t *)malloc(sizeof(eos_dispatcher_item_t) * new_capacity);
    if (!new_queue)
        return -1;

    // 拷贝旧队列内容
    int i = 0;
    for (int idx = s_head; idx != s_tail; idx = (idx + 1) % s_capacity)
    {
        new_queue[i++] = s_queue[idx];
    }

    free(s_queue);
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
        // 队列满，尝试扩容
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
