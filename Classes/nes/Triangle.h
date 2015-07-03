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

#ifndef __NesEmulator__Triangle__
#define __NesEmulator__Triangle__

class Triangle
{
public:
    bool trl_length_counter_halt_flag;
    bool trl_duration_haltRequset;
    char trl_duration_counter;
    bool trl_duration_reloadEnabled;
    char trl_duration_reload;
    bool trl_duration_reloadRequst;
    char trl_linearCounter;
    char trl_linearCounterReload;
    char trl_step;
    bool trl_linearCounterHalt;
    bool trl_halt;
    int trl_frequency;
    int trl_cycles;
    int trl_pl_output;
public:
    void TrlHardReset();
    void TrlSoftReset();
    void TrlClockEnvelope();
    void TrlClockLengthCounter();
    void TrlClockSingle();
    
public:
    ~Triangle();
    static Triangle* getInstance();
    static void destroy();

    static Triangle* __instance;
private:
    Triangle();
};

#endif /* defined(__NesEmulator__Triangle__) */
