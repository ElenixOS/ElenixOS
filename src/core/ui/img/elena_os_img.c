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

#if EOS_OVERRIDE_LVGL_STDLIB_MALLOC_ENABLE

void eos_img_set_src(lv_obj_t *img_obj, const char *bin_path)
{
    lv_image_set_src(img_obj, bin_path);
}

#else

void eos_img_set_src(lv_obj_t *img_obj, const char *bin_path)
{
    // 路径为空则不设置源
    if (!bin_path)
        return;
    EOS_CHECK_PTR_RETURN(img_obj);

    EOS_LOG_I("Load image bin: %s", bin_path);

    // 清除回调
    lv_obj_remove_event_cb(img_obj, _img_delete_event_cb);

    // 避免数据泄漏
    lv_image_set_src(img_obj, NULL);

    // 打开新图像文件
    eos_file_t fp = eos_fs_open_read(bin_path);
    if (fp == EOS_FILE_INVALID)
    {
        EOS_LOG_E("Failed to open file");
        return;
    }

    // 获取文件大小
    uint32_t file_size = 0;
    eos_fs_size(fp, &file_size);

    if (file_size <= 0)
    {
        EOS_LOG_E("Invalid file size");
        eos_fs_close(fp);
        return;
    }

    // 分配内存
    void *bin_data = eos_malloc(file_size);
    if (!bin_data)
    {
        EOS_LOG_E("Failed to allocate memory for image");
        eos_fs_close(fp);
        return;
    }

    // 读取文件内容到内存
    ssize_t bytes_read = eos_fs_read(fp, bin_data, file_size);
    eos_fs_close(fp); // 读取完成后立即关闭文件描述符

    if (bytes_read != file_size)
    {
        EOS_LOG_E("Failed to read complete file (read %zd of %ld bytes)", bytes_read, file_size);
        eos_free(bin_data);
        return;
    }

    // 动态分配图像描述符
    lv_image_dsc_t *img_dsc = eos_malloc_zeroed(sizeof(lv_image_dsc_t));
    if (!img_dsc)
    {
        EOS_LOG_E("Failed to allocate image descriptor");
        eos_free(bin_data);
        return;
    }

    memcpy(&img_dsc->header, bin_data, sizeof(lv_image_header_t));

    if (img_dsc->header.magic != LV_IMAGE_HEADER_MAGIC)
    {
        EOS_LOG_E("Invalid image magic");
        eos_free(img_dsc);
        eos_free(bin_data);
        return;
    }

    // 直接赋值图像描述符头
    img_dsc->data_size = file_size - sizeof(lv_image_header_t);
    img_dsc->data = (const uint8_t *)bin_data + sizeof(lv_image_header_t);

    // 设置图像源
    lv_image_set_src(img_obj, img_dsc);
    // 添加删除事件回调，并将用户数据附加到回调
    lv_obj_add_event_cb(img_obj, _img_delete_event_cb, LV_EVENT_DELETE, img_dsc);
    EOS_LOG_D("Image Set OK");
}

#endif /* EOS_OVERRIDE_LVGL_STDLIB_MALLOC_ENABLE */
