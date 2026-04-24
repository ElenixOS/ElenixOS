/**
 * @file elena_os_fs_port.c
 * @brief File system porting
 * @author Sab1e
 * @date 2025-11-18
 */

#include "elena_os_config.h"

#if EOS_FS_TYPE == EOS_FS_POSIX

#include "elena_os_fs_port.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include "elena_os_port.h"
#include "elena_os_log.h"
/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

/* ---------------- POSIX FS implementation ------------------ */

/* Open file read-only */
eos_file_t eos_fs_open_read(const char *path)
{
    if (!path)
        return NULL;
    return fopen(path, "rb");
}

/* Open file write-only (create if not exist, overwrite if exist) */
eos_file_t eos_fs_open_write(const char *path)
{
    if (!path)
        return NULL;
    return fopen(path, "wb");
}

/* Read file data */
int eos_fs_read(eos_file_t fp, void *buf, size_t len)
{
    if (!fp || !buf)
        return -1;
    size_t n = fread(buf, 1, len, (FILE *)fp);
    if (n == 0 && ferror((FILE *)fp))
        return -1;
    return (int)n;
}

/* Write file data */
int eos_fs_write(eos_file_t fp, const void *buf, size_t len)
{
    if (!fp || !buf)
        return -1;
    size_t n = fwrite(buf, 1, len, (FILE *)fp);
    if (n < len)
        return -1;
    return (int)n;
}

/* File positioning */
int eos_fs_seek(eos_file_t fp, uint32_t pos)
{
    if (!fp)
        return -1;
    return fseek((FILE *)fp, (long)pos, SEEK_SET);
}

/* Get file size */
int eos_fs_size(eos_file_t fp, uint32_t *size)
{
    if (!fp || !size)
        return -1;
    long cur = ftell((FILE *)fp);
    if (cur < 0)
        return -1;
    if (fseek((FILE *)fp, 0, SEEK_END) != 0)
        return -1;
    long end = ftell((FILE *)fp);
    if (end < 0)
        return -1;
    *size = (uint32_t)end;
    fseek((FILE *)fp, cur, SEEK_SET);
    return 0;
}

/* Close file */
void eos_fs_close(eos_file_t fp)
{
    if (fp)
        fclose((FILE *)fp);
}

/* Create directory (single level directory) */
int eos_fs_mkdir(const char *path)
{
    if (!path)
        return -1;
#ifdef _WIN32
    return mkdir(path) == 0 ? 0 : -1;
#else
    return mkdir(path, 0755) == 0 ? 0 : -1;
#endif
}

/* Remove empty directory */
int eos_fs_rmdir(const char *path)
{
    if (!path)
        return -1;
    return rmdir(path) == 0 ? 0 : -1;
}

/* Remove file */
int eos_fs_remove(const char *path)
{
    if (!path)
        return -1;
    return remove(path) == 0 ? 0 : -1;
}

/* Check if file or directory exists */
int eos_fs_exists(const char *path)
{
    if (!path)
        return 0;
    struct stat st;
    return stat(path, &st) == 0 ? 1 : 0;
}

int eos_fs_type(const char *path)
{
    struct stat st;
    if (stat(path, &st) != 0)
        return EOS_FS_TYPE_NOT_EXIST;
    if (S_ISDIR(st.st_mode))
        return EOS_FS_TYPE_DIR;
    return EOS_FS_TYPE_FILE;
}

eos_dir_t eos_fs_opendir(const char *path)
{
    return opendir(path);
}

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

void eos_fs_closedir(eos_dir_t dir)
{
    if (dir)
    {
        closedir(dir);
    }
}

int eos_fs_mv(const char *old_path, const char *new_path)
{
    if (rename(old_path, new_path) != 0)
    {
        perror("rename failed");
        return -1;
    }
    return 0;
}

int eos_fs_sync(eos_file_t fp)
{
    return fflush(fp);
}

#endif /* EOS_FS_TYPE */
