
/**
 * @file script_engine_core.c
 * @brief Script engine core functionality implementation
 */

#include "script_engine_core.h"

/* Includes ---------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define EOS_LOG_TAG "ScriptEngine"
#include "eos_log.h"

#include "lvgl.h"
#include "cJSON.h"

#include "eos_port.h"
#include "eos_misc.h"
#include "eos_icon.h"
#include "eos_watchface.h"
#include "eos_app.h"
#include "eos_config.h"
#include "eos_service_storage.h"
#include "eos_mem.h"
#include "eos_event.h"
#include "eos_cqueue.h"

#include "sni.h"

/* Macros and Definitions -------------------------------------*/
#define SCRIPT_INIT_FLAGS JERRY_INIT_MEM_STATS
#define SCRIPT_DEFAULT_CQUEUE_CAPACITY 10
#define SCRIPT_ERROR_STACK_BUF_SIZE 256

/**
 * @brief Module task structure
 */
typedef struct
{
    jerry_value_t specifier;
    jerry_value_t user_value;
    jerry_value_t promise;
} module_task_t;

/**
 * @brief Script engine context structure
 */
typedef struct
{
    script_state_t state;         /**< Current state */
    script_pkg_t *current_script; /**< Currently running script */
    jerry_value_t old_realm;      /**< Old realm */
    bool initialized;             /**< Whether engine is initialized */
    char *error_info;             /**< Last run error information */
    const char *base_path;        /**< Script base path */
} script_engine_context_t;

/* Variables --------------------------------------------------*/
static script_engine_context_t engine_ctx = {
    .state = SCRIPT_STATE_STOPPED,
    .current_script = NULL,
    .initialized = false,
    .base_path = NULL};

static eos_cqueue_t *g_module_queue = NULL;

static void _cleanup_module_task(module_task_t *task)
{
    if (!task)
    {
        return;
    }

    jerry_value_free(task->specifier);
    jerry_value_free(task->user_value);
    jerry_value_free(task->promise);
    eos_free(task);
}

/* Function Implementations -----------------------------------*/

static jerry_value_t _module_import_cb(const jerry_value_t specifier,
                                       const jerry_value_t user_value,
                                       void *user_p)
{
    // 如果队列不存在，创建一个
    if (!g_module_queue)
    {
        g_module_queue = eos_cqueue_create(SCRIPT_DEFAULT_CQUEUE_CAPACITY);
        if (!g_module_queue)
        {
            EOS_LOG_E("Failed to create module queue");
            return jerry_throw_sz(JERRY_ERROR_COMMON, "Failed to create module queue");
        }
    }

    // 分配模块任务
    module_task_t *task = eos_malloc_zeroed(sizeof(module_task_t));
    if (!task)
    {
        EOS_LOG_E("Failed to allocate module task");
        return jerry_throw_sz(JERRY_ERROR_COMMON, "Failed to allocate module task");
    }

    task->specifier = jerry_value_copy(specifier);
    task->user_value = jerry_value_copy(user_value);

    jerry_value_t promise = jerry_promise();
    task->promise = jerry_value_copy(promise);

    // 将任务加入队列
    if (!eos_cqueue_enqueue(g_module_queue, task))
    {
        EOS_LOG_E("Failed to enqueue module task");
        jerry_value_free(task->specifier);
        jerry_value_free(task->user_value);
        jerry_value_free(task->promise);
        eos_free(task);
        return jerry_throw_sz(JERRY_ERROR_COMMON, "Failed to enqueue module task");
    }

    return promise;
}

static jerry_value_t _module_resolve_cb(const jerry_value_t specifier,
                        const jerry_value_t referrer,
                        void *user_p)
{
    jerry_char_t specifier_buffer[256];
    jerry_size_t copied_bytes = jerry_string_to_buffer(specifier, JERRY_ENCODING_UTF8,
                                                       (jerry_char_t *)specifier_buffer,
                                                       sizeof(specifier_buffer) - 1);
    specifier_buffer[copied_bytes] = '\0';

    EOS_LOG_D("Resolving dependency: %s", (const char *)specifier_buffer);

    char full_path[512];

    if (specifier_buffer[0] == '.' && (specifier_buffer[1] == '/' || specifier_buffer[1] == '\\'))
    {
        // 相对路径，使用脚本的基础路径
        const char *referrer_path = engine_ctx.base_path ? engine_ctx.base_path : "/";
        snprintf(full_path, sizeof(full_path), "%s%s", referrer_path, specifier_buffer + 2);
    }
    else
    {
        // 绝对路径或模块名，直接使用
        snprintf(full_path, sizeof(full_path), "%s", (const char *)specifier_buffer);
    }

    EOS_LOG_D("Full path: %s", full_path);

    // 读取模块文件内容
    eos_file_t file = eos_fs_open_read(full_path);
    if (file == EOS_FILE_INVALID)
    {
        EOS_LOG_E("Failed to read dependency: %s", full_path);
        return jerry_throw_sz(JERRY_ERROR_COMMON, "Failed to read dependency");
    }

    // 获取文件大小
    uint32_t file_size;
    if (eos_fs_size(file, &file_size) != 0)
    {
        EOS_LOG_E("Failed to get file size: %s", full_path);
        eos_fs_close(file);
        return jerry_throw_sz(JERRY_ERROR_COMMON, "Failed to get file size");
    }

    // 分配内存并读取文件内容
    jerry_char_t *source_p = eos_malloc(file_size + 1);
    if (!source_p)
    {
        EOS_LOG_E("Failed to allocate memory for file content: %s", full_path);
        eos_fs_close(file);
        return jerry_throw_sz(JERRY_ERROR_COMMON, "Failed to allocate memory");
    }

    int bytes_read = eos_fs_read(file, source_p, file_size);
    eos_fs_close(file);

    if (bytes_read != (int)file_size)
    {
        EOS_LOG_E("Failed to read file content: %s", full_path);
        eos_free(source_p);
        return jerry_throw_sz(JERRY_ERROR_COMMON, "Failed to read file content");
    }

    source_p[file_size] = '\0';

    jerry_parse_options_t parse_options;
    parse_options.options = JERRY_PARSE_MODULE | JERRY_PARSE_HAS_SOURCE_NAME | JERRY_PARSE_HAS_USER_VALUE;
    parse_options.source_name = jerry_string_sz(full_path);
    parse_options.user_value = jerry_string_sz(engine_ctx.base_path);

    jerry_value_t result = jerry_parse(source_p, file_size, &parse_options);

    jerry_value_free(parse_options.source_name);
    jerry_value_free(parse_options.user_value);
    eos_free(source_p);

    return result;
}

static jerry_value_t _read_and_parse_module(const char *file_path)
{
    // 读取模块文件内容
    eos_file_t file = eos_fs_open_read(file_path);
    if (file == EOS_FILE_INVALID)
    {
        EOS_LOG_E("Failed to read module file: %s", file_path);
        return jerry_throw_sz(JERRY_ERROR_COMMON, "Failed to read module file");
    }

    // 获取文件大小
    uint32_t file_size;
    if (eos_fs_size(file, &file_size) != 0)
    {
        EOS_LOG_E("Failed to get file size: %s", file_path);
        eos_fs_close(file);
        return jerry_throw_sz(JERRY_ERROR_COMMON, "Failed to get file size");
    }

    // 分配内存并读取文件内容
    jerry_char_t *source_p = eos_malloc(file_size + 1);
    if (!source_p)
    {
        EOS_LOG_E("Failed to allocate memory for file content: %s", file_path);
        eos_fs_close(file);
        return jerry_throw_sz(JERRY_ERROR_COMMON, "Failed to allocate memory");
    }

    int bytes_read = eos_fs_read(file, source_p, file_size);
    eos_fs_close(file);

    if (bytes_read != (int)file_size)
    {
        EOS_LOG_E("Failed to read file content: %s", file_path);
        eos_free(source_p);
        return jerry_throw_sz(JERRY_ERROR_COMMON, "Failed to read file content");
    }

    source_p[file_size] = '\0';

    jerry_parse_options_t parse_options;
    parse_options.options = JERRY_PARSE_MODULE | JERRY_PARSE_HAS_SOURCE_NAME | JERRY_PARSE_HAS_USER_VALUE;
    parse_options.source_name = jerry_string_sz(file_path);
    parse_options.user_value = jerry_string_sz(engine_ctx.base_path);

    jerry_value_t result = jerry_parse(source_p, file_size, &parse_options);

    jerry_value_free(parse_options.source_name);
    jerry_value_free(parse_options.user_value);
    eos_free(source_p);

    return result;
}

static void _process_module_queue(void)
{
    if (!g_module_queue)
    {
        EOS_LOG_D("Module queue is empty");
        return;
    }

    size_t task_count = eos_cqueue_get_size(g_module_queue);
    EOS_LOG_I("Processing %zu module(s)...", task_count);

    while (eos_cqueue_get_size(g_module_queue) > 0)
    {
        module_task_t *task = (module_task_t *)eos_cqueue_dequeue(g_module_queue);
        if (!task)
        {
            continue;
        }

        jerry_char_t specifier_buffer[256];
        jerry_size_t copied_bytes = jerry_string_to_buffer(task->specifier, JERRY_ENCODING_UTF8,
                                                           (jerry_char_t *)specifier_buffer,
                                                           sizeof(specifier_buffer) - 1);
        specifier_buffer[copied_bytes] = '\0';

        EOS_LOG_D("Loading module: %s", (const char *)specifier_buffer);

        jerry_value_t module_value = _read_and_parse_module((const char *)specifier_buffer);

        if (jerry_value_is_exception(module_value))
        {
            EOS_LOG_E("Failed to parse module: %s", (const char *)specifier_buffer);
            jerry_value_free(module_value);
            _cleanup_module_task(task);
            continue;
        }

        jerry_value_t link_result = jerry_module_link(module_value, _module_resolve_cb, NULL);
        if (jerry_value_is_exception(link_result))
        {
            EOS_LOG_E("Failed to link module: %s", (const char *)specifier_buffer);
            jerry_value_free(link_result);
            jerry_value_free(module_value);
            _cleanup_module_task(task);
            continue;
        }
        jerry_value_free(link_result);

        jerry_value_t eval_result = jerry_module_evaluate(module_value);
        if (jerry_value_is_exception(eval_result))
        {
            EOS_LOG_E("Failed to evaluate module: %s", (const char *)specifier_buffer);
            jerry_value_free(eval_result);
            jerry_value_free(module_value);
            _cleanup_module_task(task);
            continue;
        }
        jerry_value_free(eval_result);

        jerry_value_t namespace_value = jerry_module_namespace(module_value);
        jerry_value_t resolve_result = jerry_promise_resolve(task->promise, namespace_value);

        if (jerry_value_is_exception(resolve_result))
        {
            EOS_LOG_E("Failed to resolve promise for module: %s", (const char *)specifier_buffer);
            jerry_value_free(resolve_result);
        }
        else
        {
            jerry_value_free(resolve_result);
        }

        jerry_value_free(namespace_value);
        jerry_value_free(module_value);

        // 清理任务资源
        _cleanup_module_task(task);
    }
}

static void _cleanup_module_queue(void)
{
    if (!g_module_queue)
    {
        return;
    }

    while (eos_cqueue_get_size(g_module_queue) > 0)
    {
        module_task_t *task = (module_task_t *)eos_cqueue_dequeue(g_module_queue);
        if (task)
        {
            jerry_value_free(task->specifier);
            jerry_value_free(task->user_value);
            jerry_value_free(task->promise);
            eos_free(task);
        }
    }

    eos_cqueue_destroy(g_module_queue);
    g_module_queue = NULL;
}

jerry_value_t script_engine_throw_error(const char *message)
{
    jerry_value_t error_obj = jerry_error_sz(JERRY_ERROR_TYPE, (const jerry_char_t *)message);
    return jerry_throw_value(error_obj, true);
}

static void _set_error_info(const char *msg)
{
    if (engine_ctx.error_info)
    {
        eos_free(engine_ctx.error_info);
        engine_ctx.error_info = NULL;
    }

    if (!msg)
    {
        return;
    }

    size_t len = strlen(msg);
    engine_ctx.error_info = (char *)eos_malloc(len + 1);

    if (engine_ctx.error_info)
    {
        memcpy(engine_ctx.error_info, msg, len + 1);
    }
}

const char *script_engine_get_error_info(void)
{
    return engine_ctx.error_info ? engine_ctx.error_info : "";
}

static void _clear_error_info(void)
{
    if (engine_ctx.error_info)
    {
        eos_free(engine_ctx.error_info);
        engine_ctx.error_info = NULL;
    }
}

static void _script_pkg_destroy(script_pkg_t *pkg)
{
    if (!pkg)
    {
        return;
    }

    if (pkg->base_path)
    {
        eos_free((void *)pkg->base_path);
        pkg->base_path = NULL;
    }

    eos_pkg_free(pkg);
    eos_free(pkg);
}

static script_pkg_t *_script_pkg_clone(const script_pkg_t *source)
{
    if (!source)
    {
        return NULL;
    }

    script_pkg_t *copy = eos_malloc_zeroed(sizeof(script_pkg_t));
    if (!copy)
    {
        return NULL;
    }

    copy->type = source->type;
    copy->id = eos_strdup(source->id);
    copy->name = eos_strdup(source->name);
    copy->version = eos_strdup(source->version);
    copy->author = eos_strdup(source->author);
    copy->description = eos_strdup(source->description);
    copy->script_str = eos_strdup(source->script_str);
    copy->base_path = eos_strdup(source->base_path);

    if ((source->id && !copy->id) ||
        (source->name && !copy->name) ||
        (source->version && !copy->version) ||
        (source->author && !copy->author) ||
        (source->description && !copy->description) ||
        (source->script_str && !copy->script_str) ||
        (source->base_path && !copy->base_path))
    {
        _script_pkg_destroy(copy);
        return NULL;
    }

    return copy;
}

#if EOS_COMPILE_MODE == DEUBG

static char *_state_get_enum_str(script_state_t state)
{
    switch (state)
    {
    case SCRIPT_STATE_STOPPED:
        return "SCRIPT_STATE_STOPPED";
    case SCRIPT_STATE_RUNNING:
        return "SCRIPT_STATE_RUNNING";
    case SCRIPT_STATE_SUSPEND:
        return "SCRIPT_STATE_SUSPEND";
    case SCRIPT_STATE_STOPPING:
        return "SCRIPT_STATE_STOPPING";
    case SCRIPT_STATE_ERROR:
        return "SCRIPT_STATE_ERROR";
    default:
        break;
    }
}

#endif /* EOS_COMPILE_MODE */

/**
 * @brief State transition function
 */
static script_engine_result_t _change_state(script_state_t new_state)
{
    // State transition validation
    switch (engine_ctx.state)
    {
    case SCRIPT_STATE_STOPPED:
        if (new_state != SCRIPT_STATE_RUNNING &&
            new_state != SCRIPT_STATE_ERROR)
        {
            EOS_LOG_E("Invalid state transition from STOPPED to %d", new_state);
            return -SE_ERR_INVALID_STATE;
        }
        break;

    case SCRIPT_STATE_RUNNING:
        if (new_state != SCRIPT_STATE_SUSPEND &&
            new_state != SCRIPT_STATE_STOPPING &&
            new_state != SCRIPT_STATE_ERROR)
        {
            EOS_LOG_E("Invalid state transition from RUNNING to %d", new_state);
            return -SE_ERR_INVALID_STATE;
        }
        break;

    case SCRIPT_STATE_SUSPEND:
        if (new_state != SCRIPT_STATE_STOPPED &&
            new_state != SCRIPT_STATE_RUNNING &&
            new_state != SCRIPT_STATE_ERROR)
        {
            EOS_LOG_E("Invalid state transition from SUSPEND to %d", new_state);
            return -SE_ERR_INVALID_STATE;
        }
        break;

    case SCRIPT_STATE_STOPPING:
        if (new_state != SCRIPT_STATE_STOPPED &&
            new_state != SCRIPT_STATE_ERROR)
        {
            EOS_LOG_E("Invalid state transition from STOPPING to %d", new_state);
            return -SE_ERR_INVALID_STATE;
        }
        break;

    case SCRIPT_STATE_ERROR:
        if (new_state != SCRIPT_STATE_STOPPED)
        {
            EOS_LOG_E("Invalid state transition from ERROR to %d", new_state);
            return -SE_ERR_INVALID_STATE;
        }
        break;

    default:
        break;
    }
#if EOS_COMPILE_MODE == DEUBG
    EOS_LOG_D("State change: %s -> %s", _state_get_enum_str(engine_ctx.state), _state_get_enum_str(new_state));
#endif /* EOS_COMPILE_MODE */
    engine_ctx.state = new_state;
    switch (new_state)
    {
    case SCRIPT_STATE_STOPPED:
        eos_event_broadcast(EOS_EVENT_SCRIPT_EXITED, NULL);
        break;

    default:
        break;
    }
    return SE_OK;
}

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

static void _collect_script_garbage(void)
{
    /* Run an aggressive pass after script-exit hooks so native handles owned by
     * the previous realm are reclaimed before the next app launch. */
    jerry_heap_gc(JERRY_GC_PRESSURE_HIGH);
    jerry_heap_gc(JERRY_GC_PRESSURE_LOW);
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
 * @brief Parse JS error variable and print error reason
 */
static void _script_engine_exception_handler(const char *tag, jerry_value_t result)
{
    jerry_value_t value = jerry_exception_value(result, false);
    jerry_value_t final_str_val = value;
    char stack_buf[SCRIPT_ERROR_STACK_BUF_SIZE];
    char *buf = stack_buf;
    bool need_free = false;

    // If not string, convert to string
    if (!(jerry_value_is_string(value)))
    {
        final_str_val = jerry_value_to_string(value);
    }
    // Get string length
    jerry_size_t req_sz = jerry_string_size(final_str_val, JERRY_ENCODING_CESU8);
    if (req_sz > 0)
    {
        if (req_sz >= sizeof(stack_buf))
        {
            buf = (char *)eos_malloc(req_sz + 1);
            need_free = (buf != NULL);
        }

        if (buf)
        {
            jerry_string_to_buffer(final_str_val, JERRY_ENCODING_CESU8,
                                   (jerry_char_t *)buf, req_sz);
            buf[req_sz] = '\0';
            EOS_LOG_E("[%s]%s", tag, buf);
            _set_error_info(buf);

            if (need_free)
            {
                eos_free(buf);
            }
        }
        else
        {
            EOS_LOG_E("malloc failed");
            _set_error_info("malloc failed");
        }
    }
    else
    {
        EOS_LOG_E("Unknown error");
        _set_error_info("Unknown error");
    }

    jerry_value_free(final_str_val);
    jerry_value_free(value);
}
/**
 * @brief Convert script_pkg_t to JS object (for JS to access script_info)
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

    char *manifest_json = eos_storage_read_file(manifest_path);
    if (!manifest_json)
    {
        EOS_LOG_E("Read manifest.json failed");
        return -SE_FAILED;
    }

    cJSON *root = cJSON_Parse(manifest_json);
    eos_free(manifest_json);
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

void script_engine_register_functions(jerry_value_t parent,
                                      const script_engine_func_entry_t *entries,
                                      size_t funcs_count)
{
    for (size_t i = 0; i < funcs_count; ++i)
    {
        const char *class_name = entries[i].class_name;
        const char *method_name = entries[i].method_name;

        EOS_ASSERT(method_name != NULL);

        jerry_value_t target_obj = parent;

        // 如果指定了 class_name，则获取或创建 class 对象
        if (class_name != NULL)
        {
            jerry_value_t class_key = jerry_string_sz(class_name);
            jerry_value_t class_obj = jerry_object_get(parent, class_key);

            if (jerry_value_is_undefined(class_obj))
            {
                // class 不存在，创建并挂到 parent
                jerry_value_free(class_obj);
                class_obj = jerry_object();
                jerry_value_free(
                    jerry_object_set(parent, class_key, class_obj));
            }

            jerry_value_free(class_key);
            target_obj = class_obj; // 方法挂到 class 下
        }

        // 创建函数
        jerry_value_t fn = jerry_function_external(entries[i].handler);
        jerry_value_t method_key = jerry_string_sz(method_name);

        jerry_value_free(
            jerry_object_set(target_obj, method_key, fn));

        // 释放
        jerry_value_free(method_key);
        jerry_value_free(fn);

        if (class_name != NULL)
        {
            jerry_value_free(target_obj);
        }
    }
}

/**
 * @brief VM execution stop callback
 */
static jerry_value_t _vm_exec_stop_callback(void *user_p)
{
    (void)user_p;

    if (engine_ctx.state == SCRIPT_STATE_STOPPING)
    {
        EOS_LOG_D("Script execution stopped by request");
        return jerry_string_sz("Script terminated by request");
    }

    return jerry_undefined();
}

script_state_t script_engine_get_state(void)
{
    return engine_ctx.state;
}

char *script_engine_get_current_script_id(void)
{
    return engine_ctx.current_script ? engine_ctx.current_script->id : NULL;
}

char *script_engine_get_current_script_name(void)
{
    return engine_ctx.current_script ? engine_ctx.current_script->name : NULL;
}

script_pkg_type_t script_engine_get_current_script_type(void)
{
    return engine_ctx.current_script ? engine_ctx.current_script->type : SCRIPT_TYPE_UNKNOWN;
}

/**
 * @brief Clean up engine resources
 */
static void _engine_cleanup(void)
{
    if (jerry_value_is_object(engine_ctx.old_realm))
    {
        jerry_value_free(jerry_set_realm(engine_ctx.old_realm));
        engine_ctx.old_realm = jerry_undefined();
    }

    jerry_heap_gc(JERRY_GC_PRESSURE_LOW);
}

/**
 * @brief Stop and clean up script
 */
static script_engine_result_t _script_engine_stop_and_cleanup(void)
{
    // Clean up resources
    _engine_cleanup();

    // Free script package
    if (engine_ctx.current_script)
    {
        _script_pkg_destroy(engine_ctx.current_script);
        engine_ctx.current_script = NULL;
    }
    engine_ctx.base_path = NULL;

    _change_state(SCRIPT_STATE_STOPPED);
    _collect_script_garbage();
    _check_mem();
    EOS_LOG_I("Script terminated successfully");

    return SE_OK;
}

script_engine_result_t script_engine_request_stop(void)
{
    EOS_LOG_I("Request stop script");
    switch (engine_ctx.state)
    {
    case SCRIPT_STATE_STOPPED:
        return SE_OK;

    case SCRIPT_STATE_RUNNING:
        // 请求停止，等待状态转换
        _change_state(SCRIPT_STATE_STOPPING);

        // 等待脚本停止
        for (int timeout = 0; timeout < 100; timeout++)
        {
            if (engine_ctx.state != SCRIPT_STATE_STOPPING)
            {
                break;
            }
        }

        // 如果还在STOPPING状态，强制清理
        if (engine_ctx.state == SCRIPT_STATE_STOPPING)
        {
            EOS_LOG_W("Force stopping script due to timeout");
            return _script_engine_stop_and_cleanup();
        }

        // 正常停止流程
        if (engine_ctx.state == SCRIPT_STATE_STOPPED)
        {
            return SE_OK;
        }
        break;

    case SCRIPT_STATE_STOPPING:
        EOS_LOG_W("Script is already stopping, waiting for completion");
        for (int timeout = 0; timeout < 100; timeout++)
        {
            if (engine_ctx.state != SCRIPT_STATE_STOPPING)
            {
                break;
            }
        }

        if (engine_ctx.state == SCRIPT_STATE_STOPPING)
        {
            EOS_LOG_W("Force stopping script from STOPPING state due to timeout");
            return _script_engine_stop_and_cleanup();
        }

        if (engine_ctx.state == SCRIPT_STATE_STOPPED)
        {
            return SE_OK;
        }
        break;

    case SCRIPT_STATE_SUSPEND:
        EOS_LOG_D("Stopping from SUSPEND state");
        return _script_engine_stop_and_cleanup();

    case SCRIPT_STATE_ERROR:
        EOS_LOG_D("Cleaning up from ERROR state");
        return _script_engine_stop_and_cleanup();

    default:
        EOS_LOG_W("Cannot stop from current state: %d", engine_ctx.state);
        return -SE_ERR_INVALID_STATE;
    }

    return SE_OK;
}

script_engine_result_t script_engine_init(void)
{
    if (engine_ctx.initialized)
    {
        EOS_LOG_E("Script engine already initialized");
        return SE_ERR_ALREADY_INITIALIZED;
    }

    if (!jerry_feature_enabled(JERRY_FEATURE_VM_EXEC_STOP) ||
        !jerry_feature_enabled(JERRY_FEATURE_REALM) ||
        !jerry_feature_enabled(JERRY_FEATURE_MODULE))
    {
        EOS_LOG_E("Required JerryScript features not enabled");
        return -SE_ERR_JERRY_INIT_FAIL;
    }

    if (!lv_is_initialized())
    {
        EOS_LOG_E("LVGL not initialized");
        return -SE_ERR_NOT_INITIALIZED;
    }

    // 初始化 JerryScript VM
    jerry_init(SCRIPT_INIT_FLAGS);

    // 注册函数和初始化
    sni_init();

    engine_ctx.initialized = true;
    EOS_LOG_I("Script engine initialized successfully");
    _check_mem();

    return SE_OK;
}

script_engine_result_t script_engine_run(const script_pkg_t *script_package)
{
    if (!script_package || !script_package->script_str)
    {
        return -SE_ERR_NULL_PACKAGE;
    }

    if (!engine_ctx.initialized)
    {
        EOS_LOG_E("Script engine not initialized");
        return -SE_ERR_NOT_INITIALIZED;
    }

    // 检查当前状态是否允许运行新脚本
    if (engine_ctx.state != SCRIPT_STATE_STOPPED &&
        engine_ctx.state != SCRIPT_STATE_ERROR)
    {
        EOS_LOG_E("Cannot run script in current state: %d", engine_ctx.state);
        return -SE_ERR_INVALID_STATE;
    }

    script_pkg_t *owned_script = _script_pkg_clone(script_package);
    if (!owned_script)
    {
        EOS_LOG_E("Failed to clone script package");
        return -SE_ERR_MALLOC;
    }

    // 设置当前脚本
    engine_ctx.current_script = owned_script;
    _change_state(SCRIPT_STATE_RUNNING);

    // 设置基础路径
    engine_ctx.base_path = engine_ctx.current_script->base_path ? engine_ctx.current_script->base_path : "/";

    // 创建新realm
    jerry_value_t new_realm = jerry_realm();
    engine_ctx.old_realm = jerry_set_realm(new_realm);

    // 挂载 API
    sni_mount(new_realm);

    // 设置停止回调和日志
    jerry_halt_handler(16, _vm_exec_stop_callback, NULL);
    jerry_log_set_level(JERRY_LOG_LEVEL_DEBUG);

    // 设置全局script_info变量
    jerry_value_t global = jerry_current_realm();
    jerry_value_t script_info = _script_engine_create_info(engine_ctx.current_script);
    jerry_value_t key = jerry_string_sz("scriptInfo");
    jerry_value_free(jerry_object_set(global, key, script_info));
    jerry_value_free(key);
    jerry_value_free(script_info);
    jerry_value_free(global);

    // 注册模块导入回调
    jerry_module_on_import(_module_import_cb, NULL);

    // 执行脚本
    jerry_parse_options_t parse_options;
    parse_options.options = JERRY_PARSE_MODULE | JERRY_PARSE_HAS_SOURCE_NAME | JERRY_PARSE_HAS_USER_VALUE;
    parse_options.source_name = jerry_string_sz("main.js");
    parse_options.user_value = jerry_string_sz(engine_ctx.base_path);

    jerry_value_t parsed_code = jerry_parse(
        (const jerry_char_t *)engine_ctx.current_script->script_str,
        strlen(engine_ctx.current_script->script_str),
        &parse_options);

    jerry_value_free(parse_options.source_name);
    jerry_value_free(parse_options.user_value);

    // 清理脚本字符串
    eos_free((void *)engine_ctx.current_script->script_str);
    engine_ctx.current_script->script_str = NULL;

    script_engine_result_t result = SE_OK;

    if (!jerry_value_is_exception(parsed_code))
    {
        // 开始链接模块
        EOS_LOG_I("Linking module");
        jerry_value_t link_result = jerry_module_link(parsed_code, _module_resolve_cb, NULL);

        if (jerry_value_is_exception(link_result))
        {
            EOS_LOG_E("Failed to link module");
            _script_engine_exception_handler("Module Link", link_result);
            _change_state(SCRIPT_STATE_ERROR);
            jerry_value_free(link_result);
            result = -SE_ERR_JERRY_EXCEPTION;
        }
        else
        {
            jerry_value_free(link_result);

            EOS_LOG_I("Evaluating module");
            jerry_value_t run_result = jerry_module_evaluate(parsed_code);

            if (jerry_value_is_exception(run_result))
            {
                if (engine_ctx.state == SCRIPT_STATE_STOPPING)
                {
                    // 请求停止导致的异常，正常处理
                    EOS_LOG_D("Script stopped by request");
                    result = SE_OK;
                }
                else
                {
                    // 执行出错
                    _script_engine_exception_handler("Script Runtime", run_result);
                    _change_state(SCRIPT_STATE_ERROR);
                    result = -SE_ERR_JERRY_EXCEPTION;
                }
            }
            else
            {
                // 执行成功
                if (engine_ctx.state == SCRIPT_STATE_RUNNING)
                {
                    _change_state(SCRIPT_STATE_SUSPEND);
                }
                eos_event_broadcast(EOS_EVENT_SCRIPT_STARTED, NULL);
                result = SE_OK;
            }

            jerry_value_free(run_result);
        }
    }
    else
    {
        // 解析出错
        _script_engine_exception_handler("Script Parse", parsed_code);
        _change_state(SCRIPT_STATE_ERROR);
        result = -SE_ERR_INVALID_JS;
    }

    jerry_value_free(parsed_code);

    // 处理模块队列
    _process_module_queue();

    // 运行 promise jobs
    EOS_LOG_D("Running promise jobs...");
    jerry_value_free(jerry_run_jobs());

    // 清理模块队列
    _cleanup_module_queue();
    jerry_module_cleanup(jerry_undefined());

    // 如果执行出错，清理资源
    if (result != SE_OK && engine_ctx.state == SCRIPT_STATE_ERROR)
    {
        _engine_cleanup();
        if (engine_ctx.current_script)
        {
            _script_pkg_destroy(engine_ctx.current_script);
            engine_ctx.current_script = NULL;
        }
        engine_ctx.base_path = NULL;
        _change_state(SCRIPT_STATE_STOPPED);
        _collect_script_garbage();
    }

    return result;
}

script_engine_result_t script_engine_reload_current_script(void)
{
    if (!engine_ctx.initialized)
    {
        EOS_LOG_E("Script engine not initialized");
        return -SE_ERR_NOT_INITIALIZED;
    }

    if (!engine_ctx.current_script)
    {
        EOS_LOG_E("No running script to reload");
        return -SE_ERR_SCRIPT_NOT_RUNNING;
    }

    if (!(engine_ctx.current_script->id && engine_ctx.current_script->base_path))
    {
        EOS_LOG_E("Current script metadata is incomplete");
        return -SE_FAILED;
    }

    const char *base_path = eos_strdup(engine_ctx.current_script->base_path);
    if (!base_path)
    {
        return -SE_ERR_MALLOC;
    }

    script_state_t state = script_engine_get_state();
    if (state == SCRIPT_STATE_RUNNING ||
        state == SCRIPT_STATE_SUSPEND ||
        state == SCRIPT_STATE_ERROR)
    {
        script_engine_result_t stop_ret = script_engine_request_stop();
        if (stop_ret != SE_OK && script_engine_get_state() != SCRIPT_STATE_STOPPED)
        {
            EOS_LOG_E("Failed to stop current script before reload: %d", stop_ret);
            eos_free((void *)base_path);
            return stop_ret;
        }
    }

    script_pkg_t pkg = {0};
    pkg.type = engine_ctx.current_script->type;

    const char *manifest_file_name = NULL;
    const char *entry_file_name = NULL;
    switch (pkg.type)
    {
    case SCRIPT_TYPE_APPLICATION:
        manifest_file_name = EOS_APP_MANIFEST_FILE_NAME;
        entry_file_name = EOS_APP_SCRIPT_ENTRY_FILE_NAME;
        break;
    case SCRIPT_TYPE_WATCHFACE:
        manifest_file_name = EOS_WATCHFACE_MANIFEST_FILE_NAME;
        entry_file_name = EOS_WATCHFACE_SCRIPT_ENTRY_FILE_NAME;
        break;
    default:
        EOS_LOG_E("Current script type does not support reload: %d", pkg.type);
        eos_free((void *)base_path);
        return -SE_ERR_INVALID_STATE;
    }

    char manifest_path[PATH_MAX];
    snprintf(manifest_path, sizeof(manifest_path), "%s%s", base_path, manifest_file_name);
    if (script_engine_get_manifest(manifest_path, &pkg) != SE_OK)
    {
        EOS_LOG_E("Read manifest failed: %s", manifest_path);
        eos_free((void *)base_path);
        return -SE_FAILED;
    }

    char script_path[PATH_MAX];
    snprintf(script_path, sizeof(script_path), "%s%s", base_path, entry_file_name);
    pkg.base_path = base_path;

    if (!eos_storage_is_file(script_path))
    {
        EOS_LOG_E("Can't find script: %s", script_path);
        eos_pkg_free(&pkg);
        return -SE_FAILED;
    }

    pkg.script_str = eos_storage_read_file(script_path);
    if (!pkg.script_str)
    {
        EOS_LOG_E("Failed to read script: %s", script_path);
        eos_pkg_free(&pkg);
        return -SE_FAILED;
    }

    script_engine_result_t run_ret = script_engine_run(&pkg);
    eos_pkg_free(&pkg);

    return run_ret;
}

script_engine_result_t script_engine_reload_current_app(void)
{
    return script_engine_reload_current_script();
}

script_engine_result_t script_engine_clean_up(void)
{
    if (engine_ctx.state != SCRIPT_STATE_STOPPED)
    {
        script_engine_request_stop();
    }

    // 清理模块队列
    _cleanup_module_queue();
    jerry_module_cleanup(jerry_undefined());

    jerry_cleanup();
    engine_ctx.initialized = false;
    _change_state(SCRIPT_STATE_STOPPED);

    return SE_OK;
}
