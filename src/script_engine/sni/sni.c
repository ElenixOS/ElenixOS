/**
 * @file sni.c
 * @brief SNI
 * @author Sab1e
 * @date 2026-03-10
 */

#include "sni.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "sni_type_bridge.h"
#include "sni_api_lv.h"
/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

void sni_init(void)
{
    // 初始化类型桥
    sni_tb_init();
    sni_api_lv_init();
}

void sni_mount(jerry_value_t js_realm)
{
    sni_api_lv_mount(js_realm);
}
