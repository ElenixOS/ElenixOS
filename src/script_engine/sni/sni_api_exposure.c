/**
 * @file sni_api_exposure.c
 * @brief API 暴露层
 * @author Sab1e
 * @date 2026-03-05
 */

#include "sni_api_exposure.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "sni_types.h"
#include "sni_type_bridge.h"
/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

jerry_value_t sni_api_build(const sni_native_entry_t *entries)
{
    if (entries == NULL)
    {
        return jerry_undefined();
    }

    // 创建命名空间对象
    jerry_value_t api_obj = jerry_object();
    if (jerry_value_is_exception(api_obj))
    {
        return jerry_undefined();
    }

    // 遍历注册项
    for (size_t i = 0; entries[i].name != NULL; i++)
    {
        const sni_native_entry_t *entry = &entries[i];

        jerry_value_t name = jerry_string_sz(entry->name);
        if (jerry_value_is_exception(name))
        {
            jerry_value_free(api_obj);
            return jerry_undefined();
        }

        jerry_value_t value = jerry_undefined();

        switch (entry->type)
        {
        case SNI_ENTRY_FUNCTION:
            value = jerry_function_external(entry->value.function);
            break;

        case SNI_ENTRY_CONSTANT:
            value = jerry_create_number(entry->value.constant);
            break;

        case SNI_ENTRY_NAMESPACE:
            value = sni_api_build(entry->value.sub_entries);
            break;

        default:
            jerry_value_free(name);
            continue;
        }

        if (jerry_value_is_exception(value))
        {
            jerry_value_free(name);
            jerry_value_free(api_obj);
            return jerry_undefined();
        }

        jerry_value_t result = jerry_object_set(api_obj, name, value);

        jerry_value_free(name);
        jerry_value_free(value);

        if (jerry_value_is_exception(result))
        {
            jerry_value_free(result);
            jerry_value_free(api_obj);
            return jerry_undefined();
        }

        jerry_value_free(result);
    }

    return api_obj;
}

bool sni_api_install(jerry_value_t realm, jerry_value_t api_obj, const char *name)
{
    if (name == NULL)
    {
        return false;
    }

    if (jerry_value_is_exception(realm) ||
        jerry_value_is_exception(api_obj))
    {
        return false;
    }

    jerry_value_t key = jerry_string_sz(name);
    if (jerry_value_is_exception(key))
    {
        jerry_value_free(key);
        return false;
    }

    jerry_value_t result = jerry_object_set(realm, key, api_obj);

    jerry_value_free(key);

    if (jerry_value_is_exception(result))
    {
        jerry_value_free(result);
        return false;
    }

    jerry_value_free(result);

    return true;
}
