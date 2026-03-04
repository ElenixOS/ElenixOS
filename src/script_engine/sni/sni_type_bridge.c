/**
 * @file sni_type_bridge.c
 * @brief 类型桥
 * @author Sab1e
 * @date 2026-02-09
 */

#include "sni_type_bridge.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "sni_types.h"
#include "elena_os_mem.h"
#include "script_engine_core.h"
#include "jerryscript.h"
#include "sni_lv_types.h"
#include "elena_os_event.h"
/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/
static sni_val_obj_t sni_val_objs[__SNI_TYPE_MAX] = {0};
static sni_handle_t *sni_map = NULL;
static void sni_handle_free_cb(void *native_p, struct jerry_object_native_info_t *info_p);
static const jerry_object_native_info_t sni_native_info =
    {
        .free_cb = sni_handle_free_cb,
        .number_of_references = 0,
        .offset_of_references = 0,
};
static sni_handle_destroy_cb_t sni_handle_destroy_cb[SNI_HANDLE_LC_REALM_COUNT] = {0};

/* Function Implementations -----------------------------------*/

static void sni_tb_write_bitfield(void *ptr, uint8_t bit_offset, uint8_t bit_width, uint32_t value)
{
    uint8_t *byte_ptr = (uint8_t *)ptr;
    uint32_t mask = (1U << bit_width) - 1;
    value &= mask;

    uint32_t *word_ptr = (uint32_t *)byte_ptr;
    uint32_t word_value = *word_ptr;

    word_value &= ~(mask << bit_offset);
    word_value |= (value << bit_offset);

    *word_ptr = word_value;
}

static uint32_t sni_tb_read_bitfield(void *ptr, uint8_t bit_offset, uint8_t bit_width)
{
    uint8_t *byte_ptr = (uint8_t *)ptr;
    uint32_t *word_ptr = (uint32_t *)byte_ptr;
    uint32_t word_value = *word_ptr;

    uint32_t mask = (1U << bit_width) - 1;
    return (word_value >> bit_offset) & mask;
}

/************************** Handle 管理函数 **************************/

static sni_handle_t *sni_map_insert(void *ptr, jerry_value_t js_obj, sni_type_t type)
{
    sni_handle_t *handle = eos_malloc_zeroed(sizeof(sni_handle_t));
    if (!handle)
    {
        return NULL;
    }

    handle->ptr = ptr;
    handle->js_obj = js_obj;
    handle->type = type;
    handle->is_alive = true;

    HASH_ADD_PTR(sni_map, ptr, handle);
    return handle;
}

static sni_handle_t *sni_map_find(void *ptr)
{
    sni_handle_t *handle = NULL;
    HASH_FIND_PTR(sni_map, &ptr, handle);
    return handle;
}

static void sni_map_remove(void *ptr)
{
    sni_handle_t *handle = NULL;
    HASH_FIND_PTR(sni_map, &ptr, handle);

    if (!handle)
    {
        return;
    }

    HASH_DEL(sni_map, handle);
    eos_free(handle);
}

static void sni_handle_free_cb(void *native_p, struct jerry_object_native_info_t *info_p)
{
    sni_handle_t *handle = (sni_handle_t *)native_p;

    if (!handle)
    {
        return;
    }

    handle->is_alive = false;

    HASH_DEL(sni_map, handle);

    eos_free(handle);
}

/************************** 类型桥函数 **************************/

const char *sni_tb_js2c_string(jerry_value_t js_val)
{
    if (!jerry_value_is_string(js_val))
    {
        return NULL;
    }

    jerry_size_t str_len = jerry_string_size(js_val, JERRY_ENCODING_UTF8);

    char *string = eos_malloc(str_len + 1);
    if (!string)
    {
        return NULL;
    }

    jerry_string_to_buffer(
        js_val,
        JERRY_ENCODING_UTF8,
        (jerry_char_t *)string,
        str_len);

    string[str_len] = '\0';
    return string;
}

bool sni_tb_js2c(jerry_value_t js_val, sni_type_t type, void *out_obj)
{
    if (out_obj == NULL)
    {
        return false;
    }

    if (SNI_TYPE_IS_VALUE(type))
    {
        return false;
    }

    if (jerry_value_is_undefined(js_val) || jerry_value_is_null(js_val))
    {
        return false;
    }

    switch (type)
    {
    case SNI_T_UINT32:
        if (!jerry_value_is_number(js_val))
        {
            return false;
        }
        *(uint32_t *)out_obj = (uint32_t)jerry_value_as_number(js_val);
        return true;

    case SNI_T_INT32:
        if (!jerry_value_is_number(js_val))
        {
            return false;
        }
        *(int32_t *)out_obj = jerry_value_as_int32(js_val);
        return true;

    case SNI_T_DOUBLE:
        if (!jerry_value_is_number(js_val))
        {
            return false;
        }
        *(double *)out_obj = jerry_value_as_number(js_val);
        return true;

    case SNI_T_BOOL:
        *(bool *)out_obj = jerry_value_to_boolean(js_val);
        return true;

    case SNI_T_STRING:
        if (!jerry_value_is_string(js_val))
        {
            return false;
        }
        *(const char **)out_obj = sni_tb_js2c_string(js_val);
        return (*(const char **)out_obj != NULL);

    case SNI_T_PTR:
        if (!jerry_value_is_number(js_val))
        {
            return false;
        }
        *(void **)out_obj = (void *)(uintptr_t)jerry_value_as_number(js_val);
        return true;

    default:
        break;
    }

    if (SNI_TYPE_IS_VALUE(type))
    {
        if (!jerry_value_is_object(js_val))
        {
            return false;
        }

        const sni_val_obj_t *val_obj = &sni_val_objs[type];
        if (val_obj->prop_count == 0 || val_obj->props == NULL)
        {
            return false;
        }

        uint8_t current_bit_offset = 0;
        size_t last_offset = 0;

        for (uint16_t i = 0; i < val_obj->prop_count; i++)
        {
            const sni_val_prop_t *prop = &val_obj->props[i];

            jerry_value_t js_prop =
                jerry_object_get_sz(js_val, prop->name);

            if (jerry_value_is_exception(js_prop))
            {
                jerry_value_free(js_prop);
                return false;
            }

            void *field_ptr = (uint8_t *)out_obj + prop->offset;

            if (prop->offset != last_offset)
            {
                current_bit_offset = 0;
                last_offset = prop->offset;
            }

            bool ok;
            if (prop->bit_width > 0)
            {
                if (!jerry_value_is_number(js_prop))
                {
                    jerry_value_free(js_prop);
                    return false;
                }
                uint32_t value = (uint32_t)jerry_value_as_number(js_prop);
                sni_tb_write_bitfield(field_ptr, current_bit_offset, prop->bit_width, value);
                current_bit_offset += prop->bit_width;
                ok = true;
            }
            else
            {
                ok = sni_tb_js2c(
                    js_prop,
                    prop->type,
                    field_ptr);
            }

            jerry_value_free(js_prop);

            if (!ok)
            {
                return false;
            }
        }

        return true;
    }

    if (SNI_TYPE_IS_HANDLE(type))
    {
        if (jerry_value_is_null(js_val) || jerry_value_is_undefined(js_val))
        {
            *(void **)out_obj = NULL;
            return true;
        }

        if (!jerry_value_is_object(js_val))
        {
            return false;
        }

        sni_handle_t *handle =
            jerry_object_get_native_ptr(js_val, &sni_native_info);

        if (!handle)
        {
            return false;
        }

        if (!handle->is_alive)
        {
            return false;
        }

        if (handle->type != type)
        {
            return false;
        }

        *(void **)out_obj = handle->ptr;
        return true;
    }

    return false;
}

jerry_value_t sni_tb_c2js(void *c_val, sni_type_t type)
{
    if (c_val == NULL)
    {
        return jerry_undefined();
    }

    if (SNI_TYPE_IS_VALUE(type))
    {
        return jerry_undefined();
    }

    switch (type)
    {
    case SNI_T_UINT32:
        return jerry_number((double)*(uint32_t *)c_val);

    case SNI_T_INT32:
        return jerry_number((double)*(int32_t *)c_val);

    case SNI_T_DOUBLE:
        return jerry_number(*(double *)c_val);

    case SNI_T_BOOL:
        return jerry_boolean(*(bool *)c_val);

    case SNI_T_STRING:
        return jerry_string_sz(*(const char **)c_val);

    case SNI_T_PTR:
        return jerry_number((double)(uintptr_t)*(void **)c_val);

    default:
        break;
    }

    if (SNI_TYPE_IS_VALUE(type))
    {
        const sni_val_obj_t *val_obj = &sni_val_objs[type];
        if (val_obj->prop_count == 0 || val_obj->props == NULL)
        {
            return jerry_undefined();
        }

        jerry_value_t js_obj = jerry_object();

        uint8_t current_bit_offset = 0;
        size_t last_offset = 0;

        for (uint16_t i = 0; i < val_obj->prop_count; i++)
        {
            const sni_val_prop_t *prop = &val_obj->props[i];
            void *field_ptr = (uint8_t *)c_val + prop->offset;

            if (prop->offset != last_offset)
            {
                current_bit_offset = 0;
                last_offset = prop->offset;
            }

            jerry_value_t js_prop;
            if (prop->bit_width > 0)
            {
                uint32_t value = sni_tb_read_bitfield(field_ptr, current_bit_offset, prop->bit_width);
                js_prop = jerry_number((double)value);
                current_bit_offset += prop->bit_width;
            }
            else
            {
                js_prop = sni_tb_c2js(field_ptr, prop->type);
            }

            if (!jerry_value_is_undefined(js_prop) && !jerry_value_is_exception(js_prop))
            {
                jerry_object_set_sz(js_obj, prop->name, js_prop);
            }

            jerry_value_free(js_prop);
        }

        return js_obj;
    }

    if (SNI_TYPE_IS_HANDLE(type))
    {
        void *ptr = *(void **)c_val;
        if (!ptr)
        {
            return jerry_null();
        }

        sni_handle_t *handle = sni_map_find(ptr);
        if (handle)
        {
            return jerry_value_copy(handle->js_obj);
        }

        jerry_value_t js_obj = jerry_object();

        sni_handle_t *new_handle =
            sni_map_insert(ptr, js_obj, type);

        jerry_object_set_native_ptr(js_obj, &sni_native_info, new_handle);
        return js_obj;
    }

    return jerry_undefined();
}

void sni_tb_register_val_obj(const sni_val_obj_t *val_obj)
{
    if (val_obj->type < __SNI_VALUE_START || val_obj->type >= __SNI_VALUE_END)
    {
        return;
    }

    sni_val_objs[val_obj->type] = *val_obj;
    sni_val_objs[val_obj->type].props = val_obj->props;
}

void sni_tb_register_handle_destroy_cb(sni_type_t type, sni_handle_destroy_cb_t destroy_cb)
{
    if (SNI_TYPE_IS_HANDLE_LC_REALM(type))
    {
        sni_handle_destroy_cb[type - __SNI_HANDLE_LC_REALM_START - 1] = destroy_cb;
    }
}

void _script_exited_cb(void)
{
    sni_handle_t *handle, *tmp;

    HASH_ITER(hh, sni_map, handle, tmp)
    {
        if (SNI_TYPE_IS_HANDLE_LC_REALM(handle->type))
        {
            if (sni_handle_destroy_cb[handle->type - __SNI_HANDLE_LC_REALM_START - 1])
            {
                sni_handle_destroy_cb[handle->type - __SNI_HANDLE_LC_REALM_START - 1](handle->ptr);
            }

            HASH_DEL(sni_map, handle);
            eos_free(handle);
        }
    }
}

void sni_tb_init(void)
{
    // 初始化类型桥
    sni_lv_types_init();
    // 注册脚本退出回调
    eos_event_add_global_cb(EOS_EVENT_SCRIPT_EXITED, _script_exited_cb, NULL);
}
