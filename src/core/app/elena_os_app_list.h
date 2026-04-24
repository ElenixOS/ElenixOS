/**
 * @file elena_os_app_list.h
 * @brief App list page
 * @author Sab1e
 * @date 2025-08-21
 */

#ifndef ELENA_OS_APP_LIST_H
#define ELENA_OS_APP_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
#include "elena_os_core.h"
#include "elena_os_activity.h"

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

typedef void (*eos_sys_app_entry_t)(void);

enum
{
   EOS_SYS_APP_SETTINGS = 0,
   EOS_SYS_APP_FLASH_LIGHT,
   /* New system apps can be added here */
   EOS_SYS_APP_LAST
};

extern const char *eos_sys_app_id_list[EOS_SYS_APP_LAST];

/* Public function prototypes --------------------------------*/

/**
 * @brief Enter app list
 * @return eos_activity_t* App list activity object
 */
void eos_app_list_enter(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_APP_LIST_H */
