/**
 * @file elena_os_img.c
 * @brief 图片显示
 * @author Sab1e
 * @date 2025-08-12
 */

#include "elena_os_img.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "lvgl.h"
#define EOS_LOG_TAG "Image"
#include "elena_os_log.h"
#include "elena_os_port.h"
#include "elena_os_mem.h"
/* Macros and Definitions -------------------------------------*/
#define LV_IMG_BIN_HEADER_SIZE 12 // Bytes
#define LV_IMG_BIN_HEADER_WIDTH_LB 4
#define LV_IMG_BIN_HEADER_HEIGHT_LB 6
#define LV_IMG_BIN_HEADER_STRIDE_LB 8
/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/
/**
 * @brief 删除事件回调函数
 */
static void _img_delete_event_cb(lv_event_t *e)
{
    lv_image_dsc_t *img_dsc = lv_event_get_user_data(e);
    if (!img_dsc)
        return;

    uint8_t *bin_data = (uint8_t *)img_dsc->data - sizeof(lv_image_header_t);

    eos_free(bin_data);
    eos_free(img_dsc);
}

void eos_img_set_size(lv_obj_t *img_obj, const uint32_t w, const uint32_t h)
{
    EOS_CHECK_PTR_RETURN(img_obj);
    const void *src = lv_image_get_src(img_obj);
    if (src == NULL)
    {
        EOS_LOG_E("Image src is NULL");
        return;
    }
    lv_image_header_t header;
    lv_image_decoder_get_info(src, &header);
    lv_obj_set_size(img_obj, w, h);
    lv_image_set_scale_x(img_obj, (uint32_t)((w * 256) / header.w));
    lv_image_set_scale_y(img_obj, (uint32_t)((h * 256) / header.h));
}

void eos_img_set_src(lv_obj_t *img_obj, const char *bin_path)
{
    lv_image_set_src(img_obj, bin_path);
}
