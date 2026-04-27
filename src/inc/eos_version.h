/**
 * @file eos_version.h
 * @brief Version definitions
 *
 * Follows [Semantic Versioning](https://semver.org/spec/v2.0.0.html)
 */

#ifndef ELENIX_OS_VERSION_H
#define ELENIX_OS_VERSION_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/* Public macros ----------------------------------------------*/

#define ELENIX_OS_VERSION_MAJOR 0
#define ELENIX_OS_VERSION_MINOR 3
#define ELENIX_OS_VERSION_PATCH 0
#define ELENIX_OS_VERSION_INFO "alpha.1"

#define STRINGIFY(x) #x
#define VERSION_STRING(major, minor, patch, info) STRINGIFY(major) "." STRINGIFY(minor) "." STRINGIFY(patch) "-" info

#define ELENIX_OS_VERSION_FULL VERSION_STRING(ELENIX_OS_VERSION_MAJOR, ELENIX_OS_VERSION_MINOR, ELENIX_OS_VERSION_PATCH, ELENIX_OS_VERSION_INFO)

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* ELENIX_OS_VERSION_H */
