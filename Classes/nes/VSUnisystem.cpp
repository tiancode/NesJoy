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

#include "VSUnisystem.h"
#include "PPU.h"
#include "util.h"

VSUnisystem::VSUnisystem()
{
    
}

VSUnisystem::~VSUnisystem()
{
    
}

bool VSUnisystem::IsVSUnisystem = false;

int VSUnisystem::VS1[] =
{
    0x18,0x3f,0x1c,0x3f,0x3f,0x3f,0x0b,0x17,0x10,0x3f,0x14,0x3f,0x36,0x37,0x1a,0x3f,
    0x25,0x3f,0x12,0x3f,0x0f,0x3f,0x3f,0x3f,0x3f,0x3f,0x22,0x19,0x3f,0x3f,0x3a,0x21,
    0x3f,0x3f,0x07,0x3f,0x3f,0x3f,0x00,0x15,0x0c,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
    0x3f,0x3f,0x07,0x16,0x3f,0x3f,0x30,0x3c,0x3f,0x27,0x3f,0x3f,0x29,0x3f,0x1b,0x09,
};

int VSUnisystem::VS2[] =
{
    0x0f,0x27,0x18,0x3f,0x3f,0x25,0x3f,0x34,0x16,0x13,0x3f,0x3f,0x20,0x23,0x3f,0x0b,
    0x3f,0x3f,0x06,0x3f,0x1b,0x3f,0x3f,0x22,0x3f,0x24,0x3f,0x3f,0x32,0x3f,0x3f,0x03,
    0x3f,0x37,0x26,0x33,0x11,0x3f,0x10,0x3f,0x14,0x3f,0x00,0x09,0x12,0x0f,0x3f,0x30,
    0x3f,0x3f,0x2a,0x17,0x0c,0x01,0x15,0x19,0x3f,0x3f,0x07,0x37,0x3f,0x05,0x3f,0x3f,
};

int VSUnisystem::VS3[] =
{
    0x3f,0x3f,0x3f,0x3f,0x1a,0x30,0x3c,0x09,0x0f,0x0f,0x3f,0x0f,0x3f,0x3f,0x3f,0x30,
    0x32,0x1c,0x3f,0x12,0x3f,0x18,0x17,0x3f,0x0c,0x3f,0x3f,0x02,0x16,0x3f,0x3f,0x3f,
    0x3f,0x3f,0x0f,0x37,0x3f,0x28,0x27,0x3f,0x29,0x3f,0x21,0x3f,0x11,0x3f,0x0f,0x3f,
    0x31,0x3f,0x3f,0x3f,0x0f,0x2a,0x28,0x3f,0x3f,0x3f,0x3f,0x3f,0x13,0x3f,0x3f,0x3f,
};


int VSUnisystem::VS4[] =
{
    0x0f,0x3f,0x3f,0x10,0x3f,0x30,0x31,0x3f,0x01,0x0f,0x36,0x3f,0x3f,0x3f,0x3f,0x3c,
    0x3f,0x3f,0x3f,0x12,0x19,0x3f,0x17,0x3f,0x00,0x3f,0x3f,0x02,0x16,0x3f,0x3f,0x3f,
    0x3f,0x3f,0x3f,0x37,0x3f,0x27,0x26,0x20,0x3f,0x04,0x22,0x3f,0x11,0x3f,0x3f,0x3f,
    0x2c,0x3f,0x3f,0x3f,0x07,0x2a,0x3f,0x3f,0x3f,0x3f,0x3f,0x38,0x13,0x3f,0x3f,0x0c,
};
int VSUnisystem::VS5[] =
{
    0x18,0x3f,0x1c,0x3f,0x3f,0x3f,0x01,0x17,0x10,0x3f,0x2a,0x3f,0x36,0x37,0x1a,0x39,
    0x25,0x3f,0x12,0x3f,0x0f,0x3f,0x3f,0x26,0x3f,0x3f,0x22,0x19,0x3f,0x0f,0x3a,0x21,
    0x3f,0x0a,0x07,0x06,0x13,0x3f,0x00,0x15,0x0c,0x3f,0x11,0x3f,0x3f,0x38,0x3f,0x3f,
    0x3f,0x3f,0x07,0x16,0x3f,0x3f,0x30,0x3c,0x0f,0x27,0x3f,0x31,0x29,0x3f,0x11,0x09,
};
int VSUnisystem::VS6[] =
{
    0x35,0x3f,0x16,0x3f,0x1c,0x3f,0x3f,0x15,0x3f,0x3f,0x27,0x05,0x04,0x3f,0x3f,0x30,
    0x21,0x3f,0x3f,0x3f,0x3f,0x3f,0x36,0x12,0x3f,0x2b,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,
    0x3f,0x31,0x3f,0x2a,0x2c,0x0c,0x3f,0x3f,0x3f,0x07,0x34,0x06,0x3f,0x25,0x26,0x0f,
    0x3f,0x19,0x10,0x3f,0x3f,0x3f,0x3f,0x17,0x3f,0x11,0x3f,0x3f,0x3f,0x3f,0x18,0x3f,
};
int VSUnisystem::VS7[] =
{
    0x35,0x3f,0x16,0x22,0x1c,0x3f,0x3f,0x15,0x3f,0x00,0x27,0x05,0x04,0x3f,0x3f,0x30,
    0x21,0x3f,0x3f,0x29,0x3c,0x3f,0x36,0x12,0x3f,0x2b,0x3f,0x3f,0x3f,0x3f,0x3f,0x01,
    0x3f,0x31,0x3f,0x2a,0x2c,0x0c,0x3f,0x3f,0x3f,0x07,0x34,0x06,0x3f,0x25,0x26,0x0f,
    0x3f,0x19,0x10,0x3f,0x3f,0x3f,0x3f,0x17,0x3f,0x11,0x3f,0x3f,0x3f,0x25,0x18,0x3f,
};



void VSUnisystem::CheckGameVSUnisystem(std::string sha1, bool trueByHeader, int mapperNumber)
{
    // TODO: finish all VSUnisystem games setup
    for(int i = 0; i < PPU::__instance->paletteIndexesSize; i++)
    {
        PPU::__instance->paletteIndexes[i] = i;
    }
    
    IsVSUnisystem = false;
    std::string str = toLower(&sha1);
    if(trueByHeader)
    {
        IsVSUnisystem = true;
        if( str == "035cc757cffedefaca2b420e12a2cfcf44409b9f" ||  //Super Skater
            str == "b21aa940728ed80c72ee23c251c96e42cc84b2d6" ||  //VS Super Mario Bros
            str == "7fd66e0a4cc0e404f404d8164fa221ee2acb7a38" )   //Clu Clu Land (VS)
        {
            memset(PPU::__instance->paletteIndexes, 0, sizeof(int)*PPU::__instance->paletteIndexesSize);
            for(int i = 0; i < sizeof(VS1)/sizeof(VS1[0]); i++)
            {
                PPU::__instance->paletteIndexes[i] = VS1[i];
                PPU::__instance->paletteIndexesSize = sizeof(VS1)/sizeof(VS1[0]);
            }
        }
        else if( str == "e0572da111d05bf622ec137df8a658f7b0687ddf" ||  //Battle City VS
                 str == "1b516cf7688792f5dbd669850c047a7afe9eb59f" ||  //Freedom Force (VS)
                 str == "e0f7bdbd2c96b14d4b8d2146a900aaad17f9e3b1" ||  //Golf
                 str == "cde1ecaf212a9f5a5a49f904f87951eda15d54dd" ||  //Ladies Golf (VS)
                 str == "f8a0f2c5a4b7212cb35f53ea7193b3dd85d6e1cd" ||  //Mach Rider (VS)
                 str == "68de623b2ad92ba19d18f17eaa0b97ee4523f6df")    //VS Slalom (VS)
        {
            memset(PPU::__instance->paletteIndexes, 0, sizeof(int)*PPU::__instance->paletteIndexesSize);
            for(int i = 0; i < sizeof(VS2)/sizeof(VS2[0]); i++)
            {
                PPU::__instance->paletteIndexes[i] = VS2[i];
                PPU::__instance->paletteIndexesSize = sizeof(VS2)/sizeof(VS2[0]);
            }
        }
        else if( str == "9eb3b75e7b45df51b8bcd29df84689a7e8557f4f" )  //VS Castlevania (VS)
        {
            memset(PPU::__instance->paletteIndexes, 0, sizeof(int)*PPU::__instance->paletteIndexesSize);
            for(int i = 0; i < sizeof(VS3)/sizeof(VS3[0]); i++)
            {
                PPU::__instance->paletteIndexes[i] = VS3[i];
                PPU::__instance->paletteIndexesSize = sizeof(VS3)/sizeof(VS3[0]);
            }
        }
        else if( str == "bbb0af27b313d7c838a38fb772a6fe8afbafbb95" )  //Soccer
        {
            memset(PPU::__instance->paletteIndexes, 0, sizeof(int)*PPU::__instance->paletteIndexesSize);
            for(int i = 0; i < sizeof(VS4)/sizeof(VS4[0]); i++)
            {
                PPU::__instance->paletteIndexes[i] = VS4[i];
                PPU::__instance->paletteIndexesSize = sizeof(VS4)/sizeof(VS4[0]);
            }
        }
        else if( str == "9f1943aade4233285589cea5bdc96b5380d49337" )  //Ice Climber (VS)
        {
            memset(PPU::__instance->paletteIndexes, 0, sizeof(int)*PPU::__instance->paletteIndexesSize);
            for(int i = 0; i < sizeof(VS5)/sizeof(VS5[0]); i++)
            {
                PPU::__instance->paletteIndexes[i] = VS5[i];
                PPU::__instance->paletteIndexesSize = sizeof(VS5)/sizeof(VS5[0]);
            }
        }
        else if( str == "1a4ec64e576bad64daf320aeed0be1b8b50d21df" )  //Pinball (VS)
        {
            memset(PPU::__instance->paletteIndexes, 0, sizeof(int)*PPU::__instance->paletteIndexesSize);
            for(int i = 0; i < sizeof(VS6)/sizeof(VS6[0]); i++)
            {
                PPU::__instance->paletteIndexes[i] = VS6[i];
                PPU::__instance->paletteIndexesSize = sizeof(VS6)/sizeof(VS6[0]);
            }
        }
        else if( str == "1a17df593c658f56d71b7026d2771396bff95b36" )  //Tetris
        {
            memset(PPU::__instance->paletteIndexes, 0, sizeof(int)*PPU::__instance->paletteIndexesSize);
            for(int i = 0; i < sizeof(VS7)/sizeof(VS7[0]); i++)
            {
                PPU::__instance->paletteIndexes[i] = VS7[i];
                PPU::__instance->paletteIndexesSize = sizeof(VS6)/sizeof(VS7[0]);
            }
        }
    }
    else
    {
        if(mapperNumber == 99)
        {
            IsVSUnisystem = true;
        }
        else
        {
            if(str == "035cc757cffedefaca2b420e12a2cfcf44409b9f" ||  //Super Skater
               str == "b21aa940728ed80c72ee23c251c96e42cc84b2d6" ||  //VS Super Mario Bros
               str == "7fd66e0a4cc0e404f404d8164fa221ee2acb7a38" )   //Clu Clu Land (VS)
            {
                IsVSUnisystem = true;
                memset(PPU::__instance->paletteIndexes, 0, sizeof(int)*PPU::__instance->paletteIndexesSize);
                for(int i = 0; i < sizeof(VS1)/sizeof(VS1[0]); i++)
                {
                    PPU::__instance->paletteIndexes[i] = VS1[i];
                    PPU::__instance->paletteIndexesSize = sizeof(VS1)/sizeof(VS1[0]);
                }
            }
            else if( str == "e0572da111d05bf622ec137df8a658f7b0687ddf" || //Battle City VS
                     str == "1b516cf7688792f5dbd669850c047a7afe9eb59f" || //Freedom Force (VS)
                     str == "e0f7bdbd2c96b14d4b8d2146a900aaad17f9e3b1" || //Golf
                     str == "cde1ecaf212a9f5a5a49f904f87951eda15d54dd" || //Ladies Golf (VS)
                     str == "f8a0f2c5a4b7212cb35f53ea7193b3dd85d6e1cd" || //Mach Rider (VS)
                     str == "68de623b2ad92ba19d18f17eaa0b97ee4523f6df" )  //VS Slalom (VS)
            {
                IsVSUnisystem = true;
                memset(PPU::__instance->paletteIndexes, 0, sizeof(int)*PPU::__instance->paletteIndexesSize);
                for(int i = 0; i < sizeof(VS2)/sizeof(VS2[0]); i++)
                {
                    PPU::__instance->paletteIndexes[i] = VS2[i];
                    PPU::__instance->paletteIndexesSize = sizeof(VS2)/sizeof(VS2[0]);
                }
            }
            else if( str == "9eb3b75e7b45df51b8bcd29df84689a7e8557f4f" )  //VS Castlevania (VS)
            {
                IsVSUnisystem = true;
                memset(PPU::__instance->paletteIndexes, 0, sizeof(int)*PPU::__instance->paletteIndexesSize);
                for(int i = 0; i < sizeof(VS3)/sizeof(VS3[0]); i++)
                {
                    PPU::__instance->paletteIndexes[i] = VS3[i];
                    PPU::__instance->paletteIndexesSize = sizeof(VS3)/sizeof(VS3[0]);
                }
            }
            else if( str == "bbb0af27b313d7c838a38fb772a6fe8afbafbb95") //Soccer
            {
                IsVSUnisystem = true;
                memset(PPU::__instance->paletteIndexes, 0, sizeof(int)*PPU::__instance->paletteIndexesSize);
                for(int i = 0; i < sizeof(VS4)/sizeof(VS4[0]); i++)
                {
                    PPU::__instance->paletteIndexes[i] = VS4[i];
                    PPU::__instance->paletteIndexesSize = sizeof(VS4)/sizeof(VS4[0]);
                }
            }
            else if( str == "9f1943aade4233285589cea5bdc96b5380d49337" ) //Ice Climber (VS)
            {
                IsVSUnisystem = true;
                memset(PPU::__instance->paletteIndexes, 0, sizeof(int)*PPU::__instance->paletteIndexesSize);
                for(int i = 0; i < sizeof(VS5)/sizeof(VS5[0]); i++)
                {
                    PPU::__instance->paletteIndexes[i] = VS5[i];
                    PPU::__instance->paletteIndexesSize = sizeof(VS5)/sizeof(VS5[0]);
                }
            }
            else if( str == "1a4ec64e576bad64daf320aeed0be1b8b50d21df" ) ////Pinball (VS)
            {
                IsVSUnisystem = true;
                memset(PPU::__instance->paletteIndexes, 0, sizeof(int)*PPU::__instance->paletteIndexesSize);
                for(int i = 0; i < sizeof(VS6)/sizeof(VS6[0]); i++)
                {
                    PPU::__instance->paletteIndexes[i] = VS6[i];
                    PPU::__instance->paletteIndexesSize = sizeof(VS6)/sizeof(VS6[0]);
                }
            }
            else if( str == "1a17df593c658f56d71b7026d2771396bff95b36" ) //Tetris
            {
                IsVSUnisystem = true;
                memset(PPU::__instance->paletteIndexes, 0, sizeof(int)*PPU::__instance->paletteIndexesSize);
                for(int i = 0; i < sizeof(VS7)/sizeof(VS7[0]); i++)
                {
                    PPU::__instance->paletteIndexes[i] = VS7[i];
                    PPU::__instance->paletteIndexesSize = sizeof(VS7)/sizeof(VS7[0]);
                }
            }
        }
    }
}
