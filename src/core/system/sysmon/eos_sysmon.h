/**
 * @file eos_sysmon.h
 * @brief System resource usage monitoring
 */

#ifndef ELENIX_OS_SYSMON_H
#define ELENIX_OS_SYSMON_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include "eos_config.h"
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

#endif /* ELENIX_OS_SYSMON_H */