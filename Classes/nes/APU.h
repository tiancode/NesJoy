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

#ifndef __NesEmulator__APU__
#define __NesEmulator__APU__

#include "BlipBuffer.h"

class IAudioProvider
{
public:
    virtual void Update(short samples[], int len) {  };
};

class APU
{
public:
    ~APU();
    static APU *getInstance();
    static void destroy();
    
    void SetupSoundPlayback(IAudioProvider *AudioOutput, int frequency, int samplesCount);
    void InitializeDACTables();
    void hardReset();
    void clock();
    
private:
    void CalculateAudioPlaybackValues();
    void APUClockEnvelope();
    void APUClockDuration();
    void APUCheckIrq();
    void APUUpdatePlayback();
public:
    bool isClockingDuration;
    bool audio_playback_sample_needed;
public:
    double _dac_table[16][16][16][16][128];
    
    int Cycles;
public:
    int systemIndex;
    bool SequencingMode;
    char CurrentSeq;
    bool FrameIrqEnabled;
    bool FrameIrqFlag;
    bool oddCycle;
    /*Playback*/
    //IAudioProvider *AudioOut;
    // Output playback buffer
    int audio_playback_buffer_frequency;
    // DAC
    const double audio_playback_amplitude = 256;
    
    // Output values
    double x, x_1;
    //BlipBuffer *audio_playback_blipbuffer;
    unsigned int audio_playback_blipbuffer_timer;
    int audio_playback_blipbuffer_size;
    
    /*Channels configuration*/
    bool audio_playback_sq1_enabled;
    bool audio_playback_sq2_enabled;
    bool audio_playback_dmc_enabled;
    bool audio_playback_trl_enabled;
    bool audio_playback_noz_enabled;
    

public:
    static APU* __instance;
private:
    APU();
};


#endif /* defined(__NesEmulator__APU__) */
