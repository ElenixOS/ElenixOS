/**
 * @file sni.c
 * @brief SNI
 */

#include "sni.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "sni_type_bridge.h"
#include "sni_api_lv.h"
#include "sni_api_eos.h"
/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

void sni_init(void)
{
    // Initialize type bridge
    sni_tb_init();
    sni_api_lv_init();
    sni_api_eos_init();
}

void sni_mount(jerry_value_t js_realm)
{
    sni_api_lv_mount(js_realm);
    sni_api_eos_mount(js_realm);
}
