/**
 * @file elena_os_misc.c
 * @brief 各种工具函数
 * @author Sab1e
 * @date 2025-08-22
 */

#include "elena_os_misc.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#define EOS_LOG_DISABLE
#define EOS_LOG_TAG "Misc"
#include "elena_os_log.h"
#include "elena_os_port.h"
#include "elena_os_mem.h"
/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

bool eos_is_valid_filename(const char *name)
{
    if (!name || name[0] == '\0')
    {
        EOS_LOG_E("Filename NULL");
        return false; // 空名不行
    }

    const char *invalid_chars = "/\\:*?\"<>|";

    for (const char *p = name; *p; p++)
    {
        // 控制字符不允许
        if ((unsigned char)*p < 32)
        {
            EOS_LOG_E("Filename control char");
            return false;
        }
        // 特殊字符不允许
        if (strchr(invalid_chars, *p))
        {
            EOS_LOG_E("Filename invalid char");
            return false;
        }
    }
    return true; // 合法
}

const char *eos_strdup(const char *s)
{
    if (!s)
        return NULL;
    size_t len = strlen(s) + 1;
    char *copy = eos_malloc(len);
    if (copy)
    {
        memcpy(copy, s, len);
    }
    return copy;
}

void eos_pkg_free(script_pkg_t *pkg)
{
    EOS_CHECK_PTR_RETURN(pkg);

    if (pkg->id)
        eos_free((void *)pkg->id);
    if (pkg->name)
        eos_free((void *)pkg->name);
    if (pkg->version)
        eos_free((void *)pkg->version);
    if (pkg->author)
        eos_free((void *)pkg->author);
    if (pkg->description)
        eos_free((void *)pkg->description);
    if (pkg->script_str)
        eos_free((void *)pkg->script_str);
    if (pkg->base_path)
        eos_free((void *)pkg->base_path);
    pkg->id = NULL;
    pkg->name = NULL;
    pkg->type = SCRIPT_TYPE_UNKNOWN;
    pkg->version = NULL;
    pkg->author = NULL;
    pkg->description = NULL;
    pkg->script_str = NULL;
    pkg->base_path = NULL;
}
