/**
 * @file elena_os_afw.h
 * @brief 异步文件写入器（Async File Writer）
 * @author Sab1e
 * @date 2025-11-20
 */

#ifndef ELENA_OS_AFW_H
#define ELENA_OS_AFW_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

/**
 * @brief 异步文件写入器，需要在系统主循环调用此函数
 */
void eos_afw_handler(void);

/**
 * @brief 新增一个异步文件写入任务
 * @param path 目标文件路径
 * @param data 要写入的数据
 * @param data_size 要写入的大小
 * @return true 任务添加成功
 * @return false 任务添加失败
 * @note 目标文件会被直接覆盖
 */
bool eos_afw_add_task(const char *path, void *data, size_t data_size);

/**
 * @brief 初始化异步文件写入器
 */
void eos_afw_init(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_AFW_H */
