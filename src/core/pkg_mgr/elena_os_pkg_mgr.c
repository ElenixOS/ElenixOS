/**
 * @file elena_os_pkg_mgr.c
 * @brief 包管理器
 * @author Sab1e
 * @date 2025-08-22
 */

#include "elena_os_pkg_mgr.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elena_os_misc.h"
#include "elena_os_port.h"
#define EOS_LOG_DISABLE
#define EOS_LOG_TAG "PackageManager"
#include "elena_os_log.h"
#include "elena_os_fs.h"
#include "elena_os_fs_port.h"
#include "elena_os_mem.h"
/* Macros and Definitions -------------------------------------*/
#define EOS_PKG_HEADER_LENGTH EOS_PKG_TABLE_OFFSET
/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

eos_result_t eos_pkg_read_header(const char *pkg_path, eos_pkg_header_t *header)
{
    // 打开包文件
    eos_file_t fp = eos_fs_open_read(pkg_path);
    if (fp == EOS_FILE_INVALID)
    {
        EOS_LOG_E("Failed to open package file: %s", pkg_path);
        return -EOS_ERR_FILE_ERROR;
    }

    // 初始化包头结构体
    memset(header, 0, sizeof(eos_pkg_header_t));

    // 读取magic
    if (eos_fs_seek(fp, EOS_PKG_MAGIC_OFFSET) != 0 ||
        eos_fs_read(fp, header->magic, 4) != 4)
    {
        eos_fs_close(fp);
        EOS_LOG_E("Failed to read magic number");
        return -EOS_ERR_FILE_ERROR;
    }

    // 读取pkg_name
    if (eos_fs_seek(fp, EOS_PKG_NAME_OFFSET) != 0 ||
        eos_fs_read(fp, header->pkg_name, EOS_PKG_NAME_LEN_MAX) != EOS_PKG_NAME_LEN_MAX)
    {
        eos_fs_close(fp);
        EOS_LOG_E("Failed to read package name");
        return -EOS_ERR_FILE_ERROR;
    }
    header->pkg_name[EOS_PKG_NAME_LEN_MAX - 1] = '\0';

    // 读取pkg_id
    if (eos_fs_seek(fp, EOS_PKG_ID_OFFSET) != 0 ||
        eos_fs_read(fp, header->pkg_id, EOS_PKG_ID_LEN_MAX) != EOS_PKG_ID_LEN_MAX)
    {
        eos_fs_close(fp);
        EOS_LOG_E("Failed to read package id");
        return -EOS_ERR_FILE_ERROR;
    }
    header->pkg_id[EOS_PKG_ID_LEN_MAX - 1] = '\0';

    // 读取pkg_version
    if (eos_fs_seek(fp, EOS_PKG_VERSION_OFFSET) != 0 ||
        eos_fs_read(fp, header->pkg_version, EOS_PKG_VERSION_LEN_MAX) != EOS_PKG_VERSION_LEN_MAX)
    {
        eos_fs_close(fp);
        EOS_LOG_E("Failed to read package version");
        return -EOS_ERR_FILE_ERROR;
    }
    header->pkg_version[EOS_PKG_VERSION_LEN_MAX - 1] = '\0';

    // 读取file_count
    if (eos_fs_seek(fp, EOS_PKG_FILE_COUNT_OFFSET) != 0 ||
        eos_fs_read(fp, &header->file_count, sizeof(uint32_t)) != sizeof(uint32_t))
    {
        eos_fs_close(fp);
        EOS_LOG_E("Failed to read file count");
        return -EOS_ERR_FILE_ERROR;
    }

    // 读取reserved字段
    if (eos_fs_seek(fp, EOS_PKG_RESERVED_OFFSET) != 0 ||
        eos_fs_read(fp, &header->reserved, sizeof(uint32_t)) != sizeof(uint32_t))
    {
        eos_fs_close(fp);
        EOS_LOG_E("Failed to read reserved field");
        return -EOS_ERR_FILE_ERROR;
    }
    eos_fs_close(fp);
    EOS_LOG_D("\n"
              "=============================\n"
              "Magic: %s | Pkg Name: %s | Pkg Version: %s\n"
              "File Count: %d | Table Offset: %d\n"
              "=============================",
              header->magic, header->pkg_name,
              header->pkg_version, header->file_count,
              EOS_PKG_HEADER_LENGTH);
    return EOS_OK;
}

eos_result_t eos_pkg_mgr_unpack(const char *pkg_path, const char *output_path, const script_pkg_type_t pkg_type)
{
    // 打开包文件
    eos_file_t fp = eos_fs_open_read(pkg_path);
    if (fp == EOS_FILE_INVALID)
    {
        EOS_LOG_E("Failed to open package file");
        return -EOS_ERR_FILE_ERROR;
    }

    // 读取包头
    eos_pkg_header_t header;
    if (eos_pkg_read_header(pkg_path, &header) != EOS_OK)
    {
        EOS_LOG_E("Failed to read header");
        eos_fs_close(fp);
        return -EOS_FAILED;
    }

    // 校验魔数
    script_pkg_type_t unpack_type = SCRIPT_TYPE_UNKNOWN;
    if (memcmp(header.magic, EOS_PKG_APP_MAGIC, 4) == 0)
    {
        unpack_type = SCRIPT_TYPE_APPLICATION;
    }
    else if (memcmp(header.magic, EOS_PKG_WATCHFACE_MAGIC, 4) == 0)
    {
        unpack_type = SCRIPT_TYPE_WATCHFACE;
    }
    else
    {
        eos_fs_close(fp);
        EOS_LOG_E("Invalid magic number");
        return -EOS_ERR_FILE_ERROR;
    }

    // 检查包类型是否匹配
    if (unpack_type != pkg_type)
    {
        eos_fs_close(fp);
        EOS_LOG_E("Package type mismatch: expected %d, got %d", pkg_type, unpack_type);
        return -EOS_ERR_VALUE_MISMATCH;
    }

    // 获取文件大小
    uint32_t file_size = 0;
    if (eos_fs_size(fp, &file_size) != 0)
    {
        eos_fs_close(fp);
        EOS_LOG_E("Failed to get file size");
        return -EOS_ERR_FILE_ERROR;
    }

    // 定位到文件表位置 (紧接在文件头之后)
    if (eos_fs_seek(fp, EOS_PKG_TABLE_OFFSET) != 0)
    {
        eos_fs_close(fp);
        EOS_LOG_E("Failed to seek to file table at offset %u", EOS_PKG_TABLE_OFFSET);
        return -EOS_ERR_FILE_ERROR;
    }

    // 创建输出目录
    if (eos_fs_mkdir_recursive(output_path) != EOS_OK)
    {
        eos_fs_close(fp);
        EOS_LOG_E("Failed to create output directory");
        return -EOS_ERR_FILE_ERROR;
    }

    // 维护当前文件表偏移，避免使用底层文件结构获取 current pos
    uint32_t table_pos = EOS_PKG_TABLE_OFFSET;

    // 处理每个文件条目
    for (uint32_t i = 0; i < header.file_count; i++)
    {
        // 读取文件名长度
        uint32_t name_len;
        if (eos_fs_read(fp, &name_len, sizeof(uint32_t)) != sizeof(uint32_t))
        {
            eos_fs_close(fp);
            EOS_LOG_E("Failed to read name length for entry %u", i);
            return -EOS_ERR_FILE_ERROR;
        }
        table_pos += sizeof(uint32_t);

        // 检查文件名长度是否合理
        if (name_len > PATH_MAX)
        {
            eos_fs_close(fp);
            EOS_LOG_E("Name length %u too long for entry %u", name_len, i);
            return -EOS_ERR_FILE_ERROR;
        }

        // 动态分配内存
        char *name = (char *)eos_malloc(name_len + 1);
        if (!name)
        {
            eos_fs_close(fp);
            EOS_LOG_E("Memory allocation failed for entry %u", i);
            return -EOS_ERR_MEM;
        }

        // 读取文件名
        if (eos_fs_read(fp, name, name_len) != (int)name_len)
        {
            eos_free(name);
            eos_fs_close(fp);
            EOS_LOG_E("Failed to read name for entry %u", i);
            return -EOS_ERR_FILE_ERROR;
        }
        name[name_len] = '\0';
        table_pos += name_len;

        // 读取条目其他字段
        uint32_t is_dir, offset, size;
        if (eos_fs_read(fp, &is_dir, sizeof(uint32_t)) != sizeof(uint32_t) ||
            eos_fs_read(fp, &offset, sizeof(uint32_t)) != sizeof(uint32_t) ||
            eos_fs_read(fp, &size, sizeof(uint32_t)) != sizeof(uint32_t))
        {
            eos_free(name);
            eos_fs_close(fp);
            EOS_LOG_E("Failed to read entry fields for %s", name);
            return -EOS_ERR_FILE_ERROR;
        }
        table_pos += sizeof(uint32_t) * 3;

        // 计算下一条目在表中的偏移
        uint32_t next_entry_pos = table_pos;

        // 构建完整输出路径
        char full_path[PATH_MAX] = {0};
        snprintf(full_path, sizeof(full_path), "%s/%s", output_path, name);

        if (is_dir)
        {
            // 创建目录
            if (eos_fs_mkdir_recursive(full_path) != EOS_OK)
            {
                eos_free(name);
                eos_fs_close(fp);
                EOS_LOG_E("Failed to create directory: %s", full_path);
                return -EOS_ERR_FILE_ERROR;
            }
            EOS_LOG_D("Created directory: %s", full_path);
        }
        else
        {
            // 验证文件偏移量和大小
            if (offset < EOS_PKG_TABLE_OFFSET || offset >= file_size)
            {
                eos_free(name);
                eos_fs_close(fp);
                EOS_LOG_E("Invalid file offset: %u for %s", offset, name);
                return -EOS_ERR_FILE_ERROR;
            }

            if (offset + size > file_size)
            {
                eos_free(name);
                eos_fs_close(fp);
                EOS_LOG_E("File size overflow: %u+%u=%u for %s",
                          offset, size, offset + size, name);
                return -EOS_ERR_FILE_ERROR;
            }

            // 确保父目录存在
            char *last_slash = strrchr(full_path, '/');
            if (last_slash)
            {
                *last_slash = '\0';
                if (eos_fs_mkdir_recursive(full_path) != EOS_OK)
                {
                    eos_free(name);
                    eos_fs_close(fp);
                    EOS_LOG_E("Failed to create parent directory: %s", full_path);
                    return -EOS_ERR_FILE_ERROR;
                }
                *last_slash = '/';
            }

            // 创建文件并写入数据
            eos_file_t out_fp = eos_fs_open_write(full_path);
            if (fp == EOS_FILE_INVALID)
            {
                eos_free(name);
                eos_fs_close(fp);
                EOS_LOG_E("Failed to create file: %s", full_path);
                return -EOS_ERR_FILE_ERROR;
            }

            // 定位到文件数据
            if (eos_fs_seek(fp, offset) != 0)
            {
                eos_fs_close(out_fp);
                eos_free(name);
                eos_fs_close(fp);
                EOS_LOG_E("Failed to seek to file data for %s", name);
                return -EOS_ERR_FILE_ERROR;
            }

            // 逐块读取并写入文件
            uint32_t remaining = size;
            uint8_t buffer[EOS_PKG_READ_BLOCK];
            while (remaining > 0)
            {
                size_t to_read = remaining > sizeof(buffer) ? sizeof(buffer) : remaining;
                int r = eos_fs_read(fp, buffer, to_read);
                if (r <= 0)
                {
                    eos_fs_close(out_fp);
                    eos_free(name);
                    eos_fs_close(fp);
                    EOS_LOG_E("Failed to read file data for %s", name);
                    return -EOS_ERR_FILE_ERROR;
                }
                if (eos_fs_write(out_fp, buffer, r) != r)
                {
                    eos_fs_close(out_fp);
                    eos_free(name);
                    eos_fs_close(fp);
                    EOS_LOG_E("Failed to write file data for %s", name);
                    return -EOS_ERR_FILE_ERROR;
                }
                remaining -= r;
            }

            eos_fs_close(out_fp);
            EOS_LOG_D("Created file: %s (size: %u bytes)", full_path, size);

            // 恢复到文件表的下一条目位置，准备读取下一个文件名
            if (eos_fs_seek(fp, next_entry_pos) != 0)
            {
                eos_free(name);
                eos_fs_close(fp);
                EOS_LOG_E("Failed to restore table position after extracting %s", name);
                return -EOS_ERR_FILE_ERROR;
            }
        }

        eos_free(name);

        if (is_dir)
        {
            if (eos_fs_seek(fp, next_entry_pos) != 0)
            {
                eos_fs_close(fp);
                EOS_LOG_E("Failed to seek to next table entry after creating dir %s", full_path);
                return -EOS_ERR_FILE_ERROR;
            }
        }

        table_pos = next_entry_pos;
    }

    eos_fs_close(fp);
    return EOS_OK;
}
