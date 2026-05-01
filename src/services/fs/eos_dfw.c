/**
 * @file eos_dfw.c
 * @brief Deferred File Writer
 */

#include "eos_config.h"

#if EOS_DFW_ENABLE

#include "eos_dfw.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define EOS_LOG_TAG "DFW"
#include "eos_log.h"
#include "eos_cqueue.h"
#include "eos_fs.h"
#include "eos_misc.h"
#include "eos_port.h"
#include "eos_mem.h"
/* Macros and Definitions -------------------------------------*/
typedef struct
{
    char *path;
    uint8_t *data;
    size_t data_size;
} eos_dfw_cache_t;
/* Variables --------------------------------------------------*/
static eos_cqueue_t *cq = NULL;
/* Function Implementations -----------------------------------*/

static eos_dfw_cache_t *_find_cache(const char *path)
{
    for (int i = 0; i < eos_cqueue_get_size(cq); i++)
    {
        eos_dfw_cache_t *cache = eos_cqueue_peek(cq, i);
        if (strcmp(cache->path, path) == 0)
        {
            return cache;
        }
    }
    return NULL;
}

bool eos_dfw_write(const char *path, const uint8_t *data, size_t data_size)
{
    EOS_CHECK_PTR_RETURN_VAL(path && data, false);
    if (data_size == 0)
    {
        EOS_LOG_E("data_size == 0");
        return false;
    }
    eos_dfw_cache_t *cache = _find_cache(path);
    if (cache)
    {
        EOS_LOG_I("Cache found");
        if (cache->data)
            eos_free(cache->data);
        cache->data = eos_malloc(data_size + 1);
        memcpy(cache->data, data, data_size);
        cache->data[data_size] = '\0';
        cache->data_size = data_size;
        return true;
    }
    else
    {
        EOS_LOG_I("Cache not found");
        cache = eos_malloc_zeroed(sizeof(eos_dfw_cache_t));
        EOS_CHECK_PTR_RETURN_VAL(cache, false);
        cache->path = eos_malloc(strlen(path) + 1);
        strcpy(cache->path, path);
        cache->data = eos_malloc(data_size + 1);
        memcpy(cache->data, data, data_size);
        cache->data[data_size] = '\0';
        cache->data_size = data_size;
        if (eos_cqueue_enqueue(cq, cache))
        {
            EOS_LOG_I("Cache enqueued");
        }
        else
        {
            EOS_LOG_E("Failed to enqueue cache");
        }
        return true;
    }
}

uint8_t *eos_dfw_read(const char *path)
{
    EOS_CHECK_PTR_RETURN_VAL(path, NULL);
    eos_dfw_cache_t *cache = _find_cache(path);
    if (cache)
    {
        EOS_LOG_I("Cache found");
        uint8_t *copy = eos_malloc_zeroed(cache->data_size);
        memcpy(copy, cache->data, cache->data_size);
        return copy;
    }
    else
    {
        return (uint8_t *)eos_fs_read_file_immediate(path);
    }
}

void eos_dfw_sync(void)
{
    while (eos_cqueue_get_size(cq) > 0)
    {
        eos_dfw_cache_t *cache = eos_cqueue_dequeue(cq);
        EOS_CHECK_PTR_RETURN(cache);
        if (eos_fs_write_file_immediate(cache->path, cache->data, cache->data_size) < 0)
        {
            EOS_LOG_E("Write failed");
        }
        else
        {
            EOS_LOG_I("Write done");
        }
        eos_free(cache->path);
        eos_free(cache->data);
        eos_free(cache);
    }
}

void eos_dfw_init(void)
{
    cq = eos_cqueue_create(4);
}

#endif /* EOS_DFW_ENABLE */
