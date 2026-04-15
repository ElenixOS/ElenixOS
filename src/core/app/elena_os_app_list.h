/**
 * @file elena_os_app_list.h
 * @brief 应用列表页面
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
   /* 此处可添加新的系统应用 */
   EOS_SYS_APP_LAST
};

extern const char *eos_sys_app_id_list[EOS_SYS_APP_LAST];

/* Public function prototypes --------------------------------*/

/**
 * @brief 进入应用列表
 * @return eos_activity_t* 应用列表活动对象
 */
void eos_app_list_enter(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_APP_LIST_H */
