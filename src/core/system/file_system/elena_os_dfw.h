/**
 * @file elena_os_dfw.h
 * @brief 延后文件写入器（Deffered File Writer）
 * @author Sab1e
 * @date 2025-11-25
 */

#ifndef ELENA_OS_DFW_H
#define ELENA_OS_DFW_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

/**
 * @brief 同步缓冲区到文件系统
 */
void eos_dfw_sync(void);

/**
 * @brief 写入数据
 *
 * 先从缓冲列表中查找有没有相同路径的文件：
 *
 * - 如果有，直接写入到缓冲区并返回
 *
 * - 如果没有，创建新的缓冲区并加入到缓冲区队列
 * @param path 目标文件路径
 * @param data 数据
 * @param data_size 数据大小
 * @return true 任务已加入队列，下次调用`eos_dfw_sync()`时将写入文件
 * @return false 失败
 */
bool eos_dfw_write(const char *path, uint8_t *data, size_t data_size);

/**
 * @brief 读取文件数据
 * @param path 文件路径
 * @return uint8_t* 读取成功返回文件内容，读取失败返回 NULL
 * @note 需要手动释放读取到的数据指针
 */
uint8_t *eos_dfw_read(const char *path);

/**
 * @brief 初始化延后文件写入器
 */
void eos_dfw_init(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_DFW_H */
