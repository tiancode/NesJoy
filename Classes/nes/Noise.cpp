/* This file is part of NesEmulator
 *
 * A Nintendo Entertainment System / Family Computer (Nes/Famicom)
 * Emulator written in C++.
 *
 * Copyright © tianshuai - 2015
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "Noise.h"
#include "APU.h"
#include <stddef.h>

Noise* Noise::__instance = NULL;

Noise* Noise::getInstance()
{
    if(__instance == NULL)
    {
        __instance = new Noise;
    }
    return __instance;
}

void Noise::destroy()
{
    if(__instance != NULL)
    {
        delete __instance;
    }
}

Noise::Noise()
{

}

Noise::~Noise()
{
    
}

void Noise::NOZShutdown()
{
    
}

extern int NozFrequencyTable[][16];
void Noise::NozHardReset()
{
    noz_envelope = 0;
    noz_env_startflag = false;
    noz_env_counter = 0;
    noz_env_devider = 0;
    noz_length_counter_halt_flag = false;
    noz_constant_volume_flag = false;
    noz_volume_decay_time = 0;
    noz_duration_haltRequset = false;
    noz_duration_counter = 0;
    noz_duration_reloadEnabled = false;
    noz_duration_reload = 0;
    noz_duration_reloadRequst = false;
    noz_mode = false;
    noz_shiftRegister = 1;
    noz_cycles = 0;
    noz_frequency = NozFrequencyTable[APU::__instance->systemIndex][0];
    noz_feedback = 0;
    noz_pl_output = 0;
}

void Noise::NozSoftReset()
{
    noz_duration_counter = 0;
    noz_duration_reloadEnabled = false;
}

void Noise::NozClockEnvelope()
{
    if (noz_env_startflag)
    {
        noz_env_startflag = false;
        noz_env_counter = 0xF;
        noz_env_devider = noz_volume_decay_time + 1;
    }
    else
    {
        if (noz_env_devider > 0)
            noz_env_devider--;
        else
        {
            noz_env_devider = noz_volume_decay_time + 1;
            if (noz_env_counter > 0)
                noz_env_counter--;
            else if (noz_length_counter_halt_flag)
                noz_env_counter = 0xF;
        }
    }
    noz_envelope = noz_constant_volume_flag ? noz_volume_decay_time : noz_env_counter;
}

void Noise::NozClockLengthCounter()
{
    if (!noz_length_counter_halt_flag)
    {
        if (noz_duration_counter > 0)
        {
            noz_duration_counter--;
        }
    }
}

void Noise::NozClockSingle()
{
    noz_length_counter_halt_flag = noz_duration_haltRequset;
    if ( APU::__instance->isClockingDuration && noz_duration_counter > 0)
        noz_duration_reloadRequst = false;
    if (noz_duration_reloadRequst)
    {
        if (noz_duration_reloadEnabled)
            noz_duration_counter = noz_duration_reload;
        noz_duration_reloadRequst = false;
    }
    if (--noz_cycles <= 0)
    {
        noz_cycles = noz_frequency;
        
        if (noz_mode)
            noz_feedback = (noz_shiftRegister >> 6 & 0x1) ^ (noz_shiftRegister & 0x1);
        else
            noz_feedback = (noz_shiftRegister >> 1 & 0x1) ^ (noz_shiftRegister & 0x1);
        noz_shiftRegister >>= 1;
        noz_shiftRegister = (noz_shiftRegister & 0x3FFF) | ((noz_feedback & 1) << 14);
        
        if (noz_duration_counter > 0 && (noz_shiftRegister & 1) == 0)
        {
            noz_pl_output = noz_envelope;
            
        }
        else
            noz_pl_output = 0;
        APU::__instance->audio_playback_sample_needed = true;
    }
}
