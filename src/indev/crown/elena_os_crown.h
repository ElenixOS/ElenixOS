/**
 * @file elena_os_crown.h
 * @brief 表冠
 * @author Sab1e
 * @date 2025-11-13
 */

#ifndef ELENA_OS_CROWN_H
#define ELENA_OS_CROWN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "elena_os_core.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/
typedef int16_t eos_crown_encoder_diff_t;
/* Public function prototypes --------------------------------*/
/**
 * @brief 设置编码器反向
 * @param reverse true = 反向，false = 正向
 */
void eos_crown_encoder_set_reverse(bool reverse);
/**
 * @brief 设置编码器的滚动对象
 * @param obj 目标滚动对象
 */
void eos_crown_encoder_set_target_obj(lv_obj_t *obj);
/**
 * @brief 根据活动 View 自动设置编码器的滚动对象，对象总是获取最先创建的对象(Firstly created)
 * @param view 目标 View
 */
void eos_crown_encoder_set_target_view(lv_obj_t *view);
/**
 * @brief 上报表冠按钮的状态
 * @param state 状态值
 */
void eos_crown_button_report(eos_button_state_t state);
/**
 * @brief 上报表冠编码器增量
 * @param diff 增量
 */
void eos_crown_encoder_report(eos_crown_encoder_diff_t diff);
/**
 * @brief 初始化
 */
void eos_crown_init(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_CROWN_H */
