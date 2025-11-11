/**
 * @file elena_os_log.h
 * @brief 日志系统
 * @author Sab1e
 * @date 2025-08-14
 */

#ifndef ELENA_OS_LOG_H
#define ELENA_OS_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"
#include "elena_os_config.h"
#if defined(EOS_USE_MSH)
#include "msh.h"
#endif
#include "elena_os_port.h"
/* Public macros ----------------------------------------------*/

/************************** 彩色日志 **************************/
#if EOS_LOG_USE_COLOR
#define EOS_LOG_COLOR_RESET   "\033[0m"
#define EOS_LOG_COLOR_RED     "\033[31m"
#define EOS_LOG_COLOR_GREEN   "\033[32m"
#define EOS_LOG_COLOR_YELLOW  "\033[33m"
#define EOS_LOG_COLOR_BLUE    "\033[34m"
#define EOS_LOG_COLOR_CYAN    "\033[36m"
#else
#define EOS_LOG_COLOR_RESET   ""
#define EOS_LOG_COLOR_RED     ""
#define EOS_LOG_COLOR_GREEN   ""
#define EOS_LOG_COLOR_YELLOW  ""
#define EOS_LOG_COLOR_BLUE    ""
#define EOS_LOG_COLOR_CYAN    ""
#endif /* EOS_LOG_USE_COLOR */


#define EOS_LOG_BASE(level, color, fmt, ...) \
    printf("[%s] " fmt "\n", \
           level, ##__VA_ARGS__)

#define EOS_LOG_ALL(level, color, fmt, ...) \
    printf(color "[%s:%d %s()] %s: " fmt EOS_LOG_COLOR_RESET "\n", \
           __FILE__, __LINE__, __func__, level, ##__VA_ARGS__)

/************************** 日志宏 **************************/

#ifdef EOS_LOG_DISABLE

#define EOS_LOG_D(fmt, ...) do {} while(0)
#define EOS_LOG_I(fmt, ...) do {} while(0)
#define EOS_LOG_W(fmt, ...) do {} while(0)
#define EOS_LOG_E(fmt, ...) do {} while(0)

#else

#ifdef EOS_LOG_TAG
    #define EOS_LOG_FMT(fmt)  "[" EOS_LOG_TAG "] " fmt
#else
    #define EOS_LOG_FMT(fmt)  fmt
#endif /* EOS_LOG_TAG */

#define EOS_LOG_D(fmt, ...) EOS_LOG_BASE("DEBUG", EOS_LOG_COLOR_CYAN, EOS_LOG_FMT(fmt), ##__VA_ARGS__)
#define EOS_LOG_I(fmt, ...) EOS_LOG_BASE("INFO",  EOS_LOG_COLOR_GREEN, EOS_LOG_FMT(fmt), ##__VA_ARGS__)
#define EOS_LOG_W(fmt, ...) EOS_LOG_BASE("WARN",  EOS_LOG_COLOR_YELLOW, EOS_LOG_FMT(fmt), ##__VA_ARGS__)
#define EOS_LOG_E(fmt, ...) EOS_LOG_ALL("ERROR", EOS_LOG_COLOR_RED, EOS_LOG_FMT(fmt), ##__VA_ARGS__)

#endif /* EOS_LOG_DISABLE */
/************************** 内存检查 **************************/


#if defined(EOS_USE_MSH)
#define EOS_MEM(tag)              \
do                                \
{                                 \
    printf("[MEM] %s\n", tag);    \
    msh_exec("list_mem", 8);      \
    msh_exec("list_memheap", 12); \
} while (0)
#else
    #define EOS_MEM(tag)
#endif /* defined(EOS_USE_MSH) */

/************************** 指针检查 **************************/
#define EOS_CHECK_PTR_RETURN(ptr) \
    do { \
        if (!(ptr)) { \
            EOS_LOG_E("NULL pointer at %s:%d, at function: %s", __FILE__, __LINE__, __func__); \
            return; \
        } \
    } while(0)

#define EOS_CHECK_PTR_RETURN_FREE(ptr,free_var) \
    do { \
        if (!(ptr)) { \
            EOS_LOG_E("NULL pointer at %s:%d, at function: %s", __FILE__, __LINE__, __func__); \
            eos_free(free_var); \
            return; \
        } \
    } while(0)

#define EOS_CHECK_PTR_RETURN_VAL(ptr, ret_val) \
    do { \
        if (!(ptr)) { \
            EOS_LOG_E("NULL pointer at %s:%d, at function: %s", __FILE__, __LINE__, __func__); \
            return ret_val; \
        } \
    } while(0)


#define EOS_CHECK_PTR_RETURN_VAL_FREE(ptr, ret_val, free_var) \
    do { \
        if (!(ptr)) { \
            EOS_LOG_E("NULL pointer at %s:%d, at function: %s", __FILE__, __LINE__, __func__); \
            eos_free(free_var); \
            return ret_val; \
        } \
    } while(0)

/************************** 断言宏 **************************/
#if EOS_USE_ASSERT
#define EOS_ASSERT(expr) \
    do { \
        if (!(expr)) { \
            EOS_LOG_E("Assertion failed: %s", #expr); \
            while(1); \
        } \
    } while(0)
#else
#define EOS_ASSERT(expr) ((void)0)
#endif /* EOS_USE_ASSERT */

/************************** 打印坐标 **************************/

#define EOS_DEBUG_PRINT_POS(obj) EOS_LOG_D("Obj[%p] (%d,%d)", obj, lv_obj_get_x(obj), lv_obj_get_y(obj))

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* ELENA_OS_LOG_H */
