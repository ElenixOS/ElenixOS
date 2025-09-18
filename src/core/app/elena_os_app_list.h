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

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/
/**
 * @brief 获取应用列表的 Screen
 * @return lv_obj_t* Screen 对象
 */
lv_obj_t *eos_app_list_get_screen(void);
EOS_DEFINE_SCREEN_ASYNC(eos_app_list_create);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_APP_LIST_H */
