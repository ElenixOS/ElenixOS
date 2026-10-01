/**
 * @file sni_types.h
 * @brief SNI runtime context, ownership records and teardown state
 */

#ifndef SNI_TYPES_H
#define SNI_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "jerryscript.h"
#include "sni_type_ids.h"
/* Runtime data types -----------------------------------------*/

/**
 * @brief Property structure
 *
 * Value objects are defined using property structure arrays
 *
 * Example:
 * ```c
 * const sni_val_prop_t lv_point_props[] = {
 *     {"x", SNI_T_INT32, offsetof(lv_point_t, x)},
 *     {"y", SNI_T_INT32, offsetof(lv_point_t, y)},
 * };
 * ```
 */
typedef struct
{
    const char *name; /**< Property name */
    sni_type_t type; /**< Property type */
    size_t offset; /**< Property offset in value object structure */
    uint8_t bit_width; /**< Bit width for bit field members (0 for non-bit field members) */
} sni_val_prop_t;

typedef struct
{
    sni_type_t type;
    uint16_t prop_count; /**< Property count */
    const sni_val_prop_t *props; /**< Property array pointer */
} sni_val_obj_t;

/**
 * @brief Control block for Object Tree Nodes only
 *
 * Bridges JS objects and native LVGL objects with bidirectional O(1) access.
 *
 * For Object Tree Nodes (SNI_H_LV_OBJ):
 *   - JS object -> native_ptr -> sni_control_block_t -> ptr (C object)
 *   - LVGL object -> user_data -> sni_control_block_t -> obj (JS object)
 *
 * Note: Managed Resources do NOT use control blocks. They store data directly
 * in sni_managed_resource_node_t for flattened memory layout.
 *
 * Sub-resources (e.g., chart series added via lv_chart_add_series) are
 * managed resource nodes linked via sub_resource_head.  When the parent
 * object is deleted (LV_EVENT_DELETE) the sub-resource list is walked
 * and every sub-handle is marked dead.
 */
struct sni_managed_resource_node;
typedef struct sni_control_block
{
    void *ptr; /**< Pointer to native C object */
    jerry_value_t js_obj; /**< JavaScript object corresponding to the handle */
    sni_type_t type; /**< Handle type for runtime validation */
    bool is_alive; /**< Whether the native object is still alive */
    void *aux; /**< Module-private auxiliary context */
    struct sni_context *owner_ctx; /**< Owning SNI context (Realm) */
    uint32_t engine_gen; /**< Engine generation at creation time */
    struct sni_managed_resource_node *sub_resource_head; /**< Linked list of sub-resource handles */
} sni_control_block_t;

typedef void (*sni_handle_destroy_cb_t)(void *native_ptr);

/**
 * @brief Managed resource linked list node (flattened data structure)
 *
 * Stores native pointer, JS object reference, and metadata directly without
 * indirection through control block. This eliminates redundant ptr storage
 * and simplifies memory management for managed resources.
 *
 * Used to organize managed resources by type within a Realm context.
 * Each type has its own linked list for O(n/k) lookup where k is the number
 * of resource types (much smaller than total resource count).
 */
typedef struct sni_managed_resource_node
{
    void *ptr; /**< Pointer to native resource */
    jerry_value_t js_obj; /**< JavaScript object (was in control block) */
    sni_type_t type; /**< Resource type (was in control block) */
    bool is_alive; /**< Lifecycle status (was in control block) */
    struct sni_managed_resource_node *next; /**< Next node in type-specific list */
    struct sni_control_block *parent_cb; /**< Parent control block (only for sub-resources) */
    struct sni_managed_resource_node *parent_next; /**< Next node in the parent's sub-resource list */
} sni_managed_resource_node_t;

/**
 * @brief Teardown phase state machine
 *
 * Tracks the current phase during Realm destruction.  Each phase restricts
 * which APIs are safe to call.  See sni.mdx "Realm 销毁顺序" for the full
 * specification.  The sweep (sni_context_sweep_all) advances through 4a→4d.
 */
typedef enum
{
    SNI_TEARDOWN_PHASE_NONE = 0, /**< Normal runtime — all APIs allowed */
    SNI_TEARDOWN_PHASE_JS_DECOUPLE, /**< Phase 0: sni_context_clear_native_ptrs_all */
    SNI_TEARDOWN_PHASE_JS_REFS, /**< Phase 1: sni_context_sweep_js_refs */
    SNI_TEARDOWN_PHASE_LVGL_EVENTS, /**< Phase 2: sni_cb_context_cleanup_events */
    SNI_TEARDOWN_PHASE_ENGINE_STOPPED, /**< Phase 3: script_engine_stop — no jerry_value_free */
    SNI_TEARDOWN_PHASE_SWEEP_TREE_DEP, /**< Phase 4a: Tree-Dependent unlinking */
    SNI_TEARDOWN_PHASE_SWEEP_HYBRID, /**< Phase 4b: Hybrid (Activity) destruction */
    SNI_TEARDOWN_PHASE_SWEEP_PURE, /**< Phase 4c: Pure Managed native destruction */
    SNI_TEARDOWN_PHASE_SWEEP_VALUE_LIKE, /**< Phase 4d: Value-like node cleanup */
    SNI_TEARDOWN_PHASE_CTX_DESTROY, /**< Phase 5: sni_context_destroy */
    SNI_TEARDOWN_PHASE_COMPLETE, /**< Phase 6+: LVGL tree deletion — no SNI access */
} sni_teardown_phase_t;

/**
 * @brief Per-Realm SNI context
 *
 * Maintains type-indexed linked lists of managed resources for lifecycle management.
 * Object Tree Nodes are NOT stored here - they use LVGL's user_data mechanism.
 *
 * Array index = type - __SNI_HANDLE_RESOURCE_START - 1
 */
typedef struct sni_context
{
    sni_managed_resource_node_t *resource_heads[SNI_MANAGED_RESOURCE_COUNT];
    int resource_counts[SNI_MANAGED_RESOURCE_COUNT];
    void *event_ctx_list;
    void *sensor_request_ctx_list;
    void *metric_subscription_ctx_list;
    struct script_program *owner;
    bool paused;
    sni_teardown_phase_t teardown_phase; /**< Current phase during Realm destruction */
} sni_context_t;

#ifdef __cplusplus
}
#endif

#endif /* SNI_TYPES_H */
