/**
 * @file elena_os_ww_clock_hand.h
 * @brief 头文件说明
 * @author Sab1e
 * @date 2025-12-18
 */

#ifndef ELENA_OS_WW_CLOCK_HAND_H
#define ELENA_OS_WW_CLOCK_HAND_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/
typedef enum{
    EOS_CLOCK_HAND_HOUR,
    EOS_CLOCK_HAND_MINUTE,
    EOS_CLOCK_HAND_SECOND,
} eos_clock_hand_type_t;
/* Public function prototypes --------------------------------*/

/**
 * @brief 将指针放到指定位置（控制锚点为旋转中心点）
 * @param hand 指针对象
 * @param target_x x
 * @param target_y y
 */
void eos_clock_hand_place_pivot(lv_obj_t *hand,
                         lv_coord_t target_x,
                         lv_coord_t target_y);

/**
 * @brief 以指针旋转中心点为锚点，相对父对象居中
 * @param hand 指针对象
 */
void eos_clock_hand_center(lv_obj_t *hand);

/**
 * @brief 创建一个时钟指针
 * @param parent 指针父对象
 * @param src 指针图像源
 * @param t 指针类型`eos_clock_hand_type_t`
 * @param hand_pivot_x 旋转中心点 x 坐标（相对图像）
 * @param hand_pivot_y 旋转中心点 y 坐标（相对图像）
 * @return lv_obj_t* 返回创建的指针图像对象
 */
lv_obj_t *eos_clock_hand_create(lv_obj_t *parent,
                                const char *src,
                                eos_clock_hand_type_t t,
                                lv_coord_t hand_pivot_x,
                                lv_coord_t hand_pivot_y);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_WW_CLOCK_HAND_H */
