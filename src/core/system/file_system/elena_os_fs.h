/**
 * @file elena_os_fs.h
 * @brief 文件系统
 * @author Sab1e
 * @date 2025-11-18
 */

#ifndef ELENA_OS_FS_H
#define ELENA_OS_FS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "elena_os_port.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

/**
 * @brief 判断目标路径是否为目录
 */
bool eos_is_dir(const char *path);

/**
 * @brief 判断目标路径是否为文件
 */
bool eos_is_file(const char *path);

/**
 * @brief 将字符串写入文件（自动写入结尾换行符）
 * @param s 要写入的字符串
 * @param fp 文件指针（eos_fs_open_write 或 eos_fs_open_readwrite 打开）
 * @return int 写入的字节数，或错误码（负值）
 */
int eos_fs_puts(const char *s, eos_file_t fp);

/**
 * @brief 判断目录是否存在，不存在则创建
 * @param path 目录路径
 * @return int 0 表示成功；否则返回错误码
 */
int eos_fs_mkdir_if_not_exist(const char *path);

/**
 * @brief 如果文件不存在，则创建并写入默认内容
 * @param path 文件路径
 * @param default_content 文件不存在时写入的默认内容，可为 NULL（创建空文件）
 * @return int 0 表示成功；否则返回错误码
 */
int eos_create_file_if_not_exist(const char *path, const char *default_content);

/**
 * @brief 读取整个文件内容到动态分配的缓冲区（只能读取文本文件）
 * @param path 文件路径
 * @return char* 返回动态分配的缓冲区；失败返回 NULL
 * @warning 使用完数据后必须使用`eos_free`释放内存
 */
char *eos_fs_read_file(const char *path);

/**
 * @brief 写入文件
 * @param path 文件路径
 * @param data 数据指针
 * @param data_size 数据长度
 * @return int 实际写入的字节数，出错返回 -1
 */
int eos_fs_write_file(const char *path, const void *data, size_t data_size);

/**
 * @brief 递归创建目录树
 * @param path 目标路径，例如 a/b/c
 * @return int 0 表示全部创建成功；否则返回错误码
 */
int eos_fs_mkdir_recursive(const char *path);

/**
 * @brief 递归删除目录及其内部所有文件和子目录
 * @param path 目录路径
 * @return int 0 表示成功；否则返回错误码
 */
int eos_fs_rm_recursive(const char *path);

#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_FS_H */
