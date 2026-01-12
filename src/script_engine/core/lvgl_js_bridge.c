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
#include "lv_bindings_anim.h"

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

/************************** 工具函数 **************************/

uint32_t get_u32_prop(jerry_value_t obj, const char *name)
{
    jerry_value_t key = jerry_string_sz(name);
    jerry_value_t val = jerry_object_get(obj, key);

    uint32_t ret = 0;

    if (jerry_value_is_number(val))
    {
        ret = (uint32_t)jerry_value_as_uint32(val);
    }

    jerry_value_free(val);
    jerry_value_free(key);
    return ret;
}

uint8_t get_u8_prop(jerry_value_t obj, const char *name)
{
    (uint8_t)get_u32_prop(obj, name);
}

int32_t get_i32_prop(jerry_value_t obj, const char *name)
{
    jerry_value_t key = jerry_string_sz(name);
    jerry_value_t val = jerry_object_get(obj, key);

    int32_t ret = 0;

    if (jerry_value_is_number(val))
    {
        ret = jerry_value_as_int32(val);
    }

    jerry_value_free(val);
    jerry_value_free(key);
    return ret;
}

jerry_value_t get_object_prop(jerry_value_t obj, const char *name)
{
    jerry_value_t key = jerry_string_sz(name);
    jerry_value_t val = jerry_object_get(obj, key);

    jerry_value_t ret = jerry_null();

    if (jerry_value_is_object(val))
    {
        ret = jerry_value_copy(val);
    }

    jerry_value_free(val);
    jerry_value_free(key);
    return ret;
}

static void set_u32_prop(jerry_value_t obj, const char *name, uint32_t v)
{
    jerry_value_t key = jerry_string_sz((const jerry_char_t *)name);
    jerry_value_t val = jerry_number(v);
    jerry_value_free(jerry_object_set(obj, key, val));
    jerry_value_free(key);
    jerry_value_free(val);
}

static void set_u8_prop(jerry_value_t obj, const char *name, uint8_t v)
{
    jerry_value_t key = jerry_string_sz((const jerry_char_t *)name);
    jerry_value_t val = jerry_number(v);
    jerry_value_free(jerry_object_set(obj, key, val));
    jerry_value_free(key);
    jerry_value_free(val);
}
static void set_i32_prop(jerry_value_t obj, const char *name, int32_t v)
{
    jerry_value_t key = jerry_string_sz((const jerry_char_t *)name);
    jerry_value_t val = jerry_number(v);
    jerry_value_free(jerry_object_set(obj, key, val));
    jerry_value_free(key);
    jerry_value_free(val);
}

void set_object_prop(jerry_value_t obj,
                     const char *name,
                     jerry_value_t func)
{
    jerry_value_t key = jerry_string_sz(name);
    jerry_value_t val = jerry_value_copy(func);
    jerry_value_free(jerry_object_set(obj, key, val));
    jerry_value_free(key);
    jerry_value_free(val);
}

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

    if (handle->type != type && handle->type != LV_TYPE_ANY && type != LV_TYPE_ANY)
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

lv_anim_t lv_js_bridge_obj_2_anim(jerry_value_t obj)
{
    lv_anim_t a = {0};
    lv_anim_init(&a);

    if (jerry_value_is_null(obj) || jerry_value_is_undefined(obj))
    {
        EOS_LOG_W("NULL obj");
        return (lv_anim_t){0};
    }

    if (!jerry_value_is_object(obj))
    {
        script_engine_throw_error("obj must be an object");
        return (lv_anim_t){0};
    }

    // var
    jerry_value_t var = get_object_prop(obj, "var");
    if (jerry_value_is_object(var))
    {
        a.var = lv_js_bridge_obj_2_ptr(var, LV_TYPE_ANY);
    }
    jerry_value_free(var);

    // 分配 user_data
    a.user_data = eos_malloc_zeroed(sizeof(js_anim_user_data_t));
    if (a.user_data == NULL)
    {
        script_engine_throw_error("anim user_data alloc failed");
        return (lv_anim_t){0};
    }

    js_anim_user_data_t *user_data = a.user_data;
    user_data->anim_obj = jerry_value_copy(obj);

    // JS userData
    jerry_value_t js_user_data = get_object_prop(obj, "userData");
    if (jerry_value_is_object(js_user_data))
    {
        user_data->user_data = lv_js_bridge_obj_2_ptr(js_user_data, LV_TYPE_ANY);
    }
    jerry_value_free(js_user_data);

    // customExecCb
    jerry_value_t cb = get_object_prop(obj, "customExecCb");
    if (jerry_value_is_function(cb))
    {
        a.custom_exec_cb = lv_bindings_anim_custom_exec_cb_handler;
        user_data->custom_exec_cb = jerry_value_copy(cb);
    }
    jerry_value_free(cb);

    // startCb
    cb = get_object_prop(obj, "startCb");
    if (jerry_value_is_function(cb))
    {
        a.start_cb = lv_bindings_anim_start_cb_handler;
        user_data->start_cb = jerry_value_copy(cb);
    }
    jerry_value_free(cb);

    // completedCb
    cb = get_object_prop(obj, "completedCb");
    if (jerry_value_is_function(cb))
    {
        a.completed_cb = lv_bindings_anim_completed_cb_handler;
        user_data->completed_cb = jerry_value_copy(cb);
    }
    jerry_value_free(cb);

    // deletedCb
    cb = get_object_prop(obj, "deletedCb");
    if (jerry_value_is_function(cb))
    {
        a.deleted_cb = lv_bindings_anim_deleted_cb_handler;
        user_data->deleted_cb = jerry_value_copy(cb);
    }
    jerry_value_free(cb);

    // getValue
    cb = get_object_prop(obj, "getValue");
    if (jerry_value_is_function(cb))
    {
        a.get_value_cb = lv_bindings_anim_get_value_cb_handler;
        user_data->get_value_cb = jerry_value_copy(cb);
    }
    jerry_value_free(cb);

    // pathCb
    jerry_value_t key = jerry_string_sz("pathCb");
    jerry_value_t val = jerry_object_get(obj, key);

    if (jerry_value_is_number(val))
    {
        uint32_t id = jerry_value_as_uint32(val);
        a.path_cb = lv_bindings_anim_get_path_cb(id);
        user_data->path_cb_id = id;
        user_data->path_cb = jerry_undefined();
    }
    else if (jerry_value_is_function(val))
    {
        a.path_cb = lv_bindings_anim_path_cb_hander;
        user_data->path_cb_id = 0;
        user_data->path_cb = jerry_value_copy(val);
    }

    jerry_value_free(val);
    jerry_value_free(key);

    // 基本参数
    a.start_value = get_i32_prop(obj, "startValue");
    a.current_value = get_i32_prop(obj, "currentValue");
    a.end_value = get_i32_prop(obj, "endValue");
    a.duration = get_i32_prop(obj, "duration");
    a.act_time = get_i32_prop(obj, "actTime");

    a.playback_delay = get_u32_prop(obj, "playbackDelay");
    a.playback_duration = get_u32_prop(obj, "playbackDuration");
    a.repeat_delay = get_u32_prop(obj, "repeatDelay");
    a.repeat_cnt = get_u32_prop(obj, "repeatCnt");

    jerry_value_t bezier3 = get_object_prop(obj, "bezier3");
    if (jerry_value_is_object(bezier3))
    {
        a.parameter.bezier3.x1 = get_i32_prop(bezier3, "x1");
        a.parameter.bezier3.y1 = get_i32_prop(bezier3, "y1");
        a.parameter.bezier3.x2 = get_i32_prop(bezier3, "x2");
        a.parameter.bezier3.y2 = get_i32_prop(bezier3, "y2");
    }
    jerry_value_free(bezier3);

    return a;
}

jerry_value_t lv_js_bridge_anim_2_obj(lv_anim_t *a)
{
    if (!a)
    {
        return jerry_null();
    }

    jerry_value_t obj = jerry_object();

    // var
    if (a->var)
    {
        jerry_value_t var_obj = lv_js_bridge_ptr_2_obj(a->var, LV_TYPE_ANY);
        jerry_value_t key = jerry_string_sz("var");
        jerry_value_free(jerry_object_set(obj,
                                          key,
                                          var_obj));
        jerry_value_free(var_obj);
    }

    // 数值字段
    set_i32_prop(obj, "startValue", a->start_value);
    set_i32_prop(obj, "currentValue", a->current_value);
    set_i32_prop(obj, "endValue", a->end_value);
    set_i32_prop(obj, "duration", a->duration);
    set_i32_prop(obj, "actTime", a->act_time);

    set_u32_prop(obj, "playbackDelay", a->playback_delay);
    set_u32_prop(obj, "playbackDuration", a->playback_duration);
    set_u32_prop(obj, "repeatDelay", a->repeat_delay);
    set_u32_prop(obj, "repeatCnt", a->repeat_cnt);

    // bezier3
    jerry_value_t bezier3 = jerry_object();
    set_i32_prop(bezier3, "x1", a->parameter.bezier3.x1);
    set_i32_prop(bezier3, "y1", a->parameter.bezier3.y1);
    set_i32_prop(bezier3, "x2", a->parameter.bezier3.x2);
    set_i32_prop(bezier3, "y2", a->parameter.bezier3.y2);

    jerry_value_t key = jerry_string_sz("bezier3");
    jerry_value_free(jerry_object_set(obj,
                                      key,
                                      bezier3));
    jerry_value_free(key);
    jerry_value_free(bezier3);

    // user_data（JS Anim 用户数据）
    if (a->user_data)
    {
        js_anim_user_data_t *ud = (js_anim_user_data_t *)a->user_data;

        // JS callbacks
        if (jerry_value_is_function(ud->custom_exec_cb))
            set_object_prop(obj, "customExecCb", ud->custom_exec_cb);

        if (jerry_value_is_function(ud->start_cb))
            set_object_prop(obj, "startCb", ud->start_cb);

        if (jerry_value_is_function(ud->completed_cb))
            set_object_prop(obj, "completedCb", ud->completed_cb);

        if (jerry_value_is_function(ud->deleted_cb))
            set_object_prop(obj, "deletedCb", ud->deleted_cb);

        if (jerry_value_is_function(ud->get_value_cb))
            set_object_prop(obj, "getValue", ud->get_value_cb);

        if (ud->path_cb_id != 0)
        {
            set_u32_prop(obj, "pathCb", ud->path_cb_id);
        }
        else if ((!jerry_value_is_undefined(ud->path_cb)) && jerry_value_is_function(ud->path_cb))
            set_object_prop(obj, "pathCb", ud->path_cb);

        // userData
        if (ud->user_data)
        {
            jerry_value_t js_ud = lv_js_bridge_ptr_2_obj(ud->user_data, LV_TYPE_ANY);
            jerry_value_t key = jerry_string_sz("userData");
            jerry_value_free(jerry_object_set(obj,
                                              key,
                                              js_ud));
            jerry_value_free(key);
            jerry_value_free(js_ud);
        }
    }

    return obj;
}

lv_color_t lv_js_bridge_obj_2_color(jerry_value_t obj)
{
    lv_color_t c;

    if (jerry_value_is_null(obj) || jerry_value_is_undefined(obj))
    {
        EOS_LOG_W("NULL obj");
        return (lv_color_t){0};
    }

    if (!jerry_value_is_object(obj))
    {
        script_engine_throw_error("obj must be an object or null");
        return (lv_color_t){0};
    }

    c.red = get_u32_prop(obj, "r");
    c.green = get_u32_prop(obj, "g");
    c.blue = get_u32_prop(obj, "b");

    return c;
}

jerry_value_t lv_js_bridge_color_2_obj(lv_color_t *c)
{
    if (!c)
    {
        return jerry_null();
    }
    jerry_value_t obj = jerry_object();
    set_u8_prop(obj, "r", c->red);
    set_u8_prop(obj, "g", c->green);
    set_u8_prop(obj, "b", c->blue);
    return obj;
}
