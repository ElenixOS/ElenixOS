/**
 * @file eos_event_def.h
 * @brief Define user event range for ElenixOS
 */

#ifndef EOS_EVENT_DEF_H
#define EOS_EVENT_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Event range definition:
 *
 * [LV_EVENT_ALL(0)]
 *
 * LVGL event range
 *
 * [LV_EVENT_LAST]
 * [EOS_EVENT_BASE(LV_EVENT_LAST)]
 *
 * ElenixOS event range
 *
 * [EOS_EVENT_LAST]
 *
 * ...
 *
 * [EOS_EVENT_USER_BASE]
 *
 * User-defined event range (for applications and scripts, etc.)
 *
 * [EOS_EVENT_USER_MAX]
 *
 * [LV_EVENT_PREPROCESS(0x8000)]
 */

#define EOS_EVENT_USER_BASE (0x1000)
#define EOS_EVENT_USER_MAX  (0x7FFF)

#ifdef __cplusplus
}
#endif

#endif /* EOS_EVENT_DEF_H */
