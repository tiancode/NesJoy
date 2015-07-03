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


#ifndef __NesEmulator__DMC__
#define __NesEmulator__DMC__

class DMC
{
public:
    void DMCShutdown();
    void DMCHardReset();
    void DMCSoftReset();
    void DMCClockSingle();
public:
    bool DeltaIrqOccur;
    bool DMCIrqEnabled;
    bool dmc_dmaLooping;
    bool dmc_dmaEnabled;
    bool dmc_bufferFull;
    int dmc_dmaAddrRefresh;
    int dmc_dmaSizeRefresh;
    int dmc_dmaSize;
    int dmc_dmaBits;
    char dmc_dmaByte;
    int dmc_dmaAddr;
    char dmc_dmaBuffer;
    char dmc_output;
    int dmc_cycles;
    int dmc_freqTimer;
    int dmc_pl_output;

public:
    ~DMC();
    static DMC* getInstance();
    static void destroy();

    static DMC* __instance;
private:
    DMC();
};

#endif /* defined(__NesEmulator__DMC__) */
