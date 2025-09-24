/**
 * @file elena_os_port.h
 * @brief ElenaOS 移植
 * @author Sab1e
 * @date 2025-08-21
 */

#ifndef ELENA_OS_PORT_H
#define ELENA_OS_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "elena_os_core.h"
#include "elena_os_sensor.h"
/* Public macros ----------------------------------------------*/
/**
 * @brief 函数弱定义宏
 */
#ifdef __CC_ARM /* ARM Compiler */
    #define EOS_WEAK __weak
#elif defined(__IAR_SYSTEMS_ICC__) /* for IAR Compiler */
    #define EOS_WEAK __weak
#elif defined(__GNUC__) /* GNU GCC Compiler */
    #define EOS_WEAK __attribute__((weak))
#elif defined(__ADSPBLACKFIN__) /* for VisualDSP++ Compiler */
    #define EOS_WEAK __attribute__((weak))
#elif defined(_MSC_VER)
    #define EOS_WEAK
#elif defined(__TI_COMPILER_VERSION__)
    #define EOS_WEAK
#else
    #error not supported tool chain
#endif
/**
 * @brief UNUSED 宏
 */
#define EOS_UNUSED(x) (void)(x)

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

/**
 * @brief 内存分配函数
 * @param size 要分配的内存大小
 * @return void* 分配成功则返回内存地址，否则返回 NULL
 * @note 主要用于图片内存分配
 */
void *eos_malloc_large(size_t size);
/**
 * @brief 释放目标内存
 * @param ptr 目标指针
 */
void eos_free_large(void *ptr);
/**
 * @brief 延时指定时间（非阻塞）
 * @param ms 毫秒数
 */
void eos_delay(uint32_t ms);
/**
 * @brief 系统重置
 */
void eos_cpu_reset();
/**
 * @brief 启用蓝牙
 * @warning 为避免阻塞 UI 线程，禁止在此处初始化蓝牙协议栈等操作，此处只向其他线程发送蓝牙启动消息。
 * @note 创建线程也属于易阻塞线程的操作
 */
void eos_bluetooth_enable(void);
/**
 * @brief 关闭蓝牙
 */
void eos_bluetooth_disable(void);
/**
 * @brief 获取当前时间结构体
 * @return eos_datetime_t 时间结构体
 * @note 推荐使用RTC获取时间
 * @warning 请自行同步时间，确保获取的是准确时间
 */
eos_datetime_t eos_time_get(void);
/**
 * @brief 设置屏幕亮度
 * @param brightness 亮度值（0~100）
 */
void eos_display_set_brightness(uint8_t brightness);
/**
 * @brief 传感器开始读取数据
 * @param type 传感器类型
 */
EOS_WEAK void eos_sensor_read(eos_sensor_type_t type);
/**
 * @brief 传感器停止读取数据
 * @param type 传感器类型
 */
EOS_WEAK void eos_sensor_stop(eos_sensor_type_t type);
/**
 * @brief 传感器添加读取数据的回调函数
 * @param type 传感器类型
 * @param cb 读取成功时返回数据的回调（用于更新UI）
 */
EOS_WEAK void eos_sensor_add_callback(eos_sensor_type_t type, eos_sensor_cb_t cb, void* user_data);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_PORT_H */
