/**
 * @file elena_os_mem_port.c
 * @brief 内存分配
 * @author Sab1e
 * @date 2025-11-18
 */

#include "elena_os_config.h"

#if (EOS_MEM_ALLOC_STRATEGY == EOS_MEM_STDLIB) || (EOS_MEM_ALLOC_STRATEGY == EOS_MEM_CUSTOM)

#include "elena_os_mem_port.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elena_os_port.h"
#include "elena_os_log.h"
#include "elena_os_config.h"

/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

EOS_WEAK void *eos_malloc(size_t size)
{
    return malloc(size);
}

EOS_WEAK void *eos_malloc_zeroed(size_t size)
{
    return calloc(1, size);
}

EOS_WEAK void eos_free(void *ptr)
{
    free(ptr);
}

EOS_WEAK void *eos_realloc(void *ptr, size_t new_size)
{
    return realloc(ptr, new_size);
}

#endif /* EOS_MEM_ALLOC_STRATEGY */
