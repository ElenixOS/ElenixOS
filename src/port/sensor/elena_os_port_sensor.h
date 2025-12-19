/**
 * @file elena_os_port_sensor.h
 * @brief 传感器移植接口
 * @author Sab1e
 * @date 2025-12-10
 */

#ifndef ELENA_OS_PORT_SENSOR_H
#define ELENA_OS_PORT_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "elena_os_sensor.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

/**
 * @brief 异步读取一次传感器的值
 *
 * 示例用法：
 * 提前创建读取线程，在此函数中启动线程，开始读取传感器，读取完毕后上报数据。
 * @param type 传感器类型
 * @note 读取完毕后，必须使用`eos_sensor_report()`上报读取结果，否则上层无法得知传感器数据
 * @warning 禁止在函数中执行高耗时任务
 */
void eos_sensor_read_async(eos_sensor_type_t type);
/**
 * @brief 同步读取一次传感器的值
 * @param type 传感器类型
 * @param out 传感器读取结果
 */
void eos_sensor_read_sync(eos_sensor_type_t type, eos_sensor_t *out);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_PORT_SENSOR_H */
