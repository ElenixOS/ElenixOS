/**
 * @file eos_pkg_mgr.c
 * @brief Package manager
 */

#include "eos_pkg_mgr.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eos_port.h"
#define EOS_LOG_TAG "PackageManager"
#include "eos_log.h"
#include "eos_service_storage.h"
#include "eos_mem.h"
/* Macros and Definitions -------------------------------------*/
/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

static bool _is_epk_path(const char *pkg_path)
{
    size_t len;

    if (!pkg_path)
    {
        return false;
    }

    len = strlen(pkg_path);
    return len >= 4U && strcmp(pkg_path + len - 4U, ".epk") == 0;
}

struct eos_pkg
{
    eos_file_t file;
    eos_pkg_header_t header;
    script_pkg_type_t type;
    uint32_t file_size;
};

void eos_pkg_free(script_pkg_t *pkg)
{
    EOS_CHECK_PTR_RETURN(pkg);

    if (pkg->id)
        eos_free((void *)pkg->id);
    if (pkg->name)
        eos_free((void *)pkg->name);
    if (pkg->version)
        eos_free((void *)pkg->version);
    if (pkg->author)
        eos_free((void *)pkg->author);
    if (pkg->description)
        eos_free((void *)pkg->description);
    if (pkg->script_str)
        eos_free((void *)pkg->script_str);
    if (pkg->base_path)
        eos_free((void *)pkg->base_path);
    if (pkg->permissions)
    {
        for (uint8_t i = 0; i < pkg->permission_count; i++)
        {
            if (pkg->permissions[i])
                eos_free((void *)pkg->permissions[i]);
        }
        eos_free(pkg->permissions);
    }
    pkg->id = NULL;
    pkg->name = NULL;
    pkg->type = SCRIPT_TYPE_UNKNOWN;
    pkg->version = NULL;
    pkg->author = NULL;
    pkg->description = NULL;
    pkg->script_str = NULL;
    pkg->base_path = NULL;
    pkg->permissions = NULL;
    pkg->permission_count = 0;
    pkg->min_api_level = 0;
    pkg->target_api_level = 0;
}

eos_result_t eos_pkg_get_type(const eos_pkg_header_t *header, script_pkg_type_t *pkg_type)
{
    if (!header || !pkg_type)
    {
        return EOS_ERR_VAR_NULL;
    }

    if (memcmp(header->magic, EOS_PKG_APP_MAGIC, sizeof(header->magic)) == 0)
    {
        *pkg_type = SCRIPT_TYPE_APPLICATION;
        return EOS_OK;
    }

    if (memcmp(header->magic, EOS_PKG_WATCHFACE_MAGIC, sizeof(header->magic)) == 0)
    {
        *pkg_type = SCRIPT_TYPE_WATCHFACE;
        return EOS_OK;
    }

    *pkg_type = SCRIPT_TYPE_UNKNOWN;
    EOS_LOG_E("Unsupported package type in header");
    return EOS_ERR_FILE_ERROR;
}

static eos_result_t _read_header_from_file(eos_file_t fp, eos_pkg_header_t *header)
{
    if (fp == EOS_FILE_INVALID || !header)
    {
        return EOS_ERR_VAR_NULL;
    }

    memset(header, 0, sizeof(eos_pkg_header_t));

    // Read magic number
    if (eos_storage_file_seek(fp, EOS_PKG_MAGIC_OFFSET) != EOS_OK || eos_storage_file_read(fp, header->magic, 4) != 4)
    {
        EOS_LOG_E("Failed to read magic number");
        return EOS_ERR_FILE_ERROR;
    }

    // Read package name
    if (eos_storage_file_seek(fp, EOS_PKG_NAME_OFFSET) != EOS_OK
        || eos_storage_file_read(fp, header->pkg_name, EOS_PKG_NAME_LEN_MAX) != EOS_PKG_NAME_LEN_MAX)
    {
        EOS_LOG_E("Failed to read package name");
        return EOS_ERR_FILE_ERROR;
    }
    header->pkg_name[EOS_PKG_NAME_LEN_MAX - 1] = '\0';

    // Read package ID
    if (eos_storage_file_seek(fp, EOS_PKG_ID_OFFSET) != EOS_OK
        || eos_storage_file_read(fp, header->pkg_id, EOS_PKG_ID_LEN_MAX) != EOS_PKG_ID_LEN_MAX)
    {
        EOS_LOG_E("Failed to read package id");
        return EOS_ERR_FILE_ERROR;
    }
    header->pkg_id[EOS_PKG_ID_LEN_MAX - 1] = '\0';

    // Read package version
    if (eos_storage_file_seek(fp, EOS_PKG_VERSION_OFFSET) != EOS_OK
        || eos_storage_file_read(fp, header->pkg_version, EOS_PKG_VERSION_LEN_MAX) != EOS_PKG_VERSION_LEN_MAX)
    {
        EOS_LOG_E("Failed to read package version");
        return EOS_ERR_FILE_ERROR;
    }
    header->pkg_version[EOS_PKG_VERSION_LEN_MAX - 1] = '\0';

    // Read min_api_level
    if (eos_storage_file_seek(fp, EOS_PKG_MIN_API_OFFSET) != EOS_OK
        || eos_storage_file_read(fp, &header->min_api_level, sizeof(uint16_t)) != sizeof(uint16_t))
    {
        EOS_LOG_E("Failed to read min_api_level");
        return EOS_ERR_FILE_ERROR;
    }

    // Read target_api_level
    if (eos_storage_file_seek(fp, EOS_PKG_TARGET_API_OFFSET) != EOS_OK
        || eos_storage_file_read(fp, &header->target_api_level, sizeof(uint16_t)) != sizeof(uint16_t))
    {
        EOS_LOG_E("Failed to read target_api_level");
        return EOS_ERR_FILE_ERROR;
    }

    // Read file_count
    if (eos_storage_file_seek(fp, EOS_PKG_FILE_COUNT_OFFSET) != EOS_OK
        || eos_storage_file_read(fp, &header->file_count, sizeof(uint32_t)) != sizeof(uint32_t))
    {
        EOS_LOG_E("Failed to read file count");
        return EOS_ERR_FILE_ERROR;
    }

    // Read reserved field
    if (eos_storage_file_seek(fp, EOS_PKG_RESERVED_OFFSET) != EOS_OK
        || eos_storage_file_read(fp, &header->reserved, sizeof(uint32_t)) != sizeof(uint32_t))
    {
        EOS_LOG_E("Failed to read reserved field");
        return EOS_ERR_FILE_ERROR;
    }

    return EOS_OK;
}

eos_result_t eos_pkg_open(const char *pkg_path, eos_pkg_t **package)
{
    if (!pkg_path || !package)
    {
        EOS_LOG_E("Invalid package open parameters");
        return EOS_ERR_VAR_NULL;
    }

    *package = NULL;
    if (!_is_epk_path(pkg_path))
    {
        EOS_LOG_E("Unsupported package extension: %s", pkg_path);
        return EOS_ERR_FILE_ERROR;
    }

    if (!eos_storage_is_file(pkg_path))
    {
        EOS_LOG_E("Path is not a file: %s", pkg_path);
        return EOS_ERR_FILE_ERROR;
    }

    eos_pkg_t *opened_package = eos_malloc(sizeof(eos_pkg_t));
    if (!opened_package)
    {
        EOS_LOG_E("Failed to allocate package context");
        return EOS_ERR_MEM;
    }
    memset(opened_package, 0, sizeof(*opened_package));

    opened_package->file = eos_storage_file_open_read(pkg_path);
    if (opened_package->file == EOS_FILE_INVALID)
    {
        eos_free(opened_package);
        EOS_LOG_E("Failed to open package file: %s", pkg_path);
        return EOS_ERR_FILE_ERROR;
    }

    eos_result_t result = _read_header_from_file(opened_package->file, &opened_package->header);
    if (result != EOS_OK)
    {
        eos_pkg_close(opened_package);
        return result;
    }

    result = eos_pkg_get_type(&opened_package->header, &opened_package->type);
    if (result != EOS_OK)
    {
        eos_pkg_close(opened_package);
        return result;
    }

    if (eos_storage_file_size(opened_package->file, &opened_package->file_size) != EOS_OK
        || opened_package->file_size < EOS_PKG_TABLE_OFFSET)
    {
        eos_pkg_close(opened_package);
        EOS_LOG_E("Invalid EPK file size");
        return EOS_ERR_FILE_ERROR;
    }

    *package = opened_package;
    return EOS_OK;
}

void eos_pkg_close(eos_pkg_t *package)
{
    if (!package)
    {
        return;
    }

    if (package->file != EOS_FILE_INVALID)
    {
        eos_storage_file_close(package->file);
        package->file = EOS_FILE_INVALID;
    }
    eos_free(package);
}

const eos_pkg_header_t *eos_pkg_get_header(const eos_pkg_t *package)
{
    return package ? &package->header : NULL;
}

script_pkg_type_t eos_pkg_get_package_type(const eos_pkg_t *package)
{
    return package ? package->type : SCRIPT_TYPE_UNKNOWN;
}

eos_result_t eos_pkg_read_header(const char *pkg_path, eos_pkg_header_t *header)
{
    if (!pkg_path || !header)
    {
        EOS_LOG_E("Invalid parameters: pkg_path=%p, header=%p", pkg_path, header);
        return EOS_ERR_VAR_NULL;
    }

    eos_pkg_t *package = NULL;
    eos_result_t result = eos_pkg_open(pkg_path, &package);
    if (result != EOS_OK)
    {
        return result;
    }

    *header = *eos_pkg_get_header(package);
    eos_pkg_close(package);
    return EOS_OK;
}

eos_result_t eos_pkg_unpack(eos_pkg_t *package, const char *output_path)
{
    if (!package || !output_path)
    {
        return EOS_ERR_VAR_NULL;
    }

    eos_file_t fp = package->file;
    const eos_pkg_header_t *header = &package->header;
    if (fp == EOS_FILE_INVALID)
    {
        EOS_LOG_E("Invalid package context");
        return EOS_ERR_FILE_ERROR;
    }

    uint32_t file_size = package->file_size;

    // Seek to the file table position (immediately after the file header)
    if (eos_storage_file_seek(fp, EOS_PKG_TABLE_OFFSET) != EOS_OK)
    {
        EOS_LOG_E("Failed to seek to file table at offset %u", EOS_PKG_TABLE_OFFSET);
        return EOS_ERR_FILE_ERROR;
    }

    // Create output directory
    if (eos_storage_mkdir_recursive(output_path) != EOS_OK)
    {
        EOS_LOG_E("Failed to create output directory");
        return EOS_ERR_FILE_ERROR;
    }

    // Track the current file table offset instead of querying the underlying file position
    uint32_t table_pos = EOS_PKG_TABLE_OFFSET;

    // Process each file entry
    for (uint32_t i = 0; i < header->file_count; i++)
    {
        // Read the file name length
        uint32_t name_len;
        if (eos_storage_file_read(fp, &name_len, sizeof(uint32_t)) != sizeof(uint32_t))
        {
            EOS_LOG_E("Failed to read name length for entry %u", i);
            return EOS_ERR_FILE_ERROR;
        }
        table_pos += sizeof(uint32_t);

        // Validate the file name length
        if (name_len >= EOS_FS_PATH_MAX)
        {
            EOS_LOG_E("Name length %u too long for entry %u", name_len, i);
            return EOS_ERR_FILE_ERROR;
        }

        // Read the file name
        char name[EOS_FS_PATH_MAX];
        if (eos_storage_file_read(fp, name, name_len) != (int)name_len)
        {
            EOS_LOG_E("Failed to read name for entry %u", i);
            return EOS_ERR_FILE_ERROR;
        }
        name[name_len] = '\0';
        table_pos += name_len;

        // Read the remaining entry fields
        uint32_t is_dir, offset, size;
        if (eos_storage_file_read(fp, &is_dir, sizeof(uint32_t)) != sizeof(uint32_t)
            || eos_storage_file_read(fp, &offset, sizeof(uint32_t)) != sizeof(uint32_t)
            || eos_storage_file_read(fp, &size, sizeof(uint32_t)) != sizeof(uint32_t))
        {
            EOS_LOG_E("Failed to read entry fields for %s", name);
            return EOS_ERR_FILE_ERROR;
        }
        table_pos += sizeof(uint32_t) * 3;

        // Compute the offset of the next entry in the table
        uint32_t next_entry_pos = table_pos;

        // Build the full output path
        char full_path[EOS_FS_PATH_MAX] = {0};
        snprintf(full_path, sizeof(full_path), "%s/%s", output_path, name);

        if (is_dir)
        {
            // Create the directory
            if (eos_storage_mkdir_recursive(full_path) != EOS_OK)
            {
                EOS_LOG_E("Failed to create directory: %s", full_path);
                return EOS_ERR_FILE_ERROR;
            }
            EOS_LOG_D("Created directory: %s", full_path);
        }
        else
        {
            // Validate the file offset and size
            if (offset < EOS_PKG_TABLE_OFFSET || offset >= file_size)
            {
                EOS_LOG_E("Invalid file offset: %u for %s", offset, name);
                return EOS_ERR_FILE_ERROR;
            }

            if (size > file_size - offset)
            {
                EOS_LOG_E("File size overflow: %u+%u for %s", offset, size, name);
                return EOS_ERR_FILE_ERROR;
            }

            // Ensure the parent directory exists
            char *last_slash = strrchr(full_path, '/');
            if (last_slash)
            {
                *last_slash = '\0';
                if (eos_storage_mkdir_recursive(full_path) != EOS_OK)
                {
                    EOS_LOG_E("Failed to create parent directory: %s", full_path);
                    return EOS_ERR_FILE_ERROR;
                }
                *last_slash = '/';
            }

            // Create the file and write data
            eos_file_t out_fp = eos_storage_file_open_write(full_path);
            if (out_fp == EOS_FILE_INVALID)
            {
                EOS_LOG_E("Failed to create file: %s", full_path);
                return EOS_ERR_FILE_ERROR;
            }

            // Seek to the file data
            if (eos_storage_file_seek(fp, offset) != EOS_OK)
            {
                eos_storage_file_close(out_fp);
                EOS_LOG_E("Failed to seek to file data for %s", name);
                return EOS_ERR_FILE_ERROR;
            }

            // Read and write the file in chunks
            uint32_t remaining = size;
            uint8_t buffer[EOS_PKG_READ_BLOCK];
            while (remaining > 0)
            {
                size_t to_read = remaining > sizeof(buffer) ? sizeof(buffer) : remaining;
                int r = eos_storage_file_read(fp, buffer, to_read);
                if (r <= 0)
                {
                    eos_storage_file_close(out_fp);
                    EOS_LOG_E("Failed to read file data for %s", name);
                    return EOS_ERR_FILE_ERROR;
                }
                if (eos_storage_file_write(out_fp, buffer, r) != r)
                {
                    eos_storage_file_close(out_fp);
                    EOS_LOG_E("Failed to write file data for %s", name);
                    return EOS_ERR_FILE_ERROR;
                }
                remaining -= r;
            }

            eos_storage_file_close(out_fp);
            EOS_LOG_D("Created file: %s (size: %u bytes)", full_path, size);

            // Restore the file table position for the next entry before reading the next file name
            if (eos_storage_file_seek(fp, next_entry_pos) != EOS_OK)
            {
                EOS_LOG_E("Failed to restore table position after extracting %s", name);
                return EOS_ERR_FILE_ERROR;
            }
        }

        if (is_dir)
        {
            if (eos_storage_file_seek(fp, next_entry_pos) != EOS_OK)
            {
                EOS_LOG_E("Failed to seek to next table entry after creating dir %s", full_path);
                return EOS_ERR_FILE_ERROR;
            }
        }

        table_pos = next_entry_pos;
    }

    return EOS_OK;
}
