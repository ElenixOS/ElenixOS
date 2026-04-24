/**
 * @file elena_os.h
 * @brief Just include this file externally
 * @author Sab1e
 * @date 2025-11-09
 */

#ifndef ELENA_OS_H
#define ELENA_OS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include "elena_os_config.h"
#include "elena_os_core.h"
#include "elena_os_sensor.h"
#if EOS_USE_VIRTUAL_DISPLAY
#include "elena_os_virtual_display.h"
#endif /* EOS_USE_VIRTUAL_DISPLAY */
#include "elena_os_side_button.h"
#include "elena_os_crown.h"
#include "elena_os_log.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_H */