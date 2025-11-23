/**
 * @file elena_os_dispatcher.h
 * @brief 任务调度器
 * @author Sab1e
 * @date 2025-11-10
 */

#ifndef ELENA_OS_DISPATCHER_H
#define ELENA_OS_DISPATCHER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/
typedef void (*eos_dispatcher_cb_t)(void *);     /**< 回调定义 */
/* Public function prototypes --------------------------------*/
/**
 * @brief 初始化任务调度器（必须在 GUI 线程启动前或初始化阶段调用）
 */
void eos_dispatcher_init(void);
/**
 * @brief 将回调加入回调队列
 * @param cb 回调函数
 * @param user_data 用户数据
 * @note 从任意线程或 ISR 调用，尽量快地返回。
 */
void eos_dispatcher_call(eos_dispatcher_cb_t cb, void *user_data);
/**
 * @brief 主处理程序，在 GUI 线程循环中定期调用以执行所有回调
 */
void eos_dispatch_tick(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_DISPATCHER_H */
