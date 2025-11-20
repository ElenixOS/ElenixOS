/**
 * @file elena_os_fs_port.c
 * @brief 文件系统移植
 * @author Sab1e
 * @date 2025-11-18
 */

#include "elena_os_fs_port.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

/* ---------------- POSIX FS 实现 ------------------ */

/* 打开文件只读 */
eos_file_t *eos_fs_open_read(const char *path)
{
    if(!path) return NULL;
    return fopen(path, "rb");
}

/* 打开文件只写（如果不存在则创建，存在则覆盖） */
eos_file_t *eos_fs_open_write(const char *path)
{
    if(!path) return NULL;
    return fopen(path, "wb");
}

/* 读取文件数据 */
int eos_fs_read(eos_file_t *fp, void *buf, size_t len)
{
    if(!fp || !buf) return -1;
    size_t n = fread(buf, 1, len, (FILE *)fp);
    if(n == 0 && ferror((FILE *)fp)) return -1;
    return (int)n;
}

/* 写入文件数据 */
int eos_fs_write(eos_file_t *fp, const void *buf, size_t len)
{
    if(!fp || !buf) return -1;
    size_t n = fwrite(buf, 1, len, (FILE *)fp);
    if(n < len) return -1;
    return (int)n;
}

/* 文件定位 */
int eos_fs_seek(eos_file_t *fp, uint32_t pos)
{
    if(!fp) return -1;
    return fseek((FILE *)fp, (long)pos, SEEK_SET);
}

/* 获取文件大小 */
int eos_fs_size(eos_file_t *fp, uint32_t *size)
{
    if(!fp || !size) return -1;
    long cur = ftell((FILE *)fp);
    if(cur < 0) return -1;
    if(fseek((FILE *)fp, 0, SEEK_END) != 0) return -1;
    long end = ftell((FILE *)fp);
    if(end < 0) return -1;
    *size = (uint32_t)end;
    fseek((FILE *)fp, cur, SEEK_SET);
    return 0;
}

/* 关闭文件 */
void eos_fs_close(eos_file_t *fp)
{
    if(fp) fclose((FILE *)fp);
}

/* 创建目录（单级目录） */
int eos_fs_mkdir(const char *path)
{
    if(!path) return -1;
    /* 0755 权限 */
    return mkdir(path, 0755) == 0 ? 0 : -1;
}

/* 删除空目录 */
int eos_fs_rmdir(const char *path)
{
    if(!path) return -1;
    return rmdir(path) == 0 ? 0 : -1;
}

/* 删除文件 */
int eos_fs_remove(const char *path)
{
    if(!path) return -1;
    return remove(path) == 0 ? 0 : -1;
}

/* 检查文件或目录是否存在 */
int eos_fs_exists(const char *path)
{
    if(!path) return 0;
    struct stat st;
    return stat(path, &st) == 0 ? 1 : 0;
}

int eos_fs_type(const char *path)
{
    struct stat st;
    if(stat(path, &st) != 0) return EOS_FS_TYPE_NOT_EXIST;
    if(S_ISDIR(st.st_mode)) return EOS_FS_TYPE_DIR;
    return EOS_FS_TYPE_FILE;
}

eos_dir_t *eos_fs_opendir(const char *path) {
    return opendir(path);
}

int eos_fs_readdir(eos_dir_t *dir, char *name, size_t max_len) {
    if (!dir) return -1;

    struct dirent *entry = readdir(dir);
    if (!entry) return -1;

    strncpy(name, entry->d_name, max_len - 1);
    name[max_len - 1] = '\0';
    return 0;
}

void eos_fs_closedir(eos_dir_t *dir) {
    if (dir) {
        closedir(dir);
    }
}

int eos_fs_mv(const char *old_path, const char *new_path)
{
    if (rename(old_path, new_path) != 0) {
        perror("rename failed");
        return -1;
    }
    return 0;
}
