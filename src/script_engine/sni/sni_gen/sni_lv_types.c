/**
 * @file sni_lv_types.c
 * @brief LVGL 类型表
 * @author Auto-generated
 * @date 2026-02-10
 */

#include "sni_lv_types.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "sni_types.h"
#include "sni_type_bridge.h"
#include "lvgl.h"

/************************** 属性表 **************************/

const sni_val_prop_t lv_point_props[] = {
    {
        .name = "x",
        .type = SNI_T_INT32,
        .offset = offsetof(lv_point_t, x),
    },
    {
        .name = "y",
        .type = SNI_T_INT32,
        .offset = offsetof(lv_point_t, y),
    },
};

const sni_val_obj_t lv_point_prop = {
    .type = SNI_V_LV_POINT,
    .prop_count = 2,
    .props = lv_point_props,
};

const sni_val_prop_t lv_area_props[] = {
    {
        .name = "x1",
        .type = SNI_T_INT32,
        .offset = offsetof(lv_area_t, x1),
    },
    {
        .name = "y1",
        .type = SNI_T_INT32,
        .offset = offsetof(lv_area_t, y1),
    },
    {
        .name = "x2",
        .type = SNI_T_INT32,
        .offset = offsetof(lv_area_t, x2),
    },
    {
        .name = "y2",
        .type = SNI_T_INT32,
        .offset = offsetof(lv_area_t, y2),
    },
};

const sni_val_obj_t lv_area_prop = {
    .type = SNI_V_LV_AREA,
    .prop_count = 4,
    .props = lv_area_props,
};

const sni_val_prop_t lv_color32_props[] = {
    {
        .name = "blue",
        .type = SNI_T_UINT32,
        .offset = offsetof(lv_color32_t, blue),
    },
    {
        .name = "green",
        .type = SNI_T_UINT32,
        .offset = offsetof(lv_color32_t, green),
    },
    {
        .name = "red",
        .type = SNI_T_UINT32,
        .offset = offsetof(lv_color32_t, red),
    },
    {
        .name = "alpha",
        .type = SNI_T_UINT32,
        .offset = offsetof(lv_color32_t, alpha),
    },
};

const sni_val_obj_t lv_color32_prop = {
    .type = SNI_V_LV_COLOR32,
    .prop_count = 4,
    .props = lv_color32_props,
};

/************************** 注册表 **************************/

const size_t sni_lv_types_count = sizeof(sni_lv_types) / sizeof(sni_lv_types[0]);

const sni_val_obj_t *sni_lv_types[] = {
    &lv_point_prop,
    &lv_area_prop,
    &lv_color32_prop,
};

/************************** 初始化函数 **************************/
void sni_lv_types_init(void)
{
    for (size_t i = 0;
         i < sni_lv_types_count;
         i++)
    {
        sni_tb_register_val_obj(sni_lv_types[i]);
    }
}
