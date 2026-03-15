/**
 * @file sni_api_lv_special.h
 * @brief LVGL SNI 手写特殊包装函数声明
 * @author Sab1e
 * @date 2026-03-14
 */

#ifndef SNI_API_LV_SPECIAL_H
#define SNI_API_LV_SPECIAL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include "jerryscript.h"

/* Macros and Definitions -------------------------------------*/

/* Function Declarations --------------------------------------*/

/* obj --------------------------------------------------------*/
jerry_value_t sni_api_lv_obj_add_event_cb(const jerry_call_info_t *call_info_p,
                                          const jerry_value_t args_p[],
                                          const jerry_length_t args_count);

jerry_value_t sni_api_lv_obj_remove_event_cb(const jerry_call_info_t *call_info_p,
                                             const jerry_value_t args_p[],
                                             const jerry_length_t args_count);

jerry_value_t sni_api_lv_obj_remove_event_dsc(const jerry_call_info_t *call_info_p,
                                              const jerry_value_t args_p[],
                                              const jerry_length_t args_count);

jerry_value_t sni_api_lv_obj_remove_event_cb_with_user_data(const jerry_call_info_t *call_info_p,
                                                            const jerry_value_t args_p[],
                                                            const jerry_length_t args_count);

/* timer ------------------------------------------------------*/
jerry_value_t sni_api_ctor_timer(const jerry_call_info_t *call_info_p,
                                 const jerry_value_t args_p[],
                                 const jerry_length_t args_count);

jerry_value_t sni_api_lv_timer_set_cb(const jerry_call_info_t *call_info_p,
                                      const jerry_value_t args_p[],
                                      const jerry_length_t args_count);

jerry_value_t sni_api_lv_timer_delete(const jerry_call_info_t *call_info_p,
                                      const jerry_value_t args_p[],
                                      const jerry_length_t args_count);

jerry_value_t sni_api_prop_set_timer_cb(const jerry_call_info_t *call_info_p,
                                        const jerry_value_t args_p[],
                                        const jerry_length_t args_count);

/* anim -------------------------------------------------------*/
jerry_value_t sni_api_ctor_anim(const jerry_call_info_t *call_info_p,
                                const jerry_value_t args_p[],
                                const jerry_length_t args_count);

jerry_value_t sni_api_lv_anim_set_values(const jerry_call_info_t *call_info_p,
                                         const jerry_value_t args_p[],
                                         const jerry_length_t args_count);

jerry_value_t sni_api_lv_anim_set_duration(const jerry_call_info_t *call_info_p,
                                           const jerry_value_t args_p[],
                                           const jerry_length_t args_count);

jerry_value_t sni_api_lv_anim_set_delay(const jerry_call_info_t *call_info_p,
                                        const jerry_value_t args_p[],
                                        const jerry_length_t args_count);

jerry_value_t sni_api_lv_anim_set_repeat_count(const jerry_call_info_t *call_info_p,
                                               const jerry_value_t args_p[],
                                               const jerry_length_t args_count);

jerry_value_t sni_api_lv_anim_start(const jerry_call_info_t *call_info_p,
                                    const jerry_value_t args_p[],
                                    const jerry_length_t args_count);

jerry_value_t sni_api_lv_anim_set_custom_exec_cb(const jerry_call_info_t *call_info_p,
                                                 const jerry_value_t args_p[],
                                                 const jerry_length_t args_count);

jerry_value_t sni_api_lv_anim_set_start_cb(const jerry_call_info_t *call_info_p,
                                           const jerry_value_t args_p[],
                                           const jerry_length_t args_count);

jerry_value_t sni_api_lv_anim_set_completed_cb(const jerry_call_info_t *call_info_p,
                                               const jerry_value_t args_p[],
                                               const jerry_length_t args_count);

jerry_value_t sni_api_lv_anim_set_deleted_cb(const jerry_call_info_t *call_info_p,
                                             const jerry_value_t args_p[],
                                             const jerry_length_t args_count);

jerry_value_t sni_api_lv_anim_set_get_value_cb(const jerry_call_info_t *call_info_p,
                                               const jerry_value_t args_p[],
                                               const jerry_length_t args_count);

jerry_value_t sni_api_lv_anim_set_path_cb(const jerry_call_info_t *call_info_p,
                                          const jerry_value_t args_p[],
                                          const jerry_length_t args_count);

jerry_value_t sni_api_lv_anim_set_var(const jerry_call_info_t *call_info_p,
                                      const jerry_value_t args_p[],
                                      const jerry_length_t args_count);

jerry_value_t sni_api_prop_set_anim_var(const jerry_call_info_t *call_info_p,
                                        const jerry_value_t args_p[],
                                        const jerry_length_t args_count);

/* buttonmatrix -----------------------------------------------*/
jerry_value_t sni_api_ctor_buttonmatrix(const jerry_call_info_t *call_info_p,
                                        const jerry_value_t args_p[],
                                        const jerry_length_t args_count);

jerry_value_t sni_api_lv_buttonmatrix_set_map(const jerry_call_info_t *call_info_p,
                                              const jerry_value_t args_p[],
                                              const jerry_length_t args_count);

jerry_value_t sni_api_lv_buttonmatrix_set_ctrl_map(const jerry_call_info_t *call_info_p,
                                                   const jerry_value_t args_p[],
                                                   const jerry_length_t args_count);

#ifdef __cplusplus
}
#endif

#endif /* SNI_API_LV_SPECIAL_H */
