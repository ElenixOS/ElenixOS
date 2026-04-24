/**
 * @file eos_mem_port.c
 * @brief Memory allocation
 */

#include "eos_config.h"

#if (EOS_MEM_ALLOC_PROVIDER == EOS_MEM_PROVIDER_STDLIB_CLIB) || (EOS_MEM_ALLOC_PROVIDER == EOS_MEM_PROVIDER_CUSTOM)

#include "eos_mem_port.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eos_port.h"
#include "eos_log.h"
#include "eos_config.h"

/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

EOS_WEAK void *eos_malloc_core(size_t size)
{
    return malloc(size);
}

EOS_WEAK void *eos_malloc_zeroed_core(size_t size)
{
    return calloc(1, size);
}

EOS_WEAK void eos_free_core(void *ptr)
{
    free(ptr);
}

EOS_WEAK void *eos_realloc_core(void *ptr, size_t new_size)
{
    return realloc(ptr, new_size);
}

#endif /* EOS_MEM_ALLOC_PROVIDER */
