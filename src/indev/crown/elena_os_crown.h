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

/* Public function prototypes --------------------------------*/
/**
 * @brief 上报表冠按钮的状态
 * @param state 状态值
 */
void eos_crown_button_report(eos_button_state_t state);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_CROWN_H */
