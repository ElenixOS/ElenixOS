/**
 * @file elena_os_sysmon.c
 * @brief System resource usage monitoring
 * @author Sab1e
 * @date 2025-11-29
 */

#include "elena_os_config.h"

#if EOS_SYSMON_TYPE == EOS_SYSMON_USE_INTERNAL

#include "elena_os_sysmon.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "elena_os_port.h"
#include "elena_os_mem.h"

/* Macros and Definitions -------------------------------------*/

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/

void eos_sysmon_print(void)
{
    static lv_mem_monitor_t *prev_mon = NULL;
    lv_mem_monitor_t *mon = eos_malloc(sizeof(lv_mem_monitor_t));
    if (!mon)
        return;

    lv_mem_monitor(mon);

    printf("=========== LVGL Memory Monitor ===========\n");
    printf("Total size         : %zu bytes\n", mon->total_size);
    printf("Free count         : %zu blocks\n", mon->free_cnt);
    printf("Free size          : %zu bytes\n", mon->free_size);
    printf("Free biggest block : %zu bytes\n", mon->free_biggest_size);
    printf("Used count         : %zu blocks\n", mon->used_cnt);
    printf("Max used           : %zu bytes\n", mon->max_used);
    printf("Used percent       : %u%%\n", mon->used_pct);
    printf("Fragmentation      : %u%%\n", mon->frag_pct);

    if (prev_mon)
    {
        printf("--------------- Diff Since Last ---------------\n");
        printf("Free size diff          : %+zd bytes\n",
               (ssize_t)mon->free_size - (ssize_t)prev_mon->free_size);
        printf("Free biggest diff       : %+zd bytes\n",
               (ssize_t)mon->free_biggest_size - (ssize_t)prev_mon->free_biggest_size);
        printf("Used count diff         : %+zd blocks\n",
               (ssize_t)mon->used_cnt - (ssize_t)prev_mon->used_cnt);
        printf("Max used diff           : %+zd bytes\n",
               (ssize_t)mon->max_used - (ssize_t)prev_mon->max_used);
        printf("Used percent diff       : %+d%%\n",
               (int)mon->used_pct - (int)prev_mon->used_pct);
        printf("Frag percent diff       : %+d%%\n",
               (int)mon->frag_pct - (int)prev_mon->frag_pct);
    }
    printf("================================================\n");

    if (prev_mon)
    {
        eos_free(prev_mon);
    }

    prev_mon = mon;
}
#endif /* EOS_SYSMON_TYPE */
