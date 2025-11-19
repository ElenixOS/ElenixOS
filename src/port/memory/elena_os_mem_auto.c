/**
 * @file elena_os_mem_auto.c
 * @brief 自动分配内存示例
 * @author Sab1e
 * @date 2025-11-18
 */

#include "elena_os_config.h"

#if EOS_MEM_ALLOC_STRATEGY == EOS_MEM_AUTO

#include "elena_os_mem_port.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elena_os_port.h"
#include "elena_os_log.h"
#include "elena_os_config.h"

/* Macros and Definitions -------------------------------------*/
#define EOS_MEM_POOL_ALLOC_THRESHOLD 1024 /**< 分配阈值，单位：字节 */

typedef enum
{
    EOS_MEM_POOL_FAST = 0,
    EOS_MEM_POOL_LARGE = 1
} eos_mem_type_t;

typedef struct
{
    uint8_t type;                                  /**< 内存类型 */
    uint8_t pad[sizeof(void *) - sizeof(uint8_t)]; /**< 内存对齐 */
} eos_mem_header_t;
/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

#define EOS_MEM_ALLOC_FAST      malloc
#define EOS_MEM_ALLOC_LARGE     malloc

#define EOS_MEM_FREE_FAST       free
#define EOS_MEM_FREE_LARGE      free

#define EOS_MEM_REALLOC_FAST    realloc
#define EOS_MEM_REALLOC_LARGE   realloc

#define EOS_MEM_CALLOC_FAST     calloc
#define EOS_MEM_CALLOC_LARGE    calloc

EOS_WEAK void *eos_malloc(size_t size)
{
    size_t total = size + sizeof(eos_mem_header_t);
    eos_mem_header_t *hdr;

    if (size < EOS_MEM_POOL_ALLOC_THRESHOLD)
        hdr = EOS_MEM_ALLOC_FAST(total);
    else
        hdr = EOS_MEM_ALLOC_LARGE(total);

    if (!hdr)
        return NULL;

    hdr->type = (size < EOS_MEM_POOL_ALLOC_THRESHOLD) ? EOS_MEM_POOL_FAST : EOS_MEM_POOL_LARGE;

    EOS_LOG_D("Auto memory alloc: [%s]%d", hdr->type == EOS_MEM_POOL_FAST ? "EOS_MEM_POOL_FAST" : "EOS_MEM_POOL_LARGE", total);

    return (void *)(hdr + 1);
}

EOS_WEAK void *eos_malloc_zeroed(size_t size)
{
    size_t total = size + sizeof(eos_mem_header_t);
    eos_mem_header_t *hdr;

    if (size < EOS_MEM_POOL_ALLOC_THRESHOLD)
        hdr = EOS_MEM_CALLOC_FAST(1, total);
    else
        hdr = EOS_MEM_CALLOC_LARGE(1, total);

    if (!hdr)
        return NULL;

    hdr->type = (size < EOS_MEM_POOL_ALLOC_THRESHOLD) ? EOS_MEM_POOL_FAST : EOS_MEM_POOL_LARGE;

    return (void *)(hdr + 1);
}

EOS_WEAK void eos_free(void *ptr)
{
    if (!ptr)
        return; // 添加NULL指针检查

    eos_mem_header_t *hdr = (eos_mem_header_t *)ptr - 1;

    switch (hdr->type)
    {
    case EOS_MEM_POOL_FAST:
        EOS_MEM_FREE_FAST(hdr);
        break;
    case EOS_MEM_POOL_LARGE:
        EOS_MEM_FREE_LARGE(hdr);
        break;
    default:
        EOS_LOG_E("Unknown memory type");
        break;
    }
}

EOS_WEAK void *eos_realloc(void *ptr, size_t new_size)
{
    if (!ptr)
        return eos_malloc(new_size);

    // 获取原内存块信息
    eos_mem_header_t *old_hdr = (eos_mem_header_t *)ptr - 1;
    eos_mem_type_t original_type = old_hdr->type;  // 保持原内存类型

    EOS_LOG_D("Realloc:  %zu, keep pool: %s",
               new_size,
              original_type == EOS_MEM_POOL_FAST ? "SRAM" : "PSRAM");

    // 始终在原始内存池中realloc
    size_t total = new_size + sizeof(eos_mem_header_t);
    eos_mem_header_t *new_hdr;

    if (original_type == EOS_MEM_POOL_FAST) {
        new_hdr = EOS_MEM_REALLOC_FAST(old_hdr, total);
    } else {
        new_hdr = EOS_MEM_REALLOC_LARGE(old_hdr, total);
    }

    if (!new_hdr) {
        EOS_LOG_E("Realloc failed for pool: %s",
                 original_type == EOS_MEM_POOL_FAST ? "SRAM" : "PSRAM");
        return NULL;
    }

    // 保持原来的内存类型
    new_hdr->type = original_type;

    EOS_LOG_D("Realloc success in original pool");
    return (void *)(new_hdr + 1);
}

#endif /* EOS_MEM_ALLOC_STRATEGY */
