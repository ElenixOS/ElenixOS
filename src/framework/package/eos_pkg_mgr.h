/**
 * @file eos_pkg_mgr.h
 * @brief EPKG v1 package format and package manager
 */

#ifndef EOS_PKG_MGR_H
#define EOS_PKG_MGR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "eos_core.h"
#include "eos_port.h"
#include "script_engine_core.h"

/* Public macros ----------------------------------------------*/
#define EOS_PKG_MAGIC "EPKG"
#define EOS_PKG_FPM_MAGIC "FPMA"
#define EOS_PKG_HEADER_SIZE 60U
#define EOS_PKG_FPM_SIZE 32U
#define EOS_PKG_FORMAT_VERSION 1U
#define EOS_PKG_FPM_VERSION 1U
#define EOS_PKG_READ_BLOCK 512U
#define EOS_PKG_ENTRY_FIXED_SIZE 28U
#define EOS_PKG_MAX_FILE_COUNT 1024U
#define EOS_PKG_MAX_PREVIEW_FILE_SIZE (1024U * 1024U)
#define EOS_PKG_LZ4_BLOCK_MAX_SIZE (16U * 1024U)
#define EOS_PKG_LZ4_COMPRESSED_MAX_SIZE (EOS_PKG_LZ4_BLOCK_MAX_SIZE + 80U)
#define EOS_PKG_MANIFEST_FILE_NAME "manifest.json"
#define EOS_PKG_ICON_FILE_NAME "icon.bin"

#define EOS_PKG_TYPE_APPLICATION 1U
#define EOS_PKG_TYPE_WATCHFACE 2U

#define EOS_PKG_ENTRY_FILE 0U
#define EOS_PKG_ENTRY_DIRECTORY 1U

#define EOS_PKG_CODEC_NONE 0U
#define EOS_PKG_CODEC_LZ4_BLOCK 1U

#define EOS_PKG_FPM_FLAG_HAS_ICON (1U << 0)

/* Public typedefs --------------------------------------------*/
/**
 * @brief Serialized EPKG header represented as host fields.
 *
 * This structure is not written directly to disk. Fields are serialized one
 * by one according to docs/epkg_v1_spec.md.
 */
typedef struct
{
    char magic[4];
    uint32_t format_version;
    uint32_t header_size;
    uint32_t package_type;
    uint32_t flags;
    uint32_t file_count;
    uint32_t fpm_offset;
    uint32_t fpm_size;
    uint32_t table_offset;
    uint32_t data_offset;
    uint32_t signature_offset;
    uint32_t signature_size;
    uint32_t total_stored_size;
    uint32_t total_original_size;
    uint32_t header_crc32;
} eos_pkg_header_t;

/**
 * @brief Serialized FPMA area represented as host fields.
 */
typedef struct
{
    char magic[4];
    uint32_t fpm_version;
    uint32_t area_size;
    uint32_t flags;
    uint32_t manifest_size;
    uint32_t icon_size;
    uint32_t manifest_crc32;
    uint32_t icon_crc32;
} eos_pkg_fpm_t;

/**
 * @brief Raw preview data loaded through FPMA.
 */
typedef struct
{
    eos_pkg_fpm_t fpm;
    uint8_t *manifest;
    uint32_t manifest_size;
    uint8_t *icon;
    uint32_t icon_size;
} eos_pkg_preview_t;

/**
 * @brief Required manifest metadata used before installation.
 */
typedef struct
{
    char *id;
    char *name;
    char *version;
    uint16_t min_api_level;
    uint16_t target_api_level;
    script_pkg_type_t manifest_type; /**< Type declared by manifest.json when present. */
    bool has_manifest_type; /**< False for legacy packages; their Header remains authoritative. */
} eos_pkg_manifest_info_t;

/**
 * @brief Open EPKG package context.
 */
typedef struct eos_pkg eos_pkg_t;

/* Public function prototypes ---------------------------------*/

void eos_pkg_free(script_pkg_t *pkg);
eos_result_t eos_pkg_read_header(const char *pkg_path, eos_pkg_header_t *header);
eos_result_t eos_pkg_open(const char *pkg_path, eos_pkg_t **package);
void eos_pkg_close(eos_pkg_t *package);
const eos_pkg_header_t *eos_pkg_get_header(const eos_pkg_t *package);
const eos_pkg_fpm_t *eos_pkg_get_fpm(const eos_pkg_t *package);
script_pkg_type_t eos_pkg_get_package_type(const eos_pkg_t *package);
eos_result_t eos_pkg_get_type(const eos_pkg_header_t *header, script_pkg_type_t *pkg_type);
eos_result_t eos_pkg_read_preview(eos_pkg_t *package, eos_pkg_preview_t *preview);
void eos_pkg_preview_free(eos_pkg_preview_t *preview);
eos_result_t eos_pkg_read_manifest_info(eos_pkg_t *package, eos_pkg_manifest_info_t *info);
void eos_pkg_manifest_info_free(eos_pkg_manifest_info_t *info);
eos_result_t eos_pkg_validate(eos_pkg_t *package);
eos_result_t eos_pkg_unpack(eos_pkg_t *package, const char *output_path);

#ifdef __cplusplus
}
#endif

#endif /* EOS_PKG_MGR_H */
