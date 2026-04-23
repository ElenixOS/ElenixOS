/**
 * @file elena_os_version.h
 * @brief Version definitions
 *
 * Follows [Semantic Versioning](https://semver.org/spec/v2.0.0.html)
 * @author Sab1e
 * @date 2025-08-21
 */

#ifndef ELENA_OS_VERSION_H
#define ELENA_OS_VERSION_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/* Public macros ----------------------------------------------*/

#define ELENA_OS_VERSION_MAJOR 0
#define ELENA_OS_VERSION_MINOR 2
#define ELENA_OS_VERSION_PATCH 0
#define ELENA_OS_VERSION_INFO "alpha.1"

#define STRINGIFY(x) #x
#define VERSION_STRING(major, minor, patch, info) STRINGIFY(major) "." STRINGIFY(minor) "." STRINGIFY(patch) "-" info

#define ELENA_OS_VERSION_FULL VERSION_STRING(ELENA_OS_VERSION_MAJOR, ELENA_OS_VERSION_MINOR, ELENA_OS_VERSION_PATCH, ELENA_OS_VERSION_INFO)

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_VERSION_H */