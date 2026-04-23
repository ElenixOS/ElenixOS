/**
 * @file elena_os_watchface_list.c
 * @brief 表盘列表
 * @author Sab1e
 * @date 2025-08-25
 */

#include "elena_os_watchface_list.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "lvgl.h"
#include "cJSON.h"
#define EOS_LOG_DISABLE
#define EOS_LOG_TAG "WatchfaceList"
#include "elena_os_log.h"
#include "elena_os_watchface.h"
#include "elena_os_basic_widgets.h"
#include "elena_os_misc.h"
#include "elena_os_img.h"
#include "elena_os_port.h"
#include "elena_os_anim.h"
#include "script_engine_core.h"
#include "elena_os_sys.h"
#include "elena_os_fs.h"
#include "elena_os_activity.h"
/* Macros and Definitions -------------------------------------*/

#define _SNAPSHOT_CONTAINER_PAD 12
#define _SNAPSHOT_CONTAINER_BORDER 5
#define _SNAPSHOT_CONTAINER_W ((EOS_DISPLAY_WIDTH * 69) / 100)
#define _SNAPSHOT_CONTAINER_H ((EOS_DISPLAY_HEIGHT * 69) / 100)
#define _SNAPSHOT_PRESS_ZOOM 245

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

/**
 * @brief 表盘列表中 表盘按下的回调
 */
static void _watchface_list_btn_cb(lv_event_t *e)
{
    if (script_engine_get_state() != SCRIPT_STATE_STOPPED)
    {
        EOS_LOG_E("Another script running");
        return;
    }
    const char *watchface_id = (const char *)lv_event_get_user_data(e);
    EOS_CHECK_PTR_RETURN(watchface_id);
    eos_sys_cfg_set_string(EOS_SYS_CFG_KEY_WATCHFACE_ID_STR, watchface_id);
    eos_activity_back();
}

void eos_watchface_list_enter(void)
{
    // 创建新的页面用于绘制应用列表
    eos_activity_t *a = eos_activity_create(NULL);
    eos_activity_set_type(a, EOS_ACTIVITY_TYPE_WATCHFACE_LIST);
    lv_obj_t *wf_list_view = eos_activity_get_view(a);
    size_t watchface_list_size = eos_watchface_list_size();

    lv_obj_t *cont = lv_list_create(wf_list_view);
    lv_obj_set_style_pad_all(cont, 24, 0);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_scroll_dir(cont, LV_DIR_HOR);
    lv_obj_center(cont);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cont,
                          LV_FLEX_ALIGN_START,   // 主轴(水平方向)居中
                          LV_FLEX_ALIGN_CENTER,  // 交叉轴(垂直方向)居中
                          LV_FLEX_ALIGN_CENTER); // 内容居中
    lv_obj_set_scroll_snap_x(cont, LV_SCROLL_SNAP_CENTER);
    for (size_t i = 0; i < watchface_list_size; i++)
    {
        const char *watchface_id = eos_watchface_list_get_id(i);
        lv_obj_t *item = lv_obj_create(cont);
        lv_obj_set_size(item, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(item, LV_FLEX_FLOW_COLUMN); // 垂直布局
        lv_obj_set_style_pad_all(item, 0, 0);
        lv_obj_set_style_margin_left(item, 50, 0);
        lv_obj_set_style_pad_gap(item, 20, 0); // snapshot 和 label 的间距
        lv_obj_set_style_border_width(item, 0, 0);
        lv_obj_set_style_shadow_width(item, 0, 0);
        lv_obj_set_style_bg_opa(item, LV_OPA_TRANSP, 0);
        lv_obj_remove_flag(item, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(item, LV_OBJ_FLAG_SNAPPABLE);
        lv_obj_set_flex_align(item,
                              LV_FLEX_ALIGN_START,   // 主轴(水平方向)居中
                              LV_FLEX_ALIGN_CENTER,  // 交叉轴(垂直方向)居中
                              LV_FLEX_ALIGN_CENTER); // 内容居中

        char icon_path[PATH_MAX];
        if (watchface_id && strcmp(watchface_id, EOS_WATCHFACE_BUILTIN_FALLBACK_ID) == 0)
        {
            memcpy(icon_path, EOS_IMG_WATCHFACE, sizeof(EOS_IMG_WATCHFACE));
        }
        else
        {
            snprintf(icon_path, sizeof(icon_path), EOS_WATCHFACE_INSTALLED_DIR "%s/" EOS_WATCHFACE_SNAPSHOT_FILE_NAME,
                     watchface_id);
            EOS_LOG_D("WFPATH:%s", icon_path);
            if (!eos_is_file(icon_path))
            {
                EOS_LOG_W("Watchface snapshot not found!");
                memcpy(icon_path, EOS_IMG_WATCHFACE, sizeof(EOS_IMG_WATCHFACE));
            }
        }

        /* 外层容器：用于显示边框、圆角与裁角，snapshot 在容器内留 12px 的内边距 */
        lv_obj_t *snapshot_container = lv_obj_create(item);
        lv_obj_set_size(snapshot_container, _SNAPSHOT_CONTAINER_W, _SNAPSHOT_CONTAINER_H);
        lv_obj_set_style_border_width(snapshot_container, _SNAPSHOT_CONTAINER_BORDER, 0);
        lv_obj_set_style_radius(snapshot_container, EOS_DISPLAY_RADIUS, 0);
        lv_obj_set_style_clip_corner(snapshot_container, true, 0);
        lv_obj_set_style_pad_all(snapshot_container, _SNAPSHOT_CONTAINER_PAD, 0);
        lv_obj_set_style_shadow_width(snapshot_container, 0, 0);
        lv_obj_set_style_bg_opa(snapshot_container, LV_OPA_TRANSP, 0);
        lv_obj_remove_flag(snapshot_container, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_remove_flag(snapshot_container, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_remove_flag(snapshot_container, LV_OBJ_FLAG_CLICK_FOCUSABLE);
        lv_obj_center(snapshot_container);

        /* 图片裁剪容器：负责 snapshot 的布局、裁角与圆角 */
        lv_obj_t *snapshot_clip_container = lv_obj_create(snapshot_container);
        lv_obj_set_size(snapshot_clip_container,
                _SNAPSHOT_CONTAINER_W - (_SNAPSHOT_CONTAINER_PAD * 2),
                _SNAPSHOT_CONTAINER_H - (_SNAPSHOT_CONTAINER_PAD * 2));
        lv_obj_set_style_radius(snapshot_clip_container, EOS_DISPLAY_RADIUS, 0);
        lv_obj_set_style_clip_corner(snapshot_clip_container, true, 0);
        lv_obj_set_style_border_width(snapshot_clip_container, 0, 0);
        lv_obj_set_style_shadow_width(snapshot_clip_container, 0, 0);
        lv_obj_set_style_margin_all(snapshot_clip_container, 0, 0);
        lv_obj_set_style_pad_all(snapshot_clip_container, 0, 0);
        lv_obj_set_style_bg_opa(snapshot_clip_container, LV_OPA_TRANSP, 0);
        lv_obj_remove_flag(snapshot_clip_container, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_remove_flag(snapshot_clip_container, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_remove_flag(snapshot_clip_container, LV_OBJ_FLAG_CLICK_FOCUSABLE);
        lv_obj_center(snapshot_clip_container);

        /* snapshot 占满新的父容器 */
        lv_obj_t *watchface_snapshot = lv_image_create(snapshot_clip_container);
        lv_obj_set_size(watchface_snapshot, lv_pct(100), lv_pct(100));
        lv_obj_set_style_shadow_width(watchface_snapshot, 0, 0);
        lv_obj_set_style_margin_all(watchface_snapshot, 0, 0);
        lv_obj_set_style_pad_all(watchface_snapshot, 0, 0);
        lv_obj_center(watchface_snapshot);
        /* 移除 CLICKABLE 标志，让触摸事件传递给父对象 */
        lv_obj_remove_flag(watchface_snapshot, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_remove_flag(watchface_snapshot, LV_OBJ_FLAG_CLICK_FOCUSABLE);
        eos_img_set_src(watchface_snapshot, icon_path);
        eos_img_set_size(watchface_snapshot,
                 _SNAPSHOT_CONTAINER_W - (_SNAPSHOT_CONTAINER_PAD * 2),
                 _SNAPSHOT_CONTAINER_H - (_SNAPSHOT_CONTAINER_PAD * 2));
        lv_obj_center(watchface_snapshot);

        // 将点击事件处理函数添加到父对象上，这样当用户点击图片时，点击事件会传递给父对象
        lv_obj_add_event_cb(item, _watchface_list_btn_cb, LV_EVENT_CLICKED, (void *)eos_watchface_list_get_id(i));
        // 确保父对象是可点击的
        lv_obj_add_flag(item, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_remove_flag(item, LV_OBJ_FLAG_CLICK_FOCUSABLE);
        // 显示名称
        lv_obj_t *label = lv_label_create(item);
        if (watchface_id && strcmp(watchface_id, EOS_WATCHFACE_BUILTIN_FALLBACK_ID) == 0)
        {
            lv_label_set_text(label, "Fallback Watchface");
        }
        else
        {
            char manifest_path[PATH_MAX];
            snprintf(manifest_path, sizeof(manifest_path), EOS_WATCHFACE_INSTALLED_DIR "%s/" EOS_WATCHFACE_MANIFEST_FILE_NAME,
                     watchface_id);
            script_pkg_t pkg = {0};
            pkg.type = SCRIPT_TYPE_WATCHFACE;
            if (script_engine_get_manifest(manifest_path, &pkg) != SE_OK)
            {
                EOS_LOG_E("Read manifest failed: %s", manifest_path);
                eos_pkg_free(&pkg);
                continue;
            }
            lv_label_set_text(label, pkg.name);
            eos_pkg_free(&pkg);
        }
        lv_obj_set_width(label, LV_SIZE_CONTENT);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    }
    lv_obj_update_snap(cont, LV_ANIM_OFF);
    eos_activity_enter(a);
}
