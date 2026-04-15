/**
 * @file elena_os_stack.h
 * @brief 栈
 * @author Sab1e
 * @date 2026-03-20
 */

#ifndef ELENA_OS_STACK_H
#define ELENA_OS_STACK_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/
typedef struct eos_stack_t eos_stack_t;

typedef enum
{
	EOS_STACK_CAPACITY_FIXED = 0,
	EOS_STACK_CAPACITY_DYNAMIC,
} eos_stack_capacity_mode_t;
/* Public function prototypes --------------------------------*/

/**
 * @brief 创建一个栈
 * @param init_capacity 初始容量，传入0时默认容量为4
 * @return eos_stack_t* 创建成功返回栈指针，失败返回NULL
 */
eos_stack_t *eos_stack_create(size_t init_capacity);

/**
 * @brief 创建一个栈并指定容量模式
 * @param init_capacity 初始容量，传入0时默认容量为4
 * @param mode 容量模式：固定容量或动态容量
 * @return eos_stack_t* 创建成功返回栈指针，失败返回NULL
 */
eos_stack_t *eos_stack_create_with_mode(size_t init_capacity, eos_stack_capacity_mode_t mode);

/**
 * @brief 入栈
 * @param stack 栈指针
 * @param data 数据指针
 * @return true 入栈成功
 * @return false 入栈失败
 */
bool eos_stack_push(eos_stack_t *stack, void *data);

/**
 * @brief 出栈
 * @param stack 栈指针
 * @return void* 栈顶元素指针，栈为空或参数无效返回NULL
 */
void *eos_stack_pop(eos_stack_t *stack);

/**
 * @brief 获取栈顶元素但不出栈
 * @param stack 栈指针
 * @return void* 栈顶元素指针，栈为空或参数无效返回NULL
 */
void *eos_stack_peek(eos_stack_t *stack);

/**
 * @brief 获取栈当前元素数量
 * @param stack 栈指针
 * @return size_t 元素数量，参数无效时返回0
 */
size_t eos_stack_get_size(eos_stack_t *stack);

/**
 * @brief 获取栈容量模式
 * @param stack 栈指针
 * @return eos_stack_capacity_mode_t 栈容量模式，参数无效时返回固定模式
 */
eos_stack_capacity_mode_t eos_stack_get_capacity_mode(eos_stack_t *stack);

/**
 * @brief 销毁栈并释放内存
 * @param stack 栈指针
 */
void eos_stack_destroy(eos_stack_t *stack);

#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_STACK_H */
