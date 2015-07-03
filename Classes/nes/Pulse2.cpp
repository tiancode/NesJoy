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

#include "Pulse2.h"
#include "APU.h"

#include <stddef.h>

Pulse2* Pulse2::__instance = NULL;

Pulse2* Pulse2::getInstance()
{
    if(__instance == NULL)
    {
        __instance = new Pulse2;
    }
    return __instance;
}

void Pulse2::destroy()
{
    if(__instance != NULL)
    {
        delete __instance;
    }
}

Pulse2::Pulse2()
{
    Sq2HardReset();
}

Pulse2::~Pulse2()
{
    
}

void Pulse2::Sq2Shutdown()
{
    
}

void Pulse2::Sq2HardReset()
{
    sq2_envelope = 0;
    sq2_env_startflag = false;
    sq2_env_counter = 0;
    sq2_env_devider = 0;
    sq2_length_counter_halt_flag = false;
    sq2_constant_volume_flag = false;
    sq2_volume_decay_time = 0;
    sq2_duration_haltRequset = false;
    sq2_duration_counter = 0;
    sq2_duration_reloadEnabled = false;
    sq2_duration_reload = 0;
    sq2_duration_reloadRequst = false;
    sq2_dutyForm = 0;
    sq2_dutyStep = 0;
    sq2_sweepDeviderPeriod = 0;
    sq2_sweepShiftCount = 0;
    sq2_sweepCounter = 0;
    sq2_sweepEnable = false;
    sq2_sweepReload = false;
    sq2_sweepNegateFlag = false;
    sq2_cycles = 0;
    sq2_frequency = 0;
    sq2_sweep = 0;
    sq2_pl_output = 0;
}

void Pulse2::Sq2SoftReset()
{
    sq2_duration_counter = 0;
    sq2_duration_reloadEnabled = false;
}

bool Pulse2::Sq2IsValidFrequency()
{
    return (sq2_frequency >= 0x8) && ((sq2_sweepNegateFlag) || (((sq2_frequency + (sq2_frequency >> sq2_sweepShiftCount)) & 0x800) == 0));
}

void Pulse2::Sq2ClockEnvelope()
{
    if (sq2_env_startflag)
    {
        sq2_env_startflag = false;
        sq2_env_counter = 0xF;
        sq2_env_devider = sq2_volume_decay_time + 1;
    }
    else
    {
        if (sq2_env_devider > 0)
            sq2_env_devider--;
        else
        {
            sq2_env_devider = sq2_volume_decay_time + 1;
            if (sq2_env_counter > 0)
                sq2_env_counter--;
            else if (sq2_length_counter_halt_flag)
                sq2_env_counter = 0xF;
        }
    }
    sq2_envelope = sq2_constant_volume_flag ? sq2_volume_decay_time : sq2_env_counter;
}

void Pulse2::Sq2ClockLengthCounter()
{
    if (!sq2_length_counter_halt_flag)
    {
        if (sq2_duration_counter > 0)
        {
            sq2_duration_counter--;
        }
    }
    
    sq2_sweepCounter--;
    if (sq2_sweepCounter == 0)
    {
        sq2_sweepCounter = sq2_sweepDeviderPeriod + 1;
        if (sq2_sweepEnable && (sq2_sweepShiftCount > 0) && Sq2IsValidFrequency())
        {
            sq2_sweep = sq2_frequency >> sq2_sweepShiftCount;
            sq2_frequency += sq2_sweepNegateFlag ? -sq2_sweep : sq2_sweep;
        }
    }
    if (sq2_sweepReload)
    {
        sq2_sweepReload = false;
        sq2_sweepCounter = sq2_sweepDeviderPeriod + 1;
    }
}

extern char PulseDutyForms[][8];
void Pulse2::Sq2ClockSingle()
{
    sq2_length_counter_halt_flag = sq2_duration_haltRequset;
    if (APU::__instance->isClockingDuration && sq2_duration_counter > 0)
        sq2_duration_reloadRequst = false;
    if (sq2_duration_reloadRequst)
    {
        if (sq2_duration_reloadEnabled)
            sq2_duration_counter = sq2_duration_reload;
        sq2_duration_reloadRequst = false;
    }
    
    if (--sq2_cycles <= 0)
    {
        // "Since the period of the timer is t+1 APU cycles and the sequencer has 8 steps,
        // the period of the waveform is 8*(t+1) APU cycles"
        // Its t+1 APU clock, so we add 1 first then shift left by one ((t+1)* 2)
        sq2_cycles = (sq2_frequency + 1) << 1;
        sq2_dutyStep = (sq2_dutyStep + 1) & 0x7;
        if (sq2_duration_counter > 0 && Sq2IsValidFrequency())
        {
            //if (audio_playback_sq2_enabled)
            sq2_pl_output = PulseDutyForms[sq2_dutyForm][sq2_dutyStep] * sq2_envelope;
            
        }
        else
            sq2_pl_output = 0;
        APU::__instance->audio_playback_sample_needed = true;
    }
}
