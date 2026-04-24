/**
 * @file eos_utils.h
 * @brief Utility macro definitions
 */

#ifndef ELENIX_OS_UTILS_H
#define ELENIX_OS_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/* Public macros ----------------------------------------------*/
#define EOS_CLAMP(v, min, max)  ((v) < (min) ? (min) : ((v) > (max) ? (max) : (v)))

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* ELENIX_OS_UTILS_H */