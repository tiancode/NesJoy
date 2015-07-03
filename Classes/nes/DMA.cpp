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

#include "DMA.h"

#include "Memory.h"
#include "APU.h"
#include "NesEmulator.h"
#include "DMC.h"
#include "Interrupt.h"

DMA* DMA::__instance = NULL;

DMA* DMA::getInstance()
{
    if(__instance == NULL)
    {
        __instance = new DMA;
    }
    return __instance;
}

void DMA::destroy()
{
    if(__instance != NULL)
    {
        delete __instance;
    }
}

DMA::DMA()
{
    hardReset();
}

DMA::~DMA()
{
    
}

void DMA::hardReset()
{
    dmaDMCDMAWaitCycles = 0;
    dmaOAMDMAWaitCycles = 0;
    isOamDma = false;
    oamdma_i = 0;
    dmaDMCOn = false;
    dmaOAMOn = false;
    dmaDMC_occurring = false;
    dmaOAM_occurring = false;
    dmaOAMFinishCounter = 0;
    dmaOamaddress = 0;
    OAMCYCLE = 0;
    latch = 0;
}

void DMA::DMASoftReset()
{
    dmaDMCDMAWaitCycles = 0;
    dmaOAMDMAWaitCycles = 0;
    isOamDma = false;
    oamdma_i = 0;
    dmaDMCOn = false;
    dmaOAMOn = false;
    dmaDMC_occurring = false;
    dmaOAM_occurring = false;
    dmaOAMFinishCounter = 0;
    dmaOamaddress = 0;
    OAMCYCLE = 0;
    latch = 0;
}

void DMA::AssertDMCDMA()
{
    if (dmaOAM_occurring)
    {
        if (OAMCYCLE < 508)
            // OAM DMA is occurring here
            dmaDMCDMAWaitCycles = Memory::__instance->BUS_RW ? 1 : 0;
        else
        {
            // Here the oam dma is about to finish
            // Remaining cycles of oam dma determines the dmc dma waiting cycles.
            dmaDMCDMAWaitCycles = 4 - (512 - OAMCYCLE);
        }
    }
    else if (dmaDMC_occurring)
    {
        // DMC occurring now !!? is that possible ?
        // Anyway, let's ignore this call !
        return;
    }
    else
    {
        // Nothing occurring, initialize brand new dma
        // DMC DMA depends on r/w flag for the wait cycles.
        dmaDMCDMAWaitCycles = Memory::getInstance()->BUS_RW ? 3 : 2;
        // After 2 cycles of oam dma, add extra cycle for the waiting.
        if (dmaOAMFinishCounter == 3)
            dmaDMCDMAWaitCycles++;
    }
    isOamDma = false;
    dmaDMCOn = true;
}

void DMA::AssertOAMDMA()
{
    if (dmaOAM_occurring) return;
    // Setup
    // OAM DMA depends on apu odd timer for odd cycles
    dmaOAMDMAWaitCycles = APU::__instance->oddCycle ? 1 : 2;
    isOamDma = true;
    dmaOAMOn = true;
}

void DMA::DMAClock()
{
    if (dmaOAMFinishCounter > 0)
        dmaOAMFinishCounter--;
    if (!Memory::getInstance()->BUS_RW)// Clocks only on reads
    {
        if (dmaDMCDMAWaitCycles > 0)
            dmaDMCDMAWaitCycles--;
        if (dmaOAMDMAWaitCycles > 0)
            dmaOAMDMAWaitCycles--;
        return;
    }
    if (dmaDMCOn)
    {
        dmaDMC_occurring = true;
        // This is it !
        dmaDMCOn = false;
        // Do wait cycles (extra reads)
        if (dmaDMCDMAWaitCycles > 0)
        {
            if (Memory::getInstance()->BUS_ADDRESS == 0x4016 || Memory::getInstance()->BUS_ADDRESS == 0x4017)
            {
                Memory::getInstance()->read(Memory::getInstance()->BUS_ADDRESS);
                dmaDMCDMAWaitCycles--;
                
                while (dmaDMCDMAWaitCycles > 0)
                {
                    NesEmulator::__instance->syncClock();
                    dmaDMCDMAWaitCycles--;
                }
            }
            else
            {
                while (dmaDMCDMAWaitCycles > 0)
                {
                    Memory::getInstance()->read(Memory::getInstance()->BUS_ADDRESS);
                    dmaDMCDMAWaitCycles--;
                }
            }
        }
        // Do DMC DMA
        DMC::__instance->dmc_bufferFull = true;
        
        DMC::__instance->dmc_dmaBuffer = Memory::__instance->read(DMC::__instance->dmc_dmaAddr);
        
        if (++DMC::getInstance()->dmc_dmaAddr == 0x10000)
            DMC::getInstance()->dmc_dmaAddr = 0x8000;
        if (DMC::getInstance()->dmc_dmaSize > 0)
            DMC::getInstance()->dmc_dmaSize--;
        
        if (DMC::getInstance()->dmc_dmaSize == 0)
        {
            if (DMC::getInstance()->dmc_dmaLooping)
            {
                DMC::getInstance()->dmc_dmaAddr = DMC::getInstance()->dmc_dmaAddrRefresh;
                DMC::getInstance()->dmc_dmaSize = DMC::getInstance()->dmc_dmaSizeRefresh;
            }
            else if (DMC::getInstance()->DMCIrqEnabled)
            {
                Interrupt::__instance->IRQFlags |= Interrupt::__instance->IRQ_DMC;
                DMC::getInstance()->DeltaIrqOccur = true;
            }
        }
        
        dmaDMC_occurring = false;
    }
    if (dmaOAMOn)
    {
        dmaOAM_occurring = true;
        // This is it ! pause the cpu
        dmaOAMOn = false;
        // Do wait cycles (extra reads)
        if (dmaOAMDMAWaitCycles > 0)
        {
            if (Memory::__instance->BUS_ADDRESS == 0x4016 || Memory::__instance->BUS_ADDRESS == 0x4017)
            {
                Memory::__instance->read(Memory::__instance->BUS_ADDRESS);
                dmaOAMDMAWaitCycles--;
                
                while (dmaOAMDMAWaitCycles > 0)
                {
                    NesEmulator::__instance->syncClock();
                    dmaOAMDMAWaitCycles--;
                }
            }
            else
            {
                while (dmaOAMDMAWaitCycles > 0)
                {
                    Memory::__instance->read(Memory::__instance->BUS_ADDRESS);
                    dmaOAMDMAWaitCycles--;
                }
            }
        }
        
        // Do OAM DMA
        OAMCYCLE = 0;
        for (oamdma_i = 0; oamdma_i < 256; oamdma_i++)
        {
            latch = Memory::__instance->read(dmaOamaddress);
            OAMCYCLE++;
            Memory::__instance->write(0x2004, latch);
            OAMCYCLE++;
            dmaOamaddress = (++dmaOamaddress) & 0xFFFF;
        }
        OAMCYCLE = 0;
        dmaOAMFinishCounter = 5;
        dmaOAM_occurring = false;
    }
}

