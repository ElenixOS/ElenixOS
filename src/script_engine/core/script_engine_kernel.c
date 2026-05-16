/**
 * @file script_engine_kernel.c
 * @brief Script engine kernel layer implementation (low-level JerryScript operations)
 */

#include "script_engine_kernel.h"

/* Includes ---------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define EOS_LOG_TAG "ScriptEngine"
#include "eos_log.h"

#include "cJSON.h"

#include "eos_port.h"
#include "eos_core.h"
#include "eos_service_storage.h"
#include "eos_mem.h"
#include "eos_pkg_mgr.h"
#include "eos_cqueue.h"
#include "eos_dispatcher.h"

#include "sni.h"

/* Macros and Definitions -------------------------------------*/
#define SCRIPT_INIT_FLAGS JERRY_INIT_MEM_STATS
#define SCRIPT_DEFAULT_CQUEUE_CAPACITY 10
#define SCRIPT_ERROR_STACK_BUF_SIZE 256
#define SCRIPT_BACKTRACE_MAX_FRAMES 16
#define SCRIPT_BACKTRACE_SOURCE_SIZE 128
#define SCRIPT_DEFAULT_TIMEOUT_MS 3000

typedef struct
{
    jerry_value_t specifier;
    jerry_value_t user_value;
    jerry_value_t promise;
} _module_task_t;

typedef struct
{
    script_state_t state;
    script_pkg_t *current_script;
    jerry_value_t script_realm;
    jerry_value_t old_realm;
    bool initialized;
    char *error_info;
    const char *base_path;
    script_error_location_t error_location;
    script_error_location_t backtrace[SCRIPT_BACKTRACE_MAX_FRAMES];
    uint32_t backtrace_count;
    uint32_t script_start_time;
    uint32_t script_timeout_ms;
    bool stop_is_timeout;
} _engine_context_t;

struct script_engine_saved_context_t
{
    jerry_value_t script_realm;
    jerry_value_t old_realm;
    script_pkg_t *script;
    char *base_path;
    uint32_t script_timeout_ms;
};

/* Variables --------------------------------------------------*/

static _engine_context_t engine_ctx = {
    .state = SCRIPT_STATE_STOPPED,
    .current_script = NULL,
    .initialized = false,
    .base_path = NULL,
    .script_start_time = 0,
    .script_timeout_ms = SCRIPT_DEFAULT_TIMEOUT_MS,
    .stop_is_timeout = false};

static eos_cqueue_t *_module_queue = NULL;

/* Function Implementations -----------------------------------*/

static void _populate_result_from_ctx(se_kernel_run_result_t *result)
{
    result->result = SE_ERR_JERRY_EXCEPTION;
    result->error_type = EOS_SCRIPT_FAULT_ERROR_EXCEPTION;
    result->error_info[0] = '\0';
    if (engine_ctx.error_info)
    {
        size_t len = strlen(engine_ctx.error_info);
        if (len > sizeof(result->error_info) - 1)
            len = sizeof(result->error_info) - 1;
        memcpy(result->error_info, engine_ctx.error_info, len);
        result->error_info[len] = '\0';
    }
    result->location = engine_ctx.error_location;
    result->backtrace_count = engine_ctx.backtrace_count;
    for (uint32_t i = 0; i < engine_ctx.backtrace_count && i < 16; i++)
        result->backtrace[i] = engine_ctx.backtrace[i];
}

/************************** Module helpers **************************/

static void _cleanup_module_task(_module_task_t *task)
{
    if (!task) return;
    jerry_value_free(task->specifier);
    jerry_value_free(task->user_value);
    jerry_value_free(task->promise);
    eos_free(task);
}

static jerry_value_t _module_import_cb(const jerry_value_t specifier,
                                       const jerry_value_t user_value,
                                       void *user_p)
{
    if (!_module_queue)
    {
        _module_queue = eos_cqueue_create(SCRIPT_DEFAULT_CQUEUE_CAPACITY);
        if (!_module_queue)
            return jerry_throw_sz(JERRY_ERROR_COMMON, "Failed to create module queue");
    }
    _module_task_t *task = eos_malloc_zeroed(sizeof(_module_task_t));
    if (!task)
        return jerry_throw_sz(JERRY_ERROR_COMMON, "Failed to allocate module task");
    task->specifier = jerry_value_copy(specifier);
    task->user_value = jerry_value_copy(user_value);
    jerry_value_t promise = jerry_promise();
    task->promise = jerry_value_copy(promise);
    if (!eos_cqueue_enqueue(_module_queue, task))
    {
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
    char full_path[512];
    if (specifier_buffer[0] == '.' && (specifier_buffer[1] == '/' || specifier_buffer[1] == '\\'))
    {
        const char *referrer_path = engine_ctx.base_path ? engine_ctx.base_path : "/";
        snprintf(full_path, sizeof(full_path), "%s%s", referrer_path, specifier_buffer + 2);
    }
    else
    {
        snprintf(full_path, sizeof(full_path), "%s", (const char *)specifier_buffer);
    }
    char *source_str = eos_storage_read_file(full_path);
    if (!source_str)
        return jerry_throw_sz(JERRY_ERROR_COMMON, "Failed to read dependency");
    jerry_size_t file_size = strlen(source_str);
    jerry_parse_options_t parse_options;
    parse_options.options = JERRY_PARSE_MODULE | JERRY_PARSE_HAS_SOURCE_NAME | JERRY_PARSE_HAS_USER_VALUE;
    parse_options.source_name = jerry_string_sz(full_path);
    parse_options.user_value = jerry_string_sz(engine_ctx.base_path);
    jerry_value_t result = jerry_parse((const jerry_char_t *)source_str, file_size, &parse_options);
    jerry_value_free(parse_options.source_name);
    jerry_value_free(parse_options.user_value);
    eos_free(source_str);
    return result;
}

static jerry_value_t _read_and_parse_module(const char *file_path)
{
    char *source_str = eos_storage_read_file(file_path);
    if (!source_str)
        return jerry_throw_sz(JERRY_ERROR_COMMON, "Failed to read module file");
    jerry_size_t file_size = strlen(source_str);
    jerry_parse_options_t parse_options;
    parse_options.options = JERRY_PARSE_MODULE | JERRY_PARSE_HAS_SOURCE_NAME | JERRY_PARSE_HAS_USER_VALUE;
    parse_options.source_name = jerry_string_sz(file_path);
    parse_options.user_value = jerry_string_sz(engine_ctx.base_path);
    jerry_value_t result = jerry_parse((const jerry_char_t *)source_str, file_size, &parse_options);
    jerry_value_free(parse_options.source_name);
    jerry_value_free(parse_options.user_value);
    eos_free(source_str);
    return result;
}

static void _process_module_queue(void)
{
    if (!_module_queue) return;
    while (eos_cqueue_get_size(_module_queue) > 0)
    {
        _module_task_t *task = (_module_task_t *)eos_cqueue_dequeue(_module_queue);
        if (!task) continue;
        jerry_char_t specifier_buffer[256];
        jerry_size_t copied_bytes = jerry_string_to_buffer(task->specifier, JERRY_ENCODING_UTF8,
                                                           (jerry_char_t *)specifier_buffer,
                                                           sizeof(specifier_buffer) - 1);
        specifier_buffer[copied_bytes] = '\0';
        jerry_value_t module_value = _read_and_parse_module((const char *)specifier_buffer);
        if (jerry_value_is_exception(module_value))
        {
            jerry_value_free(module_value);
            _cleanup_module_task(task);
            continue;
        }
        jerry_value_t link_result = jerry_module_link(module_value, _module_resolve_cb, NULL);
        if (jerry_value_is_exception(link_result))
        {
            jerry_value_free(link_result);
            jerry_value_free(module_value);
            _cleanup_module_task(task);
            continue;
        }
        jerry_value_free(link_result);
        jerry_value_t eval_result = jerry_module_evaluate(module_value);
        if (jerry_value_is_exception(eval_result))
        {
            jerry_value_free(eval_result);
            jerry_value_free(module_value);
            _cleanup_module_task(task);
            continue;
        }
        jerry_value_free(eval_result);
        jerry_value_t namespace_value = jerry_module_namespace(module_value);
        jerry_value_t resolve_result = jerry_promise_resolve(task->promise, namespace_value);
        if (!jerry_value_is_exception(resolve_result))
            jerry_value_free(resolve_result);
        jerry_value_free(namespace_value);
        jerry_value_free(module_value);
        _cleanup_module_task(task);
    }
}

static void _cleanup_module_queue(void)
{
    if (!_module_queue) return;
    while (eos_cqueue_get_size(_module_queue) > 0)
    {
        _module_task_t *task = (_module_task_t *)eos_cqueue_dequeue(_module_queue);
        if (task)
        {
            jerry_value_free(task->specifier);
            jerry_value_free(task->user_value);
            jerry_value_free(task->promise);
            eos_free(task);
        }
    }
    eos_cqueue_destroy(_module_queue);
    _module_queue = NULL;
}

/************************** error helpers / info helpers **************************/

jerry_value_t se_kernel_throw_error(const char *message)
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
    if (!msg) return;
    size_t len = strlen(msg);
    engine_ctx.error_info = (char *)eos_malloc(len + 1);
    if (engine_ctx.error_info)
        memcpy(engine_ctx.error_info, msg, len + 1);
}

const char *se_kernel_get_error_info(void)
{
    return engine_ctx.error_info ? engine_ctx.error_info : "";
}

const script_error_location_t *se_kernel_get_error_location(void)
{
    return &engine_ctx.error_location;
}

const script_error_location_t *se_kernel_get_error_backtrace(uint32_t *count)
{
    if (count) *count = engine_ctx.backtrace_count;
    return engine_ctx.backtrace;
}

uint32_t se_kernel_get_backtrace_count(void)
{
    return engine_ctx.backtrace_count;
}

static void _clear_error_location(void)
{
    memset(&engine_ctx.error_location, 0, sizeof(script_error_location_t));
    memset(engine_ctx.backtrace, 0, sizeof(engine_ctx.backtrace));
    engine_ctx.backtrace_count = 0;
}

static void _clear_error_info(void)
{
    if (engine_ctx.error_info)
    {
        eos_free(engine_ctx.error_info);
        engine_ctx.error_info = NULL;
    }
    _clear_error_location();
}

static void _script_pkg_destroy(script_pkg_t *pkg)
{
    if (!pkg) return;
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
    if (!source) return NULL;
    script_pkg_t *copy = eos_malloc_zeroed(sizeof(script_pkg_t));
    if (!copy) return NULL;
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

/************************** State machine **************************/

static script_engine_result_t _change_state(script_state_t new_state)
{
    switch (engine_ctx.state)
    {
    case SCRIPT_STATE_STOPPED:
        if (new_state != SCRIPT_STATE_RUNNING && new_state != SCRIPT_STATE_SUSPEND && new_state != SCRIPT_STATE_ERROR)
        {
            EOS_LOG_E("Invalid state transition from STOPPED to %d", new_state);
            return -SE_ERR_INVALID_STATE;
        }
        break;
    case SCRIPT_STATE_RUNNING:
        if (new_state != SCRIPT_STATE_SUSPEND && new_state != SCRIPT_STATE_STOPPING && new_state != SCRIPT_STATE_ERROR)
        {
            EOS_LOG_E("Invalid state transition from RUNNING to %d", new_state);
            return -SE_ERR_INVALID_STATE;
        }
        break;
    case SCRIPT_STATE_SUSPEND:
        if (new_state != SCRIPT_STATE_STOPPED && new_state != SCRIPT_STATE_RUNNING && new_state != SCRIPT_STATE_ERROR)
        {
            EOS_LOG_E("Invalid state transition from SUSPEND to %d", new_state);
            return -SE_ERR_INVALID_STATE;
        }
        break;
    case SCRIPT_STATE_STOPPING:
        if (new_state != SCRIPT_STATE_STOPPED && new_state != SCRIPT_STATE_ERROR)
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
    engine_ctx.state = new_state;
    return SE_OK;
}

static void _check_mem(void)
{
    if (!jerry_feature_enabled(JERRY_FEATURE_HEAP_STATS)) return;
    jerry_heap_stats_t stats = {0};
    if (jerry_heap_stats(&stats))
        EOS_LOG_I("Heap total sz: %d\nAllocated bytes: %d\nPeak allocated bytes: %d\n",
                  stats.size, stats.allocated_bytes, stats.peak_allocated_bytes);
}

static void _collect_script_garbage(void)
{
    jerry_heap_gc(JERRY_GC_PRESSURE_HIGH);
    jerry_heap_gc(JERRY_GC_PRESSURE_LOW);
}

void se_kernel_set_prop_number(jerry_value_t obj, const char *prop_name, double value)
{
    jerry_value_t prop = jerry_string_sz(prop_name);
    jerry_value_t jv = jerry_number(value);
    jerry_value_t ret = jerry_object_set(obj, prop, jv);
    jerry_value_free(ret);
    jerry_value_free(jv);
    jerry_value_free(prop);
}

void se_kernel_set_prop_bool(jerry_value_t obj, const char *prop_name, bool value)
{
    jerry_value_t prop = jerry_string_sz(prop_name);
    jerry_value_t jv = jerry_boolean(value);
    jerry_value_t ret = jerry_object_set(obj, prop, jv);
    jerry_value_free(ret);
    jerry_value_free(jv);
    jerry_value_free(prop);
}

void se_kernel_set_prop_string(jerry_value_t obj, const char *prop_name, const char *value)
{
    jerry_value_t prop = jerry_string_sz(prop_name);
    jerry_value_t jv = jerry_string_sz(value);
    jerry_value_t ret = jerry_object_set(obj, prop, jv);
    jerry_value_free(ret);
    jerry_value_free(jv);
    jerry_value_free(prop);
}

/************************** Backtrace parsing **************************/

static void _parse_backtrace_from_js_array(jerry_value_t backtrace_array)
{
    _clear_error_location();
    if (!jerry_value_is_array(backtrace_array)) return;
    uint32_t array_len = jerry_array_length(backtrace_array);
    if (array_len == 0) return;
    uint32_t max_frames = array_len < SCRIPT_BACKTRACE_MAX_FRAMES ? array_len : SCRIPT_BACKTRACE_MAX_FRAMES;
    for (uint32_t i = 0; i < max_frames; i++)
    {
        jerry_value_t element = jerry_object_get_index(backtrace_array, i);
        if (!jerry_value_is_string(element)) { jerry_value_free(element); continue; }
        jerry_char_t buffer[SCRIPT_ERROR_STACK_BUF_SIZE];
        jerry_size_t copied = jerry_string_to_buffer(element, JERRY_ENCODING_UTF8, buffer, SCRIPT_ERROR_STACK_BUF_SIZE - 1);
        buffer[copied] = '\0';
        jerry_value_free(element);
        const char *str = (const char *)buffer;
        script_error_location_t *loc = &engine_ctx.backtrace[i];
        const char *file_start = strstr(str, " ");
        if (file_start == NULL) file_start = str; else file_start++;
        const char *colon1 = strchr(file_start, ':');
        if (!colon1) continue;
        const char *colon2 = strchr(colon1 + 1, ':');
        if (!colon2) continue;
        size_t file_len = colon1 - file_start;
        if (file_len > SCRIPT_BACKTRACE_SOURCE_SIZE - 1) file_len = SCRIPT_BACKTRACE_SOURCE_SIZE - 1;
        strncpy(loc->source_name, file_start, file_len);
        loc->source_name[file_len] = '\0';
        loc->line = (uint32_t)atoi(colon1 + 1);
        loc->column = (uint32_t)atoi(colon2 + 1);
        engine_ctx.backtrace_count++;
    }
    if (engine_ctx.backtrace_count > 0)
        engine_ctx.error_location = engine_ctx.backtrace[0];
}

static void _capture_error_backtrace(void)
{
    if (!jerry_feature_enabled(JERRY_FEATURE_LINE_INFO))
    {
        _clear_error_location();
        return;
    }
    jerry_value_t backtrace_array = jerry_backtrace(SCRIPT_BACKTRACE_MAX_FRAMES);
    if (jerry_value_is_exception(backtrace_array))
    {
        jerry_value_free(backtrace_array);
        _clear_error_location();
        return;
    }
    _parse_backtrace_from_js_array(backtrace_array);
    jerry_value_free(backtrace_array);
}

static void _log_backtrace(void)
{
    if (engine_ctx.backtrace_count == 0)
    {
        if (engine_ctx.error_location.line > 0)
            EOS_LOG_E("  --> %s:%u:%u", engine_ctx.error_location.source_name,
                      engine_ctx.error_location.line, engine_ctx.error_location.column);
        return;
    }
    EOS_LOG_E("Backtrace:");
    for (uint32_t i = 0; i < engine_ctx.backtrace_count; i++)
    {
        script_error_location_t *loc = &engine_ctx.backtrace[i];
        if (loc->source_name[0] != '\0')
            EOS_LOG_E("  %u: %s:%u:%u", i, loc->source_name, loc->line, loc->column);
        else
            EOS_LOG_E("  %u: <unknown>", i);
    }
}

static void _extract_error_location_from_exception(jerry_value_t exception_value)
{
    if (!jerry_value_is_object(exception_value)) return;
    _clear_error_location();
    jerry_value_t stack_prop = jerry_object_get_sz(exception_value, "stack");
    if (jerry_value_is_object(stack_prop))
    {
        if (jerry_value_is_array(stack_prop))
        {
            uint32_t array_len = jerry_array_length(stack_prop);
            for (uint32_t i = 0; i < array_len && engine_ctx.backtrace_count < SCRIPT_BACKTRACE_MAX_FRAMES; i++)
            {
                jerry_value_t element = jerry_object_get_index(stack_prop, i);
                if (jerry_value_is_string(element))
                {
                    jerry_char_t buffer[SCRIPT_ERROR_STACK_BUF_SIZE];
                    jerry_size_t copied = jerry_string_to_buffer(element, JERRY_ENCODING_UTF8,
                                                                 buffer, sizeof(buffer) - 1);
                    buffer[copied] = '\0';
                    script_error_location_t *loc = &engine_ctx.backtrace[engine_ctx.backtrace_count];
                    const char *line_start = (const char *)buffer;
                    const char *colon1 = strchr(line_start, ':');
                    const char *colon2 = colon1 ? strchr(colon1 + 1, ':') : NULL;
                    if (colon1 && colon2 && colon2 > colon1)
                    {
                        size_t name_len = colon1 - line_start;
                        if (name_len > SCRIPT_BACKTRACE_SOURCE_SIZE - 1) name_len = SCRIPT_BACKTRACE_SOURCE_SIZE - 1;
                        memcpy(loc->source_name, line_start, name_len);
                        loc->source_name[name_len] = '\0';
                        loc->line = (uint32_t)atoi(colon1 + 1);
                        loc->column = (uint32_t)atoi(colon2 + 1);
                        if (engine_ctx.backtrace_count == 0) engine_ctx.error_location = *loc;
                        engine_ctx.backtrace_count++;
                    }
                }
                jerry_value_free(element);
            }
        }
        jerry_value_free(stack_prop);
        return;
    }
    jerry_value_free(stack_prop);
    jerry_value_t line_prop = jerry_object_get_sz(exception_value, "lineNumber");
    if (jerry_value_is_number(line_prop)) engine_ctx.error_location.line = (uint32_t)jerry_value_as_number(line_prop);
    jerry_value_free(line_prop);
    jerry_value_t col_prop = jerry_object_get_sz(exception_value, "columnNumber");
    if (jerry_value_is_number(col_prop)) engine_ctx.error_location.column = (uint32_t)jerry_value_as_number(col_prop);
    jerry_value_free(col_prop);
    jerry_value_t msg_prop = jerry_object_get_sz(exception_value, "message");
    if (jerry_value_is_string(msg_prop))
    {
        jerry_size_t size = jerry_string_size(msg_prop, JERRY_ENCODING_UTF8);
        if (size > 0 && size < SCRIPT_BACKTRACE_SOURCE_SIZE)
        {
            jerry_string_to_buffer(msg_prop, JERRY_ENCODING_UTF8,
                                  (jerry_char_t *)engine_ctx.error_location.source_name, size);
            engine_ctx.error_location.source_name[size] = '\0';
        }
        else if (size >= SCRIPT_BACKTRACE_SOURCE_SIZE)
        {
            jerry_string_to_buffer(msg_prop, JERRY_ENCODING_UTF8,
                                  (jerry_char_t *)engine_ctx.error_location.source_name,
                                  SCRIPT_BACKTRACE_SOURCE_SIZE - 1);
            engine_ctx.error_location.source_name[SCRIPT_BACKTRACE_SOURCE_SIZE - 1] = '\0';
        }
    }
    jerry_value_free(msg_prop);
}

static void _script_engine_exception_handler(const char *tag, jerry_value_t result)
{
    EOS_LOG_E("===================================");
    jerry_value_t value = jerry_exception_value(result, false);
    jerry_value_t final_str_val = value;
    char stack_buf[SCRIPT_ERROR_STACK_BUF_SIZE];
    char *buf = stack_buf;
    bool need_free = false;
    if (!(jerry_value_is_string(value)))
        final_str_val = jerry_value_to_string(value);
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
            EOS_LOG_E("%s Error: %s", tag, buf);
            _set_error_info(buf);
            if (need_free) eos_free(buf);
        }
    }
    else
    {
        EOS_LOG_E("Unknown error");
        _set_error_info("Unknown error");
    }
    jerry_value_free(final_str_val);
    _extract_error_location_from_exception(value);
    _log_backtrace();
    jerry_value_free(value);
    EOS_LOG_E("===================================");
}

/************************** Helper script_info helper **************************/

static jerry_value_t _create_info(const script_pkg_t *pkg)
{
    jerry_value_t obj = jerry_object();
    se_kernel_set_prop_string(obj, "id", pkg->id);
    se_kernel_set_prop_string(obj, "name", pkg->name);
    se_kernel_set_prop_string(obj, "version", pkg->version);
    se_kernel_set_prop_string(obj, "author", pkg->author);
    se_kernel_set_prop_string(obj, "description", pkg->description);
    return obj;
}

/************************** Helper manifest parser **************************/

script_engine_result_t se_kernel_get_manifest(const char *manifest_path, script_pkg_t *pkg)
{
    if (!manifest_path || !pkg) return -SE_ERR_NULL_PACKAGE;
    char *manifest_json = eos_storage_read_file(manifest_path);
    if (!manifest_json) return -SE_FAILED;
    cJSON *root = cJSON_Parse(manifest_json);
    eos_free(manifest_json);
    if (!root) return -SE_FAILED;
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
        cJSON_Delete(root);
        return -SE_FAILED;
    }
    if (pkg->id) eos_free((void *)pkg->id);
    if (pkg->name) eos_free((void *)pkg->name);
    if (pkg->version) eos_free((void *)pkg->version);
    if (pkg->author) eos_free((void *)pkg->author);
    if (pkg->description) eos_free((void *)pkg->description);
    pkg->id = eos_strdup(id->valuestring);
    pkg->name = eos_strdup(name->valuestring);
    pkg->version = eos_strdup(version->valuestring);
    pkg->author = eos_strdup(author->valuestring);
    pkg->description = eos_strdup(description->valuestring);
    cJSON_Delete(root);
    return SE_OK;
}

void se_kernel_register_functions(jerry_value_t parent,
                                  const script_engine_func_entry_t *entries,
                                  size_t funcs_count)
{
    for (size_t i = 0; i < funcs_count; ++i)
    {
        const char *class_name = entries[i].class_name;
        const char *method_name = entries[i].method_name;
        jerry_value_t target_obj = parent;
        if (class_name != NULL)
        {
            jerry_value_t class_key = jerry_string_sz(class_name);
            jerry_value_t class_obj = jerry_object_get(parent, class_key);
            if (jerry_value_is_undefined(class_obj))
            {
                jerry_value_free(class_obj);
                class_obj = jerry_object();
                jerry_value_free(jerry_object_set(parent, class_key, class_obj));
            }
            jerry_value_free(class_key);
            target_obj = class_obj;
        }
        jerry_value_t fn = jerry_function_external(entries[i].handler);
        jerry_value_t method_key = jerry_string_sz(method_name);
        jerry_value_free(jerry_object_set(target_obj, method_key, fn));
        jerry_value_free(method_key);
        jerry_value_free(fn);
        if (class_name != NULL) jerry_value_free(target_obj);
    }
}

/************************** VM exec stop callback **************************/

static jerry_value_t _vm_exec_stop_callback(void *user_p)
{
    (void)user_p;
    if (engine_ctx.state == SCRIPT_STATE_STOPPING)
        return jerry_string_sz("Script terminated by request");
    if (engine_ctx.script_timeout_ms > 0 && engine_ctx.state == SCRIPT_STATE_RUNNING)
    {
        uint32_t elapsed = eos_tick_get() - engine_ctx.script_start_time;
        if (elapsed >= engine_ctx.script_timeout_ms)
        {
            engine_ctx.stop_is_timeout = true;
            _change_state(SCRIPT_STATE_STOPPING);
            return jerry_string_sz("Script execution timeout");
        }
    }
    return jerry_undefined();
}

/************************** State queries **************************/

script_state_t se_kernel_get_state(void) { return engine_ctx.state; }
script_pkg_type_t se_kernel_get_current_type(void)
{
    return engine_ctx.current_script ? engine_ctx.current_script->type : SCRIPT_TYPE_UNKNOWN;
}
char *se_kernel_get_current_id(void)
{
    return engine_ctx.current_script ? (char *)engine_ctx.current_script->id : NULL;
}
char *se_kernel_get_current_name(void)
{
    return engine_ctx.current_script ? (char *)engine_ctx.current_script->name : NULL;
}

void se_kernel_set_timeout(uint32_t timeout_ms) { engine_ctx.script_timeout_ms = timeout_ms; }
uint32_t se_kernel_get_timeout(void) { return engine_ctx.script_timeout_ms; }
/************************** Call **************************/

se_kernel_call_result_t se_kernel_call(jerry_value_t func,
                                       jerry_value_t this_val,
                                       const jerry_value_t args_p[],
                                       const jerry_length_t args_count)
{
    se_kernel_call_result_t result;
    memset(&result, 0, sizeof(result));
    result.return_value = jerry_undefined();

    script_state_t state = se_kernel_get_state();
    if (state != SCRIPT_STATE_RUNNING && state != SCRIPT_STATE_SUSPEND)
    {
        result.base.result = SE_ERR_INVALID_STATE;
        return result;
    }

    bool was_suspended = (state == SCRIPT_STATE_SUSPEND);
    if (was_suspended)
    {
        _change_state(SCRIPT_STATE_RUNNING);
        if (engine_ctx.script_timeout_ms > 0)
            engine_ctx.script_start_time = eos_tick_get();
    }

    jerry_value_t call_result = jerry_call(func, this_val, args_p, args_count);

    if (jerry_value_is_exception(call_result))
    {
        if (engine_ctx.state == SCRIPT_STATE_STOPPING)
        {
            result.base.result = engine_ctx.stop_is_timeout ? -SE_ERR_TIMEOUT : SE_OK;
            if (engine_ctx.stop_is_timeout)
            {
                result.base.error_type = EOS_SCRIPT_FAULT_UNRESPONSIVE;
                _set_error_info("Script execution timeout");
            }
        }
        else
        {
            _script_engine_exception_handler("Jerry Call", call_result);
            _change_state(SCRIPT_STATE_ERROR);
            result.base.result = -SE_ERR_JERRY_EXCEPTION;
            _populate_result_from_ctx(&result.base);
            if (se_kernel_get_error_info() && strstr(se_kernel_get_error_info(), "timeout"))
                result.base.error_type = EOS_SCRIPT_FAULT_UNRESPONSIVE;
        }
        jerry_value_free(call_result);
    }
    else
    {
        result.base.result = SE_OK;
        result.return_value = call_result;
    }

    state = se_kernel_get_state();
    if (state == SCRIPT_STATE_RUNNING)
        _change_state(SCRIPT_STATE_SUSPEND);

    return result;
}
/************************** Engine cleanup **************************/

static void _engine_cleanup(void)
{
    if (jerry_value_is_object(engine_ctx.old_realm))
    {
        jerry_set_realm(engine_ctx.old_realm);
    }
    else if (jerry_value_is_object(engine_ctx.script_realm))
    {
        jerry_value_t temp = jerry_realm();
        jerry_set_realm(temp);
        jerry_value_free(temp);
    }
    if (jerry_value_is_object(engine_ctx.script_realm))
    {
        jerry_value_free(engine_ctx.script_realm);
        engine_ctx.script_realm = jerry_undefined();
    }
    engine_ctx.old_realm = jerry_undefined();
    jerry_heap_gc(JERRY_GC_PRESSURE_LOW);
}

/************************** Context management **************************/

script_engine_saved_context_t *se_kernel_save_context(void)
{
    if (engine_ctx.state != SCRIPT_STATE_SUSPEND) return NULL;
    script_engine_saved_context_t *ctx = eos_malloc_zeroed(sizeof(script_engine_saved_context_t));
    if (!ctx) return NULL;
    ctx->script_realm = engine_ctx.script_realm;
    engine_ctx.script_realm = jerry_undefined();
    ctx->old_realm = engine_ctx.old_realm;
    engine_ctx.old_realm = jerry_undefined();
    ctx->script = engine_ctx.current_script;
    engine_ctx.current_script = NULL;
    ctx->base_path = engine_ctx.base_path;
    engine_ctx.base_path = NULL;
    ctx->script_timeout_ms = engine_ctx.script_timeout_ms;
    _change_state(SCRIPT_STATE_STOPPED);
    return ctx;
}

se_kernel_run_result_t se_kernel_restore_context(script_engine_saved_context_t *ctx)
{
    se_kernel_run_result_t result;
    memset(&result, 0, sizeof(result));

    if (!ctx)
    {
        result.result = -SE_ERR_NULL_PACKAGE;
        return result;
    }
    if (engine_ctx.state != SCRIPT_STATE_STOPPED)
    {
        result.result = -SE_ERR_INVALID_STATE;
        return result;
    }

    if (jerry_value_is_object(engine_ctx.script_realm))
    {
        jerry_value_free(engine_ctx.script_realm);
        engine_ctx.script_realm = jerry_undefined();
    }

    engine_ctx.script_realm = ctx->script_realm;
    ctx->script_realm = jerry_undefined();
    engine_ctx.old_realm = ctx->old_realm;
    ctx->old_realm = jerry_undefined();
    engine_ctx.current_script = ctx->script;
    ctx->script = NULL;
    engine_ctx.base_path = ctx->base_path;
    ctx->base_path = NULL;
    engine_ctx.script_timeout_ms = ctx->script_timeout_ms;

    eos_free(ctx);
    _change_state(SCRIPT_STATE_SUSPEND);
    jerry_value_free(jerry_run_jobs());

    result.result = SE_OK;
    return result;
}

void se_kernel_destroy_context(script_engine_saved_context_t *ctx)
{
    if (!ctx) return;
    if (jerry_value_is_object(ctx->script_realm))
    {
        if (jerry_value_is_object(ctx->old_realm))
        {
            jerry_set_realm(ctx->old_realm);
        }
        else
        {
            jerry_value_t temp = jerry_realm();
            jerry_set_realm(temp);
            jerry_value_free(temp);
        }
        jerry_value_free(ctx->script_realm);
        ctx->script_realm = jerry_undefined();
    }
    ctx->old_realm = jerry_undefined();
    if (ctx->script)
    {
        _script_pkg_destroy(ctx->script);
        ctx->script = NULL;
    }
    ctx->base_path = NULL;
    eos_free(ctx);
    jerry_heap_gc(JERRY_GC_PRESSURE_HIGH);
}

static void _script_engine_stop_and_cleanup(void)
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
    _check_mem();
}

se_kernel_run_result_t se_kernel_stop(void)
{
    se_kernel_run_result_t result;
    memset(&result, 0, sizeof(result));
    result.result = SE_OK;

    switch (engine_ctx.state)
    {
    case SCRIPT_STATE_STOPPED:
        return result;
    case SCRIPT_STATE_RUNNING:
        engine_ctx.stop_is_timeout = false;
        _change_state(SCRIPT_STATE_STOPPING);
        eos_dispatcher_call((eos_dispatcher_cb_t)_script_engine_stop_and_cleanup, NULL);
        return result;
    case SCRIPT_STATE_STOPPING:
        return result;
    case SCRIPT_STATE_SUSPEND:
        _script_engine_stop_and_cleanup();
        return result;
    case SCRIPT_STATE_ERROR:
        _script_engine_stop_and_cleanup();
        return result;
    default:
        result.result = -SE_ERR_INVALID_STATE;
        return result;
    }
}

se_kernel_run_result_t se_kernel_init(void)
{
    se_kernel_run_result_t result;
    memset(&result, 0, sizeof(result));
    result.result = SE_OK;

    if (engine_ctx.initialized)
    {
        result.result = SE_ERR_ALREADY_INITIALIZED;
        return result;
    }

    if (!jerry_feature_enabled(JERRY_FEATURE_VM_EXEC_STOP) ||
        !jerry_feature_enabled(JERRY_FEATURE_REALM) ||
        !jerry_feature_enabled(JERRY_FEATURE_MODULE))
    {
        result.result = -SE_ERR_JERRY_INIT_FAIL;
        return result;
    }

    jerry_init(SCRIPT_INIT_FLAGS);
    sni_init();
    engine_ctx.script_realm = jerry_undefined();
    engine_ctx.old_realm = jerry_undefined();
    engine_ctx.initialized = true;
    _check_mem();
    return result;
}

se_kernel_run_result_t se_kernel_clean_up(void)
{
    se_kernel_run_result_t result;
    memset(&result, 0, sizeof(result));
    result.result = SE_OK;

    if (engine_ctx.state != SCRIPT_STATE_STOPPED)
    {
        se_kernel_run_result_t stop_result = se_kernel_stop();
        if (stop_result.result != SE_OK)
        {
            result = stop_result;
            return result;
        }
    }

    _cleanup_module_queue();
    jerry_module_cleanup(jerry_undefined());
    jerry_cleanup();
    engine_ctx.initialized = false;
    _change_state(SCRIPT_STATE_STOPPED);
    return result;
}

/************************** Run **************************/

se_kernel_run_result_t se_kernel_run(const script_pkg_t *script_package)
{
    se_kernel_run_result_t result;
    memset(&result, 0, sizeof(result));
    result.result = SE_OK;

    if (!script_package || !script_package->script_str)
    {
        result.result = -SE_ERR_NULL_PACKAGE;
        return result;
    }

    if (!engine_ctx.initialized)
    {
        result.result = -SE_ERR_NOT_INITIALIZED;
        return result;
    }

    if (engine_ctx.state != SCRIPT_STATE_STOPPED && engine_ctx.state != SCRIPT_STATE_ERROR)
    {
        result.result = -SE_ERR_INVALID_STATE;
        return result;
    }

    script_pkg_t *owned_script = _script_pkg_clone(script_package);
    if (!owned_script)
    {
        result.result = -SE_ERR_MALLOC;
        return result;
    }

    engine_ctx.current_script = owned_script;
    engine_ctx.script_start_time = eos_tick_get();
    _change_state(SCRIPT_STATE_RUNNING);
    engine_ctx.base_path = engine_ctx.current_script->base_path ? engine_ctx.current_script->base_path : "/";

    jerry_value_t new_realm = jerry_realm();
    engine_ctx.script_realm = new_realm;
    engine_ctx.old_realm = jerry_set_realm(new_realm);
    sni_mount(new_realm);

    jerry_halt_handler(16, _vm_exec_stop_callback, NULL);
    jerry_log_set_level(JERRY_LOG_LEVEL_DEBUG);

    jerry_value_t global = jerry_current_realm();
    jerry_value_t script_info = _create_info(engine_ctx.current_script);
    jerry_value_t key = jerry_string_sz("scriptInfo");
    jerry_value_free(jerry_object_set(global, key, script_info));
    jerry_value_free(key);
    jerry_value_free(script_info);
    jerry_value_free(global);

    jerry_module_on_import(_module_import_cb, NULL);

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

    eos_free((void *)engine_ctx.current_script->script_str);
    engine_ctx.current_script->script_str = NULL;

    if (!jerry_value_is_exception(parsed_code))
    {
        jerry_value_t link_result = jerry_module_link(parsed_code, _module_resolve_cb, NULL);
        if (jerry_value_is_exception(link_result))
        {
            _script_engine_exception_handler("Module Link", link_result);
            _change_state(SCRIPT_STATE_ERROR);
            jerry_value_free(link_result);
            result.result = -SE_ERR_JERRY_EXCEPTION;
            result.error_type = EOS_SCRIPT_FAULT_ERROR_MODULE_LINK;
            _populate_result_from_ctx(&result);
        }
        else
        {
            jerry_value_free(link_result);
            jerry_value_t run_result = jerry_module_evaluate(parsed_code);
            if (jerry_value_is_exception(run_result))
            {
                if (engine_ctx.state == SCRIPT_STATE_STOPPING)
                {
                    if (engine_ctx.stop_is_timeout)
                    {
                        result.result = -SE_ERR_TIMEOUT;
                        result.error_type = EOS_SCRIPT_FAULT_UNRESPONSIVE;
                        _set_error_info("Script execution timeout");
                    }
                    else
                    {
                        result.result = SE_OK;
                    }
                }
                else
                {
                    _script_engine_exception_handler("Script Runtime", run_result);
                    _change_state(SCRIPT_STATE_ERROR);
                    result.result = -SE_ERR_JERRY_EXCEPTION;
                    _populate_result_from_ctx(&result);
                    if (se_kernel_get_error_info() && strstr(se_kernel_get_error_info(), "timeout"))
                        result.error_type = EOS_SCRIPT_FAULT_UNRESPONSIVE;
                }
            }
            else
            {
                if (engine_ctx.state == SCRIPT_STATE_RUNNING)
                    _change_state(SCRIPT_STATE_SUSPEND);
                result.result = SE_OK;
            }
            jerry_value_free(run_result);
        }
    }
    else
    {
        _script_engine_exception_handler("Script Parse", parsed_code);
        _change_state(SCRIPT_STATE_ERROR);
        result.result = -SE_ERR_INVALID_JS;
        result.error_type = EOS_SCRIPT_FAULT_ERROR_PARSE;
        _populate_result_from_ctx(&result);
    }

    jerry_value_free(parsed_code);
    _process_module_queue();
    jerry_value_free(jerry_run_jobs());
    _cleanup_module_queue();
    jerry_module_cleanup(jerry_undefined());

    if ((result.result != SE_OK && engine_ctx.state == SCRIPT_STATE_ERROR) ||
        engine_ctx.state == SCRIPT_STATE_STOPPING)
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

    engine_ctx.stop_is_timeout = false;
    return result;
}
