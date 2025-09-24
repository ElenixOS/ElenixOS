/**
 * @file elena_os_control_center.h
 * @brief 上拉控制台
 * @author Sab1e
 * @date 2025-09-22
 */

#ifndef ELENA_OS_CONTROL_CENTER_H
#define ELENA_OS_CONTROL_CENTER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/
void eos_control_center_create(lv_obj_t *parent);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_CONTROL_CENTER_H */
