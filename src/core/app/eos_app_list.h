/**
 * @file eos_app_list.h
 * @brief App list page
 */

#ifndef ELENIX_OS_APP_LIST_H
#define ELENIX_OS_APP_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
#include "eos_core.h"
#include "eos_activity.h"

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

#endif /* ELENIX_OS_APP_LIST_H */
