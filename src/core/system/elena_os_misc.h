/**
 * @file elena_os_misc.h
 * @brief 各种工具函数
 * @author Sab1e
 * @date 2025-08-22
 */

#ifndef ELENA_OS_MISC_H
#define ELENA_OS_MISC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include "elena_os_core.h"
#include "script_engine_core.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/
/**
 * @brief 是否为有效的文件名
 * @param name 文件名称
 * @return eos_result_t 判断结果
 */
bool eos_is_valid_filename(const char *name);
/**
 * @brief 用于创建给定字符串 s 的副本。
 * @param s 目标字符串
 * @return const char* 创建成功则返回新分配的字符串指针
 *
 * 内存分配失败则返回 NULL
 * @note 使用`eos_malloc(str)`释放复制的字符串
 */
const char *eos_strdup(const char *s);
/**
 * @brief 释放并清空脚本包内的数据
 * @param pkg 目标脚本包
 */
void eos_pkg_free(script_pkg_t *pkg);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_MISC_H */
