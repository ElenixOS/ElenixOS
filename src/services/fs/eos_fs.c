/**
 * @file eos_fs.c
 * @brief File system utility functions
 */

#include "eos_fs.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eos_config.h"
#include "eos_log.h"
#include "eos_mem.h"
#include "eos_dfw.h"
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

int eos_fs_puts(const char *s, eos_file_t fp)
{
    if (fp == EOS_FILE_INVALID || !s)
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
        eos_file_t fp = eos_fs_open_write(path);
        if (fp == EOS_FILE_INVALID)
            return -1;

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

    // Copy path to temporary buffer
    strncpy(tmp, path, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';

    len = strlen(tmp);

    // Remove trailing path separator
#if EOS_FS_TYPE == EOS_FS_FATFS
    // FatFS uses backslash
    if (len > 0 && (tmp[len - 1] == '\\' || tmp[len - 1] == '/'))
    {
        tmp[len - 1] = '\0';
        len--;
    }
#else
    // Other file systems use forward slash
    if (len > 0 && tmp[len - 1] == '/')
    {
        tmp[len - 1] = '\0';
        len--;
    }
#endif

    // Check path length
    if (len >= sizeof(tmp) - 1)
    {
        return -1; // Path too long
    }

    // Skip root directory (for absolute paths)
    p = tmp;
#if EOS_FS_TYPE == EOS_FS_FATFS
    // FatFS may contain drive letter, e.g., "C:"
    if (len >= 2 && tmp[1] == ':')
    {
        p = tmp + 2; // Skip drive letter
        if (*p == '\\' || *p == '/')
        {
            p++; // Skip path separator
        }
    }
#else
    // POSIX and other systems
    if (tmp[0] == '/')
    {
        p = tmp + 1; // Skip root directory
    }
#endif

    // Create directories level by level
    for (; *p; p++)
    {
#if EOS_FS_TYPE == EOS_FS_FATFS
        if (*p == '\\' || *p == '/')
        {
#else
        if (*p == '/')
        {
#endif
            *p = '\0'; // Temporarily truncate path

            // Check if directory already exists
            int type = eos_fs_type(tmp);
            if (type == EOS_FS_TYPE_NOT_EXIST)
            {
                // Directory does not exist, try to create
                if (eos_fs_mkdir(tmp) != 0)
                {
                    return -1; // Creation failed
                }
            }
            else if (type != EOS_FS_TYPE_DIR)
            {
                return -1; // Path exists but is not a directory
            }

            // Restore path separator
#if EOS_FS_TYPE == EOS_FS_FATFS
            *p = '\\';
#else
            *p = '/';
#endif
        }
    }

    // Create final directory
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
        return -1; // Final path exists but is not a directory
    }

    return 0;
}

int eos_fs_write_file_immediate(const char *path, const void *data, size_t data_size)
{
    eos_file_t fp = eos_fs_open_write(path);
    if (fp == EOS_FILE_INVALID)
        return -1;

    ssize_t written = eos_fs_write(fp, data, data_size);

    eos_fs_close(fp);

    return written;
}

char *eos_fs_read_file_immediate(const char *path)
{
    eos_file_t fp = eos_fs_open_read(path);
    if (fp == EOS_FILE_INVALID)
    {
        EOS_LOG_E("Failed to open file");
        return NULL;
    }

    // Get file size
    uint32_t file_size = 0;
    eos_fs_size(fp, &file_size);

    if (file_size <= 0)
    {
        EOS_LOG_E("Invalid file size");
        eos_fs_close(fp);
        return NULL;
    }

    // Allocate memory
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

int eos_fs_write_file(const char *path, const void *data, size_t data_size)
{
#if EOS_DFW_ENABLE
    if (!path || !data || data_size == 0)
    {
        return -1;
    }
    return eos_dfw_write(path, (const uint8_t *)data, data_size) ? (int)data_size : -1;
#else
    return eos_fs_write_file_immediate(path, data, data_size);
#endif
}

char *eos_fs_read_file(const char *path)
{
#if EOS_DFW_ENABLE
    return (char *)eos_dfw_read(path);
#else
    return eos_fs_read_file_immediate(path);
#endif
}

int eos_fs_rm_recursive(const char *path)
{
    if (!path || path[0] == '\0')
    {
        return -1;
    }

    // Safety check: prevent deleting root directory
    if (strcmp(path, "/") == 0 || strcmp(path, "\\") == 0)
    {
        return -1;
    }

    // Check path type
    int type = eos_fs_type(path);

    switch (type)
    {
    case EOS_FS_TYPE_NOT_EXIST:
        // Path does not exist, return success directly
        return 0;

    case EOS_FS_TYPE_FILE:
        // If it's a file, delete it directly
        return eos_fs_remove(path);

    case EOS_FS_TYPE_DIR:
    {
        // If it's a directory, need to recursively delete its contents
        eos_dir_t dir = eos_fs_opendir(path);
        if (!dir)
        {
            return -1; // Failed to open directory
        }

        char filename[_FILE_NAME_MAX_LENGTH];
        char fullpath[PATH_MAX];
        int result = 0;

        // Traverse all entries in the directory
        while (eos_fs_readdir(dir, filename, sizeof(filename)) == 0)
        {
            // Skip current and parent directories
            if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0)
            {
                continue;
            }

            // Build full path
#if EOS_FS_TYPE == EOS_FS_FATFS
            // FatFS uses backslash
            snprintf(fullpath, sizeof(fullpath), "%s\\%s", path, filename);
#else
            // Other file systems use forward slash
            snprintf(fullpath, sizeof(fullpath), "%s/%s", path, filename);
#endif

            // Recursively delete sub-items
            if (eos_fs_rm_recursive(fullpath) != 0)
            {
                result = -1;
                break;
            }
        }

        eos_fs_closedir(dir);

        if (result != 0)
        {
            return -1; // Sub-item deletion failed
        }

        // Delete empty directory
        return eos_fs_rmdir(path);
    }

    default:
        // Type acquisition failed or other errors
        return -1;
    }
}
