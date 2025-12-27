/**
 * @file lvgl_js_bridge.c
 * @brief LVGL 与 JS 的桥接层
 *
 * LVGL <=[ BRIDGE ]=> JerryScript
 *
 * @author Sab1e
 * @date 2025-12-21
 */

#include "lvgl_js_bridge.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#define EOS_LOG_TAG "LVGL-JS-Bridge"
#include "elena_os_log.h"
#include "lvgl.h"
#include "uthash.h"
#include "elena_os_mem.h"
#include "script_engine_core.h"

/* Macros and Definitions -------------------------------------*/
#define BINDING_OBJ script_engine_eos_obj

typedef struct lv_js_handle
{
    void *ptr;
    jerry_value_t js_obj;
    lv_type_t type;
    bool is_alive;
    UT_hash_handle hh;
} lv_js_handle_t;

/* Variables --------------------------------------------------*/
extern jerry_value_t script_engine_eos_obj;
static lv_js_handle_t *lv_js_map = NULL;
static void lv_js_handle_free_cb(void *native_p, struct jerry_object_native_info_t *info_p);
static const jerry_object_native_info_t lv_js_native_info =
    {
        .free_cb = lv_js_handle_free_cb,
        .number_of_references = 0,
        .offset_of_references = 0,
};
/* Function Implementations -----------------------------------*/

/************************** 哈希表存储已知指针 **************************/

static lv_js_handle_t *lv_js_map_insert(void *ptr, jerry_value_t js_obj, lv_type_t type)
{
    lv_js_handle_t *handle = eos_malloc_zeroed(sizeof(lv_js_handle_t));
    if (!handle)
    {
        return NULL;
    }

    handle->ptr = ptr;
    handle->js_obj = jerry_value_copy(js_obj);
    handle->type = type;
    handle->is_alive = true;

    HASH_ADD_PTR(lv_js_map, ptr, handle);
    return handle;
}

static lv_js_handle_t *lv_js_map_find(void *ptr)
{
    lv_js_handle_t *handle = NULL;
    HASH_FIND_PTR(lv_js_map, &ptr, handle);
    return handle;
}

static void lv_js_map_remove(void *ptr)
{
    lv_js_handle_t *handle = NULL;
    HASH_FIND_PTR(lv_js_map, &ptr, handle);

    if (!handle)
    {
        return;
    }

    HASH_DEL(lv_js_map, handle);
    jerry_value_free(handle->js_obj);
    eos_free(handle);
}

/************************** 指针对象桥 **************************/

static void lv_js_handle_free_cb(void *native_p, struct jerry_object_native_info_t *info_p)
{
    lv_js_handle_t *handle = (lv_js_handle_t *)native_p;

    EOS_CHECK_PTR_RETURN(handle);

    // 标记失效，避免 C 侧继续使用
    handle->is_alive = false;

    // 从 uthash 中移除
    HASH_DEL(lv_js_map, handle);

    // 释放你分配的 handle
    eos_free(handle);

    EOS_LOG_D("handle freed: %p", handle);
}

lv_type_t lv_js_bridge_obj_get_type(const jerry_value_t obj)
{
    if (jerry_value_is_null(obj) || jerry_value_is_undefined(obj) || !jerry_value_is_object(obj))
    {
        return LV_TYPE_UNKNWON;
    }

    lv_js_handle_t *handle =
        jerry_object_get_native_ptr(obj, &lv_js_native_info);

    if (!handle || !handle->is_alive)
    {
        return LV_TYPE_UNKNWON;
    }

    return handle->type;
}

void *lv_js_bridge_obj_2_ptr(const jerry_value_t obj, lv_type_t type)
{
    if (jerry_value_is_null(obj) || jerry_value_is_undefined(obj))
    {
        EOS_LOG_W("NULL obj");
        return NULL;
    }

    if (!jerry_value_is_object(obj))
    {
        script_engine_throw_error("obj must be an object or null");
        return NULL;
    }

    lv_js_handle_t *handle =
        jerry_object_get_native_ptr(obj, &lv_js_native_info);

    if (!handle)
    {
        script_engine_throw_error("Invalid native object");
        return NULL;
    }

    if (!handle->is_alive)
    {
        script_engine_throw_error("Object already deleted");
        return NULL;
    }

    if (handle->type != type)
    {
        script_engine_throw_error("Type mismatch");
        return NULL;
    }

    return handle->ptr;
}

jerry_value_t lv_js_bridge_ptr_2_obj(void *ptr, lv_type_t type)
{
    if (!ptr)
    {
        return jerry_null();
    }

    lv_js_handle_t *handle = lv_js_map_find(ptr);
    if (handle)
    {
        return jerry_value_copy(handle->js_obj);
    }

    jerry_value_t js_obj = jerry_object();

    lv_js_handle_t *new_handle =
        lv_js_map_insert(ptr, js_obj, type);

    jerry_object_set_native_ptr(js_obj, &lv_js_native_info, new_handle);
    return js_obj;
}
