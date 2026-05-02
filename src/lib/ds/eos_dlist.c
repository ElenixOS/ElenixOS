/**
 * @file eos_dlist.c
 * @brief Doubly linked list
 */

#include "eos_dlist.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#define EOS_LOG_TAG "DList"
#include "eos_log.h"
#include "eos_mem.h"

/* Macros and Definitions -------------------------------------*/

struct eos_dlist_node_t
{
    void *data;
    struct eos_dlist_node_t *prev;
    struct eos_dlist_node_t *next;
};

struct eos_dlist_t
{
    struct eos_dlist_node_t *head;
    struct eos_dlist_node_t *tail;
    size_t size;
};

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

eos_dlist_t *eos_dlist_create(void)
{
    eos_dlist_t *list = eos_malloc_zeroed(sizeof(eos_dlist_t));
    EOS_CHECK_PTR_RETURN_VAL(list, NULL);

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;

    EOS_LOG_I("dlist created");
    return list;
}

void eos_dlist_destroy(eos_dlist_t *list)
{
    EOS_CHECK_PTR_RETURN(list);

    eos_dlist_clear(list);
    eos_free(list);
    EOS_LOG_I("dlist destroyed");
}

bool eos_dlist_push_front(eos_dlist_t *list, void *data)
{
    EOS_CHECK_PTR_RETURN_VAL(list, false);

    struct eos_dlist_node_t *node = eos_malloc_zeroed(sizeof(struct eos_dlist_node_t));
    EOS_CHECK_PTR_RETURN_VAL(node, false);

    node->data = data;
    node->prev = NULL;
    node->next = list->head;

    if (list->head)
        list->head->prev = node;
    else
        list->tail = node;

    list->head = node;
    list->size++;

    EOS_LOG_I("push_front data[%p]", data);
    return true;
}

bool eos_dlist_push_back(eos_dlist_t *list, void *data)
{
    EOS_CHECK_PTR_RETURN_VAL(list, false);

    struct eos_dlist_node_t *node = eos_malloc_zeroed(sizeof(struct eos_dlist_node_t));
    EOS_CHECK_PTR_RETURN_VAL(node, false);

    node->data = data;
    node->prev = list->tail;
    node->next = NULL;

    if (list->tail)
        list->tail->next = node;
    else
        list->head = node;

    list->tail = node;
    list->size++;

    EOS_LOG_I("push_back data[%p]", data);
    return true;
}

bool eos_dlist_insert_at(eos_dlist_t *list, size_t index, void *data)
{
    EOS_CHECK_PTR_RETURN_VAL(list, false);

    if (index > list->size)
        return false;

    if (index == 0)
        return eos_dlist_push_front(list, data);

    if (index == list->size)
        return eos_dlist_push_back(list, data);

    struct eos_dlist_node_t *node = eos_malloc_zeroed(sizeof(struct eos_dlist_node_t));
    EOS_CHECK_PTR_RETURN_VAL(node, false);

    node->data = data;

    struct eos_dlist_node_t *cur = list->head;
    for (size_t i = 0; i < index; i++)
        cur = cur->next;

    node->prev = cur->prev;
    node->next = cur;
    cur->prev->next = node;
    cur->prev = node;
    list->size++;

    EOS_LOG_I("insert_at data[%p] at index %zu", data, index);
    return true;
}

void *eos_dlist_pop_front(eos_dlist_t *list)
{
    EOS_CHECK_PTR_RETURN_VAL(list, NULL);

    if (list->size == 0)
        return NULL;

    struct eos_dlist_node_t *node = list->head;
    void *data = node->data;

    list->head = node->next;
    if (list->head)
        list->head->prev = NULL;
    else
        list->tail = NULL;

    list->size--;
    eos_free(node);

    EOS_LOG_I("pop_front data[%p]", data);
    return data;
}

void *eos_dlist_pop_back(eos_dlist_t *list)
{
    EOS_CHECK_PTR_RETURN_VAL(list, NULL);

    if (list->size == 0)
        return NULL;

    struct eos_dlist_node_t *node = list->tail;
    void *data = node->data;

    list->tail = node->prev;
    if (list->tail)
        list->tail->next = NULL;
    else
        list->head = NULL;

    list->size--;
    eos_free(node);

    EOS_LOG_I("pop_back data[%p]", data);
    return data;
}

void *eos_dlist_remove_at(eos_dlist_t *list, size_t index)
{
    EOS_CHECK_PTR_RETURN_VAL(list, NULL);

    if (index >= list->size)
        return NULL;

    if (index == 0)
        return eos_dlist_pop_front(list);

    if (index == list->size - 1)
        return eos_dlist_pop_back(list);

    struct eos_dlist_node_t *cur = list->head;
    for (size_t i = 0; i < index; i++)
        cur = cur->next;

    cur->prev->next = cur->next;
    cur->next->prev = cur->prev;

    void *data = cur->data;
    list->size--;
    eos_free(cur);

    EOS_LOG_I("remove_at data[%p] at index %zu", data, index);
    return data;
}

bool eos_dlist_remove(eos_dlist_t *list, void *data)
{
    EOS_CHECK_PTR_RETURN_VAL(list, false);

    struct eos_dlist_node_t *cur = list->head;
    while (cur)
    {
        if (cur->data == data)
        {
            if (cur->prev)
                cur->prev->next = cur->next;
            else
                list->head = cur->next;

            if (cur->next)
                cur->next->prev = cur->prev;
            else
                list->tail = cur->prev;

            list->size--;
            eos_free(cur);

            EOS_LOG_I("remove data[%p]", data);
            return true;
        }
        cur = cur->next;
    }

    return false;
}

void eos_dlist_clear(eos_dlist_t *list)
{
    EOS_CHECK_PTR_RETURN(list);

    struct eos_dlist_node_t *cur = list->head;
    while (cur)
    {
        struct eos_dlist_node_t *next = cur->next;
        eos_free(cur);
        cur = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;

    EOS_LOG_I("dlist cleared");
}

void *eos_dlist_get_front(eos_dlist_t *list)
{
    EOS_CHECK_PTR_RETURN_VAL(list, NULL);

    if (list->size == 0)
        return NULL;

    return list->head->data;
}

void *eos_dlist_get_back(eos_dlist_t *list)
{
    EOS_CHECK_PTR_RETURN_VAL(list, NULL);

    if (list->size == 0)
        return NULL;

    return list->tail->data;
}

void *eos_dlist_get_at(eos_dlist_t *list, size_t index)
{
    EOS_CHECK_PTR_RETURN_VAL(list, NULL);

    if (index >= list->size)
        return NULL;

    struct eos_dlist_node_t *cur = list->head;
    for (size_t i = 0; i < index; i++)
        cur = cur->next;

    return cur->data;
}

size_t eos_dlist_get_size(eos_dlist_t *list)
{
    EOS_CHECK_PTR_RETURN_VAL(list, 0);
    return list->size;
}

bool eos_dlist_is_empty(eos_dlist_t *list)
{
    if (!list)
        return true;
    return list->size == 0;
}

int eos_dlist_find(eos_dlist_t *list, void *data)
{
    EOS_CHECK_PTR_RETURN_VAL(list, -1);

    struct eos_dlist_node_t *cur = list->head;
    size_t index = 0;

    while (cur)
    {
        if (cur->data == data)
            return (int)index;
        cur = cur->next;
        index++;
    }

    return -1;
}

bool eos_dlist_reverse(eos_dlist_t *list)
{
    EOS_CHECK_PTR_RETURN_VAL(list, false);

    if (list->size <= 1)
        return true;

    struct eos_dlist_node_t *cur = list->head;
    struct eos_dlist_node_t *tmp = NULL;

    while (cur)
    {
        tmp = cur->prev;
        cur->prev = cur->next;
        cur->next = tmp;
        cur = cur->prev;
    }

    tmp = list->head;
    list->head = list->tail;
    list->tail = tmp;

    EOS_LOG_I("dlist reversed");
    return true;
}

bool eos_dlist_iterate(eos_dlist_t *list, eos_dlist_iter_cb_t callback, void *user_data)
{
    EOS_CHECK_PTR_RETURN_VAL(list, false);
    EOS_CHECK_PTR_RETURN_VAL(callback, false);

    struct eos_dlist_node_t *cur = list->head;
    while (cur)
    {
        if (!callback(cur->data, user_data))
            return false;
        cur = cur->next;
    }

    return true;
}

void **eos_dlist_to_array(eos_dlist_t *list, size_t *out_size)
{
    if (!list || !out_size)
        return NULL;

    *out_size = 0;

    if (list->size == 0)
        return NULL;

    void **arr = eos_malloc(list->size * sizeof(void *));
    EOS_CHECK_PTR_RETURN_VAL(arr, NULL);

    struct eos_dlist_node_t *cur = list->head;
    size_t i = 0;

    while (cur)
    {
        arr[i++] = cur->data;
        cur = cur->next;
    }

    *out_size = list->size;
    return arr;
}
