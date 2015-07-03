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

#include "Pulse1.h"
#include "APU.h"

#include <stdlib.h>

Pulse1* Pulse1::__instance = NULL;

Pulse1* Pulse1::getInstance()
{
    if(__instance == NULL)
    {
        __instance = new Pulse1;
    }
    return __instance;
}

void Pulse1::destroy()
{
    if(__instance != NULL)
    {
        delete __instance;
    }
}

Pulse1::Pulse1()
{
    Sq1HardReset();
}

Pulse1::~Pulse1()
{
    
}

void Pulse1::Sq1Shutdown()
{
    
}

void Pulse1::Sq1HardReset()
{
    sq1_envelope = 0;
    sq1_env_startflag = false;
    sq1_env_counter = 0;
    sq1_env_devider = 0;
    sq1_length_counter_halt_flag = false;
    sq1_constant_volume_flag = false;
    sq1_volume_decay_time = 0;
    sq1_duration_haltRequset = false;
    sq1_duration_counter = 0;
    sq1_duration_reloadEnabled = false;
    sq1_duration_reload = 0;
    sq1_duration_reloadRequst = false;
    sq1_dutyForm = 0;
    sq1_dutyStep = 0;
    sq1_sweepDeviderPeriod = 0;
    sq1_sweepShiftCount = 0;
    sq1_sweepCounter = 0;
    sq1_sweepEnable = false;
    sq1_sweepReload = false;
    sq1_sweepNegateFlag = false;
    sq1_frequency = 0;
    sq1_sweep = 0;
    sq1_cycles = 0;
    sq1_pl_output = 0;
}

void Pulse1::Sq1SoftReset()
{
    sq1_duration_counter = 0;
    sq1_duration_reloadEnabled = false;
}

bool Pulse1::Sq1IsValidFrequency()
{
    return (sq1_frequency >= 0x8) && ((sq1_sweepNegateFlag) || (((sq1_frequency + (sq1_frequency >> sq1_sweepShiftCount)) & 0x800) == 0));
}

void Pulse1::Sq1ClockEnvelope()
{
    if (sq1_env_startflag)
    {
        sq1_env_startflag = false;
        sq1_env_counter = 0xF;
        sq1_env_devider = sq1_volume_decay_time + 1;
    }
    else
    {
        if (sq1_env_devider > 0)
            sq1_env_devider--;
        else
        {
            sq1_env_devider = sq1_volume_decay_time + 1;
            if (sq1_env_counter > 0)
                sq1_env_counter--;
            else if (sq1_length_counter_halt_flag)
                sq1_env_counter = 0xF;
        }
    }
    sq1_envelope = sq1_constant_volume_flag ? sq1_volume_decay_time : sq1_env_counter;
}

void Pulse1::Sq1ClockLengthCounter()
{
    if (!sq1_length_counter_halt_flag)
    {
        if (sq1_duration_counter > 0)
        {
            sq1_duration_counter--;
        }
    }
    
    sq1_sweepCounter--;
    if (sq1_sweepCounter == 0)
    {
        sq1_sweepCounter = sq1_sweepDeviderPeriod + 1;
        if (sq1_sweepEnable && (sq1_sweepShiftCount > 0) && Sq1IsValidFrequency())
        {
            sq1_sweep = sq1_frequency >> sq1_sweepShiftCount;
            sq1_frequency += sq1_sweepNegateFlag ? ~sq1_sweep : sq1_sweep;
        }
    }
    if (sq1_sweepReload)
    {
        sq1_sweepReload = false;
        sq1_sweepCounter = sq1_sweepDeviderPeriod + 1;
    }
}

extern char PulseDutyForms[][8];
void Pulse1::Sq1ClockSingle()
{
    sq1_length_counter_halt_flag = sq1_duration_haltRequset;
   
    if ( APU::__instance->isClockingDuration && sq1_duration_counter > 0)
        sq1_duration_reloadRequst = false;
    if (sq1_duration_reloadRequst)
    {
        if (sq1_duration_reloadEnabled)
            sq1_duration_counter = sq1_duration_reload;
        sq1_duration_reloadRequst = false;
    }
    
    if (--sq1_cycles <= 0)
    {
        // "Since the period of the timer is t+1 APU cycles and the sequencer has 8 steps,
        // the period of the waveform is 8*(t+1) APU cycles"
        // Its t+1 APU clock, so we add 1 first then shift left by one ((t+1)* 2)
        sq1_cycles = (sq1_frequency + 1) << 1;
        sq1_dutyStep = (sq1_dutyStep + 1) & 0x7;
        if (sq1_duration_counter > 0 && Sq1IsValidFrequency())
        {
            // if (audio_playback_sq1_enabled)
            sq1_pl_output = PulseDutyForms[sq1_dutyForm][sq1_dutyStep] * sq1_envelope;
        }
        else
            sq1_pl_output = 0;
        APU::__instance->audio_playback_sample_needed = true;
    }
}
