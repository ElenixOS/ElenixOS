/**
 * @file elena_os_anim.h
 * @brief 动画库
 * @author Sab1e
 * @date 2025-08-14
 * @details
 *
 * # 动画系统
 *
 * 基于 LVGL 动画封装的动画库，为 ElenaOS 提供统一且易用的动画接口。
 * 该库将常用动画效果进行归纳与标准化，并以枚举类型 `eos_anim` 统一表示，
 * 便于在系统中以一致的方式创建和使用标准动画。
 *
 * ## 使用方法
 *
 * ### 使用专用函数创建不同类型的动画，例如缩放动画、透明度动画：
 *
 * ```c
 * eos_anim_t *anim = eos_anim_scale_create(obj, w_start, w_end, h_start, h_end, duration);
 * eos_anim_t *fade_anim = eos_anim_fade_create(obj, opa_start, opa_end, duration);
 * ```
 *
 * ### 动画播放完成后触发的回调：
 *
 * ```c
 * eos_anim_add_cb(anim, user_cb, user_data);
 * ```
 *
 * ### 启动动画
 *
 * ```c
 * eos_anim_start(anim);
 * ```
 *
 * ### 快捷方式（直接创建并播放）
 *
 * ```c
 * eos_anim_scale_start(obj, w_start, w_end, h_start, h_end, duration);
 * ```
 *
 *  - 不需要手动管理动画对象。
 *  - 完成后自动释放。
 *
 * ### 删除动画
 *
 * 如果想提前停止或手动清理动画：
 *
 * ```c
 * eos_anim_del(anim);
 * ```
 *
 * ## 轻动画
 *
 * 轻动画（Lite Anim）是基于 LVGL 动画系统的轻量封装，
 * 提供统一且简洁的接口，用于创建单一类型的 LVGL 动画。
 * 每个轻动画在调用后会立即生成并启动，无需额外配置，
 * 适合场景内快速使用的简单过渡效果。
 *
 * >[!NOTE] 轻动画传入的播放完成回调与标准动画库不同，
 * 轻动画直接使用 LVGL 默认回调函数。
 *
 * ## 注意事项
 *
 *  - 动画对象 eos_anim_t 会自动释放。
 *  - 支持多动画并行，通过 anim_count 管理子动画完成情况。
 *  - 可扩展不同类型的动画（缩放、透明度、宽高、位置等）。
 *  - 多个动画可以同时启动，但同类型动画可能出现竞态。
 */

#ifndef ELENA_OS_ANIM_H
#define ELENA_OS_ANIM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/**
 * @brief 所有动画类型，可以继续添加。
 */
typedef enum
{
    EOS_ANIM_SCALE,          /**< 缩放动画 */
    EOS_ANIM_FADE,           /**< 透明度渐变动画 */
    EOS_ANIM_MOVE,           /**< 位置移动动画 */
    EOS_ANIM_TRANSFORM_SCALE /**< 缩放动画，支持 Label */
    // 此处可以添加其他动画类型
} eos_anim;
typedef struct eos_anim_t eos_anim_t; // 预定义
/**
 * @brief 回调函数的类型定义
 */
typedef void (*eos_anim_cb_t)(eos_anim_t *a);
/**
 * @brief ElenaOS 动画对象的结构体
 */
struct eos_anim_t
{
    lv_anim_timeline_t *anim_timeline; /**< 动画的时间线指针 */
    eos_anim type;                     /**< 动画类型 */
    uint32_t anim_count;               /**< 该类型总的动画数量 */
    uint32_t anim_completed_count;     /**< 当前动画已经播放完毕的数量（用于判断动画是不是全部播放完毕）*/
    eos_anim_cb_t user_cb;             /**< 用户设定的回调函数 */
    lv_obj_t *tar_obj;
    bool auto_delete_obj; /**< 动画播放完成时自动删除被绑定对象 */
    void *user_data;      /**< 用户数据 */
    union
    { /**< 用于存储动画对象的共用体 */
        struct
        {
            lv_anim_t a_width;  /**< 缩放动画的宽度动画对象 */
            lv_anim_t a_height; /**< 缩放动画的高度动画对象 */
        } scale;
        struct
        {
            lv_anim_t a_opa;
        } fade;
        struct
        {
            lv_anim_t a_x; /**< X 轴位置动画 */
            lv_anim_t a_y; /**< Y 轴位置动画 */
        } move;
        struct
        {
            lv_anim_t a_scale;
        } transform_scale;
        // 此处可以添加其他动画类型的结构
    } anim;
    union
    { /**< 用于存储动画对象的共用体 */
        struct
        {
            bool layered; /**< 是否随子对象调整透明度 */
        } fade;
        struct
        {
            bool disable_x; /**< 是否随子对象调整透明度 */
            bool disable_y; /**< 是否随子对象调整透明度 */
        } move;
        // 此处可以添加其他动画类型的结构
    } cfg;
};
/* Public function prototypes --------------------------------*/

/************************** 通用 **************************/

/**
 * @brief 当动画播放完毕时自动删除`tar_obj`
 * @param anim 目标动画
 */
void eos_anim_set_auto_delete(eos_anim_t *anim);

/**
 * @brief 开始播放动画
 * @param anim 由create函数创建的动画对象
 * @return 成功返回true，失败返回false
 */
bool eos_anim_start(eos_anim_t* anim);

/**
 * @brief 给动画设置播放完毕时的回调
 * @param anim 要设置的动画
 * @param user_cb 回调函数
 * @param user_data 用户数据指针（需要用户自行管理生命周期）
 */
void eos_anim_add_cb(eos_anim_t *anim,
                        eos_anim_cb_t user_cb,
                        void *user_data);

/**
 * @brief 获取动画对象的用户数据
 */
void* eos_anim_get_user_data(eos_anim_t *anim);
/**
 * @brief 删除动画对象
 * @param anim 动画对象指针
 * @note 如果动画正在运行会自动停止
 */
void eos_anim_del(eos_anim_t* anim);
/**
 * @brief 添加透明阻碍层，禁止用户输入
 */
void eos_anim_blocker_show(void);
/**
 * @brief 移除透明阻碍层
 */
void eos_anim_blocker_hide(void);

/************************** 动画 **************************/

/**
 * @brief 创建缩放动画对象
 * @param tar_obj 目标对象
 * @param w_start 起始宽度
 * @param w_end 结束宽度
 * @param h_start 起始高度
 * @param h_end 结束高度
 * @param duration 持续时间(ms)
 * @return 创建的动画对象指针，失败返回NULL
 */
eos_anim_t* eos_anim_scale_create(lv_obj_t* tar_obj,
                                int32_t w_start, int32_t w_end,
                                int32_t h_start, int32_t h_end,
                                uint32_t duration, bool auto_delete);

/**
 * @brief 创建并立即播放缩放动画，无法设置回调
 * @note 动画完成后会自动删除
 */
void eos_anim_scale_start(lv_obj_t* tar_obj,
                                int32_t w_start, int32_t w_end,
                                int32_t h_start, int32_t h_end,
                                uint32_t duration, bool auto_delete);

/**
 * @brief 创建透明度渐变动画
 */
eos_anim_t *eos_anim_fade_create(lv_obj_t *tar_obj,
                                 int32_t opa_start,
                                 int32_t opa_end,
                                 uint32_t duration, bool auto_delete);
/**
 * @brief 创建并立即播放透明度渐变动画
 */
void eos_anim_fade_start(lv_obj_t *tar_obj,
                         int32_t opa_start,
                         int32_t opa_end,
                         uint32_t duration, bool auto_delete);

/**
 * @brief 创建并返回一个移动动画对象（位置从 start_x,start_y -> end_x,end_y）
 */
eos_anim_t *eos_anim_move_create(lv_obj_t *tar_obj,
                                 int32_t start_x, int32_t start_y,
                                 int32_t end_x, int32_t end_y,
                                 uint32_t duration, bool auto_delete);

/**
 * @brief 创建并立即播放移动动画
 */
void eos_anim_move_start(lv_obj_t *tar_obj,
                         int32_t start_x, int32_t start_y,
                         int32_t end_x, int32_t end_y,
                         uint32_t duration, bool auto_delete);

/**
 * @brief Fade 动画是否按层调整透明度
 */
void eos_anim_fade_set_layered(eos_anim_t *a, bool layered);

/**
 * @brief 创建变换缩放动画（基于 transform_scale）
 * @note 缩放是整体缩放，即宽高同时缩放
 */
eos_anim_t *eos_anim_transform_scale_create(lv_obj_t *tar_obj,
                                            int32_t scale_start, int32_t scale_end,
                                            uint32_t duration, bool auto_delete);

/**
 * @brief 启动变换缩放动画（带高级配置）
 */
void eos_anim_transform_scale_start_ex(lv_obj_t *tar_obj,
                                      int32_t scale_start, int32_t scale_end,
                                      uint32_t duration, uint32_t playback_time,
                                      uint16_t repeat_count, bool auto_delete);

/**
 * @brief 启动简单的变换缩放动画（默认参数）
 */
void eos_anim_transform_scale_start(lv_obj_t *tar_obj,
                                    int32_t scale_start, int32_t scale_end,
                                    uint32_t duration, bool auto_delete);

/************************** 轻动画 **************************/

/**
 * @brief 创建水平移动动画
 * @param target_obj 目标对象
 * @param start 起始 X 坐标
 * @param end 结束 X 坐标
 * @param duration 动画持续时间（毫秒）
 * @param delay 动画延迟时间（毫秒）
 * @param completed_cb 动画完成回调，可为 NULL
 * @param user_data 绑定到`lv_anim_t`上的用户数据，可为 NULL
 */
void eos_lite_anim_move_hor_start(lv_obj_t *target_obj,
                                  int32_t start, int32_t end,
                                  uint32_t duration, uint32_t delay,
                                  lv_anim_completed_cb_t completed_cb, void *user_data);

/**
 * @brief 创建垂直移动动画
 * @param target_obj 目标对象
 * @param start 起始 Y 坐标
 * @param end 结束 Y 坐标
 * @param duration 动画持续时间（毫秒）
 * @param delay 动画延迟时间（毫秒）
 * @param completed_cb 动画完成回调，可为 NULL
 * @param user_data 绑定到`lv_anim_t`上的用户数据，可为 NULL
 */
void eos_lite_anim_move_ver_start(lv_obj_t *target_obj,
                                  int32_t start, int32_t end,
                                  uint32_t duration, uint32_t delay,
                                  lv_anim_completed_cb_t completed_cb, void *user_data);

/**
 * @brief 创建宽度缩放动画
 * @param target_obj 目标对象
 * @param start 起始宽度
 * @param end 结束宽度
 * @param duration 动画持续时间（毫秒）
 * @param delay 动画延迟时间（毫秒）
 * @param completed_cb 动画完成回调，可为 NULL
 * @param user_data 绑定到`lv_anim_t`上的用户数据，可为 NULL
 */
void eos_lite_anim_scale_w_start(lv_obj_t *target_obj,
                                 int32_t start, int32_t end,
                                 uint32_t duration, uint32_t delay,
                                 lv_anim_completed_cb_t completed_cb, void *user_data);

/**
 * @brief 创建高度缩放动画
 * @param target_obj 目标对象
 * @param start 起始高度
 * @param end 结束高度
 * @param duration 动画持续时间（毫秒）
 * @param delay 动画延迟时间（毫秒）
 * @param completed_cb 动画完成回调，可为 NULL
 * @param user_data 绑定到`lv_anim_t`上的用户数据，可为 NULL
 */
void eos_lite_anim_scale_h_start(lv_obj_t *target_obj,
                                 int32_t start, int32_t end,
                                 uint32_t duration, uint32_t delay,
                                 lv_anim_completed_cb_t completed_cb, void *user_data);

/**
 * @brief 创建基于 transform_scale 的缩放动画
 * @param target_obj 目标对象
 * @param start 起始缩放值
 * @param end 结束缩放值
 * @param duration 动画持续时间（毫秒）
 * @param delay 动画延迟时间（毫秒）
 * @param completed_cb 动画完成回调，可为 NULL
 * @param user_data 绑定到`lv_anim_t`上的用户数据，可为 NULL
 */
void eos_lite_anim_transform_scale_start(lv_obj_t *target_obj,
                                         int32_t start, int32_t end,
                                         uint32_t duration, uint32_t delay,
                                         lv_anim_completed_cb_t completed_cb, void *user_data);

/**
 * @brief 创建透明度渐变动画
 * @param target_obj 目标对象
 * @param start 起始透明度（0-255），适用`LV_OPA_*`
 * @param end 结束透明度（0-255），适用`LV_OPA_*`
 * @param duration 动画持续时间（毫秒）
 * @param delay 动画延迟时间（毫秒）
 * @param completed_cb 动画完成回调，可为 NULL
 * @param user_data 绑定到`lv_anim_t`上的用户数据，可为 NULL
 */
void eos_lite_anim_fade_start(lv_obj_t *target_obj,
                              int32_t start, int32_t end,
                              uint32_t duration, uint32_t delay,
                              lv_anim_completed_cb_t completed_cb, void *user_data);

/**
 * @brief 创建透明度渐变动画
 * @param target_obj 目标对象
 * @param start 起始透明度（0-255），适用`LV_OPA_*`
 * @param end 结束透明度（0-255），适用`LV_OPA_*`
 * @param duration 动画持续时间（毫秒）
 * @param delay 动画延迟时间（毫秒）
 * @param completed_cb 动画完成回调，可为 NULL
 * @param user_data 绑定到`lv_anim_t`上的用户数据，可为 NULL
 */
void eos_lite_anim_fade_layered_start(lv_obj_t *target_obj,
                              int32_t start, int32_t end,
                              uint32_t duration, uint32_t delay,
                              lv_anim_completed_cb_t completed_cb, void *user_data);

#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_ANIM_H */
