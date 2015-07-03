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

#include "Triangle.h"
#include "APU.h"

#include <stddef.h>

Triangle* Triangle::__instance = NULL;

Triangle* Triangle::getInstance()
{
    if(__instance == NULL)
    {
        __instance = new Triangle;
    }
    return __instance;
}

void Triangle::destroy()
{
    if(__instance != NULL)
    {
        delete __instance;
    }
}

Triangle::Triangle()
{
    TrlHardReset();
}

Triangle::~Triangle()
{
    
}

void Triangle::TrlHardReset()
{
    trl_length_counter_halt_flag = false;
    trl_duration_haltRequset = false;
    trl_duration_counter = 0;
    trl_duration_reloadEnabled = false;
    trl_duration_reload = 0;
    trl_duration_reloadRequst = false;
    trl_linearCounter = 0;
    trl_linearCounterReload = 0;
    trl_step = 0xF;
    trl_linearCounterHalt = false;
    trl_halt = true;
    trl_frequency = 0;
    trl_cycles = 0;
    trl_pl_output = 0;
}

void Triangle::TrlSoftReset()
{
    trl_duration_counter = 0;
    trl_duration_reloadEnabled = false;
}

void Triangle::TrlClockEnvelope()
{
    if (trl_halt)
    {
        trl_linearCounter = trl_linearCounterReload;
    }
    else
    {
        if (trl_linearCounter != 0)
        {
            trl_linearCounter--;
        }
    }
    
    trl_halt &= trl_linearCounterHalt;
}

void Triangle::TrlClockLengthCounter()
{
    if (!trl_length_counter_halt_flag)
    {
        if (trl_duration_counter > 0)
        {
            trl_duration_counter--;
        }
    }
}

extern char TrlStepSequence[];
void Triangle::TrlClockSingle()
{
    trl_length_counter_halt_flag = trl_duration_haltRequset;
    if (APU::__instance->isClockingDuration && trl_duration_counter > 0)
        trl_duration_reloadRequst = false;
    if (trl_duration_reloadRequst)
    {
        if (trl_duration_reloadEnabled)
            trl_duration_counter = trl_duration_reload;
        trl_duration_reloadRequst = false;
    }
    if (--trl_cycles <= 0)
    {
        APU::__instance->audio_playback_sample_needed = true;
        trl_cycles = trl_frequency + 1;
        if (trl_duration_counter > 0 && trl_linearCounter > 0)
        {
            if (trl_frequency >= 4)
            {
                trl_step++;
                trl_step &= 0x1F;
                trl_pl_output = TrlStepSequence[trl_step];
            }
        }
    }
}
