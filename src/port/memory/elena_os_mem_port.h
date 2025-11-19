/**
 * @file elena_os_mem_port.h
 * @brief 内存分配
 * @author Sab1e
 * @date 2025-11-18
 */

#ifndef ELENA_OS_MEM_PORT_H
#define ELENA_OS_MEM_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "elena_os_port.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

/**
 * @brief 内存分配函数
 * @param size 内存大小，单位：字节
 * @return void* 分配成功则返回内存地址，否则返回 NULL
 */
void *eos_malloc(size_t size);
/**
 * @brief 分配一块连续内存并置 0
 * @param size 内存大小，单位：字节
 * @return void* 分配成功则返回内存地址，否则返回 NULL
 */
void *eos_malloc_zeroed(size_t size);
/**
 * @brief 释放目标内存
 * @param ptr 目标内存指针
 */
void eos_free(void *ptr);
/**
 * @brief 重新分配目标内存
 * @param ptr 目标内存指针
 * @param new_size 新的内存大小
 * @return void* 分配成功则返回内存地址，否则返回 NULL
 */
void *eos_realloc(void *ptr, size_t new_size);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_MEM_PORT_H */
