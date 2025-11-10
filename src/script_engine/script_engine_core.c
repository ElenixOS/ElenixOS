
/**
 * @file script_engine_core.c
 * @brief 脚本引擎核心功能实现
 * @author Sab1e
 * @date 2025-07-26
 * @details
 *
 * # 脚本引擎
 *
 * ## 启动流程
 *
 * 1. 系统启动时需要调用`script_engine_init`初始化脚本引擎；
 * 1. 脚本启动时，会创建一个新的`realm`提供沙盒进行隔离；
 * 1. 新的`realm`会自动注册所有函数和符号到`script_engine_eos_obj`；
 * 1. 在脚本内使用`eos.*`访问函数和符号；
 *
 * ## 脚本使用方法
 *
 * 脚本内直接调用LVGL的函数绘制UI即可，绘制完成后无需进行任何操作，
 * 由系统内部调用`lv_timer_handler`执行渲染操作。
 *
 * 如果想关闭脚本，使用`script_engine_request_stop();`。
 *
 * ## 脚本状态说明
 *
 * ### SCRIPT_STATE_RUNNING
 *
 * 脚本正在运行，例如正在执行`eos.lv_label_create(lv_screen_active());`。
 *
 * ### SCRIPT_STATE_SUSPEND
 *
 * 一般来说绘制完成后，脚本进入挂起状态`SCRIPT_STATE_SUSPEND`，
 * 此时如果外部触发回调，可以正常调用。
 *
 * ### SCRIPT_STATE_STOPPED
 *
 * 脚本未启动以及脚本已关闭即为此状态，此时脚本的相关资源都已经被清理，沙盒已经被删除，
 * 不会再调用任何脚本内注册的回调。
 *
 * ## 脚本使用注意事项
 *
 * 1. 脚本中禁止使用死循环，否则会阻塞UI。
 *
 */

#include "script_engine_core.h"

/* Includes ---------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include <stdatomic.h>
#include <stdlib.h>

#include "lvgl.h"

#include "cJSON.h"

#include "lv_bindings.h"
#include "lv_bindings_misc.h"

#include "script_engine_native_func.h"
#include "elena_os_port.h"
#include "elena_os_log.h"
#include "elena_os_misc.h"
#include "elena_os_nav.h"
#include "elena_os_icon.h"
#include "elena_os_watchface.h"

/* Macros and Definitions -------------------------------------*/
#define SCRIPT_EOS_OBJ_KEY "eos"
#define SCRIPT_INIT_FLAGS JERRY_INIT_MEM_STATS
/* Variables --------------------------------------------------*/
static atomic_bool should_terminate = ATOMIC_VAR_INIT(false); /**< 请求终止脚本标志位 */
static script_state_t script_state = SCRIPT_STATE_STOPPED;
static bool is_terminated_by_req = false;
static script_pkg_t *current_script_pkg = NULL; /**< 保存指针以便清理内存 */
static bool script_engine_initialized = false;
jerry_value_t script_engine_eos_obj; /**< 通过此对象访问到所有已注册的函数 */
jerry_value_t old_realm;
/* Function Implementations -----------------------------------*/

static void _check_mem(void)
{
    if (!jerry_feature_enabled(JERRY_FEATURE_HEAP_STATS))
    {
        EOS_LOG_E("Feature not enabled: JERRY_FEATURE_HEAP_STATS");
        return;
    }
    jerry_heap_stats_t stats = {0};
    bool get_stats_ret = jerry_heap_stats(&stats);
    if (get_stats_ret)
    {
        EOS_LOG_I("Heap total sz: %d\nAllocated bytes: %d\nPeak allocated bytes: %d\n",
                  stats.size, stats.allocated_bytes, stats.peak_allocated_bytes);
    }
}

void script_engine_set_script_state(script_state_t state)
{
    script_state = state;
}

inline void script_engine_set_prop_number(jerry_value_t obj,
                                          const char *prop_name,
                                          double value)
{
    jerry_value_t prop = jerry_string_sz(prop_name);
    jerry_value_t jerry_value = jerry_number(value);
    jerry_value_t ret = jerry_object_set(obj, prop, jerry_value);

    jerry_value_free(ret);
    jerry_value_free(jerry_value);
    jerry_value_free(prop);
}

inline void script_engine_set_prop_bool(jerry_value_t obj,
                                        const char *prop_name,
                                        bool value)
{
    jerry_value_t prop = jerry_string_sz(prop_name);
    jerry_value_t jerry_value = jerry_boolean(value);
    jerry_value_t ret = jerry_object_set(obj, prop, jerry_value);

    jerry_value_free(ret);
    jerry_value_free(jerry_value);
    jerry_value_free(prop);
}

inline void script_engine_set_prop_string(jerry_value_t obj,
                                          const char *prop_name,
                                          const char *value)
{
    jerry_value_t prop = jerry_string_sz(prop_name);
    jerry_value_t jerry_value = jerry_string_sz(value);
    jerry_value_t ret = jerry_object_set(obj, prop, jerry_value);

    jerry_value_free(ret);
    jerry_value_free(jerry_value);
    jerry_value_free(prop);
}

/**
 * @brief VM 终止运行回调
 */
static jerry_value_t _vm_exec_stop_callback(void *user_p)
{
    (void)user_p; // 不使用参数
    if (should_terminate)
    {
        atomic_store(&should_terminate, false);
        is_terminated_by_req = true;
        script_state = SCRIPT_STATE_STOPPED;
        EOS_LOG_D("Script execution stopped by request.\n");
        return jerry_string_sz("Script terminated by request");
    }

    return jerry_undefined();
}
/**
 * @brief 请求停止当前脚本运行
 */
void _request_script_termination(void)
{
    atomic_store(&should_terminate, true);
}

script_state_t script_engine_get_state(void)
{
    return script_state;
}

char *script_engine_get_current_script_id(void)
{
    return current_script_pkg == NULL ? NULL : current_script_pkg->id;
}

char *script_engine_get_current_script_name(void)
{
    return current_script_pkg == NULL ? NULL : current_script_pkg->name;
}

script_pkg_type_t script_engine_get_current_script_type(void)
{
    return current_script_pkg == NULL ? SCRIPT_TYPE_UNKNOWN : current_script_pkg->type;
}

void _engine_cleanup()
{
    if (jerry_value_is_object(old_realm))
    {
        jerry_value_free(jerry_set_realm(old_realm));
    }
    jerry_heap_gc(JERRY_GC_PRESSURE_LOW);
}

static script_engine_result_t _script_engine_cleanup_and_stop(void)
{
    // 删除根页面
    script_pkg_type_t type = script_engine_get_current_script_type();
    if (type == SCRIPT_TYPE_APPLICATION)
    {
        eos_result_t ret = eos_nav_clean_up();
        if (ret != EOS_OK)
        {
            EOS_LOG_E("Navigation clean up failed!");
            return SE_FAILED;
        }
    }

    // 清理资源
    _engine_cleanup();
    script_state = SCRIPT_STATE_STOPPED;
    eos_pkg_free(current_script_pkg);
    current_script_pkg = NULL;
    _check_mem();
    EOS_LOG_I("Script terminated");

    return SE_OK;
}

script_engine_result_t script_engine_request_stop(void)
{
    if (script_state == SCRIPT_STATE_RUNNING)
    {
        // 请求终止运行脚本
        _request_script_termination();

        // 等待停止
        while (script_state != SCRIPT_STATE_STOPPED)
        {
            eos_delay(10);
        }

        return _script_engine_cleanup_and_stop();
    }
    else if (script_state == SCRIPT_STATE_SUSPEND)
    {
        EOS_LOG_D("SUSPEND REQUEST STOP");
        return _script_engine_cleanup_and_stop();
    }

    return -SE_ERR_SCRIPT_NOT_RUNNING;
}
/**
 * @brief 解析js错误变量并打印错误原因
 */
static void _script_engine_exception_handler(const char *tag, jerry_value_t result)
{
    printf("[%s] Error: ", tag);
    jerry_value_t value = jerry_exception_value(result, false);
    jerry_char_t str_buf_p[256];

    jerry_size_t req_sz = jerry_string_size(value, JERRY_ENCODING_CESU8);

    if (req_sz <= 255)
    {
        if (req_sz <= 1)
        {
            printf("unknown error");
        }
        else
        {
            jerry_string_to_buffer(value, JERRY_ENCODING_CESU8, str_buf_p, req_sz);
            str_buf_p[req_sz] = '\0';
            printf("%s", (const char *)str_buf_p);
        }
    }
    else
    {
        printf("error: buffer isn't big enough");
    }
    printf("\r\n");
    jerry_value_free(value);
}
/**
 * @brief 把 script_pkg_t 转换成 JS 对象（供 JS 访问 script_info）
 */
jerry_value_t _script_engine_create_info(const script_pkg_t *script_package)
{
    jerry_value_t obj = jerry_object();

    script_engine_set_prop_string(obj, "id", script_package->id);
    script_engine_set_prop_string(obj, "name", script_package->name);
    script_engine_set_prop_string(obj, "version", script_package->version);
    script_engine_set_prop_string(obj, "author", script_package->author);
    script_engine_set_prop_string(obj, "description", script_package->description);

    return obj;
}

script_engine_result_t script_engine_get_manifest(const char *manifest_path, script_pkg_t *pkg)
{
    if (!manifest_path || !pkg)
    {
        EOS_LOG_E("Invalid manifest_path or pkg pointer");
        return -SE_ERR_NULL_PACKAGE;
    }

    char *manifest_json = eos_read_file(manifest_path);
    if (!manifest_json)
    {
        EOS_LOG_E("Read manifest.json failed");
        return -SE_FAILED;
    }

    cJSON *root = cJSON_Parse(manifest_json);
    eos_free_large(manifest_json);
    if (!root)
    {
        EOS_LOG_E("parse error: %s\n", cJSON_GetErrorPtr());
        return -SE_FAILED;
    }

    cJSON *id = cJSON_GetObjectItemCaseSensitive(root, "id");
    cJSON *name = cJSON_GetObjectItemCaseSensitive(root, "name");
    cJSON *version = cJSON_GetObjectItemCaseSensitive(root, "version");
    cJSON *author = cJSON_GetObjectItemCaseSensitive(root, "author");
    cJSON *description = cJSON_GetObjectItemCaseSensitive(root, "description");

    if (!cJSON_IsString(id) || !id->valuestring ||
        !cJSON_IsString(name) || !name->valuestring ||
        !cJSON_IsString(version) || !version->valuestring ||
        !cJSON_IsString(author) || !author->valuestring ||
        !cJSON_IsString(description) || !description->valuestring)
    {
        EOS_LOG_E("Manifest missing required fields");
        cJSON_Delete(root);
        return -SE_FAILED;
    }

    // 释放原有指针（如果有），防止内存泄漏
    if (pkg->id)
        eos_free((void *)pkg->id);
    if (pkg->name)
        eos_free((void *)pkg->name);
    if (pkg->version)
        eos_free((void *)pkg->version);
    if (pkg->author)
        eos_free((void *)pkg->author);
    if (pkg->description)
        eos_free((void *)pkg->description);

    // 分配并赋值
    pkg->id = eos_strdup(id->valuestring);
    pkg->name = eos_strdup(name->valuestring);
    pkg->version = eos_strdup(version->valuestring);
    pkg->author = eos_strdup(author->valuestring);
    pkg->description = eos_strdup(description->valuestring);

    cJSON_Delete(root);
    return SE_OK;
}

script_engine_result_t script_engine_clean_up(void)
{
    jerry_cleanup();
    script_state = SCRIPT_STATE_STOPPED;
}

script_engine_result_t script_engine_init(void)
{
    if (!script_engine_initialized)
    {
        if (!jerry_feature_enabled(JERRY_FEATURE_VM_EXEC_STOP))
        {
            EOS_LOG_E("JerryScript feature JERRY_FEATURE_VM_EXEC_STOP is not enabled\n");
            return -SE_ERR_JERRY_INIT_FAIL;
        }

        if (!jerry_feature_enabled(JERRY_FEATURE_REALM))
        {
            EOS_LOG_E("JerryScript feature JERRY_FEATURE_REALM is not enabled\n");
            return -SE_ERR_JERRY_INIT_FAIL;
        }

        // 检查 LVGL 是否已初始化
        if (!lv_is_initialized())
        {
            EOS_LOG_E("LVGL not initialized, please initialize it first.\n");
            return -SE_ERR_NOT_INITIALIZED;
        }

        // 初始化 JerryScript VM
        jerry_init(SCRIPT_INIT_FLAGS);

        script_engine_eos_obj = jerry_object();

        // 注册函数
        script_engine_register_natives();

        // 初始化 LVGL 绑定
        lv_binding_init();

        // 初始化图标
        eos_icon_register();

        script_engine_initialized = true;
        EOS_LOG_I("Script engine init successful");
        _check_mem();
        return SE_OK;
    }
    else
    {
        EOS_LOG_E("Script engine already initialized.");
        return SE_ERR_ALREADY_INITIALIZED;
    }
}

script_engine_result_t script_engine_run(script_pkg_t *script_package)
{
    if (script_package == NULL || script_package->script_str == NULL)
    {
        return -SE_ERR_NULL_PACKAGE;
    }

    if (!script_engine_initialized)
    {
        EOS_LOG_E("Script engine not initialized!");
        return -SE_ERR_NOT_INITIALIZED;
    }

    if (script_state == SCRIPT_STATE_RUNNING || script_state == SCRIPT_STATE_SUSPEND)
    {
        return -SE_ERR_ALREADY_RUNNING;
    }

    script_state = SCRIPT_STATE_RUNNING;
    current_script_pkg = script_package;
    atomic_store(&should_terminate, false);
    is_terminated_by_req = false;

    jerry_value_t new_realm = jerry_realm();
    old_realm = jerry_set_realm(new_realm);

    jerry_value_t key = jerry_string_sz(SCRIPT_EOS_OBJ_KEY);
    jerry_value_free(jerry_object_set(new_realm, key, script_engine_eos_obj));
    jerry_value_free(key);

    // 初始化停止回调
    jerry_halt_handler(16, _vm_exec_stop_callback, NULL);
    jerry_log_set_level(JERRY_LOG_LEVEL_DEBUG);

    // 设置全局 script_info 变量
    jerry_value_t global = jerry_current_realm();
    jerry_value_t script_info = _script_engine_create_info(script_package);

    key = jerry_string_sz("script_info");
    jerry_value_free(jerry_object_set(global, key, script_info));

    jerry_value_free(key);
    jerry_value_free(script_info);
    jerry_value_free(global);

    // 执行主 JS 脚本
    jerry_value_t parsed_code = jerry_parse(
        (const jerry_char_t *)script_package->script_str,
        strlen(script_package->script_str),
        JERRY_PARSE_NO_OPTS);
    // 清理脚本字符串
    eos_free_large((void *)script_package->script_str);
    script_package->script_str = NULL;
    if (!jerry_value_is_exception(parsed_code))
    {
        EOS_LOG_I("JerryScript run.");
        jerry_value_t result = jerry_run(parsed_code);
        // 检查是否执行成功
        if (jerry_value_is_exception(result) && !is_terminated_by_req)
        {
            // 执行出错
            _script_engine_exception_handler("Script Runtime", result);
            jerry_value_free(parsed_code);
            jerry_value_free(result);
            _engine_cleanup();
            script_state = SCRIPT_STATE_STOPPED;
            return -SE_ERR_JERRY_EXCEPTION;
        }
        else
        {
            // 执行成功
            jerry_value_free(parsed_code);
            jerry_value_free(result);
            script_state = SCRIPT_STATE_SUSPEND;
            return SE_OK;
        }
    }
    else
    {
        // 代码解析出错
        _script_engine_exception_handler("Script Parse", parsed_code);
        jerry_value_free(parsed_code);
        _engine_cleanup();
        script_state = SCRIPT_STATE_STOPPED;
        return -SE_ERR_INVALID_JS;
    }
}

void script_engine_register_functions(const script_engine_func_entry_t *entry, const size_t funcs_count)
{
    for (size_t i = 0; i < funcs_count; ++i)
    {
        jerry_value_t fn = jerry_function_external(entry[i].handler);
        jerry_value_t name = jerry_string_sz(entry[i].name);
        jerry_value_free(jerry_object_set(script_engine_eos_obj, name, fn));
        jerry_value_free(name);
        jerry_value_free(fn);
    }
}
