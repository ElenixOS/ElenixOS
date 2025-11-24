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
#include "elena_os_fs_port.h"
#include "elena_os_mem_port.h"
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

#define EOS_TIMEOUT_INFINITE UINT32_MAX

/* Public typedefs --------------------------------------------*/
typedef struct eos_sem_t eos_sem_t;   /**< 信号量 */
/* Public function prototypes --------------------------------*/

/**
 * @brief 创建一个信号量
 * @param initial_count 初始计数
 * @param max_count 最大计数
 * @return 指向信号量对象的指针，如果失败返回 NULL
 */
eos_sem_t* eos_sem_create(uint32_t initial_count, uint32_t max_count);
/**
 * @brief 销毁信号量
 */
void eos_sem_destroy(eos_sem_t* sem);
/**
 * @brief 等待信号量
 * @param timeout_ms 超时时间（毫秒），0 表示非阻塞，EOS_TIMEOUT_INFINITE 表示无限等待
 * @return true 成功获取
 * @return false 超时或失败
 */
bool eos_sem_take(eos_sem_t* sem, uint32_t timeout_ms);
/**
 * @brief 释放信号量
 */
void eos_sem_give(eos_sem_t* sem);
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
/**
 * @brief 定位手机
 *
 * 通过蓝牙或其他方式让手机响铃来定位手机。
 */
void eos_locate_phone(void);
/**
 * @brief 设置扬声器音量
 * @param volume 音量
 */
void eos_speaker_set_volume(uint8_t volume);
/**
 * @brief 系统进入休眠模式（低功耗状态）
 *
 * 此函数内需要完成：
 *
 * - 停止触摸
 *
 * - 关闭屏幕
 *
 * - 关闭其他外设
 *
 * - CPU 降频、进入低功耗
 */
void eos_sys_sleep(void);
/**
 * @brief 系统进入屏幕常亮模式
 *
 * 此函数内需要完成：
 *
 * - 停止触摸
 *
 * - 关闭其他外设
 *
 * - CPU 降频、进入低功耗
 */
void eos_sys_aod(void);
/**
 * @brief 系统退出休眠模式
 *
 * 此函数内需要完成：
 *
 * - 唤醒`eos_sys_sleep()`关闭的外设，关闭低功耗模式
 */
void eos_sys_wake(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_PORT_H */
