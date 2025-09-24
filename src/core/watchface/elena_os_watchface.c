/**
 * @file elena_os_watchface.c
 * @brief 表盘
 * @author Sab1e
 * @date 2025-08-22
 */

#include "elena_os_watchface.h"

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include "elena_os_misc.h"
#include "elena_os_port.h"
#include "elena_os_log.h"
#include "elena_os_pkg_mgr.h"
#include "script_engine_core.h"
#include "elena_os_msg_list.h"
#include "elena_os_watchface_list.h"
#include "elena_os_theme.h"
#include "elena_os_control_center.h"
// Macros and Definitions
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
// Variables
static eos_watchface_list_t watchface_list;
static bool _is_watchface_initialized = false;
static lv_obj_t *watchface_screen = NULL;
// Function Implementations

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
    list->data = malloc(capacity * sizeof(char *));
    list->size = 0;
    list->capacity = capacity;
}

void _eos_watchface_list_add(eos_watchface_list_t *list, const char *id)
{
    if (list->size == list->capacity)
    {
        list->capacity *= 2;
        list->data = realloc(list->data, list->capacity * sizeof(char *));
    }
    list->data[list->size] = eos_strdup(id); // 复制字符串
    list->size++;
}

void _eos_watchface_list_free(eos_watchface_list_t *list)
{
    for (size_t i = 0; i < list->size; i++)
    {
        free(list->data[i]);
    }
    free(list->data);
}

eos_result_t _eos_watchface_list_get_installed()
{
    DIR *dir;
    struct dirent *entry;

    // 打开应用程序安装目录
    dir = opendir(EOS_WATCHFACE_INSTALLED_DIR);
    if (!dir)
    {
        EOS_LOG_E("Failed to open watchface directory: %s", EOS_WATCHFACE_INSTALLED_DIR);
        return EOS_FAILED;
    }

    // 遍历目录中的所有条目
    while ((entry = readdir(dir)) != NULL)
    {
        // 跳过 "." 和 ".." 目录
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        // 构建完整路径
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), EOS_WATCHFACE_INSTALLED_DIR "%s", entry->d_name);

        // 检查是否为目录
        if (eos_is_dir(full_path))
        {
            EOS_LOG_D("Found installed watchface: %s", entry->d_name);
            // 添加到应用程序列表
            _eos_watchface_list_add(&watchface_list, entry->d_name);
        }
    }

    closedir(dir);
    EOS_LOG_I("Loaded %zu installed watchfaces", watchface_list.size);
    return EOS_OK;
}

eos_result_t _eos_watchface_list_refresh()
{
    memcpy(&watchface_list, 0, sizeof(watchface_list));
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
        eos_rm_recursive(path);
    }
    // 创建应用名称的文件夹
    if (mkdir(path, 0755) == 0)
    {
        EOS_LOG_I("Created dir: %s\n", path);
    }
    else
    {
        if (errno != EEXIST)
        {
            EOS_LOG_E("mkdir");
            return -EOS_ERR_FILE_ERROR;
        }
    }
    if (!eos_is_dir(data_path))
    {
        if (mkdir(data_path, 0755) == 0)
        {
            EOS_LOG_I("Created dir: %s\n", data_path);
        }
        else
        {
            if (errno != EEXIST)
            {
                EOS_LOG_E("mkdir");
                return -EOS_ERR_FILE_ERROR;
            }
        }
    }
    // 安装应用程序
    script_pkg_type_t type = SCRIPT_TYPE_WATCHFACE;
    eos_result_t ret = eos_pkg_mgr_unpack(eapk_path, path, type);
    if (ret != EOS_OK)
    {
        EOS_LOG_E("Watchface unpack failed. Code: %d", ret);
        eos_rm_recursive(path);
        return EOS_FAILED;
    }
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

    eos_result_t ret = eos_rm_recursive(path);

    if (ret != EOS_OK)
    {
        EOS_LOG_E("Uninstall failed, code: %d", ret);
        return EOS_FAILED;
    }

    // 清理应用数据
    if (eos_is_dir(data_path))
    {
        ret = eos_rm_recursive(path);
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

void eos_watchface_delete(void)
{
    if (script_engine_get_state() != SCRIPT_STATE_STOPPED)
    {
        EOS_LOG_D("Request Stop");
        script_engine_request_stop();
        lv_obj_clean(watchface_screen);
    }
}

static void _watchface_long_pressed_cb(lv_event_t *e)
{
    eos_watchface_delete();
    eos_watchface_list_create_async();
}

lv_obj_t *eos_watchface_get_screen(void)
{
    return watchface_screen;
}

EOS_DECLARE_SCREEN_ASYNC(eos_watchface_create)
{
    if (watchface_screen)
    {
        // 如果不为空则清空
        lv_obj_del(watchface_screen);
    }
    watchface_screen = lv_obj_create(NULL);
    lv_obj_add_style(watchface_screen, eos_theme_get_screen_style(), 0);
    lv_screen_load(watchface_screen);
    // JSON中获取表盘id
    EOS_LOG_D("Loading wf_id");
    const char *wf_id = eos_sys_cfg_get_string(EOS_SYS_CFG_KEY_WATCHFACE_ID, "cn.sab1e.clock");
    EOS_CHECK_PTR_RETURN(wf_id);
    // 直接通过表盘id 获取相关信息并存储到script_package
    EOS_LOG_D("Loading manifest");
    char manifest_path[PATH_MAX];
    snprintf(manifest_path, sizeof(manifest_path), EOS_WATCHFACE_INSTALLED_DIR "%s/" EOS_WATCHFACE_MANIFEST_FILE_NAME,
             wf_id);
    script_pkg_t *pkg = malloc(sizeof(script_pkg_t));
    memset((void *)pkg, 0, sizeof(script_pkg_t));
    pkg->type = SCRIPT_TYPE_WATCHFACE;
    EOS_LOG_D("script_engine_get_manifest");
    if (script_engine_get_manifest(manifest_path, pkg) != SE_OK)
    {
        EOS_LOG_E("Read manifest failed: %s", manifest_path);
        return;
    }
    EOS_LOG_D("App Info:\n"
              "id=%s | name=%s | version=%s |\n"
              "author:%s | description:%s",
              pkg->id, pkg->name, pkg->version,
              pkg->author, pkg->description);
    char script_path[PATH_MAX];
    snprintf(script_path, sizeof(script_path), EOS_WATCHFACE_INSTALLED_DIR "%s/" EOS_WATCHFACE_SCRIPT_ENTRY_FILE_NAME,
             wf_id);
    free((void *)wf_id);
    if (!eos_is_file(script_path))
    {
        EOS_LOG_E("Can't find script: %s", script_path);
        return;
    }
    pkg->script_str = eos_read_file(script_path);

    // 设置下拉面板
    msg_list_t *msg_list = eos_msg_list_create(watchface_screen);
    if (!msg_list)
    {
        EOS_LOG_E("Create msg_list failed");
        return;
    }
    // 设置上拉面板
    eos_control_center_create(watchface_screen);
    // 设置长按回调 进入 watchface list 使用普通 nav 导航
    lv_obj_add_event_cb(watchface_screen, _watchface_long_pressed_cb, LV_EVENT_LONG_PRESSED, NULL);
    // 正式运行表盘脚本（脚本禁止阻塞线程）
    script_engine_result_t ret = script_engine_run(pkg);
    if (ret != SE_OK)
    {
        EOS_LOG_E("Script encounter a fatal error");
    }
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

    return EOS_OK;
}
