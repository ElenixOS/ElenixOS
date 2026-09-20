/**
 * @file eos_pkg_installer.h
 * @brief EPK package installation dispatcher
 */

#ifndef EOS_PKG_INSTALLER_H
#define EOS_PKG_INSTALLER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ---------------------------------------------------*/
#include "eos_core.h"

/* Public macros ----------------------------------------------*/

/* Public typedefs --------------------------------------------*/

/* Public function prototypes ---------------------------------*/

/**
 * @brief Install an EPK package and dispatch by its Header Package Type
 * @param pkg_path EPK package path
 * @return eos_result_t Installation result
 */
eos_result_t eos_pkg_install(const char *pkg_path);

#ifdef __cplusplus
}
#endif

#endif /* EOS_PKG_INSTALLER_H */
