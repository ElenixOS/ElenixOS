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

void sni_mount(jerry_value_t js_realm)
{

}
