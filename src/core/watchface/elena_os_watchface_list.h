/**
 * @file elena_os_watchface_list.h
 * @brief 表盘列表
 * @author Sab1e
 * @date 2025-08-25
 */

#ifndef ELENA_OS_WATCHFACE_LIST_H
#define ELENA_OS_WATCHFACE_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "elena_os_core.h"
#include "elena_os_activity.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

/**
 * @brief 进入表盘列表页面
 * @return eos_activity_t* 表盘列表页面的 Activity
 */
void eos_watchface_list_enter(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_WATCHFACE_LIST_H */
