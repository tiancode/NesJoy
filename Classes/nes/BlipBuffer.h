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


#ifndef __NesEmulator__BlipBuffer__
#define __NesEmulator__BlipBuffer__

#include "blip_buff.h"

class BlipBuffer
{
private:
    const int MaxRatio = 1048576;
    const int MaxFrame = 4000;
    blip_t* context;
    
public:
    BlipBuffer(int sample_count);
    ~BlipBuffer();
    
    void SetRates(double clock_rate, double sample_rate);
    void Clear();
    void AddDelta(unsigned int clock_time, int delta);
    void AddDeltaFast(unsigned int clock_time, int delta);
    int ClocksNeeded(int sample_count);
    void EndFrame(unsigned int clock_duration);
    int SamplesAvailable();
    int ReadSamples(short output[],int len, int count, bool stereo);
};

#endif /* defined(__NesEmulator__BlipBuffer__) */
