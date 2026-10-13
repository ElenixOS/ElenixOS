/**
 * @file eos_recent_apps.c
 * @brief Recent Apps registry — LRU list, suspend/resume orchestration, eviction
 */

#include "eos_recent_apps.h"

#if EOS_RECENT_APPS_ENABLE

/* Includes ---------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "lvgl.h"
#define EOS_LOG_TAG "RecentApps"
#include "eos_log.h"
#include "eos_mem.h"
#include "eos_app.h"
#include "eos_app_list.h"
#include "eos_event.h"
#include "eos_dispatcher.h"
#include "spm.h"
#include "sni_context.h"
#include "sni_callback_runtime.h"
#include "eos_config.h"
#include "eos_basic_widgets.h"

/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

static eos_recent_app_entry_t *s_head = NULL;
static eos_recent_app_entry_t *s_tail = NULL;
static uint32_t s_count = 0;
static uint32_t s_total_mem_bytes = 0;
static eos_suspend_timer_strategy_t s_timer_strategy = EOS_SUSPEND_TIMER_RUN_ONCE;
static eos_suspend_anim_strategy_t s_anim_strategy = EOS_SUSPEND_ANIM_CONTINUE;
static bool s_initialized = false;

/* Forward Declarations ---------------------------------------*/
static void _lru_link_head(eos_recent_app_entry_t *entry);
static void _lru_unlink(eos_recent_app_entry_t *entry);
static void _lru_eviction_check(void);
static void _app_uninstalled_cb(eos_event_t *e);
static uint32_t _estimate_entry_mem(eos_recent_app_entry_t *entry);

/* Function Implementations -----------------------------------*/

static void _lru_link_head(eos_recent_app_entry_t *entry)
{
    entry->next = s_head;
    entry->prev = NULL;
    if (s_head)
        s_head->prev = entry;
    s_head = entry;
    if (!s_tail)
        s_tail = entry;
    s_count++;
    s_total_mem_bytes += entry->est_mem_bytes;
}

static void _lru_unlink(eos_recent_app_entry_t *entry)
{
    /* Guard against double-unlink: if both pointers are NULL and this
     * isn't the sole head entry, it was already unlinked. */
    if (!entry->next && !entry->prev && s_head != entry)
    {
        EOS_LOG_W("Attempted to unlink an already-detached entry");
        return;
    }

    if (entry->prev)
        entry->prev->next = entry->next;
    else
        s_head = entry->next;

    if (entry->next)
        entry->next->prev = entry->prev;
    else
        s_tail = entry->prev;

    if (s_count > 0)
        s_count--;
    if (s_total_mem_bytes >= entry->est_mem_bytes)
        s_total_mem_bytes -= entry->est_mem_bytes;
    else
        s_total_mem_bytes = 0;

    entry->next = NULL;
    entry->prev = NULL;
}

static uint32_t _estimate_entry_mem(eos_recent_app_entry_t *e)
{
    if (!e)
        return 0;
    /* Entry struct + overhead estimate for SPM realm + sni_ctx + activity */
    uint32_t struct_bytes = (uint32_t)sizeof(eos_recent_app_entry_t);
    uint32_t overhead = 64 * 1024;
    return struct_bytes + overhead;
}

static void _lru_eviction_check(void)
{
    uint32_t max = (uint32_t)EOS_RECENT_APPS_MAX;
    uint32_t watermark = (uint32_t)EOS_RECENT_APPS_MEM_HIGH_WATERMARK;

    while ((s_count > max || s_total_mem_bytes > watermark) && s_tail)
    {
        EOS_LOG_W("LRU eviction triggered: count=%u (max=%u) mem=%u (limit=%u)",
                  s_count,
                  max,
                  s_total_mem_bytes,
                  watermark);
        if (eos_recent_apps_evict(s_tail) != EOS_OK)
        {
            EOS_LOG_E("LRU eviction stopped because the tail cannot be terminated safely");
            break;
        }
    }
}

/* Public API -------------------------------------------------*/

void eos_recent_apps_init(void)
{
    if (s_initialized)
        return;

    /* Load strategy from Kconfig */
#if defined(EOS_RECENT_APPS_TIMER_STRATEGY)
    s_timer_strategy = (eos_suspend_timer_strategy_t)EOS_RECENT_APPS_TIMER_STRATEGY;
#endif
#if defined(EOS_RECENT_APPS_ANIM_STRATEGY)
    s_anim_strategy = (eos_suspend_anim_strategy_t)EOS_RECENT_APPS_ANIM_STRATEGY;
#endif

    /* Subscribe to app uninstall events for cleanup */
    eos_event_subscribe_ex(EOS_EVENT_APP_UNINSTALLED, _app_uninstalled_cb, NULL, NULL);

    s_initialized = true;
    EOS_LOG_I("Recent Apps initialized: max=%d mem_limit=%d timer_strat=%d anim_strat=%d",
              EOS_RECENT_APPS_MAX,
              EOS_RECENT_APPS_MEM_HIGH_WATERMARK,
              (int)s_timer_strategy,
              (int)s_anim_strategy);
}

/**
 * @brief Internal: suspend an app and register it in the LRU recents list
 *
 * Shared by register_for_suspend() and suspend_current(). Handles:
 *  1. Screenshot capture of snapshot_target
 *  2. Dedup (finds by app_id, evicts old entry if exists)
 *  3. Entry allocation and LRU linking
 *  4. Optional sub-stack detach (with flag marking)
 *  5. SPM program suspend
 *  6. LRU eviction check
 *
 * @param app_root       The AppRoot activity (carries the app_id)
 * @param stack_top      Top of the sub-stack (might equal app_root)
 * @param snapshot_target Activity whose view to screenshot
 * @param depth          Sub-stack depth (1 = just AppRoot)
 * @param detach         If true, mark flags and detach the sub-stack
 * @return EOS_OK on success
 */
static eos_result_t _suspend_and_register(eos_activity_t *app_root,
                                          eos_activity_t *stack_top,
                                          eos_activity_t *snapshot_target,
                                          uint32_t depth,
                                          bool detach)
{
    /* Get the stable app identity from Activity metadata.  Script apps also
     * keep their launch context, but native apps must not use user_data for
     * identity because each native app owns that field. */
    const char *app_id = eos_activity_get_app_id(app_root);
    if (!app_id)
    {
        EOS_LOG_W("AppRoot has no app_id");
        return EOS_FAILED;
    }

    /* Deduplicate: if this app is already in recents, evict the old entry */
    eos_recent_app_entry_t *existing = eos_recent_apps_find(app_id);
    if (existing)
    {
        EOS_LOG_I("App '%s' already in recents, evicting old entry", app_id);
        eos_result_t evict_ret = eos_recent_apps_evict(existing);
        if (evict_ret != EOS_OK)
            return evict_ret;
    }

    /* Allocate entry */
    eos_recent_app_entry_t *entry = eos_malloc_zeroed(sizeof(eos_recent_app_entry_t));
    if (!entry)
    {
        EOS_LOG_E("Failed to allocate recents entry");
        return EOS_FAILED;
    }

    snprintf(entry->app_id, sizeof(entry->app_id), "%s", app_id);

    /* Store the display name for the recents page UI */
    const char *title = eos_activity_get_title(app_root);
    snprintf(entry->app_name, sizeof(entry->app_name), "%s", title ? title : app_id);

    entry->activity = app_root;
    entry->saved_stack_top = stack_top;
    entry->saved_stack_depth = depth;
    entry->program_instance_id = 0;
    script_program_t *program = spm_get_program_by_id_any_state(app_id);
    /* An installed Script App must always have a live SPM program while it is
     * represented by Recent Apps.  Native/system apps are allowed to use the
     * Activity-only path, but silently treating a crashed Script App as a
     * native entry would leave a zombie Recent Apps record that cannot resume. */
    if (!program && eos_app_list_contains(app_id) && !eos_app_list_is_system_app(app_id))
    {
        EOS_LOG_W("Cannot register Script App '%s' without a live SPM program", app_id);
        eos_free(entry);
        return EOS_ERR_INVALID_STATE;
    }
    if (program)
    {
        if (program->type != SCRIPT_TYPE_APPLICATION || program->state != SCRIPT_PROGRAM_STATE_ACTIVE)
        {
            EOS_LOG_W("Cannot suspend '%s': program state=%d type=%d", app_id, program->state, program->type);
            eos_free(entry);
            return EOS_ERR_INVALID_STATE;
        }
        entry->program_instance_id = spm_program_get_instance_id(program);
    }
    entry->last_used_tick = eos_tick_get();
    entry->est_mem_bytes = _estimate_entry_mem(entry);

    /* Capture the app's screenshot for Recent Apps page display.
     * Done BEFORE detach so the view is still visible and rendered.
     * eos_activity_take_snapshot_standalone() returns an eos-allocated
     * full-resolution (390x450) RGB565 draw buffer matching the screen
     * aspect ratio, so the portrait card displays it without stretching and
     * eos_draw_buf_destroy() can free it with the matching allocator. */
    {
        lv_draw_buf_t *thumb = eos_activity_take_snapshot_standalone(snapshot_target, true);
        if (thumb)
        {
            entry->thumb_buf = thumb;
            entry->est_mem_bytes += thumb->data_size;
        }
        else
        {
            EOS_LOG_W("Thumbnail snapshot failed for '%s'", app_id);
        }
    }

    if (detach)
    {
        /* Mark the detached sub-stack as owned by Recent Apps BEFORE detach. */
        eos_activity_t *node = stack_top;
        while (node && node != eos_activity_get_app_substack_next(app_root))
        {
            eos_activity_set_suspended(node, true);
            node = eos_activity_get_app_substack_next(node);
        }

        /* Detach sub-stack from main activity stack (calls on_pause chain) */
        eos_activity_t *detached = eos_activity_detach_app_substack();
        if (!detached)
        {
            EOS_LOG_E("Failed to detach app sub-stack");
            if (entry->thumb_buf)
                eos_draw_buf_destroy(entry->thumb_buf);
            for (eos_activity_t *node = stack_top; node; node = eos_activity_get_app_substack_next(node))
            {
                eos_activity_set_suspended(node, false);
                if (node == app_root)
                    break;
            }
            eos_free(entry);
            return EOS_FAILED;
        }
    }

    /* Activity pause callbacks are UI lifecycle callbacks and must run while
     * the owning program is still ACTIVE.  Commit the SPM transition only
     * after the stack has been detached; otherwise a callback dispatched by a
     * child Activity would be rejected as coming from a suspended program. */
    if (program)
    {
        eos_result_t suspend_ret = spm_suspend_program(program);
        if (suspend_ret != EOS_OK)
        {
            EOS_LOG_W("SPM suspend returned %d for '%s'", suspend_ret, app_id);
            if (detach)
            {
                for (eos_activity_t *node = stack_top; node; node = eos_activity_get_app_substack_next(node))
                {
                    eos_activity_set_suspended(node, false);
                    if (node == app_root)
                        break;
                }
                if (eos_activity_reattach_app_substack(stack_top, NULL) != EOS_OK)
                    EOS_LOG_E("Failed to roll back detached App '%s' after suspend failure", app_id);
            }
            if (entry->thumb_buf)
                eos_draw_buf_destroy(entry->thumb_buf);
            eos_free(entry);
            return suspend_ret;
        }
    }

    if (!program)
        EOS_LOG_I("Native app '%s' suspended through Activity lifecycle", app_id);

    /* Link to LRU head */
    _lru_link_head(entry);

    EOS_LOG_I("App %s: '%s' depth=%u mem=%u total_count=%u total_mem=%u",
              detach ? "suspended" : "registered for suspend",
              entry->app_id,
              depth,
              entry->est_mem_bytes,
              s_count,
              s_total_mem_bytes);

    /* Run LRU eviction check */
    _lru_eviction_check();

    return EOS_OK;
}

eos_result_t eos_recent_apps_register_for_suspend(eos_activity_t *app_activity)
{
    eos_activity_t *current;
    eos_activity_t *app_root;
    uint32_t depth = 1;

    if (!s_initialized || !app_activity)
        return EOS_FAILED;

    if (!eos_recent_apps_is_suspendable(app_activity))
        return EOS_FAILED;

    current = eos_activity_get_current();
    if (current != app_activity)
        return EOS_FAILED;

    app_root = current;
    while (eos_activity_get_app_substack_next(app_root))
    {
        app_root = eos_activity_get_app_substack_next(app_root);
        depth++;
    }

    if (eos_activity_get_type(app_root) != EOS_ACTIVITY_TYPE_APP)
        return EOS_FAILED;

    /* The Activity remains on the main stack until the back transition's
     * cleanup parks it, so the APP→APP_LIST animation can use the live view. */
    return _suspend_and_register(app_root, current, current, depth, false);
}

eos_result_t eos_recent_apps_suspend_current(void)
{
    if (!s_initialized)
    {
        EOS_LOG_W("Recent Apps not initialized");
        return EOS_FAILED;
    }

    eos_activity_t *current = eos_activity_get_current();
    if (!current)
    {
        EOS_LOG_W("No current activity to suspend");
        return EOS_FAILED;
    }

    /* Validate: must be an APP activity or a sub-activity belonging to an app */
    if (!eos_recent_apps_is_suspendable(current))
    {
        EOS_LOG_W("Current activity is not a suspendable app");
        return EOS_FAILED;
    }

    if (eos_activity_is_transition_in_progress())
    {
        EOS_LOG_W("Cannot suspend during transition");
        return EOS_FAILED;
    }

    /* Walk to AppRoot and count depth */
    eos_activity_t *app_root = current;
    uint32_t depth = 1;
    while (eos_activity_get_app_substack_next(app_root))
    {
        app_root = eos_activity_get_app_substack_next(app_root);
        depth++;
    }

    if (eos_activity_get_type(app_root) != EOS_ACTIVITY_TYPE_APP)
    {
        EOS_LOG_W("AppRoot not found");
        return EOS_FAILED;
    }

    /* Explicit suspend: detach the sub-stack, mark flags, suspend SPM */
    return _suspend_and_register(app_root, current, current, depth, true);
}

eos_result_t eos_recent_apps_resume(eos_recent_app_entry_t *entry)
{
    if (!entry)
        return EOS_FAILED;

    if (eos_activity_is_transition_in_progress())
    {
        EOS_LOG_W("Cannot resume during transition");
        return EOS_FAILED;
    }

    EOS_LOG_I("Resuming app: '%s' depth=%u snap_buf=%p",
              entry->app_id,
              entry->saved_stack_depth,
              (void *)entry->snap_buf);

    script_program_t *prog = NULL;
    if (entry->program_instance_id)
    {
        prog = spm_get_program_by_instance_id(entry->program_instance_id);
        eos_activity_t *entry_root = entry->activity;
        const char *program_id = prog ? prog->script.id : NULL;
        if (!prog || prog->type != SCRIPT_TYPE_APPLICATION || prog->state != SCRIPT_PROGRAM_STATE_SUSPENDED
            || !program_id || strcmp(program_id, entry->app_id) != 0 || !entry_root
            || eos_activity_get_app_id(entry_root) == NULL
            || strcmp(eos_activity_get_app_id(entry_root), entry->app_id) != 0
            || eos_activity_get_script_instance_id(entry_root) != entry->program_instance_id)
        {
            EOS_LOG_W("Recent entry '%s' has no matching suspended program", entry->app_id);
            eos_recent_apps_evict(entry);
            return EOS_ERR_INVALID_STATE;
        }
    }

    /* Unlink from LRU list */
    _lru_unlink(entry);

    if (prog)
    {
#if defined(EOS_RECENT_APPS_TIMER_STRATEGY)
        int timer_strategy = EOS_RECENT_APPS_TIMER_STRATEGY;
#else
        int timer_strategy = (int)s_timer_strategy;
#endif
#if defined(EOS_RECENT_APPS_ANIM_STRATEGY)
        int anim_strategy = EOS_RECENT_APPS_ANIM_STRATEGY;
#else
        int anim_strategy = (int)s_anim_strategy;
#endif
        eos_result_t resume_ret = spm_resume_program_with_strategies(prog, timer_strategy, anim_strategy);
        if (resume_ret != EOS_OK)
        {
            EOS_LOG_E("Failed to resume program instance=%u for '%s': %d",
                      entry->program_instance_id,
                      entry->app_id,
                      resume_ret);
            _lru_link_head(entry);
            return resume_ret;
        }
    }

    /* Re-attach sub-stack to main activity stack (calls on_resume chain bottom-up).
     * Pass the stored snapshot so the transition animation shows the actual app
     * screenshot instead of a black placeholder.  Ownership of snap_buf transfers
     * to eos_activity_reattach_app_substack which destroys it after use. */
    if (eos_activity_reattach_app_substack(entry->saved_stack_top, entry->snap_buf) != EOS_OK)
    {
        EOS_LOG_E("Failed to re-attach app sub-stack for '%s'", entry->app_id);
        if (prog)
            spm_suspend_program(prog);
        _lru_link_head(entry);
        return EOS_FAILED;
    }
    entry->snap_buf = NULL;

    entry->activity = NULL;
    entry->saved_stack_top = NULL;

    /* Free the Recent Apps page thumbnail — no longer needed after resume */
    if (entry->thumb_buf)
    {
        eos_draw_buf_destroy(entry->thumb_buf);
        entry->thumb_buf = NULL;
    }

    eos_free(entry);

    EOS_LOG_I("App resumed successfully");
    return EOS_OK;
}

eos_result_t eos_recent_apps_resume_by_id(const char *app_id)
{
    eos_recent_app_entry_t *entry = eos_recent_apps_find(app_id);
    if (!entry)
        return EOS_FAILED;
    return eos_recent_apps_resume(entry);
}

eos_recent_app_entry_t *eos_recent_apps_find(const char *app_id)
{
    if (!app_id)
        return NULL;
    eos_recent_app_entry_t *entry = s_head;
    while (entry)
    {
        if (strcmp(entry->app_id, app_id) == 0)
            return entry;
        entry = entry->next;
    }
    return NULL;
}

eos_result_t eos_recent_apps_evict(eos_recent_app_entry_t *entry)
{
    if (!entry)
        return EOS_FAILED;

    EOS_LOG_I("Evicting app: '%s'", entry->app_id);

    _lru_unlink(entry);

    if (entry->program_instance_id)
    {
        script_program_t *prog = spm_get_program_by_instance_id(entry->program_instance_id);
        if (prog)
        {
            if (prog->type != SCRIPT_TYPE_APPLICATION || prog->state != SCRIPT_PROGRAM_STATE_SUSPENDED)
            {
                /* A Recent entry never owns an ACTIVE or STOPPING program.
                 * Refuse to tear down a live foreground generation merely to
                 * make an inconsistent entry disappear. */
                EOS_LOG_E("Cannot evict entry '%s': program instance=%u state=%d type=%d",
                          entry->app_id,
                          entry->program_instance_id,
                          prog->state,
                          prog->type);
                _lru_link_head(entry);
                return prog->state == SCRIPT_PROGRAM_STATE_STOPPING ? EOS_ERR_BUSY : EOS_ERR_INVALID_STATE;
            }
            eos_result_t ret = spm_app_stop_by_instance_id(entry->program_instance_id);
            if (ret != EOS_OK)
            {
                EOS_LOG_E("Failed to evict program instance=%u: %d", entry->program_instance_id, ret);
                _lru_link_head(entry);
                return ret;
            }
        }
    }

    /* Destroy the entire sub-stack: walk app_substack_next chain from top down.
     * Use eos_activity_destroy() which handles the full teardown including
     * on_destroy lifecycle, view cleanup, and memory free. */
    eos_activity_t *node = entry->saved_stack_top;
    while (node)
    {
        eos_activity_t *next = eos_activity_get_app_substack_next(node);

        /* Clear suspended flag so _activity_run_destroy proceeds */
        eos_activity_set_suspended(node, false);

        /* Destroy the activity (calls on_destroy, deletes view, frees memory).
         * For the AppRoot, on_destroy sees the already-terminated instance and
         * cannot affect a later program with the same app_id. */
        eos_activity_destroy(node);

        if (node == entry->activity)
            break;
        node = next;
    }

    /* Free the stored snapshot if it was never used for resume */
    if (entry->snap_buf)
    {
        eos_draw_buf_destroy(entry->snap_buf);
        entry->snap_buf = NULL;
    }

    /* Free the Recent Apps page thumbnail */
    if (entry->thumb_buf)
    {
        eos_draw_buf_destroy(entry->thumb_buf);
        entry->thumb_buf = NULL;
    }

    eos_free(entry);
    return EOS_OK;
}

void eos_recent_apps_clear_all(void)
{
    while (s_head)
    {
        if (eos_recent_apps_evict(s_head) != EOS_OK)
        {
            EOS_LOG_E("Unable to clear all Recent Apps entries safely");
            break;
        }
    }
    if (!s_head)
    {
        s_count = 0;
        s_total_mem_bytes = 0;
        EOS_LOG_I("All recents entries cleared");
    }
    else
    {
        EOS_LOG_W("Recent Apps clear stopped with entries still present");
    }
}

void eos_recent_apps_on_engine_reset(void)
{
    EOS_LOG_W("Engine reset detected — clearing recents");
    /* Walk and evict all entries. The programs are already destroyed
     * by spm_handle_engine_reset, so we must destroy the parked activity
     * objects (views, launch contexts) that would otherwise leak. */
    eos_recent_app_entry_t *entry = s_head;
    while (entry)
    {
        eos_recent_app_entry_t *next = entry->next;
        /* Destroy every parked activity, not just the root.  The saved chain
         * owns the complete navigation state and each page has its own view
         * and lifecycle resources. */
        eos_activity_t *node = entry->saved_stack_top ? entry->saved_stack_top : entry->activity;
        while (node)
        {
            eos_activity_t *next = eos_activity_get_app_substack_next(node);
            eos_activity_set_suspended(node, false);
            eos_activity_destroy(node);
            if (node == entry->activity)
                break;
            node = next;
        }
        /* Free the stored snapshot if the app never got a chance to resume */
        if (entry->snap_buf)
        {
            eos_draw_buf_destroy(entry->snap_buf);
            entry->snap_buf = NULL;
        }
        /* Free the Recent Apps page thumbnail */
        if (entry->thumb_buf)
        {
            eos_draw_buf_destroy(entry->thumb_buf);
            entry->thumb_buf = NULL;
        }
        eos_free(entry);
        entry = next;
    }
    s_head = NULL;
    s_tail = NULL;
    s_count = 0;
    s_total_mem_bytes = 0;
}

void eos_recent_apps_set_timer_strategy(eos_suspend_timer_strategy_t strategy)
{
    s_timer_strategy = strategy;
}

void eos_recent_apps_set_anim_strategy(eos_suspend_anim_strategy_t strategy)
{
    s_anim_strategy = strategy;
}

uint32_t eos_recent_apps_count(void)
{
    return s_count;
}

eos_recent_app_entry_t *eos_recent_apps_get_head(void)
{
    return s_head;
}

eos_recent_app_entry_t *eos_recent_apps_get_next(eos_recent_app_entry_t *entry)
{
    return entry ? entry->next : NULL;
}

bool eos_recent_apps_is_suspendable(eos_activity_t *activity)
{
    if (!activity)
        return false;
    return eos_activity_get_app_id(activity) != NULL;
}

/* Event Handlers ---------------------------------------------*/

static void _app_uninstalled_cb(eos_event_t *e)
{
    const char *app_id = (const char *)eos_event_get_param(e);
    if (!app_id)
        return;
    EOS_LOG_I("App uninstalled: '%s' — checking recents", app_id);
    eos_recent_app_entry_t *entry = eos_recent_apps_find(app_id);
    if (entry)
    {
        EOS_LOG_I("Evicting recents entry for uninstalled app: '%s'", app_id);
        eos_recent_apps_evict(entry);
    }
}

#endif /* EOS_RECENT_APPS_ENABLE */
