/**
 * @file elena_os_app_list.h
 * @brief 应用列表页面
 * @author Sab1e
 * @date 2025-08-21
 * @details
 * 系统启动时无需调用应用列表的函数
 *
 * 生命周期：
 *
 * 当需要加载应用列表时只需要调用`eos_app_list_create()`即可。
 *
 * 当离开页面时时无需删除，下次加载此页面时自动删除旧的 Screen，并重新加载。
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
 * @brief 获取应用列表的 Screen
 * @return lv_obj_t* Screen 对象
 */
lv_obj_t *eos_app_list_get_screen(void);
/**
 * @brief 创建应用列表
 */
void eos_app_list_create(void);

void eos_app_list_get_clicked_icon_center_pos(lv_coord_t *x, lv_coord_t *y);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_APP_LIST_H */
