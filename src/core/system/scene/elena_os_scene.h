/**
 * @file elena_os_scene.h
 * @brief 场景系统
 * @author Sab1e
 * @date 2025-11-11
 * @details
 *
 * # Scene
 *
 * ## 简述
 *
 * Scene（场景）是系统中用于表示一组相关界面状态的抽象单元。
 * 每个 Scene 管理一个或多个 LVGL screen，并负责它们的创建、显示与销毁逻辑。
 * 系统在任意时刻只会处于一个活动 Scene 中，通过场景切换机制可在不同模块间平滑过渡。
 */

#ifndef ELENA_OS_SCENE_H
#define ELENA_OS_SCENE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "elena_os_config.h"

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/
/**
 * @brief 场景定义
 */
typedef enum
{
    EOS_SCENE_UNKNOWN = 0,
#if EOS_SYSTEM_MODE == TEST_MODE
    EOS_SCENE_TEST,
#endif /* EOS_SYSTEM_MODE */
    EOS_SCENE_WATCHFACE,
    EOS_SCENE_APP_LIST,
    EOS_SCENE_WATCHFACE_LIST,
    EOS_SCENE_NAVIGATION,
} eos_scene_type_t;

typedef void (*eos_scene_entry_t)(void);
typedef void (*eos_scene_exit_t)(void);

typedef struct
{
    eos_scene_type_t type;     /**< 场景类型      */
    eos_scene_entry_t entry;   /**< 场景入口      */
    eos_scene_exit_t exit;     /**< 离开场景时调用 */
} eos_scene_t;


/* Public function prototypes --------------------------------*/

/**
 * @brief 判断当前场景过程与指定是否一致
 * @param current_type 当前场景类型
 * @param last_type 之前场景类型
 * @return true
 * @return false
 */
bool eos_secne_is_equal(eos_scene_type_t current_type, eos_scene_type_t last_type);
/**
 * @brief 获取上一个场景类型
 */
eos_scene_type_t eos_scene_get_last_type(void);
/**
 * @brief 获取当前的场景类型
 */
eos_scene_type_t eos_scene_get_current_type(void);
/**
 * @brief 初始化场景系统
 * @param watchface_entry 表盘入口函数
 * @param watchface_exit 表盘场景退出回调
 * @param app_list_entry 应用列表入口函数
 * @param watchface_list_entry 表盘列表入口函数
 */
void eos_scene_init(
    eos_scene_entry_t watchface_entry,
    eos_scene_exit_t watchface_exit,
    eos_scene_entry_t app_list_entry,
    eos_scene_entry_t watchface_list_entry);
/**
 * @brief 按照内部设定好的顺序自动切换场景
 */
void eos_scene_auto_switch(void);
/**
 * @brief 切入指定场景
 * @param type 场景类型
 */
void eos_scene_switch(eos_scene_type_t type);
/**
 * @brief 返回上一级场景
 * @note 只能返回一级
 */
void eos_scene_back(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_SCENE_H */
