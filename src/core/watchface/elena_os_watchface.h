/**
 * @file elena_os_watchface.h
 * @brief 表盘
 * @author Sab1e
 * @date 2025-08-22
 * @details
 * 系统初始化时需要调用`eos_watchface_init()`加载系统中的表盘数据，以便获取表盘列表。
 * 
 * 系统启动后需要调用`eos_watchface_create()`加载表盘。
 * 当位于其他页面时，如果想返回表盘页面，
 * 尽量使用`eos_watchface_create_async()`异步加载表盘。
 * 
 * 当表盘页面初次加载时自动生成 Screen。
 * 当再次调用`eos_watchface_create()`就会删除已有的 Screen，
 * 创建新的 Screen，并在这个 Screen 上加载表盘。
 * 
 * @note
 * 可以通过
 * ```c
 * lv_screen_active() == eos_watchface_get_screen()
 * ```
 * 判断当前是否位于表盘页面
 */

#ifndef ELENA_OS_WATCHFACE_H
#define ELENA_OS_WATCHFACE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "elena_os_core.h"
#include "elena_os_sys.h"
#include "lvgl.h"
/* Public macros ----------------------------------------------*/
#define EOS_WATCHFACE_DIR EOS_SYS_DIR "wf/"
#define EOS_WATCHFACE_INSTALLED_DIR EOS_WATCHFACE_DIR "faces/"
#define EOS_WATCHFACE_DATA_DIR EOS_WATCHFACE_DIR "wf_data/"
#define EOS_WATCHFACE_MANIFEST_FILE_NAME "manifest.json"
#define EOS_WATCHFACE_SNAPSHOT_FILE_NAME "snapshot.bin"
#define EOS_WATCHFACE_SCRIPT_ENTRY_FILE_NAME "main.js"
/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

/**
 * @brief 获取当前已安装的表盘数量，即列表大小
 * @return size_t 列表大小
 */
size_t eos_watchface_list_size(void);
/**
 * @brief 根据索引值获取表盘的 id
 * @param index 索引值（0基）
 * @return const char* id 字符串
 */
const char* eos_watchface_list_get_id(size_t index);
/**
 * @brief 判断指定 id 字符串的代码是否存在于表盘列表中
 * @param watchface_id id 字符串
 * @return true 
 * @return false 
 */
bool eos_watchface_list_contains(const char* watchface_id);
/**
 * @brief 安装表盘
 * @param eapk_path eapk 安装包路径
 * @return eos_result_t 安装结果
 */
eos_result_t eos_watchface_install(const char *eapk_path);
/**
 * @brief 卸载表盘
 * @param watchface_id 表盘 id
 * @return eos_result_t 卸载结果
 */
eos_result_t eos_watchface_uninstall(const char *watchface_id);
/**
 * @brief 创建表盘页面
 */
void eos_watchface_create(void);
/**
 * @brief 异步创建表盘页面
 */
void eos_watchface_create_async(void);
/**
 * @brief 初始化表盘系统
 * @return eos_result_t 初始化结果
 */
eos_result_t eos_watchface_init(void);
/**
 * @brief 获取表盘的 Screen 对象
 * @return lv_obj_t* Screen 对象
 */
lv_obj_t *eos_watchface_get_screen(void);
/**
 * @brief 删除表盘页面
 */
void eos_watchface_delete(void);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_WATCHFACE_H */
