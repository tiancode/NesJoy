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

#ifndef __NesEmulator__Pulse1__
#define __NesEmulator__Pulse1__

/*Pulse 1 sound channel*/
class Pulse1
{
public:
    void Sq1Shutdown();
    void Sq1HardReset();
    void Sq1SoftReset();
    bool Sq1IsValidFrequency();
    void Sq1ClockEnvelope();
    void Sq1ClockLengthCounter();
    void Sq1ClockSingle();
public:
    int sq1_envelope;
    bool sq1_env_startflag;
    int sq1_env_counter;
    int sq1_env_devider;
    bool sq1_length_counter_halt_flag;
    bool sq1_constant_volume_flag;
    int sq1_volume_decay_time;
    bool sq1_duration_haltRequset;
    char sq1_duration_counter;
    bool sq1_duration_reloadEnabled;
    char sq1_duration_reload;
    bool sq1_duration_reloadRequst;
    int sq1_dutyForm;
    int sq1_dutyStep;
    int sq1_sweepDeviderPeriod;
    int sq1_sweepShiftCount;
    int sq1_sweepCounter;
    bool sq1_sweepEnable;
    bool sq1_sweepReload;
    bool sq1_sweepNegateFlag;
    int sq1_frequency;
    int sq1_sweep;
    int sq1_cycles;
    int sq1_pl_output;
    
public:
    ~Pulse1();
    static Pulse1* getInstance();
    static void destroy();

    static Pulse1* __instance;
private:
    Pulse1();
};

#endif /* defined(__NesEmulator__Pulse1__) */
