/**
 * @file elena_os_touch.c
 * @brief Get touch device
 * @author Sab1e
 * @date 2025-11-24
 */

#include "elena_os_touch.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

lv_indev_t *eos_touch_get_indev(void)
{
    lv_indev_t *indev = lv_indev_get_next(NULL);
    while (indev)
    {
        if (lv_indev_get_type(indev) == LV_INDEV_TYPE_POINTER)
        {
            // Find touch device
            return indev;
        }
        indev = lv_indev_get_next(indev);
    }
}
