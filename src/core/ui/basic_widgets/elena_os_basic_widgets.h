/**
 * @file elena_os_basic_widgets.h
 * @brief 基本控件
 * @author Sab1e
 * @date 2025-08-17
 */

#ifndef ELENA_OS_BASIC_WIDGETS_H
#define ELENA_OS_BASIC_WIDGETS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
#include "elena_os_lang.h"

/* Public macros ----------------------------------------------*/

#define EOS_LIST_CONTAINER_HEIGHT 100
#define EOS_LIST_OBJ_RADIUS 25
#define EOS_LIST_SECTION_PLACEHOLDER_HEIGHT 30
#define EOS_LIST_CONTAINER_PAD_ALL 24
#define EOS_ITEM_RADIUS 20

#define EOS_SCREEN_SWITCH_DURATION 300

/* Public typedefs --------------------------------------------*/

/**
 * @brief 列表内的滑块定义
 */
typedef struct
{
    lv_obj_t *slider;
    lv_obj_t *plus_btn;
    lv_obj_t *minus_btn;
    lv_obj_t *plus_label;
    lv_obj_t *minus_label;
    uint16_t plus_label_scale;
    uint16_t minus_label_scale;
} eos_list_slider_t;

/**
 * @brief 设置指定圆角位置，搭配`eos_obj_set_corner_radius_bg()`使用
 */
typedef enum
{
    EOS_ROUND_TOP_LEFT = 0x1 << 0,     /**< 左上角圆角 */
    EOS_ROUND_TOP_RIGHT = 0x1 << 1,    /**< 右上角圆角 */
    EOS_ROUND_BOTTOM_RIGHT = 0x1 << 2, /**< 右下角圆角 */
    EOS_ROUND_BOTTOM_LEFT = 0x1 << 3,  /**< 左下角圆角 */
} eos_corner_round_t;

/* Public function prototypes --------------------------------*/

/**
 * @brief 创建绘制缓冲区
 * @param w 宽度
 * @param h 高度
 * @param cf 色彩格式
 * @param stride 每行字节数，置`0`则自动计算
 * @return lv_draw_buf_t* 创建成功则返回绘制缓冲区，否则返回 NULL
 */
lv_draw_buf_t *eos_draw_buf_create(uint32_t w, uint32_t h, lv_color_format_t cf, uint32_t stride);
/**
 * @brief 销毁绘制缓冲区
 */
void eos_draw_buf_destory(lv_draw_buf_t *draw_buf);
/**
 * @brief 设置开关状态（带有动画）
 * @param sw 目标开关对象
 * @param checked true = 开启，false = 关闭
 */
void eos_switch_set_state(lv_obj_t *sw, bool checked);
/**
 * @brief 创建列表，设置标准样式并添加占位符
 * @param parent 父对象
 * @return lv_obj_t* 列表对象
 */
lv_obj_t *eos_list_create(lv_obj_t *parent);
/**
 * @brief 创建一个返回按钮
 * @param parent 父对象
 * @param show_text 是否显示返回文字
 * @return lv_obj_t* 创建成功则返回 btn 对象，否则返回 NULL
 */
lv_obj_t *eos_back_btn_create(lv_obj_t *parent, bool show_text);
/**
 * @brief 向列表中添加一个带有图标的按钮
 * @param list 目标列表
 * @param icon 左侧图标 支持图片路径
 * @param txt 按钮中的文字
 * @return lv_obj_t* 创建成功则返回按钮对象指针
 *
 * 创建失败则返回 NULL
 */
lv_obj_t *eos_list_add_button(lv_obj_t *list, const void *icon, const char *txt);
/**
 * @brief 向列表中添加一个入口按钮
 *
 * 例如：
 * (Bluetooth   >)
 *
 * @param list 目标列表
 * @param txt  左侧字符串
 * @return lv_obj_t* 创建成功则返回按钮对象指针
 *
 * 创建失败则返回 NULL
 */
lv_obj_t *eos_list_add_entry_button(lv_obj_t *list, const char *txt);
/**
 * @brief 向列表中添加一个入口按钮
 *
 * 例如：
 * (Bluetooth   >)
 *
 * @param list 目标列表
 * @param id  左侧字符串 ID
 * @return lv_obj_t* 创建成功则返回按钮对象指针
 *
 * 创建失败则返回 NULL
 */
lv_obj_t *eos_list_add_entry_button_str_id(lv_obj_t *list, language_id_t id);
/**
 * @brief 向列表中添加指定像素高度的占位符
 * @param list 目标列表
 * @param height 占位符的高度(px)
 * @return lv_obj_t* 创建成功则返回创建的占位符对象指针
 *
 * 创建失败则返回 NULL
 */
lv_obj_t *eos_list_add_placeholder(lv_obj_t *list, uint32_t height);
/**
 * @brief 向列表中添加一个开关
 * @param list 目标列表
 * @param txt 开关功能的描述
 * @return lv_obj_t* 创建成功则返回开关对象（标准 lv_switch 对象）
 *
 * 创建失败则返回 NULL
 */
lv_obj_t *eos_list_add_switch(lv_obj_t *list, const char *txt);
/**
 * @brief 向列表内添加标题文字
 * @param txt 文字内容
 * @return lv_obj_t* 创建成功则返回label对象，否则返回 NULL
 */
lv_obj_t *eos_list_add_title(lv_obj_t *list, const char *txt);
/**
 * @brief 向列表内添加提示文字
 * @param txt 文字内容
 * @return lv_obj_t* 创建成功则返回label对象，否则返回 NULL
 */
lv_obj_t *eos_list_add_comment(lv_obj_t *list, const char *txt);
/**
 * @brief 向列表中添加圆形图标的按钮
 *
 * btn{  [icon]  [txt]  }
 *
 * @param list 目标列表
 * @param circle_color 圆形图标的背景色
 * @param icon 图标
 * @param txt 按钮描述文字
 * @return lv_obj_t* 创建成功则返回按钮对象（标准 lv_button 对象）
 *
 * 创建失败则返回 NULL
 */
lv_obj_t *eos_list_add_circle_icon_button(lv_obj_t *list, lv_color_t circle_color, const void *icon, const char *txt);
/**
 * @brief 向列表中添加圆形图标的按钮
 *
 * btn{  [icon]  [txt]  }
 *
 * @param list 目标列表
 * @param circle_color 圆形图标的背景色
 * @param icon 图标
 * @param id 字符串 ID，用于适配多语言
 * @return lv_obj_t* 创建成功则返回按钮对象（标准 lv_button 对象）
 *
 * 创建失败则返回 NULL
 */
lv_obj_t *eos_list_add_circle_icon_button_str_id(lv_obj_t *list, lv_color_t circle_color, const void *icon_src, lang_string_id_t id);
/**
 * @brief 向列表内创建滑块
 * @param list 目标列表
 * @param txt 滑块功能描述文字（左上角）
 * @return eos_list_slider_t* 创建成功则返回滑块对象
 *
 * 创建失败则返回 NULL
 */
eos_list_slider_t *eos_list_add_slider(lv_obj_t *list, const char *txt);
/**
 * @brief 设置滚动条“减少文本“的缩放
 * @param ls 目标列表滚动条
 * @param scale 缩放值（0～512）
 */
void eos_list_slider_set_minus_label_scale(eos_list_slider_t *ls, uint16_t scale);
/**
 * @brief 设置滚动条“增加文本“的缩放
 * @param ls 目标列表滚动条
 * @param scale 缩放值（0～512）
 */
void eos_list_slider_set_plus_label_scale(eos_list_slider_t *ls, uint16_t scale);
/**
 * @brief 向列表内创建一个容器
 * @param list 目标列表
 * @return lv_obj_t* 创建成功则返回容器对象
 *
 * 创建失败则返回 NULL
 */
lv_obj_t *eos_list_add_container(lv_obj_t *list);
/**
 * @brief 创建一行左右对齐的控件
 * @param parent 父对象
 * @param left_text 左边文本，可以为 NULL
 * @param right_text 右边文本，可以为 NULL
 * @param left_img_path 左边图像路径，如果不需要可传 NULL
 * @param icon_w 图像宽度
 * @param icon_h 图像高度
 * @return lv_obj_t* 返回行对象，方便进一步操作
 */
lv_obj_t *eos_row_create(lv_obj_t *parent,
                            const char *left_text,
                            const char *right_text,
                            const char *left_img_path,
                            int icon_w, int icon_h);
/**
 * @brief 创建一个带有标题的容器（标题位于容器外部）
 * @param list 目标列表
 * @param title 标题字符串
 * @return lv_obj_t* 创建成功则返回内部容器
 *
 * 创建失败则返回 NULL
 */
lv_obj_t *eos_list_add_title_container(lv_obj_t *list, const char *title);
/**
 * @brief 给对象设置指定圆角的背景
 * @param obj 对象
 * @param corners 指定圆角，示例：`EOS_ROUND_TOP_LEFT | EOS_ROUND_BOTTOM_LEFT`
 * @param radius 圆角半径
 * @param color 背景色
 */
void eos_obj_set_corner_radius_bg(lv_obj_t *obj, eos_corner_round_t corners, lv_coord_t radius, lv_color_t color);
/**
 * @brief 删除一个对象的圆角背景
 */
void eos_obj_remove_corner_radius_bg(lv_obj_t *obj);
#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_BASIC_WIDGETS_H */
