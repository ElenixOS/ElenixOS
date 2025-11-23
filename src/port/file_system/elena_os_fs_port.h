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
#include "elena_os_config.h"
/* Public macros ----------------------------------------------*/
#if EOS_FS_TYPE == EOS_FS_POSIX
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
typedef FILE* eos_file_t;
typedef DIR* eos_dir_t;
#define EOS_FILE_INVALID NULL
#define EOS_DIR_INVALID NULL

#elif EOS_FS_TYPE == EOS_FS_FATFS
#include "ff.h"
typedef FIL* eos_file_t;
typedef FF_DIR* eos_dir_t;
#define EOS_FILE_INVALID NULL
#define EOS_DIR_INVALID NULL

#elif EOS_FS_TYPE == EOS_FS_LITTLEFS
#include "lfs.h"
typedef lfs_file_t* eos_file_t;
typedef lfs_dir_t* eos_dir_t;
#define EOS_FILE_INVALID NULL
#define EOS_DIR_INVALID NULL

#elif EOS_FS_TYPE == EOS_FS_RTTHREAD
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
typedef int eos_file_t;
typedef DIR* eos_dir_t;
#define EOS_FILE_INVALID (-1)
#define EOS_DIR_INVALID NULL

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

typedef struct
{
    eos_file_t file;
    void *data;
    size_t size;
} eos_async_write_task_t;
/* Public function prototypes --------------------------------*/

/**
 * @brief 打开一个文件，只读模式
 * @param path 文件路径
 * @return void* 文件句柄，失败返回 NULL
 */
eos_file_t eos_fs_open_read(const char *path);

/**
 * @brief 打开一个文件，只写模式，如果文件不存在会创建
 * @param path 文件路径
 * @return void* 文件句柄，失败返回 NULL
 */
eos_file_t eos_fs_open_write(const char *path);

/**
 * @brief 从文件中读取数据
 * @param file 文件句柄
 * @param buf 数据缓冲区
 * @param len 要读取的字节数
 * @return int 实际读取的字节数，出错返回 -1
 */
int eos_fs_read(eos_file_t file, void *buf, size_t len);

/**
 * @brief 向文件中写入数据
 * @param file 文件句柄
 * @param buf 数据缓冲区
 * @param len 要写入的字节数
 * @return int 实际写入的字节数，出错返回 -1
 */
int eos_fs_write(eos_file_t file, const void *buf, size_t len);

/**
 * @brief 文件指针定位
 * @param file 文件句柄
 * @param pos 文件偏移位置（从文件头开始）
 * @return int 成功返回 0，失败返回 -1
 */
int eos_fs_seek(eos_file_t file, uint32_t pos);

/**
 * @brief 获取文件大小
 * @param file 文件句柄
 * @param size 输出文件大小（单位字节）
 * @return int 成功返回 0，失败返回 -1
 */
int eos_fs_size(eos_file_t file, uint32_t *size);

/**
 * @brief 关闭文件
 * @param file 文件句柄
 */
void eos_fs_close(eos_file_t file);

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

/**
 * @brief 打开目录
 * @param path 目录路径
 * @return eos_dir_t* 打开成功则返回目录指针，否则返回 NULL
 */
eos_dir_t eos_fs_opendir(const char *path);

/**
 * @brief 读取目录中的下一个文件名
 *
 * 该函数从指定的目录流中读取下一个文件或子目录的名称。
 * 每次调用返回目录中的下一个条目，直到目录结束。
 *
 * @param dir 指向已打开的目录流的指针（通过 eos_fs_opendir 打开）
 * @param name 用于存放读取到的文件名的缓冲区
 * @param max_len name 缓冲区的最大长度（包括结尾的 '\0'）
 *
 * @return int
 * @retval 0 读取成功，name 中存放了文件名
 * @retval -1 读取失败（如 dir 为 NULL 或目录读取结束）
 *
 * @note 如果文件名长度超过 max_len-1，则会被截断，并保证以 '\0' 结尾
 * @note 该函数仅返回文件名，不包含路径
 */
int eos_fs_readdir(eos_dir_t dir, char *name, size_t max_len);

/**
 * @brief 关闭文件目录
 * @param dir 目标文件目录指针
 */
void eos_fs_closedir(eos_dir_t dir);

/**
 * @brief 移动文件或重命名
 * @param old_path
 * @param new_path
 * @return int 成功返回 0，失败返回 -1
 */
int eos_fs_mv(const char *old_path, const char *new_path);

/**
 * @brief 同步文件数据
 * @param file 文件
 * @return int
 */
int eos_fs_sync(eos_file_t file);

/**
 * @brief 异步写入整个文件
 * @param file 文件
 * @return int 成功返回 0，失败返回 -1
 * @note 必须创建一个线程写入文件，否中会阻塞 UI
 */
int eos_fs_async_write(eos_file_t file, void *data, size_t data_size);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_FS_PORT_H */
