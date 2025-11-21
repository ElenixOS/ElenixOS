/**
 * @file elena_os_fs.c
 * @brief 文件系统相关工具函数
 * @author Sab1e
 * @date 2025-11-18
 */

#include "elena_os_fs.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elena_os_log.h"
/* Macros and Definitions -------------------------------------*/
#define _FILE_NAME_MAX_LENGTH 256
/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

bool eos_is_dir(const char *path)
{
    return (eos_fs_type(path) == EOS_FS_TYPE_DIR) ? true : false;
}

bool eos_is_file(const char *path)
{
    return (eos_fs_type(path) == EOS_FS_TYPE_FILE) ? true : false;
}

int eos_fs_puts(const char *s, eos_file_t *fp)
{
    if (!fp || !s)
        return -1;
    return eos_fs_write(fp, s, strlen(s));
}

int eos_fs_mkdir_if_not_exist(const char *path)
{
    if (!eos_fs_exists(path))
    {
        return eos_fs_mkdir(path);
    }
    return -1;
}

int eos_create_file_if_not_exist(const char *path, const char *default_content)
{
    EOS_CHECK_PTR_RETURN_VAL(path && default_content, -1);
    if (eos_fs_type(path) == EOS_FS_TYPE_NOT_EXIST)
    {
        eos_file_t *fp = eos_fs_open_write(path);
        EOS_CHECK_PTR_RETURN_VAL(fp, -1);

        ssize_t len = strlen(default_content);
        ssize_t written = eos_fs_write(fp, default_content, len);
        if (written != len)
        {
            EOS_LOG_E("write %s failed, written=%zd", path, written);
            eos_fs_close(fp);
            return -1;
        }

        eos_fs_close(fp);
        EOS_LOG_I("Created file: %s", path);
        return 0;
    }
    return -1;
}

int eos_fs_mkdir_recursive(const char *path)
{
    if (!path || path[0] == '\0')
    {
        return -1;
    }

    char tmp[_FILE_NAME_MAX_LENGTH];
    char *p = NULL;
    size_t len;

    // 复制路径到临时缓冲区
    strncpy(tmp, path, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';

    len = strlen(tmp);

    // 去除末尾的路径分隔符
#if EOS_FS_TYPE == EOS_FS_FATFS
    // FatFS 使用反斜杠
    if (len > 0 && (tmp[len - 1] == '\\' || tmp[len - 1] == '/'))
    {
        tmp[len - 1] = '\0';
        len--;
    }
#else
    // 其他文件系统使用斜杠
    if (len > 0 && tmp[len - 1] == '/')
    {
        tmp[len - 1] = '\0';
        len--;
    }
#endif

    // 检查路径长度
    if (len >= sizeof(tmp) - 1)
    {
        return -1; // 路径过长
    }

    // 跳过根目录（如果是绝对路径）
    p = tmp;
#if EOS_FS_TYPE == EOS_FS_FATFS
    // FatFS 可能包含盘符，如 "C:"
    if (len >= 2 && tmp[1] == ':')
    {
        p = tmp + 2; // 跳过盘符
        if (*p == '\\' || *p == '/')
        {
            p++; // 跳过路径分隔符
        }
    }
#else
    // POSIX 和其他系统
    if (tmp[0] == '/')
    {
        p = tmp + 1; // 跳过根目录
    }
#endif

    // 逐级创建目录
    for (; *p; p++)
    {
#if EOS_FS_TYPE == EOS_FS_FATFS
        if (*p == '\\' || *p == '/')
        {
#else
        if (*p == '/')
        {
#endif
            *p = '\0'; // 临时截断路径

            // 检查目录是否已存在
            int type = eos_fs_type(tmp);
            if (type == EOS_FS_TYPE_NOT_EXIST)
            {
                // 目录不存在，尝试创建
                if (eos_fs_mkdir(tmp) != 0)
                {
                    return -1; // 创建失败
                }
            }
            else if (type != EOS_FS_TYPE_DIR)
            {
                return -1; // 路径已存在但不是目录
            }

            // 恢复路径分隔符
#if EOS_FS_TYPE == EOS_FS_FATFS
            *p = '\\';
#else
            *p = '/';
#endif
        }
    }

    // 创建最终目录
    int type = eos_fs_type(tmp);
    if (type == EOS_FS_TYPE_NOT_EXIST)
    {
        if (eos_fs_mkdir(tmp) != 0)
        {
            return -1;
        }
    }
    else if (type != EOS_FS_TYPE_DIR)
    {
        return -1; // 最终路径已存在但不是目录
    }

    return 0;
}

int eos_fs_write_file(const char *path, const void *data, size_t data_size)
{
    eos_file_t *fp = eos_fs_open_write(path);
    EOS_CHECK_PTR_RETURN_VAL(fp, -1);

    ssize_t written = eos_fs_write(fp, data, data_size);

    eos_fs_close(fp);

    return written;
}

char *eos_fs_read_file(const char *path)
{
    eos_file_t *fp = eos_fs_open_read(path);
    // 获取文件大小
    uint32_t file_size = 0;
    eos_fs_size(fp, &file_size);

    if (file_size <= 0)
    {
        EOS_LOG_E("Invalid file size");
        eos_fs_close(fp);
        return NULL;
    }

    // 分配内存
    char *buf = eos_malloc(file_size + 1);
    if (!buf)
    {
        EOS_LOG_E("Failed to allocate memory for file");
        eos_fs_close(fp);
        return NULL;
    }

    ssize_t bytes_read = eos_fs_read(fp, buf, file_size);

    if (bytes_read != file_size)
    {
        EOS_LOG_E("Failed to read complete file (read %zd of %ld bytes)", bytes_read, file_size);
        eos_fs_close(fp);
        return NULL;
    }
    eos_fs_close(fp);
    buf[file_size] = '\0';
    return buf;
}

int eos_fs_rm_recursive(const char *path)
{
    if (!path || path[0] == '\0')
    {
        return -1;
    }

    // 安全检查：防止删除根目录
    if (strcmp(path, "/") == 0 || strcmp(path, "\\") == 0)
    {
        return -1;
    }

    // 检查路径类型
    int type = eos_fs_type(path);

    switch (type)
    {
    case EOS_FS_TYPE_NOT_EXIST:
        // 路径不存在，直接返回成功
        return 0;

    case EOS_FS_TYPE_FILE:
        // 如果是文件，直接删除
        return eos_fs_remove(path);

    case EOS_FS_TYPE_DIR:
    {
        // 如果是目录，需要递归删除其中的内容
        eos_dir_t *dir = eos_fs_opendir(path);
        if (!dir)
        {
            return -1; // 打开目录失败
        }

        char filename[_FILE_NAME_MAX_LENGTH];
        char fullpath[PATH_MAX];
        int result = 0;

        // 遍历目录中的所有条目
        while (eos_fs_readdir(dir, filename, sizeof(filename)) == 0)
        {
            // 跳过当前目录和父目录
            if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0)
            {
                continue;
            }

            // 构建完整路径
#if EOS_FS_TYPE == EOS_FS_FATFS
            // FatFS使用反斜杠
            snprintf(fullpath, sizeof(fullpath), "%s\\%s", path, filename);
#else
            // 其他文件系统使用斜杠
            snprintf(fullpath, sizeof(fullpath), "%s/%s", path, filename);
#endif

            // 递归删除子项
            if (eos_fs_rm_recursive(fullpath) != 0)
            {
                result = -1;
                break;
            }
        }

        eos_fs_closedir(dir);

        if (result != 0)
        {
            return -1; // 子项删除失败
        }

        // 删除空目录
        return eos_fs_rmdir(path);
    }

    default:
        // 类型获取失败或其他错误
        return -1;
    }
}
