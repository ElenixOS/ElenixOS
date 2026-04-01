/**
 * @file elena_os_watchface.c
 * @brief 表盘
 * @author Sab1e
 * @date 2025-08-22
 */

#include "elena_os_watchface.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elena_os_misc.h"
#include "elena_os_port.h"
#define EOS_LOG_TAG "Watchface"
#include "elena_os_log.h"
#include "elena_os_pkg_mgr.h"
#include "script_engine_core.h"
#include "elena_os_msg_list.h"
#include "elena_os_watchface_list.h"
#include "elena_os_theme.h"
#include "elena_os_control_center.h"
#include "elena_os_basic_widgets.h"
#include "elena_os_fs.h"
#include "elena_os_mem.h"
#include "elena_os_icon.h"
#include "elena_os_std_widgets.h"
#include "elena_os_ww_clock_hand.h"
#include "elena_os_activity.h"
/* Macros and Definitions -------------------------------------*/
#define EOS_WATCHFACE_LIST_DEFAULT_CAPACITY 1
/**
 * @brief 应用结构体
 */
typedef script_pkg_t eos_watchface_t;

typedef struct
{
    char **data;
    size_t size;
    size_t capacity;
} eos_watchface_list_t;
/* Variables --------------------------------------------------*/
static eos_watchface_list_t watchface_list;
static bool _is_watchface_initialized = false;

static void eos_watchface_on_enter(eos_activity_t *a);
static void eos_watchface_on_pause(eos_activity_t *a);
static void eos_watchface_on_resume(eos_activity_t *a);
static void eos_watchface_on_destroy(eos_activity_t *a);
static void _watchface_stop_and_reset(eos_activity_t *a);

static const eos_activity_lifecycle_t watchface_lifecycle = {
    .on_enter = eos_watchface_on_enter,
    .on_pause = eos_watchface_on_pause,
    .on_resume = eos_watchface_on_resume,
    .on_destroy = eos_watchface_on_destroy,
};
/* Function Implementations -----------------------------------*/

size_t eos_watchface_list_size(void)
{
    return watchface_list.size;
}

const char *eos_watchface_list_get_id(size_t index)
{
    if (index >= watchface_list.size)
    {
        EOS_LOG_E("Index out of bounds: %zu", index);
        return NULL;
    }
    return watchface_list.data[index];
}

bool eos_watchface_list_contains(const char *watchface_id)
{
    for (size_t i = 0; i < watchface_list.size; i++)
    {
        if (strcmp(watchface_list.data[i], watchface_id) == 0)
        {
            return true;
        }
    }
    return false;
}

void _eos_watchface_list_init(eos_watchface_list_t *list, size_t capacity)
{
    list->data = eos_malloc(capacity * sizeof(char *));
    list->size = 0;
    list->capacity = capacity;
}

void _eos_watchface_list_add(eos_watchface_list_t *list, const char *id)
{
    if (list->size == list->capacity)
    {
        list->capacity *= 2;
        list->data = eos_realloc(list->data, list->capacity * sizeof(char *));
    }
    list->data[list->size] = eos_strdup(id); // 复制字符串
    list->size++;
}

void _eos_watchface_list_free(eos_watchface_list_t *list)
{
    for (size_t i = 0; i < list->size; i++)
    {
        eos_free(list->data[i]);
    }
    eos_free(list->data);
}

eos_result_t _eos_watchface_list_get_installed()
{
    eos_dir_t dir;
    char name_buf[256];

    // 打开应用程序安装目录
    dir = eos_fs_opendir(EOS_WATCHFACE_INSTALLED_DIR);
    if (!dir)
    {
        EOS_LOG_E("Failed to open watchface directory: %s", EOS_WATCHFACE_INSTALLED_DIR);
        return EOS_FAILED;
    }

    // 遍历目录中的所有条目
    while (eos_fs_readdir(dir, name_buf, sizeof(name_buf)) == 0)
    {
        // 跳过 "." 和 ".." 目录
        if (strcmp(name_buf, ".") == 0 || strcmp(name_buf, "..") == 0)
        {
            continue;
        }

        // 构建完整路径
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), EOS_WATCHFACE_INSTALLED_DIR "%s", name_buf);

        // 检查是否为目录
        if (eos_is_dir(full_path))
        {
            EOS_LOG_D("Found installed watchface: %s", name_buf);
            // 添加到应用程序列表
            _eos_watchface_list_add(&watchface_list, name_buf);
        }
    }

    eos_fs_closedir(dir);
    EOS_LOG_I("Loaded %zu installed watchfaces", watchface_list.size);
    return EOS_OK;
}

eos_result_t _eos_watchface_list_refresh()
{
    memset(&watchface_list, 0, sizeof(watchface_list));
    _eos_watchface_list_init(&watchface_list, EOS_WATCHFACE_LIST_DEFAULT_CAPACITY);
    if (_eos_watchface_list_get_installed() != EOS_OK)
    {
        EOS_LOG_E("Get installed watchface failed");
        return EOS_FAILED;
    }
    return EOS_OK;
}

eos_result_t eos_watchface_install(const char *eapk_path)
{
    EOS_CHECK_PTR_RETURN_VAL(eapk_path, EOS_ERR_VAR_NULL);
    // 获取软件包头
    eos_pkg_header_t header;
    if (eos_pkg_read_header(eapk_path, &header) != EOS_OK)
    {
        EOS_LOG_E("Read header failed: %s", eapk_path);
        return EOS_FAILED;
    }
    if (!eos_is_valid_filename(header.pkg_id))
    {
        EOS_LOG_E("Invalid package id");
        return EOS_FAILED;
    }
    // 拼接路径
    char path[PATH_MAX];
    snprintf(path, sizeof(path), EOS_WATCHFACE_INSTALLED_DIR "%s", header.pkg_id);
    char data_path[PATH_MAX];
    snprintf(data_path, sizeof(data_path), EOS_WATCHFACE_DATA_DIR "%s", header.pkg_id);
    EOS_LOG_D("WATCHFACE_PATH: %s", path);
    // 检查应用是否存在
    if (eos_is_dir(path))
    {
        // 如果存在则删除
        eos_fs_rm_recursive(path);
    }
    // 创建应用名称的文件夹
    if (eos_fs_mkdir(path) == 0)
    {
        EOS_LOG_I("Created dir: %s\n", path);
    }
    else
    {
        return -EOS_ERR_FILE_ERROR;
    }
    // 安装应用程序
    script_pkg_type_t type = SCRIPT_TYPE_WATCHFACE;
    eos_result_t ret = eos_pkg_mgr_unpack(eapk_path, path, type);
    if (ret != EOS_OK)
    {
        EOS_LOG_E("Watchface unpack failed. Code: %d", ret);
        eos_fs_rm_recursive(path);
        return EOS_FAILED;
    }
    eos_fs_mkdir_if_not_exist(data_path);
    _eos_watchface_list_refresh();
    EOS_LOG_D("Watchface installed successfully: %s", header.pkg_name);
    return EOS_OK;
}

eos_result_t eos_watchface_uninstall(const char *watchface_id)
{
    // 卸载应用程序
    char path[PATH_MAX];
    snprintf(path, sizeof(path), EOS_WATCHFACE_INSTALLED_DIR "%s", watchface_id);
    char data_path[PATH_MAX];
    snprintf(data_path, sizeof(data_path), EOS_WATCHFACE_DATA_DIR "%s", watchface_id);
    if (!eos_is_dir(path))
    {
        EOS_LOG_E("Watchface does not esist: %s", watchface_id);
        return EOS_FAILED;
    }

    eos_result_t ret = eos_fs_rm_recursive(path);

    if (ret != EOS_OK)
    {
        EOS_LOG_E("Uninstall failed, code: %d", ret);
        return EOS_FAILED;
    }

    // 清理应用数据
    if (eos_is_dir(data_path))
    {
        ret = eos_fs_rm_recursive(path);
    }

    if (ret != EOS_OK)
    {
        EOS_LOG_E("Uninstall failed, code: %d", ret);
        return EOS_FAILED;
    }
    _eos_watchface_list_refresh();
    EOS_LOG_D("Watchface uninstalled successfully: %s", watchface_id);
    return EOS_OK;
}

static void _watchface_long_pressed_cb(lv_event_t *e)
{
    eos_watchface_list_enter();
}

void eos_watchface_on_enter(eos_activity_t *a)
{
    EOS_LOG_I("Enter watchface activity");
    lv_obj_t *view = lv_obj_create(eos_activity_get_root_screen());
    lv_obj_remove_style_all(view);
    lv_obj_add_style(view, eos_theme_get_view_style(), 0);
    eos_activity_set_view(a, view);
    // JSON中获取表盘id
    char wf_id[EOS_PKG_ID_LEN_MAX];
    char *selected_wf_id = eos_sys_cfg_get_string(EOS_SYS_CFG_KEY_WATCHFACE_ID_STR, "cn.sab1e.clock");
    snprintf(wf_id, sizeof(wf_id),
             "%s",
             selected_wf_id);
    eos_free(selected_wf_id);
    // 直接通过表盘id 获取相关信息并存储到script_package
    char manifest_path[PATH_MAX];
    snprintf(manifest_path, sizeof(manifest_path),
             EOS_WATCHFACE_INSTALLED_DIR "%s/" EOS_WATCHFACE_MANIFEST_FILE_NAME,
             wf_id);
    script_pkg_t pkg = {0};
    pkg.type = SCRIPT_TYPE_WATCHFACE;
    if (script_engine_get_manifest(manifest_path, &pkg) != SE_OK)
    {
        EOS_LOG_E("Read manifest failed: %s", manifest_path);
        return;
    }
    char script_path[PATH_MAX];
    snprintf(script_path, sizeof(script_path),
             EOS_WATCHFACE_INSTALLED_DIR "%s/" EOS_WATCHFACE_SCRIPT_ENTRY_FILE_NAME,
             wf_id);

    // 设置脚本基础路径，用于解析相对路径的模块导入
    char base_path[PATH_MAX];
    snprintf(base_path, sizeof(base_path), EOS_WATCHFACE_INSTALLED_DIR "%s/", wf_id);
    pkg.base_path = eos_strdup(base_path);
    if (!eos_is_file(script_path))
    {
        EOS_LOG_E("Can't find script: %s", script_path);
        eos_pkg_free(&pkg);
        return;
    }
    pkg.script_str = eos_fs_read_file(script_path);
    // 设置长按回调 进入 watchface list
    lv_obj_add_event_cb(eos_activity_get_view(a), _watchface_long_pressed_cb, LV_EVENT_LONG_PRESSED, NULL);
    // 正式运行表盘脚本（脚本禁止阻塞线程）
    script_engine_result_t ret = script_engine_run(&pkg);
    if (ret != SE_OK)
    {
        // 显示错误信息
        lv_obj_t *list = eos_std_info_create(
            eos_activity_get_view(a),
            EOS_COLOR_RED,
            RI_BUG_LINE,
            eos_lang_get_str(STR_ID_WATCHFACE_RUN_ERR_TITLE),
            eos_lang_get_str(STR_ID_WATCHFACE_RUN_ERR));
        lv_obj_set_style_pad_top(list, 30, 0);
        char info_str[1024];
        snprintf(info_str, sizeof(info_str), "Code: %d\nWFID: %s\nError: %s", ret, wf_id, script_engine_get_error_info());
        lv_obj_t *err_label = eos_list_add_comment(list, info_str);
        lv_obj_t *btn = eos_button_create(list, eos_lang_get_str(STR_ID_WATCHFACE_SWITCH), _watchface_long_pressed_cb, NULL);
        EOS_LOG_E("Watchface encounter a fatal error");
    }

    eos_pkg_free(&pkg);
}

static void _watchface_stop_and_reset(eos_activity_t *a)
{
    EOS_CHECK_PTR_RETURN(a);

    if (script_engine_get_state() != SCRIPT_STATE_STOPPED)
    {
        if (script_engine_request_stop() != SE_OK)
        {
            EOS_LOG_E("Script engine request stop failed");
        }
    }

    if (eos_activity_get_view(a) != NULL)
    {
        eos_activity_set_view(a, NULL);
    }
}

static void eos_watchface_on_pause(eos_activity_t *a)
{
    EOS_LOG_I("Pause watchface activity");
    _watchface_stop_and_reset(a);
}

static void eos_watchface_on_resume(eos_activity_t *a)
{
    EOS_LOG_I("Resume watchface activity");
    eos_watchface_on_enter(a);
}

void eos_watchface_on_destroy(eos_activity_t *a)
{
    EOS_LOG_I("Exit watchface activity");
    _watchface_stop_and_reset(a);
}

eos_result_t eos_watchface_init(void)
{
    EOS_LOG_D("Init eos_watchface");
    if (!_is_watchface_initialized)
    {
        // 初始化 从文件系统中读取应用列表
        _eos_watchface_list_refresh();
        _is_watchface_initialized = true;
    }
    else
    {
        EOS_LOG_E("Watchface already initialized");
        return EOS_FAILED;
    }
    // 创建表盘 Activity
    eos_activity_t *watchface_activity =
        eos_activity_create(&watchface_lifecycle);
    if (!watchface_activity)
    {
        EOS_LOG_E("Create watchface activity failed");
        return EOS_FAILED;
    }
    if (eos_activity_controller_init(watchface_activity) != EOS_OK)
    {
        EOS_LOG_E("Init watchface activity controller failed");
        return EOS_FAILED;
    }

    return EOS_OK;
}
