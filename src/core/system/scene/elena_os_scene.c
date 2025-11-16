/**
 * @file elena_os_scene.c
 * @brief 场景系统
 * @author Sab1e
 * @date 2025-11-11
 */

#include "elena_os_scene.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#define EOS_LOG_TAG "Scene"
#include "elena_os_log.h"
#include "elena_os_nav.h"
#include "elena_os_config.h"
#if EOS_SYSTEM_MODE == TEST_MODE
#include "elena_os_test.h"
#endif /* EOS_SYSTEM_MODE */
#include "elena_os_control_center.h"
#include "elena_os_msg_list.h"
#include "elena_os_crown.h"
#include "elena_os_event.h"
/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

/************************** 场景列表 **************************/
#if EOS_SYSTEM_MODE == TEST_MODE
static eos_scene_t test_scene = {
    .type = EOS_SCENE_TEST};
#endif /* EOS_SYSTEM_MODE */
static eos_scene_t watchface_scene = {
    .type = EOS_SCENE_WATCHFACE};

static eos_scene_t app_list_scene = {
    .type = EOS_SCENE_APP_LIST};

static eos_scene_t watchface_list_scene = {
    .type = EOS_SCENE_WATCHFACE_LIST};

static eos_scene_t nav_scene = {
    .type = EOS_SCENE_NAVIGATION};
/************************** 局部变量 **************************/
static eos_scene_t *current_scene = NULL;
static eos_scene_t *last_scene = NULL;

/* Function Implementations -----------------------------------*/

#if EOS_COMPILE_MODE == DEUBG
const char *eos_scene_type_t_to_string(eos_scene_type_t v)
{
    switch (v)
    {
    case EOS_SCENE_UNKNOWN:
        return "EOS_SCENE_UNKNOWN";
    case EOS_SCENE_WATCHFACE:
        return "EOS_SCENE_WATCHFACE";
    case EOS_SCENE_APP_LIST:
        return "EOS_SCENE_APP_LIST";
    case EOS_SCENE_WATCHFACE_LIST:
        return "EOS_SCENE_WATCHFACE_LIST";
    case EOS_SCENE_NAVIGATION:
        return "EOS_SCENE_NAVIGATION";
    default:
        return "UNKNOWN";
    }
}
#endif /* EOS_COMPILE_MODE */

static void _set_current_scene(eos_scene_t *scene)
{
    current_scene = scene;
    if (scene == &watchface_scene)
    {
        eos_control_center_show();
        eos_msg_list_show();
    }
    else
    {
        eos_control_center_hide();
        eos_msg_list_hide();
    }

#if EOS_COMPILE_MODE == DEUBG
    EOS_LOG_I("Current scene: %s", eos_scene_type_t_to_string(current_scene->type));
#else
    EOS_LOG_I("Current scene: eos_scene_type_t[%d]", current_scene->type);
#endif /* EOS_COMPILE_MODE */
}

void eos_scene_back(void)
{
    EOS_CHECK_PTR_RETURN(current_scene);
    if (current_scene == last_scene)
        return;
    // 返回上一个场景
    _set_current_scene(last_scene);
}

void eos_scene_switch(eos_scene_type_t type)
{
    EOS_CHECK_PTR_RETURN(current_scene);
    if (current_scene->type == type)
        return;
    if (current_scene->exit && (type != EOS_SCENE_NAVIGATION))
        current_scene->exit();
    last_scene = current_scene;
    switch (type)
    {
    case EOS_SCENE_WATCHFACE:
        _set_current_scene(&watchface_scene);
        break;
    case EOS_SCENE_APP_LIST:
        _set_current_scene(&app_list_scene);
        break;
    case EOS_SCENE_WATCHFACE_LIST:
        _set_current_scene(&watchface_list_scene);
        break;
#if EOS_SYSTEM_MODE == TEST_MODE
    case EOS_SCENE_TEST:
        _set_current_scene(&test_scene);
        break;
#endif /* EOS_SYSTEM_MODE */
    case EOS_SCENE_NAVIGATION:
        _set_current_scene(&nav_scene);
        return;
    case EOS_SCENE_UNKNOWN:
    default:
        EOS_LOG_E("Unknown scene");
        return;
    }
    current_scene->entry();
}

void eos_scene_auto_switch(void)
{
    EOS_CHECK_PTR_RETURN(current_scene);
    if (current_scene->exit)
        current_scene->exit();
    switch (current_scene->type)
    {
    case EOS_SCENE_WATCHFACE:
        // 如果当前在表盘页面，切换场景会进入应用列表
        _set_current_scene(&app_list_scene);
        break;
    case EOS_SCENE_APP_LIST:
        // 如果当前在应用列表页面，切换场景会进入表盘
        _set_current_scene(&watchface_scene);
        break;
    case EOS_SCENE_WATCHFACE_LIST:
        // 如果当前在表盘列表页面，切换场景会进入表盘
        _set_current_scene(&watchface_scene);
        break;
#if EOS_SYSTEM_MODE == TEST_MODE
    case EOS_SCENE_TEST:
        _set_current_scene(&watchface_scene);
        break;
#endif /* EOS_SYSTEM_MODE */
    case EOS_SCENE_NAVIGATION:
        // 如果当前在导航页面，切换场景会返回上级页面
        _set_current_scene(&nav_scene);
        eos_nav_back_clean();
        return;
    case EOS_SCENE_UNKNOWN:
    default:
        EOS_LOG_E("Unknown scene");
        return;
    }
    current_scene->entry();
}

static void _screen_load_cb(lv_event_t *e)
{
    eos_crown_encoder_set_target_screen(lv_event_get_param(e));
}

void eos_scene_init(
    eos_scene_entry_t watchface_entry,
    eos_scene_exit_t watchface_exit,
    eos_scene_entry_t app_list_entry,
    eos_scene_entry_t watchface_list_entry)
{
    eos_event_add_global_cb(_screen_load_cb, EOS_EVENT_GLOBAL_SCREEN_LOADED, NULL);
    watchface_scene.entry = watchface_entry;
    watchface_scene.exit = watchface_exit;
    app_list_scene.entry = app_list_entry;
    watchface_list_scene.entry = watchface_list_entry;
#if EOS_SYSTEM_MODE == TEST_MODE
    test_scene.entry = eos_test_start;
    _set_current_scene(&test_scene);
#else
    _set_current_scene(&watchface_scene);
#endif /* EOS_SYSTEM_MODE */

    current_scene->entry();
}
