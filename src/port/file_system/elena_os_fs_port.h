/**
 * @file elena_os_fs_port.h
 * @brief 文件系统移植
 * @author Sab1e
 * @date 2025-11-18
 */

#ifndef ELENA_OS_FS_PORT_H
#define ELENA_OS_FS_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "elena_os_config.h"
/* Public macros ----------------------------------------------*/
#if EOS_FS_TYPE == EOS_FS_POSIX
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
typedef FILE eos_file_t;
typedef DIR eos_dir_t;

#elif EOS_FS_TYPE == EOS_FS_FATFS
#include "ff.h"
typedef FIL eos_file_t;
typedef FF_DIR eos_dir_t;

#elif EOS_FS_TYPE == EOS_FS_LITTLEFS
#include "lfs.h"
typedef lfs_file_t eos_file_t;
typedef lfs_dir_t eos_dir_t;

#elif EOS_FS_TYPE == EOS_FS_CUSTOM
typedef EOS_FS_FILE_TYPE eos_file_t;
typedef EOS_FS_DIR_TYPE eos_dir_t;
#endif /* EOS_FS_TYPE */
/* Public typedefs --------------------------------------------*/
typedef enum {
    EOS_FS_TYPE_NOT_EXIST = 0,
    EOS_FS_TYPE_FILE = 1,
    EOS_FS_TYPE_DIR = 2
} eos_fs_type_t;
/* Public function prototypes --------------------------------*/

/**
 * @brief 打开一个文件，只读模式
 * @param path 文件路径
 * @return void* 文件句柄，失败返回 NULL
 */
eos_file_t *eos_fs_open_read(const char *path);

/**
 * @brief 打开一个文件，只写模式，如果文件不存在会创建
 * @param path 文件路径
 * @return void* 文件句柄，失败返回 NULL
 */
eos_file_t *eos_fs_open_write(const char *path);

/**
 * @brief 从文件中读取数据
 * @param fp 文件句柄
 * @param buf 数据缓冲区
 * @param len 要读取的字节数
 * @return int 实际读取的字节数，出错返回 -1
 */
int eos_fs_read(eos_file_t *fp, void *buf, size_t len);

/**
 * @brief 向文件中写入数据
 * @param fp 文件句柄
 * @param buf 数据缓冲区
 * @param len 要写入的字节数
 * @return int 实际写入的字节数，出错返回 -1
 */
int eos_fs_write(eos_file_t *fp, const void *buf, size_t len);

/**
 * @brief 文件指针定位
 * @param fp 文件句柄
 * @param pos 文件偏移位置（从文件头开始）
 * @return int 成功返回 0，失败返回 -1
 */
int eos_fs_seek(eos_file_t *fp, uint32_t pos);

/**
 * @brief 获取文件大小
 * @param fp 文件句柄
 * @param size 输出文件大小（单位字节）
 * @return int 成功返回 0，失败返回 -1
 */
int eos_fs_size(eos_file_t *fp, uint32_t *size);

/**
 * @brief 关闭文件
 * @param fp 文件句柄
 */
void eos_fs_close(eos_file_t *fp);

/**
 * @brief 创建目录（单级）
 * @param path 目录路径
 * @return int 成功返回 0，失败返回 -1
 */
int eos_fs_mkdir(const char *path);

/**
 * @brief 删除空目录
 * @param path 目录路径
 * @return int 成功返回 0，失败返回 -1
 */
int eos_fs_rmdir(const char *path);

/**
 * @brief 删除文件
 * @param path 文件路径
 * @return int 成功返回 0，失败返回 -1
 */
int eos_fs_remove(const char *path);

/**
 * @brief 检查文件或目录是否存在
 * @param path 文件或目录路径
 * @return int 1 表示存在，0 表示不存在，<0 表示错误
 */
int eos_fs_exists(const char *path);

/**
 * @brief 获取指定路径是文件还是目录
 * @param path 文件或目录路径
 * @return int 参见`eos_fs_type_t`
 */
int eos_fs_type(const char *path);

eos_dir_t *eos_fs_opendir(const char *path);
int eos_fs_readdir(eos_dir_t *dir, char *name, size_t max_len);
void eos_fs_closedir(eos_dir_t *dir);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_FS_PORT_H */
