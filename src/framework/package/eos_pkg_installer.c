/**
 * @file eos_pkg_installer.c
 * @brief EPK package installation dispatcher
 */

#include "eos_pkg_installer.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include "eos_app.h"
#define EOS_LOG_TAG "PackageInstaller"
#include "eos_log.h"
#include "eos_pkg_mgr.h"
#include "eos_storage_paths.h"
#include "eos_watchface.h"

/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

eos_result_t eos_pkg_install(const char *pkg_path)
{
    if (!pkg_path)
    {
        return EOS_ERR_VAR_NULL;
    }

    eos_pkg_t *package = NULL;
    eos_result_t result = eos_pkg_open(pkg_path, &package);
    if (result != EOS_OK)
    {
        EOS_LOG_E("Failed to open EPK package: %s", pkg_path);
        return result;
    }

    const eos_pkg_header_t *header = eos_pkg_get_header(package);
    script_pkg_type_t package_type = eos_pkg_get_package_type(package);
    eos_pkg_manifest_info_t manifest_info = {0};
    char target_path[EOS_FS_PATH_MAX];
    const char *type_name;

    switch (package_type)
    {
        case SCRIPT_TYPE_APPLICATION:
            type_name = "Application";
            break;
        case SCRIPT_TYPE_WATCHFACE:
            type_name = "Watch Face";
            break;
        default:
            eos_pkg_close(package);
            EOS_LOG_E("Unsupported Package Type in EPK Header: %d", package_type);
            return EOS_ERR_VALUE_MISMATCH;
    }
    result = eos_pkg_read_manifest_info(package, &manifest_info);
    if (result != EOS_OK)
    {
        eos_pkg_close(package);
        EOS_LOG_E("Invalid EPKG manifest: %s", pkg_path);
        return result;
    }
    if (package_type == SCRIPT_TYPE_APPLICATION)
    {
        snprintf(target_path, sizeof(target_path), EOS_APP_INSTALLED_DIR "%s", manifest_info.id);
    }
    else
    {
        snprintf(target_path, sizeof(target_path), EOS_WATCHFACE_INSTALLED_DIR "%s", manifest_info.id);
    }

    EOS_LOG_I("[EPK] Installing package\n"
              "  Path: %s\n"
              "  Type: %s (Header %.4s)\n"
              "  Name: %s\n"
              "  ID: %s\n"
              "  Version: %s\n"
              "  API: min=%u target=%u\n"
              "  Files: %u\n"
              "  Install path: %s",
              pkg_path,
              type_name,
              header->magic,
              manifest_info.name,
              manifest_info.id,
              manifest_info.version,
              (unsigned int)manifest_info.min_api_level,
              (unsigned int)manifest_info.target_api_level,
              (unsigned int)header->file_count,
              target_path);

    if (package_type == SCRIPT_TYPE_APPLICATION)
    {
        result = eos_app_install_package(package);
    }
    else
    {
        result = eos_watchface_install_package(package);
    }

    if (result == EOS_OK)
    {
        EOS_LOG_I("[EPK] Installation complete: %s", manifest_info.id);
    }
    else
    {
        EOS_LOG_E("[EPK] Installation failed: id=%s code=%d", manifest_info.id, result);
    }

    eos_pkg_manifest_info_free(&manifest_info);
    eos_pkg_close(package);
    return result;
}
