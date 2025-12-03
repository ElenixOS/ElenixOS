/**
 * @file elena_os_darray.h
 * @brief 动态数组
 * @author Sab1e
 * @date 2025-12-03
 */

#ifndef ELENA_OS_DARRAY_H
#define ELENA_OS_DARRAY_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/* Public macros ----------------------------------------------*/
typedef struct eos_darray_t eos_darray_t;
/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

/**
 * @brief 创建一个动态数组，并初始化容量
 * @param init_capacity 初始容量，如果为0则默认4
 * @return eos_darray_t* 返回创建的动态数组指针，失败返回NULL
 */
eos_darray_t *eos_darray_create(size_t init_capacity);

/**
 * @brief 设置指定索引的数据，如果索引超出当前容量会自动扩容
 * @param arr 动态数组指针
 * @param index 要设置的数据索引
 * @param data 要设置的数据指针
 * @return true 设置成功
 * @return false 设置失败（如内存分配失败或arr为NULL）
 */
bool eos_darray_set(eos_darray_t *arr, size_t index, void *data);

/**
 * @brief 获取指定索引的数据，如果启用收缩机制，可能在获取时自动收缩容量
 * @param arr 动态数组指针
 * @param index 要获取的数据索引
 * @return void* 返回对应索引的数据指针，越界或arr为NULL返回NULL
 */
void *eos_darray_get(eos_darray_t *arr, size_t index);

/**
 * @brief 获取动态数组当前有效元素数量
 * @param arr 动态数组指针
 * @return size_t 返回当前有效元素数量，如果arr为NULL返回0
 */
size_t eos_darray_get_size(eos_darray_t *arr);

/**
 * @brief 销毁动态数组，释放内存
 * @param arr 动态数组指针
 */
void eos_darray_destroy(eos_darray_t *arr);

#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_DARRAY_H */
