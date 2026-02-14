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
/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/
static sni_val_obj_t sni_val_objs[__SNI_TYPE_MAX] = {0};

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

/* Function Implementations -----------------------------------*/

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

    if (type >= __SNI_VALUE_START && type < __SNI_VALUE_END)
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

    if (type >= __SNI_VALUE_START && type < __SNI_VALUE_END)
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

    // TODO: 解析Handle对象

    return false;
}

jerry_value_t sni_tb_c2js(void *c_val, sni_type_t type)
{
    if (c_val == NULL)
    {
        return jerry_undefined();
    }

    if (type >= __SNI_VALUE_START && type < __SNI_VALUE_END)
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

    // TODO: 解析Value对象
    if (type >= __SNI_VALUE_START && type < __SNI_VALUE_END)
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

    // TODO: 解析Handle对象

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
void sni_tb_init(void)
{
    // 初始化类型桥
    sni_lv_types_init();
}
