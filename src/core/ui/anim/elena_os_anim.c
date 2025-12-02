/**
 * @file elena_os_anim.c
 * @brief 动画库
 * @author Sab1e
 * @date 2025-08-14
 */

#include "elena_os_anim.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
// #define EOS_LOG_DISABLE
#define EOS_LOG_TAG "Animation"
#include "elena_os_log.h"
#include "elena_os_theme.h"
#include "elena_os_port.h"

/* Macros and Definitions -------------------------------------*/
#define DEBUG_BLOCKER_VISIBLE 0
/* Variables --------------------------------------------------*/
static lv_obj_t *blocker = NULL;
static bool is_blocker_show = false;
/* Function Implementations -----------------------------------*/

/************************** 1. 基础功能函数 **************************/

void eos_anim_del(eos_anim_t *anim)
{
    if (!anim)
        return;

    if (anim->anim_timeline)
    {
        EOS_LOG_D("Timeline freed: [%p]", anim->anim_timeline);
        lv_anim_timeline_delete(anim->anim_timeline);
        anim->anim_timeline = NULL;
    }

    if (anim->auto_delete_obj && anim->tar_obj && lv_obj_is_valid(anim->tar_obj))
    {
        EOS_LOG_D("Target obj freed [%p]", anim->tar_obj);
        lv_obj_delete_async(anim->tar_obj);
        anim->tar_obj = NULL;
    }

    eos_free(anim);
    EOS_LOG_D("Anim freed");
}

void eos_anim_set_auto_delete(eos_anim_t *anim)
{
    EOS_CHECK_PTR_RETURN(anim);
    anim->auto_delete_obj = true;
}

void eos_anim_add_cb(
    eos_anim_t *anim,
    eos_anim_cb_t user_cb,
    void *user_data)
{
    if (!anim)
        return;
    anim->user_cb = user_cb;
    anim->user_data = user_data;
}

void *eos_anim_get_user_data(eos_anim_t *anim)
{
    return anim ? anim->user_data : NULL;
}

void eos_anim_blocker_show(void)
{
    if (is_blocker_show)
        return;

    blocker = lv_obj_create(lv_layer_top());
    lv_obj_remove_style_all(blocker); // 去掉样式，保持透明
#if DEBUG_BLOCKER_VISIBLE
    lv_obj_set_style_bg_color(blocker, EOS_COLOR_MINT, 0);
    lv_obj_set_style_bg_opa(blocker, LV_OPA_40, 0);
#endif
    lv_obj_set_size(blocker, LV_PCT(100), LV_PCT(100));
    lv_obj_add_flag(blocker, LV_OBJ_FLAG_CLICKABLE); // 吸收点击
    is_blocker_show = true;
}

void eos_anim_blocker_hide(void)
{
    if (is_blocker_show)
    {
        if (!(blocker && lv_obj_is_valid(blocker) && lv_obj_has_class(blocker, &lv_obj_class)))
            return;
        lv_obj_delete(blocker);
        blocker = NULL;
        is_blocker_show = false;
    }
}

/************************** 2. 动画执行回调 **************************/

/**
 * @brief 动画播放时设置宽度的回调
 */
static void _set_width_cb(lv_anim_t *var, int32_t v)
{
    lv_obj_set_width(var->var, v);
}
/**
 * @brief 动画播放时设置高度的回调
 */
static void _set_height_cb(lv_anim_t *var, int32_t v)
{
    lv_obj_set_height(var->var, v);
}
/**
 * @brief 动画播放时设置 X 位置的回调
 */
static void _set_x_cb(lv_anim_t *var, int32_t v)
{
    lv_obj_set_style_translate_x(var->var, v, 0);
}
/**
 * @brief 动画播放时设置 Y 位置的回调
 */
static void _set_y_cb(lv_anim_t *var, int32_t v)
{
    lv_obj_set_style_translate_y(var->var, v, 0);
}
/**
 * @brief 动画播放时设置变换缩放的回调
 */
static void _set_scale_cb(void *var, int32_t v)
{
    lv_obj_set_style_transform_scale((lv_obj_t *)var, v, 0);
}
/**
 * @brief 动画播放时设置透明度的回调
 */
static void _set_opa_cb(void *var, int32_t v)
{
    lv_obj_set_style_bg_opa((lv_obj_t *)var, (lv_opa_t)v, 0);
}

/**
 * @brief 动画播放时设置透明度的回调
 */
static void _set_opa_layered_cb(void *var, int32_t v)
{
    lv_obj_set_style_opa_layered((lv_obj_t *)var, (lv_opa_t)v, 0);
}

/************************** 3. 动画播放完成回调 **************************/

static void _free_anim_later(lv_timer_t *t)
{
    eos_anim_t *anim = lv_timer_get_user_data(t);
    eos_anim_del(anim);
}
/**
 * @brief 动画播放完毕回调，回调用户函数，自动清理资源
 */
static void _eos_anim_ready_cb(lv_anim_t *a)
{
    eos_anim_t *anim = lv_anim_get_user_data(a);
    anim->anim_completed_count++;

    if (anim->anim_completed_count == anim->anim_count)
    {
        if (anim->user_cb)
        {
            anim->user_cb(anim);
        }
        // 自动清理资源，延时删除
        lv_timer_t *t = lv_timer_create(_free_anim_later, 10, anim);
        lv_timer_set_repeat_count(t, 1);
    }
    eos_anim_blocker_hide();
}

/************************** 4. 动画初始化函数 **************************/

/**
 * @brief 内部函数：初始化宽度动画
 */
static void _init_width_anim(lv_anim_t *a, lv_obj_t *obj,
                             int32_t start, int32_t end,
                             uint32_t duration, eos_anim_t *ctx)
{
    lv_anim_init(a);
    lv_anim_set_var(a, obj);
    lv_anim_set_values(a, start, end);
    lv_anim_set_custom_exec_cb(a, _set_width_cb);
    lv_anim_set_path_cb(a, lv_anim_path_ease_out);
    lv_anim_set_duration(a, duration);
    if (!ctx)
        return;
    lv_anim_set_completed_cb(a, _eos_anim_ready_cb);
    lv_anim_set_user_data(a, ctx);
}
/**
 * @brief 内部函数：初始化高度动画
 */
static void _init_height_anim(lv_anim_t *a, lv_obj_t *obj,
                              int32_t start, int32_t end,
                              uint32_t duration, eos_anim_t *ctx)
{
    lv_anim_init(a);
    lv_anim_set_var(a, obj);
    lv_anim_set_values(a, start, end);
    lv_anim_set_custom_exec_cb(a, _set_height_cb);
    lv_anim_set_path_cb(a, lv_anim_path_ease_out);
    lv_anim_set_duration(a, duration);
    if (!ctx)
        return;
    lv_anim_set_completed_cb(a, _eos_anim_ready_cb);
    lv_anim_set_user_data(a, ctx);
}

/**
 * @brief 内部函数：初始化 X 位置动画
 */
static void _init_x_anim(lv_anim_t *a, lv_obj_t *obj,
                         int32_t start, int32_t end,
                         uint32_t duration, eos_anim_t *ctx)
{
    lv_anim_init(a);
    lv_anim_set_var(a, obj);
    lv_anim_set_values(a, start, end);
    lv_anim_set_custom_exec_cb(a, _set_x_cb);
    lv_anim_set_path_cb(a, lv_anim_path_ease_out);
    lv_anim_set_duration(a, duration);
    if (!ctx)
        return;
    lv_anim_set_completed_cb(a, _eos_anim_ready_cb);
    lv_anim_set_user_data(a, ctx);
}

/**
 * @brief 内部函数：初始化 Y 位置动画
 */
static void _init_y_anim(lv_anim_t *a, lv_obj_t *obj,
                         int32_t start, int32_t end,
                         uint32_t duration, eos_anim_t *ctx)
{
    lv_anim_init(a);
    lv_anim_set_var(a, obj);
    lv_anim_set_values(a, start, end);
    lv_anim_set_custom_exec_cb(a, _set_y_cb);
    lv_anim_set_path_cb(a, lv_anim_path_ease_out);
    lv_anim_set_duration(a, duration);
    if (!ctx)
        return;
    lv_anim_set_completed_cb(a, _eos_anim_ready_cb);
    lv_anim_set_user_data(a, ctx);
}

/**
 * @brief 内部函数：初始化变换缩放动画
 */
static void _init_scale_anim(lv_anim_t *a, lv_obj_t *obj,
                             int32_t start, int32_t end,
                             uint32_t duration, eos_anim_t *ctx)
{
    lv_anim_init(a);
    lv_anim_set_var(a, obj);
    lv_anim_set_values(a, start, end);
    lv_anim_set_exec_cb(a, _set_scale_cb);
    lv_anim_set_path_cb(a, lv_anim_path_ease_in_out);
    lv_anim_set_duration(a, duration);
    if (!ctx)
        return;
    lv_anim_set_completed_cb(a, _eos_anim_ready_cb);
    lv_anim_set_user_data(a, ctx);
}

/**
 * @brief 内部函数：初始化透明度动画
 */
static void _init_opa_anim(lv_anim_t *a, lv_obj_t *obj,
                           int32_t start, int32_t end,
                           uint32_t duration, eos_anim_t *ctx)
{
    lv_anim_init(a);
    lv_anim_set_var(a, obj);
    lv_anim_set_values(a, start, end);
    lv_anim_set_exec_cb(a, _set_opa_cb);
    lv_anim_set_path_cb(a, lv_anim_path_ease_in_out);
    lv_anim_set_duration(a, duration);
    if (!ctx)
        return;
    lv_anim_set_completed_cb(a, _eos_anim_ready_cb);
    lv_anim_set_user_data(a, ctx);
}

/**
 * @brief 内部函数：初始化透明度动画
 */
static void _init_opa_layered_anim(lv_anim_t *a, lv_obj_t *obj,
                                   int32_t start, int32_t end,
                                   uint32_t duration, eos_anim_t *ctx)
{
    lv_anim_init(a);
    lv_anim_set_var(a, obj);
    lv_anim_set_values(a, start, end);
    lv_anim_set_exec_cb(a, _set_opa_layered_cb);
    lv_anim_set_path_cb(a, lv_anim_path_ease_in_out);
    lv_anim_set_duration(a, duration);
    if (!ctx)
        return;
    lv_anim_set_completed_cb(a, _eos_anim_ready_cb);
    lv_anim_set_user_data(a, ctx);
}

/************************** 5. 动画创建与启动 **************************/

// 缩放动画组
eos_anim_t *eos_anim_scale_create(lv_obj_t *tar_obj,
                                  int32_t w_start, int32_t w_end,
                                  int32_t h_start, int32_t h_end,
                                  uint32_t duration, bool auto_delete)
{
    if (!tar_obj || duration == 0)
        return NULL;

    eos_anim_t *anim = eos_malloc(sizeof(eos_anim_t));
    if (!anim)
        return NULL;

    // 基础初始化
    anim->type = EOS_ANIM_SCALE;
    anim->anim_count = 0;
    anim->anim_completed_count = 0;
    anim->user_cb = NULL;
    anim->user_data = NULL;
    anim->auto_delete_obj = auto_delete;
    anim->tar_obj = tar_obj;
    anim->anim_timeline = lv_anim_timeline_create();
    if (!anim->anim_timeline)
    {
        eos_free(anim);
        return NULL;
    }

    // 初始化宽度动画
    _init_width_anim(&anim->anim.scale.a_width, tar_obj, w_start, w_end, duration, anim);
    anim->anim_count++;

    // 初始化高度动画
    _init_height_anim(&anim->anim.scale.a_height, tar_obj, h_start, h_end, duration, anim);
    anim->anim_count++;

    EOS_LOG_I("Scale anim created: anim[%p] obj[%p]", anim, anim->tar_obj);

    return anim;
}

void eos_anim_scale_start(lv_obj_t *tar_obj,
                          int32_t w_start, int32_t w_end,
                          int32_t h_start, int32_t h_end,
                          uint32_t duration, bool auto_delete)
{
    eos_anim_t *anim = eos_anim_scale_create(tar_obj, w_start, w_end, h_start, h_end, duration, auto_delete);
    if (!anim)
        return;

    if (!eos_anim_start(anim))
    {
        eos_anim_del(anim);
    }
}

// 变换缩放动画组
eos_anim_t *eos_anim_transform_scale_create(lv_obj_t *tar_obj,
                                            int32_t scale_start, int32_t scale_end,
                                            uint32_t duration, bool auto_delete)
{
    if (!tar_obj || duration == 0)
        return NULL;

    eos_anim_t *anim = eos_malloc(sizeof(eos_anim_t));
    if (!anim)
        return NULL;

    // 基础初始化
    anim->type = EOS_ANIM_TRANSFORM_SCALE;
    anim->anim_count = 0;
    anim->anim_completed_count = 0;
    anim->user_cb = NULL;
    anim->user_data = NULL;
    anim->auto_delete_obj = auto_delete;
    anim->tar_obj = tar_obj;
    anim->anim_timeline = lv_anim_timeline_create();
    if (!anim->anim_timeline)
    {
        eos_free(anim);
        return NULL;
    }

    // 初始化变换缩放动画
    _init_scale_anim(&anim->anim.transform_scale.a_scale, tar_obj, scale_start, scale_end, duration, anim);
    anim->anim_count++;

    EOS_LOG_I("Transform Scale anim created: anim[%p] obj[%p]", anim, anim->tar_obj);

    return anim;
}

void eos_anim_transform_scale_start(lv_obj_t *tar_obj,
                                    int32_t scale_start, int32_t scale_end,
                                    uint32_t duration, bool auto_delete)
{
    eos_anim_t *anim = eos_anim_transform_scale_create(tar_obj, scale_start, scale_end, duration, auto_delete);
    if (!anim)
        return;

    if (!eos_anim_start(anim))
    {
        eos_anim_del(anim);
    }
}

void eos_anim_transform_scale_start_ex(lv_obj_t *tar_obj,
                                       int32_t scale_start, int32_t scale_end,
                                       uint32_t duration, uint32_t playback_time,
                                       uint16_t repeat_count, bool auto_delete)
{
    if (!tar_obj)
        return;

    eos_anim_t *anim = eos_anim_transform_scale_create(tar_obj, scale_start, scale_end, duration, auto_delete);
    if (!anim)
        return;

    // 设置高级参数
    if (playback_time > 0)
    {
        EOS_LOG_D("Playback: %d", playback_time);
        lv_anim_set_playback_time(&anim->anim.transform_scale.a_scale, playback_time);
    }
    if (repeat_count > 0)
    {
        lv_anim_set_repeat_count(&anim->anim.transform_scale.a_scale, repeat_count);
    }

    if (!eos_anim_start(anim))
    {
        eos_anim_del(anim);
    }
}

// 位移动画组
eos_anim_t *eos_anim_move_create(lv_obj_t *tar_obj,
                                 int32_t start_x, int32_t start_y,
                                 int32_t end_x, int32_t end_y,
                                 uint32_t duration, bool auto_delete)
{
    if (!tar_obj || duration == 0)
        return NULL;

    eos_anim_t *anim = eos_malloc(sizeof(eos_anim_t));
    EOS_LOG_D("MOVE alloc size(%d) ptr[%p]", sizeof(eos_anim_t), anim);
    if (!anim)
        return NULL;

    // 基础初始化
    anim->type = EOS_ANIM_MOVE;
    anim->anim_count = 0;
    anim->anim_completed_count = 0;
    anim->user_cb = NULL;
    anim->user_data = NULL;
    anim->auto_delete_obj = auto_delete;
    anim->tar_obj = tar_obj;
    anim->anim_timeline = lv_anim_timeline_create();
    if (!anim->anim_timeline)
    {
        eos_free(anim);
        return NULL;
    }

    // 初始化 X 动画
    if (start_x == end_x)
    {
        anim->cfg.move.disable_x = true;
    }
    else
    {
        _init_x_anim(&anim->anim.move.a_x, tar_obj, start_x, end_x, duration, anim);
        anim->anim_count++;
    }

    // 初始化 Y 动画
    if (start_y == end_y)
    {
        anim->cfg.move.disable_y = true;
    }
    else
    {
        _init_y_anim(&anim->anim.move.a_y, tar_obj, start_y, end_y, duration, anim);
        anim->anim_count++;
    }

    if (anim->anim_count == 0)
    {
        lv_anim_timeline_delete(anim->anim_timeline);
        eos_free(anim);
        return NULL;
    }

    EOS_LOG_I("Move anim created: anim[%p] obj[%p]", anim, anim->tar_obj);

    return anim;
}

void eos_anim_move_start(lv_obj_t *tar_obj,
                         int32_t start_x, int32_t start_y,
                         int32_t end_x, int32_t end_y,
                         uint32_t duration, bool auto_delete)
{
    eos_anim_t *anim = eos_anim_move_create(tar_obj, start_x, start_y, end_x, end_y, duration, auto_delete);
    if (!anim)
        return;

    if (!eos_anim_start(anim))
    {
        EOS_LOG_D("Delete anim: %p", anim);
        eos_anim_del(anim);
    }
}

// 透明度动画组
eos_anim_t *eos_anim_fade_create(lv_obj_t *tar_obj,
                                 int32_t opa_start,
                                 int32_t opa_end,
                                 uint32_t duration,
                                 bool auto_delete)
{
    if (!tar_obj || duration == 0)
        return NULL;

    eos_anim_t *anim = eos_malloc(sizeof(eos_anim_t));
    EOS_LOG_D("FADE alloc size(%d) ptr[%p]", sizeof(eos_anim_t), anim);
    if (!anim)
        return NULL;

    anim->type = EOS_ANIM_FADE;
    anim->anim_count = 0;
    anim->anim_completed_count = 0;
    anim->user_cb = NULL;
    anim->user_data = NULL;
    anim->auto_delete_obj = auto_delete;
    anim->tar_obj = tar_obj;
    anim->anim_timeline = lv_anim_timeline_create();
    anim->cfg.fade.layered = true;
    if (!anim->anim_timeline)
    {
        eos_free(anim);
        return NULL;
    }

    if (anim->cfg.fade.layered)
    {
        _init_opa_layered_anim(&anim->anim.fade.a_opa, tar_obj, opa_start, opa_end, duration, anim);
    }
    else
    {
        _init_opa_anim(&anim->anim.fade.a_opa, tar_obj, opa_start, opa_end, duration, anim);
    }

    anim->anim_count++;
    lv_anim_set_user_data(&anim->anim.fade.a_opa, anim);

    EOS_LOG_I("Fade anim created: anim[%p] obj[%p]", anim, anim->tar_obj);

    return anim;
}

void eos_anim_fade_start(lv_obj_t *tar_obj,
                         int32_t opa_start,
                         int32_t opa_end,
                         uint32_t duration,
                         bool auto_delete)
{
    eos_anim_t *anim = eos_anim_fade_create(tar_obj, opa_start, opa_end, duration, auto_delete);
    if (!anim)
        return;

    if (!eos_anim_start(anim))
    {
        eos_anim_del(anim);
    }
}

void eos_anim_fade_set_layered(eos_anim_t *a, bool layered)
{
    EOS_CHECK_PTR_RETURN(a);
    if (a->type == EOS_ANIM_FADE)
    {
        a->cfg.fade.layered = layered;
        if (layered)
        {
            lv_anim_set_exec_cb(&a->anim.fade.a_opa, _set_opa_layered_cb);
        }
        else
        {
            lv_anim_set_exec_cb(&a->anim.fade.a_opa, _set_opa_cb);
        }
    }
}

/************************** 轻量动画函数 **************************/

void eos_lite_anim_move_hor_start(lv_obj_t *target_obj,
                                  int32_t start, int32_t end,
                                  uint32_t duration, uint32_t delay,
                                  lv_anim_completed_cb_t completed_cb, void *user_data)
{
    lv_anim_t a;
    _init_x_anim(&a, target_obj, start, end, duration, NULL);
    if (completed_cb)
        lv_anim_set_completed_cb(&a, completed_cb);
    if (user_data)
        lv_anim_set_user_data(&a, user_data);
    lv_anim_set_delay(&a, delay);
    lv_anim_start(&a);
}

void eos_lite_anim_move_ver_start(lv_obj_t *target_obj,
                                  int32_t start, int32_t end,
                                  uint32_t duration, uint32_t delay,
                                  lv_anim_completed_cb_t completed_cb, void *user_data)
{
    lv_anim_t a;
    _init_y_anim(&a, target_obj, start, end, duration, NULL);
    if (completed_cb)
        lv_anim_set_completed_cb(&a, completed_cb);
    if (user_data)
        lv_anim_set_user_data(&a, user_data);
    lv_anim_set_delay(&a, delay);
    lv_anim_start(&a);
}

void eos_lite_anim_scale_w_start(lv_obj_t *target_obj,
                                 int32_t start, int32_t end,
                                 uint32_t duration, uint32_t delay,
                                 lv_anim_completed_cb_t completed_cb, void *user_data)
{
    lv_anim_t a;
    _init_width_anim(&a, target_obj, start, end, duration, NULL);
    if (completed_cb)
        lv_anim_set_completed_cb(&a, completed_cb);
    if (user_data)
        lv_anim_set_user_data(&a, user_data);
    lv_anim_set_delay(&a, delay);
    lv_anim_start(&a);
}

void eos_lite_anim_scale_h_start(lv_obj_t *target_obj,
                                 int32_t start, int32_t end,
                                 uint32_t duration, uint32_t delay,
                                 lv_anim_completed_cb_t completed_cb, void *user_data)
{
    lv_anim_t a;
    _init_height_anim(&a, target_obj, start, end, duration, NULL);
    if (completed_cb)
        lv_anim_set_completed_cb(&a, completed_cb);
    if (user_data)
        lv_anim_set_user_data(&a, user_data);
    lv_anim_set_delay(&a, delay);
    lv_anim_start(&a);
}

void eos_lite_anim_transform_scale_start(lv_obj_t *target_obj,
                                         int32_t start, int32_t end,
                                         uint32_t duration, uint32_t delay,
                                         lv_anim_completed_cb_t completed_cb, void *user_data)
{
    lv_anim_t a;
    _init_scale_anim(&a, target_obj, start, end, duration, NULL);
    if (completed_cb)
        lv_anim_set_completed_cb(&a, completed_cb);
    if (user_data)
        lv_anim_set_user_data(&a, user_data);
    lv_anim_set_delay(&a, delay);
    lv_anim_start(&a);
}

void eos_lite_anim_fade_start(lv_obj_t *target_obj,
                              int32_t start, int32_t end,
                              uint32_t duration, uint32_t delay,
                              lv_anim_completed_cb_t completed_cb, void *user_data)
{
    lv_anim_t a;
    _init_opa_anim(&a, target_obj, start, end, duration, NULL);
    if (completed_cb)
        lv_anim_set_completed_cb(&a, completed_cb);
    if (user_data)
        lv_anim_set_user_data(&a, user_data);
    lv_anim_set_delay(&a, delay);
    lv_anim_start(&a);
}

void eos_lite_anim_fade_layered_start(lv_obj_t *target_obj,
                                      int32_t start, int32_t end,
                                      uint32_t duration, uint32_t delay,
                                      lv_anim_completed_cb_t completed_cb, void *user_data)
{
    lv_anim_t a;
    _init_opa_layered_anim(&a, target_obj, start, end, duration, NULL);
    if (completed_cb)
        lv_anim_set_completed_cb(&a, completed_cb);
    if (user_data)
        lv_anim_set_user_data(&a, user_data);
    lv_anim_set_delay(&a, delay);
    lv_anim_start(&a);
}

/************************** 「动画启动」 **************************/

bool eos_anim_start(eos_anim_t *anim)
{
    if (!anim || !anim->anim_timeline)
        return false;

    eos_anim_blocker_show();

    // 添加所有子动画到时间线
    switch (anim->type)
    {
    case EOS_ANIM_SCALE:
        lv_anim_timeline_add(anim->anim_timeline, 0, &anim->anim.scale.a_width);
        lv_anim_timeline_add(anim->anim_timeline, 0, &anim->anim.scale.a_height);
        break;
    case EOS_ANIM_FADE:
        lv_anim_timeline_add(anim->anim_timeline, 0, &anim->anim.fade.a_opa);
        break;
    case EOS_ANIM_MOVE:
        if (!anim->cfg.move.disable_x)
            lv_anim_timeline_add(anim->anim_timeline, 0, &anim->anim.move.a_x);
        if (!anim->cfg.move.disable_y)
            lv_anim_timeline_add(anim->anim_timeline, 0, &anim->anim.move.a_y);
        break;
    case EOS_ANIM_TRANSFORM_SCALE:
        lv_anim_start(&anim->anim.transform_scale.a_scale);
        return true;
    default:
        return false;
    }

    lv_anim_timeline_start(anim->anim_timeline);
    return true;
}
