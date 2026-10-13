/**
 * @file eos_pkg_mgr.c
 * @brief EPKG v1 package parser, validator, preview reader, and unpacker
 */

#include "eos_pkg_mgr.h"

/* Includes ---------------------------------------------------*/
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "eos_pkg_codec.h"
#include "eos_service_storage.h"
#define EOS_LOG_TAG "PackageManager"
#include "eos_log.h"
#include "eos_mem.h"

/* Macros and Definitions -------------------------------------*/
#define EOS_PKG_ENTRY_FLAG_MASK 0U
#define EOS_PKG_HEADER_CRC_OFFSET 56U
#define EOS_PKG_FPM_FLAG_MASK EOS_PKG_FPM_FLAG_HAS_ICON

/* Variables --------------------------------------------------*/
typedef struct
{
    uint32_t record_offset;
    uint32_t record_size;
    uint32_t path_len;
    uint8_t entry_type;
    uint8_t codec;
    uint16_t flags;
    uint32_t data_offset;
    uint32_t stored_size;
    uint32_t original_size;
    uint32_t crc32;
    char *path;
} eos_pkg_entry_t;

struct eos_pkg
{
    eos_file_t file;
    eos_pkg_header_t header;
    eos_pkg_fpm_t fpm;
    script_pkg_type_t type;
    uint32_t file_size;
    eos_pkg_entry_t *entries;
};

/* Function Prototypes ----------------------------------------*/
static bool _is_epk_path(const char *pkg_path);
static uint16_t _read_u16(const uint8_t *data);
static uint32_t _read_u32(const uint8_t *data);
static bool _read_exact(eos_file_t file, void *buffer, size_t size);
static bool _is_valid_utf8(const uint8_t *data, size_t size);
static bool _is_valid_relative_path(const uint8_t *path, size_t path_len);
static void _free_entries(eos_pkg_t *package);
static eos_result_t _read_header_from_file(eos_file_t file, eos_pkg_header_t *header);
static eos_result_t _read_fpm_from_file(eos_file_t file, const eos_pkg_header_t *header, eos_pkg_fpm_t *fpm);
static eos_result_t _read_entries(eos_pkg_t *package);
static eos_result_t _validate_layout(eos_pkg_t *package);
static eos_pkg_entry_t *_find_entry(eos_pkg_t *package, const char *path);
static eos_result_t _read_manifest_bytes(eos_pkg_t *package, uint8_t **manifest, uint32_t *manifest_size);
static eos_result_t _parse_manifest_info(const uint8_t *manifest,
                                         uint32_t manifest_size,
                                         eos_pkg_manifest_info_t *info);
static eos_result_t _unpack_entry(eos_pkg_t *package,
                                  const eos_pkg_entry_t *entry,
                                  const char *output_path,
                                  uint8_t *compressed_buffer,
                                  uint8_t *raw_buffer);

/* Function Implementations -----------------------------------*/
static bool _is_epk_path(const char *pkg_path)
{
    size_t length;

    if (!pkg_path)
    {
        return false;
    }
    length = strlen(pkg_path);
    return length >= 4U && strcmp(pkg_path + length - 4U, ".epk") == 0;
}

static uint16_t _read_u16(const uint8_t *data)
{
    return (uint16_t)data[0] | (uint16_t)((uint16_t)data[1] << 8U);
}

static uint32_t _read_u32(const uint8_t *data)
{
    return (uint32_t)data[0] | ((uint32_t)data[1] << 8U) | ((uint32_t)data[2] << 16U) | ((uint32_t)data[3] << 24U);
}

static bool _read_exact(eos_file_t file, void *buffer, size_t size)
{
    return eos_storage_file_read(file, buffer, size) == (ssize_t)size;
}

static bool _is_valid_utf8(const uint8_t *data, size_t size)
{
    size_t index = 0U;

    while (index < size)
    {
        uint8_t first = data[index];
        size_t continuation_count;
        uint32_t codepoint;

        if (first <= 0x7FU)
        {
            index++;
            continue;
        }
        if (first >= 0xC2U && first <= 0xDFU)
        {
            continuation_count = 1U;
            codepoint = first & 0x1FU;
        }
        else if (first >= 0xE0U && first <= 0xEFU)
        {
            continuation_count = 2U;
            codepoint = first & 0x0FU;
        }
        else if (first >= 0xF0U && first <= 0xF4U)
        {
            continuation_count = 3U;
            codepoint = first & 0x07U;
        }
        else
        {
            return false;
        }

        if (index + continuation_count >= size)
        {
            return false;
        }
        for (size_t offset = 1U; offset <= continuation_count; offset++)
        {
            uint8_t continuation = data[index + offset];
            if ((continuation & 0xC0U) != 0x80U)
            {
                return false;
            }
            codepoint = (codepoint << 6U) | (continuation & 0x3FU);
        }
        if ((continuation_count == 2U && codepoint < 0x800U) || (continuation_count == 3U && codepoint < 0x10000U)
            || codepoint > 0x10FFFFU || (codepoint >= 0xD800U && codepoint <= 0xDFFFU))
        {
            return false;
        }
        index += continuation_count + 1U;
    }
    return true;
}

static bool _is_valid_relative_path(const uint8_t *path, size_t path_len)
{
    size_t component_start = 0U;

    if (!path || path_len == 0U || path_len >= EOS_FS_PATH_MAX || path[0] == '/' || path[path_len - 1U] == '/')
    {
        return false;
    }
    if (!_is_valid_utf8(path, path_len))
    {
        return false;
    }

    for (size_t index = 0U; index < path_len; index++)
    {
        if (path[index] == '\\' || path[index] == 0U || path[index] < 0x20U)
        {
            return false;
        }
        if (path[index] == '/')
        {
            size_t component_length = index - component_start;
            if (component_length == 0U || (component_length == 1U && path[component_start] == '.')
                || (component_length == 2U && path[component_start] == '.' && path[component_start + 1U] == '.'))
            {
                return false;
            }
            component_start = index + 1U;
        }
    }

    size_t component_length = path_len - component_start;
    return component_length != 0U && !(component_length == 1U && path[component_start] == '.')
           && !(component_length == 2U && path[component_start] == '.' && path[component_start + 1U] == '.');
}

static void _free_entries(eos_pkg_t *package)
{
    if (!package || !package->entries)
    {
        return;
    }

    for (uint32_t index = 0U; index < package->header.file_count; index++)
    {
        eos_free(package->entries[index].path);
    }
    eos_free(package->entries);
    package->entries = NULL;
}

void eos_pkg_free(script_pkg_t *pkg)
{
    EOS_CHECK_PTR_RETURN(pkg);

    eos_free((void *)pkg->id);
    eos_free((void *)pkg->name);
    eos_free((void *)pkg->version);
    eos_free((void *)pkg->author);
    eos_free((void *)pkg->description);
    eos_free((void *)pkg->script_str);
    eos_free((void *)pkg->base_path);
    if (pkg->permissions)
    {
        for (uint8_t index = 0U; index < pkg->permission_count; index++)
        {
            eos_free((void *)pkg->permissions[index]);
        }
        eos_free(pkg->permissions);
    }

    memset(pkg, 0, sizeof(*pkg));
    pkg->type = SCRIPT_TYPE_UNKNOWN;
}

eos_result_t eos_pkg_get_type(const eos_pkg_header_t *header, script_pkg_type_t *pkg_type)
{
    if (!header || !pkg_type)
    {
        return EOS_ERR_VAR_NULL;
    }
    if (header->package_type == EOS_PKG_TYPE_APPLICATION)
    {
        *pkg_type = SCRIPT_TYPE_APPLICATION;
        return EOS_OK;
    }
    if (header->package_type == EOS_PKG_TYPE_WATCHFACE)
    {
        *pkg_type = SCRIPT_TYPE_WATCHFACE;
        return EOS_OK;
    }
    *pkg_type = SCRIPT_TYPE_UNKNOWN;
    EOS_LOG_E("Unsupported EPKG package type: %" PRIu32, header->package_type);
    return EOS_ERR_VALUE_MISMATCH;
}

static eos_result_t _read_header_from_file(eos_file_t file, eos_pkg_header_t *header)
{
    uint8_t raw[EOS_PKG_HEADER_SIZE];
    uint8_t crc_header[EOS_PKG_HEADER_SIZE];

    if (file == EOS_FILE_INVALID || !header)
    {
        return EOS_ERR_VAR_NULL;
    }
    if (eos_storage_file_seek(file, 0U) != EOS_OK || !_read_exact(file, raw, sizeof(raw)))
    {
        EOS_LOG_E("Failed to read EPKG header");
        return EOS_ERR_FILE_ERROR;
    }

    memcpy(header->magic, raw, sizeof(header->magic));
    header->format_version = _read_u32(raw + 4U);
    header->header_size = _read_u32(raw + 8U);
    header->package_type = _read_u32(raw + 12U);
    header->flags = _read_u32(raw + 16U);
    header->file_count = _read_u32(raw + 20U);
    header->fpm_offset = _read_u32(raw + 24U);
    header->fpm_size = _read_u32(raw + 28U);
    header->table_offset = _read_u32(raw + 32U);
    header->data_offset = _read_u32(raw + 36U);
    header->signature_offset = _read_u32(raw + 40U);
    header->signature_size = _read_u32(raw + 44U);
    header->total_stored_size = _read_u32(raw + 48U);
    header->total_original_size = _read_u32(raw + 52U);
    header->header_crc32 = _read_u32(raw + EOS_PKG_HEADER_CRC_OFFSET);

    memcpy(crc_header, raw, sizeof(crc_header));
    memset(crc_header + EOS_PKG_HEADER_CRC_OFFSET, 0, sizeof(uint32_t));
    if (memcmp(header->magic, EOS_PKG_MAGIC, sizeof(header->magic)) != 0
        || header->format_version != EOS_PKG_FORMAT_VERSION || header->header_size != EOS_PKG_HEADER_SIZE
        || header->flags != 0U || header->header_crc32 != eos_pkg_crc32(crc_header, sizeof(crc_header)))
    {
        EOS_LOG_E("Invalid EPKG header");
        return EOS_ERR_VALUE_MISMATCH;
    }
    if (header->signature_offset != 0U || header->signature_size != 0U)
    {
        EOS_LOG_E("Signed EPKG packages are not implemented yet");
        return EOS_ERR_VALUE_MISMATCH;
    }
    return EOS_OK;
}

static eos_result_t _read_entries(eos_pkg_t *package)
{
    uint32_t table_pos = package->header.table_offset;

    if (package->header.file_count == 0U || package->header.file_count > EOS_PKG_MAX_FILE_COUNT
        || package->header.data_offset < package->header.table_offset
        || package->header.data_offset > package->file_size)
    {
        EOS_LOG_E("Invalid EPKG file table bounds");
        return EOS_ERR_VALUE_MISMATCH;
    }

    package->entries = eos_malloc(sizeof(eos_pkg_entry_t) * package->header.file_count);
    if (!package->entries)
    {
        return EOS_ERR_MEM;
    }
    memset(package->entries, 0, sizeof(eos_pkg_entry_t) * package->header.file_count);

    for (uint32_t index = 0U; index < package->header.file_count; index++)
    {
        uint8_t fixed[EOS_PKG_ENTRY_FIXED_SIZE];
        eos_pkg_entry_t *entry = &package->entries[index];
        uint32_t record_end;

        if (table_pos > package->header.data_offset
            || package->header.data_offset - table_pos < EOS_PKG_ENTRY_FIXED_SIZE
            || eos_storage_file_seek(package->file, table_pos) != EOS_OK
            || !_read_exact(package->file, fixed, sizeof(fixed)))
        {
            EOS_LOG_E("Failed to read EPKG table entry %" PRIu32, index);
            return EOS_ERR_VALUE_MISMATCH;
        }

        entry->record_offset = table_pos;
        entry->record_size = _read_u32(fixed);
        entry->path_len = _read_u32(fixed + 4U);
        entry->entry_type = fixed[8];
        entry->codec = fixed[9];
        entry->flags = _read_u16(fixed + 10U);
        entry->data_offset = _read_u32(fixed + 12U);
        entry->stored_size = _read_u32(fixed + 16U);
        entry->original_size = _read_u32(fixed + 20U);
        entry->crc32 = _read_u32(fixed + 24U);

        if ((entry->record_size & 3U) != 0U || entry->record_size < EOS_PKG_ENTRY_FIXED_SIZE + entry->path_len
            || entry->record_size > package->header.data_offset - table_pos || entry->path_len >= EOS_FS_PATH_MAX)
        {
            EOS_LOG_E("Invalid EPKG table entry %" PRIu32, index);
            return EOS_ERR_VALUE_MISMATCH;
        }

        entry->path = eos_malloc(entry->path_len + 1U);
        if (!entry->path)
        {
            return EOS_ERR_MEM;
        }
        if (!_read_exact(package->file, entry->path, entry->path_len))
        {
            EOS_LOG_E("Failed to read EPKG path for entry %" PRIu32, index);
            return EOS_ERR_FILE_ERROR;
        }
        entry->path[entry->path_len] = '\0';
        if (!_is_valid_relative_path((const uint8_t *)entry->path, entry->path_len)
            || entry->flags != EOS_PKG_ENTRY_FLAG_MASK)
        {
            EOS_LOG_E("Unsafe or unsupported EPKG table entry %" PRIu32, index);
            return EOS_ERR_VALUE_MISMATCH;
        }

        uint32_t padding_size = entry->record_size - EOS_PKG_ENTRY_FIXED_SIZE - entry->path_len;
        if (padding_size > 0U)
        {
            uint8_t padding[3];
            if (padding_size > sizeof(padding) || !_read_exact(package->file, padding, padding_size))
            {
                return EOS_ERR_VALUE_MISMATCH;
            }
            for (uint32_t padding_index = 0U; padding_index < padding_size; padding_index++)
            {
                if (padding[padding_index] != 0U)
                {
                    EOS_LOG_E("Non-zero EPKG table padding");
                    return EOS_ERR_VALUE_MISMATCH;
                }
            }
        }

        record_end = table_pos + entry->record_size;
        if (record_end < table_pos || eos_storage_file_seek(package->file, record_end) != EOS_OK)
        {
            return EOS_ERR_VALUE_MISMATCH;
        }
        table_pos = record_end;
    }

    if (table_pos != package->header.data_offset)
    {
        EOS_LOG_E("EPKG table does not end at data_offset");
        return EOS_ERR_VALUE_MISMATCH;
    }
    return EOS_OK;
}

static eos_result_t _validate_layout(eos_pkg_t *package)
{
    uint32_t data_cursor = package->header.data_offset;
    uint32_t total_stored = 0U;
    uint32_t total_original = 0U;
    eos_pkg_entry_t *manifest;
    eos_pkg_entry_t *icon;

    for (uint32_t index = 0U; index < package->header.file_count; index++)
    {
        eos_pkg_entry_t *entry = &package->entries[index];

        for (uint32_t previous = 0U; previous < index; previous++)
        {
            if (strcmp(entry->path, package->entries[previous].path) == 0)
            {
                EOS_LOG_E("Duplicate EPKG path: %s", entry->path);
                return EOS_ERR_VALUE_MISMATCH;
            }
        }

        if (entry->entry_type == EOS_PKG_ENTRY_DIRECTORY)
        {
            if (entry->codec != EOS_PKG_CODEC_NONE || entry->data_offset != 0U || entry->stored_size != 0U
                || entry->original_size != 0U || entry->crc32 != 0U)
            {
                return EOS_ERR_VALUE_MISMATCH;
            }
            continue;
        }
        if (entry->entry_type != EOS_PKG_ENTRY_FILE || entry->codec > EOS_PKG_CODEC_LZ4_BLOCK
            || entry->data_offset != data_cursor || entry->stored_size > package->file_size - data_cursor)
        {
            EOS_LOG_E("Invalid EPKG data range for %s", entry->path);
            return EOS_ERR_VALUE_MISMATCH;
        }
        if (entry->codec == EOS_PKG_CODEC_NONE && entry->stored_size != entry->original_size)
        {
            return EOS_ERR_VALUE_MISMATCH;
        }
        if (entry->codec == EOS_PKG_CODEC_LZ4_BLOCK
            && (entry->original_size == 0U || entry->stored_size < sizeof(uint32_t) * 2U))
        {
            return EOS_ERR_VALUE_MISMATCH;
        }
        if (entry->original_size > UINT32_MAX - total_original)
        {
            EOS_LOG_E("EPKG original size total overflows");
            return EOS_ERR_VALUE_MISMATCH;
        }
        data_cursor += entry->stored_size;
        total_stored += entry->stored_size;
        total_original += entry->original_size;
    }

    if (data_cursor != package->file_size || total_stored != package->header.total_stored_size
        || total_original != package->header.total_original_size)
    {
        EOS_LOG_E("EPKG data totals do not match header");
        return EOS_ERR_VALUE_MISMATCH;
    }

    manifest = _find_entry(package, EOS_PKG_MANIFEST_FILE_NAME);
    icon = _find_entry(package, EOS_PKG_ICON_FILE_NAME);
    if (!manifest || manifest->entry_type != EOS_PKG_ENTRY_FILE || manifest->codec != EOS_PKG_CODEC_NONE
        || manifest->data_offset != package->header.data_offset || manifest->stored_size != package->fpm.manifest_size
        || manifest->original_size != package->fpm.manifest_size || manifest->crc32 != package->fpm.manifest_crc32)
    {
        EOS_LOG_E("EPKG manifest does not match FPMA");
        return EOS_ERR_VALUE_MISMATCH;
    }

    if ((package->fpm.flags & EOS_PKG_FPM_FLAG_HAS_ICON) != 0U)
    {
        uint32_t expected_icon_offset = package->header.data_offset + package->fpm.manifest_size;
        if (!icon || icon->entry_type != EOS_PKG_ENTRY_FILE || icon->codec != EOS_PKG_CODEC_NONE
            || icon->data_offset != expected_icon_offset || icon->stored_size != package->fpm.icon_size
            || icon->original_size != package->fpm.icon_size || icon->crc32 != package->fpm.icon_crc32)
        {
            EOS_LOG_E("EPKG icon does not match FPMA");
            return EOS_ERR_VALUE_MISMATCH;
        }
    }
    else if (icon)
    {
        EOS_LOG_E("EPKG contains icon.bin without FPMA icon flag");
        return EOS_ERR_VALUE_MISMATCH;
    }
    return EOS_OK;
}

static eos_pkg_entry_t *_find_entry(eos_pkg_t *package, const char *path)
{
    if (!package || !path)
    {
        return NULL;
    }
    for (uint32_t index = 0U; index < package->header.file_count; index++)
    {
        if (strcmp(package->entries[index].path, path) == 0)
        {
            return &package->entries[index];
        }
    }
    return NULL;
}

eos_result_t eos_pkg_open(const char *pkg_path, eos_pkg_t **package)
{
    eos_pkg_t *opened_package;
    eos_result_t result;

    if (!pkg_path || !package)
    {
        return EOS_ERR_VAR_NULL;
    }
    *package = NULL;
    if (!_is_epk_path(pkg_path) || !eos_storage_is_file(pkg_path))
    {
        return EOS_ERR_FILE_ERROR;
    }

    opened_package = eos_malloc(sizeof(*opened_package));
    if (!opened_package)
    {
        return EOS_ERR_MEM;
    }
    memset(opened_package, 0, sizeof(*opened_package));
    opened_package->file = EOS_FILE_INVALID;
    opened_package->file = eos_storage_file_open_read(pkg_path);
    if (opened_package->file == EOS_FILE_INVALID)
    {
        eos_free(opened_package);
        return EOS_ERR_FILE_ERROR;
    }
    if (eos_storage_file_size(opened_package->file, &opened_package->file_size) != EOS_OK
        || opened_package->file_size < EOS_PKG_HEADER_SIZE)
    {
        eos_pkg_close(opened_package);
        return EOS_ERR_FILE_ERROR;
    }

    result = _read_header_from_file(opened_package->file, &opened_package->header);
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
    if (opened_package->header.fpm_offset > opened_package->file_size
        || opened_package->header.fpm_size > opened_package->file_size - opened_package->header.fpm_offset)
    {
        eos_pkg_close(opened_package);
        return EOS_ERR_VALUE_MISMATCH;
    }
    result = _read_fpm_from_file(opened_package->file, &opened_package->header, &opened_package->fpm);
    if (result != EOS_OK)
    {
        eos_pkg_close(opened_package);
        return result;
    }
    result = _read_entries(opened_package);
    if (result == EOS_OK)
    {
        result = _validate_layout(opened_package);
    }
    if (result != EOS_OK)
    {
        eos_pkg_close(opened_package);
        return result;
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
    _free_entries(package);
    if (package->file != EOS_FILE_INVALID)
    {
        eos_storage_file_close(package->file);
    }
    eos_free(package);
}

const eos_pkg_header_t *eos_pkg_get_header(const eos_pkg_t *package)
{
    return package ? &package->header : NULL;
}

const eos_pkg_fpm_t *eos_pkg_get_fpm(const eos_pkg_t *package)
{
    return package ? &package->fpm : NULL;
}

script_pkg_type_t eos_pkg_get_package_type(const eos_pkg_t *package)
{
    return package ? package->type : SCRIPT_TYPE_UNKNOWN;
}

eos_result_t eos_pkg_read_header(const char *pkg_path, eos_pkg_header_t *header)
{
    eos_pkg_t *package = NULL;
    eos_result_t result;

    if (!pkg_path || !header)
    {
        return EOS_ERR_VAR_NULL;
    }
    result = eos_pkg_open(pkg_path, &package);
    if (result != EOS_OK)
    {
        return result;
    }
    *header = package->header;
    eos_pkg_close(package);
    return EOS_OK;
}

static eos_result_t _read_manifest_bytes(eos_pkg_t *package, uint8_t **manifest, uint32_t *manifest_size)
{
    uint8_t *buffer;

    if (!package || !manifest || !manifest_size)
    {
        return EOS_ERR_VAR_NULL;
    }
    *manifest = NULL;
    *manifest_size = package->fpm.manifest_size;
    buffer = eos_malloc(package->fpm.manifest_size + 1U);
    if (!buffer)
    {
        return EOS_ERR_MEM;
    }
    if (eos_storage_file_seek(package->file, package->header.data_offset) != EOS_OK
        || !_read_exact(package->file, buffer, package->fpm.manifest_size))
    {
        eos_free(buffer);
        return EOS_ERR_FILE_ERROR;
    }
    buffer[package->fpm.manifest_size] = '\0';
    if (eos_pkg_crc32(buffer, package->fpm.manifest_size) != package->fpm.manifest_crc32)
    {
        eos_free(buffer);
        return EOS_ERR_VALUE_MISMATCH;
    }
    *manifest = buffer;
    return EOS_OK;
}

eos_result_t eos_pkg_read_preview(eos_pkg_t *package, eos_pkg_preview_t *preview)
{
    eos_result_t result;

    if (!package || !preview)
    {
        return EOS_ERR_VAR_NULL;
    }
    memset(preview, 0, sizeof(*preview));
    preview->fpm = package->fpm;
    result = _read_manifest_bytes(package, &preview->manifest, &preview->manifest_size);
    if (result != EOS_OK)
    {
        return result;
    }
    if ((package->fpm.flags & EOS_PKG_FPM_FLAG_HAS_ICON) != 0U)
    {
        if (package->fpm.icon_size == 0U)
        {
            return EOS_OK;
        }
        preview->icon = eos_malloc(package->fpm.icon_size);
        if (!preview->icon)
        {
            eos_pkg_preview_free(preview);
            return EOS_ERR_MEM;
        }
        preview->icon_size = package->fpm.icon_size;
        if (eos_storage_file_seek(package->file, package->header.data_offset + package->fpm.manifest_size) != EOS_OK
            || !_read_exact(package->file, preview->icon, preview->icon_size)
            || eos_pkg_crc32(preview->icon, preview->icon_size) != package->fpm.icon_crc32)
        {
            eos_pkg_preview_free(preview);
            return EOS_ERR_VALUE_MISMATCH;
        }
    }
    return EOS_OK;
}

void eos_pkg_preview_free(eos_pkg_preview_t *preview)
{
    if (!preview)
    {
        return;
    }
    eos_free(preview->manifest);
    eos_free(preview->icon);
    memset(preview, 0, sizeof(*preview));
}

static eos_result_t _parse_manifest_info(const uint8_t *manifest, uint32_t manifest_size, eos_pkg_manifest_info_t *info)
{
    cJSON *root;
    cJSON *id;
    cJSON *name;
    cJSON *version;
    cJSON *author;
    cJSON *description;
    cJSON *min_api;
    cJSON *target_api;
    cJSON *type;
    double min_value;
    double target_value;

    if (!manifest || !info || manifest_size == 0U)
    {
        return EOS_ERR_VAR_NULL;
    }
    root = cJSON_Parse((const char *)manifest);
    if (!root)
    {
        return EOS_ERR_JSON_ERROR;
    }
    id = cJSON_GetObjectItemCaseSensitive(root, "id");
    name = cJSON_GetObjectItemCaseSensitive(root, "name");
    version = cJSON_GetObjectItemCaseSensitive(root, "version");
    author = cJSON_GetObjectItemCaseSensitive(root, "author");
    description = cJSON_GetObjectItemCaseSensitive(root, "description");
    min_api = cJSON_GetObjectItemCaseSensitive(root, "minApiLevel");
    target_api = cJSON_GetObjectItemCaseSensitive(root, "targetApiLevel");
    type = cJSON_GetObjectItemCaseSensitive(root, "type");
    if (!cJSON_IsString(id) || !id->valuestring || !cJSON_IsString(name) || !name->valuestring
        || !cJSON_IsString(version) || !version->valuestring || !cJSON_IsNumber(min_api) || !cJSON_IsNumber(target_api)
        || !cJSON_IsString(author) || !author->valuestring || !cJSON_IsString(description) || !description->valuestring
        || cJSON_GetObjectItemCaseSensitive(root, "icon"))
    {
        cJSON_Delete(root);
        return EOS_ERR_JSON_ERROR;
    }

    if (type)
    {
        if (!cJSON_IsString(type) || !type->valuestring)
        {
            cJSON_Delete(root);
            return EOS_ERR_JSON_ERROR;
        }
        if (strcmp(type->valuestring, "application") == 0)
        {
            info->manifest_type = SCRIPT_TYPE_APPLICATION;
        }
        else if (strcmp(type->valuestring, "watchface") == 0)
        {
            info->manifest_type = SCRIPT_TYPE_WATCHFACE;
        }
        else
        {
            cJSON_Delete(root);
            return EOS_ERR_VALUE_MISMATCH;
        }
        info->has_manifest_type = true;
    }

    min_value = cJSON_GetNumberValue(min_api);
    target_value = cJSON_GetNumberValue(target_api);
    if (min_value < 0.0 || min_value > 65535.0 || min_value != (double)(uint16_t)min_value || target_value < 0.0
        || target_value > 65535.0 || target_value != (double)(uint16_t)target_value || min_value > target_value
        || !eos_storage_is_valid_filename(id->valuestring) || strchr(id->valuestring, '/')
        || strchr(id->valuestring, '\\'))
    {
        cJSON_Delete(root);
        return EOS_ERR_VALUE_MISMATCH;
    }

    info->id = eos_strdup(id->valuestring);
    info->name = eos_strdup(name->valuestring);
    info->version = eos_strdup(version->valuestring);
    info->min_api_level = (uint16_t)min_value;
    info->target_api_level = (uint16_t)target_value;
    cJSON_Delete(root);
    if (!info->id || !info->name || !info->version)
    {
        eos_pkg_manifest_info_free(info);
        return EOS_ERR_MEM;
    }
    return EOS_OK;
}

eos_result_t eos_pkg_read_manifest_info(eos_pkg_t *package, eos_pkg_manifest_info_t *info)
{
    uint8_t *manifest = NULL;
    uint32_t manifest_size = 0U;
    eos_result_t result;

    if (!package || !info)
    {
        return EOS_ERR_VAR_NULL;
    }
    memset(info, 0, sizeof(*info));
    result = _read_manifest_bytes(package, &manifest, &manifest_size);
    if (result == EOS_OK)
    {
        result = _parse_manifest_info(manifest, manifest_size, info);
    }
    eos_free(manifest);
    if (result == EOS_OK && info->has_manifest_type && info->manifest_type != package->type)
    {
        eos_pkg_manifest_info_free(info);
        result = EOS_ERR_VALUE_MISMATCH;
    }
    return result;
}

void eos_pkg_manifest_info_free(eos_pkg_manifest_info_t *info)
{
    if (!info)
    {
        return;
    }
    eos_free(info->id);
    eos_free(info->name);
    eos_free(info->version);
    memset(info, 0, sizeof(*info));
}

static eos_result_t _unpack_entry(eos_pkg_t *package,
                                  const eos_pkg_entry_t *entry,
                                  const char *output_path,
                                  uint8_t *compressed_buffer,
                                  uint8_t *raw_buffer)
{
    char full_path[EOS_FS_PATH_MAX];
    char *last_slash;
    eos_file_t output_file = EOS_FILE_INVALID;
    bool write_output = output_path != NULL;

    if (!write_output && entry->entry_type == EOS_PKG_ENTRY_DIRECTORY)
    {
        return EOS_OK;
    }
    if (write_output
        && snprintf(full_path, sizeof(full_path), "%s/%s", output_path, entry->path) >= (int)sizeof(full_path))
    {
        return EOS_ERR_PATH_TOO_LONG;
    }
    if (entry->entry_type == EOS_PKG_ENTRY_DIRECTORY)
    {
        return eos_storage_mkdir_recursive(full_path);
    }

    if (write_output)
    {
        last_slash = strrchr(full_path, '/');
        if (last_slash)
        {
            *last_slash = '\0';
            if (eos_storage_mkdir_recursive(full_path) != EOS_OK)
            {
                return EOS_ERR_FILE_ERROR;
            }
            *last_slash = '/';
        }
        output_file = eos_storage_file_open_write(full_path);
        if (output_file == EOS_FILE_INVALID)
        {
            return EOS_ERR_FILE_ERROR;
        }
    }
    if (eos_storage_file_seek(package->file, entry->data_offset) != EOS_OK)
    {
        if (output_file != EOS_FILE_INVALID)
        {
            eos_storage_file_close(output_file);
        }
        return EOS_ERR_FILE_ERROR;
    }

    uint32_t crc_state = EOS_PKG_CRC32_INITIAL;
    uint32_t written_raw = 0U;
    eos_result_t result = EOS_OK;
    if (entry->codec == EOS_PKG_CODEC_NONE)
    {
        uint32_t remaining = entry->stored_size;
        uint8_t buffer[EOS_PKG_READ_BLOCK];
        while (remaining > 0U)
        {
            size_t chunk = remaining > sizeof(buffer) ? sizeof(buffer) : remaining;
            ssize_t read_size = eos_storage_file_read(package->file, buffer, chunk);
            if (read_size <= 0
                || (write_output && eos_storage_file_write(output_file, buffer, (size_t)read_size) != read_size))
            {
                result = EOS_ERR_FILE_ERROR;
                break;
            }
            crc_state = eos_pkg_crc32_update(crc_state, buffer, (size_t)read_size);
            written_raw += (uint32_t)read_size;
            remaining -= (uint32_t)read_size;
        }
    }
    else
    {
        uint32_t remaining = entry->stored_size;
        while (result == EOS_OK && remaining > 0U)
        {
            uint8_t block_header[sizeof(uint32_t) * 2U];
            uint32_t raw_size;
            uint32_t compressed_size;
            int decoded_size;

            if (remaining < sizeof(block_header) || !_read_exact(package->file, block_header, sizeof(block_header)))
            {
                result = EOS_ERR_VALUE_MISMATCH;
                break;
            }
            remaining -= sizeof(block_header);
            raw_size = _read_u32(block_header);
            compressed_size = _read_u32(block_header + sizeof(uint32_t));
            if (raw_size == 0U || raw_size > EOS_PKG_LZ4_BLOCK_MAX_SIZE || compressed_size == 0U
                || compressed_size > EOS_PKG_LZ4_COMPRESSED_MAX_SIZE || compressed_size > remaining
                || raw_size > entry->original_size || written_raw > entry->original_size - raw_size)
            {
                result = EOS_ERR_VALUE_MISMATCH;
                break;
            }
            if (!_read_exact(package->file, compressed_buffer, compressed_size))
            {
                result = EOS_ERR_FILE_ERROR;
                break;
            }
            remaining -= compressed_size;
            decoded_size = eos_pkg_lz4_decompress_block(compressed_buffer, compressed_size, raw_buffer, raw_size);
            if (decoded_size != (int)raw_size
                || (write_output && eos_storage_file_write(output_file, raw_buffer, raw_size) != (ssize_t)raw_size))
            {
                result = EOS_ERR_VALUE_MISMATCH;
                break;
            }
            crc_state = eos_pkg_crc32_update(crc_state, raw_buffer, raw_size);
            written_raw += raw_size;
        }
        if (result == EOS_OK && (remaining != 0U || written_raw != entry->original_size))
        {
            result = EOS_ERR_VALUE_MISMATCH;
        }
    }

    if (result == EOS_OK && (written_raw != entry->original_size || eos_pkg_crc32_finish(crc_state) != entry->crc32))
    {
        result = EOS_ERR_VALUE_MISMATCH;
    }
    if (output_file != EOS_FILE_INVALID)
    {
        eos_storage_file_close(output_file);
    }
    return result;
}

eos_result_t eos_pkg_validate(eos_pkg_t *package)
{
    uint8_t *compressed_buffer = NULL;
    uint8_t *raw_buffer = NULL;
    eos_pkg_manifest_info_t manifest_info = {0};
    eos_result_t result;

    if (!package)
    {
        return EOS_ERR_VAR_NULL;
    }
    result = eos_pkg_read_manifest_info(package, &manifest_info);
    eos_pkg_manifest_info_free(&manifest_info);
    if (result != EOS_OK)
    {
        return result;
    }
    for (uint32_t index = 0U; index < package->header.file_count; index++)
    {
        if (package->entries[index].codec == EOS_PKG_CODEC_LZ4_BLOCK)
        {
            compressed_buffer = eos_malloc(EOS_PKG_LZ4_COMPRESSED_MAX_SIZE);
            raw_buffer = eos_malloc(EOS_PKG_LZ4_BLOCK_MAX_SIZE);
            if (!compressed_buffer || !raw_buffer)
            {
                result = EOS_ERR_MEM;
            }
            break;
        }
    }
    if (result == EOS_OK)
    {
        for (uint32_t index = 0U; index < package->header.file_count; index++)
        {
            result = _unpack_entry(package, &package->entries[index], NULL, compressed_buffer, raw_buffer);
            if (result != EOS_OK)
            {
                break;
            }
        }
    }
    eos_free(compressed_buffer);
    eos_free(raw_buffer);
    return result;
}

eos_result_t eos_pkg_unpack(eos_pkg_t *package, const char *output_path)
{
    uint8_t *compressed_buffer = NULL;
    uint8_t *raw_buffer = NULL;
    eos_result_t result = EOS_OK;

    if (!package || !output_path)
    {
        return EOS_ERR_VAR_NULL;
    }
    if (eos_storage_mkdir_recursive(output_path) != EOS_OK)
    {
        return EOS_ERR_FILE_ERROR;
    }
    for (uint32_t index = 0U; index < package->header.file_count; index++)
    {
        if (package->entries[index].codec == EOS_PKG_CODEC_LZ4_BLOCK)
        {
            compressed_buffer = eos_malloc(EOS_PKG_LZ4_COMPRESSED_MAX_SIZE);
            raw_buffer = eos_malloc(EOS_PKG_LZ4_BLOCK_MAX_SIZE);
            if (!compressed_buffer || !raw_buffer)
            {
                result = EOS_ERR_MEM;
            }
            break;
        }
    }
    if (result == EOS_OK)
    {
        for (uint32_t index = 0U; index < package->header.file_count; index++)
        {
            result = _unpack_entry(package, &package->entries[index], output_path, compressed_buffer, raw_buffer);
            if (result != EOS_OK)
            {
                break;
            }
        }
    }
    eos_free(compressed_buffer);
    eos_free(raw_buffer);
    return result;
}

static eos_result_t _read_fpm_from_file(eos_file_t file, const eos_pkg_header_t *header, eos_pkg_fpm_t *fpm)
{
    uint8_t raw[EOS_PKG_FPM_SIZE];

    if (!header || !fpm)
    {
        return EOS_ERR_VAR_NULL;
    }
    if (header->fpm_offset != EOS_PKG_HEADER_SIZE || header->fpm_size != EOS_PKG_FPM_SIZE
        || header->table_offset != header->fpm_offset + header->fpm_size
        || eos_storage_file_seek(file, header->fpm_offset) != EOS_OK || !_read_exact(file, raw, sizeof(raw)))
    {
        EOS_LOG_E("Invalid FPMA location");
        return EOS_ERR_VALUE_MISMATCH;
    }

    memcpy(fpm->magic, raw, sizeof(fpm->magic));
    fpm->fpm_version = _read_u32(raw + 4U);
    fpm->area_size = _read_u32(raw + 8U);
    fpm->flags = _read_u32(raw + 12U);
    fpm->manifest_size = _read_u32(raw + 16U);
    fpm->icon_size = _read_u32(raw + 20U);
    fpm->manifest_crc32 = _read_u32(raw + 24U);
    fpm->icon_crc32 = _read_u32(raw + 28U);

    if (memcmp(fpm->magic, EOS_PKG_FPM_MAGIC, sizeof(fpm->magic)) != 0 || fpm->fpm_version != EOS_PKG_FPM_VERSION
        || fpm->area_size != EOS_PKG_FPM_SIZE || (fpm->flags & ~EOS_PKG_FPM_FLAG_MASK) != 0U || fpm->manifest_size == 0U
        || fpm->manifest_size > EOS_PKG_MAX_PREVIEW_FILE_SIZE || fpm->icon_size > EOS_PKG_MAX_PREVIEW_FILE_SIZE
        || ((fpm->flags & EOS_PKG_FPM_FLAG_HAS_ICON) == 0U && (fpm->icon_size != 0U || fpm->icon_crc32 != 0U)))
    {
        EOS_LOG_E("Invalid FPMA area");
        return EOS_ERR_VALUE_MISMATCH;
    }
    return EOS_OK;
}
