/**
 * @file script_engine_lv.c
 * @brief lv库
 * @author Sab1e
 * @date 2025-12-25
 */

#include "script_engine_lv.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "jerryscript.h"
#include "script_engine_core.h"
#include "lv_bindings.h"
#include "lv_bindings_special.h"
/* Macros and Definitions -------------------------------------*/
#define LV_LIB_NAME "lv"
/* Variables --------------------------------------------------*/
static jerry_value_t root_obj;
/* Function Implementations -----------------------------------*/

jerry_value_t script_engine_lv_get_root(void)
{
    return root_obj;
}

void script_engine_lv_attach(jerry_value_t realm)
{
    // 挂载到 realm
    jerry_value_t key = jerry_string_sz(LV_LIB_NAME);
    jerry_value_free(jerry_object_set(realm, key, root_obj));
    jerry_value_free(key);
}

void script_engine_lv_init(void)
{
    // 创建对象
    root_obj = jerry_object();
    // 注册对象
    lv_binding_init(root_obj);
    lv_bindings_special_init(root_obj);
}
