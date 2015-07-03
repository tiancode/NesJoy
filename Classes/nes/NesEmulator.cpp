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

#include "NesEmulator.h"
#include "Interrupt.h"
#include "APU.h"
#include "Memory.h"
#include "CPU.h"
#include "DMC.h"
#include "Triangle.h"
#include "Noise.h"
#include "Pulse1.h"
#include "Pulse2.h"
#include "PPU.h"
#include "DMA.h"

#include "NTSCPaletteGenerator.h"
#include "PALBPaletteGenerator.h"

#include "Rom.h"
#include "util.h"
#include "VSUnisystem.h"

#include "Input.h"

NesEmulator* NesEmulator::__instance = NULL;

NesEmulator* NesEmulator::getInstance()
{
    if(__instance == NULL)
    {
        __instance = new NesEmulator;
    }
    return __instance;
}

void NesEmulator::destroy()
{
    if(__instance != NULL)
    {
        delete __instance;
        __instance = NULL;
    }
}

NesEmulator::NesEmulator()
{
    nesCartDatabase = new NesCartDatabase;
    
    FPS = 0;
    FramePeriod = (1.0 / 60.0988);
    
    EmulationON = false;
}
NesEmulator::~NesEmulator()
{
    delete nesCartDatabase;
}

void NesEmulator::warmUp(const char *data)
{
    nesCartDatabase->loadDatabase(data);
    
    CPU::getInstance();
    Memory::getInstance();
    Interrupt::getInstance();
    PPU::getInstance();
    
    DMA::getInstance();
    DMC::getInstance();
    APU::getInstance();
    Noise::getInstance();
    Pulse1::getInstance();
    Pulse2::getInstance();
    Triangle::getInstance();
    
    Input::getInstance();
    //APU::__instance->InitializeDACTables();
}

void NesEmulator::setupSpeedLimiter()
{
    if (TVFormat == NTSC)
        FramePeriod = (1.0 / (FPS = 60.0988));
    else if (TVFormat == PALB)
        FramePeriod = (1.0 / (FPS = 50.0));
    else if (TVFormat == DENDY)
        FramePeriod = (1.0 / (FPS = 50.0));
}

void NesEmulator::hardReset()
{
    switch (TVFormat)
    {
        case AUTO:
        {
            //Board *board = Memory::__instance->board;
            if(Memory::__instance->board->GameInfo != NULL && !Memory::__instance->board->GameInfo->Cartridges.empty() )
            {
                if(toLower(&Memory::__instance->board->GameCartInfo->System).find("pal") != std::string::npos)
                {
                    TVFormat = PALB;
                    PALBPaletteGenerator *palette = new PALBPaletteGenerator;
                    PPU::__instance->SetPalette(palette->GeneratePaletteGBR());
                    APU::__instance->systemIndex = 1;
                    CPU::__instance->cpuSpeedInHz = 1662607;
                }
                else if( toLower(&Memory::__instance->board->GameCartInfo->System).find("dendy") != std::string::npos )
                {
                    TVFormat = DENDY;
                    PALBPaletteGenerator *palette = new PALBPaletteGenerator;
                    PPU::__instance->SetPalette(palette->GeneratePaletteGBR());
                    APU::__instance->systemIndex = 2;
                    CPU::__instance->cpuSpeedInHz = 1773448;
                }
                else
                {
                    TVFormat = NTSC;
                    NTSCPaletteGenerator *palette = new NTSCPaletteGenerator;  //delete
                    PPU::__instance->SetPalette(palette->GeneratePaletteGBR());
                    APU::__instance->systemIndex = 0;
                    CPU::__instance->cpuSpeedInHz = 1789773;
                }
            }
            else
            {
                TVFormat = NTSC; // force NTSC
                NTSCPaletteGenerator *palette = new NTSCPaletteGenerator;  //delete
                PPU::__instance->SetPalette(palette->GeneratePaletteGBR());
                APU::__instance->systemIndex = 0;
                CPU::__instance->cpuSpeedInHz = 1789773;
            }
            break;
        }
        case NTSC:
        {
            TVFormat = NTSC;
            NTSCPaletteGenerator *palette = new NTSCPaletteGenerator;  //delete
            PPU::__instance->SetPalette(palette->GeneratePaletteGBR());
            APU::__instance->systemIndex = 0;
            CPU::__instance->cpuSpeedInHz = 1789773;
            break;
        }
        case PALB:
        {
            TVFormat = PALB;
            PALBPaletteGenerator *palette = new PALBPaletteGenerator;
            PPU::__instance->SetPalette(palette->GeneratePaletteGBR());
            APU::__instance->systemIndex = 1;
            CPU::__instance->cpuSpeedInHz = 1662607;
            break;
        }
        case DENDY:
        {
            TVFormat = DENDY;
            PALBPaletteGenerator *palette = new PALBPaletteGenerator;
            PPU::__instance->SetPalette(palette->GeneratePaletteGBR());
            APU::__instance->systemIndex = 2;
            CPU::__instance->cpuSpeedInHz = 1773448;
            break;
        }
    }
    
    DoPalAdditionalClock = TVFormat == PALB;
    palCyc = 0;
    setupSpeedLimiter();
    
    Memory::__instance->hardReset();
    CPU::__instance->hardReset();
    PPU::__instance->hardReset();
    DMA::__instance->hardReset();
    APU::__instance->hardReset();
}

void NesEmulator::setScreenPointer(int* scan0, int scanStart, int scanSize)
{
    PPU::__instance->SetupVideoRenderer(scan0, scanStart, scanSize);
}

void NesEmulator::setInput(IJoypadConnecter *joy1, IJoypadConnecter *joy2, IJoypadConnecter *joy3, IJoypadConnecter *joy4)
{
    Input::__instance->SetupJoypads(joy1, joy2, joy3, joy4);
}
void NesEmulator::SetupSoundPlayback(IAudioProvider *AudioOutput, int frequency, int samplesCount)
{
    APU::__instance->SetupSoundPlayback(AudioOutput, frequency, samplesCount);
}

bool NesEmulator::CreateNew(unsigned char* fileContent, int len, TVSystem tvsetting)
{
    Rom rom;
    if(rom.parseNes(fileContent, len, true) == false)
    {
        return false;
    }
    
    TVFormat = tvsetting;
    Memory::__instance->init(&rom);
    
    VSUnisystem::CheckGameVSUnisystem(rom.getSHA1(), rom.isVSUnisystem(), rom.getMapperNumber());
    
    hardReset();
    
    EmulationON = true;
    
    EmulationThread = new std::thread(&NesEmulator::clock, this);
    EmulationThread->detach();

    return true;
}

void NesEmulator::setStop(bool stop)
{
    EmulationON = false;
}

//void NesEmulator::setPaused(bool paused)
//{
//    EmulationON = paused;
//    LastTime = getTime();
//}

void NesEmulator::clock()
{
    while (EmulationON)
    {
        //CPU::__instance->print();
        CPU::__instance->clock();
    }
}

void NesEmulator::syncClock()
{
    PPU::__instance->clock();
    /*
     * NMI edge detector polls the status of the NMI line during φ2 of each CPU cycle
     * (i.e., during the second half of each cycle)
     */
    Interrupt::__instance->pollInterruptStatus();
    PPU::__instance->clock();
    PPU::__instance->clock();
    if (DoPalAdditionalClock)// In pal system ..
    {
        palCyc++;
        if (palCyc == 5)
        {
            PPU::__instance->clock();
            palCyc = 0;
        }
    }
    APU::__instance->clock();
    DMA::__instance->DMAClock();
    Memory::__instance->board->OnCPUClock();
}

void NesEmulator::OnFinishFrame()
{
    // input
    Input::__instance->InputFinishFrame();
    
    // sound
    {        
        // Blip buffer
//        APU::__instance->audio_playback_blipbuffer->EndFrame((uint)APU::__instance->audio_playback_blipbuffer_timer);
//        APU::__instance->audio_playback_blipbuffer_timer = 0;
//        
//        int n = APU::__instance->audio_playback_blipbuffer->SamplesAvailable();
//        short* returnBuffer = new short[n];
//        APU::__instance->audio_playback_blipbuffer->ReadSamples(returnBuffer, n, n, false);
//        APU::__instance->AudioOut->Update(returnBuffer, n);
//        delete[] returnBuffer;
    }

    static  double LastTime = getTime();
    gapTime = getTime() - LastTime;

    while (gapTime < FramePeriod)
    {
        gapTime = getTime() - LastTime;
    }
    
    LastTime = getTime();
    
//    FPSDone++;
//    // Reset
//    if (FPSDone > 1000000)
//        FPSDone = 0;
}

double NesEmulator::getTime()
{
    timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec/1000000.0;
}
