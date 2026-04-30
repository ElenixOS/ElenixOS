/**
 * @file eos_misc.h
 * @brief Various utility functions
 */

#ifndef ELENIX_OS_MISC_H
#define ELENIX_OS_MISC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "eos_core.h"
#include "script_engine_core.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/
/**
 * @brief Check if it's a valid filename
 * @param name File name
 * @return eos_result_t Judgment result
 */
bool eos_is_valid_filename(const char *name);
/**
 * @brief Create a copy of the given string s.
 * @param s Source string
 * @return const char* Returns newly allocated string pointer on success
 *
 * Returns NULL if memory allocation fails
 * @note Use `eos_free(str)` to release the copied string
 */
const char *eos_strdup(const char *s);
/**
 * @brief Release and clear data inside script package
 * @param pkg Target script package
 */
void eos_pkg_free(script_pkg_t *pkg);
#ifdef __cplusplus
}
#endif

#endif /* ELENIX_OS_MISC_H */