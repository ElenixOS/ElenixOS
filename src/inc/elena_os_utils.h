/**
 * @file elena_os_utils.h
 * @brief 工具宏定义
 * @author Sab1e
 * @date 2025-10-20
 */

#ifndef ELENA_OS_UTILS_H
#define ELENA_OS_UTILS_H

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

#endif /* ELENA_OS_UTILS_H */
