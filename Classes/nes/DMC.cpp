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


#include "DMC.h"
#include "APU.h"
#include "DMA.h"
#include <stddef.h>

DMC* DMC::__instance = NULL;

DMC* DMC::getInstance()
{
    if(__instance == NULL)
    {
        __instance = new DMC;
    }
    return __instance;
}

void DMC::destroy()
{
    if(__instance != NULL)
    {
        delete __instance;
    }
}

DMC::DMC()
{

}

DMC::~DMC()
{
    
}

void DMC::DMCShutdown()
{
    
}

extern int DMCFrequencyTable[][16];
void DMC::DMCHardReset()
{
    DeltaIrqOccur = false;
    DMCIrqEnabled = false;
    dmc_dmaLooping = false;
    dmc_dmaEnabled = false;
    dmc_bufferFull = false;
    dmc_output = 0;
    dmc_dmaAddr = dmc_dmaAddrRefresh = 0xC000;
    dmc_dmaSizeRefresh = 0;
    dmc_dmaSize = 0;
    dmc_dmaBits = 1;
    dmc_dmaByte = 1;
    dmc_dmaAddr = 0;
    dmc_dmaBuffer = 0;
    dmc_freqTimer = 0;
    dmc_cycles = DMCFrequencyTable[APU::getInstance()->systemIndex][dmc_freqTimer];
}

void DMC::DMCSoftReset()
{
    DeltaIrqOccur = false;
}

void DMC::DMCClockSingle()
{
    if (--dmc_cycles <= 0)
    {
        dmc_cycles = DMCFrequencyTable[APU::getInstance()->systemIndex][dmc_freqTimer];
        
        if (dmc_dmaEnabled)
        {
            if ((dmc_dmaByte & 0x01) != 0)
            {
                if (dmc_output <= 0x7D)
                    dmc_output += 2;
            }
            else
            {
                if (dmc_output >= 0x02)
                    dmc_output -= 2;
            }
            dmc_dmaByte >>= 1;
        }
        dmc_dmaBits--;
        if (dmc_dmaBits == 0)
        {
            dmc_dmaBits = 8;
            if (dmc_bufferFull)
            {
                dmc_bufferFull = false;
                dmc_dmaEnabled = true;
                dmc_dmaByte = dmc_dmaBuffer;
                // RDY ?
                if (dmc_dmaSize > 0)
                {
                    DMA::__instance->AssertDMCDMA();
                }
            }
            else
            {
                dmc_dmaEnabled = false;
            }
        }
        
        dmc_pl_output = dmc_output;
        APU::__instance->audio_playback_sample_needed = true;
    }
}
