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

#ifndef __NesEmulator__NesEmulator__
#define __NesEmulator__NesEmulator__

#include <thread>
#include "Types.h"
#include "NesCartDatabase.h"
#include "Input.h"
#include "APU.h"

class NesEmulator
{
public:
    /* Call this at application start up to set nes default stuff */
    void warmUp(const char *data);
    
    bool CreateNew(unsigned char* fileContent, int len,TVSystem tvsetting = AUTO);
    
    //void setPaused(bool paused);
    
    void setScreenPointer(int* scan0, int scanStart, int scanSize);
    void setInput(IJoypadConnecter *joy1, IJoypadConnecter *joy2, IJoypadConnecter *joy3, IJoypadConnecter *joy4);
    void SetupSoundPlayback(IAudioProvider *AudioOutput, int frequency, int samplesCount);
    
    void setStop(bool stop);
    
    int FPSDone;
private:
    void setupSpeedLimiter();
    void hardReset();
    
    std::thread *EmulationThread;
    void clock();
public:
    void syncClock();
    void OnFinishFrame();
    
    TVSystem TVFormat;
private:
    bool DoPalAdditionalClock;
    unsigned char palCyc;

    double gapTime;
    double FramePeriod;;
    double FPS;
public:
    
    double getTime();
private:
    bool EmulationON;
    
public:
    NesCartDatabase *nesCartDatabase;
    
public:
    static NesEmulator* getInstance();
    static void destroy();
    virtual ~NesEmulator();
    
    static NesEmulator* __instance;
private:
    NesEmulator();
};

#endif /* defined(__NesEmulator__NesEmulator__) */
