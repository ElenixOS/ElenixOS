/**
 * @file elena_os_darray.c
 * @brief 动态数组
 * @author Sab1e
 * @date 2025-12-03
 */

#include "elena_os_darray.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elena_os_mem.h"

/* Macros and Definitions -------------------------------------*/
#define _SHRINK_ENABLE 1
#define _SHRINK_THRESHOLD 4
#define _SHRINK_PROPORTION 2
#define _CAPACITY_GROWTH 2

struct eos_darray_t
{
    void **buffer;
    size_t size;
    size_t capacity;
    size_t min_capacity;
};
/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

static bool _darray_expand(eos_darray_t *arr)
{
    size_t new_capacity = arr->capacity * _CAPACITY_GROWTH;
    void **new_buffer = eos_malloc_zeroed(new_capacity * sizeof(void *));
    if (!new_buffer)
        return false;

    memcpy(new_buffer, arr->buffer, arr->size * sizeof(void *));
    eos_free(arr->buffer);
    arr->buffer = new_buffer;
    arr->capacity = new_capacity;
    return true;
}

static bool _darray_shrink(eos_darray_t *arr)
{
#if _SHRINK_ENABLE
    if (arr->capacity <= arr->min_capacity)
        return true;
    if (arr->size > arr->capacity / _SHRINK_THRESHOLD)
        return true;

    size_t new_capacity = arr->capacity / _SHRINK_PROPORTION;
    if (new_capacity < arr->min_capacity)
        new_capacity = arr->min_capacity;

    void **new_buffer = eos_malloc_zeroed(new_capacity * sizeof(void *));
    if (!new_buffer)
        return false;

    memcpy(new_buffer, arr->buffer, arr->size * sizeof(void *));
    eos_free(arr->buffer);
    arr->buffer = new_buffer;
    arr->capacity = new_capacity;
#endif
    return true;
}

eos_darray_t *eos_darray_create(size_t init_capacity)
{
    if (init_capacity == 0)
        init_capacity = 4;

    eos_darray_t *arr = eos_malloc_zeroed(sizeof(eos_darray_t));
    if (!arr)
        return NULL;

    arr->buffer = eos_malloc_zeroed(init_capacity * sizeof(void *));
    if (!arr->buffer)
    {
        eos_free(arr);
        return NULL;
    }

    arr->size = 0;
    arr->capacity = init_capacity;
    arr->min_capacity = init_capacity;
    return arr;
}

bool eos_darray_set(eos_darray_t *arr, size_t index, void *data)
{
    if (!arr)
        return false;

    // 如果索引超过当前容量，先扩容直到够
    while (index >= arr->capacity)
    {
        if (!_darray_expand(arr))
            return false;
    }

    arr->buffer[index] = data;
    if (index >= arr->size)
        arr->size = index + 1;

    return true;
}

void *eos_darray_get(eos_darray_t *arr, size_t index)
{
    if (!arr || index >= arr->size)
        return NULL;

    void *data = arr->buffer[index];

#if _SHRINK_ENABLE
    _darray_shrink(arr);
#endif

    return data;
}

size_t eos_darray_get_size(eos_darray_t *arr)
{
    if (!arr)
        return 0;
    return arr->size;
}

void eos_darray_destroy(eos_darray_t *arr)
{
    if (!arr)
        return;

    eos_free(arr->buffer);
    eos_free(arr);
}
