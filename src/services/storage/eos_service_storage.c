/**
 * @file eos_service_storage.c
 * @brief Storage service file operation utilities
 */

#include "eos_service_storage.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eos_config.h"
#include "eos_log.h"
#include "eos_mem.h"
#include "eos_dfw.h"
#include "cJSON.h"
#include "eos_misc.h"
/* Macros and Definitions -------------------------------------*/
#define _FILE_NAME_MAX_LENGTH 256
/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

/************************** JSON Storage API **************************/

cJSON *eos_storage_json_load(const char *path)
{
    EOS_CHECK_PTR_RETURN_VAL(path, NULL);

    if (!eos_storage_is_file(path)) {
        return NULL;
    }

    char *content = eos_storage_read_file(path);
    if (!content) {
        return NULL;
    }

    cJSON *root = cJSON_Parse(content);
    eos_free(content);

    return root;
}

eos_result_t eos_storage_json_save(const char *path, cJSON *root)
{
    EOS_CHECK_PTR_RETURN_VAL(path && root, EOS_ERR_INVALID_ARG);

    char *json_str = cJSON_PrintUnformatted(root);
    if (!json_str) {
        return EOS_ERR_JSON_ERROR;
    }

    eos_result_t ret = eos_storage_write_file(path, json_str, strlen(json_str));
    cJSON_free(json_str);

    return ret;
}

bool eos_storage_json_get_bool(const char *path, const char *key, bool default_value)
{
    EOS_CHECK_PTR_RETURN_VAL(path && key, default_value);

    cJSON *root = eos_storage_json_load(path);
    if (!root) {
        return default_value;
    }

    cJSON *item = cJSON_GetObjectItem(root, key);
    bool result = (item && cJSON_IsBool(item)) ? cJSON_IsTrue(item) : default_value;

    cJSON_Delete(root);
    return result;
}

eos_result_t eos_storage_json_set_bool(const char *path, const char *key, bool value)
{
    EOS_CHECK_PTR_RETURN_VAL(path && key, EOS_ERR_INVALID_ARG);

    cJSON *root = eos_storage_json_load(path);
    if (!root) {
        root = cJSON_CreateObject();
        if (!root) {
            return EOS_ERR_MEM;
        }
    }

    cJSON *item = cJSON_GetObjectItem(root, key);
    if (item) {
        cJSON_SetBoolValue(item, value);
    } else {
        cJSON_AddBoolToObject(root, key, value);
    }

    eos_result_t ret = eos_storage_json_save(path, root);
    cJSON_Delete(root);

    return ret;
}

char *eos_storage_json_get_string(const char *path, const char *key, const char *default_value)
{
    EOS_CHECK_PTR_RETURN_VAL(path && key, eos_strdup(default_value));

    cJSON *root = eos_storage_json_load(path);
    if (!root) {
        return eos_strdup(default_value);
    }

    cJSON *item = cJSON_GetObjectItem(root, key);
    char *result = (item && cJSON_IsString(item)) ? eos_strdup(item->valuestring) : eos_strdup(default_value);

    cJSON_Delete(root);
    return result;
}

eos_result_t eos_storage_json_set_string(const char *path, const char *key, const char *value)
{
    EOS_CHECK_PTR_RETURN_VAL(path && key && value, EOS_ERR_INVALID_ARG);

    cJSON *root = eos_storage_json_load(path);
    if (!root) {
        root = cJSON_CreateObject();
        if (!root) {
            return EOS_ERR_MEM;
        }
    }

    cJSON *item = cJSON_GetObjectItem(root, key);
    if (item) {
        cJSON_SetValuestring(item, value);
    } else {
        cJSON_AddStringToObject(root, key, value);
    }

    eos_result_t ret = eos_storage_json_save(path, root);
    cJSON_Delete(root);

    return ret;
}

double eos_storage_json_get_number(const char *path, const char *key, double default_value)
{
    EOS_CHECK_PTR_RETURN_VAL(path && key, default_value);

    cJSON *root = eos_storage_json_load(path);
    if (!root) {
        return default_value;
    }

    cJSON *item = cJSON_GetObjectItem(root, key);
    double result = (item && cJSON_IsNumber(item)) ? item->valuedouble : default_value;

    cJSON_Delete(root);
    return result;
}

eos_result_t eos_storage_json_set_number(const char *path, const char *key, double value)
{
    EOS_CHECK_PTR_RETURN_VAL(path && key, EOS_ERR_INVALID_ARG);

    cJSON *root = eos_storage_json_load(path);
    if (!root) {
        root = cJSON_CreateObject();
        if (!root) {
            return EOS_ERR_MEM;
        }
    }

    cJSON *item = cJSON_GetObjectItem(root, key);
    if (item) {
        cJSON_SetNumberValue(item, value);
    } else {
        cJSON_AddNumberToObject(root, key, value);
    }

    eos_result_t ret = eos_storage_json_save(path, root);
    cJSON_Delete(root);

    return ret;
}

cJSON *eos_storage_json_get_json(const char *path, const char *key)
{
    EOS_CHECK_PTR_RETURN_VAL(path && key, NULL);

    cJSON *root = eos_storage_json_load(path);
    if (!root) {
        return NULL;
    }

    cJSON *item = cJSON_GetObjectItem(root, key);
    if (!item || (!cJSON_IsObject(item) && !cJSON_IsArray(item))) {
        cJSON_Delete(root);
        return NULL;
    }

    cJSON_DetachItemViaPointer(root, item);
    cJSON_Delete(root);

    return item;
}

eos_result_t eos_storage_json_set_json(const char *path, const char *key, cJSON *json_value)
{
    EOS_CHECK_PTR_RETURN_VAL(path && key && json_value, EOS_ERR_INVALID_ARG);

    cJSON *root = eos_storage_json_load(path);
    if (!root) {
        root = cJSON_CreateObject();
        if (!root) {
            return EOS_ERR_MEM;
        }
    }

    cJSON *item = cJSON_GetObjectItem(root, key);
    if (item) {
        cJSON_ReplaceItemInObject(root, key, json_value);
    } else {
        cJSON_AddItemToObject(root, key, json_value);
    }

    eos_result_t ret = eos_storage_json_save(path, root);
    cJSON_Delete(root);

    return ret;
}

eos_result_t eos_storage_json_create_if_not_exist(const char *path, const char *default_json)
{
    EOS_CHECK_PTR_RETURN_VAL(path, EOS_ERR_INVALID_ARG);

    if (eos_storage_is_file(path)) {
        return EOS_OK;
    }

    const char *content = default_json ? default_json : "{}";
    return eos_storage_create_file_if_not_exist(path, content);
}

bool eos_storage_is_dir(const char *path)
{
    return (eos_fs_type(path) == EOS_FS_TYPE_DIR) ? true : false;
}

bool eos_storage_is_file(const char *path)
{
    return (eos_fs_type(path) == EOS_FS_TYPE_FILE) ? true : false;
}

eos_result_t eos_storage_puts(const char *s, eos_file_t fp)
{
    if (fp == EOS_FILE_INVALID || !s)
        return EOS_ERR_INVALID_ARG;

    ssize_t written = eos_fs_write(fp, s, strlen(s));
    return (written < 0) ? EOS_ERR_IO : EOS_OK;
}

eos_result_t eos_storage_mkdir_if_not_exist(const char *path)
{
    EOS_CHECK_PTR_RETURN_VAL(path, EOS_ERR_INVALID_ARG);

    int type = eos_fs_type(path);
    if (type == EOS_FS_TYPE_DIR)
    {
        return EOS_OK;
    }

    if (type == EOS_FS_TYPE_FILE)
    {
        return EOS_ERR_ALREADY_EXISTS;
    }

    if (type == EOS_FS_TYPE_NOT_EXIST)
    {
        return (eos_fs_mkdir(path) == 0) ? EOS_OK : EOS_ERR_FILE_ERROR;
    }

    return EOS_ERR_FILE_ERROR;
}

eos_result_t eos_storage_create_file_if_not_exist(const char *path, const char *default_content)
{
    EOS_CHECK_PTR_RETURN_VAL(path, EOS_ERR_INVALID_ARG);

    int type = eos_fs_type(path);
    if (type == EOS_FS_TYPE_FILE)
    {
        return EOS_OK;
    }

    if (type == EOS_FS_TYPE_DIR)
    {
        return EOS_ERR_ALREADY_EXISTS;
    }

    if (type == EOS_FS_TYPE_NOT_EXIST)
    {
        eos_file_t fp = eos_fs_open_write(path);
        if (fp == EOS_FILE_INVALID)
            return EOS_ERR_FILE_ERROR;

        if (default_content)
        {
            ssize_t len = strlen(default_content);
            ssize_t written = eos_fs_write(fp, default_content, len);
            if (written != len)
            {
                EOS_LOG_E("write %s failed, written=%zd", path, written);
                eos_fs_close(fp);
                return EOS_ERR_IO;
            }
        }

        eos_fs_close(fp);
        EOS_LOG_I("Created file: %s", path);
        return EOS_OK;
    }

    return EOS_ERR_FILE_ERROR;
}

eos_result_t eos_storage_mkdir_recursive(const char *path)
{
    if (!path || path[0] == '\0')
    {
        return EOS_ERR_INVALID_ARG;
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
        return EOS_ERR_PATH_TOO_LONG;
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
                    return EOS_ERR_FILE_ERROR;
                }
            }
            else if (type != EOS_FS_TYPE_DIR)
            {
                return EOS_ERR_INVALID_STATE;
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
            return EOS_ERR_FILE_ERROR;
        }
    }
    else if (type != EOS_FS_TYPE_DIR)
    {
        return EOS_ERR_INVALID_STATE;
    }

    return EOS_OK;
}

eos_result_t eos_storage_write_file_immediate(const char *path, const void *data, size_t data_size)
{
    EOS_CHECK_PTR_RETURN_VAL(path && data, EOS_ERR_INVALID_ARG);
    if (data_size == 0)
    {
        return EOS_ERR_INVALID_ARG;
    }

    eos_file_t fp = eos_fs_open_write(path);
    if (fp == EOS_FILE_INVALID)
        return EOS_ERR_FILE_ERROR;

    ssize_t written = eos_fs_write(fp, data, data_size);

    eos_fs_close(fp);

    return (written == data_size) ? EOS_OK : EOS_ERR_IO;
}

char *eos_storage_read_file_immediate(const char *path)
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

eos_result_t eos_storage_write_file(const char *path, const void *data, size_t data_size)
{
#if EOS_DFW_ENABLE
    if (!path || !data || data_size == 0)
    {
        return EOS_ERR_INVALID_ARG;
    }
    return eos_dfw_write(path, (const uint8_t *)data, data_size) ? EOS_OK : EOS_ERR_FILE_ERROR;
#else
    return eos_storage_write_file_immediate(path, data, data_size);
#endif
}

char *eos_storage_read_file(const char *path)
{
#if EOS_DFW_ENABLE
    return (char *)eos_dfw_read(path);
#else
    return eos_storage_read_file_immediate(path);
#endif
}

eos_result_t eos_storage_rm_recursive(const char *path)
{
    if (!path || path[0] == '\0')
    {
        return EOS_ERR_INVALID_ARG;
    }

    // Safety check: prevent deleting root directory
    if (strcmp(path, "/") == 0 || strcmp(path, "\\") == 0)
    {
        return EOS_ERR_INVALID_ARG;
    }

    // Check path type
    int type = eos_fs_type(path);

    switch (type)
    {
    case EOS_FS_TYPE_NOT_EXIST:
        // Path does not exist, return success directly
        return EOS_OK;

    case EOS_FS_TYPE_FILE:
        // If it's a file, delete it directly
        return (eos_fs_remove(path) == 0) ? EOS_OK : EOS_ERR_FILE_ERROR;

    case EOS_FS_TYPE_DIR:
    {
        // If it's a directory, need to recursively delete its contents
        eos_dir_t dir = eos_fs_opendir(path);
        if (!dir)
        {
            return EOS_ERR_FILE_ERROR;
        }

        char filename[_FILE_NAME_MAX_LENGTH];
        char fullpath[PATH_MAX];
        eos_result_t result = EOS_OK;

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
            if (eos_storage_rm_recursive(fullpath) != EOS_OK)
            {
                result = EOS_ERR_FILE_ERROR;
                break;
            }
        }

        eos_fs_closedir(dir);

        if (result != EOS_OK)
        {
            return result;
        }

        // Delete empty directory
        return (eos_fs_rmdir(path) == 0) ? EOS_OK : EOS_ERR_FILE_ERROR;
    }

    default:
        // Type acquisition failed or other errors
        return EOS_ERR_FILE_ERROR;
    }
}
