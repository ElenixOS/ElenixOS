/**
 * @file sni.h
 * @brief SNI
 */

#ifndef SNI_H
#define SNI_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "jerryscript.h"
/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes --------------------------------*/

/**
 * @brief Initialize SNI
 */
void sni_init(void);

/**
 * @brief Mount SNI to specified JerryScript Realm
 *
 * @param js_realm JerryScript Realm object to mount to
 */
void sni_mount(jerry_value_t js_realm);

/**
 * @brief Unmount SNI resources associated with a Realm.
 *
 * Called at a well-defined point when a realm is destroyed, to clean up
 * any native handles (timers, event callbacks, etc.) before the realm is
 * freed.  Currently a placeholder – the implementation will be added when
 * sandbox resource tracking is introduced.
 *
 * @param js_realm JerryScript Realm object being torn down
 */
void sni_unmount(jerry_value_t js_realm);

#ifdef __cplusplus
}
#endif

#endif /* SNI_H */
