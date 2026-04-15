/**
 * @file elena_os_services.h
 * @brief 系统服务
 * @author Sab1e
 * @date 2025-10-07
 */

#ifndef ELENA_OS_SERVICES_H
#define ELENA_OS_SERVICES_H

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
 * @brief 电池电量获取函数
 * @return uint8_t 返回电池电量，范围：0～100
 */
uint8_t eos_battery_service_get_level(void);

/**
 * @brief 电池是否正在充电
 * @return true
 * @return false
 */
bool eos_battery_service_get_charging(void);

/**
 * @brief 启动服务
 */
void eos_services_start(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_SERVICES_H */
