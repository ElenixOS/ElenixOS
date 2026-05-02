/**
 * @file eos_port.c
 * @brief ElenixOS porting
 */

#include "eos_port.h"

/* Includes ---------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eos_log.h"
#include "eos_mem.h"

/* Variables --------------------------------------------------*/

/* Function Implementations -----------------------------------*/
EOS_WEAK void eos_delay(uint32_t ms)
{
    LV_UNUSED(ms);
    return;
}

EOS_WEAK void eos_cpu_reset(void)
{
    return;
}

EOS_WEAK void eos_bluetooth_enable(void)
{
    return;
}

EOS_WEAK void eos_bluetooth_disable(void)
{
    return;
}

EOS_WEAK void eos_locate_phone(void)
{
    return;
}

EOS_WEAK void eos_speaker_set_volume(uint8_t volume)
{
    LV_UNUSED(volume);
    return;
}

EOS_WEAK bool eos_speaker_detect(void)
{
    return false;
}

EOS_WEAK bool eos_microphone_detect(void)
{
    return false;
}

EOS_WEAK eos_audio_state_t eos_audio_get_state(void)
{
    return EOS_AUDIO_STATE_UNAVAILABLE;
}

EOS_WEAK int eos_audio_play_file(const char *file_path)
{
    LV_UNUSED(file_path);
    return -1;
}

EOS_WEAK int eos_audio_stop(void)
{
    return -1;
}
