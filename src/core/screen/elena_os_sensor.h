/**
 * @file elena_os_sensor.h
 * @brief 传感器定义
 * @author Sab1e
 * @date 2025-09-14
 */

#ifndef ELENA_OS_SENSOR_H
#define ELENA_OS_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "elena_os_core.h"

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/**
 * @brief 传感器类型
 */
typedef enum
{
    EOS_SENSOR_UNKONWN = 0,
    EOS_SENSOR_ACCE,      /**< 加速度传感器     */
    EOS_SENSOR_GYRO,      /**< 重力传感器       */
    EOS_SENSOR_MAG,       /**< 磁传感器         */
    EOS_SENSOR_TEMP,      /**< 温度传感器       */
    EOS_SENSOR_HUMI,      /**< 相对湿度传感器   */
    EOS_SENSOR_BARO,      /**< 气压传感器       */
    EOS_SENSOR_LIGHT,     /**< 环境光传感器     */
    EOS_SENSOR_PROXIMITY, /**< 距离传感器       */
    EOS_SENSOR_HR,        /**< 心率传感器       */
    EOS_SENSOR_TVOC,      /**< TOVC传感器       */
    EOS_SENSOR_NOISE,     /**< 噪声传感器       */
    EOS_SENSOR_STEP,      /**< 计步传感器       */
    EOS_SENSOR_FORCE      /**< 力传感器         */
} eos_sensor_type_t;

/**
 * @brief 心率传感器数据
 */
typedef struct {
    uint16_t heart_rate;        /**<  */
    uint8_t heart_rate_valid;
    uint8_t spo2;
    uint8_t spo2_valid;
}eos_heart_rate_data_t;

/**
 * @brief 传感器数据共用体
 */
typedef union
{
    eos_heart_rate_data_t hr;
}eos_sensor_data_t;

/**
 * @brief 传感器结构体
 */
typedef struct
{
    eos_sensor_type_t type;
    eos_sensor_data_t data;
}eos_sensor_t;

typedef void (*eos_sensor_cb_t)(eos_sensor_t, void *);

/* Public function prototypes --------------------------------*/
EOS_DEFINE_SCREEN_ASYNC(eos_sensor_tester_create);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_SENSOR_H */
