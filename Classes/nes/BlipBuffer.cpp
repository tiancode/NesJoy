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


#include "BlipBuffer.h"
#include <stdio.h>

BlipBuffer::BlipBuffer(int sample_count)
{
    context = blip_new(sample_count);
    if (context == NULL)
    {
        printf("%s\n", "blip_new returned NULL!");
    }
}

BlipBuffer::~BlipBuffer()
{
    blip_delete(context);
}

void BlipBuffer::SetRates(double clock_rate, double sample_rate)
{
    blip_set_rates(context, clock_rate, sample_rate);
}

void BlipBuffer::Clear()
{
    blip_clear(context);
}

void BlipBuffer::AddDelta(unsigned int clock_time, int delta)
{
    blip_add_delta(context, clock_time, delta);
}

void BlipBuffer::AddDeltaFast(unsigned int clock_time, int delta)
{
    blip_add_delta_fast(context, clock_time, delta);
}

int BlipBuffer::ClocksNeeded(int sample_count)
{
    return blip_clocks_needed(context, sample_count);
}

void BlipBuffer::EndFrame(unsigned int clock_duration)
{
    blip_end_frame(context, clock_duration);
}

int BlipBuffer::SamplesAvailable()
{
    return blip_samples_avail(context);
}

int BlipBuffer::ReadSamples(short output[], int len, int count, bool stereo)
{
    if (len < count * (stereo ? 2 : 1))
    {
        printf("out of range\n");
    }
    return blip_read_samples(context, output, count, stereo ? 1 : 0);
}
