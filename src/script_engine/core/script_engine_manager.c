/**
 * @file script_engine_manager.c
 * @brief Script engine manager layer implementation (high-level policy and control)
 */

#include "script_engine_manager.h"
#include "script_engine_kernel.h"

/* Includes ---------------------------------------------------*/
#define EOS_LOG_TAG "ScriptMgr"
#include "eos_log.h"
#include "eos_event.h"
#include "eos_app.h"
#include "eos_mem.h"
#include "eos_service_storage.h"
#include "eos_pkg_mgr.h"
#include "eos_config.h"
#include "eos_watchface.h"

#include <string.h>
#include <stdio.h>

/* Macros and Definitions -------------------------------------*/

#define EOS_APP_MANIFEST_FILE_NAME "manifest.json"
#define EOS_APP_SCRIPT_ENTRY_FILE_NAME "main.js"
#define EOS_WATCHFACE_MANIFEST_FILE_NAME "manifest.json"
#define EOS_WATCHFACE_SCRIPT_ENTRY_FILE_NAME "main.js"

/* Variables --------------------------------------------------*/

static script_engine_saved_context_t *_saved_ctx = NULL;
static char _saved_script_id[EOS_PKG_ID_LEN_MAX] = {0};

/* Function Implementations -----------------------------------*/

static script_engine_result_t _run_result_to_engine(const se_kernel_run_result_t *kr)
{
    return (script_engine_result_t)kr->result;
}

static script_engine_result_t _do_kernel_restore(void)
{
    if (!_saved_ctx)
        return -SE_ERR_NULL_PACKAGE;

    se_kernel_run_result_t kr = se_kernel_restore_context(_saved_ctx);
    _saved_ctx = NULL;
    _saved_script_id[0] = '\0';

    if (kr.result == SE_OK)
    {
        eos_event_post(EOS_EVENT_SCRIPT_STARTED, NULL, NULL);
    }
    else
    {
        eos_event_post(EOS_EVENT_SCRIPT_EXITED, NULL, NULL);
    }
    return (script_engine_result_t)kr.result;
}

static script_engine_result_t _do_kernel_run(const script_pkg_t *pkg)
{
    se_kernel_run_result_t kr = se_kernel_run(pkg);
    if (kr.result == SE_OK)
    {
        eos_event_post(EOS_EVENT_SCRIPT_STARTED, NULL, NULL);
    }
    else
    {
        const char *script_id = pkg ? pkg->id : NULL;
        eos_script_error_type_t etype = kr.error_type != EOS_SCRIPT_FAULT_ERROR_UNKNOWN
            ? kr.error_type : EOS_SCRIPT_FAULT_ERROR_EXCEPTION;
        eos_app_handle_script_error(etype, kr.result, script_id, NULL);
    }
    return (script_engine_result_t)kr.result;
}

static void _clear_saved_context(void)
{
    if (_saved_ctx)
    {
        se_kernel_destroy_context(_saved_ctx);
        _saved_ctx = NULL;
        _saved_script_id[0] = '\0';
        eos_event_post(EOS_EVENT_SCRIPT_EXITED, NULL, NULL);
    }
}

static bool _is_watchface_pkg(const script_pkg_t *pkg)
{
    return pkg && pkg->type == SCRIPT_TYPE_WATCHFACE;
}

static bool _is_application_pkg(const script_pkg_t *pkg)
{
    return pkg && pkg->type == SCRIPT_TYPE_APPLICATION;
}

/* ===== Public API ===== */

script_engine_result_t script_engine_init(void)
{
    se_kernel_run_result_t kr = se_kernel_init();
    return (script_engine_result_t)kr.result;
}

/* ===== Watchface resume (lightweight, no file I/O) ===== */

script_engine_result_t script_engine_watchface_resume(const char *watchface_id)
{
    if (!watchface_id)
        return -SE_ERR_NULL_PACKAGE;

    // Saved context matches → restore.
    if (_saved_ctx && _saved_script_id[0] &&
        strcmp(_saved_script_id, watchface_id) == 0)
    {
        EOS_LOG_D("watchface_resume: restoring saved context for %s", watchface_id);
        return _do_kernel_restore();
    }

    // Engine is SUSPEND with the same watchface → no-op.
    script_state_t state = se_kernel_get_state();
    script_pkg_type_t current_type = se_kernel_get_current_type();

    if (state == SCRIPT_STATE_SUSPEND && current_type == SCRIPT_TYPE_WATCHFACE)
    {
        const char *current_id = se_kernel_get_current_id();
        if (current_id && strcmp(current_id, watchface_id) == 0)
        {
            EOS_LOG_D("watchface_resume: same watchface already running, no-op");
            return SE_OK;
        }
    }

    // Cannot resume – caller must do a full script_engine_run().
    EOS_LOG_D("watchface_resume: cannot resume %s, caller must do full run", watchface_id);
    return -SE_FAILED;
}



script_engine_result_t script_engine_run(const script_pkg_t *script_package)
{
    if (!script_package || !script_package->script_str)
        return -SE_ERR_NULL_PACKAGE;

    script_state_t state = se_kernel_get_state();
    script_pkg_type_t current_type = se_kernel_get_current_type();

    /*
     * Case 1: A watchface context was saved earlier (watchface -> app path).
     *          Now the caller wants to run a watchface.
     *          If it matches the saved one -> restore context.
     *          If different -> destroy saved context and run fresh.
     */
    if (_saved_ctx && _is_watchface_pkg(script_package))
    {
        se_kernel_stop();

        bool matches_saved = (_saved_script_id[0] &&
                              script_package->id &&
                              strcmp(_saved_script_id, script_package->id) == 0);

        if (matches_saved)
        {
            EOS_LOG_D("Restoring saved watchface context: %s", _saved_script_id);
            return _do_kernel_restore();
        }

        EOS_LOG_D("Saved context differs from requested watchface, destroying it");
        _clear_saved_context();
        return _do_kernel_run(script_package);
    }

    switch (state)
    {
    case SCRIPT_STATE_SUSPEND:
        if (current_type == SCRIPT_TYPE_WATCHFACE && _is_application_pkg(script_package))
        {
            /* Watchface → Application: save the watchface context, then run app. */
            _saved_ctx = se_kernel_save_context();
            if (_saved_ctx)
            {
                const char *id = se_kernel_get_current_id();
                if (id)
                    strncpy(_saved_script_id, id, sizeof(_saved_script_id) - 1);
            }
            return _do_kernel_run(script_package);
        }
        if (_is_watchface_pkg(script_package))
        {
            const char *current_id = se_kernel_get_current_id();
            if (current_id && script_package->id &&
                strcmp(current_id, script_package->id) == 0)
            {
                return SE_OK;
            }
        }
        se_kernel_stop();
        return _do_kernel_run(script_package);

    case SCRIPT_STATE_STOPPED:
    case SCRIPT_STATE_ERROR:
        return _do_kernel_run(script_package);

    default:
        EOS_LOG_E("Cannot run script in current state: %d", state);
        return -SE_ERR_INVALID_STATE;
    }
}

script_engine_result_t script_engine_stop(void)
{
    script_state_t state = se_kernel_get_state();

    switch (state)
    {
    case SCRIPT_STATE_SUSPEND:
    case SCRIPT_STATE_RUNNING:
    case SCRIPT_STATE_ERROR:
    {
        se_kernel_run_result_t kr = se_kernel_stop();
        return (script_engine_result_t)kr.result;
    }
    default:
        return SE_OK;
    }
}

script_engine_result_t script_engine_clean_up(void)
{
    _clear_saved_context();
    se_kernel_run_result_t kr = se_kernel_clean_up();
    return (script_engine_result_t)kr.result;
}

script_engine_result_t script_engine_reload_current_script(void)
{
    script_state_t state = se_kernel_get_state();
    if (state == SCRIPT_STATE_STOPPED && !_saved_ctx)
    {
        EOS_LOG_E("No running script to reload");
        return -SE_ERR_SCRIPT_NOT_RUNNING;
    }

    script_pkg_type_t pkg_type;
    const char *base_path;
    const char *script_id;

    if (_saved_ctx || state != SCRIPT_STATE_STOPPED)
    {
        if (_saved_ctx && state == SCRIPT_STATE_STOPPED)
        {
            se_kernel_restore_context(_saved_ctx);
            _saved_ctx = NULL;
            _saved_script_id[0] = '\0';
            state = se_kernel_get_state();
        }

        script_id = se_kernel_get_current_id();
        base_path = NULL;

        if (!script_id)
        {
            EOS_LOG_E("Current script metadata is incomplete");
            return -SE_FAILED;
        }

        pkg_type = se_kernel_get_current_type();

        char *bp = eos_strdup("");
        if (!bp) return -SE_ERR_MALLOC;

        const char *manifest_file_name = NULL;
        const char *entry_file_name = NULL;
        switch (pkg_type)
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
            eos_free(bp);
            return -SE_ERR_INVALID_STATE;
        }

        char manifest_path[EOS_FS_PATH_MAX];
        snprintf(manifest_path, sizeof(manifest_path), "%s%s", bp, manifest_file_name);

        script_pkg_t pkg = {0};
        pkg.type = pkg_type;
        if (se_kernel_get_manifest(manifest_path, &pkg) != SE_OK)
        {
            EOS_LOG_E("Read manifest failed: %s", manifest_path);
            eos_free(bp);
            return -SE_FAILED;
        }

        char script_path[EOS_FS_PATH_MAX];
        snprintf(script_path, sizeof(script_path), "%s%s", bp, entry_file_name);

        pkg.base_path = eos_strdup(script_id);

        if (!eos_storage_is_file(script_path))
        {
            EOS_LOG_E("Can't find script: %s", script_path);
            eos_pkg_free(&pkg);
            eos_free(bp);
            return -SE_FAILED;
        }

        pkg.script_str = eos_storage_read_file(script_path);
        if (!pkg.script_str)
        {
            eos_pkg_free(&pkg);
            eos_free(bp);
            return -SE_FAILED;
        }

        script_engine_result_t run_ret = script_engine_run(&pkg);
        eos_pkg_free(&pkg);
        eos_free(bp);
        return run_ret;
    }

    return -SE_ERR_SCRIPT_NOT_RUNNING;
}

script_engine_result_t script_engine_reload_current_app(void)
{
    return script_engine_reload_current_script();
}

/* ===== Thin delegates to kernel ===== */

jerry_value_t script_engine_throw_error(const char *message)
{
    return se_kernel_throw_error(message);
}

const char *script_engine_get_error_info(void)
{
    return se_kernel_get_error_info();
}

const script_error_location_t *script_engine_get_error_location(void)
{
    return se_kernel_get_error_location();
}

const script_error_location_t *script_engine_get_error_backtrace(uint32_t *count)
{
    return se_kernel_get_error_backtrace(count);
}

uint32_t script_engine_get_backtrace_count(void)
{
    return se_kernel_get_backtrace_count();
}

script_state_t script_engine_get_state(void)
{
    return se_kernel_get_state();
}

script_pkg_type_t script_engine_get_current_script_type(void)
{
    return se_kernel_get_current_type();
}

char *script_engine_get_current_script_id(void)
{
    return se_kernel_get_current_id();
}

char *script_engine_get_current_script_name(void)
{
    return se_kernel_get_current_name();
}

script_engine_result_t script_engine_get_manifest(const char *manifest_path, script_pkg_t *pkg)
{
    return se_kernel_get_manifest(manifest_path, pkg);
}

void script_engine_register_functions(jerry_value_t parent,
                                      const script_engine_func_entry_t *entries,
                                      size_t funcs_count)
{
    se_kernel_register_functions(parent, entries, funcs_count);
}

inline void script_engine_set_prop_number(jerry_value_t obj, const char *prop_name, double value)
{
    se_kernel_set_prop_number(obj, prop_name, value);
}

inline void script_engine_set_prop_bool(jerry_value_t obj, const char *prop_name, bool value)
{
    se_kernel_set_prop_bool(obj, prop_name, value);
}

inline void script_engine_set_prop_string(jerry_value_t obj, const char *prop_name, const char *value)
{
    se_kernel_set_prop_string(obj, prop_name, value);
}

void script_engine_set_script_state(script_state_t state)
{
    (void)state;
}

void script_engine_set_timeout(uint32_t timeout_ms)
{
    se_kernel_set_timeout(timeout_ms);
}

uint32_t script_engine_get_timeout(void)
{
    return se_kernel_get_timeout();
}

jerry_value_t script_engine_call(jerry_value_t func,
                                  jerry_value_t this_val,
                                  const jerry_value_t args_p[],
                                  const jerry_length_t args_count)
{
    se_kernel_call_result_t kr = se_kernel_call(func, this_val, args_p, args_count);
    if (kr.base.result != SE_OK && kr.base.result != -SE_ERR_TIMEOUT)
    {
        const char *script_id = se_kernel_get_current_id();
        eos_app_handle_script_error(kr.base.error_type, kr.base.result, script_id, NULL);
    }
    return kr.return_value;
}

script_engine_result_t script_engine_request_stop(void)
{
    return script_engine_stop();
}
