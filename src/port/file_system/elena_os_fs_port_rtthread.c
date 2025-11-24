/**
 * @file elena_os_fs_port_rtthread.c
 * @brief 模块简要说明
 * @author Sab1e
 * @date 2025-11-22
 */

#include "elena_os_config.h"

#if EOS_FS_TYPE == EOS_FS_RTTHREAD

#include "elena_os_fs_port.h"

/* Includes ---------------------------------------------------*/
#include <string.h>
#include "elena_os_log.h"

/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

/* 打开文件只读 */
eos_file_t eos_fs_open_read(const char *path)
{
    if (!path)
        return -1;
    return open(path, O_RDONLY, 0);
}

/* 打开文件只写（覆盖或创建） */
eos_file_t eos_fs_open_write(const char *path)
{
    if (!path)
        return -1;
    return open(path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
}

/* 读取文件数据 */
int eos_fs_read(eos_file_t fp, void *buf, size_t len)
{
    if (fp < 0 || !buf)
        return -1;
    int n = read(fp, buf, len);
    return n >= 0 ? n : -1;
}

/* 写入文件数据 */
int eos_fs_write(eos_file_t fp, const void *buf, size_t len)
{
    if (fp < 0 || !buf)
        return -1;
    int n = write(fp, buf, len);
    return n == (int)len ? n : -1;
}

/* 文件定位 */
int eos_fs_seek(eos_file_t fp, uint32_t pos)
{
    if (fp < 0)
        return -1;
    int r = lseek(fp, pos, SEEK_SET);
    return r >= 0 ? 0 : -1;
}

/* 获取文件大小 */
int eos_fs_size(eos_file_t fp, uint32_t *size)
{
    if (fp < 0 || !size)
        return -1;
    struct stat st;
    if (fstat(fp, &st) != 0)
        return -1;
    *size = st.st_size;
    return 0;
}

/* 关闭文件 */
void eos_fs_close(eos_file_t fp)
{
    if (fp >= 0)
        close(fp);
}

/* 创建目录（单级目录） */
int eos_fs_mkdir(const char *path)
{
    if (!path)
        return -1;
    return mkdir(path, 0755) == 0 ? 0 : -1;
}

/* 删除空目录 */
int eos_fs_rmdir(const char *path)
{
    if (!path)
        return -1;
    return rmdir(path) == 0 ? 0 : -1;
}

/* 删除文件 */
int eos_fs_remove(const char *path)
{
    if (!path)
        return -1;
    return unlink(path) == 0 ? 0 : -1;
}

/* 检查文件或目录是否存在 */
int eos_fs_exists(const char *path)
{
    if (!path)
        return 0;
    struct stat st;
    return stat(path, &st) == 0 ? 1 : 0;
}

/* 判断文件类型 */
int eos_fs_type(const char *path)
{
    struct stat st;
    if (stat(path, &st) != 0)
        return EOS_FS_TYPE_NOT_EXIST;
    if (S_ISDIR(st.st_mode))
        return EOS_FS_TYPE_DIR;
    return EOS_FS_TYPE_FILE;
}

/* 打开目录 */
eos_dir_t eos_fs_opendir(const char *path)
{
    return opendir(path);
}

/* 读取目录 */
int eos_fs_readdir(eos_dir_t dir, char *name, size_t max_len)
{
    if (!dir)
        return -1;
    struct dirent *entry = readdir(dir);
    if (!entry)
        return -1;
    strncpy(name, entry->d_name, max_len - 1);
    name[max_len - 1] = '\0';
    return 0;
}

/* 关闭目录 */
void eos_fs_closedir(eos_dir_t dir)
{
    if (dir)
        closedir(dir);
}

/* 重命名文件/目录 */
int eos_fs_mv(const char *old_path, const char *new_path)
{
    return rename(old_path, new_path) == 0 ? 0 : -1;
}

/* 文件同步 */
int eos_fs_sync(eos_file_t fp)
{
    if (fp < 0)
        return -1;
    return fsync(fp);
}

#endif /* EOS_FS_TYPE */
