/**
 * @file elena_os_cqueue.h
 * @brief 可扩容的循环队列 FIFO
 * @author Sab1e
 * @date 2025-11-22
 */

#ifndef ELENA_OS_CQUEUE_H
#define ELENA_OS_CQUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/
typedef struct eos_cqueue_t eos_cqueue_t;
/* Public function prototypes --------------------------------*/

/**
 * @brief 创建一个循环队列
 * @param init_capacity 初始容量
 * @return eos_cqueue_t* 创建成功则返回循环队列指针，否则返回 NULL
 */
eos_cqueue_t *eos_cqueue_create(size_t init_capacity);
/**
 * @brief 循环队列入队
 * @param cq 目标循环队列
 * @param data 要存储的数据
 * @return true 入队成功
 * @return false 入队失败
 */
bool eos_cqueue_enqueue(eos_cqueue_t *cq, void *data);
/**
 * @brief 循环队列出队
 * @param cq 目标循环队列
 * @return void* 出队成功则返回队首的数据指针，否则返回 NULL
 */
void *eos_cqueue_dequeue(eos_cqueue_t *cq);
/**
 * @brief 销毁循环队列
 */
void eos_cqueue_destroy(eos_cqueue_t *cq);
/**
 * @brief 获取循环队列大小
 * @param cq 循环队列指针
 * @return size_t 队列内元素总数，队列指针无效则返回 0
 */
size_t eos_cqueue_get_size(eos_cqueue_t *cq);
/**
 * @brief 从循环队列中按索引读取元素
 * @param cq 循环队列指针
 * @param index 索引（0对应head）
 * @return 队列中元素指针，索引越界返回NULL
 */
void *eos_cqueue_peek(eos_cqueue_t *cq, size_t index);

#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_CQUEUE_H */
