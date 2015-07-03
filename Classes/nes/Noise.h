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


#ifndef __NesEmulator__Noise__
#define __NesEmulator__Noise__

class Noise
{
public:
    void NOZShutdown();
    void NozHardReset();
    void NozSoftReset();
    void NozClockEnvelope();
    void NozClockLengthCounter();
    void NozClockSingle();
public:
    int noz_envelope;
    bool noz_env_startflag;
    int noz_env_counter;
    int noz_env_devider;
    bool noz_length_counter_halt_flag;
    bool noz_constant_volume_flag;
    int noz_volume_decay_time;
    bool noz_duration_haltRequset;
    char noz_duration_counter;
    bool noz_duration_reloadEnabled;
    char noz_duration_reload;
    bool noz_duration_reloadRequst;
    bool noz_mode;
    int noz_shiftRegister;
    int noz_feedback;
    int noz_frequency;
    int noz_cycles;
    int noz_pl_output;

public:
    ~Noise();
    static Noise* getInstance();
    static void destroy();

    static Noise* __instance;
private:
    Noise();
};

#endif /* defined(__NesEmulator__Noise__) */
