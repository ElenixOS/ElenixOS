/**
 * @file elena_os_sysmon.h
 * @brief System resource usage monitoring
 * @author Sab1e
 * @date 2025-11-29
 */

#ifndef ELENA_OS_SYSMON_H
#define ELENA_OS_SYSMON_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include "elena_os_config.h"
#if EOS_SYSMON_TYPE == EOS_SYSMON_USE_INTERNAL
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/
void eos_sysmon_print(void);
#endif /* EOS_SYSMON_TYPE */

#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_SYSMON_H */