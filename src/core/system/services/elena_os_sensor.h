/**
 * @file elena_os_sensor.h
 * @brief 传感器定义
 * @author Sab1e
 * @date 2025-09-14
 * @details
 *
 * # Sensor
 *
 * ## 简述
 *
 * 本模块提供了 ElenaOS 中对传感器对象的统一抽象与管理，
 * 支持多类型、多实例的注册、查找和异步数据上报。
 *
 * >[!NOTE] 传感器读取函数参见`elena_os_port.c`
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
#include "elena_os_event.h"

/* Public macros ----------------------------------------------*/
#define EOS_SENSOR_NUMBER       _EOS_SENSOR_MAX - 1   /**< 传感器总数 */

/* Public typedefs --------------------------------------------*/

typedef enum
{
    EOS_SENSOR_STATE_IDLE,
    EOS_SENSOR_STATE_READING,
    EOS_SENSOR_STATE_ERROR,
} eos_sensor_state_t;

/**
 * @brief 传感器类型
 */
typedef enum
{
    EOS_SENSOR_TYPE_UNKNOWN = 0,
    EOS_SENSOR_TYPE_ACCE,      /**< 加速度传感器     */
    EOS_SENSOR_TYPE_GYRO,      /**< 重力传感器       */
    EOS_SENSOR_TYPE_MAG,       /**< 磁传感器         */
    EOS_SENSOR_TYPE_TEMP,      /**< 温度传感器       */
    EOS_SENSOR_TYPE_HUMI,      /**< 相对湿度传感器   */
    EOS_SENSOR_TYPE_BARO,      /**< 气压传感器       */
    EOS_SENSOR_TYPE_LIGHT,     /**< 环境光传感器     */
    EOS_SENSOR_TYPE_PROXIMITY, /**< 距离传感器       */
    EOS_SENSOR_TYPE_HR,        /**< 心率传感器       */
    EOS_SENSOR_TYPE_TVOC,      /**< TOVC传感器       */
    EOS_SENSOR_TYPE_NOISE,     /**< 噪声传感器       */
    EOS_SENSOR_TYPE_STEP,      /**< 计步传感器       */
    EOS_SENSOR_TYPE_FORCE,     /**< 力传感器         */
    EOS_SENSOR_TYPE_BAT,       /**< 电池电量传感器    */
    _EOS_SENSOR_MAX            /**< 传感器最大的枚举值 */
} eos_sensor_type_t;

/**
 * @brief 加速度传感器数据
 */
typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} eos_acce_data_t;

/**
 * @brief 陀螺仪数据
 */
typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} eos_gyro_data_t;

/**
 * @brief 磁力计数据
 */
typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} eos_mag_data_t;

/**
 * @brief 温度传感器数据
 */
typedef struct {
    int32_t temp;       /**< 单位: 0.01 °C  */
} eos_temp_data_t;

/**
 * @brief 湿度传感器数据
 */
typedef struct {
    uint32_t humidity;   /**< 单位: 0.01 %RH */
} eos_humi_data_t;

/**
 * @brief 气压传感器数据
 */
typedef struct {
    int32_t pressure;   /**< 单位: 0.01 hPa */
} eos_baro_data_t;

/**
 * @brief 光照传感器数据
 */
typedef struct {
    uint32_t lux;       /**< 单位: lx */
} eos_light_data_t;

/**
 * @brief 距离传感器数据
 */
typedef struct {
    uint16_t distance_mm;
} eos_proximity_data_t;

/**
 * @brief 心率传感器数据
 */
typedef struct {
    uint16_t heart_rate;    /**< 单位: bpm */
    uint16_t spo2;          /**< 单位: 0.01% */
} eos_heart_rate_data_t;

/**
 * @brief TVOC传感器数据
 */
typedef struct {
    uint16_t tvoc;     /**< 单位: ppb */
} eos_tvoc_data_t;

/**
 * @brief 噪声传感器数据
 */
typedef struct {
    uint16_t noise_db;  /**< 单位: 0.01 dB */
} eos_noise_data_t;

/**
 * @brief 计步传感器数据
 */
typedef struct {
    uint32_t steps;
} eos_step_data_t;

/**
 * @brief 力传感器数据
 */
typedef struct {
    uint32_t force;       /**< 单位: 0.01 牛顿 */
} eos_force_data_t;

/**
 * @brief 电池电量传感器数据
 */
typedef struct {
    uint8_t level;     /**< 单位: % */
    bool charging;  /**< 充电状态: false-未充电,true-充电中 */
} eos_bat_data_t;

/**
 * @brief 传感器数据共用体
 */
typedef union
{
    eos_acce_data_t acce;
    eos_gyro_data_t gyro;
    eos_mag_data_t mag;
    eos_temp_data_t temp;
    eos_humi_data_t humi;
    eos_baro_data_t baro;
    eos_light_data_t light;
    eos_proximity_data_t proximity;
    eos_heart_rate_data_t hr;
    eos_tvoc_data_t tvoc;
    eos_noise_data_t noise;
    eos_step_data_t step;
    eos_force_data_t force;
    eos_bat_data_t bat;
}eos_sensor_data_t;

typedef uint8_t eos_sensor_id_t;

/**
 * @brief 传感器结构体
 */
typedef struct
{
    eos_sensor_id_t id;
    eos_sensor_type_t type;
    eos_sensor_data_t data;
}eos_sensor_t;

typedef void (*eos_sensor_cb_t)(eos_sensor_t, void *);

/* Public function prototypes --------------------------------*/
/**
 * @brief 上报传感器数据
 * @param sensor 传感器指针
 */
void eos_sensor_report(eos_sensor_t *sensor);
/**
 * @brief 根据传感器类型获取第一个传感器实例
 * @param type 传感器类型
 * @return eos_sensor_t* 获取成功则返回传感器指针，否则返回 NULL
 */
eos_sensor_t *eos_sensor_get(eos_sensor_type_t type);
/**
 * @brief 根据类型和索引查找传感器实例
 * @param type 传感器类型
 * @param index 传感器数值
 * @return eos_sensor_t* 查找成功则返回传感器指针，否则返回 NULL
 */
eos_sensor_t *eos_sensor_find(eos_sensor_type_t type, uint8_t index);
/**
 * @brief 注册传感器实例（自动分配ID）
 * @param type 传感器类型
 * @return eos_sensor_t* 注册成功则返回传感器指针，否则返回 NULL
 */
eos_sensor_t *eos_sensor_register(eos_sensor_type_t type);
/**
 * @brief 获取某类型传感器数量
 * @param type 传感器类型
 * @return uint8_t 数量
 */
uint8_t eos_sensor_get_count(eos_sensor_type_t type);
/**
 * @brief 获取传感器事件代码
 * @param type 传感器类型
 * @return eos_event_code_t 事件代码
 */
eos_event_code_t eos_sensor_get_event_code(eos_sensor_type_t type);
/**
 * @brief 通过事件代码获取传感器类型
 * @param code 事件代码
 * @return eos_sensor_type_t 传感器类型
 */
eos_sensor_type_t eos_sensor_get_type_by_event_code(eos_event_code_t code);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_SENSOR_H */
