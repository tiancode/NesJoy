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

#ifndef __NesEmulator__DMA__
#define __NesEmulator__DMA__

class DMA
{
public:
    void hardReset();
    void DMASoftReset();
    void AssertDMCDMA();
    void AssertOAMDMA();
    void DMAClock();
public:
    int dmaDMCDMAWaitCycles;
    int dmaOAMDMAWaitCycles;
    bool isOamDma;
    int oamdma_i;
    bool dmaDMCOn;
    bool dmaOAMOn;
    bool dmaDMC_occurring;
    bool dmaOAM_occurring;
    int dmaOAMFinishCounter;
    int dmaOamaddress;
    int OAMCYCLE;
    char latch;
public:
    ~DMA();
    static DMA* getInstance();
    static void destroy();

    static DMA* __instance;
private:
    DMA();
};

#endif /* defined(__NesEmulator__DMA__) */
