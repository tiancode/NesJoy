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

#include "APU.h"

#include "DMC.h"
#include "Pulse1.h"
#include "Pulse2.h"
#include "Noise.h"
#include "Triangle.h"

#include "Memory.h"
#include "Interrupt.h"
#include "CPU.h"

APU* APU::__instance = NULL;

APU* APU::getInstance()
{
    if(__instance == NULL)
    {
        __instance = new APU;
    }
    return __instance;
}

void APU::destroy()
{
    if(__instance != NULL)
    {
        delete __instance;
        __instance = NULL;
    }
}

int SequenceMode0[][7] =
{
    { 7459, 7456, 7458, 7457, 1, 1, 7457 }, // NTSC
    { 8315, 8314, 8312, 8313, 1, 1, 8313 }, // PALB
    { 7459, 7456, 7458, 7457, 1, 1, 7457 }, // DENDY (acts like NTSC)
};
int SequenceMode1[][5] =
{
    { 1, 7458, 7456, 7458, 14910 } , // NTSC
    { 1, 8314, 8314, 8312, 16626 } , // PALB
    { 1, 7458, 7456, 7458, 14910 } , // DENDY (acts like NTSC)
};
unsigned char DurationTable[] =
{
    0x0A, 0xFE, 0x14, 0x02, 0x28, 0x04, 0x50, 0x06, 0xA0, 0x08, 0x3C, 0x0A, 0x0E, 0x0C, 0x1A, 0x0E,
    0x0C, 0x10, 0x18, 0x12, 0x30, 0x14, 0x60, 0x16, 0xC0, 0x18, 0x48, 0x1A, 0x10, 0x1C, 0x20, 0x1E,
};
char PulseDutyForms[][8] =
{
    {  0, 1, 0, 0, 0, 0, 0, 0 }, // 12.5%
    {  0, 1, 1, 0, 0, 0, 0, 0 }, // 25.0%
    {  0, 1, 1, 1, 1, 0, 0, 0 }, // 50.0%
    {  1, 0, 0, 1, 1, 1, 1, 1 }, // 75.0% (25.0% negated)
};
char TrlStepSequence[] =
{
    15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0,
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15
};

int NozFrequencyTable[][16] =
{
    //NTSC
    {
        4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068
    },
    //PAL
    {
        4, 7, 14, 30, 60, 88, 118, 148, 188, 236, 354, 472, 708,  944, 1890, 3778
    },
    //DENDY (same as ntsc for now)
    {
        4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068
    }
};
int DMCFrequencyTable[][16] =
{
    //NTSC
    {
        428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106,  84,  72,  54
    },
    //PAL
    {
        398, 354, 316, 298, 276, 236, 210, 198, 176, 148, 132, 118,  98,  78,  66,  50
    },
    //DENDY (same as ntsc for now)
    {
        428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106,  84,  72,  54
    },
};

APU::APU()
{
    Cycles = 0;
    CurrentSeq = 0;
    isClockingDuration = false;
    
    //AudioOut = new IAudioProvider;
    
    audio_playback_sq1_enabled = true;
    audio_playback_sq2_enabled = true;
    audio_playback_dmc_enabled = true;
    audio_playback_trl_enabled = true;
    audio_playback_noz_enabled = true;
}

APU::~APU()
{
    //delete AudioOut;
}

void APU::SetupSoundPlayback(IAudioProvider *AudioOutput, int frequency, int samplesCount)
{
    audio_playback_buffer_frequency = frequency;
    //AudioOut = AudioOutput;
    audio_playback_blipbuffer_size = samplesCount;
    x = x_1 = 0;
}

void APU::InitializeDACTables()
{
    for (int sq1 = 0; sq1 < 16; sq1++)
    {
        for (int sq2 = 0; sq2 < 16; sq2++)
        {
            for (int trl = 0; trl < 16; trl++)
            {
                for (int noz = 0; noz < 16; noz++)
                {
                    for (int dmc = 0; dmc < 128; dmc++)
                    {
                        double rgroup1_dac_output = (8128.0 / ((double)sq1 + (double)sq2));
                        rgroup1_dac_output = 95.88 / (rgroup1_dac_output + 100.0);
                        
                        double rgroup2_dac_output = 1.0 / (((double)dmc / 22638.0) + ((double)trl / 8827.0) + ((double)noz / 12241.0));
                        rgroup2_dac_output = 159.79 / (rgroup2_dac_output + 100.0);
                        
                        _dac_table[sq1][sq2][trl][noz][dmc] = rgroup1_dac_output + rgroup2_dac_output;
                    }
                }
            }
        }
    }
}

void APU::CalculateAudioPlaybackValues()
{
    // Playback buffer
    //audio_playback_blipbuffer = new BlipBuffer(audio_playback_blipbuffer_size);  //delete
    //audio_playback_blipbuffer->SetRates( CPU::__instance->cpuSpeedInHz, audio_playback_buffer_frequency);
    
    audio_playback_blipbuffer_timer = 0;
    audio_playback_sample_needed = false;
    x = x_1 = 0;
}

void APU::hardReset()
{
    CalculateAudioPlaybackValues();
    
    Cycles = SequenceMode0[systemIndex][0] - 10;
    FrameIrqFlag = false;
    FrameIrqEnabled = true;
    SequencingMode = false;
    CurrentSeq = 0;
    oddCycle = false;
    isClockingDuration = false;
    
    Pulse1::__instance->Sq1HardReset();
    Pulse2::__instance->Sq2HardReset();
    Triangle::__instance->TrlHardReset();
    Noise::__instance->NozHardReset();
    DMC::__instance->DMCHardReset();
}

void APU::APUClockEnvelope()
{
    Pulse1::__instance->Sq1ClockEnvelope();
    Pulse2::__instance->Sq2ClockEnvelope();
    Triangle::__instance->TrlClockEnvelope();
    Noise::__instance->NozClockEnvelope();
    if (Memory::__instance->board->enable_external_sound)
        Memory::__instance->board->OnAPUClockEnvelope();
}

void APU::APUClockDuration()
{
    APUClockEnvelope();
    
    Pulse1::__instance->Sq1ClockLengthCounter();
    Pulse2::__instance->Sq2ClockLengthCounter();
    Triangle::__instance->TrlClockLengthCounter();
    Noise::__instance->NozClockLengthCounter();
    if (Memory::__instance->board->enable_external_sound)
        Memory::__instance->board->OnAPUClockDuration();
}

void APU::APUCheckIrq()
{
    if (FrameIrqEnabled)
        FrameIrqFlag = true;
    if (FrameIrqFlag)
        Interrupt::__instance->IRQFlags |= Interrupt::__instance->IRQ_APU;
}

void APU::clock()
{
    isClockingDuration = false;
    Cycles--;
    oddCycle = !oddCycle;
    
    if (Cycles == 0)
    {
        if (!SequencingMode)
        {
            switch (CurrentSeq)
            {
                case 0: APUClockEnvelope(); break;
                case 1: APUClockDuration(); isClockingDuration = true; break;
                case 2: APUClockEnvelope(); break;
                case 3: APUCheckIrq(); break;
                case 4: APUCheckIrq(); APUClockDuration(); isClockingDuration = true; break;
                case 5: APUCheckIrq(); break;
            }
            CurrentSeq++;
            Cycles += SequenceMode0[systemIndex][CurrentSeq];
            if (CurrentSeq == 6)
                CurrentSeq = 0;
        }
        else
        {
            switch (CurrentSeq)
            {
                case 0:
                case 2: APUClockDuration(); isClockingDuration = true; break;
                case 1:
                case 3: APUClockEnvelope(); break;
            }
            CurrentSeq++;
            Cycles = SequenceMode1[systemIndex][CurrentSeq];
            if (CurrentSeq == 4)
                CurrentSeq = 0;
        }
    }
    // Clock single
    Pulse1::__instance->Sq1ClockSingle();
    Pulse2::__instance->Sq2ClockSingle();
    Triangle::__instance->TrlClockSingle();
    Noise::__instance->NozClockSingle();
    DMC::__instance->DMCClockSingle();
    if (Memory::__instance->board->enable_external_sound)
        Memory::__instance->board->OnAPUClockSingle(isClockingDuration);
    // Playback
    APUUpdatePlayback();
}

void APU::APUUpdatePlayback()
{
    audio_playback_blipbuffer_timer++;
    if (audio_playback_sample_needed)
    {
        audio_playback_sample_needed = false;
        if (!audio_playback_sq1_enabled)
            Pulse1::__instance->sq1_pl_output = 0;
        if (!audio_playback_sq2_enabled)
            Pulse2::__instance->sq2_pl_output = 0;
        if (!audio_playback_dmc_enabled)
            DMC::__instance->dmc_pl_output = 0;
        if (!audio_playback_trl_enabled)
            Triangle::__instance->trl_pl_output = 0;
        if (!audio_playback_noz_enabled)
            Noise::__instance->noz_pl_output = 0;
        // Collect the sample
        //x = (_dac_table[Pulse1::__instance->sq1_pl_output][Pulse2::__instance->sq2_pl_output][Triangle::__instance->trl_pl_output][Noise::__instance->noz_pl_output][DMC::__instance->dmc_pl_output] +
             //(Memory::__instance->board->enable_external_sound ? Memory::__instance->board->APUGetSamples() : 0)) * audio_playback_amplitude;
        
        // Add delta to the blip-buffer
        if (x != x_1)
        {
            //audio_playback_blipbuffer->AddDelta(audio_playback_blipbuffer_timer, (int)(x - x_1));
            x_1 = x;
        }
    }
}
