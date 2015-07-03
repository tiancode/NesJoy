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

#ifndef __NesEmulator__Pulse2__
#define __NesEmulator__Pulse2__

/*Pulse 2 sound channel*/
class Pulse2
{
public:
    /*Envelope and length counter*/
    int sq2_envelope;
    bool sq2_env_startflag;
    int sq2_env_counter;
    int sq2_env_devider;
    bool sq2_length_counter_halt_flag;
    bool sq2_constant_volume_flag;
    int sq2_volume_decay_time;
    bool sq2_duration_haltRequset;
    char sq2_duration_counter;
    bool sq2_duration_reloadEnabled;
    char sq2_duration_reload;
    bool sq2_duration_reloadRequst;
    
    int sq2_dutyForm;
    int sq2_dutyStep;
    int sq2_sweepDeviderPeriod;
    int sq2_sweepShiftCount;
    int sq2_sweepCounter;
    bool sq2_sweepEnable;
    bool sq2_sweepReload;
    bool sq2_sweepNegateFlag;
    int sq2_frequency;
    int sq2_sweep;
    int sq2_cycles;
    int sq2_pl_output;
    
public:
    void Sq2Shutdown();
    void Sq2HardReset();
    void Sq2SoftReset();
    bool Sq2IsValidFrequency();
    void Sq2ClockEnvelope();
    void Sq2ClockLengthCounter();
    void Sq2ClockSingle();
    
public:
    ~Pulse2();
    static Pulse2* getInstance();
    static void destroy();

    static Pulse2* __instance;
private:
    Pulse2();
};

#endif /* defined(__NesEmulator__Pulse2__) */
