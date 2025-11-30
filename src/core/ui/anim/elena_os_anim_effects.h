/**
 * @file elena_os_anim_effects.h
 * @brief 特殊动画
 * @author Sab1e
 * @date 2025-11-28
 */

#ifndef ELENA_OS_ANIM_EFFECTS_H
#define ELENA_OS_ANIM_EFFECTS_H

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

/**
 * @brief 绑定列表选中动画
 * @param screen 旧的 Screen
 * @param list 列表或普通对象
 * @param selected_obj 选中的对象
 */
void eos_anim_list_bind(lv_obj_t *screen, lv_obj_t *list, lv_obj_t *selected_obj);

#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_ANIM_EFFECTS_H */
