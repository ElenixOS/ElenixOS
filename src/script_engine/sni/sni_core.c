/**
 * @file sni_core.c
 * @brief SNI 核心
 * @author Sab1e
 * @date 2026-02-16
 */

#include "sni_core.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "sni_type_bridge.h"

/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

void sni_init(void)
{
    // 初始化类型桥
    sni_tb_init();
}

void sni_attach(jerry_value_t js_realm)
{
    // TODO：调用`sni_nn_attach()` 绑定原生命名空间到 realm
    // sni_nn_attach(js_realm, sni_nn_create(NULL), "lvgl");

}
